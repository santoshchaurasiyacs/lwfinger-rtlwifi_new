/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/
#ifndef	__HALBT_PRECOMP_H__
#define __HALBT_PRECOMP_H__
/*************************************************************
 * include files
 *************************************************************/
#include "../wifi.h"
#include "../efuse.h"
#include "../base.h"
#include "../regd.h"
#include "../cam.h"
#include "../ps.h"
#include "../pci.h"

#include "halbtcoutsrc.h"

/* Interface type */
#define RT_PCI_INTERFACE	1
#define RT_USB_INTERFACE	2
#define RT_SDIO_INTERFACE	3
#define DEV_BUS_TYPE		RT_PCI_INTERFACE

/* IC type */
#define RTL_HW_TYPE(_Adapter)			(rtl_hal((struct rtl_priv *)_Adapter)->hw_type)

#define IS_NEW_GENERATION_IC(_Adapter)	(RTL_HW_TYPE(_Adapter) >= HARDWARE_TYPE_RTL8192EE)
#define IS_HARDWARE_TYPE_8812(_Adapter)	(RTL_HW_TYPE(_Adapter) == HARDWARE_TYPE_RTL8812AE)
#define IS_HARDWARE_TYPE_8821(_Adapter)	(RTL_HW_TYPE(_Adapter) == HARDWARE_TYPE_RTL8821AE)

#define IS_HARDWARE_TYPE_8723A(_Adapter)	(RTL_HW_TYPE(_Adapter) == HARDWARE_TYPE_RTL8723AE)
#define IS_HARDWARE_TYPE_8723B(_Adapter)	(RTL_HW_TYPE(_Adapter) == HARDWARE_TYPE_RTL8723BE)
#define IS_HARDWARE_TYPE_8192E(_Adapter)	(RTL_HW_TYPE(_Adapter) == HARDWARE_TYPE_RTL8192EE)


#include "halbtc8192e2ant.h"
#include "halbtc8723b1ant.h"
#include "halbtc8723b2ant.h"
#include "halbtc8821a2ant.h"
#include "halbtc8821a1ant.h"

#define GetDefaultAdapter(padapter)	padapter

#define	MASKBYTE0	0xff
#define	MASKBYTE1	0xff00
#define	MASKBYTE2	0xff0000
#define	MASKBYTE3	0xff000000
#define	MASKHWORD	0xffff0000
#define	MASKLWORD	0x0000ffff
#define	MASKDWORD	0xffffffff
#define	MASK12BITS	0xfff
#define	MASKH4BITS	0xf0000000
#define MASKOFDM_D	0xffc00000
#define	MASKCCK		0x3f3f3f3f

#endif	/* __HALBT_PRECOMP_H__ */
