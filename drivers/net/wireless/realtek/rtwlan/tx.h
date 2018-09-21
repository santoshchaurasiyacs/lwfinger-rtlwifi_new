/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 2018  Realtek Corporation.
 */

#ifndef __RTW_TX_H_
#define __RTW_TX_H_

#define RTK_TX_MAX_AGG_NUM_MASK		0x1f

#define SET_TX_DESC_TXPKTSIZE(txdesc, value)                                   \
	SET_BITS_TO_LE_4BYTE((txdesc) + 0x00, 0, 16, value)
#define SET_TX_DESC_OFFSET(txdesc, value)                                      \
	SET_BITS_TO_LE_4BYTE((txdesc) + 0x00, 16, 8, value)
#define SET_TX_DESC_PKT_OFFSET(txdesc, value)                                  \
	SET_BITS_TO_LE_4BYTE((txdesc) + 0x04, 24, 5, value)
#define SET_TX_DESC_QSEL(txdesc, value)                                        \
	SET_BITS_TO_LE_4BYTE((txdesc) + 0x04, 8, 5, value)
#define SET_TX_DESC_BMC(txdesc, value)                                         \
	SET_BITS_TO_LE_4BYTE((txdesc) + 0x00, 24, 1, value)
#define SET_TX_DESC_RATE_ID(txdesc, value)                                     \
	SET_BITS_TO_LE_4BYTE((txdesc) + 0x04, 16, 5, value)
#define SET_TX_DESC_DATARATE(txdesc, value)                                    \
	SET_BITS_TO_LE_4BYTE((txdesc) + 0x10, 0, 7, value)
#define SET_TX_DESC_DISDATAFB(txdesc, value)                                   \
	SET_BITS_TO_LE_4BYTE((txdesc) + 0x0C, 10, 1, value)
#define SET_TX_DESC_USE_RATE(txdesc, value)                                    \
	SET_BITS_TO_LE_4BYTE((txdesc) + 0x0C, 8, 1, value)
#define SET_TX_DESC_SEC_TYPE(txdesc, value)                                    \
	SET_BITS_TO_LE_4BYTE((txdesc) + 0x04, 22, 2, value)
#define SET_TX_DESC_DATA_BW(txdesc, value)                                     \
	SET_BITS_TO_LE_4BYTE((txdesc) + 0x14, 5, 2, value)
#define SET_TX_DESC_SW_SEQ(txdesc, value)                                      \
	SET_BITS_TO_LE_4BYTE((txdesc) + 0x24, 12, 12, value)
#define SET_TX_DESC_MAX_AGG_NUM(txdesc, value)                                 \
	SET_BITS_TO_LE_4BYTE((txdesc) + 0x0C, 17, 5, value)
#define SET_TX_DESC_AMPDU_DENSITY(txdesc, value)                               \
	SET_BITS_TO_LE_4BYTE((txdesc) + 0x08, 20, 3, value)
#define SET_TX_DESC_DATA_STBC(txdesc, value)                                   \
	SET_BITS_TO_LE_4BYTE((txdesc) + 0x14, 8, 2, value)
#define SET_TX_DESC_DATA_LDPC(txdesc, value)                                   \
	SET_BITS_TO_LE_4BYTE((txdesc) + 0x14, 7, 1, value)
#define SET_TX_DESC_AGG_EN(txdesc, value)                                      \
	SET_BITS_TO_LE_4BYTE((txdesc) + 0x08, 12, 1, value)
#define SET_TX_DESC_LS(txdesc, value)                                          \
	SET_BITS_TO_LE_4BYTE((txdesc) + 0x00, 26, 1, value)
#define SET_TX_DESC_DATA_SHORT(txdesc, value)				       \
	SET_BITS_TO_LE_4BYTE((txdesc) + 0x14, 4, 1, value)

enum rtw_tx_desc_queue_select {
	TX_DESC_QSEL_TID0	= 0,
	TX_DESC_QSEL_TID1	= 1,
	TX_DESC_QSEL_TID2	= 2,
	TX_DESC_QSEL_TID3	= 3,
	TX_DESC_QSEL_TID4	= 4,
	TX_DESC_QSEL_TID5	= 5,
	TX_DESC_QSEL_TID6	= 6,
	TX_DESC_QSEL_TID7	= 7,
	TX_DESC_QSEL_TID8	= 8,
	TX_DESC_QSEL_TID9	= 9,
	TX_DESC_QSEL_TID10	= 10,
	TX_DESC_QSEL_TID11	= 11,
	TX_DESC_QSEL_TID12	= 12,
	TX_DESC_QSEL_TID13	= 13,
	TX_DESC_QSEL_TID14	= 14,
	TX_DESC_QSEL_TID15	= 15,
	TX_DESC_QSEL_BEACON	= 16,
	TX_DESC_QSEL_HIGH	= 17,
	TX_DESC_QSEL_MGMT	= 18,
	TX_DESC_QSEL_H2C	= 19,
};

void rtw_tx_pkt_info_update(struct rtw_dev *rtwdev,
			    struct rtw_tx_pkt_info *pkt_info,
			    struct ieee80211_tx_control *control,
			    struct sk_buff *skb);
void rtw_tx_fill_tx_desc(struct rtw_tx_pkt_info *pkt_info, struct sk_buff *skb);
void rtw_rsvd_page_pkt_info_update(struct rtw_dev *rtwdev,
				   struct rtw_tx_pkt_info *pkt_info,
				   struct sk_buff *skb);

#endif
