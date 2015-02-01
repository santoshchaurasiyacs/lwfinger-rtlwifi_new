/******************************************************************************
 *
 * Copyright(c) 2009-2010  Realtek Corporation.
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
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
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

#ifndef __RTL92E_DEF_H__
#define __RTL92E_DEF_H__


#define HAL_PRIME_CHNL_OFFSET_DONT_CARE			0
#define HAL_PRIME_CHNL_OFFSET_LOWER			1
#define HAL_PRIME_CHNL_OFFSET_UPPER			2

#define RX_MPDU_QUEUE					0

#define IS_HT_RATE(_rate)	\
	(_rate >= DESC_RATEMCS0)
#define IS_CCK_RATE(_rate)	\
	(_rate >= DESC_RATE1M && _rate <= DESC_RATE11M)
#define IS_OFDM_RATE(_rate)	\
	(_rate >= DESC_RATE6M && _rate <= DESC_RATE54M)


enum version_8192e {
	VERSION_TEST_CHIP_2T2R_8192E = 0x0024,
	VERSION_NORMAL_CHIP_2T2R_8192E = 0x102C,
	VERSION_UNKNOWN = 0xFF,
};

enum rx_packet_type {
	NORMAL_RX,
	TX_REPORT1,
	TX_REPORT2,
	HIS_REPORT,
	C2H_PACKET,
};

enum rtl_desc_qsel {
	QSLT_BK = 0x2,
	QSLT_BE = 0x0,
	QSLT_VI = 0x5,
	QSLT_VO = 0x7,
	QSLT_BEACON = 0x10,
	QSLT_HIGH = 0x11,
	QSLT_MGNT = 0x12,
	QSLT_CMD = 0x13,
};

#endif
