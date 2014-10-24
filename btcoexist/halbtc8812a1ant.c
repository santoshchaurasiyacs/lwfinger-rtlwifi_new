//============================================================
// Description:
//
// This file is for 8812a1ant Co-exist mechanism
//
// History
// 2012/11/15 Cosa first check in.
//
//============================================================

//============================================================
// include files
//============================================================
#include "halbt_precomp.h"
#if(1)
//============================================================
// Global variables, these are static variables
//============================================================
static coex_dm_8812a_1ant		glcoex_dm_8812a_1ant;
static coex_dm_8812a_1ant* 	coex_dm=&glcoex_dm_8812a_1ant;
static coex_sta_8812a_1ant		glcoex_sta_8812a_1ant;
static coex_sta_8812a_1ant*	coex_sta=&glcoex_sta_8812a_1ant;

const char *const glbt_info_src_8812a_1ant[]={
	"BT Info[wifi fw]",
	"BT Info[bt rsp]",
	"BT Info[bt auto report]",
};

u32	glcoex_ver_date_8812a_1ant=20130729;
u32	glcoex_ver_8812a_1ant=0x10;

//============================================================
// local function proto type if needed
//============================================================
//============================================================
// local function start with halbtc8812a1ant_
//============================================================
u8
halbtc8812a1ant_bt_rssi_state(
	u8			level_num,
	u8			rssi_thresh,
	u8			rssi_thresh1
	)
{
	s32			bt_rssi=0;
	u8			bt_rssi_state=coex_sta->pre_bt_rssi_state;

	bt_rssi = coex_sta->bt_rssi;

	if(level_num == 2)
	{			
		if( (coex_sta->pre_bt_rssi_state == BTC_RSSI_STATE_LOW) ||
			(coex_sta->pre_bt_rssi_state == BTC_RSSI_STATE_STAY_LOW))
		{
			if(bt_rssi >= (rssi_thresh+BTC_RSSI_COEX_THRESH_TOL_8812A_1ANT))
			{
				bt_rssi_state = BTC_RSSI_STATE_HIGH;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_RSSI_STATE, ("[BTCoex], BT Rssi state switch to High\n"));
			}
			else
			{
				bt_rssi_state = BTC_RSSI_STATE_STAY_LOW;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_RSSI_STATE, ("[BTCoex], BT Rssi state stay at Low\n"));
			}
		}
		else
		{
			if(bt_rssi < rssi_thresh)
			{
				bt_rssi_state = BTC_RSSI_STATE_LOW;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_RSSI_STATE, ("[BTCoex], BT Rssi state switch to Low\n"));
			}
			else
			{
				bt_rssi_state = BTC_RSSI_STATE_STAY_HIGH;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_RSSI_STATE, ("[BTCoex], BT Rssi state stay at High\n"));
			}
		}
	}
	else if(level_num == 3)
	{
		if(rssi_thresh > rssi_thresh1)
		{
			BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_RSSI_STATE, ("[BTCoex], BT Rssi thresh error!!\n"));
			return coex_sta->pre_bt_rssi_state;
		}
		
		if( (coex_sta->pre_bt_rssi_state == BTC_RSSI_STATE_LOW) ||
			(coex_sta->pre_bt_rssi_state == BTC_RSSI_STATE_STAY_LOW))
		{
			if(bt_rssi >= (rssi_thresh+BTC_RSSI_COEX_THRESH_TOL_8812A_1ANT))
			{
				bt_rssi_state = BTC_RSSI_STATE_MEDIUM;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_RSSI_STATE, ("[BTCoex], BT Rssi state switch to Medium\n"));
			}
			else
			{
				bt_rssi_state = BTC_RSSI_STATE_STAY_LOW;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_RSSI_STATE, ("[BTCoex], BT Rssi state stay at Low\n"));
			}
		}
		else if( (coex_sta->pre_bt_rssi_state == BTC_RSSI_STATE_MEDIUM) ||
			(coex_sta->pre_bt_rssi_state == BTC_RSSI_STATE_STAY_MEDIUM))
		{
			if(bt_rssi >= (rssi_thresh1+BTC_RSSI_COEX_THRESH_TOL_8812A_1ANT))
			{
				bt_rssi_state = BTC_RSSI_STATE_HIGH;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_RSSI_STATE, ("[BTCoex], BT Rssi state switch to High\n"));
			}
			else if(bt_rssi < rssi_thresh)
			{
				bt_rssi_state = BTC_RSSI_STATE_LOW;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_RSSI_STATE, ("[BTCoex], BT Rssi state switch to Low\n"));
			}
			else
			{
				bt_rssi_state = BTC_RSSI_STATE_STAY_MEDIUM;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_RSSI_STATE, ("[BTCoex], BT Rssi state stay at Medium\n"));
			}
		}
		else
		{
			if(bt_rssi < rssi_thresh1)
			{
				bt_rssi_state = BTC_RSSI_STATE_MEDIUM;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_RSSI_STATE, ("[BTCoex], BT Rssi state switch to Medium\n"));
			}
			else
			{
				bt_rssi_state = BTC_RSSI_STATE_STAY_HIGH;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_RSSI_STATE, ("[BTCoex], BT Rssi state stay at High\n"));
			}
		}
	}
		
	coex_sta->pre_bt_rssi_state = bt_rssi_state;

	return bt_rssi_state;
}

u8
halbtc8812a1ant_wifi_rssi_state(
		struct btc_coexist	*		btcoexist,
		u8			index,
		u8			level_num,
		u8			rssi_thresh,
		u8			rssi_thresh1
	)
{
	s32			wifi_rssi=0;
	u8			wifi_rssi_state=coex_sta->pre_wifi_rssi_state[index];

	btcoexist->btc_get(btcoexist, BTC_GET_S4_WIFI_RSSI, &wifi_rssi);
	
	if(level_num == 2)
	{
		if( (coex_sta->pre_wifi_rssi_state[index] == BTC_RSSI_STATE_LOW) ||
			(coex_sta->pre_wifi_rssi_state[index] == BTC_RSSI_STATE_STAY_LOW))
		{
			if(wifi_rssi >= (rssi_thresh+BTC_RSSI_COEX_THRESH_TOL_8812A_1ANT))
			{
				wifi_rssi_state = BTC_RSSI_STATE_HIGH;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_WIFI_RSSI_STATE, ("[BTCoex], wifi RSSI state switch to High\n"));
			}
			else
			{
				wifi_rssi_state = BTC_RSSI_STATE_STAY_LOW;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_WIFI_RSSI_STATE, ("[BTCoex], wifi RSSI state stay at Low\n"));
			}
		}
		else
		{
			if(wifi_rssi < rssi_thresh)
			{
				wifi_rssi_state = BTC_RSSI_STATE_LOW;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_WIFI_RSSI_STATE, ("[BTCoex], wifi RSSI state switch to Low\n"));
			}
			else
			{
				wifi_rssi_state = BTC_RSSI_STATE_STAY_HIGH;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_WIFI_RSSI_STATE, ("[BTCoex], wifi RSSI state stay at High\n"));
			}
		}
	}
	else if(level_num == 3)
	{
		if(rssi_thresh > rssi_thresh1)
		{
			BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_WIFI_RSSI_STATE, ("[BTCoex], wifi RSSI thresh error!!\n"));
			return coex_sta->pre_wifi_rssi_state[index];
		}
		
		if( (coex_sta->pre_wifi_rssi_state[index] == BTC_RSSI_STATE_LOW) ||
			(coex_sta->pre_wifi_rssi_state[index] == BTC_RSSI_STATE_STAY_LOW))
		{
			if(wifi_rssi >= (rssi_thresh+BTC_RSSI_COEX_THRESH_TOL_8812A_1ANT))
			{
				wifi_rssi_state = BTC_RSSI_STATE_MEDIUM;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_WIFI_RSSI_STATE, ("[BTCoex], wifi RSSI state switch to Medium\n"));
			}
			else
			{
				wifi_rssi_state = BTC_RSSI_STATE_STAY_LOW;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_WIFI_RSSI_STATE, ("[BTCoex], wifi RSSI state stay at Low\n"));
			}
		}
		else if( (coex_sta->pre_wifi_rssi_state[index] == BTC_RSSI_STATE_MEDIUM) ||
			(coex_sta->pre_wifi_rssi_state[index] == BTC_RSSI_STATE_STAY_MEDIUM))
		{
			if(wifi_rssi >= (rssi_thresh1+BTC_RSSI_COEX_THRESH_TOL_8812A_1ANT))
			{
				wifi_rssi_state = BTC_RSSI_STATE_HIGH;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_WIFI_RSSI_STATE, ("[BTCoex], wifi RSSI state switch to High\n"));
			}
			else if(wifi_rssi < rssi_thresh)
			{
				wifi_rssi_state = BTC_RSSI_STATE_LOW;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_WIFI_RSSI_STATE, ("[BTCoex], wifi RSSI state switch to Low\n"));
			}
			else
			{
				wifi_rssi_state = BTC_RSSI_STATE_STAY_MEDIUM;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_WIFI_RSSI_STATE, ("[BTCoex], wifi RSSI state stay at Medium\n"));
			}
		}
		else
		{
			if(wifi_rssi < rssi_thresh1)
			{
				wifi_rssi_state = BTC_RSSI_STATE_MEDIUM;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_WIFI_RSSI_STATE, ("[BTCoex], wifi RSSI state switch to Medium\n"));
			}
			else
			{
				wifi_rssi_state = BTC_RSSI_STATE_STAY_HIGH;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_WIFI_RSSI_STATE, ("[BTCoex], wifi RSSI state stay at High\n"));
			}
		}
	}
		
	coex_sta->pre_wifi_rssi_state[index] = wifi_rssi_state;

	return wifi_rssi_state;
}

void
halbtc8812a1ant_update_ra_mask(
		struct btc_coexist	*		btcoexist,
		bool				force_exec,
		u8				type,
		u32				rate_mask
	)
{
	if(BTC_RATE_DISABLE == type)
	{
		coex_dm->cur_ra_mask |= rate_mask;		// disable rate
	}
	else if(BTC_RATE_ENABLE == type)
	{
		coex_dm->cur_ra_mask &= ~rate_mask;	// enable rate
	}
	
	if( force_exec || (coex_dm->pre_ra_mask != coex_dm->cur_ra_mask))
	{
		btcoexist->btc_set(btcoexist, BTC_SET_ACT_UPDATE_ra_mask, &coex_dm->cur_ra_mask);
	}
	coex_dm->pre_ra_mask = coex_dm->cur_ra_mask;
}

void
halbtc8812a1ant_monitor_bt_ctr(
		struct btc_coexist	*		btcoexist
	)
{
	u32 			reg_hp_tx_rx, reg_lp_tx_rx, u4_tmp;
	u32			reg_hp_tx=0, reg_hp_rx=0, reg_lp_tx=0, reg_lp_rx=0;
	//u8			u1_tmp;
	
	reg_hp_tx_rx = 0x770;
	reg_lp_tx_rx = 0x774;

	u4_tmp = btcoexist->btc_read_4byte(btcoexist, reg_hp_tx_rx);
	reg_hp_tx = u4_tmp & MASKLWORD;
	reg_hp_rx = (u4_tmp & MASKHWORD)>>16;

	u4_tmp = btcoexist->btc_read_4byte(btcoexist, reg_lp_tx_rx);
	reg_lp_tx = u4_tmp & MASKLWORD;
	reg_lp_rx = (u4_tmp & MASKHWORD)>>16;
		
	coex_sta->high_priority_tx = reg_hp_tx;
	coex_sta->high_priority_rx = reg_hp_rx;
	coex_sta->low_priority_tx = reg_lp_tx;
	coex_sta->low_priority_rx = reg_lp_rx;

	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_MONITOR, "[BTCoex], High Priority Tx/Rx (reg 0x%x)=0x%x(%d)/0x%x(%d)\n", 
		reg_hp_tx_rx, reg_hp_tx, reg_hp_tx, reg_hp_rx, reg_hp_rx);
	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_MONITOR, "[BTCoex], Low Priority Tx/Rx (reg 0x%x)=0x%x(%d)/0x%x(%d)\n", 
		reg_lp_tx_rx, reg_lp_tx, reg_lp_tx, reg_lp_rx, reg_lp_rx);

	// reset counter
	btcoexist->btc_write_1byte(btcoexist, 0x76e, 0xc);
}

void
halbtc8812a1ant_query_bt_info(
		struct btc_coexist	*		btcoexist
	)
{	
	u8	data_len=3;
	u8	buf[5] = {0};

	if(!btcoexist->bt_info.bt_disabled)
	{
		if(!coex_sta->bt_info_query_cnt ||
			(coex_sta->bt_info_c2h_cnt[BT_INFO_SRC_8812A_1ANT_BT_RSP]-coex_sta->bt_info_query_cnt)>2)
		{
			buf[0] = data_len;
			buf[1] = 0x1;	// polling enable, 1=enable, 0=disable
			buf[2] = 0x2;	// polling time in seconds
			buf[3] = 0x1;	// auto report enable, 1=enable, 0=disable
				
			btcoexist->btc_set(btcoexist, BTC_SET_ACT_CTRL_BT_INFO, (void*)&buf[0]);
		}
	}
	coex_sta->bt_info_query_cnt++;
}

void
halbtc8812a1ant_update_bt_link_info(
		struct btc_coexist	*		btcoexist
	)
{
	struct btc_bt_link_info*	bt_link_info=&btcoexist->bt_link_info;

	bt_link_info->bt_link_exist = coex_sta->bt_link_exist;
	bt_link_info->sco_exist = coex_sta->sco_exist;
	bt_link_info->a2dp_exist = coex_sta->a2dp_exist;
	bt_link_info->pan_exist = coex_sta->pan_exist;
	bt_link_info->hid_exist = coex_sta->hid_exist;

	// check if Sco only
	if( bt_link_info->sco_exist &&
		!bt_link_info->a2dp_exist &&
		!bt_link_info->pan_exist &&
		!bt_link_info->hid_exist )
		bt_link_info->sco_only = true;
	else
		bt_link_info->sco_only = false;

	// check if A2dp only
	if( !bt_link_info->sco_exist &&
		bt_link_info->a2dp_exist &&
		!bt_link_info->pan_exist &&
		!bt_link_info->hid_exist )
		bt_link_info->a2dp_only = true;
	else
		bt_link_info->a2dp_only = false;

	// check if Pan only
	if( !bt_link_info->sco_exist &&
		!bt_link_info->a2dp_exist &&
		bt_link_info->pan_exist &&
		!bt_link_info->hid_exist )
		bt_link_info->pan_only = true;
	else
		bt_link_info->pan_only = false;
	
	// check if Hid only
	if( !bt_link_info->sco_exist &&
		!bt_link_info->a2dp_exist &&
		!bt_link_info->pan_exist &&
		bt_link_info->hid_exist )
		bt_link_info->hid_only = true;
	else
		bt_link_info->hid_only = false;
}

u8
halbtc8812a1ant_action_algorithm(
		struct btc_coexist	*		btcoexist
	)
{
	struct btc_bt_link_info*	bt_link_info=&btcoexist->bt_link_info;
	bool				bt_hs_on=false;
	u8				algorithm=BT_8812A_1ANT_COEX_ALGO_UNDEFINED;
	u8				num_of_diff_profile=0;

	btcoexist->btc_get(btcoexist, BTC_GET_BL_HS_OPERATION, &bt_hs_on);
	
	if(!bt_link_info->bt_link_exist)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], No profile exists!!!\n"));
		return algorithm;
	}

	if(bt_link_info->sco_exist)
		num_of_diff_profile++;
	if(bt_link_info->hid_exist)
		num_of_diff_profile++;
	if(bt_link_info->pan_exist)
		num_of_diff_profile++;
	if(bt_link_info->a2dp_exist)
		num_of_diff_profile++;
	
	if(num_of_diff_profile == 1)
	{
		if(bt_link_info->sco_exist)
		{
			BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], SCO only\n"));
			algorithm = BT_8812A_1ANT_COEX_ALGO_SCO;
		}
		else
		{
			if(bt_link_info->hid_exist)
			{
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], HID only\n"));
				algorithm = BT_8812A_1ANT_COEX_ALGO_HID;
			}
			else if(bt_link_info->a2dp_exist)
			{
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], A2DP only\n"));
				algorithm = BT_8812A_1ANT_COEX_ALGO_A2DP;
			}
			else if(bt_link_info->pan_exist)
			{
				if(bt_hs_on)
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], PAN(HS) only\n"));
					algorithm = BT_8812A_1ANT_COEX_ALGO_PANHS;
				}
				else
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], PAN(EDR) only\n"));
					algorithm = BT_8812A_1ANT_COEX_ALGO_PANEDR;
				}
			}
		}
	}
	else if(num_of_diff_profile == 2)
	{
		if(bt_link_info->sco_exist)
		{
			if(bt_link_info->hid_exist)
			{
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], SCO + HID\n"));
				algorithm = BT_8812A_1ANT_COEX_ALGO_HID;
			}
			else if(bt_link_info->a2dp_exist)
			{
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], SCO + A2DP ==> SCO\n"));
				algorithm = BT_8812A_1ANT_COEX_ALGO_SCO;
			}
			else if(bt_link_info->pan_exist)
			{
				if(bt_hs_on)
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], SCO + PAN(HS)\n"));
					algorithm = BT_8812A_1ANT_COEX_ALGO_SCO;
				}
				else
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], SCO + PAN(EDR)\n"));
					algorithm = BT_8812A_1ANT_COEX_ALGO_PANEDR_HID;
				}
			}
		}
		else
		{
			if( bt_link_info->hid_exist &&
				bt_link_info->a2dp_exist )
			{
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], HID + A2DP\n"));
				algorithm = BT_8812A_1ANT_COEX_ALGO_HID_A2DP;
			}
			else if( bt_link_info->hid_exist &&
				bt_link_info->pan_exist )
			{
				if(bt_hs_on)
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], HID + PAN(HS)\n"));
					algorithm = BT_8812A_1ANT_COEX_ALGO_HID_A2DP;
				}
				else
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], HID + PAN(EDR)\n"));
					algorithm = BT_8812A_1ANT_COEX_ALGO_PANEDR_HID;
				}
			}
			else if( bt_link_info->pan_exist &&
				bt_link_info->a2dp_exist )
			{
				if(bt_hs_on)
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], A2DP + PAN(HS)\n"));
					algorithm = BT_8812A_1ANT_COEX_ALGO_A2DP_PANHS;
				}
				else
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], A2DP + PAN(EDR)\n"));
					algorithm = BT_8812A_1ANT_COEX_ALGO_PANEDR_A2DP;
				}
			}
		}
	}
	else if(num_of_diff_profile == 3)
	{
		if(bt_link_info->sco_exist)
		{
			if( bt_link_info->hid_exist &&
				bt_link_info->a2dp_exist )
			{
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], SCO + HID + A2DP ==> HID\n"));
				algorithm = BT_8812A_1ANT_COEX_ALGO_HID;
			}
			else if( bt_link_info->hid_exist &&
				bt_link_info->pan_exist )
			{
				if(bt_hs_on)
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], SCO + HID + PAN(HS)\n"));
					algorithm = BT_8812A_1ANT_COEX_ALGO_HID_A2DP;
				}
				else
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], SCO + HID + PAN(EDR)\n"));
					algorithm = BT_8812A_1ANT_COEX_ALGO_PANEDR_HID;
				}
			}
			else if( bt_link_info->pan_exist &&
				bt_link_info->a2dp_exist )
			{
				if(bt_hs_on)
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], SCO + A2DP + PAN(HS)\n"));
					algorithm = BT_8812A_1ANT_COEX_ALGO_SCO;
				}
				else
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], SCO + A2DP + PAN(EDR) ==> HID\n"));
					algorithm = BT_8812A_1ANT_COEX_ALGO_PANEDR_HID;
				}
			}
		}
		else
		{
			if( bt_link_info->hid_exist &&
				bt_link_info->pan_exist &&
				bt_link_info->a2dp_exist )
			{
				if(bt_hs_on)
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], HID + A2DP + PAN(HS)\n"));
					algorithm = BT_8812A_1ANT_COEX_ALGO_HID_A2DP;
				}
				else
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], HID + A2DP + PAN(EDR)\n"));
					algorithm = BT_8812A_1ANT_COEX_ALGO_HID_A2DP_PANEDR;
				}
			}
		}
	}
	else if(num_of_diff_profile >= 3)
	{
		if(bt_link_info->sco_exist)
		{
			if( bt_link_info->hid_exist &&
				bt_link_info->pan_exist &&
				bt_link_info->a2dp_exist )
			{
				if(bt_hs_on)
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Error!!! SCO + HID + A2DP + PAN(HS)\n"));

				}
				else
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], SCO + HID + A2DP + PAN(EDR)==>PAN(EDR)+HID\n"));
					algorithm = BT_8812A_1ANT_COEX_ALGO_PANEDR_HID;
				}
			}
		}
	}

	return algorithm;
}

void
halbtc8812a1ant_set_bt_auto_report(
		struct btc_coexist	*		btcoexist,
		bool			enable_auto_report
	)
{
}

void
halbtc8812a1ant_bt_auto_report(
		struct btc_coexist	*		btcoexist,
		bool			force_exec,
		bool			enable_auto_report
	)
{
	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW, "[BTCoex], %s BT Auto report = %s\n",  
		(force_exec? "force to":""), ((enable_auto_report)? "Enabled":"Disabled"));
	coex_dm->cur_bt_auto_report = enable_auto_report;

	if(!force_exec)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_DETAIL, "[BTCoex], pre_bt_auto_report=%d, cur_bt_auto_report=%d\n", 
			coex_dm->pre_bt_auto_report, coex_dm->cur_bt_auto_report);

		if(coex_dm->pre_bt_auto_report == coex_dm->cur_bt_auto_report) 
			return;
	}
	halbtc8812a1ant_set_bt_auto_report(btcoexist, coex_dm->cur_bt_auto_report);

	coex_dm->pre_bt_auto_report = coex_dm->cur_bt_auto_report;
}

void
halbtc8812a1ant_set_sw_penalty_tx_rate_adaptive(
		struct btc_coexist	*		btcoexist,
		bool			low_penalty_ra
	)
{
	u8	tmp_u1;

	tmp_u1 = btcoexist->btc_read_1byte(btcoexist, 0x4fd);
	tmp_u1 |= BIT0;
	if(low_penalty_ra)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_SW_EXEC, ("[BTCoex], Tx rate adaptive, set low penalty!!\n"));
		tmp_u1 &= ~BIT2;
	}
	else
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_SW_EXEC, ("[BTCoex], Tx rate adaptive, set normal!!\n"));
		tmp_u1 |= BIT2;
	}

	btcoexist->btc_write_1byte(btcoexist, 0x4fd, tmp_u1);
}

void
halbtc8812a1ant_low_penalty_ra(
		struct btc_coexist	*		btcoexist,
		bool			force_exec,
		bool			low_penalty_ra
	)
{
	return;
	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_SW, "[BTCoex], %s turn LowPenaltyRA = %s\n",  
		(force_exec? "force to":""), ((low_penalty_ra)? "ON":"OFF"));
	coex_dm->cur_low_penalty_ra = low_penalty_ra;

	if(!force_exec)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_SW_DETAIL, "[BTCoex], pre_low_penalty_ra=%d, cur_low_penalty_ra=%d\n", 
			coex_dm->pre_low_penalty_ra, coex_dm->cur_low_penalty_ra);

		if(coex_dm->pre_low_penalty_ra == coex_dm->cur_low_penalty_ra) 
			return;
	}
	halbtc8812a1ant_set_sw_penalty_tx_rate_adaptive(btcoexist, coex_dm->cur_low_penalty_ra);

	coex_dm->pre_low_penalty_ra = coex_dm->cur_low_penalty_ra;
}

void
halbtc8812a1ant_set_coex_table(
		struct btc_coexist	*	btcoexist,
		u32		val0x6c0,
		u32		val0x6c4,
		u32		val0x6c8,
		u8		val0x6cc
	)
{
	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_SW_EXEC, "[BTCoex], set coex table, set 0x6c0=0x%x\n", val0x6c0);
	btcoexist->btc_write_4byte(btcoexist, 0x6c0, val0x6c0);

	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_SW_EXEC, "[BTCoex], set coex table, set 0x6c4=0x%x\n", val0x6c4);
	btcoexist->btc_write_4byte(btcoexist, 0x6c4, val0x6c4);

	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_SW_EXEC, "[BTCoex], set coex table, set 0x6c8=0x%x\n", val0x6c8);
	btcoexist->btc_write_4byte(btcoexist, 0x6c8, val0x6c8);

	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_SW_EXEC, "[BTCoex], set coex table, set 0x6cc=0x%x\n", val0x6cc);
	btcoexist->btc_write_1byte(btcoexist, 0x6cc, val0x6cc);
}

void
halbtc8812a1ant_coex_table(
		struct btc_coexist	*		btcoexist,
		bool			force_exec,
		u32			val0x6c0,
		u32			val0x6c4,
		u32			val0x6c8,
		u8			val0x6cc
	)
{
	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_SW, "[BTCoex], %s write Coex Table 0x6c0=0x%x, 0x6c4=0x%x, 0x6c8=0x%x, 0x6cc=0x%x\n", 
		(force_exec? "force to":""), val0x6c0, val0x6c4, val0x6c8, val0x6cc);
	coex_dm->cur_val_0x6c0 = val0x6c0;
	coex_dm->cur_val_0x6c4 = val0x6c4;
	coex_dm->cur_val_0x6c8 = val0x6c8;
	coex_dm->cur_val_0x6cc = val0x6cc;

	if(!force_exec)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_SW_DETAIL, "[BTCoex], pre_val_0x6c0=0x%x, pre_val_0x6c4=0x%x, pre_val_0x6c8=0x%x, pre_val_0x6cc=0x%x !!\n", 
			coex_dm->pre_val_0x6c0, coex_dm->pre_val_0x6c4, coex_dm->pre_val_0x6c8, coex_dm->pre_val_0x6cc);
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_SW_DETAIL, "[BTCoex], cur_val_0x6c0=0x%x, cur_val_0x6c4=0x%x, cur_val_0x6c8=0x%x, cur_val_0x6cc=0x%x !!\n", 
			coex_dm->cur_val_0x6c0, coex_dm->cur_val_0x6c4, coex_dm->cur_val_0x6c8, coex_dm->cur_val_0x6cc);
	
		if( (coex_dm->pre_val_0x6c0 == coex_dm->cur_val_0x6c0) &&
			(coex_dm->pre_val_0x6c4 == coex_dm->cur_val_0x6c4) &&
			(coex_dm->pre_val_0x6c8 == coex_dm->cur_val_0x6c8) &&
			(coex_dm->pre_val_0x6cc == coex_dm->cur_val_0x6cc) )
			return;
	}
	halbtc8812a1ant_set_coex_table(btcoexist, val0x6c0, val0x6c4, val0x6c8, val0x6cc);

	coex_dm->pre_val_0x6c0 = coex_dm->cur_val_0x6c0;
	coex_dm->pre_val_0x6c4 = coex_dm->cur_val_0x6c4;
	coex_dm->pre_val_0x6c8 = coex_dm->cur_val_0x6c8;
	coex_dm->pre_val_0x6cc = coex_dm->cur_val_0x6cc;
}

void
halbtc8812a1ant_coex_table_with_type(
		struct btc_coexist	*		btcoexist,
		bool				force_exec,
		u8				type
	)
{
	switch(type)
	{
		case 0:
			halbtc8812a1ant_coex_table(btcoexist, force_exec, 0x55555555, 0x55555555, 0xffff, 0x3);
			break;
		case 1:
			halbtc8812a1ant_coex_table(btcoexist, force_exec, 0x55555555, 0x5a5a5a5a, 0xffff, 0x3);
			break;
		case 2:
			halbtc8812a1ant_coex_table(btcoexist, force_exec, 0x5a5a5a5a, 0x5a5a5a5a, 0xffff, 0x3);
			break;
		case 3:
			halbtc8812a1ant_coex_table(btcoexist, force_exec, 0xaaaaaaaa, 0xaaaaaaaa, 0xffff, 0x3);
			break;
		case 4:
			halbtc8812a1ant_coex_table(btcoexist, force_exec, 0xffffffff, 0xffffffff, 0xffff, 0x3);
			break;
		case 5:
			halbtc8812a1ant_coex_table(btcoexist, force_exec, 0x5fff5fff, 0x5fff5fff, 0xffff, 0x3);
			break;
		case 6:
			halbtc8812a1ant_coex_table(btcoexist, force_exec, 0x55ff55ff, 0x5a5a5a5a, 0xffff, 0x3);
			break;
		case 7:
			halbtc8812a1ant_coex_table(btcoexist, force_exec, 0x5afa5afa, 0x5afa5afa, 0xffff, 0x3);
			break;
		default:
			break;
	}
}

void
halbtc8812a1ant_set_fw_ignore_wlan_act(
		struct btc_coexist	*		btcoexist,
		bool			enable
	)
{
	u8	data_len=3;
	u8	buf[5] = {0};

	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_EXEC, "[BTCoex], %s BT Ignore Wlan_Act\n",
		(enable? "Enable":"Disable"));

	buf[0] = data_len;
	buf[1] = 0x1;			// OP_Code
	buf[2] = 0x1;			// OP_Code_Length
	if(enable)
		buf[3] = 0x1; 		// OP_Code_Content
	else
		buf[3] = 0x0;
		
	btcoexist->btc_set(btcoexist, BTC_SET_ACT_CTRL_BT_COEX, (void*)&buf[0]);	
}

void
halbtc8812a1ant_ignore_wlan_act(
		struct btc_coexist	*		btcoexist,
		bool			force_exec,
		bool			enable
	)
{
	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW, "[BTCoex], %s turn Ignore WlanAct %s\n", 
		(force_exec? "force to":""), (enable? "ON":"OFF"));
	coex_dm->cur_ignore_wlan_act = enable;

	if(!force_exec)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_DETAIL, "[BTCoex], pre_ignore_wlan_act = %d, cur_ignore_wlan_act = %d!!\n", 
			coex_dm->pre_ignore_wlan_act, coex_dm->cur_ignore_wlan_act);

		if(coex_dm->pre_ignore_wlan_act == coex_dm->cur_ignore_wlan_act)
			return;
	}
	halbtc8812a1ant_set_fw_ignore_wlan_act(btcoexist, enable);

	coex_dm->pre_ignore_wlan_act = coex_dm->cur_ignore_wlan_act;
}

void
halbtc8812a1ant_set_fw_pstdma(
		struct btc_coexist	*		btcoexist,
		u8			byte1,
		u8			byte2,
		u8			byte3,
		u8			byte4,
		u8			byte5
	)
{
	u8			h2c_parameter[5] ={0};

	h2c_parameter[0] = byte1;	
	h2c_parameter[1] = byte2;	
	h2c_parameter[2] = byte3;
	h2c_parameter[3] = byte4;
	h2c_parameter[4] = byte5;

	coex_dm->ps_tdma_para[0] = byte1;
	coex_dm->ps_tdma_para[1] = byte2;
	coex_dm->ps_tdma_para[2] = byte3;
	coex_dm->ps_tdma_para[3] = byte4;
	coex_dm->ps_tdma_para[4] = byte5;
	
	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_EXEC, "[BTCoex], FW write 0x60(5bytes)=0x%x%08x\n", 
		h2c_parameter[0], 
		h2c_parameter[1]<<24|h2c_parameter[2]<<16|h2c_parameter[3]<<8|h2c_parameter[4]);

	btcoexist->btc_fill_h2c(btcoexist, 0x60, 5, h2c_parameter);
}

void
halbtc8812a1ant_set_lps_rpwm(
		struct btc_coexist	*		btcoexist,
		u8			lps_val,
		u8			rpwm_val
	)
{
	u8	lps=lps_val;
	u8	rpwm=rpwm_val;
	
	btcoexist->btc_set(btcoexist, BTC_SET_U1_LPS_VAL, &lps);
	btcoexist->btc_set(btcoexist, BTC_SET_U1_RPWM_VAL, &rpwm);
}

void
halbtc8812a1ant_lps_rpwm(
		struct btc_coexist	*		btcoexist,
		bool			force_exec,
		u8			lps_val,
		u8			rpwm_val
	)
{
	//bool	bForceExecPwrCmd=false;
	
	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW, "[BTCoex], %s set lps/rpwm=0x%x/0x%x \n", 
		(force_exec? "force to":""), lps_val, rpwm_val);
	coex_dm->cur_lps = lps_val;
	coex_dm->cur_rpwm = rpwm_val;

	if(!force_exec)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_DETAIL, "[BTCoex], pre_lps/cur_lps=0x%x/0x%x, pre_rpwm/cur_rpwm=0x%x/0x%x!!\n", 
			coex_dm->pre_lps, coex_dm->cur_lps, coex_dm->pre_rpwm, coex_dm->cur_rpwm);

		if( (coex_dm->pre_lps == coex_dm->cur_lps) &&
			(coex_dm->pre_rpwm == coex_dm->cur_rpwm) )
		{
			return;
		}
	}
	halbtc8812a1ant_set_lps_rpwm(btcoexist, lps_val, rpwm_val);

	coex_dm->pre_lps = coex_dm->cur_lps;
	coex_dm->pre_rpwm = coex_dm->cur_rpwm;
}

void
halbtc8812a1ant_set_ant_path(
		struct btc_coexist	*		btcoexist,
		u8				ant_pos_type,
		bool				init_hw_cfg,
		bool				wifi_off
	)
{
	u8			u1_tmp=0;
	
	if(init_hw_cfg)
	{
		btcoexist->btc_write_1byte(btcoexist, 0xcb3, 0x77);
		
		btcoexist->btc_write_4byte(btcoexist, 0x900, 0x00000400);
	 	btcoexist->btc_write_1byte(btcoexist, 0x76d, 0x1);
	}
	else if(wifi_off)
	{

	}
	
	// ext switch setting
	switch(ant_pos_type)
	{
		case BTC_ANT_PATH_WIFI:
			u1_tmp = btcoexist->btc_read_1byte(btcoexist, 0xcb7);
			u1_tmp |= BIT3;
			u1_tmp &= ~BIT2;			
			btcoexist->btc_write_1byte(btcoexist, 0xcb7, u1_tmp);
			break;
		case BTC_ANT_PATH_BT:
			u1_tmp = btcoexist->btc_read_1byte(btcoexist, 0xcb7);
			u1_tmp &= ~BIT3;
			u1_tmp |= BIT2;
			btcoexist->btc_write_1byte(btcoexist, 0xcb7, u1_tmp);
			break;
		default:
		case BTC_ANT_PATH_PTA:
			u1_tmp = btcoexist->btc_read_1byte(btcoexist, 0xcb7);
			u1_tmp |= BIT3;
			u1_tmp &= ~BIT2;			
			btcoexist->btc_write_1byte(btcoexist, 0xcb7, u1_tmp);
			break;
	}
}

void
halbtc8812a1ant_ps_tdma(
		struct btc_coexist	*		btcoexist,
		bool			force_exec,
		bool			turn_on,
		u8			type
	)
{
	//bool			bTurnOnByCnt=false;
	//u8			psTdmaTypeByCnt=0, rssi_adjust_val=0;
	u8 rssi_adjust_val=0;

	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW, "[BTCoex], %s turn %s PS TDMA, type=%d\n", 
		(force_exec? "force to":""), (turn_on? "ON":"OFF"), type);
	coex_dm->cur_ps_tdma_on = turn_on;
	coex_dm->cur_ps_tdma = type;

	if(!force_exec)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_DETAIL, "[BTCoex], pre_ps_tdma_on = %d, cur_ps_tdma_on = %d!!\n", 
			coex_dm->pre_ps_tdma_on, coex_dm->cur_ps_tdma_on);
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_DETAIL, "[BTCoex], pre_ps_tdma = %d, cur_ps_tdma = %d!!\n", 
			coex_dm->pre_ps_tdma, coex_dm->cur_ps_tdma);

		if( (coex_dm->pre_ps_tdma_on == coex_dm->cur_ps_tdma_on) &&
			(coex_dm->pre_ps_tdma == coex_dm->cur_ps_tdma) )
			return;
	}
	if(turn_on)
	{
		switch(type)
		{
			default:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0xd3, 0x1a, 0x1a, 0x0, 0x50);
				break;
			case 1:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0xd3, 0x1a, 0x1a, 0x0, 0x50);
				rssi_adjust_val = 11;
				break;
			case 2:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0xd3, 0x12, 0x12, 0x0, 0x50);
				rssi_adjust_val = 14;
				break;
			case 3:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0x93, 0x25, 0x3, 0x10, 0x40);
				break;
			case 4:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0x93, 0x15, 0x3, 0x14, 0x0);
				rssi_adjust_val = 17;
				break;
			case 5:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0x61, 0x15, 0x3, 0x31, 0x0);
				break;
			case 6:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0x13, 0xa, 0x3, 0x0, 0x0);
				break;
			case 7:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0x13, 0xc, 0x5, 0x0, 0x0);
				break;
			case 8:	
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0x93, 0x25, 0x3, 0x10, 0x0);
				break;
			case 9:	
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0xd3, 0xa, 0xa, 0x0, 0x50);
				rssi_adjust_val = 18;
				break;
			case 10:	
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0x13, 0xa, 0xa, 0x0, 0x40);
				break;
			case 11:	
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0xd3, 0x5, 0x5, 0x0, 0x50);
				rssi_adjust_val = 20;
				break;
			case 12:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0xeb, 0xa, 0x3, 0x31, 0x18);
				break;

			case 15:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0x13, 0xa, 0x3, 0x8, 0x0);
				break;
			case 16:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0x93, 0x15, 0x3, 0x10, 0x0);
				rssi_adjust_val = 18;
				break;

			case 18:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0x93, 0x25, 0x3, 0x10, 0x0);
				rssi_adjust_val = 14;
				break;			
				
			case 20:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0x13, 0x25, 0x25, 0x0, 0x0);
				break;
			case 21:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0x93, 0x20, 0x3, 0x10, 0x40);
				break;
			case 22:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0x13, 0x8, 0x8, 0x0, 0x40);
				break;
			case 23:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0xe3, 0x25, 0x3, 0x31, 0x18);
				rssi_adjust_val = 22;
				break;
			case 24:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0xe3, 0x15, 0x3, 0x31, 0x18);
				rssi_adjust_val = 22;
				break;
			case 25:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0xe3, 0xa, 0x3, 0x31, 0x18);
				rssi_adjust_val = 22;
				break;
			case 26:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0xe3, 0xa, 0x3, 0x31, 0x18);
				rssi_adjust_val = 22;
				break;
			case 27:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0xe3, 0x25, 0x3, 0x31, 0x98);
				rssi_adjust_val = 22;
				break;
			case 28:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0x69, 0x25, 0x3, 0x31, 0x0);
				break;
			case 29:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0xab, 0x1a, 0x1a, 0x1, 0x10);
				break;
			case 30:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0x93, 0x15, 0x3, 0x14, 0x0);
				break;
			case 31:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0xd3, 0x1a, 0x1a, 0, 0x58);
				break;
			case 32:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0xab, 0xa, 0x3, 0x31, 0x90);
				break;
			case 33:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0xa3, 0x25, 0x3, 0x30, 0x90);
				break;
			case 34:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0xd3, 0x1a, 0x1a, 0x0, 0x10);
				break;
			case 35:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0xe3, 0x1a, 0x1a, 0x0, 0x10);
				break;
			case 36:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0xd3, 0x12, 0x3, 0x14, 0x50);
				break;
			case 37:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0x53, 0x25, 0x3, 0x10, 0x50);
				break;
		}
	}
	else
	{
		// disable PS tdma
		switch(type)
		{
			case 8:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0x8, 0x0, 0x0, 0x0, 0x0);
				halbtc8812a1ant_set_ant_path(btcoexist, BTC_ANT_PATH_PTA, false, false);
				break;
			case 0:
			default:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0x0, 0x0, 0x0, 0x0, 0x0);
				halbtc8812a1ant_set_ant_path(btcoexist, BTC_ANT_PATH_BT, false, false);
				break;
			case 9:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0x0, 0x0, 0x0, 0x0, 0x0);
				halbtc8812a1ant_set_ant_path(btcoexist, BTC_ANT_PATH_WIFI, false, false);
				break;
			case 10:
				halbtc8812a1ant_set_fw_pstdma(btcoexist, 0x0, 0x0, 0x0, 0x8, 0x0);
				halbtc8812a1ant_set_ant_path(btcoexist, BTC_ANT_PATH_BT, false, false);
				break;
		}
	}
	rssi_adjust_val =0;
	btcoexist->btc_set(btcoexist, BTC_SET_U1_RSSI_ADJ_VAL_FOR_1ANT_COEX_TYPE, &rssi_adjust_val);

	// update pre state
	coex_dm->pre_ps_tdma_on = coex_dm->cur_ps_tdma_on;
	coex_dm->pre_ps_tdma = coex_dm->cur_ps_tdma;
}

void
halbtc8812a1ant_coex_all_off(
		struct btc_coexist	*		btcoexist
	)
{
	// hw all off
	halbtc8812a1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 0);
}

bool
halbtc8812a1ant_is_common_action(
		struct btc_coexist	*		btcoexist
	)
{
	bool			common=false, wifi_connected=false, wifi_busy=false;

	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_CONNECTED, &wifi_connected);
	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_BUSY, &wifi_busy);

	if(!wifi_connected && 
		BT_8812A_1ANT_BT_STATUS_NON_CONNECTED_IDLE == coex_dm->bt_status)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Wifi non connected-idle + BT non connected-idle!!\n"));

		common = true;
	}
	else if(wifi_connected && 
		(BT_8812A_1ANT_BT_STATUS_NON_CONNECTED_IDLE == coex_dm->bt_status) )
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Wifi connected + BT non connected-idle!!\n"));

		common = true;
	}
	else if(!wifi_connected && 
		(BT_8812A_1ANT_BT_STATUS_CONNECTED_IDLE == coex_dm->bt_status) )
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Wifi non connected-idle + BT connected-idle!!\n"));

		common = true;
	}
	else if(wifi_connected && 
		(BT_8812A_1ANT_BT_STATUS_CONNECTED_IDLE == coex_dm->bt_status) )
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Wifi connected + BT connected-idle!!\n"));

		common = true;
	}
	else if(!wifi_connected && 
		(BT_8812A_1ANT_BT_STATUS_CONNECTED_IDLE != coex_dm->bt_status) )
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Wifi non connected-idle + BT Busy!!\n"));
		
		common = true;
	}
	else
	{	
		common = false;
	}
	
	return common;
}


void
halbtc8812a1ant_tdma_duration_adjust_for_acl(
		struct btc_coexist	*		btcoexist,
		u8				wifi_status
	)
{
	static s32		up,dn,m,n,wait_count;
	s32			result;   //0: no change, +1: increase WiFi duration, -1: decrease WiFi duration
	u8			retry_count=0, bt_info_ext;

	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW, ("[BTCoex], TdmaDurationAdjustForAcl()\n"));

	if( (BT_8812A_1ANT_WIFI_STATUS_NON_CONNECTED_ASSO_AUTH_SCAN == wifi_status) ||
		(BT_8812A_1ANT_WIFI_STATUS_CONNECTED_SCAN == wifi_status) ||
		(BT_8812A_1ANT_WIFI_STATUS_CONNECTED_SPECIAL_PKT == wifi_status) )
	{
		if( coex_dm->cur_ps_tdma != 1 &&
			coex_dm->cur_ps_tdma != 2 &&
			coex_dm->cur_ps_tdma != 3 &&
			coex_dm->cur_ps_tdma != 9 )
		{
			halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 9);
			coex_dm->ps_tdma_du_adj_type = 9;

			up = 0;
			dn = 0;
			m = 1;
			n= 3;
			result = 0;
			wait_count = 0;
		}		
		return;
	}
	
	if(coex_dm->reset_tdma_adjust)
	{
		coex_dm->reset_tdma_adjust = false;
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_DETAIL, ("[BTCoex], first run TdmaDurationAdjust()!!\n"));

		halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 2);
		coex_dm->ps_tdma_du_adj_type = 2;
		//============
		up = 0;
		dn = 0;
		m = 1;
		n= 3;
		result = 0;
		wait_count = 0;
	}
	else
	{
		//accquire the BT TRx retry count from BT_Info byte2
		retry_count = coex_sta->bt_retry_cnt;
		bt_info_ext = coex_sta->bt_info_ext;
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_DETAIL, "[BTCoex], retry_count = %d\n", retry_count);
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_DETAIL, "[BTCoex], up=%d, dn=%d, m=%d, n=%d, wait_count=%d\n", 
			up, dn, m, n, wait_count);
		result = 0;
		wait_count++; 
		  
		if(retry_count == 0)  // no retry in the last 2-second duration
		{
			up++;
			dn--;

			if (dn <= 0)
				dn = 0;				 

			if(up >= n)	// if \B3s\C4\F2 n \AD\D32\AC\ED retry count\AC\B00, \ABh\BDռeWiFi duration
			{
				wait_count = 0; 
				n = 3;
				up = 0;
				dn = 0;
				result = 1; 
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_DETAIL, ("[BTCoex], Increase wifi duration!!\n"));
			}
		}
		else if (retry_count <= 3)	// <=3 retry in the last 2-second duration
		{
			up--; 
			dn++;

			if (up <= 0)
				up = 0;

			if (dn == 2)	// if \B3s\C4\F2 2 \AD\D32\AC\ED retry count< 3, \ABh\BDկ\B6WiFi duration
			{
				if (wait_count <= 2)
					m++; // \C1קK\A4@\AA\BD\A6b\A8\E2\AD\D3level\A4\A4\A8Ӧ^
				else
					m = 1;

				if ( m >= 20) //m \B3̤j\AD\C8 = 20 ' \B3̤j120\AC\ED recheck\ACO\A7_\BDվ\E3 WiFi duration.
					m = 20;

				n = 3*m;
				up = 0;
				dn = 0;
				wait_count = 0;
				result = -1; 
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_DETAIL, ("[BTCoex], Decrease wifi duration for retryCounter<3!!\n"));
			}
		}
		else  //retry count > 3, \A5u\ADn1\A6\B8 retry count > 3, \ABh\BDկ\B6WiFi duration
		{
			if (wait_count == 1)
				m++; // \C1קK\A4@\AA\BD\A6b\A8\E2\AD\D3level\A4\A4\A8Ӧ^
			else
				m = 1;

			if ( m >= 20) //m \B3̤j\AD\C8 = 20 ' \B3̤j120\AC\ED recheck\ACO\A7_\BDվ\E3 WiFi duration.
				m = 20;

			n = 3*m;
			up = 0;
			dn = 0;
			wait_count = 0; 
			result = -1;
			BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_DETAIL, ("[BTCoex], Decrease wifi duration for retryCounter>3!!\n"));
		}

		if(result == -1)
		{
			if( (BT_INFO_8812A_1ANT_A2DP_BASIC_RATE(bt_info_ext)) &&
				((coex_dm->cur_ps_tdma == 1) ||(coex_dm->cur_ps_tdma == 2)) )
			{
				halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 9);
				coex_dm->ps_tdma_du_adj_type = 9;
			}
			else if(coex_dm->cur_ps_tdma == 1)
			{
				halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 2);
				coex_dm->ps_tdma_du_adj_type = 2;
			}
			else if(coex_dm->cur_ps_tdma == 2)
			{
				halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 9);
				coex_dm->ps_tdma_du_adj_type = 9;
			}
			else if(coex_dm->cur_ps_tdma == 9)
			{
				halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 11);
				coex_dm->ps_tdma_du_adj_type = 11;
			}
		}
		else if(result == 1)
		{
			if( (BT_INFO_8812A_1ANT_A2DP_BASIC_RATE(bt_info_ext)) &&
				((coex_dm->cur_ps_tdma == 1) ||(coex_dm->cur_ps_tdma == 2)) )
			{
				halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 9);
				coex_dm->ps_tdma_du_adj_type = 9;
			}
			else if(coex_dm->cur_ps_tdma == 11)
			{
				halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 9);
				coex_dm->ps_tdma_du_adj_type = 9;
			}
			else if(coex_dm->cur_ps_tdma == 9)
			{
				halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 2);
				coex_dm->ps_tdma_du_adj_type = 2;
			}
			else if(coex_dm->cur_ps_tdma == 2)
			{
				halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 1);
				coex_dm->ps_tdma_du_adj_type = 1;
			}
		}

		if( coex_dm->cur_ps_tdma != 1 &&
			coex_dm->cur_ps_tdma != 2 &&
			coex_dm->cur_ps_tdma != 9 &&
			coex_dm->cur_ps_tdma != 11 )
		{
			// recover to previous adjust type
			halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, coex_dm->ps_tdma_du_adj_type);
		}
	}
}

u8
halbtc8812a1ant_PsTdmaTypeByWifiRssi(
		s32	wifi_rssi,
		s32	pre_wifi_rssi,
		u8	wifi_rssi_thresh
	)
{
	u8	ps_tdma_type=0;
	
	if(wifi_rssi > pre_wifi_rssi)
	{
		if(wifi_rssi > (wifi_rssi_thresh+5))
		{
			ps_tdma_type = 26;
		}
		else
		{
			ps_tdma_type = 25;
		}
	}
	else
	{
		if(wifi_rssi > wifi_rssi_thresh)
		{
			ps_tdma_type = 26;
		}
		else
		{
			ps_tdma_type = 25;
		}
	}

	return ps_tdma_type;
}

void
halbtc8812a1ant_ps_tdma_check_for_power_save_state(
		struct btc_coexist	*		btcoexist,
		bool			new_ps_state
	)
{
	u8	lps_mode=0x0;

	btcoexist->btc_get(btcoexist, BTC_GET_U1_LPS_MODE, &lps_mode);
	
	if(lps_mode)	// already under LPS state
	{
		if(new_ps_state)		
		{
			// keep state under LPS, do nothing.
		}
		else
		{
			// will leave LPS state, turn off psTdma first
			halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, false, 0);
		}
	}
	else						// NO PS state
	{
		if(new_ps_state)
		{
			// will enter LPS state, turn off psTdma first
			halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, false, 0);
		}
		else
		{
			// keep state under NO PS state, do nothing.
		}
	}
}

void
halbtc8812a1ant_power_save_state(
		struct btc_coexist	*		btcoexist,
		u8				ps_type,
		u8				lps_val,
		u8				rpwm_val
	)
{
	bool		low_pwr_disable=false;

	switch(ps_type)
	{
		case BTC_PS_WIFI_NATIVE:
			// recover to original 32k low power setting
			low_pwr_disable = false;
			btcoexist->btc_set(btcoexist, BTC_SET_ACT_DISABLE_LOW_POWER, &low_pwr_disable);
			btcoexist->btc_set(btcoexist, BTC_SET_ACT_NORMAL_LPS, NULL);
			break;
		case BTC_PS_LPS_ON:
			halbtc8812a1ant_ps_tdma_check_for_power_save_state(btcoexist, true);
			halbtc8812a1ant_lps_rpwm(btcoexist, NORMAL_EXEC, lps_val, rpwm_val);
			// when coex force to enter LPS, do not enter 32k low power.
			low_pwr_disable = true;
			btcoexist->btc_set(btcoexist, BTC_SET_ACT_DISABLE_LOW_POWER, &low_pwr_disable);
			// power save must executed before psTdma.
			btcoexist->btc_set(btcoexist, BTC_SET_ACT_ENTER_LPS, NULL);
			break;
		case BTC_PS_LPS_OFF:
			halbtc8812a1ant_ps_tdma_check_for_power_save_state(btcoexist, false);
			btcoexist->btc_set(btcoexist, BTC_SET_ACT_LEAVE_LPS, NULL);
			break;
		default:
			break;
	}
}


void
halbtc8812a1ant_coex_under_5g(
		struct btc_coexist	*		btcoexist
	)
{
	halbtc8812a1ant_power_save_state(btcoexist, BTC_PS_WIFI_NATIVE, 0x0, 0x0);
	
	halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, false, 10);

	halbtc8812a1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 0);
}

void
halbtc8812a1ant_action_wifi_only(
		struct btc_coexist	*		btcoexist
	)
{
	halbtc8812a1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 0);
	halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, false, 9);	
}

void
halbtc8812a1ant_monitor_bt_enable_disable(
	 	struct btc_coexist	*		btcoexist
	)
{
	struct btc_stack_info*	stack_info=&btcoexist->stack_info;
	static bool	pre_bt_disabled=false;
	static u32	bt_disable_cnt=0;
	bool			bt_active=true, bt_disabled=false;

	// only 8812a need to consider if core stack is installed.
	if(!stack_info->hci_version)
	{
		bt_active = false;
	}

	// This function check if bt is disabled

	if(	coex_sta->high_priority_tx == 0 &&
		coex_sta->high_priority_rx == 0 &&
		coex_sta->low_priority_tx == 0 &&
		coex_sta->low_priority_rx == 0)
	{
		bt_active = false;
	}
	if(	coex_sta->high_priority_tx == 0xffff &&
		coex_sta->high_priority_rx == 0xffff &&
		coex_sta->low_priority_tx == 0xffff &&
		coex_sta->low_priority_rx == 0xffff)
	{
		bt_active = false;
	}
	if(bt_active)
	{
		bt_disable_cnt = 0;
		bt_disabled = false;
		btcoexist->btc_set(btcoexist, BTC_SET_BL_BT_DISABLE, &bt_disabled);
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_MONITOR, ("[BTCoex], BT is enabled !!\n"));
	}
	else
	{
		bt_disable_cnt++;
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_MONITOR, "[BTCoex], bt all counters=0, %d times!!\n", 
				bt_disable_cnt);
		if(bt_disable_cnt >= 2)
		{
			bt_disabled = true;
			btcoexist->btc_set(btcoexist, BTC_SET_BL_BT_DISABLE, &bt_disabled);
			BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_MONITOR, ("[BTCoex], BT is disabled !!\n"));
			halbtc8812a1ant_action_wifi_only(btcoexist);
		}
	}
	if(pre_bt_disabled != bt_disabled)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_MONITOR, "[BTCoex], BT is from %s to %s!!\n", 
			(pre_bt_disabled ? "disabled":"enabled"), 
			(bt_disabled ? "disabled":"enabled"));
		pre_bt_disabled = bt_disabled;
		if(!bt_disabled)
		{
		}
		else
		{
			btcoexist->btc_set(btcoexist, BTC_SET_ACT_LEAVE_LPS, NULL);
			btcoexist->btc_set(btcoexist, BTC_SET_ACT_NORMAL_LPS, NULL);
		}
	}
}

//=============================================
//
//	Software Coex Mechanism start
//
//=============================================

// SCO only or SCO+PAN(HS)
void
halbtc8812a1ant_action_sco(
		struct btc_coexist	*		btcoexist
	)
{
}

void
halbtc8812a1ant_action_hid(
		struct btc_coexist	*		btcoexist
	)
{
}

//A2DP only / PAN(EDR) only/ A2DP+PAN(HS)
void
halbtc8812a1ant_action_a2dp(
		struct btc_coexist	*		btcoexist
	)
{
}

void
halbtc8812a1ant_action_a2dp_pan_hs(
		struct btc_coexist	*		btcoexist
	)
{
}

void
halbtc8812a1ant_action_pan_edr(
		struct btc_coexist	*		btcoexist
	)
{
}

//PAN(HS) only
void
halbtc8812a1ant_action_pan_hs(
		struct btc_coexist	*		btcoexist
	)
{
}

//PAN(EDR)+A2DP
void
halbtc8812a1ant_action_pan_edr_a2dp(
		struct btc_coexist	*		btcoexist
	)
{
}

void
halbtc8812a1ant_action_pan_edr_hid(
		struct btc_coexist	*		btcoexist
	)
{
}

// HID+A2DP+PAN(EDR)
void
halbtc8812a1ant_action_hid_a2dp_pan_edr(
		struct btc_coexist	*		btcoexist
	)
{
}

void
halbtc8812a1ant_action_hid_a2dp(
		struct btc_coexist	*		btcoexist
	)
{
}

//=============================================
//
//	Non-Software Coex Mechanism start
//
//=============================================

void
halbtc8812a1ant_action_hs(
		struct btc_coexist	*		btcoexist
	)
{
	bool 		hs_connecting=false;

	btcoexist->btc_get(btcoexist, BTC_GET_BL_HS_CONNECTING, &hs_connecting);

	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, "[BTCoex], Action for HS, hs_connecting=%d!!!\n", hs_connecting);
	halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, false, 8);

	if(hs_connecting)
	{
		halbtc8812a1ant_coex_table_with_type(btcoexist, FORCE_EXEC, 3);
	}
	else
	{
		if((coex_sta->high_priority_tx+coex_sta->high_priority_rx+
			coex_sta->low_priority_tx+coex_sta->low_priority_rx)<=1200)
			halbtc8812a1ant_coex_table_with_type(btcoexist, FORCE_EXEC, 3);
		else
			halbtc8812a1ant_coex_table_with_type(btcoexist, FORCE_EXEC, 4);	
	}
}

void
halbtc8812a1ant_action_bt_inquiry(
		struct btc_coexist	*		btcoexist
	)
{
	struct btc_bt_link_info* bt_link_info=&btcoexist->bt_link_info;
	bool			wifi_connected=false;
	
	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_CONNECTED, &wifi_connected);

	if(!wifi_connected)
	{
		halbtc8812a1ant_power_save_state(btcoexist, BTC_PS_WIFI_NATIVE, 0x0, 0x0);
		halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, false, 8);
		halbtc8812a1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 0);
	}
	else if( (bt_link_info->sco_exist) ||
			(bt_link_info->hid_only) )
	{
		// SCO/HID-only busy
		halbtc8812a1ant_power_save_state(btcoexist, BTC_PS_WIFI_NATIVE, 0x0, 0x0);
		halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 32);
		halbtc8812a1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 1);
	}
	else
	{
		halbtc8812a1ant_power_save_state(btcoexist, BTC_PS_LPS_ON, 0x50, 0x0);
		halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 30);
		halbtc8812a1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 0);
	}
}

void
halbtc8812a1ant_action_bt_sco_hid_only_busy(
		struct btc_coexist	*		btcoexist,
		u8				wifi_status
	)
{
	// tdma and coex table
	halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, false, 8);

	if(BT_8812A_1ANT_WIFI_STATUS_NON_CONNECTED_ASSO_AUTH_SCAN == wifi_status)
		halbtc8812a1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 0);		
	else
		halbtc8812a1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 2);
}

void
halbtc8812a1ant_action_wifi_connected_bt_acl_busy(
		struct btc_coexist	*		btcoexist,
		u8				wifi_status
	)
{
	struct btc_bt_link_info* bt_link_info=&btcoexist->bt_link_info;

	if(bt_link_info->hid_only)
	{
		halbtc8812a1ant_action_bt_sco_hid_only_busy(btcoexist, wifi_status);
		coex_dm->reset_tdma_adjust = true;
		return;
	}
	else if( (bt_link_info->a2dp_only) ||
			 (bt_link_info->hid_exist&&bt_link_info->a2dp_exist) )
	{
		halbtc8812a1ant_tdma_duration_adjust_for_acl(btcoexist, wifi_status);
	}
	else if( (bt_link_info->pan_only) ||
			(bt_link_info->hid_exist&&bt_link_info->pan_exist) )
	{
		halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 3);
		coex_dm->reset_tdma_adjust = true;
	}
	else
	{
		if( (BT_8812A_1ANT_WIFI_STATUS_NON_CONNECTED_ASSO_AUTH_SCAN == wifi_status) ||
			(BT_8812A_1ANT_WIFI_STATUS_CONNECTED_SCAN == wifi_status) ||
			(BT_8812A_1ANT_WIFI_STATUS_CONNECTED_SPECIAL_PKT == wifi_status) )
			halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 9);
		else
			halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 11);
		coex_dm->reset_tdma_adjust = true;
	}
		
	halbtc8812a1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 1);
}

void
halbtc8812a1ant_action_wifi_not_connected(
		struct btc_coexist	*		btcoexist
	)
{
	// power save state
	halbtc8812a1ant_power_save_state(btcoexist, BTC_PS_WIFI_NATIVE, 0x0, 0x0);

	// tdma and coex table	
	halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, false, 8);
	halbtc8812a1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 0);
}

void
halbtc8812a1ant_action_wifi_not_connected_asso_auth_scan(
		struct btc_coexist	*		btcoexist
	)
{
	halbtc8812a1ant_power_save_state(btcoexist, BTC_PS_WIFI_NATIVE, 0x0, 0x0);

	// tdma and coex table
	halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, false, 8);
	halbtc8812a1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 2);
}

void
halbtc8812a1ant_action_wifi_connected_scan(
		struct btc_coexist	*		btcoexist
	)
{
	// power save state
	if(BT_8812A_1ANT_BT_STATUS_ACL_BUSY == coex_dm->bt_status && !btcoexist->bt_link_info.hid_only)
		halbtc8812a1ant_power_save_state(btcoexist, BTC_PS_LPS_ON, 0x50, 0x0);
	else
		halbtc8812a1ant_power_save_state(btcoexist, BTC_PS_WIFI_NATIVE, 0x0, 0x0);

	// tdma and coex table
	if(BT_8812A_1ANT_BT_STATUS_ACL_BUSY == coex_dm->bt_status)
	{
		halbtc8812a1ant_action_wifi_connected_bt_acl_busy(btcoexist,
			BT_8812A_1ANT_WIFI_STATUS_CONNECTED_SCAN);
	}
	else if( (BT_8812A_1ANT_BT_STATUS_SCO_BUSY == coex_dm->bt_status) ||
			(BT_8812A_1ANT_BT_STATUS_ACL_SCO_BUSY == coex_dm->bt_status) )
	{
		halbtc8812a1ant_action_bt_sco_hid_only_busy(btcoexist,
			BT_8812A_1ANT_WIFI_STATUS_CONNECTED_SCAN);
	}
	else
	{
		halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, false, 8);
		halbtc8812a1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 2);
	}
}

void
halbtc8812a1ant_action_wifi_connected_special_packet(
		struct btc_coexist	*		btcoexist
	)
{
	// power save state
	if(BT_8812A_1ANT_BT_STATUS_ACL_BUSY == coex_dm->bt_status && !btcoexist->bt_link_info.hid_only)
		halbtc8812a1ant_power_save_state(btcoexist, BTC_PS_LPS_ON, 0x50, 0x0);
	else
		halbtc8812a1ant_power_save_state(btcoexist, BTC_PS_WIFI_NATIVE, 0x0, 0x0);

	// tdma and coex table
	if(BT_8812A_1ANT_BT_STATUS_ACL_BUSY == coex_dm->bt_status)
	{
		halbtc8812a1ant_action_wifi_connected_bt_acl_busy(btcoexist,
			BT_8812A_1ANT_WIFI_STATUS_CONNECTED_SPECIAL_PKT);
	}
	else
	{
		halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, false, 8);
		halbtc8812a1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 2);
	}
}

void
halbtc8812a1ant_action_wifi_connected(
		struct btc_coexist	*		btcoexist
	)
{
	bool 	wifi_connected=false, wifi_busy=false;
	bool 	scan=false, link=false, roam=false;
	bool 	under_4way=false;
	//u32		wifi_bw;

	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], CoexForWifiConnect()===>\n"));

	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_CONNECTED, &wifi_connected);
	if(!wifi_connected)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], CoexForWifiConnect(), return for wifi not connected<===\n"));
		return;
	}

	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_4_WAY_PROGRESS, &under_4way);
	if(under_4way)
	{
		halbtc8812a1ant_action_wifi_connected_special_packet(btcoexist);
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], CoexForWifiConnect(), return for wifi is under 4way<===\n"));
		return;
	}
	
	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_SCAN, &scan);
	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_LINK, &link);
	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_ROAM, &roam);
	if(scan || link || roam)
	{
		halbtc8812a1ant_action_wifi_connected_scan(btcoexist);
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], CoexForWifiConnect(), return for wifi is under scan<===\n"));
		return;
	}

	// power save state
	if(BT_8812A_1ANT_BT_STATUS_ACL_BUSY == coex_dm->bt_status && !btcoexist->bt_link_info.hid_only)
		halbtc8812a1ant_power_save_state(btcoexist, BTC_PS_LPS_ON, 0x50, 0x0);
	else
		halbtc8812a1ant_power_save_state(btcoexist, BTC_PS_WIFI_NATIVE, 0x0, 0x0);

	// tdma and coex table
	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_BUSY, &wifi_busy);	
	if(!wifi_busy)
	{
		if(BT_8812A_1ANT_BT_STATUS_ACL_BUSY == coex_dm->bt_status)
		{
			halbtc8812a1ant_action_wifi_connected_bt_acl_busy(btcoexist, 
				BT_8812A_1ANT_WIFI_STATUS_CONNECTED_IDLE);
		}
		else if( (BT_8812A_1ANT_BT_STATUS_SCO_BUSY == coex_dm->bt_status) ||
			(BT_8812A_1ANT_BT_STATUS_ACL_SCO_BUSY == coex_dm->bt_status) )
		{
			halbtc8812a1ant_action_bt_sco_hid_only_busy(btcoexist,
				BT_8812A_1ANT_WIFI_STATUS_CONNECTED_IDLE);
		}
		else
		{
			halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, false, 8);
			halbtc8812a1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 2);
		}
	}
	else
	{
		if(BT_8812A_1ANT_BT_STATUS_NON_CONNECTED_IDLE == coex_dm->bt_status)
		{
			halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 5);
			halbtc8812a1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 2);
		}
		else if(BT_8812A_1ANT_BT_STATUS_CONNECTED_IDLE == coex_dm->bt_status)
		{			
			halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, true, 5);
			halbtc8812a1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 2);
		}
		else if(BT_8812A_1ANT_BT_STATUS_ACL_BUSY == coex_dm->bt_status)
		{
			halbtc8812a1ant_action_wifi_connected_bt_acl_busy(btcoexist,
				BT_8812A_1ANT_WIFI_STATUS_CONNECTED_BUSY);
		}
		else if( (BT_8812A_1ANT_BT_STATUS_SCO_BUSY == coex_dm->bt_status) ||
			(BT_8812A_1ANT_BT_STATUS_ACL_SCO_BUSY == coex_dm->bt_status) )
		{
			halbtc8812a1ant_action_bt_sco_hid_only_busy(btcoexist,
				BT_8812A_1ANT_WIFI_STATUS_CONNECTED_BUSY);
		}
		else 
		{
			halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, false, 8);
			halbtc8812a1ant_coex_table_with_type(btcoexist, NORMAL_EXEC, 2);
		}
	}
}

void
halbtc8812a1ant_run_sw_coexist_mechanism(
		struct btc_coexist	*		btcoexist
	)
{
	//bool				wifi_under_5g=false, wifi_busy=false, wifi_connected=false;
	//u8				bt_info_original=0, bt_retry_cnt=0;
	u8				algorithm=0;

	return;

	algorithm = halbtc8812a1ant_action_algorithm(btcoexist);
	coex_dm->cur_algorithm = algorithm;		

	if(halbtc8812a1ant_is_common_action(btcoexist))
	{
	}
	else
	{
		switch(coex_dm->cur_algorithm)
		{
			case BT_8812A_1ANT_COEX_ALGO_SCO:
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Action algorithm = SCO.\n"));
				halbtc8812a1ant_action_sco(btcoexist);
				break;
			case BT_8812A_1ANT_COEX_ALGO_HID:
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Action algorithm = HID.\n"));
				halbtc8812a1ant_action_hid(btcoexist);
				break;
			case BT_8812A_1ANT_COEX_ALGO_A2DP:
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Action algorithm = A2DP.\n"));
				halbtc8812a1ant_action_a2dp(btcoexist);
				break;
			case BT_8812A_1ANT_COEX_ALGO_A2DP_PANHS:
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Action algorithm = A2DP+PAN(HS).\n"));
				halbtc8812a1ant_action_a2dp_pan_hs(btcoexist);
				break;
			case BT_8812A_1ANT_COEX_ALGO_PANEDR:
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Action algorithm = PAN(EDR).\n"));
				halbtc8812a1ant_action_pan_edr(btcoexist);
				break;
			case BT_8812A_1ANT_COEX_ALGO_PANHS:
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Action algorithm = HS mode.\n"));
				halbtc8812a1ant_action_pan_hs(btcoexist);
				break;
			case BT_8812A_1ANT_COEX_ALGO_PANEDR_A2DP:
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Action algorithm = PAN+A2DP.\n"));
				halbtc8812a1ant_action_pan_edr_a2dp(btcoexist);
				break;
			case BT_8812A_1ANT_COEX_ALGO_PANEDR_HID:
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Action algorithm = PAN(EDR)+HID.\n"));
				halbtc8812a1ant_action_pan_edr_hid(btcoexist);
				break;
			case BT_8812A_1ANT_COEX_ALGO_HID_A2DP_PANEDR:
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Action algorithm = HID+A2DP+PAN.\n"));
				halbtc8812a1ant_action_hid_a2dp_pan_edr(btcoexist);
				break;
			case BT_8812A_1ANT_COEX_ALGO_HID_A2DP:
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Action algorithm = HID+A2DP.\n"));
				halbtc8812a1ant_action_hid_a2dp(btcoexist);
				break;
			default:
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Action algorithm = coexist All Off!!\n"));
				halbtc8812a1ant_coex_all_off(btcoexist);
				break;
		}
		coex_dm->pre_algorithm = coex_dm->cur_algorithm;
	}
}

void
halbtc8812a1ant_RunCoexistMechanism(
		struct btc_coexist	*		btcoexist
	)
{
	bool	wifi_under_5g=false, /*wifi_busy=false,*/ wifi_connected=false, bt_hs_on=false;

	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], RunCoexistMechanism()===>\n"));

	if(btcoexist->manual_control)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], RunCoexistMechanism(), return for Manual CTRL <===\n"));
		return;
	}
	
	if(btcoexist->stop_coex_dm)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], RunCoexistMechanism(), return for Stop Coex DM <===\n"));
		return;
	}

	if(coex_sta->under_ips)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], wifi is under IPS !!!\n"));
		return;
	}

	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_UNDER_5G, &wifi_under_5g);
	if(wifi_under_5g)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], RunCoexistMechanism(), run 5G coex setting!!<===\n"));
		halbtc8812a1ant_coex_under_5g(btcoexist);
		return;
	}

	halbtc8812a1ant_run_sw_coexist_mechanism(btcoexist);

	btcoexist->btc_get(btcoexist, BTC_GET_BL_HS_OPERATION, &bt_hs_on);
	if(coex_sta->c2h_bt_inquiry_page)
	{
		halbtc8812a1ant_action_bt_inquiry(btcoexist);
		return;
	}
	else if(bt_hs_on)
	{
		halbtc8812a1ant_action_hs(btcoexist);
		return;
	}

	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_CONNECTED, &wifi_connected);
	if(!wifi_connected)
	{
		bool	scan=false, link=false, roam=false;
		
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], wifi is non connected-idle !!!\n"));

		btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_SCAN, &scan);
		btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_LINK, &link);
		btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_ROAM, &roam);

		if(scan || link || roam)
			halbtc8812a1ant_action_wifi_not_connected_asso_auth_scan(btcoexist);
		else
			halbtc8812a1ant_action_wifi_not_connected(btcoexist);
	}
	else	// wifi LPS/Busy
	{
		halbtc8812a1ant_action_wifi_connected(btcoexist);
	}
}

void
halbtc8812a1ant_init_coex_dm(
		struct btc_coexist	*		btcoexist
	)
{	
	// force to reset coex mechanism
	halbtc8812a1ant_ps_tdma(btcoexist, FORCE_EXEC, false, 8);
	halbtc8812a1ant_coex_table_with_type(btcoexist, FORCE_EXEC, 0);
}

//============================================================
// work around function start with wa_halbtc8812a1ant_
//============================================================
//============================================================
// extern function start with EXhalbtc8812a1ant_
//============================================================
void
ex_halbtc8812a1ant_poweron_setting(
		struct btc_coexist	*		btcoexist
	)
{
}

void
ex_halbtc8812a1ant_init_hwconfig(struct btc_coexist* btcoexist)
{
	//u32	u4_tmp=0;
	//u16	u2_tmp=0;
	u8	u1_tmp=0;

	BTC_PRINT(BTC_MSG_INTERFACE, INTF_INIT, ("[BTCoex], 1Ant Init HW Config!!\n"));
	
	//ant sw control to BT
	halbtc8812a1ant_set_ant_path(btcoexist, BTC_ANT_PATH_BT, true, false);

	// 0x790[5:0]=0x5
	u1_tmp = btcoexist->btc_read_1byte(btcoexist, 0x790);
	u1_tmp &= 0xc0;
	u1_tmp |= 0x5;
	btcoexist->btc_write_1byte(btcoexist, 0x790, u1_tmp);

	// PTA parameter
	btcoexist->btc_write_1byte(btcoexist, 0x6cc, 0x0);
	btcoexist->btc_write_4byte(btcoexist, 0x6c8, 0xffff);
	btcoexist->btc_write_4byte(btcoexist, 0x6c4, 0x55555555);
	btcoexist->btc_write_4byte(btcoexist, 0x6c0, 0x55555555);

	// coex parameters
	btcoexist->btc_write_1byte(btcoexist, 0x778, 0x1);

	// enable counter statistics
	btcoexist->btc_write_1byte(btcoexist, 0x76e, 0x4);

	// enable PTA
	btcoexist->btc_write_1byte(btcoexist, 0x40, 0x20);

	// bt clock related
	u1_tmp = btcoexist->btc_read_1byte(btcoexist, 0x4);
	u1_tmp |= BIT7;
	btcoexist->btc_write_1byte(btcoexist, 0x4, u1_tmp);

	// bt clock related
	u1_tmp = btcoexist->btc_read_1byte(btcoexist, 0x7);
	u1_tmp |= BIT1;
	btcoexist->btc_write_1byte(btcoexist, 0x7, u1_tmp);
}

void
ex_halbtc8812a1ant_init_coex_dm(
		struct btc_coexist	*		btcoexist
	)
{
	BTC_PRINT(BTC_MSG_INTERFACE, INTF_INIT, ("[BTCoex], Coex Mechanism Init!!\n"));

	btcoexist->stop_coex_dm = false;

	halbtc8812a1ant_init_coex_dm(btcoexist);
}

void
ex_halbtc8812a1ant_display_coex_info(
		struct btc_coexist	*		btcoexist
	)
{
	struct btc_board_info*		board_info=&btcoexist->board_info;
	struct btc_stack_info*		stack_info=&btcoexist->stack_info;
	struct btc_bt_link_info*	bt_link_info=&btcoexist->bt_link_info;
	u8 *				cli_buf=btcoexist->cli_buf;
	u8				u1_tmp[4], i, bt_info_ext, ps_tdma_case=0;
	u32				u4_tmp[4];
	bool				roam=false, scan=false, link=false, wifi_under_5g=false;
	bool				bt_hs_on=false, wifi_busy=false;
	s32				wifi_rssi=0, bt_hs_rssi=0;
	u32				wifi_bw, wifi_traffic_dir;
	u8				wifi_dot11_chnl, wifi_hs_chnl;
	u32				fw_ver=0, bt_patch_ver=0;

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n ============[BT Coexist info]============");
	CL_PRINTF(cli_buf);

	if(btcoexist->manual_control)
	{
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n ============[Under Manual Control]============");
		CL_PRINTF(cli_buf);
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n ==========================================");
		CL_PRINTF(cli_buf);
	}
	if(btcoexist->stop_coex_dm)
	{
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n ============[Coex is STOPPED]============");
		CL_PRINTF(cli_buf);
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n ==========================================");
		CL_PRINTF(cli_buf);
	}

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d ", "Ant PG number/ Ant mechanism:", \
		board_info->pg_ant_num, board_info->btdm_ant_num);
	CL_PRINTF(cli_buf);	
	
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %s / %d", "BT stack/ hci ext ver", \
		((stack_info->profile_notified)? "Yes":"No"), stack_info->hci_version);
	CL_PRINTF(cli_buf);

	btcoexist->btc_get(btcoexist, BTC_GET_U4_BT_PATCH_VER, &bt_patch_ver);
	btcoexist->btc_get(btcoexist, BTC_GET_U4_WIFI_FW_VER, &fw_ver);
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d_%d/ 0x%x/ 0x%x(%d)", "CoexVer/ FwVer/ PatchVer", \
		glcoex_ver_date_8812a_1ant, glcoex_ver_8812a_1ant, fw_ver, bt_patch_ver, bt_patch_ver);
	CL_PRINTF(cli_buf);

	btcoexist->btc_get(btcoexist, BTC_GET_BL_HS_OPERATION, &bt_hs_on);
	btcoexist->btc_get(btcoexist, BTC_GET_U1_WIFI_DOT11_CHNL, &wifi_dot11_chnl);
	btcoexist->btc_get(btcoexist, BTC_GET_U1_WIFI_HS_CHNL, &wifi_hs_chnl);
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d / %d(%d)", "Dot11 channel / HsChnl(HsMode)", \
		wifi_dot11_chnl, wifi_hs_chnl, bt_hs_on);
	CL_PRINTF(cli_buf);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %02x %02x %02x ", "H2C Wifi inform bt chnl Info", \
		coex_dm->wifi_chnl_info[0], coex_dm->wifi_chnl_info[1],
		coex_dm->wifi_chnl_info[2]);
	CL_PRINTF(cli_buf);

	btcoexist->btc_get(btcoexist, BTC_GET_S4_WIFI_RSSI, &wifi_rssi);
	btcoexist->btc_get(btcoexist, BTC_GET_S4_HS_RSSI, &bt_hs_rssi);
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d", "Wifi rssi/ HS rssi", \
		wifi_rssi, bt_hs_rssi);
	CL_PRINTF(cli_buf);

	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_SCAN, &scan);
	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_LINK, &link);
	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_ROAM, &roam);
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d/ %d ", "Wifi link/ roam/ scan", \
		link, roam, scan);
	CL_PRINTF(cli_buf);

	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_UNDER_5G, &wifi_under_5g);
	btcoexist->btc_get(btcoexist, BTC_GET_U4_WIFI_BW, &wifi_bw);
	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_BUSY, &wifi_busy);
	btcoexist->btc_get(btcoexist, BTC_GET_U4_WIFI_TRAFFIC_DIRECTION, &wifi_traffic_dir);
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %s / %s/ %s ", "Wifi status", \
		(wifi_under_5g? "5G":"2.4G"),
		((BTC_WIFI_BW_LEGACY==wifi_bw)? "Legacy": (((BTC_WIFI_BW_HT40==wifi_bw)? "HT40":"HT20"))),
		((!wifi_busy)? "idle": ((BTC_WIFI_TRAFFIC_TX==wifi_traffic_dir)? "uplink":"downlink")));
	CL_PRINTF(cli_buf);
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = [%s/ %d/ %d] ", "BT [status/ rssi/ retryCnt]", \
		((btcoexist->bt_info.bt_disabled)? ("disabled"):	((coex_sta->c2h_bt_inquiry_page)?("inquiry/page scan"):((BT_8812A_1ANT_BT_STATUS_NON_CONNECTED_IDLE == coex_dm->bt_status)? "non-connected idle":
		(  (BT_8812A_1ANT_BT_STATUS_CONNECTED_IDLE == coex_dm->bt_status)? "connected-idle":"busy")))),
		coex_sta->bt_rssi, coex_sta->bt_retry_cnt);
	CL_PRINTF(cli_buf);
					
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d / %d / %d / %d", "SCO/HID/PAN/A2DP", \
		bt_link_info->sco_exist, bt_link_info->hid_exist, bt_link_info->pan_exist, bt_link_info->a2dp_exist);
	CL_PRINTF(cli_buf);	
	//btcoexist->fBtcDispDbgMsg(btcoexist, BTC_DBG_DISP_BT_LINK_INFO);

	bt_info_ext = coex_sta->bt_info_ext;
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %s", "BT Info A2DP rate", \
		(bt_info_ext&BIT0)? "Basic rate":"EDR rate");
	CL_PRINTF(cli_buf);	

	for(i=0; i<BT_INFO_SRC_8812A_1ANT_MAX; i++)
	{
		if(coex_sta->bt_info_c2h_cnt[i])
		{				
			CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %02x %02x %02x %02x %02x %02x %02x(%d)", glbt_info_src_8812a_1ant[i], \
				coex_sta->bt_info_c2h[i][0], coex_sta->bt_info_c2h[i][1],
				coex_sta->bt_info_c2h[i][2], coex_sta->bt_info_c2h[i][3],
				coex_sta->bt_info_c2h[i][4], coex_sta->bt_info_c2h[i][5],
				coex_sta->bt_info_c2h[i][6], coex_sta->bt_info_c2h_cnt[i]);
			CL_PRINTF(cli_buf);
		}
	}
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %s/%s, (0x%x/0x%x)", "PS state, IPS/LPS, (lps/rpwm)", \
		((coex_sta->under_ips? "IPS ON":"IPS OFF")),
		((coex_sta->under_lps? "LPS ON":"LPS OFF")), 
		btcoexist->bt_info.lps_val, 
		btcoexist->bt_info.rpwm_val);
	CL_PRINTF(cli_buf);
	//btcoexist->fBtcDispDbgMsg(btcoexist, BTC_DBG_DISP_FW_PWR_MODE_CMD);

	if(!btcoexist->manual_control)
	{
		// Sw mechanism	
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s", "============[Sw mechanism]============");
		CL_PRINTF(cli_buf);
	
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %s/ %s/ %d ", "DelBA/ BtCtrlAgg/ AggSize", \
			(btcoexist->bt_info.reject_agg_pkt? "Yes":"No"), (btcoexist->bt_info.bt_ctrl_buf_size? "Yes":"No"),
				btcoexist->bt_info.agg_buf_size);
		CL_PRINTF(cli_buf);
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = 0x%x ", "Rate Mask", \
				btcoexist->bt_info.ra_mask);
		CL_PRINTF(cli_buf);

		// Fw mechanism		
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s", "============[Fw mechanism]============");
		CL_PRINTF(cli_buf);	

		ps_tdma_case = coex_dm->cur_ps_tdma;
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %02x %02x %02x %02x %02x case-%d", "PS TDMA", \
			coex_dm->ps_tdma_para[0], coex_dm->ps_tdma_para[1],
			coex_dm->ps_tdma_para[2], coex_dm->ps_tdma_para[3],
			coex_dm->ps_tdma_para[4], ps_tdma_case);
		CL_PRINTF(cli_buf);

		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = 0x%x ", "Latest error condition(should be 0)", \
			coex_dm->error_condition);
		CL_PRINTF(cli_buf);
		
		CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d ", "IgnWlanAct", \
			coex_dm->cur_ignore_wlan_act);
		CL_PRINTF(cli_buf);
	}

	// Hw setting		
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s", "============[Hw setting]============");
	CL_PRINTF(cli_buf);	

	u1_tmp[0] = btcoexist->btc_read_1byte(btcoexist, 0x778);
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = 0x%x", "0x778", \
		u1_tmp[0]);
	CL_PRINTF(cli_buf);
	
	u1_tmp[0] = btcoexist->btc_read_1byte(btcoexist, 0xcb3);
	u1_tmp[1] = btcoexist->btc_read_1byte(btcoexist, 0xcb7);
	u4_tmp[0] = btcoexist->btc_read_4byte(btcoexist, 0x900);
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = 0x%x/ 0x%x/ 0x%x", "0xcb3/0xcb7/0x900", \
		u1_tmp[0], u1_tmp[1], u4_tmp[0]);
	CL_PRINTF(cli_buf);

	u1_tmp[0] = btcoexist->btc_read_1byte(btcoexist, 0x40);
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = 0x%x", "0x40", \
		u1_tmp[0]);
	CL_PRINTF(cli_buf);

	u4_tmp[0] = btcoexist->btc_read_4byte(btcoexist, 0x550);
	u1_tmp[0] = btcoexist->btc_read_1byte(btcoexist, 0x522);
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = 0x%x/ 0x%x", "0x550(bcn ctrl)/0x522", \
		u4_tmp[0], u1_tmp[0]);
	CL_PRINTF(cli_buf);

	u4_tmp[0] = btcoexist->btc_read_4byte(btcoexist, 0xc50);
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = 0x%x", "0xc50(dig)", \
		u4_tmp[0]);
	CL_PRINTF(cli_buf);

	u4_tmp[0] = btcoexist->btc_read_4byte(btcoexist, 0x6c0);
	u4_tmp[1] = btcoexist->btc_read_4byte(btcoexist, 0x6c4);
	u4_tmp[2] = btcoexist->btc_read_4byte(btcoexist, 0x6c8);
	u1_tmp[0] = btcoexist->btc_read_1byte(btcoexist, 0x6cc);
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = 0x%x/ 0x%x/ 0x%x/ 0x%x", "0x6c0/0x6c4/0x6c8/0x6cc(coexTable)", \
		u4_tmp[0], u4_tmp[1], u4_tmp[2], u1_tmp[0]);
	CL_PRINTF(cli_buf);

	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d", "0x770(hp rx[31:16]/tx[15:0])", \
		coex_sta->high_priority_rx, coex_sta->high_priority_tx);
	CL_PRINTF(cli_buf);
	CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d", "0x774(lp rx[31:16]/tx[15:0])", \
		coex_sta->low_priority_rx, coex_sta->low_priority_tx);
	CL_PRINTF(cli_buf);
	
	//btcoexist->fBtcDispDbgMsg(btcoexist, BTC_DBG_DISP_COEX_STATISTICS);
}


void
ex_halbtc8812a1ant_ips_notify(
		struct btc_coexist	*		btcoexist,
		u8			type
	)
{
	//u32	u4_tmp=0;

	if(btcoexist->manual_control ||	btcoexist->stop_coex_dm)
		return;

	if(BTC_IPS_ENTER == type)
	{
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], IPS ENTER notify\n"));
		coex_sta->under_ips = true;		
		halbtc8812a1ant_coex_all_off(btcoexist);
		halbtc8812a1ant_set_ant_path(btcoexist, BTC_ANT_PATH_BT, false, true);
	}
	else if(BTC_IPS_LEAVE == type)
	{
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], IPS LEAVE notify\n"));
		coex_sta->under_ips = false;
	}
}

void
ex_halbtc8812a1ant_lps_notify(
		struct btc_coexist	*		btcoexist,
		u8			type
	)
{
	if(btcoexist->manual_control || btcoexist->stop_coex_dm)
		return;

	if(BTC_LPS_ENABLE == type)
	{
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], LPS ENABLE notify\n"));
		coex_sta->under_lps = true;
	}
	else if(BTC_LPS_DISABLE == type)
	{
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], LPS DISABLE notify\n"));
		coex_sta->under_lps = false;
	}
}

void
ex_halbtc8812a1ant_scan_notify(
		struct btc_coexist	*		btcoexist,
		u8			type
	)
{
	bool wifi_connected=false, bt_hs_on=false;	

	if(btcoexist->manual_control ||
		btcoexist->stop_coex_dm ||
		btcoexist->bt_info.bt_disabled )
		return;

	btcoexist->btc_get(btcoexist, BTC_GET_BL_HS_OPERATION, &bt_hs_on);
	if(coex_sta->c2h_bt_inquiry_page)
	{
		halbtc8812a1ant_action_bt_inquiry(btcoexist);
		return;
	}
	else if(bt_hs_on)
	{
		halbtc8812a1ant_action_hs(btcoexist);
		return;
	}

	btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_CONNECTED, &wifi_connected);
	if(BTC_SCAN_START == type)
	{	
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], SCAN START notify\n"));
		if(!wifi_connected)	// non-connected scan
		{
			halbtc8812a1ant_action_wifi_not_connected_asso_auth_scan(btcoexist);
		}
		else	// wifi is connected
		{
			halbtc8812a1ant_action_wifi_connected_scan(btcoexist);
		}
	}
	else if(BTC_SCAN_FINISH == type)
	{
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], SCAN FINISH notify\n"));
		if(!wifi_connected)	// non-connected scan
		{
			halbtc8812a1ant_action_wifi_not_connected(btcoexist);
		}
		else
		{
			halbtc8812a1ant_action_wifi_connected(btcoexist);
		}
	}
}

void
ex_halbtc8812a1ant_connect_notify(
		struct btc_coexist	*		btcoexist,
		u8			type
	)
{
	bool	wifi_connected=false, bt_hs_on=false;	

	if(btcoexist->manual_control ||
		btcoexist->stop_coex_dm ||
		btcoexist->bt_info.bt_disabled )
		return;

	btcoexist->btc_get(btcoexist, BTC_GET_BL_HS_OPERATION, &bt_hs_on);
	if(coex_sta->c2h_bt_inquiry_page)
	{
		halbtc8812a1ant_action_bt_inquiry(btcoexist);
		return;
	}
	else if(bt_hs_on)
	{
		halbtc8812a1ant_action_hs(btcoexist);
		return;
	}

	if(BTC_ASSOCIATE_START == type)
	{
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], CONNECT START notify\n"));
		halbtc8812a1ant_action_wifi_not_connected_asso_auth_scan(btcoexist);
	}
	else if(BTC_ASSOCIATE_FINISH == type)
	{
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], CONNECT FINISH notify\n"));
		
		btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_CONNECTED, &wifi_connected);
		if(!wifi_connected) // non-connected scan
		{
			halbtc8812a1ant_action_wifi_not_connected(btcoexist);
		}
		else
		{
			halbtc8812a1ant_action_wifi_connected(btcoexist);
		}
	}
}

void
ex_halbtc8812a1ant_media_status_notify(
		struct btc_coexist	*			btcoexist,
		u8				type
	)
{
	u8			data_len=5;
	u8			buf[6] = {0};
	u8			h2c_parameter[3] ={0};
	u32			wifi_bw;
	u8			wifi_central_chnl;

	if(btcoexist->manual_control ||
		btcoexist->stop_coex_dm ||
		btcoexist->bt_info.bt_disabled )
		return;

	if(BTC_MEDIA_CONNECT == type)
	{
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], MEDIA connect notify\n"));
	}
	else
	{
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], MEDIA disconnect notify\n"));
	}

	// only 2.4G we need to inform bt the chnl mask
	btcoexist->btc_get(btcoexist, BTC_GET_U1_WIFI_CENTRAL_CHNL, &wifi_central_chnl);
	if( (BTC_MEDIA_CONNECT == type) &&
		(wifi_central_chnl <= 14) )
	{
		h2c_parameter[0] = 0x1;
		h2c_parameter[1] = wifi_central_chnl;
		btcoexist->btc_get(btcoexist, BTC_GET_U4_WIFI_BW, &wifi_bw);
		if(BTC_WIFI_BW_HT40 == wifi_bw)
			h2c_parameter[2] = 0x30;
		else
			h2c_parameter[2] = 0x20;
	}
		
	coex_dm->wifi_chnl_info[0] = h2c_parameter[0];
	coex_dm->wifi_chnl_info[1] = h2c_parameter[1];
	coex_dm->wifi_chnl_info[2] = h2c_parameter[2];
	
	buf[0] = data_len;
	buf[1] = 0x5;				// OP_Code
	buf[2] = 0x3;				// OP_Code_Length
	buf[3] = h2c_parameter[0]; 	// OP_Code_Content
	buf[4] = h2c_parameter[1];
	buf[5] = h2c_parameter[2];
		
	btcoexist->btc_set(btcoexist, BTC_SET_ACT_CTRL_BT_COEX, (void*)&buf[0]);		
}

void
ex_halbtc8812a1ant_special_packet_notify(
		struct btc_coexist	*			btcoexist,
		u8				type
	)
{
	bool bt_hs_on=false;

	if(btcoexist->manual_control ||
		btcoexist->stop_coex_dm ||
		btcoexist->bt_info.bt_disabled )
		return;

	btcoexist->btc_get(btcoexist, BTC_GET_BL_HS_OPERATION, &bt_hs_on);
	if(coex_sta->c2h_bt_inquiry_page)
	{
		halbtc8812a1ant_action_bt_inquiry(btcoexist);
		return;
	}
	else if(bt_hs_on)
	{
		halbtc8812a1ant_action_hs(btcoexist);
		return;
	}

	if( BTC_PACKET_DHCP == type ||
		BTC_PACKET_EAPOL == type )
	{
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, "[BTCoex], special Packet(%d) notify\n", type);
		halbtc8812a1ant_action_wifi_connected_special_packet(btcoexist);
	}
}

void
ex_halbtc8812a1ant_bt_info_notify(
		struct btc_coexist	*		btcoexist,
		u8 *			tmp_buf,
		u8			length
	)
{
	struct btc_bt_link_info*	bt_link_info=&btcoexist->bt_link_info;
	u8				bt_info=0;
	u8				i, rsp_source=0;
	static u32		set_bt_psd_mode=0;
	bool				bt_busy=false;
	bool				wifi_connected=false;
	bool				b_bt_ctrl_buf_size=false, rej_ap_agg_pkt=false;

	rsp_source = tmp_buf[0]&0xf;
	if(rsp_source >= BT_INFO_SRC_8812A_1ANT_MAX)
		rsp_source = BT_INFO_SRC_8812A_1ANT_WIFI_FW;
	coex_sta->bt_info_c2h_cnt[rsp_source]++;

	if(BT_INFO_SRC_8812A_1ANT_BT_RSP == rsp_source)
		coex_sta->bt_info_query_cnt = coex_sta->bt_info_c2h_cnt[BT_INFO_SRC_8812A_1ANT_BT_RSP];

	BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, "[BTCoex], Bt info[%d], length=%d, hex data=[", rsp_source, length);
	for(i=0; i<length; i++)
	{
		coex_sta->bt_info_c2h[rsp_source][i] = tmp_buf[i];
		if(i == 1)
			bt_info = tmp_buf[i];
		if(i == length-1)
		{
			BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, "0x%02x]\n", tmp_buf[i]);
		}
		else
		{
			BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, "0x%02x, ", tmp_buf[i]);
		}
	}

	if(btcoexist->bt_info.bt_disabled)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BtInfoNotify(), return for BT is disabled <===\n"));
		return;
	}	

	if(btcoexist->manual_control)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BtInfoNotify(), return for Manual CTRL<===\n"));
		return;
	}
	if(btcoexist->stop_coex_dm)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BtInfoNotify(), return for Coex STOPPED!!<===\n"));
		return;
	}

	if(BT_INFO_SRC_8812A_1ANT_WIFI_FW != rsp_source)
	{
		coex_sta->bt_retry_cnt =	// [3:0]
			coex_sta->bt_info_c2h[rsp_source][2]&0xf;

		coex_sta->bt_rssi =
			coex_sta->bt_info_c2h[rsp_source][3]*2+10;

		coex_sta->bt_info_ext = 
			coex_sta->bt_info_c2h[rsp_source][4];

		// Here we need to resend some wifi info to BT
		// because bt is reset and loss of the info.
		if( (coex_sta->bt_info_ext & BIT1) )
		{
			BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BT ext info bit1 check, send wifi BW&Chnl to BT!!\n"));
			btcoexist->btc_get(btcoexist, BTC_GET_BL_WIFI_CONNECTED, &wifi_connected);
			if(wifi_connected)
			{
				ex_halbtc8812a1ant_media_status_notify(btcoexist, BTC_MEDIA_CONNECT);
			}
			else
			{
				ex_halbtc8812a1ant_media_status_notify(btcoexist, BTC_MEDIA_DISCONNECT);
			}

			set_bt_psd_mode = 0;
		}

		// test-chip bt patch only rsp the status for BT_RSP, 
		// so temporary we consider the following only under BT_RSP
		if(BT_INFO_SRC_8812A_1ANT_BT_RSP == rsp_source)
		{
			if( (coex_sta->bt_info_ext & BIT3) )
			{
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BT ext info bit3 check, set BT NOT to ignore Wlan active!!\n"));
				halbtc8812a1ant_ignore_wlan_act(btcoexist, FORCE_EXEC, false);
			}
			else
			{
				// BT already NOT ignore Wlan active, do nothing here.
			}

			if( (coex_sta->bt_info_ext & BIT4) )
			{
				// BT auto report already enabled, do nothing
			}
			else
			{
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BT ext info bit4 check, set BT to enable Auto Report!!\n"));
				halbtc8812a1ant_bt_auto_report(btcoexist, FORCE_EXEC, true);
			}
		}
	}
		
	// check BIT2 first ==> check if bt is under inquiry or page scan
	if(bt_info & BT_INFO_8812A_1ANT_B_INQ_PAGE)
		coex_sta->c2h_bt_inquiry_page = true;
	else
		coex_sta->c2h_bt_inquiry_page = false;

	// set link exist status
	if(!(bt_info&BT_INFO_8812A_1ANT_B_CONNECTION))
	{
		coex_sta->bt_link_exist = false;
		coex_sta->pan_exist = false;
		coex_sta->a2dp_exist = false;
		coex_sta->hid_exist = false;
		coex_sta->sco_exist = false;
	}
	else	// connection exists
	{
		coex_sta->bt_link_exist = true;
		if(bt_info & BT_INFO_8812A_1ANT_B_FTP)
			coex_sta->pan_exist = true;
		else
			coex_sta->pan_exist = false;
		if(bt_info & BT_INFO_8812A_1ANT_B_A2DP)
			coex_sta->a2dp_exist = true;
		else
			coex_sta->a2dp_exist = false;
		if(bt_info & BT_INFO_8812A_1ANT_B_HID)
			coex_sta->hid_exist = true;
		else
			coex_sta->hid_exist = false;
		if(bt_info & BT_INFO_8812A_1ANT_B_SCO_ESCO)
			coex_sta->sco_exist = true;
		else
			coex_sta->sco_exist = false;
	}

	halbtc8812a1ant_update_bt_link_info(btcoexist);
	
	if(!(bt_info&BT_INFO_8812A_1ANT_B_CONNECTION))
	{
		coex_dm->bt_status = BT_8812A_1ANT_BT_STATUS_NON_CONNECTED_IDLE;
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BtInfoNotify(), bt non-connected idle!!!\n"));
	}
	else if(bt_info == BT_INFO_8812A_1ANT_B_CONNECTION)	// connection exists but no busy
	{
		coex_dm->bt_status = BT_8812A_1ANT_BT_STATUS_CONNECTED_IDLE;
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BtInfoNotify(), bt connected-idle!!!\n"));
	}		
	else if((bt_info&BT_INFO_8812A_1ANT_B_SCO_ESCO) ||
		(bt_info&BT_INFO_8812A_1ANT_B_SCO_BUSY))
	{
		coex_dm->bt_status = BT_8812A_1ANT_BT_STATUS_SCO_BUSY;
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BtInfoNotify(), bt sco busy!!!\n"));
	}
	else if(bt_info&BT_INFO_8812A_1ANT_B_ACL_BUSY)
	{
		if(BT_8812A_1ANT_BT_STATUS_ACL_BUSY != coex_dm->bt_status)
			coex_dm->reset_tdma_adjust = true;
		coex_dm->bt_status = BT_8812A_1ANT_BT_STATUS_ACL_BUSY;
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BtInfoNotify(), bt acl busy!!!\n"));
	}
	else
	{
		coex_dm->bt_status = BT_8812A_1ANT_BT_STATUS_MAX;
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BtInfoNotify(), bt non-defined state!!!\n"));
	}

	if(bt_link_info->sco_exist)
	{
		rej_ap_agg_pkt = true;
		halbtc8812a1ant_update_ra_mask(btcoexist, NORMAL_EXEC, BTC_RATE_DISABLE, 0x00000003);	// disable cck 1M2M.
	}
	else
	{
		halbtc8812a1ant_update_ra_mask(btcoexist, NORMAL_EXEC, BTC_RATE_ENABLE, 0x00000003);	// enable cck 1M2M.
	}

	if( (BT_8812A_1ANT_BT_STATUS_ACL_BUSY == coex_dm->bt_status) ||
		(BT_8812A_1ANT_BT_STATUS_SCO_BUSY == coex_dm->bt_status) ||
		(BT_8812A_1ANT_BT_STATUS_ACL_SCO_BUSY == coex_dm->bt_status) )
	{
		bt_busy = true;
		if(bt_link_info->hid_exist)
			b_bt_ctrl_buf_size = true;
	}
	else
	{
		bt_busy = false;
	}
	btcoexist->btc_set(btcoexist, BTC_SET_BL_BT_TRAFFIC_BUSY, &bt_busy);

	//============================================
	//	Aggregation related setting
	//============================================	
	// if sco, reject AddBA
	btcoexist->btc_set(btcoexist, BTC_SET_BL_TO_REJ_AP_AGG_PKT, &rej_ap_agg_pkt);
	// decide BT control aggregation buf size or not
	btcoexist->btc_set(btcoexist, BTC_SET_BL_BT_CTRL_AGG_SIZE, &b_bt_ctrl_buf_size);
	// real update aggregation setting
	btcoexist->btc_set(btcoexist, BTC_SET_ACT_AGGREGATE_CTRL, NULL);
	//============================================

	halbtc8812a1ant_RunCoexistMechanism(btcoexist);
}

void
ex_halbtc8812a1ant_halt_notify(
		struct btc_coexist	*			btcoexist
	)
{
	BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], Halt notify\n"));

	halbtc8812a1ant_ignore_wlan_act(btcoexist, FORCE_EXEC, true);
	halbtc8812a1ant_ps_tdma(btcoexist, FORCE_EXEC, false, 0);
	halbtc8812a1ant_set_ant_path(btcoexist, BTC_ANT_PATH_BT, false, true);
	ex_halbtc8812a1ant_media_status_notify(btcoexist, BTC_MEDIA_DISCONNECT);
}

void
ex_halbtc8812a1ant_pnp_notify(
		struct btc_coexist	*			btcoexist,
		u8				pnp_state
	)
{
	BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], Pnp notify\n"));

	if(BTC_WIFI_PNP_SLEEP == pnp_state)
	{
		btcoexist->stop_coex_dm = true;
		halbtc8812a1ant_ignore_wlan_act(btcoexist, FORCE_EXEC, true);
		halbtc8812a1ant_power_save_state(btcoexist, BTC_PS_WIFI_NATIVE, 0x0, 0x0);
		halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, false, 9);	
	}
	else if(BTC_WIFI_PNP_WAKE_UP == pnp_state)
	{
		
	}
}

void
ex_halbtc8812a1ant_periodical(
		struct btc_coexist	*			btcoexist
	)
{
	static u8		dis_ver_info_cnt=0;
	u32				fw_ver=0, bt_patch_ver=0;
	struct btc_board_info*		board_info=&btcoexist->board_info;
	struct btc_stack_info*		stack_info=&btcoexist->stack_info;

	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], ==========================Periodical===========================\n"));

	if(dis_ver_info_cnt <= 5)
	{
		dis_ver_info_cnt += 1;
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_INIT, ("[BTCoex], ****************************************************************\n"));
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_INIT, "[BTCoex], Ant PG Num/ Ant Mech/ Ant Pos = %d/ %d/ %d\n", \
			board_info->pg_ant_num, board_info->btdm_ant_num, board_info->btdm_ant_pos);
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_INIT, "[BTCoex], BT stack/ hci ext ver = %s / %d\n", \
			((stack_info->profile_notified)? "Yes":"No"), stack_info->hci_version);
		btcoexist->btc_get(btcoexist, BTC_GET_U4_BT_PATCH_VER, &bt_patch_ver);
		btcoexist->btc_get(btcoexist, BTC_GET_U4_WIFI_FW_VER, &fw_ver);
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_INIT, "[BTCoex], CoexVer/ FwVer/ PatchVer = %d_%x/ 0x%x/ 0x%x(%d)\n", \
			glcoex_ver_date_8812a_1ant, glcoex_ver_8812a_1ant, fw_ver, bt_patch_ver, bt_patch_ver);
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_INIT, ("[BTCoex], ****************************************************************\n"));
	}

	halbtc8812a1ant_query_bt_info(btcoexist);
	halbtc8812a1ant_monitor_bt_ctr(btcoexist);
	halbtc8812a1ant_monitor_bt_enable_disable(btcoexist);
}

void
ex_halbtc8812a1ant_dbg_control(
		struct btc_coexist	*			btcoexist,
		u8				op_code,
		u8				op_len,
		u8 *				data
	)
{
	switch(op_code)
	{
		case BTC_DBG_SET_COEX_NORMAL:
			BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Set CoexMode to Normal\n"));
			btcoexist->manual_control = false;
			halbtc8812a1ant_init_coex_dm(btcoexist);
			break;
		case BTC_DBG_SET_COEX_WIFI_ONLY:
			BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Set CoexMode to Wifi Only\n"));
			btcoexist->manual_control = true;
			halbtc8812a1ant_power_save_state(btcoexist, BTC_PS_WIFI_NATIVE, 0x0, 0x0);
			halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, false, 9);	
			break;
		case BTC_DBG_SET_COEX_BT_ONLY:
			BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Set CoexMode to BT only\n"));
			btcoexist->manual_control = true;
			halbtc8812a1ant_power_save_state(btcoexist, BTC_PS_WIFI_NATIVE, 0x0, 0x0);
			halbtc8812a1ant_ps_tdma(btcoexist, NORMAL_EXEC, false, 0);	
			break;
		case BTC_DBG_SET_COEX_DEC_BT_PWR:
			BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Set Dec BT power\n"));
			{
				u8	data_len=4;
				u8	buf[6] = {0};
				u8	decBtPwr=0, pwrLevel=0;
				if(op_len == 2)
				{
					decBtPwr = data[0];
					pwrLevel = data[1];
				
					buf[0] = data_len;
					buf[1] = 0x3;		// OP_Code
					buf[2] = 0x2;		// OP_Code_Length
					
					buf[3] = decBtPwr;	// OP_Code_Content
					buf[4] = pwrLevel;
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, "[BTCoex], Set Dec BT power=%d, pwrLevel=%d\n", decBtPwr, pwrLevel);
					btcoexist->btc_set(btcoexist, BTC_SET_ACT_CTRL_BT_COEX, (void*)&buf[0]);	
				}
			}
			break;

		case BTC_DBG_SET_COEX_BT_AFH_MAP:
			BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Set BT AFH Map\n"));
			{
				u8	data_len=5;
				u8	buf[6] = {0};
				if(op_len == 3)
				{
					buf[0] = data_len;
					buf[1] = 0x5;				// OP_Code
					buf[2] = 0x3;				// OP_Code_Length

					buf[3] = data[0]; 			// OP_Code_Content
					buf[4] = data[1];
					buf[5] = data[2];

					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, "[BTCoex], Set BT AFH Map = %02x %02x %02x\n", 
						data[0], data[1], data[2]);
					btcoexist->btc_set(btcoexist, BTC_SET_ACT_CTRL_BT_COEX, (void*)&buf[0]);
				}
			}
			break;
		
		case BTC_DBG_SET_COEX_BT_IGNORE_WLAN_ACT:
			BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Set BT Ignore Wlan Active\n"));
			{
				u8	data_len=3;
				u8	buf[6] = {0};
				if(op_len == 1)
				{
					buf[0] = data_len;
					buf[1] = 0x1;			// OP_Code
					buf[2] = 0x1;			// OP_Code_Length

					buf[3] = data[0];		// OP_Code_Content
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, "[BTCoex], Set BT Ignore Wlan Active = 0x%x\n", 
						data[0]);
						
					btcoexist->btc_set(btcoexist, BTC_SET_ACT_CTRL_BT_COEX, (void*)&buf[0]);
				}
			}
			break;
		default:
			break;
	}
}
#endif

