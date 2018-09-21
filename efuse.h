/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 2018  Realtek Corporation.
 */

#ifndef __RTW_EFUSE_H__
#define __RTW_EFUSE_H__

#define EFUSE_HW_CAP_IGNORE		0
#define EFUSE_HW_CAP_PTCL_VHT		3
#define EFUSE_HW_CAP_SUPP_BW80		7
#define EFUSE_HW_CAP_SUPP_BW40		6

struct efuse_hw_cap {
	u8 rsvd_0;
	u8 rsvd_1;
	u8 rsvd_2;
	u8 rsvd_3;
#ifdef __LITTLE_ENDIAN
	u8 hci:4;
	u8 rsvd_4:4;
#else
	u8 rsvd_4:4;
	u8 hci:4;
#endif
	u8 rsvd_5;
#ifdef __LITTLE_ENDIAN
	u8 bw:3;
	u8 nss:2;
	u8 ant_num:3;
#else
	u8 ant_num:3;
	u8 nss:2;
	u8 bw:3;
#endif
#ifdef __LITTLE_ENDIAN
	u8 rsvd_7_1:2;
	u8 ptcl:2;
	u8 rsvd_7_2:4;
#else
	u8 rsvd_7_2:4;
	u8 ptcl:2;
	u8 rsvd_7_1:2;
#endif
	u8 rsvd_8;
	u8 rsvd_9;
	u8 rsvd_10;
	u8 rsvd_11;
	u8 rsvd_12;
};

int rtw_parse_efuse_map(struct rtw_dev *rtwdev);

#endif
