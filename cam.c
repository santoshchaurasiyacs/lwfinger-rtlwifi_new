/******************************************************************************
 *
 * Copyright(c) 2009-2010  Realtek Corporation.
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
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
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
#include "wifi.h"
#include "cam.h"
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 0))
#include <linux/export.h>
#endif


s8 rtl_cam_set_key(struct ieee80211_hw *hw, struct ieee80211_sta *sta,
			struct ieee80211_key_conf *key,
		       enum rtl_cam_key_type cam_key_type)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_mac *mac = rtl_mac(rtl_priv(hw));
	struct rtl_efuse *rtlefuse = rtl_efuse(rtl_priv(hw));

	u8 entry_id = 0;
	u8 enc_algo = 0;
	u8 key_index = 0;
	u8 macaddr[ETH_ALEN];
	u8 key_data[128] = {0};
	enum rt_enc_alg enc_alg_in_driver = NO_ENCRYPTION;

	static u8 cam_const_addr[4][6] = {
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x02},
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x03}
	};
	static u8 cam_const_broad[] = {
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff
	};



	key_index = key->keyidx;
	memcpy(key_data, key->key, key->keylen);/* avoid  (key->len) < 128*/

	switch (key->cipher) {
	case WLAN_CIPHER_SUITE_WEP40:
		enc_algo = rtlpriv->cfg->maps[SEC_CAM_WEP40];
		enc_alg_in_driver = WEP40_ENCRYPTION;
		break;
	case WLAN_CIPHER_SUITE_WEP104:
		enc_algo = rtlpriv->cfg->maps[SEC_CAM_WEP104];
		enc_alg_in_driver = WEP104_ENCRYPTION;
		break;
	case WLAN_CIPHER_SUITE_TKIP:
		enc_algo = rtlpriv->cfg->maps[SEC_CAM_TKIP];
		enc_alg_in_driver = TKIP_ENCRYPTION;
		break;
	case WLAN_CIPHER_SUITE_CCMP:
		enc_algo = rtlpriv->cfg->maps[SEC_CAM_AES];
		enc_alg_in_driver = AESCCMP_ENCRYPTION;
		break;
	case WLAN_CIPHER_SUITE_AES_CMAC:
	/* dead code here, already returned not support in rtl_op_set_key*/
		RT_TRACE(rtlpriv, COMP_SEC, DBG_DMESG,
			 "not support,use software CMAC encrypiton\n");
		return -1;
		/* no need to break */
	default:
		RT_TRACE(rtlpriv, COMP_ERR, DBG_EMERG,
				 "switch case not process\n");
		enc_algo = rtlpriv->cfg->maps[SEC_CAM_AES];
		break;
	}


	/* wep + wep_group */
	if (cam_key_type == wep_only) {
			RT_TRACE(rtlpriv, COMP_SEC, DBG_DMESG, "set WEP(group/pairwise) key\n");
			rtlpriv->sec.pairwise_enc_algorithm = enc_alg_in_driver;
			rtlpriv->sec.group_enc_algorithm = enc_alg_in_driver;
			memcpy(macaddr, cam_const_addr[key_index], ETH_ALEN);

	/* group */
	} else if (cam_key_type == group_key) {
			RT_TRACE(rtlpriv, COMP_SEC, DBG_DMESG, "set group key\n");
			rtlpriv->sec.group_enc_algorithm = enc_alg_in_driver;
			memcpy(macaddr, cam_const_broad, ETH_ALEN);
	/* pairwise key */
	} else if (cam_key_type == pairwise_key) {
			RT_TRACE(rtlpriv, COMP_SEC, DBG_DMESG, "set pairwise key\n");
			rtlpriv->sec.pairwise_enc_algorithm = enc_alg_in_driver;
			memcpy(macaddr, sta->addr, ETH_ALEN);/* real addr */
	} else {
			RT_TRACE(rtlpriv, COMP_SEC, DBG_DMESG,
				"not support key type\n");
			return -1;
	}


	/* this is dead code */
	/* adhoc encrypt is done in mac80211,not there*/
	/* refer to:To support IBSS, use sw-crypto for GTK */
	/* for ADHOC pairwise,use rtlefuse->dev_addr */
	if (mac->opmode == NL80211_IFTYPE_ADHOC) {
		memcpy(macaddr, rtlefuse->dev_addr, ETH_ALEN);
	}


	/*
		macaddr:
		key_index: passed from mac80211
		entry_id: free entry of total 32 in hardware
		enc_algo:wep,tkip,ccmp
		key_data:key in binary

		when group key or wep key, sta is null

	*/

	/* entry_id from 0 to 31 */
	entry_id = rtl_cam_get_free_entry(hw, sta, key_index);
	if (TOTAL_CAM_ENTRY == entry_id)
		return -1;

	rtl_cam_clear_one_entry(hw, entry_id);
	rtl_cam_add_one_entry(hw, macaddr, key_index, entry_id, enc_algo, key_data);


	return 0;
}


void rtl_cam_reset_sec_info(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	rtlpriv->sec.use_defaultkey = false;
	rtlpriv->sec.pairwise_enc_algorithm = NO_ENCRYPTION;
	rtlpriv->sec.group_enc_algorithm = NO_ENCRYPTION;

}

static void rtl_cam_program_entry(struct ieee80211_hw *hw, u32 entry_no,
			   u8 *mac_addr, u8 *key_cont_128, u16 us_config)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	u32 target_command;
	u32 target_content = 0;
	u8 entry_i;

	RT_PRINT_DATA(rtlpriv, COMP_SEC, DBG_DMESG, "Key content :",
			key_cont_128, 16);

	/* 2-5 fill 128key,6-7 are reserved */
	for (entry_i = 0; entry_i < CAM_CONTENT_COUNT - 2 ; entry_i++) {

		target_command = entry_i + CAM_CONTENT_COUNT * entry_no;
		target_command = target_command | BIT(31) | BIT(16);

		if (entry_i == 0) {
			target_content = (u32) (*(mac_addr + 0)) << 16 |
			    (u32) (*(mac_addr + 1)) << 24 | (u32) us_config;

			rtl_write_dword(rtlpriv, rtlpriv->cfg->maps[WCAMI],
					target_content);
			rtl_write_dword(rtlpriv, rtlpriv->cfg->maps[RWCAM],
					target_command);

			RT_TRACE(rtlpriv, COMP_SEC, DBG_LOUD,
				"WRITE %x: %x\n",
				  rtlpriv->cfg->maps[WCAMI], target_content);
			RT_TRACE(rtlpriv, COMP_SEC, DBG_LOUD,
				"The Key ID is %d\n", entry_no);
			RT_TRACE(rtlpriv, COMP_SEC, DBG_LOUD,
				"WRITE %x: %x\n",
				  rtlpriv->cfg->maps[RWCAM], target_command);

		} else if (entry_i == 1) {

			target_content = (u32) (*(mac_addr + 5)) << 24 |
			    (u32) (*(mac_addr + 4)) << 16 |
			    (u32) (*(mac_addr + 3)) << 8 |
			    (u32) (*(mac_addr + 2));

			rtl_write_dword(rtlpriv, rtlpriv->cfg->maps[WCAMI],
					target_content);
			rtl_write_dword(rtlpriv, rtlpriv->cfg->maps[RWCAM],
					target_command);

			RT_TRACE(rtlpriv, COMP_SEC, DBG_LOUD,
				"WRITE A4: %x\n", target_content);
			RT_TRACE(rtlpriv, COMP_SEC, DBG_LOUD,
				"WRITE A0: %x\n", target_command);

		} else {

			target_content =
			    (u32) (*(key_cont_128 + (entry_i * 4 - 8) + 3)) <<
			    24 | (u32) (*(key_cont_128 + (entry_i * 4 - 8) + 2))
			    << 16 |
			    (u32) (*(key_cont_128 + (entry_i * 4 - 8) + 1)) << 8
			    | (u32) (*(key_cont_128 + (entry_i * 4 - 8) + 0));

			rtl_write_dword(rtlpriv, rtlpriv->cfg->maps[WCAMI],
					target_content);
			rtl_write_dword(rtlpriv, rtlpriv->cfg->maps[RWCAM],
					target_command);
			udelay(100);

			RT_TRACE(rtlpriv, COMP_SEC, DBG_LOUD,
				"WRITE A4: %x\n", target_content);
			RT_TRACE(rtlpriv, COMP_SEC, DBG_LOUD,
				"WRITE A0: %x\n", target_command);
		}
	}

	RT_TRACE(rtlpriv, COMP_SEC, DBG_LOUD,
		 "after set key, usconfig:%x\n", us_config);
}

u8 rtl_cam_add_one_entry(struct ieee80211_hw *hw, u8 *mac_addr,
			 u32 ul_key_id, u32 ul_entry_idx, u32 ul_enc_alg,
			 u8 *key_content)
{
	u32 us_config;
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	RT_TRACE(rtlpriv, COMP_SEC, DBG_DMESG,
		 "CAM_ENTRY_ID:%d, KEY_ID=%d, ENC_ALG=%02x,MacAddr %pM\n",
		  ul_entry_idx, ul_key_id, ul_enc_alg,
		  mac_addr);

	if (ul_key_id == TOTAL_CAM_ENTRY) {
		RT_TRACE(rtlpriv, COMP_ERR, DBG_WARNING,
			 "ulKeyId exceed!\n");
		return 0;
	}


	us_config = CFG_VALID | ((ul_enc_alg) << 2) | ul_key_id;

	rtl_cam_program_entry(hw, ul_entry_idx, mac_addr, (u8 *) key_content, us_config);

	return 1;

}
EXPORT_SYMBOL(rtl_cam_add_one_entry);

/* troy add */
void rtl_cam_clear_one_entry(struct ieee80211_hw *hw, u32 entry_idx)
{
	u32 us_config = 0;
	u8 null_key[128] = { 0 };
	u8 null_sta[6] = { 0 };

	rtl_cam_program_entry(hw, entry_idx, null_sta, (u8 *) null_key, us_config);

}

int rtl_cam_delete_one_entry(struct ieee80211_hw *hw,
			     u8 *mac_addr, u32 ul_key_id)
{
	u32 ul_command;
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	RT_TRACE(rtlpriv, COMP_SEC, DBG_DMESG, "key_idx:%d\n", ul_key_id);

	ul_command = ul_key_id * CAM_CONTENT_COUNT;
	ul_command = ul_command | BIT(31) | BIT(16);

	rtl_write_dword(rtlpriv, rtlpriv->cfg->maps[WCAMI], 0);
	rtl_write_dword(rtlpriv, rtlpriv->cfg->maps[RWCAM], ul_command);

	RT_TRACE(rtlpriv, COMP_SEC, DBG_DMESG,
		"rtl_cam_delete_one_entry(): WRITE A4: %x\n", 0);
	RT_TRACE(rtlpriv, COMP_SEC, DBG_DMESG,
		"rtl_cam_delete_one_entry(): WRITE A0: %x\n", ul_command);

	return 0;

}
EXPORT_SYMBOL(rtl_cam_delete_one_entry);

void rtl_cam_reset_all_entry(struct ieee80211_hw *hw)
{
	u32 ul_command;
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	ul_command = BIT(31) | BIT(30);
	rtl_write_dword(rtlpriv, rtlpriv->cfg->maps[RWCAM], ul_command);
}
EXPORT_SYMBOL(rtl_cam_reset_all_entry);

void rtl_cam_mark_invalid(struct ieee80211_hw *hw, u8 uc_index)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	u32 ul_command;
	u32 ul_content;
	u32 ul_enc_algo = rtlpriv->cfg->maps[SEC_CAM_AES];

	switch (rtlpriv->sec.pairwise_enc_algorithm) {
	case WEP40_ENCRYPTION:
		ul_enc_algo = rtlpriv->cfg->maps[SEC_CAM_WEP40];
		break;
	case WEP104_ENCRYPTION:
		ul_enc_algo = rtlpriv->cfg->maps[SEC_CAM_WEP104];
		break;
	case TKIP_ENCRYPTION:
		ul_enc_algo = rtlpriv->cfg->maps[SEC_CAM_TKIP];
		break;
	case AESCCMP_ENCRYPTION:
		ul_enc_algo = rtlpriv->cfg->maps[SEC_CAM_AES];
		break;
	default:
		ul_enc_algo = rtlpriv->cfg->maps[SEC_CAM_AES];
	}

	ul_content = (uc_index & 3) | ((u16) (ul_enc_algo) << 2);

	ul_content |= BIT(15);
	ul_command = CAM_CONTENT_COUNT * uc_index;
	ul_command = ul_command | BIT(31) | BIT(16);

	rtl_write_dword(rtlpriv, rtlpriv->cfg->maps[WCAMI], ul_content);
	rtl_write_dword(rtlpriv, rtlpriv->cfg->maps[RWCAM], ul_command);

	RT_TRACE(rtlpriv, COMP_SEC, DBG_DMESG,
		"rtl_cam_mark_invalid(): WRITE A4: %x\n", ul_content);
	RT_TRACE(rtlpriv, COMP_SEC, DBG_DMESG,
		"rtl_cam_mark_invalid(): WRITE A0: %x\n", ul_command);
}
EXPORT_SYMBOL(rtl_cam_mark_invalid);

void rtl_cam_empty_entry(struct ieee80211_hw *hw, u8 uc_index)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	u32 ul_command;
	u32 ul_content;
	u32 ul_encalgo = rtlpriv->cfg->maps[SEC_CAM_AES];
	u8 entry_i;

	switch (rtlpriv->sec.pairwise_enc_algorithm) {
	case WEP40_ENCRYPTION:
		ul_encalgo = rtlpriv->cfg->maps[SEC_CAM_WEP40];
		break;
	case WEP104_ENCRYPTION:
		ul_encalgo = rtlpriv->cfg->maps[SEC_CAM_WEP104];
		break;
	case TKIP_ENCRYPTION:
		ul_encalgo = rtlpriv->cfg->maps[SEC_CAM_TKIP];
		break;
	case AESCCMP_ENCRYPTION:
		ul_encalgo = rtlpriv->cfg->maps[SEC_CAM_AES];
		break;
	default:
		ul_encalgo = rtlpriv->cfg->maps[SEC_CAM_AES];
	}

	for (entry_i = 0; entry_i < CAM_CONTENT_COUNT; entry_i++) {

		if (entry_i == 0) {
			ul_content =
			    (uc_index & 0x03) | ((u16) (ul_encalgo) << 2);
			ul_content |= BIT(15);

		} else {
			ul_content = 0;
		}

		ul_command = CAM_CONTENT_COUNT * uc_index + entry_i;
		ul_command = ul_command | BIT(31) | BIT(16);

		rtl_write_dword(rtlpriv, rtlpriv->cfg->maps[WCAMI], ul_content);
		rtl_write_dword(rtlpriv, rtlpriv->cfg->maps[RWCAM], ul_command);

		RT_TRACE(rtlpriv, COMP_SEC, DBG_LOUD,
			"rtl_cam_empty_entry(): WRITE A4: %x\n",
			  ul_content);
		RT_TRACE(rtlpriv, COMP_SEC, DBG_LOUD,
			"rtl_cam_empty_entry(): WRITE A0: %x\n",
				ul_command);
	}

}
EXPORT_SYMBOL(rtl_cam_empty_entry);




u8 rtl_cam_get_free_entry(struct ieee80211_hw *hw, struct ieee80211_sta *sta, u8 key_index)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	u32 bitmap = rtlpriv->sec.cam_bitmap;
	u8 entry_idx = 0;/* return val */

	struct rtl_sta_info *sta_entry = NULL;
	bool found = false;

	if (NULL == sta) {
		entry_idx = key_index;
		rtlpriv->sec.cam_bitmap |= BIT(0) << entry_idx;
		found = true;
	} else {

		for (entry_idx = CAM_PAIRWISE_KEY_OFFSET; entry_idx < TOTAL_CAM_ENTRY; entry_idx++) {

			if (((bitmap >> entry_idx) & BIT(0)) == 0) {
				rtlpriv->sec.cam_bitmap |= BIT(0) << entry_idx;
				sta_entry = (struct rtl_sta_info *) sta->drv_priv;
				sta_entry->cam_entry_id = entry_idx;
				found = true;
				break;
			}
		}
	}

	if (found) {
		RT_TRACE(rtlpriv, COMP_SEC, DBG_EMERG,
				"key_index=%d,cam_bitmap: 0x%x entry_idx=%d\n",
				 key_index, rtlpriv->sec.cam_bitmap, entry_idx);
		return entry_idx;
	} else {
		RT_TRACE(rtlpriv, COMP_SEC, DBG_EMERG,
				"critical error! no entry found!!!\n");
		return TOTAL_CAM_ENTRY;
	}
}
EXPORT_SYMBOL(rtl_cam_get_free_entry);

void rtl_cam_del_entry(struct ieee80211_hw *hw, struct ieee80211_sta *sta, u8 key_index)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_sta_info *sta_entry = NULL;

	u8 entry_id;

	if (NULL != sta) {
		RT_TRACE(rtlpriv, COMP_SEC, DBG_DMESG, "delete according to cam id\n");
		sta_entry = (struct rtl_sta_info *) sta->drv_priv;
		entry_id = sta_entry->cam_entry_id;

	} else {

		RT_TRACE(rtlpriv, COMP_SEC, DBG_DMESG, "delete according to key id\n");
		entry_id = key_index;
	}
	rtlpriv->sec.cam_bitmap &= ~(BIT(0) << entry_id);
	rtl_cam_clear_one_entry(hw, entry_id);


	RT_TRACE(rtlpriv, COMP_SEC, DBG_EMERG, "cam_bitmap: 0x%x\n", rtlpriv->sec.cam_bitmap);

	return;
}
EXPORT_SYMBOL(rtl_cam_del_entry);
