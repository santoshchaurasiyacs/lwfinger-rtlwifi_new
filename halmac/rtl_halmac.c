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

#include "halmac_api.h"
#include "rtl_halmac.h"
#include <linux/module.h>
#include <linux/vmalloc.h>

#define DEFAULT_INDICATOR_TIMELMT msecs_to_jiffies(1000) /* ms */
#define FIRMWARE_MAX_SIZE HALMAC_FW_SIZE_MAX_88XX

static struct rtl_halmac_ops rtl_halmac_operation = {
	.halmac_init_adapter = rtl_halmac_init_adapter,
	.halmac_deinit_adapter = rtl_halmac_deinit_adapter,
	.halmac_init_hal = rtl_halmac_init_hal,
	.halmac_deinit_hal = rtl_halmac_deinit_hal,
	.halmac_poweron = rtl_halmac_poweron,
	.halmac_poweroff = rtl_halmac_poweroff,

	.halmac_phy_power_switch = rtl_halmac_phy_power_switch,
	.halmac_set_mac_address = rtl_halmac_set_mac_address,
	.halmac_set_bssid = rtl_halmac_set_bssid,

	.halmac_get_physical_efuse_size = rtl_halmac_get_physical_efuse_size,
	.halmac_read_physical_efuse_map = rtl_halmac_read_physical_efuse_map,
	.halmac_get_logical_efuse_size = rtl_halmac_get_logical_efuse_size,
	.halmac_read_logical_efuse_map = rtl_halmac_read_logical_efuse_map,

	.halmac_set_bandwidth = rtl_halmac_set_bandwidth,

	.halmac_c2h_handle = rtl_halmac_c2h_handle,

	.halmac_chk_txdesc = rtl_halmac_chk_txdesc,
	.halmac_iqk = rtl_halmac_iqk,
};

struct rtl_halmac_ops *rtl_halmac_get_ops_pointer(void)
{
	return &rtl_halmac_operation;
}
EXPORT_SYMBOL(rtl_halmac_get_ops_pointer);

/*
 * Driver API for HALMAC operations
 */

#ifdef CONFIG_SDIO_HCI
#include <rtl_sdio.h>
static u8 _halmac_sdio_cmd52_read(void *p, u32 offset)
{
	struct rtl_priv *rtlpriv;
	u8 val;
	u8 ret;


	rtlpriv = (struct rtlpriv *)p;
	ret = rtl_sdio_read_cmd52(rtlpriv, offset, &val, 1);
	if (ret == _FAIL) {
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD, "%s: [ERROR] I/O FAIL!\n", __FUNCTION__);
		return SDIO_ERR_VAL8;
	}

	return val;
}

static void _halmac_sdio_cmd52_write(void *p, u32 offset, u8 val)
{
	struct rtl_priv *rtlpriv;
	u8 ret;


	rtlpriv = (struct rtlpriv *)p;
	ret = rtl_sdio_write_cmd52(rtlpriv, offset, &val, 1);
	if (ret == _FAIL)
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD, "%s: [ERROR] I/O FAIL!\n", __FUNCTION__);
}

static u8 _halmac_sdio_reg_read_8(void *p, u32 offset)
{
	struct rtl_priv *rtlpriv;
	u8 *pbuf;
	u8 val;
	int err;


	rtlpriv = (struct rtlpriv *)p;
	val = SDIO_ERR_VAL8;
	pbuf = rtl_zmalloc(1);
	if (!pbuf)
		return val;

	err = d->intf_ops->read(rtlpriv, offset, pbuf, 1, 0);
	if (err) {
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD, "%s: [ERROR] I/O FAIL!\n", __FUNCTION__);
		goto exit;
	}

	val = *pbuf;

exit:
	rtl_mfree(pbuf, 1);

	return val;
}

static u16 _halmac_sdio_reg_read_16(void *p, u32 offset)
{
	struct rtl_priv *rtlpriv;
	u8 *pbuf;
	u16 val;
	int err;


	rtlpriv = (struct rtlpriv *)p;
	val = SDIO_ERR_VAL16;
	pbuf = rtl_zmalloc(2);
	if (!pbuf)
		return val;

	err = d->intf_ops->read(rtlpriv, offset, pbuf, 2, 0);
	if (err) {
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD, "%s: [ERROR] I/O FAIL!\n", __FUNCTION__);
		goto exit;
	}

	val = le16_to_cpu(*(u16 *)pbuf);

exit:
	rtl_mfree(pbuf, 2);

	return val;
}

static u32 _halmac_sdio_reg_read_32(void *p, u32 offset)
{
	struct rtl_priv *rtlpriv;
	u8 *pbuf;
	u32 val;
	int err;


	rtlpriv = (struct rtlpriv *)p;
	val = SDIO_ERR_VAL32;
	pbuf = rtl_zmalloc(4);
	if (!pbuf)
		return val;

	err = d->intf_ops->read(rtlpriv, offset, pbuf, 4, 0);
	if (err) {
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD, "%s: [ERROR] I/O FAIL!\n", __FUNCTION__);
		goto exit;
	}

	val = le32_to_cpu(*(u32 *)pbuf);

exit:
	rtl_mfree(pbuf, 4);

	return val;
}

static BOOL _halmac_sdio_reg_read_n(void *p, u32 offset, u32 size, u8 *data)
{
	struct rtl_priv *rtlpriv;
	u8 *pbuf;
	int err;
	BOOL rst = _FALSE;

	rtlpriv = (struct rtlpriv *)p;
	pbuf = rtl_zmalloc(size);
	if ((!pbuf) || (!data))
		return rst;

	err = d->intf_ops->read(rtlpriv, offset, pbuf, size, 0);
	if (err) {
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD, "%s: [ERROR] I/O FAIL!\n", __FUNCTION__);
		goto exit;
	}

	_rtl_memcpy(data, pbuf, size);
	rst = _TRUE;
exit:
	rtl_mfree(pbuf, size);

	return rst;
}

static void _halmac_sdio_reg_write_8(void *p, u32 offset, u8 val)
{
	struct rtl_priv *rtlpriv;
	u8 *pbuf;
	int err;


	rtlpriv = (struct rtlpriv *)p;
	pbuf = rtl_zmalloc(1);
	if (!pbuf)
		return;
	_rtl_memcpy(pbuf, &val, 1);

	err = d->intf_ops->write(rtlpriv, offset, pbuf, 1, 0);
	if (err)
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD, "%s: [ERROR] I/O FAIL!\n", __FUNCTION__);

	rtl_mfree(pbuf, 1);
}

static void _halmac_sdio_reg_write_16(void *p, u32 offset, u16 val)
{
	struct rtl_priv *rtlpriv;
	u8 *pbuf;
	int err;


	rtlpriv = (struct rtlpriv *)p;
	val = cpu_to_le16(val);
	pbuf = rtl_zmalloc(2);
	if (!pbuf)
		return;
	_rtl_memcpy(pbuf, &val, 2);

	err = d->intf_ops->write(rtlpriv, offset, pbuf, 2, 0);
	if (err)
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD, "%s: [ERROR] I/O FAIL!\n", __FUNCTION__);

	rtl_mfree(pbuf, 2);
}

static void _halmac_sdio_reg_write_32(void *p, u32 offset, u32 val)
{
	struct rtl_priv *rtlpriv;
	u8 *pbuf;
	int err;


	rtlpriv = (struct rtlpriv *)p;
	val = cpu_to_le32(val);
	pbuf = rtl_zmalloc(4);
	if (!pbuf)
		return;
	_rtl_memcpy(pbuf, &val, 4);

	err = d->intf_ops->write(rtlpriv, offset, pbuf, 4, 0);
	if (err)
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD, "%s: [ERROR] I/O FAIL!\n", __FUNCTION__);

	rtl_mfree(pbuf, 4);
}

#else /* !CONFIG_SDIO_HCI */

static u8 _halmac_reg_read_8(void *p, u32 offset)
{
	struct rtl_priv *rtlpriv = (struct rtl_priv *)p;

	return rtl_read_byte(rtlpriv, offset);
}

static u16 _halmac_reg_read_16(void *p, u32 offset)
{
	struct rtl_priv *rtlpriv = (struct rtl_priv *)p;

	return rtl_read_word(rtlpriv, offset);
}

static u32 _halmac_reg_read_32(void *p, u32 offset)
{
	struct rtl_priv *rtlpriv = (struct rtl_priv *)p;

	return rtl_read_dword(rtlpriv, offset);
}

static void _halmac_reg_write_8(void *p, u32 offset, u8 val)
{
	struct rtl_priv *rtlpriv = (struct rtl_priv *)p;

	rtl_write_byte(rtlpriv, offset, val);
}

static void _halmac_reg_write_16(void *p, u32 offset, u16 val)
{
	struct rtl_priv *rtlpriv = (struct rtl_priv *)p;

	rtl_write_word(rtlpriv, offset, val);
}

static void _halmac_reg_write_32(void *p, u32 offset, u32 val)
{
	struct rtl_priv *rtlpriv = (struct rtl_priv *)p;

	rtl_write_dword(rtlpriv, offset, val);
}
#endif /* !CONFIG_SDIO_HCI */

static bool _halmac_write_data_rsvd_page(void *p, u8 *buf, u32 size)
{
	struct rtl_priv *rtlpriv = (struct rtl_priv *)p;

	if (rtlpriv->cfg->ops->halmac_cb_write_data_rsvd_page &&
	    rtlpriv->cfg->ops->halmac_cb_write_data_rsvd_page(rtlpriv, buf,
							      size))
		return true;

	return false;
}

static bool _halmac_write_data_h2c(void *p, u8 *buf, u32 size)
{
	struct rtl_priv *rtlpriv = (struct rtl_priv *)p;

	if (rtlpriv->cfg->ops->halmac_cb_write_data_h2c &&
	    rtlpriv->cfg->ops->halmac_cb_write_data_h2c(rtlpriv, buf, size))
		return true;

	return false;
}

#ifdef PLATFORM_RTL_FREE	/* for uncooked use */
static BOOL _halmac_mfree(void *p, void *buffer, u32 size)
{
	kfree(buffer);
	return _TRUE;
}

static void *_halmac_malloc(void *p, u32 size)
{
	return kzalloc(size, GFP_KERNEL);
}

static BOOL _halmac_memcpy(void *p, void *dest, void *src, u32 size)
{
	memcpy(dest, src, size);
	return _TRUE;
}

static BOOL _halmac_memset(void *p, void *addr, u8 value, u32 size)
{
	memset(addr, value, size);
	return _TRUE;
}
#endif

#ifdef PLATFORM_RTL_DELAY_US	/* for uncooked use */
static void _halmac_udelay(void *p, u32 us)
{
	udelay(us);
}
#endif

#ifdef PLATFORM_MUTEX_INIT	/* for uncooked use */
static BOOL _halmac_mutex_init(void *p, HALMAC_MUTEX *pMutex)
{
#if 0
	mutex_init(pMutex);
#else
	spin_lock_init(pMutex);
#endif
	return _TRUE;
}

static BOOL _halmac_mutex_deinit(void *p, HALMAC_MUTEX *pMutex)
{
#if 0
	mutex_destroy(pMutex);
#endif
	return _TRUE;
}

static BOOL _halmac_mutex_lock(void *p, HALMAC_MUTEX *pMutex)
{
#if 0
	int err;

	err = mutex_lock_interruptible(pMutex);
	if (err)
		return _FALSE;
#else
	spin_lock(pMutex);
#endif

	return _TRUE;
}

static BOOL _halmac_mutex_unlock(void *p, HALMAC_MUTEX *pMutex)
{
#if 0
	mutex_unlock(pMutex);
#else
	spin_unlock(pMutex);
#endif

	return _TRUE;
}
#endif

#ifdef PLATFORM_MSG_PRINT	/* for uncooked use */
static bool _halmac_msg_print(void *p, u32 msg_type, u8 msg_level, s8 *fmt, ...)
{
#define MSG_LEN 100
#define MSG_PREFIX "[HALMAC]"
	va_list args;
	u8 str[MSG_LEN] = {0};
	u32 type;
	struct rtl_priv *rtlpriv = (struct rtl_priv *)p;

	str[0] = '\n';
	type = 0xFFFFFFFF;

	if (!(type & BIT(msg_type)))
		return true;

	va_start(args, fmt);
	vsnprintf(str, MSG_LEN, fmt, args);
	va_end(args);

	if (msg_level <= HALMAC_DBG_ERR)
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_EMERG, MSG_PREFIX "%s", str);
	else if (msg_level <= HALMAC_DBG_WARN)
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_WARNING, MSG_PREFIX "%s",
			 str);
	else
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_DMESG, MSG_PREFIX "%s", str);

	return true;
}

static bool _halmac_buff_print(void *p, u32 msg_type, u8 msg_level, s8 *buf,
			       u32 size)
{
#define MSG_PREFIX "[HALMAC]"
	u32 type;
	struct rtl_priv *rtlpriv = (struct rtl_priv *)p;

	type = 0xFFFFFFFF;

	if (!(type & BIT(msg_type)))
		return true;

	if (msg_level <= HALMAC_DBG_WARN)
		RT_PRINT_DATA(rtlpriv, COMP_HALMAC, DBG_WARNING, MSG_PREFIX,
			      buf, size);
	else
		RT_PRINT_DATA(rtlpriv, COMP_HALMAC, DBG_DMESG, MSG_PREFIX, buf,
			      size);

	return true;
}
#endif

static const char *const RTL_HALMAC_FEATURE_NAME[] = {
	"HALMAC_FEATURE_CFG_PARA",
	"HALMAC_FEATURE_DUMP_PHYSICAL_EFUSE",
	"HALMAC_FEATURE_DUMP_LOGICAL_EFUSE",
	"HALMAC_FEATURE_UPDATE_PACKET",
	"HALMAC_FEATURE_UPDATE_DATAPACK",
	"HALMAC_FEATURE_RUN_DATAPACK",
	"HALMAC_FEATURE_CHANNEL_SWITCH",
	"HALMAC_FEATURE_IQK",
	"HALMAC_FEATURE_POWER_TRACKING",
	"HALMAC_FEATURE_PSD",
	"HALMAC_FEATURE_ALL"};

static inline bool is_valid_id_status(struct rtl_priv *rtlpriv,
				      enum halmac_feature_id id,
				      enum halmac_cmd_process_status status)
{
	switch (id) {
	case HALMAC_FEATURE_CFG_PARA:
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD, "%s: %s\n", __func__,
			 RTL_HALMAC_FEATURE_NAME[id]);
		break;
	case HALMAC_FEATURE_DUMP_PHYSICAL_EFUSE:
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD, "%s: %s\n", __func__,
			 RTL_HALMAC_FEATURE_NAME[id]);
		if (status != HALMAC_CMD_PROCESS_DONE) {
			RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD,
				 "%s: <WARN> id(%d) unspecified status(%d)!\n",
				 __func__, id, status);
		}
		break;
	case HALMAC_FEATURE_DUMP_LOGICAL_EFUSE:
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD, "%s: %s\n", __func__,
			 RTL_HALMAC_FEATURE_NAME[id]);
		if (status != HALMAC_CMD_PROCESS_DONE) {
			RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD,
				 "%s: <WARN> id(%d) unspecified status(%d)!\n",
				 __func__, id, status);
		}
		break;
	case HALMAC_FEATURE_UPDATE_PACKET:
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD, "%s: %s\n", __func__,
			 RTL_HALMAC_FEATURE_NAME[id]);
		break;
	case HALMAC_FEATURE_UPDATE_DATAPACK:
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD, "%s: %s\n", __func__,
			 RTL_HALMAC_FEATURE_NAME[id]);
		break;
	case HALMAC_FEATURE_RUN_DATAPACK:
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD, "%s: %s\n", __func__,
			 RTL_HALMAC_FEATURE_NAME[id]);
		break;
	case HALMAC_FEATURE_CHANNEL_SWITCH:
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD, "%s: %s\n", __func__,
			 RTL_HALMAC_FEATURE_NAME[id]);
		break;
	case HALMAC_FEATURE_IQK:
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD, "%s: %s\n", __func__,
			 RTL_HALMAC_FEATURE_NAME[id]);
		break;
	case HALMAC_FEATURE_POWER_TRACKING:
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD, "%s: %s\n", __func__,
			 RTL_HALMAC_FEATURE_NAME[id]);
		break;
	case HALMAC_FEATURE_PSD:
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD, "%s: %s\n", __func__,
			 RTL_HALMAC_FEATURE_NAME[id]);
		break;
	case HALMAC_FEATURE_ALL:
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD, "%s: %s\n", __func__,
			 RTL_HALMAC_FEATURE_NAME[id]);
		break;
	default:
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD,
			 "%s: unknown feature id(%d)\n", __func__, id);
		return false;
	}

	return true;
}

static int init_halmac_event_with_waittime(struct rtl_priv *rtlpriv,
					   enum halmac_feature_id id, u8 *buf,
					   u32 size, u32 time)
{
	struct completion *comp;

	if (!rtlpriv->halmac.indicator[id].comp) {
		comp = kzalloc(sizeof(*comp), GFP_KERNEL);
		if (!comp)
			return -1;
	} else {
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD,
			 "%s: <WARN> id(%d) sctx is not NULL!!\n", __func__,
			 id);
		comp = rtlpriv->halmac.indicator[id].comp;
		rtlpriv->halmac.indicator[id].comp = NULL;
	}

	init_completion(comp);
	rtlpriv->halmac.indicator[id].wait_ms = time;

	rtlpriv->halmac.indicator[id].buffer = buf;
	rtlpriv->halmac.indicator[id].buf_size = size;
	rtlpriv->halmac.indicator[id].ret_size = 0;
	rtlpriv->halmac.indicator[id].status = 0;
	/* fill sctx at least to sure other variables are all ready! */
	rtlpriv->halmac.indicator[id].comp = comp;

	return 0;
}

static inline int init_halmac_event(struct rtl_priv *rtlpriv,
				    enum halmac_feature_id id, u8 *buf,
				    u32 size)
{
	return init_halmac_event_with_waittime(rtlpriv, id, buf, size,
					       DEFAULT_INDICATOR_TIMELMT);
}

static void free_halmac_event(struct rtl_priv *rtlpriv,
			      enum halmac_feature_id id)
{
	struct completion *comp;

	if (!rtlpriv->halmac.indicator[id].comp)
		return;

	comp = rtlpriv->halmac.indicator[id].comp;
	rtlpriv->halmac.indicator[id].comp = NULL;
	kfree(comp);
}

static int wait_halmac_event(struct rtl_priv *rtlpriv,
			     enum halmac_feature_id id)
{
	struct completion *comp;
	int ret;

	comp = rtlpriv->halmac.indicator[id].comp;
	if (!comp)
		return -1;

	ret = wait_for_completion_timeout(
		comp, rtlpriv->halmac.indicator[id].wait_ms);
	free_halmac_event(rtlpriv, id);
	if (ret > 0)
		return 0;

	return -1;
}

/*
 * Return:
 *	Always return true, HALMAC don't care the return value.
 */
static bool
_halmac_event_indication(void *p, enum halmac_feature_id feature_id,
			 enum halmac_cmd_process_status process_status, u8 *buf,
			 u32 size)
{
	struct rtl_priv *rtlpriv;
	struct rtl_halmac_indicator *tbl, *indicator;
	struct completion *comp;
	u32 cpsz;
	bool ret;

	rtlpriv = (struct rtl_priv *)p;
	tbl = rtlpriv->halmac.indicator;

	ret = is_valid_id_status(rtlpriv, feature_id, process_status);
	if (!ret)
		goto exit;

	indicator = &tbl[feature_id];
	indicator->status = process_status;
	indicator->ret_size = size;
	if (!indicator->comp) {
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD,
			 "%s: No feature id(%d) waiting!!\n", __func__,
			 feature_id);
		goto exit;
	}
	comp = indicator->comp;

	if (process_status == HALMAC_CMD_PROCESS_ERROR) {
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD,
			 "%s: Something wrong id(%d)!!\n", __func__,
			 feature_id);
		complete(comp); /* may provide error code */
		goto exit;
	}

	if (size > indicator->buf_size) {
		RT_TRACE(
			rtlpriv, COMP_HALMAC, DBG_LOUD,
			"%s: <WARN> id(%d) buffer is not enough(%d<%d), data will be truncated!\n",
			__func__, feature_id, indicator->buf_size, size);
		cpsz = indicator->buf_size;
	} else {
		cpsz = size;
	}

	if (cpsz && indicator->buffer)
		memcpy(indicator->buffer, buf, cpsz);

	complete(comp);

exit:
	return true;
}

static struct halmac_platform_api rtl_halmac_platform_api = {
	/* R/W register */
#ifdef CONFIG_SDIO_HCI
	.SDIO_CMD52_READ = _halmac_sdio_cmd52_read,
	.SDIO_CMD53_READ_8 = _halmac_sdio_reg_read_8,
	.SDIO_CMD53_READ_16 = _halmac_sdio_reg_read_16,
	.SDIO_CMD53_READ_32 = _halmac_sdio_reg_read_32,
	.SDIO_CMD53_READ_N = _halmac_sdio_reg_read_n,
	.SDIO_CMD52_WRITE = _halmac_sdio_cmd52_write,
	.SDIO_CMD53_WRITE_8 = _halmac_sdio_reg_write_8,
	.SDIO_CMD53_WRITE_16 = _halmac_sdio_reg_write_16,
	.SDIO_CMD53_WRITE_32 = _halmac_sdio_reg_write_32,

#endif /* CONFIG_SDIO_HCI */
#if defined(CONFIG_USB_HCI) || defined(CONFIG_PCIE_HCI)
	.REG_READ_8 = _halmac_reg_read_8,
	.REG_READ_16 = _halmac_reg_read_16,
	.REG_READ_32 = _halmac_reg_read_32,
	.REG_WRITE_8 = _halmac_reg_write_8,
	.REG_WRITE_16 = _halmac_reg_write_16,
	.REG_WRITE_32 = _halmac_reg_write_32,
#endif /* CONFIG_USB_HCI || CONFIG_PCIE_HCI */

	/* Write data */
	/* impletement in HAL-IC level */
	.SEND_RSVD_PAGE = _halmac_write_data_rsvd_page,
	.SEND_H2C_PKT = _halmac_write_data_h2c,

#ifdef PLATFORM_RTL_FREE	/* for uncooked use */
	/* Memory allocate */
	.RTL_FREE = _halmac_mfree,
	.RTL_MALLOC = _halmac_malloc,
	.RTL_MEMCPY = _halmac_memcpy,
	.RTL_MEMSET = _halmac_memset,
#endif

#ifdef PLATFORM_RTL_DELAY_US	/* for uncooked use */
	/* Sleep */
	.RTL_DELAY_US = _halmac_udelay,
#endif

#ifdef PLATFORM_MUTEX_INIT	/* for uncooked use */
	/* Process Synchronization */
	.MUTEX_INIT = _halmac_mutex_init,
	.MUTEX_DEINIT = _halmac_mutex_deinit,
	.MUTEX_LOCK = _halmac_mutex_lock,
	.MUTEX_UNLOCK = _halmac_mutex_unlock,
#endif

#ifdef PLATFORM_MSG_PRINT	/* for uncooked use */
	/* debug message */
	.MSG_PRINT = _halmac_msg_print,
	.BUFF_PRINT = _halmac_buff_print,
#endif

	.EVENT_INDICATION = _halmac_event_indication,
};

#if 0
u8 rtl_halmac_read8(struct intf_hdl *pintfhdl, u32 addr)
{
	PHALMAC_ADAPTER mac;
	PHALMAC_API api;


	/* WARNING: pintf_dev should not be null! */
	mac = dvobj_to_halmac(pintfhdl->pintf_dev);
	api = HALMAC_GET_API(mac);

	return api->halmac_reg_read_8(mac, addr);
}

u16 rtl_halmac_read16(struct intf_hdl *pintfhdl, u32 addr)
{
	PHALMAC_ADAPTER mac;
	PHALMAC_API api;


	/* WARNING: pintf_dev should not be null! */
	mac = dvobj_to_halmac(pintfhdl->pintf_dev);
	api = HALMAC_GET_API(mac);

	return api->halmac_reg_read_16(mac, addr);
}

u32 rtl_halmac_read32(struct intf_hdl *pintfhdl, u32 addr)
{
	PHALMAC_ADAPTER mac;
	PHALMAC_API api;


	/* WARNING: pintf_dev should not be null! */
	mac = dvobj_to_halmac(pintfhdl->pintf_dev);
	api = HALMAC_GET_API(mac);

	return api->halmac_reg_read_32(mac, addr);
}

int rtl_halmac_write8(struct intf_hdl *pintfhdl, u32 addr, u8 value)
{
	PHALMAC_ADAPTER mac;
	PHALMAC_API api;
	HALMAC_RET_STATUS status;


	/* WARNING: pintf_dev should not be null! */
	mac = dvobj_to_halmac(pintfhdl->pintf_dev);
	api = HALMAC_GET_API(mac);

	status = api->halmac_reg_write_8(mac, addr, value);

	if (status == HALMAC_RET_SUCCESS)
		return 0;

	return -1;
}

int rtl_halmac_write16(struct intf_hdl *pintfhdl, u32 addr, u16 value)
{
	PHALMAC_ADAPTER mac;
	PHALMAC_API api;
	HALMAC_RET_STATUS status;


	/* WARNING: pintf_dev should not be null! */
	mac = dvobj_to_halmac(pintfhdl->pintf_dev);
	api = HALMAC_GET_API(mac);

	status = api->halmac_reg_write_16(mac, addr, value);

	if (status == HALMAC_RET_SUCCESS)
		return 0;

	return -1;
}

int rtl_halmac_write32(struct intf_hdl *pintfhdl, u32 addr, u32 value)
{
	PHALMAC_ADAPTER mac;
	PHALMAC_API api;
	HALMAC_RET_STATUS status;


	/* WARNING: pintf_dev should not be null! */
	mac = dvobj_to_halmac(pintfhdl->pintf_dev);
	api = HALMAC_GET_API(mac);

	status = api->halmac_reg_write_32(mac, addr, value);

	if (status == HALMAC_RET_SUCCESS)
		return 0;

	return -1;
}
#endif

static int init_priv(struct rtl_halmac *halmac)
{
	struct rtl_halmac_indicator *indicator;
	u32 count, size;

	halmac->send_general_info = 0;

	count = HALMAC_FEATURE_ALL + 1;
	size = sizeof(*indicator) * count;
	indicator = kzalloc(size, GFP_KERNEL);
	if (!indicator)
		return -1;
	halmac->indicator = indicator;

	return 0;
}

static void deinit_priv(struct rtl_halmac *halmac)
{
	struct rtl_halmac_indicator *indicator;

	indicator = halmac->indicator;
	halmac->indicator = NULL;
	kfree(indicator);
}

int rtl_halmac_init_adapter(struct rtl_priv *rtlpriv)
{
	struct halmac_adapter *halmac;
	struct halmac_api *api;
	enum halmac_interface intf;
	enum halmac_ret_status status;
	int err = 0;
	struct halmac_platform_api *pf_api = &rtl_halmac_platform_api;

	halmac = rtlpriv_to_halmac(rtlpriv);
	if (halmac) {
		err = 0;
		goto out;
	}

	err = init_priv(&rtlpriv->halmac);
	if (err)
		goto out;

#ifdef CONFIG_SDIO_HCI
	intf = HALMAC_INTERFACE_SDIO;
#elif defined(CONFIG_USB_HCI)
	intf = HALMAC_INTERFACE_USB;
#elif defined(CONFIG_PCIE_HCI)
	intf = HALMAC_INTERFACE_PCIE;
#else
#warning "INTERFACE(CONFIG_XXX_HCI) not be defined!!"
	intf = HALMAC_INTERFACE_UNDEFINE;
#endif
	status = halmac_init_adapter(rtlpriv, pf_api, intf, &halmac, &api);
	if (status != HALMAC_RET_SUCCESS) {
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD,
			 "%s: halmac_init_adapter fail!(status=%d)\n", __func__,
			 status);
		err = -1;
		goto out;
	}

	rtlpriv->halmac.internal = halmac;

out:
	if (err)
		rtl_halmac_deinit_adapter(rtlpriv);

	return err;
}

int rtl_halmac_deinit_adapter(struct rtl_priv *rtlpriv)
{
	struct halmac_adapter *halmac;
	enum halmac_ret_status status;
	int err = 0;

	halmac = rtlpriv_to_halmac(rtlpriv);
	if (!halmac) {
		err = 0;
		goto out;
	}

	deinit_priv(&rtlpriv->halmac);

	halmac_halt_api(halmac);

	status = halmac_deinit_adapter(halmac);
	rtlpriv->halmac.internal = NULL;
	if (status != HALMAC_RET_SUCCESS) {
		err = -1;
		goto out;
	}

out:
	return err;
}

int rtl_halmac_poweron(struct rtl_priv *rtlpriv)
{
	struct halmac_adapter *halmac;
	struct halmac_api *api;
	enum halmac_ret_status status;
	int err = -1;

	halmac = rtlpriv_to_halmac(rtlpriv);
	if (!halmac)
		goto out;

	api = HALMAC_GET_API(halmac);

	status = api->halmac_pre_init_system_cfg(halmac);
	if (status != HALMAC_RET_SUCCESS)
		goto out;

	status = api->halmac_mac_power_switch(halmac, HALMAC_MAC_POWER_ON);
	if (status != HALMAC_RET_SUCCESS)
		goto out;

	status = api->halmac_init_system_cfg(halmac);
	if (status != HALMAC_RET_SUCCESS)
		goto out;

	err = 0;
out:
	return err;
}

int rtl_halmac_poweroff(struct rtl_priv *rtlpriv)
{
	struct halmac_adapter *halmac;
	struct halmac_api *api;
	enum halmac_ret_status status;
	int err = -1;

	halmac = rtlpriv_to_halmac(rtlpriv);
	if (!halmac)
		goto out;

	api = HALMAC_GET_API(halmac);

	status = api->halmac_mac_power_switch(halmac, HALMAC_MAC_POWER_OFF);
	if (status != HALMAC_RET_SUCCESS)
		goto out;

	err = 0;
out:
	return err;
}

/*
 * Note:
 *	When this function return, the register REG_RCR may be changed.
 */
int rtl_halmac_config_rx_info(struct rtl_priv *rtlpriv,
			      enum halmac_drv_info info)
{
	struct halmac_adapter *halmac;
	struct halmac_api *api;
	enum halmac_ret_status status;
	int err = -1;

	halmac = rtlpriv_to_halmac(rtlpriv);
	api = HALMAC_GET_API(halmac);

	status = api->halmac_cfg_drv_info(halmac, info);
	if (status != HALMAC_RET_SUCCESS)
		goto out;

	err = 0;
out:
	return err;
}

static enum halmac_ret_status init_mac_flow(struct rtl_priv *rtlpriv)
{
	struct halmac_adapter *halmac;
	struct halmac_api *api;
	enum halmac_ret_status status;
	u8 wifi_test = 0;
	int err;

	halmac = rtlpriv_to_halmac(rtlpriv);
	api = HALMAC_GET_API(halmac);

#ifdef CONFIG_USB_HCI
	status = api->halmac_set_bulkout_num(halmac, d->RtNumOutPipes);
	if (status != HALMAC_RET_SUCCESS)
		goto out;
#endif /* CONFIG_USB_HCI */

	if (wifi_test)
		status = api->halmac_init_mac_cfg(halmac, HALMAC_TRX_MODE_WMM);
	else
		status = api->halmac_init_mac_cfg(halmac,
						  HALMAC_TRX_MODE_NORMAL);
	if (status != HALMAC_RET_SUCCESS)
		goto out;

	err = rtl_halmac_rx_agg_switch(rtlpriv, true);
	if (err)
		goto out;

	if (rtlpriv->cfg->maps[RTL_RC_VHT_RATE_1SS_MCS7])
		status = api->halmac_cfg_operation_mode(
			halmac, HALMAC_WIRELESS_MODE_AC);
	else if (rtlpriv->cfg->maps[RTL_RC_HT_RATEMCS7])
		status = api->halmac_cfg_operation_mode(halmac,
							HALMAC_WIRELESS_MODE_N);
	else if (rtlpriv->cfg->maps[RTL_RC_OFDM_RATE6M])
		status = api->halmac_cfg_operation_mode(halmac,
							HALMAC_WIRELESS_MODE_G);
	else
		status = api->halmac_cfg_operation_mode(halmac,
							HALMAC_WIRELESS_MODE_B);
	if (status != HALMAC_RET_SUCCESS)
		goto out;

out:
	return status;
}

static inline enum halmac_rf_type _rf_type_drv2halmac(enum rf_type rf_drv)
{
	enum halmac_rf_type rf_mac;

	switch (rf_drv) {
	case RF_1T2R:
		rf_mac = HALMAC_RF_1T2R;
		break;
#if 0
	case RF_2T4R:
		rf_mac = HALMAC_RF_2T4R;
		break;
#endif
	case RF_2T2R:
		rf_mac = HALMAC_RF_2T2R;
		break;
	case RF_1T1R:
		rf_mac = HALMAC_RF_1T1R;
		break;
	case RF_2T2R_GREEN:
		rf_mac = HALMAC_RF_2T2R_GREEN;
		break;
#if 0
	case RF_2T3R:
		rf_mac = HALMAC_RF_2T3R;
		break;
	case RF_3T3R:
		rf_mac = HALMAC_RF_3T3R;
		break;
	case RF_3T4R:
		rf_mac = HALMAC_RF_3T4R;
		break;
	case RF_4T4R:
		rf_mac = HALMAC_RF_4T4R;
		break;
#endif
	default:
		rf_mac = (enum halmac_rf_type)rf_drv;
		break;
	}

	return rf_mac;
}

static int _send_general_info(struct rtl_priv *rtlpriv)
{
	struct halmac_adapter *halmac;
	struct halmac_api *api;
	struct halmac_general_info info;
	enum halmac_ret_status status;

	halmac = rtlpriv_to_halmac(rtlpriv);
	if (!halmac)
		return -1;
	api = HALMAC_GET_API(halmac);

	memset(&info, 0, sizeof(info));
	info.rfe_type = rtlpriv->rtlhal.rfe_type;
	info.rf_type = _rf_type_drv2halmac(rtlpriv->phy.rf_type);

	status = api->halmac_send_general_info(halmac, &info);
	switch (status) {
	case HALMAC_RET_SUCCESS:
		break;
	case HALMAC_RET_NO_DLFW:
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_WARNING,
			 "%s: halmac_send_general_info() fail because fw not dl!\n",
			 __func__);
	/* fall through */
	default:
		return -1;
	}

	return 0;
}

/*
 * Notices:
 *	Make sure
 *	1. rtl_hal_get_hwreg(HW_VAR_RF_TYPE)
 *	2. HAL_DATA_TYPE.rfe_type
 *	already ready for use before calling this function.
 */
static int _halmac_init_hal(struct rtl_priv *rtlpriv, u8 *fw, u32 fwsize)
{
	struct halmac_adapter *halmac;
	struct halmac_api *api;
	enum halmac_ret_status status;
	bool ok;
	bool fw_ok = false;
	int err, err_ret = -1;

	halmac = rtlpriv_to_halmac(rtlpriv);
	if (!halmac)
		goto out;
	api = HALMAC_GET_API(halmac);

	/* StatePowerOff */

	/* SKIP: halmac_init_adapter (Already done before) */

	/* halmac_pre_Init_system_cfg */
	/* halmac_mac_power_switch(on) */
	/* halmac_Init_system_cfg */
	err = rtl_halmac_poweron(rtlpriv);
	if (err)
		goto out;

	/* StatePowerOn */

	/* DownloadFW */
	rtlpriv->halmac.send_general_info = 0;
	if (fw && fwsize) {
		err = rtl_halmac_dlfw(rtlpriv, fw, fwsize);
		if (err)
			goto out;
		fw_ok = true;
	}

	/* InitMACFlow */
	status = init_mac_flow(rtlpriv);
	if (status != HALMAC_RET_SUCCESS)
		goto out;

	/* halmac_send_general_info */
	if (fw_ok) {
		rtlpriv->halmac.send_general_info = 0;
		err = _send_general_info(rtlpriv);
		if (err)
			goto out;
	} else {
		rtlpriv->halmac.send_general_info = 1;
	}

	/* Init Phy parameter-MAC */
	if (rtlpriv->cfg->ops->halmac_cb_init_mac_register)
		ok = rtlpriv->cfg->ops->halmac_cb_init_mac_register(rtlpriv);
	else
		ok = false;

	if (!ok)
		goto out;

	/* StateMacInitialized */

	/* halmac_cfg_drv_info */
	err = rtl_halmac_config_rx_info(rtlpriv, HALMAC_DRV_INFO_PHY_STATUS);
	if (err)
		goto out;

	/* halmac_set_hw_value(HALMAC_HW_EN_BB_RF) */
	/* Init BB, RF */
	if (rtlpriv->cfg->ops->halmac_cb_init_bb_rf_register)
		ok = rtlpriv->cfg->ops->halmac_cb_init_bb_rf_register(rtlpriv);
	else
		ok = false;

	if (!ok)
		goto out;

	status = api->halmac_init_interface_cfg(halmac);
	if (status != HALMAC_RET_SUCCESS)
		goto out;

	/* SKIP: halmac_verify_platform_api */
	/* SKIP: halmac_h2c_lb */

	/* StateRxIdle */

	err_ret = 0;
out:
	return err_ret;
}

int rtl_halmac_init_hal(struct rtl_priv *rtlpriv)
{
	if (!rtlpriv->rtlhal.pfirmware || rtlpriv->rtlhal.fwsize == 0)
		return -1;

	return _halmac_init_hal(rtlpriv, rtlpriv->rtlhal.pfirmware,
				rtlpriv->rtlhal.fwsize);
}

#if 0 /* download fw in rtl8822be_init_sw_vars() */
/*
 * Notices:
 *	Make sure
 *	1. rtl_hal_get_hwreg(HW_VAR_RF_TYPE)
 *	2. HAL_DATA_TYPE.rfe_type
 *	already ready for use before calling this function.
 */
int rtl_halmac_init_hal_fw(struct rtl_priv *rtlpriv, u8 *fw, u32 fwsize)
{
	return _halmac_init_hal(rtlpriv, fw, fwsize);
}
#endif

#if 0 /* download fw in rtl8822be_init_sw_vars() */
/*
 * Notices:
 *	Make sure
 *	1. rtl_hal_get_hwreg(HW_VAR_RF_TYPE)
 *	2. HAL_DATA_TYPE.rfe_type
 *	already ready for use before calling this function.
 */
int rtl_halmac_init_hal_fw_file(struct rtl_priv *rtlpriv, u8 *fwpath)
{
	u8 *fw = NULL;
	u32 fwmaxsize, size = 0;
	int err = 0;


	fwmaxsize = FIRMWARE_MAX_SIZE;
	fw = kzalloc(fwmaxsize, GFP_KERNEL);
	if (!fw)
		return -1;

	size = rtl_retrieve_from_file(fwpath, fw, fwmaxsize);
	if (!size) {
		err = -1;
		goto exit;
	}

	err = _halmac_init_hal(rtlpriv, fw, size);

exit:
	kfree(fw, fwmaxsize);
	fw = NULL;

	return err;
}
#endif

int rtl_halmac_deinit_hal(struct rtl_priv *rtlpriv)
{
	struct halmac_adapter *halmac;
	struct halmac_api *api;
	enum halmac_ret_status status;
	int err = -1;

	halmac = rtlpriv_to_halmac(rtlpriv);
	if (!halmac)
		goto out;
	api = HALMAC_GET_API(halmac);

	status = api->halmac_deinit_interface_cfg(halmac);
	if (status != HALMAC_RET_SUCCESS)
		goto out;

	/* rtw_hal_power_off(adapter); */
#if 0
	err = rtl_halmac_poweroff(rtlpriv);
	if (err)
		goto out;
#else
	status = api->halmac_mac_power_switch(halmac, HALMAC_MAC_POWER_OFF);
	if (status != HALMAC_RET_SUCCESS)
		goto out;
#endif

	err = 0;
out:
	return err;
}

int rtl_halmac_self_verify(struct rtl_priv *rtlpriv)
{
	struct halmac_adapter *mac;
	struct halmac_api *api;
	enum halmac_ret_status status;
	int err = -1;

	mac = rtlpriv_to_halmac(rtlpriv);
	api = HALMAC_GET_API(mac);

	status = api->halmac_verify_platform_api(mac);
	if (status != HALMAC_RET_SUCCESS)
		goto out;

	status = api->halmac_h2c_lb(mac);
	if (status != HALMAC_RET_SUCCESS)
		goto out;

	err = 0;
out:
	return err;
}

int rtl_halmac_dlfw(struct rtl_priv *rtlpriv, u8 *fw, u32 fwsize)
{
	struct halmac_adapter *mac;
	struct halmac_api *api;
	enum halmac_ret_status status;
	struct halmac_fw_version fw_version;
	int err = 0;

	mac = rtlpriv_to_halmac(rtlpriv);
	api = HALMAC_GET_API(mac);

	if ((!fw) || (!fwsize))
		return -1;

	/* 1. Driver Stop Tx */
	/* ToDo */

	/* 2. Driver Check Tx FIFO is empty */
	/* ToDo */

	/* 3. Config MAX download size */
	api->halmac_cfg_max_dl_size(mac, 0x1000);

	/* 4. Download Firmware */
	mac->h2c_packet_seq = 0;
	status = api->halmac_download_firmware(mac, fw, fwsize);
	if (status != HALMAC_RET_SUCCESS)
		return -1;

	status = api->halmac_get_fw_version(mac, &fw_version);
	if (status == HALMAC_RET_SUCCESS) {
		rtlpriv->rtlhal.fw_version = fw_version.version;
		rtlpriv->rtlhal.fw_subversion =
			(fw_version.sub_version << 8) | (fw_version.sub_index);

		RT_TRACE(
			rtlpriv, COMP_HALMAC, DBG_DMESG,
			"halmac report firmware version %04X.%04X\n",
			rtlpriv->rtlhal.fw_version,
			rtlpriv->rtlhal.fw_subversion);
	}

	if (rtlpriv->halmac.send_general_info) {
		rtlpriv->halmac.send_general_info = 0;
		err = _send_general_info(rtlpriv);
	}

	/* 5. Driver resume TX if needed */
	/* ToDo */

	/* 6. Reset driver variables if needed */
	/*hal->LastHMEBoxNum = 0;*/

	return err;
}

#if 0 /* download fw in rtl8822be_init_sw_vars() */
int rtl_halmac_dlfw_from_file(struct rtl_priv *rtlpriv, u8 *fwpath)
{
	u8 *fw = NULL;
	u32 fwmaxsize, size = 0;
	int err = 0;


	fwmaxsize = FIRMWARE_MAX_SIZE;
	fw = rtl_zmalloc(fwmaxsize);
	if (!fw)
		return -1;

	size = rtl_retrieve_from_file(fwpath, fw, fwmaxsize);
	if (size)
		err = rtl_halmac_dlfw(rtlpriv, fw, size);
	else
		err = -1;

	rtl_mfree(fw, fwmaxsize);
	fw = NULL;

	return err;
}
#endif

/*
 * Description:
 *	Power on/off BB/RF domain.
 *
 * Parameters:
 *	enable	true/false for power on/off
 *
 * Return:
 *	0	Success
 *	others	Fail
 */
int rtl_halmac_phy_power_switch(struct rtl_priv *rtlpriv, u8 enable)
{
	struct halmac_adapter *halmac;
	struct halmac_api *api;
	enum halmac_ret_status status;

	halmac = rtlpriv_to_halmac(rtlpriv);
	if (!halmac)
		return -1;
	api = HALMAC_GET_API(halmac);

	status = api->halmac_set_hw_value(halmac, HALMAC_HW_EN_BB_RF, &enable);
	if (status != HALMAC_RET_SUCCESS)
		return -1;

	return 0;
}

static bool _is_fw_read_cmd_down(struct rtl_priv *rtlpriv, u8 msgbox_num)
{
	bool read_down = false;
	int retry_cnts = 100;
	u8 valid;

	RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD,
		 "_is_fw_read_cmd_down, reg_1cc(%x), msg_box(%d)...\n",
		 rtl_read_byte(rtlpriv, REG_HMETFR), msgbox_num);

	do {
		valid = rtl_read_byte(rtlpriv, REG_HMETFR) & BIT(msgbox_num);
		if (valid == 0)
			read_down = true;
		else
			schedule();
	} while ((!read_down) && (retry_cnts--));

	return read_down;
}

int rtl_halmac_send_h2c(struct rtl_priv *rtlpriv, u8 *h2c)
{
	u8 h2c_box_num = 0;
	u32 msgbox_addr = 0;
	u32 msgbox_ex_addr = 0;
	__le32 h2c_cmd = 0;
	__le32 h2c_cmd_ex = 0;
	s32 ret = -1;
	unsigned long flag = 0;
	struct rtl_hal *rtlhal = rtl_hal(rtlpriv);

	if (!h2c) {
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD, "%s: pbuf is NULL\n",
			 __func__);
		return ret;
	}

	spin_lock_irqsave(&rtlpriv->locks.h2c_lock, flag);

	/* pay attention to if race condition happened in  H2C cmd setting */
	h2c_box_num = rtlhal->last_hmeboxnum;

	if (!_is_fw_read_cmd_down(rtlpriv, h2c_box_num)) {
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD,
			 " fw read cmd failed...\n");
		goto exit;
	}

	/* Write Ext command(byte 4 -7) */
	msgbox_ex_addr = REG_HMEBOX_E0 + (h2c_box_num * EX_MESSAGE_BOX_SIZE);
	memcpy((u8 *)(&h2c_cmd_ex), h2c + 4, EX_MESSAGE_BOX_SIZE);
	rtl_write_dword(rtlpriv, msgbox_ex_addr, le32_to_cpu(h2c_cmd_ex));

	/* Write command (byte 0 -3 ) */
	msgbox_addr = REG_HMEBOX0 + (h2c_box_num * MESSAGE_BOX_SIZE);
	memcpy((u8 *)(&h2c_cmd), h2c, 4);
	rtl_write_dword(rtlpriv, msgbox_addr, le32_to_cpu(h2c_cmd));

	/* update last msg box number */
	rtlhal->last_hmeboxnum = (h2c_box_num + 1) % MAX_H2C_BOX_NUMS;
	ret = 0;

exit:
	spin_unlock_irqrestore(&rtlpriv->locks.h2c_lock, flag);
	return ret;
}

int rtl_halmac_c2h_handle(struct rtl_priv *rtlpriv, u8 *c2h, u32 size)
{
	struct halmac_adapter *mac;
	struct halmac_api *api;
	enum halmac_ret_status status;

	mac = rtlpriv_to_halmac(rtlpriv);
	api = HALMAC_GET_API(mac);

	status = api->halmac_get_c2h_info(mac, c2h, size);
	if (status != HALMAC_RET_SUCCESS)
		return -1;

	return 0;
}

int rtl_halmac_get_physical_efuse_size(struct rtl_priv *rtlpriv, u32 *size)
{
	struct halmac_adapter *mac;
	struct halmac_api *api;
	enum halmac_ret_status status;
	u32 val;

	mac = rtlpriv_to_halmac(rtlpriv);
	api = HALMAC_GET_API(mac);

	status = api->halmac_get_efuse_size(mac, &val);
	if (status != HALMAC_RET_SUCCESS)
		return -1;

	*size = val;
	return 0;
}

int rtl_halmac_read_physical_efuse_map(struct rtl_priv *rtlpriv, u8 *map,
				       u32 size)
{
	struct halmac_adapter *mac;
	struct halmac_api *api;
	enum halmac_ret_status status;
	enum halmac_feature_id id;
	int ret;

	mac = rtlpriv_to_halmac(rtlpriv);
	api = HALMAC_GET_API(mac);
	id = HALMAC_FEATURE_DUMP_PHYSICAL_EFUSE;

	ret = init_halmac_event(rtlpriv, id, map, size);
	if (ret)
		return -1;

	status = api->halmac_dump_efuse_map(mac, HALMAC_EFUSE_R_DRV);
	if (status != HALMAC_RET_SUCCESS) {
		free_halmac_event(rtlpriv, id);
		return -1;
	}

	ret = wait_halmac_event(rtlpriv, id);
	if (ret)
		return -1;

	return 0;
}

int rtl_halmac_read_physical_efuse(struct rtl_priv *rtlpriv, u32 offset,
				   u32 cnt, u8 *data)
{
	struct halmac_adapter *mac;
	struct halmac_api *api;
	enum halmac_ret_status status;
	u8 v;
	u32 i;

	mac = rtlpriv_to_halmac(rtlpriv);
	api = HALMAC_GET_API(mac);

	for (i = 0; i < cnt; i++) {
		status = api->halmac_read_efuse(mac, offset + i, &v);
		if (status != HALMAC_RET_SUCCESS)
			return -1;
		data[i] = v;
	}

	return 0;
}

int rtl_halmac_write_physical_efuse(struct rtl_priv *rtlpriv, u32 offset,
				    u32 cnt, u8 *data)
{
	struct halmac_adapter *mac;
	struct halmac_api *api;
	enum halmac_ret_status status;
	u32 i;

	mac = rtlpriv_to_halmac(rtlpriv);
	api = HALMAC_GET_API(mac);

	for (i = 0; i < cnt; i++) {
		status = api->halmac_write_efuse(mac, offset + i, data[i]);
		if (status != HALMAC_RET_SUCCESS)
			return -1;
	}

	return 0;
}

int rtl_halmac_get_logical_efuse_size(struct rtl_priv *rtlpriv, u32 *size)
{
	struct halmac_adapter *mac;
	struct halmac_api *api;
	enum halmac_ret_status status;
	u32 val;

	mac = rtlpriv_to_halmac(rtlpriv);
	api = HALMAC_GET_API(mac);

	status = api->halmac_get_logical_efuse_size(mac, &val);
	if (status != HALMAC_RET_SUCCESS)
		return -1;

	*size = val;
	return 0;
}

int rtl_halmac_read_logical_efuse_map(struct rtl_priv *rtlpriv, u8 *map,
				      u32 size)
{
	struct halmac_adapter *mac;
	struct halmac_api *api;
	enum halmac_ret_status status;
	enum halmac_feature_id id;
	int ret;

	mac = rtlpriv_to_halmac(rtlpriv);
	api = HALMAC_GET_API(mac);
	id = HALMAC_FEATURE_DUMP_LOGICAL_EFUSE;

	ret = init_halmac_event(rtlpriv, id, map, size);
	if (ret)
		return -1;

	status = api->halmac_dump_logical_efuse_map(mac, HALMAC_EFUSE_R_AUTO);
	if (status != HALMAC_RET_SUCCESS) {
		free_halmac_event(rtlpriv, id);
		return -1;
	}

	ret = wait_halmac_event(rtlpriv, id);
	if (ret)
		return -1;

	return 0;
}

int rtl_halmac_write_logical_efuse_map(struct rtl_priv *rtlpriv, u8 *map,
				       u32 size, u8 *maskmap, u32 masksize)
{
	struct halmac_adapter *mac;
	struct halmac_api *api;
	struct halmac_pg_efuse_info pginfo;
	enum halmac_ret_status status;

	mac = rtlpriv_to_halmac(rtlpriv);
	api = HALMAC_GET_API(mac);

	pginfo.efuse_map = map;
	pginfo.efuse_map_size = size;
	pginfo.efuse_mask = maskmap;
	pginfo.efuse_mask_size = masksize;

	status = api->halmac_pg_efuse_by_map(mac, &pginfo, HALMAC_EFUSE_R_AUTO);
	if (status != HALMAC_RET_SUCCESS)
		return -1;

	return 0;
}

int rtl_halmac_read_logical_efuse(struct rtl_priv *rtlpriv, u32 offset, u32 cnt,
				  u8 *data)
{
	struct halmac_adapter *mac;
	struct halmac_api *api;
	enum halmac_ret_status status;
	u8 v;
	u32 i;

	mac = rtlpriv_to_halmac(rtlpriv);
	api = HALMAC_GET_API(mac);

	for (i = 0; i < cnt; i++) {
		status = api->halmac_read_logical_efuse(mac, offset + i, &v);
		if (status != HALMAC_RET_SUCCESS)
			return -1;
		data[i] = v;
	}

	return 0;
}

int rtl_halmac_write_logical_efuse(struct rtl_priv *rtlpriv, u32 offset,
				   u32 cnt, u8 *data)
{
	struct halmac_adapter *mac;
	struct halmac_api *api;
	enum halmac_ret_status status;
	u32 i;

	mac = rtlpriv_to_halmac(rtlpriv);
	api = HALMAC_GET_API(mac);

	for (i = 0; i < cnt; i++) {
		status = api->halmac_write_logical_efuse(mac, offset + i,
							 data[i]);
		if (status != HALMAC_RET_SUCCESS)
			return -1;
	}

	return 0;
}

#if 0	/* remove this function, and use u8 intuitively */
static inline u8 _hw_port_drv2halmac(enum _hw_port hwport)
{
	u8 port = 0;


	switch (hwport) {
	case HW_PORT0:
		port = 0;
		break;
	case HW_PORT1:
		port = 1;
		break;
	case HW_PORT2:
		port = 2;
		break;
	case HW_PORT3:
		port = 3;
		break;
	case HW_PORT4:
		port = 4;
		break;
	default:
		port = hwport;
		break;
	}

	return port;
}
#endif

int rtl_halmac_set_mac_address(struct rtl_priv *rtlpriv, u8 hwport, u8 *addr)
{
	struct halmac_adapter *halmac;
	struct halmac_api *api;
	u8 port;
	union halmac_wlan_addr hwa;
	enum halmac_ret_status status;
	int err = -1;

	halmac = rtlpriv_to_halmac(rtlpriv);
	api = HALMAC_GET_API(halmac);

	port = hwport;
	memset(&hwa, 0, sizeof(hwa));
	memcpy(hwa.address, addr, 6);

	status = api->halmac_cfg_mac_addr(halmac, port, &hwa);
	if (status != HALMAC_RET_SUCCESS)
		goto out;

	err = 0;
out:
	return err;
}

int rtl_halmac_set_bssid(struct rtl_priv *rtlpriv, u8 hwport, u8 *addr)
{
	struct halmac_adapter *halmac;
	struct halmac_api *api;
	u8 port;
	union halmac_wlan_addr hwa;
	enum halmac_ret_status status;
	int err = -1;

	halmac = rtlpriv_to_halmac(rtlpriv);
	api = HALMAC_GET_API(halmac);
	port = hwport;

	memset(&hwa, 0, sizeof(union halmac_wlan_addr));
	memcpy(hwa.address, addr, 6);
	status = api->halmac_cfg_bssid(halmac, port, &hwa);
	if (status != HALMAC_RET_SUCCESS)
		goto out;

	err = 0;
out:
	return err;
}

int rtl_halmac_set_bandwidth(struct rtl_priv *rtlpriv, u8 channel,
			     u8 pri_ch_idx, u8 bw)
{
	struct halmac_adapter *mac;
	struct halmac_api *api;
	enum halmac_ret_status status;

	mac = rtlpriv_to_halmac(rtlpriv);
	api = HALMAC_GET_API(mac);

	status = api->halmac_cfg_ch_bw(mac, channel, pri_ch_idx, bw);
	if (status != HALMAC_RET_SUCCESS)
		return -1;

	return 0;
}

int rtl_halmac_get_hw_value(struct rtl_priv *rtlpriv, enum halmac_hw_id hw_id,
			    void *pvalue)
{
	struct halmac_adapter *mac;
	struct halmac_api *api;
	enum halmac_ret_status status;

	mac = rtlpriv_to_halmac(rtlpriv);
	api = HALMAC_GET_API(mac);

	status = api->halmac_get_hw_value(mac, hw_id, pvalue);
	if (status != HALMAC_RET_SUCCESS)
		return -1;

	return 0;
}

int rtl_halmac_dump_fifo(struct rtl_priv *rtlpriv,
			 enum hal_fifo_sel halmac_fifo_sel)
{
	struct halmac_adapter *mac;
	struct halmac_api *api;
	enum halmac_ret_status status;
	u8 *pfifo_map = NULL;
	u32 fifo_size = 0;
	s8 ret = 0;

	mac = rtlpriv_to_halmac(rtlpriv);
	api = HALMAC_GET_API(mac);

	fifo_size = api->halmac_get_fifo_size(mac, halmac_fifo_sel);
	if (fifo_size)
		pfifo_map = vmalloc(fifo_size);
	if (!pfifo_map)
		return -1;

	status = api->halmac_dump_fifo(mac, halmac_fifo_sel, 0, fifo_size,
				       pfifo_map);

	if (status != HALMAC_RET_SUCCESS) {
		ret = -1;
		goto _exit;
	}

_exit:
	if (pfifo_map)
		vfree(pfifo_map);
	return ret;
}

int rtl_halmac_rx_agg_switch(struct rtl_priv *rtlpriv, bool enable)
{
	struct halmac_adapter *halmac;
	struct halmac_api *api;
	struct halmac_rxagg_cfg rxaggcfg;
	enum halmac_ret_status status;
	int err = -1;

	halmac = rtlpriv_to_halmac(rtlpriv);
	api = HALMAC_GET_API(halmac);
	memset((void *)&rxaggcfg, 0, sizeof(rxaggcfg));

	if (enable) {
#ifdef CONFIG_SDIO_HCI
		rxaggcfg.mode = HALMAC_RX_AGG_MODE_DMA;
		rxaggcfg.threshold.drv_define = 0;
#elif defined(CONFIG_USB_HCI) && defined(CONFIG_USB_RX_AGGREGATION)
		switch (hal->rxagg_mode) {
		case RX_AGG_DISABLE:
			rxaggcfg.mode = HALMAC_RX_AGG_MODE_NONE;
			break;

		case RX_AGG_DMA:
			rxaggcfg.mode = HALMAC_RX_AGG_MODE_DMA;
			if (hal->rxagg_dma_size || hal->rxagg_dma_timeout) {
				rxaggcfg.threshold.drv_define = 1;
				rxaggcfg.threshold.timeout = hal->rxagg_dma_timeout;
				rxaggcfg.threshold.size = hal->rxagg_dma_size;
			}
			break;

		case RX_AGG_USB:
		case RX_AGG_MIX:
			rxaggcfg.mode = HALMAC_RX_AGG_MODE_USB;
			if (hal->rxagg_usb_size || hal->rxagg_usb_timeout) {
				rxaggcfg.threshold.drv_define = 1;
				rxaggcfg.threshold.timeout = hal->rxagg_usb_timeout;
				rxaggcfg.threshold.size = hal->rxagg_usb_size;
			}
			break;
		}
#elif defined(CONFIG_PCIE_HCI)
		/* enable RX agg. */
		/* PCIE do nothing */
#endif /* CONFIG_USB_HCI */
	} else {
		/* disable RX agg. */
		rxaggcfg.mode = HALMAC_RX_AGG_MODE_NONE;
	}

	status = api->halmac_cfg_rx_aggregation(halmac, &rxaggcfg);
	if (status != HALMAC_RET_SUCCESS)
		goto out;

	err = 0;
out:
	return err;
}

int rtl_halmac_get_wow_reason(struct rtl_priv *rtlpriv, u8 *reason)
{
	u8 val8;
	int err = -1;

	val8 = rtl_read_byte(rtlpriv, 0x1C7);
	if (val8 == 0xEA)
		goto out;

	*reason = val8;
	err = 0;
out:
	return err;
}

/*
 * Description:
 *	Get RX driver info size. RX driver info is a small memory space between
 *	scriptor and RX payload.
 *
 *	+-------------------------+
 *	| RX descriptor           |
 *	| usually 24 bytes        |
 *	+-------------------------+
 *	| RX driver info          |
 *	| depends on driver cfg   |
 *	+-------------------------+
 *	| RX paylad               |
 *	|                         |
 *	+-------------------------+
 *
 * Parameter:
 *	d	pointer to struct dvobj_priv of driver
 *	sz	rx driver info size in bytes.
 *
 * Rteurn:
 *	0	Success
 *	other	Fail
 */
int rtl_halmac_get_drv_info_sz(struct rtl_priv *rtlpriv, u8 *sz)
{
	/*	enum halmac_ret_status status; */
	u8 dw = 6; /* max number */

#if 0 /* TODO wait for halmac ready */
	status = api->halmac_get_hw_value(halmac, HALMAC_HW_RX_, &dw);
	if (status != HALMAC_RET_SUCCESS)
		return -1;
#endif

	*sz = dw * 8;
	return 0;
}

int rtl_halmac_get_rsvd_drv_pg_bndy(struct rtl_priv *rtlpriv, u16 *drv_pg)
{
	enum halmac_ret_status status;
	struct halmac_adapter *halmac = rtlpriv_to_halmac(rtlpriv);
	struct halmac_api *api = HALMAC_GET_API(halmac);

	status = api->halmac_get_hw_value(halmac, HALMAC_HW_RSVD_PG_BNDY,
					  drv_pg);
	if (status != HALMAC_RET_SUCCESS)
		return -1;

	return 0;
}

#if 0 /* Turn off temporally. */
int rtl_halmac_download_rsvd_page(struct rtl_priv *rtlpriv, u8 pg_offset, u8 *pbuf, u32 size)
{
	HALMAC_RET_STATUS status = HALMAC_RET_SUCCESS;
	PHALMAC_ADAPTER halmac = rtlpriv_to_halmac(rtlpriv);
	PHALMAC_API api = HALMAC_GET_API(halmac);

	status = api->halmac_dl_drv_rsvd_page(halmac, pg_offset, pbuf, size);
	if (status != HALMAC_RET_SUCCESS)
		return -1;

	return 0;

}
#endif

#ifdef CONFIG_SDIO_HCI

/*
 * Description:
 *	Update queue allocated page number to driver
 *
 * Parameter:
 *	d	pointer to struct dvobj_priv of driver
 *
 * Rteurn:
 *	0	Success, "page" is valid.
 *	others	Fail, "page" is invalid.
 */
int rtl_halmac_query_tx_page_num(struct rtl_priv *rtlpriv)
{
	struct halmacpriv *hmpriv;
	PHALMAC_ADAPTER halmac;
	PHALMAC_API api;
	HALMAC_RQPN_MAP rqpn;
	HALMAC_DMA_MAPPING dmaqueue;
	HALMAC_TXFF_ALLOCATION fifosize;
	HALMAC_RET_STATUS status;
	u8 i;


	hmpriv = &d->hmpriv;
	halmac = rtlpriv_to_halmac(rtlpriv);
	api = HALMAC_GET_API(halmac);
	_rtl_memset((void *)&rqpn, 0, sizeof(rqpn));
	_rtl_memset((void *)&fifosize, 0, sizeof(fifosize));

	status = api->halmac_get_hw_value(halmac, HALMAC_HW_RQPN_MAPPING, &rqpn);
	if (status != HALMAC_RET_SUCCESS)
		return -1;
	status = api->halmac_get_hw_value(halmac, HALMAC_HW_TXFF_ALLOCATION, &fifosize);
	if (status != HALMAC_RET_SUCCESS)
		return -1;

	for (i = 0; i < HW_QUEUE_ENTRY; i++) {
		hmpriv->txpage[i] = 0;

		/* Driver index mapping to HALMAC DMA queue */
		dmaqueue = HALMAC_DMA_MAPPING_UNDEFINE;
		switch (i) {
		case VO_QUEUE_INX:
			dmaqueue = rqpn.dma_map_vo;
			break;
		case VI_QUEUE_INX:
			dmaqueue = rqpn.dma_map_vi;
			break;
		case BE_QUEUE_INX:
			dmaqueue = rqpn.dma_map_be;
			break;
		case BK_QUEUE_INX:
			dmaqueue = rqpn.dma_map_bk;
			break;
		case MGT_QUEUE_INX:
			dmaqueue = rqpn.dma_map_mg;
			break;
		case HIGH_QUEUE_INX:
			dmaqueue = rqpn.dma_map_hi;
			break;
		case BCN_QUEUE_INX:
		case TXCMD_QUEUE_INX:
			/* Unlimited */
			hmpriv->txpage[i] = 0xFFFF;
			continue;
		}

		switch (dmaqueue) {
		case HALMAC_DMA_MAPPING_EXTRA:
			hmpriv->txpage[i] = fifosize.extra_queue_pg_num;
			break;
		case HALMAC_DMA_MAPPING_LOW:
			hmpriv->txpage[i] = fifosize.low_queue_pg_num;
			break;
		case HALMAC_DMA_MAPPING_NORMAL:
			hmpriv->txpage[i] = fifosize.normal_queue_pg_num;
			break;
		case HALMAC_DMA_MAPPING_HIGH:
			hmpriv->txpage[i] = fifosize.high_queue_pg_num;
			break;
		case HALMAC_DMA_MAPPING_UNDEFINE:
			break;
		}
		hmpriv->txpage[i] += fifosize.pub_queue_pg_num;
	}

	return 0;
}

/*
 * Description:
 *	Get specific queue allocated page number
 *
 * Parameter:
 *	d	pointer to struct dvobj_priv of driver
 *	queue	target queue to query, VO/VI/BE/BK/.../TXCMD_QUEUE_INX
 *	page	return allocated page number
 *
 * Rteurn:
 *	0	Success, "page" is valid.
 *	others	Fail, "page" is invalid.
 */
int rtl_halmac_get_tx_queue_page_num(struct rtl_priv *rtlpriv, u8 queue, u32 *page)
{
	*page = 0;
	if (queue < HW_QUEUE_ENTRY)
		*page = d->hmpriv.txpage[queue];

	return 0;
}

/*
 * Return:
 *	address for SDIO command
 */
u32 rtl_halmac_sdio_get_tx_addr(struct rtl_priv *rtlpriv, u8 *desc, u32 size)
{
	PHALMAC_ADAPTER mac;
	PHALMAC_API api;
	HALMAC_RET_STATUS status;
	u32 addr;


	mac = rtlpriv_to_halmac(rtlpriv);
	api = HALMAC_GET_API(mac);

	status = api->halmac_get_sdio_tx_addr(mac, desc, size, &addr);
	if (status != HALMAC_RET_SUCCESS)
		return 0;

	return addr;
}

int rtl_halmac_sdio_tx_allowed(struct rtl_priv *rtlpriv, u8 *buf, u32 size)
{
	PHALMAC_ADAPTER mac;
	PHALMAC_API api;
	HALMAC_RET_STATUS status;


	mac = rtlpriv_to_halmac(rtlpriv);
	api = HALMAC_GET_API(mac);

	status = api->halmac_tx_allowed_sdio(mac, buf, size);
	if (status != HALMAC_RET_SUCCESS)
		return -1;

	return 0;
}

u32 rtl_halmac_sdio_get_rx_addr(struct rtl_priv *rtlpriv, u8 *seq)
{
	u8 id;

#define RTL_SDIO_ADDR_RX_RX0FF_PRFIX	0x0E000
#define RTL_SDIO_ADDR_RX_RX0FF_GEN(a)	(RTL_SDIO_ADDR_RX_RX0FF_PRFIX|(a&0x3))

	id = *seq;
	(*seq)++;
	return RTL_SDIO_ADDR_RX_RX0FF_GEN(id);
}
#endif /* CONFIG_SDIO_HCI */

#ifdef CONFIG_USB_HCI
u8 rtl_halmac_usb_get_bulkout_id(struct rtl_priv *rtlpriv, u8 *buf, u32 size)
{
	PHALMAC_ADAPTER mac;
	PHALMAC_API api;
	HALMAC_RET_STATUS status;
	u8 bulkout_id;


	mac = rtlpriv_to_halmac(rtlpriv);
	api = HALMAC_GET_API(mac);

	status = api->halmac_get_usb_bulkout_id(mac, buf, size, &bulkout_id);
	if (status != HALMAC_RET_SUCCESS)
		return 0;

	return bulkout_id;
}

static inline HALMAC_USB_MODE _usb_mode_drv2halmac(enum RTL_USB_SPEED usb_mode)
{
	HALMAC_USB_MODE halmac_usb_mode = HALMAC_USB_MODE_U2;

	switch (usb_mode) {
	case RTL_USB_SPEED_2:
		halmac_usb_mode = HALMAC_USB_MODE_U2;
		break;
	case RTL_USB_SPEED_3:
		halmac_usb_mode = HALMAC_USB_MODE_U3;
		break;
	default:
		halmac_usb_mode = HALMAC_USB_MODE_U2;
		break;
	}

	return halmac_usb_mode;
}

u8 rtl_halmac_switch_usb_mode(struct rtl_priv *rtlpriv, enum RTL_USB_SPEED usb_mode)
{
	PHALMAC_ADAPTER mac;
	PHALMAC_API api;
	HALMAC_RET_STATUS status;
	HALMAC_USB_MODE halmac_usb_mode;

	mac = rtlpriv_to_halmac(rtlpriv);
	api = HALMAC_GET_API(mac);
	halmac_usb_mode = _usb_mode_drv2halmac(usb_mode);
	status = api->halmac_set_hw_value(mac, HALMAC_HW_USB_MODE, (void *)&halmac_usb_mode);

	if (status != HALMAC_RET_SUCCESS)
		return _FAIL;

	return _SUCCESS;
}
#endif /* CONFIG_USB_HCI */

int rtl_halmac_chk_txdesc(struct rtl_priv *rtlpriv, u8 *txdesc, u32 size)
{
	struct halmac_adapter *mac;
	struct halmac_api *api;
	enum halmac_ret_status status;

	mac = rtlpriv_to_halmac(rtlpriv);
	api = HALMAC_GET_API(mac);

	status = api->halmac_chk_txdesc(mac, txdesc, size);

	if (status != HALMAC_RET_SUCCESS)
		return -1;

	return 0;
}

/**
 * rtl_halmac_iqk() - Run IQ Calibration
 * @rtl_priv:	struct rtl_priv*
 * @clear:	IQK parameters
 * @segment:	IQK parameters
 *
 * Process IQ Calibration(IQK).
 *
 * Rteurn: 0 for OK, otherwise fail.
 */
int rtl_halmac_iqk(struct rtl_priv *rtlpriv, u8 clear, u8 segment)
{
	struct halmac_adapter *mac;
	struct halmac_api *api;
	enum halmac_ret_status status;
	enum halmac_feature_id id;
	struct halmac_iqk_para_ para;
	int ret;
	u8 retry = 3;
	u8 delay = 1; /* ms */
	unsigned long us;

	mac = rtlpriv_to_halmac(rtlpriv);
	api = HALMAC_GET_API(mac);
	id = HALMAC_FEATURE_IQK;

	ret = init_halmac_event(rtlpriv, id, NULL, 0);
	if (ret)
		return -1;

	para.clear = clear;
	para.segment_iqk = segment;

	do {
		status = api->halmac_start_iqk(mac, &para);
		if (status != HALMAC_RET_BUSY_STATE)
			break;
		RT_TRACE(rtlpriv, COMP_HALMAC, DBG_LOUD,
			 "%s: Fail to start IQK, status is BUSY! retry=%d\n",
			 __func__, retry);

		if (!retry)
			break;
		retry--;

		if (delay < 20) {
			us = delay * 1000UL;
			usleep_range(us, us + 1000UL);
		} else {
			msleep(delay);
		}

	} while (1);
	if (status != HALMAC_RET_SUCCESS) {
		free_halmac_event(rtlpriv, id);
		return -1;
	}

	ret = wait_halmac_event(rtlpriv, id);
	if (ret)
		return -1;

	return 0;
}

MODULE_AUTHOR("Realtek WlanFAE	<wlanfae@realtek.com>");
MODULE_AUTHOR("Larry Finger	<Larry.FInger@lwfinger.net>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Realtek 802.11n PCI wireless core");
