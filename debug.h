/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 2018  Realtek Corporation.
 */

#ifndef __RTW_DEBUG_H
#define __RTW_DEBUG_H

#ifdef CONFIG_RTW88_DEBUGFS

void rtw_debugfs_init(struct rtw_dev *rtwdev);

#else

static inline void rtw_debugfs_init(struct rtw_dev *rtwdev) {}

#endif /* CONFIG_RTW88_DEBUGFS */

#ifdef CONFIG_RTW88_DEBUG

__printf(2, 3)
void __rtw_dbg(struct rtw_dev *rtwdev, const char *fmt, ...);

#define rtw_dbg(rtwdev, a...) __rtw_dbg(rtwdev, ##a)

#else

static inline void rtw_dbg(struct rtw_dev *rtwdev, const char *fmt, ...) {}

#endif /* CONFIG_RTW88_DEBUG */

#define rtw_info(rtwdev, a...) dev_info(rtwdev->dev, ##a)
#define rtw_warn(rtwdev, a...) dev_warn(rtwdev->dev, ##a)
#define rtw_err(rtwdev, a...) dev_err(rtwdev->dev, ##a)

#endif
