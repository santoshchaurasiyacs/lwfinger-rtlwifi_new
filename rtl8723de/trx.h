/******************************************************************************
 *
 * Copyright(c) 2009-2014  Realtek Corporation.
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

#ifndef __RTL8723DE_TRX_H__
#define __RTL8723DE_TRX_H__

/* TX/RX buffer descriptor */

/* for Txfilldescroptor8723de, fill the desc content. */
#define SET_TXBUFFER_DESC_LEN_WITH_OFFSET(__pdesc, __offset, __val)            \
	SET_BITS_TO_LE_4BYTE((__pdesc) + ((__offset) * 16), 0, 16, __val)
#define SET_TXBUFFER_DESC_AMSDU_WITH_OFFSET(__pdesc, __offset, __val)          \
	SET_BITS_TO_LE_4BYTE((__pdesc) + ((__offset) * 16), 31, 1, __val)
#define SET_TXBUFFER_DESC_ADD_LOW_WITH_OFFSET(__pdesc, __offset, __val)        \
	SET_BITS_TO_LE_4BYTE((__pdesc) + ((__offset) * 16) + 4, 0, 32, __val)
#define SET_TXBUFFER_DESC_ADD_HIGH_WITH_OFFSET(pbd, off, val, dma64)	       \
	(dma64 ? SET_BITS_TO_LE_4BYTE((pbd) + ((off) * 16) + 8, 0, 32, val) : 0)
#define GET_TXBUFFER_DESC_ADDR_LOW(__pdesc, __offset)                          \
	LE_BITS_TO_4BYTE((__pdesc) + ((__offset) * 16) + 4, 0, 32)
#define GET_TXBUFFER_DESC_ADDR_HIGH(pbd, off, dma64)			       \
	(dma64 ? LE_BITS_TO_4BYTE((pbd) + ((off) * 16) + 8, 0, 32) : 0)

/* Dword 0 */
#define SET_TX_BUFF_DESC_LEN_0(__pdesc, __val)                                 \
	SET_BITS_TO_LE_4BYTE(__pdesc, 0, 14, __val)
#define SET_TX_BUFF_DESC_PSB(__pdesc, __val)                                   \
	SET_BITS_TO_LE_4BYTE(__pdesc, 16, 15, __val)
#define SET_TX_BUFF_DESC_OWN(__pdesc, __val)                                   \
	SET_BITS_TO_LE_4BYTE(__pdesc, 31, 1, __val)

/* Dword 1 */
#define SET_TX_BUFF_DESC_ADDR_LOW_0(__pdesc, __val)                            \
	SET_BITS_TO_LE_4BYTE((__pdesc) + 4, 0, 32, __val)
/* Dword 2 */
#define SET_TX_BUFF_DESC_ADDR_HIGH_0(bdesc, val, dma64)			       \
	SET_TXBUFFER_DESC_ADD_HIGH_WITH_OFFSET(bdesc, 0, val, dma64)
/* Dword 3 / RESERVED 0 */

/* RX buffer  */

/* DWORD 0 */
#define SET_RX_BUFFER_DESC_DATA_LENGTH(__rx_status_desc, __val)                \
	SET_BITS_TO_LE_4BYTE(__rx_status_desc, 0, 14, __val)
#define SET_RX_BUFFER_DESC_LS(__rx_status_desc, __val)                         \
	SET_BITS_TO_LE_4BYTE(__rx_status_desc, 15, 1, __val)
#define SET_RX_BUFFER_DESC_FS(__rx_status_desc, __val)                         \
	SET_BITS_TO_LE_4BYTE(__rx_status_desc, 16, 1, __val)
#define SET_RX_BUFFER_DESC_TOTAL_LENGTH(__rx_status_desc, __val)               \
	SET_BITS_TO_LE_4BYTE(__rx_status_desc, 16, 15, __val)

#define GET_RX_BUFFER_DESC_OWN(__rx_status_desc)                               \
	LE_BITS_TO_4BYTE(__rx_status_desc, 31, 1)
#define GET_RX_BUFFER_DESC_LS(__rx_status_desc)                                \
	LE_BITS_TO_4BYTE(__rx_status_desc, 15, 1)
#define GET_RX_BUFFER_DESC_FS(__rx_status_desc)                                \
	LE_BITS_TO_4BYTE(__rx_status_desc, 16, 1)
#define GET_RX_BUFFER_DESC_TOTAL_LENGTH(__rx_status_desc)                      \
	LE_BITS_TO_4BYTE(__rx_status_desc, 16, 15)

/* DWORD 1 */
#define SET_RX_BUFFER_PHYSICAL_LOW(__rx_status_desc, __val)                    \
	SET_BITS_TO_LE_4BYTE(__rx_status_desc + 4, 0, 32, __val)

/* DWORD 2 */
#define SET_RX_BUFFER_PHYSICAL_HIGH(__rx_status_desc, __val, dma64)            \
	(dma64 ? SET_BITS_TO_LE_4BYTE((__rx_status_desc) + 8, 0, 32, __val) : 0)


#define TX_DESC_ALLOC_SIZE			64 /* sizeof(rtl_tx_desc) */
#define TX_DESC_SIZE				40
#define RX_DESC_SIZE				24
#define RX_DRV_INFO_SIZE_UNIT			8
#define MAX_RECEIVE_BUFFER_SIZE			8192

/* Dword 0 */
#define SET_TX_DESC_PKT_SIZE_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc, 0, 16, __Value)
#define SET_TX_DESC_OFFSET_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc, 16, 8, __Value)
#define SET_TX_DESC_BMC_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc, 24, 1, __Value)
#define SET_TX_DESC_HTC_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc, 25, 1, __Value)
#define SET_TX_DESC_AMSDU_PAD_EN_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc, 27, 1, __Value)
#define SET_TX_DESC_NO_ACM_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc, 29, 1, __Value)
#define SET_TX_DESC_GF_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc, 30, 1, __Value)

/* Dword 1 */
#define SET_TX_DESC_MACID_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+4, 0, 7, __Value)
#define SET_TX_DESC_QUEUE_SEL_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+4, 8, 5, __Value)
#define SET_TX_DESC_RDG_NAV_EXT_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+4, 13, 1, __Value)
#define SET_TX_DESC_LSIG_TXOP_EN_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+4, 14, 1, __Value)
#define SET_TX_DESC_PIFS_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+4, 15, 1, __Value)
#define SET_TX_DESC_RATE_ID_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+4, 16, 5, __Value)
#define SET_TX_DESC_EN_DESC_ID_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+4, 21, 1, __Value)
#define SET_TX_DESC_SEC_TYPE_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+4, 22, 2, __Value)
#define SET_TX_DESC_PKT_OFFSET_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+4, 24, 5, __Value)
#define SET_TX_DESC_MORE_DATA_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+4, 29, 1, __Value)

/* Dword 2  remove P_AID, G_ID field*/
#define SET_TX_DESC_CCA_RTS_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+8, 10, 2, __Value)
#define SET_TX_DESC_AGG_ENABLE_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+8, 12, 1, __Value)
#define SET_TX_DESC_RDG_ENABLE_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+8, 13, 1, __Value)
#define SET_TX_DESC_NULL0_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+8, 14, 1, __Value)
#define SET_TX_DESC_NULL1_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+8, 15, 1, __Value)
#define SET_TX_DESC_BK_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+8, 16, 1, __Value)
#define SET_TX_DESC_MORE_FRAG_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+8, 17, 1, __Value)
#define SET_TX_DESC_RAW_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+8, 18, 1, __Value)
#define SET_TX_DESC_CCX_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+8, 19, 1, __Value)
#define SET_TX_DESC_AMPDU_DENSITY_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+8, 20, 3, __Value)
#define SET_TX_DESC_BT_INT_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+8, 23, 1, __Value)
#define SET_TX_DESC_FTM_EN_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+8, 30, 1, __Value)

/* Dword 3 */
#define SET_TX_DESC_NAV_USE_HDR_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 5, 1, __Value)
#define SET_TX_DESC_HWSEQ_SEL_8723D(__pTxDesc, __Value) SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 6, 2, __Value)
#define SET_TX_DESC_USE_RATE_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 8, 1, __Value)
#define SET_TX_DESC_DISABLE_RTS_FB_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 9, 1, __Value)
#define SET_TX_DESC_DISABLE_FB_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 10, 1, __Value)
#define SET_TX_DESC_CTS2SELF_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 11, 1, __Value)
#define SET_TX_DESC_RTS_ENABLE_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 12, 1, __Value)
#define SET_TX_DESC_HW_RTS_ENABLE_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 13, 1, __Value)
#define SET_TX_DESC_PORT_ID_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 14, 2, __Value)
#define SET_TX_DESC_USE_MAX_LEN_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 16, 1, __Value)
#define SET_TX_DESC_MAX_AGG_NUM_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 17, 5, __Value)
#define SET_TX_DESC_AMPDU_MAX_TIME_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+12, 24, 8, __Value)

/* Dword 4 */
#define SET_TX_DESC_TX_RATE_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+16, 0, 7, __Value)
#define SET_TX_DESC_TX_TRY_RATE_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+16, 7, 1, __Value)
#define SET_TX_DESC_DATA_RATE_FB_LIMIT_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+16, 8, 5, __Value)
#define SET_TX_DESC_RTS_RATE_FB_LIMIT_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+16, 13, 4, __Value)
#define SET_TX_DESC_RETRY_LIMIT_ENABLE_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+16, 17, 1, __Value)
#define SET_TX_DESC_DATA_RETRY_LIMIT_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+16, 18, 6, __Value)
#define SET_TX_DESC_RTS_RATE_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+16, 24, 5, __Value)
#define SET_TX_DESC_PCTS_EN_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+16, 29, 1, __Value)
#define SET_TX_DESC_PCTS_MASK_IDX_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+16, 30, 2, __Value)

/* Dword 5 */
#define SET_TX_DESC_DATA_SC_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+20, 0, 4, __Value)
#define SET_TX_DESC_DATA_SHORT_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+20, 4, 1, __Value)
#define SET_TX_DESC_DATA_BW_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+20, 5, 2, __Value)
#define SET_TX_DESC_DATA_STBC_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+20, 8, 2, __Value)
#define SET_TX_DESC_RTS_STBC_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+20, 10, 2, __Value)
#define SET_TX_DESC_RTS_SHORT_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+20, 12, 1, __Value)
#define SET_TX_DESC_RTS_SC_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+20, 13, 4, __Value)
#define SET_TX_DESC_PATH_A_EN_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+20, 24, 1, __Value)
#define SET_TX_DESC_TXPWR_OF_SET_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+20, 28, 3, __Value)

/* Dword 6 */
#define SET_TX_DESC_SW_DEFINE_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+24, 0, 12, __Value)
#define SET_TX_DESC_MBSSID_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+24, 12, 4, __Value)
#define SET_TX_DESC_RF_SEL_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+24, 16, 3, __Value)

/* Dword 7 */
#define SET_TX_DESC_TX_BUFFER_SIZE_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+28, 0, 16, __Value)
#define SET_TX_DESC_TX_DESC_CHECKSUM_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+28, 0, 16, __Value)
#define SET_TX_DESC_TX_TIMESTAMP_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+28, 6, 18, __Value)
#define SET_TX_DESC_USB_TXAGG_NUM_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+28, 24, 8, __Value)

/* Dword 8 */
#define SET_TX_DESC_RTS_RC_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+32, 0, 6, __Value)
#define SET_TX_DESC_BAR_RC_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+32, 6, 2, __Value)
#define SET_TX_DESC_DATA_RC_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+32, 8, 6, __Value)
#define SET_TX_DESC_HWSEQ_EN_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+32, 15, 1, __Value)
#define SET_TX_DESC_NEXTHEADPAGE_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+32, 16, 8, __Value)
#define SET_TX_DESC_TAILPAGE_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+32, 24, 8, __Value)

/* Dword 9 */
#define SET_TX_DESC_PADDING_LEN_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+36, 0, 11, __Value)
#define SET_TX_DESC_SEQ_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+36, 12, 12, __Value)
#define SET_TX_DESC_FINAL_DATA_RATE_8723D(__pTxDesc, __Value) \
	SET_BITS_TO_LE_4BYTE(__pTxDesc+36, 24, 8, __Value)

/* DWORD 0 */
#define GET_RX_DESC_PKT_LEN_8723D(__pRxStatusDesc) \
	LE_BITS_TO_4BYTE(__pRxStatusDesc, 0, 14)
#define GET_RX_DESC_CRC32_8723D(__pRxStatusDesc) \
	LE_BITS_TO_4BYTE(__pRxStatusDesc, 14, 1)
#define GET_RX_DESC_ICV_8723D(__pRxStatusDesc) \
	LE_BITS_TO_4BYTE(__pRxStatusDesc, 15, 1)
#define GET_RX_DESC_DRV_INFO_SIZE_8723D(__pRxStatusDesc) \
	LE_BITS_TO_4BYTE(__pRxStatusDesc, 16, 4)
#define GET_RX_DESC_SECURITY_8723D(__pRxStatusDesc) \
	LE_BITS_TO_4BYTE(__pRxStatusDesc, 20, 3)
#define GET_RX_DESC_QOS_8723D(__pRxStatusDesc) \
	LE_BITS_TO_4BYTE(__pRxStatusDesc, 23, 1)
#define GET_RX_DESC_SHIFT_8723D(__pRxStatusDesc) \
	LE_BITS_TO_4BYTE(__pRxStatusDesc, 24, 2)
#define GET_RX_DESC_PHYST_8723D(__pRxStatusDesc) \
	LE_BITS_TO_4BYTE(__pRxStatusDesc, 26, 1)
#define GET_RX_DESC_SWDEC_8723D(__pRxStatusDesc) \
	LE_BITS_TO_4BYTE(__pRxStatusDesc, 27, 1)
#define GET_RX_DESC_EOR_8723D(__pRxStatusDesc) \
	LE_BITS_TO_4BYTE(__pRxStatusDesc, 30, 1)
#define GET_RX_DESC_OWN_8723D(__pRxStatusDesc) \
	LE_BITS_TO_4BYTE(__pRxStatusDesc, 31, 1)

/* DWORD 1 */
#define GET_RX_DESC_MACID_8723D(__pRxDesc) \
	LE_BITS_TO_4BYTE(__pRxDesc+4, 0, 7)
#define GET_RX_DESC_TID_8723D(__pRxDesc) \
	LE_BITS_TO_4BYTE(__pRxDesc+4, 8, 4)
#define GET_RX_DESC_AMSDU_8723D(__pRxDesc) \
	LE_BITS_TO_4BYTE(__pRxDesc+4, 13, 1)
#define GET_RX_DESC_RXID_MATCH_8723D(__pRxDesc) \
	LE_BITS_TO_4BYTE(__pRxDesc+4, 14, 1)
#define GET_RX_DESC_PAGGR_8723D(__pRxDesc) \
	LE_BITS_TO_4BYTE(__pRxDesc+4, 15, 1)
#define GET_RX_DESC_A1_FIT_8723D(__pRxDesc) \
	LE_BITS_TO_4BYTE(__pRxDesc+4, 16, 4)
#define GET_RX_DESC_CHKERR_8723D(__pRxDesc) \
	LE_BITS_TO_4BYTE(__pRxDesc+4, 20, 1)
#define GET_RX_DESC_IPVER_8723D(__pRxDesc) \
	LE_BITS_TO_4BYTE(__pRxDesc+4, 21, 1)
#define GET_RX_DESC_IS_TCPUDP__8723D(__pRxDesc) \
	LE_BITS_TO_4BYTE(__pRxDesc+4, 22, 1)
#define GET_RX_DESC_CHK_VLD_8723D(__pRxDesc) \
	LE_BITS_TO_4BYTE(__pRxDesc+4, 23, 1)
#define GET_RX_DESC_PAM_8723D(__pRxDesc) \
	LE_BITS_TO_4BYTE(__pRxDesc+4, 24, 1)
#define GET_RX_DESC_PWR_8723D(__pRxDesc) \
	LE_BITS_TO_4BYTE(__pRxDesc+4, 25, 1)
#define GET_RX_DESC_MORE_DATA_8723D(__pRxDesc) \
	LE_BITS_TO_4BYTE(__pRxDesc+4, 26, 1)
#define GET_RX_DESC_MORE_FRAG_8723D(__pRxDesc) \
	LE_BITS_TO_4BYTE(__pRxDesc+4, 27, 1)
#define GET_RX_DESC_TYPE_8723D(__pRxDesc) \
	LE_BITS_TO_4BYTE(__pRxDesc+4, 28, 2)
#define GET_RX_DESC_MC_8723D(__pRxDesc) \
	LE_BITS_TO_4BYTE(__pRxDesc+4, 30, 1)
#define GET_RX_DESC_BC_8723D(__pRxDesc) \
	LE_BITS_TO_4BYTE(__pRxDesc+4, 31, 1)

/* DWORD 2 */
#define GET_RX_DESC_SEQ_8723D(__pRxStatusDesc) \
	LE_BITS_TO_4BYTE(__pRxStatusDesc+8, 0, 12)
#define GET_RX_DESC_FRAG_8723D(__pRxStatusDesc) \
	LE_BITS_TO_4BYTE(__pRxStatusDesc+8, 12, 4)
#define GET_RX_DESC_RX_IS_QOS_8723D(__pRxStatusDesc) \
	LE_BITS_TO_4BYTE(__pRxStatusDesc+8, 16, 1)
#define GET_RX_DESC_WLANHD_IV_LEN_8723D(__pRxStatusDesc) \
	LE_BITS_TO_4BYTE(__pRxStatusDesc+8, 18, 6)
#define GET_RX_DESC_RPT_SEL_8723D(__pRxStatusDesc) \
	LE_BITS_TO_4BYTE(__pRxStatusDesc+8, 28, 1)
#define GET_RX_DESC_FCS_OK_8723D(__pRxStatusDesc) \
	LE_BITS_TO_4BYTE(__pRxStatusDesc+8, 31, 1)

/* DWORD 3 */
#define GET_RX_DESC_RX_RATE_8723D(__pRxStatusDesc) \
	LE_BITS_TO_4BYTE(__pRxStatusDesc+12, 0, 7)
#define GET_RX_DESC_RXHT_8723D(__pdesc)		\
	LE_BITS_TO_4BYTE(__pdesc+12, 6, 1)
#define GET_RX_DESC_HTC_8723D(__pRxStatusDesc) \
	LE_BITS_TO_4BYTE(__pRxStatusDesc+12, 10, 1)
#define GET_RX_DESC_EOSP_8723D(__pRxStatusDesc) \
	LE_BITS_TO_4BYTE(__pRxStatusDesc+12, 11, 1)
#define GET_RX_DESC_BSSID_FIT_8723D(__pRxStatusDesc) \
	LE_BITS_TO_4BYTE(__pRxStatusDesc+12, 12, 2)
#ifdef CONFIG_USB_RX_AGGREGATION
#define GET_RX_DESC_USB_AGG_PKTNUM_8723D(__pRxStatusDesc) \
	LE_BITS_TO_4BYTE(__pRxStatusDesc+12, 16, 8)
#endif
#define GET_RX_DESC_PATTERN_MATCH_8723D(__pRxDesc) \
	LE_BITS_TO_4BYTE(__pRxDesc+12, 29, 1)
#define GET_RX_DESC_UNICAST_MATCH_8723D(__pRxDesc) \
	LE_BITS_TO_4BYTE(__pRxDesc+12, 30, 1)
#define GET_RX_DESC_MAGIC_MATCH_8723D(__pRxDesc) \
	LE_BITS_TO_4BYTE(__pRxDesc+12, 31, 1)

/* DWORD 4 */
#define GET_RX_DESC_MATCH_ID_8723D(__pRxDesc) \
	LE_BITS_TO_4BYTE(__pRxDesc+16, 0, 7)

/* DWORD 5 */
#define GET_RX_DESC_TSFL_8723D(__pRxStatusDesc) \
	LE_BITS_TO_4BYTE(__pRxStatusDesc+20, 0, 32)

#if 0
/* TX report 2 format in Rx desc*/
#define GET_RX_RPT2_DESC_PKT_LEN(__rxstatusdesc)	\
	LE_BITS_TO_4BYTE(__rxstatusdesc, 0, 9)
#define GET_RX_RPT2_DESC_MACID_VALID_1(__rxstatusdesc)	\
	LE_BITS_TO_4BYTE(__rxstatusdesc+16, 0, 32)
#define GET_RX_RPT2_DESC_MACID_VALID_2(__rxstatusdesc)	\
	LE_BITS_TO_4BYTE(__rxstatusdesc+20, 0, 32)
#endif

#define SET_EARLYMODE_PKTNUM(__paddr, __value)		\
	SET_BITS_TO_LE_4BYTE(__paddr, 0, 4, __value)
#define SET_EARLYMODE_LEN0(__paddr, __value)		\
	SET_BITS_TO_LE_4BYTE(__paddr, 4, 12, __value)
#define SET_EARLYMODE_LEN1(__paddr, __value)		\
	SET_BITS_TO_LE_4BYTE(__paddr, 16, 12, __value)
#define SET_EARLYMODE_LEN2_1(__paddr, __value)		\
	SET_BITS_TO_LE_4BYTE(__paddr, 28, 4, __value)
#define SET_EARLYMODE_LEN2_2(__paddr, __value)		\
	SET_BITS_TO_LE_4BYTE(__paddr+4, 0, 8, __value)
#define SET_EARLYMODE_LEN3(__paddr, __value)		\
	SET_BITS_TO_LE_4BYTE(__paddr+4, 8, 12, __value)
#define SET_EARLYMODE_LEN4(__paddr, __value)		\
	SET_BITS_TO_LE_4BYTE(__paddr+4, 20, 12, __value)

#define CLEAR_PCI_TX_DESC_CONTENT(__pdesc, _size)		\
do {								\
	if (_size > TX_DESC_SIZE)				\
		memset(__pdesc, 0, TX_DESC_SIZE);		\
	else							\
		memset(__pdesc, 0, _size);			\
} while (0)

struct phy_rx_agc_info_t {
	#ifdef __LITTLE_ENDIAN
		u8 gain:7, trsw:1;
	#else
		u8 trsw:1, gain:7;
	#endif
};
struct phy_status_rpt {
	struct phy_rx_agc_info_t path_agc[2];
	u8 ch_corr[2];
	u8 cck_sig_qual_ofdm_pwdb_all;
	u8 cck_agc_rpt_ofdm_cfosho_a;
	u8 cck_rpt_b_ofdm_cfosho_b;
	u8 rsvd_1;/* ch_corr_msb; */
	u8 noise_power_db_msb;
	s8 path_cfotail[2];
	u8 pcts_mask[2];
	s8 stream_rxevm[2];
	u8 path_rxsnr[2];
	u8 noise_power_db_lsb;
	u8 rsvd_2[3];
	u8 stream_csi[2];
	u8 stream_target_csi[2];
	u8 sig_evm;
	u8 rsvd_3;
#ifdef __LITTLE_ENDIAN
	u8 antsel_rx_keep_2:1;	/*ex_intf_flg:1;*/
	u8 sgi_en:1;
	u8 rxsc:2;
	u8 idle_long:1;
	u8 r_ant_train_en:1;
	u8 ant_sel_b:1;
	u8 ant_sel:1;
#else	/* _BIG_ENDIAN_	*/
	u8 ant_sel:1;
	u8 ant_sel_b:1;
	u8 r_ant_train_en:1;
	u8 idle_long:1;
	u8 rxsc:2;
	u8 sgi_en:1;
	u8 antsel_rx_keep_2:1;	/*ex_intf_flg:1;*/
#endif
} __packed;

#if 0
struct rx_fwinfo_8723de {
	u8 gain_trsw[2];
	u16 chl_num:10;
	u16 sub_chnl:4;
	u16 r_rfmod:2;
	u8 pwdb_all;
	u8 cfosho[4];
	u8 cfotail[4];
	s8 rxevm[2];
	s8 rxsnr[2];
	u8 pcts_msk_rpt[2];
	u8 pdsnr[2];
	u8 csi_current[2];
	u8 rx_gain_c;
	u8 rx_gain_d;
	u8 sigevm;
	u8 resvd_0;
	u8 antidx_anta:3;
	u8 antidx_antb:3;
	u8 resvd_1:2;
} __packed;
#endif

struct tx_desc_8723de {
	u32 pktsize:16;
	u32 offset:8;
	u32 bmc:1;
	u32 htc:1;
	u32 lastseg:1;
	u32 firstseg:1;
	u32 linip:1;
	u32 noacm:1;
	u32 gf:1;
	u32 own:1;

	u32 macid:6;
	u32 rsvd0:2;
	u32 queuesel:5;
	u32 rd_nav_ext:1;
	u32 lsig_txop_en:1;
	u32 pifs:1;
	u32 rateid:4;
	u32 nav_usehdr:1;
	u32 en_descid:1;
	u32 sectype:2;
	u32 pktoffset:8;

	u32 rts_rc:6;
	u32 data_rc:6;
	u32 agg_en:1;
	u32 rdg_en:1;
	u32 bar_retryht:2;
	u32 agg_break:1;
	u32 morefrag:1;
	u32 raw:1;
	u32 ccx:1;
	u32 ampdudensity:3;
	u32 bt_int:1;
	u32 ant_sela:1;
	u32 ant_selb:1;
	u32 txant_cck:2;
	u32 txant_l:2;
	u32 txant_ht:2;

	u32 nextheadpage:8;
	u32 tailpage:8;
	u32 seq:12;
	u32 cpu_handle:1;
	u32 tag1:1;
	u32 trigger_int:1;
	u32 hwseq_en:1;

	u32 rtsrate:5;
	u32 apdcfe:1;
	u32 qos:1;
	u32 hwseq_ssn:1;
	u32 userrate:1;
	u32 dis_rtsfb:1;
	u32 dis_datafb:1;
	u32 cts2self:1;
	u32 rts_en:1;
	u32 hwrts_en:1;
	u32 portid:1;
	u32 pwr_status:3;
	u32 waitdcts:1;
	u32 cts2ap_en:1;
	u32 txsc:2;
	u32 stbc:2;
	u32 txshort:1;
	u32 txbw:1;
	u32 rtsshort:1;
	u32 rtsbw:1;
	u32 rtssc:2;
	u32 rtsstbc:2;

	u32 txrate:6;
	u32 shortgi:1;
	u32 ccxt:1;
	u32 txrate_fb_lmt:5;
	u32 rtsrate_fb_lmt:4;
	u32 retrylmt_en:1;
	u32 txretrylmt:6;
	u32 usb_txaggnum:8;

	u32 txagca:5;
	u32 txagcb:5;
	u32 usemaxlen:1;
	u32 maxaggnum:5;
	u32 mcsg1maxlen:4;
	u32 mcsg2maxlen:4;
	u32 mcsg3maxlen:4;
	u32 mcs7sgimaxlen:4;

	u32 txbuffersize:16;
	u32 sw_offset30:8;
	u32 sw_offset31:4;
	u32 rsvd1:1;
	u32 antsel_c:1;
	u32 null_0:1;
	u32 null_1:1;

	u32 txbuffaddr;
	u32 txbufferaddr64;
	u32 nextdescaddress;
	u32 nextdescaddress64;

	u32 reserve_pass_pcie_mm_limit[4];
} __packed;

struct rx_desc_8723de {
	u32 length:14;
	u32 crc32:1;
	u32 icverror:1;
	u32 drv_infosize:4;
	u32 security:3;
	u32 qos:1;
	u32 shift:2;
	u32 phystatus:1;
	u32 swdec:1;
	u32 lastseg:1;
	u32 firstseg:1;
	u32 eor:1;
	u32 own:1;

	u32 macid:6;
	u32 tid:4;
	u32 hwrsvd:5;
	u32 paggr:1;
	u32 faggr:1;
	u32 a1_fit:4;
	u32 a2_fit:4;
	u32 pam:1;
	u32 pwr:1;
	u32 moredata:1;
	u32 morefrag:1;
	u32 type:2;
	u32 mc:1;
	u32 bc:1;

	u32 seq:12;
	u32 frag:4;
	u32 nextpktlen:14;
	u32 nextind:1;
	u32 rsvd:1;

	u32 rxmcs:6;
	u32 rxht:1;
	u32 amsdu:1;
	u32 splcp:1;
	u32 bandwidth:1;
	u32 htc:1;
	u32 tcpchk_rpt:1;
	u32 ipcchk_rpt:1;
	u32 tcpchk_valid:1;
	u32 hwpcerr:1;
	u32 hwpcind:1;
	u32 iv0:16;

	u32 iv1;

	u32 tsfl;

	u32 bufferaddress;
	u32 bufferaddress64;

} __packed;

void rtl8723de_rx_check_dma_ok(struct ieee80211_hw *hw, u8 *header_desc,
			       u8 queue_index);
u16 rtl8723de_rx_desc_buff_remained_cnt(struct ieee80211_hw *hw,
					u8 queue_index);
void rtl8723de_tx_fill_desc(struct ieee80211_hw *hw,
			    struct ieee80211_hdr *hdr,
			    u8 *pdesc_tx, u8 *txbd,
			    struct ieee80211_tx_info *info,
			    struct ieee80211_sta *sta, struct sk_buff *skb,
			    u8 hw_queue, struct rtl_tcb_desc *ptcb_desc);
bool rtl8723de_rx_query_desc(struct ieee80211_hw *hw,
			     struct rtl_stats *status,
			     struct ieee80211_rx_status *rx_status,
			     u8 *pdesc, struct sk_buff *skb);
void rtl8723de_set_desc(struct ieee80211_hw *hw, u8 *pdesc,
			bool istx, u8 desc_name, u8 *val);
u64 rtl8723de_get_desc(struct ieee80211_hw *hw,
		       u8 *pdesc, bool istx, u8 desc_name);
bool rtl8723de_is_tx_desc_closed(struct ieee80211_hw *hw,
				 u8 hw_queue, u16 index);
void rtl8723de_tx_polling(struct ieee80211_hw *hw, u8 hw_queue);
void rtl8723de_tx_fill_special_desc(struct ieee80211_hw *hw, u8 *pdesc,
				    u8 *pbd_desc, struct sk_buff *skb,
				    u8 hw_queue);
u32 rtl8723de_rx_command_packet(struct ieee80211_hw *hw,
				const struct rtl_stats *status,
				struct sk_buff *skb);
#endif
