// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2018  Realtek Corporation.
 */

#include "main.h"
#include "rx.h"
#include "ps.h"

void rtw_rx_stats(struct rtw_dev *rtwdev, struct ieee80211_vif *vif,
		  struct sk_buff *skb)
{
	struct ieee80211_hdr *hdr;
	struct rtw_vif *rtwvif;

	hdr = (struct ieee80211_hdr *)skb->data;

	if (!ieee80211_is_data(hdr->frame_control))
		return;

	if (!is_broadcast_ether_addr(hdr->addr1) &&
	    !is_multicast_ether_addr(hdr->addr1)) {
		rtwdev->stats.rx_unicast += skb->len;
		rtwdev->stats.rx_cnt++;
		if (vif) {
			rtwvif = (struct rtw_vif *)vif->drv_priv;
			rtwvif->stats.rx_unicast += skb->len;
			rtwvif->stats.rx_cnt++;
			if (rtwvif->stats.rx_cnt > RTW_LPS_THRESHOLD)
				rtw_leave_lps_irqsafe(rtwdev, rtwvif);
		}
	}
}
EXPORT_SYMBOL(rtw_rx_stats);

static void rtw_rx_rssi_add(struct rtw_dev *rtwdev,
			    struct rtw_rx_pkt_stat *pkt_stat,
			    struct ieee80211_hdr *hdr)
{
	struct ieee80211_vif *vif = NULL;
	struct rtw_sta_info *si = NULL;
	__le16 fc = hdr->frame_control;
	u8 *bssid;
	u8 macid = RTW_BC_MC_MACID;
	bool match_bssid = false;
	bool is_packet_match_bssid;
	bool if_addr_match;
	bool hw_err;
	bool ctl;

	bssid = get_hdr_bssid(hdr);
	vif = get_hdr_vif(rtwdev, hdr);
	pkt_stat->vif = vif;
	if (unlikely(is_broadcast_ether_addr(hdr->addr1) ||
		     is_multicast_ether_addr(hdr->addr1)))
		match_bssid = get_hdr_match_bssid(rtwdev, hdr, bssid);
	else if (vif)
		match_bssid = ether_addr_equal(vif->bss_conf.bssid, bssid);
	si = get_hdr_sta(rtwdev, vif, hdr);
	macid = si ? si->mac_id : RTW_BC_MC_MACID;
	pkt_stat->mac_id = macid;
	pkt_stat->si = si;

	if_addr_match = !!vif;
	hw_err = pkt_stat->crc_err || pkt_stat->icv_err;
	ctl = ieee80211_is_ctl(fc);
	is_packet_match_bssid = !hw_err && !ctl && match_bssid;

	if (((match_bssid && if_addr_match) || ieee80211_is_beacon(fc)) &&
	    (!hw_err && !ctl) && (pkt_stat->phy_status && pkt_stat->si))
		ewma_rssi_add(&pkt_stat->si->avg_rssi, pkt_stat->rssi);
}

void rtw_rx_fill_rx_status(struct rtw_dev *rtwdev,
			   struct rtw_rx_pkt_stat *pkt_stat,
			   struct ieee80211_hdr *hdr,
			   struct ieee80211_rx_status *rx_status,
			   u8 *phy_status)
{
	struct ieee80211_hw *hw = rtwdev->hw;

	memset(rx_status, 0, sizeof(*rx_status));
	rx_status->freq = hw->conf.chandef.chan->center_freq;
	rx_status->band = hw->conf.chandef.chan->band;
	if (pkt_stat->crc_err)
		rx_status->flag |= RX_FLAG_FAILED_FCS_CRC;
	if (pkt_stat->decrypted)
		rx_status->flag |= RX_FLAG_DECRYPTED;

	if (pkt_stat->rate >= DESC_RATEVHT1SS_MCS0)
		rx_status->encoding = RX_ENC_VHT;
	else if (pkt_stat->rate >= DESC_RATEMCS0)
		rx_status->encoding = RX_ENC_HT;

	if (pkt_stat->rate >= DESC_RATEVHT1SS_MCS0 &&
	    pkt_stat->rate <= DESC_RATEVHT1SS_MCS9) {
		rx_status->nss = 1;
		rx_status->rate_idx = pkt_stat->rate - DESC_RATEVHT1SS_MCS0;
	} else if (pkt_stat->rate >= DESC_RATEVHT2SS_MCS0 &&
		   pkt_stat->rate <= DESC_RATEVHT2SS_MCS9) {
		rx_status->nss = 2;
		rx_status->rate_idx = pkt_stat->rate - DESC_RATEVHT2SS_MCS0;
	} else if (pkt_stat->rate >= DESC_RATEVHT3SS_MCS0 &&
		   pkt_stat->rate <= DESC_RATEVHT3SS_MCS9) {
		rx_status->nss = 3;
		rx_status->rate_idx = pkt_stat->rate - DESC_RATEVHT3SS_MCS0;
	} else if (pkt_stat->rate >= DESC_RATEVHT4SS_MCS0 &&
		   pkt_stat->rate <= DESC_RATEVHT4SS_MCS9) {
		rx_status->nss = 4;
		rx_status->rate_idx = pkt_stat->rate - DESC_RATEVHT4SS_MCS0;
	} else if (pkt_stat->rate >= DESC_RATEMCS0 &&
		   pkt_stat->rate <= DESC_RATEMCS15) {
		rx_status->rate_idx = pkt_stat->rate - DESC_RATEMCS0;
	} else if (rx_status->band == NL80211_BAND_5GHZ &&
		   pkt_stat->rate >= DESC_RATE6M &&
		   pkt_stat->rate <= DESC_RATE54M) {
		rx_status->rate_idx = pkt_stat->rate - DESC_RATE6M;
	} else if (rx_status->band == NL80211_BAND_2GHZ &&
		   pkt_stat->rate >= DESC_RATE1M &&
		   pkt_stat->rate <= DESC_RATE54M) {
		rx_status->rate_idx = pkt_stat->rate - DESC_RATE1M;
	} else {
		rx_status->rate_idx = 0;
	}

	rx_status->flag |= RX_FLAG_MACTIME_START;
	rx_status->mactime = pkt_stat->tsf_low;

	if (pkt_stat->bw == RTW_CHANNEL_WIDTH_80)
		rx_status->bw = RATE_INFO_BW_80;
	else if (pkt_stat->bw == RTW_CHANNEL_WIDTH_40)
		rx_status->bw = RATE_INFO_BW_40;
	else
		rx_status->bw = RATE_INFO_BW_20;

	rx_status->signal = pkt_stat->signal_power;

	rtw_rx_rssi_add(rtwdev, pkt_stat, hdr);
}
