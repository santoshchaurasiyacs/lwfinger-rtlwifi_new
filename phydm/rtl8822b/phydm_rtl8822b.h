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
#if (RTL8822B_SUPPORT == 1)
#ifndef __ODM_RTL8822B_H__
#define __ODM_RTL8822B_H__

void phydm_1rcca_setting(struct PHY_DM_STRUCT *p_dm, boolean enable_1rcca);

void phydm_somlrxhp_setting(struct PHY_DM_STRUCT *p_dm, boolean switch_soml);

void phydm_hwsetting_8822b(struct PHY_DM_STRUCT *p_dm);

void phydm_rxagc_switch_8822b(struct PHY_DM_STRUCT *p_dm,
			      boolean enable_rxagc_swich);

void phydm_config_tx2path_8822b(struct PHY_DM_STRUCT *p_dm,
				enum wireless_set wireless_mode,
				boolean is_tx2_path);

#endif /* #define __ODM_RTL8822B_H__ */
#endif
