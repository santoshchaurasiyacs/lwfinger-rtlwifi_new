#ifndef __8812A_EXT_H__
#define __8812A_EXT_H__



/* for socket */
#include <net/sock.h>
#include <net/tcp.h>
#include <linux/udp.h>
#include <linux/in.h>
#include <linux/netlink.h>



#define _FAIL 0
#define _SUCCESS 1
#define BT_INFO_LENGTH 8
#define RECV_DATA_MAX_LEN 255
#define BTINFO_WIFI_FETCH 0x23
#define BTINFO_BT_AUTO_RPT 0x27



#define CONNECT_PORT 30000
#define CONNECT_PORT_BT 30001
#define KERNEL_SOCKET_OK 0x01

#define OTHER 0
#define RX_ATTEND_ACK 1
#define RX_LEAVE_ACK 2
#define RX_BT_LEAVE 3
#define RX_INVITE_REQ 4
#define RX_ATTEND_REQ 5
#define RX_INVITE_RSP 6


#define invite_req "INVITE_REQ"
#define invite_rsp "INVITE_RSP"
#define attend_req "ATTEND_REQ"
#define attend_ack "ATTEND_ACK"
#define wifi_leave "WIFI_LEAVE"
#define leave_ack "LEAVE_ACK"
#define bt_leave "BT_LEAVE"

#define BT_INFO_NOTIFY_CMD 0x0106
#define BT_INFO_LEN 8

struct HCI_LINK_INFO {
	u16					connect_handle;
	u8					incoming_traffic_mode;
	u8					outgoing_traffic_mode;
	u8					bt_profile;
	u8					bt_corespec;
	s8					bt_rssi;
	u8					traffic_profile;
	u8					link_role;
};

#define	MAX_BT_ACL_LINK_NUM				8

struct HCI_EXT_CONFIG {
	struct HCI_LINK_INFO	acl_link[MAX_BT_ACL_LINK_NUM];
	u8	bt_operation_code;
	u16	current_connect_handle;
	u8	current_incoming_traffic_mode;
	u8	current_outgoing_traffic_mode;

	u8	number_of_acl;
	u8	number_of_sco;
	u8	current_bt_status;
	u16	hci_extension_ver;

	bool	enable_wifi_scan_notify;
};

struct HCI_PHY_LINK_BSS_INFO {
	u16 bd_cap;	/* capability information */

	/* Qos related. Added by Annie, 2005-11-01. */
	/* BSS_QOS						BssQos; */

};

enum BT_CONNECT_TYPE {
	BT_CONNECT_AUTH_REQ	= 0x00,
	BT_CONNECT_AUTH_RSP	= 0x01,
	BT_CONNECT_ASOC_REQ	= 0x02,
	BT_CONNECT_ASOC_RSP	= 0x03,
	BT_DISCONNECT		= 0x04
};


struct rtl_hci_event {
	u8		event_code;
	/* total cmd length =
	 * extension event length+1(extension event code length) */
	u8		length;
	u8		data[1]; /* byte1 is extension event code */
};


struct btinfo_8761au {
	u8 cid;
	u8 len;

	u8 connection:1;
	u8 scoe_sco:1;
	u8 inq_page:1;
	u8 acl_busy:1;
	u8 sco_busy:1;
	u8 hid:1;
	u8 a2dp:1;
	u8 ftp:1;

	u8 retry_cnt:4;
	u8 rsvd_34:1;
	u8 page:1;
	u8 trx_mask:1;
	u8 sniff_attempt:1;

	u8 rssi;

	u8 a2dp_rate:1;
	u8 re_init:1;
	u8 max_power:1;
	u8 en_ignore_wlan_act:1;
	u8 tx_power_low:1;
	u8 tx_power_high:1;
	u8 esco_sco:1;
	u8 master_slave:1;

	u8 acl_trx_tp_low;
	u8 acl_trx_tp_high;
};

#define HCIOPCODE(_OCF, _OGF)     ((_OGF)<<10|(_OCF))
#define HCIOPCODELOW(_OCF, _OGF)	(u8)(HCIOPCODE(_OCF, _OGF)&0x00ff)
#define HCIOPCODEHIGHT(_OCF, _OGF) (u8)(HCIOPCODE(_OCF, _OGF)>>8)
#define HCI_OGF(op_code)  (unsigned char)((0xFC00 & (op_code)) >> 10)
#define HCI_OCF(op_code)  (0x3FF & (op_code))


enum RTL_HCI_STATUS {
	/* Success */
	HCI_STATUS_SUCCESS					= 0x00,
	/* Unknown HCI Command */
	HCI_STATUS_UNKNOWN_HCI_CMD				= 0x01,
	/* Unknown Connection Identifier */
	HCI_STATUS_UNKNOWN_CONNECT_ID				= 0X02,
	/* Hardware Failure */
	HCI_STATUS_HW_FAIL					= 0X03,
	/* Page Timeout */
	HCI_STATUS_PAGE_TIMEOUT					= 0X04,
	/* Authentication Failure */
	HCI_STATUS_AUTH_FAIL					= 0X05,
	/* PIN or Key Missing */
	HCI_STATUS_PIN_OR_KEY_MISSING				= 0X06,
	/* Memory Capacity Exceeded */
	HCI_STATUS_MEM_CAP_EXCEED				= 0X07,
	/* Connection Timeout */
	HCI_STATUS_CONNECT_TIMEOUT				= 0X08,
	/* Connection Limit Exceeded */
	HCI_STATUS_CONNECT_LIMIT				= 0X09,
	/* Synchronous Connection Limit To A Device Exceeded */
	HCI_STATUS_SYN_CONNECT_LIMIT				= 0X0a,
	/* ACL Connection Already Exists */
	HCI_STATUS_ACL_CONNECT_EXISTS				= 0X0b,
	/* Command Disallowed */
	HCI_STATUS_CMD_DISALLOW					= 0X0c,
	/* Connection Rejected due to Limited Resources */
	HCI_STATUS_CONNECT_RJT_LIMIT_RESOURCE			= 0X0d,
	/* Connection Rejected Due To Security Reasons */
	HCI_STATUS_CONNECT_RJT_SEC_REASON			= 0X0e,
	/* Connection Rejected due to Unacceptable BD_ADDR */
	HCI_STATUS_CONNECT_RJT_UNACCEPT_BD_ADDR			= 0X0f,
	/* Connection Accept Timeout Exceeded */
	HCI_STATUS_CONNECT_ACCEPT_TIMEOUT			= 0X10,
	/* Unsupported Feature or Parameter Value */
	HCI_STATUS_UNSUPPORT_FEATURE_PARA_VALUE			= 0X11,
	/* Invalid HCI Command Parameters */
	HCI_STATUS_INVALID_HCI_CMD_PARA_VALUE			= 0X12,
	/* Remote User Terminated Connection */
	HCI_STATUS_REMOTE_USER_TERMINATE_CONNECT		= 0X13,
	/* Remote Device Terminated Connection due to Low Resources */
	HCI_STATUS_REMOTE_DEV_TERMINATE_LOW_RESOURCE		= 0X14,
	/* Remote Device Terminated Connection due to Power Off */
	HCI_STATUS_REMOTE_DEV_TERMINATE_CONNECT_POWER_OFF	= 0X15,
	/* Connection Terminated By Local Host */
	HCI_STATUS_CONNECT_TERMINATE_LOCAL_HOST			= 0X16,
	/* Repeated Attempts */
	HCI_STATUS_REPEATE_ATTEMPT				= 0X17,
	/* Pairing Not Allowed */
	HCI_STATUS_PAIR_NOT_ALLOW				= 0X18,
	/* Unknown LMP PDU */
	HCI_STATUS_UNKNOWN_LMP_PDU				= 0X19,
	/* Unsupported Remote Feature / Unsupported LMP Feature */
	HCI_STATUS_UNSUPPORT_REMOTE_LMP_FEATURE			= 0X1a,
	/* SCO Offset Rejected */
	HCI_STATUS_SOC_OFFSET_REJECT				= 0X1b,
	/* SCO Interval Rejected */
	HCI_STATUS_SOC_INTERVAL_REJECT				= 0X1c,
	/* SCO Air Mode Rejected */
	HCI_STATUS_SOC_AIR_MODE_REJECT				= 0X1d,
	/* Invalid LMP Parameters */
	HCI_STATUS_INVALID_LMP_PARA				= 0X1e,
	/* Unspecified Error */
	HCI_STATUS_UNSPECIFIC_ERROR				= 0X1f,
	/* Unsupported LMP Parameter Value */
	HCI_STATUS_UNSUPPORT_LMP_PARA_VALUE			= 0X20,
	/* Role Change Not Allowed */
	HCI_STATUS_ROLE_CHANGE_NOT_ALLOW			= 0X21,
	/* LMP Response Timeout */
	HCI_STATUS_LMP_RESPONSE_TIMEOUT				= 0X22,
	/* LMP Error Transaction Collision */
	HCI_STATUS_LMP_ERROR_TRANSACTION_COLLISION		= 0X23,
	/* LMP PDU Not Allowed */
	HCI_STATUS_LMP_PDU_NOT_ALLOW				= 0X24,
	/* Encryption Mode Not Acceptable */
	HCI_STATUS_ENCRYPTION_MODE_NOT_ALLOW			= 0X25,
	/* Link Key Can Not be Changed */
	HCI_STATUS_LINK_KEY_CAN_NOT_CHANGE			= 0X26,
	/* Requested QoS Not Supported */
	HCI_STATUS_REQUEST_QOS_NOT_SUPPORT			= 0X27,
	/* Instant Passed */
	HCI_STATUS_INSTANT_PASSED				= 0X28,
	/* Pairing With Unit Key Not Supported */
	HCI_STATUS_PAIRING_UNIT_KEY_NOT_SUPPORT			= 0X29,
	/* Different Transaction Collision */
	HCI_STATUS_DIFFERENT_TRANSACTION_COLLISION		= 0X2a,
	/* Reserved */
	HCI_STATUS_RESERVE_1					= 0X2b,
	/* QoS Unacceptable Parameter */
	HCI_STATUS_QOS_UNACCEPT_PARA				= 0X2c,
	/* QoS Rejected */
	HCI_STATUS_QOS_REJECT					= 0X2d,
	/* Channel Classification Not Supported */
	HCI_STATUS_CHNL_CLASSIFICATION_NOT_SUPPORT		= 0X2e,
	/* Insufficient Security */
	HCI_STATUS_INSUFFICIENT_SECURITY			= 0X2f,
	/* Parameter Out Of Mandatory Range */
	HCI_STATUS_PARA_OUT_OF_RANGE				= 0x30,
	/* Reserved */
	HCI_STATUS_RESERVE_2					= 0X31,
	/* Role Switch Pending */
	HCI_STATUS_ROLE_SWITCH_PENDING				= 0X32,
	/* Reserved */
	HCI_STATUS_RESERVE_3					= 0X33,
	/* Reserved Slot Violation */
	HCI_STATUS_RESERVE_SOLT_VIOLATION			= 0X34,
	/* Role Switch Failed */
	HCI_STATUS_ROLE_SWITCH_FAIL				= 0X35,
	/* Extended Inquiry Response Too Large */
	HCI_STATUS_EXTEND_INQUIRY_RSP_TOO_LARGE			= 0X36,
	/* Secure Simple Pairing Not Supported By Host. */
	HCI_STATUS_SEC_SIMPLE_PAIRING_NOT_SUPPORT		= 0X37,
	/* Host Busy - Pairing */
	HCI_STATUS_HOST_BUSY_PAIRING				= 0X38,
	/* Connection Rejected due to No Suitable Channel Found */
	HCI_STATUS_CONNECT_REJ_NOT_SUIT_CHNL_FOUND		= 0X39,
	/* CONTROLLER BUSY */
	HCI_STATUS_CONTROLLER_BUSY				= 0X3a,
};

#define HCI_EVENT_COMMAND_COMPLETE		0x0e

#define OGF_EXTENSION				0X3f
enum RTW_HCI_EXT_CMD {
	HCI_SET_ACL_LINK_DATA_FLOW_MODE		= 0x0010,
	HCI_SET_ACL_LINK_STATUS			= 0x0020,
	HCI_SET_SCO_LINK_STATUS			= 0x0030,
	HCI_SET_RSSI_VALUE			= 0x0040,
	HCI_SET_CURRENT_BLUETOOTH_STATUS	= 0x0041,

	/* The following is for RTK8723 */
	HCI_EXTENSION_VERSION_NOTIFY		= 0x0100,
	HCI_LINK_STATUS_NOTIFY			= 0x0101,
	HCI_BT_OPERATION_NOTIFY			= 0x0102,
	HCI_ENABLE_WIFI_SCAN_NOTIFY		= 0x0103,
	HCI_QUERY_RF_STATUS			= 0x0104,
	HCI_BT_ABNORMAL_NOTIFY			= 0x0105,
	HCI_BT_INFO_NOTIFY			= 0x0106,
	HCI_BT_COEX_NOTIFY			= 0x0107,
	HCI_BT_PATCH_VERSION_NOTIFY		= 0x0108,
	HCI_BT_AFH_MAP_NOTIFY			= 0x0109,
	HCI_BT_REGISTER_VALUE_NOTIFY		= 0x010a,

	/* The following is for IVT */
	HCI_WIFI_CURRENT_CHANNEL		= 0x0300,
	HCI_WIFI_CURRENT_BANDWIDTH		= 0x0301,
	HCI_WIFI_CONNECTION_STATUS		= 0x0302,
};

#define HCI_EVENT_EXTENSION_RTK			0xfe
enum RTW_HCI_EXT_EVENT {
	HCI_EVENT_EXT_WIFI_SCAN_NOTIFY		= 0x01,
	HCI_EVENT_EXT_WIFI_RF_STATUS_NOTIFY	= 0x02,
	HCI_EVENT_EXT_BT_INFO_CONTROL		= 0x03,
	HCI_EVENT_EXT_BT_COEX_CONTROL		= 0x04,
};

enum BT_TRAFFIC_MODE {
	/* Best Effort. Default. for HCRP, PAN, SDP, RFCOMM-based profiles
	 * like FTP,OPP, SPP, DUN, etc. */
	BT_MOTOR_EXT_BE		= 0x00,
	/* Guaranteed Latency. This type of traffic is used
	 * e.g. for HID and AVRCP. */
	BT_MOTOR_EXT_GUL	= 0x01,
	/* Guaranteed Bandwidth. */
	BT_MOTOR_EXT_GUB	= 0X02,
	/* Guaranteed Latency and Bandwidth. for A2DP and VDP. */
	BT_MOTOR_EXT_GULB	= 0X03,
};

enum BT_TRAFFIC_MODE_PROFILE {
	BT_PROFILE_NONE,
	BT_PROFILE_A2DP,
	BT_PROFILE_PAN	,
	BT_PROFILE_HID,
	BT_PROFILE_SCO
};

struct BT_MGNT {
	bool				bt_connect_in_progress;
	bool				loglink_in_progress;
	bool				phylink_in_progress;
	bool				phylink_in_progress_start_ll;
	u8				bt_current_phy_link_handle;
	u16				bt_current_log_link_handle;
	u8				current_connect_entry_num;
	u8				disconnect_entry_num;
	u8				current_bt_connection_cnt;
	enum BT_CONNECT_TYPE		bt_current_connect_type;
	enum BT_CONNECT_TYPE		bt_receive_connect_pkt;
	u8				bt_auth_count;
	u8				bt_asoc_count;
	bool				start_send_supervision_pkt;
	bool				bt_operation_on;
	bool				bt_need_amp_status_chg;
	bool				joiner_need_send_auth;
	struct HCI_PHY_LINK_BSS_INFO	bss_desc;
	struct HCI_EXT_CONFIG		ext_config;
	bool				need_notify_amp_no_cap;
	bool				create_support_qos;
	bool				support_profile;
	u8				bt_hannel;
	bool				check_chnl_is_suit;
	bool				bt_scan;
	bool				bt_logo_rest;
	bool				rf_status_notified;
	bool				bt_rsved_page_download;
};


#define SOCK_MSG_SIZE	32
struct socket_msg_to_send {

	struct list_head list;
	u8 msg[SOCK_MSG_SIZE];
	u8 msg_size;

};

#define SOCK_STORE_MAX 10
struct bt_coex_info {
	/* For Kernel Socket */
	struct socket *udpsock;
	struct sockaddr_in sin;
	struct sockaddr_in bt_addr;
	struct sock *sk_store;/*back up socket for UDP RX int*/

	struct list_head sock_msg_send_list;
	bool socket_thread_started;

	u32 pid;
	/* store which socket is OK */
	u8 sock_open;
	u8 bt_attend;
	u8 is_exist; /* socket exist */
	struct BT_MGNT bt_mgnt;
};

#define	PACKET_NORMAL			0
#define	PACKET_DHCP				1
#define	PACKET_ARP				2
#define	PACKET_EAPOL			3

#endif
