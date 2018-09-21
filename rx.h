/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 2018  Realtek Corporation.
 */

#ifndef __RTW_RX_H_
#define __RTW_RX_H_

#define GET_RX_DESC_PHYST(rxdesc) LE_BITS_TO_4BYTE((rxdesc) + 0x00, 26, 1)
#define GET_RX_DESC_ICV_ERR(rxdesc) LE_BITS_TO_4BYTE((rxdesc) + 0x00, 15, 1)
#define GET_RX_DESC_CRC32(rxdesc) LE_BITS_TO_4BYTE((rxdesc) + 0x00, 14, 1)
#define GET_RX_DESC_SWDEC(rxdesc) LE_BITS_TO_4BYTE((rxdesc) + 0x00, 27, 1)
#define GET_RX_DESC_C2H(rxdesc) LE_BITS_TO_4BYTE((rxdesc) + 0x08, 28, 1)
#define GET_RX_DESC_PKT_LEN(rxdesc) LE_BITS_TO_4BYTE((rxdesc) + 0x00, 0, 14)
#define GET_RX_DESC_DRV_INFO_SIZE(rxdesc) \
	LE_BITS_TO_4BYTE((rxdesc) + 0x00, 16, 4)
#define GET_RX_DESC_SHIFT(rxdesc) LE_BITS_TO_4BYTE((rxdesc) + 0x00, 24, 2)
#define GET_RX_DESC_RX_RATE(rxdesc) LE_BITS_TO_4BYTE((rxdesc) + 0x0C, 0, 7)
#define GET_RX_DESC_MACID(rxdesc) LE_BITS_TO_4BYTE((rxdesc) + 0x04, 0, 7)
#define GET_RX_DESC_PPDU_CNT(rxdesc) LE_BITS_TO_4BYTE((rxdesc) + 0x08, 29, 2)
#define GET_RX_DESC_TSFL(rxdesc) LE_BITS_TO_4BYTE((rxdesc) + 0x14, 0, 32)

void rtw_rx_stats(struct rtw_dev *rtwdev, struct ieee80211_vif *vif,
		  struct sk_buff *skb);
void rtw_rx_fill_rx_status(struct rtw_dev *rtwdev,
			   struct rtw_rx_pkt_stat *pkt_stat,
			   struct ieee80211_hdr *hdr,
			   struct ieee80211_rx_status *rx_status,
			   u8 *phy_status);

#endif
