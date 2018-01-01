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

/*Image2HeaderVersion: R2 1.2.1*/
#include "mp_precomp.h"
#include "../phydm_precomp.h"

#if (RTL8822B_SUPPORT == 1)
static boolean check_positive(struct PHY_DM_STRUCT *p_dm_odm,
			      const u32 condition1, const u32 condition2,
			      const u32 condition3, const u32 condition4)
{
	u32 cond1 = condition1, cond2 = condition2, cond3 = condition3,
	    cond4 = condition4;

	u8 cut_version_for_para = (p_dm_odm->cut_version == ODM_CUT_A) ?
					  15 :
					  p_dm_odm->cut_version;
	u8 pkg_type_for_para =
		(p_dm_odm->package_type == 0) ? 15 : p_dm_odm->package_type;

	u32 driver1 =
		cut_version_for_para << 24 |
		(p_dm_odm->support_interface & 0xF0) << 16 |
		p_dm_odm->support_platform << 16 | pkg_type_for_para << 12 |
		(p_dm_odm->support_interface & 0x0F) << 8 | p_dm_odm->rfe_type;

	u32 driver2 = (p_dm_odm->type_glna & 0xFF) << 0 |
		      (p_dm_odm->type_gpa & 0xFF) << 8 |
		      (p_dm_odm->type_alna & 0xFF) << 16 |
		      (p_dm_odm->type_apa & 0xFF) << 24;

	u32 driver3 = 0;

	u32 driver4 = (p_dm_odm->type_glna & 0xFF00) >> 8 |
		      (p_dm_odm->type_gpa & 0xFF00) |
		      (p_dm_odm->type_alna & 0xFF00) << 8 |
		      (p_dm_odm->type_apa & 0xFF00) << 16;

	ODM_RT_TRACE(
		p_dm_odm, ODM_COMP_INIT, ODM_DBG_TRACE,
		("===> check_positive (cond1, cond2, cond3, cond4) = (0x%X 0x%X 0x%X 0x%X)\n",
		 cond1, cond2, cond3, cond4));
	ODM_RT_TRACE(
		p_dm_odm, ODM_COMP_INIT, ODM_DBG_TRACE,
		("===> check_positive (driver1, driver2, driver3, driver4) = (0x%X 0x%X 0x%X 0x%X)\n",
		 driver1, driver2, driver3, driver4));

	ODM_RT_TRACE(p_dm_odm, ODM_COMP_INIT, ODM_DBG_TRACE,
		     ("	(Platform, Interface) = (0x%X, 0x%X)\n",
		      p_dm_odm->support_platform, p_dm_odm->support_interface));
	ODM_RT_TRACE(p_dm_odm, ODM_COMP_INIT, ODM_DBG_TRACE,
		     ("	(RFE, Package) = (0x%X, 0x%X)\n",
		      p_dm_odm->rfe_type, p_dm_odm->package_type));

	/*============== value Defined Check ===============*/
	/*cut version [27:24] need to do value check*/

	if (((cond1 & 0x0F000000) != 0) &&
	    ((cond1 & 0x0F000000) != (driver1 & 0x0F000000)))
		return false;

	/*pkg type [15:12] need to do value check*/

	if (((cond1 & 0x0000F000) != 0) &&
	    ((cond1 & 0x0000F000) != (driver1 & 0x0000F000)))
		return false;

	/*=============== Bit Defined Check ================*/
	/* We don't care [31:28] */

	cond1 &= 0x000000FF;
	driver1 &= 0x000000FF;

	if (cond1 == driver1)
		return true;
	else
		return false;
}

/******************************************************************************
*                           mac_reg.TXT
******************************************************************************/

u32 array_mp_8822b_mac_reg[] = {
	BBREG_0x029, 0x000000F9,  BBREG_0x420, 0x00000080,  BBREG_0x421,
	0x0000001F,  BBREG_0x428, 0x0000000A,  BBREG_0x429, 0x00000010,
	BBREG_0x430, 0x00000000,  BBREG_0x431, 0x00000000,  BBREG_0x432,
	0x00000000,  BBREG_0x433, 0x00000001,  BBREG_0x434, 0x00000004,
	BBREG_0x435, 0x00000005,  BBREG_0x436, 0x00000007,  BBREG_0x437,
	0x00000008,  BBREG_0x43c, 0x00000004,  BBREG_0x43d, 0x00000005,
	BBREG_0x43e, 0x00000007,  BBREG_0x43f, 0x00000008,  BBREG_0x440,
	0x0000005D,  BBREG_0x441, 0x00000001,  BBREG_0x442, 0x00000000,
	BBREG_0x444, 0x00000010,  BBREG_0x445, 0x000000F0,  BBREG_0x446,
	0x00000001,  BBREG_0x447, 0x000000FE,  BBREG_0x448, 0x00000000,
	BBREG_0x449, 0x00000000,  BBREG_0x44a, 0x00000000,  BBREG_0x44b,
	0x00000040,  BBREG_0x44c, 0x00000010,  BBREG_0x44d, 0x000000F0,
	BBREG_0x44e, 0x0000003F,  BBREG_0x44f, 0x00000000,  BBREG_0x450,
	0x00000000,  BBREG_0x451, 0x00000000,  BBREG_0x452, 0x00000000,
	BBREG_0x453, 0x00000040,  BBREG_0x455, 0x00000070,  BBREG_0x45e,
	0x00000004,  BBREG_0x49c, 0x00000010,  BBREG_0x49d, 0x000000F0,
	BBREG_0x49e, 0x00000000,  BBREG_0x49f, 0x00000006,  BBREG_0x4a0,
	0x000000E0,  BBREG_0x4a1, 0x00000003,  BBREG_0x4a2, 0x00000000,
	BBREG_0x4a3, 0x00000040,  BBREG_0x4a4, 0x00000015,  BBREG_0x4a5,
	0x000000F0,  BBREG_0x4a6, 0x00000000,  BBREG_0x4a7, 0x00000006,
	BBREG_0x4a8, 0x000000E0,  BBREG_0x4a9, 0x00000000,  BBREG_0x4aa,
	0x00000000,  BBREG_0x4ab, 0x00000000,  BBREG_0x7da, 0x00000008,
	0x1448,      0x00000006,  0x144A,      0x00000006,  0x144C,
	0x00000006,  0x144E,      0x00000006,  BBREG_0x4c8, 0x000000FF,
	BBREG_0x4c9, 0x00000008,  BBREG_0x4ca, 0x00000020,  BBREG_0x4cb,
	0x00000020,  BBREG_0x4cc, 0x000000FF,  BBREG_0x4cd, 0x000000FF,
	BBREG_0x4ce, 0x00000001,  BBREG_0x4cf, 0x00000008,  BBREG_0x500,
	0x00000026,  BBREG_0x501, 0x000000A2,  BBREG_0x502, 0x0000002F,
	BBREG_0x503, 0x00000000,  BBREG_0x504, 0x00000028,  BBREG_0x505,
	0x000000A3,  BBREG_0x506, 0x0000005E,  BBREG_0x507, 0x00000000,
	BBREG_0x508, 0x0000002B,  BBREG_0x509, 0x000000A4,  BBREG_0x50a,
	0x0000005E,  BBREG_0x50b, 0x00000000,  BBREG_0x50c, 0x0000004F,
	BBREG_0x50d, 0x000000A4,  BBREG_0x50e, 0x00000000,  BBREG_0x50f,
	0x00000000,  BBREG_0x512, 0x0000001C,  BBREG_0x514, 0x0000000A,
	BBREG_0x516, 0x0000000A,  BBREG_0x521, 0x0000002F,  BBREG_0x525,
	0x0000004F,  BBREG_0x551, 0x00000010,  BBREG_0x559, 0x00000002,
	BBREG_0x55c, 0x00000050,  BBREG_0x55d, 0x000000FF,  BBREG_0x577,
	0x0000000B,  BBREG_0x5be, 0x00000064,  BBREG_0x605, 0x00000030,
	BBREG_0x608, 0x0000000E,  BBREG_0x609, 0x00000022,  BBREG_0x60c,
	0x00000018,  BBREG_0x6a0, 0x000000FF,  BBREG_0x6a1, 0x000000FF,
	BBREG_0x6a2, 0x000000FF,  BBREG_0x6a3, 0x000000FF,  BBREG_0x6a4,
	0x000000FF,  BBREG_0x6a5, 0x000000FF,  BBREG_0x6de, 0x00000084,
	BBREG_0x620, 0x000000FF,  BBREG_0x621, 0x000000FF,  BBREG_0x622,
	0x000000FF,  BBREG_0x623, 0x000000FF,  BBREG_0x624, 0x000000FF,
	BBREG_0x625, 0x000000FF,  BBREG_0x626, 0x000000FF,  BBREG_0x627,
	0x000000FF,  BBREG_0x638, 0x00000050,  BBREG_0x63c, 0x0000000A,
	BBREG_0x63d, 0x0000000A,  BBREG_0x63e, 0x0000000E,  BBREG_0x63f,
	0x0000000E,  BBREG_0x640, 0x00000040,  BBREG_0x642, 0x00000040,
	BBREG_0x643, 0x00000000,  BBREG_0x652, 0x000000C8,  BBREG_0x66e,
	0x00000005,  BBREG_0x718, 0x00000040,  BBREG_0x7d4, 0x00000098,

};

void odm_read_and_config_mp_8822b_mac_reg(struct PHY_DM_STRUCT *p_dm_odm)
{
	u32 i = 0;
	u8 c_cond;
	boolean is_matched = true, is_skipped = false;
	u32 array_len = sizeof(array_mp_8822b_mac_reg) / sizeof(u32);
	u32 *array = array_mp_8822b_mac_reg;

	u32 v1 = 0, v2 = 0, pre_v1 = 0, pre_v2 = 0;

	ODM_RT_TRACE(p_dm_odm, ODM_COMP_INIT, ODM_DBG_LOUD,
		     ("===> odm_read_and_config_mp_8822b_mac_reg\n"));

	while ((i + 1) < array_len) {
		v1 = array[i];
		v2 = array[i + 1];

		if (v1 &
		    (BIT(31) | BIT(30))) { /*positive & negative condition*/
			if (v1 & BIT(31)) { /* positive condition*/
				c_cond = (u8)((v1 & (BIT(29) | BIT(28))) >> 28);
				if (c_cond == COND_ENDIF) { /*end*/
					is_matched = true;
					is_skipped = false;
					ODM_RT_TRACE(p_dm_odm, ODM_COMP_INIT,
						     ODM_DBG_LOUD, ("ENDIF\n"));
				} else if (c_cond == COND_ELSE) { /*else*/
					is_matched = is_skipped ? false : true;
					ODM_RT_TRACE(p_dm_odm, ODM_COMP_INIT,
						     ODM_DBG_LOUD, ("ELSE\n"));
				} else { /*if , else if*/
					pre_v1 = v1;
					pre_v2 = v2;
					ODM_RT_TRACE(p_dm_odm, ODM_COMP_INIT,
						     ODM_DBG_LOUD,
						     ("IF or ELSE IF\n"));
				}
			} else if (v1 & BIT(30)) { /*negative condition*/
				if (is_skipped == false) {
					if (check_positive(p_dm_odm, pre_v1,
							   pre_v2, v1, v2)) {
						is_matched = true;
						is_skipped = true;
					} else {
						is_matched = false;
						is_skipped = false;
					}
				} else
					is_matched = false;
			}
		} else {
			if (is_matched)
				odm_config_mac_8822b(p_dm_odm, v1, (u8)v2);
		}
		i = i + 2;
	}
}

u32 odm_get_version_mp_8822b_mac_reg(void) { return 100; }

#endif /* end of HWIMG_SUPPORT*/

