


/* *******************************************
 * The following is for 8812A 2Ant BT Co-exist definition
 * ******************************************* */
#define	BT_AUTO_REPORT_ONLY_8812A_2ANT				0

#define	BT_INFO_8812A_2ANT_B_FTP						BIT(7)
#define	BT_INFO_8812A_2ANT_B_A2DP					BIT(6)
#define	BT_INFO_8812A_2ANT_B_HID						BIT(5)
#define	BT_INFO_8812A_2ANT_B_SCO_BUSY				BIT(4)
#define	BT_INFO_8812A_2ANT_B_ACL_BUSY				BIT(3)
#define	BT_INFO_8812A_2ANT_B_INQ_PAGE				BIT(2)
#define	BT_INFO_8812A_2ANT_B_SCO_ESCO				BIT(1)
#define	BT_INFO_8812A_2ANT_B_CONNECTION				BIT(0)

#define	BT_INFO_8812A_2ANT_A2DP_BASIC_RATE(_BT_INFO_EXT_)	\
		(((_BT_INFO_EXT_&BIT(0))) ? true : false)

#define		BTC_RSSI_COEX_THRESH_TOL_8812A_2ANT		2

enum bt_info_src_8812a_2ant {
	BT_INFO_SRC_8812A_2ANT_WIFI_FW			= 0x0,
	BT_INFO_SRC_8812A_2ANT_BT_RSP				= 0x1,
	BT_INFO_SRC_8812A_2ANT_BT_ACTIVE_SEND		= 0x2,
	BT_INFO_SRC_8812A_2ANT_MAX
};

enum bt_8812a_2ant_bt_status {
	BT_8812A_2ANT_BT_STATUS_NON_CONNECTED_IDLE	= 0x0,
	BT_8812A_2ANT_BT_STATUS_CONNECTED_IDLE		= 0x1,
	BT_8812A_2ANT_BT_STATUS_INQ_PAGE				= 0x2,
	BT_8812A_2ANT_BT_STATUS_ACL_BUSY				= 0x3,
	BT_8812A_2ANT_BT_STATUS_SCO_BUSY				= 0x4,
	BT_8812A_2ANT_BT_STATUS_ACL_SCO_BUSY			= 0x5,
	BT_8812A_2ANT_BT_STATUS_MAX
};

enum bt_8812a_2ant_coex_algo {
	BT_8812A_2ANT_COEX_ALGO_UNDEFINED		= 0x0,
	BT_8812A_2ANT_COEX_ALGO_SCO				= 0x1,
	BT_8812A_2ANT_COEX_ALGO_SCO_HID		= 0x2,
	BT_8812A_2ANT_COEX_ALGO_HID				= 0x3,
	BT_8812A_2ANT_COEX_ALGO_A2DP			= 0x4,
	BT_8812A_2ANT_COEX_ALGO_A2DP_PANHS		= 0x5,
	BT_8812A_2ANT_COEX_ALGO_PANEDR			= 0x6,
	BT_8812A_2ANT_COEX_ALGO_PANHS			= 0x7,
	BT_8812A_2ANT_COEX_ALGO_PANEDR_A2DP		= 0x8,
	BT_8812A_2ANT_COEX_ALGO_PANEDR_HID		= 0x9,
	BT_8812A_2ANT_COEX_ALGO_HID_A2DP_PANEDR	= 0xa,
	BT_8812A_2ANT_COEX_ALGO_HID_A2DP_PANHS	= 0xb,
	BT_8812A_2ANT_COEX_ALGO_HID_A2DP		= 0xc,
	BT_8812A_2ANT_COEX_ALGO_MAX				= 0xd
};

struct coex_dm_8812a_2ant {
	/* fw mechanism */
	u8		pre_bt_dec_pwr_lvl;
	u8		cur_bt_dec_pwr_lvl;
	u8		pre_fw_dac_swing_lvl;
	u8		cur_fw_dac_swing_lvl;
	bool		cur_ignore_wlan_act;
	bool		pre_ignore_wlan_act;
	u8		pre_ps_tdma;
	u8		cur_ps_tdma;
	u8		ps_tdma_para[5];
	u8		ps_tdma_du_adj_type;
	bool		auto_tdma_adjust;
	bool		auto_tdma_adjust_low_rssi;
	bool		pre_ps_tdma_on;
	bool		cur_ps_tdma_on;
	bool		pre_bt_auto_report;
	bool		cur_bt_auto_report;
	u8		pre_lps;
	u8		cur_lps;
	u8		pre_rpwm;
	u8		cur_rpwm;

	/* sw mechanism */
	bool		pre_rf_rx_lpf_shrink;
	bool		cur_rf_rx_lpf_shrink;
	u32		bt_rf_0x1e_backup;
	bool	pre_low_penalty_ra;
	bool		cur_low_penalty_ra;
	bool		pre_dac_swing_on;
	u32		pre_dac_swing_lvl;
	bool		cur_dac_swing_on;
	u32		cur_dac_swing_lvl;
	bool		pre_adc_back_off;
	bool		cur_adc_back_off;
	bool	pre_agc_table_en;
	bool		cur_agc_table_en;
	u32		pre_val0x6c0;
	u32		cur_val0x6c0;
	u32		pre_val0x6c4;
	u32		cur_val0x6c4;
	u32		pre_val0x6c8;
	u32		cur_val0x6c8;
	u8		pre_val0x6cc;
	u8		cur_val0x6cc;
	bool		limited_dig;
	u32		backup_arfr_cnt1;	/* Auto Rate Fallback Retry cnt */
	u32		backup_arfr_cnt2;	/* Auto Rate Fallback Retry cnt */
	u16		backup_retry_limit;
	u8		backup_ampdu_max_time;

	/* algorithm related */
	u8		pre_algorithm;
	u8		cur_algorithm;
	u8		bt_status;
	u8		wifi_chnl_info[3];

	u32		pre_ra_mask;
	u32		cur_ra_mask;
	u8		cur_ra_mask_type;
	u8		pre_arfr_type;
	u8		cur_arfr_type;
	u8		pre_retry_limit_type;
	u8		cur_retry_limit_type;
	u8		pre_ampdu_time_type;
	u8		cur_ampdu_time_type;
};

struct coex_sta_8812a_2ant {
	bool					bt_disabled;
	bool					bt_link_exist;
	bool					sco_exist;
	bool					a2dp_exist;
	bool					hid_exist;
	bool					pan_exist;
	bool					acl_busy;

	bool					under_lps;
	bool					under_ips;
	u32					high_priority_tx;
	u32					high_priority_rx;
	u32					low_priority_tx;
	u32					low_priority_rx;
	u8					bt_rssi;
	u8					pre_bt_rssi_state;
	u8					pre_wifi_rssi_state[4];
	bool					c2h_bt_info_req_sent;
	u8					bt_info_c2h[BT_INFO_SRC_8812A_2ANT_MAX][10];
	u32					bt_info_c2h_cnt[BT_INFO_SRC_8812A_2ANT_MAX];
	u32					bt_info_query_cnt;
	bool					c2h_bt_inquiry_page;
	u8					bt_retry_cnt;
	u8					bt_info_ext;
};

/* *******************************************
 * The following is interface which will notify coex module.
 * ******************************************* */
void ex_halbtc8812a2ant_power_on_setting(struct btc_coexist *btcoexist);
void ex_halbtc8812a2ant_init_hw_config(struct btc_coexist *btcoexist,
				       bool wifi_only);
void ex_halbtc8812a2ant_init_coex_dm(struct btc_coexist *btcoexist);
void ex_halbtc8812a2ant_ips_notify(struct btc_coexist *btcoexist,
				   u8 type);
void ex_halbtc8812a2ant_lps_notify(struct btc_coexist *btcoexist,
				   u8 type);
void ex_halbtc8812a2ant_scan_notify(struct btc_coexist *btcoexist,
				    u8 type);
void ex_halbtc8812a2ant_connect_notify(struct btc_coexist *btcoexist,
				       u8 type);
void ex_halbtc8812a2ant_media_status_notify(struct btc_coexist *btcoexist,
		u8 type);
void ex_halbtc8812a2ant_specific_packet_notify(struct btc_coexist *btcoexist,
		u8 type);
void ex_halbtc8812a2ant_bt_info_notify(struct btc_coexist *btcoexist,
				       u8 *tmp_buf, u8 length);
void ex_halbtc8812a2ant_rf_status_notify(struct btc_coexist *btcoexist,
		u8 type);

void ex_halbtc8812a2ant_halt_notify(struct btc_coexist *btcoexist);
void ex_halbtc8812a2ant_periodical(struct btc_coexist *btcoexist);
void ex_halbtc8812a2ant_display_coex_info(struct btc_coexist *btcoexist);
void ex_halbtc8812a2ant_dbg_control(struct btc_coexist *btcoexist,
				    u8 op_code, u8 op_len, u8 *pdata);
