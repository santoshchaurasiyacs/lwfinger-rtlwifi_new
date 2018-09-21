/* SPDX-License-Identifier: GPL-2.0 */
/* Copyright(c) 2018  Realtek Corporation.
 */

#ifndef __RTW_FW_H_
#define __RTW_FW_H_

#define RTW_H2C_CLASS_OFFSET	5

#define H2C_PKT_SIZE		32
#define H2C_PKT_HDR_SIZE	8

/* FW bin information */
#define FW_HDR_SIZE			64
#define FW_HDR_CHKSUM_SIZE		8
#define FW_HDR_VERSION			4
#define FW_HDR_SUBVERSION		6
#define FW_HDR_SUBINDEX			7
#define FW_HDR_MONTH			16
#define FW_HDR_DATE			17
#define FW_HDR_HOUR			18
#define FW_HDR_MIN			19
#define FW_HDR_YEAR			20
#define FW_HDR_MEM_USAGE		24
#define FW_HDR_H2C_FMT_VER		28
#define FW_HDR_DMEM_ADDR		32
#define FW_HDR_DMEM_SIZE		36
#define FW_HDR_IMEM_SIZE		48
#define FW_HDR_EMEM_SIZE		52
#define FW_HDR_EMEM_ADDR		56
#define FW_HDR_IMEM_ADDR		60

#define FIFO_PAGE_SIZE_SHIFT		12
#define FIFO_PAGE_SIZE			4096
#define RSVD_PAGE_START_ADDR		0x780
#define FIFO_DUMP_ADDR			0x8000

enum rtw_c2h_cmd_id {
	C2H_BT_INFO = 0x09,
	C2H_HW_FEATURE_REPORT = 0x19,
	C2H_HW_FEATURE_DUMP = 0xfd,
	C2H_HALMAC = 0xff,
};

struct rtw_c2h_cmd {
	u8 id;
	u8 seq;
	u8 payload[0];
} __packed;

enum rtw_rsvd_packet_type {
	RSVD_BEACON,
	RSVD_PS_POLL,
	RSVD_PROBE_RESP,
	RSVD_NULL,
	RSVD_QOS_NULL,
};

enum rtw_fw_rf_type {
	FW_RF_1T2R = 0,
	FW_RF_2T4R = 1,
	FW_RF_2T2R = 2,
	FW_RF_2T3R = 3,
	FW_RF_1T1R = 4,
	FW_RF_2T2R_GREEN = 5,
	FW_RF_3T3R = 6,
	FW_RF_3T4R = 7,
	FW_RF_4T4R = 8,
	FW_RF_MAX_TYPE = 0xF,
};

struct rtw_general_info {
	u8 rfe_type;
	u8 rf_type;
	u8 tx_ant_status;
	u8 rx_ant_status;
};

struct rtw_iqk_para {
	u8 clear;
	u8 segment_iqk;
};

struct rtw_rsvd_page {
	struct list_head list;
	struct sk_buff *skb;
	enum rtw_rsvd_packet_type type;
	u8 page;
	bool add_txdesc;
};

#define SUB_CMD_ID_GENERAL_INFO 0X0D
#define SUB_CMD_ID_PHYDM_INFO 0X11
#define SUB_CMD_ID_IQK 0X0E

#define CMD_ID_RSSI_MONITOR	0x02
#define CLASS_RSSI_MONITOR	0x02
#define CMD_ID_RA_INFO		0x00
#define CLASS_RA_INFO		0x02
#define CMD_ID_MEDIA_STATUS_RPT	0X01
#define CLASS_MEDIA_STATUS_RPT	0X0
#define CMD_ID_SET_PWR_MODE	0X00
#define CLASS_SET_PWR_MODE	0X01
#define CMD_ID_RSVD_PAGE	0X0
#define CLASS_RSVD_PAGE		0X0

#define FW_OFFLOAD_H2C_SET_SEQ_NUM(h2c_pkt, value)                             \
	SET_BITS_TO_LE_4BYTE((h2c_pkt) + 0X04, 16, 16, value)
#define GENERAL_INFO_SET_FW_TX_BOUNDARY(h2c_pkt, value)                        \
	SET_BITS_TO_LE_4BYTE((h2c_pkt) + 0X08, 16, 8, value)
#define FW_OFFLOAD_H2C_SET_TOTAL_LEN(h2c_pkt, value)                           \
	SET_BITS_TO_LE_4BYTE((h2c_pkt) + 0X04, 0, 16, value)
#define FW_OFFLOAD_H2C_SET_SUB_CMD_ID(h2c_pkt, value)                          \
	SET_BITS_TO_LE_4BYTE((h2c_pkt) + 0X00, 16, 16, value)
#define FW_OFFLOAD_H2C_SET_CATEGORY(h2c_pkt, value)                            \
	SET_BITS_TO_LE_4BYTE((h2c_pkt) + 0X00, 0, 7, value)
#define FW_OFFLOAD_H2C_SET_CMD_ID(h2c_pkt, value)                              \
	SET_BITS_TO_LE_4BYTE((h2c_pkt) + 0X00, 8, 8, value)
#define PHYDM_INFO_SET_REF_TYPE(h2c_pkt, value)                                \
	SET_BITS_TO_LE_4BYTE((h2c_pkt) + 0X08, 0, 8, value)
#define PHYDM_INFO_SET_RF_TYPE(h2c_pkt, value)                                 \
	SET_BITS_TO_LE_4BYTE((h2c_pkt) + 0X08, 8, 8, value)
#define PHYDM_INFO_SET_CUT_VER(h2c_pkt, value)                                 \
	SET_BITS_TO_LE_4BYTE((h2c_pkt) + 0X08, 16, 8, value)
#define PHYDM_INFO_SET_RX_ANT_STATUS(h2c_pkt, value)                           \
	SET_BITS_TO_LE_4BYTE((h2c_pkt) + 0X08, 24, 4, value)
#define PHYDM_INFO_SET_TX_ANT_STATUS(h2c_pkt, value)                           \
	SET_BITS_TO_LE_4BYTE((h2c_pkt) + 0X08, 28, 4, value)
#define IQK_SET_CLEAR(h2c_pkt, value)                                          \
	SET_BITS_TO_LE_4BYTE((h2c_pkt) + 0X08, 0, 1, value)
#define IQK_SET_SEGMENT_IQK(h2c_pkt, value)                                    \
	SET_BITS_TO_LE_4BYTE((h2c_pkt) + 0X08, 1, 1, value)
#define MEDIA_STATUS_RPT_SET_CMD_ID(h2c_pkt, value)                            \
	SET_BITS_TO_LE_4BYTE((h2c_pkt) + 0X00, 0, 5, value)
#define MEDIA_STATUS_RPT_SET_CLASS(h2c_pkt, value)                             \
	SET_BITS_TO_LE_4BYTE((h2c_pkt) + 0X00, 5, 3, value)
#define MEDIA_STATUS_RPT_SET_OP_MODE(h2c_pkt, value)                           \
	SET_BITS_TO_LE_4BYTE((h2c_pkt) + 0X00, 8, 1, value)
#define MEDIA_STATUS_RPT_SET_MACID(h2c_pkt, value)                             \
	SET_BITS_TO_LE_4BYTE((h2c_pkt) + 0X00, 16, 8, value)
#define SET_PWR_MODE_SET_CMD_ID(h2c_pkt, value)                                \
	SET_BITS_TO_LE_4BYTE((h2c_pkt) + 0X00, 0, 5, value)
#define SET_PWR_MODE_SET_CLASS(h2c_pkt, value)                                 \
	SET_BITS_TO_LE_4BYTE((h2c_pkt) + 0X00, 5, 3, value)
#define SET_PWR_MODE_SET_MODE(h2c_pkt, value)                                  \
	SET_BITS_TO_LE_4BYTE((h2c_pkt) + 0X00, 8, 7, value)
#define SET_PWR_MODE_SET_RLBM(h2c_pkt, value)                                  \
	SET_BITS_TO_LE_4BYTE((h2c_pkt) + 0X00, 16, 4, value)
#define SET_PWR_MODE_SET_SMART_PS(h2c_pkt, value)                              \
	SET_BITS_TO_LE_4BYTE((h2c_pkt) + 0X00, 20, 4, value)
#define SET_PWR_MODE_SET_AWAKE_INTERVAL(h2c_pkt, value)                        \
	SET_BITS_TO_LE_4BYTE((h2c_pkt) + 0X00, 24, 8, value)
#define SET_PWR_MODE_SET_PORT_ID(h2c_pkt, value)                               \
	SET_BITS_TO_LE_4BYTE((h2c_pkt) + 0X04, 5, 3, value)
#define SET_PWR_MODE_SET_PWR_STATE(h2c_pkt, value)                             \
	SET_BITS_TO_LE_4BYTE((h2c_pkt) + 0X04, 8, 8, value)
#define RSVD_PAGE_SET_CMD_ID(h2c_pkt, value)                                   \
	SET_BITS_TO_LE_4BYTE((h2c_pkt) + 0X00, 0, 5, value)
#define RSVD_PAGE_SET_CLASS(h2c_pkt, value)                                    \
	SET_BITS_TO_LE_4BYTE((h2c_pkt) + 0X00, 5, 3, value)

void rtw_fw_c2h_cmd_handle(struct rtw_dev *rtwdev, struct sk_buff *skb);
void rtw_fw_send_general_info(struct rtw_dev *rtwdev,
			      struct rtw_general_info *info);
void rtw_fw_send_phydm_info(struct rtw_dev *rtwdev,
			    struct rtw_general_info *info);
void rtw_fw_do_iqk(struct rtw_dev *rtwdev, struct rtw_iqk_para *para);
void rtw_fw_fill_h2c_cmd(struct rtw_dev *rtwdev, u8 cmd, u8 len, u8 *buf);
void rtw_fw_set_pwr_mode(struct rtw_dev *rtwdev);
void rtw_fw_send_rssi_info(struct rtw_dev *rtwdev, struct rtw_sta_info *si);
void rtw_fw_send_ra_info(struct rtw_dev *rtwdev, struct rtw_sta_info *si);
void rtw_fw_media_status_report(struct rtw_dev *rtwdev, u8 mac_id, bool conn);
void rtw_add_rsvd_page(struct rtw_dev *rtwdev, enum rtw_rsvd_packet_type type,
		       bool txdesc);
int rtw_fw_write_data_rsvd_page(struct rtw_dev *rtwdev, u16 pg_addr,
				u8 *buf, u32 size);
void rtw_reset_rsvd_page(struct rtw_dev *rtwdev);
int rtw_fw_download_rsvd_page(struct rtw_dev *rtwdev,
			      struct ieee80211_vif *vif);
int rtw_dump_drv_rsvd_page(struct rtw_dev *rtwdev,
			   u32 offset, u32 size, u32 *buf);
#endif
