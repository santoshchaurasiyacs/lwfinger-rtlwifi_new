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
#include "mp_precomp.h"
#include "phydm_precomp.h"
#include <linux/module.h>
#include "../base.h"

static void rtl_hal_turbo_edca(struct ieee80211_hw *hw);
static void dump_sta_info(struct rtl_priv *rtlpriv, void *sel,
			  struct rtl_sta_info *sta_entry);

enum hal_status rtl_phydm_fw_iqk(void *adapter, u8 clear, u8 segment)
{
	struct rtl_priv *rtlpriv = (struct rtl_priv *)adapter;

	if (rtlpriv->halmac.ops->halmac_iqk(rtlpriv, clear, segment) == 0)
		return HAL_STATUS_SUCCESS;

	return HAL_STATUS_FAILURE;
}

static void rtl_hal_update_iqk_fw_offload_cap(struct rtl_priv *rtlpriv)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);
	struct rtl_hal *rtlhal = rtl_hal(rtlpriv);

	if (rtlhal->iqk_fw_offload)
		phydm_fwoffload_ability_init(dm, PHYDM_RF_IQK_OFFLOAD);
	else
		phydm_fwoffload_ability_clear(dm, PHYDM_RF_IQK_OFFLOAD);

	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG, "IQK FW offload:%s\n",
		 rtlhal->iqk_fw_offload ? "enable" : "disable");
}

static int _rtl_phydm_init_com_info(struct rtl_priv *rtlpriv,
				    enum odm_ic_type ic_type,
				    struct rtl_phydm_params *params)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);
	struct rtl_hal *rtlhal = rtl_hal(rtlpriv);
	struct rtl_phy *rtlphy = &rtlpriv->phy;
	struct rtl_mac *mac = rtl_mac(rtlpriv);
	struct rtl_ps_ctl *ppsc = rtl_psc(rtlpriv);
	struct rtl_efuse *rtlefuse = rtl_efuse(rtlpriv);
	u8 odm_board_type = ODM_BOARD_DEFAULT;
	int i;

	dm->adapter = (void *)rtlpriv;

	rtlpriv->phydm.phydm_op_mode = PHYDM_PERFORMANCE_MODE;

	odm_cmn_info_init(dm, ODM_CMNINFO_PLATFORM, ODM_CE);

	odm_cmn_info_init(dm, ODM_CMNINFO_IC_TYPE, ic_type);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG, "ODM_CMNINFO_IC_TYPE: %d\n",
		 ic_type);

	odm_cmn_info_init(dm, ODM_CMNINFO_INTERFACE, ODM_ITRF_PCIE);

	odm_cmn_info_init(dm, ODM_CMNINFO_MP_TEST_CHIP, params->mp_chip);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG,
		 "ODM_CMNINFO_MP_TEST_CHIP: %d\n", params->mp_chip);

	odm_cmn_info_init(dm, ODM_CMNINFO_PATCH_ID, rtlhal->oem_id);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG, "ODM_CMNINFO_PATCH_ID: %d\n",
		 rtlhal->oem_id);

	//odm_cmn_info_init(dm, ODM_CMNINFO_BWIFI_TEST, 1);
	odm_cmn_info_init(dm, ODM_CMNINFO_BWIFI_TEST, 0);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG, "ODM_CMNINFO_BWIFI_TEST: %d\n",
		 0);

	odm_cmn_info_init(dm, ODM_CMNINFO_ADVANCE_OTA, 0);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG,
		 "ODM_CMNINFO_ADVANCE_OTA: %d\n", 0);

	if (rtlphy->rf_type == RF_1T1R)
		odm_cmn_info_init(dm, ODM_CMNINFO_RF_TYPE, RF_1T1R);
	else if (rtlphy->rf_type == RF_1T2R)
		odm_cmn_info_init(dm, ODM_CMNINFO_RF_TYPE, RF_1T2R);
	else if (rtlphy->rf_type == RF_2T2R)
		odm_cmn_info_init(dm, ODM_CMNINFO_RF_TYPE, RF_2T2R);
	else if (rtlphy->rf_type == RF_2T2R_GREEN)
		odm_cmn_info_init(dm, ODM_CMNINFO_RF_TYPE, RF_2T2R_GREEN);
	else if (rtlphy->rf_type == RF_2T3R)
		odm_cmn_info_init(dm, ODM_CMNINFO_RF_TYPE, RF_2T3R);
	else if (rtlphy->rf_type == RF_2T4R)
		odm_cmn_info_init(dm, ODM_CMNINFO_RF_TYPE, RF_2T4R);
	else if (rtlphy->rf_type == RF_3T3R)
		odm_cmn_info_init(dm, ODM_CMNINFO_RF_TYPE, RF_3T3R);
	else if (rtlphy->rf_type == RF_3T4R)
		odm_cmn_info_init(dm, ODM_CMNINFO_RF_TYPE, RF_3T4R);
	else if (rtlphy->rf_type == RF_4T4R)
		odm_cmn_info_init(dm, ODM_CMNINFO_RF_TYPE, RF_4T4R);
	else
		RT_TRACE(rtlpriv, COMP_PHYDM, DBG_WARNING,
			 "rf_type=%d is not support!\n", rtlphy->rf_type);

	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG, "ODM_CMNINFO_RF_TYPE: %d\n",
		 rtlphy->rf_type);

	/* 1 ======= BoardType: ODM_CMNINFO_BOARD_TYPE ======= */
	if (rtlhal->external_lna_2g != 0) {
		odm_board_type |= ODM_BOARD_EXT_LNA;
		odm_cmn_info_init(dm, ODM_CMNINFO_EXT_LNA, 1);
		RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG,
			 "ODM_CMNINFO_EXT_LNA: %d\n", 1);
	}
	if (rtlhal->external_lna_5g != 0) {
		odm_board_type |= ODM_BOARD_EXT_LNA_5G;
		odm_cmn_info_init(dm, ODM_CMNINFO_5G_EXT_LNA, 1);
		RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG,
			 "ODM_CMNINFO_5G_EXT_LNA: %d\n", 1);
	}
	if (rtlhal->external_pa_2g != 0) {
		odm_board_type |= ODM_BOARD_EXT_PA;
		odm_cmn_info_init(dm, ODM_CMNINFO_EXT_PA, 1);
		RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG,
			 "ODM_CMNINFO_EXT_PA: %d\n", 1);
	}
	if (rtlhal->external_pa_5g != 0) {
		odm_board_type |= ODM_BOARD_EXT_PA_5G;
		odm_cmn_info_init(dm, ODM_CMNINFO_5G_EXT_PA, 1);
		RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG,
			 "ODM_CMNINFO_5G_EXT_PA: %d\n", 1);
	}
	if (rtlpriv->cfg->ops->get_btc_status())
		odm_board_type |= ODM_BOARD_BT;

	odm_cmn_info_init(dm, ODM_CMNINFO_BOARD_TYPE, odm_board_type);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG, "ODM_CMNINFO_BOARD_TYPE: %d\n",
		 odm_board_type);
	/* 1 ============== End of BoardType ============== */

	/* used to auto enable/disable adaptivity by SD7 */
	odm_cmn_info_init(dm, ODM_CMNINFO_DOMAIN_CODE_2G, 0);
	odm_cmn_info_init(dm, ODM_CMNINFO_DOMAIN_CODE_5G, 0);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG,
		 "ODM_CMNINFO_DOMAIN_CODE_2G: %d\n", 0);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG,
		 "ODM_CMNINFO_DOMAIN_CODE_5G: %d\n", 0);

#ifdef CONFIG_DFS_MASTER
	odm_cmn_info_init(dm, ODM_CMNINFO_DFS_REGION_DOMAIN,
			  adapter->registrypriv.dfs_region_domain);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG,"ODM_CMNINFO_DFS_REGION_DOMAIN: %d\n",
			adapter->registrypriv.dfs_region_domain);
	odm_cmn_info_hook(dm, ODM_CMNINFO_DFS_MASTER_ENABLE,
			  &adapter_to_rfctl(adapter)->dfs_master_enabled);
#endif

	odm_cmn_info_init(dm, ODM_CMNINFO_GPA, rtlhal->type_gpa);
	odm_cmn_info_init(dm, ODM_CMNINFO_APA, rtlhal->type_apa);
	odm_cmn_info_init(dm, ODM_CMNINFO_GLNA, rtlhal->type_glna);
	odm_cmn_info_init(dm, ODM_CMNINFO_ALNA, rtlhal->type_alna);

	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG, "ODM_CMNINFO_GPA: %d\n",
		 rtlhal->type_gpa);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG, "ODM_CMNINFO_APA: %d\n",
		 rtlhal->type_apa);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG, "ODM_CMNINFO_GLNA: %d\n",
		 rtlhal->type_glna);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG, "ODM_CMNINFO_ALNA: %d\n",
		 rtlhal->type_alna);

	odm_cmn_info_init(dm, ODM_CMNINFO_RFE_TYPE, rtlhal->rfe_type);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG, "ODM_CMNINFO_RFE_TYPE: %d\n",
		 rtlhal->rfe_type);

	odm_cmn_info_init(dm, ODM_CMNINFO_EXT_TRSW, 0);

	/*Add by YuChen for kfree init*/
	odm_cmn_info_init(dm, ODM_CMNINFO_REGRFKFREEENABLE, 2);
	odm_cmn_info_init(dm, ODM_CMNINFO_RFKFREEENABLE, 0);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG,
		 "ODM_CMNINFO_REGRFKFREEENABLE: %d\n", 2);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG,
		 "ODM_CMNINFO_RFKFREEENABLE: %d\n", 0);

	odm_cmn_info_init(dm, ODM_CMNINFO_RF_ANTENNA_TYPE,
			  rtlefuse->antenna_div_type);
	odm_cmn_info_init(dm, ODM_CMNINFO_BE_FIX_TX_ANT, 0);
	odm_cmn_info_init(dm, ODM_CMNINFO_WITH_EXT_ANTENNA_SWITCH, 0);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG,
		 "ODM_CMNINFO_RF_ANTENNA_TYPE: %d\n",
		 rtlefuse->antenna_div_type);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG,
		 "ODM_CMNINFO_BE_FIX_TX_ANT: %d\n", 0);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG,
		 "ODM_CMNINFO_WITH_EXT_ANTENNA_SWITCH: %d\n", 0);

	/* (8822B) efuse 0x3d7 & 0x3d8 for TX PA bias */
	odm_cmn_info_init(dm, ODM_CMNINFO_EFUSE0X3D7, params->efuse0x3d7);
	odm_cmn_info_init(dm, ODM_CMNINFO_EFUSE0X3D8, params->efuse0x3d8);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG, "ODM_CMNINFO_EFUSE0X3D7: %d\n",
		 0xF0);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG, "ODM_CMNINFO_EFUSE0X3D8: %d\n",
		 0xF0);

	/*Add by YuChen for adaptivity init*/
	odm_cmn_info_hook(dm, ODM_CMNINFO_ADAPTIVITY,
			  &rtlpriv->phydm.adaptivity_en);
	phydm_adaptivity_info_init(dm, PHYDM_ADAPINFO_CARRIER_SENSE_ENABLE,
				   false);
	phydm_adaptivity_info_init(dm, PHYDM_ADAPINFO_DCBACKOFF, 0);
	phydm_adaptivity_info_init(dm, PHYDM_ADAPINFO_DYNAMICLINKADAPTIVITY,
				   false);
	phydm_adaptivity_info_init(dm, PHYDM_ADAPINFO_TH_L2H_INI, 0);
	phydm_adaptivity_info_init(dm, PHYDM_ADAPINFO_TH_EDCCA_HL_DIFF, 0);

	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG, "ODM_CMNINFO_ADAPTIVITY: %d\n",
		 rtlpriv->phydm.adaptivity_en);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG,
		 "PHYDM_ADAPINFO_CARRIER_SENSE_ENABLE: %d\n", false);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG,
		 "PHYDM_ADAPINFO_DCBACKOFF: %d\n", 0);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG,
		 "PHYDM_ADAPINFO_DYNAMICLINKADAPTIVITY: %d\n", false);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG,
		 "PHYDM_ADAPINFO_TH_L2H_INI: %d\n", 0);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG,
		 "PHYDM_ADAPINFO_TH_EDCCA_HL_DIFF: %d\n", 0);

#if 0
	/*halrf info init*/
	halrf_cmn_info_init(dm, HALRF_CMNINFO_EEPROM_THERMAL_VALUE, pHalData->eeprom_thermal_meter);
	halrf_cmn_info_init(dm, HALRF_CMNINFO_FW_VER,
		((pHalData->firmware_version << 16) | pHalData->firmware_sub_version));
#endif

#ifdef CONFIG_IQK_PA_OFF
	odm_cmn_info_init(dm, ODM_CMNINFO_IQKPAOFF, 1);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG, "ODM_CMNINFO_IQKPAOFF: %d\n",
		 1);
#endif

	rtl_hal_update_iqk_fw_offload_cap(rtlpriv);

	/* Pointer reference */
	/*Antenna diversity relative parameters*/
	odm_cmn_info_hook(dm, ODM_CMNINFO_ANT_DIV, &rtlefuse->antenna_div_cfg);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG, "ODM_CMNINFO_ANT_DIV: %d\n",
		 rtlefuse->antenna_div_cfg);

	odm_cmn_info_hook(dm, ODM_CMNINFO_MP_MODE, &rtlpriv->mp_mode);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG, "ODM_CMNINFO_MP_MODE: %d\n",
		 rtlpriv->mp_mode);

	odm_cmn_info_hook(dm, ODM_CMNINFO_BB_OPERATION_MODE,
			  &rtlpriv->phydm.phydm_op_mode);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG,
		 "ODM_CMNINFO_BB_OPERATION_MODE: %d\n",
		 rtlpriv->phydm.phydm_op_mode);
	odm_cmn_info_hook(dm, ODM_CMNINFO_TX_UNI,
			  &rtlpriv->stats.txbytesunicast);
	odm_cmn_info_hook(dm, ODM_CMNINFO_RX_UNI,
			  &rtlpriv->stats.rxbytesunicast);

	odm_cmn_info_hook(dm, ODM_CMNINFO_BAND, &rtlhal->current_bandtype);
	odm_cmn_info_hook(dm, ODM_CMNINFO_FORCED_RATE,
			  &rtlpriv->phydm.forced_data_rate);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG,
		 "ODM_CMNINFO_FORCED_RATE: %d\n",
		 rtlpriv->phydm.forced_data_rate);

	odm_cmn_info_hook(dm, ODM_CMNINFO_SEC_CHNL_OFFSET,
			  &mac->cur_40_prime_sc);
#if 0
	odm_cmn_info_hook(dm, ODM_CMNINFO_SEC_MODE,
			  &adapter->securitypriv.dot11PrivacyAlgrthm);
#endif
	odm_cmn_info_hook(dm, ODM_CMNINFO_BW, &rtlphy->current_chan_bw);
	odm_cmn_info_hook(dm, ODM_CMNINFO_CHNL, &rtlphy->current_channel);
#if 0
	odm_cmn_info_hook(dm, ODM_CMNINFO_NET_CLOSED, &adapter->net_closed);
#endif

	odm_cmn_info_hook(dm, ODM_CMNINFO_SCAN, &mac->act_scanning);
	odm_cmn_info_hook(dm, ODM_CMNINFO_POWER_SAVING,
			  &ppsc->dot11_psmode); /* may add new boolean flag */
	/*Add by Yuchen for phydm beamforming*/
	odm_cmn_info_hook(dm, ODM_CMNINFO_TX_TP,
			  &rtlpriv->stats.txbytesunicast_inperiod_tp);
	odm_cmn_info_hook(dm, ODM_CMNINFO_RX_TP,
			  &rtlpriv->stats.rxbytesunicast_inperiod_tp);
	odm_cmn_info_hook(dm, ODM_CMNINFO_ANT_TEST,
			  &rtlpriv->phydm.antenna_test);
#ifdef CONFIG_USB_HCI
	odm_cmn_info_hook(dm, ODM_CMNINFO_HUBUSBMODE, &dvobj->usb_speed);
#endif

#ifdef CONFIG_MP_INCLUDED
	halrf_cmn_info_hook(dm, HALRF_CMNINFO_CON_TX, NULL);
	halrf_cmn_info_hook(dm, HALRF_CMNINFO_SINGLE_TONE, NULL);
	halrf_cmn_info_hook(dm, HALRF_CMNINFO_CARRIER_SUPPRESSION, NULL);
	halrf_cmn_info_hook(dm, HALRF_CMNINFO_MP_RATE_INDEX, NULL);
#endif/*CONFIG_MP_INCLUDED*/

	for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++)
		odm_cmn_info_ptr_array_hook(dm, ODM_CMNINFO_STA_STATUS, i,
					    NULL);

	phydm_init_debug_setting(dm);

	odm_cmn_info_init(dm, ODM_CMNINFO_FAB_VER, params->fab_ver);
	odm_cmn_info_init(dm, ODM_CMNINFO_CUT_VER, params->cut_ver);

	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG, "ODM_CMNINFO_FAB_VER, %d\n",
		 params->fab_ver);
	RT_TRACE(rtlpriv, COMP_PHYDM, DBG_DMESG, "ODM_CMNINFO_CUT_VER, %d\n",
		 params->cut_ver);

	return 0;
}

void rtl_odm_acquirespinlock(struct rtl_priv *rtlpriv,
			     enum rt_spinlock_type type)
{
	switch (type) {
	case RT_IQK_SPINLOCK:
		spin_lock(&rtlpriv->locks.iqk_lock);
	default:
		break;
	}
}

void rtl_odm_releasespinlock(struct rtl_priv *rtlpriv,
			     enum rt_spinlock_type type)
{
	switch (type) {
	case RT_IQK_SPINLOCK:
		spin_unlock(&rtlpriv->locks.iqk_lock);
	default:
		break;
	}
}

static int rtl_phydm_init_priv(struct rtl_priv *rtlpriv,
			       struct rtl_phydm_params *params)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);
	enum odm_ic_type ic;

	if (IS_HARDWARE_TYPE_8822B(rtlpriv))
		ic = ODM_RTL8822B;
	else if (IS_HARDWARE_TYPE_8723D(rtlpriv))
		ic = ODM_RTL8723D;
	else
		return 0;

	rtlpriv->phydm.internal =
		kzalloc(sizeof(struct phy_dm_struct), GFP_KERNEL);

	_rtl_phydm_init_com_info(rtlpriv, ic, params);

	odm_init_all_timers(dm);

	return 1;
}

static int rtl_phydm_deinit_priv(struct rtl_priv *rtlpriv)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);

	odm_cancel_all_timers(dm);

	kfree(rtlpriv->phydm.internal);
	rtlpriv->phydm.internal = NULL;

	return 0;
}

static bool rtl_phydm_load_txpower_by_rate(struct rtl_priv *rtlpriv)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);
	enum hal_status status;

	status = odm_config_bb_with_header_file(dm, CONFIG_BB_PHY_REG_PG);
	if (status != HAL_STATUS_SUCCESS)
		return false;

	return true;
}

static bool rtl_phydm_load_txpower_limit(struct rtl_priv *rtlpriv)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);
	enum hal_status status;

	if (IS_HARDWARE_TYPE_8822B(rtlpriv)) {
		odm_read_and_config_mp_8822b_txpwr_lmt(dm);
	} else {
		status = odm_config_rf_with_header_file(dm, CONFIG_RF_TXPWR_LMT,
							0);
		if (status != HAL_STATUS_SUCCESS)
			return false;
	}

	return true;
}

static int rtl_phydm_init_dm(struct rtl_priv *rtlpriv)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);

	odm_dm_init(dm);

	return 0;
}

static int rtl_phydm_deinit_dm(struct rtl_priv *rtlpriv) { return 0; }

static int rtl_phydm_reset_dm(struct rtl_priv *rtlpriv)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);

	odm_dm_reset(dm);

	return 0;
}

static bool rtl_phydm_parameter_init(struct rtl_priv *rtlpriv, bool post)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);

	if (IS_HARDWARE_TYPE_8822B(rtlpriv))
		return config_phydm_parameter_init_8822b(
			dm, post ? ODM_POST_SETTING : ODM_PRE_SETTING);

	return false;
}

static bool rtl_phydm_phy_bb_config(struct rtl_priv *rtlpriv)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);
	enum hal_status status;

	status = odm_config_bb_with_header_file(dm, CONFIG_BB_PHY_REG);
	if (status != HAL_STATUS_SUCCESS)
		return false;

	status = odm_config_bb_with_header_file(dm, CONFIG_BB_AGC_TAB);
	if (status != HAL_STATUS_SUCCESS)
		return false;

	return true;
}

static bool rtl_phydm_phy_rf_config(struct rtl_priv *rtlpriv)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);
	struct rtl_phy *rtlphy = &rtlpriv->phy;
	enum hal_status status;
	enum rf_path rfpath;

	for (rfpath = 0; rfpath < rtlphy->num_total_rfpath; rfpath++) {
		status = odm_config_rf_with_header_file(dm, CONFIG_RF_RADIO,
							rfpath);
		if (status != HAL_STATUS_SUCCESS)
			return false;
	}

	status = odm_config_rf_with_tx_pwr_track_header_file(dm);
	if (status != HAL_STATUS_SUCCESS)
		return false;

	return true;
}

static bool rtl_phydm_phy_mac_config(struct rtl_priv *rtlpriv)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);
	enum hal_status status;

	status = odm_config_mac_with_header_file(dm);
	if (status != HAL_STATUS_SUCCESS)
		return false;

	return true;
}

static bool rtl_phydm_trx_mode(struct rtl_priv *rtlpriv,
			       enum radio_mask tx_path, enum radio_mask rx_path,
			       bool is_tx2_path)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);

	if (IS_HARDWARE_TYPE_8822B(rtlpriv))
		return config_phydm_trx_mode_8822b(dm, (enum rf_type)tx_path,
						   (enum rf_type)rx_path,
						   is_tx2_path);

	return false;
}

static u8 _rtl_phydm_rfk_condition_check(struct rtl_priv *rtlpriv)
{
	u8 rst = false;

	/*ToDo: check */
	rst = true;

	return rst;
}

static u8 _rtl_phydm_iqk_segment_chk(struct rtl_priv *rtlpriv)
{
	u8 rst = false;

	/*ToDo: check */
	rst = true;

	return rst;
}

static u8 rtl_phydm_is_iqk_in_progress(struct rtl_priv *rtlpriv)
{
	u8 rts = false;
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);

	odm_acquire_spin_lock(dm, RT_IQK_SPINLOCK);
	if (dm->rf_calibrate_info.is_iqk_in_progress == true) {
		RT_TRACE(rtlpriv, COMP_IQK, DBG_DMESG, "IQK is InProgress!\n");
		rts = true;
	}
	odm_release_spin_lock(dm, RT_IQK_SPINLOCK);

	return rts;
}

static bool rtl_phydm_watchdog(struct rtl_priv *rtlpriv)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);
	struct rtl_mac *mac = rtl_mac(rtlpriv);
	struct rtl_ps_ctl *ppsc = rtl_psc(rtlpriv);
	bool fw_current_inpsmode = false;
	bool fw_ps_awake = true;
	u8 is_linked = false;
	u8 bsta_state = false;
	u8 is_bt_enabled = false;
	u8 rfk_forbidden = true;
	u8 segment_iqk = true;

	/* check whether do watchdog */
	rtlpriv->cfg->ops->get_hw_reg(rtlpriv->hw, HW_VAR_FW_PSMODE_STATUS,
				      (u8 *)(&fw_current_inpsmode));
	rtlpriv->cfg->ops->get_hw_reg(rtlpriv->hw, HW_VAR_FWLPS_RF_ON,
				      (u8 *)(&fw_ps_awake));
	if (ppsc->p2p_ps_info.p2p_ps_mode)
		fw_ps_awake = false;

	if ((ppsc->rfpwr_state == ERFON) &&
	    ((!fw_current_inpsmode) && fw_ps_awake) &&
	    (!ppsc->rfchange_inprogress))
		;
	else
		return false;

	/* update common info before doing watchdog */
	if (mac->link_state >= MAC80211_LINKED) {
		is_linked = true;
		if (mac->vif && mac->vif->type == NL80211_IFTYPE_STATION)
			bsta_state = true;
	}

	if (rtlpriv->cfg->ops->get_btc_status())
		is_bt_enabled = !rtlpriv->btcoexist.btc_ops->btc_is_bt_disabled(
			rtlpriv);

	odm_cmn_info_update(dm, ODM_CMNINFO_LINK, is_linked);
	odm_cmn_info_update(dm, ODM_CMNINFO_STATION_STATE, bsta_state);
	odm_cmn_info_update(dm, ODM_CMNINFO_BT_ENABLED, is_bt_enabled);
	odm_cmn_info_update(dm, ODM_CMNINFO_POWER_TRAINING, false);

	if (is_linked == true) {
		/*Add check to prevent the iqk parameters be changed
		  * during IQK progress.
		  */
		if (rtl_phydm_is_iqk_in_progress(rtlpriv) == false) {
			rfk_forbidden =
				_rtl_phydm_rfk_condition_check(rtlpriv) ?
					false :
					true;
			halrf_cmn_info_set(dm, HALRF_CMNINFO_RFK_FORBIDDEN,
					   rfk_forbidden);

			if (IS_HARDWARE_TYPE_8822B(rtlpriv)) {
				segment_iqk =
					_rtl_phydm_iqk_segment_chk(rtlpriv);
				halrf_cmn_info_set(dm,
						   HALRF_CMNINFO_IQK_SEGMENT,
						   segment_iqk);
			}
		}
	}

	/* do watchdog */
	phydm_watchdog(dm);
	rtl_hal_turbo_edca(rtlpriv->hw);

	return true;
}

static bool rtl_phydm_switch_band(struct rtl_priv *rtlpriv, u8 central_ch)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);

	if (IS_HARDWARE_TYPE_8822B(rtlpriv))
		return config_phydm_switch_band_8822b(dm, central_ch);

	return false;
}

static bool rtl_phydm_switch_channel(struct rtl_priv *rtlpriv, u8 central_ch)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);

	if (IS_HARDWARE_TYPE_8822B(rtlpriv))
		return config_phydm_switch_channel_8822b(dm, central_ch);

	return false;
}

static bool rtl_phydm_switch_bandwidth(struct rtl_priv *rtlpriv,
				       u8 primary_ch_idx,
				       enum ht_channel_width bandwidth)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);
	enum channel_width odm_bw = (enum channel_width)bandwidth;

	if (IS_HARDWARE_TYPE_8822B(rtlpriv))
		return config_phydm_switch_bandwidth_8822b(dm, primary_ch_idx,
							   odm_bw);

	return false;
}

static bool rtl_phydm_lq_calibrate(struct rtl_priv *rtlpriv)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);

	halrf_lck_trigger(dm);

	return true;
}

static bool rtl_phydm_iq_calibrate(struct rtl_priv *rtlpriv)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);
	u8 clear = false;
	u8 segment = false;
	u8 rfk_forbidden = false;

	if (rtl_phydm_is_iqk_in_progress(rtlpriv))
		RT_TRACE(rtlpriv, COMP_IQK, DBG_WARNING,
			 "%s, line%d, IQK may race condition!\n", __func__,
			 __LINE__);

	if (IS_HARDWARE_TYPE_8822B(rtlpriv)) {
		/*phy_iq_calibrate_8822b(dm, false);*/
		/* halrf_iqk_trigger(dm, false);*/
		halrf_cmn_info_set(dm, HALRF_CMNINFO_RFK_FORBIDDEN,
				   rfk_forbidden);
		halrf_cmn_info_set(dm, HALRF_CMNINFO_IQK_SEGMENT, segment);
		halrf_segment_iqk_trigger(dm, clear, segment);
	} else {
		halrf_iqk_trigger(dm, false);
	}

	return true;
}

static bool rtl_phydm_clear_txpowertracking_state(struct rtl_priv *rtlpriv)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);

	odm_clear_txpowertracking_state(dm);

	return true;
}

static bool rtl_phydm_pause_dig(struct rtl_priv *rtlpriv, bool pause)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);

	if (pause)
		odm_pause_dig(dm, PHYDM_PAUSE, PHYDM_PAUSE_LEVEL_0, 0x1e);
	else /* resume */
		odm_pause_dig(dm, PHYDM_RESUME, PHYDM_PAUSE_LEVEL_0, 0xff);

	return true;
}

static u32 rtl_phydm_read_rf_reg(struct rtl_priv *rtlpriv,
				 enum radio_path rfpath, u32 addr, u32 mask)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);
	enum rf_path odm_rfpath = (enum rf_path)rfpath;

	if (IS_HARDWARE_TYPE_8822B(rtlpriv))
		return config_phydm_read_rf_reg_8822b(dm, odm_rfpath, addr,
						      mask);

	return -1;
}

static bool rtl_phydm_write_rf_reg(struct rtl_priv *rtlpriv,
				   enum radio_path rfpath, u32 addr, u32 mask,
				   u32 data)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);
	enum rf_path odm_rfpath = (enum rf_path)rfpath;

	if (IS_HARDWARE_TYPE_8822B(rtlpriv))
		return config_phydm_write_rf_reg_8822b(dm, odm_rfpath, addr,
						       mask, data);

	return false;
}

static u8 rtl_phydm_read_txagc(struct rtl_priv *rtlpriv, enum radio_path rfpath,
			       u8 hw_rate)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);
	enum rf_path odm_rfpath = (enum rf_path)rfpath;

	if (IS_HARDWARE_TYPE_8822B(rtlpriv))
		return config_phydm_read_txagc_8822b(dm, odm_rfpath, hw_rate);

	return -1;
}

static bool rtl_phydm_write_txagc(struct rtl_priv *rtlpriv, u32 power_index,
				  enum radio_path rfpath, u8 hw_rate)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);
	enum rf_path odm_rfpath = (enum rf_path)rfpath;

	if (IS_HARDWARE_TYPE_8822B(rtlpriv))
		return config_phydm_write_txagc_8822b(dm, power_index,
						      odm_rfpath, hw_rate);

	return false;
}

static bool rtl_phydm_c2h_content_parsing(struct rtl_priv *rtlpriv, u8 cmd_id,
					  u8 cmd_len, u8 *content)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);

	if (phydm_c2H_content_parsing(dm, cmd_id, cmd_len, content))
		return true;

	return false;
}

static bool rtl_phydm_query_phy_status(struct rtl_priv *rtlpriv, u8 *phystrpt,
				       struct ieee80211_hdr *hdr,
				       struct rtl_stats *pstatus)
{
/* NOTE: phystrpt may be NULL, and need to fill default value */

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 5, 0))
#define ether_addr_equal(a, b)	(compare_ether_addr(a, b) == 0)
#endif
#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 14, 0))
#define ether_addr_copy(to, from) memcpy(to, from, ETH_ALEN)
#define ether_addr_equal_unaligned(a1, a2) (compare_ether_addr(a1, a2) == 0)
#endif
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);
	struct rtl_efuse *rtlefuse = rtl_efuse(rtlpriv);
	struct rtl_mac *mac = rtl_mac(rtlpriv);
	struct dm_per_pkt_info pktinfo; /* input of pydm */
	struct dm_phy_status_info phy_info; /* output of phydm */
	__le16 fc = hdr->frame_control;

	/* fill driver pstatus */
	ether_addr_copy(pstatus->psaddr, ieee80211_get_SA(hdr));

	/* fill pktinfo */
	memset(&pktinfo, 0, sizeof(pktinfo));

	pktinfo.data_rate = pstatus->rate;
	pktinfo.ppdu_cnt = pstatus->ppdu_cnt;

	if (rtlpriv->mac80211.opmode == NL80211_IFTYPE_STATION) {
		pktinfo.station_id = 0;
	} else {
		/* TODO: use rtl_find_sta() to find ID */
		pktinfo.station_id = 0xFF;
	}

	pktinfo.is_packet_match_bssid =
		(!ieee80211_is_ctl(fc) &&
		 (ether_addr_equal(mac->bssid,
				   ieee80211_has_tods(fc) ?
					   hdr->addr1 :
					   ieee80211_has_fromds(fc) ?
					   hdr->addr2 :
					   hdr->addr3)) &&
		 (!pstatus->hwerror) && (!pstatus->crc) && (!pstatus->icv));
	pktinfo.is_packet_to_self =
		pktinfo.is_packet_match_bssid &&
		(ether_addr_equal(hdr->addr1, rtlefuse->dev_addr));
	pktinfo.is_to_self = (!pstatus->icv) && (!pstatus->crc) &&
			     (ether_addr_equal(hdr->addr1, rtlefuse->dev_addr));
	pktinfo.is_packet_beacon = (ieee80211_is_beacon(fc) ? true : false);

	/* query phy status */
	if (phystrpt)
		odm_phy_status_query(dm, &phy_info, phystrpt, &pktinfo);
	else
		memset(&phy_info, 0, sizeof(phy_info));

	/* copy phy_info from phydm to driver */
	pstatus->rx_pwdb_all = phy_info.rx_pwdb_all;
	pstatus->bt_rx_rssi_percentage = phy_info.bt_rx_rssi_percentage;
	pstatus->recvsignalpower = phy_info.recv_signal_power;
	pstatus->signalquality = phy_info.signal_quality;
	pstatus->rx_mimo_signalquality[0] = phy_info.rx_mimo_signal_quality[0];
	pstatus->rx_mimo_signalquality[1] = phy_info.rx_mimo_signal_quality[1];
	pstatus->rx_packet_bw =
		phy_info.band_width; /* HT_CHANNEL_WIDTH_20 <- ODM_BW20M */

	/* fill driver pstatus */
	pstatus->packet_matchbssid = pktinfo.is_packet_match_bssid;
	pstatus->packet_toself = pktinfo.is_packet_to_self;
	pstatus->packet_beacon = pktinfo.is_packet_beacon;

	return true;
}

static u8 rtl_phydm_rate_id_mapping(struct rtl_priv *rtlpriv,
				    enum wireless_mode wireless_mode,
				    enum rf_type rf_type,
				    enum ht_channel_width bw)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);

	return phydm_rate_id_mapping(dm, wireless_mode, rf_type, bw);
}

static bool rtl_phydm_get_ra_bitmap(struct rtl_priv *rtlpriv,
				    enum wireless_mode wireless_mode,
				    enum rf_type rf_type,
				    enum ht_channel_width bw,
				    u8 tx_rate_level, /* 0~6 */
				    u32 *tx_bitmap_msb, u32 *tx_bitmap_lsb)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);
	const u8 mimo_ps_enable = 0;
	const u8 disable_cck_rate = 0;

	phydm_update_hal_ra_mask(dm, wireless_mode, rf_type, bw, mimo_ps_enable,
				 disable_cck_rate, tx_bitmap_msb, tx_bitmap_lsb,
				 tx_rate_level);

	return true;
}

static u8 _rtl_phydm_get_macid(struct rtl_priv *rtlpriv,
			       struct ieee80211_sta *sta)
{
	struct rtl_mac *mac = rtl_mac(rtlpriv);

	if (mac->opmode == NL80211_IFTYPE_STATION ||
	    mac->opmode == NL80211_IFTYPE_MESH_POINT) {
		return 0;
	} else if (mac->opmode == NL80211_IFTYPE_AP ||
		   mac->opmode == NL80211_IFTYPE_ADHOC)
		return sta->aid + 1;

	return 0;
}

static bool rtl_phydm_add_sta(struct rtl_priv *rtlpriv,
			      struct ieee80211_sta *sta)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);
	struct rtl_sta_info *sta_entry = (struct rtl_sta_info *)sta->drv_priv;
	u8 mac_id = _rtl_phydm_get_macid(rtlpriv, sta);

	odm_cmn_info_ptr_array_hook(dm, ODM_CMNINFO_STA_STATUS, mac_id,
				    sta_entry);

	/* init for DM */
	sta_entry->cmn_info.rssi_stat.rssi = (-1);
	sta_entry->cmn_info.rssi_stat.rssi_cck = (-1);
	sta_entry->cmn_info.rssi_stat.rssi_ofdm = (-1);

	sta_entry->cmn_info.mac_id = mac_id;
	sta_entry->cmn_info.dm_ctrl = STA_DM_CTRL_ACTIVE;
	sta_entry->cmn_info.sm_ps =
		SM_PS_DISABLE; /*Spatial Multiplexing Power Save*/

	phydm_cmn_sta_info_hook(dm, mac_id, &sta_entry->cmn_info);

	/* ra_info.is_vht_enable is set in rtlpriv->cfg->ops->update_rate_tb() */
	/* ra_info.is_support_sgi is set in rtlpriv->cfg->ops->update_rate_tb() */
	/* ra_info.ramask is set in rtlpriv->cfg->ops->update_rate_tb() */

	/* cmn.support_wireless_set is set in rtlpriv->cfg->ops->update_rate_tb() */
	/* cmn.mimo_type is set in rtlpriv->cfg->ops->update_rate_tb() */
	/* cmn.bw_mode is set in rtl_op_sta_add(), and rtl_op_bss_info_changed */

	/* cmn.stbc_en is hard code to disable in rtl_op_sta_add() */
	/* cmn.ldpc_en is hard code to disable in rtl_op_sta_add() */

	return true;
}

static void rtl_phydm_ra_registered(struct rtl_priv *rtlpriv,
				    struct ieee80211_sta *sta)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);
	struct rtl_sta_info *sta_entry = (struct rtl_sta_info *)sta->drv_priv;

	sta_entry->cmn_info.ra_info.ra_bw_mode = sta_entry->cmn_info.bw_mode;
#ifdef USE_ORIGINAL_RA
/* Do nothing for original RA */
#else
	phydm_ra_registed(dm, sta_entry->cmn_info.mac_id,
			  sta_entry->cmn_info.rssi_stat.rssi);
#endif

	dump_sta_info(rtlpriv, 0, sta_entry);
}

static bool rtl_phydm_del_sta(struct rtl_priv *rtlpriv,
			      struct ieee80211_sta *sta)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);
	struct rtl_sta_info *sta_entry = (struct rtl_sta_info *)sta->drv_priv;
	u8 mac_id = _rtl_phydm_get_macid(rtlpriv, sta);

	sta_entry->cmn_info.ra_info.rssi_level = 0;
	sta_entry->cmn_info.dm_ctrl = 0;
	odm_cmn_info_ptr_array_hook(dm, ODM_CMNINFO_STA_STATUS, mac_id, NULL);
	phydm_cmn_sta_info_hook(dm, mac_id, NULL);

	return true;
}

static u32 rtl_phydm_get_version(struct rtl_priv *rtlpriv)
{
	u32 ver = 0;

	if (IS_HARDWARE_TYPE_8822B(rtlpriv))
		ver = RELEASE_VERSION_8822B;

	return ver;
}

static bool rtl_phydm_modify_ra_pcr_threshold(struct rtl_priv *rtlpriv,
					      u8 ra_offset_direction,
					      u8 ra_threshold_offset)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);

	phydm_modify_RA_PCR_threshold(dm, ra_offset_direction,
				      ra_threshold_offset);

	return true;
}

static u32 rtl_phydm_query_counter(struct rtl_priv *rtlpriv,
				   const char *info_type)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);

	if (!strcmp(info_type, "IQK_TOTAL"))
		return dm->n_iqk_cnt;

	if (!strcmp(info_type, "IQK_OK"))
		return dm->n_iqk_ok_cnt;

	if (!strcmp(info_type, "IQK_FAIL"))
		return dm->n_iqk_fail_cnt;

	if (!strcmp(info_type, "PHYDM_INFO_FA_OFDM"))
		return phydm_cmn_info_query(dm, PHYDM_INFO_FA_OFDM);

	if (!strcmp(info_type, "PHYDM_INFO_FA_CCK"))
		return phydm_cmn_info_query(dm, PHYDM_INFO_FA_CCK);

	if (!strcmp(info_type, "PHYDM_INFO_CCA_OFDM"))
		return phydm_cmn_info_query(dm, PHYDM_INFO_CCA_OFDM);

	if (!strcmp(info_type, "PHYDM_INFO_CCA_CCK"))
		return phydm_cmn_info_query(dm, PHYDM_INFO_CCA_CCK);

	return 0xDEADDEAD;
}

static bool rtl_phydm_debug_cmd(struct rtl_priv *rtlpriv, char *in, u32 in_len,
				char *out, u32 out_len)
{
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);

	phydm_cmd(dm, in, in_len, 1, out, out_len);

	return true;
}

static void rtl_phydm_dump_sta_info(struct rtl_priv *rtlpriv, void *sel,
				    struct rtl_sta_info *sta_entry)
{
	dump_sta_info(rtlpriv, sel, sta_entry);
}

static struct rtl_phydm_ops rtl_phydm_operation = {
	/* init/deinit priv */
	.phydm_init_priv = rtl_phydm_init_priv,
	.phydm_deinit_priv = rtl_phydm_deinit_priv,
	.phydm_load_txpower_by_rate = rtl_phydm_load_txpower_by_rate,
	.phydm_load_txpower_limit = rtl_phydm_load_txpower_limit,

	/* init hw */
	.phydm_init_dm = rtl_phydm_init_dm,
	.phydm_deinit_dm = rtl_phydm_deinit_dm,
	.phydm_reset_dm = rtl_phydm_reset_dm,
	.phydm_parameter_init = rtl_phydm_parameter_init,
	.phydm_phy_bb_config = rtl_phydm_phy_bb_config,
	.phydm_phy_rf_config = rtl_phydm_phy_rf_config,
	.phydm_phy_mac_config = rtl_phydm_phy_mac_config,
	.phydm_trx_mode = rtl_phydm_trx_mode,

	/* watchdog */
	.phydm_watchdog = rtl_phydm_watchdog,

	/* channel */
	.phydm_switch_band = rtl_phydm_switch_band,
	.phydm_switch_channel = rtl_phydm_switch_channel,
	.phydm_switch_bandwidth = rtl_phydm_switch_bandwidth,
	.phydm_lc_calibrate = rtl_phydm_lq_calibrate,
	.phydm_iq_calibrate = rtl_phydm_iq_calibrate,
	.phydm_clear_txpowertracking_state =
		rtl_phydm_clear_txpowertracking_state,
	.phydm_pause_dig = rtl_phydm_pause_dig,

	/* read/write reg */
	.phydm_read_rf_reg = rtl_phydm_read_rf_reg,
	.phydm_write_rf_reg = rtl_phydm_write_rf_reg,
	.phydm_read_txagc = rtl_phydm_read_txagc,
	.phydm_write_txagc = rtl_phydm_write_txagc,

	/* RX */
	.phydm_c2h_content_parsing = rtl_phydm_c2h_content_parsing,
	.phydm_query_phy_status = rtl_phydm_query_phy_status,

	/* TX */
	.phydm_rate_id_mapping = rtl_phydm_rate_id_mapping,
	.phydm_get_ra_bitmap = rtl_phydm_get_ra_bitmap,
	.phydm_ra_registered = rtl_phydm_ra_registered,

	/* STA */
	.phydm_add_sta = rtl_phydm_add_sta,
	.phydm_del_sta = rtl_phydm_del_sta,

	/* BTC */
	.phydm_get_version = rtl_phydm_get_version,
	.phydm_modify_ra_pcr_threshold = rtl_phydm_modify_ra_pcr_threshold,
	.phydm_query_counter = rtl_phydm_query_counter,

	/* debug */
	.phydm_debug_cmd = rtl_phydm_debug_cmd,
	.phydm_dump_sta_info = rtl_phydm_dump_sta_info,
};

struct rtl_phydm_ops *rtl_phydm_get_ops_pointer(void)
{
	return &rtl_phydm_operation;
}
EXPORT_SYMBOL(rtl_phydm_get_ops_pointer);

/* ********************************************************
 * Define phydm callout function in below
 * ********************************************************
 */

u8 phy_get_tx_power_index(void *adapter, u8 rf_path, u8 rate,
			  enum ht_channel_width bandwidth, u8 channel)
{
	/* rate: DESC_RATE1M */
	struct rtl_priv *rtlpriv = (struct rtl_priv *)adapter;

	return rtlpriv->cfg->ops->get_txpower_index(rtlpriv->hw, rf_path, rate,
						    bandwidth, channel);
}

void phy_set_tx_power_index_by_rs(void *adapter, u8 ch, u8 path, u8 rs)
{
	struct rtl_priv *rtlpriv = (struct rtl_priv *)adapter;

	return rtlpriv->cfg->ops->set_tx_power_index_by_rs(rtlpriv->hw, ch,
							   path, rs);
}

void phy_store_tx_power_by_rate(void *adapter, u32 band, u32 rfpath, u32 txnum,
				u32 regaddr, u32 bitmask, u32 data)
{
	struct rtl_priv *rtlpriv = (struct rtl_priv *)adapter;

	rtlpriv->cfg->ops->store_tx_power_by_rate(
		rtlpriv->hw, band, rfpath, txnum, regaddr, bitmask, data);
}

void phy_set_tx_power_limit(void *dm, u8 *regulation, u8 *band, u8 *bandwidth,
			    u8 *rate_section, u8 *rf_path, u8 *channel,
			    u8 *power_limit)
{
	struct rtl_priv *rtlpriv =
		(struct rtl_priv *)((struct phy_dm_struct *)dm)->adapter;

	rtlpriv->cfg->ops->phy_set_txpower_limit(rtlpriv->hw, regulation, band,
						 bandwidth, rate_section,
						 rf_path, channel, power_limit);
}

#if 0 /* phydm v21 will not call this API to update RA mask. */
void rtw_phydm_update_ra_mask(struct cmn_sta_info *pcmn_info, u8 rssi_level, u8 is_update_bw)
{
	struct rtl_priv *rtlpriv;
	struct rtl_sta_info *rtl_sta =
		container_of((void *)pcmn_info, struct rtl_sta_info, cmn_info);
	struct ieee80211_sta *sta =
		container_of((void *)rtl_sta, struct ieee80211_sta, drv_priv);

	rtlpriv = rtl_sta->rtlpriv;

	rtlpriv->cfg->ops->update_rate_tbl(rtlpriv->hw, sta, rssi_level, is_update_bw);
}
#endif

u8 rtl_efuse_onebyte_read(void *adapter, u16 addr, u8 *data, bool bPseudoTest)
{
	struct rtl_priv *rtlpriv = (struct rtl_priv *)adapter;

	if (bPseudoTest)
		RT_TRACE(
			rtlpriv, COMP_PHYDM, DBG_WARNING,
			"rtl_efuse_onebyte_read: bPseudoTest=TRUE is not supportted!\n");

	return (u8)rtlpriv->efuse.efuse_ops->efuse_onebyte_read(rtlpriv->hw,
								addr, data);
}

void rtl_efuse_logical_map_read(void *adapter, u8 Type, u16 Offset, u32 *Value)
{
	struct rtl_priv *rtlpriv = (struct rtl_priv *)adapter;

	rtlpriv->efuse.efuse_ops->efuse_logical_map_read(rtlpriv->hw, Type,
							 Offset, Value);
}

static u32 edca_setting_ul[PEER_MAX] =
	/*UNKNOWN, REALTEK_90, REALTEK_92SE, BROADCOM,*/
	/*RALINK, ATHEROS, CISCO, MARVELL, */
	/*default, default */
	{0x5e4322, 0xa44f,   0x5e4322, 0x5ea32b, 0x5ea422,
	 0x5ea322, 0x3ea430, 0x5ea44f, 0x5ea42b, 0x5ea42b};

static u32 edca_setting_dl[PEER_MAX] =
	/*UNKNOWN, REALTEK_90, REALTEK_92SE, BROADCOM,*/
	/*RALINK, ATHEROS, CISCO, MARVELL */
	/*default, default */
	{0xa44f, 0x5ea44f, 0x5e4322, 0x5ea42b, 0xa44f,
	 0xa630, 0x5ea630, 0xa44f,   0x00a42b, 0x00a42b};

static u32 edca_setting_dl_g_mode[PEER_MAX] =
	/*UNKNOWN, REALTEK_90, REALTEK_92SE, BROADCOM,*/
	/*RALINK, ATHEROS, CISCO, MARVELL */
	/*default, default */
	{0x4322, 0xa44f, 0x5e4322, 0xa42b,   0x5e4322,
	 0x4322, 0xa42b, 0xa44f,   0x00a42b, 0x00a42b};

static void rtl_hal_turbo_edca(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_mac *mac = rtl_mac(rtl_priv(hw));
	struct rtl_hal *rtlhal = rtl_hal(rtl_priv(hw));
	struct phy_dm_struct *dm = rtlpriv_to_phydm(rtlpriv);

	u32 EDCA_BE_UL = 0x5ea42b;
	u32 EDCA_BE_DL = 0x00a42b;
	u8 ic_type;
	u8 iot_peer = 0;
	u16 wireless_mode = 0xFFFF;
	u8 traffic_index;
	u32 edca_param;
	u64 cur_tx_bytes = 0;
	u64 cur_rx_bytes = 0;
	u8 is_bias_on_rx = false;

	if (rtlpriv->dm.dis_turboedca)
		return;

	if (dm->wifi_test)
		return;

	if (!dm->is_linked) {
		rtlpriv->dm.is_any_nonbepkts = false;
		return;
	}

	ic_type = rtlhal->hw_type;
	wireless_mode = mac->mode;
	iot_peer = rtlpriv->mac80211.vendor;

	if (iot_peer >= PEER_MAX) {
		rtlpriv->dm.is_any_nonbepkts = false;
		return;
	}

	if (ic_type == HARDWARE_TYPE_RTL8188EE) {
		if (iot_peer == PEER_RAL || iot_peer == PEER_ATH)
			is_bias_on_rx = true;
	}

	if (rtlpriv->dm.dbginfo.num_non_be_pkt > BBREG_0x100)
		rtlpriv->dm.is_any_nonbepkts = true;
	rtlpriv->dm.dbginfo.num_non_be_pkt = 0;

	/* Check if the status needs to be changed. */
	if (!rtlpriv->dm.is_any_nonbepkts &&
	    !rtlpriv->dm.disable_framebursting) {
		cur_tx_bytes = rtlpriv->stats.txbytesunicast_inperiod;
		cur_rx_bytes = rtlpriv->stats.rxbytesunicast_inperiod;

		/* traffic, TX or RX */
		if (is_bias_on_rx) {
			if (cur_tx_bytes > (cur_rx_bytes << 2)) {
				/* Uplink TP is present. */
				traffic_index = UP_LINK;
			} else {
				/* Balance TP is present. */
				traffic_index = DOWN_LINK;
			}
		} else {
			if (cur_rx_bytes > (cur_tx_bytes << 2)) {
				/* Downlink TP is present. */
				traffic_index = DOWN_LINK;
			} else {
				/* Balance TP is present. */
				traffic_index = UP_LINK;
			}
		}

		EDCA_BE_UL = 0x6ea42b;
		EDCA_BE_DL = 0x6ea42b;

		/* 92D txop can't be set to 0x3e for cisco1250 */
		if (iot_peer == PEER_CISCO &&
		    wireless_mode == WIRELESS_MODE_N_24G) {
			EDCA_BE_DL = edca_setting_dl[iot_peer];
			EDCA_BE_UL = edca_setting_ul[iot_peer];
		} else if (iot_peer == PEER_CISCO &&
			 (wireless_mode == WIRELESS_MODE_G ||
			  wireless_mode == (WIRELESS_MODE_B |
					    WIRELESS_MODE_G) ||
			  wireless_mode == WIRELESS_MODE_A ||
			  wireless_mode == WIRELESS_MODE_B)) {
			/* merge from 92s_92c_merge temp*/
			EDCA_BE_DL = edca_setting_dl_g_mode[iot_peer];
		} else if (iot_peer == PEER_AIRGO &&
			 (wireless_mode == WIRELESS_MODE_G ||
			  wireless_mode == WIRELESS_MODE_A)) {
			EDCA_BE_DL = 0xa630;
		} else if (iot_peer == PEER_MARV) {
			EDCA_BE_DL = edca_setting_dl[iot_peer];
			EDCA_BE_UL = edca_setting_ul[iot_peer];
		} else if (iot_peer == PEER_ATH) {
			/* Set DL EDCA for Atheros peer to 0x3ea42b.*/
			/* Suggested by SD3 Wilson for ASUS TP issue.*/
			EDCA_BE_DL = edca_setting_dl[iot_peer];
		}

		if (ic_type == HARDWARE_TYPE_RTL8812AE ||
		    ic_type == HARDWARE_TYPE_RTL8821AE ||
		    ic_type == HARDWARE_TYPE_RTL8192EE) {
			EDCA_BE_UL = 0x5ea42b;
			EDCA_BE_DL = 0x5ea42b;
		}

		if (ic_type == HARDWARE_TYPE_RTL8822BE) {
			EDCA_BE_UL = 0x6ea42b;
			EDCA_BE_DL = 0x6ea42b;
		}

		if (traffic_index == DOWN_LINK)
			edca_param = EDCA_BE_DL;
		else
			edca_param = EDCA_BE_UL;

		rtlpriv->cfg->ops->set_hw_reg(hw, HW_VAR_AC_PARAM_BE,
					      (u8 *)(&edca_param));

		RT_TRACE(rtlpriv, COMP_TURBO, DBG_TRACE, "Turbo EDCA =0x%x\n",
			 edca_param);

		rtlpriv->dm.current_turbo_edca = true;
	} else {
		/*  */
		/* Turn Off EDCA turbo here. */
		/* Restore original EDCA according to the declaration of AP. */
		/*  */
		if (rtlpriv->dm.current_turbo_edca) {
			edca_param =
			    rtl_get_hal_edca_param(hw, mac->vif, wireless_mode,
						   &mac->edca_param[AC0_BE]);

			rtlpriv->cfg->ops->set_hw_reg(hw, HW_VAR_AC_PARAM_BE,
						      (u8 *)(&edca_param));
			rtlpriv->dm.current_turbo_edca = false;
			RT_TRACE(rtlpriv, COMP_TURBO, DBG_TRACE,
				 "Restore EDCA =0x%x\n", edca_param);
		}
	}
}

static const char *const _ch_width_str[] = {
	"20MHz", "40MHz", "80MHz", "160MHz", "80_80MHz", "CHANNEL_WIDTH_MAX",
};

#define ch_width_str(bw)                                                       \
	(((bw) >= CHANNEL_WIDTH_MAX) ? _ch_width_str[CHANNEL_WIDTH_MAX] :      \
				       _ch_width_str[(bw)])

static void dump_sta_info(struct rtl_priv *rtlpriv, void *sel,
			  struct rtl_sta_info *sta_entry)
{
	struct ra_sta_info *ra_info;
	u8 curr_tx_sgi = false;
	u8 curr_tx_rate;

	if (!sta_entry)
		return;

	ra_info = &sta_entry->cmn_info.ra_info;

	if (sel != 0) {
		seq_printf(sel, "============ STA [" MAC_FMT
				"]  ===================\n",
			   MAC_ARG(sta_entry->cmn_info.mac_addr));
		seq_printf(sel, "mac_id : %d\n", sta_entry->cmn_info.mac_id);
		seq_printf(sel, "wireless_set : 0x%02x\n",
			   sta_entry->cmn_info.support_wireless_set);
		seq_printf(sel, "mimo_type : %d\n",
			   sta_entry->cmn_info.mimo_type);
		seq_printf(sel, "bw_mode : %s, ra_bw_mode : %s\n",
			   ch_width_str(sta_entry->cmn_info.bw_mode),
			   ch_width_str(ra_info->ra_bw_mode));
		seq_printf(sel, "rate_id : %d\n", ra_info->rate_id);
		seq_printf(sel, "rssi : %d (%%), rssi_level : %d\n",
			   sta_entry->cmn_info.rssi_stat.rssi,
			   ra_info->rssi_level);
		seq_printf(sel, "is_support_sgi : %s, is_vht_enable : %s\n",
			   (ra_info->is_support_sgi) ? "Y" : "N",
			   (ra_info->is_vht_enable) ? "Y" : "N");
		seq_printf(sel, "disable_ra : %s, disable_pt : %s\n",
			   (ra_info->disable_ra) ? "Y" : "N",
			   (ra_info->disable_pt) ? "Y" : "N");
		seq_printf(sel, "is_noisy : %s\n",
			   (ra_info->is_noisy) ? "Y" : "N");
		seq_printf(
			sel, "txrx_state : %d\n",
			ra_info->txrx_state); /*0: uplink, 1:downlink, 2:bi-direction*/

		curr_tx_sgi = (ra_info->curr_tx_rate & 0x80) ? true : false;
		curr_tx_rate = ra_info->curr_tx_rate & 0x7F;
		seq_printf(sel, "curr_tx_rate : %s, curr_tx_sgi: %s\n",
			   HDATA_RATE(curr_tx_rate), (curr_tx_sgi) ? "Y" : "N");
		seq_printf(sel, "curr_tx_bw : %s\n",
			   ch_width_str(ra_info->curr_tx_bw));
		seq_printf(sel, "curr_retry_ratio : %d\n",
			   ra_info->curr_retry_ratio);
		seq_printf(sel, "ra_mask : 0x%016llx\n", ra_info->ramask);
	} else {
		RT_TRACE(rtlpriv, COMP_RATE, DBG_DMESG,
			 "============ STA [" MAC_FMT
			 "]  ===================\n",
			 MAC_ARG(sta_entry->cmn_info.mac_addr));
		RT_TRACE(rtlpriv, COMP_RATE, DBG_DMESG, "mac_id : %d\n",
			 sta_entry->cmn_info.mac_id);
		RT_TRACE(rtlpriv, COMP_RATE, DBG_DMESG,
			 "wireless_set : 0x%02x\n",
			 sta_entry->cmn_info.support_wireless_set);
		RT_TRACE(rtlpriv, COMP_RATE, DBG_DMESG, "mimo_type : %d\n",
			 sta_entry->cmn_info.mimo_type);
		RT_TRACE(rtlpriv, COMP_RATE, DBG_DMESG,
			 "bw_mode : %s, ra_bw_mode : %s\n",
			 ch_width_str(sta_entry->cmn_info.bw_mode),
			 ch_width_str(ra_info->ra_bw_mode));
		RT_TRACE(rtlpriv, COMP_RATE, DBG_DMESG, "rate_id : %d\n",
			 ra_info->rate_id);
		RT_TRACE(rtlpriv, COMP_RATE, DBG_DMESG,
			 "rssi : %d (%%), rssi_level : %d\n",
			 sta_entry->cmn_info.rssi_stat.rssi,
			 ra_info->rssi_level);
		RT_TRACE(rtlpriv, COMP_RATE, DBG_DMESG,
			 "is_support_sgi : %s, is_vht_enable : %s\n",
			 (ra_info->is_support_sgi) ? "Y" : "N",
			 (ra_info->is_vht_enable) ? "Y" : "N");
		RT_TRACE(rtlpriv, COMP_RATE, DBG_DMESG,
			 "disable_ra : %s, disable_pt : %s\n",
			 (ra_info->disable_ra) ? "Y" : "N",
			 (ra_info->disable_pt) ? "Y" : "N");
		RT_TRACE(rtlpriv, COMP_RATE, DBG_DMESG, "is_noisy : %s\n",
			 (ra_info->is_noisy) ? "Y" : "N");
		RT_TRACE(
			rtlpriv, COMP_RATE, DBG_DMESG, "txrx_state : %d\n",
			ra_info->txrx_state); /*0: uplink, 1:downlink, 2:bi-direction*/

		curr_tx_sgi = (ra_info->curr_tx_rate & 0x80) ? true : false;
		curr_tx_rate = ra_info->curr_tx_rate & 0x7F;
		RT_TRACE(rtlpriv, COMP_RATE, DBG_DMESG,
			 "curr_tx_rate : %s, curr_tx_sgi: %s\n",
			 HDATA_RATE(curr_tx_rate), (curr_tx_sgi) ? "Y" : "N");
		RT_TRACE(rtlpriv, COMP_RATE, DBG_DMESG, "curr_tx_bw : %s\n",
			 ch_width_str(ra_info->curr_tx_bw));
		RT_TRACE(rtlpriv, COMP_RATE, DBG_DMESG,
			 "curr_retry_ratio : %d\n", ra_info->curr_retry_ratio);
		RT_TRACE(rtlpriv, COMP_RATE, DBG_DMESG, "ra_mask : 0x%016llx\n",
			 ra_info->ramask);
	}
}

MODULE_AUTHOR("Realtek WlanFAE	<wlanfae@realtek.com>");
MODULE_AUTHOR("Larry Finger	<Larry.FInger@lwfinger.net>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Realtek 802.11n PCI wireless core");
