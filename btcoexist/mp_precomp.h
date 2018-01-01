/* only uncooked code include this file */

#include "halbt_precomp.h"

#define btc_phydm_modify_RA_PCR_threshold	btc_phydm_modify_ra_pcr_threshold
#define btc_phydm_query_PHY_counter		btc_phydm_query_phy_counter

#define PHYDM_INFO_FA_OFDM	"PHYDM_INFO_FA_OFDM"
#define PHYDM_INFO_FA_CCK	"PHYDM_INFO_FA_CCK"
#define PHYDM_INFO_CCA_OFDM	"PHYDM_INFO_CCA_OFDM"
#define PHYDM_INFO_CCA_CCK	"PHYDM_INFO_CCA_CCK"

#define PHYDM_INFO_CRC32_OK_CCK		"PHYDM_INFO_CRC32_OK_CCK"
#define PHYDM_INFO_CRC32_OK_LEGACY	"PHYDM_INFO_CRC32_OK_LEGACY"
#define PHYDM_INFO_CRC32_OK_HT		"PHYDM_INFO_CRC32_OK_HT"
#define PHYDM_INFO_CRC32_OK_VHT		"PHYDM_INFO_CRC32_OK_VHT"
#define PHYDM_INFO_CRC32_ERROR_CCK	"PHYDM_INFO_CRC32_ERROR_CCK"
#define PHYDM_INFO_CRC32_ERROR_LEGACY	"PHYDM_INFO_CRC32_ERROR_LEGACY"
#define PHYDM_INFO_CRC32_ERROR_HT	"PHYDM_INFO_CRC32_ERROR_HT"
#define PHYDM_INFO_CRC32_ERROR_VHT	"PHYDM_INFO_CRC32_ERROR_VHT"

#define delay_ms(ms)	mdelay(ms)

#define TRUE	true
#define FALSE	false

#define CL_SPRINTF(cli_buf, BT_TMP_BUF_SIZE, ...)	seq_printf(global_m, ##__VA_ARGS__)
#define CL_PRINTF(...)

extern struct seq_file *global_m;

#define BTC_SPRINTF	snprintf

#define BTC_TRACE(x)		{		\
					struct rtl_priv *rtlpriv = btcoexist->adapter;	\
					RT_TRACE(rtlpriv, COMP_BT_COEXIST, DBG_LOUD, x);\
				}

#define btc_disp_dbg_msg(x, y)	btc_disp_dbg_msg(x, y, global_m)
