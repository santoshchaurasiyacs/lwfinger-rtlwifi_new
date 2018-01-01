/******************************************************************************
 *
 * Copyright(c) 2016  Realtek Corporation.
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
#ifndef _RTL_HALMAC_H_
#define _RTL_HALMAC_H_

#include "halmac_api.h"

/* for uncooked halmac */
#ifdef PLATFORM_RTL_FREE	/* borrow this flag to check */
#define halmac_drv_info	_HALMAC_DRV_INFO
#define halmac_hw_id _HALMAC_HW_ID
#define halmac_cmd_process_status _HALMAC_CMD_PROCESS_STATUS
#define halmac_ret_status _HALMAC_RET_STATUS
#define halmac_adapter _HALMAC_ADAPTER
#define halmac_api _HALMAC_API
#define halmac_rxagg_cfg _HALMAC_RXAGG_CFG
#define hal_fifo_sel _HAL_FIFO_SEL
#define halmac_wlan_addr _HALMAC_WLAN_ADDR
#define address Address
#define halmac_pg_efuse_info _HALMAC_PG_EFUSE_INFO
#define efuse_map pEfuse_map
#define efuse_mask pEfuse_mask
#define halmac_feature_id _HALMAC_FEATURE_ID
#define halmac_fw_version _HALMAC_FW_VERSION
#define halmac_general_info _HALMAC_GENERAL_INFO
#define halmac_rf_type _HALMAC_RF_TYPE
#define halmac_interface _HALMAC_INTERFACE
#define halmac_platform_api _HALMAC_PLATFORM_API
#endif

/* HALMAC Definition for Driver */
#define RTL_HALMAC_H2C_MAX_SIZE		HALMAC_H2C_CMD_ORIGINAL_SIZE_88XX
#define RTL_HALMAC_BA_SSN_RPT_SIZE	4

#define rtlpriv_to_halmac(priv)                                                \
	((struct halmac_adapter *)((priv)->halmac.internal))

/* for H2C cmd */
#define MAX_H2C_BOX_NUMS 4
#define MESSAGE_BOX_SIZE 4
#define EX_MESSAGE_BOX_SIZE 4

typedef enum _RTL_HALMAC_MODE {
	RTL_HALMAC_MODE_NORMAL,
	RTL_HALMAC_MODE_WIFI_TEST,
} RTL_HALMAC_MODE;

#if 0
u8 rtl_halmac_read8(struct intf_hdl *, u32 addr);
u16 rtl_halmac_read16(struct intf_hdl *, u32 addr);
u32 rtl_halmac_read32(struct intf_hdl *, u32 addr);
int rtl_halmac_write8(struct intf_hdl *, u32 addr, u8 value);
int rtl_halmac_write16(struct intf_hdl *, u32 addr, u16 value);
int rtl_halmac_write32(struct intf_hdl *, u32 addr, u32 value);
#endif

/* HALMAC API for Driver(HAL) */
int rtl_halmac_init_adapter(struct rtl_priv *rtlpriv);
int rtl_halmac_deinit_adapter(struct rtl_priv *rtlpriv);
int rtl_halmac_poweron(struct rtl_priv *rtlpriv);
int rtl_halmac_poweroff(struct rtl_priv *rtlpriv);
int rtl_halmac_init_hal(struct rtl_priv *rtlpriv);
int rtl_halmac_init_hal_fw(struct rtl_priv *rtlpriv, u8 *fw, u32 fwsize);
int rtl_halmac_init_hal_fw_file(struct rtl_priv *rtlpriv, u8 *fwpath);
int rtl_halmac_deinit_hal(struct rtl_priv *rtlpriv);
int rtl_halmac_self_verify(struct rtl_priv *rtlpriv);
int rtl_halmac_dlfw(struct rtl_priv *rtlpriv, u8 *fw, u32 fwsize);
int rtl_halmac_dlfw_from_file(struct rtl_priv *rtlpriv, u8 *fwpath);
int rtl_halmac_phy_power_switch(struct rtl_priv *rtlpriv, u8 enable);
int rtl_halmac_send_h2c(struct rtl_priv *rtlpriv, u8 *h2c);
int rtl_halmac_c2h_handle(struct rtl_priv *rtlpriv, u8 *c2h, u32 size);

int rtl_halmac_get_physical_efuse_size(struct rtl_priv *rtlpriv, u32 *size);
int rtl_halmac_read_physical_efuse_map(struct rtl_priv *rtlpriv, u8 *map,
				       u32 size);
int rtl_halmac_read_physical_efuse(struct rtl_priv *rtlpriv, u32 offset,
				   u32 cnt, u8 *data);
int rtl_halmac_write_physical_efuse(struct rtl_priv *rtlpriv, u32 offset,
				    u32 cnt, u8 *data);
int rtl_halmac_get_logical_efuse_size(struct rtl_priv *rtlpriv, u32 *size);
int rtl_halmac_read_logical_efuse_map(struct rtl_priv *rtlpriv, u8 *map,
				      u32 size);
int rtl_halmac_write_logical_efuse_map(struct rtl_priv *rtlpriv, u8 *map,
				       u32 size, u8 *maskmap, u32 masksize);
int rtl_halmac_read_logical_efuse(struct rtl_priv *rtlpriv, u32 offset, u32 cnt,
				  u8 *data);
int rtl_halmac_write_logical_efuse(struct rtl_priv *rtlpriv, u32 offset,
				   u32 cnt, u8 *data);

int rtl_halmac_config_rx_info(struct rtl_priv *rtlpriv, enum halmac_drv_info);
int rtl_halmac_set_mac_address(struct rtl_priv *rtlpriv, u8 hwport, u8 *addr);
int rtl_halmac_set_bssid(struct rtl_priv *d, u8 hwport, u8 *addr);

int rtl_halmac_set_bandwidth(struct rtl_priv *rtlpriv, u8 channel,
			     u8 pri_ch_idx, u8 bw);
int rtl_halmac_rx_agg_switch(struct rtl_priv *rtlpriv, bool enable);
int rtl_halmac_get_hw_value(struct rtl_priv *d, enum halmac_hw_id hw_id,
			    void *pvalue);
int rtl_halmac_dump_fifo(struct rtl_priv *rtlpriv,
			 enum hal_fifo_sel halmac_fifo_sel);

int rtl_halmac_get_wow_reason(struct rtl_priv *rtlpriv, u8 *reason);
int rtl_halmac_get_drv_info_sz(struct rtl_priv *d, u8 *sz);

int rtl_halmac_get_rsvd_drv_pg_bndy(struct rtl_priv *dvobj, u16 *drv_pg);
int rtl_halmac_download_rsvd_page(struct rtl_priv *dvobj, u8 pg_offset,
				  u8 *pbuf, u32 size);

#ifdef CONFIG_SDIO_HCI
int rtl_halmac_query_tx_page_num(struct rtl_priv *);
int rtl_halmac_get_tx_queue_page_num(struct rtl_priv *, u8 queue, u32 *page);
u32 rtl_halmac_sdio_get_tx_addr(struct rtl_priv *, u8 *desc, u32 size);
int rtl_halmac_sdio_tx_allowed(struct rtl_priv *, u8 *buf, u32 size);
u32 rtl_halmac_sdio_get_rx_addr(struct rtl_priv *, u8 *seq);
#endif /* CONFIG_SDIO_HCI */

#ifdef CONFIG_USB_HCI
u8 rtl_halmac_usb_get_bulkout_id(struct rtl_priv *, u8 *buf, u32 size);
u8 rtl_halmac_switch_usb_mode(struct rtl_priv *d, enum RTL_USB_SPEED usb_mode);
#endif /* CONFIG_USB_HCI */

int rtl_halmac_chk_txdesc(struct rtl_priv *rtlpriv, u8 *txdesc, u32 size);
int rtl_halmac_iqk(struct rtl_priv *rtlpriv, u8 clear, u8 segment);

struct rtl_halmac_ops *rtl_halmac_get_ops_pointer(void);

#endif /* _RTL_HALMAC_H_ */
