/******************************************************************************
 *
 * Copyright(c) 2007 - 2017  Realtek Corporation.
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
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/

/* ************************************************************
 * include files
 * ************************************************************ */

#include "mp_precomp.h"
#include "phydm_precomp.h"

void phydm_dynamicsoftmletting(void *p_dm_void)
{
	struct PHY_DM_STRUCT *p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	u32 ret_val;

#if (RTL8822B_SUPPORT == 1)
	if (*p_dm->p_mp_mode == false) {
		if (p_dm->support_ic_type & ODM_RTL8822B) {
			if ((!p_dm->is_linked) |
			    (p_dm->iot_table.is_linked_cmw500))
				return;

			if (true == p_dm->bsomlenabled) {
				PHYDM_DBG(
					p_dm, ODM_COMP_API,
					("PHYDM_DynamicSoftMLSetting(): SoML has been enable, skip dynamic SoML switch\n"));
				return;
			}

			ret_val = odm_get_bb_reg(p_dm, BBREG_0xf8c, MASKBYTE0);
			PHYDM_DBG(
				p_dm, ODM_COMP_API,
				("PHYDM_DynamicSoftMLSetting(): Read BBREG_0xf8c = 0x%08X\n",
				 ret_val));

			if (ret_val < 0x16) {
				PHYDM_DBG(
					p_dm, ODM_COMP_API,
					("PHYDM_DynamicSoftMLSetting(): BBREG_0xf8c(== 0x%08X) < 0x16, enable SoML\n",
					 ret_val));
				phydm_somlrxhp_setting(p_dm, true);
				/*odm_set_bb_reg(p_dm, 0x19a8, MASKDWORD, 0xc10a0000);*/
				p_dm->bsomlenabled = true;
			}
		}
	}
#endif
}

#ifdef CONFIG_ADAPTIVE_SOML
void
phydm_soml_on_off(
	void		*p_dm_void,
	u8		swch
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct adaptive_soml	*p_dm_soml_table = &p_dm->dm_soml_table;

	if (swch == SOML_ON) {
		PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("(( Turn on )) SOML\n"));

		if (p_dm->support_ic_type == ODM_RTL8822B)
			phydm_somlrxhp_setting(p_dm, true);
		else if (p_dm->support_ic_type == ODM_RTL8197F)
			odm_set_bb_reg(p_dm, BBREG_0x998, BIT(6), swch);

	} else if (swch == SOML_OFF) {
		PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("(( Turn off )) SOML\n"));

		if (p_dm->support_ic_type == ODM_RTL8822B)
			phydm_somlrxhp_setting(p_dm, false);
		else if (p_dm->support_ic_type == ODM_RTL8197F)
			odm_set_bb_reg(p_dm, BBREG_0x998, BIT(6), swch);
	}
	p_dm_soml_table->soml_on_off = swch;
}

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
void
phydm_adaptive_soml_callback(
	struct timer_list		*p_timer
)
{
	struct _ADAPTER		*adapter = (struct _ADAPTER *)p_timer->Adapter;
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(adapter);
	struct PHY_DM_STRUCT		*p_dm = &p_hal_data->DM_OutSrc;
	struct adaptive_soml	*p_dm_soml_table = &p_dm->dm_soml_table;

#if DEV_BUS_TYPE == RT_PCI_INTERFACE
#if USE_WORKITEM
	odm_schedule_work_item(&p_dm_soml_table->phydm_adaptive_soml_workitem);
#else
	{
		/*dbg_print("phydm_adaptive_soml-phydm_adaptive_soml_callback\n");*/
		phydm_adsl(p_dm);
	}
#endif
#else
	odm_schedule_work_item(&p_dm_soml_table->phydm_adaptive_soml_workitem);
#endif
}

void
phydm_adaptive_soml_workitem_callback(
	void		*p_context
)
{
#ifdef CONFIG_ADAPTIVE_SOML
	struct _ADAPTER		*p_adapter = (struct _ADAPTER *)p_context;
	HAL_DATA_TYPE	*p_hal_data = GET_HAL_DATA(p_adapter);
	struct PHY_DM_STRUCT		*p_dm = &p_hal_data->DM_OutSrc;

	/*dbg_print("phydm_adaptive_soml-phydm_adaptive_soml_workitem_callback\n");*/
	phydm_adsl(p_dm);
#endif
}

#else

void
phydm_adaptive_soml_callback(
	void		*p_dm_void
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;

	PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("******SOML_Callback******\n"));
	phydm_adsl(p_dm);

}
#endif

void
phydm_rx_qam_for_soml(
	void		*p_dm_void,
	void		*p_pkt_info_void
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct adaptive_soml				*p_dm_soml_table = &p_dm->dm_soml_table;
	struct phydm_perpkt_info_struct	*p_pktinfo = (struct phydm_perpkt_info_struct *)p_pkt_info_void;
	u8								date_rate = (p_pktinfo->data_rate & 0x7f);

	if (p_dm_soml_table->soml_state_cnt < ((p_dm_soml_table->soml_train_num)<<1)) {
		if (p_dm_soml_table->soml_on_off == SOML_ON)
			return;
		else if (p_dm_soml_table->soml_on_off == SOML_OFF) {
			if ((date_rate >= ODM_RATEMCS8) && (date_rate <= ODM_RATEMCS10))
				p_dm_soml_table->num_ht_qam[BPSK_QPSK]++;

			else if ((date_rate >= ODM_RATEMCS11) && (date_rate <= ODM_RATEMCS12))
				p_dm_soml_table->num_ht_qam[QAM16]++;

			else if ((date_rate >= ODM_RATEMCS13) && (date_rate <= ODM_RATEMCS15))
				p_dm_soml_table->num_ht_qam[QAM64]++;

			else if ((date_rate >= ODM_RATEVHTSS2MCS0) && (date_rate <= ODM_RATEVHTSS2MCS2))
				p_dm_soml_table->num_vht_qam[BPSK_QPSK]++;

			else if ((date_rate >= ODM_RATEVHTSS2MCS3) && (date_rate <= ODM_RATEVHTSS2MCS4))
				p_dm_soml_table->num_vht_qam[QAM16]++;

			else if ((date_rate >= ODM_RATEVHTSS2MCS5) && (date_rate <= ODM_RATEVHTSS2MCS5))
				p_dm_soml_table->num_vht_qam[QAM64]++;

			else if ((date_rate >= ODM_RATEVHTSS2MCS8) && (date_rate <= ODM_RATEVHTSS2MCS9))
				p_dm_soml_table->num_vht_qam[QAM256]++;
		}
	}
}

void
phydm_soml_reset_rx_rate(
	void		*p_dm_void
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct adaptive_soml	*p_dm_soml_table = &p_dm->dm_soml_table;
	u8	order;

	for (order = 0; order < HT_ORDER_TYPE; order++)
		p_dm_soml_table->num_ht_qam[order] = 0;

	for (order = 0; order < VHT_ORDER_TYPE; order++)
		p_dm_soml_table->num_vht_qam[order] = 0;
}

void
phydm_soml_cfo_process(
	void		*p_dm_void,
	s32		*p_diff_a,
	s32		*p_diff_b
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct adaptive_soml	*p_dm_soml_table = &p_dm->dm_soml_table;
	u32	value32, value32_1, value32_2, value32_3;
	s32	cfo_acq_a, cfo_acq_b, cfo_end_a, cfo_end_b;
	s32	cfo_diff_a, cfo_diff_b;

	value32 = odm_get_bb_reg(p_dm, BBREG_0xd10, MASKDWORD);
	value32_1 = odm_get_bb_reg(p_dm, BBREG_0xd14, MASKDWORD);
	value32_2 = odm_get_bb_reg(p_dm, BBREG_0xd50, MASKDWORD);
	value32_3 = odm_get_bb_reg(p_dm, BBREG_0xd54, MASKDWORD);

	cfo_acq_a = (s32)((value32 & 0x1fff0000) >> 16);
	cfo_end_a = (s32)((value32_1 & 0x1fff0000) >> 16);
	cfo_acq_b = (s32)((value32_2 & 0x1fff0000) >> 16);
	cfo_end_b = (s32)((value32_3 & 0x1fff0000) >> 16);

	*p_diff_a = ((cfo_acq_a >= cfo_end_a) ? (cfo_acq_a - cfo_end_a) : (cfo_end_a - cfo_acq_a));
	*p_diff_b = ((cfo_acq_b >= cfo_end_b) ? (cfo_acq_b - cfo_end_b) : (cfo_end_b - cfo_acq_b));

	*p_diff_a =  ((*p_diff_a * 312) + (*p_diff_a >> 1)) >> 12;  /* 312.5/2^12 */
	*p_diff_b = ((*p_diff_b * 312) + (*p_diff_b >> 1)) >> 12; /* 312.5/2^12 */

}

void
phydm_soml_debug(
	void		*p_dm_void,
	u32		*const dm_value,
	u32		*_used,
	char		*output,
	u32		*_out_len
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct adaptive_soml	*p_dm_soml_table = &p_dm->dm_soml_table;
	u32 used = *_used;
	u32 out_len = *_out_len;

	if (dm_value[0] == 1) { /*Turn on/off SOML*/
		p_dm_soml_table->soml_select = (u8)dm_value[1];

	} else if (dm_value[0] == 2) { /*training number for SOML*/

		p_dm_soml_table->soml_train_num = (u8)dm_value[1];
		PHYDM_SNPRINTF((output + used, out_len - used, "soml_train_num = ((%d))\n", p_dm_soml_table->soml_train_num));
	} else if (dm_value[0] == 3) { /*training interval for SOML*/

		p_dm_soml_table->soml_intvl = (u8)dm_value[1];
		PHYDM_SNPRINTF((output + used, out_len - used, "soml_intvl = ((%d))\n", p_dm_soml_table->soml_intvl));
	} else if (dm_value[0] == 4) { /*function period for SOML*/

		p_dm_soml_table->soml_period = (u8)dm_value[1];
		PHYDM_SNPRINTF((output + used, out_len - used, "soml_period = ((%d))\n", p_dm_soml_table->soml_period));
	} else if (dm_value[0] == 5) { /*delay_time for SOML*/

		p_dm_soml_table->soml_delay_time = (u8)dm_value[1];
		PHYDM_SNPRINTF((output + used, out_len - used, "soml_delay_time = ((%d))\n", p_dm_soml_table->soml_delay_time));
	} else if (dm_value[0] == 6) { /* for SOML Rx QAM distribution th*/
		if (dm_value[1] == 256) {
			p_dm_soml_table->qam256_dist_th = (u8)dm_value[2];
			PHYDM_SNPRINTF((output + used, out_len - used, "qam256_dist_th = ((%d))\n", p_dm_soml_table->qam256_dist_th));
		} else if (dm_value[1] == 64) {
			p_dm_soml_table->qam64_dist_th = (u8)dm_value[2];
			PHYDM_SNPRINTF((output + used, out_len - used, "qam64_dist_th = ((%d))\n", p_dm_soml_table->qam64_dist_th));
		} else if (dm_value[1] == 16) {
			p_dm_soml_table->qam16_dist_th = (u8)dm_value[2];
			PHYDM_SNPRINTF((output + used, out_len - used, "qam16_dist_th = ((%d))\n", p_dm_soml_table->qam16_dist_th));
		} else if (dm_value[1] == 4) {
			p_dm_soml_table->bpsk_qpsk_dist_th = (u8)dm_value[2];
			PHYDM_SNPRINTF((output + used, out_len - used, "bpsk_qpsk_dist_th = ((%d))\n", p_dm_soml_table->bpsk_qpsk_dist_th));
		}
	} else if (dm_value[0] == 7) { /* for SOML cfo th*/
		if (dm_value[1] == 256) {
			p_dm_soml_table->cfo_qam256_th = (u8)dm_value[2];
			PHYDM_SNPRINTF((output + used, out_len - used, "cfo_qam256_th = ((%d KHz))\n", p_dm_soml_table->cfo_qam256_th));
		} else if (dm_value[1] == 64) {
			p_dm_soml_table->cfo_qam64_th = (u8)dm_value[2];
			PHYDM_SNPRINTF((output + used, out_len - used, "cfo_qam64_th = ((%d KHz))\n", p_dm_soml_table->cfo_qam64_th));
		} else if (dm_value[1] == 16) {
			p_dm_soml_table->cfo_qam16_th = (u8)dm_value[2];
			PHYDM_SNPRINTF((output + used, out_len - used, "cfo_qam16_th = ((%d KHz))\n", p_dm_soml_table->cfo_qam16_th));
		} else if (dm_value[1] == 4) {
			p_dm_soml_table->cfo_qpsk_th = (u8)dm_value[2];
			PHYDM_SNPRINTF((output + used, out_len - used, "cfo_qpsk_th = ((%d KHz))\n", p_dm_soml_table->cfo_qpsk_th));
		}
	} else if (dm_value[0] == 100) { /*show parameters*/
		PHYDM_SNPRINTF((output + used, out_len - used, "soml_select = ((%d))\n", p_dm_soml_table->soml_select));
		PHYDM_SNPRINTF((output + used, out_len - used, "soml_train_num = ((%d))\n", p_dm_soml_table->soml_train_num));
		PHYDM_SNPRINTF((output + used, out_len - used, "soml_intvl = ((%d))\n", p_dm_soml_table->soml_intvl));
		PHYDM_SNPRINTF((output + used, out_len - used, "soml_period = ((%d))\n", p_dm_soml_table->soml_period));
		PHYDM_SNPRINTF((output + used, out_len - used, "soml_delay_time = ((%d))\n\n", p_dm_soml_table->soml_delay_time));
		PHYDM_SNPRINTF((output + used, out_len - used, "qam256_dist_th = ((%d)),  qam64_dist_th = ((%d)), ", p_dm_soml_table->qam256_dist_th, p_dm_soml_table->qam64_dist_th));
		PHYDM_SNPRINTF((output + used, out_len - used, "qam16_dist_th = ((%d)),  bpsk_qpsk_dist_th = ((%d))\n", p_dm_soml_table->qam16_dist_th, p_dm_soml_table->bpsk_qpsk_dist_th));
		PHYDM_SNPRINTF((output + used, out_len - used, "cfo_qam256_th = ((%d KHz)),  cfo_qam64_th = ((%d KHz)), ", p_dm_soml_table->cfo_qam256_th, p_dm_soml_table->cfo_qam64_th));
		PHYDM_SNPRINTF((output + used, out_len - used, "cfo_qam16_th = ((%d KHz)),  cfo_qpsk_th  = ((%d KHz))\n", p_dm_soml_table->cfo_qam16_th, p_dm_soml_table->cfo_qpsk_th));
	}
	*_used = used;
	*_out_len = out_len;
}

void
phydm_soml_statistics(
	void		*p_dm_void,
	u8		on_off_state

)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct adaptive_soml	*p_dm_soml_table = &p_dm->dm_soml_table;

	u8	i;
	u32	num_bytes_diff;

	if (p_dm->support_ic_type == ODM_RTL8197F) {
		if (on_off_state == SOML_ON) {
			for (i = 0; i < HT_RATE_IDX; i++) {
				num_bytes_diff = p_dm_soml_table->num_ht_bytes[i] - p_dm_soml_table->pre_num_ht_bytes[i];
				p_dm_soml_table->num_ht_bytes_on[i] += num_bytes_diff;
				p_dm_soml_table->pre_num_ht_bytes[i] = p_dm_soml_table->num_ht_bytes[i];
			}
		} else if (on_off_state == SOML_OFF) {
			for (i = 0; i < HT_RATE_IDX; i++) {
				num_bytes_diff = p_dm_soml_table->num_ht_bytes[i] - p_dm_soml_table->pre_num_ht_bytes[i];
				p_dm_soml_table->num_ht_bytes_off[i] += num_bytes_diff;
				p_dm_soml_table->pre_num_ht_bytes[i] = p_dm_soml_table->num_ht_bytes[i];
			}
		}
	} else if (p_dm->support_ic_type == ODM_RTL8822B) {
		if (on_off_state == SOML_ON) {
			for (i = 0; i < VHT_RATE_IDX; i++) {
				num_bytes_diff = p_dm_soml_table->num_vht_bytes[i] - p_dm_soml_table->pre_num_vht_bytes[i];
				p_dm_soml_table->num_vht_bytes_on[i] += num_bytes_diff;
				p_dm_soml_table->pre_num_vht_bytes[i] = p_dm_soml_table->num_vht_bytes[i];
			}
		} else if (on_off_state == SOML_OFF) {
			for (i = 0; i < VHT_RATE_IDX; i++) {
				num_bytes_diff = p_dm_soml_table->num_vht_bytes[i] - p_dm_soml_table->pre_num_vht_bytes[i];
				p_dm_soml_table->num_vht_bytes_off[i] += num_bytes_diff;
				p_dm_soml_table->pre_num_vht_bytes[i] = p_dm_soml_table->num_vht_bytes[i];
			}
		}
	}
}

void
phydm_adsl(
	void		*p_dm_void
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct adaptive_soml		*p_dm_soml_table = &p_dm->dm_soml_table;

	u8	i;
	u8	next_on_off;
	u8	rate_num = 1, rate_ss_shift = 0;
	u32	byte_total_on = 0, byte_total_off = 0, num_total_qam = 0;
	u32	ht_reset[HT_RATE_IDX] = {0}, vht_reset[VHT_RATE_IDX] = {0};
	u8	size = sizeof(ht_reset[0]);

	if (p_dm->support_ic_type & ODM_IC_4SS)
		rate_num = 4;
	else if (p_dm->support_ic_type & ODM_IC_3SS)
		rate_num = 3;
	else if (p_dm->support_ic_type & ODM_IC_2SS)
		rate_num = 2;

	if ((p_dm->support_ic_type & ODM_ADAPTIVE_SOML_SUPPORT_IC)) {
		if (TRUE) {
			if ((p_dm->rssi_min >= SOML_RSSI_TH_HIGH) || (p_dm_soml_table->is_soml_method_enable == 1)) {
				PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("> TH_H || is_soml_method_enable==1\n"));
				PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("soml_state_cnt =((%d)),  soml_on_off =((%s))\n", p_dm_soml_table->soml_state_cnt, (p_dm_soml_table->soml_on_off == SOML_ON) ? "SOML_ON" : "SOML_OFF"));
				/*Traning state: 0(alt) 1(ori) 2(alt) 3(ori)============================================================*/
				if (p_dm_soml_table->soml_state_cnt < ((p_dm_soml_table->soml_train_num)<<1)) {
					if (p_dm_soml_table->soml_state_cnt == 0) {
						if (p_dm->support_ic_type == ODM_RTL8197F) {
							odm_move_memory(p_dm, p_dm_soml_table->num_ht_bytes, ht_reset, HT_RATE_IDX*size);
							odm_move_memory(p_dm, p_dm_soml_table->num_ht_bytes_on, ht_reset, HT_RATE_IDX*size);
							odm_move_memory(p_dm, p_dm_soml_table->num_ht_bytes_off, ht_reset, HT_RATE_IDX*size);
						} else if (p_dm->support_ic_type == ODM_RTL8822B) {
							odm_move_memory(p_dm, p_dm_soml_table->num_vht_bytes, vht_reset, VHT_RATE_IDX*size);
							odm_move_memory(p_dm, p_dm_soml_table->num_vht_bytes_on, vht_reset, VHT_RATE_IDX*size);
							odm_move_memory(p_dm, p_dm_soml_table->num_vht_bytes_off, vht_reset, VHT_RATE_IDX*size);
							p_dm_soml_table->cfo_counter++;
							phydm_soml_cfo_process(p_dm,
									       &p_dm_soml_table->cfo_diff_a,
									       &p_dm_soml_table->cfo_diff_b);
							PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[ (%d) cfo_diff_a = %d KHz; cfo_diff_b = %d KHz ]\n", p_dm_soml_table->cfo_counter, p_dm_soml_table->cfo_diff_a, p_dm_soml_table->cfo_diff_b));
							p_dm_soml_table->cfo_diff_sum_a += p_dm_soml_table->cfo_diff_a;
							p_dm_soml_table->cfo_diff_sum_b += p_dm_soml_table->cfo_diff_b;
						}

						p_dm_soml_table->is_soml_method_enable = 1;
						p_dm_soml_table->soml_state_cnt++;
						next_on_off = (p_dm_soml_table->soml_on_off == SOML_ON) ? SOML_ON : SOML_OFF;
						phydm_soml_on_off(p_dm, next_on_off);
						odm_set_timer(p_dm, &p_dm_soml_table->phydm_adaptive_soml_timer, p_dm_soml_table->soml_delay_time); /*ms*/
					} else if ((p_dm_soml_table->soml_state_cnt % 2) != 0) {
						p_dm_soml_table->soml_state_cnt++;
						if (p_dm->support_ic_type == ODM_RTL8197F)
							odm_move_memory(p_dm, p_dm_soml_table->pre_num_ht_bytes, p_dm_soml_table->num_ht_bytes, HT_RATE_IDX*size);
						else if (p_dm->support_ic_type == ODM_RTL8822B) {
							odm_move_memory(p_dm, p_dm_soml_table->pre_num_vht_bytes, p_dm_soml_table->num_vht_bytes, VHT_RATE_IDX*size);
							p_dm_soml_table->cfo_counter++;
							phydm_soml_cfo_process(p_dm,
									       &p_dm_soml_table->cfo_diff_a,
									       &p_dm_soml_table->cfo_diff_b);
							PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[ (%d) cfo_diff_a = %d KHz; cfo_diff_b = %d KHz ]\n", p_dm_soml_table->cfo_counter, p_dm_soml_table->cfo_diff_a, p_dm_soml_table->cfo_diff_b));
							p_dm_soml_table->cfo_diff_sum_a += p_dm_soml_table->cfo_diff_a;
							p_dm_soml_table->cfo_diff_sum_b += p_dm_soml_table->cfo_diff_b;
						}
						odm_set_timer(p_dm, &p_dm_soml_table->phydm_adaptive_soml_timer, p_dm_soml_table->soml_intvl); /*ms*/
					} else if ((p_dm_soml_table->soml_state_cnt % 2) == 0) {
						if (p_dm->support_ic_type == ODM_RTL8822B) {
							p_dm_soml_table->cfo_counter++;
							phydm_soml_cfo_process(p_dm,
									       &p_dm_soml_table->cfo_diff_a,
									       &p_dm_soml_table->cfo_diff_b);
							PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[ (%d) cfo_diff_a = %d KHz; cfo_diff_b = %d KHz ]\n", p_dm_soml_table->cfo_counter, p_dm_soml_table->cfo_diff_a, p_dm_soml_table->cfo_diff_b));
							p_dm_soml_table->cfo_diff_sum_a += p_dm_soml_table->cfo_diff_a;
							p_dm_soml_table->cfo_diff_sum_b += p_dm_soml_table->cfo_diff_b;
						}
						p_dm_soml_table->soml_state_cnt++;
						phydm_soml_statistics(p_dm, p_dm_soml_table->soml_on_off);
						next_on_off = (p_dm_soml_table->soml_on_off == SOML_ON) ? SOML_OFF : SOML_ON;
						phydm_soml_on_off(p_dm, next_on_off);
						odm_set_timer(p_dm, &p_dm_soml_table->phydm_adaptive_soml_timer, p_dm_soml_table->soml_delay_time); /*ms*/
					}
				}
				/*Decision state: ==============================================================*/
				else {
					PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[Decisoin state ]\n"));
					phydm_soml_statistics(p_dm, p_dm_soml_table->soml_on_off);
					p_dm_soml_table->cfo_diff_avg_a = (p_dm_soml_table->cfo_counter != 0) ? (p_dm_soml_table->cfo_diff_sum_a  / p_dm_soml_table->cfo_counter) : 0;
					p_dm_soml_table->cfo_diff_avg_b = (p_dm_soml_table->cfo_counter != 0) ? (p_dm_soml_table->cfo_diff_sum_b / p_dm_soml_table->cfo_counter) : 0;
					PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[ cfo_diff_avg_a = %d KHz; cfo_diff_avg_b = %d KHz]\n", p_dm_soml_table->cfo_diff_avg_a, p_dm_soml_table->cfo_diff_avg_b));

					/* [Search 1st and 2ed rate by counter] */
					if (p_dm->support_ic_type == ODM_RTL8197F) {
						for (i = 0; i < rate_num; i++) {
							rate_ss_shift = (i << 3);
							PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("*num_ht_bytes_on  HT MCS[%d :%d ] = {%d, %d, %d, %d, %d, %d, %d, %d}\n",
								(rate_ss_shift), (rate_ss_shift+7),
								p_dm_soml_table->num_ht_bytes_on[rate_ss_shift + 0], p_dm_soml_table->num_ht_bytes_on[rate_ss_shift + 1],
								p_dm_soml_table->num_ht_bytes_on[rate_ss_shift + 2], p_dm_soml_table->num_ht_bytes_on[rate_ss_shift + 3],
								p_dm_soml_table->num_ht_bytes_on[rate_ss_shift + 4], p_dm_soml_table->num_ht_bytes_on[rate_ss_shift + 5],
								p_dm_soml_table->num_ht_bytes_on[rate_ss_shift + 6], p_dm_soml_table->num_ht_bytes_on[rate_ss_shift + 7]));
						}

						for (i = 0; i < rate_num; i++) {
							rate_ss_shift = (i << 3);
							PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("*num_ht_bytes_off  HT MCS[%d :%d ] = {%d, %d, %d, %d, %d, %d, %d, %d}\n",
								(rate_ss_shift), (rate_ss_shift+7),
								p_dm_soml_table->num_ht_bytes_off[rate_ss_shift + 0], p_dm_soml_table->num_ht_bytes_off[rate_ss_shift + 1],
								p_dm_soml_table->num_ht_bytes_off[rate_ss_shift + 2], p_dm_soml_table->num_ht_bytes_off[rate_ss_shift + 3],
								p_dm_soml_table->num_ht_bytes_off[rate_ss_shift + 4], p_dm_soml_table->num_ht_bytes_off[rate_ss_shift + 5],
								p_dm_soml_table->num_ht_bytes_off[rate_ss_shift + 6], p_dm_soml_table->num_ht_bytes_off[rate_ss_shift + 7]));
						}

						for (i = 0; i < HT_RATE_IDX; i++) {
							byte_total_on += p_dm_soml_table->num_ht_bytes_on[i];
							byte_total_off += p_dm_soml_table->num_ht_bytes_off[i];
						}

					} else if (p_dm->support_ic_type == ODM_RTL8822B) {
						for (i = 0; i < VHT_ORDER_TYPE; i++)
							num_total_qam += p_dm_soml_table->num_vht_qam[i];

						PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[ ((2SS)) BPSK_QPSK_count = %d ; 16QAM_count = %d ; 64QAM_count = %d ; 256QAM_count = %d ; num_total_qam = %d]\n", p_dm_soml_table->num_vht_qam[BPSK_QPSK], p_dm_soml_table->num_vht_qam[QAM16], p_dm_soml_table->num_vht_qam[QAM64], p_dm_soml_table->num_vht_qam[QAM256], num_total_qam));
						if (((p_dm_soml_table->num_vht_qam[QAM256] * 100) > (num_total_qam * p_dm_soml_table->qam256_dist_th)) && (p_dm_soml_table->cfo_diff_avg_a > p_dm_soml_table->cfo_qam256_th) && (p_dm_soml_table->cfo_diff_avg_b > p_dm_soml_table->cfo_qam256_th)) {
							PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[  QAM256_ratio > %d ; cfo_diff_avg_a > %d KHz ==> SOML_OFF]\n", p_dm_soml_table->qam256_dist_th, p_dm_soml_table->cfo_qam256_th));
							PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[ Final decisoin ] : "));
							phydm_soml_on_off(p_dm, SOML_OFF);
							return;
						} else if (((p_dm_soml_table->num_vht_qam[QAM64] * 100) > (num_total_qam * p_dm_soml_table->qam64_dist_th)) && (p_dm_soml_table->cfo_diff_avg_a > p_dm_soml_table->cfo_qam64_th) && (p_dm_soml_table->cfo_diff_avg_b > p_dm_soml_table->cfo_qam64_th)) {
							PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[  QAM64_ratio > %d ; cfo_diff_avg_a > %d KHz ==> SOML_OFF]\n", p_dm_soml_table->qam64_dist_th, p_dm_soml_table->cfo_qam64_th));
							PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[ Final decisoin ] : "));
							phydm_soml_on_off(p_dm, SOML_OFF);
							return;
						} else if (((p_dm_soml_table->num_vht_qam[QAM16] * 100) > (num_total_qam * p_dm_soml_table->qam16_dist_th)) && (p_dm_soml_table->cfo_diff_avg_a > p_dm_soml_table->cfo_qam16_th) && (p_dm_soml_table->cfo_diff_avg_b > p_dm_soml_table->cfo_qam16_th)) {
							PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[  QAM16_ratio > %d ; cfo_diff_avg_a > %d KHz ==> SOML_OFF]\n", p_dm_soml_table->qam16_dist_th, p_dm_soml_table->cfo_qam16_th));
							PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[ Final decisoin ] : "));
							phydm_soml_on_off(p_dm, SOML_OFF);
							return;
						} else if (((p_dm_soml_table->num_vht_qam[BPSK_QPSK] * 100) > (num_total_qam * p_dm_soml_table->bpsk_qpsk_dist_th)) && (p_dm_soml_table->cfo_diff_avg_a > p_dm_soml_table->cfo_qpsk_th) && (p_dm_soml_table->cfo_diff_avg_b > p_dm_soml_table->cfo_qpsk_th)) {
							PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[  BPSK_QPSK_ratio > %d ; cfo_diff_avg_a > %d KHz ==> SOML_OFF]\n", p_dm_soml_table->bpsk_qpsk_dist_th, p_dm_soml_table->cfo_qpsk_th));
							PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[ Final decisoin ] : "));
							phydm_soml_on_off(p_dm, SOML_OFF);
							return;
						}

						for (i = 0; i < rate_num; i++) {
							rate_ss_shift = 10 * i;
							PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[  num_vht_bytes_on  VHT-%d ss MCS[0:9] = {%d, %d, %d, %d, %d, %d, %d, %d, %d, %d} ]\n",
								(i + 1),
								p_dm_soml_table->num_vht_bytes_on[rate_ss_shift + 0], p_dm_soml_table->num_vht_bytes_on[rate_ss_shift + 1],
								p_dm_soml_table->num_vht_bytes_on[rate_ss_shift + 2], p_dm_soml_table->num_vht_bytes_on[rate_ss_shift + 3],
								p_dm_soml_table->num_vht_bytes_on[rate_ss_shift + 4], p_dm_soml_table->num_vht_bytes_on[rate_ss_shift + 5],
								p_dm_soml_table->num_vht_bytes_on[rate_ss_shift + 6], p_dm_soml_table->num_vht_bytes_on[rate_ss_shift + 7],
								p_dm_soml_table->num_vht_bytes_on[rate_ss_shift + 8], p_dm_soml_table->num_vht_bytes_on[rate_ss_shift + 9]));
						}

						for (i = 0; i < rate_num; i++) {
							rate_ss_shift = 10 * i;
							PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[  num_vht_bytes_off  VHT-%d ss MCS[0:9] = {%d, %d, %d, %d, %d, %d, %d, %d, %d, %d} ]\n",
								(i + 1),
								p_dm_soml_table->num_vht_bytes_off[rate_ss_shift + 0], p_dm_soml_table->num_vht_bytes_off[rate_ss_shift + 1],
								p_dm_soml_table->num_vht_bytes_off[rate_ss_shift + 2], p_dm_soml_table->num_vht_bytes_off[rate_ss_shift + 3],
								p_dm_soml_table->num_vht_bytes_off[rate_ss_shift + 4], p_dm_soml_table->num_vht_bytes_off[rate_ss_shift + 5],
								p_dm_soml_table->num_vht_bytes_off[rate_ss_shift + 6], p_dm_soml_table->num_vht_bytes_off[rate_ss_shift + 7],
								p_dm_soml_table->num_vht_bytes_off[rate_ss_shift + 8], p_dm_soml_table->num_vht_bytes_off[rate_ss_shift + 9]));
						}

						for (i = 0; i < VHT_RATE_IDX; i++) {
							byte_total_on += p_dm_soml_table->num_vht_bytes_on[i];
							byte_total_off += p_dm_soml_table->num_vht_bytes_off[i];
						}

					}

					/* [Decision] */
					PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[  byte_total_on = %d ; byte_total_off = %d ]\n", byte_total_on, byte_total_off));
					if (byte_total_on > byte_total_off) {
						next_on_off = SOML_ON;
						PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[ byte_total_on > byte_total_off ==> SOML_ON ]\n"));
					} else if (byte_total_on < byte_total_off) {
						next_on_off = SOML_OFF;
						PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[ byte_total_on < byte_total_off ==> SOML_OFF ]\n"));
					} else {
						PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[ stay at soml_last_state ]\n"));
						next_on_off = p_dm_soml_table->soml_last_state;
					}

					PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[ Final decisoin ] : "));
					phydm_soml_on_off(p_dm, next_on_off);
					p_dm_soml_table->soml_last_state = next_on_off;
				}
			} else {
				PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[escape from > TH_H || is_soml_method_enable==1]\n"));
				phydm_adaptive_soml_reset(p_dm);
				phydm_soml_on_off(p_dm, SOML_ON);
			}
		} else {
			PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[number_active_client != 1]\n"));
			phydm_adaptive_soml_reset(p_dm);
			phydm_soml_on_off(p_dm, SOML_OFF);
		}
	}
}

void
phydm_adaptive_soml_reset(
	void		*p_dm_void
)
{
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct adaptive_soml	*p_dm_soml_table = &p_dm->dm_soml_table;

	p_dm_soml_table->soml_state_cnt = 0;
	p_dm_soml_table->is_soml_method_enable = 0;
	p_dm_soml_table->soml_counter = 0;
}

#endif /* end of CONFIG_ADAPTIVE_SOML*/

void phydm_soml_bytes_acq(void *p_dm_void, u8 rate_id, u32 length)
{
#ifdef CONFIG_ADAPTIVE_SOML
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct adaptive_soml	*p_dm_soml_table = &p_dm->dm_soml_table;

	if ((rate_id >= ODM_RATEMCS0) && (rate_id <= ODM_RATEMCS31))
		p_dm_soml_table->num_ht_bytes[rate_id - ODM_RATEMCS0] += length;
	else if ((rate_id >= ODM_RATEVHTSS1MCS0) && (rate_id <= ODM_RATEVHTSS4MCS9))
		p_dm_soml_table->num_vht_bytes[rate_id - ODM_RATEVHTSS1MCS0] += length;

#endif
}

void phydm_adaptive_soml_timers(void *p_dm_void, u8 state)
{
#ifdef CONFIG_ADAPTIVE_SOML
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct adaptive_soml	*p_dm_soml_table = &p_dm->dm_soml_table;

	if (state == INIT_SOML_TIMMER) {
		odm_initialize_timer(p_dm, &p_dm_soml_table->phydm_adaptive_soml_timer,
			(void *)phydm_adaptive_soml_callback, NULL, "phydm_adaptive_soml_timer");
	} else if (state == CANCEL_SOML_TIMMER) {
		odm_cancel_timer(p_dm, &p_dm_soml_table->phydm_adaptive_soml_timer);
	} else if (state == RELEASE_SOML_TIMMER) {
		odm_release_timer(p_dm, &p_dm_soml_table->phydm_adaptive_soml_timer);
	}
#endif
}

void phydm_adaptive_soml_init(void *p_dm_void)
{
#ifdef CONFIG_ADAPTIVE_SOML
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct adaptive_soml	*p_dm_soml_table = &p_dm->dm_soml_table;

	if (!(p_dm->support_ability & ODM_BB_ADAPTIVE_SOML)) {
		PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[Return]   Not Support Adaptive SOML\n"));
		return;
	}
	PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("phydm_adaptive_soml_init\n"));

	p_dm_soml_table->soml_state_cnt = 0;
	p_dm_soml_table->soml_delay_time = 40;
	p_dm_soml_table->soml_intvl = 150;
	p_dm_soml_table->soml_train_num = 4;
	p_dm_soml_table->is_soml_method_enable = 0;
	p_dm_soml_table->soml_counter = 0;
	p_dm_soml_table->soml_period = 1;
	p_dm_soml_table->soml_select = 0;
	p_dm_soml_table->cfo_counter = 0;
	p_dm_soml_table->cfo_diff_sum_a = 0;
	p_dm_soml_table->cfo_diff_sum_b = 0;

	p_dm_soml_table->cfo_qpsk_th = 94;
	p_dm_soml_table->cfo_qam16_th  = 38;
	p_dm_soml_table->cfo_qam64_th = 17;
	p_dm_soml_table->cfo_qam256_th = 7;

	p_dm_soml_table->bpsk_qpsk_dist_th = 20;
	p_dm_soml_table->qam16_dist_th = 20;
	p_dm_soml_table->qam64_dist_th = 20;
	p_dm_soml_table->qam256_dist_th = 20;

	if (p_dm->support_ic_type == ODM_RTL8197F)
		odm_set_bb_reg(p_dm, BBREG_0x998, BIT(25), 1);
#endif
}

void phydm_adaptive_soml(void *p_dm_void)
{
#ifdef CONFIG_ADAPTIVE_SOML
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct adaptive_soml	*p_dm_soml_table = &p_dm->dm_soml_table;

	if (!(p_dm->support_ability & ODM_BB_ADAPTIVE_SOML)) {
		PHYDM_DBG(p_dm, DBG_ADPTV_SOML,
			("[Return!!!]   Not Support Adaptive SOML Function\n"));
		return;
	}

	if (p_dm_soml_table->soml_counter <  p_dm_soml_table->soml_period) {
		p_dm_soml_table->soml_counter++;
		return;
	}
	p_dm_soml_table->soml_counter = 0;
	p_dm_soml_table->soml_state_cnt = 0;
	p_dm_soml_table->cfo_counter = 0;
	p_dm_soml_table->cfo_diff_sum_a = 0;
	p_dm_soml_table->cfo_diff_sum_b = 0;

	phydm_soml_reset_rx_rate(p_dm);

	if (p_dm_soml_table->soml_select == 0) {
		PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[ Adaptive SOML Training !!!]\n"));
	} else if (p_dm_soml_table->soml_select == 1) {
		PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[ Stop Adaptive SOML !!!]\n"));
		phydm_soml_on_off(p_dm, SOML_ON);
		return;
	} else if (p_dm_soml_table->soml_select == 2) {
		PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[ Stop Adaptive SOML !!!]\n"));
		phydm_soml_on_off(p_dm, SOML_OFF);
		return;
	}

	phydm_adsl(p_dm);

#endif
}

void phydm_stop_adaptive_soml(void *p_dm_void)
{
#ifdef CONFIG_ADAPTIVE_SOML
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;

	PHYDM_DBG(p_dm, DBG_ADPTV_SOML, ("[%s][Return!!!]  Stop Adaptive SOML\n", __func__));
	p_dm->support_ability &= ~ODM_BB_ADAPTIVE_SOML;
	phydm_soml_on_off(p_dm, SOML_ON);
	
#endif
}

void phydm_adaptive_soml_para_set(void *p_dm_void, u8 train_num, u8 intvl,
				  u8 period, u8 delay_time

				  )
{
#ifdef CONFIG_ADAPTIVE_SOML
	struct PHY_DM_STRUCT		*p_dm = (struct PHY_DM_STRUCT *)p_dm_void;
	struct adaptive_soml	*p_dm_soml_table = &p_dm->dm_soml_table;

	p_dm_soml_table->soml_train_num = train_num;
	p_dm_soml_table->soml_intvl = intvl;
	p_dm_soml_table->soml_period = period;
	p_dm_soml_table->soml_delay_time = delay_time;
#endif
}

void phydm_init_soft_ml_setting(void *p_dm_void)
{
	struct PHY_DM_STRUCT *p_dm = (struct PHY_DM_STRUCT *)p_dm_void;

#if (RTL8822B_SUPPORT == 1)
	if (*p_dm->p_mp_mode == false) {
		if (p_dm->support_ic_type & ODM_RTL8822B)
			/*odm_set_bb_reg(p_dm, 0x19a8, MASKDWORD, 0xd10a0000);*/
			phydm_somlrxhp_setting(p_dm, true);
		p_dm->bsomlenabled = true;
	}
#endif
#if (RTL8821C_SUPPORT == 1)
	if (*p_dm->p_mp_mode == false) {
		if (p_dm->support_ic_type & ODM_RTL8821C)
			odm_set_bb_reg(p_dm, BBREG_0x19a8, BIT(31)|BIT(30)|BIT(29)|BIT(28), 0xd);
	}
#endif
}

