/******************************************************************************
 *
 * Copyright(c) 2016-2017  Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 *****************************************************************************/

/* ************************************************************
 * Description:
 *
 * This file is for RTL8723D Co-exist mechanism
 *
 * History
 * 2012/11/15 Cosa first check in.
 *
 * *************************************************************/

/* ************************************************************
 * include files
 * *************************************************************/
#include "halbt_precomp.h"

/* ************************************************************
 * Global variables, these are static variables
 * *************************************************************/
static struct coex_dm_8723d_1ant glcoex_dm_8723d_1ant;
static struct coex_dm_8723d_1ant *coex_dm = &glcoex_dm_8723d_1ant;
static struct coex_sta_8723d_1ant glcoex_sta_8723d_1ant;
static struct coex_sta_8723d_1ant *coex_sta = &glcoex_sta_8723d_1ant;
static struct psdscan_sta_8723d_1ant gl_psd_scan_8723d_1ant;
static struct psdscan_sta_8723d_1ant *psd_scan = &gl_psd_scan_8723d_1ant;

static const char *const glbt_info_src_8723d_1ant[] = {
	"BT Info[wifi fw]", "BT Info[bt rsp]", "BT Info[bt auto report]",
};

/* ************************************************************
 * BtCoex Version Format:
 * 1. date :			glcoex_ver_date_XXXXX_1ant
 * 2. WifiCoexVersion : glcoex_ver_XXXX_1ant
 * 3. BtCoexVersion :	glcoex_ver_btdesired_XXXXX_1ant
 * 4. others :			glcoex_ver_XXXXXX_XXXXX_1ant
 *
 * Variable should be indicated IC and Antenna numbers !!!
 * Please strictly follow this order and naming style !!!
 *
 * *************************************************************/
static u32 glcoex_ver_date_8723d_1ant = 20171212;
static u32 glcoex_ver_8723d_1ant = 0x22;
static u32 glcoex_ver_btdesired_8723d_1ant = 0x20;

/* ************************************************************
 * local function proto type if needed
 * ************************************************************
 * ************************************************************
 * local function start with halbtc8723d1ant_
 * *************************************************************/

static void halbtc8723d1ant_limited_rx(struct btc_coexist *btcoexist,
				       bool force_exec, bool rej_ap_agg_pkt,
				       bool bt_ctrl_agg_buf_size,
				       u8 agg_buf_size)
{
	bool reject_rx_agg = rej_ap_agg_pkt;
	bool bt_ctrl_rx_agg_size = bt_ctrl_agg_buf_size;
	u8 rx_agg_size = agg_buf_size;

	/* ============================================ */
	/*	Rx Aggregation related setting */
	/* ============================================ */
	btcoexist->btc_set(btcoexist, BTC_SET_BL_TO_REJ_AP_AGG_PKT,
			   &reject_rx_agg);
	/* decide BT control aggregation buf size or not */
	btcoexist->btc_set(btcoexist, BTC_SET_BL_BT_CTRL_AGG_SIZE,
			   &bt_ctrl_rx_agg_size);
	/* aggregation buf size, only work when BT control Rx aggregation size*/
	btcoexist->btc_set(btcoexist, BTC_SET_U1_AGG_BUF_SIZE, &rx_agg_size);
	/* real update aggregation setting */
	btcoexist->btc_set(btcoexist, BTC_SET_ACT_AGGREGATE_CTRL, NULL);
}

static void halbtc8723d1ant_query_bt_info(struct btc_coexist *btcoexist)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;

	u8 h2c_parameter[1] = {0};

	h2c_parameter[0] |= BIT(0); /* trigger */

	btcoexist->btc_fill_h2c(btcoexist, 0x61, 1, h2c_parameter);

	RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		 "[BTCoex], WL query BT info!!\n");
}

static void halbtc8723d1ant_monitor_bt_ctr(struct btc_coexist *btcoexist)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;
	u32 reg_hp_txrx, reg_lp_txrx, u32tmp;
	u32 reg_hp_tx = 0, reg_hp_rx = 0, reg_lp_tx = 0, reg_lp_rx = 0;
	static u8 num_of_bt_counter_chk, cnt_slave, cnt_overhead,
		cnt_autoslot_hang;
	struct btc_bt_link_info *bt_link_info = &btcoexist->bt_link_info;

	/* to avoid 0x76e[3] = 1 (WLAN_Act control by PTA) during IPS */
	/* if (! (btcoexist->btc_read_1byte(btcoexist, 0x76e) & 0x8) ) */

	reg_hp_txrx = 0x770;
	reg_lp_txrx = 0x774;

	u32tmp = btcoexist->btc_read_4byte(btcoexist, reg_hp_txrx);
	reg_hp_tx = u32tmp & MASKLWORD;
	reg_hp_rx = (u32tmp & MASKHWORD) >> 16;

	u32tmp = btcoexist->btc_read_4byte(btcoexist, reg_lp_txrx);
	reg_lp_tx = u32tmp & MASKLWORD;
	reg_lp_rx = (u32tmp & MASKHWORD) >> 16;

	coex_sta->high_priority_tx = reg_hp_tx;
	coex_sta->high_priority_rx = reg_hp_rx;
	coex_sta->low_priority_tx = reg_lp_tx;
	coex_sta->low_priority_rx = reg_lp_rx;

	RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		 "[BTCoex], Hi-Pri Rx/Tx: %d/%d, Lo-Pri Rx/Tx: %d/%d\n",
		 reg_hp_rx, reg_hp_tx, reg_lp_rx, reg_lp_tx);

	if (coex_dm->bt_status == BT_8723D_1ANT_BT_STATUS_NON_CONNECTED_IDLE) {
		if (coex_sta->high_priority_rx >= 15) {
			if (cnt_overhead < 3)
				cnt_overhead++;

			if (cnt_overhead == 3)
				coex_sta->is_hi_pri_rx_overhead = true;
		} else {
			if (cnt_overhead > 0)
				cnt_overhead--;

			if (cnt_overhead == 0)
				coex_sta->is_hi_pri_rx_overhead = false;
		}
	} else {
		coex_sta->is_hi_pri_rx_overhead = false;
	}

	RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		 "[BTCoex], Hi-Pri Rx/Tx: %d/%d, Lo-Pri Rx/Tx: %d/%d\n",
		 reg_hp_rx, reg_hp_tx, reg_lp_rx, reg_lp_tx);

	/* reset counter */
	btcoexist->btc_write_1byte(btcoexist, 0x76e, 0xc);

	if ((coex_sta->low_priority_tx > 1150) &&
	    (!coex_sta->c2h_bt_inquiry_page))
		coex_sta->pop_event_cnt++;

	if ((coex_sta->low_priority_rx >= 1150) &&
	    (coex_sta->low_priority_rx >= coex_sta->low_priority_tx) &&
	    (!coex_sta->under_ips) && (!coex_sta->c2h_bt_inquiry_page) &&
	    (coex_sta->bt_link_exist)) {
		if (cnt_slave >= 2) {
			bt_link_info->slave_role = true;
			cnt_slave = 2;
		} else {
			cnt_slave++;
		}
	} else {
		if (cnt_slave == 0) {
			bt_link_info->slave_role = false;
			cnt_slave = 0;
		} else {
			cnt_slave--;
		}
	}

	if (coex_sta->is_tdma_btautoslot) {
		if ((coex_sta->low_priority_tx >= 1300) &&
		    (coex_sta->low_priority_rx <= 150)) {
			if (cnt_autoslot_hang >= 2) {
				coex_sta->is_tdma_btautoslot_hang = true;
				cnt_autoslot_hang = 2;
			} else {
				cnt_autoslot_hang++;
			}
		} else {
			if (cnt_autoslot_hang == 0) {
				coex_sta->is_tdma_btautoslot_hang = false;
				cnt_autoslot_hang = 0;
			} else {
				cnt_autoslot_hang--;
			}
		}
	}

	if (bt_link_info->hid_only) {
		if (coex_sta->low_priority_tx > 50)
			coex_sta->is_hid_low_pri_tx_overhead = true;
		else
			coex_sta->is_hid_low_pri_tx_overhead = false;
	}

	if (!coex_sta->bt_disabled) {
		if ((coex_sta->high_priority_tx == 0) &&
		    (coex_sta->high_priority_rx == 0) &&
		    (coex_sta->low_priority_tx == 0) &&
		    (coex_sta->low_priority_rx == 0)) {
			num_of_bt_counter_chk++;
			if (num_of_bt_counter_chk >= 3) {
				halbtc8723d1ant_query_bt_info(btcoexist);
				num_of_bt_counter_chk = 0;
			}
		}
	}
}

static void halbtc8723d1ant_monitor_wifi_ctr(struct btc_coexist *btcoexist)
{
	s32 wifi_rssi = 0;
	bool wifi_busy = false, wifi_under_b_mode = false, wifi_scan = false;
	static u8 wl_noisy_count0, wl_noisy_count1 = 3, wl_noisy_count2;
	u32 cnt_cck;
	u32 cnt_crcok = 0, cnt_crcerr = 0;
	static u8 cnt_ccklocking;
	u8 h2c_parameter[1] = {0};
	struct btc_bt_link_info *bt_link_info = &btcoexist->bt_link_info;

	/*send h2c to query WL FW dbg info	*/
	if (((coex_dm->cur_ps_tdma_on) && (coex_sta->force_lps_ctrl)) ||
	    ((coex_sta->acl_busy) && (bt_link_info->a2dp_exist))) {
		h2c_parameter[0] = 0x8;
		btcoexist->btc_fill_h2c(btcoexist, 0x69, 1, h2c_parameter);
	}

	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_BUSY, &wifi_busy);
	btcoexist->btc_get(btcoexist, BTC_GET_S4_WIFI_RSSI, &wifi_rssi);
	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_UNDER_B_MODE,
			   &wifi_under_b_mode);

	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_SCAN, &wifi_scan);

	coex_sta->crc_ok_cck = btcoexist->btc_phydm_query_phy_counter(
		btcoexist, "PHYDM_INFO_CRC32_OK_CCK");
	coex_sta->crc_ok_11g = btcoexist->btc_phydm_query_phy_counter(
		btcoexist, "PHYDM_INFO_CRC32_OK_LEGACY");
	coex_sta->crc_ok_11n = btcoexist->btc_phydm_query_phy_counter(
		btcoexist, "PHYDM_INFO_CRC32_OK_HT");
	coex_sta->crc_ok_11n_vht = btcoexist->btc_phydm_query_phy_counter(
		btcoexist, "PHYDM_INFO_CRC32_OK_VHT");

	coex_sta->crc_err_cck = btcoexist->btc_phydm_query_phy_counter(
		btcoexist, "PHYDM_INFO_CRC32_ERROR_CCK");
	coex_sta->crc_err_11g = btcoexist->btc_phydm_query_phy_counter(
		btcoexist, "PHYDM_INFO_CRC32_ERROR_LEGACY");
	coex_sta->crc_err_11n = btcoexist->btc_phydm_query_phy_counter(
		btcoexist, "PHYDM_INFO_CRC32_ERROR_HT");
	coex_sta->crc_err_11n_vht = btcoexist->btc_phydm_query_phy_counter(
		btcoexist, "PHYDM_INFO_CRC32_ERROR_VHT");

	cnt_crcok = coex_sta->crc_ok_cck + coex_sta->crc_ok_11g +
		    coex_sta->crc_ok_11n + coex_sta->crc_ok_11n_vht;

	cnt_crcerr = coex_sta->crc_err_cck + coex_sta->crc_err_11g +
		     coex_sta->crc_err_11n + coex_sta->crc_err_11n_vht;

	/*	CCK lock identification	*/
	if (coex_sta->cck_lock)
		cnt_ccklocking++;
	else if (cnt_ccklocking != 0)
		cnt_ccklocking--;

	if (cnt_ccklocking >= 3) {
		cnt_ccklocking = 3;
		coex_sta->cck_lock_ever = true;
	}

	/* WiFi environment noisy identification */
	cnt_cck = coex_sta->crc_ok_cck + coex_sta->crc_err_cck;

	if ((!wifi_busy) && (!coex_sta->cck_lock)) {
		if (cnt_cck > 250) {
			if (wl_noisy_count2 < 3)
				wl_noisy_count2++;

			if (wl_noisy_count2 == 3) {
				wl_noisy_count0 = 0;
				wl_noisy_count1 = 0;
			}

		} else if (cnt_cck < 50) {
			if (wl_noisy_count0 < 3)
				wl_noisy_count0++;

			if (wl_noisy_count0 == 3) {
				wl_noisy_count1 = 0;
				wl_noisy_count2 = 0;
			}

		} else {
			if (wl_noisy_count1 < 3)
				wl_noisy_count1++;

			if (wl_noisy_count1 == 3) {
				wl_noisy_count0 = 0;
				wl_noisy_count2 = 0;
			}
		}

		if (wl_noisy_count2 == 3)
			coex_sta->wl_noisy_level = 2;
		else if (wl_noisy_count1 == 3)
			coex_sta->wl_noisy_level = 1;
		else
			coex_sta->wl_noisy_level = 0;
	}
}

static void halbtc8723d1ant_update_bt_link_info(struct btc_coexist *btcoexist)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;
	struct btc_bt_link_info *bt_link_info = &btcoexist->bt_link_info;
	bool bt_hs_on = false;
	bool bt_busy = false;
	u32 val = 0;
	static u8 pre_num_of_profile, cur_num_of_profile, cnt;

	if (coex_sta->is_ble_scan_toggle) {
		RT_TRACE(
			rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			"[BTCoex], BT ext info bit4 check, query BLE Scan type!!\n");
		coex_sta->bt_ble_scan_type =
			btcoexist->btc_get_ble_scan_type_from_bt(btcoexist);

		if ((coex_sta->bt_ble_scan_type & 0x1) == 0x1)
			coex_sta->bt_ble_scan_para[0] =
				btcoexist->btc_get_ble_scan_para_from_bt(
					btcoexist, 0x1);
		if ((coex_sta->bt_ble_scan_type & 0x2) == 0x2)
			coex_sta->bt_ble_scan_para[1] =
				btcoexist->btc_get_ble_scan_para_from_bt(
					btcoexist, 0x2);
		if ((coex_sta->bt_ble_scan_type & 0x4) == 0x4)
			coex_sta->bt_ble_scan_para[2] =
				btcoexist->btc_get_ble_scan_para_from_bt(
					btcoexist, 0x4);
	}

	coex_sta->num_of_profile = 0;

	/* set link exist status */
	if (!(coex_sta->bt_info & BT_INFO_8723D_1ANT_B_CONNECTION)) {
		coex_sta->bt_link_exist = false;
		coex_sta->pan_exist = false;
		coex_sta->a2dp_exist = false;
		coex_sta->hid_exist = false;
		coex_sta->sco_exist = false;
	} else { /* connection exists */
		coex_sta->bt_link_exist = true;
		if (coex_sta->bt_info & BT_INFO_8723D_1ANT_B_FTP) {
			coex_sta->pan_exist = true;
			coex_sta->num_of_profile++;
		} else {
			coex_sta->pan_exist = false;
		}

		if (coex_sta->bt_info & BT_INFO_8723D_1ANT_B_A2DP) {
			coex_sta->a2dp_exist = true;
			coex_sta->num_of_profile++;
		} else {
			coex_sta->a2dp_exist = false;
		}

		if (coex_sta->bt_info & BT_INFO_8723D_1ANT_B_HID) {
			coex_sta->hid_exist = true;
			coex_sta->num_of_profile++;
		} else {
			coex_sta->hid_exist = false;
		}

		if (coex_sta->bt_info & BT_INFO_8723D_1ANT_B_SCO_ESCO) {
			coex_sta->sco_exist = true;
			coex_sta->num_of_profile++;
		} else {
			coex_sta->sco_exist = false;
		}
	}

	btcoexist->btc_get(btcoexist, BTC_GET_BL_HS_OPERATION, &bt_hs_on);

	bt_link_info->bt_link_exist = coex_sta->bt_link_exist;
	bt_link_info->sco_exist = coex_sta->sco_exist;
	bt_link_info->a2dp_exist = coex_sta->a2dp_exist;
	bt_link_info->pan_exist = coex_sta->pan_exist;
	bt_link_info->hid_exist = coex_sta->hid_exist;
	bt_link_info->acl_busy = coex_sta->acl_busy;

	/* work around for HS mode. */
	if (bt_hs_on) {
		bt_link_info->pan_exist = true;
		bt_link_info->bt_link_exist = true;
	}

	/* check if Sco only */
	if (bt_link_info->sco_exist && !bt_link_info->a2dp_exist &&
	    !bt_link_info->pan_exist && !bt_link_info->hid_exist)
		bt_link_info->sco_only = true;
	else
		bt_link_info->sco_only = false;

	/* check if A2dp only */
	if (!bt_link_info->sco_exist && bt_link_info->a2dp_exist &&
	    !bt_link_info->pan_exist && !bt_link_info->hid_exist)
		bt_link_info->a2dp_only = true;
	else
		bt_link_info->a2dp_only = false;

	/* check if Pan only */
	if (!bt_link_info->sco_exist && !bt_link_info->a2dp_exist &&
	    bt_link_info->pan_exist && !bt_link_info->hid_exist)
		bt_link_info->pan_only = true;
	else
		bt_link_info->pan_only = false;

	/* check if Hid only */
	if (!bt_link_info->sco_exist && !bt_link_info->a2dp_exist &&
	    !bt_link_info->pan_exist && bt_link_info->hid_exist)
		bt_link_info->hid_only = true;
	else
		bt_link_info->hid_only = false;

	if (coex_sta->bt_info & BT_INFO_8723D_1ANT_B_INQ_PAGE) {
		coex_dm->bt_status = BT_8723D_1ANT_BT_STATUS_INQ_PAGE;
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], BtInfoNotify(), BT Inq/page!!!\n");
	} else if (!(coex_sta->bt_info & BT_INFO_8723D_1ANT_B_CONNECTION)) {
		coex_dm->bt_status = BT_8723D_1ANT_BT_STATUS_NON_CONNECTED_IDLE;
		RT_TRACE(
			rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			"[BTCoex], BtInfoNotify(), BT Non-Connected idle!!!\n");
	} else if (coex_sta->bt_info == BT_INFO_8723D_1ANT_B_CONNECTION) {
		/* connection exists but no busy */
		coex_dm->bt_status = BT_8723D_1ANT_BT_STATUS_CONNECTED_IDLE;
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], BtInfoNotify(), BT Connected-idle!!!\n");
	} else if (((coex_sta->bt_info & BT_INFO_8723D_1ANT_B_SCO_ESCO) ||
		    (coex_sta->bt_info & BT_INFO_8723D_1ANT_B_SCO_BUSY)) &&
		   (coex_sta->bt_info & BT_INFO_8723D_1ANT_B_ACL_BUSY)) {
		coex_dm->bt_status = BT_8723D_1ANT_BT_STATUS_ACL_SCO_BUSY;
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], BtInfoNotify(), BT ACL SCO busy!!!\n");
	} else if ((coex_sta->bt_info & BT_INFO_8723D_1ANT_B_SCO_ESCO) ||
		   (coex_sta->bt_info & BT_INFO_8723D_1ANT_B_SCO_BUSY)) {
		coex_dm->bt_status = BT_8723D_1ANT_BT_STATUS_SCO_BUSY;
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], BtInfoNotify(), BT SCO busy!!!\n");
	} else if (coex_sta->bt_info & BT_INFO_8723D_1ANT_B_ACL_BUSY) {
		coex_dm->bt_status = BT_8723D_1ANT_BT_STATUS_ACL_BUSY;
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], BtInfoNotify(), BT ACL busy!!!\n");
	} else {
		coex_dm->bt_status = BT_8723D_1ANT_BT_STATUS_MAX;
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], BtInfoNotify(), BT Non-Defined state!!!\n");
	}

	if ((coex_dm->bt_status == BT_8723D_1ANT_BT_STATUS_ACL_BUSY) ||
	    (coex_dm->bt_status == BT_8723D_1ANT_BT_STATUS_SCO_BUSY) ||
	    (coex_dm->bt_status == BT_8723D_1ANT_BT_STATUS_ACL_SCO_BUSY))
		bt_busy = true;
	else
		bt_busy = false;

	btcoexist->btc_set(btcoexist, BTC_SET_BL_BT_TRAFFIC_BUSY, &bt_busy);

	cur_num_of_profile = coex_sta->num_of_profile;

	if (cur_num_of_profile != pre_num_of_profile)
		cnt = 2;

	if (bt_link_info->a2dp_exist) {
		if (((coex_sta->bt_a2dp_vendor_id == 0) &&
		     (coex_sta->bt_a2dp_device_name == 0)) ||
		    (cur_num_of_profile != pre_num_of_profile)) {
			btcoexist->btc_get(btcoexist, BTC_GET_U4_BT_DEVICE_INFO,
					   &val);

			RT_TRACE(
				rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
				"[BTCoex], BtInfoNotify(), get BT DEVICE_INFO = %x\n",
				val);

			coex_sta->bt_a2dp_vendor_id = (u8)(val & 0xff);
			coex_sta->bt_a2dp_device_name = (val & 0xffffff00) >> 8;
		}

		if (((coex_sta->legacy_forbidden_slot == 0) &&
		     (coex_sta->le_forbidden_slot == 0)) ||
		    (cur_num_of_profile != pre_num_of_profile) || (cnt > 0)) {
			if (cnt > 0)
				cnt--;

			btcoexist->btc_get(btcoexist,
					   BTC_GET_U4_BT_FORBIDDEN_SLOT_VAL,
					   &val);

			RT_TRACE(
				rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
				"[BTCoex], BtInfoNotify(), get BT FORBIDDEN_SLOT_VAL = %x, cnt = %d\n",
				val, cnt);

			coex_sta->legacy_forbidden_slot = (u16)(val & 0xffff);
			coex_sta->le_forbidden_slot =
				(u16)((val & 0xffff0000) >> 16);
		}
	}

	pre_num_of_profile = coex_sta->num_of_profile;
}

static void halbtc8723d1ant_update_wifi_ch_info(struct btc_coexist *btcoexist,
						u8 type)
{
	u8 h2c_parameter[3] = {0};
	u32 wifi_bw;
	u8 wifi_central_chnl;

	/* only 2.4G we need to inform bt the chnl mask */
	btcoexist->btc_get(btcoexist, BTC_GET_U1_WIFI_CENTRAL_CHNL,
			   &wifi_central_chnl);
	if ((type == BTC_MEDIA_CONNECT) && (wifi_central_chnl <= 14)) {
		/* enable BT AFH skip WL channel for 8723d because BT Rx LO
		 * interference
		 */
		h2c_parameter[0] = 0x1;
		/* h2c_parameter[0] = 0x0; */
		h2c_parameter[1] = wifi_central_chnl;
		btcoexist->btc_get(btcoexist, BTC_GET_U4_WIFI_BW, &wifi_bw);
		if (wifi_bw == BTC_WIFI_BW_HT40)
			h2c_parameter[2] = 0x30;
		else
			h2c_parameter[2] = 0x20;
	}

	coex_dm->wifi_chnl_info[0] = h2c_parameter[0];
	coex_dm->wifi_chnl_info[1] = h2c_parameter[1];
	coex_dm->wifi_chnl_info[2] = h2c_parameter[2];

	btcoexist->btc_fill_h2c(btcoexist, 0x66, 3, h2c_parameter);
}

static u8 halbtc8723d1ant_action_algorithm(struct btc_coexist *btcoexist)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;

	struct btc_bt_link_info *bt_link_info = &btcoexist->bt_link_info;
	bool bt_hs_on = false;
	u8 algorithm = BT_8723D_1ANT_COEX_ALGO_UNDEFINED;
	u8 num_of_diff_profile = 0;

	btcoexist->btc_get(btcoexist, BTC_GET_BL_HS_OPERATION, &bt_hs_on);

	if (!bt_link_info->bt_link_exist) {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], No BT link exists!!!\n");
		return algorithm;
	}

	if (bt_link_info->sco_exist)
		num_of_diff_profile++;
	if (bt_link_info->hid_exist)
		num_of_diff_profile++;
	if (bt_link_info->pan_exist)
		num_of_diff_profile++;
	if (bt_link_info->a2dp_exist)
		num_of_diff_profile++;

	if (num_of_diff_profile == 1) {
		if (bt_link_info->sco_exist) {
			RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
				 "[BTCoex], BT Profile = SCO only\n");
			algorithm = BT_8723D_1ANT_COEX_ALGO_SCO;
		} else {
			if (bt_link_info->hid_exist) {
				RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
					 "[BTCoex], BT Profile = HID only\n");
				algorithm = BT_8723D_1ANT_COEX_ALGO_HID;
			} else if (bt_link_info->a2dp_exist) {
				RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
					 "[BTCoex], BT Profile = A2DP only\n");
				algorithm = BT_8723D_1ANT_COEX_ALGO_A2DP;
			} else if (bt_link_info->pan_exist) {
				if (bt_hs_on) {
					RT_TRACE(
						rtlpriv, COMP_BT_COEXIST,
						DBG_LOUD,
						"[BTCoex], BT Profile = PAN(HS) only\n");
					algorithm =
						BT_8723D_1ANT_COEX_ALGO_PANHS;
				} else {
					RT_TRACE(
						rtlpriv, COMP_BT_COEXIST,
						DBG_LOUD,
						"[BTCoex], BT Profile = PAN(EDR) only\n");
					algorithm =
						BT_8723D_1ANT_COEX_ALGO_PANEDR;
				}
			}
		}
	} else if (num_of_diff_profile == 2) {
		if (bt_link_info->sco_exist) {
			if (bt_link_info->hid_exist) {
				RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
					 "[BTCoex], BT Profile = SCO + HID\n");
				algorithm = BT_8723D_1ANT_COEX_ALGO_HID;
			} else if (bt_link_info->a2dp_exist) {
				RT_TRACE(
					rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
					"[BTCoex], BT Profile = SCO + A2DP ==> SCO\n");
				algorithm = BT_8723D_1ANT_COEX_ALGO_SCO;
			} else if (bt_link_info->pan_exist) {
				if (bt_hs_on) {
					RT_TRACE(
						rtlpriv, COMP_BT_COEXIST,
						DBG_LOUD,
						"[BTCoex], BT Profile = SCO + PAN(HS)\n");
					algorithm = BT_8723D_1ANT_COEX_ALGO_SCO;
				} else {
					RT_TRACE(
						rtlpriv, COMP_BT_COEXIST,
						DBG_LOUD,
						"[BTCoex], BT Profile = SCO + PAN(EDR)\n");
					algorithm =
					    BT_8723D_1ANT_COEX_ALGO_PANEDR_HID;
				}
			}
		} else {
			if (bt_link_info->hid_exist &&
			    bt_link_info->a2dp_exist) {
				RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
					 "[BTCoex], BT Profile = HID + A2DP\n");
				algorithm = BT_8723D_1ANT_COEX_ALGO_HID_A2DP;
			} else if (bt_link_info->hid_exist &&
				   bt_link_info->pan_exist) {
				if (bt_hs_on) {
					RT_TRACE(
						rtlpriv, COMP_BT_COEXIST,
						DBG_LOUD,
						"[BTCoex], BT Profile = HID + PAN(HS)\n");
					algorithm =
					    BT_8723D_1ANT_COEX_ALGO_HID_A2DP;
				} else {
					RT_TRACE(
						rtlpriv, COMP_BT_COEXIST,
						DBG_LOUD,
						"[BTCoex], BT Profile = HID + PAN(EDR)\n");
					algorithm =
					    BT_8723D_1ANT_COEX_ALGO_PANEDR_HID;
				}
			} else if (bt_link_info->pan_exist &&
				   bt_link_info->a2dp_exist) {
				if (bt_hs_on) {
					RT_TRACE(
						rtlpriv, COMP_BT_COEXIST,
						DBG_LOUD,
						"[BTCoex], BT Profile = A2DP + PAN(HS)\n");
					algorithm =
					    BT_8723D_1ANT_COEX_ALGO_A2DP_PANHS;
				} else {
					RT_TRACE(
						rtlpriv, COMP_BT_COEXIST,
						DBG_LOUD,
						"[BTCoex], BT Profile = A2DP + PAN(EDR)\n");
					algorithm =
					    BT_8723D_1ANT_COEX_ALGO_PANEDR_A2DP;
				}
			}
		}
	} else if (num_of_diff_profile == 3) {
		if (bt_link_info->sco_exist) {
			if (bt_link_info->hid_exist &&
			    bt_link_info->a2dp_exist) {
				RT_TRACE(
					rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
					"[BTCoex], BT Profile = SCO + HID + A2DP ==> HID\n");
				algorithm = BT_8723D_1ANT_COEX_ALGO_HID;
			} else if (bt_link_info->hid_exist &&
				   bt_link_info->pan_exist) {
				if (bt_hs_on) {
					RT_TRACE(
						rtlpriv, COMP_BT_COEXIST,
						DBG_LOUD,
						"[BTCoex], BT Profile = SCO + HID + PAN(HS)\n");
					algorithm =
					    BT_8723D_1ANT_COEX_ALGO_HID_A2DP;
				} else {
					RT_TRACE(
						rtlpriv, COMP_BT_COEXIST,
						DBG_LOUD,
						"[BTCoex], BT Profile = SCO + HID + PAN(EDR)\n");
					algorithm =
					    BT_8723D_1ANT_COEX_ALGO_PANEDR_HID;
				}
			} else if (bt_link_info->pan_exist &&
				   bt_link_info->a2dp_exist) {
				if (bt_hs_on) {
					RT_TRACE(
						rtlpriv, COMP_BT_COEXIST,
						DBG_LOUD,
						"[BTCoex], BT Profile = SCO + A2DP + PAN(HS)\n");
					algorithm = BT_8723D_1ANT_COEX_ALGO_SCO;
				} else {
					RT_TRACE(
						rtlpriv, COMP_BT_COEXIST,
						DBG_LOUD,
						"[BTCoex], BT Profile = SCO + A2DP + PAN(EDR) ==> HID\n");
					algorithm =
					    BT_8723D_1ANT_COEX_ALGO_PANEDR_HID;
				}
			}
		} else {
			if (bt_link_info->hid_exist &&
			    bt_link_info->pan_exist &&
			    bt_link_info->a2dp_exist) {
				if (bt_hs_on) {
					RT_TRACE(
						rtlpriv, COMP_BT_COEXIST,
						DBG_LOUD,
						"[BTCoex], BT Profile = HID + A2DP + PAN(HS)\n");
					algorithm =
					    BT_8723D_1ANT_COEX_ALGO_HID_A2DP;
				} else {
					RT_TRACE(
						rtlpriv, COMP_BT_COEXIST,
						DBG_LOUD,
						"[BTCoex], BT Profile = HID + A2DP + PAN(EDR)\n");
					algorithm =
					BT_8723D_1ANT_COEX_ALGO_HID_A2DP_PANEDR;
				}
			}
		}
	} else if (num_of_diff_profile >= 3) {
		if (bt_link_info->sco_exist) {
			if (bt_link_info->hid_exist &&
			    bt_link_info->pan_exist &&
			    bt_link_info->a2dp_exist) {
				if (bt_hs_on) {
					RT_TRACE(
						rtlpriv, COMP_BT_COEXIST,
						DBG_LOUD,
						"[BTCoex], Error!!! BT Profile = SCO + HID + A2DP + PAN(HS)\n");

				} else {
					RT_TRACE(
						rtlpriv, COMP_BT_COEXIST,
						DBG_LOUD,
						"[BTCoex], BT Profile = SCO + HID + A2DP + PAN(EDR)==>PAN(EDR)+HID\n");
					algorithm =
					    BT_8723D_1ANT_COEX_ALGO_PANEDR_HID;
				}
			}
		}
	}

	return algorithm;
}

static void halbtc8723d1ant_set_fw_low_penalty_ra(struct btc_coexist *btcoexist,
						  bool low_penalty_ra)
{
	u8 h2c_parameter[6] = {0};

	h2c_parameter[0] = 0x6; /* op_code, 0x6= Retry_Penalty */

	if (low_penalty_ra) {
		h2c_parameter[1] |= BIT(0);
		h2c_parameter[2] =
			0x00; /* normal rate except MCS7/6/5, OFDM54/48/36 */
		h2c_parameter[3] = 0xf7; /* MCS7 or OFDM54 */
		h2c_parameter[4] = 0xf8; /* MCS6 or OFDM48 */
		h2c_parameter[5] = 0xf9; /* MCS5 or OFDM36	 */
	}

	btcoexist->btc_fill_h2c(btcoexist, 0x69, 6, h2c_parameter);
}

static void halbtc8723d1ant_low_penalty_ra(struct btc_coexist *btcoexist,
					   bool force_exec, bool low_penalty_ra)
{
	coex_dm->cur_low_penalty_ra = low_penalty_ra;

	if (!force_exec) {
		if (coex_dm->pre_low_penalty_ra == coex_dm->cur_low_penalty_ra)
			return;
	}

	halbtc8723d1ant_set_fw_low_penalty_ra(btcoexist,
					      coex_dm->cur_low_penalty_ra);

	coex_dm->pre_low_penalty_ra = coex_dm->cur_low_penalty_ra;
}

static void halbtc8723d1ant_write_score_board(struct btc_coexist *btcoexist,
					      u16 bitpos, bool state)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;
	static u16 originalval = 0x8002, preval;

	if (state)
		originalval = originalval | bitpos;
	else
		originalval = originalval & (~bitpos);

	coex_sta->score_board_WB = originalval;

	if (originalval != preval) {
		preval = originalval;
		btcoexist->btc_write_2byte(btcoexist, 0xaa, originalval);
	} else {
		RT_TRACE(
			rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			"[BTCoex], %s: return for nochange\n", __func__);
	}
}

static void halbtc8723d1ant_read_score_board(struct btc_coexist *btcoexist,
					     u16 *score_board_val)
{
	*score_board_val =
		(btcoexist->btc_read_2byte(btcoexist, 0xaa)) & 0x7fff;
}

static void halbtc8723d1ant_post_state_to_bt(struct btc_coexist *btcoexist,
					     u16 type, bool state)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;

	RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		 "[BTCoex], %s: type = %d, state =%d\n", __func__,
		 type, state);

	halbtc8723d1ant_write_score_board(btcoexist, (u16)type, state);
}

static bool
halbtc8723d1ant_is_wifibt_status_changed(struct btc_coexist *btcoexist)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;
	static bool pre_wifi_busy, pre_under_4way, pre_bt_hs_on, pre_bt_off,
		pre_bt_slave, pre_hid_low_pri_tx_overhead, pre_wifi_under_lps,
		pre_bt_setup_link, pre_cck_lock, pre_cck_lock_warn;
	static u8 pre_hid_busy_num, pre_wl_noisy_level;
	bool wifi_busy = false, under_4way = false, bt_hs_on = false;
	bool wifi_connected = false;
	struct btc_bt_link_info *bt_link_info = &btcoexist->bt_link_info;

	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_CONNECTED,
			   &wifi_connected);
	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_BUSY, &wifi_busy);
	btcoexist->btc_get(btcoexist, BTC_GET_BL_HS_OPERATION, &bt_hs_on);
	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_4_WAY_PROGRESS,
			   &under_4way);

	if (coex_sta->bt_disabled != pre_bt_off) {
		pre_bt_off = coex_sta->bt_disabled;

		if (coex_sta->bt_disabled)
			RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
				 "[BTCoex], BT is disabled !!\n");
		else
			RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
				 "[BTCoex], BT is enabled !!\n");

		coex_sta->bt_coex_supported_feature = 0;
		coex_sta->bt_coex_supported_version = 0;
		coex_sta->bt_ble_scan_type = 0;
		coex_sta->bt_ble_scan_para[0] = 0;
		coex_sta->bt_ble_scan_para[1] = 0;
		coex_sta->bt_ble_scan_para[2] = 0;
		coex_sta->bt_reg_vendor_ac = 0xffff;
		coex_sta->bt_reg_vendor_ae = 0xffff;
		coex_sta->legacy_forbidden_slot = 0;
		coex_sta->le_forbidden_slot = 0;
		coex_sta->bt_a2dp_vendor_id = 0;
		coex_sta->bt_a2dp_device_name = 0;
		return true;
	}

	if (wifi_connected) {
		if (wifi_busy != pre_wifi_busy) {
			pre_wifi_busy = wifi_busy;

			if (wifi_busy)
				halbtc8723d1ant_post_state_to_bt(
					btcoexist,
					BT_8723D_1ANT_SCOREBOARD_UNDERTEST,
					true);
			else
				halbtc8723d1ant_post_state_to_bt(
					btcoexist,
					BT_8723D_1ANT_SCOREBOARD_UNDERTEST,
					false);
			return true;
		}
		if (under_4way != pre_under_4way) {
			pre_under_4way = under_4way;
			return true;
		}
		if (bt_hs_on != pre_bt_hs_on) {
			pre_bt_hs_on = bt_hs_on;
			return true;
		}
		if (coex_sta->wl_noisy_level != pre_wl_noisy_level) {
			pre_wl_noisy_level = coex_sta->wl_noisy_level;
			return true;
		}
		if (coex_sta->under_lps != pre_wifi_under_lps) {
			pre_wifi_under_lps = coex_sta->under_lps;
			if (coex_sta->under_lps)
				return true;
		}
		if (coex_sta->cck_lock != pre_cck_lock) {
			pre_cck_lock = coex_sta->cck_lock;
			return true;
		}
		if (coex_sta->cck_lock_warn != pre_cck_lock_warn) {
			pre_cck_lock_warn = coex_sta->cck_lock_warn;
			return true;
		}
	}

	if (!coex_sta->bt_disabled) {
		if (coex_sta->hid_busy_num != pre_hid_busy_num) {
			pre_hid_busy_num = coex_sta->hid_busy_num;
			return true;
		}

		if (bt_link_info->slave_role != pre_bt_slave) {
			pre_bt_slave = bt_link_info->slave_role;
			return true;
		}

		if (pre_hid_low_pri_tx_overhead !=
		    coex_sta->is_hid_low_pri_tx_overhead) {
			pre_hid_low_pri_tx_overhead =
				coex_sta->is_hid_low_pri_tx_overhead;
			return true;
		}

		if (pre_bt_setup_link != coex_sta->is_setup_link) {
			pre_bt_setup_link = coex_sta->is_setup_link;
			return true;
		}
	}

	return false;
}

static void
halbtc8723d1ant_monitor_bt_enable_disable(struct btc_coexist *btcoexist)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;
	static u32 bt_disable_cnt;
	bool bt_active = true, bt_disabled = false;
	u16 u16tmp;

	/* This function check if bt is disabled */

	/* Read BT on/off status from scoreboard[1],
	 * enable this only if BT patch support this feature
	 */
	halbtc8723d1ant_read_score_board(btcoexist, &u16tmp);

	bt_active = u16tmp & BIT(1);

	if (bt_active) {
		bt_disable_cnt = 0;
		bt_disabled = false;
		btcoexist->btc_set(btcoexist, BTC_SET_BL_BT_DISABLE,
				   &bt_disabled);
	} else {
		bt_disable_cnt++;
		if (bt_disable_cnt >= 2) {
			bt_disabled = true;
			bt_disable_cnt = 2;
		}

		btcoexist->btc_set(btcoexist, BTC_SET_BL_BT_DISABLE,
				   &bt_disabled);
	}

	if (bt_disabled)
		halbtc8723d1ant_low_penalty_ra(btcoexist, NORMAL_EXEC, false);
	else
		halbtc8723d1ant_low_penalty_ra(btcoexist, NORMAL_EXEC, true);

	if (coex_sta->bt_disabled != bt_disabled) {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], BT is from %s to %s!!\n",
			 (coex_sta->bt_disabled ? "disabled" : "enabled"),
			 (bt_disabled ? "disabled" : "enabled"));
		coex_sta->bt_disabled = bt_disabled;
	}
}

static void halbtc8723d1ant_enable_gnt_to_gpio(struct btc_coexist *btcoexist,
					       bool isenable)
{
#if BT_8723D_1ANT_COEX_DBG
	if (isenable) {
		btcoexist->btc_write_1byte_bitmask(btcoexist, 0x73, 0x8, 0x1);

		/* enable GNT_BT to GPIO debug */
		btcoexist->btc_write_1byte_bitmask(btcoexist, 0x4e, 0x40, 0x0);
		btcoexist->btc_write_1byte_bitmask(btcoexist, 0x67, 0x1, 0x0);

		/* 0x48[20] = 0  for GPIO14 =  GNT_WL*/
		btcoexist->btc_write_1byte_bitmask(btcoexist, 0x4a, 0x10, 0x0);
		/* 0x40[17] = 0  for GPIO14 =  GNT_WL*/
		btcoexist->btc_write_1byte_bitmask(btcoexist, 0x42, 0x02, 0x0);

		/* 0x66[9] = 0   for GPIO15 =  GNT_B T*/
		btcoexist->btc_write_1byte_bitmask(btcoexist, 0x67, 0x02, 0x0);
		/* 0x66[7] = 0   for GPIO15 =  GNT_BT*/
		btcoexist->btc_write_1byte_bitmask(btcoexist, 0x66, 0x80, 0x0);
		/* 0x8[8] = 0    for GPIO15 =  GNT_BT*/
		btcoexist->btc_write_1byte_bitmask(btcoexist, 0x9, 0x1, 0x0);

		/* BT Vendor Reg 0x76[0]=0 for GPIO15=GNT_BT, not set here*/
	} else {
		btcoexist->btc_write_1byte_bitmask(btcoexist, 0x73, 0x8, 0x0);

		/* Disable GNT_BT debug to GPIO, and enable chip_wakeup_host */
		btcoexist->btc_write_1byte_bitmask(btcoexist, 0x4e, 0x40, 0x1);
		btcoexist->btc_write_1byte_bitmask(btcoexist, 0x67, 0x1, 0x1);

		/* 0x48[20] = 0  for GPIO14 =  GNT_WL*/
		btcoexist->btc_write_1byte_bitmask(btcoexist, 0x4a, 0x10, 0x1);
	}

#endif
}

static u32
halbtc8723d1ant_ltecoex_indirect_read_reg(struct btc_coexist *btcoexist,
					  u16 reg_addr)
{
	u32 delay_count = 0;

	while (1) {
		if ((btcoexist->btc_read_1byte(btcoexist, 0x7c3) & BIT(5)) ==
		    0) {
			mdelay(50);
			delay_count++;
			if (delay_count >= 10) {
				delay_count = 0;
				break;
			}
		} else {
			break;
		}
	}

	/* wait for ready bit before access 0x7c0 */
	btcoexist->btc_write_4byte(btcoexist, 0x7c0, 0x800F0000 | reg_addr);

	return btcoexist->btc_read_4byte(btcoexist, 0x7c8); /* get read data */
}

static void
halbtc8723d1ant_ltecoex_indirect_write_reg(struct btc_coexist *btcoexist,
					   u16 reg_addr, u32 bit_mask,
					   u32 reg_value)
{
	u32 val, i = 0, bitpos = 0, delay_count = 0;

	if (bit_mask == 0x0)
		return;
	if (bit_mask == 0xffffffff) {
		/* wait for ready bit before access 0x7c0/0x7c4 */
		while (1) {
			if ((btcoexist->btc_read_1byte(btcoexist, 0x7c3) &
			     BIT(5)) == 0) {
				mdelay(50);
				delay_count++;
				if (delay_count >= 10) {
					delay_count = 0;
					break;
				}
			} else {
				break;
			}
		}

		btcoexist->btc_write_4byte(btcoexist, 0x7c4,
					   reg_value); /* put write data */

		btcoexist->btc_write_4byte(btcoexist, 0x7c0,
					   0xc00F0000 | reg_addr);
	} else {
		for (i = 0; i <= 31; i++) {
			if (((bit_mask >> i) & 0x1) == 0x1) {
				bitpos = i;
				break;
			}
		}

		/* read back register value before write */
		val = halbtc8723d1ant_ltecoex_indirect_read_reg(btcoexist,
								reg_addr);
		val = (val & (~bit_mask)) | (reg_value << bitpos);

		/* wait for ready bit before access 0x7c0/0x7c4 */
		while (1) {
			if ((btcoexist->btc_read_1byte(btcoexist, 0x7c3) &
			     BIT(5)) == 0) {
				mdelay(50);
				delay_count++;
				if (delay_count >= 10) {
					delay_count = 0;
					break;
				}
			} else {
				break;
			}
		}

		btcoexist->btc_write_4byte(btcoexist, 0x7c4,
					   val); /* put write data */

		btcoexist->btc_write_4byte(btcoexist, 0x7c0,
					   0xc00F0000 | reg_addr);
	}
}

static void halbtc8723d1ant_ltecoex_enable(struct btc_coexist *btcoexist,
					   bool enable)
{
	u8 val;

	val = (enable) ? 1 : 0;
	halbtc8723d1ant_ltecoex_indirect_write_reg(btcoexist, 0x38, 0x80,
						   val); /* 0x38[7] */
}

static void
halbtc8723d1ant_ltecoex_pathcontrol_owner(struct btc_coexist *btcoexist,
					  bool wifi_control)
{
	u8 val;

	val = (wifi_control) ? 1 : 0;
	btcoexist->btc_write_1byte_bitmask(btcoexist, 0x73, 0x4,
					   val); /* 0x70[26] */
}

static void halbtc8723d1ant_ltecoex_set_gnt_bt(struct btc_coexist *btcoexist,
					       u8 control_block,
					       bool sw_control, u8 state)
{
	u32 val = 0, val_orig = 0;

	if (!sw_control)
		val = 0x0;
	else if (state & 0x1)
		val = 0x3;
	else
		val = 0x1;

	val_orig = halbtc8723d1ant_ltecoex_indirect_read_reg(btcoexist, 0x38);

	switch (control_block) {
	case BT_8723D_1ANT_GNT_BLOCK_RFC_BB:
	default:
		val = ((val << 14) | (val << 10)) | (val_orig & 0xffff33ff);
		break;
	case BT_8723D_1ANT_GNT_BLOCK_RFC:
		val = (val << 14) | (val_orig & 0xffff3fff);
		break;
	case BT_8723D_1ANT_GNT_BLOCK_BB:
		val = (val << 10) | (val_orig & 0xfffff3ff);
		break;
	}

	halbtc8723d1ant_ltecoex_indirect_write_reg(btcoexist, 0x38, 0xffffffff,
						   val);
}

static void halbtc8723d1ant_ltecoex_set_gnt_wl(struct btc_coexist *btcoexist,
					       u8 control_block,
					       bool sw_control, u8 state)
{
	u32 val = 0, val_orig = 0;

	if (!sw_control)
		val = 0x0;
	else if (state & 0x1)
		val = 0x3;
	else
		val = 0x1;

	val_orig = halbtc8723d1ant_ltecoex_indirect_read_reg(btcoexist, 0x38);

	switch (control_block) {
	case BT_8723D_1ANT_GNT_BLOCK_RFC_BB:
	default:
		val = ((val << 12) | (val << 8)) | (val_orig & 0xffffccff);
		break;
	case BT_8723D_1ANT_GNT_BLOCK_RFC:
		val = (val << 12) | (val_orig & 0xffffcfff);
		break;
	case BT_8723D_1ANT_GNT_BLOCK_BB:
		val = (val << 8) | (val_orig & 0xfffffcff);
		break;
	}

	halbtc8723d1ant_ltecoex_indirect_write_reg(btcoexist, 0x38, 0xffffffff,
						   val);
}

static void
halbtc8723d1ant_ltecoex_set_coex_table(struct btc_coexist *btcoexist,
				       u8 table_type, u16 table_content)
{
	u16 reg_addr = 0x0000;

	switch (table_type) {
	case BT_8723D_1ANT_CTT_WL_VS_LTE:
		reg_addr = 0xa0;
		break;
	case BT_8723D_1ANT_CTT_BT_VS_LTE:
		reg_addr = 0xa4;
		break;
	}

	if (reg_addr != 0x0000)
		halbtc8723d1ant_ltecoex_indirect_write_reg(
			btcoexist, reg_addr, 0xffff,
			table_content); /* 0xa0[15:0] or 0xa4[15:0] */
}

static void halbtc8723d1ant_set_coex_table(struct btc_coexist *btcoexist,
					   u32 val0x6c0, u32 val0x6c4,
					   u32 val0x6c8, u8 val0x6cc)
{
	btcoexist->btc_write_4byte(btcoexist, 0x6c0, val0x6c0);

	btcoexist->btc_write_4byte(btcoexist, 0x6c4, val0x6c4);

	btcoexist->btc_write_4byte(btcoexist, 0x6c8, val0x6c8);

	btcoexist->btc_write_1byte(btcoexist, 0x6cc, val0x6cc);
}

static void halbtc8723d1ant_coex_table(struct btc_coexist *btcoexist,
				       bool force_exec, u32 val0x6c0,
				       u32 val0x6c4, u32 val0x6c8, u8 val0x6cc)
{
	coex_dm->cur_val0x6c0 = val0x6c0;
	coex_dm->cur_val0x6c4 = val0x6c4;
	coex_dm->cur_val0x6c8 = val0x6c8;
	coex_dm->cur_val0x6cc = val0x6cc;

	if (!force_exec) {
		if ((coex_dm->pre_val0x6c0 == coex_dm->cur_val0x6c0) &&
		    (coex_dm->pre_val0x6c4 == coex_dm->cur_val0x6c4) &&
		    (coex_dm->pre_val0x6c8 == coex_dm->cur_val0x6c8) &&
		    (coex_dm->pre_val0x6cc == coex_dm->cur_val0x6cc))
			return;
	}

	halbtc8723d1ant_set_coex_table(btcoexist, val0x6c0, val0x6c4, val0x6c8,
				       val0x6cc);

	coex_dm->pre_val0x6c0 = coex_dm->cur_val0x6c0;
	coex_dm->pre_val0x6c4 = coex_dm->cur_val0x6c4;
	coex_dm->pre_val0x6c8 = coex_dm->cur_val0x6c8;
	coex_dm->pre_val0x6cc = coex_dm->cur_val0x6cc;
}

static void halbtc8723d1ant_coex_table_with_type(struct btc_coexist *btcoexist,
						 bool force_exec, u8 type)
{
	u32 break_table;
	u8 select_table;

	coex_sta->coex_table_type = type;

	if (coex_sta->concurrent_rx_mode_on) {
		break_table = 0xf0ffffff; /* set WL hi-pri can break BT */
		select_table = 0xb; /* set Tx response = Hi-Pri
				     * (ex: Transmitting ACK,BA,CTS)
				     */
	} else {
		break_table = 0xffffff;
		select_table = 0x3;
	}

	switch (type) {
	case 0:
		halbtc8723d1ant_coex_table(btcoexist, force_exec, 0x55555555,
					   0x55555555, break_table,
					   select_table);
		break;
	case 1:
		halbtc8723d1ant_coex_table(btcoexist, force_exec, 0xa5555555,
					   0xaa5a5a5a, break_table,
					   select_table);
		break;
	case 2:
		halbtc8723d1ant_coex_table(btcoexist, force_exec, 0xaa5a5a5a,
					   0xaa5a5a5a, break_table,
					   select_table);
		break;
	case 3:
		halbtc8723d1ant_coex_table(btcoexist, force_exec, 0x55555555,
					   0x5a5a5a5a, break_table,
					   select_table);
		break;
	case 4:
		halbtc8723d1ant_coex_table(btcoexist, force_exec, 0xa5555555,
					   0xaa5a5a5a, break_table,
					   select_table);
		break;
	case 5:
		halbtc8723d1ant_coex_table(btcoexist, force_exec, 0x5a5a5a5a,
					   0x5a5a5a5a, break_table,
					   select_table);
		break;
	case 6:
		halbtc8723d1ant_coex_table(btcoexist, force_exec, 0xa5555555,
					   0xaa5a5a5a, break_table,
					   select_table);
		break;
	case 7:
		halbtc8723d1ant_coex_table(btcoexist, force_exec, 0xaa555555,
					   0xaa555555, break_table,
					   select_table);
		break;
	case 8:
		halbtc8723d1ant_coex_table(btcoexist, force_exec, 0xa5555555,
					   0xaaaa5aaa, break_table,
					   select_table);
		break;
	case 9:
		halbtc8723d1ant_coex_table(btcoexist, force_exec, 0x5a5a5a5a,
					   0xaaaa5aaa, break_table,
					   select_table);
		break;
	case 10:
		halbtc8723d1ant_coex_table(btcoexist, force_exec, 0xaaaaaaaa,
					   0xaaaaaaaa, break_table,
					   select_table);
		break;
	case 11:
		halbtc8723d1ant_coex_table(btcoexist, force_exec, 0xa5a55555,
					   0xaaaa5a5a, break_table,
					   select_table);
		break;
	case 12:
		halbtc8723d1ant_coex_table(btcoexist, force_exec, 0xa5555555,
					   0xaaaa5a5a, break_table,
					   select_table);
		break;
	case 13:
		halbtc8723d1ant_coex_table(btcoexist, force_exec, 0xaa5555aa,
					   0xaa5a5a5a, break_table,
					   select_table);
		break;
	case 14:
		halbtc8723d1ant_coex_table(btcoexist, force_exec, 0xaa5555aa,
					   0x5a5a5a5a, break_table,
					   select_table);
		break;
	case 15:
		halbtc8723d1ant_coex_table(btcoexist, force_exec, 0xaaaaaaaa,
					   0x5a5a5a5a, break_table,
					   select_table);
		break;
	default:
		break;
	}
}

static void
halbtc8723d1ant_set_fw_ignore_wlan_act(struct btc_coexist *btcoexist,
				       bool enable)
{
	u8 h2c_parameter[1] = {0};

	if (enable)
		h2c_parameter[0] |= BIT(0); /* function enable */

	btcoexist->btc_fill_h2c(btcoexist, 0x63, 1, h2c_parameter);
}

static void halbtc8723d1ant_ignore_wlan_act(struct btc_coexist *btcoexist,
					    bool force_exec, bool enable)
{
	coex_dm->cur_ignore_wlan_act = enable;

	if (!force_exec) {
		if (coex_dm->pre_ignore_wlan_act ==
		    coex_dm->cur_ignore_wlan_act)
			return;
	}
	halbtc8723d1ant_set_fw_ignore_wlan_act(btcoexist, enable);

	coex_dm->pre_ignore_wlan_act = coex_dm->cur_ignore_wlan_act;
}

static void halbtc8723d1ant_set_lps_rpwm(struct btc_coexist *btcoexist,
					 u8 lps_val, u8 rpwm_val)
{
	u8 lps = lps_val;
	u8 rpwm = rpwm_val;

	btcoexist->btc_set(btcoexist, BTC_SET_U1_LPS_VAL, &lps);
	btcoexist->btc_set(btcoexist, BTC_SET_U1_RPWM_VAL, &rpwm);
}

static void halbtc8723d1ant_lps_rpwm(struct btc_coexist *btcoexist,
				     bool force_exec, u8 lps_val, u8 rpwm_val)
{
	coex_dm->cur_lps = lps_val;
	coex_dm->cur_rpwm = rpwm_val;

	if (!force_exec) {
		if ((coex_dm->pre_lps == coex_dm->cur_lps) &&
		    (coex_dm->pre_rpwm == coex_dm->cur_rpwm))
			return;
	}
	halbtc8723d1ant_set_lps_rpwm(btcoexist, lps_val, rpwm_val);

	coex_dm->pre_lps = coex_dm->cur_lps;
	coex_dm->pre_rpwm = coex_dm->cur_rpwm;
}

static void halbtc8723d1ant_ps_tdma_check_for_power_save_state(
	struct btc_coexist *btcoexist, bool new_ps_state)
{
	u8 lps_mode = 0x0;
	u8 h2c_parameter[5] = {0x8, 0, 0, 0, 0};

	btcoexist->btc_get(btcoexist, BTC_GET_U1_LPS_MODE, &lps_mode);

	if (lps_mode) { /* already under LPS state */
		if (new_ps_state) {
			/* keep state under LPS, do nothing. */
		} else {
			/* will leave LPS state, turn off psTdma first */
			btcoexist->btc_fill_h2c(btcoexist, 0x60, 5,
						h2c_parameter);
		}
	} else { /* NO PS state */
		if (new_ps_state) {
			/* will enter LPS state, turn off psTdma first */
			btcoexist->btc_fill_h2c(btcoexist, 0x60, 5,
						h2c_parameter);
		} else {
			/* keep state under NO PS state, do nothing. */
		}
	}
}

static void halbtc8723d1ant_power_save_state(struct btc_coexist *btcoexist,
					     u8 ps_type, u8 lps_val,
					     u8 rpwm_val)
{
	bool low_pwr_disable = false;

	switch (ps_type) {
	case BTC_PS_WIFI_NATIVE:
		/* recover to original 32k low power setting */
		coex_sta->force_lps_ctrl = false;
		low_pwr_disable = false;
		btcoexist->btc_set(btcoexist, BTC_SET_ACT_PRE_NORMAL_LPS, NULL);

		break;
	case BTC_PS_LPS_ON:
		coex_sta->force_lps_ctrl = true;
		halbtc8723d1ant_ps_tdma_check_for_power_save_state(btcoexist,
								   true);
		halbtc8723d1ant_lps_rpwm(btcoexist, NORMAL_EXEC, lps_val,
					 rpwm_val);
		/* when coex force to enter LPS, do not enter 32k low power. */
		low_pwr_disable = true;
		btcoexist->btc_set(btcoexist, BTC_SET_ACT_DISABLE_LOW_POWER,
				   &low_pwr_disable);
		/* power save must executed before psTdma. */
		btcoexist->btc_set(btcoexist, BTC_SET_ACT_ENTER_LPS, NULL);

		break;
	case BTC_PS_LPS_OFF:
		coex_sta->force_lps_ctrl = true;
		halbtc8723d1ant_ps_tdma_check_for_power_save_state(btcoexist,
								   false);
		btcoexist->btc_set(btcoexist, BTC_SET_ACT_LEAVE_LPS, NULL);

		break;
	default:
		break;
	}
}

static void halbtc8723d1ant_set_fw_pstdma(struct btc_coexist *btcoexist,
					  u8 byte1, u8 byte2, u8 byte3,
					  u8 byte4, u8 byte5)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;

	u8 h2c_parameter[5] = {0};
	u8 real_byte1 = byte1, real_byte5 = byte5;
	bool ap_enable = false;
	struct btc_bt_link_info *bt_link_info = &btcoexist->bt_link_info;
	u8 ps_type = BTC_PS_WIFI_NATIVE;

	if (byte5 & BIT(2))
		coex_sta->is_tdma_btautoslot = true;
	else
		coex_sta->is_tdma_btautoslot = false;

	/* release bt-auto slot for auto-slot hang is detected!! */
	if (coex_sta->is_tdma_btautoslot)
		if ((coex_sta->is_tdma_btautoslot_hang) ||
		    (bt_link_info->slave_role))
			byte5 = byte5 & 0xfb;

	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_AP_MODE_ENABLE,
			   &ap_enable);

	if ((ap_enable) && (byte1 & BIT(4) && !(byte1 & BIT(5)))) {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], FW for AP mode\n");
		real_byte1 &= ~BIT(4);
		real_byte1 |= BIT(5);

		real_byte5 |= BIT(5);
		real_byte5 &= ~BIT(6);

		ps_type = BTC_PS_WIFI_NATIVE;
		halbtc8723d1ant_power_save_state(btcoexist, ps_type, 0x0, 0x0);
	} else if (byte1 & BIT(4) && !(byte1 & BIT(5))) {
		ps_type = BTC_PS_LPS_ON;
		halbtc8723d1ant_power_save_state(btcoexist, ps_type, 0x50, 0x4);
	} else {
		ps_type = BTC_PS_WIFI_NATIVE;
		halbtc8723d1ant_power_save_state(btcoexist, ps_type, 0x0, 0x0);
	}

	h2c_parameter[0] = real_byte1;
	h2c_parameter[1] = byte2;
	h2c_parameter[2] = byte3;
	h2c_parameter[3] = byte4;
	h2c_parameter[4] = real_byte5;

	coex_dm->ps_tdma_para[0] = real_byte1;
	coex_dm->ps_tdma_para[1] = byte2;
	coex_dm->ps_tdma_para[2] = byte3;
	coex_dm->ps_tdma_para[3] = byte4;
	coex_dm->ps_tdma_para[4] = real_byte5;

	btcoexist->btc_fill_h2c(btcoexist, 0x60, 5, h2c_parameter);

	if (ps_type == BTC_PS_WIFI_NATIVE)
		btcoexist->btc_set(btcoexist, BTC_SET_ACT_POST_NORMAL_LPS,
				   NULL);
}

static void halbtc8723d1ant_ps_tdma(struct btc_coexist *btcoexist,
				    bool force_exec, bool turn_on, u8 type)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;
	struct btc_bt_link_info *bt_link_info = &btcoexist->bt_link_info;
	bool wifi_busy = false;
	static u8 ps_tdma_byte4_modify, pre_ps_tdma_byte4_modify;
	static bool pre_wifi_busy;

	coex_dm->cur_ps_tdma_on = turn_on;
	coex_dm->cur_ps_tdma = type;

	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_BUSY, &wifi_busy);

	if (wifi_busy != pre_wifi_busy) {
		force_exec = true;
		pre_wifi_busy = wifi_busy;
	}

	/* 0x778 = 0x1 at wifi slot (no blocking BT Low-Pri pkts) */
	if ((bt_link_info->slave_role) && (bt_link_info->a2dp_exist))
		ps_tdma_byte4_modify = 0x1;
	else
		ps_tdma_byte4_modify = 0x0;

	if (pre_ps_tdma_byte4_modify != ps_tdma_byte4_modify) {
		force_exec = true;
		pre_ps_tdma_byte4_modify = ps_tdma_byte4_modify;
	}

	if (!force_exec) {
		if ((coex_dm->pre_ps_tdma_on == coex_dm->cur_ps_tdma_on) &&
		    (coex_dm->pre_ps_tdma == coex_dm->cur_ps_tdma))
			return;
	}

	if (coex_dm->cur_ps_tdma_on) {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], ********** TDMA(on, %d) **********\n",
			 coex_dm->cur_ps_tdma);

		btcoexist->btc_write_1byte_bitmask(
			btcoexist, 0x550, 0x8, 0x1); /* enable TBTT nterrupt */
	} else {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], ********** TDMA(off, %d) **********\n",
			 coex_dm->cur_ps_tdma);
	}

	if (turn_on) {
		switch (type) {
		default:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0x61, 0x35,
						      0x03, 0x11, 0x11);
			break;
		case 3:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0x51, 0x30,
						      0x03, 0x10, 0x50);
			break;
		case 4:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0x51, 0x21,
						      0x03, 0x10, 0x50);
			break;
		case 5:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0x61, 0x3a,
						      0x03, 0x11, 0x11);
			break;
		case 6:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0x61, 0x20,
						      0x03, 0x11, 0x11);
			break;
		case 7:
			halbtc8723d1ant_set_fw_pstdma(
				btcoexist, 0x51, 0x10, 0x03, 0x10,
				0x54 | ps_tdma_byte4_modify);
			break;
		case 8:
			halbtc8723d1ant_set_fw_pstdma(
				btcoexist, 0x51, 0x10, 0x03, 0x10,
				0x54 | ps_tdma_byte4_modify);
			break;
		case 9:
			halbtc8723d1ant_set_fw_pstdma(
				btcoexist, 0x51, 0x10, 0x03, 0x10,
				0x54 | ps_tdma_byte4_modify);
			break;
		case 10:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0x61, 0x30,
						      0x03, 0x11, 0x10);
			break;
		case 11:
			halbtc8723d1ant_set_fw_pstdma(
				btcoexist, 0x61, 0x25, 0x03, 0x11,
				0x11 | ps_tdma_byte4_modify);
			break;
		case 12:
			halbtc8723d1ant_set_fw_pstdma(
				btcoexist, 0x51, 0x35, 0x03, 0x10,
				0x50 | ps_tdma_byte4_modify);
			break;
		case 13:
			halbtc8723d1ant_set_fw_pstdma(
				btcoexist, 0x51, 0x10, 0x07, 0x10,
				0x54 | ps_tdma_byte4_modify);
			break;
		case 14:
			halbtc8723d1ant_set_fw_pstdma(
				btcoexist, 0x51, 0x15, 0x03, 0x10,
				0x50 | ps_tdma_byte4_modify);
			break;
		case 15:
			halbtc8723d1ant_set_fw_pstdma(
				btcoexist, 0x51, 0x20, 0x03, 0x10,
				0x50 | ps_tdma_byte4_modify);
			break;
		case 16:
			halbtc8723d1ant_set_fw_pstdma(
				btcoexist, 0x61, 0x10, 0x03, 0x11,
				0x15 | ps_tdma_byte4_modify);
			break;
		case 17:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0x61, 0x10,
						      0x03, 0x11, 0x14);
			break;
		case 18:
			halbtc8723d1ant_set_fw_pstdma(
				btcoexist, 0x51, 0x30, 0x03, 0x10,
				0x50 | ps_tdma_byte4_modify);
			break;
		case 19:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0x61, 0x15,
						      0x03, 0x11, 0x10);
			break;
		case 20:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0x61, 0x30,
						      0x03, 0x11, 0x10);
			break;
		case 21:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0x61, 0x30,
						      0x03, 0x11, 0x10);
			break;
		case 22:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0x61, 0x25,
						      0x03, 0x11, 0x10);
			break;
		case 23:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0x61, 0x10,
						      0x03, 0x11, 0x10);
			break;
		case 24:
			halbtc8723d1ant_set_fw_pstdma(
				btcoexist, 0x51, 0x08, 0x03, 0x10,
				0x54 | ps_tdma_byte4_modify);
			break;
		case 25:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0x51, 0x3a,
						      0x03, 0x11, 0x50);
			break;
		case 26:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0x51, 0x10,
						      0x03, 0x10, 0x55);
			break;
		case 27:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0x61, 0x10,
						      0x03, 0x11, 0x15);
			break;
		case 28:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0x51, 0x10,
						      0x0b, 0x10, 0x54);
			break;
		case 32:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0x61, 0x35,
						      0x03, 0x11, 0x11);
			break;
		case 33:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0x61, 0x35,
						      0x03, 0x11, 0x10);
			break;
		case 36:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0x61, 0x48,
						      0x03, 0x11, 0x10);
			break;
		case 57:
			halbtc8723d1ant_set_fw_pstdma(
				btcoexist, 0x51, 0x10, 0x03, 0x10,
				0x50 | ps_tdma_byte4_modify);
			break;
		case 58:
			halbtc8723d1ant_set_fw_pstdma(
				btcoexist, 0x51, 0x10, 0x03, 0x10,
				0x50 | ps_tdma_byte4_modify);
			break;
		case 67:
			halbtc8723d1ant_set_fw_pstdma(
				btcoexist, 0x61, 0x10, 0x03, 0x11,
				0x10 | ps_tdma_byte4_modify);
			break;
		/*     1-Ant to 2-Ant      TDMA case */
		case 103:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0xd3, 0x3a,
						      0x03, 0x70, 0x10);
			break;
		case 104:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0xd3, 0x21,
						      0x03, 0x70, 0x10);
			break;
		case 105:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0xe3, 0x15,
						      0x03, 0x71, 0x11);
			break;
		case 106:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0xe3, 0x20,
						      0x03, 0x71, 0x11);
			break;
		case 107:
			halbtc8723d1ant_set_fw_pstdma(
				btcoexist, 0xd3, 0x10, 0x03, 0x70,
				0x14 | ps_tdma_byte4_modify);
			break;
		case 108:
			halbtc8723d1ant_set_fw_pstdma(
				btcoexist, 0xd3, 0x10, 0x03, 0x70,
				0x14 | ps_tdma_byte4_modify);
			break;
		case 113:
			halbtc8723d1ant_set_fw_pstdma(
				btcoexist, 0xd3, 0x25, 0x03, 0x70,
				0x10 | ps_tdma_byte4_modify);
			break;
		case 114:
			halbtc8723d1ant_set_fw_pstdma(
				btcoexist, 0xd3, 0x15, 0x03, 0x70,
				0x10 | ps_tdma_byte4_modify);
			break;
		case 115:
			halbtc8723d1ant_set_fw_pstdma(
				btcoexist, 0xd3, 0x20, 0x03, 0x70,
				0x10 | ps_tdma_byte4_modify);
			break;
		case 117:
			halbtc8723d1ant_set_fw_pstdma(
				btcoexist, 0xe3, 0x10, 0x03, 0x71,
				0x14 | ps_tdma_byte4_modify);
			break;
		case 119:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0xe3, 0x15,
						      0x03, 0x71, 0x10);
			break;
		case 120:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0xe3, 0x30,
						      0x03, 0x71, 0x10);
			break;
		case 121:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0xe3, 0x30,
						      0x03, 0x71, 0x10);
			break;
		case 122:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0xe3, 0x25,
						      0x03, 0x71, 0x10);
			break;
		case 132:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0xe3, 0x35,
						      0x03, 0x71, 0x11);
			break;
		case 133:
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0xe3, 0x35,
						      0x03, 0x71, 0x10);
			break;
		}
	} else {
		/* disable PS tdma */
		switch (type) {
		case 8: /* PTA Control */
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0x8, 0x0, 0x0,
						      0x0, 0x0);
			break;
		case 0:
		default: /* Software control, Antenna at BT side */
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0x0, 0x0, 0x0,
						      0x0, 0x0);
			break;
		case 1: /* 2-Ant, 0x778=3, antenna ctrl by antenna diversity */
			halbtc8723d1ant_set_fw_pstdma(btcoexist, 0x0, 0x0, 0x0,
						      0x48, 0x0);
			break;
		}
	}

	coex_dm->pre_ps_tdma_on = coex_dm->cur_ps_tdma_on;
	coex_dm->pre_ps_tdma = coex_dm->cur_ps_tdma;
}

static void halbtc8723d1ant_set_ant_path(struct btc_coexist *btcoexist,
					 u8 ant_pos_type, bool force_exec,
					 u8 phase)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;
	struct btc_board_info *board_info = &btcoexist->board_info;
	u32 cnt_bt_cal_chk = 0;
	bool is_hw_ant_div_on = false;
	u8 u8tmp0 = 0, u8tmp1 = 0;
	u32 u32tmp1 = 0;
#if BT_8723D_1ANT_COEX_DBG
	u32 u32tmp2 = 0;
	u16 u16tmp0, u16tmp1 = 0;
#endif

	u32tmp1 = halbtc8723d1ant_ltecoex_indirect_read_reg(btcoexist, 0x38);

	/* To avoid indirect access fail	*/
	if (((u32tmp1 & 0xf000) >> 12) != ((u32tmp1 & 0x0f00) >> 8)) {
		force_exec = true;
		coex_sta->gnt_error_cnt++;
	}

#if BT_8723D_1ANT_COEX_DBG
	u32tmp2 = halbtc8723d1ant_ltecoex_indirect_read_reg(btcoexist, 0x54);
	u16tmp0 = btcoexist->btc_read_2byte(btcoexist, 0xaa);
	u16tmp1 = btcoexist->btc_read_2byte(btcoexist, 0x948);
	u8tmp1 = btcoexist->btc_read_1byte(btcoexist, 0x73);
	u8tmp0 = btcoexist->btc_read_1byte(btcoexist, 0x67);

	RT_TRACE(
		rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		"[BTCoex], ********** 0x67 = 0x%x, 0x948 = 0x%x, 0x73 = 0x%x(Before Set Ant Pat)\n",
		u8tmp0, u16tmp1, u8tmp1);

	RT_TRACE(
		rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		"[BTCoex], **********0x38= 0x%x, 0x54= 0x%x, 0xaa = 0x%x(Before Set Ant Path)\n",
		u32tmp1, u32tmp2, u16tmp0);
#endif

	coex_dm->cur_ant_pos_type = ant_pos_type;

	if (!force_exec) {
		if (coex_dm->cur_ant_pos_type == coex_dm->pre_ant_pos_type) {
			RT_TRACE(
				rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
				"[BTCoex], ********** Skip Antenna Path Setup because no change!!**********\n");
			return;
		}
	}

	coex_dm->pre_ant_pos_type = coex_dm->cur_ant_pos_type;

	switch (phase) {
	case BT_8723D_1ANT_PHASE_COEX_POWERON:
		/* Set Path control to WL */
		btcoexist->btc_write_1byte_bitmask(btcoexist, 0x67, 0x80, 0x0);

		/* set Path control owner to WL at initial step */
		halbtc8723d1ant_ltecoex_pathcontrol_owner(
			btcoexist, BT_8723D_1ANT_PCO_BTSIDE);

		/* set GNT_BT to SW high */
		halbtc8723d1ant_ltecoex_set_gnt_bt(
			btcoexist, BT_8723D_1ANT_GNT_BLOCK_RFC_BB,
			BT_8723D_1ANT_GNT_TYPE_CTRL_BY_SW,
			BT_8723D_1ANT_SIG_STA_SET_TO_HIGH);
		/* Set GNT_WL to SW low */
		halbtc8723d1ant_ltecoex_set_gnt_wl(
			btcoexist, BT_8723D_1ANT_GNT_BLOCK_RFC_BB,
			BT_8723D_1ANT_GNT_TYPE_CTRL_BY_SW,
			BT_8723D_1ANT_SIG_STA_SET_TO_HIGH);

		if (ant_pos_type == BTC_ANT_PATH_AUTO)
			ant_pos_type = BTC_ANT_PATH_BT;

		coex_sta->run_time_state = false;

		break;
	case BT_8723D_1ANT_PHASE_COEX_INIT:
		/* Disable LTE Coex Function in WiFi side
		 * (this should be on if LTE coex is required)
		 */
		halbtc8723d1ant_ltecoex_enable(btcoexist, 0x0);

		/* GNT_WL_LTE always = 1
		 * (this should be config if LTE coex is required)
		 */
		halbtc8723d1ant_ltecoex_set_coex_table(
			btcoexist, BT_8723D_1ANT_CTT_WL_VS_LTE, 0xffff);

		/* GNT_BT_LTE always = 1
		 * (this should be config if LTE coex is required)
		 */
		halbtc8723d1ant_ltecoex_set_coex_table(
			btcoexist, BT_8723D_1ANT_CTT_BT_VS_LTE, 0xffff);

		/* Wait If BT IQK running, because Path control owner is at BT
		 * during BT IQK (setup by WiFi firmware)
		 */
		while (cnt_bt_cal_chk <= 20) {
			u8tmp0 = btcoexist->btc_read_1byte(btcoexist, 0x49d);
			cnt_bt_cal_chk++;
			if (u8tmp0 & BIT(0)) {
				RT_TRACE(
					rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
					"[BTCoex], ########### BT is calibrating (wait cnt=%d) ###########\n",
					cnt_bt_cal_chk);
				mdelay(50);
			} else {
				RT_TRACE(
					rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
					"[BTCoex], ********** WL is NOT calibrating (wait cnt=%d)**********\n",
					cnt_bt_cal_chk);
				break;
			}
		}

		/* Set Path control to WL */
		btcoexist->btc_write_1byte_bitmask(btcoexist, 0x67, 0x80, 0x1);

		/* set Path control owner to WL at initial step */
		halbtc8723d1ant_ltecoex_pathcontrol_owner(
			btcoexist, BT_8723D_1ANT_PCO_WLSIDE);

		/* set GNT_BT to SW high */
		halbtc8723d1ant_ltecoex_set_gnt_bt(
			btcoexist, BT_8723D_1ANT_GNT_BLOCK_RFC_BB,
			BT_8723D_1ANT_GNT_TYPE_CTRL_BY_SW,
			BT_8723D_1ANT_SIG_STA_SET_TO_HIGH);
		/* Set GNT_WL to SW low */
		halbtc8723d1ant_ltecoex_set_gnt_wl(
			btcoexist, BT_8723D_1ANT_GNT_BLOCK_RFC_BB,
			BT_8723D_1ANT_GNT_TYPE_CTRL_BY_SW,
			BT_8723D_1ANT_SIG_STA_SET_TO_HIGH);

		if (ant_pos_type == BTC_ANT_PATH_AUTO)
			ant_pos_type = BTC_ANT_PATH_BT;

		coex_sta->run_time_state = false;
		break;
	case BT_8723D_1ANT_PHASE_WLANONLY_INIT:
		/* Disable LTE Coex Function in WiFi side
		 * (this should be on if LTE coex is required)
		 */
		halbtc8723d1ant_ltecoex_enable(btcoexist, 0x0);

		/* GNT_WL_LTE always = 1
		 * (this should be config if LTE coex is required)
		 */
		halbtc8723d1ant_ltecoex_set_coex_table(
			btcoexist, BT_8723D_1ANT_CTT_WL_VS_LTE, 0xffff);

		/* GNT_BT_LTE always = 1
		 * (this should be config if LTE coex is required)
		 */
		halbtc8723d1ant_ltecoex_set_coex_table(
			btcoexist, BT_8723D_1ANT_CTT_BT_VS_LTE, 0xffff);

		/* Set Path control to WL */
		btcoexist->btc_write_1byte_bitmask(btcoexist, 0x67, 0x80, 0x1);

		/* set Path control owner to WL at initial step */
		halbtc8723d1ant_ltecoex_pathcontrol_owner(
			btcoexist, BT_8723D_1ANT_PCO_WLSIDE);

		/* set GNT_BT to SW low */
		halbtc8723d1ant_ltecoex_set_gnt_bt(
			btcoexist, BT_8723D_1ANT_GNT_BLOCK_RFC_BB,
			BT_8723D_1ANT_GNT_TYPE_CTRL_BY_SW,
			BT_8723D_1ANT_SIG_STA_SET_TO_LOW);
		/* Set GNT_WL to SW high */
		halbtc8723d1ant_ltecoex_set_gnt_wl(
			btcoexist, BT_8723D_1ANT_GNT_BLOCK_RFC_BB,
			BT_8723D_1ANT_GNT_TYPE_CTRL_BY_SW,
			BT_8723D_1ANT_SIG_STA_SET_TO_HIGH);

		if (ant_pos_type == BTC_ANT_PATH_AUTO)
			ant_pos_type = BTC_ANT_PATH_WIFI;

		coex_sta->run_time_state = false;
		break;
	case BT_8723D_1ANT_PHASE_WLAN_OFF:
		/* Disable LTE Coex Function in WiFi side */
		halbtc8723d1ant_ltecoex_enable(btcoexist, 0x0);

		/* Set Path control to BT */
		btcoexist->btc_write_1byte_bitmask(btcoexist, 0x67, 0x80, 0x0);

		/* set Path control owner to BT */
		halbtc8723d1ant_ltecoex_pathcontrol_owner(
			btcoexist, BT_8723D_1ANT_PCO_BTSIDE);

		if (ant_pos_type == BTC_ANT_PATH_AUTO)
			ant_pos_type = BTC_ANT_PATH_BT;

		coex_sta->run_time_state = false;
		break;
	case BT_8723D_1ANT_PHASE_2G_RUNTIME:

		/* wait for WL/BT IQK finish, keep 0x38 = 0xff00 for WL IQK */
		while (cnt_bt_cal_chk <= 20) {
			u8tmp0 = btcoexist->btc_read_1byte(btcoexist, 0x1e6);

			u8tmp1 = btcoexist->btc_read_1byte(btcoexist, 0x49d);

			cnt_bt_cal_chk++;
			if ((u8tmp0 & BIT(0)) || (u8tmp1 & BIT(0))) {
				RT_TRACE(
					rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
					"[BTCoex], ########### WL or BT is IQK (wait cnt=%d)\n",
					cnt_bt_cal_chk);
				mdelay(50);
			} else {
				RT_TRACE(
					rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
					"[BTCoex], ********** WL and BT is NOT IQK (wait cnt=%d)\n",
					cnt_bt_cal_chk);
				break;
			}
		}

		halbtc8723d1ant_ltecoex_pathcontrol_owner(
			btcoexist, BT_8723D_1ANT_PCO_WLSIDE);

		/* set GNT_BT to PTA */
		halbtc8723d1ant_ltecoex_set_gnt_bt(
			btcoexist, BT_8723D_1ANT_GNT_BLOCK_RFC_BB,
			BT_8723D_1ANT_GNT_TYPE_CTRL_BY_PTA,
			BT_8723D_1ANT_SIG_STA_SET_BY_HW);
		/* Set GNT_WL to PTA */
		halbtc8723d1ant_ltecoex_set_gnt_wl(
			btcoexist, BT_8723D_1ANT_GNT_BLOCK_RFC_BB,
			BT_8723D_1ANT_GNT_TYPE_CTRL_BY_PTA,
			BT_8723D_1ANT_SIG_STA_SET_BY_HW);

		if (ant_pos_type == BTC_ANT_PATH_AUTO)
			ant_pos_type = BTC_ANT_PATH_PTA;

		coex_sta->run_time_state = true;
		break;
	case BT_8723D_1ANT_PHASE_BTMPMODE:
		halbtc8723d1ant_ltecoex_pathcontrol_owner(
			btcoexist, BT_8723D_1ANT_PCO_WLSIDE);

		/* Set Path control to WL */
		btcoexist->btc_write_1byte_bitmask(btcoexist, 0x67, 0x80, 0x1);

		/* set GNT_BT to high */
		halbtc8723d1ant_ltecoex_set_gnt_bt(
			btcoexist, BT_8723D_1ANT_GNT_BLOCK_RFC_BB,
			BT_8723D_1ANT_GNT_TYPE_CTRL_BY_SW,
			BT_8723D_1ANT_SIG_STA_SET_TO_HIGH);
		/* Set GNT_WL to low */
		halbtc8723d1ant_ltecoex_set_gnt_wl(
			btcoexist, BT_8723D_1ANT_GNT_BLOCK_RFC_BB,
			BT_8723D_1ANT_GNT_TYPE_CTRL_BY_SW,
			BT_8723D_1ANT_SIG_STA_SET_TO_LOW);

		if (ant_pos_type == BTC_ANT_PATH_AUTO)
			ant_pos_type = BTC_ANT_PATH_BT;

		coex_sta->run_time_state = false;
		break;
	case BT_8723D_1ANT_PHASE_ANTENNA_DET:
		halbtc8723d1ant_ltecoex_pathcontrol_owner(
			btcoexist, BT_8723D_1ANT_PCO_WLSIDE);

		/* Set Path control to WL */
		btcoexist->btc_write_1byte_bitmask(btcoexist, 0x67, 0x80, 0x1);

		/* set GNT_BT to high */
		halbtc8723d1ant_ltecoex_set_gnt_bt(
			btcoexist, BT_8723D_1ANT_GNT_BLOCK_RFC_BB,
			BT_8723D_1ANT_GNT_TYPE_CTRL_BY_SW,
			BT_8723D_1ANT_SIG_STA_SET_TO_HIGH);
		/* Set GNT_WL to high */
		halbtc8723d1ant_ltecoex_set_gnt_wl(
			btcoexist, BT_8723D_1ANT_GNT_BLOCK_RFC_BB,
			BT_8723D_1ANT_GNT_TYPE_CTRL_BY_SW,
			BT_8723D_1ANT_SIG_STA_SET_TO_HIGH);

		if (ant_pos_type == BTC_ANT_PATH_AUTO)
			ant_pos_type = BTC_ANT_PATH_BT;

		coex_sta->run_time_state = false;

		break;
	}

	is_hw_ant_div_on = board_info->ant_div_cfg;

	if ((is_hw_ant_div_on) && (phase != BT_8723D_1ANT_PHASE_ANTENNA_DET))

		if (board_info->btdm_ant_pos == BTC_ANTENNA_AT_MAIN_PORT)
			/* 0x948 = 0x200, 0x0 while antenna diversity */
			btcoexist->btc_write_2byte(btcoexist, 0x948, 0x100);
		else /* 0x948 = 0x80, 0x0 while antenna diversity */
			btcoexist->btc_write_2byte(btcoexist, 0x948, 0x40);

	else if (!is_hw_ant_div_on &&
		 (phase !=
		  BT_8723D_1ANT_PHASE_WLAN_OFF)) { /* internal switch setting */

		switch (ant_pos_type) {
		case BTC_ANT_PATH_WIFI:
			if (board_info->btdm_ant_pos ==
			    BTC_ANTENNA_AT_MAIN_PORT)

				btcoexist->btc_write_2byte(btcoexist, 0x948,
							   0x0);
			else
				btcoexist->btc_write_2byte(btcoexist, 0x948,
							   0x280);

			break;
		case BTC_ANT_PATH_BT:
			if (board_info->btdm_ant_pos ==
			    BTC_ANTENNA_AT_MAIN_PORT)

				btcoexist->btc_write_2byte(btcoexist, 0x948,
							   0x280);
			else
				btcoexist->btc_write_2byte(btcoexist, 0x948,
							   0x0);

			break;
		default:
		case BTC_ANT_PATH_PTA:
			if (board_info->btdm_ant_pos ==
			    BTC_ANTENNA_AT_MAIN_PORT)
				btcoexist->btc_write_2byte(btcoexist, 0x948,
							   0x200);
			else
				btcoexist->btc_write_2byte(btcoexist, 0x948,
							   0x80);
			break;
		}
	}

#if BT_8723D_1ANT_COEX_DBG
	u32tmp1 = halbtc8723d1ant_ltecoex_indirect_read_reg(btcoexist, 0x38);
	u32tmp2 = halbtc8723d1ant_ltecoex_indirect_read_reg(btcoexist, 0x54);
	u16tmp0 = btcoexist->btc_read_2byte(btcoexist, 0xaa);
	u16tmp1 = btcoexist->btc_read_2byte(btcoexist, 0x948);
	u8tmp1 = btcoexist->btc_read_1byte(btcoexist, 0x73);
	u8tmp0 = btcoexist->btc_read_1byte(btcoexist, 0x67);

	RT_TRACE(
		rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		"[BTCoex], ********** 0x67 = 0x%x, 0x948 = 0x%x, 0x73 = 0x%x(After Set Ant Pat)\n",
		u8tmp0, u16tmp1, u8tmp1);

	RT_TRACE(
		rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		"[BTCoex], **********0x38= 0x%x, 0x54= 0x%x, 0xaa = 0x%x(After Set Ant Path)\n",
		u32tmp1, u32tmp2, u16tmp0);
#endif
}

static bool halbtc8723d1ant_is_common_action(struct btc_coexist *btcoexist)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;

	bool common = false, wifi_connected = false, wifi_busy = false;

	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_CONNECTED,
			   &wifi_connected);
	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_BUSY, &wifi_busy);

	if (!wifi_connected &&
	    coex_dm->bt_status == BT_8723D_1ANT_BT_STATUS_NON_CONNECTED_IDLE) {
		RT_TRACE(
			rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			"[BTCoex], Wifi non connected-idle + BT non connected-idle!!\n");
		common = true;
	} else if (wifi_connected &&
		   (coex_dm->bt_status ==
		    BT_8723D_1ANT_BT_STATUS_NON_CONNECTED_IDLE)) {
		RT_TRACE(
			rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			"[BTCoex], Wifi connected + BT non connected-idle!!\n");
		common = true;
	} else if (!wifi_connected &&
		   (coex_dm->bt_status ==
		    BT_8723D_1ANT_BT_STATUS_CONNECTED_IDLE)) {
		RT_TRACE(
			rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			"[BTCoex], Wifi non connected-idle + BT connected-idle!!\n");
		common = true;
	} else if (wifi_connected && (coex_dm->bt_status ==
				      BT_8723D_1ANT_BT_STATUS_CONNECTED_IDLE)) {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], Wifi connected + BT connected-idle!!\n");
		common = true;
	} else if (!wifi_connected &&
		   (coex_dm->bt_status !=
		    BT_8723D_1ANT_BT_STATUS_CONNECTED_IDLE)) {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], Wifi non connected-idle + BT Busy!!\n");
		common = true;
	} else {
		if (wifi_busy) {
			RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
				 "[BTCoex], Wifi Connected-Busy + BT Busy!!\n");
		} else {
			RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
				 "[BTCoex], Wifi Connected-Idle + BT Busy!!\n");
		}

		common = false;
	}

	return common;
}

/* *********************************************
 *
 *	Non-Software Coex Mechanism start
 *
 * **********************************************/
static void halbtc8723d1ant_action_bt_whql_test(struct btc_coexist *btcoexist)
{
	halbtc8723d1ant_set_ant_path(btcoexist, BTC_ANT_PATH_AUTO, NORMAL_EXEC,
				     BT_8723D_1ANT_PHASE_2G_RUNTIME);
	halbtc8723d1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 0);
	halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, false, 8);
}

static void halbtc8723d1ant_action_bt_hs(struct btc_coexist *btcoexist)
{
	halbtc8723d1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 2);
	halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 5);
}

static void halbtc8723d1ant_action_bt_relink(struct btc_coexist *btcoexist)
{
	struct btc_bt_link_info *bt_link_info = &btcoexist->bt_link_info;

	if (((!coex_sta->is_bt_multi_link) && (!bt_link_info->pan_exist)) ||
	    ((bt_link_info->a2dp_exist) && (bt_link_info->hid_exist))) {
		halbtc8723d1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 1);
		halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 5);
	}
}

static void halbtc8723d1ant_action_bt_idle(struct btc_coexist *btcoexist)
{
	bool wifi_busy = false;

	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_BUSY, &wifi_busy);

	if (!wifi_busy) {
		halbtc8723d1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 3);
		halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 6);

		return;
	}

	/* if wl not busy */
	if (coex_dm->bt_status == BT_8723D_1ANT_BT_STATUS_NON_CONNECTED_IDLE) {
		halbtc8723d1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 8);

		halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 33);
	} else {
		halbtc8723d1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 8);
		halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 32);
	}
}

static void halbtc8723d1ant_action_bt_inquiry(struct btc_coexist *btcoexist)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;

	struct btc_bt_link_info *bt_link_info = &btcoexist->bt_link_info;
	bool wifi_connected = false, wifi_busy = false, bt_busy = false;
	bool wifi_scan = false, wifi_link = false, wifi_roam = false;

	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_CONNECTED,
			   &wifi_connected);
	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_BUSY, &wifi_busy);
	btcoexist->btc_set(btcoexist, BTC_SET_BL_BT_TRAFFIC_BUSY, &bt_busy);

	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_SCAN, &wifi_scan);
	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_LINK, &wifi_link);
	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_ROAM, &wifi_roam);

	if ((coex_sta->bt_create_connection) &&
	    ((wifi_link) || (wifi_roam) || (wifi_scan) ||
	     (coex_sta->wifi_is_high_pri_task))) {
		RT_TRACE(
			rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			"[BTCoex], Wifi link/roam/Scan/busy/hi-pri-task + BT Inq/Page!!\n");

		halbtc8723d1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 1);

		if ((bt_link_info->a2dp_exist) && (!bt_link_info->pan_exist))
			halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true,
						17);
		else
			halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true,
						33);
	} else if ((!wifi_connected) && (!wifi_scan)) {
		halbtc8723d1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 0);

		halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, false, 8);
	} else if (bt_link_info->pan_exist) {
		halbtc8723d1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 4);

		halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 22);
	} else if (bt_link_info->a2dp_exist) {
		halbtc8723d1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 4);

		halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 16);
	} else {
		halbtc8723d1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 4);

		if ((wifi_link) || (wifi_roam) || (wifi_scan) ||
		    (coex_sta->wifi_is_high_pri_task))
			halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true,
						21);
		else
			halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true,
						23);
	}
}

static void
halbtc8723d1ant_action_bt_sco_hid_only_busy(struct btc_coexist *btcoexist)
{
	struct btc_bt_link_info *bt_link_info = &btcoexist->bt_link_info;
	bool wifi_connected = false, wifi_busy = false,
	     wifi_cckdeadlock_ap = false;
	u32 wifi_bw = 1;
	u8 iot_peer = BTC_IOT_PEER_UNKNOWN;

	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_CONNECTED,
			   &wifi_connected);
	btcoexist->btc_get(btcoexist, BTC_GET_U4_WIFI_BW, &wifi_bw);
	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_BUSY, &wifi_busy);
	btcoexist->btc_get(btcoexist, BTC_GET_U1_IOT_PEER, &iot_peer);

	if ((iot_peer == BTC_IOT_PEER_ATHEROS) && (coex_sta->cck_lock_ever))
		wifi_cckdeadlock_ap = true;

	if (bt_link_info->sco_exist) {
		if (coex_sta->is_bt_multi_link) {
			if (coex_sta->specific_pkt_period_cnt > 0)
				halbtc8723d1ant_coex_table_with_type(
					btcoexist, NORMAL_EXEC, 15);
			else
				halbtc8723d1ant_coex_table_with_type(
					btcoexist, NORMAL_EXEC, 3);
			halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true,
						25);
		} else {
			halbtc8723d1ant_coex_table_with_type(btcoexist,
							     NORMAL_EXEC, 1);
			halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true,
						5);
		}
	} else if (coex_sta->is_hid_rcu) {
		halbtc8723d1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 3);

		if (wifi_busy)
			halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true,
						36);
		else
			halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true,
						6);
	} else {
		if ((wifi_cckdeadlock_ap) &&
		    (coex_sta->is_hid_low_pri_tx_overhead)) {
			if (coex_sta->hid_busy_num < 2)
				halbtc8723d1ant_coex_table_with_type(
					btcoexist, NORMAL_EXEC, 14);
			else
				halbtc8723d1ant_coex_table_with_type(
					btcoexist, NORMAL_EXEC, 13);
			halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true,
						18);
		} else if (coex_sta->is_hid_low_pri_tx_overhead) {
			if (coex_sta->hid_busy_num < 2)
				halbtc8723d1ant_coex_table_with_type(
					btcoexist, NORMAL_EXEC, 3);
			else
				halbtc8723d1ant_coex_table_with_type(
					btcoexist, NORMAL_EXEC, 6);
			halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true,
						18);
		} else if (coex_sta->hid_busy_num < 2) {
			halbtc8723d1ant_coex_table_with_type(btcoexist,
							     NORMAL_EXEC, 3);
			halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true,
						11);
		} else if (wifi_bw == 0) { /* if 11bg mode */
			halbtc8723d1ant_coex_table_with_type(btcoexist,
							     NORMAL_EXEC, 11);
			halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true,
						11);
		} else {
			halbtc8723d1ant_coex_table_with_type(btcoexist,
							     NORMAL_EXEC, 6);
			halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true,
						11);
		}
	}
}

static void halbtc8723d1ant_action_wifi_only(struct btc_coexist *btcoexist)
{
	halbtc8723d1ant_set_ant_path(btcoexist, BTC_ANT_PATH_AUTO, FORCE_EXEC,
				     BT_8723D_1ANT_PHASE_2G_RUNTIME);
	halbtc8723d1ant_coex_table_with_type(btcoexist, FORCE_EXEC, 10);
	halbtc8723d1ant_ps_tdma(btcoexist, FORCE_EXEC, false, 8);
}

static void
halbtc8723d1ant_action_wifi_native_lps(struct btc_coexist *btcoexist)
{
	halbtc8723d1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 5);
	halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, false, 8);
}

static void
halbtc8723d1ant_action_wifi_multi_port(struct btc_coexist *btcoexist)
{
	struct btc_bt_link_info *bt_link_info = &btcoexist->bt_link_info;
	u8 multi_port_type;

	halbtc8723d1ant_set_ant_path(btcoexist, BTC_ANT_PATH_AUTO, NORMAL_EXEC,
				     BT_8723D_1ANT_PHASE_2G_RUNTIME);

	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_DUAL_BAND_CONNECTED,
			   &multi_port_type);

	if (multi_port_type == BTC_MULTIPORT_SCC) {
		if (bt_link_info->a2dp_exist) {
			halbtc8723d1ant_coex_table_with_type(btcoexist,
							     NORMAL_EXEC, 1);
			halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true,
						17);
		} else if (bt_link_info->pan_exist) {
			halbtc8723d1ant_coex_table_with_type(btcoexist,
							     NORMAL_EXEC, 1);
			halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true,
						33);
		} else {
			halbtc8723d1ant_coex_table_with_type(btcoexist,
							     NORMAL_EXEC, 7);
			halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, false,
						8);
		}
	} else {
		if ((coex_dm->bt_status ==
		     BT_8723D_1ANT_BT_STATUS_NON_CONNECTED_IDLE) ||
		    (coex_dm->bt_status ==
		     BT_8723D_1ANT_BT_STATUS_CONNECTED_IDLE))
			halbtc8723d1ant_coex_table_with_type(btcoexist,
							     NORMAL_EXEC, 7);
		else if (!bt_link_info->pan_exist)
			halbtc8723d1ant_coex_table_with_type(btcoexist,
							     NORMAL_EXEC, 7);
		else
			halbtc8723d1ant_coex_table_with_type(btcoexist,
							     NORMAL_EXEC, 2);

		halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, false, 8);
	}
}

static void
halbtc8723d1ant_action_wifi_linkscan_process(struct btc_coexist *btcoexist)
{
	struct btc_bt_link_info *bt_link_info = &btcoexist->bt_link_info;

	halbtc8723d1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 4);

	if (bt_link_info->pan_exist)
		halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 22);
	else if (bt_link_info->a2dp_exist)
		halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 27);
	else
		halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 21);
}

static void
halbtc8723d1ant_action_wifi_connected_bt_acl_busy(struct btc_coexist *btcoexist)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;

	struct btc_bt_link_info *bt_link_info = &btcoexist->bt_link_info;
	bool wifi_busy = false, wifi_turbo = false, wifi_cckdeadlock_ap = false;
	u32 wifi_bw = 1;
	u8 iot_peer = BTC_IOT_PEER_UNKNOWN;

	btcoexist->btc_get(btcoexist, BTC_GET_U4_WIFI_BW, &wifi_bw);
	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_BUSY, &wifi_busy);
	btcoexist->btc_get(btcoexist, BTC_GET_U1_AP_NUM,
			   &coex_sta->scan_ap_num);
	btcoexist->btc_get(btcoexist, BTC_GET_U1_IOT_PEER, &iot_peer);

	RT_TRACE(
		rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		"############# [BTCoex],  scan_ap_num = %d, wl_noisy_level = %d\n",
		coex_sta->scan_ap_num, coex_sta->wl_noisy_level);

	if ((wifi_busy) && (coex_sta->wl_noisy_level == 0))
		wifi_turbo = true;

	if ((iot_peer == BTC_IOT_PEER_ATHEROS) && (coex_sta->cck_lock_ever))
		wifi_cckdeadlock_ap = true;

	if ((bt_link_info->a2dp_exist) && (coex_sta->is_bt_a2dp_sink)) {
		if (wifi_cckdeadlock_ap)
			halbtc8723d1ant_coex_table_with_type(btcoexist,
							     NORMAL_EXEC, 13);
		else
			halbtc8723d1ant_coex_table_with_type(btcoexist,
							     NORMAL_EXEC, 6);

		halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 12);
	} else if (bt_link_info->a2dp_only) { /* A2DP */

		if (wifi_cckdeadlock_ap)
			halbtc8723d1ant_coex_table_with_type(btcoexist,
							     NORMAL_EXEC, 13);
		else if (wifi_turbo)
			halbtc8723d1ant_coex_table_with_type(btcoexist,
							     NORMAL_EXEC, 8);
		else
			halbtc8723d1ant_coex_table_with_type(btcoexist,
							     NORMAL_EXEC, 4);

		if (coex_sta->connect_ap_period_cnt > 0)
			halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true,
						26);
		else
			halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true,
						7);

	} else if (((bt_link_info->a2dp_exist) && (bt_link_info->pan_exist)) ||
		   (bt_link_info->hid_exist && bt_link_info->a2dp_exist &&
		    bt_link_info->pan_exist)) {
		/* A2DP+PAN(OPP,FTP), HID+A2DP+PAN(OPP,FTP) */

		if (wifi_cckdeadlock_ap) {
			if ((bt_link_info->hid_exist) &&
			    (coex_sta->hid_busy_num < 2))
				halbtc8723d1ant_coex_table_with_type(
					btcoexist, NORMAL_EXEC, 14);
			else
				halbtc8723d1ant_coex_table_with_type(
					btcoexist, NORMAL_EXEC, 13);
		} else if (bt_link_info->hid_exist) {
			if (coex_sta->hid_busy_num < 2)
				halbtc8723d1ant_coex_table_with_type(
					btcoexist, NORMAL_EXEC, 3);
			else
				halbtc8723d1ant_coex_table_with_type(
					btcoexist, NORMAL_EXEC, 1);
		} else if (wifi_turbo)
			halbtc8723d1ant_coex_table_with_type(btcoexist,
							     NORMAL_EXEC, 8);
		else
			halbtc8723d1ant_coex_table_with_type(btcoexist,
							     NORMAL_EXEC, 4);

		if (wifi_busy)
			halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true,
						13);
		else
			halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true,
						14);
	} else if (bt_link_info->hid_exist &&
		   bt_link_info->a2dp_exist) { /* HID+A2DP */

		if (wifi_cckdeadlock_ap) {
			if (coex_sta->hid_busy_num < 2)
				halbtc8723d1ant_coex_table_with_type(
					btcoexist, NORMAL_EXEC, 14);
			else
				halbtc8723d1ant_coex_table_with_type(
					btcoexist, NORMAL_EXEC, 13);

			if (coex_sta->hid_pair_cnt > 1)
				halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC,
							true, 24);
			else
				halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC,
							true, 8);

		} else {
			if (coex_sta->hid_busy_num < 2) /* 2/18 HID */
				halbtc8723d1ant_coex_table_with_type(
					btcoexist, NORMAL_EXEC, 3);
			else if (wifi_bw == 0) /* if 11bg mode */
				halbtc8723d1ant_coex_table_with_type(
					btcoexist, NORMAL_EXEC, 12);
			else
				halbtc8723d1ant_coex_table_with_type(
					btcoexist, NORMAL_EXEC, 1);

			if (coex_sta->hid_pair_cnt > 1)
				halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC,
							true, 24);
			else
				halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC,
							true, 8);
		}

	} else if ((bt_link_info->pan_only) ||
		   (bt_link_info->hid_exist && bt_link_info->pan_exist)) {
		/* PAN(OPP,FTP), HID+PAN(OPP,FTP) */

		if (coex_sta->cck_lock_ever) {
			if ((bt_link_info->hid_exist) &&
			    (coex_sta->hid_busy_num < 2))
				halbtc8723d1ant_coex_table_with_type(
					btcoexist, NORMAL_EXEC, 14);
			else
				halbtc8723d1ant_coex_table_with_type(
					btcoexist, NORMAL_EXEC, 13);
		} else if (bt_link_info->hid_exist) {
			if (coex_sta->hid_busy_num < 2)
				halbtc8723d1ant_coex_table_with_type(
					btcoexist, NORMAL_EXEC, 3);
			else
				halbtc8723d1ant_coex_table_with_type(
					btcoexist, NORMAL_EXEC, 1);
		} else if (wifi_turbo)
			halbtc8723d1ant_coex_table_with_type(btcoexist,
							     NORMAL_EXEC, 8);
		else
			halbtc8723d1ant_coex_table_with_type(btcoexist,
							     NORMAL_EXEC, 4);

		if (!wifi_busy)
			halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true,
						4);
		else
			halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true,
						3);
	} else {
		/* BT no-profile busy (0x9) */
		halbtc8723d1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 4);
		halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 33);
	}
}

static void
halbtc8723d1ant_action_wifi_not_connected(struct btc_coexist *btcoexist)
{
	halbtc8723d1ant_set_ant_path(btcoexist, BTC_ANT_PATH_AUTO, NORMAL_EXEC,
				     BT_8723D_1ANT_PHASE_2G_RUNTIME);
	/* tdma and coex table */
	halbtc8723d1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 0);
	halbtc8723d1ant_ps_tdma(btcoexist, FORCE_EXEC, false, 8);
}

static void halbtc8723d1ant_action_wifi_connected(struct btc_coexist *btcoexist)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;

	struct btc_bt_link_info *bt_link_info = &btcoexist->bt_link_info;

	RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		 "[BTCoex], CoexForWifiConnect()===>\n");

	halbtc8723d1ant_set_ant_path(btcoexist, BTC_ANT_PATH_AUTO, NORMAL_EXEC,
				     BT_8723D_1ANT_PHASE_2G_RUNTIME);

	if ((coex_dm->bt_status == BT_8723D_1ANT_BT_STATUS_ACL_BUSY) ||
	    (coex_dm->bt_status == BT_8723D_1ANT_BT_STATUS_ACL_SCO_BUSY)) {
		if (bt_link_info->hid_only) /* HID only */
			halbtc8723d1ant_action_bt_sco_hid_only_busy(btcoexist);
		else
			halbtc8723d1ant_action_wifi_connected_bt_acl_busy(
				btcoexist);

	} else if (coex_dm->bt_status == BT_8723D_1ANT_BT_STATUS_SCO_BUSY) {
		halbtc8723d1ant_action_bt_sco_hid_only_busy(btcoexist);
	} else {
		halbtc8723d1ant_action_bt_idle(btcoexist);
	}
}

static void
halbtc8723d1ant_run_sw_coexist_mechanism(struct btc_coexist *btcoexist)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;

	u8 algorithm = 0;

	algorithm = halbtc8723d1ant_action_algorithm(btcoexist);
	coex_dm->cur_algorithm = algorithm;

	if (halbtc8723d1ant_is_common_action(btcoexist)) {
	} else {
		switch (coex_dm->cur_algorithm) {
		case BT_8723D_1ANT_COEX_ALGO_SCO:
			RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
				 "[BTCoex], Action algorithm = SCO.\n");
			break;
		case BT_8723D_1ANT_COEX_ALGO_HID:
			RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
				 "[BTCoex], Action algorithm = HID.\n");
			break;
		case BT_8723D_1ANT_COEX_ALGO_A2DP:
			RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
				 "[BTCoex], Action algorithm = A2DP.\n");
			break;
		case BT_8723D_1ANT_COEX_ALGO_A2DP_PANHS:
			RT_TRACE(
				rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
				"[BTCoex], Action algorithm = A2DP+PAN(HS).\n");
			break;
		case BT_8723D_1ANT_COEX_ALGO_PANEDR:
			RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
				 "[BTCoex], Action algorithm = PAN(EDR).\n");
			break;
		case BT_8723D_1ANT_COEX_ALGO_PANHS:
			RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
				 "[BTCoex], Action algorithm = HS mode.\n");
			break;
		case BT_8723D_1ANT_COEX_ALGO_PANEDR_A2DP:
			RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
				 "[BTCoex], Action algorithm = PAN+A2DP.\n");
			break;
		case BT_8723D_1ANT_COEX_ALGO_PANEDR_HID:
			RT_TRACE(
				rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
				"[BTCoex], Action algorithm = PAN(EDR)+HID.\n");
			break;
		case BT_8723D_1ANT_COEX_ALGO_HID_A2DP_PANEDR:
			RT_TRACE(
				rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
				"[BTCoex], Action algorithm = HID+A2DP+PAN.\n");
			break;
		case BT_8723D_1ANT_COEX_ALGO_HID_A2DP:
			RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
				 "[BTCoex], Action algorithm = HID+A2DP.\n");
			break;
		default:
			RT_TRACE(
				rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
				"[BTCoex], Action algorithm = coexist All Off!!\n");
			break;
		}
		coex_dm->pre_algorithm = coex_dm->cur_algorithm;
	}
}

static void halbtc8723d1ant_run_coexist_mechanism(struct btc_coexist *btcoexist)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;
	struct btc_bt_link_info *bt_link_info = &btcoexist->bt_link_info;
	bool wifi_connected = false, bt_hs_on = false;
	bool increase_scan_dev_num = false;
	bool miracast_plus_bt = false;
	u32 wifi_link_status = 0;
	u32 num_of_wifi_link = 0, wifi_bw;
	u8 iot_peer = BTC_IOT_PEER_UNKNOWN;
	bool scan = false, link = false, roam = false, under_4way = false;

	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_SCAN, &scan);
	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_LINK, &link);
	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_ROAM, &roam);
	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_4_WAY_PROGRESS,
			   &under_4way);

	RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		 "[BTCoex], RunCoexistMechanism()===>\n");

	RT_TRACE(
		rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		"[BTCoex], under_lps = %d, force_lps_ctrl = %d, acl_busy = %d!!!\n",
		coex_sta->under_lps, coex_sta->force_lps_ctrl,
		coex_sta->acl_busy);

	if (btcoexist->manual_control) {
		RT_TRACE(
			rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			"[BTCoex], RunCoexistMechanism(), return for Manual CTRL <===\n");
		return;
	}

	if (btcoexist->stop_coex_dm) {
		RT_TRACE(
			rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			"[BTCoex], RunCoexistMechanism(), return for Stop Coex DM <===\n");
		return;
	}

	if (coex_sta->under_ips) {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], wifi is under IPS !!!\n");
		return;
	}

	if (!coex_sta->run_time_state) {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], return for run_time_state = false !!!\n");
		return;
	}

	if (coex_sta->freeze_coexrun_by_btinfo) {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], return for freeze_coexrun_by_btinfo\n");
		return;
	}

	if ((coex_sta->under_lps) && (!coex_sta->force_lps_ctrl) &&
	    (!coex_sta->acl_busy)) {
		RT_TRACE(
			rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			"[BTCoex], RunCoexistMechanism(), wifi is under LPS !!!\n");
		halbtc8723d1ant_action_wifi_native_lps(btcoexist);
		return;
	}

	if (coex_sta->bt_whck_test) {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], BT is under WHCK TEST!!!\n");
		halbtc8723d1ant_action_bt_whql_test(btcoexist);
		return;
	}

	if (coex_sta->bt_disabled) {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], BT is disabled !!!\n");
		halbtc8723d1ant_action_wifi_only(btcoexist);
		return;
	}

	if (coex_sta->c2h_bt_inquiry_page) {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], BT is under inquiry/page scan !!\n");
		halbtc8723d1ant_action_bt_inquiry(btcoexist);
		return;
	}

	if ((coex_sta->is_setup_link) && (coex_sta->bt_relink_downcount != 0)) {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], BT is re-link !!!\n");
		halbtc8723d1ant_action_bt_relink(btcoexist);
		return;
	}

	if ((coex_dm->bt_status == BT_8723D_1ANT_BT_STATUS_ACL_BUSY) ||
	    (coex_dm->bt_status == BT_8723D_1ANT_BT_STATUS_SCO_BUSY) ||
	    (coex_dm->bt_status == BT_8723D_1ANT_BT_STATUS_ACL_SCO_BUSY))
		increase_scan_dev_num = true;

	btcoexist->btc_set(btcoexist, BTC_SET_BL_INC_SCAN_DEV_NUM,
			   &increase_scan_dev_num);

	btcoexist->btc_get(btcoexist, BTC_GET_U4_WIFI_LINK_STATUS,
			   &wifi_link_status);

	num_of_wifi_link = wifi_link_status >> 16;

	if ((num_of_wifi_link >= 2) ||
	    (wifi_link_status & WIFI_P2P_GO_CONNECTED)) {
		RT_TRACE(
			rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			"############# [BTCoex],  Multi-Port num_of_wifi_link = %d, wifi_link_status = 0x%x\n",
			num_of_wifi_link, wifi_link_status);

		if (scan || link || roam || under_4way) {
			RT_TRACE(
				rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
				"[BTCoex], scan = %d, link = %d, roam = %d 4way = %d!!!\n",
				scan, link, roam, under_4way);

			if (bt_link_info->bt_link_exist)
				miracast_plus_bt = true;
			else
				miracast_plus_bt = false;

			btcoexist->btc_set(btcoexist,
					   BTC_SET_BL_MIRACAST_PLUS_BT,
					   &miracast_plus_bt);

			RT_TRACE(
				rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
				"[BTCoex], wifi is under linkscan process + Multi-Port !!\n");

			halbtc8723d1ant_action_wifi_linkscan_process(btcoexist);
		} else {
			halbtc8723d1ant_action_wifi_multi_port(btcoexist);
		}

		return;
	}

	btcoexist->btc_get(btcoexist, BTC_GET_U4_WIFI_BW, &wifi_bw);
	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_CONNECTED,
			   &wifi_connected);

	if ((bt_link_info->bt_link_exist) && (wifi_connected)) {
		btcoexist->btc_get(btcoexist, BTC_GET_U1_IOT_PEER, &iot_peer);

		if (iot_peer == BTC_IOT_PEER_CISCO) {
			if (wifi_bw == BTC_WIFI_BW_HT40)
				halbtc8723d1ant_limited_rx(btcoexist,
							   NORMAL_EXEC, false,
							   true, 0x10);
			else
				halbtc8723d1ant_limited_rx(btcoexist,
							   NORMAL_EXEC, false,
							   true, 0x8);
		}
	}

	halbtc8723d1ant_run_sw_coexist_mechanism(
		btcoexist); /* just print debug message */

	btcoexist->btc_get(btcoexist, BTC_GET_BL_HS_OPERATION, &bt_hs_on);

	if (bt_hs_on) {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "############# [BTCoex],  BT Is hs\n");
		halbtc8723d1ant_action_bt_hs(btcoexist);
		return;
	}

	if ((coex_dm->bt_status ==
	     BT_8723D_1ANT_BT_STATUS_NON_CONNECTED_IDLE) ||
	    (coex_dm->bt_status == BT_8723D_1ANT_BT_STATUS_CONNECTED_IDLE)) {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "############# [BTCoex],  BT Is idle\n");
		halbtc8723d1ant_action_bt_idle(btcoexist);
		return;
	}

	if (scan || link || roam || under_4way) {
		RT_TRACE(
			rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			"[BTCoex], scan = %d, link = %d, roam = %d 4way = %d!!!\n",
			scan, link, roam, under_4way);

		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], wifi is under linkscan process!!\n");

		halbtc8723d1ant_action_wifi_linkscan_process(btcoexist);
	} else if (wifi_connected) {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], wifi is under connected!!\n");

		halbtc8723d1ant_action_wifi_connected(btcoexist);
	} else {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], wifi is under not-connected!!\n");

		halbtc8723d1ant_action_wifi_not_connected(btcoexist);
	}
}

static void halbtc8723d1ant_init_coex_dm(struct btc_coexist *btcoexist)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;

	/* force to reset coex mechanism */
	halbtc8723d1ant_low_penalty_ra(btcoexist, NORMAL_EXEC, false);

	RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		 "[BTCoex], Coex Mechanism Init!!\n");

	coex_sta->pop_event_cnt = 0;
	coex_sta->cnt_remote_name_req = 0;
	coex_sta->cnt_reinit = 0;
	coex_sta->cnt_setup_link = 0;
	coex_sta->cnt_ign_wlan_act = 0;
	coex_sta->cnt_page = 0;
	coex_sta->cnt_role_switch = 0;

	halbtc8723d1ant_query_bt_info(btcoexist);
}

static void halbtc8723d1ant_init_hw_config(struct btc_coexist *btcoexist,
					   bool back_up, bool wifi_only)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;

#if BT_8723D_1ANT_COEX_DBG
	u32 u32tmp1 = 0, u32tmp2 = 0;
	u16 u16tmp1 = 0;
	u8 u8tmp0 = 0, u8tmp1 = 0;
#endif
	struct btc_board_info *board_info = &btcoexist->board_info;
	u8 i = 0;

	RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		 "[BTCoex], 1Ant Init HW Config!!\n");

#if BT_8723D_1ANT_COEX_DBG
	u32tmp1 = halbtc8723d1ant_ltecoex_indirect_read_reg(btcoexist, 0x38);
	u32tmp2 = halbtc8723d1ant_ltecoex_indirect_read_reg(btcoexist, 0x54);
	u16tmp1 = btcoexist->btc_read_2byte(btcoexist, 0x948);
	u8tmp1 = btcoexist->btc_read_1byte(btcoexist, 0x73);
	u8tmp0 = btcoexist->btc_read_1byte(btcoexist, 0x67);

	RT_TRACE(
		rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		"[BTCoex], ********** 0x67 = 0x%x, 0x948 = 0x%x, 0x73 = 0x%x(Before init_hw_config)\n",
		u8tmp0, u16tmp1, u8tmp1);

	RT_TRACE(
		rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		"[BTCoex], **********0x38= 0x%x, 0x54= 0x%x (Before init_hw_config)\n",
		u32tmp1, u32tmp2);
#endif

	coex_sta->bt_coex_supported_feature = 0;
	coex_sta->bt_coex_supported_version = 0;
	coex_sta->bt_ble_scan_type = 0;
	coex_sta->bt_ble_scan_para[0] = 0;
	coex_sta->bt_ble_scan_para[1] = 0;
	coex_sta->bt_ble_scan_para[2] = 0;
	coex_sta->bt_reg_vendor_ac = 0xffff;
	coex_sta->bt_reg_vendor_ae = 0xffff;
	coex_sta->isolation_btween_wb = BT_8723D_1ANT_DEFAULT_ISOLATION;
	coex_sta->gnt_error_cnt = 0;
	coex_sta->bt_relink_downcount = 0;
	coex_sta->wl_rx_rate = BTC_UNKNOWN;

	for (i = 0; i <= 9; i++)
		coex_sta->bt_afh_map[i] = 0;

	/* 0xf0[15:12] --> Chip Cut information */
	coex_sta->cut_version =
		(btcoexist->btc_read_1byte(btcoexist, 0xf1) & 0xf0) >> 4;

	if (coex_sta->cut_version >= 0x3)
		halbtc8723d1ant_post_state_to_bt(
			btcoexist, BT_8723D_1ANT_SCOREBOARD_DKTOPP2M, true);
	else
		halbtc8723d1ant_post_state_to_bt(
			btcoexist, BT_8723D_1ANT_SCOREBOARD_DKTOPP2M, false);

	btcoexist->btc_write_1byte_bitmask(btcoexist, 0x550, 0x8,
					   0x1); /* enable TBTT nterrupt */

	/* BT report packet sample rate	 */
	btcoexist->btc_write_1byte(btcoexist, 0x790, 0x5);

	/* Init 0x778 = 0x1 for 1-Ant */
	btcoexist->btc_write_1byte(btcoexist, 0x778, 0x1);

	/* Enable PTA (3-wire function form BT side) */
	btcoexist->btc_write_1byte_bitmask(btcoexist, 0x40, 0x20, 0x1);
	btcoexist->btc_write_1byte_bitmask(btcoexist, 0x41, 0x02, 0x1);

	/* Enable PTA (tx/rx signal form WiFi side) */
	btcoexist->btc_write_1byte_bitmask(btcoexist, 0x4c6, 0x10, 0x1);

	halbtc8723d1ant_enable_gnt_to_gpio(btcoexist, true);

	/* PTA parameter */
	halbtc8723d1ant_coex_table_with_type(btcoexist, FORCE_EXEC, 0);

	halbtc8723d1ant_ps_tdma(btcoexist, FORCE_EXEC, false, 8);

	psd_scan->ant_det_is_ant_det_available = true;

	/* Antenna config */
	if (coex_sta->is_rf_state_off) {
		halbtc8723d1ant_set_ant_path(btcoexist, BTC_ANT_PATH_AUTO,
					     FORCE_EXEC,
					     BT_8723D_1ANT_PHASE_WLAN_OFF);

		btcoexist->stop_coex_dm = true;

		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], **********  %s (RF Off)**********\n",
			 __func__);
	} else if (wifi_only) {
		coex_sta->concurrent_rx_mode_on = false;
		halbtc8723d1ant_set_ant_path(btcoexist, BTC_ANT_PATH_WIFI,
					     FORCE_EXEC,
					     BT_8723D_1ANT_PHASE_WLANONLY_INIT);

		btcoexist->stop_coex_dm = true;
	} else {
		/* Set BT polluted packet on for Tx rate adaptive not including
		 * Tx retry break by PTA, 0x45c[19] =1
		 */
		btcoexist->btc_write_1byte_bitmask(btcoexist, 0x45e, 0x8, 0x1);

		coex_sta->concurrent_rx_mode_on = true;
		btcoexist->btc_write_1byte_bitmask(btcoexist, 0x953, 0x2, 0x1);
		/* RF 0x1[0] = 0->Set GNT_WL_RF_Rx always = 1 for
		 * con-current Rx
		 */
		btcoexist->btc_set_rf_reg(btcoexist, BTC_RF_A, 0x1, 0x1, 0x0);
		halbtc8723d1ant_set_ant_path(btcoexist, BTC_ANT_PATH_AUTO,
					     FORCE_EXEC,
					     BT_8723D_1ANT_PHASE_COEX_INIT);

		btcoexist->stop_coex_dm = false;
	}

	if (board_info->btdm_ant_pos == BTC_ANTENNA_AT_MAIN_PORT) {
		RT_TRACE(
			rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			"[BTCoex], **********  Single Antenna, Antenna at Main Port: S1**********\n");
	} else {
		RT_TRACE(
			rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			"[BTCoex], **********  Single Antenna, Antenna at Aux Port: S0**********\n");
	}
}

/* ************************************************************
 * work around function start with wa_halbtc8723d1ant_
 * ************************************************************
 * ************************************************************
 * extern function start with ex_halbtc8723d1ant_
 * *************************************************************/
void ex_btc8723d1ant_power_on_setting(struct btc_coexist *btcoexist)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;

	struct btc_board_info *board_info = &btcoexist->board_info;
	u8 u8tmp = 0x0;
	u16 u16tmp = 0x0;
	u32 value = 0;

	RT_TRACE(
		rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		"xxxxxxxxxxxxxxxx Execute 8723d 1-Ant PowerOn Setting xxxxxxxxxxxxxxxx!!\n");

	RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		 "Ant Det Finish = %s, Ant Det Number  = %d\n",
		 (board_info->btdm_ant_det_finish ? "Yes" : "No"),
		 board_info->btdm_ant_num_by_ant_det);

	btcoexist->stop_coex_dm = true;
	coex_sta->is_rf_state_off = false;
	psd_scan->ant_det_is_ant_det_available = false;

	/* enable BB, REG_SYS_FUNC_EN such that we can write BB Register
	 * correctly.
	 */
	u16tmp = btcoexist->btc_read_2byte(btcoexist, 0x2);
	btcoexist->btc_write_2byte(btcoexist, 0x2, u16tmp | BIT(0) | BIT(1));

	/* Local setting bit define */
	/*	BIT0: "0" for no antenna inverse; "1" for antenna inverse  */
	/*	BIT1: "0" for internal switch; "1" for external switch */
	/*	BIT2: "0" for one antenna; "1" for two antenna */
	/* NOTE: here default all internal switch and 1-antenna ==> */
	/*       BIT1=0 and BIT2=0 */

	/* Set Antenna Path to BT side */
	/* Check efuse 0xc3[6] for Single Antenna Path */
	if (board_info->single_ant_path == 0) {
		/* set to S1 */
		board_info->btdm_ant_pos = BTC_ANTENNA_AT_MAIN_PORT;
		u8tmp = 0;
		value = 1;
	} else if (board_info->single_ant_path == 1) {
		/* set to S0 */
		board_info->btdm_ant_pos = BTC_ANTENNA_AT_AUX_PORT;
		u8tmp = 1;
		value = 0;
	}

	RT_TRACE(
		rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		"[BTCoex], ********** (Power On) single_ant_path  = %d, btdm_ant_pos = %d **********\n",
		board_info->single_ant_path, board_info->btdm_ant_pos);

	/* Set Antenna Path to BT side */
	halbtc8723d1ant_set_ant_path(btcoexist, BTC_ANT_PATH_AUTO, FORCE_EXEC,
				     BT_8723D_1ANT_PHASE_COEX_POWERON);

	/* Write Single Antenna Position to Registry to tell BT for 8723d.
	 * This line can be removed since BT EFuse also add
	 * "single antenna position" in EFuse for 8723d
	 */
	btcoexist->btc_set(btcoexist, BTC_SET_ACT_ANTPOSREGRISTRY_CTRL, &value);

	/* Save"single antenna position" info in Local register setting for FW
	 * reading, because FW may not ready at  power on
	 */
	if (btcoexist->chip_interface == BTC_INTF_PCI)
		btcoexist->btc_write_local_reg_1byte(btcoexist, 0x3e0, u8tmp);
	else if (btcoexist->chip_interface == BTC_INTF_USB)
		btcoexist->btc_write_local_reg_1byte(btcoexist, 0xfe08, u8tmp);
	else if (btcoexist->chip_interface == BTC_INTF_SDIO)
		btcoexist->btc_write_local_reg_1byte(btcoexist, 0x60, u8tmp);

	/* enable GNT_WL/GNT_BT debug signal to GPIO14/15 */
	halbtc8723d1ant_enable_gnt_to_gpio(btcoexist, true);

	RT_TRACE(
		rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		"[BTCoex], **********  LTE coex Reg 0x38 (Power-On) = 0x%x**********\n",
		halbtc8723d1ant_ltecoex_indirect_read_reg(btcoexist, 0x38));

	RT_TRACE(
		rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		"[BTCoex], **********  MAC Reg 0x70/ BB Reg 0x948 (Power-On) = 0x%x / 0x%x**********\n",
		btcoexist->btc_read_4byte(btcoexist, 0x70),
		btcoexist->btc_read_2byte(btcoexist, 0x948));
}

void ex_btc8723d1ant_pre_load_firmware(struct btc_coexist *btcoexist) {}

void ex_btc8723d1ant_init_hw_config(struct btc_coexist *btcoexist,
				    bool wifi_only)
{
	halbtc8723d1ant_init_hw_config(btcoexist, true, wifi_only);
}

void ex_btc8723d1ant_init_coex_dm(struct btc_coexist *btcoexist)
{
	halbtc8723d1ant_init_coex_dm(btcoexist);
}

void ex_btc8723d1ant_display_coex_info(struct btc_coexist *btcoexist,
				       struct seq_file *m)
{
	struct btc_board_info *board_info = &btcoexist->board_info;
	struct btc_bt_link_info *bt_link_info = &btcoexist->bt_link_info;
	u8 u8tmp[4], i, ps_tdma_case = 0;
	u16 u16tmp[4];
	u32 u32tmp[4];
	u32 fa_ofdm, fa_cck, cca_ofdm, cca_cck;
	u32 fw_ver = 0, bt_patch_ver = 0, bt_coex_ver = 0;
	static u8 pop_report_in_10s, cnt;
	u32 phyver = 0;
	bool lte_coex_on = false;

	seq_puts(m, "\r\n ============[BT Coexist info]============");

	if (btcoexist->manual_control) {
		seq_puts(m,
			 "\r\n ============[Under Manual Control]============");
		seq_puts(m, "\r\n ==========================================");
	}
	if (btcoexist->stop_coex_dm) {
		seq_puts(m, "\r\n ============[Coex is STOPPED]============");
		seq_puts(m, "\r\n ==========================================");
	}

	if (!coex_sta->bt_disabled) {
		if (coex_sta->bt_coex_supported_feature == 0)
			btcoexist->btc_get(
				btcoexist, BTC_GET_U4_SUPPORTED_FEATURE,
				&coex_sta->bt_coex_supported_feature);

		if ((coex_sta->bt_coex_supported_version == 0) ||
		    (coex_sta->bt_coex_supported_version == 0xffff))
			btcoexist->btc_get(
				btcoexist, BTC_GET_U4_SUPPORTED_VERSION,
				&coex_sta->bt_coex_supported_version);

		if (coex_sta->bt_reg_vendor_ac == 0xffff)
			coex_sta->bt_reg_vendor_ac = (u16)(
				btcoexist->btc_get_bt_reg(btcoexist, 3, 0xac) &
				0xffff);

		if (coex_sta->bt_reg_vendor_ae == 0xffff)
			coex_sta->bt_reg_vendor_ae = (u16)(
				btcoexist->btc_get_bt_reg(btcoexist, 3, 0xae) &
				0xffff);

		btcoexist->btc_get(btcoexist, BTC_GET_U4_BT_PATCH_VER,
				   &bt_patch_ver);
		btcoexist->bt_info.bt_get_fw_ver = bt_patch_ver;

		if (coex_sta->num_of_profile > 0) {
			cnt++;

			if (cnt >= 3) {
				btcoexist->btc_get_bt_afh_map_from_bt(
					btcoexist, 0, &coex_sta->bt_afh_map[0]);
				cnt = 0;
			}
		}
	}

	if (psd_scan->ant_det_try_count == 0) {
		seq_printf(m, "\r\n %-35s = %d/ %d/ %s",
			   "Ant PG Num/ Mech/ Pos", board_info->pg_ant_num,
			   board_info->btdm_ant_num,
			   (board_info->btdm_ant_pos == 1 ? "S1" : "S0"));
	} else {
		seq_printf(m, "\r\n %-35s = %d/ %d/ %s  (%d/%d/%d)",
			   "Ant PG Num/ Mech(Ant_Det)/ Pos",
			   board_info->pg_ant_num,
			   board_info->btdm_ant_num_by_ant_det,
			   (board_info->btdm_ant_pos == 1 ? "S1" : "S0"),
			   psd_scan->ant_det_try_count,
			   psd_scan->ant_det_fail_count,
			   psd_scan->ant_det_result);

		if (board_info->btdm_ant_det_finish) {
			if (psd_scan->ant_det_result != 12)
				seq_printf(m, "\r\n %-35s = %s",
					   "Ant Det PSD Value",
					   psd_scan->ant_det_peak_val);
			else
				seq_printf(m, "\r\n %-35s = %d",
					   "Ant Det PSD Value",
					   psd_scan->ant_det_psd_scan_peak_val /
						   100);
		}
	}

	bt_patch_ver = btcoexist->bt_info.bt_get_fw_ver;
	btcoexist->btc_get(btcoexist, BTC_GET_U4_WIFI_FW_VER, &fw_ver);
	phyver = btcoexist->btc_get_bt_phydm_version(btcoexist);

	bt_coex_ver = ((coex_sta->bt_coex_supported_version & 0xff00) >> 8);

	seq_printf(
		m, "\r\n %-35s = %d_%02x/ 0x%02x/ 0x%02x (%s)",
		"CoexVer WL/  BT_Desired/ BT_Report",
		glcoex_ver_date_8723d_1ant, glcoex_ver_8723d_1ant,
		glcoex_ver_btdesired_8723d_1ant, bt_coex_ver,
		(bt_coex_ver == 0xff ?
		 "Unknown" :
		 (coex_sta->bt_disabled ?
			  "BT-disable" :
			  (bt_coex_ver >= glcoex_ver_btdesired_8723d_1ant ?
				   "Match" :
				   "Mis-Match"))));

	seq_printf(m, "\r\n %-35s = 0x%x/ 0x%x/ v%d/ %c", "W_FW/ B_FW/ Phy/ Kt",
		   fw_ver, bt_patch_ver, phyver, coex_sta->cut_version + 65);

	seq_printf(m, "\r\n %-35s = %02x %02x %02x ",
		   "Wifi channel informed to BT", coex_dm->wifi_chnl_info[0],
		   coex_dm->wifi_chnl_info[1], coex_dm->wifi_chnl_info[2]);

	/* wifi status */
	seq_printf(m, "\r\n %-35s", "============[Wifi Status]============");
	btcoexist->btc_disp_dbg_msg(btcoexist, BTC_DBG_DISP_WIFI_STATUS, m);

	seq_printf(m, "\r\n %-35s", "============[BT Status]============");

	pop_report_in_10s++;
	seq_printf(
		m, "\r\n %-35s = %s/ %ddBm/ %d/ %d",
		"BT status/ rssi/ retryCnt/ popCnt",
		((coex_sta->bt_disabled) ?
		 ("disabled") :
		 ((coex_sta->c2h_bt_inquiry_page) ?
			  ("inquiry-page") :
			  ((coex_dm->bt_status ==
			    BT_8723D_1ANT_BT_STATUS_NON_CONNECTED_IDLE) ?
				   "non-connected-idle" :
				   ((coex_dm->bt_status ==
				     BT_8723D_1ANT_BT_STATUS_CONNECTED_IDLE) ?
					    "connected-idle" :
					    "busy")))),
		coex_sta->bt_rssi - 100, coex_sta->bt_retry_cnt,
		coex_sta->pop_event_cnt);

	if (pop_report_in_10s >= 5) {
		coex_sta->pop_event_cnt = 0;
		pop_report_in_10s = 0;
	}

	if (coex_sta->num_of_profile != 0)
		seq_printf(m, "\r\n %-35s = %s%s%s%s%s (multilink = %d)",
			   "Profiles", ((bt_link_info->a2dp_exist) ?
						((coex_sta->is_bt_a2dp_sink) ?
							 "A2DP sink," :
							 "A2DP,") :
						""),
			   ((bt_link_info->sco_exist) ? "HFP," : ""),
			   ((bt_link_info->hid_exist) ?
				    ((coex_sta->is_hid_rcu) ?
					     "HID(RCU)" :
					     ((coex_sta->hid_busy_num >= 2) ?
						      "HID(4/18)," :
						      "HID(2/18),")) :
				    ""),
			   ((bt_link_info->pan_exist) ?
				    ((coex_sta->is_bt_opp_exist) ? "OPP," :
								   "PAN,") :
				    ""),
			   ((coex_sta->voice_over_HOGP) ? "Voice" : ""),
			   coex_sta->is_bt_multi_link);
	else
		seq_printf(m, "\r\n %-35s = None", "Profiles");

	if (bt_link_info->a2dp_exist) {
		seq_printf(m, "\r\n %-35s = %s/ %d/ %s",
			   "A2DP Rate/Bitpool/Auto_Slot",
			   ((coex_sta->is_A2DP_3M) ? "3M" : "No_3M"),
			   coex_sta->a2dp_bit_pool,
			   ((coex_sta->is_autoslot) ? "On" : "Off"));

		seq_printf(m, "\r\n %-35s = 0x%x/ 0x%x/ %d/ %d",
			   "V_ID/D_name/FBSlot_Legacy/FBSlot_Le",
			   coex_sta->bt_a2dp_vendor_id,
			   coex_sta->bt_a2dp_device_name,
			   coex_sta->legacy_forbidden_slot,
			   coex_sta->le_forbidden_slot);
	}

	if (bt_link_info->hid_exist) {
		seq_printf(m, "\r\n %-35s = %d", "HID PairNum/Forbid_Slot",
			   coex_sta->hid_pair_cnt);
	}

	seq_printf(m, "\r\n %-35s = %s/ %d/ %s/ 0x%x",
		   "Role/RoleSwCnt/IgnWlact/Feature",
		   ((bt_link_info->slave_role) ? "Slave" : "Master"),
		   coex_sta->cnt_role_switch,
		   ((coex_dm->cur_ignore_wlan_act) ? "Yes" : "No"),
		   coex_sta->bt_coex_supported_feature);

	if ((coex_sta->bt_ble_scan_type & 0x7) != 0x0) {
		seq_printf(m, "\r\n %-35s = 0x%x/ 0x%x/ 0x%x/ 0x%x",
			   "BLEScan Type/TV/Init/Ble",
			   coex_sta->bt_ble_scan_type,
			   (coex_sta->bt_ble_scan_type & 0x1 ?
				    coex_sta->bt_ble_scan_para[0] :
				    0x0),
			   (coex_sta->bt_ble_scan_type & 0x2 ?
				    coex_sta->bt_ble_scan_para[1] :
				    0x0),
			   (coex_sta->bt_ble_scan_type & 0x4 ?
				    coex_sta->bt_ble_scan_para[2] :
				    0x0));
	}

	seq_printf(m, "\r\n %-35s = %d/ %d/ %d/ %d/ %d",
		   "ReInit/ReLink/IgnWlact/Page/NameReq", coex_sta->cnt_reinit,
		   coex_sta->cnt_setup_link, coex_sta->cnt_ign_wlan_act,
		   coex_sta->cnt_page, coex_sta->cnt_remote_name_req);

	halbtc8723d1ant_read_score_board(btcoexist, &u16tmp[0]);

	if ((coex_sta->bt_reg_vendor_ae == 0xffff) ||
	    (coex_sta->bt_reg_vendor_ac == 0xffff))
		seq_printf(m, "\r\n %-35s = x/ x/ 0x%04x",
			   "0xae[4]/0xac[1:0]/Scoreboard(B->W)", u16tmp[0]);
	else
		seq_printf(m, "\r\n %-35s = 0x%x/ 0x%x/ 0x%04x",
			   "0xae[4]/0xac[1:0]/Scoreboard(B->W)",
			   (u16)((coex_sta->bt_reg_vendor_ae & BIT(4)) >> 4),
			   coex_sta->bt_reg_vendor_ac & 0x3, u16tmp[0]);

	if (coex_sta->num_of_profile > 0) {
		seq_printf(
			m,
			"\r\n %-35s = %02x%02x%02x%02x %02x%02x%02x%02x %02x %02x",
			"AFH MAP", coex_sta->bt_afh_map[0],
			coex_sta->bt_afh_map[1], coex_sta->bt_afh_map[2],
			coex_sta->bt_afh_map[3], coex_sta->bt_afh_map[4],
			coex_sta->bt_afh_map[5], coex_sta->bt_afh_map[6],
			coex_sta->bt_afh_map[7], coex_sta->bt_afh_map[8],
			coex_sta->bt_afh_map[9]);
	}

	for (i = 0; i < BT_INFO_SRC_8723D_1ANT_MAX; i++) {
		if (coex_sta->bt_info_c2h_cnt[i]) {
			seq_printf(
				m,
				"\r\n %-35s = %02x %02x %02x %02x %02x %02x %02x (%d)",
				glbt_info_src_8723d_1ant[i],
				coex_sta->bt_info_c2h[i][0],
				coex_sta->bt_info_c2h[i][1],
				coex_sta->bt_info_c2h[i][2],
				coex_sta->bt_info_c2h[i][3],
				coex_sta->bt_info_c2h[i][4],
				coex_sta->bt_info_c2h[i][5],
				coex_sta->bt_info_c2h[i][6],
				coex_sta->bt_info_c2h_cnt[i]);
		}
	}

	if (btcoexist->manual_control)
		seq_printf(
			m, "\r\n %-35s",
			"============[mechanisms] (before Manual)============");
	else
		seq_printf(m, "\r\n %-35s",
			   "============[Mechanisms]============");

	ps_tdma_case = coex_dm->cur_ps_tdma;
	seq_printf(m, "\r\n %-35s = %02x %02x %02x %02x %02x (case-%d, %s)",
		   "TDMA", coex_dm->ps_tdma_para[0], coex_dm->ps_tdma_para[1],
		   coex_dm->ps_tdma_para[2], coex_dm->ps_tdma_para[3],
		   coex_dm->ps_tdma_para[4], ps_tdma_case,
		   (coex_dm->cur_ps_tdma_on ? "TDMA On" : "TDMA Off"));

	u32tmp[0] = btcoexist->btc_read_4byte(btcoexist, 0x6c0);
	u32tmp[1] = btcoexist->btc_read_4byte(btcoexist, 0x6c4);
	u32tmp[2] = btcoexist->btc_read_4byte(btcoexist, 0x6c8);
	seq_printf(m, "\r\n %-35s = %d/ 0x%x/ 0x%x/ 0x%x",
		   "Table/0x6c0/0x6c4/0x6c8", coex_sta->coex_table_type,
		   u32tmp[0], u32tmp[1], u32tmp[2]);

	u8tmp[0] = btcoexist->btc_read_1byte(btcoexist, 0x778);
	u32tmp[0] = btcoexist->btc_read_4byte(btcoexist, 0x6cc);
	seq_printf(m, "\r\n %-35s = 0x%x/ 0x%x/ 0x%04x",
		   "0x778/0x6cc/Scoreboard(W->B)", u8tmp[0], u32tmp[0],
		   coex_sta->score_board_WB);

	seq_printf(m, "\r\n %-35s = %s/ %s", "AntDiv/ ForceLPS",
		   ((board_info->ant_div_cfg) ? "On" : "Off"),
		   ((coex_sta->force_lps_ctrl) ? "On" : "Off"));

	seq_printf(m, "\r\n %-35s = %d/ %d", "BT_Empty/BT_Late",
		   coex_sta->wl_fw_dbg_info[4], coex_sta->wl_fw_dbg_info[5]);

	u32tmp[0] = halbtc8723d1ant_ltecoex_indirect_read_reg(btcoexist, 0x38);
	lte_coex_on = ((u32tmp[0] & BIT(7)) >> 7) ? true : false;

	if (lte_coex_on) {
		u32tmp[0] = halbtc8723d1ant_ltecoex_indirect_read_reg(btcoexist,
								      0xa0);
		u32tmp[1] = halbtc8723d1ant_ltecoex_indirect_read_reg(btcoexist,
								      0xa4);

		seq_printf(m, "\r\n %-35s = 0x%x/ 0x%x",
			   "LTE Coex  Table W_L/B_L", u32tmp[0] & 0xffff,
			   u32tmp[1] & 0xffff);

		u32tmp[0] = halbtc8723d1ant_ltecoex_indirect_read_reg(btcoexist,
								      0xa8);
		u32tmp[1] = halbtc8723d1ant_ltecoex_indirect_read_reg(btcoexist,
								      0xac);
		u32tmp[2] = halbtc8723d1ant_ltecoex_indirect_read_reg(btcoexist,
								      0xb0);
		u32tmp[3] = halbtc8723d1ant_ltecoex_indirect_read_reg(btcoexist,
								      0xb4);

		seq_printf(m, "\r\n %-35s = 0x%x/ 0x%x/ 0x%x/ 0x%x",
			   "LTE Break Table W_L/B_L/L_W/L_B",
			   u32tmp[0] & 0xffff, u32tmp[1] & 0xffff,
			   u32tmp[2] & 0xffff, u32tmp[3] & 0xffff);
	}

	/* Hw setting		 */
	seq_printf(m, "\r\n %-35s", "============[Hw setting]============");

	u32tmp[0] = halbtc8723d1ant_ltecoex_indirect_read_reg(btcoexist, 0x38);
	u32tmp[1] = halbtc8723d1ant_ltecoex_indirect_read_reg(btcoexist, 0x54);
	u8tmp[0] = btcoexist->btc_read_1byte(btcoexist, 0x73);

	seq_printf(m, "\r\n %-35s = %s/ %s", "LTE Coex/Path Owner",
		   ((lte_coex_on) ? "On" : "Off"),
		   ((u8tmp[0] & BIT(2)) ? "WL" : "BT"));

	if (lte_coex_on) {
		seq_printf(m, "\r\n %-35s = %d/ %d/ %d/ %d",
			   "LTE 3Wire/OPMode/UART/UARTMode",
			   (int)((u32tmp[0] & BIT(6)) >> 6),
			   (int)((u32tmp[0] & (BIT(5) | BIT(4))) >> 4),
			   (int)((u32tmp[0] & BIT(3)) >> 3),
			   (int)(u32tmp[0] & (BIT(2) | BIT(1) | BIT(0))));

		seq_printf(m, "\r\n %-35s = %d/ %d", "LTE_Busy/UART_Busy",
			   (int)((u32tmp[1] & BIT(1)) >> 1),
			   (int)(u32tmp[1] & BIT(0)));
	}

	seq_printf(m, "\r\n %-35s = %s (BB:%s)/ %s (BB:%s)/ %s (gnt_err = %d)",
		   "GNT_WL_Ctrl/GNT_BT_Ctrl/Dbg",
		   ((u32tmp[0] & BIT(12)) ? "SW" : "HW"),
		   ((u32tmp[0] & BIT(8)) ? "SW" : "HW"),
		   ((u32tmp[0] & BIT(14)) ? "SW" : "HW"),
		   ((u32tmp[0] & BIT(10)) ? "SW" : "HW"),
		   ((u8tmp[0] & BIT(3)) ? "On" : "Off"),
		   coex_sta->gnt_error_cnt);

	seq_printf(m, "\r\n %-35s = %d/ %d", "GNT_WL/GNT_BT",
		   (int)((u32tmp[1] & BIT(2)) >> 2),
		   (int)((u32tmp[1] & BIT(3)) >> 3));

	u16tmp[0] = btcoexist->btc_read_2byte(btcoexist, 0x948);
	u8tmp[0] = btcoexist->btc_read_1byte(btcoexist, 0x67);

	seq_printf(m, "\r\n %-35s = 0x%x/ 0x%x", "0x948/0x67[7]", u16tmp[0],
		   (int)((u8tmp[0] & BIT(7)) >> 7));

	u8tmp[0] = btcoexist->btc_read_1byte(btcoexist, 0x964);
	u8tmp[1] = btcoexist->btc_read_1byte(btcoexist, 0x864);
	u8tmp[2] = btcoexist->btc_read_1byte(btcoexist, 0xab7);
	u8tmp[3] = btcoexist->btc_read_1byte(btcoexist, 0xa01);

	seq_printf(m, "\r\n %-35s = 0x%x/ 0x%x/ 0x%x/ 0x%x",
		   "0x964[1]/0x864[0]/0xab7[5]/0xa01[7]",
		   (int)((u8tmp[0] & BIT(1)) >> 1), (int)((u8tmp[1] & BIT(0))),
		   (int)((u8tmp[2] & BIT(3)) >> 3),
		   (int)((u8tmp[3] & BIT(7)) >> 7));

	u8tmp[0] = btcoexist->btc_read_1byte(btcoexist, 0x4c6);
	u8tmp[1] = btcoexist->btc_read_1byte(btcoexist, 0x40);
	u8tmp[2] = btcoexist->btc_read_1byte(btcoexist, 0x45e);

	seq_printf(m, "\r\n %-35s = 0x%x/ 0x%x/ 0x%x",
		   "0x4c6[4]/0x40[5]/0x45e[3](TxRetry)",
		   (int)((u8tmp[0] & BIT(4)) >> 4),
		   (int)((u8tmp[1] & BIT(5)) >> 5),
		   (int)((u8tmp[2] & BIT(3)) >> 3));

	u32tmp[0] = btcoexist->btc_read_4byte(btcoexist, 0x550);
	u8tmp[0] = btcoexist->btc_read_1byte(btcoexist, 0x522);
	u8tmp[1] = btcoexist->btc_read_1byte(btcoexist, 0x953);
	seq_printf(m, "\r\n %-35s = 0x%x/ 0x%x/ %s", "0x550/0x522/4-RxAGC",
		   u32tmp[0], u8tmp[0], (u8tmp[1] & 0x2) ? "On" : "Off");

	fa_ofdm = btcoexist->btc_phydm_query_phy_counter(btcoexist,
							 "PHYDM_INFO_FA_OFDM");
	fa_cck = btcoexist->btc_phydm_query_phy_counter(btcoexist,
							"PHYDM_INFO_FA_CCK");
	cca_ofdm = btcoexist->btc_phydm_query_phy_counter(
		btcoexist, "PHYDM_INFO_CCA_OFDM");
	cca_cck = btcoexist->btc_phydm_query_phy_counter(btcoexist,
							 "PHYDM_INFO_CCA_CCK");

	seq_printf(m, "\r\n %-35s = 0x%x/ 0x%x/ 0x%x/ 0x%x",
		   "CCK-CCA/CCK-FA/OFDM-CCA/OFDM-FA", cca_cck, fa_cck, cca_ofdm,
		   fa_ofdm);

	seq_printf(m, "\r\n %-35s = %d/ %d/ %d/ %d (Rx_rate Data/RTS= %d/%d)",
		   "CRC_OK CCK/11g/11n/11ac", coex_sta->crc_ok_cck,
		   coex_sta->crc_ok_11g, coex_sta->crc_ok_11n,
		   coex_sta->crc_ok_11n_vht, coex_sta->wl_rx_rate,
		   coex_sta->wl_rts_rx_rate);

	seq_printf(m, "\r\n %-35s = %d/ %d/ %d/ %d",
		   "CRC_Err CCK/11g/11n/11n-agg", coex_sta->crc_err_cck,
		   coex_sta->crc_err_11g, coex_sta->crc_err_11n,
		   coex_sta->crc_err_11n_vht);

	seq_printf(m, "\r\n %-35s = %s/ %s/ %s/ %d",
		   "WlHiPri/ Locking/ Locked/ Noisy",
		   (coex_sta->wifi_is_high_pri_task ? "Yes" : "No"),
		   (coex_sta->cck_lock ? "Yes" : "No"),
		   (coex_sta->cck_lock_ever ? "Yes" : "No"),
		   coex_sta->wl_noisy_level);

	seq_printf(m, "\r\n %-35s = %d/ %d %s", "0x770(Hi-pri rx/tx)",
		   coex_sta->high_priority_rx, coex_sta->high_priority_tx,
		   coex_sta->is_hi_pri_rx_overhead ? "(scan overhead!!)" : "");

	seq_printf(m, "\r\n %-35s = %d/ %d %s", "0x774(Lo-pri rx/tx)",
		   coex_sta->low_priority_rx, coex_sta->low_priority_tx,
		   (bt_link_info->slave_role ?
			    "(Slave!!)" :
			    (coex_sta->is_tdma_btautoslot_hang ?
				     "(auto-slot hang!!)" :
				     "")));

	btcoexist->btc_disp_dbg_msg(btcoexist, BTC_DBG_DISP_COEX_STATISTICS, m);
}

void ex_btc8723d1ant_ips_notify(struct btc_coexist *btcoexist, u8 type)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;

	if (btcoexist->manual_control || btcoexist->stop_coex_dm)
		return;

	if (type == BTC_IPS_ENTER) {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], IPS ENTER notify\n");
		coex_sta->under_ips = true;

		/* Write WL "Active" in Score-board for LPS off */
		halbtc8723d1ant_post_state_to_bt(
			btcoexist, BT_8723D_1ANT_SCOREBOARD_ACTIVE |
					   BT_8723D_1ANT_SCOREBOARD_ONOFF |
					   BT_8723D_1ANT_SCOREBOARD_SCAN |
					   BT_8723D_1ANT_SCOREBOARD_UNDERTEST,
			false);

		halbtc8723d1ant_set_ant_path(btcoexist, BTC_ANT_PATH_AUTO,
					     FORCE_EXEC,
					     BT_8723D_1ANT_PHASE_WLAN_OFF);

		halbtc8723d1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 0);

		halbtc8723d1ant_ps_tdma(btcoexist, NORMAL_EXEC, false, 0);

	} else if (type == BTC_IPS_LEAVE) {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], IPS LEAVE notify\n");

		halbtc8723d1ant_init_hw_config(btcoexist, false, false);
		halbtc8723d1ant_init_coex_dm(btcoexist);
		;

		coex_sta->under_ips = false;
	}
}

void ex_btc8723d1ant_lps_notify(struct btc_coexist *btcoexist, u8 type)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;
	static bool pre_force_lps_on;

	if (btcoexist->manual_control || btcoexist->stop_coex_dm)
		return;

	if (type == BTC_LPS_ENABLE) {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], LPS ENABLE notify\n");
		coex_sta->under_lps = true;

		if (coex_sta->force_lps_ctrl) { /* LPS No-32K */
			/* Write WL "Active" in Score-board for PS-TDMA */
			pre_force_lps_on = true;
			halbtc8723d1ant_post_state_to_bt(
				btcoexist, BT_8723D_1ANT_SCOREBOARD_ACTIVE,
				true);

		} else {
			/* LPS-32K, need check if this h2c 0x71 can work??
			 * (2015/08/28)
			 */
			/* Write WL "Non-Active" in Score-board for Native-PS */
			pre_force_lps_on = false;
			halbtc8723d1ant_post_state_to_bt(
				btcoexist, BT_8723D_1ANT_SCOREBOARD_ACTIVE,
				false);

			halbtc8723d1ant_action_wifi_native_lps(btcoexist);
		}
	} else if (type == BTC_LPS_DISABLE) {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], LPS DISABLE notify\n");
		coex_sta->under_lps = false;

		/* Write WL "Active" in Score-board for LPS off */
		halbtc8723d1ant_post_state_to_bt(
			btcoexist, BT_8723D_1ANT_SCOREBOARD_ACTIVE, true);

		if ((!pre_force_lps_on) && (!coex_sta->force_lps_ctrl))
			halbtc8723d1ant_query_bt_info(btcoexist);
	}
}

void ex_btc8723d1ant_scan_notify(struct btc_coexist *btcoexist, u8 type)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;

	bool wifi_connected = false;

	if (btcoexist->manual_control || btcoexist->stop_coex_dm)
		return;

	coex_sta->freeze_coexrun_by_btinfo = false;

	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_CONNECTED,
			   &wifi_connected);

	if (type == BTC_SCAN_START) {
		if (!wifi_connected)
			coex_sta->wifi_is_high_pri_task = true;

		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], SCAN START notify\n");

		halbtc8723d1ant_post_state_to_bt(
			btcoexist, BT_8723D_1ANT_SCOREBOARD_ACTIVE |
					   BT_8723D_1ANT_SCOREBOARD_SCAN |
					   BT_8723D_1ANT_SCOREBOARD_ONOFF,
			true);

		halbtc8723d1ant_query_bt_info(btcoexist);

		/* Force antenna setup for no scan result issue */
		halbtc8723d1ant_set_ant_path(btcoexist, BTC_ANT_PATH_AUTO,
					     FORCE_EXEC,
					     BT_8723D_1ANT_PHASE_2G_RUNTIME);

		halbtc8723d1ant_run_coexist_mechanism(btcoexist);

	} else {
		coex_sta->wifi_is_high_pri_task = false;

		btcoexist->btc_get(btcoexist, BTC_GET_U1_AP_NUM,
				   &coex_sta->scan_ap_num);

		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], SCAN FINISH notify  (Scan-AP = %d)\n",
			 coex_sta->scan_ap_num);

		halbtc8723d1ant_run_coexist_mechanism(btcoexist);
	}

	RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		 "[BTCoex], SCAN Notify() end\n");
}

void ex_btc8723d1ant_connect_notify(struct btc_coexist *btcoexist, u8 type)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;

	bool wifi_connected = false;

	if (btcoexist->manual_control || btcoexist->stop_coex_dm)
		return;

	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_CONNECTED,
			   &wifi_connected);

	if (type == BTC_ASSOCIATE_START) {
		coex_sta->wifi_is_high_pri_task = true;

		halbtc8723d1ant_post_state_to_bt(
			btcoexist, BT_8723D_1ANT_SCOREBOARD_ACTIVE |
					   BT_8723D_1ANT_SCOREBOARD_SCAN |
					   BT_8723D_1ANT_SCOREBOARD_ONOFF,
			true);

		/* Force antenna setup for no scan result issue */
		halbtc8723d1ant_set_ant_path(btcoexist, BTC_ANT_PATH_AUTO,
					     FORCE_EXEC,
					     BT_8723D_1ANT_PHASE_2G_RUNTIME);

		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], CONNECT START notify\n");

		coex_dm->arp_cnt = 0;
		coex_sta->connect_ap_period_cnt = 2;

		halbtc8723d1ant_run_coexist_mechanism(btcoexist);

		/* To keep TDMA case during connect process,
		 * to avoid changed by Btinfo and runcoexmechanism
		 */
		coex_sta->freeze_coexrun_by_btinfo = true;
	} else {
		coex_sta->wifi_is_high_pri_task = false;
		coex_sta->freeze_coexrun_by_btinfo = false;

		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], CONNECT FINISH notify\n");

		halbtc8723d1ant_run_coexist_mechanism(btcoexist);
	}
}

void ex_btc8723d1ant_media_status_notify(struct btc_coexist *btcoexist, u8 type)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;

	bool wifi_under_b_mode = false;

	if (btcoexist->manual_control || btcoexist->stop_coex_dm)
		return;

	if (type == BTC_MEDIA_CONNECT) {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], MEDIA connect notify\n");

		halbtc8723d1ant_post_state_to_bt(
			btcoexist, BT_8723D_1ANT_SCOREBOARD_ACTIVE |
					   BT_8723D_1ANT_SCOREBOARD_ONOFF,
			true);

		/* Force antenna setup for no scan result issue */
		halbtc8723d1ant_set_ant_path(btcoexist, BTC_ANT_PATH_AUTO,
					     FORCE_EXEC,
					     BT_8723D_1ANT_PHASE_2G_RUNTIME);

		btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_UNDER_B_MODE,
				   &wifi_under_b_mode);

		/* Set CCK Tx/Rx high Pri except 11b mode */
		if (wifi_under_b_mode) {
			btcoexist->btc_write_1byte(btcoexist, 0x6cd,
						   0x00); /* CCK Tx */
			btcoexist->btc_write_1byte(btcoexist, 0x6cf,
						   0x00); /* CCK Rx */
		} else {
			btcoexist->btc_write_1byte(btcoexist, 0x6cd,
						   0x00); /* CCK Tx */
			btcoexist->btc_write_1byte(btcoexist, 0x6cf,
						   0x10); /* CCK Rx */
		}

		coex_dm->backup_arfr_cnt1 =
			btcoexist->btc_read_4byte(btcoexist, 0x430);
		coex_dm->backup_arfr_cnt2 =
			btcoexist->btc_read_4byte(btcoexist, 0x434);
		coex_dm->backup_retry_limit =
			btcoexist->btc_read_2byte(btcoexist, 0x42a);
		coex_dm->backup_ampdu_max_time =
			btcoexist->btc_read_1byte(btcoexist, 0x456);
	} else {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], MEDIA disconnect notify\n");

		halbtc8723d1ant_post_state_to_bt(
			btcoexist, BT_8723D_1ANT_SCOREBOARD_ACTIVE, false);

		btcoexist->btc_write_1byte(btcoexist, 0x6cd, 0x0); /* CCK Tx */
		btcoexist->btc_write_1byte(btcoexist, 0x6cf, 0x0); /* CCK Rx */

		coex_sta->cck_lock_ever = false;
	}

	halbtc8723d1ant_update_wifi_ch_info(btcoexist, type);
}

void ex_btc8723d1ant_specific_packet_notify(struct btc_coexist *btcoexist,
					    u8 type)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;

	bool under_4way = false;
	struct btc_bt_link_info *bt_link_info = &btcoexist->bt_link_info;

	if (btcoexist->manual_control || btcoexist->stop_coex_dm)
		return;

	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_4_WAY_PROGRESS,
			   &under_4way);

	if (under_4way) {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], specific Packet ---- under_4way!!\n");

		coex_sta->wifi_is_high_pri_task = true;
		coex_sta->specific_pkt_period_cnt = 2;
	} else if (type == BTC_PACKET_ARP) {
		coex_dm->arp_cnt++;
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], specific Packet ARP notify -cnt = %d\n",
			 coex_dm->arp_cnt);

		coex_sta->specific_pkt_period_cnt = 1;

		if (bt_link_info->sco_exist)
			halbtc8723d1ant_run_coexist_mechanism(btcoexist);
	} else {
		RT_TRACE(
			rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			"[BTCoex], specific Packet DHCP or EAPOL notify [Type = %d]\n",
			type);

		coex_sta->wifi_is_high_pri_task = true;
		coex_sta->specific_pkt_period_cnt = 2;
	}

	if (coex_sta->wifi_is_high_pri_task) {
		halbtc8723d1ant_post_state_to_bt(
			btcoexist, BT_8723D_1ANT_SCOREBOARD_SCAN, true);
		halbtc8723d1ant_run_coexist_mechanism(btcoexist);
	}
}

void ex_btc8723d1ant_bt_info_notify(struct btc_coexist *btcoexist, u8 *tmp_buf,
				    u8 length)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;
	u8 i, rsp_source = 0;
	bool wifi_connected = false;
	bool wifi_scan = false, wifi_link = false, wifi_roam = false,
	     wifi_busy = false;
	static bool is_scoreboard_scan;

	if (psd_scan->is_ant_det_running) {
		RT_TRACE(
			rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			"[BTCoex], bt_info_notify return for AntDet is running\n");
		return;
	}

	rsp_source = tmp_buf[0] & 0xf;
	if (rsp_source >= BT_INFO_SRC_8723D_1ANT_MAX)
		rsp_source = BT_INFO_SRC_8723D_1ANT_WIFI_FW;
	coex_sta->bt_info_c2h_cnt[rsp_source]++;

	RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		 "[BTCoex], Bt_info[%d], len=%d, data=[", rsp_source, length);

	for (i = 0; i < length; i++) {
		coex_sta->bt_info_c2h[rsp_source][i] = tmp_buf[i];

		if (i == length - 1) {
			RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
				 "0x%02x]\n", tmp_buf[i]);
		} else {
			RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD, "0x%02x, ",
				 tmp_buf[i]);
		}
	}

	coex_sta->bt_info = coex_sta->bt_info_c2h[rsp_source][1];
	coex_sta->bt_info_ext = coex_sta->bt_info_c2h[rsp_source][4];
	coex_sta->bt_info_ext2 = coex_sta->bt_info_c2h[rsp_source][5];

	if (rsp_source != BT_INFO_SRC_8723D_1ANT_WIFI_FW) {
		/* if 0xff, it means BT is under WHCK test */
		coex_sta->bt_whck_test =
			((coex_sta->bt_info == 0xff) ? true : false);

		coex_sta->bt_create_connection =
			((coex_sta->bt_info_c2h[rsp_source][2] & 0x80) ? true :
									 false);

		/* unit: %, value-100 to translate to unit: dBm */
		coex_sta->bt_rssi =
			coex_sta->bt_info_c2h[rsp_source][3] * 2 + 10;

		coex_sta->c2h_bt_remote_name_req =
			((coex_sta->bt_info_c2h[rsp_source][2] & 0x20) ? true :
									 false);

		coex_sta->is_A2DP_3M =
			((coex_sta->bt_info_c2h[rsp_source][2] & 0x10) ? true :
									 false);

		coex_sta->acl_busy =
			((coex_sta->bt_info_c2h[rsp_source][1] & 0x8) ? true :
									false);

		coex_sta->voice_over_HOGP =
			((coex_sta->bt_info_ext & 0x10) ? true : false);

		coex_sta->c2h_bt_inquiry_page =
			((coex_sta->bt_info & BT_INFO_8723D_1ANT_B_INQ_PAGE) ?
				 true :
				 false);

		coex_sta->a2dp_bit_pool =
			(((coex_sta->bt_info_c2h[rsp_source][1] & 0x49) ==
			  0x49) ?
				 (coex_sta->bt_info_c2h[rsp_source][6] & 0x7f) :
				 0);

		coex_sta->is_bt_a2dp_sink =
			(coex_sta->bt_info_c2h[rsp_source][6] & 0x80) ? true :
									false;

		coex_sta->bt_retry_cnt =
			coex_sta->bt_info_c2h[rsp_source][2] & 0xf;

		coex_sta->is_autoslot = coex_sta->bt_info_ext2 & 0x8;

		coex_sta->forbidden_slot = coex_sta->bt_info_ext2 & 0x7;

		coex_sta->hid_busy_num = (coex_sta->bt_info_ext2 & 0x30) >> 4;

		coex_sta->hid_pair_cnt = (coex_sta->bt_info_ext2 & 0xc0) >> 6;

		coex_sta->is_bt_opp_exist =
			(coex_sta->bt_info_ext2 & 0x1) ? true : false;

		if (coex_sta->bt_retry_cnt >= 1)
			coex_sta->pop_event_cnt++;

		if (coex_sta->c2h_bt_remote_name_req)
			coex_sta->cnt_remote_name_req++;

		if (coex_sta->bt_info_ext & BIT(1))
			coex_sta->cnt_reinit++;

		if (coex_sta->bt_info_ext & BIT(2)) {
			coex_sta->cnt_setup_link++;
			coex_sta->is_setup_link = true;
			coex_sta->bt_relink_downcount = 2;
			RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
				 "[BTCoex], Re-Link start in BT info!!\n");
		} else {
			coex_sta->is_setup_link = false;
			coex_sta->bt_relink_downcount = 0;
			RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
				 "[BTCoex], Re-Link stop in BT info!!\n");
		}

		if (coex_sta->bt_info_ext & BIT(3))
			coex_sta->cnt_ign_wlan_act++;

		if (coex_sta->bt_info_ext & BIT(6))
			coex_sta->cnt_role_switch++;

		if (coex_sta->bt_info_ext & BIT(7))
			coex_sta->is_bt_multi_link = true;
		else
			coex_sta->is_bt_multi_link = false;

		if (coex_sta->bt_info_ext & BIT(0))
			coex_sta->is_hid_rcu = true;
		else
			coex_sta->is_hid_rcu = false;

		if (coex_sta->bt_info_ext & BIT(5))
			coex_sta->is_ble_scan_toggle = true;
		else
			coex_sta->is_ble_scan_toggle = false;

		if (coex_sta->bt_create_connection) {
			coex_sta->cnt_page++;

			btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_BUSY,
					   &wifi_busy);

			btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_SCAN,
					   &wifi_scan);
			btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_LINK,
					   &wifi_link);
			btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_ROAM,
					   &wifi_roam);

			if ((wifi_link) || (wifi_roam) || (wifi_scan) ||
			    (coex_sta->wifi_is_high_pri_task) || (wifi_busy)) {
				is_scoreboard_scan = true;
				halbtc8723d1ant_post_state_to_bt(
					btcoexist,
					BT_8723D_1ANT_SCOREBOARD_SCAN, true);

			} else
				halbtc8723d1ant_post_state_to_bt(
					btcoexist,
					BT_8723D_1ANT_SCOREBOARD_SCAN, false);

		} else {
			if (is_scoreboard_scan) {
				halbtc8723d1ant_post_state_to_bt(
					btcoexist,
					BT_8723D_1ANT_SCOREBOARD_SCAN, false);
				is_scoreboard_scan = false;
			}
		}

		/* Here we need to resend some wifi info to BT */
		/* because bt is reset and loss of the info. */

		if ((!btcoexist->manual_control) &&
		    (!btcoexist->stop_coex_dm)) {
			btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_CONNECTED,
					   &wifi_connected);

			/*	Re-Init */
			if ((coex_sta->bt_info_ext & BIT(1))) {
				RT_TRACE(
					rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
					"[BTCoex], BT ext info bit1 check, send wifi BW&Chnl to BT!!\n");
				if (wifi_connected)
					halbtc8723d1ant_update_wifi_ch_info(
						btcoexist, BTC_MEDIA_CONNECT);
				else
					halbtc8723d1ant_update_wifi_ch_info(
						btcoexist,
						BTC_MEDIA_DISCONNECT);
			}

			/* If Ignore_WLanAct && not SetUp_Link or Role_Switch */
			if ((coex_sta->bt_info_ext & BIT(3)) &&
			    (!(coex_sta->bt_info_ext & BIT(2))) &&
			    (!(coex_sta->bt_info_ext & BIT(6)))) {
				RT_TRACE(
					rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
					"[BTCoex], BT ext info bit3 check, set BT NOT to ignore Wlan active!!\n");
				halbtc8723d1ant_ignore_wlan_act(
					btcoexist, FORCE_EXEC, false);
			} else {
				if (coex_sta->bt_info_ext & BIT(2)) {
					RT_TRACE(
						rtlpriv, COMP_BT_COEXIST,
						DBG_LOUD,
						"[BTCoex], BT ignore Wlan active because Re-link!!\n");
				} else if (coex_sta->bt_info_ext & BIT(6)) {
					RT_TRACE(
						rtlpriv, COMP_BT_COEXIST,
						DBG_LOUD,
						"[BTCoex], BT ignore Wlan active because Role-Switch!!\n");
				}
			}
		}
	}

	halbtc8723d1ant_update_bt_link_info(btcoexist);

	halbtc8723d1ant_run_coexist_mechanism(btcoexist);
}

void ex_halbtc8723d1ant_wl_fwdbginfo_notify(struct btc_coexist *btcoexist,
					    u8 *tmp_buf, u8 length)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;

	u8 i = 0;
	static u8 tmp_buf_pre[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		 "[BTCoex], WiFi Fw Dbg info = %d %d %d %d %d %d (len = %d)\n",
		 tmp_buf[0], tmp_buf[1], tmp_buf[2], tmp_buf[3], tmp_buf[4],
		 tmp_buf[5], length);

	if (tmp_buf[0] == 0x8) {
		for (i = 1; i <= 5; i++) {
			coex_sta->wl_fw_dbg_info[i] =
				(tmp_buf[i] >= tmp_buf_pre[i]) ?
					(tmp_buf[i] - tmp_buf_pre[i]) :
					(255 - tmp_buf_pre[i] + tmp_buf[i]);

			tmp_buf_pre[i] = tmp_buf[i];
		}
	}
}

void ex_halbtc8723d1ant_rx_rate_change_notify(struct btc_coexist *btcoexist,
					      bool is_data_frame,
					      u8 btc_rate_id)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;

	bool wifi_connected = false;

	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_CONNECTED,
			   &wifi_connected);

	if (is_data_frame) {
		coex_sta->wl_rx_rate = btc_rate_id;

		RT_TRACE(
			rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			"[BTCoex], rx_rate_change_notify data rate id = %d, RTS_Rate = %d\n",
			coex_sta->wl_rx_rate, coex_sta->wl_rts_rx_rate);
	} else {
		coex_sta->wl_rts_rx_rate = btc_rate_id;

		RT_TRACE(
			rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			"[BTCoex], rts_rate_change_notify RTS rate id = %d, RTS_Rate = %d\n",
			coex_sta->wl_rts_rx_rate, coex_sta->wl_rts_rx_rate);
	}

	if ((wifi_connected) &&
	    ((coex_dm->bt_status == BT_8723D_1ANT_BT_STATUS_ACL_BUSY) ||
	     (coex_dm->bt_status == BT_8723D_1ANT_BT_STATUS_ACL_SCO_BUSY) ||
	     (coex_dm->bt_status == BT_8723D_1ANT_BT_STATUS_SCO_BUSY))) {
		if ((coex_sta->wl_rx_rate == BTC_CCK_5_5) ||
		    (coex_sta->wl_rx_rate == BTC_OFDM_6) ||
		    (coex_sta->wl_rx_rate == BTC_MCS_0)) {
			coex_sta->cck_lock_warn = true;

			RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
				 "[BTCoex], cck lock warning...\n");
		} else if ((coex_sta->wl_rx_rate == BTC_CCK_1) ||
			   (coex_sta->wl_rx_rate == BTC_CCK_2) ||
			   (coex_sta->wl_rts_rx_rate == BTC_CCK_1) ||
			   (coex_sta->wl_rts_rx_rate == BTC_CCK_2)) {
			coex_sta->cck_lock = true;
			coex_sta->cck_lock_ever = true;

			RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
				 "[BTCoex], cck locking...\n");
		} else {
			coex_sta->cck_lock_warn = false;
			coex_sta->cck_lock = false;

			RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
				 "[BTCoex], cck unlock...\n");
		}
	} else {
		if ((coex_dm->bt_status ==
		     BT_8723D_1ANT_BT_STATUS_CONNECTED_IDLE) ||
		    (coex_dm->bt_status ==
		     BT_8723D_1ANT_BT_STATUS_NON_CONNECTED_IDLE)) {
			coex_sta->cck_lock_warn = false;
			coex_sta->cck_lock = false;
		}
	}
}

void ex_btc8723d1ant_rf_status_notify(struct btc_coexist *btcoexist, u8 type)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;

	RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		 "[BTCoex], RF Status notify\n");

	if (type == BTC_RF_ON) {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], RF is turned ON!!\n");

		btcoexist->stop_coex_dm = false;
		coex_sta->is_rf_state_off = false;

	} else if (type == BTC_RF_OFF) {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], RF is turned OFF!!\n");

		halbtc8723d1ant_post_state_to_bt(
			btcoexist, BT_8723D_1ANT_SCOREBOARD_ACTIVE |
					   BT_8723D_1ANT_SCOREBOARD_ONOFF |
					   BT_8723D_1ANT_SCOREBOARD_SCAN |
					   BT_8723D_1ANT_SCOREBOARD_UNDERTEST,
			false);

		halbtc8723d1ant_set_ant_path(btcoexist, BTC_ANT_PATH_AUTO,
					     FORCE_EXEC,
					     BT_8723D_1ANT_PHASE_WLAN_OFF);

		halbtc8723d1ant_ps_tdma(btcoexist, FORCE_EXEC, false, 0);

		btcoexist->stop_coex_dm = true;
		coex_sta->is_rf_state_off = true;
	}
}

void ex_btc8723d1ant_halt_notify(struct btc_coexist *btcoexist)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;

	RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD, "[BTCoex], Halt notify\n");

	halbtc8723d1ant_post_state_to_bt(
		btcoexist, BT_8723D_1ANT_SCOREBOARD_ACTIVE |
				   BT_8723D_1ANT_SCOREBOARD_ONOFF |
				   BT_8723D_1ANT_SCOREBOARD_SCAN |
				   BT_8723D_1ANT_SCOREBOARD_UNDERTEST,
		false);

	halbtc8723d1ant_set_ant_path(btcoexist, BTC_ANT_PATH_AUTO, FORCE_EXEC,
				     BT_8723D_1ANT_PHASE_WLAN_OFF);

	ex_btc8723d1ant_media_status_notify(btcoexist, BTC_MEDIA_DISCONNECT);

	halbtc8723d1ant_ps_tdma(btcoexist, FORCE_EXEC, false, 0);

	btcoexist->stop_coex_dm = true;
}

void ex_btc8723d1ant_pnp_notify(struct btc_coexist *btcoexist, u8 pnp_state)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;

	RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD, "[BTCoex], Pnp notify\n");

	if ((pnp_state == BTC_WIFI_PNP_SLEEP) ||
	    (pnp_state == BTC_WIFI_PNP_SLEEP_KEEP_ANT)) {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], Pnp notify to SLEEP\n");

		halbtc8723d1ant_post_state_to_bt(
			btcoexist, BT_8723D_1ANT_SCOREBOARD_ACTIVE |
					   BT_8723D_1ANT_SCOREBOARD_ONOFF |
					   BT_8723D_1ANT_SCOREBOARD_SCAN |
					   BT_8723D_1ANT_SCOREBOARD_UNDERTEST,
			false);

		if (pnp_state == BTC_WIFI_PNP_SLEEP_KEEP_ANT) {
			halbtc8723d1ant_set_ant_path(
				btcoexist, BTC_ANT_PATH_AUTO, FORCE_EXEC,
				BT_8723D_1ANT_PHASE_2G_RUNTIME);
		} else {
			halbtc8723d1ant_set_ant_path(
				btcoexist, BTC_ANT_PATH_AUTO, FORCE_EXEC,
				BT_8723D_1ANT_PHASE_WLAN_OFF);
		}

		btcoexist->stop_coex_dm = true;
	} else if (pnp_state == BTC_WIFI_PNP_WAKE_UP) {
		RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
			 "[BTCoex], Pnp notify to WAKE UP\n");
		/*btcoexist->stop_coex_dm = false;*/
	}
}

void ex_btc8723d1ant_coex_dm_reset(struct btc_coexist *btcoexist)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;

	RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		 "[BTCoex], *****************Coex DM Reset*****************\n");

	halbtc8723d1ant_init_hw_config(btcoexist, false, false);
	halbtc8723d1ant_init_coex_dm(btcoexist);
}

void ex_btc8723d1ant_periodical(struct btc_coexist *btcoexist)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;

	bool bt_relink_finish = false, special_pkt_finish = false;

	RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		 "[BTCoex], ************* Periodical *************\n");

#if (BT_AUTO_REPORT_ONLY_8723D_1ANT == 0)
	halbtc8723d1ant_query_bt_info(btcoexist);

#endif

	halbtc8723d1ant_monitor_bt_ctr(btcoexist);
	halbtc8723d1ant_monitor_wifi_ctr(btcoexist);

	halbtc8723d1ant_monitor_bt_enable_disable(btcoexist);

	if (coex_sta->bt_relink_downcount != 0) {
		coex_sta->bt_relink_downcount--;

		if (coex_sta->bt_relink_downcount == 0) {
			coex_sta->is_setup_link = false;
			bt_relink_finish = true;
		}
	}

	/* for 4-way, DHCP, EAPOL packet */
	if (coex_sta->specific_pkt_period_cnt > 0) {
		coex_sta->specific_pkt_period_cnt--;

		if ((coex_sta->specific_pkt_period_cnt == 0) &&
		    (coex_sta->wifi_is_high_pri_task))
			coex_sta->wifi_is_high_pri_task = false;
		special_pkt_finish = true;
	}

	/*for A2DP glitch during connecting AP*/
	if (coex_sta->connect_ap_period_cnt > 0)
		coex_sta->connect_ap_period_cnt--;

	if (halbtc8723d1ant_is_wifibt_status_changed(btcoexist) ||
	    (bt_relink_finish) || (special_pkt_finish))
		halbtc8723d1ant_run_coexist_mechanism(btcoexist);
}

void ex_halbtc8723d1ant_set_antenna_notify(struct btc_coexist *btcoexist,
					   u8 type)
{
	struct btc_board_info *board_info = &btcoexist->board_info;

	if (btcoexist->manual_control || btcoexist->stop_coex_dm)
		return;

	if (type == 2) { /* two antenna */
		board_info->ant_div_cfg = true;
		halbtc8723d1ant_set_ant_path(btcoexist, BTC_ANT_PATH_WIFI,
					     FORCE_EXEC,
					     BT_8723D_1ANT_PHASE_2G_RUNTIME);
	} else { /* one antenna */
		halbtc8723d1ant_set_ant_path(btcoexist, BTC_ANT_PATH_AUTO,
					     FORCE_EXEC,
					     BT_8723D_1ANT_PHASE_2G_RUNTIME);
	}
}

void ex_btc8723d1ant_antenna_detection(struct btc_coexist *btcoexist,
				       u32 cent_freq, u32 offset, u32 span,
				       u32 seconds)
{
	struct rtl_priv *rtlpriv = btcoexist->adapter;

	RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD,
		 "xxxxxxxxxxxxxxxx Ext Call AntennaDetect()!!\n");
}

void ex_btc8723d1ant_display_ant_detection(struct btc_coexist *btcoexist) {}

void ex_btc8723d1ant_antenna_isolation(struct btc_coexist *btcoexist,
				       u32 cent_freq, u32 offset, u32 span,
				       u32 seconds)
{
}

void ex_btc8723d1ant_psd_scan(struct btc_coexist *btcoexist, u32 cent_freq,
			      u32 offset, u32 span, u32 seconds)
{
}
