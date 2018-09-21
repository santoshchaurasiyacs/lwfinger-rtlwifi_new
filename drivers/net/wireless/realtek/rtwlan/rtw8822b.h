/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 2018  Realtek Corporation.
 */

#ifndef __RTW8822B_H__
#define __RTW8822B_H__

#include <asm/byteorder.h>

#define RCR_VHT_ACK		BIT(26)

struct rtw8822bu_efuse {
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

struct rtw8822be_efuse {
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

struct rtw8822b_efuse {
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
		struct rtw8822bu_efuse u;
		struct rtw8822be_efuse e;
	};
};

struct phy_status_jaguar2_page0 {
	u8 page_num;
	u8 pwdb;
#ifdef __LITTLE_ENDIAN
	u8 gain:6;
	u8 rsvd_0:1;
	u8 trsw:1;
#else
	u8 trsw:1;
	u8 rsvd_0:1;
	u8 gain:6;
#endif
	u8 rsvd_1;
	u8 rsvd_2;
#ifdef __LITTLE_ENDIAN
	u8 rxsc:4;
	u8 agc_table:4;
#else
	u8 agc_table:4;
	u8 rxsc:4;
#endif
	u8 channel;
	u8 band;
	u16 length;
#ifdef __LITTLE_ENDIAN
	u8 antidx_a:3;
	u8 antidx_b:3;
	u8 rsvd_3:2;
	u8 antidx_c:3;
	u8 antidx_d:3;
	u8 rsvd_4:2;
#else
	u8 rsvd_3:2;
	u8 antidx_b:3;
	u8 antidx_a:3;
	u8 rsvd_4:2;
	u8 antidx_d:3;
	u8 antidx_c:3;
#endif
	u8 signal_quality;
#ifdef __LITTLE_ENDIAN
	u8 vga:5;
	u8 lna_l:3;
	u8 bb_power:6;
	u8 rsvd_9:1;
	u8 lna_h:1;
#else
	u8 lna_l:3;
	u8 vga:5;
	u8 lna_h:1;
	u8 rsvd_9:1;
	u8 bb_power:6;
#endif
	u8 rsvd_5;
	u32 rsvd_6;
	u32 rsvd_7;
	u32 rsvd_8;
};

struct phy_status_jaguar2_page1 {
	u8 page_num;
	u8 pwdb[4];
#ifdef __LITTLE_ENDIAN
	u8 l_rxsc:4;
	u8 ht_rxsc:4;
#else
	u8 ht_rxsc:4;
	u8 l_rxsc:4;
#endif
	u8 channel;
#ifdef __LITTLE_ENDIAN
	u8 band:2;
	u8 rsvd_0:1;
	u8 hw_antsw_occu:1;
	u8 gnt_bt:1;
	u8 ldpc:1;
	u8 stbc:1;
	u8 beamformed:1;
#else
	u8 beamformed:1;
	u8 stbc:1;
	u8 ldpc:1;
	u8 gnt_bt:1;
	u8 hw_antsw_occu:1;
	u8 rsvd_0:1;
	u8 band:2;
#endif
	u16 lsig_length;
#ifdef __LITTLE_ENDIAN
	u8 antidx_a:3;
	u8 antidx_b:3;
	u8 rsvd_1:2;
	u8 antidx_c:3;
	u8 antidx_d:3;
	u8 rsvd_2:2;
#else
	u8 rsvd_1:2;
	u8 antidx_b:3;
	u8 antidx_a:3;
	u8 rsvd_2:2;
	u8 antidx_d:3;
	u8 antidx_c:3;
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
	u8 intf_pos;
#ifdef __LITTLE_ENDIAN
	u8 intf_pos_msb:1;
	u8 rsvd_4:2;
	u8 nb_intf_flag:1;
	u8 rf_mode:2;
	u8 rsvd_5:2;
#else
	u8 rsvd_5:2;
	u8 rf_mode:2;
	u8 nb_intf_flag:1;
	u8 rsvd_4:2;
	u8 intf_pos_msb:1;
#endif
	s8 rxevm[4]; /* s(8,1) */
	s8 cfo_tail[4]; /* s(8,7) */
	s8 rxsnr[4]; /* s(8,1) */
};

#define REG_HTSTFWT	0x800
#define REG_RXPSEL	0x808
#define REG_TXPSEL	0x80c
#define REG_RXCCAMSK	0x814
#define REG_CCASEL	0x82c
#define REG_PDMFTH	0x830
#define REG_CCA2ND	0x838
#define REG_L1WT	0x83c
#define REG_L1PKWT	0x840
#define REG_MRC		0x850
#define REG_CLKTRK	0x860
#define REG_ADCCLK	0x8ac
#define REG_ADC160	0x8c4
#define REG_ADC40	0x8c8
#define REG_CDDTXP	0x93c
#define REG_TXPSEL1	0x940
#define REG_ACBB0	0x948
#define REG_ACBBRXFIR	0x94c
#define REG_ACGG2TBL	0x958
#define REG_RXSB	0xa00
#define REG_ADCINI	0xa04
#define REG_TXSF2	0xa24
#define REG_TXSF6	0xa28
#define REG_RXDESC	0xa2c
#define REG_ENTXCCK	0xa80
#define REG_AGCTR_A	0xc08
#define REG_TXDFIR	0xc20
#define REG_RXIGI_A	0xc50
#define REG_TRSW	0xca0
#define REG_RFESEL0	0xcb0
#define REG_RFESEL8	0xcb4
#define REG_RFECTL	0xcb8
#define REG_RFEINV	0xcbc
#define REG_AGCTR_B	0xe08
#define REG_RXIGI_B	0xe50
#define REG_ANTWT	0x1904
#define REG_IQKFAILMSK	0x1bf0

#endif
