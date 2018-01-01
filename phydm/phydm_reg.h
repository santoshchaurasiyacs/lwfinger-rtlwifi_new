/******************************************************************************
 *
 * Copyright(c) 2007 - 2017  Realtek Corporation.
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
/* ************************************************************
 * File Name: odm_reg.h
 *
 * Description:
 *
 * This file is for general register definition.
 *
 *
 * ************************************************************ */
#ifndef __HAL_ODM_REG_H__
#define __HAL_ODM_REG_H__

/*
 * Register Definition
 *   */

/* MAC REG */
#define ODM_BB_RESET 0x002
#define ODM_DUMMY 0x4fe
#define RF_T_METER_OLD 0x24
#define RF_T_METER_NEW 0x42

#define ODM_EDCA_VO_PARAM 0x500
#define ODM_EDCA_VI_PARAM 0x504
#define ODM_EDCA_BE_PARAM 0x508
#define ODM_EDCA_BK_PARAM 0x50C
#define ODM_TXPAUSE 0x522

/* LTE_COEX */
#define REG_LTECOEX_CTRL 0x07C0
#define REG_LTECOEX_WRITE_DATA 0x07C4
#define REG_LTECOEX_READ_DATA 0x07C8
#define REG_LTECOEX_PATH_CONTROL 0x70

/* BB REG */
#define ODM_FPGA_PHY0_PAGE8 0x800
#define ODM_PSD_SETTING 0x808
#define ODM_AFE_SETTING 0x818
#define ODM_TXAGC_B_6_18 0x830
#define ODM_TXAGC_B_24_54 0x834
#define ODM_TXAGC_B_MCS32_5 0x838
#define ODM_TXAGC_B_MCS0_MCS3 0x83c
#define ODM_TXAGC_B_MCS4_MCS7 0x848
#define ODM_TXAGC_B_MCS8_MCS11 0x84c
#define ODM_ANALOG_REGISTER 0x85c
#define ODM_RF_INTERFACE_OUTPUT 0x860
#define ODM_TXAGC_B_MCS12_MCS15 0x868
#define ODM_TXAGC_B_11_A_2_11 0x86c
#define ODM_AD_DA_LSB_MASK 0x874
#define ODM_ENABLE_3_WIRE 0x88c
#define ODM_PSD_REPORT 0x8b4
#define ODM_R_ANT_SELECT 0x90c
#define ODM_CCK_ANT_SELECT 0xa07
#define ODM_CCK_PD_THRESH 0xa0a
#define ODM_CCK_RF_REG1 0xa11
#define ODM_CCK_MATCH_FILTER 0xa20
#define ODM_CCK_RAKE_MAC 0xa2e
#define ODM_CCK_CNT_RESET 0xa2d
#define ODM_CCK_TX_DIVERSITY 0xa2f
#define ODM_CCK_FA_CNT_MSB 0xa5b
#define ODM_CCK_FA_CNT_LSB 0xa5c
#define ODM_CCK_NEW_FUNCTION 0xa75
#define ODM_OFDM_PHY0_PAGE_C 0xc00
#define ODM_OFDM_RX_ANT 0xc04
#define ODM_R_A_RXIQI 0xc14
#define ODM_R_A_AGC_CORE1 0xc50
#define ODM_R_A_AGC_CORE2 0xc54
#define ODM_R_B_AGC_CORE1 0xc58
#define ODM_R_AGC_PAR 0xc70
#define ODM_R_HTSTF_AGC_PAR 0xc7c
#define ODM_TX_PWR_TRAINING_A 0xc90
#define ODM_TX_PWR_TRAINING_B 0xc98
#define ODM_OFDM_FA_CNT1 0xcf0
#define ODM_OFDM_PHY0_PAGE_D 0xd00
#define ODM_OFDM_FA_CNT2 0xda0
#define ODM_OFDM_FA_CNT3 0xda4
#define ODM_OFDM_FA_CNT4 0xda8
#define ODM_TXAGC_A_6_18 0xe00
#define ODM_TXAGC_A_24_54 0xe04
#define ODM_TXAGC_A_1_MCS32 0xe08
#define ODM_TXAGC_A_MCS0_MCS3 0xe10
#define ODM_TXAGC_A_MCS4_MCS7 0xe14
#define ODM_TXAGC_A_MCS8_MCS11 0xe18
#define ODM_TXAGC_A_MCS12_MCS15 0xe1c

/* RF REG */
#define ODM_GAIN_SETTING 0x00
#define ODM_CHANNEL 0x18
#define ODM_RF_T_METER 0x24
#define ODM_RF_T_METER_92D 0x42
#define ODM_RF_T_METER_88E 0x42
#define ODM_RF_T_METER_92E 0x42
#define ODM_RF_T_METER_8812 0x42
#define REG_RF_TX_GAIN_OFFSET 0x55

/* ant Detect Reg */
#define ODM_DPDT 0x300

/* PSD Init */
#define ODM_PSDREG 0x808

/* 92D path Div */
#define PATHDIV_REG 0xB30
#define PATHDIV_TRI 0xBA0

/*
 * Bitmap Definition
 *   */
#if (DM_ODM_SUPPORT_TYPE & (ODM_AP))
	/* TX AGC */
	#define		REG_TX_AGC_A_CCK_11_CCK_1_JAGUAR	0xc20
	#define		REG_TX_AGC_A_OFDM18_OFDM6_JAGUAR	0xc24
	#define		REG_TX_AGC_A_OFDM54_OFDM24_JAGUAR	0xc28
	#define		REG_TX_AGC_A_MCS3_MCS0_JAGUAR	0xc2c
	#define		REG_TX_AGC_A_MCS7_MCS4_JAGUAR	0xc30
	#define		REG_TX_AGC_A_MCS11_MCS8_JAGUAR	0xc34
	#define		REG_TX_AGC_A_MCS15_MCS12_JAGUAR	0xc38
	#define		REG_TX_AGC_A_NSS1_INDEX3_NSS1_INDEX0_JAGUAR	0xc3c
	#define		REG_TX_AGC_A_NSS1_INDEX7_NSS1_INDEX4_JAGUAR	0xc40
	#define		REG_TX_AGC_A_NSS2_INDEX1_NSS1_INDEX8_JAGUAR	0xc44
	#define		REG_TX_AGC_A_NSS2_INDEX5_NSS2_INDEX2_JAGUAR	0xc48
	#define		REG_TX_AGC_A_NSS2_INDEX9_NSS2_INDEX6_JAGUAR	0xc4c
	#if defined(CONFIG_WLAN_HAL_8814AE)
		#define		REG_TX_AGC_A_MCS19_MCS16_JAGUAR	0xcd8
		#define		REG_TX_AGC_A_MCS23_MCS20_JAGUAR	0xcdc
		#define		REG_TX_AGC_A_NSS3_INDEX3_NSS3_INDEX0_JAGUAR	0xce0
		#define		REG_TX_AGC_A_NSS3_INDEX7_NSS3_INDEX4_JAGUAR	0xce4
		#define		REG_TX_AGC_A_NSS3_INDEX9_NSS3_INDEX8_JAGUAR	0xce8
	#endif
	#define		REG_TX_AGC_B_CCK_11_CCK_1_JAGUAR	0xe20
	#define		REG_TX_AGC_B_OFDM18_OFDM6_JAGUAR	0xe24
	#define		REG_TX_AGC_B_OFDM54_OFDM24_JAGUAR	0xe28
	#define		REG_TX_AGC_B_MCS3_MCS0_JAGUAR	0xe2c
	#define		REG_TX_AGC_B_MCS7_MCS4_JAGUAR	0xe30
	#define		REG_TX_AGC_B_MCS11_MCS8_JAGUAR	0xe34
	#define		REG_TX_AGC_B_MCS15_MCS12_JAGUAR	0xe38
	#define		REG_TX_AGC_B_NSS1_INDEX3_NSS1_INDEX0_JAGUAR	0xe3c
	#define		REG_TX_AGC_B_NSS1_INDEX7_NSS1_INDEX4_JAGUAR	0xe40
	#define		REG_TX_AGC_B_NSS2_INDEX1_NSS1_INDEX8_JAGUAR	0xe44
	#define		REG_TX_AGC_B_NSS2_INDEX5_NSS2_INDEX2_JAGUAR	0xe48
	#define		REG_TX_AGC_B_NSS2_INDEX9_NSS2_INDEX6_JAGUAR	0xe4c
	#if defined(CONFIG_WLAN_HAL_8814AE)
		#define		REG_TX_AGC_B_MCS19_MCS16_JAGUAR	0xed8
		#define		REG_TX_AGC_B_MCS23_MCS20_JAGUAR	0xedc
		#define		REG_TX_AGC_B_NSS3_INDEX3_NSS3_INDEX0_JAGUAR	0xee0
		#define		REG_TX_AGC_B_NSS3_INDEX7_NSS3_INDEX4_JAGUAR	0xee4
		#define		REG_TX_AGC_B_NSS3_INDEX9_NSS3_INDEX8_JAGUAR	0xee8
		#define		REG_TX_AGC_C_CCK_11_CCK_1_JAGUAR	0x1820
		#define		REG_TX_AGC_C_OFDM18_OFDM6_JAGUAR	0x1824
		#define		REG_TX_AGC_C_OFDM54_OFDM24_JAGUAR	0x1828
		#define		REG_TX_AGC_C_MCS3_MCS0_JAGUAR	0x182c
		#define		REG_TX_AGC_C_MCS7_MCS4_JAGUAR	0x1830
		#define		REG_TX_AGC_C_MCS11_MCS8_JAGUAR	0x1834
		#define		REG_TX_AGC_C_MCS15_MCS12_JAGUAR	0x1838
		#define		REG_TX_AGC_C_NSS1_INDEX3_NSS1_INDEX0_JAGUAR	0x183c
		#define		REG_TX_AGC_C_NSS1_INDEX7_NSS1_INDEX4_JAGUAR	0x1840
		#define		REG_TX_AGC_C_NSS2_INDEX1_NSS1_INDEX8_JAGUAR	0x1844
		#define		REG_TX_AGC_C_NSS2_INDEX5_NSS2_INDEX2_JAGUAR	0x1848
		#define		REG_TX_AGC_C_NSS2_INDEX9_NSS2_INDEX6_JAGUAR	0x184c
		#define		REG_TX_AGC_C_MCS19_MCS16_JAGUAR	0x18d8
		#define		REG_TX_AGC_C_MCS23_MCS20_JAGUAR	0x18dc
		#define		REG_TX_AGC_C_NSS3_INDEX3_NSS3_INDEX0_JAGUAR	0x18e0
		#define		REG_TX_AGC_C_NSS3_INDEX7_NSS3_INDEX4_JAGUAR	0x18e4
		#define		REG_TX_AGC_C_NSS3_INDEX9_NSS3_INDEX8_JAGUAR	0x18e8
		#define		REG_TX_AGC_D_CCK_11_CCK_1_JAGUAR	0x1a20
		#define		REG_TX_AGC_D_OFDM18_OFDM6_JAGUAR	0x1a24
		#define		REG_TX_AGC_D_OFDM54_OFDM24_JAGUAR	0x1a28
		#define		REG_TX_AGC_D_MCS3_MCS0_JAGUAR	0x1a2c
		#define		REG_TX_AGC_D_MCS7_MCS4_JAGUAR	0x1a30
		#define		REG_TX_AGC_D_MCS11_MCS8_JAGUAR	0x1a34
		#define		REG_TX_AGC_D_MCS15_MCS12_JAGUAR	0x1a38
		#define		REG_TX_AGC_D_NSS1_INDEX3_NSS1_INDEX0_JAGUAR	0x1a3c
		#define		REG_TX_AGC_D_NSS1_INDEX7_NSS1_INDEX4_JAGUAR	0x1a40
		#define		REG_TX_AGC_D_NSS2_INDEX1_NSS1_INDEX8_JAGUAR	0x1a44
		#define		REG_TX_AGC_D_NSS2_INDEX5_NSS2_INDEX2_JAGUAR	0x1a48
		#define		REG_TX_AGC_D_NSS2_INDEX9_NSS2_INDEX6_JAGUAR	0x1a4c
		#define		REG_TX_AGC_D_MCS19_MCS16_JAGUAR	0x1ad8
		#define		REG_TX_AGC_D_MCS23_MCS20_JAGUAR	0x1adc
		#define		REG_TX_AGC_D_NSS3_INDEX3_NSS3_INDEX0_JAGUAR	0x1ae0
		#define		REG_TX_AGC_D_NSS3_INDEX7_NSS3_INDEX4_JAGUAR	0x1ae4
		#define		REG_TX_AGC_D_NSS3_INDEX9_NSS3_INDEX8_JAGUAR	0x1ae8
	#endif

	#define		is_tx_agc_byte0_jaguar	0xff
	#define		is_tx_agc_byte1_jaguar	0xff00
	#define		is_tx_agc_byte2_jaguar	0xff0000
	#define		is_tx_agc_byte3_jaguar	0xff000000
#endif

#define BIT_FA_RESET BIT(0)

#if (DM_ODM_SUPPORT_TYPE == ODM_CE) && defined(DM_ODM_CE_MAC80211)
#define REG_OFDM_0_XA_TX_IQ_IMBALANCE 0xC80
#define REG_OFDM_0_ECCA_THRESHOLD 0xC4C
#define REG_FPGA0_XB_LSSI_READ_BACK 0x8A4
#define REG_FPGA0_TX_GAIN_STAGE 0x80C
#define REG_OFDM_0_XA_AGC_CORE1 0xC50
#define REG_OFDM_0_XB_AGC_CORE1 0xC58
#define REG_A_TX_SCALE_JAGUAR 0xC1C
#define REG_B_TX_SCALE_JAGUAR 0xE1C

#define REG_AFE_XTAL_CTRL 0x0024
#define REG_AFE_PLL_CTRL 0x0028
#define REG_MAC_PHY_CTRL 0x002C

#define RF_CHNLBW 0x18
#endif

/*
 * 3. Page8(0x800)
 *   */
#define		rFPGA0_RFMOD				0x800	/* RF mode & CCK TxSC // RF BW Setting?? */

#define		rFPGA0_TxInfo				0x804	/* Status report?? */
#define		rFPGA0_PSDFunction			0x808

#define		rFPGA0_TxGainStage			0x80c	/* Set TX PWR init gain? */

#define		rFPGA0_RFTiming1			0x810	/* Useless now */
#define		rFPGA0_RFTiming2			0x814

#define		rFPGA0_XA_HSSIParameter1		0x820	/* RF 3 wire register */
#define		rFPGA0_XA_HSSIParameter2		0x824
#define		rFPGA0_XB_HSSIParameter1		0x828
#define		rFPGA0_XB_HSSIParameter2		0x82c
#define		rTxAGC_B_Rate18_06				0x830
#define		rTxAGC_B_Rate54_24				0x834
#define		rTxAGC_B_CCK1_55_Mcs32		0x838
#define		rTxAGC_B_Mcs03_Mcs00			0x83c

#define		rTxAGC_B_Mcs07_Mcs04			0x848
#define		rTxAGC_B_Mcs11_Mcs08			0x84c

#define		rFPGA0_XA_LSSIParameter		0x840
#define		rFPGA0_XB_LSSIParameter		0x844

#define		rFPGA0_RFWakeUpParameter		0x850	/* Useless now */
#define		rFPGA0_RFSleepUpParameter		0x854

#define		rFPGA0_XAB_SwitchControl		0x858	/* RF Channel switch */
#define		rFPGA0_XCD_SwitchControl		0x85c

#define		rFPGA0_XA_RFInterfaceOE		0x860	/* RF Channel switch */
#define		rFPGA0_XB_RFInterfaceOE		0x864

#define		rTxAGC_B_Mcs15_Mcs12			0x868
#define		rTxAGC_B_CCK11_A_CCK2_11		0x86c

#define		rFPGA0_XAB_RFInterfaceSW		0x870	/* RF Interface Software Control */
#define		rFPGA0_XCD_RFInterfaceSW		0x874

#define		rFPGA0_XAB_RFParameter		0x878	/* RF Parameter */
#define		rFPGA0_XCD_RFParameter		0x87c

#define		rFPGA0_AnalogParameter1		0x880	/* Crystal cap setting RF-R/W protection for parameter4?? */
#define		rFPGA0_AnalogParameter2		0x884
#define		rFPGA0_AnalogParameter3		0x888	/* Useless now */
#define		rFPGA0_AnalogParameter4		0x88c

#define		rFPGA0_XA_LSSIReadBack		0x8a0	/* Tranceiver LSSI Readback */
#define		rFPGA0_XB_LSSIReadBack		0x8a4
#define		rFPGA0_XC_LSSIReadBack		0x8a8
#define		rFPGA0_XD_LSSIReadBack		0x8ac

#define		rFPGA0_PSDReport				0x8b4	/* Useless now */
#define		TransceiverA_HSPI_Readback	0x8b8	/* Transceiver A HSPI Readback */
#define		TransceiverB_HSPI_Readback	0x8bc	/* Transceiver B HSPI Readback */
#define		rFPGA0_XAB_RFInterfaceRB		0x8e0	/* Useless now // RF Interface Readback Value */
#define		rFPGA0_XCD_RFInterfaceRB		0x8e4	/* Useless now */


/*
 * 4. Page9(0x900)
 *   */
#define	rFPGA1_RFMOD				0x900	/* RF mode & OFDM TxSC // RF BW Setting?? */
#define	rFPGA1_TxBlock				0x904	/* Useless now */
#define	rFPGA1_DebugSelect			0x908	/* Useless now */
#define	rFPGA1_TxInfo				0x90c	/* Useless now // Status report?? */
#define	rDPDT_control				0x92c
#define	rfe_ctrl_anta_src				0x930
#define	rS0S1_PathSwitch			0x948
#define	rBBrx_DFIR					0x954

/*
 * 5. PageA(0xA00)
 *
 * Set Control channel to upper or lower. These settings are required only for 40MHz */
#define		rCCK0_System				0xa00

#define		rCCK0_AFESetting			0xa04	/* Disable init gain now // Select RX path by RSSI */
#define		rCCK0_CCA					0xa08	/* Disable init gain now // Init gain */

#define		rCCK0_RxAGC1				0xa0c	/* AGC default value, saturation level // Antenna Diversity, RX AGC, LNA Threshold, RX LNA Threshold useless now. Not the same as 90 series */
#define		rCCK0_RxAGC2				0xa10	/* AGC & DAGC */

#define		rCCK0_RxHP					0xa14

#define		rCCK0_DSPParameter1		0xa18	/* Timing recovery & Channel estimation threshold */
#define		rCCK0_DSPParameter2		0xa1c	/* SQ threshold */

#define		rCCK0_TxFilter1				0xa20
#define		rCCK0_TxFilter2				0xa24
#define		rCCK0_DebugPort			0xa28	/* debug port and Tx filter3 */
#define		rCCK0_FalseAlarmReport		0xa2c	/* 0xa2d	useless now 0xa30-a4f channel report */
#define		rCCK0_TRSSIReport		0xa50
#define		rCCK0_RxReport			0xa54  /* 0xa57 */
#define		rCCK0_FACounterLower		0xa5c  /* 0xa5b */
#define		rCCK0_FACounterUpper		0xa58  /* 0xa5c */

/*
 * PageB(0xB00)
 *   */
#define rPdp_AntA						0xb00
#define rPdp_AntA_4						0xb04
#define rPdp_AntA_8						0xb08
#define rPdp_AntA_C						0xb0c
#define rPdp_AntA_10					0xb10
#define rPdp_AntA_14					0xb14
#define rPdp_AntA_18					0xb18
#define rPdp_AntA_1C					0xb1c
#define rPdp_AntA_20					0xb20
#define rPdp_AntA_24					0xb24

#define rConfig_Pmpd_AntA				0xb28
#define rConfig_ram64x16				0xb2c

#define rBndA							0xb30
#define rHssiPar						0xb34

#define rConfig_AntA					0xb68
#define rConfig_AntB					0xb6c

#define rPdp_AntB						0xb70
#define rPdp_AntB_4						0xb74
#define rPdp_AntB_8						0xb78
#define rPdp_AntB_C						0xb7c
#define rPdp_AntB_10					0xb80
#define rPdp_AntB_14					0xb84
#define rPdp_AntB_18					0xb88
#define rPdp_AntB_1C					0xb8c
#define rPdp_AntB_20					0xb90
#define rPdp_AntB_24					0xb94

#define rConfig_Pmpd_AntB				0xb98

#define rBndB							0xba0

#define rAPK							0xbd8
#define rPm_Rx0_AntA					0xbdc
#define rPm_Rx1_AntA					0xbe0
#define rPm_Rx2_AntA					0xbe4
#define rPm_Rx3_AntA					0xbe8
#define rPm_Rx0_AntB					0xbec
#define rPm_Rx1_AntB					0xbf0
#define rPm_Rx2_AntB					0xbf4
#define rPm_Rx3_AntB					0xbf8


/*
 * 6. PageC(0xC00)
 *   */
#define		rOFDM0_LSTF				0xc00

#define		rOFDM0_TRxPathEnable		0xc04
#define		rOFDM0_TRMuxPar			0xc08
#define		rOFDM0_TRSWIsolation		0xc0c

#define		rOFDM0_XARxAFE			0xc10  /* RxIQ DC offset, Rx digital filter, DC notch filter */
#define		rOFDM0_XARxIQImbalance		0xc14  /* RxIQ imbalance matrix */
#define		rOFDM0_XBRxAFE		0xc18
#define		rOFDM0_XBRxIQImbalance	0xc1c
#define		rOFDM0_XCRxAFE		0xc20
#define		rOFDM0_XCRxIQImbalance	0xc24
#define		rOFDM0_XDRxAFE		0xc28
#define		rOFDM0_XDRxIQImbalance	0xc2c

#define		rOFDM0_RxDetector1			0xc30  /* PD, BW & SBD	// DM tune init gain */
#define		rOFDM0_RxDetector2			0xc34  /* SBD & Fame Sync. */
#define		rOFDM0_RxDetector3			0xc38  /* Frame Sync. */
#define		rOFDM0_RxDetector4			0xc3c  /* PD, SBD, Frame Sync & Short-GI */

#define		rOFDM0_RxDSP				0xc40  /* Rx Sync Path */
#define		rOFDM0_CFOandDAGC		0xc44  /* CFO & DAGC */
#define		rOFDM0_CCADropThreshold	0xc48 /* CCA Drop threshold */
#define		rOFDM0_ECCAThreshold		0xc4c /* energy CCA */

#define		rOFDM0_XAAGCCore1			0xc50	/* DIG */
#define		rOFDM0_XAAGCCore2			0xc54
#define		rOFDM0_XBAGCCore1			0xc58
#define		rOFDM0_XBAGCCore2			0xc5c
#define		rOFDM0_XCAGCCore1			0xc60
#define		rOFDM0_XCAGCCore2			0xc64
#define		rOFDM0_XDAGCCore1			0xc68
#define		rOFDM0_XDAGCCore2			0xc6c

#define		rOFDM0_AGCParameter1			0xc70
#define		rOFDM0_AGCParameter2			0xc74
#define		rOFDM0_AGCRSSITable			0xc78
#define		rOFDM0_HTSTFAGC				0xc7c

#define		rOFDM0_XATxIQImbalance		0xc80	/* TX PWR TRACK and DIG */
#define		rOFDM0_XATxAFE				0xc84
#define		rOFDM0_XBTxIQImbalance		0xc88
#define		rOFDM0_XBTxAFE				0xc8c
#define		rOFDM0_XCTxIQImbalance		0xc90
#define		rOFDM0_XCTxAFE			0xc94
#define		rOFDM0_XDTxIQImbalance		0xc98
#define		rOFDM0_XDTxAFE				0xc9c

#define		rOFDM0_RxIQExtAnta			0xca0
#define		rOFDM0_TxCoeff1				0xca4
#define		rOFDM0_TxCoeff2				0xca8
#define		rOFDM0_TxCoeff3				0xcac
#define		rOFDM0_TxCoeff4				0xcb0
#define		rOFDM0_TxCoeff5				0xcb4
#define		rOFDM0_TxCoeff6				0xcb8
#define		rOFDM0_RxHPParameter			0xce0
#define		rOFDM0_TxPseudoNoiseWgt		0xce4
#define		rOFDM0_FrameSync				0xcf0
#define		rOFDM0_DFSReport				0xcf4

/*
 * 7. PageD(0xD00)
 *   */
#define		rOFDM1_LSTF					0xd00
#define		rOFDM1_TRxPathEnable			0xd04

#define		rOFDM1_CFO						0xd08	/* No setting now */
#define		rOFDM1_CSI1					0xd10
#define		rOFDM1_SBD						0xd14
#define		rOFDM1_CSI2					0xd18
#define		rOFDM1_CFOTracking			0xd2c
#define		rOFDM1_TRxMesaure1			0xd34
#define		rOFDM1_IntfDet					0xd3c
#define		rOFDM1_PseudoNoiseStateAB		0xd50
#define		rOFDM1_PseudoNoiseStateCD		0xd54
#define		rOFDM1_RxPseudoNoiseWgt		0xd58

#define		rOFDM_PHYCounter1				0xda0  /* cca, parity fail */
#define		rOFDM_PHYCounter2				0xda4  /* rate illegal, crc8 fail */
#define		rOFDM_PHYCounter3				0xda8  /* MCS not support */

#define		rOFDM_ShortCFOAB				0xdac	/* No setting now */
#define		rOFDM_ShortCFOCD				0xdb0
#define		rOFDM_LongCFOAB				0xdb4
#define		rOFDM_LongCFOCD				0xdb8
#define		rOFDM_TailCFOAB				0xdbc
#define		rOFDM_TailCFOCD				0xdc0
#define		rOFDM_PWMeasure1		0xdc4
#define		rOFDM_PWMeasure2		0xdc8
#define		rOFDM_BWReport				0xdcc
#define		rOFDM_AGCReport				0xdd0
#define		rOFDM_RxSNR					0xdd4
#define		rOFDM_RxEVMCSI				0xdd8
#define		rOFDM_SIGReport				0xddc


/*
 * 8. PageE(0xE00)
 *   */
#define		rTxAGC_A_Rate18_06			0xe00
#define		rTxAGC_A_Rate54_24			0xe04
#define		rTxAGC_A_CCK1_Mcs32			0xe08
#define		rTxAGC_A_Mcs03_Mcs00			0xe10
#define		rTxAGC_A_Mcs07_Mcs04			0xe14
#define		rTxAGC_A_Mcs11_Mcs08			0xe18
#define		rTxAGC_A_Mcs15_Mcs12			0xe1c

#define		rFPGA0_IQK					0xe28
#define		rTx_IQK_Tone_A				0xe30
#define		rRx_IQK_Tone_A				0xe34
#define		rTx_IQK_PI_A					0xe38
#define		rRx_IQK_PI_A					0xe3c

#define		rTx_IQK						0xe40
#define		rRx_IQK						0xe44
#define		rIQK_AGC_Pts					0xe48
#define		rIQK_AGC_Rsp					0xe4c
#define		rTx_IQK_Tone_B				0xe50
#define		rRx_IQK_Tone_B				0xe54
#define		rTx_IQK_PI_B					0xe58
#define		rRx_IQK_PI_B					0xe5c
#define		rIQK_AGC_Cont				0xe60

#define		rBlue_Tooth					0xe6c
#define		rRx_Wait_CCA					0xe70
#define		rTx_CCK_RFON					0xe74
#define		rTx_CCK_BBON				0xe78
#define		rTx_OFDM_RFON				0xe7c
#define		rTx_OFDM_BBON				0xe80
#define		rTx_To_Rx					0xe84
#define		rTx_To_Tx					0xe88
#define		rRx_CCK						0xe8c

#define		rTx_Power_Before_IQK_A		0xe94
#define		rTx_Power_After_IQK_A			0xe9c

#define		rRx_Power_Before_IQK_A		0xea0
#define		rRx_Power_Before_IQK_A_2		0xea4
#define		rRx_Power_After_IQK_A			0xea8
#define		rRx_Power_After_IQK_A_2		0xeac

#define		rTx_Power_Before_IQK_B		0xeb4
#define		rTx_Power_After_IQK_B			0xebc

#define		rRx_Power_Before_IQK_B		0xec0
#define		rRx_Power_Before_IQK_B_2		0xec4
#define		rRx_Power_After_IQK_B			0xec8
#define		rRx_Power_After_IQK_B_2		0xecc

#define		rRx_OFDM					0xed0
#define		rRx_Wait_RIFS				0xed4
#define		rRx_TO_Rx					0xed8
#define		rStandby						0xedc
#define		rSleep						0xee0
#define		rPMPD_ANAEN				0xeec




#define REG_FPGA0_XAB_RF_INTERFACE_SW	rFPGA0_XAB_RFInterfaceSW
#define REG_FPGA0_XAB_RF_PARAMETER	rFPGA0_XAB_RFParameter
#define REG_FPGA0_XA_HSSI_PARAMETER1	rFPGA0_XA_HSSIParameter1
#define REG_FPGA0_XA_LSSI_PARAMETER	rFPGA0_XA_LSSIParameter
#define REG_FPGA0_XA_RF_INTERFACE_OE	rFPGA0_XA_RFInterfaceOE
#define REG_FPGA0_XB_HSSI_PARAMETER1	rFPGA0_XB_HSSIParameter1
#define REG_FPGA0_XB_LSSI_PARAMETER	rFPGA0_XB_LSSIParameter
#define REG_FPGA0_XB_RF_INTERFACE_OE	rFPGA0_XB_RFInterfaceOE
#define REG_FPGA0_XCD_RF_INTERFACE_SW	rFPGA0_XCD_RFInterfaceSW
#define REG_FPGA0_XCD_SWITCH_CONTROL	rFPGA0_XCD_SwitchControl
#define REG_FPGA1_TX_BLOCK	rFPGA1_TxBlock
#define REG_FPGA1_TX_INFO	rFPGA1_TxInfo
#define REG_IQK_AGC_CONT	rIQK_AGC_Cont
#define REG_IQK_AGC_PTS	rIQK_AGC_Pts
#define REG_IQK_AGC_RSP	rIQK_AGC_Rsp
#define REG_OFDM_0_AGC_RSSI_TABLE	rOFDM0_AGCRSSITable
#define REG_OFDM_0_RX_IQ_EXT_ANTA	rOFDM0_RxIQExtAnta
#define REG_OFDM_0_TR_MUX_PAR	rOFDM0_TRMuxPar
#define REG_OFDM_0_TRX_PATH_ENABLE	rOFDM0_TRxPathEnable
#define REG_OFDM_0_XA_RX_IQ_IMBALANCE	rOFDM0_XARxIQImbalance
#define REG_OFDM_0_XB_RX_IQ_IMBALANCE	rOFDM0_XBRxIQImbalance
#define REG_OFDM_0_XB_TX_IQ_IMBALANCE	rOFDM0_XBTxIQImbalance
#define REG_OFDM_0_XC_TX_AFE	rOFDM0_XCTxAFE
#define REG_OFDM_0_XD_TX_AFE	rOFDM0_XDTxAFE

/*#define REG_A_CFO_LONG_DUMP_92E	rA_CfoLongDump_92E*/
#define REG_A_CFO_LONG_DUMP_JAGUAR	rA_CfoLongDump_Jaguar
/*#define REG_A_CFO_SHORT_DUMP_92E	rA_CfoShortDump_92E*/
#define REG_A_CFO_SHORT_DUMP_JAGUAR	rA_CfoShortDump_Jaguar
#define REG_A_RFE_PINMUX_JAGUAR	rA_RFE_Pinmux_Jaguar
/*#define REG_A_RSSI_DUMP_92E	rA_RSSIDump_92E*/
#define REG_A_RSSI_DUMP_JAGUAR	rA_RSSIDump_Jaguar
/*#define REG_A_RX_SNR_DUMP_92E	rA_RXsnrDump_92E*/
#define REG_A_RX_SNR_DUMP_JAGUAR	rA_RXsnrDump_Jaguar
/*#define REG_A_TX_AGC	rA_TXAGC*/
#define REG_BW_INDICATION_JAGUAR	rBWIndication_Jaguar
/*#define REG_B_BBSWING	rB_BBSWING*/
/*#define REG_B_CFO_LONG_DUMP_92E	rB_CfoLongDump_92E*/
#define REG_B_CFO_LONG_DUMP_JAGUAR	rB_CfoLongDump_Jaguar
/*#define REG_B_CFO_SHORT_DUMP_92E	rB_CfoShortDump_92E*/
#define REG_B_CFO_SHORT_DUMP_JAGUAR	rB_CfoShortDump_Jaguar
/*#define REG_B_RSSI_DUMP_92E	rB_RSSIDump_92E*/
#define REG_B_RSSI_DUMP_JAGUAR	rB_RSSIDump_Jaguar
/*#define REG_B_RX_SNR_DUMP_92E	rB_RXsnrDump_92E*/
#define REG_B_RX_SNR_DUMP_JAGUAR	rB_RXsnrDump_Jaguar
/*#define REG_B_TX_AGC	rB_TXAGC*/
#define REG_BLUE_TOOTH	rBlue_Tooth
#define REG_CCK_0_AFE_SETTING	rCCK0_AFESetting
/*#define REG_C_BBSWING	rC_BBSWING*/
/*#define REG_C_TX_AGC	rC_TXAGC*/
#define REG_C_TX_SCALE_JAGUAR2	rC_TxScale_Jaguar2
#define REG_CONFIG_ANT_A	rConfig_AntA
#define REG_CONFIG_ANT_B	rConfig_AntB
#define REG_CONFIG_PMPD_ANT_A	rConfig_Pmpd_AntA
#define REG_CONFIG_PMPD_ANT_B	rConfig_Pmpd_AntB
#define REG_DPDT_CONTROL	rDPDT_control
/*#define REG_D_BBSWING	rD_BBSWING*/
/*#define REG_D_TX_AGC	rD_TXAGC*/
#define REG_D_TX_SCALE_JAGUAR2	rD_TxScale_Jaguar2
#define REG_FPGA0_ANALOG_PARAMETER4	rFPGA0_AnalogParameter4
#define REG_FPGA0_IQK	rFPGA0_IQK
#define REG_FPGA0_PSD_FUNCTION	rFPGA0_PSDFunction
#define REG_FPGA0_PSD_REPORT	rFPGA0_PSDReport
#define REG_FPGA0_RFMOD	rFPGA0_RFMOD

#define REG_PMPD_ANAEN	rPMPD_ANAEN
#define REG_PDP_ANT_A	rPdp_AntA
#define REG_PDP_ANT_A_4	rPdp_AntA_4
#define REG_PDP_ANT_B	rPdp_AntB
#define REG_PDP_ANT_B_4	rPdp_AntB_4
#define REG_PWED_TH_JAGUAR	rPwed_TH_Jaguar
#define REG_RX_CCK	rRx_CCK
#define REG_RX_IQK	rRx_IQK
#define REG_RX_IQK_PI_A	rRx_IQK_PI_A
#define REG_RX_IQK_PI_B	rRx_IQK_PI_B
#define REG_RX_IQK_TONE_A	rRx_IQK_Tone_A
#define REG_RX_IQK_TONE_B	rRx_IQK_Tone_B
#define REG_RX_OFDM	rRx_OFDM
#define REG_RX_POWER_AFTER_IQK_A_2	rRx_Power_After_IQK_A_2
#define REG_RX_POWER_AFTER_IQK_B_2	rRx_Power_After_IQK_B_2
#define REG_RX_POWER_BEFORE_IQK_A_2	rRx_Power_Before_IQK_A_2
#define REG_RX_POWER_BEFORE_IQK_B_2	rRx_Power_Before_IQK_B_2
#define REG_RX_TO_RX	rRx_TO_Rx
#define REG_RX_WAIT_CCA	rRx_Wait_CCA
#define REG_RX_WAIT_RIFS	rRx_Wait_RIFS
#define REG_S0_S1_PATH_SWITCH	rS0S1_PathSwitch
/*#define REG_S1_RXEVM_DUMP_92E	rS1_RXevmDump_92E*/
#define REG_S1_RXEVM_DUMP_JAGUAR	rS1_RXevmDump_Jaguar
/*#define REG_S2_RXEVM_DUMP_92E	rS2_RXevmDump_92E*/
#define REG_S2_RXEVM_DUMP_JAGUAR	rS2_RXevmDump_Jaguar
#define REG_SYM_WLBT_PAPE_SEL	rSYM_WLBT_PAPE_SEL
#define REG_SINGLE_TONE_CONT_TX_JAGUAR	rSingleTone_ContTx_Jaguar
#define REG_SLEEP	rSleep
#define REG_STANDBY	rStandby
#define REG_TX_AGC_A_CCK_11_CCK_1_JAGUAR	rTxAGC_A_CCK11_CCK1_JAguar
#define REG_TX_AGC_A_CCK_1_MCS32	rTxAGC_A_CCK1_Mcs32
#define REG_TX_AGC_A_MCS11_MCS8_JAGUAR	rTxAGC_A_MCS11_MCS8_JAguar
#define REG_TX_AGC_A_MCS15_MCS12_JAGUAR	rTxAGC_A_MCS15_MCS12_JAguar
#define REG_TX_AGC_A_MCS19_MCS16_JAGUAR	rTxAGC_A_MCS19_MCS16_JAguar
#define REG_TX_AGC_A_MCS23_MCS20_JAGUAR	rTxAGC_A_MCS23_MCS20_JAguar
#define REG_TX_AGC_A_MCS3_MCS0_JAGUAR	rTxAGC_A_MCS3_MCS0_JAguar
#define REG_TX_AGC_A_MCS7_MCS4_JAGUAR	rTxAGC_A_MCS7_MCS4_JAguar
#define REG_TX_AGC_A_MCS03_MCS00	rTxAGC_A_Mcs03_Mcs00
#define REG_TX_AGC_A_MCS07_MCS04	rTxAGC_A_Mcs07_Mcs04
#define REG_TX_AGC_A_MCS11_MCS08	rTxAGC_A_Mcs11_Mcs08
#define REG_TX_AGC_A_MCS15_MCS12	rTxAGC_A_Mcs15_Mcs12
#define REG_TX_AGC_A_NSS1_INDEX3_NSS1_INDEX0_JAGUAR	rTxAGC_A_Nss1Index3_Nss1Index0_JAguar
#define REG_TX_AGC_A_NSS1_INDEX7_NSS1_INDEX4_JAGUAR	rTxAGC_A_Nss1Index7_Nss1Index4_JAguar
#define REG_TX_AGC_A_NSS2_INDEX1_NSS1_INDEX8_JAGUAR	rTxAGC_A_Nss2Index1_Nss1Index8_JAguar
#define REG_TX_AGC_A_NSS2_INDEX5_NSS2_INDEX2_JAGUAR	rTxAGC_A_Nss2Index5_Nss2Index2_JAguar
#define REG_TX_AGC_A_NSS2_INDEX9_NSS2_INDEX6_JAGUAR	rTxAGC_A_Nss2Index9_Nss2Index6_JAguar
#define REG_TX_AGC_A_NSS3_INDEX3_NSS3_INDEX0_JAGUAR	rTxAGC_A_Nss3Index3_Nss3Index0_JAguar
#define REG_TX_AGC_A_NSS3_INDEX7_NSS3_INDEX4_JAGUAR	rTxAGC_A_Nss3Index7_Nss3Index4_JAguar
#define REG_TX_AGC_A_NSS3_INDEX9_NSS3_INDEX8_JAGUAR	rTxAGC_A_Nss3Index9_Nss3Index8_JAguar
#define REG_TX_AGC_A_OFDM18_OFDM6_JAGUAR	rTxAGC_A_Ofdm18_Ofdm6_JAguar
#define REG_TX_AGC_A_OFDM54_OFDM24_JAGUAR	rTxAGC_A_Ofdm54_Ofdm24_JAguar
#define REG_TX_AGC_A_RATE18_06	rTxAGC_A_Rate18_06
#define REG_TX_AGC_A_RATE54_24	rTxAGC_A_Rate54_24
#define REG_TX_AGC_B_CCK_11_A_CCK_2_11	rTxAGC_B_CCK11_A_CCK2_11
#define REG_TX_AGC_B_CCK_11_CCK_1_JAGUAR	rTxAGC_B_CCK11_CCK1_JAguar
#define REG_TX_AGC_B_CCK_1_55_MCS32	rTxAGC_B_CCK1_55_Mcs32
#define REG_TX_AGC_B_MCS11_MCS8_JAGUAR	rTxAGC_B_MCS11_MCS8_JAguar
#define REG_TX_AGC_B_MCS15_MCS12_JAGUAR	rTxAGC_B_MCS15_MCS12_JAguar
#define REG_TX_AGC_B_MCS19_MCS16_JAGUAR	rTxAGC_B_MCS19_MCS16_JAguar
#define REG_TX_AGC_B_MCS23_MCS20_JAGUAR	rTxAGC_B_MCS23_MCS20_JAguar
#define REG_TX_AGC_B_MCS3_MCS0_JAGUAR	rTxAGC_B_MCS3_MCS0_JAguar
#define REG_TX_AGC_B_MCS7_MCS4_JAGUAR	rTxAGC_B_MCS7_MCS4_JAguar
#define REG_TX_AGC_B_MCS03_MCS00	rTxAGC_B_Mcs03_Mcs00
#define REG_TX_AGC_B_MCS07_MCS04	rTxAGC_B_Mcs07_Mcs04
#define REG_TX_AGC_B_MCS11_MCS08	rTxAGC_B_Mcs11_Mcs08
#define REG_TX_AGC_B_MCS15_MCS12	rTxAGC_B_Mcs15_Mcs12
#define REG_TX_AGC_B_NSS1_INDEX3_NSS1_INDEX0_JAGUAR	rTxAGC_B_Nss1Index3_Nss1Index0_JAguar
#define REG_TX_AGC_B_NSS1_INDEX7_NSS1_INDEX4_JAGUAR	rTxAGC_B_Nss1Index7_Nss1Index4_JAguar
#define REG_TX_AGC_B_NSS2_INDEX1_NSS1_INDEX8_JAGUAR	rTxAGC_B_Nss2Index1_Nss1Index8_JAguar
#define REG_TX_AGC_B_NSS2_INDEX5_NSS2_INDEX2_JAGUAR	rTxAGC_B_Nss2Index5_Nss2Index2_JAguar
#define REG_TX_AGC_B_NSS2_INDEX9_NSS2_INDEX6_JAGUAR	rTxAGC_B_Nss2Index9_Nss2Index6_JAguar
#define REG_TX_AGC_B_NSS3_INDEX3_NSS3_INDEX0_JAGUAR	rTxAGC_B_Nss3Index3_Nss3Index0_JAguar
#define REG_TX_AGC_B_NSS3_INDEX7_NSS3_INDEX4_JAGUAR	rTxAGC_B_Nss3Index7_Nss3Index4_JAguar
#define REG_TX_AGC_B_NSS3_INDEX9_NSS3_INDEX8_JAGUAR	rTxAGC_B_Nss3Index9_Nss3Index8_JAguar
#define REG_TX_AGC_B_OFDM18_OFDM6_JAGUAR	rTxAGC_B_Ofdm18_Ofdm6_JAguar
#define REG_TX_AGC_B_OFDM54_OFDM24_JAGUAR	rTxAGC_B_Ofdm54_Ofdm24_JAguar
#define REG_TX_AGC_B_RATE18_06	rTxAGC_B_Rate18_06
#define REG_TX_AGC_B_RATE54_24	rTxAGC_B_Rate54_24
#define REG_TX_AGC_C_CCK_11_CCK_1_JAGUAR	rTxAGC_C_CCK11_CCK1_JAguar
#define REG_TX_AGC_C_MCS11_MCS8_JAGUAR	rTxAGC_C_MCS11_MCS8_JAguar
#define REG_TX_AGC_C_MCS15_MCS12_JAGUAR	rTxAGC_C_MCS15_MCS12_JAguar
#define REG_TX_AGC_C_MCS19_MCS16_JAGUAR	rTxAGC_C_MCS19_MCS16_JAguar
#define REG_TX_AGC_C_MCS23_MCS20_JAGUAR	rTxAGC_C_MCS23_MCS20_JAguar
#define REG_TX_AGC_C_MCS3_MCS0_JAGUAR	rTxAGC_C_MCS3_MCS0_JAguar
#define REG_TX_AGC_C_MCS7_MCS4_JAGUAR	rTxAGC_C_MCS7_MCS4_JAguar
#define REG_TX_AGC_C_NSS1_INDEX3_NSS1_INDEX0_JAGUAR	rTxAGC_C_Nss1Index3_Nss1Index0_JAguar
#define REG_TX_AGC_C_NSS1_INDEX7_NSS1_INDEX4_JAGUAR	rTxAGC_C_Nss1Index7_Nss1Index4_JAguar
#define REG_TX_AGC_C_NSS2_INDEX1_NSS1_INDEX8_JAGUAR	rTxAGC_C_Nss2Index1_Nss1Index8_JAguar
#define REG_TX_AGC_C_NSS2_INDEX5_NSS2_INDEX2_JAGUAR	rTxAGC_C_Nss2Index5_Nss2Index2_JAguar
#define REG_TX_AGC_C_NSS2_INDEX9_NSS2_INDEX6_JAGUAR	rTxAGC_C_Nss2Index9_Nss2Index6_JAguar
#define REG_TX_AGC_C_NSS3_INDEX3_NSS3_INDEX0_JAGUAR	rTxAGC_C_Nss3Index3_Nss3Index0_JAguar
#define REG_TX_AGC_C_NSS3_INDEX7_NSS3_INDEX4_JAGUAR	rTxAGC_C_Nss3Index7_Nss3Index4_JAguar
#define REG_TX_AGC_C_NSS3_INDEX9_NSS3_INDEX8_JAGUAR	rTxAGC_C_Nss3Index9_Nss3Index8_JAguar
#define REG_TX_AGC_C_OFDM18_OFDM6_JAGUAR	rTxAGC_C_Ofdm18_Ofdm6_JAguar
#define REG_TX_AGC_C_OFDM54_OFDM24_JAGUAR	rTxAGC_C_Ofdm54_Ofdm24_JAguar
#define REG_TX_AGC_D_CCK_11_CCK_1_JAGUAR	rTxAGC_D_CCK11_CCK1_JAguar
#define REG_TX_AGC_D_MCS11_MCS8_JAGUAR	rTxAGC_D_MCS11_MCS8_JAguar
#define REG_TX_AGC_D_MCS15_MCS12_JAGUAR	rTxAGC_D_MCS15_MCS12_JAguar
#define REG_TX_AGC_D_MCS19_MCS16_JAGUAR	rTxAGC_D_MCS19_MCS16_JAguar
#define REG_TX_AGC_D_MCS23_MCS20_JAGUAR	rTxAGC_D_MCS23_MCS20_JAguar
#define REG_TX_AGC_D_MCS3_MCS0_JAGUAR	rTxAGC_D_MCS3_MCS0_JAguar
#define REG_TX_AGC_D_MCS7_MCS4_JAGUAR	rTxAGC_D_MCS7_MCS4_JAguar
#define REG_TX_AGC_D_NSS1_INDEX3_NSS1_INDEX0_JAGUAR	rTxAGC_D_Nss1Index3_Nss1Index0_JAguar
#define REG_TX_AGC_D_NSS1_INDEX7_NSS1_INDEX4_JAGUAR	rTxAGC_D_Nss1Index7_Nss1Index4_JAguar
#define REG_TX_AGC_D_NSS2_INDEX1_NSS1_INDEX8_JAGUAR	rTxAGC_D_Nss2Index1_Nss1Index8_JAguar
#define REG_TX_AGC_D_NSS2_INDEX5_NSS2_INDEX2_JAGUAR	rTxAGC_D_Nss2Index5_Nss2Index2_JAguar
#define REG_TX_AGC_D_NSS2_INDEX9_NSS2_INDEX6_JAGUAR	rTxAGC_D_Nss2Index9_Nss2Index6_JAguar
#define REG_TX_AGC_D_NSS3_INDEX3_NSS3_INDEX0_JAGUAR	rTxAGC_D_Nss3Index3_Nss3Index0_JAguar
#define REG_TX_AGC_D_NSS3_INDEX7_NSS3_INDEX4_JAGUAR	rTxAGC_D_Nss3Index7_Nss3Index4_JAguar
#define REG_TX_AGC_D_NSS3_INDEX9_NSS3_INDEX8_JAGUAR	rTxAGC_D_Nss3Index9_Nss3Index8_JAguar
#define REG_TX_AGC_D_OFDM18_OFDM6_JAGUAR	rTxAGC_D_Ofdm18_Ofdm6_JAguar
#define REG_TX_AGC_D_OFDM54_OFDM24_JAGUAR	rTxAGC_D_Ofdm54_Ofdm24_JAguar
#define REG_TX_PATH_JAGUAR	rTxPath_Jaguar
#define REG_TX_CCK_BBON	rTx_CCK_BBON
#define REG_TX_CCK_RFON	rTx_CCK_RFON
#define REG_TX_IQK	rTx_IQK
#define REG_TX_IQK_PI_A	rTx_IQK_PI_A
#define REG_TX_IQK_PI_B	rTx_IQK_PI_B
#define REG_TX_IQK_TONE_A	rTx_IQK_Tone_A
#define REG_TX_IQK_TONE_B	rTx_IQK_Tone_B
#define REG_TX_OFDM_BBON	rTx_OFDM_BBON
#define REG_TX_OFDM_RFON	rTx_OFDM_RFON
#define REG_TX_POWER_AFTER_IQK_A	rTx_Power_After_IQK_A
#define REG_TX_POWER_AFTER_IQK_B	rTx_Power_After_IQK_B
#define REG_TX_POWER_BEFORE_IQK_A	rTx_Power_Before_IQK_A
#define REG_TX_POWER_BEFORE_IQK_B	rTx_Power_Before_IQK_B
#define REG_TX_TO_RX	rTx_To_Rx
#define REG_TX_TO_TX	rTx_To_Tx
#define REG_APK	rAPK
#define REG_ANTSEL_SW_JAGUAR	r_ANTSEL_SW_Jaguar


#define REG_BCN_CTRL_1					0x0551

#define	RF_T_METER_88E				0x42


#endif
