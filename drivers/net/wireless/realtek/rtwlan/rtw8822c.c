// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2018  Realtek Corporation.
 */

#include "main.h"
#include "fw.h"
#include "tx.h"
#include "rx.h"
#include "phy.h"
#include "rtw8822c.h"
#include "rtw8822c_table.h"
#include "mac.h"
#include "reg.h"
#include "debug.h"

static void rtw8822c_config_trx_mode(struct rtw_dev *rtwdev, u8 tx_path,
				     u8 rx_path, bool is_tx2_path);

static u8 temp_addr[ETH_ALEN] = {0x00, 0xe0, 0x4c, 0x88, 0x22, 0xce};

static void rtw8822ce_efuse_parsing(struct rtw_efuse *efuse,
				    struct rtw8822c_efuse *map)
{
	ether_addr_copy(efuse->addr, temp_addr);
}

static int rtw8822c_read_efuse(struct rtw_dev *rtwdev, u8 *log_map)
{
	struct rtw_efuse *efuse = &rtwdev->efuse;
	struct rtw8822c_efuse *map;
	int i;

	map = (struct rtw8822c_efuse *)log_map;

	efuse->rfe_option = map->rfe_option;
	efuse->crystal_cap = map->xtal_k;
	efuse->pa_type_2g = map->pa_type;
	efuse->pa_type_5g = map->pa_type;
	efuse->lna_type_2g = map->lna_type_2g[0];
	efuse->lna_type_5g = map->lna_type_5g[0];
	efuse->channel_plan = map->channel_plan;
	efuse->bt_setting = map->rf_bt_setting;
	efuse->regd = map->rf_board_option & 0x7;

	for (i = 0; i < 4; i++)
		efuse->txpwr_idx_table[i] = map->txpwr_idx_table[i];

	switch (rtw_hci_type(rtwdev)) {
	case RTW_HCI_TYPE_PCIE:
		rtw8822ce_efuse_parsing(efuse, map);
		break;
	default:
		/* unsupported now */
		return -ENOTSUPP;
	}

	return 0;
}

static void rtw8822c_header_file_init(struct rtw_dev *rtwdev, bool pre)
{
	rtw_write32_mask(rtwdev, REG_3WIRE, 0x3, 0x3);
	rtw_write32_mask(rtwdev, REG_3WIRE, BIT(28), 0x1);
	rtw_write32_mask(rtwdev, REG_3WIRE2, 0x3, 0x3);
	rtw_write32_mask(rtwdev, REG_3WIRE2, BIT(28), 0x1);

	if (pre)
		rtw_write32_mask(rtwdev, REG_ENCCK, (BIT(0) | BIT(1)), 0x0);
	else
		rtw_write32_mask(rtwdev, REG_ENCCK, (BIT(0) | BIT(1)), 0x3);
}

static void rtw8822c_phy_set_param(struct rtw_dev *rtwdev)
{
	struct rtw_dm_info *dm_info = &rtwdev->dm_info;
	struct rtw_hal *hal = &rtwdev->hal;
	u8 crystal_cap;
	u8 cck_gi_u_bnd_msb = 0;
	u8 cck_gi_u_bnd_lsb = 0;
	u8 cck_gi_l_bnd_msb = 0;
	u8 cck_gi_l_bnd_lsb = 0;
	bool is_tx2_path;

	/* power on BB/RF domain */
	rtw_write8_set(rtwdev, REG_SYS_FUNC_EN, BIT(0) | BIT(1));
	rtw_write8_set(rtwdev, REG_RF_CTRL, BIT(0) | BIT(1) | BIT(2));
	rtw_write32_set(rtwdev, REG_WLRF1, BIT(24) | BIT(25) | BIT(26));

	/* pre init before header files config */
	rtw8822c_header_file_init(rtwdev, true);

	rtw_phy_load_tables(rtwdev);

	crystal_cap = rtwdev->efuse.crystal_cap & 0x3F;
	rtw_write32_mask(rtwdev, 0x24, 0x7e000000, crystal_cap);
	rtw_write32_mask(rtwdev, 0x28, 0x7e, crystal_cap);

	/* post init after header files config */
	rtw8822c_header_file_init(rtwdev, false);

	is_tx2_path = false;
	rtw8822c_config_trx_mode(rtwdev, hal->antenna_tx, hal->antenna_rx,
				 is_tx2_path);
	rtw_phy_init(rtwdev);

	cck_gi_u_bnd_msb = (u8)rtw_read32_mask(rtwdev, 0x1a98, 0xc000);
	cck_gi_u_bnd_lsb = (u8)rtw_read32_mask(rtwdev, 0x1aa8, 0xf0000);
	cck_gi_l_bnd_msb = (u8)rtw_read32_mask(rtwdev, 0x1a98, 0xc0);
	cck_gi_l_bnd_lsb = (u8)rtw_read32_mask(rtwdev, 0x1a70, 0x0f000000);

	dm_info->cck_gi_u_bnd = ((cck_gi_u_bnd_msb << 4) | (cck_gi_u_bnd_lsb));
	dm_info->cck_gi_l_bnd = ((cck_gi_l_bnd_msb << 4) | (cck_gi_l_bnd_lsb));

	/* wifi path controller */
	rtw_write32_mask(rtwdev, 0x70, 0xff000000, 0x0e);
	rtw_write32_mask(rtwdev, 0x1704, 0xffffffff, 0x7700);
	rtw_write32_mask(rtwdev, 0x1700, 0xffffffff, 0xc00f0038);
	rtw_write32_mask(rtwdev, 0x6c0, 0xffffffff, 0xaaaaaaaa);
	rtw_write32_mask(rtwdev, 0x6c4, 0xffffffff, 0xaaaaaaaa);
}

#define WLAN_TXQ_RPT_EN		0x1F
#define WLAN_SLOT_TIME		0x05
#define WLAN_PIFS_TIME		0x1C
#define WLAN_SIFS_CCK_CONT_TX	0x0A
#define WLAN_SIFS_OFDM_CONT_TX	0x10
#define WLAN_SIFS_CCK_TRX	0x0A
#define WLAN_SIFS_OFDM_TRX	0x10
#define WLAN_NAV_MAX		0xC8
#define WLAN_RDG_NAV		0x05
#define WLAN_TXOP_NAV		0x1B
#define WLAN_CCK_RX_TSF		0x30
#define WLAN_OFDM_RX_TSF	0x30
#define WLAN_TBTT_PROHIBIT	0x04 /* unit : 32us */
#define WLAN_TBTT_HOLD_TIME	0x064 /* unit : 32us */
#define WLAN_DRV_EARLY_INT	0x04
#define WLAN_BCN_CTRL_CLT0	0x10
#define WLAN_BCN_DMA_TIME	0x02
#define WLAN_BCN_MAX_ERR	0xFF
#define WLAN_SIFS_CCK_DUR_TUNE	0x0A
#define WLAN_SIFS_OFDM_DUR_TUNE	0x10
#define WLAN_SIFS_CCK_CTX	0x0A
#define WLAN_SIFS_CCK_IRX	0x0A
#define WLAN_SIFS_OFDM_CTX	0x0E
#define WLAN_SIFS_OFDM_IRX	0x0E
#define WLAN_EIFS_DUR_TUNE	0x40
#define WLAN_EDCA_VO_PARAM	0x002FA226
#define WLAN_EDCA_VI_PARAM	0x005EA328
#define WLAN_EDCA_BE_PARAM	0x005EA42B
#define WLAN_EDCA_BK_PARAM	0x0000A44F

#define WLAN_RX_FILTER0		0xFFFFFFFF
#define WLAN_RX_FILTER2		0xFFFF
#define WLAN_RCR_CFG		0xE400220E
#define WLAN_RXPKT_MAX_SZ	12288
#define WLAN_RXPKT_MAX_SZ_512	(WLAN_RXPKT_MAX_SZ >> 9)

#define WLAN_AMPDU_MAX_TIME		0x70
#define WLAN_RTS_LEN_TH			0xFF
#define WLAN_RTS_TX_TIME_TH		0x08
#define WLAN_MAX_AGG_PKT_LIMIT		0x20
#define WLAN_RTS_MAX_AGG_PKT_LIMIT	0x20
#define WLAN_PRE_TXCNT_TIME_TH		0x1E0
#define FAST_EDCA_VO_TH		0x06
#define FAST_EDCA_VI_TH		0x06
#define FAST_EDCA_BE_TH		0x06
#define FAST_EDCA_BK_TH		0x06
#define WLAN_BAR_RETRY_LIMIT		0x01
#define WLAN_BAR_ACK_TYPE		0x05
#define WLAN_RA_TRY_RATE_AGG_LIMIT	0x08
#define WLAN_RESP_TXRATE		0x84
#define WLAN_ACK_TO			0x40
#define WLAN_DATA_RATE_FB_CNT_1_4	0x01000000
#define WLAN_DATA_RATE_FB_CNT_5_8	0x08070504
#define WLAN_RTS_RATE_FB_CNT_5_8	0x08070504
#define WLAN_DATA_RATE_FB_RATE0		0xFE01F010
#define WLAN_DATA_RATE_FB_RATE0_H	0x40000000
#define WLAN_RTS_RATE_FB_RATE1		0x003FF010
#define WLAN_RTS_RATE_FB_RATE1_H	0x40000000
#define WLAN_RTS_RATE_FB_RATE4		0x0600F010
#define WLAN_RTS_RATE_FB_RATE4_H	0x400003E0
#define WLAN_RTS_RATE_FB_RATE5		0x0600F015
#define WLAN_RTS_RATE_FB_RATE5_H	0x000000E0

#define WLAN_TX_FUNC_CFG1		0x30
#define WLAN_TX_FUNC_CFG2		0x30
#define WLAN_MAC_OPT_NORM_FUNC1		0x90
#define WLAN_MAC_OPT_LB_FUNC1		0x80
#define WLAN_MAC_OPT_FUNC2		0x30810041

#define WLAN_SIFS_CFG	(WLAN_SIFS_CCK_CONT_TX | \
			(WLAN_SIFS_OFDM_CONT_TX << BIT_SHIFT_SIFS_OFDM_CTX) | \
			(WLAN_SIFS_CCK_TRX << BIT_SHIFT_SIFS_CCK_TRX) | \
			(WLAN_SIFS_OFDM_TRX << BIT_SHIFT_SIFS_OFDM_TRX))

#define WLAN_SIFS_DUR_TUNE	(WLAN_SIFS_CCK_DUR_TUNE | \
				(WLAN_SIFS_OFDM_DUR_TUNE << 8))

#define WLAN_TBTT_TIME	(WLAN_TBTT_PROHIBIT |\
			(WLAN_TBTT_HOLD_TIME << BIT_SHIFT_TBTT_HOLD_TIME_AP))

#define WLAN_NAV_CFG		(WLAN_RDG_NAV | (WLAN_TXOP_NAV << 16))
#define WLAN_RX_TSF_CFG		(WLAN_CCK_RX_TSF | (WLAN_OFDM_RX_TSF) << 8)

#define MAC_CLK_SPEED	80 /* 80M */
#define EFUSE_PCB_INFO_OFFSET	0xCA

static int rtw8822c_mac_init(struct rtw_dev *rtwdev)
{
	u8 value8;
	u16 value16;
	u32 value32;
	u16 pre_txcnt;

	/* txq control */
	value8 = rtw_read8(rtwdev, REG_FWHW_TXQ_CTRL);
	value8 |= (BIT(7) & ~BIT(1) & ~BIT(2));
	rtw_write8(rtwdev, REG_FWHW_TXQ_CTRL, value8);
	rtw_write8(rtwdev, REG_FWHW_TXQ_CTRL + 1, WLAN_TXQ_RPT_EN);
	/* sifs control */
	rtw_write16(rtwdev, REG_SPEC_SIFS, WLAN_SIFS_DUR_TUNE);
	rtw_write32(rtwdev, REG_SIFS, WLAN_SIFS_CFG);
	rtw_write16(rtwdev, REG_RESP_SIFS_CCK,
		    WLAN_SIFS_CCK_CTX | WLAN_SIFS_CCK_IRX << 8);
	rtw_write16(rtwdev, REG_RESP_SIFS_OFDM,
		    WLAN_SIFS_OFDM_CTX | WLAN_SIFS_OFDM_IRX << 8);
	/* rate fallback control */
	rtw_write32(rtwdev, REG_DARFRC, WLAN_DATA_RATE_FB_CNT_1_4);
	rtw_write32(rtwdev, REG_DARFRCH, WLAN_DATA_RATE_FB_CNT_5_8);
	rtw_write32(rtwdev, REG_RARFRCH, WLAN_RTS_RATE_FB_CNT_5_8);
	rtw_write32(rtwdev, REG_ARFR0, WLAN_DATA_RATE_FB_RATE0);
	rtw_write32(rtwdev, REG_ARFRH0, WLAN_DATA_RATE_FB_RATE0_H);
	rtw_write32(rtwdev, REG_ARFR1_V1, WLAN_RTS_RATE_FB_RATE1);
	rtw_write32(rtwdev, REG_ARFRH1_V1, WLAN_RTS_RATE_FB_RATE1_H);
	rtw_write32(rtwdev, REG_ARFR4, WLAN_RTS_RATE_FB_RATE4);
	rtw_write32(rtwdev, REG_ARFRH4, WLAN_RTS_RATE_FB_RATE4_H);
	rtw_write32(rtwdev, REG_ARFR5, WLAN_RTS_RATE_FB_RATE5);
	rtw_write32(rtwdev, REG_ARFRH5, WLAN_RTS_RATE_FB_RATE5_H);
	/* protocol configuration */
	rtw_write8(rtwdev, REG_AMPDU_MAX_TIME_V1, WLAN_AMPDU_MAX_TIME);
	rtw_write8_set(rtwdev, REG_TX_HANG_CTRL, BIT_EN_EOF_V1);
	pre_txcnt = WLAN_PRE_TXCNT_TIME_TH | BIT_EN_PRECNT;
	rtw_write8(rtwdev, REG_PRECNT_CTRL, (u8)(pre_txcnt & 0xFF));
	rtw_write8(rtwdev, REG_PRECNT_CTRL + 1, (u8)(pre_txcnt >> 8));
	value32 = WLAN_RTS_LEN_TH | (WLAN_RTS_TX_TIME_TH << 8) |
		  (WLAN_MAX_AGG_PKT_LIMIT << 16) |
		  (WLAN_RTS_MAX_AGG_PKT_LIMIT << 24);
	rtw_write32(rtwdev, REG_PROT_MODE_CTRL, value32);
	rtw_write16(rtwdev, REG_BAR_MODE_CTRL + 2,
		    WLAN_BAR_RETRY_LIMIT | WLAN_RA_TRY_RATE_AGG_LIMIT << 8);
	rtw_write8(rtwdev, REG_FAST_EDCA_VOVI_SETTING, FAST_EDCA_VO_TH);
	rtw_write8(rtwdev, REG_FAST_EDCA_VOVI_SETTING + 2, FAST_EDCA_VI_TH);
	rtw_write8(rtwdev, REG_FAST_EDCA_BEBK_SETTING, FAST_EDCA_BE_TH);
	rtw_write8(rtwdev, REG_FAST_EDCA_BEBK_SETTING + 2, FAST_EDCA_BK_TH);
	/* close BA parser */
	rtw_write8_clr(rtwdev, REG_LIFETIME_EN, BIT(5));

	/* EDCA configuration */
	rtw_write32(rtwdev, REG_EDCA_VO_PARAM, WLAN_EDCA_VO_PARAM);
	rtw_write32(rtwdev, REG_EDCA_VI_PARAM, WLAN_EDCA_VI_PARAM);
	rtw_write32(rtwdev, REG_EDCA_BE_PARAM, WLAN_EDCA_BE_PARAM);
	rtw_write32(rtwdev, REG_EDCA_BK_PARAM, WLAN_EDCA_BK_PARAM);
	rtw_write8(rtwdev, REG_PIFS, WLAN_PIFS_TIME);
	rtw_write8_clr(rtwdev, REG_TX_PTCL_CTRL + 1, BIT(4));
	rtw_write8_set(rtwdev, REG_RD_CTRL + 1, BIT(0) | BIT(1) | BIT(2));

	/* MAC clock configuration */
	rtw_write32_clr(rtwdev, REG_AFE_CTRL1, BIT(20) | BIT(21));
	rtw_write8(rtwdev, REG_USTIME_TSF, MAC_CLK_SPEED);
	rtw_write8(rtwdev, REG_USTIME_EDCA, MAC_CLK_SPEED);

	rtw_write8_set(rtwdev, REG_MISC_CTRL, BIT(3) | BIT(1) | BIT(0));
	rtw_write8_clr(rtwdev, REG_TIMER0_SRC_SEL, BIT(4) | BIT(5) | BIT(6));
	rtw_write16(rtwdev, REG_TXPAUSE, 0x0000);
	rtw_write8(rtwdev, REG_SLOT, WLAN_SLOT_TIME);
	rtw_write32(rtwdev, REG_RD_NAV_NXT, WLAN_NAV_CFG);
	rtw_write16(rtwdev, REG_RXTSF_OFFSET_CCK, WLAN_RX_TSF_CFG);
	/* Set beacon cotnrol - enable TSF and other related functions */
	rtw_write8_set(rtwdev, REG_BCN_CTRL, BIT_EN_BCN_FUNCTION);
	/* Set send beacon related registers */
	rtw_write32(rtwdev, REG_TBTT_PROHIBIT, WLAN_TBTT_TIME);
	rtw_write8(rtwdev, REG_DRVERLYINT, WLAN_DRV_EARLY_INT);
	rtw_write8(rtwdev, REG_BCN_CTRL_CLINT0, WLAN_BCN_CTRL_CLT0);
	rtw_write8(rtwdev, REG_BCNDMATIM, WLAN_BCN_DMA_TIME);
	rtw_write8(rtwdev, REG_BCN_MAX_ERR, WLAN_BCN_MAX_ERR);

	/* WMAC configuration */
	rtw_write8(rtwdev, REG_BBPSF_CTRL + 2, WLAN_RESP_TXRATE);
	rtw_write8(rtwdev, REG_ACKTO, WLAN_ACK_TO);
	rtw_write16(rtwdev, REG_EIFS, WLAN_EIFS_DUR_TUNE);
	rtw_write8(rtwdev, REG_NAV_CTRL + 2, WLAN_NAV_MAX);
	rtw_write8(rtwdev, REG_WMAC_TRXPTCL_CTL_H  + 2, WLAN_BAR_ACK_TYPE);
	rtw_write32(rtwdev, REG_RXFLTMAP0, WLAN_RX_FILTER0);
	rtw_write16(rtwdev, REG_RXFLTMAP2, WLAN_RX_FILTER2);
	rtw_write32(rtwdev, REG_RCR, WLAN_RCR_CFG);
	rtw_write8(rtwdev, REG_RX_PKT_LIMIT, WLAN_RXPKT_MAX_SZ_512);
	rtw_write8(rtwdev, REG_TCR + 2, WLAN_TX_FUNC_CFG2);
	rtw_write8(rtwdev, REG_TCR + 1, WLAN_TX_FUNC_CFG1);
	rtw_write32(rtwdev, REG_WMAC_OPTION_FUNCTION + 8, WLAN_MAC_OPT_FUNC2);
	rtw_write8(rtwdev, REG_WMAC_OPTION_FUNCTION_1, WLAN_MAC_OPT_NORM_FUNC1);

	/* init low power */
	value16 = rtw_read16(rtwdev, REG_RXPSF_CTRL + 2) & 0xF00F;
	value16 |= BIT(10) | BIT(8) | BIT(6) | BIT(4);
	rtw_write16(rtwdev, REG_RXPSF_CTRL + 2, value16);
	value16 = 0;
	value16 = BIT_SET_RXPSF_PKTLENTHR(value16, 1);
	value16 |= BIT_RXPSF_CTRLEN | BIT_RXPSF_VHTCHKEN | BIT_RXPSF_HTCHKEN
		| BIT_RXPSF_OFDMCHKEN | BIT_RXPSF_CCKCHKEN
		| BIT_RXPSF_OFDMRST | BIT_RXPSF_CCKRST;
	rtw_write16(rtwdev, REG_RXPSF_CTRL, value16);
	rtw_write32(rtwdev, REG_RXPSF_TYPE_CTRL, 0xFFFFFFFF);
	/* rx ignore configuration */
	value16 = rtw_read16(rtwdev, REG_RXPSF_CTRL);
	value16 &= ~(BIT_RXPSF_MHCHKEN | BIT_RXPSF_CCKRST |
		     BIT_RXPSF_CONT_ERRCHKEN);
	value16 = BIT_SET_RXPSF_ERRTHR(value16, 0x07);
	rtw_write16(rtwdev, REG_RXPSF_CTRL, value16);

	return 0;
}

static void rtw8822c_set_channel_rf(struct rtw_dev *rtwdev, u8 channel, u8 bw)
{
#define RF18_BAND_MASK		(BIT(16) | BIT(9) | BIT(8))
#define RF18_BAND_2G		(0)
#define RF18_BAND_5G		(BIT(16) | BIT(8))
#define RF18_CHANNEL_MASK	(MASKBYTE0)
#define RF18_RFSI_MASK		(BIT(18) | BIT(17))
#define RF18_RFSI_GE_CH80	(BIT(17))
#define RF18_RFSI_GT_CH140	(BIT(18))
#define RF18_BW_MASK		(BIT(13) | BIT(12))
#define RF18_BW_20M		(BIT(13) | BIT(12))
#define RF18_BW_40M		(BIT(13))
#define RF18_BW_80M		(BIT(12))

	u32 rf_reg18 = 0;

	rf_reg18 = rtw_read_rf(rtwdev, RF_PATH_A, 0x18, RFREG_MASK);

	rf_reg18 &= ~(RF18_BAND_MASK | RF18_CHANNEL_MASK | RF18_RFSI_MASK |
		      RF18_BW_MASK);

	rf_reg18 |= (channel <= 14 ? RF18_BAND_2G : RF18_BAND_5G);
	rf_reg18 |= (channel & RF18_CHANNEL_MASK);
	if (channel > 140)
		rf_reg18 |= RF18_RFSI_GT_CH140;
	else if (channel >= 80)
		rf_reg18 |= RF18_RFSI_GE_CH80;

	switch (bw) {
	case RTW_CHANNEL_WIDTH_5:
	case RTW_CHANNEL_WIDTH_10:
	case RTW_CHANNEL_WIDTH_20:
	default:
		rf_reg18 |= RF18_BW_20M;
		break;
	case RTW_CHANNEL_WIDTH_40:
		/* RF bandwidth */
		rf_reg18 |= RF18_BW_40M;
		break;
	case RTW_CHANNEL_WIDTH_80:
		rf_reg18 |= RF18_BW_80M;
		break;
	}

	rtw_write_rf(rtwdev, RF_PATH_A, RF_CFGCH, RFREG_MASK, rf_reg18);
	rtw_write_rf(rtwdev, RF_PATH_B, RF_CFGCH, RFREG_MASK, rf_reg18);
}

static void rtw8822c_toggle_igi(struct rtw_dev *rtwdev)
{
	u32 igi;

	igi = rtw_read32_mask(rtwdev, REG_RXIGI, 0x7f);
	rtw_write32_mask(rtwdev, REG_RXIGI, 0x7f, igi - 2);
	rtw_write32_mask(rtwdev, REG_RXIGI, 0x7f00, igi - 2);
	rtw_write32_mask(rtwdev, REG_RXIGI, 0x7f, igi);
	rtw_write32_mask(rtwdev, REG_RXIGI, 0x7f00, igi);
}

static void rtw8822c_set_channel_bb(struct rtw_dev *rtwdev, u8 channel, u8 bw,
				    u8 primary_ch_idx)
{
	if (channel <= 14) {
		rtw_write32_mask(rtwdev, REG_ENCCK, BIT(1), 0x1);
		rtw_write32_mask(rtwdev, REG_CCK_CHECK, BIT(7), 0x0);
		rtw_write32_mask(rtwdev, REG_CCKTXONLY, BIT(18), 0x0);
		rtw_write32_mask(rtwdev, REG_CCAMSK, 0x3F000000, 0xF);

		rtw_write32_mask(rtwdev, REG_RXAGCCTL0, 0x1f0, 0x0);
		rtw_write32_mask(rtwdev, REG_RXAGCCTL, 0x1f0, 0x0);
		if (channel == 13 || channel == 14)
			rtw_write32_mask(rtwdev, REG_SCOTRK, 0xfff, 0x969);
		else if (channel == 11 || channel == 12)
			rtw_write32_mask(rtwdev, REG_SCOTRK, 0xfff, 0x96a);
		else
			rtw_write32_mask(rtwdev, REG_SCOTRK, 0xfff, 0x9aa);

		if (channel == 14) {
			rtw_write32_mask(rtwdev, REG_PSFGC2, MASKDWORD, 0x0000b81c);
			rtw_write32_mask(rtwdev, REG_PSFGC6, MASKLWORD, 0x0000);
			rtw_write32_mask(rtwdev, REG_PSFGC, MASKDWORD, 0x00003667);
		} else {
			rtw_write32_mask(rtwdev, REG_PSFGC2, MASKDWORD, 0x64b80c1c);
			rtw_write32_mask(rtwdev, REG_PSFGC6, MASKLWORD, 0x8810);
			rtw_write32_mask(rtwdev, REG_PSFGC, MASKDWORD, 0x01235667);
		}
	} else if (channel > 35) {
		rtw_write32_mask(rtwdev, REG_CCKTXONLY, BIT(18), 0x1);
		rtw_write32_mask(rtwdev, REG_CCK_CHECK, BIT(7), 0x1);
		rtw_write32_mask(rtwdev, REG_ENCCK, BIT(1), 0x0);
		rtw_write32_mask(rtwdev, REG_CCAMSK, 0x3F000000, 0x22);

		if (channel >= 36 && channel <= 64) {
			rtw_write32_mask(rtwdev, REG_RXAGCCTL0, 0x1f0, 0x1);
			rtw_write32_mask(rtwdev, REG_RXAGCCTL, 0x1f0, 0x1);
		} else if (channel >= 100 && channel <= 144) {
			rtw_write32_mask(rtwdev, REG_RXAGCCTL0, 0x1f0, 0x2);
			rtw_write32_mask(rtwdev, REG_RXAGCCTL, 0x1f0, 0x2);
		} else if (channel >= 149) {
			rtw_write32_mask(rtwdev, REG_RXAGCCTL0, 0x1f0, 0x3);
			rtw_write32_mask(rtwdev, REG_RXAGCCTL, 0x1f0, 0x3);
		}

		if (channel >= 36 && channel <= 51)
			rtw_write32_mask(rtwdev, REG_SCOTRK, 0xfff, 0x494);
		else if (channel >= 52 && channel <= 55)
			rtw_write32_mask(rtwdev, REG_SCOTRK, 0xfff, 0x493);
		else if (channel >= 56 && channel <= 111)
			rtw_write32_mask(rtwdev, REG_SCOTRK, 0xfff, 0x453);
		else if (channel >= 112 && channel <= 119)
			rtw_write32_mask(rtwdev, REG_SCOTRK, 0xfff, 0x452);
		else if (channel >= 120 && channel <= 172)
			rtw_write32_mask(rtwdev, REG_SCOTRK, 0xfff, 0x412);
		else if (channel >= 173 && channel <= 177)
			rtw_write32_mask(rtwdev, REG_SCOTRK, 0xfff, 0x411);
	}

	switch (bw) {
	case RTW_CHANNEL_WIDTH_20:
		rtw_write32_mask(rtwdev, REG_DFIRBW, 0x3FF0, 0x19B);
		rtw_write32_mask(rtwdev, REG_TXBWCTL, 0xf, 0x0);
		rtw_write32_mask(rtwdev, REG_TXBWCTL, 0xc0, 0x0);
		rtw_write32_mask(rtwdev, REG_TXBWCTL, 0xff00,
				 (primary_ch_idx | (primary_ch_idx << 4)));
		rtw_write32_mask(rtwdev, REG_TXCLK, MASKH3BYTES, 0xdb6db6);
		rtw_write32_mask(rtwdev, REG_DYMPRITH, 0xffffc000, 0x1F8A3);
		rtw_write32_mask(rtwdev, REG_DYMPRITH, 0x1, 0x0);
		rtw_write32_mask(rtwdev, REG_DYMTHMIN, 0x3f, 0x1a);
		break;
	case RTW_CHANNEL_WIDTH_40:
		rtw_write32_mask(rtwdev, REG_CCKSB, BIT(4),
				 (primary_ch_idx == 1 ? 1 : 0));
		rtw_write32_mask(rtwdev, REG_TXBWCTL, 0xf, 0x5);
		rtw_write32_mask(rtwdev, REG_TXBWCTL, 0xc0, 0x0);
		rtw_write32_mask(rtwdev, REG_TXBWCTL, 0xff00,
				 (primary_ch_idx | (primary_ch_idx << 4)));
		rtw_write32_mask(rtwdev, REG_DYMPRITH, 0xffffc000, 0x1D821);
		rtw_write32_mask(rtwdev, REG_DYMPRITH, 0x1, 0x0);
		rtw_write32_mask(rtwdev, REG_DYMTHMIN, MASK12BITS, 0x618);
		rtw_write32_mask(rtwdev, REG_DYMENTH0, 0x3ffff, 0x1D821);
		break;
	case RTW_CHANNEL_WIDTH_80:
		rtw_write32_mask(rtwdev, REG_TXBWCTL, 0xf, 0xa);
		rtw_write32_mask(rtwdev, REG_TXBWCTL, 0xc0, 0x0);
		rtw_write32_mask(rtwdev, REG_TXBWCTL, 0xff00,
				 (primary_ch_idx | (primary_ch_idx << 4)));
		rtw_write32_mask(rtwdev, REG_DYMPRITH, 0xffffc000, 0x1B79F);
		rtw_write32_mask(rtwdev, REG_DYMPRITH, 0x1, 0x0);
		rtw_write32_mask(rtwdev, REG_DYMTHMIN, 0x3ffff, 0x18596);
		rtw_write32_mask(rtwdev, REG_DYMENTH0, 0x3FFFFFFF, 0x2085D75E);
		rtw_write32_mask(rtwdev, REG_DYMENTH, 0x3f, 0x1d);
		break;
	case RTW_CHANNEL_WIDTH_5:
		rtw_write32_mask(rtwdev, REG_DFIRBW, 0x3FF0, 0x2AB);
		rtw_write32_mask(rtwdev, REG_TXBWCTL, 0xf, 0x0);
		rtw_write32_mask(rtwdev, REG_TXBWCTL, 0xffc0, 0x1);
		rtw_write32_mask(rtwdev, REG_TXCLK, MASKH3BYTES, 0xDB4DB2);
		break;
	case RTW_CHANNEL_WIDTH_10:
		rtw_write32_mask(rtwdev, REG_DFIRBW, 0x3FF0, 0x2AB);
		rtw_write32_mask(rtwdev, REG_TXBWCTL, 0xf, 0x0);
		rtw_write32_mask(rtwdev, REG_TXBWCTL, 0xffc0, 0x2);
		rtw_write32_mask(rtwdev, REG_TXCLK, MASKH3BYTES, 0xDB5DB4);
		break;
	}
}

static void rtw8822c_set_channel(struct rtw_dev *rtwdev, u8 channel, u8 bw,
				 u8 primary_chan_idx)
{
	rtw8822c_set_channel_bb(rtwdev, channel, bw, primary_chan_idx);
	rtw_set_channel_mac(rtwdev, channel, bw, primary_chan_idx);
	rtw8822c_set_channel_rf(rtwdev, channel, bw);
	rtw8822c_toggle_igi(rtwdev);
}

static void rtw8822c_config_trx_mode(struct rtw_dev *rtwdev, u8 tx_path,
				     u8 rx_path, bool is_tx2_path)
{
	if ((tx_path | rx_path) & BB_PATH_A)
		rtw_write32_mask(rtwdev, REG_ORITXCODE, MASK20BITS, 0x33312);
	else
		rtw_write32_mask(rtwdev, REG_ORITXCODE, MASK20BITS, 0x11111);
	if ((tx_path | rx_path) & BB_PATH_B)
		rtw_write32_mask(rtwdev, REG_ORITXCODE2, MASK20BITS, 0x33312);
	else
		rtw_write32_mask(rtwdev, REG_ORITXCODE2, MASK20BITS, 0x11111);

	rtw_write32_mask(rtwdev, REG_RXCCKSEL, 0xf0000000, 0x8);
	rtw_write32_mask(rtwdev, REG_CCKPATH, BIT(30), 0x1);

	if (tx_path == BB_PATH_A)
		rtw_write32_mask(rtwdev, REG_RXCCKSEL, 0xf0000000, 0x8);
	else if (tx_path == BB_PATH_B)
		rtw_write32_mask(rtwdev, REG_RXCCKSEL, 0xf0000000, 0x4);
	else if (tx_path == BB_PATH_AB)
		rtw_write32_mask(rtwdev, REG_RXCCKSEL, 0xf0000000, 0xc);

	rtw_write32_mask(rtwdev, REG_TXANTSEG, 0xf, tx_path);
	rtw_write32_mask(rtwdev, REG_ENFN, BIT(16), 0x0);
	rtw_write32_mask(rtwdev, REG_ENFN, BIT(31), 0x1);

	if (tx_path == BB_PATH_A) {
		rtw_write32_mask(rtwdev, REG_ANTMAP0, 0xf, 0x1);
		rtw_write32_mask(rtwdev, REG_TXLGMAP, 0x3, 0x0);
		rtw_write32_mask(rtwdev, REG_TXLGMAP, 0xc, 0x0);
	} else if (tx_path == BB_PATH_B) {
		rtw_write32_mask(rtwdev, REG_ANTMAP0, 0xf, 0x2);
		rtw_write32_mask(rtwdev, REG_TXLGMAP, 0x3, 0x0);
		rtw_write32_mask(rtwdev, REG_TXLGMAP, 0xc, 0x0);
	} else if (tx_path == BB_PATH_AB) {
		rtw_write32_mask(rtwdev, REG_ANTMAP0, 0xf, 0x3);
		rtw_write32_mask(rtwdev, REG_TXLGMAP, 0x3, 0x0);
		rtw_write32_mask(rtwdev, REG_TXLGMAP, 0xc, 0x1);
	}

	if (tx_path == BB_PATH_A || tx_path == BB_PATH_B) {
		rtw_write32_mask(rtwdev, REG_ANTMAP0, 0xf0, 0x1);
		rtw_write32_mask(rtwdev, REG_TXLGMAP, 0x300, 0x0);
		rtw_write32_mask(rtwdev, REG_TXLGMAP, 0xc00, 0x0);
	} else if (tx_path == BB_PATH_AB) {
		rtw_write32_mask(rtwdev, REG_ANTMAP0, 0xf0, 0x3);
		rtw_write32_mask(rtwdev, REG_TXLGMAP, 0x300, 0x0);
		rtw_write32_mask(rtwdev, REG_TXLGMAP, 0xc00, 0x1);
	}
	rtw_write32_mask(rtwdev, REG_TXANT, 0xf, tx_path);

	if (rx_path == BB_PATH_A || rx_path == BB_PATH_B) {
		rtw_write32_mask(rtwdev, REG_CCANRX, 0x00600000, 0x0);
		rtw_write32_mask(rtwdev, REG_PCCAWT, 0x80000000, 0x0);
	} else if (rx_path == BB_PATH_AB) {
		rtw_write32_mask(rtwdev, REG_CCANRX, 0x00600000, 0x1);
		rtw_write32_mask(rtwdev, REG_CCANRX, 0x00060000, 0x1);
	}

	rtw_write32_mask(rtwdev, REG_RXCCKSEL, 0x0f000000, 0x0);

	if (rx_path == BB_PATH_A)
		rtw_write32_mask(rtwdev, REG_RXCCKSEL, 0x0f000000, 0x0);
	else if (rx_path == BB_PATH_B)
		rtw_write32_mask(rtwdev, REG_RXCCKSEL, 0x0f000000, 0x5);
	else if (rx_path == BB_PATH_AB)
		rtw_write32_mask(rtwdev, REG_RXCCKSEL, 0x0f000000, 0x1);

	rtw_write32_mask(rtwdev, REG_ANTMAP, MASKBYTE3LOWNIBBLE, rx_path);
	rtw_write32_mask(rtwdev, REG_ANTMAP, 0x000F0000, rx_path);

	if (!(rx_path & BB_PATH_A))
		rtw_write_rf(rtwdev, 0, 0, 0xf0000, 0x1);
	if (!(rx_path & BB_PATH_B))
		rtw_write_rf(rtwdev, 1, 0, 0xf0000, 0x1);

	if (rx_path == BB_PATH_A || rx_path == BB_PATH_B) {
		rtw_write32_mask(rtwdev, REG_RXFNCTL, 0x300, 0x0);
		rtw_write32_mask(rtwdev, REG_RXFNCTL, 0x600000, 0x0);
		rtw_write32_mask(rtwdev, REG_AGCSWSH, BIT(17), 0x0);
		rtw_write32_mask(rtwdev, REG_ANTWTPD, BIT(20), 0x0);
		rtw_write32_mask(rtwdev, REG_MRCM, BIT(24), 0x0);
	} else if (rx_path == BB_PATH_AB) {
		rtw_write32_mask(rtwdev, REG_RXFNCTL, 0x300, 0x1);
		rtw_write32_mask(rtwdev, REG_RXFNCTL, 0x600000, 0x1);
		rtw_write32_mask(rtwdev, REG_AGCSWSH, BIT(17), 0x1);
		rtw_write32_mask(rtwdev, REG_ANTWTPD, BIT(20), 0x1);
		rtw_write32_mask(rtwdev, REG_MRCM, BIT(24), 0x1);
	}

	rtw8822c_toggle_igi(rtwdev);
}

static void query_phy_status_page0(struct rtw_dev *rtwdev, u8 *phy_status,
				   struct rtw_rx_pkt_stat *pkt_stat)
{
	struct rtw_dm_info *dm_info = &rtwdev->dm_info;
	struct phy_status_jaguar3_page0 *phy_stat;
	u8 l_bnd, u_bnd;
	s8 rx_power[RTW_RF_PATH_MAX];
	s8 min_rx_power = -120;

	phy_stat = (struct phy_status_jaguar3_page0 *)phy_status;

	rx_power[RF_PATH_A] = phy_stat->pwdb_a;
	rx_power[RF_PATH_B] = phy_stat->pwdb_b;
	l_bnd = dm_info->cck_gi_l_bnd;
	u_bnd = dm_info->cck_gi_u_bnd;
	if (phy_stat->gain_a < l_bnd)
		rx_power[RF_PATH_A] += (l_bnd - phy_stat->gain_a) << 1;
	else if (phy_stat->gain_a > u_bnd)
		rx_power[RF_PATH_A] -= (phy_stat->gain_a - u_bnd) << 1;
	if (phy_stat->gain_b < l_bnd)
		rx_power[RF_PATH_A] += (l_bnd - phy_stat->gain_b) << 1;
	else if (phy_stat->gain_b > u_bnd)
		rx_power[RF_PATH_A] -= (phy_stat->gain_b - u_bnd) << 1;

	rx_power[RF_PATH_A] -= 110;
	rx_power[RF_PATH_B] -= 110;

	pkt_stat->rx_power[RF_PATH_A] = max3(rx_power[RF_PATH_A],
					     rx_power[RF_PATH_B],
					     min_rx_power);
	pkt_stat->rssi = rtw_phy_rf_power_2_rssi(pkt_stat->rx_power, 1);
	pkt_stat->bw = RTW_CHANNEL_WIDTH_20;
	pkt_stat->signal_power = max(pkt_stat->rx_power[RF_PATH_A],
				     min_rx_power);
}

static void query_phy_status_page1(struct rtw_dev *rtwdev, u8 *phy_status,
				   struct rtw_rx_pkt_stat *pkt_stat)
{
	struct phy_status_jaguar3_page1 *phy_stat;
	u8 rxsc, bw;
	s8 min_rx_power = -120;

	phy_stat = (struct phy_status_jaguar3_page1 *)phy_status;

	if (pkt_stat->rate > DESC_RATE11M && pkt_stat->rate < DESC_RATEMCS0)
		rxsc = phy_stat->l_rxsc;
	else
		rxsc = phy_stat->ht_rxsc;

	if (rxsc >= 9 && rxsc <= 12)
		bw = RTW_CHANNEL_WIDTH_40;
	else if (rxsc >= 13)
		bw = RTW_CHANNEL_WIDTH_80;
	else
		bw = RTW_CHANNEL_WIDTH_20;

	pkt_stat->rx_power[RF_PATH_A] = phy_stat->pwdb_a - 110;
	pkt_stat->rx_power[RF_PATH_B] = phy_stat->pwdb_b - 110;
	pkt_stat->rssi = rtw_phy_rf_power_2_rssi(pkt_stat->rx_power, 2);
	pkt_stat->bw = bw;
	pkt_stat->signal_power = max3(pkt_stat->rx_power[RF_PATH_A],
				      pkt_stat->rx_power[RF_PATH_B],
				      min_rx_power);
}

static void query_phy_status(struct rtw_dev *rtwdev, u8 *phy_status,
			     struct rtw_rx_pkt_stat *pkt_stat)
{
	u8 page;

	page = *phy_status & 0xf;

	switch (page) {
	case 0:
		query_phy_status_page0(rtwdev, phy_status, pkt_stat);
		break;
	case 1:
		query_phy_status_page1(rtwdev, phy_status, pkt_stat);
		break;
	default:
		rtw_warn(rtwdev, "unused phy status page (%d)", page);
		return;
	}
}

static void rtw8822c_query_rx_desc(struct rtw_dev *rtwdev, u8 *rx_desc,
				   struct rtw_rx_pkt_stat *pkt_stat,
				   struct ieee80211_rx_status *rx_status)
{
	struct ieee80211_hdr *hdr;
	u32 desc_sz = rtwdev->chip->rx_pkt_desc_sz;
	u8 *phy_status = NULL;

	memset(pkt_stat, 0, sizeof(*pkt_stat));

	pkt_stat->phy_status = GET_RX_DESC_PHYST(rx_desc);
	pkt_stat->icv_err = GET_RX_DESC_ICV_ERR(rx_desc);
	pkt_stat->crc_err = GET_RX_DESC_CRC32(rx_desc);
	pkt_stat->decrypted = !GET_RX_DESC_SWDEC(rx_desc);
	pkt_stat->is_c2h = GET_RX_DESC_C2H(rx_desc);
	pkt_stat->pkt_len = GET_RX_DESC_PKT_LEN(rx_desc);
	pkt_stat->drv_info_sz = GET_RX_DESC_DRV_INFO_SIZE(rx_desc);
	pkt_stat->shift = GET_RX_DESC_SHIFT(rx_desc);
	pkt_stat->rate = GET_RX_DESC_RX_RATE(rx_desc);
	pkt_stat->cam_id = GET_RX_DESC_MACID(rx_desc);
	pkt_stat->ppdu_cnt = GET_RX_DESC_PPDU_CNT(rx_desc);
	pkt_stat->tsf_low = GET_RX_DESC_TSFL(rx_desc);

	/* drv_info_sz is in unit of 8-bytes */
	pkt_stat->drv_info_sz *= 8;

	/* c2h cmd pkt's rx/phy status is not interested */
	if (pkt_stat->is_c2h)
		return;

	hdr = (struct ieee80211_hdr *)(rx_desc + desc_sz + pkt_stat->shift +
				       pkt_stat->drv_info_sz);
	if (pkt_stat->phy_status) {
		phy_status = rx_desc + desc_sz + pkt_stat->shift;
		query_phy_status(rtwdev, phy_status, pkt_stat);
	}

	rtw_rx_fill_rx_status(rtwdev, pkt_stat, hdr, rx_status, phy_status);
}

static void rtw8822c_set_tx_power_index(struct rtw_dev *rtwdev, u8 power_index,
					u8 path, u8 rate)
{
	/* 8822C will use TSSI to track tx power */
}

static void rtw8822c_cfg_ldo25(struct rtw_dev *rtwdev, bool enable)
{
	u8 ldo_pwr;

	ldo_pwr = rtw_read8(rtwdev, REG_ANAPARLDO_POW_MAC);
	ldo_pwr = enable ? ldo_pwr | BIT(0) : ldo_pwr & ~BIT(0);
	rtw_write8(rtwdev, REG_ANAPARLDO_POW_MAC, ldo_pwr);
}

static void rtw8822c_false_alarm_statistics(struct rtw_dev *rtwdev)
{
	struct rtw_dm_info *dm_info = &rtwdev->dm_info;
	u32 cck_enable;
	u32 cck_fa_cnt;
	u32 ofdm_fa_cnt;
	u32 ofdm_tx_counter;

	cck_enable = rtw_read32(rtwdev, 0x1c3c) & BIT(1);
	cck_fa_cnt = rtw_read16(rtwdev, 0x1a5c);
	ofdm_fa_cnt = rtw_read16(rtwdev, 0x2d00);
	ofdm_tx_counter = rtw_read16(rtwdev, 0x2de0);
	ofdm_fa_cnt -= ofdm_tx_counter;

	dm_info->cck_fa_cnt = cck_fa_cnt;
	dm_info->ofdm_fa_cnt = ofdm_fa_cnt;
	dm_info->total_fa_cnt = ofdm_fa_cnt;
	dm_info->total_fa_cnt += cck_enable ? cck_fa_cnt : 0;

	rtw_write32_mask(rtwdev, 0x1a2c, BIT(14) | BIT(15), 0);
	rtw_write32_mask(rtwdev, 0x1a2c, BIT(14) | BIT(15), 2);
	rtw_write32_mask(rtwdev, 0x1a2c, BIT(12) | BIT(13), 0);
	rtw_write32_mask(rtwdev, 0x1a2c, BIT(12) | BIT(13), 2);
	rtw_write32_set(rtwdev, 0x1eb4, BIT(25));
	rtw_write32_clr(rtwdev, 0x1eb4, BIT(25));
}

static void rtw8822c_do_iqk(struct rtw_dev *rtwdev)
{
}

static struct rtw_pwr_seq_cmd trans_carddis_to_cardemu_8822c[] = {
	{0x0086,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_SDIO_MSK,
	 RTW_PWR_ADDR_SDIO,
	 RTW_PWR_CMD_WRITE, BIT(0), 0},
	{0x0086,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_SDIO_MSK,
	 RTW_PWR_ADDR_SDIO,
	 RTW_PWR_CMD_POLLING, BIT(1), BIT(1)},
	{0x002E,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(2), BIT(2)},
	{0x002D,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(0), 0},
	{0x007F,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(7), 0},
	{0x004A,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_USB_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(0), 0},
	{0x0005,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(3) | BIT(4) | BIT(7), 0},
	{0xFFFF,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 0,
	 RTW_PWR_CMD_END, 0, 0},
};

static struct rtw_pwr_seq_cmd trans_cardemu_to_act_8822c[] = {
	{0x0000,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_USB_MSK | RTW_PWR_INTF_SDIO_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(5), 0},
	{0x0005,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, (BIT(4) | BIT(3) | BIT(2)), 0},
	{0x0075,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_PCI_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(0), BIT(0)},
	{0x0006,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_POLLING, BIT(1), BIT(1)},
	{0x0075,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_PCI_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(0), 0},
	{0xFF1A,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_USB_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, 0xFF, 0},
	{0x002E,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(3), 0},
	{0x0006,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(0), BIT(0)},
	{0x0005,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(7), 0},
	{0x0005,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, (BIT(4) | BIT(3)), 0},
	{0x0005,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(0), BIT(0)},
	{0x0005,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_POLLING, BIT(0), 0},
	{0x0074,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_PCI_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(5), BIT(5)},
	{0x0071,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_PCI_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(4), 0},
	{0x0062,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_PCI_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, (BIT(7) | BIT(6) | BIT(5)),
	 (BIT(7) | BIT(6) | BIT(5))},
	{0x0061,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_PCI_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, (BIT(7) | BIT(6) | BIT(5)), 0},
	{0x001F,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, (BIT(7) | BIT(6)), BIT(7)},
	{0x00EF,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, (BIT(7) | BIT(6)), BIT(7)},
	{0x1045,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(4), BIT(4)},
	{0xFFFF,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 0,
	 RTW_PWR_CMD_END, 0, 0},
};

static struct rtw_pwr_seq_cmd trans_act_to_cardemu_8822c[] = {
	{0x0093,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(3), 0},
	{0x001F,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, 0xFF, 0},
	{0x00EF,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, 0xFF, 0},
	{0x1045,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(4), 0},
	{0xFF1A,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_USB_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, 0xFF, 0x30},
	{0x0049,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(1), 0},
	{0x0006,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(0), BIT(0)},
	{0x0002,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(1), 0},
	{0x0005,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(1), BIT(1)},
	{0x0005,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_POLLING, BIT(1), 0},
	{0x0000,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_USB_MSK | RTW_PWR_INTF_SDIO_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(5), BIT(5)},
	{0xFFFF,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 0,
	 RTW_PWR_CMD_END, 0, 0},
};

static struct rtw_pwr_seq_cmd trans_cardemu_to_carddis_8822c[] = {
	{0x0005,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_SDIO_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(7), BIT(7)},
	{0x0007,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_USB_MSK | RTW_PWR_INTF_SDIO_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, 0xFF, 0x00},
	{0x0067,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(5), 0},
	{0x004A,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_USB_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(0), 0},
	{0x0081,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(7) | BIT(6), 0},
	{0x0090,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(1), 0},
	{0x0005,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_USB_MSK | RTW_PWR_INTF_SDIO_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(3) | BIT(4), BIT(3)},
	{0x0005,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_PCI_MSK,
	 RTW_PWR_ADDR_MAC,
	 RTW_PWR_CMD_WRITE, BIT(2), BIT(2)},
	{0x0086,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_SDIO_MSK,
	 RTW_PWR_ADDR_SDIO,
	 RTW_PWR_CMD_WRITE, BIT(0), BIT(0)},
	{0xFFFF,
	 RTW_PWR_CUT_ALL_MSK,
	 RTW_PWR_INTF_ALL_MSK,
	 0,
	 RTW_PWR_CMD_END, 0, 0},
};

static struct rtw_pwr_seq_cmd *card_enable_flow_8822c[] = {
	trans_carddis_to_cardemu_8822c,
	trans_cardemu_to_act_8822c,
	NULL
};

static struct rtw_pwr_seq_cmd *card_disable_flow_8822c[] = {
	trans_act_to_cardemu_8822c,
	trans_cardemu_to_carddis_8822c,
	NULL
};

static struct rtw_intf_phy_para usb2_param_8822c[] = {
	{0xFFFF, 0x00,
	 RTW_IP_SEL_PHY,
	 RTW_INTF_PHY_CUT_ALL,
	 RTW_INTF_PHY_PLATFORM_ALL},
};

static struct rtw_intf_phy_para usb3_param_8822c[] = {
	{0xFFFF, 0x0000,
	 RTW_IP_SEL_PHY,
	 RTW_INTF_PHY_CUT_ALL,
	 RTW_INTF_PHY_PLATFORM_ALL},
};

static struct rtw_intf_phy_para pcie_gen1_param_8822c[] = {
	{0xFFFF, 0x0000,
	 RTW_IP_SEL_PHY,
	 RTW_INTF_PHY_CUT_ALL,
	 RTW_INTF_PHY_PLATFORM_ALL},
};

static struct rtw_intf_phy_para pcie_gen2_param_8822c[] = {
	{0xFFFF, 0x0000,
	 RTW_IP_SEL_PHY,
	 RTW_INTF_PHY_CUT_ALL,
	 RTW_INTF_PHY_PLATFORM_ALL},
};

static struct rtw_intf_phy_para_table phy_para_table_8822c = {
	.usb2_para	= usb2_param_8822c,
	.usb3_para	= usb3_param_8822c,
	.gen1_para	= pcie_gen1_param_8822c,
	.gen2_para	= pcie_gen2_param_8822c,
	.n_usb2_para	= ARRAY_SIZE(usb2_param_8822c),
	.n_usb3_para	= ARRAY_SIZE(usb2_param_8822c),
	.n_gen1_para	= ARRAY_SIZE(pcie_gen1_param_8822c),
	.n_gen2_para	= ARRAY_SIZE(pcie_gen2_param_8822c),
};

static const struct rtw_rfe_def rtw8822c_rfe_defs[] = {
	[0] = RTW_DEF_RFE(8822c, 0, 0),
};

static struct rtw_hw_reg rtw8822c_dig[] = {
	[0] = { .addr = 0x1d70, .mask = 0x7f },
	[1] = { .addr = 0x1d70, .mask = 0x7f00 },
};

static struct rtw_page_table page_table_8822c[] = {
	{64, 64, 64, 64, 1},
	{64, 64, 64, 64, 1},
	{64, 64, 0, 0, 1},
	{64, 64, 64, 0, 1},
	{64, 64, 64, 64, 1},
};

static struct rtw_rqpn rqpn_table_8822c[] = {
	{RTW_DMA_MAPPING_NORMAL, RTW_DMA_MAPPING_NORMAL,
	 RTW_DMA_MAPPING_LOW, RTW_DMA_MAPPING_LOW,
	 RTW_DMA_MAPPING_EXTRA, RTW_DMA_MAPPING_HIGH},
	{RTW_DMA_MAPPING_NORMAL, RTW_DMA_MAPPING_NORMAL,
	 RTW_DMA_MAPPING_LOW, RTW_DMA_MAPPING_LOW,
	 RTW_DMA_MAPPING_EXTRA, RTW_DMA_MAPPING_HIGH},
	{RTW_DMA_MAPPING_NORMAL, RTW_DMA_MAPPING_NORMAL,
	 RTW_DMA_MAPPING_NORMAL, RTW_DMA_MAPPING_HIGH,
	 RTW_DMA_MAPPING_HIGH, RTW_DMA_MAPPING_HIGH},
	{RTW_DMA_MAPPING_NORMAL, RTW_DMA_MAPPING_NORMAL,
	 RTW_DMA_MAPPING_LOW, RTW_DMA_MAPPING_LOW,
	 RTW_DMA_MAPPING_HIGH, RTW_DMA_MAPPING_HIGH},
	{RTW_DMA_MAPPING_NORMAL, RTW_DMA_MAPPING_NORMAL,
	 RTW_DMA_MAPPING_LOW, RTW_DMA_MAPPING_LOW,
	 RTW_DMA_MAPPING_EXTRA, RTW_DMA_MAPPING_HIGH},
};

static struct rtw_chip_ops rtw8822c_ops = {
	.phy_set_param		= rtw8822c_phy_set_param,
	.read_efuse		= rtw8822c_read_efuse,
	.query_rx_desc		= rtw8822c_query_rx_desc,
	.set_channel		= rtw8822c_set_channel,
	.mac_init		= rtw8822c_mac_init,
	.read_rf		= rtw_phy_read_rf,
	.write_rf		= rtw_phy_write_rf_reg_mix,
	.set_tx_power_index	= rtw8822c_set_tx_power_index,
	.cfg_ldo25		= rtw8822c_cfg_ldo25,
	.false_alarm_statistics	= rtw8822c_false_alarm_statistics,
	.do_iqk			= rtw8822c_do_iqk,
};

struct rtw_chip_info rtw8822c_hw_spec = {
	.ops = &rtw8822c_ops,
	.id = RTW_CHIP_TYPE_8822C,
	.fw_name = "rtwlan/rtw8822c_fw.bin",
	.tx_pkt_desc_sz = 48,
	.tx_buf_desc_sz = 16,
	.rx_pkt_desc_sz = 24,
	.rx_buf_desc_sz = 8,
	.phy_efuse_size = 1024,
	.log_efuse_size = 768,
	.ptct_efuse_size = 124,
	.txff_size = 262144,
	.rxff_size = 24576,
	.csi_buf_pg_num = 50,
	.band = RTW_BAND_2G | RTW_BAND_5G,
	.page_size = 128,
	.dig_min = 0x20,
	.ht_supported = true,
	.vht_supported = true,
	.sys_func_en = 0xD8,
	.pwr_on_seq = card_enable_flow_8822c,
	.pwr_off_seq = card_disable_flow_8822c,
	.page_table = page_table_8822c,
	.rqpn_table = rqpn_table_8822c,
	.intf_table = &phy_para_table_8822c,
	.dig = rtw8822c_dig,
	.rf_base_addr = {0x3c00, 0x4c00},
	.rf_sipi_addr = {0x1808, 0x4108},
	.mac_tbl = &rtw8822c_mac_tbl,
	.agc_tbl = &rtw8822c_agc_tbl,
	.bb_tbl = &rtw8822c_bb_tbl,
	.rf_tbl = {&rtw8822c_rf_a_tbl, &rtw8822c_rf_b_tbl},
	.rfe_defs = rtw8822c_rfe_defs,
	.rfe_defs_size = ARRAY_SIZE(rtw8822c_rfe_defs),
};
EXPORT_SYMBOL(rtw8822c_hw_spec);

MODULE_FIRMWARE("rtwlan/rtw8822c_fw.bin");
