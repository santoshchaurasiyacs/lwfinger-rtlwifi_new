/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 2018  Realtek Corporation.
 */

#ifndef __RTW8822C_H__
#define __RTW8822C_H__

#include <asm/byteorder.h>

#define REG_ANAPARLDO_POW_MAC 0x0029
#define RCR_VHT_ACK		BIT(26)

struct rtw8822cu_efuse {
	u8 res4[4];			/* 0xd0 */
	u8 usb_optional_function;
	u8 res5[0x1e];
	u8 res6[2];
	u8 serial[0x0b];		/* 0xf5 */
	u8 vid;				/* 0x100 */
	u8 res7;
	u8 pid;
	u8 res8[4];
	u8 mac_addr[ETH_ALEN];		/* 0x107 */
	u8 res9[2];
	u8 vendor_name[0x07];
	u8 res10[2];
	u8 device_name[0x14];
	u8 res11[0xcf];
	u8 package_type;		/* 0x1fb */
	u8 res12[0x4];
};

struct rtw8822ce_efuse {
	u8 mac_addr[ETH_ALEN];		/* 0xd0 */
	u8 vender_id[2];
	u8 device_id[2];
	u8 sub_vender_id[2];
	u8 sub_device_id[2];
	u8 pmc[2];
	u8 exp_device_cap[2];
	u8 msi_cap;
	u8 ltr_cap;			/* 0xe3 */
	u8 exp_link_control[2];
	u8 link_cap[4];
	u8 link_control[2];
	u8 serial_number[8];
	u8 res0:2;			/* 0xf4 */
	u8 ltr_en:1;
	u8 res1:2;
	u8 obff:2;
	u8 res2:3;
	u8 obff_cap:2;
	u8 res3:4;
	u8 res4[3];
	u8 class_code[3];
	u8 pci_pm_L1_2_supp:1;
	u8 pci_pm_L1_1_supp:1;
	u8 aspm_pm_L1_2_supp:1;
	u8 aspm_pm_L1_1_supp:1;
	u8 L1_pm_substates_supp:1;
	u8 res5:3;
	u8 port_common_mode_restore_time;
	u8 port_t_power_on_scale:2;
	u8 res6:1;
	u8 port_t_power_on_value:5;
	u8 res7;
};

struct rtw8822c_efuse {
	__le16 rtl_id;
	u8 res0[0x0e];

	/* power index for four RF paths */
	struct rtw_txpwr_idx txpwr_idx_table[4];

	u8 channel_plan;		/* 0xb8 */
	u8 xtal_k;
	u8 thermal_meter;
	u8 iqk_lck;
	u8 pa_type;			/* 0xbc */
	u8 lna_type_2g[2];		/* 0xbd */
	u8 lna_type_5g[2];
	u8 rf_board_option;
	u8 rf_feature_option;
	u8 rf_bt_setting;
	u8 eeprom_version;
	u8 eeprom_customer_id;
	u8 tx_bb_swing_setting_2g;
	u8 tx_bb_swing_setting_5g;
	u8 tx_pwr_calibrate_rate;
	u8 rf_antenna_option;		/* 0xc9 */
	u8 rfe_option;
	u8 country_code[2];
	u8 res[3];
	union {
		struct rtw8822cu_efuse u;
		struct rtw8822ce_efuse e;
	};
};

struct phy_status_jaguar3_page0 {
#ifdef __LITTLE_ENDIAN
	u8 page_num:4;
	u8 pkt_cnt:2;
	u8 channel_msb:2;
#else
	u8 channel_msb:2;
	u8 pkt_cnt:2;
	u8 page_num:4;
#endif
	s8 pwdb_a;
#ifdef __LITTLE_ENDIAN
	u8 gain_a:6;
	u8 rsvd_0:1;
	u8 trsw:1;
#else
	u8 trsw:1;
	u8 rsvd_0:1;
	u8 gain_a:6;
#endif

#ifdef __LITTLE_ENDIAN
	u8 agc_table_b:4;
	u8 agc_table_c:4;
#else
	u8 agc_table_c:4;
	u8 agc_table_b:4;
#endif
#ifdef __LITTLE_ENDIAN
	u8 rsvd_1:4;
	u8 agc_table_d:4;
#else
	u8 agc_table_d:4;
	u8 rsvd_1:4;
#endif
#ifdef __LITTLE_ENDIAN
	u8 l_rxsc:4;
	u8 agc_table_a:4;
#else
	u8 agc_table_a:4;
	u8 l_rxsc:4;
#endif
	u8 channel;
#ifdef __LITTLE_ENDIAN
	u8 band:2;
	u8 rsvd_2_1:1;
	u8 hw_antsw_occur_keep_cck:1;
	u8 gnt_bt_keep_cck:1;
	u8 rsvd_2_2:1;
	u8 path_sel_o:2;
#else
	u8 path_sel_o:2;
	u8 rsvd_2_2:1;
	u8 gnt_bt_keep_cck:1;
	u8 hw_antsw_occur_keep_cck:1;
	u8 rsvd_2_1:1;
	u8 band:2;
#endif
	u16 length;
#ifdef __LITTLE_ENDIAN
	u8 antidx_a:4;
	u8 antidx_b:4;
#else
	u8 antidx_b:4;
	u8 antidx_a:4;
#endif
#ifdef __LITTLE_ENDIAN
	u8 antidx_c:4;
	u8 antidx_d:4;
#else
	u8 antidx_d:4;
	u8 antidx_c:4;
#endif
	u8 signal_quality;
#ifdef __LITTLE_ENDIAN
	u8 vga_a:5;
	u8 lna_l_a:3;
#else
	u8 lna_l_a:3;
	u8 vga_a:5;
#endif
#ifdef __LITTLE_ENDIAN
	u8 bb_power_a:6;
	u8 rsvd_3_1:1;
	u8 lna_h_a:1;
#else

	u8 lna_h_a:1;
	u8 rsvd_3_1:1;
	u8 bb_power_a:6;
#endif
#ifdef __LITTLE_ENDIAN
	u8 rxrate:2;
	u8 raterr:1;
	u8 lockbit:1;
	u8 sqloss:1;
	u8 mf_off:1;
	u8 rsvd_3_2:2;
#else
	u8 rsvd_3_2:2;
	u8 mf_off:1;
	u8 sqloss:1;
	u8 lockbit:1;
	u8 raterr:1;
	u8 rxrate:2;
#endif
	s8 pwdb_b;
#ifdef __LITTLE_ENDIAN
	u8 vga_b:5;
	u8 lna_l_b:3;
#else
	u8 lna_l_b:3;
	u8 vga_b:5;
#endif
#ifdef __LITTLE_ENDIAN
	u8 bb_power_b:6;
	u8 rsvd_4_1:1;
	u8 lna_h_b:1;
#else
	u8 lna_h_b:1;
	u8 rsvd_4_1:1;
	u8 bb_power_b:6;
#endif
#ifdef __LITTLE_ENDIAN
	u8 gain_b:6;
	u8 rsvd_4_2:2;
#else
	u8 rsvd_4_2:2;
	u8 gain_b:6;
#endif
	s8 pwdb_c;
#ifdef __LITTLE_ENDIAN
	u8 vga_c:5;
	u8 lna_l_c:3;
#else
	u8 lna_l_c:3;
	u8 vga_c:5;
#endif
#ifdef __LITTLE_ENDIAN
	u8 bb_power_c:6;
	u8 rsvd_5_1:1;
	u8 lna_h_c:1;
#else
	u8 lna_h_c:1;
	u8 rsvd_5_1:1;
	u8 bb_power_c:6;
#endif
#ifdef __LITTLE_ENDIAN
	u8 gain_c:6;
	u8 rsvd_5_2:2;
#else
	u8 rsvd_5_2:2;
	u8 gain_c:6;
#endif
	s8 pwdb_d;
#ifdef __LITTLE_ENDIAN
	u8 vga_d:5;
	u8 lna_l_d:3;
#else
	u8 lna_l_d:3;
	u8 vga_d:5;
#endif
#ifdef __LITTLE_ENDIAN
	u8 bb_power_d:6;
	u8 rsvd_6_1:1;
	u8 lna_h_d:1;
#else
	u8 lna_h_d:1;
	u8 rsvd_6_1:1;
	u8 bb_power_d:6;
#endif
#ifdef __LITTLE_ENDIAN
	u8 gain_d:6;
	u8 rsvd_6_2:2;
#else
	u8 rsvd_6_2:2;
	u8 gain_d:6;
#endif
};

struct phy_status_jaguar3_page1 {
/* @DW0:Offset 0 */
#ifdef __LITTLE_ENDIAN
	u8 page_num:4;
	u8 pkt_cnt:2;
	u8 channel_pri_msb:2;
#else
	u8 channel_pri_msb:2;
	u8 pkt_cnt:2;
	u8 page_num:4;
#endif
	s8 pwdb_a;
	s8 pwdb_b;
	s8 pwdb_c;
	s8 pwdb_d;
#ifdef __LITTLE_ENDIAN
	u8 l_rxsc:4;
	u8 ht_rxsc:4;
#else
	u8 ht_rxsc:4;
	u8 l_rxsc:4;
#endif
	u8 channel_pri_lsb;
#ifdef __LITTLE_ENDIAN
	u8 band:2;
	u8 rsvd_0:2;
	u8 gnt_bt:1;
	u8 ldpc:1;
	u8 stbc:1;
	u8 beamformed:1;
#else
	u8 beamformed:1;
	u8 stbc:1;
	u8 ldpc:1;
	u8 gnt_bt:1;
	u8 rsvd_0:2;
	u8 band:2;
#endif
	u8 channel_sec_lsb;
#ifdef __LITTLE_ENDIAN
	u8 channel_sec_msb:2;
	u8 rsvd_1:2;
	u8 hw_antsw_occur_a:1;
	u8 hw_antsw_occur_b:1;
	u8 hw_antsw_occur_c:1;
	u8 hw_antsw_occur_d:1;
#else
	u8 hw_antsw_occur_d:1;
	u8 hw_antsw_occur_c:1;
	u8 hw_antsw_occur_b:1;
	u8 hw_antsw_occur_a:1;
	u8 rsvd_1:2;
	u8 channel_sec_msb:2;

#endif
#ifdef __LITTLE_ENDIAN
	u8 antidx_a:4;
	u8 antidx_b:4;
#else
	u8 antidx_b:4;
	u8 antidx_a:4;
#endif
#ifdef __LITTLE_ENDIAN
	u8 antidx_c:4;
	u8 antidx_d:4;
#else
	u8 antidx_d:4;
	u8 antidx_c:4;
#endif
	u8 paid;
#ifdef __LITTLE_ENDIAN
	u8 paid_msb:1;
	u8 gid:6;
	u8 rsvd_3:1;
#else
	u8 rsvd_3:1;
	u8 gid:6;
	u8 paid_msb:1;
#endif
	u16 rsvd_4;
	s8 rxevm[4];
	s8 cfo_tail[4];
	s8 rxsnr[4];
};

#define REG_DFIRBW	0x810
#define REG_ANTMAP0	0x820
#define REG_ANTMAP	0x824
#define REG_DYMPRITH	0x86c
#define REG_DYMENTH0	0x870
#define REG_DYMENTH	0x874
#define REG_DYMTHMIN	0x8a4
#define REG_TXBWCTL	0x9b0
#define REG_TXCLK	0x9b4
#define REG_SCOTRK	0xc30
#define REG_MRCM	0xc38
#define REG_AGCSWSH	0xc44
#define REG_ANTWTPD	0xc54
#define REG_ORITXCODE	0x1800
#define REG_3WIRE	0x180c
#define REG_RXAGCCTL0	0x18ac
#define REG_CCKSB	0x1a00
#define REG_RXCCKSEL	0x1a04
#define REG_PSFGC2	0x1a24
#define REG_PSFGC6	0x1a28
#define REG_CCANRX	0x1a2c
#define REG_CCKTXONLY	0x1a80
#define REG_PSFGC	0x1aac
#define REG_PCCAWT	0x1ac0
#define REG_TXANT	0x1c28
#define REG_ENCCK	0x1c3c
#define REG_CCAMSK	0x1c80
#define REG_RXFNCTL	0x1d30
#define REG_RXIGI	0x1d70
#define REG_ENFN	0x1e24
#define REG_TXANTSEG	0x1e28
#define REG_TXLGMAP	0x1e2c
#define REG_CCKPATH	0x1e5c
#define REG_ORITXCODE2	0x4100
#define REG_3WIRE2	0x410c
#define REG_RXAGCCTL	0x41ac

#endif
