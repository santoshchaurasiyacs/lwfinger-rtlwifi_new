/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 2018  Realtek Corporation.
 */

#ifndef __RTW_DEBUG_H
#define __RTW_DEBUG_H

#ifdef CONFIG_RTWLAN_DEBUGFS

void rtw_debugfs_init(struct rtw_dev *rtwdev);
void rtw_debugfs_deinit(void);

#else

static inline void rtw_debugfs_init(struct rtw_dev *rtwdev) {}
static inline void rtw_debugfs_deinit(void) {}

#endif /* CONFIG_RTWLAN_DEBUGFS */

#ifdef CONFIG_RTWLAN_DEBUG

__printf(2, 3)
void __rtw_dbg(struct rtw_dev *rtwdev, const char *fmt, ...);
__printf(2, 3)
void __rtw_info(struct rtw_dev *rtwdev, const char *fmt, ...);
__printf(2, 3)
void __rtw_warn(struct rtw_dev *rtwdev, const char *fmt, ...);
__printf(2, 3)
void __rtw_err(struct rtw_dev *rtwdev, const char *fmt, ...);

#define rtw_dbg(rtwdev, a...) __rtw_dbg(rtwdev, ##a)
#define rtw_info(rtwdev, a...) __rtw_info(rtwdev, ##a)
#define rtw_warn(rtwdev, a...) __rtw_warn(rtwdev, ##a)
#define rtw_err(rtwdev, a...) __rtw_err(rtwdev, ##a)

#else

static inline void rtw_dbg(struct rtw_dev *rtwdev, const char *fmt, ...) {}
static inline void rtw_info(struct rtw_dev *rtwdev, const char *fmt, ...) {}
static inline void rtw_warn(struct rtw_dev *rtwdev, const char *fmt, ...) {}
static inline void rtw_err(struct rtw_dev *rtwdev, const char *fmt, ...) {}

#endif /* CONFIG_RTWLAN_DEBUG */

#endif
