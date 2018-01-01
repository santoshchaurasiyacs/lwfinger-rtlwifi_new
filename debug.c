/******************************************************************************
 *
 * Copyright(c) 2009-2012  Realtek Corporation.
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
 *****************************************************************************/

#include "wifi.h"
#include "cam.h"

#include <linux/moduleparam.h>
#include <linux/vmalloc.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 2, 0))
#include <linux/export.h>
#endif

#ifdef CONFIG_RTLWIFI_DEBUG
void _rtl_dbg_trace(struct rtl_priv *rtlpriv, u64 comp, int level,
		    const char *fmt, ...)
{
	if (unlikely((comp & rtlpriv->cfg->mod_params->debug_mask) &&
		     level <= rtlpriv->cfg->mod_params->debug_level)) {
		struct va_format vaf;
		va_list args;

		va_start(args, fmt);

		vaf.fmt = fmt;
		vaf.va = &args;

		pr_info(":<%lx> %pV", in_interrupt(), &vaf);

		va_end(args);
	}
}
EXPORT_SYMBOL_GPL(_rtl_dbg_trace);

void _rtl_dbg_print(struct rtl_priv *rtlpriv, u64 comp, int level,
		    const char *fmt, ...)
{
	if (unlikely((comp & rtlpriv->cfg->mod_params->debug_mask) &&
		     level <= rtlpriv->cfg->mod_params->debug_level)) {
		struct va_format vaf;
		va_list args;

		va_start(args, fmt);

		vaf.fmt = fmt;
		vaf.va = &args;

		pr_info("%pV", &vaf);

		va_end(args);
	}
}
EXPORT_SYMBOL_GPL(_rtl_dbg_print);

void _rtl_dbg_print_data(struct rtl_priv *rtlpriv, u64 comp, int level,
			 const char *titlestring,
			 const void *hexdata, int hexdatalen)
{
	if (unlikely(((comp) & rtlpriv->cfg->mod_params->debug_mask) &&
		     ((level) <= rtlpriv->cfg->mod_params->debug_level))) {
		pr_info("In process \"%s\" (pid %i): %s\n",
			current->comm, current->pid, titlestring);
		print_hex_dump_bytes("", DUMP_PREFIX_NONE,
				     hexdata, hexdatalen);
	}
}
EXPORT_SYMBOL_GPL(_rtl_dbg_print_data);

struct rtl_debugfs_priv {
	struct rtl_priv *rtlpriv;
	int (*cb_read)(struct seq_file *m, void *v);
	ssize_t (*cb_write)(struct file *filp, const char __user *buffer,
			    size_t count, loff_t *loff);
	u32 cb_data;
};

static struct dentry *debugfs_topdir;

static int rtl_debug_get_common(struct seq_file *m, void *v)
{
	struct rtl_debugfs_priv *debugfs_priv = m->private;

	return debugfs_priv->cb_read(m, v);
}

static int dl_debug_open_common(struct inode *inode, struct file *file)
{
	return single_open(file, rtl_debug_get_common, inode->i_private);
}

static const struct file_operations file_ops_common = {
	.open = dl_debug_open_common,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

static int rtl_debug_get_mac_page(struct seq_file *m, void *v)
{
	struct rtl_debugfs_priv *debugfs_priv = m->private;
	struct rtl_priv *rtlpriv = debugfs_priv->rtlpriv;
	u32 page = debugfs_priv->cb_data;
	int i, n;
	int max = 0xff;

	for (n = 0; n <= max; ) {
		seq_printf(m, "\n%8.8x  ", n + page);
		for (i = 0; i < 4 && n <= max; i++, n += 4)
			seq_printf(m, "%8.8x    ",
				   rtl_read_dword(rtlpriv, (page | n)));
	}
	seq_puts(m, "\n");
	return 0;
}

#define RTL_DEBUG_IMPL_MAC_SERIES(page, addr)			\
static struct rtl_debugfs_priv rtl_debug_priv_mac_ ##page = {	\
	.cb_read = rtl_debug_get_mac_page,			\
	.cb_data = addr,					\
}

RTL_DEBUG_IMPL_MAC_SERIES(0, 0x0000);
RTL_DEBUG_IMPL_MAC_SERIES(1, 0x0100);
RTL_DEBUG_IMPL_MAC_SERIES(2, 0x0200);
RTL_DEBUG_IMPL_MAC_SERIES(3, 0x0300);
RTL_DEBUG_IMPL_MAC_SERIES(4, 0x0400);
RTL_DEBUG_IMPL_MAC_SERIES(5, 0x0500);
RTL_DEBUG_IMPL_MAC_SERIES(6, 0x0600);
RTL_DEBUG_IMPL_MAC_SERIES(7, 0x0700);
RTL_DEBUG_IMPL_MAC_SERIES(10, 0x1000);
RTL_DEBUG_IMPL_MAC_SERIES(11, 0x1100);
RTL_DEBUG_IMPL_MAC_SERIES(12, 0x1200);
RTL_DEBUG_IMPL_MAC_SERIES(13, 0x1300);
RTL_DEBUG_IMPL_MAC_SERIES(14, 0x1400);
RTL_DEBUG_IMPL_MAC_SERIES(15, 0x1500);
RTL_DEBUG_IMPL_MAC_SERIES(16, 0x1600);
RTL_DEBUG_IMPL_MAC_SERIES(17, 0x1700);

static int rtl_debug_get_bb_page(struct seq_file *m, void *v)
{
	struct rtl_debugfs_priv *debugfs_priv = m->private;
	struct rtl_priv *rtlpriv = debugfs_priv->rtlpriv;
	struct ieee80211_hw *hw = rtlpriv->hw;
	u32 page = debugfs_priv->cb_data;
	int i, n;
	int max = 0xff;

	for (n = 0; n <= max; ) {
		seq_printf(m, "\n%8.8x  ", n + page);
		for (i = 0; i < 4 && n <= max; i++, n += 4)
			seq_printf(m, "%8.8x    ",
				   rtl_get_bbreg(hw, (page | n), 0xffffffff));
	}
	seq_puts(m, "\n");
	return 0;
}

#define RTL_DEBUG_IMPL_BB_SERIES(page, addr)			\
static struct rtl_debugfs_priv rtl_debug_priv_bb_ ##page = {	\
	.cb_read = rtl_debug_get_bb_page,			\
	.cb_data = addr,					\
}

RTL_DEBUG_IMPL_BB_SERIES(8, 0x0800);
RTL_DEBUG_IMPL_BB_SERIES(9, 0x0900);
RTL_DEBUG_IMPL_BB_SERIES(a, 0x0a00);
RTL_DEBUG_IMPL_BB_SERIES(b, 0x0b00);
RTL_DEBUG_IMPL_BB_SERIES(c, 0x0c00);
RTL_DEBUG_IMPL_BB_SERIES(d, 0x0d00);
RTL_DEBUG_IMPL_BB_SERIES(e, 0x0e00);
RTL_DEBUG_IMPL_BB_SERIES(f, 0x0f00);
RTL_DEBUG_IMPL_BB_SERIES(18, 0x1800);
RTL_DEBUG_IMPL_BB_SERIES(19, 0x1900);
RTL_DEBUG_IMPL_BB_SERIES(1a, 0x1a00);
RTL_DEBUG_IMPL_BB_SERIES(1b, 0x1b00);
RTL_DEBUG_IMPL_BB_SERIES(1c, 0x1c00);
RTL_DEBUG_IMPL_BB_SERIES(1d, 0x1d00);
RTL_DEBUG_IMPL_BB_SERIES(1e, 0x1e00);
RTL_DEBUG_IMPL_BB_SERIES(1f, 0x1f00);

static int rtl_debug_get_reg_rf(struct seq_file *m, void *v)
{
	struct rtl_debugfs_priv *debugfs_priv = m->private;
	struct rtl_priv *rtlpriv = debugfs_priv->rtlpriv;
	struct ieee80211_hw *hw = rtlpriv->hw;
	enum radio_path rfpath = debugfs_priv->cb_data;
	int i, n;
	int max = 0x40;

	if (IS_HARDWARE_TYPE_8822B(rtlpriv))
		max = 0xff;

	seq_printf(m, "\nPATH(%d)", rfpath);

	for (n = 0; n <= max; ) {
		seq_printf(m, "\n%8.8x  ", n);
		for (i = 0; i < 4 && n <= max; n += 1, i++)
			seq_printf(m, "%8.8x    ",
				   rtl_get_rfreg(hw, rfpath, n, 0xffffffff));
	}
	seq_puts(m, "\n");
	return 0;
}

#define RTL_DEBUG_IMPL_RF_SERIES(page, addr)			\
static struct rtl_debugfs_priv rtl_debug_priv_rf_ ##page = {	\
	.cb_read = rtl_debug_get_reg_rf,			\
	.cb_data = addr,					\
}

RTL_DEBUG_IMPL_RF_SERIES(a, RF90_PATH_A);
RTL_DEBUG_IMPL_RF_SERIES(b, RF90_PATH_B);

static int rtl_debug_get_cam_register(struct seq_file *m, void *v)
{
	struct rtl_debugfs_priv *debugfs_priv = m->private;
	struct rtl_priv *rtlpriv = debugfs_priv->rtlpriv;
	int start = debugfs_priv->cb_data;
	u32 target_cmd = 0;
	u32 target_val = 0;
	u8 entry_i = 0;
	u32 ulstatus;
	int i = 100, j = 0;
	int end = (start + 11 > TOTAL_CAM_ENTRY ? TOTAL_CAM_ENTRY : start + 11);

	/* This dump the current register page */
	seq_printf(m,
		   "\n#################### SECURITY CAM (%d-%d) ##################\n",
		   start, end - 1);

	for (j = start; j < end; j++) {
		seq_printf(m, "\nD:  %2x > ", j);
		for (entry_i = 0; entry_i < CAM_CONTENT_COUNT; entry_i++) {
			/* polling bit, and No Write enable, and address  */
			target_cmd = entry_i + CAM_CONTENT_COUNT * j;
			target_cmd = target_cmd | BIT(31);

			/* Check polling bit is clear */
			while ((i--) >= 0) {
				ulstatus = rtl_read_dword(
						rtlpriv,
						rtlpriv->cfg->maps[RWCAM]);
				if (ulstatus & BIT(31))
					continue;
				else
					break;
			}

			rtl_write_dword(rtlpriv, rtlpriv->cfg->maps[RWCAM],
					target_cmd);
			target_val = rtl_read_dword(rtlpriv,
						    rtlpriv->cfg->maps[RCAMO]);
			seq_printf(m, "%8.8x ", target_val);
		}
	}
	seq_puts(m, "\n");
	return 0;
}

#define RTL_DEBUG_IMPL_CAM_SERIES(page, addr)			\
static struct rtl_debugfs_priv rtl_debug_priv_cam_ ##page = {	\
	.cb_read = rtl_debug_get_cam_register,			\
	.cb_data = addr,					\
}

RTL_DEBUG_IMPL_CAM_SERIES(1, 0);
RTL_DEBUG_IMPL_CAM_SERIES(2, 11);
RTL_DEBUG_IMPL_CAM_SERIES(3, 22);

static const char * const table_txpwr_lmt[] = {
	"FCC", "2.4G", "20M", "CCK", "1T", "01", "32",
	"ETSI", "2.4G", "20M", "CCK", "1T", "01", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "01", "30",
	"FCC", "2.4G", "20M", "CCK", "1T", "02", "32",
	"ETSI", "2.4G", "20M", "CCK", "1T", "02", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "02", "30",
	"FCC", "2.4G", "20M", "CCK", "1T", "03", "32",
	"ETSI", "2.4G", "20M", "CCK", "1T", "03", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "03", "30",
	"FCC", "2.4G", "20M", "CCK", "1T", "04", "32",
	"ETSI", "2.4G", "20M", "CCK", "1T", "04", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "04", "30",
	"FCC", "2.4G", "20M", "CCK", "1T", "05", "32",
	"ETSI", "2.4G", "20M", "CCK", "1T", "05", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "05", "30",
	"FCC", "2.4G", "20M", "CCK", "1T", "06", "32",
	"ETSI", "2.4G", "20M", "CCK", "1T", "06", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "06", "30",
	"FCC", "2.4G", "20M", "CCK", "1T", "07", "32",
	"ETSI", "2.4G", "20M", "CCK", "1T", "07", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "07", "30",
	"FCC", "2.4G", "20M", "CCK", "1T", "08", "32",
	"ETSI", "2.4G", "20M", "CCK", "1T", "08", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "08", "30",
	"FCC", "2.4G", "20M", "CCK", "1T", "09", "32",
	"ETSI", "2.4G", "20M", "CCK", "1T", "09", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "09", "30",
	"FCC", "2.4G", "20M", "CCK", "1T", "10", "32",
	"ETSI", "2.4G", "20M", "CCK", "1T", "10", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "10", "30",
	"FCC", "2.4G", "20M", "CCK", "1T", "11", "32",
	"ETSI", "2.4G", "20M", "CCK", "1T", "11", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "11", "30",
	"FCC", "2.4G", "20M", "CCK", "1T", "12", "26",
	"ETSI", "2.4G", "20M", "CCK", "1T", "12", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "12", "30",
	"FCC", "2.4G", "20M", "CCK", "1T", "13", "20",
	"ETSI", "2.4G", "20M", "CCK", "1T", "13", "28",
	"MKK", "2.4G", "20M", "CCK", "1T", "13", "28",
	"FCC", "2.4G", "20M", "CCK", "1T", "14", "63",
	"ETSI", "2.4G", "20M", "CCK", "1T", "14", "63",
	"MKK", "2.4G", "20M", "CCK", "1T", "14", "32",
	"FCC", "2.4G", "20M", "OFDM", "1T", "01", "26",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "01", "30",
	"MKK", "2.4G", "20M", "OFDM", "1T", "01", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "02", "30",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "02", "30",
	"MKK", "2.4G", "20M", "OFDM", "1T", "02", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "03", "32",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "03", "30",
	"MKK", "2.4G", "20M", "OFDM", "1T", "03", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "04", "34",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "04", "30",
	"MKK", "2.4G", "20M", "OFDM", "1T", "04", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "05", "34",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "05", "30",
	"MKK", "2.4G", "20M", "OFDM", "1T", "05", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "06", "34",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "06", "30",
	"MKK", "2.4G", "20M", "OFDM", "1T", "06", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "07", "34",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "07", "30",
	"MKK", "2.4G", "20M", "OFDM", "1T", "07", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "08", "34",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "08", "30",
	"MKK", "2.4G", "20M", "OFDM", "1T", "08", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "09", "32",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "09", "30",
	"MKK", "2.4G", "20M", "OFDM", "1T", "09", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "10", "30",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "10", "30",
	"MKK", "2.4G", "20M", "OFDM", "1T", "10", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "11", "28",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "11", "30",
	"MKK", "2.4G", "20M", "OFDM", "1T", "11", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "12", "22",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "12", "30",
	"MKK", "2.4G", "20M", "OFDM", "1T", "12", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "13", "14",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "13", "30",
	"MKK", "2.4G", "20M", "OFDM", "1T", "13", "34",
	"FCC", "2.4G", "20M", "OFDM", "1T", "14", "63",
	"ETSI", "2.4G", "20M", "OFDM", "1T", "14", "63",
	"MKK", "2.4G", "20M", "OFDM", "1T", "14", "63",
	"FCC", "2.4G", "20M", "HT", "1T", "01", "26",
	"ETSI", "2.4G", "20M", "HT", "1T", "01", "30",
	"MKK", "2.4G", "20M", "HT", "1T", "01", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "02", "30",
	"ETSI", "2.4G", "20M", "HT", "1T", "02", "30",
	"MKK", "2.4G", "20M", "HT", "1T", "02", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "03", "32",
	"ETSI", "2.4G", "20M", "HT", "1T", "03", "30",
	"MKK", "2.4G", "20M", "HT", "1T", "03", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "04", "34",
	"ETSI", "2.4G", "20M", "HT", "1T", "04", "30",
	"MKK", "2.4G", "20M", "HT", "1T", "04", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "05", "34",
	"ETSI", "2.4G", "20M", "HT", "1T", "05", "30",
	"MKK", "2.4G", "20M", "HT", "1T", "05", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "06", "34",
	"ETSI", "2.4G", "20M", "HT", "1T", "06", "30",
	"MKK", "2.4G", "20M", "HT", "1T", "06", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "07", "34",
	"ETSI", "2.4G", "20M", "HT", "1T", "07", "30",
	"MKK", "2.4G", "20M", "HT", "1T", "07", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "08", "34",
	"ETSI", "2.4G", "20M", "HT", "1T", "08", "30",
	"MKK", "2.4G", "20M", "HT", "1T", "08", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "09", "32",
	"ETSI", "2.4G", "20M", "HT", "1T", "09", "30",
	"MKK", "2.4G", "20M", "HT", "1T", "09", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "10", "30",
	"ETSI", "2.4G", "20M", "HT", "1T", "10", "30",
	"MKK", "2.4G", "20M", "HT", "1T", "10", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "11", "26",
	"ETSI", "2.4G", "20M", "HT", "1T", "11", "30",
	"MKK", "2.4G", "20M", "HT", "1T", "11", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "12", "20",
	"ETSI", "2.4G", "20M", "HT", "1T", "12", "30",
	"MKK", "2.4G", "20M", "HT", "1T", "12", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "13", "14",
	"ETSI", "2.4G", "20M", "HT", "1T", "13", "30",
	"MKK", "2.4G", "20M", "HT", "1T", "13", "34",
	"FCC", "2.4G", "20M", "HT", "1T", "14", "63",
	"ETSI", "2.4G", "20M", "HT", "1T", "14", "63",
	"MKK", "2.4G", "20M", "HT", "1T", "14", "63",
	"FCC", "2.4G", "20M", "HT", "2T", "01", "26",
	"ETSI", "2.4G", "20M", "HT", "2T", "01", "18",
	"MKK", "2.4G", "20M", "HT", "2T", "01", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "02", "28",
	"ETSI", "2.4G", "20M", "HT", "2T", "02", "18",
	"MKK", "2.4G", "20M", "HT", "2T", "02", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "03", "30",
	"ETSI", "2.4G", "20M", "HT", "2T", "03", "18",
	"MKK", "2.4G", "20M", "HT", "2T", "03", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "04", "30",
	"ETSI", "2.4G", "20M", "HT", "2T", "04", "18",
	"MKK", "2.4G", "20M", "HT", "2T", "04", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "05", "32",
	"ETSI", "2.4G", "20M", "HT", "2T", "05", "18",
	"MKK", "2.4G", "20M", "HT", "2T", "05", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "06", "32",
	"ETSI", "2.4G", "20M", "HT", "2T", "06", "18",
	"MKK", "2.4G", "20M", "HT", "2T", "06", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "07", "32",
	"ETSI", "2.4G", "20M", "HT", "2T", "07", "18",
	"MKK", "2.4G", "20M", "HT", "2T", "07", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "08", "30",
	"ETSI", "2.4G", "20M", "HT", "2T", "08", "18",
	"MKK", "2.4G", "20M", "HT", "2T", "08", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "09", "30",
	"ETSI", "2.4G", "20M", "HT", "2T", "09", "18",
	"MKK", "2.4G", "20M", "HT", "2T", "09", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "10", "28",
	"ETSI", "2.4G", "20M", "HT", "2T", "10", "18",
	"MKK", "2.4G", "20M", "HT", "2T", "10", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "11", "26",
	"ETSI", "2.4G", "20M", "HT", "2T", "11", "18",
	"MKK", "2.4G", "20M", "HT", "2T", "11", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "12", "20",
	"ETSI", "2.4G", "20M", "HT", "2T", "12", "18",
	"MKK", "2.4G", "20M", "HT", "2T", "12", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "13", "14",
	"ETSI", "2.4G", "20M", "HT", "2T", "13", "18",
	"MKK", "2.4G", "20M", "HT", "2T", "13", "30",
	"FCC", "2.4G", "20M", "HT", "2T", "14", "63",
	"ETSI", "2.4G", "20M", "HT", "2T", "14", "63",
	"MKK", "2.4G", "20M", "HT", "2T", "14", "63",
	"FCC", "2.4G", "40M", "HT", "1T", "01", "63",
	"ETSI", "2.4G", "40M", "HT", "1T", "01", "63",
	"MKK", "2.4G", "40M", "HT", "1T", "01", "63",
	"FCC", "2.4G", "40M", "HT", "1T", "02", "63",
	"ETSI", "2.4G", "40M", "HT", "1T", "02", "63",
	"MKK", "2.4G", "40M", "HT", "1T", "02", "63",
	"FCC", "2.4G", "40M", "HT", "1T", "03", "26",
	"ETSI", "2.4G", "40M", "HT", "1T", "03", "30",
	"MKK", "2.4G", "40M", "HT", "1T", "03", "34",
	"FCC", "2.4G", "40M", "HT", "1T", "04", "26",
	"ETSI", "2.4G", "40M", "HT", "1T", "04", "30",
	"MKK", "2.4G", "40M", "HT", "1T", "04", "34",
	"FCC", "2.4G", "40M", "HT", "1T", "05", "30",
	"ETSI", "2.4G", "40M", "HT", "1T", "05", "30",
	"MKK", "2.4G", "40M", "HT", "1T", "05", "34",
	"FCC", "2.4G", "40M", "HT", "1T", "06", "32",
	"ETSI", "2.4G", "40M", "HT", "1T", "06", "30",
	"MKK", "2.4G", "40M", "HT", "1T", "06", "34",
	"FCC", "2.4G", "40M", "HT", "1T", "07", "30",
	"ETSI", "2.4G", "40M", "HT", "1T", "07", "30",
	"MKK", "2.4G", "40M", "HT", "1T", "07", "34",
	"FCC", "2.4G", "40M", "HT", "1T", "08", "26",
	"ETSI", "2.4G", "40M", "HT", "1T", "08", "30",
	"MKK", "2.4G", "40M", "HT", "1T", "08", "34",
	"FCC", "2.4G", "40M", "HT", "1T", "09", "26",
	"ETSI", "2.4G", "40M", "HT", "1T", "09", "30",
	"MKK", "2.4G", "40M", "HT", "1T", "09", "34",
	"FCC", "2.4G", "40M", "HT", "1T", "10", "20",
	"ETSI", "2.4G", "40M", "HT", "1T", "10", "30",
	"MKK", "2.4G", "40M", "HT", "1T", "10", "34",
	"FCC", "2.4G", "40M", "HT", "1T", "11", "14",
	"ETSI", "2.4G", "40M", "HT", "1T", "11", "30",
	"MKK", "2.4G", "40M", "HT", "1T", "11", "34",
	"FCC", "2.4G", "40M", "HT", "1T", "12", "63",
	"ETSI", "2.4G", "40M", "HT", "1T", "12", "63",
	"MKK", "2.4G", "40M", "HT", "1T", "12", "63",
	"FCC", "2.4G", "40M", "HT", "1T", "13", "63",
	"ETSI", "2.4G", "40M", "HT", "1T", "13", "63",
	"MKK", "2.4G", "40M", "HT", "1T", "13", "63",
	"FCC", "2.4G", "40M", "HT", "1T", "14", "63",
	"ETSI", "2.4G", "40M", "HT", "1T", "14", "63",
	"MKK", "2.4G", "40M", "HT", "1T", "14", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "01", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "01", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "01", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "02", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "02", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "02", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "03", "24",
	"ETSI", "2.4G", "40M", "HT", "2T", "03", "18",
	"MKK", "2.4G", "40M", "HT", "2T", "03", "30",
	"FCC", "2.4G", "40M", "HT", "2T", "04", "24",
	"ETSI", "2.4G", "40M", "HT", "2T", "04", "18",
	"MKK", "2.4G", "40M", "HT", "2T", "04", "30",
	"FCC", "2.4G", "40M", "HT", "2T", "05", "26",
	"ETSI", "2.4G", "40M", "HT", "2T", "05", "18",
	"MKK", "2.4G", "40M", "HT", "2T", "05", "30",
	"FCC", "2.4G", "40M", "HT", "2T", "06", "28",
	"ETSI", "2.4G", "40M", "HT", "2T", "06", "18",
	"MKK", "2.4G", "40M", "HT", "2T", "06", "30",
	"FCC", "2.4G", "40M", "HT", "2T", "07", "26",
	"ETSI", "2.4G", "40M", "HT", "2T", "07", "18",
	"MKK", "2.4G", "40M", "HT", "2T", "07", "30",
	"FCC", "2.4G", "40M", "HT", "2T", "08", "26",
	"ETSI", "2.4G", "40M", "HT", "2T", "08", "18",
	"MKK", "2.4G", "40M", "HT", "2T", "08", "30",
	"FCC", "2.4G", "40M", "HT", "2T", "09", "26",
	"ETSI", "2.4G", "40M", "HT", "2T", "09", "18",
	"MKK", "2.4G", "40M", "HT", "2T", "09", "30",
	"FCC", "2.4G", "40M", "HT", "2T", "10", "20",
	"ETSI", "2.4G", "40M", "HT", "2T", "10", "18",
	"MKK", "2.4G", "40M", "HT", "2T", "10", "30",
	"FCC", "2.4G", "40M", "HT", "2T", "11", "14",
	"ETSI", "2.4G", "40M", "HT", "2T", "11", "18",
	"MKK", "2.4G", "40M", "HT", "2T", "11", "30",
	"FCC", "2.4G", "40M", "HT", "2T", "12", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "12", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "12", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "13", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "13", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "13", "63",
	"FCC", "2.4G", "40M", "HT", "2T", "14", "63",
	"ETSI", "2.4G", "40M", "HT", "2T", "14", "63",
	"MKK", "2.4G", "40M", "HT", "2T", "14", "63",
	"FCC", "5G", "20M", "OFDM", "1T", "36", "30",
	"ETSI", "5G", "20M", "OFDM", "1T", "36", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "36", "30",
	"FCC", "5G", "20M", "OFDM", "1T", "40", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "40", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "40", "30",
	"FCC", "5G", "20M", "OFDM", "1T", "44", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "44", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "44", "30",
	"FCC", "5G", "20M", "OFDM", "1T", "48", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "48", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "48", "30",
	"FCC", "5G", "20M", "OFDM", "1T", "52", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "52", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "52", "28",
	"FCC", "5G", "20M", "OFDM", "1T", "56", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "56", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "56", "28",
	"FCC", "5G", "20M", "OFDM", "1T", "60", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "60", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "60", "28",
	"FCC", "5G", "20M", "OFDM", "1T", "64", "28",
	"ETSI", "5G", "20M", "OFDM", "1T", "64", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "64", "28",
	"FCC", "5G", "20M", "OFDM", "1T", "100", "26",
	"ETSI", "5G", "20M", "OFDM", "1T", "100", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "100", "32",
	"FCC", "5G", "20M", "OFDM", "1T", "104", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "104", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "104", "32",
	"FCC", "5G", "20M", "OFDM", "1T", "108", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "108", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "108", "32",
	"FCC", "5G", "20M", "OFDM", "1T", "112", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "112", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "112", "32",
	"FCC", "5G", "20M", "OFDM", "1T", "116", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "116", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "116", "32",
	"FCC", "5G", "20M", "OFDM", "1T", "120", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "120", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "120", "32",
	"FCC", "5G", "20M", "OFDM", "1T", "124", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "124", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "124", "32",
	"FCC", "5G", "20M", "OFDM", "1T", "128", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "128", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "128", "32",
	"FCC", "5G", "20M", "OFDM", "1T", "132", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "132", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "132", "32",
	"FCC", "5G", "20M", "OFDM", "1T", "136", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "136", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "136", "32",
	"FCC", "5G", "20M", "OFDM", "1T", "140", "28",
	"ETSI", "5G", "20M", "OFDM", "1T", "140", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "140", "32",
	"FCC", "5G", "20M", "OFDM", "1T", "144", "28",
	"ETSI", "5G", "20M", "OFDM", "1T", "144", "32",
	"MKK", "5G", "20M", "OFDM", "1T", "144", "63",
	"FCC", "5G", "20M", "OFDM", "1T", "149", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "149", "63",
	"MKK", "5G", "20M", "OFDM", "1T", "149", "63",
	"FCC", "5G", "20M", "OFDM", "1T", "153", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "153", "63",
	"MKK", "5G", "20M", "OFDM", "1T", "153", "63",
	"FCC", "5G", "20M", "OFDM", "1T", "157", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "157", "63",
	"MKK", "5G", "20M", "OFDM", "1T", "157", "63",
	"FCC", "5G", "20M", "OFDM", "1T", "161", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "161", "63",
	"MKK", "5G", "20M", "OFDM", "1T", "161", "63",
	"FCC", "5G", "20M", "OFDM", "1T", "165", "32",
	"ETSI", "5G", "20M", "OFDM", "1T", "165", "63",
	"MKK", "5G", "20M", "OFDM", "1T", "165", "63",
	"FCC", "5G", "20M", "HT", "1T", "36", "30",
	"ETSI", "5G", "20M", "HT", "1T", "36", "32",
	"MKK", "5G", "20M", "HT", "1T", "36", "28",
	"FCC", "5G", "20M", "HT", "1T", "40", "32",
	"ETSI", "5G", "20M", "HT", "1T", "40", "32",
	"MKK", "5G", "20M", "HT", "1T", "40", "28",
	"FCC", "5G", "20M", "HT", "1T", "44", "32",
	"ETSI", "5G", "20M", "HT", "1T", "44", "32",
	"MKK", "5G", "20M", "HT", "1T", "44", "28",
	"FCC", "5G", "20M", "HT", "1T", "48", "32",
	"ETSI", "5G", "20M", "HT", "1T", "48", "32",
	"MKK", "5G", "20M", "HT", "1T", "48", "28",
	"FCC", "5G", "20M", "HT", "1T", "52", "32",
	"ETSI", "5G", "20M", "HT", "1T", "52", "32",
	"MKK", "5G", "20M", "HT", "1T", "52", "28",
	"FCC", "5G", "20M", "HT", "1T", "56", "32",
	"ETSI", "5G", "20M", "HT", "1T", "56", "32",
	"MKK", "5G", "20M", "HT", "1T", "56", "28",
	"FCC", "5G", "20M", "HT", "1T", "60", "32",
	"ETSI", "5G", "20M", "HT", "1T", "60", "32",
	"MKK", "5G", "20M", "HT", "1T", "60", "28",
	"FCC", "5G", "20M", "HT", "1T", "64", "28",
	"ETSI", "5G", "20M", "HT", "1T", "64", "32",
	"MKK", "5G", "20M", "HT", "1T", "64", "28",
	"FCC", "5G", "20M", "HT", "1T", "100", "26",
	"ETSI", "5G", "20M", "HT", "1T", "100", "32",
	"MKK", "5G", "20M", "HT", "1T", "100", "32",
	"FCC", "5G", "20M", "HT", "1T", "104", "32",
	"ETSI", "5G", "20M", "HT", "1T", "104", "32",
	"MKK", "5G", "20M", "HT", "1T", "104", "32",
	"FCC", "5G", "20M", "HT", "1T", "108", "32",
	"ETSI", "5G", "20M", "HT", "1T", "108", "32",
	"MKK", "5G", "20M", "HT", "1T", "108", "32",
	"FCC", "5G", "20M", "HT", "1T", "112", "32",
	"ETSI", "5G", "20M", "HT", "1T", "112", "32",
	"MKK", "5G", "20M", "HT", "1T", "112", "32",
	"FCC", "5G", "20M", "HT", "1T", "116", "32",
	"ETSI", "5G", "20M", "HT", "1T", "116", "32",
	"MKK", "5G", "20M", "HT", "1T", "116", "32",
	"FCC", "5G", "20M", "HT", "1T", "120", "32",
	"ETSI", "5G", "20M", "HT", "1T", "120", "32",
	"MKK", "5G", "20M", "HT", "1T", "120", "32",
	"FCC", "5G", "20M", "HT", "1T", "124", "32",
	"ETSI", "5G", "20M", "HT", "1T", "124", "32",
	"MKK", "5G", "20M", "HT", "1T", "124", "32",
	"FCC", "5G", "20M", "HT", "1T", "128", "32",
	"ETSI", "5G", "20M", "HT", "1T", "128", "32",
	"MKK", "5G", "20M", "HT", "1T", "128", "32",
	"FCC", "5G", "20M", "HT", "1T", "132", "32",
	"ETSI", "5G", "20M", "HT", "1T", "132", "32",
	"MKK", "5G", "20M", "HT", "1T", "132", "32",
	"FCC", "5G", "20M", "HT", "1T", "136", "32",
	"ETSI", "5G", "20M", "HT", "1T", "136", "32",
	"MKK", "5G", "20M", "HT", "1T", "136", "32",
	"FCC", "5G", "20M", "HT", "1T", "140", "26",
	"ETSI", "5G", "20M", "HT", "1T", "140", "32",
	"MKK", "5G", "20M", "HT", "1T", "140", "32",
	"FCC", "5G", "20M", "HT", "1T", "144", "26",
	"ETSI", "5G", "20M", "HT", "1T", "144", "63",
	"MKK", "5G", "20M", "HT", "1T", "144", "63",
	"FCC", "5G", "20M", "HT", "1T", "149", "32",
	"ETSI", "5G", "20M", "HT", "1T", "149", "63",
	"MKK", "5G", "20M", "HT", "1T", "149", "63",
	"FCC", "5G", "20M", "HT", "1T", "153", "32",
	"ETSI", "5G", "20M", "HT", "1T", "153", "63",
	"MKK", "5G", "20M", "HT", "1T", "153", "63",
	"FCC", "5G", "20M", "HT", "1T", "157", "32",
	"ETSI", "5G", "20M", "HT", "1T", "157", "63",
	"MKK", "5G", "20M", "HT", "1T", "157", "63",
	"FCC", "5G", "20M", "HT", "1T", "161", "32",
	"ETSI", "5G", "20M", "HT", "1T", "161", "63",
	"MKK", "5G", "20M", "HT", "1T", "161", "63",
	"FCC", "5G", "20M", "HT", "1T", "165", "32",
	"ETSI", "5G", "20M", "HT", "1T", "165", "63",
	"MKK", "5G", "20M", "HT", "1T", "165", "63",
	"FCC", "5G", "20M", "HT", "2T", "36", "28",
	"ETSI", "5G", "20M", "HT", "2T", "36", "20",
	"MKK", "5G", "20M", "HT", "2T", "36", "22",
	"FCC", "5G", "20M", "HT", "2T", "40", "30",
	"ETSI", "5G", "20M", "HT", "2T", "40", "20",
	"MKK", "5G", "20M", "HT", "2T", "40", "22",
	"FCC", "5G", "20M", "HT", "2T", "44", "30",
	"ETSI", "5G", "20M", "HT", "2T", "44", "20",
	"MKK", "5G", "20M", "HT", "2T", "44", "22",
	"FCC", "5G", "20M", "HT", "2T", "48", "30",
	"ETSI", "5G", "20M", "HT", "2T", "48", "20",
	"MKK", "5G", "20M", "HT", "2T", "48", "22",
	"FCC", "5G", "20M", "HT", "2T", "52", "30",
	"ETSI", "5G", "20M", "HT", "2T", "52", "20",
	"MKK", "5G", "20M", "HT", "2T", "52", "22",
	"FCC", "5G", "20M", "HT", "2T", "56", "30",
	"ETSI", "5G", "20M", "HT", "2T", "56", "20",
	"MKK", "5G", "20M", "HT", "2T", "56", "22",
	"FCC", "5G", "20M", "HT", "2T", "60", "30",
	"ETSI", "5G", "20M", "HT", "2T", "60", "20",
	"MKK", "5G", "20M", "HT", "2T", "60", "22",
	"FCC", "5G", "20M", "HT", "2T", "64", "28",
	"ETSI", "5G", "20M", "HT", "2T", "64", "20",
	"MKK", "5G", "20M", "HT", "2T", "64", "22",
	"FCC", "5G", "20M", "HT", "2T", "100", "26",
	"ETSI", "5G", "20M", "HT", "2T", "100", "20",
	"MKK", "5G", "20M", "HT", "2T", "100", "30",
	"FCC", "5G", "20M", "HT", "2T", "104", "30",
	"ETSI", "5G", "20M", "HT", "2T", "104", "20",
	"MKK", "5G", "20M", "HT", "2T", "104", "30",
	"FCC", "5G", "20M", "HT", "2T", "108", "32",
	"ETSI", "5G", "20M", "HT", "2T", "108", "20",
	"MKK", "5G", "20M", "HT", "2T", "108", "30",
	"FCC", "5G", "20M", "HT", "2T", "112", "32",
	"ETSI", "5G", "20M", "HT", "2T", "112", "20",
	"MKK", "5G", "20M", "HT", "2T", "112", "30",
	"FCC", "5G", "20M", "HT", "2T", "116", "32",
	"ETSI", "5G", "20M", "HT", "2T", "116", "20",
	"MKK", "5G", "20M", "HT", "2T", "116", "30",
	"FCC", "5G", "20M", "HT", "2T", "120", "32",
	"ETSI", "5G", "20M", "HT", "2T", "120", "20",
	"MKK", "5G", "20M", "HT", "2T", "120", "30",
	"FCC", "5G", "20M", "HT", "2T", "124", "32",
	"ETSI", "5G", "20M", "HT", "2T", "124", "20",
	"MKK", "5G", "20M", "HT", "2T", "124", "30",
	"FCC", "5G", "20M", "HT", "2T", "128", "32",
	"ETSI", "5G", "20M", "HT", "2T", "128", "20",
	"MKK", "5G", "20M", "HT", "2T", "128", "30",
	"FCC", "5G", "20M", "HT", "2T", "132", "32",
	"ETSI", "5G", "20M", "HT", "2T", "132", "20",
	"MKK", "5G", "20M", "HT", "2T", "132", "30",
	"FCC", "5G", "20M", "HT", "2T", "136", "30",
	"ETSI", "5G", "20M", "HT", "2T", "136", "20",
	"MKK", "5G", "20M", "HT", "2T", "136", "30",
	"FCC", "5G", "20M", "HT", "2T", "140", "26",
	"ETSI", "5G", "20M", "HT", "2T", "140", "20",
	"MKK", "5G", "20M", "HT", "2T", "140", "30",
	"FCC", "5G", "20M", "HT", "2T", "144", "26",
	"ETSI", "5G", "20M", "HT", "2T", "144", "63",
	"MKK", "5G", "20M", "HT", "2T", "144", "63",
	"FCC", "5G", "20M", "HT", "2T", "149", "32",
	"ETSI", "5G", "20M", "HT", "2T", "149", "63",
	"MKK", "5G", "20M", "HT", "2T", "149", "63",
	"FCC", "5G", "20M", "HT", "2T", "153", "32",
	"ETSI", "5G", "20M", "HT", "2T", "153", "63",
	"MKK", "5G", "20M", "HT", "2T", "153", "63",
	"FCC", "5G", "20M", "HT", "2T", "157", "32",
	"ETSI", "5G", "20M", "HT", "2T", "157", "63",
	"MKK", "5G", "20M", "HT", "2T", "157", "63",
	"FCC", "5G", "20M", "HT", "2T", "161", "32",
	"ETSI", "5G", "20M", "HT", "2T", "161", "63",
	"MKK", "5G", "20M", "HT", "2T", "161", "63",
	"FCC", "5G", "20M", "HT", "2T", "165", "32",
	"ETSI", "5G", "20M", "HT", "2T", "165", "63",
	"MKK", "5G", "20M", "HT", "2T", "165", "63",
	"FCC", "5G", "40M", "HT", "1T", "38", "22",
	"ETSI", "5G", "40M", "HT", "1T", "38", "30",
	"MKK", "5G", "40M", "HT", "1T", "38", "30",
	"FCC", "5G", "40M", "HT", "1T", "46", "30",
	"ETSI", "5G", "40M", "HT", "1T", "46", "30",
	"MKK", "5G", "40M", "HT", "1T", "46", "30",
	"FCC", "5G", "40M", "HT", "1T", "54", "30",
	"ETSI", "5G", "40M", "HT", "1T", "54", "30",
	"MKK", "5G", "40M", "HT", "1T", "54", "30",
	"FCC", "5G", "40M", "HT", "1T", "62", "24",
	"ETSI", "5G", "40M", "HT", "1T", "62", "30",
	"MKK", "5G", "40M", "HT", "1T", "62", "30",
	"FCC", "5G", "40M", "HT", "1T", "102", "24",
	"ETSI", "5G", "40M", "HT", "1T", "102", "30",
	"MKK", "5G", "40M", "HT", "1T", "102", "30",
	"FCC", "5G", "40M", "HT", "1T", "110", "30",
	"ETSI", "5G", "40M", "HT", "1T", "110", "30",
	"MKK", "5G", "40M", "HT", "1T", "110", "30",
	"FCC", "5G", "40M", "HT", "1T", "118", "30",
	"ETSI", "5G", "40M", "HT", "1T", "118", "30",
	"MKK", "5G", "40M", "HT", "1T", "118", "30",
	"FCC", "5G", "40M", "HT", "1T", "126", "30",
	"ETSI", "5G", "40M", "HT", "1T", "126", "30",
	"MKK", "5G", "40M", "HT", "1T", "126", "30",
	"FCC", "5G", "40M", "HT", "1T", "134", "30",
	"ETSI", "5G", "40M", "HT", "1T", "134", "30",
	"MKK", "5G", "40M", "HT", "1T", "134", "30",
	"FCC", "5G", "40M", "HT", "1T", "142", "30",
	"ETSI", "5G", "40M", "HT", "1T", "142", "63",
	"MKK", "5G", "40M", "HT", "1T", "142", "63",
	"FCC", "5G", "40M", "HT", "1T", "151", "30",
	"ETSI", "5G", "40M", "HT", "1T", "151", "63",
	"MKK", "5G", "40M", "HT", "1T", "151", "63",
	"FCC", "5G", "40M", "HT", "1T", "159", "30",
	"ETSI", "5G", "40M", "HT", "1T", "159", "63",
	"MKK", "5G", "40M", "HT", "1T", "159", "63",
	"FCC", "5G", "40M", "HT", "2T", "38", "20",
	"ETSI", "5G", "40M", "HT", "2T", "38", "20",
	"MKK", "5G", "40M", "HT", "2T", "38", "22",
	"FCC", "5G", "40M", "HT", "2T", "46", "30",
	"ETSI", "5G", "40M", "HT", "2T", "46", "20",
	"MKK", "5G", "40M", "HT", "2T", "46", "22",
	"FCC", "5G", "40M", "HT", "2T", "54", "30",
	"ETSI", "5G", "40M", "HT", "2T", "54", "20",
	"MKK", "5G", "40M", "HT", "2T", "54", "22",
	"FCC", "5G", "40M", "HT", "2T", "62", "22",
	"ETSI", "5G", "40M", "HT", "2T", "62", "20",
	"MKK", "5G", "40M", "HT", "2T", "62", "22",
	"FCC", "5G", "40M", "HT", "2T", "102", "22",
	"ETSI", "5G", "40M", "HT", "2T", "102", "20",
	"MKK", "5G", "40M", "HT", "2T", "102", "30",
	"FCC", "5G", "40M", "HT", "2T", "110", "30",
	"ETSI", "5G", "40M", "HT", "2T", "110", "20",
	"MKK", "5G", "40M", "HT", "2T", "110", "30",
	"FCC", "5G", "40M", "HT", "2T", "118", "30",
	"ETSI", "5G", "40M", "HT", "2T", "118", "20",
	"MKK", "5G", "40M", "HT", "2T", "118", "30",
	"FCC", "5G", "40M", "HT", "2T", "126", "30",
	"ETSI", "5G", "40M", "HT", "2T", "126", "20",
	"MKK", "5G", "40M", "HT", "2T", "126", "30",
	"FCC", "5G", "40M", "HT", "2T", "134", "30",
	"ETSI", "5G", "40M", "HT", "2T", "134", "20",
	"MKK", "5G", "40M", "HT", "2T", "134", "30",
	"FCC", "5G", "40M", "HT", "2T", "142", "30",
	"ETSI", "5G", "40M", "HT", "2T", "142", "63",
	"MKK", "5G", "40M", "HT", "2T", "142", "63",
	"FCC", "5G", "40M", "HT", "2T", "151", "30",
	"ETSI", "5G", "40M", "HT", "2T", "151", "63",
	"MKK", "5G", "40M", "HT", "2T", "151", "63",
	"FCC", "5G", "40M", "HT", "2T", "159", "30",
	"ETSI", "5G", "40M", "HT", "2T", "159", "63",
	"MKK", "5G", "40M", "HT", "2T", "159", "63",
	"FCC", "5G", "80M", "VHT", "1T", "42", "20",
	"ETSI", "5G", "80M", "VHT", "1T", "42", "30",
	"MKK", "5G", "80M", "VHT", "1T", "42", "28",
	"FCC", "5G", "80M", "VHT", "1T", "58", "20",
	"ETSI", "5G", "80M", "VHT", "1T", "58", "30",
	"MKK", "5G", "80M", "VHT", "1T", "58", "28",
	"FCC", "5G", "80M", "VHT", "1T", "106", "20",
	"ETSI", "5G", "80M", "VHT", "1T", "106", "30",
	"MKK", "5G", "80M", "VHT", "1T", "106", "30",
	"FCC", "5G", "80M", "VHT", "1T", "122", "30",
	"ETSI", "5G", "80M", "VHT", "1T", "122", "30",
	"MKK", "5G", "80M", "VHT", "1T", "122", "30",
	"FCC", "5G", "80M", "VHT", "1T", "138", "30",
	"ETSI", "5G", "80M", "VHT", "1T", "138", "63",
	"MKK", "5G", "80M", "VHT", "1T", "138", "63",
	"FCC", "5G", "80M", "VHT", "1T", "155", "30",
	"ETSI", "5G", "80M", "VHT", "1T", "155", "63",
	"MKK", "5G", "80M", "VHT", "1T", "155", "63",
	"FCC", "5G", "80M", "VHT", "2T", "42", "18",
	"ETSI", "5G", "80M", "VHT", "2T", "42", "20",
	"MKK", "5G", "80M", "VHT", "2T", "42", "22",
	"FCC", "5G", "80M", "VHT", "2T", "58", "18",
	"ETSI", "5G", "80M", "VHT", "2T", "58", "20",
	"MKK", "5G", "80M", "VHT", "2T", "58", "22",
	"FCC", "5G", "80M", "VHT", "2T", "106", "20",
	"ETSI", "5G", "80M", "VHT", "2T", "106", "20",
	"MKK", "5G", "80M", "VHT", "2T", "106", "30",
	"FCC", "5G", "80M", "VHT", "2T", "122", "30",
	"ETSI", "5G", "80M", "VHT", "2T", "122", "20",
	"MKK", "5G", "80M", "VHT", "2T", "122", "30",
	"FCC", "5G", "80M", "VHT", "2T", "138", "30",
	"ETSI", "5G", "80M", "VHT", "2T", "138", "63",
	"MKK", "5G", "80M", "VHT", "2T", "138", "63",
	"FCC", "5G", "80M", "VHT", "2T", "155", "30",
	"ETSI", "5G", "80M", "VHT", "2T", "155", "63",
	"MKK", "5G", "80M", "VHT", "2T", "155", "63"
};
#define TABLE_TXPWR_LIMIT_NUM		\
	(sizeof(table_txpwr_lmt)/sizeof(table_txpwr_lmt[0]))

struct rtl_channel_info {
	u8 channel;
	bool is_5g;	/* 5g channel? */
	enum band_type band;
	u8 ch_idx;	/* 2G: 0~13 */
			/* 5G: 0~49 (include 80M channel) */
			/* 5G 80M: within 49 */
	u8 ch_idx2;	/* 5G 80M: 0~6 */
	u64 rate_mask;	/* bitmap of possible rate */
	u32 bw_mask;	/* bitmap of possible bandwidth */
	u8 efuse_cck_offset;	/* 2G only */
	u8 efuse_bw40_offset;	/* 2G or 5G */
};

struct rtl_rate_info {
	enum rtl_desc_rate rate;
	const char *rate_str;
	enum rate_section rate_section;
	bool is_cck;
	bool is_ofdm;
	u8 nss_1;	/* nss - 1 */
};

static u8 get_pwr_limit_from_table(struct rtl_priv *rtlpriv,
				   enum radio_path path,
				   const struct rtl_rate_info *rate_info,
				   enum ht_channel_width bandwidth,
				   const struct rtl_channel_info *ch_info)
{
	int i;
	struct tab_pwr_limit {
		const char *p[7];
	} const *p;
	u8 ret = 63;
	unsigned long tmpl;
	int junk;

	/* In 5G and bandwidth is 20M or 40M, VHT and HT can cross reference */
	bool cross_ref = (ch_info->is_5g &&
			  (bandwidth == HT_CHANNEL_WIDTH_20 ||
			   bandwidth == HT_CHANNEL_WIDTH_20_40));

	for (i = 0, p = (struct tab_pwr_limit *)table_txpwr_lmt;
	     i < TABLE_TXPWR_LIMIT_NUM; i += 7, p++) {
		/* band */
		if (ch_info->band == BAND_ON_5G && strcmp(p->p[1], "5G"))
			continue;
		if (ch_info->band == BAND_ON_2_4G && strcmp(p->p[1], "2.4G"))
			continue;

		/* bandwidth */
		if (rate_info->rate_section == CCK ||
		    rate_info->rate_section == OFDM) {
			bandwidth = HT_CHANNEL_WIDTH_20;
		}

		if (bandwidth == HT_CHANNEL_WIDTH_20 && strcmp(p->p[2], "20M"))
			continue;
		if (bandwidth == HT_CHANNEL_WIDTH_20_40 &&
		    strcmp(p->p[2], "40M"))
			continue;
		if (bandwidth == HT_CHANNEL_WIDTH_80 && strcmp(p->p[2], "80M"))
			continue;

		/* rate section */
		if (rate_info->rate_section == CCK && strcmp(p->p[3], "CCK"))
			continue;
		if (rate_info->rate_section == OFDM && strcmp(p->p[3], "OFDM"))
			continue;
		if ((rate_info->rate_section == HT_MCS0_MCS7 ||
		     rate_info->rate_section == HT_MCS8_MCS15)) {
			if (strcmp(p->p[3], "HT") == 0)
				;
			else if (cross_ref && strcmp(p->p[3], "VHT") == 0)
				;
			else
				continue;
		}
		if ((rate_info->rate_section == VHT_1SSMCS0_1SSMCS9 ||
		     rate_info->rate_section == VHT_2SSMCS0_2SSMCS9)) {
			if (strcmp(p->p[3], "VHT") == 0)
				;
			else if (cross_ref && strcmp(p->p[3], "HT") == 0)
				;
			else
				continue;
		}

		/* NSS */
		if (sscanf(p->p[4], "%ldT", &tmpl) != 1)
			tmpl = 99;

		if (rate_info->nss_1 + 1 != tmpl)
			continue;

		/* channel */
		junk = kstrtoul(p->p[5], 10, &tmpl);

		if (ch_info->channel != tmpl)
			continue;

		/* value */
		junk = kstrtoul(p->p[6], 10, &tmpl);

		if (ret > tmpl)
			ret = tmpl;
	}

	return ret;
}

static void get_rate_value(struct rtl_priv *rtlpriv,
	u32 reg_addr, u32 bit_mask, u32 value,
	u8 out_rate[], s8 out_pwr_val[], u8 *out_num)
{
	/* verify: replace original parsing function, and compare the result */

#define DESC_RATENA	-1
#define _M_REG_2_RATE_CORE(a0, a1, a2, a3, r0, r1, r2, r3, ign_mask)	\
	{{a0, a1, a2, a3,}, ign_mask,					\
	 {DESC_RATE ##r0, DESC_RATE ##r1, DESC_RATE ##r2, DESC_RATE ##r3,},}
#define _M_REG_2_RATE(a0, a1, a2, a3, r0, r1, r2, r3)		\
	_M_REG_2_RATE_CORE(a0, a1, a2, a3, r0, r1, r2, r3, true)
#define _M_REG_2_RATE_CHKMASK(a0, r0, r1, r2, r3)	\
	_M_REG_2_RATE_CORE(a0, 0, 0, 0, r0, r1, r2, r3, false)

	struct rtl_reg2rate {
		u32 reg_addr[4];
		bool ign_mask;
		enum rtl_desc_rate rate[4];
	} reg2rates[] = {
		_M_REG_2_RATE(0xe00, 0x830, 0, 0, 6M, 9M, 12M, 18M),
		_M_REG_2_RATE(0xe04, 0x834, 0, 0, 24M, 36M, 48M, 54M),
		_M_REG_2_RATE(0xe08, 0, 0, 0, NA, 1M, NA, NA),
		_M_REG_2_RATE_CHKMASK(0x86c, 11M, 2M, 5_5M, 11M),
		_M_REG_2_RATE(0xe10, 0x83c, 0, 0, MCS0, MCS1, MCS2, MCS3),
		_M_REG_2_RATE(0xe14, 0x848, 0, 0, MCS4, MCS5, MCS6, MCS7),
		_M_REG_2_RATE(0xe18, 0x84c, 0, 0, MCS8, MCS9, MCS10, MCS11),
		_M_REG_2_RATE(0xe1c, 0x868, 0, 0, MCS12, MCS13, MCS14, MCS15),
		_M_REG_2_RATE(0x838, 0, 0, 0, NA, 1M, 2M, 5_5M),
		_M_REG_2_RATE(0xC20, 0xE20, 0x1820, 0x1a20, 1M, 2M, 5_5M, 11M),
		_M_REG_2_RATE(0xC24, 0xE24, 0x1824, 0x1a24, 6M, 9M, 12M, 18M),
		_M_REG_2_RATE(0xC28, 0xE28, 0x1828, 0x1a28, 24M, 36M, 48M, 54M),
		_M_REG_2_RATE(0xC2C, 0xE2C, 0x182C, 0x1a2C,
			      MCS0, MCS1, MCS2, MCS3),
		_M_REG_2_RATE(0xC30, 0xE30, 0x1830, 0x1a30,
			      MCS4, MCS5, MCS6, MCS7),
		_M_REG_2_RATE(0xC34, 0xE34, 0x1834, 0x1a34,
			      MCS8, MCS9, MCS10, MCS11),
		_M_REG_2_RATE(0xC38, 0xE38, 0x1838, 0x1a38,
			      MCS12, MCS13, MCS14, MCS15),
		_M_REG_2_RATE(0xC3C, 0xE3C, 0x183C, 0x1a3C, VHT1SS_MCS0,
			      VHT1SS_MCS1, VHT1SS_MCS2, VHT1SS_MCS3),
		_M_REG_2_RATE(0xC40, 0xE40, 0x1840, 0x1a40, VHT1SS_MCS4,
			      VHT1SS_MCS5, VHT1SS_MCS6, VHT1SS_MCS7),
		_M_REG_2_RATE(0xC44, 0xE44, 0x1844, 0x1a44, VHT1SS_MCS8,
			      VHT1SS_MCS9, VHT2SS_MCS0, VHT2SS_MCS1),
		_M_REG_2_RATE(0xC48, 0xE48, 0x1848, 0x1a48, VHT2SS_MCS2,
			      VHT2SS_MCS3, VHT2SS_MCS4, VHT2SS_MCS5),
		_M_REG_2_RATE(0xC4C, 0xE4C, 0x184C, 0x1a4C, VHT2SS_MCS6,
			      VHT2SS_MCS7, VHT2SS_MCS8, VHT2SS_MCS9),
#if 0
		_M_REG_2_RATE(0xCD8, 0xED8, 0x18D8, 0x1aD8,
			      MCS16, MCS17, MCS18, MCS19),
		_M_REG_2_RATE(0xCDC, 0xEDC, 0x18DC, 0x1aDC,
			      MCS20, MCS21, MCS22, MCS23),
		_M_REG_2_RATE(0xCE0, 0xEE0, 0x18E0, 0x1aE0, VHT3SS_MCS0,
			      VHT3SS_MCS1, VHT3SS_MCS2, VHT3SS_MCS3),
		_M_REG_2_RATE(0xCE4, 0xEE4, 0x18E4, 0x1aE4, VHT3SS_MCS4,
			      VHT3SS_MCS5, VHT3SS_MCS6, VHT3SS_MCS7),
		_M_REG_2_RATE(0xCE8, 0xEE8, 0x18E8, 0x1aE8, VHT3SS_MCS8,
			      VHT3SS_MCS9),
#endif
	};

#define REG2RATES_NUM		(sizeof(reg2rates)/sizeof(reg2rates[0]))

	int i, j;
	u32 mask;
	u8 tmp;
	struct rtl_reg2rate *reg2rate;

	for (i = 0; i < REG2RATES_NUM; i++) {
		reg2rate = &reg2rates[i];
		for (j = 0; j < 4; j++) {
			if (reg2rate->reg_addr[j] == 0)
				break;

			if (reg2rate->reg_addr[j] == reg_addr)
				goto label_found;
		}
	}

	/* not found */
	*out_num = 0;

	RT_TRACE(rtlpriv, COMP_INIT, DBG_DMESG,
		 "Invalid reg_addr 0x%x in %s()\n", reg_addr, __func__);
	return;

	/* found */
label_found:
	*out_num = 0;

	for (i = 0, mask = 0xFF; i < 4; i++, mask <<= 8) {
		if (!(bit_mask & mask))
			continue;

		if (reg2rate->rate[i] == DESC_RATENA)
			continue;

		/* out rate */
		out_rate[*out_num] = reg2rate->rate[i];

		/* out pwr value ('value' is BCD code) */
		tmp = (value & mask) >> (i * 8);
		out_pwr_val[*out_num] = (tmp >> 4) * 10 + (tmp & 0x0F);

		/* increase out num */
		*out_num += 1;
	}

}
EXPORT_SYMBOL(get_rate_value);

static void get_diff_pwr_from_efuse(struct rtl_priv *rtlpriv,
				    enum radio_path path,
				    const struct rtl_rate_info *rate_info,
				    enum ht_channel_width bandwidth,
				    const struct rtl_channel_info *ch_info,
				    int nss_1,
				    s8 *diff_pwr,
				    s8 *diff_pwr2)
{
	/* DONT use rate_info->nss_1 because this is recursive call! */
	struct rtl_efuse *rtlefuse = rtl_efuse(rtlpriv);

	int offset = 0;
	int nibble = 0;
	int offset2 = 0;
	int nibble2 = 0;
	s8 tmp;

	if (!ch_info->is_5g) {
		if (nss_1 == 0) {
			if (rate_info->is_cck) {
				/* no diff */
			} else if (rate_info->is_ofdm) {
				offset2 = 0x1b;
				nibble2 = 0;
			}

			if (bandwidth == HT_CHANNEL_WIDTH_20) {
				offset = 0x1b;
				nibble = 1;
			} else if (bandwidth == HT_CHANNEL_WIDTH_20_40) {
				/* no diff */
			}
		} else {
			if (rate_info->is_cck) {
				offset2 = 0x1d;
				nibble2 = 0;
			} else if (rate_info->is_ofdm) {
				offset2 = 0x1d;
				nibble2 = 1;
			}

			if (bandwidth == HT_CHANNEL_WIDTH_20) {
				offset = 0x1c;
				nibble = 0;
			} else if (bandwidth == HT_CHANNEL_WIDTH_20_40) {
				offset = 0x1c;
				nibble = 1;
			}

			offset += (nss_1 - 1) * 2;
			offset2 += (nss_1 - 1) * 2;
		}
	} else {
		if (rate_info->is_ofdm) {
			switch (nss_1) {
			case 0:
				offset2 = 0x30;
				nibble2 = 0;
				break;
			case 1:
				offset2 = 0x34;
				nibble2 = 1;
				break;
			case 2:
				offset2 = 0x34;
				nibble2 = 0;
				break;
			case 3:
				offset2 = 0x35;
				nibble2 = 0;
				break;
			}
		}

		if (bandwidth == HT_CHANNEL_WIDTH_80) {
			offset = 0x36 + nss_1 * 1;
			nibble = 1;
			offset2 = 0;	/* 80M no OFDM diff */
			nibble2 = 0;
		} else if (bandwidth == HT_CHANNEL_WIDTH_20_40) {
			if (nss_1 > 0) {
				offset = 0x31 + (nss_1 - 1) * 1;
				nibble = 1;
			}
		} else if (bandwidth == HT_CHANNEL_WIDTH_20) {
			if (nss_1 == 0) {
				offset = 0x30;
				nibble = 1;
			} else {
				offset = 0x31 + (nss_1 - 1) * 1;
				nibble = 0;
			}
		}
	}

	if (offset)
		offset += path * (0x3A - 0x10);	/* 0x2A bytes per path */
	if (offset2)
		offset2 += path * (0x3A - 0x10);/* 0x2A bytes per path */

	/* fill diff_pwr */
	if (offset) {
		tmp = rtlefuse->efuse_map[0][offset];
		if (nibble)
			tmp = tmp >> 4;
		else
			tmp = (tmp << 4) >> 4;
		*diff_pwr += tmp;
	}

	/* fill diff_pwr2 */
	if (offset2) {
		tmp = rtlefuse->efuse_map[0][offset2];
		if (nibble2)
			tmp = tmp >> 4;
		else
			tmp = (tmp << 4) >> 4;
		*diff_pwr2 += tmp;
	}

	/* recursive? */
	if (nss_1 == 0)
		return;

	get_diff_pwr_from_efuse(rtlpriv, path, rate_info, bandwidth, ch_info,
				nss_1 - 1,
				diff_pwr, diff_pwr2);
}

static void get_base_pwr_from_efuse(struct rtl_priv *rtlpriv,
				    enum radio_path path,
				    const struct rtl_rate_info *rate_info,
				    enum ht_channel_width bandwidth,
				    const struct rtl_channel_info *ch_info,
				    u8 *base_pwr,
				    s8 *diff_pwr,
				    s8 *diff_pwr2)
{
	struct rtl_efuse *rtlefuse = rtl_efuse(rtlpriv);

	int offset;

	/* base_pwr */
	if (rate_info->is_cck)
		offset = ch_info->efuse_cck_offset;
	else
		offset = ch_info->efuse_bw40_offset;

	offset += path * (0x3A - 0x10);	/* 0x2A bytes per path */

	*base_pwr = rtlefuse->efuse_map[0][offset];

	if (ch_info->is_5g && bandwidth == HT_CHANNEL_WIDTH_80) {
		*base_pwr += rtlefuse->efuse_map[0][offset + 1];
		*base_pwr >>= 1;	/* avg. */
	}

	/* diff_pwr */
	*diff_pwr = 0;
	*diff_pwr2 = 0;

	get_diff_pwr_from_efuse(rtlpriv, path, rate_info, bandwidth, ch_info,
				rate_info->nss_1,
				diff_pwr, diff_pwr2);
}

static void dump_tx_power_index_core(struct rtl_priv *rtlpriv,
				     struct seq_file *m,
				     enum radio_path path,
				     const struct rtl_rate_info *rate_info,
				     enum ht_channel_width bandwidth,
				     const struct rtl_channel_info *ch_info)
{
#define HACK_REGULATORY		1	/* 0: by efuse */
					/* 1: pwr by rate + pwr limit*/
					/* 2: base pwr */
					/* 3: pwr by rate */

	struct rtl_hal_ops *halops = rtlpriv->cfg->ops;
	struct rtl_efuse *rtlefuse = rtl_efuse(rtlpriv);
	struct rtl_phy *rtlphy = &rtlpriv->phy;
	u8 tx_pwr = 0xEA;
	u8 base_pwr;
	s8 diff_pwr, diff_pwr2 = 0;

	u8 channel = ch_info->channel;
	u8 ch_idx = ch_info->ch_idx;
	u8 ch_idx2 = ch_info->ch_idx2;
	bool is_5g = ch_info->is_5g;
	u8 band = ch_info->band;

	enum rtl_desc_rate rate = rate_info->rate;
	const char *rate_str = rate_info->rate_str;
	enum rate_section rate_section = rate_info->rate_section;
	int nss_idx = rate_info->nss_1;
	bool is_cck = rate_info->is_cck;
	bool is_ofdm = rate_info->is_ofdm;

	bool is_80M = (bandwidth == HT_CHANNEL_WIDTH_80);
	bool is_40M = (bandwidth == HT_CHANNEL_WIDTH_20_40);

	u8 bak_eeprom_regulatory = rtlefuse->eeprom_regulatory;

	s8 pwr_by_rate_offset;
	u8 pwr_by_rate_base;
	s8 pwr_limit, pwr_limit_tmp;
	u8 bw_limit;
	int regulation = TXPWR_LMT_WW;
	int i;

	u8 efuse_base_pwr;
	s8 efuse_diff_pwr, efuse_diff_pwr2;

	u8 tab_pwr_limit;

	u8 reg_tx_pwr = 0xFF;

	/* tx_pwr */
	if (halops->get_txpower_index) {
		if (HACK_REGULATORY)
			rtlefuse->eeprom_regulatory = HACK_REGULATORY;

		tx_pwr = halops->get_txpower_index(rtlpriv->hw, path, rate,
						   bandwidth, channel);

		if (HACK_REGULATORY)
			rtlefuse->eeprom_regulatory = bak_eeprom_regulatory;
	}

	if (rtlpriv->phydm.ops &&
	    rtlphy->current_channel == ch_info->channel &&
	    rtlphy->current_chan_bw == bandwidth)
		reg_tx_pwr = rtlpriv->phydm.ops->phydm_read_txagc(rtlpriv, path,
							rate_info->rate);

	/* base_pwr */
	if (is_5g) {
		if (is_80M)
			base_pwr = rtlefuse->txpwr_5g_bw80base[path][ch_idx2];
		else
			base_pwr = rtlefuse->txpwr_5g_bw40base[path][ch_idx];
	} else {
		if (is_cck)
			base_pwr = rtlefuse->txpwrlevel_cck[path][ch_idx];
		else
			base_pwr = rtlefuse->txpwrlevel_ht40_1s[path][ch_idx];
	}

	get_base_pwr_from_efuse(rtlpriv, path, rate_info, bandwidth, ch_info,
				&efuse_base_pwr,
				&efuse_diff_pwr, &efuse_diff_pwr2);

	/* diff_pwr */
	if (is_5g) {
		if (is_ofdm && !is_80M) {
			for (i = 0, diff_pwr2 = 0; i <= nss_idx; i++)
			    diff_pwr2 += rtlefuse->txpwr_5g_ofdmdiff[path][i];
		}

		if (is_80M) {
			for (i = 0, diff_pwr = 0; i <= nss_idx; i++)
			    diff_pwr += rtlefuse->txpwr_5g_bw80diff[path][i];
		} else if (is_40M) {
			for (i = 0, diff_pwr = 0; i <= nss_idx; i++)
			    diff_pwr += rtlefuse->txpwr_5g_bw40diff[path][i];
		} else {
			for (i = 0, diff_pwr = 0; i <= nss_idx; i++)
			    diff_pwr += rtlefuse->txpwr_5g_bw20diff[path][i];
		}
	} else {
		if (is_cck)
			diff_pwr2 = 0;/*rtlefuse->txpwr_cckdiff;*/
		else if (is_ofdm) {
			for (i = 0, diff_pwr2 = 0; i <= nss_idx; i++)
			    diff_pwr2 += rtlefuse->txpwr_legacyhtdiff[path][i];
		}

		if (is_40M) {
			for (i = 0, diff_pwr = 0; i <= nss_idx; i++)
				diff_pwr += rtlefuse->txpwr_ht40diff[path][i];
		} else {
			for (i = 0, diff_pwr = 0; i <= nss_idx; i++)
				diff_pwr += rtlefuse->txpwr_ht20diff[path][i];
		}
	}

	/* power by rate */
	if (is_5g)
		pwr_by_rate_base = rtlphy->txpwr_by_rate_base_5g
					[path][nss_idx][rate_section - 1];
	else
		pwr_by_rate_base = rtlphy->txpwr_by_rate_base_24g
					[path][nss_idx][rate_section];

	pwr_by_rate_offset =
		(s8)rtlphy->tx_power_by_rate_offset[band][path][nss_idx][rate];


	/* power limit */
	if (rate_section == 1 || rate_section == 0)
		bw_limit = 0;	/* CCK or OFDM ==> 20M */
	else if ((rate_section == 2 || rate_section == 3) &&
		 band == BAND_ON_5G && bandwidth == 2)
		bw_limit = 1;	/* HT 1/2SS && 5G && 80M ==> 40M */
	else
		bw_limit = bandwidth;

	if (regulation == TXPWR_LMT_WW) {
		for (i = 0, pwr_limit = 0x3F; i < 4; i++) {
			pwr_limit_tmp = (is_5g ?
				rtlphy->txpwr_limit_5g[i]
					[bw_limit][rate_section]
					[ch_idx][path]
				:
				rtlphy->txpwr_limit_2_4g[i]
					[bw_limit][rate_section]
					[ch_idx][path]
				);

			if (pwr_limit > pwr_limit_tmp)
				pwr_limit = pwr_limit_tmp;
		}
	} else {
		pwr_limit = (is_5g ?
			rtlphy->txpwr_limit_5g[regulation]
				[bw_limit][rate_section][ch_idx][path]
			:
			rtlphy->txpwr_limit_2_4g[regulation]
				[bw_limit][rate_section][ch_idx][path]);
	}

	tab_pwr_limit = get_pwr_limit_from_table(rtlpriv,
				  path, rate_info, bandwidth, ch_info);

	/* output */
#define DUMP_TX_POWER_TITLE()						\
	seq_printf(m, "%3s %1s %2s %13s "				\
		   "%4s %4s %2s "					\
		   "%4s %4s %2s %4s %3s %2s "				\
		   "%5s %4s %2s %4s "					\
		   "%6s %5s %4s "					\
		   "%6s %5s %2s %4s\n",					\
		   "CH", "p", "bw", "RATE",				\
		   "[PWR", "rPWR", "C]",				\
		   "[(BAS", "eBAS", "C)", "(DIF", "eDF", "C)",		\
		   "(DIF2", "eDF2", "C)", "CHK]",			\
		   "[RaBas", "RaOff", "CHK]",				\
		   "[LiOff", "(tLim", "C)", "CHK]")

	seq_printf(m,
		   "%3d %1d %2d %13s "
		   " %02xh  %02xh %-2s "
		   "  %02xh  %02xh %-2s %4d %3d %-2s %5d %4d %-2s %-4s "
		   "%5xh %5d %-4s "
		   "%6d %5d %-2s %-4s\n",
		   channel, path, bandwidth, rate_str,
		   tx_pwr, reg_tx_pwr,
		   (reg_tx_pwr == 0xFF ? " " :
		    reg_tx_pwr == tx_pwr ? "Q" : "X"),
		   base_pwr, efuse_base_pwr,
		   (base_pwr == efuse_base_pwr ? "O" : "X"),
		   diff_pwr, efuse_diff_pwr,
		   (diff_pwr == efuse_diff_pwr ? "O" : "X"),
		   diff_pwr2, efuse_diff_pwr2,
		   (diff_pwr2 == efuse_diff_pwr2 ? "O" : "X"),
		   (HACK_REGULATORY != 2 ? "?" :
			tx_pwr == base_pwr + diff_pwr + diff_pwr2 ? "O" : "X"),
		   pwr_by_rate_base, pwr_by_rate_offset,
		   (HACK_REGULATORY != 3 ? "?" :
			tx_pwr == base_pwr + diff_pwr + diff_pwr2 +
				  pwr_by_rate_offset ? "O" : "X"),
		   pwr_limit, tab_pwr_limit,
		   ((s8)(tab_pwr_limit - pwr_by_rate_base) == pwr_limit ? "O" :
		    tab_pwr_limit == pwr_limit && pwr_limit == 0x3F ?
		    "O" : "X"),
		   (HACK_REGULATORY != 1 ? "?" :
			tx_pwr == base_pwr + diff_pwr + diff_pwr2 +
				  (pwr_by_rate_offset < pwr_limit ?
				   pwr_by_rate_offset : pwr_limit) ? "O" : "X")
		   );
}

static void dump_tx_power_index(struct rtl_priv *rtlpriv, struct seq_file *m)
{
	enum ch_idx {
		CH_5G_IDX_36,
		CH_5G_IDX_38,
		CH_5G_IDX_40,
		CH_5G_IDX_42,	/* 80M */
		CH_5G_IDX_44,
		CH_5G_IDX_46,
		CH_5G_IDX_48,
		CH_5G_IDX_52,
		CH_5G_IDX_54,
		CH_5G_IDX_56,
		CH_5G_IDX_58,	/* 80M */
		CH_5G_IDX_60,
		CH_5G_IDX_62,
		CH_5G_IDX_64,
		CH_5G_IDX_100,
		CH_5G_IDX_102,
		CH_5G_IDX_104,
		CH_5G_IDX_106,	/* 80M */
		CH_5G_IDX_108,
		CH_5G_IDX_110,
		CH_5G_IDX_112,
		CH_5G_IDX_116,
		CH_5G_IDX_118,
		CH_5G_IDX_120,
		CH_5G_IDX_122,	/* 80M */
		CH_5G_IDX_124,
		CH_5G_IDX_126,
		CH_5G_IDX_128,
		CH_5G_IDX_132,
		CH_5G_IDX_134,
		CH_5G_IDX_136,
		CH_5G_IDX_138,	/* 80M */
		CH_5G_IDX_140,
		CH_5G_IDX_142,
		CH_5G_IDX_144,
		CH_5G_IDX_149,
		CH_5G_IDX_151,
		CH_5G_IDX_153,
		CH_5G_IDX_155,	/* 80M */
		CH_5G_IDX_157,
		CH_5G_IDX_159,
		CH_5G_IDX_161,
		CH_5G_IDX_165,
		CH_5G_IDX_167,
		CH_5G_IDX_169,
		CH_5G_IDX_171,	/* 80M */
		CH_5G_IDX_173,
		CH_5G_IDX_175,
		CH_5G_IDX_177,

		/* 80M */
		CH_5G_80M_IDX_42 = 0,
		CH_5G_80M_IDX_58,
		CH_5G_80M_IDX_106,
		CH_5G_80M_IDX_122,
		CH_5G_80M_IDX_138,
		CH_5G_80M_IDX_155,
		CH_5G_80M_IDX_171,
	};

#define RATE_ALL	0xFFFFFFFFFFFFFFFFULL	/* number to bitmap */
#define RATE_NO_CCK	0xFFFFFFFFFFFFFFF0ULL	/* number to bitmap */
#define BW_20		BIT(HT_CHANNEL_WIDTH_20)
#define BW_40		BIT(HT_CHANNEL_WIDTH_20_40)
#define BW_80		BIT(HT_CHANNEL_WIDTH_80)

#define _M_EFUSE_CCK_OFFSET(ch)	(			\
		(ch >= 1 && ch <= 2) ? 0x10 :		\
		(ch >= 3 && ch <= 5) ? 0x11 :		\
		(ch >= 6 && ch <= 8) ? 0x12 :		\
		(ch >= 9 && ch <= 11) ? 0x13 :		\
		(ch >= 12 && ch <= 13) ? 0x14 :		\
		0x15 /* ch == 14 */)
#define _M_EFUSE_BW40_OFFSET_2G(ch)	(		\
		(ch >= 1 && ch <= 2) ? 0x16 :		\
		(ch >= 3 && ch <= 5) ? 0x17 :		\
		(ch >= 6 && ch <= 8) ? 0x18 :		\
		(ch >= 9 && ch <= 11) ? 0x19 :		\
		0x1A /* ch == 12~14 */)
#define _M_EFUSE_BW40_OFFSET_5G(ch)	(		\
		(ch >= 36 && ch <= 42) ? 0x22 :		\
		(ch >= 44 && ch <= 48) ? 0x23 :		\
		(ch >= 52 && ch <= 58) ? 0x24 :		\
		(ch >= 60 && ch <= 64) ? 0x25 :		\
		(ch >= 100 && ch <= 106) ? 0x26 :	\
		(ch >= 108 && ch <= 112) ? 0x27 :	\
		(ch >= 116 && ch <= 122) ? 0x28 :	\
		(ch >= 124 && ch <= 128) ? 0x29 :	\
		(ch >= 132 && ch <= 138) ? 0x2a :	\
		(ch >= 140 && ch <= 144) ? 0x2b :	\
		(ch >= 149 && ch <= 155) ? 0x2c :	\
		(ch >= 157 && ch <= 161) ? 0x2d :	\
		(ch >= 165 && ch <= 171) ? 0x2e :	\
		(ch >= 173 && ch <= 177) ? 0x2f :	\
		0 /* wrong */)

#define _M_EFUSE_BW40_OFFSET(ch) (ch <= 14 ? _M_EFUSE_BW40_OFFSET_2G(ch) : \
				  _M_EFUSE_BW40_OFFSET_5G(ch))

#define _M_CH_2G(x)	{x, false, BAND_ON_2_4G,			\
			 (x) - 1, (x) - 1,				\
			 RATE_ALL, BW_20 | BW_40,			\
			 _M_EFUSE_CCK_OFFSET(x), _M_EFUSE_BW40_OFFSET(x), \
			}
#define _M_CH_5G_20M(x)	{x, true, BAND_ON_5G,				\
			 CH_5G_IDX_ ##x, CH_5G_IDX_ ##x,		\
			 RATE_NO_CCK, BW_20,				\
			 0, _M_EFUSE_BW40_OFFSET(x),			\
			}
#define _M_CH_5G_40M(x)	{x, true, BAND_ON_5G,				\
			 CH_5G_IDX_ ##x, CH_5G_IDX_ ##x,		\
			 RATE_NO_CCK, BW_40,				\
			 0, _M_EFUSE_BW40_OFFSET(x),			\
			}
#define _M_CH_5G_80M(x)	{x, true, BAND_ON_5G,				\
			 CH_5G_IDX_ ##x, CH_5G_80M_IDX_ ##x,		\
			 RATE_NO_CCK, BW_80,				\
			 0, _M_EFUSE_BW40_OFFSET(x),			\
			}

	static const struct rtl_channel_info channels[] = {
		/* 2G */
		_M_CH_2G(1),   _M_CH_2G(2),   _M_CH_2G(3),   _M_CH_2G(4),
		_M_CH_2G(5),   _M_CH_2G(6),   _M_CH_2G(7),
		_M_CH_2G(8),   _M_CH_2G(9),   _M_CH_2G(10),  _M_CH_2G(11),
		_M_CH_2G(12),  _M_CH_2G(13),  _M_CH_2G(14),
		/* 5G: Band 1 */
		_M_CH_5G_20M(36),  _M_CH_5G_40M(38),  _M_CH_5G_20M(40),
		_M_CH_5G_80M(42),
		_M_CH_5G_20M(44),  _M_CH_5G_40M(46),  _M_CH_5G_20M(48),
		/* 5G: Band 2 */
		_M_CH_5G_20M(52),  _M_CH_5G_40M(54),  _M_CH_5G_20M(56),
		_M_CH_5G_80M(58),
		_M_CH_5G_20M(60),  _M_CH_5G_40M(62),  _M_CH_5G_20M(64),
		/* 5G: Band 3 */
		_M_CH_5G_20M(100), _M_CH_5G_40M(102), _M_CH_5G_20M(104),
		_M_CH_5G_80M(106),
		_M_CH_5G_20M(108), _M_CH_5G_40M(110), _M_CH_5G_20M(112),
		/* 5G: Band 3 */
		_M_CH_5G_20M(116), _M_CH_5G_40M(118), _M_CH_5G_20M(120),
		_M_CH_5G_80M(122),
		_M_CH_5G_20M(124), _M_CH_5G_40M(126), _M_CH_5G_20M(128),
		/* 5G: Band 3 */
		_M_CH_5G_20M(132), _M_CH_5G_40M(134), _M_CH_5G_20M(136),
		_M_CH_5G_80M(138),
		_M_CH_5G_20M(140), _M_CH_5G_40M(142), _M_CH_5G_20M(144),
		/* 5G: Band 4 */
		_M_CH_5G_20M(149), _M_CH_5G_40M(151), _M_CH_5G_20M(153),
		_M_CH_5G_80M(155),
		_M_CH_5G_20M(157), _M_CH_5G_40M(159), _M_CH_5G_20M(161),
		/* 5G: Band 4 */
		_M_CH_5G_20M(165), _M_CH_5G_40M(167), _M_CH_5G_20M(169),
		_M_CH_5G_80M(171),
		_M_CH_5G_20M(173), _M_CH_5G_40M(175), _M_CH_5G_20M(177),
	};
#undef RATE_ALL
#undef RATE_NO_CCK
#undef BW_20
#undef BW_40
#undef BW_80
#undef _M_CH_2G
#undef _M_CH_5G_20M
#undef _M_CH_5G_40M
#undef _M_CH_5G_80M

#define _M_RATE_SECTION(rate) (						 \
	(rate >= DESC_RATE1M && rate <= DESC_RATE11M) ? CCK :		 \
	(rate >= DESC_RATE6M && rate <= DESC_RATE54M) ? OFDM :		 \
	(rate >= DESC_RATEMCS0 && rate <= DESC_RATEMCS7) ?		 \
		HT_MCS0_MCS7 :						 \
	(rate >= DESC_RATEMCS8 && rate <= DESC_RATEMCS15) ?		 \
		HT_MCS8_MCS15 :						 \
	(rate >= DESC_RATEVHT1SS_MCS0 && rate <= DESC_RATEVHT1SS_MCS9) ? \
		VHT_1SSMCS0_1SSMCS9 :					 \
	(rate >= DESC_RATEVHT2SS_MCS0 && rate <= DESC_RATEVHT2SS_MCS9) ? \
		VHT_2SSMCS0_2SSMCS9 : MAX_RATE_SECTION)
#define _M_IS_CCK(rate)		(_M_RATE_SECTION(rate) == CCK)
#define _M_IS_OFDM(rate)	(_M_RATE_SECTION(rate) == OFDM)
#define _M_RATE_NSS_1(rate)	(					\
		 _M_IS_CCK(rate) ? 0 :					\
		 _M_IS_OFDM(rate) ? 0 :					\
		 _M_RATE_SECTION(rate) == HT_MCS0_MCS7 ? 0 :		\
		 _M_RATE_SECTION(rate) == HT_MCS8_MCS15 ? 1 :		\
		 _M_RATE_SECTION(rate) == VHT_1SSMCS0_1SSMCS9 ? 0 :	\
		 _M_RATE_SECTION(rate) == VHT_2SSMCS0_2SSMCS9 ? 1 : 0)
#define _M_RATE(x)	{						\
	DESC_RATE##x, #x, _M_RATE_SECTION(DESC_RATE##x),		\
	_M_IS_CCK(DESC_RATE##x), _M_IS_OFDM(DESC_RATE##x),		\
	_M_RATE_NSS_1(DESC_RATE##x),					\
	}

	static const struct rtl_rate_info rates[] = {
		_M_RATE(1M),
		_M_RATE(2M),
		_M_RATE(5_5M),
		_M_RATE(11M),

		_M_RATE(6M),
		_M_RATE(9M),
		_M_RATE(12M),
		_M_RATE(18M),
		_M_RATE(24M),
		_M_RATE(36M),
		_M_RATE(48M),
		_M_RATE(54M),

		_M_RATE(MCS0),
		_M_RATE(MCS1),
		_M_RATE(MCS2),
		_M_RATE(MCS3),
		_M_RATE(MCS4),
		_M_RATE(MCS5),
		_M_RATE(MCS6),
		_M_RATE(MCS7),
		_M_RATE(MCS8),
		_M_RATE(MCS9),
		_M_RATE(MCS10),
		_M_RATE(MCS11),
		_M_RATE(MCS12),
		_M_RATE(MCS13),
		_M_RATE(MCS14),
		_M_RATE(MCS15),

		_M_RATE(VHT1SS_MCS0),
		_M_RATE(VHT1SS_MCS1),
		_M_RATE(VHT1SS_MCS2),
		_M_RATE(VHT1SS_MCS3),
		_M_RATE(VHT1SS_MCS4),
		_M_RATE(VHT1SS_MCS5),
		_M_RATE(VHT1SS_MCS6),
		_M_RATE(VHT1SS_MCS7),
		_M_RATE(VHT1SS_MCS8),
		_M_RATE(VHT1SS_MCS9),
		_M_RATE(VHT2SS_MCS0),
		_M_RATE(VHT2SS_MCS1),
		_M_RATE(VHT2SS_MCS2),
		_M_RATE(VHT2SS_MCS3),
		_M_RATE(VHT2SS_MCS4),
		_M_RATE(VHT2SS_MCS5),
		_M_RATE(VHT2SS_MCS6),
		_M_RATE(VHT2SS_MCS7),
		_M_RATE(VHT2SS_MCS8),
		_M_RATE(VHT2SS_MCS9),
	};
#undef _M_RATE
#undef _M_RATE_SECTION
#define CHANNELS_NUM	(sizeof(channels)/sizeof(channels[0]))
#define RATES_NUM	(sizeof(rates)/sizeof(rates[0]))

	enum radio_path path;
	enum ht_channel_width bandwidth;
	int rate_idx, channel_idx;

	const struct rtl_rate_info *rate_info;
	const struct rtl_channel_info *channel_info;

	for (channel_idx = 0; channel_idx < CHANNELS_NUM; channel_idx++) {
		channel_info = &channels[channel_idx];

		DUMP_TX_POWER_TITLE();

		for (path = 0; path < 2; path++) {
			for (bandwidth = 0; bandwidth < HT_CHANNEL_WIDTH_MAX;
			     bandwidth++) {

				if (!(channel_info->bw_mask & BIT(bandwidth)))
					continue;

				for (rate_idx = 0; rate_idx < RATES_NUM;
				     rate_idx++) {
					rate_info = &rates[rate_idx];

					if (!(channel_info->rate_mask &
					      BIT_ULL(rate_info->rate)))
						continue;

					dump_tx_power_index_core(rtlpriv, m,
								 path,
								 rate_info,
								 bandwidth,
								 channel_info);
				}
			}
		}
	}
}

static void rtl_hex_dump(struct seq_file *m, const char *prefix, int dump_type,
			 int row, int group, const void *data_void, long size,
			 bool ascii)
{
	long i;
	const u8 *data = data_void;
	bool last_ch_nl = false;

	seq_printf(m, "%s:\n", prefix);

	for (i = 0; i < size; i++) {

		if ((i % row) == 0)
			seq_printf(m, "%08lX ", i);

		seq_printf(m, "%02X ", *data++);
		last_ch_nl = false;

		if ((i % row) == (row - 1)) {
			seq_puts(m, "\n");
			last_ch_nl = true;
		}
	}

	if (last_ch_nl)
		seq_puts(m, "\n");
	else
		seq_puts(m, "\n\n");
}

static int rtl_debug_get_txpwr(struct seq_file *m, void *v)
{
	struct rtl_debugfs_priv *debugfs_priv = m->private;
	struct rtl_priv *rtlpriv = debugfs_priv->rtlpriv;
	struct rtl_phy *rtlphy = &rtlpriv->phy;
	struct rtl_efuse *rtlefuse = rtl_efuse(rtlpriv);

	/* show regulartory */
	seq_printf(m, "rtlefuse->eeprom_regulatory=%d\n",
		   rtlefuse->eeprom_regulatory);
	seq_printf(m, "\t(%c) != 2: use tx power by rate\n",
		   rtlefuse->eeprom_regulatory != 2 ? 'O' : 'X');
	seq_printf(m, "\t(%c) == 1: use tx power limit\n\n",
		   rtlefuse->eeprom_regulatory == 1 ? 'O' : 'X');

	/* dump tx pwr array */
#define TXPWR_HEX_DUMP(_x)	\
	rtl_hex_dump(m, "" #_x, DUMP_PREFIX_NONE,	\
		     16, 64, _x,	\
		     sizeof(_x),	\
		     0)

	/* efuse map */
	TXPWR_HEX_DUMP(rtlefuse->efuse_map[0]);

	/* tx power base (2.4G) */
	TXPWR_HEX_DUMP(rtlefuse->txpwrlevel_cck);
	TXPWR_HEX_DUMP(rtlefuse->txpwrlevel_ht40_1s);

	TXPWR_HEX_DUMP(rtlefuse->txpwr_cckdiff);
	TXPWR_HEX_DUMP(rtlefuse->txpwr_legacyhtdiff);
	TXPWR_HEX_DUMP(rtlefuse->txpwr_ht20diff);
	TXPWR_HEX_DUMP(rtlefuse->txpwr_ht40diff);

	/* tx power base (5G) */
	TXPWR_HEX_DUMP(rtlefuse->txpwr_5g_bw40base);
	TXPWR_HEX_DUMP(rtlefuse->txpwr_5g_bw80base);

	TXPWR_HEX_DUMP(rtlefuse->txpwr_5g_ofdmdiff);
	TXPWR_HEX_DUMP(rtlefuse->txpwr_5g_bw20diff);
	TXPWR_HEX_DUMP(rtlefuse->txpwr_5g_bw40diff);
	TXPWR_HEX_DUMP(rtlefuse->txpwr_5g_bw80diff);

	/* tx power by rate */
	TXPWR_HEX_DUMP(rtlphy->tx_power_by_rate_offset);
	TXPWR_HEX_DUMP(rtlphy->txpwr_by_rate_base_24g);
	TXPWR_HEX_DUMP(rtlphy->txpwr_by_rate_base_5g);

	/* tx power limit */
	TXPWR_HEX_DUMP(rtlphy->txpwr_limit_2_4g);
	TXPWR_HEX_DUMP(rtlphy->txpwr_limit_5g);

#undef TXPWR_HEX_DUMP

	/* dump txpower index */
	dump_tx_power_index(rtlpriv, m);

	return 0;
}

static int dl_debug_open_txpwr(struct inode *inode, struct file *file)
{
	return single_open(file, rtl_debug_get_txpwr,
			   inode->i_private);
}

static struct rtl_debugfs_priv rtl_debug_priv_txpwr = {
	.cb_read = rtl_debug_get_txpwr,
	.cb_data = 0,
};

static const struct file_operations file_ops_txpwr = {
	.open = dl_debug_open_txpwr,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

static int rtl_debug_get_btcoex(struct seq_file *m, void *v)
{
	struct rtl_debugfs_priv *debugfs_priv = m->private;
	struct rtl_priv *rtlpriv = debugfs_priv->rtlpriv;

	if (rtlpriv->cfg->ops->get_btc_status())
		rtlpriv->btcoexist.btc_ops->btc_display_bt_coex_info(rtlpriv,
								     m);

	seq_puts(m, "\n");

	return 0;
}

static struct rtl_debugfs_priv rtl_debug_priv_btcoex = {
	.cb_read = rtl_debug_get_btcoex,
	.cb_data = 0,
};

static int rtl_dump_sta_info(struct seq_file *m, void *v)
{
	struct rtl_debugfs_priv *debugfs_priv = m->private;
	struct rtl_priv *rtlpriv = debugfs_priv->rtlpriv;
	struct rtl_sta_info *sta = rtlpriv->sta;

	rtlpriv->phydm.ops->phydm_dump_sta_info(rtlpriv, m, sta);

	return 0;
}

static struct rtl_debugfs_priv rtl_debug_priv_dump_sta_info = {
	.cb_read = rtl_dump_sta_info,
	.cb_data = 0,
};

static int rtl_debug_get_rsvd(struct seq_file *m, void *v)
{
	struct rtl_debugfs_priv *debugfs_priv = m->private;
	struct rtl_priv *rtlpriv = debugfs_priv->rtlpriv;
	u32 /*addr = 0, size = 0,*/ count = 0;
	u32 page_size = 128, data_low = 0, data_high = 0;
	u16 txbndy = 0, offset = 0;
	u8 i = 0, j;
	u8 *data_raw;
	u32 page_offset = 0;
	u32 buffer_size = 1280;

#define REG_TDECTRL						0x0208
#define REG_PKT_BUFF_ACCESS_CTRL		0x0106
#define REG_PKTBUF_DBG_CTRL			0x0140
#define REG_PKTBUF_DBG_DATA_L			0x0144
#define REG_PKTBUF_DBG_DATA_H		0x0148

	txbndy = rtl_read_byte(rtlpriv, REG_TDECTRL + 1);

	offset = (txbndy + page_offset) * page_size / 8;
	count = (buffer_size / 8);

	rtl_write_byte(rtlpriv, REG_PKT_BUFF_ACCESS_CTRL, 0x69);

	for (i = 0 ; i < count ; i++) {
		rtl_write_dword(rtlpriv, REG_PKTBUF_DBG_CTRL, offset + i);
		data_low = rtl_read_dword(rtlpriv, REG_PKTBUF_DBG_DATA_L);
		data_high = rtl_read_dword(rtlpriv, REG_PKTBUF_DBG_DATA_H);

		if ((i & 0x1) == 0)
			seq_printf(m, "0x%04X: ", i * 8);

		data_raw = (u8 *)&data_low;
		for (j = 0; j < 4; j++)
			seq_printf(m, "%02X ", *(data_raw + j));

		data_raw = (u8 *)&data_high;
		for (j = 0; j < 4; j++)
			seq_printf(m, "%02X ", *(data_raw + j));

		if (i & 0x1)
			seq_puts(m, "\n");
	}
	rtl_write_byte(rtlpriv, REG_PKT_BUFF_ACCESS_CTRL, 0x0);

	seq_puts(m, "\n");

	return 0;
}

static struct rtl_debugfs_priv rtl_debug_priv_rsvd = {
	.cb_read = rtl_debug_get_rsvd,
	.cb_data = 0,
};

static ssize_t rtl_debugfs_set_write_reg(struct file *filp,
					 const char __user *buffer,
					 size_t count, loff_t *loff)
{
	struct rtl_debugfs_priv *debugfs_priv = filp->private_data;
	struct rtl_priv *rtlpriv = debugfs_priv->rtlpriv;
	char tmp[32 + 1];
	int tmp_len;
	u32 addr, val, len;
	int num;

	if (count < 3)
		return -EFAULT;

	tmp_len = (count > sizeof(tmp) - 1 ? sizeof(tmp) - 1 : count);

	if (!buffer || copy_from_user(tmp, buffer, tmp_len))
		return count;

	tmp[tmp_len] = '\0';

	/* write BB/MAC register */
	num = sscanf(tmp, "%x %x %x", &addr, &val, &len);

	if (num !=  3)
		return count;

	switch (len) {
	case 1:
		rtl_write_byte(rtlpriv, addr, (u8)val);
		break;
	case 2:
		rtl_write_word(rtlpriv, addr, (u16)val);
		break;
	case 4:
		rtl_write_dword(rtlpriv, addr, val);
		break;
	default:
		/*printk("error write length=%d", len);*/
		break;
	}

	return count;
}

static struct rtl_debugfs_priv rtl_debug_priv_write_reg = {
	.cb_write = rtl_debugfs_set_write_reg,
};

static ssize_t rtl_debugfs_set_write_h2c(struct file *filp,
					 const char __user *buffer,
					 size_t count, loff_t *loff)
{
	struct rtl_debugfs_priv *debugfs_priv = filp->private_data;
	struct rtl_priv *rtlpriv = debugfs_priv->rtlpriv;
	struct ieee80211_hw *hw = rtlpriv->hw;
	char tmp[32 + 1];
	int tmp_len;
	u8 h2c_len, h2c_data_packed[8];
	int h2c_data[8];	/* idx 0: cmd */
	int i;

	if (count < 3)
		return -EFAULT;

	tmp_len = (count > sizeof(tmp) - 1 ? sizeof(tmp) - 1 : count);

	if (!buffer || copy_from_user(tmp, buffer, tmp_len))
		return count;

	tmp[tmp_len] = '\0';

	h2c_len = sscanf(tmp, "%X %X %X %X %X %X %X %X",
			 &h2c_data[0], &h2c_data[1],
			 &h2c_data[2], &h2c_data[3],
			 &h2c_data[4], &h2c_data[5],
			 &h2c_data[6], &h2c_data[7]);

	if (h2c_len <= 0)
		return count;

	for (i = 0; i < h2c_len; i++)
		h2c_data_packed[i] = (u8)h2c_data[i];

	rtlpriv->cfg->ops->fill_h2c_cmd(hw, h2c_data_packed[0],
					h2c_len - 1,
					&h2c_data_packed[1]);

	return count;
}

static struct rtl_debugfs_priv rtl_debug_priv_write_h2c = {
	.cb_write = rtl_debugfs_set_write_h2c,
};

static ssize_t rtl_debugfs_set_write_rfreg(struct file *filp,
					   const char __user *buffer,
					    size_t count, loff_t *loff)
{
	struct rtl_debugfs_priv *debugfs_priv = filp->private_data;
	struct rtl_priv *rtlpriv = debugfs_priv->rtlpriv;
	struct ieee80211_hw *hw = rtlpriv->hw;
	char tmp[32 + 1];
	int tmp_len;
	int num;
	int path;
	u32 addr, bitmask, data;

	if (count < 3)
		return -EFAULT;

	tmp_len = (count > sizeof(tmp) - 1 ? sizeof(tmp) - 1 : count);

	if (!buffer || copy_from_user(tmp, buffer, tmp_len))
		return count;

	tmp[tmp_len] = '\0';

	num = sscanf(tmp, "%X %X %X %X",
		     &path, &addr, &bitmask, &data);

	if (num != 4) {
		RT_TRACE(rtlpriv, COMP_ERR, DBG_DMESG,
			 "Format is <path> <addr> <mask> <data>\n");
		return count;
	}

	rtl_set_rfreg(hw, path, addr, bitmask, data);

	return count;
}

static struct rtl_debugfs_priv rtl_debug_priv_write_rfreg = {
	.cb_write = rtl_debugfs_set_write_rfreg,
};

static int rtl_debugfs_close(struct inode *inode, struct file *filp)
{
	return 0;
}

static ssize_t rtl_debugfs_common_write(struct file *filp,
					const char __user *buffer,
					size_t count, loff_t *loff)
{
	struct rtl_debugfs_priv *debugfs_priv = filp->private_data;

	return debugfs_priv->cb_write(filp, buffer, count, loff);
}

static const struct file_operations file_ops_common_write = {
	.owner = THIS_MODULE,
	.write = rtl_debugfs_common_write,
	.open = simple_open,
	.release = rtl_debugfs_close,
};

static ssize_t rtl_debugfs_phydm_cmd(struct file *filp,
				     const char __user *buffer,
				     size_t count, loff_t *loff)
{
	struct rtl_debugfs_priv *debugfs_priv = filp->private_data;
	struct rtl_priv *rtlpriv = debugfs_priv->rtlpriv;

	char tmp[64];

	if (!rtlpriv->dbg.msg_buf)
		return -ENOMEM;

	if (!rtlpriv->phydm.ops)
		return -EFAULT;

	if (buffer && !copy_from_user(tmp, buffer, sizeof(tmp))) {
		tmp[count] = '\0';

		rtlpriv->phydm.ops->phydm_debug_cmd(rtlpriv, tmp, count,
						    rtlpriv->dbg.msg_buf,
						    80 * 25);
	}

	return count;
}

static int rtl_debug_get_phydm_cmd(struct seq_file *m, void *v)
{
	struct rtl_debugfs_priv *debugfs_priv = m->private;
	struct rtl_priv *rtlpriv = debugfs_priv->rtlpriv;

	if (rtlpriv->dbg.msg_buf)
		seq_puts(m, rtlpriv->dbg.msg_buf);

	return 0;
}

static int rtl_debugfs_open_rw(struct inode *inode, struct file *filp)
{
	if (filp->f_mode & FMODE_READ)
		single_open(filp, rtl_debug_get_common, inode->i_private);
	else
		filp->private_data = inode->i_private;

	return 0;
}

static int rtl_debugfs_close_rw(struct inode *inode, struct file *filp)
{
	if (filp->f_mode == FMODE_READ)
		seq_release(inode, filp);

	return 0;
}

static struct rtl_debugfs_priv rtl_debug_priv_phydm_cmd = {
	.cb_read = rtl_debug_get_phydm_cmd,
	.cb_write = rtl_debugfs_phydm_cmd,
	.cb_data = 0,
};

static const struct file_operations file_ops_common_rw = {
	.owner = THIS_MODULE,
	.open = rtl_debugfs_open_rw,
	.release = rtl_debugfs_close_rw,
	.read = seq_read,
	.llseek = seq_lseek,
	.write = rtl_debugfs_common_write,
};

static ssize_t rtl_debugfs_dis_turboedca(struct file *filp,
					 const char __user *buffer,
					 size_t count, loff_t *loff)
{
	struct rtl_debugfs_priv *debugfs_priv = filp->private_data;
	struct rtl_priv *rtlpriv = debugfs_priv->rtlpriv;

	unsigned long val;

	if (count > 2 || !buffer)
		return -EFAULT;

	if (kstrtoul_from_user(buffer, count, 16, &val))
		return count;

	rtlpriv->dm.dis_turboedca = (val ? true : false);

	return count;
}

static int rtl_debug_get_dis_turboedca(struct seq_file *m, void *v)
{
	struct rtl_debugfs_priv *debugfs_priv = m->private;
	struct rtl_priv *rtlpriv = debugfs_priv->rtlpriv;

	seq_printf(m, "Turbo EDCA is %s\n",
		   rtlpriv->dm.dis_turboedca ? "Disable" : "Enable");

	return 0;
}

static struct rtl_debugfs_priv rtl_debug_priv_dis_turboedca = {
	.cb_read = rtl_debug_get_dis_turboedca,
	.cb_write = rtl_debugfs_dis_turboedca,
	.cb_data = 0,
};

#define RTL_DEBUGFS_ADD_CORE(name, mode, fopname)			   \
	do {								   \
		rtl_debug_priv_ ##name.rtlpriv = rtlpriv;		   \
		if (!debugfs_create_file(#name, mode,			   \
					 parent, &rtl_debug_priv_ ##name,  \
					 &file_ops_ ##fopname))		   \
			pr_err("Unable to initialize debugfs:%s/%s\n",	   \
			       rtlpriv->dbg.debugfs_name,		   \
			       #name);					   \
	} while (0)

#define RTL_DEBUGFS_ADD(name)						   \
		RTL_DEBUGFS_ADD_CORE(name, S_IFREG | 0444, common)
#define RTL_DEBUGFS_ADD_W(name)						   \
		RTL_DEBUGFS_ADD_CORE(name, S_IFREG | 0222, common_write)
#define RTL_DEBUGFS_ADD_RW(name)					   \
		RTL_DEBUGFS_ADD_CORE(name, S_IFREG | 0666, common_rw)

void rtl_debug_add_one(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);
	struct rtl_efuse *rtlefuse = rtl_efuse(rtl_priv(hw));
	struct dentry *parent;

	rtlpriv->dbg.msg_buf = vzalloc(80 * 25);

	snprintf(rtlpriv->dbg.debugfs_name, 18, "%pMF", rtlefuse->dev_addr);

	rtlpriv->dbg.debugfs_dir =
		debugfs_create_dir(rtlpriv->dbg.debugfs_name, debugfs_topdir);
	if (!rtlpriv->dbg.debugfs_dir) {
		pr_err("Unable to init debugfs:/%s/%s\n", rtlpriv->cfg->name,
		       rtlpriv->dbg.debugfs_name);
		return;
	}

	parent = rtlpriv->dbg.debugfs_dir;

	RTL_DEBUGFS_ADD(mac_0);
	RTL_DEBUGFS_ADD(mac_1);
	RTL_DEBUGFS_ADD(mac_2);
	RTL_DEBUGFS_ADD(mac_3);
	RTL_DEBUGFS_ADD(mac_4);
	RTL_DEBUGFS_ADD(mac_5);
	RTL_DEBUGFS_ADD(mac_6);
	RTL_DEBUGFS_ADD(mac_7);
	RTL_DEBUGFS_ADD(bb_8);
	RTL_DEBUGFS_ADD(bb_9);
	RTL_DEBUGFS_ADD(bb_a);
	RTL_DEBUGFS_ADD(bb_b);
	RTL_DEBUGFS_ADD(bb_c);
	RTL_DEBUGFS_ADD(bb_d);
	RTL_DEBUGFS_ADD(bb_e);
	RTL_DEBUGFS_ADD(bb_f);
	RTL_DEBUGFS_ADD(mac_10);
	RTL_DEBUGFS_ADD(mac_11);
	RTL_DEBUGFS_ADD(mac_12);
	RTL_DEBUGFS_ADD(mac_13);
	RTL_DEBUGFS_ADD(mac_14);
	RTL_DEBUGFS_ADD(mac_15);
	RTL_DEBUGFS_ADD(mac_16);
	RTL_DEBUGFS_ADD(mac_17);
	RTL_DEBUGFS_ADD(bb_18);
	RTL_DEBUGFS_ADD(bb_19);
	RTL_DEBUGFS_ADD(bb_1a);
	RTL_DEBUGFS_ADD(bb_1b);
	RTL_DEBUGFS_ADD(bb_1c);
	RTL_DEBUGFS_ADD(bb_1d);
	RTL_DEBUGFS_ADD(bb_1e);
	RTL_DEBUGFS_ADD(bb_1f);
	RTL_DEBUGFS_ADD(rf_a);
	RTL_DEBUGFS_ADD(rf_b);

	RTL_DEBUGFS_ADD(cam_1);
	RTL_DEBUGFS_ADD(cam_2);
	RTL_DEBUGFS_ADD(cam_3);

	RTL_DEBUGFS_ADD(txpwr);
	RTL_DEBUGFS_ADD(btcoex);
	RTL_DEBUGFS_ADD(dump_sta_info);
	RTL_DEBUGFS_ADD(rsvd);

	RTL_DEBUGFS_ADD_W(write_reg);
	RTL_DEBUGFS_ADD_W(write_h2c);
	RTL_DEBUGFS_ADD_W(write_rfreg);

	RTL_DEBUGFS_ADD_RW(phydm_cmd);
	RTL_DEBUGFS_ADD_RW(dis_turboedca);
}
EXPORT_SYMBOL_GPL(rtl_debug_add_one);

void rtl_debug_remove_one(struct ieee80211_hw *hw)
{
	struct rtl_priv *rtlpriv = rtl_priv(hw);

	debugfs_remove_recursive(rtlpriv->dbg.debugfs_dir);
	rtlpriv->dbg.debugfs_dir = NULL;

	vfree(rtlpriv->dbg.msg_buf);
}
EXPORT_SYMBOL_GPL(rtl_debug_remove_one);

void rtl_debugfs_add_topdir(void)
{
	debugfs_topdir = debugfs_create_dir("rtlwifi", NULL);
}

void rtl_debugfs_remove_topdir(void)
{
	debugfs_remove_recursive(debugfs_topdir);
}

#endif
