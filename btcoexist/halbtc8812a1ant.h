//===========================================
// The following is for 8812A_1ANT BT Co-exist definition
//===========================================
#define	BT_INFO_8812A_1ANT_B_FTP						BIT7
#define	BT_INFO_8812A_1ANT_B_A2DP					BIT6
#define	BT_INFO_8812A_1ANT_B_HID						BIT5
#define	BT_INFO_8812A_1ANT_B_SCO_BUSY				BIT4
#define	BT_INFO_8812A_1ANT_B_ACL_BUSY				BIT3
#define	BT_INFO_8812A_1ANT_B_INQ_PAGE				BIT2
#define	BT_INFO_8812A_1ANT_B_SCO_ESCO				BIT1
#define	BT_INFO_8812A_1ANT_B_CONNECTION				BIT0

#define	BT_INFO_8812A_1ANT_A2DP_BASIC_RATE(_BT_INFO_EXT_)	\
		(((_BT_INFO_EXT_&BIT0))? true:false)

#define	BTC_RSSI_COEX_THRESH_TOL_8812A_1ANT		2

#define	BTC_8812A_1ANT_SWITCH_TO_WIFI				0
#define	BTC_8812A_1ANT_SWITCH_TO_BT					1

typedef enum _BT_INFO_SRC_8812A_1ANT{
	BT_INFO_SRC_8812A_1ANT_WIFI_FW			= 0x0,
	BT_INFO_SRC_8812A_1ANT_BT_RSP				= 0x1,
	BT_INFO_SRC_8812A_1ANT_BT_ACTIVE_SEND		= 0x2,
	BT_INFO_SRC_8812A_1ANT_MAX
}BT_INFO_SRC_8812A_1ANT,*PBT_INFO_SRC_8812A_1ANT;

typedef enum _BT_8812A_1ANT_BT_STATUS{
	BT_8812A_1ANT_BT_STATUS_NON_CONNECTED_IDLE	= 0x0,
	BT_8812A_1ANT_BT_STATUS_CONNECTED_IDLE		= 0x1,
	BT_8812A_1ANT_BT_STATUS_INQ_PAGE				= 0x2,
	BT_8812A_1ANT_BT_STATUS_ACL_BUSY				= 0x3,
	BT_8812A_1ANT_BT_STATUS_SCO_BUSY				= 0x4,
	BT_8812A_1ANT_BT_STATUS_ACL_SCO_BUSY			= 0x5,
	BT_8812A_1ANT_BT_STATUS_MAX
}BT_8812A_1ANT_BT_STATUS,*PBT_8812A_1ANT_BT_STATUS;

typedef enum _BT_8812A_1ANT_WIFI_STATUS{
	BT_8812A_1ANT_WIFI_STATUS_NON_CONNECTED_IDLE				= 0x0,
	BT_8812A_1ANT_WIFI_STATUS_NON_CONNECTED_ASSO_AUTH_SCAN		= 0x1,
	BT_8812A_1ANT_WIFI_STATUS_CONNECTED_SCAN					= 0x2,
	BT_8812A_1ANT_WIFI_STATUS_CONNECTED_SPECIAL_PKT				= 0x3,
	BT_8812A_1ANT_WIFI_STATUS_CONNECTED_IDLE					= 0x4,
	BT_8812A_1ANT_WIFI_STATUS_CONNECTED_BUSY					= 0x5,
	BT_8812A_1ANT_WIFI_STATUS_MAX
}BT_8812A_1ANT_WIFI_STATUS,*PBT_8812A_1ANT_WIFI_STATUS;

typedef enum _BT_8812A_1ANT_COEX_ALGO{
	BT_8812A_1ANT_COEX_ALGO_UNDEFINED			= 0x0,
	BT_8812A_1ANT_COEX_ALGO_SCO				= 0x1,
	BT_8812A_1ANT_COEX_ALGO_HID				= 0x2,
	BT_8812A_1ANT_COEX_ALGO_A2DP				= 0x3,
	BT_8812A_1ANT_COEX_ALGO_A2DP_PANHS		= 0x4,
	BT_8812A_1ANT_COEX_ALGO_PANEDR			= 0x5,
	BT_8812A_1ANT_COEX_ALGO_PANHS			= 0x6,
	BT_8812A_1ANT_COEX_ALGO_PANEDR_A2DP		= 0x7,
	BT_8812A_1ANT_COEX_ALGO_PANEDR_HID		= 0x8,
	BT_8812A_1ANT_COEX_ALGO_HID_A2DP_PANEDR	= 0x9,
	BT_8812A_1ANT_COEX_ALGO_HID_A2DP			= 0xa,
	BT_8812A_1ANT_COEX_ALGO_MAX				= 0xb,
}BT_8812A_1ANT_COEX_ALGO,*PBT_8812A_1ANT_COEX_ALGO;

typedef struct _coex_dm_8812a_1ant{
	// fw mechanism
	bool	cur_ignore_wlan_act;
	bool	pre_ignore_wlan_act;
	u8		pre_ps_tdma;
	u8		cur_ps_tdma;
	u8		ps_tdma_para[5];
	u8		ps_tdma_du_adj_type;

	bool		reset_tdma_adjust;
	bool		pre_ps_tdma_on;
	bool		cur_ps_tdma_on;

	bool		pre_bt_auto_report;
	bool		cur_bt_auto_report;
	u8		pre_lps;
	u8		cur_lps;
	u8		pre_rpwm;
	u8		cur_rpwm;


	// sw mechanism
	// sw mechanism
	bool 	pre_low_penalty_ra;
	bool		cur_low_penalty_ra;
	bool		pre_dac_swing_on;//not used
	u32		pre_val_0x6c0;
	u32		cur_val_0x6c0;
	u32		pre_val_0x6c4;
	u32		cur_val_0x6c4;
	u32		pre_val_0x6c8;
	u32		cur_val_0x6c8;
	u8		pre_val_0x6cc;
	u8		cur_val_0x6cc;



	// algorithm related
	u8		pre_algorithm;
	u8		cur_algorithm;
	u8		bt_status;
	u8		wifi_chnl_info[3];


	u32		pre_ra_mask;
	u32		cur_ra_mask;


	u8		error_condition;
} coex_dm_8812a_1ant;

typedef struct _COEX_STA_8812A_1ANT{
	bool					bt_link_exist;
	bool					sco_exist;
	bool					a2dp_exist;
	bool					hid_exist;
	bool					pan_exist;

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
	u8					bt_info_c2h[BT_INFO_SRC_8812A_1ANT_MAX][10];
	u32					bt_info_c2h_cnt[BT_INFO_SRC_8812A_1ANT_MAX];

	u32					bt_info_query_cnt;
	bool					c2h_bt_inquiry_page;
	u8					bt_retry_cnt;
	u8					bt_info_ext;

}coex_sta_8812a_1ant;

//===========================================
// The following is interface which will notify coex module.
//===========================================
void
ex_halbtc8812a1ant_power_on_setting(
		struct btc_coexist *		btcoexist
	);
void
ex_halbtc8812a1ant_init_hwconfig(
		struct btc_coexist *		btcoexist
	);
void
ex_halbtc8812a1ant_init_coex_dm(
		struct btc_coexist *		btcoexist
	);
void
ex_halbtc8812a1ant_ips_notify(
		struct btc_coexist *		btcoexist,
		u8			type
	);
void
ex_halbtc8812a1ant_lps_notify(
		struct btc_coexist *		btcoexist,
		u8			type
	);
void
ex_halbtc8812a1ant_scan_notify(
		struct btc_coexist *		btcoexist,
		u8			type
	);
void
ex_halbtc8812a1ant_connect_notify(
		struct btc_coexist *		btcoexist,
		u8			type
	);
void
ex_halbtc8812a1ant_media_status_notify(
		struct btc_coexist *			btcoexist,
		u8				type
	);
void
ex_halbtc8812a1ant_special_packet_notify(
		struct btc_coexist *			btcoexist,
		u8				type
	);
void
ex_halbtc8812a1ant_bt_info_notify(
		struct btc_coexist *		btcoexist,
		u8 *			tmp_buf,
		u8			length
	);
void
ex_halbtc8812a1ant_halt_notify(
		struct btc_coexist *			btcoexist
	);
void
ex_halbtc8812a1ant_pnp_notify(
		struct btc_coexist *			btcoexist,
		u8				pnp_state
	);
void
ex_halbtc8812a1ant_periodical(
		struct btc_coexist *			btcoexist
	);
void
ex_halbtc8812a1ant_display_coex_info(
		struct btc_coexist *		btcoexist
	);
void
ex_halbtc8812a1ant_dbg_control(
		struct btc_coexist *			btcoexist,
		u8				op_code,
		u8				op_len,
		u8 * 			data
	);
