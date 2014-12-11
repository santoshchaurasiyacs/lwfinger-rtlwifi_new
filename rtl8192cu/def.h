/******************************************************************************
 *
 * Copyright(c) 2009-2012  Realtek Corporation.
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

#include "../rtl8192ce/def.h"

/*-------------------------------------------------------------------------
 *	Chip specific
 *-------------------------------------------------------------------------*/
#define CHIP_8723			BIT(2) /* RTL8723 With BT feature */
#define CHIP_8723_DRV_REV		BIT(3) /* RTL8723 Driver Revised */
#define NORMAL_CHIP			BIT(4)
#define CHIP_VENDOR_UMC			BIT(5)
#define CHIP_VENDOR_UMC_B_CUT		BIT(6)

#define IS_8723_SERIES(version)		\
	(((version) & CHIP_8723) ? true : false)

#define IS_92C_1T2R(version)		\
	(((version) & CHIP_92C) && ((version) & CHIP_92C_1T2R))

#define IS_VENDOR_UMC(version)		\
	(((version) & CHIP_VENDOR_UMC) ? true : false)

#define IS_VENDOR_8723_A_CUT(version)	\
	(((version) & CHIP_VENDOR_UMC) ? (((version) & (BIT(6))) ? \
	false : true) : false)

#define RX_HAL_IS_CCK_RATE(_pdesc)\
	(_pdesc->rxmcs == DESC_RATE1M ||		\
	 _pdesc->rxmcs == DESC_RATE2M ||		\
	 _pdesc->rxmcs == DESC_RATE5_5M ||		\
	 _pdesc->rxmcs == DESC_RATE11M)

#define IS_HARDWARE_TYPE_8192CE(rtlhal)			\
	(rtlhal->hw_type == HARDWARE_TYPE_RTL8192CE)
#define IS_HARDWARE_TYPE_8192CU(rtlhal)			\
	(rtlhal->hw_type == HARDWARE_TYPE_RTL8192CU)

#if 0
enum version_8192c {
	VERSION_A_CHIP_92C = 0x01,
	VERSION_A_CHIP_88C = 0x00,
	VERSION_B_CHIP_92C = 0x11,
	VERSION_B_CHIP_88C = 0x10,
	VERSION_TEST_CHIP_88C = 0x00,
	VERSION_TEST_CHIP_92C = 0x01,
	VERSION_NORMAL_TSMC_CHIP_88C = 0x10,
	VERSION_NORMAL_TSMC_CHIP_92C = 0x11,
	VERSION_NORMAL_TSMC_CHIP_92C_1T2R = 0x13,
	VERSION_NORMAL_UMC_CHIP_88C_A_CUT = 0x30,
	VERSION_NORMAL_UMC_CHIP_92C_A_CUT = 0x31,
	VERSION_NORMAL_UMC_CHIP_92C_1T2R_A_CUT = 0x33,
	VERSION_NORMA_UMC_CHIP_8723_1T1R_A_CUT = 0x34,
	VERSION_NORMA_UMC_CHIP_8723_1T1R_B_CUT = 0x3c,
	VERSION_NORMAL_UMC_CHIP_88C_B_CUT = 0x70,
	VERSION_NORMAL_UMC_CHIP_92C_B_CUT = 0x71,
	VERSION_NORMAL_UMC_CHIP_92C_1T2R_B_CUT = 0x73,
	VERSION_UNKNOWN = 0x88,
};
#endif

