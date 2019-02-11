// SPDX-License-Identifier: GPL-2.0
/* Copyright(c) 2018  Realtek Corporation.
 */

#include "main.h"
#include "efuse.h"
#include "reg.h"
#include "debug.h"

#define RTW_EFUSE_BANK_WIFI		0x0

static void switch_efuse_bank(struct rtw_dev *rtwdev)
{
	rtw_write32_mask(rtwdev, REG_LDO_EFUSE_CTRL, BIT_MASK_EFUSE_BANK_SEL,
			 RTW_EFUSE_BANK_WIFI);
}

static int rtw_dump_logical_efuse_map(struct rtw_dev *rtwdev, u8 *phy_map,
				      u8 *log_map)
{
	u32 physical_size = rtwdev->efuse.physical_size;
	u32 protect_size = rtwdev->efuse.protect_size;
	u32 logical_size = rtwdev->efuse.logical_size;
	u32 phy_idx, log_idx;
	u8 hdr1, hdr2;
	u8 blk_idx;
	u8 valid;
	u8 word_en;
	int i;

	phy_idx = 0;

	do {
		hdr1 = *(phy_map + phy_idx);
		if ((hdr1 & 0x1f) == 0xf) {
			phy_idx++;
			hdr2 = *(phy_map + phy_idx);
			if (hdr2 == 0xff)
				break;
			blk_idx = ((hdr2 & 0xf0) >> 1) | ((hdr1 >> 5) & 0x07);
			word_en = hdr2 & 0x0f;
		} else {
			blk_idx = (hdr1 & 0xf0) >> 4;
			word_en = hdr1 & 0x0f;
		}

		if (hdr1 == 0xff)
			break;

		phy_idx++;
		for (i = 0; i < 4; i++) {
			valid = (~(word_en >> i)) & 0x1;
			if (valid != 0x1)
				continue;
			log_idx = (blk_idx << 3) + (i << 1);
			*(log_map + log_idx) = *(phy_map + phy_idx);
			log_idx++;
			phy_idx++;
			*(log_map + log_idx) = *(phy_map + phy_idx);
			phy_idx++;
			if (phy_idx > physical_size - protect_size ||
			    log_idx > logical_size)
				return -EINVAL;
		}
	} while (1);

	return 0;
}

static int rtw_dump_physical_efuse_map(struct rtw_dev *rtwdev, u8 *map)
{
	struct rtw_chip_info *chip = rtwdev->chip;
	u32 size = rtwdev->efuse.physical_size;
	u32 efuse_ctl;
	u32 addr;
	u32 cnt;

	switch_efuse_bank(rtwdev);

	/* disable 2.5V LDO */
	chip->ops->cfg_ldo25(rtwdev, false);

	efuse_ctl = rtw_read32(rtwdev, REG_EFUSE_CTRL);

	for (addr = 0; addr < size; addr++) {
		efuse_ctl &= ~(BIT_MASK_EF_DATA | BITS_EF_ADDR);
		efuse_ctl |= (addr & BIT_MASK_EF_ADDR) << BIT_SHIFT_EF_ADDR;
		rtw_write32(rtwdev, REG_EFUSE_CTRL, efuse_ctl & (~BIT_EF_FLAG));

		cnt = 1000000;
		do {
			udelay(1);
			efuse_ctl = rtw_read32(rtwdev, REG_EFUSE_CTRL);
			if (--cnt == 0)
				return -EBUSY;
		} while (!(efuse_ctl & BIT_EF_FLAG));

		*(map + addr) = (u8)(efuse_ctl & BIT_MASK_EF_DATA);
	}

	return 0;
}

int rtw_parse_efuse_map(struct rtw_dev *rtwdev)
{
	struct rtw_chip_info *chip = rtwdev->chip;
	struct rtw_efuse *efuse = &rtwdev->efuse;
	u32 phy_size = efuse->physical_size;
	u32 log_size = efuse->logical_size;
	u8 *phy_map = NULL;
	u8 *log_map = NULL;
	int ret = 0;

	phy_map = kmalloc(phy_size, GFP_KERNEL);
	log_map = kmalloc(log_size, GFP_KERNEL);
	if (!phy_map || !log_map) {
		ret = -ENOMEM;
		goto out_free;
	}

	ret = rtw_dump_physical_efuse_map(rtwdev, phy_map);
	if (ret) {
		rtw_err(rtwdev, "failed to dump efuse physical map\n");
		goto out_free;
	}

	memset(log_map, 0xff, log_size);
	ret = rtw_dump_logical_efuse_map(rtwdev, phy_map, log_map);
	if (ret) {
		rtw_err(rtwdev, "failed to dump efuse logical map\n");
		goto out_free;
	}

	print_hex_dump_bytes("efuse: ", DUMP_PREFIX_OFFSET, log_map, log_size);

	efuse->x3d7 = phy_map[0x3d7];
	efuse->x3d8 = phy_map[0x3d8];

	ret = chip->ops->read_efuse(rtwdev, log_map);
	if (ret) {
		rtw_err(rtwdev, "failed to read efuse map\n");
		goto out_free;
	}

out_free:
	kfree(log_map);
	kfree(phy_map);

	return ret;
}
