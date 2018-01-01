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
#ifndef __RTL_PHYDM_H__
#define __RTL_PHYDM_H__

/* for uncooked phydm */
#ifdef PHYDM_TESTCHIP_SUPPORT	/* borrow to check uncooked code */
#define phy_dm_struct PHY_DM_STRUCT
#define dm_per_pkt_info phydm_perpkt_info_struct
#define dm_phy_status_info phydm_phyinfo_struct
#define odm_ic_type phydm_ic_e
#endif

enum {
	UP_LINK,
	DOWN_LINK,
};

struct rtl_phydm_ops *rtl_phydm_get_ops_pointer(void);

#define rtlpriv_to_phydm(priv)                                                 \
	((struct phy_dm_struct *)((priv)->phydm.internal))

enum rt_spinlock_type;

void rtl_odm_acquirespinlock(struct rtl_priv *rtlpriv,
			     enum rt_spinlock_type type);
void rtl_odm_releasespinlock(struct rtl_priv *rtlpriv,
			     enum rt_spinlock_type type);
u8 phy_get_tx_power_index(void *adapter, u8 rf_path, u8 rate,
			  enum ht_channel_width bandwidth, u8 channel);
void phy_set_tx_power_index_by_rs(void *adapter, u8 ch, u8 path, u8 rs);
void phy_store_tx_power_by_rate(void *adapter, u32 band, u32 rfpath, u32 txnum,
				u32 regaddr, u32 bitmask, u32 data);
void phy_set_tx_power_limit(void *dm, u8 *regulation, u8 *band, u8 *bandwidth,
			    u8 *rate_section, u8 *rf_path, u8 *channel,
			    u8 *power_limit);

#if 0
void rtw_phydm_update_ra_mask(struct cmn_sta_info *pcmn_info, u8 rssi_level, u8 is_update_bw);
#endif

u8 rtl_efuse_onebyte_read(void *adapter, u16 addr, u8 *data, bool bPseudoTest);

void rtl_efuse_logical_map_read(void *adapter, u8 Type, u16 Offset, u32 *Value);
enum hal_status rtl_phydm_fw_iqk(void *adapter, u8 clear, u8 segment);

#endif
