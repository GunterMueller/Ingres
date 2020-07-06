/*
** Copyright (c) 2007 Ingres Corporation
*/
/* Jam hints
**
LIBRARY = IMPADFLIBDATA
**
*/
#include <compat.h>
#include <gl.h>
#include <iicommon.h>
#include <adf.h>
#include <adp.h>
#include <ulf.h>
#include <adfint.h>
#include <aduint.h>

/**
**
**  Name: ADGFI_DEFN_LU.ROC - ADF's function instance lookup table.
**
**  Description:
**	This file initializes the function instance lookup table, an ADF
**	internal structure used for quick access to a particular function
**	instance given the function instance ID.
**
**	** DO NOT MAKE MANUAL CHANGES TO THIS FILE **
**
**	This file is generated from fi_defn.txt by the AWK program fi_defn.awk.
**	To change function instance info, modify fi_defn.txt and run fi_defn.awk;
*/


/*
** Definition of all global variables used by this file.
*/

# ifdef NT_GENERIC
FACILITYREF ADI_FI_DESC Adi_fis[];
# else /* NT_GENERIC */
GLOBALREF ADI_FI_DESC Adi_fis[]; /* Actual Adf fi descriptors */
# endif

GLOBALCONSTDEF ADI_FI_LOOKUP Adi_fi_lkup[] = {
/*
** This table is the function instance lookup table, and ADF internal
** structure used for quick access to a particular f.i. given the f.i. ID.
** It also holds pieces of information for each f.i. that are not visible
** to the outside world.  The index into this table is equal to the f.i. ID
** of the f.i. corresponding to that row.
**
** By having this level of indirection, we gain the ability to add new
** f.i.'s with new f.i. ID's, keep the f.i. table in the proper 'grouped'
** order, and not change existing f.i. ID's.  Therefore, once a f.i. ID
** has been established, it will not change.  This will allow f.i. IDs to
** be stored in the database in the future.
*/

/*----------------------------------------------------------------------------*/
/*  Each array element in the function instance lookup table looks like this: */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/*  { pointer into      & f.i.          & agbgn         & agend        /* f.i.*/
/*     f.i. table,       routine,        routine,        routine }     /* ID  */
/*                                                                            */
/*----------------------------------------------------------------------------*/

{ &Adi_fis[67], NULL, NULL, NULL },	/* ADFI_000_C_EQ_C */
{ &Adi_fis[68], NULL, NULL, NULL },	/* ADFI_001_TEXT_EQ_TEXT */
{ &Adi_fis[71], adu_4date_cmp, NULL, NULL },	/* ADFI_002_DATE_EQ_DATE */
{ &Adi_fis[72], NULL, NULL, NULL },	/* ADFI_003_F_EQ_F */
{ &Adi_fis[73], NULL, NULL, NULL },	/* ADFI_004_F_EQ_I */
{ &Adi_fis[75], NULL, NULL, NULL },	/* ADFI_005_I_EQ_F */
{ &Adi_fis[76], NULL, NULL, NULL },	/* ADFI_006_I_EQ_I */
{ &Adi_fis[77], adu_6mny_cmp, NULL, NULL },	/* ADFI_007_MONEY_EQ_MONEY */
{ &Adi_fis[1], NULL, NULL, NULL },	/* ADFI_008_C_NE_C */
{ &Adi_fis[2], NULL, NULL, NULL },	/* ADFI_009_TEXT_NE_TEXT */
{ &Adi_fis[5], adu_4date_cmp, NULL, NULL },	/* ADFI_010_DATE_NE_DATE */
{ &Adi_fis[6], NULL, NULL, NULL },	/* ADFI_011_F_NE_F */
{ &Adi_fis[7], NULL, NULL, NULL },	/* ADFI_012_F_NE_I */
{ &Adi_fis[9], NULL, NULL, NULL },	/* ADFI_013_I_NE_F */
{ &Adi_fis[10], NULL, NULL, NULL },	/* ADFI_014_I_NE_I */
{ &Adi_fis[11], adu_6mny_cmp, NULL, NULL },	/* ADFI_015_MONEY_NE_MONEY */
{ &Adi_fis[45], NULL, NULL, NULL },	/* ADFI_016_C_LE_C */
{ &Adi_fis[46], NULL, NULL, NULL },	/* ADFI_017_TEXT_LE_TEXT */
{ &Adi_fis[49], adu_4date_cmp, NULL, NULL },	/* ADFI_018_DATE_LE_DATE */
{ &Adi_fis[50], NULL, NULL, NULL },	/* ADFI_019_F_LE_F */
{ &Adi_fis[51], NULL, NULL, NULL },	/* ADFI_020_F_LE_I */
{ &Adi_fis[53], NULL, NULL, NULL },	/* ADFI_021_I_LE_F */
{ &Adi_fis[54], NULL, NULL, NULL },	/* ADFI_022_I_LE_I */
{ &Adi_fis[55], adu_6mny_cmp, NULL, NULL },	/* ADFI_023_MONEY_LE_MONEY */
{ &Adi_fis[90], NULL, NULL, NULL },	/* ADFI_024_C_GT_C */
{ &Adi_fis[91], NULL, NULL, NULL },	/* ADFI_025_TEXT_GT_TEXT */
{ &Adi_fis[94], adu_4date_cmp, NULL, NULL },	/* ADFI_026_DATE_GT_DATE */
{ &Adi_fis[95], NULL, NULL, NULL },	/* ADFI_027_F_GT_F */
{ &Adi_fis[96], NULL, NULL, NULL },	/* ADFI_028_F_GT_I */
{ &Adi_fis[98], NULL, NULL, NULL },	/* ADFI_029_I_GT_F */
{ &Adi_fis[99], NULL, NULL, NULL },	/* ADFI_030_I_GT_I */
{ &Adi_fis[100], adu_6mny_cmp, NULL, NULL },	/* ADFI_031_MONEY_GT_MONEY */
{ &Adi_fis[112], NULL, NULL, NULL },	/* ADFI_032_C_GE_C */
{ &Adi_fis[113], NULL, NULL, NULL },	/* ADFI_033_TEXT_GE_TEXT */
{ &Adi_fis[116], adu_4date_cmp, NULL, NULL },	/* ADFI_034_DATE_GE_DATE */
{ &Adi_fis[117], NULL, NULL, NULL },	/* ADFI_035_F_GE_F */
{ &Adi_fis[118], NULL, NULL, NULL },	/* ADFI_036_F_GE_I */
{ &Adi_fis[119], NULL, NULL, NULL },	/* ADFI_037_I_GE_F */
{ &Adi_fis[121], NULL, NULL, NULL },	/* ADFI_038_I_GE_I */
{ &Adi_fis[122], adu_6mny_cmp, NULL, NULL },	/* ADFI_039_MONEY_GE_MONEY */
{ &Adi_fis[23], NULL, NULL, NULL },	/* ADFI_040_C_LT_C */
{ &Adi_fis[24], NULL, NULL, NULL },	/* ADFI_041_TEXT_LT_TEXT */
{ &Adi_fis[27], adu_4date_cmp, NULL, NULL },	/* ADFI_042_DATE_LT_DATE */
{ &Adi_fis[28], NULL, NULL, NULL },	/* ADFI_043_F_LT_F */
{ &Adi_fis[29], NULL, NULL, NULL },	/* ADFI_044_F_LT_I */
{ &Adi_fis[31], NULL, NULL, NULL },	/* ADFI_045_I_LT_F */
{ &Adi_fis[32], NULL, NULL, NULL },	/* ADFI_046_I_LT_I */
{ &Adi_fis[33], adu_6mny_cmp, NULL, NULL },	/* ADFI_047_MONEY_LT_MONEY */
{ &Adi_fis[199], adu_1date_addu, NULL, NULL },	/* ADFI_048_DATE_ADD_DATE */
{ &Adi_fis[200], NULL, NULL, NULL },	/* ADFI_049_F_ADD_F */
{ &Adi_fis[202], NULL, NULL, NULL },	/* ADFI_050_I_ADD_I */
{ &Adi_fis[203], adu_4mny_addu, NULL, NULL },	/* ADFI_051_MONEY_ADD_MONEY */
{ &Adi_fis[204], adu_4strconcat, NULL, NULL },	/* ADFI_052_C_ADD_C */
{ &Adi_fis[208], adu_4strconcat, NULL, NULL },	/* ADFI_053_TEXT_ADD_C */
{ &Adi_fis[209], adu_4strconcat, NULL, NULL },	/* ADFI_054_TEXT_ADD_TEXT */
{ &Adi_fis[233], adu_7date_subu, NULL, NULL },	/* ADFI_055_DATE_SUB_DATE */
{ &Adi_fis[234], NULL, NULL, NULL },	/* ADFI_056_F_SUB_F */
{ &Adi_fis[236], NULL, NULL, NULL },	/* ADFI_057_I_SUB_I */
{ &Adi_fis[237], adu_12mny_subu, NULL, NULL },	/* ADFI_058_MONEY_SUB_MONEY */
{ &Adi_fis[175], NULL, NULL, NULL },	/* ADFI_059_F_MUL_F */
{ &Adi_fis[183], NULL, NULL, NULL },	/* ADFI_060_I_MUL_I */
{ &Adi_fis[185], adu_10mny_multu, NULL, NULL },	/* ADFI_061_F_MUL_MONEY */
{ &Adi_fis[186], adu_10mny_multu, NULL, NULL },	/* ADFI_062_I_MUL_MONEY */
{ &Adi_fis[188], adu_10mny_multu, NULL, NULL },	/* ADFI_063_MONEY_MUL_F */
{ &Adi_fis[189], adu_10mny_multu, NULL, NULL },	/* ADFI_064_MONEY_MUL_I */
{ &Adi_fis[184], adu_10mny_multu, NULL, NULL },	/* ADFI_065_MONEY_MUL_MONEY */
{ &Adi_fis[238], NULL, NULL, NULL },	/* ADFI_066_F_DIV_F */
{ &Adi_fis[246], NULL, NULL, NULL },	/* ADFI_067_I_DIV_I */
{ &Adi_fis[248], adu_7mny_divf, NULL, NULL },	/* ADFI_068_F_DIV_MONEY */
{ &Adi_fis[249], adu_7mny_divf, NULL, NULL },	/* ADFI_069_I_DIV_MONEY */
{ &Adi_fis[251], adu_8mny_divu, NULL, NULL },	/* ADFI_070_MONEY_DIV_F */
{ &Adi_fis[252], adu_8mny_divu, NULL, NULL },	/* ADFI_071_MONEY_DIV_I */
{ &Adi_fis[247], adu_8mny_divu, NULL, NULL },	/* ADFI_072_MONEY_DIV_MONEY */
{ &Adi_fis[193], NULL, NULL, NULL },	/* ADFI_073_F_POW_F */
{ &Adi_fis[194], adu_2int_pow, NULL, NULL },	/* ADFI_074_F_POW_I */
{ &Adi_fis[256], NULL, NULL, NULL },	/* ADFI_075_PLUS_F */
{ &Adi_fis[258], NULL, NULL, NULL },	/* ADFI_076_PLUS_I */
{ &Adi_fis[259], adu_posmny, NULL, NULL },	/* ADFI_077_PLUS_MONEY */
{ &Adi_fis[261], NULL, NULL, NULL },	/* ADFI_078_MINUS_F */
{ &Adi_fis[263], NULL, NULL, NULL },	/* ADFI_079_MINUS_I */
{ &Adi_fis[264], adu_negmny, NULL, NULL },	/* ADFI_080_MINUS_MONEY */
{ &Adi_fis[266], adu_N1a_any, adu_B0a_noagwork, adu_E0a_anycount },	/* ADFI_081_ANY_C_ALL */
{ &Adi_fis[268], adu_N1a_any, adu_B0a_noagwork, adu_E0a_anycount },	/* ADFI_082_ANY_DATE */
{ &Adi_fis[269], adu_N1a_any, adu_B0a_noagwork, adu_E0a_anycount },	/* ADFI_083_ANY_F */
{ &Adi_fis[271], adu_N1a_any, adu_B0a_noagwork, adu_E0a_anycount },	/* ADFI_084_ANY_I */
{ &Adi_fis[272], adu_N1a_any, adu_B0a_noagwork, adu_E0a_anycount },	/* ADFI_085_ANY_MONEY */
{ &Adi_fis[273], adu_N1a_any, adu_B0a_noagwork, adu_E0a_anycount },	/* ADFI_086_ANY_TEXT */
{ &Adi_fis[279], adu_N3f_avg_f, adu_B0a_noagwork, adu_E3f_avg_f },	/* ADFI_087_AVG_F */
{ &Adi_fis[281], adu_N3i_avg_i, adu_B0a_noagwork, adu_E3i_avg_i },	/* ADFI_088_AVG_I */
{ &Adi_fis[282], adu_N3m_avg_mny, adu_B0a_noagwork, adu_E3m_avg_mny },	/* ADFI_089_AVG_MONEY */
{ &Adi_fis[284], adu_N2a_count, adu_B0a_noagwork, adu_E0a_anycount },	/* ADFI_090_COUNT_C_ALL */
{ &Adi_fis[286], adu_N2a_count, adu_B0a_noagwork, adu_E0a_anycount },	/* ADFI_091_COUNT_DATE */
{ &Adi_fis[287], adu_N2a_count, adu_B0a_noagwork, adu_E0a_anycount },	/* ADFI_092_COUNT_F */
{ &Adi_fis[289], adu_N2a_count, adu_B0a_noagwork, adu_E0a_anycount },	/* ADFI_093_COUNT_I */
{ &Adi_fis[290], adu_N2a_count, adu_B0a_noagwork, adu_E0a_anycount },	/* ADFI_094_COUNT_MONEY */
{ &Adi_fis[291], adu_N2a_count, adu_B0a_noagwork, adu_E0a_anycount },	/* ADFI_095_COUNT_TEXT */
{ &Adi_fis[298], adu_N6s_max_str, adu_B0s_minmax_str, adu_E0s_minmax_str },	/* ADFI_096_MAX_C */
{ &Adi_fis[300], adu_N6d_max_date, adu_B0d_minmax_date, adu_E0d_minmax_date },	/* ADFI_097_MAX_DATE */
{ &Adi_fis[301], adu_N6f_max_f, adu_B0a_noagwork, adu_E0f_minmax_f },	/* ADFI_098_MAX_F */
{ &Adi_fis[303], adu_N6i_max_i, adu_B0a_noagwork, adu_E0i_minmax_i },	/* ADFI_099_MAX_I */
{ &Adi_fis[304], adu_N6m_max_mny, adu_B0a_noagwork, adu_E0m_minmax_mny },	/* ADFI_100_MAX_MONEY */
{ &Adi_fis[305], adu_N6s_max_str, adu_B0s_minmax_str, adu_E0s_minmax_str },	/* ADFI_101_MAX_TEXT */
{ &Adi_fis[311], adu_N5s_min_str, adu_B0s_minmax_str, adu_E0s_minmax_str },	/* ADFI_102_MIN_C */
{ &Adi_fis[313], adu_N5d_min_date, adu_B0d_minmax_date, adu_E0d_minmax_date },	/* ADFI_103_MIN_DATE */
{ &Adi_fis[314], adu_N5f_min_f, adu_B0a_noagwork, adu_E0f_minmax_f },	/* ADFI_104_MIN_F */
{ &Adi_fis[316], adu_N5i_min_i, adu_B0a_noagwork, adu_E0i_minmax_i },	/* ADFI_105_MIN_I */
{ &Adi_fis[317], adu_N5m_min_mny, adu_B0a_noagwork, adu_E0m_minmax_mny },	/* ADFI_106_MIN_MONEY */
{ &Adi_fis[318], adu_N5s_min_str, adu_B0s_minmax_str, adu_E0s_minmax_str },	/* ADFI_107_MIN_TEXT */
{ &Adi_fis[324], adu_N4f_sum_f, adu_B0a_noagwork, adu_E4f_sum_f },	/* ADFI_108_SUM_F */
{ &Adi_fis[326], adu_N4i_sum_i, adu_B0a_noagwork, adu_E4i_sum_i },	/* ADFI_109_SUM_I */
{ &Adi_fis[327], adu_N4m_sum_mny, adu_B0a_noagwork, adu_E4m_sum_mny },	/* ADFI_110_SUM_MONEY */
{ &Adi_fis[363], NULL, NULL, NULL },	/* ADFI_111_ABS_F */
{ &Adi_fis[365], NULL, NULL, NULL },	/* ADFI_112_ABS_I */
{ &Adi_fis[366], adu_3mny_absu, NULL, NULL },	/* ADFI_113_ABS_MONEY */
{ NULL, NULL, NULL, NULL },	/* No FI 114 */
{ NULL, NULL, NULL, NULL },	/* No FI 115 */
{ NULL, NULL, NULL, NULL },	/* No FI 116 */
{ NULL, NULL, NULL, NULL },	/* No FI 117 */
{ NULL, NULL, NULL, NULL },	/* No FI 118 */
{ NULL, NULL, NULL, NULL },	/* No FI 119 */
{ &Adi_fis[371], NULL, NULL, NULL },	/* ADFI_120_ATAN_F */
{ &Adi_fis[377], adu_4strconcat, NULL, NULL },	/* ADFI_121_CONCAT_C_C */
{ &Adi_fis[378], adu_4strconcat, NULL, NULL },	/* ADFI_122_CONCAT_TEXT_C */
{ &Adi_fis[379], adu_4strconcat, NULL, NULL },	/* ADFI_123_CONCAT_TEXT_TEXT */
{ &Adi_fis[397], NULL, NULL, NULL },	/* ADFI_124_COS_F */
{ &Adi_fis[398], adu_14strtodate, NULL, NULL },	/* ADFI_125_DATE_C */
{ &Adi_fis[400], adu_14strtodate, NULL, NULL },	/* ADFI_126_DATE_TEXT */
{ &Adi_fis[414], adu_1dayofweek, NULL, NULL },	/* ADFI_127_DOW_DATE */
{ &Adi_fis[405], adu_dextract, NULL, NULL },	/* ADFI_128_DATE_PART_C_DATE */
{ &Adi_fis[407], adu_dextract, NULL, NULL },	/* ADFI_129_DATE_PART_TEXT_DATE */
{ &Adi_fis[466], adu_intrvl_in, NULL, NULL },	/* ADFI_130_INTERVAL_C_DATE */
{ &Adi_fis[468], adu_intrvl_in, NULL, NULL },	/* ADFI_131_INTERVAL_TEXT_DATE */
{ &Adi_fis[409], adu_dtruncate, NULL, NULL },	/* ADFI_132_DATE_TRUNC_C_DATE */
{ &Adi_fis[411], adu_dtruncate, NULL, NULL },	/* ADFI_133_DATE_TRUNC_TEXT_DATE */
{ &Adi_fis[415], NULL, NULL, NULL },	/* ADFI_134_EXP_F */
{ &Adi_fis[416], adu_1flt_coerce, NULL, NULL },	/* ADFI_135_FLOAT4_C */
{ &Adi_fis[418], adu_1flt_coerce, NULL, NULL },	/* ADFI_136_FLOAT4_F */
{ &Adi_fis[420], adu_1flt_coerce, NULL, NULL },	/* ADFI_137_FLOAT4_I */
{ &Adi_fis[422], adu_1flt_coerce, NULL, NULL },	/* ADFI_138_FLOAT4_TEXT */
{ &Adi_fis[421], adu_1mnytonum, NULL, NULL },	/* ADFI_139_FLOAT4_MONEY */
{ &Adi_fis[426], adu_1flt_coerce, NULL, NULL },	/* ADFI_140_FLOAT8_C */
{ &Adi_fis[428], adu_1flt_coerce, NULL, NULL },	/* ADFI_141_FLOAT8_F */
{ &Adi_fis[430], adu_1flt_coerce, NULL, NULL },	/* ADFI_142_FLOAT8_I */
{ &Adi_fis[432], adu_1flt_coerce, NULL, NULL },	/* ADFI_143_FLOAT8_TEXT */
{ &Adi_fis[431], adu_1mnytonum, NULL, NULL },	/* ADFI_144_FLOAT8_MONEY */
{ &Adi_fis[436], adu_1int_coerce, NULL, NULL },	/* ADFI_145_INT1_C */
{ &Adi_fis[438], adu_1int_coerce, NULL, NULL },	/* ADFI_146_INT1_F */
{ &Adi_fis[440], adu_1int_coerce, NULL, NULL },	/* ADFI_147_INT1_I */
{ &Adi_fis[442], adu_1int_coerce, NULL, NULL },	/* ADFI_148_INT1_TEXT */
{ &Adi_fis[441], adu_1int_coerce, NULL, NULL },	/* ADFI_149_INT1_MONEY */
{ &Adi_fis[446], adu_1int_coerce, NULL, NULL },	/* ADFI_150_INT2_C */
{ &Adi_fis[448], adu_1int_coerce, NULL, NULL },	/* ADFI_151_INT2_F */
{ &Adi_fis[450], adu_1int_coerce, NULL, NULL },	/* ADFI_152_INT2_I */
{ &Adi_fis[452], adu_1int_coerce, NULL, NULL },	/* ADFI_153_INT2_TEXT */
{ &Adi_fis[451], adu_1int_coerce, NULL, NULL },	/* ADFI_154_INT2_MONEY */
{ &Adi_fis[456], adu_1int_coerce, NULL, NULL },	/* ADFI_155_INT4_C */
{ &Adi_fis[458], adu_1int_coerce, NULL, NULL },	/* ADFI_156_INT4_F */
{ &Adi_fis[460], adu_1int_coerce, NULL, NULL },	/* ADFI_157_INT4_I */
{ &Adi_fis[462], adu_1int_coerce, NULL, NULL },	/* ADFI_158_INT4_TEXT */
{ &Adi_fis[461], adu_1int_coerce, NULL, NULL },	/* ADFI_159_INT4_MONEY */
{ &Adi_fis[470], adu_6strleft, NULL, NULL },	/* ADFI_160_LEFT_C_I */
{ &Adi_fis[472], adu_6strleft, NULL, NULL },	/* ADFI_161_LEFT_TEXT_I */
{ &Adi_fis[480], adu_7strlength, NULL, NULL },	/* ADFI_162_LENGTH_C */
{ &Adi_fis[482], adu_7strlength, NULL, NULL },	/* ADFI_163_LENGTH_TEXT */
{ &Adi_fis[493], adu_8strlocate, NULL, NULL },	/* ADFI_164_LOCATE_STR_STR */
{ &Adi_fis[494], adu_nvchr_locate, NULL, NULL },	/* ADFI_165_LOCATE_UNISTR_UNISTR */
{ &Adi_fis[495], NULL, NULL, NULL },	/* ADFI_166_LOG_F */
{ &Adi_fis[496], adu_9strlower, NULL, NULL },	/* ADFI_167_LOWERCASE_C */
{ &Adi_fis[498], adu_9strlower, NULL, NULL },	/* ADFI_168_LOWERCASE_TEXT */
{ &Adi_fis[505], NULL, NULL, NULL },	/* ADFI_169_MOD_I_I */
{ &Adi_fis[506], adu_numtomny, NULL, NULL },	/* ADFI_170_MONEY_F */
{ &Adi_fis[508], adu_numtomny, NULL, NULL },	/* ADFI_171_MONEY_I */
{ &Adi_fis[509], adu_2strtomny, NULL, NULL },	/* ADFI_172_MONEY_C */
{ &Adi_fis[510], adu_2strtomny, NULL, NULL },	/* ADFI_173_MONEY_TEXT */
{ &Adi_fis[518], adu_pad, NULL, NULL },	/* ADFI_174_PAD_TEXT */
{ &Adi_fis[522], adu_10strright, NULL, NULL },	/* ADFI_175_RIGHT_C_I */
{ &Adi_fis[524], adu_10strright, NULL, NULL },	/* ADFI_176_RIGHT_TEXT_I */
{ &Adi_fis[532], adu_11strshift, NULL, NULL },	/* ADFI_177_SHIFT_C_I */
{ &Adi_fis[534], adu_11strshift, NULL, NULL },	/* ADFI_178_SHIFT_TEXT_I */
{ &Adi_fis[541], NULL, NULL, NULL },	/* ADFI_179_SIN_F */
{ &Adi_fis[542], adu_12strsize, NULL, NULL },	/* ADFI_180_SIZE_C */
{ &Adi_fis[544], adu_12strsize, NULL, NULL },	/* ADFI_181_SIZE_TEXT */
{ &Adi_fis[555], NULL, NULL, NULL },	/* ADFI_182_SQRT_F */
{ &Adi_fis[558], adu_squeezewhite, NULL, NULL },	/* ADFI_183_SQUEEZE_TEXT */
{ NULL, NULL, NULL, NULL },	/* No FI 184 */
{ NULL, NULL, NULL, NULL },	/* No FI 185 */
{ NULL, NULL, NULL, NULL },	/* No FI 186 */
{ NULL, NULL, NULL, NULL },	/* No FI 187 */
{ NULL, NULL, NULL, NULL },	/* No FI 188 */
{ NULL, NULL, NULL, NULL },	/* No FI 189 */
{ &Adi_fis[566], adu_trim, NULL, NULL },	/* ADFI_190_TRIM_C */
{ &Adi_fis[568], adu_trim, NULL, NULL },	/* ADFI_191_TRIM_TEXT */
{ &Adi_fis[572], adu_15strupper, NULL, NULL },	/* ADFI_192_UPPERCASE_C */
{ &Adi_fis[574], adu_15strupper, NULL, NULL },	/* ADFI_193_UPPERCASE_TEXT */
{ &Adi_fis[346], NULL, NULL, NULL },	/* ADFI_194__BINTIM_I */
{ &Adi_fis[355], adu_1cvrt_date, NULL, NULL },	/* ADFI_195__DATE_I */
{ &Adi_fis[360], adu_2cvrt_time, NULL, NULL },	/* ADFI_196__TIME_I */
{ &Adi_fis[354], NULL, NULL, NULL },	/* ADFI_197__CPU_MS */
{ &Adi_fis[357], NULL, NULL, NULL },	/* ADFI_198__ET_SEC */
{ &Adi_fis[356], NULL, NULL, NULL },	/* ADFI_199__DIO_CNT */
{ &Adi_fis[347], NULL, NULL, NULL },	/* ADFI_200__BIO_CNT */
{ &Adi_fis[358], NULL, NULL, NULL },	/* ADFI_201__PFAULT_CNT */
{ &Adi_fis[362], NULL, NULL, NULL },	/* ADFI_202__WS_PAGE */
{ &Adi_fis[359], NULL, NULL, NULL },	/* ADFI_203__PHYS_PAGE */
{ &Adi_fis[349], NULL, NULL, NULL },	/* ADFI_204__CACHE_REQ */
{ &Adi_fis[348], NULL, NULL, NULL },	/* ADFI_205__CACHE_READ */
{ &Adi_fis[353], NULL, NULL, NULL },	/* ADFI_206__CACHE_WRITE */
{ &Adi_fis[350], NULL, NULL, NULL },	/* ADFI_207__CACHE_RREAD */
{ &Adi_fis[351], NULL, NULL, NULL },	/* ADFI_208__CACHE_SIZE */
{ &Adi_fis[352], NULL, NULL, NULL },	/* ADFI_209__CACHE_USED */
{ &Adi_fis[361], NULL, NULL, NULL },	/* ADFI_210__VERSION */
{ &Adi_fis[413], NULL, NULL, NULL },	/* ADFI_211_DBA */
{ &Adi_fis[582], NULL, NULL, NULL },	/* ADFI_212_USERNAME */
{ &Adi_fis[931], adu_1flt_coerce, NULL, NULL },	/* ADFI_213_I_TO_F */
{ &Adi_fis[941], adu_1int_coerce, NULL, NULL },	/* ADFI_214_F_TO_I */
{ &Adi_fis[949], adu_14strtodate, NULL, NULL },	/* ADFI_215_C_TO_DATE */
{ &Adi_fis[950], adu_14strtodate, NULL, NULL },	/* ADFI_216_TEXT_TO_DATE */
{ &Adi_fis[951], adu_6datetostr, NULL, NULL },	/* ADFI_217_DATE_TO_C */
{ &Adi_fis[952], adu_6datetostr, NULL, NULL },	/* ADFI_218_DATE_TO_TEXT */
{ &Adi_fis[953], adu_9mnytostr, NULL, NULL },	/* ADFI_219_MONEY_TO_C */
{ &Adi_fis[954], adu_9mnytostr, NULL, NULL },	/* ADFI_220_MONEY_TO_TEXT */
{ &Adi_fis[955], adu_2strtomny, NULL, NULL },	/* ADFI_221_C_TO_MONEY */
{ &Adi_fis[956], adu_2strtomny, NULL, NULL },	/* ADFI_222_TEXT_TO_MONEY */
{ &Adi_fis[957], adu_numtomny, NULL, NULL },	/* ADFI_223_F_TO_MONEY */
{ &Adi_fis[959], adu_numtomny, NULL, NULL },	/* ADFI_224_I_TO_MONEY */
{ &Adi_fis[960], adu_1mnytonum, NULL, NULL },	/* ADFI_225_MONEY_TO_F */
{ &Adi_fis[962], adu_1strtostr, NULL, NULL },	/* ADFI_226_C_TO_TEXT */
{ &Adi_fis[963], adu_1strtostr, NULL, NULL },	/* ADFI_227_TEXT_TO_C */
{ &Adi_fis[1013], adu_1strtostr, NULL, NULL },	/* ADFI_228_C_TO_C */
{ &Adi_fis[1014], adu_1strtostr, NULL, NULL },	/* ADFI_229_TEXT_TO_TEXT */
{ &Adi_fis[1015], adu_1flt_coerce, NULL, NULL },	/* ADFI_230_F_TO_F */
{ &Adi_fis[1017], adu_1int_coerce, NULL, NULL },	/* ADFI_231_I_TO_I */
{ &Adi_fis[1018], adu_2mnytomny, NULL, NULL },	/* ADFI_232_MONEY_TO_MONEY */
{ &Adi_fis[1019], adu_2datetodate, NULL, NULL },	/* ADFI_233_DATE_TO_DATE */
{ &Adi_fis[69], NULL, NULL, NULL },	/* ADFI_234_CHAR_EQ_CHAR */
{ &Adi_fis[70], NULL, NULL, NULL },	/* ADFI_235_VARCHAR_EQ_VARCHAR */
{ &Adi_fis[3], NULL, NULL, NULL },	/* ADFI_236_CHAR_NE_CHAR */
{ &Adi_fis[4], NULL, NULL, NULL },	/* ADFI_237_VARCHAR_NE_VARCHAR */
{ &Adi_fis[47], NULL, NULL, NULL },	/* ADFI_238_CHAR_LE_CHAR */
{ &Adi_fis[48], NULL, NULL, NULL },	/* ADFI_239_VARCHAR_LE_VARCHAR */
{ &Adi_fis[92], NULL, NULL, NULL },	/* ADFI_240_CHAR_GT_CHAR */
{ &Adi_fis[93], NULL, NULL, NULL },	/* ADFI_241_VARCHAR_GT_VARCHAR */
{ &Adi_fis[114], NULL, NULL, NULL },	/* ADFI_242_CHAR_GE_CHAR */
{ &Adi_fis[115], NULL, NULL, NULL },	/* ADFI_243_VARCHAR_GE_VARCHAR */
{ &Adi_fis[25], NULL, NULL, NULL },	/* ADFI_244_CHAR_LT_CHAR */
{ &Adi_fis[26], NULL, NULL, NULL },	/* ADFI_245_VARCHAR_LT_VARCHAR */
{ &Adi_fis[159], NULL, NULL, NULL },	/* ADFI_246_C_LIKE_C */
{ &Adi_fis[160], NULL, NULL, NULL },	/* ADFI_247_CHAR_LIKE_CHAR */
{ &Adi_fis[161], NULL, NULL, NULL },	/* ADFI_248_TEXT_LIKE_TEXT */
{ &Adi_fis[162], NULL, NULL, NULL },	/* ADFI_249_VARCHAR_LIKE_VARCHAR */
{ &Adi_fis[164], NULL, NULL, NULL },	/* ADFI_250_C_NOTLIKE_C */
{ &Adi_fis[165], NULL, NULL, NULL },	/* ADFI_251_CHAR_NOTLIKE_CHAR */
{ &Adi_fis[166], NULL, NULL, NULL },	/* ADFI_252_TEXT_NOTLIKE_TEXT */
{ &Adi_fis[167], NULL, NULL, NULL },	/* ADFI_253_VARCHAR_NOTLIKE_VARCHA */
{ &Adi_fis[212], adu_4strconcat, NULL, NULL },	/* ADFI_254_CHAR_ADD_CHAR */
{ &Adi_fis[214], adu_4strconcat, NULL, NULL },	/* ADFI_255_VARCHAR_ADD_C */
{ &Adi_fis[216], adu_4strconcat, NULL, NULL },	/* ADFI_256_VARCHAR_ADD_CHAR */
{ &Adi_fis[217], adu_4strconcat, NULL, NULL },	/* ADFI_257_VARCHAR_ADD_VARCHAR */
{ &Adi_fis[267], adu_N1a_any, adu_B0a_noagwork, adu_E0a_anycount },	/* ADFI_258_ANY_CHAR */
{ &Adi_fis[274], adu_N1a_any, adu_B0a_noagwork, adu_E0a_anycount },	/* ADFI_259_ANY_VARCHAR */
{ &Adi_fis[285], adu_N2a_count, adu_B0a_noagwork, adu_E0a_anycount },	/* ADFI_260_COUNT_CHAR */
{ &Adi_fis[292], adu_N2a_count, adu_B0a_noagwork, adu_E0a_anycount },	/* ADFI_261_COUNT_VARCHAR */
{ &Adi_fis[299], adu_N6s_max_str, adu_B0s_minmax_str, adu_E0s_minmax_str },	/* ADFI_262_MAX_CHAR */
{ &Adi_fis[306], adu_N6s_max_str, adu_B0s_minmax_str, adu_E0s_minmax_str },	/* ADFI_263_MAX_VARCHAR */
{ &Adi_fis[312], adu_N5s_min_str, adu_B0s_minmax_str, adu_E0s_minmax_str },	/* ADFI_264_MIN_CHAR */
{ &Adi_fis[321], adu_N5s_min_str, adu_B0s_minmax_str, adu_E0s_minmax_str },	/* ADFI_265_MIN_VARCHAR */
{ NULL, NULL, NULL, NULL },	/* No FI 266 */
{ NULL, NULL, NULL, NULL },	/* No FI 267 */
{ NULL, NULL, NULL, NULL },	/* No FI 268 */
{ NULL, NULL, NULL, NULL },	/* No FI 269 */
{ NULL, NULL, NULL, NULL },	/* No FI 270 */
{ NULL, NULL, NULL, NULL },	/* No FI 271 */
{ NULL, NULL, NULL, NULL },	/* No FI 272 */
{ NULL, NULL, NULL, NULL },	/* No FI 273 */
{ NULL, NULL, NULL, NULL },	/* No FI 274 */
{ NULL, NULL, NULL, NULL },	/* No FI 275 */
{ &Adi_fis[382], adu_4strconcat, NULL, NULL },	/* ADFI_276_CONCAT_CHAR_CHAR */
{ &Adi_fis[384], adu_4strconcat, NULL, NULL },	/* ADFI_277_CONCAT_VARCHAR_C */
{ &Adi_fis[386], adu_4strconcat, NULL, NULL },	/* ADFI_278_CONCAT_VARCHAR_CHAR */
{ &Adi_fis[387], adu_4strconcat, NULL, NULL },	/* ADFI_279_CONCAT_VARCHAR_VARCHAR */
{ &Adi_fis[399], adu_14strtodate, NULL, NULL },	/* ADFI_280_DATE_CHAR */
{ &Adi_fis[401], adu_14strtodate, NULL, NULL },	/* ADFI_281_DATE_VARCHAR */
{ &Adi_fis[406], adu_dextract, NULL, NULL },	/* ADFI_282_DATE_PART_CHAR_DATE */
{ &Adi_fis[408], adu_dextract, NULL, NULL },	/* ADFI_283_DATE_PART_VARCHAR_DATE */
{ &Adi_fis[467], adu_intrvl_in, NULL, NULL },	/* ADFI_284_INTERVAL_CHAR_DATE */
{ &Adi_fis[469], adu_intrvl_in, NULL, NULL },	/* ADFI_285_INTERVAL_VARCHAR_DATE */
{ &Adi_fis[410], adu_dtruncate, NULL, NULL },	/* ADFI_286_DATE_TRUNC_CHAR_DATE */
{ &Adi_fis[412], adu_dtruncate, NULL, NULL },	/* ADFI_287_DATE_TRUNC_VARCHAR_DAT */
{ &Adi_fis[417], adu_1flt_coerce, NULL, NULL },	/* ADFI_288_FLOAT4_CHAR */
{ &Adi_fis[423], adu_1flt_coerce, NULL, NULL },	/* ADFI_289_FLOAT4_VARCHAR */
{ &Adi_fis[427], adu_1flt_coerce, NULL, NULL },	/* ADFI_290_FLOAT8_CHAR */
{ &Adi_fis[433], adu_1flt_coerce, NULL, NULL },	/* ADFI_291_FLOAT8_VARCHAR */
{ &Adi_fis[437], adu_1int_coerce, NULL, NULL },	/* ADFI_292_INT1_CHAR */
{ &Adi_fis[443], adu_1int_coerce, NULL, NULL },	/* ADFI_293_INT1_VARCHAR */
{ &Adi_fis[447], adu_1int_coerce, NULL, NULL },	/* ADFI_294_INT2_CHAR */
{ &Adi_fis[453], adu_1int_coerce, NULL, NULL },	/* ADFI_295_INT2_VARCHAR */
{ &Adi_fis[457], adu_1int_coerce, NULL, NULL },	/* ADFI_296_INT4_CHAR */
{ &Adi_fis[463], adu_1int_coerce, NULL, NULL },	/* ADFI_297_INT4_VARCHAR */
{ &Adi_fis[471], adu_6strleft, NULL, NULL },	/* ADFI_298_LEFT_CHAR_I */
{ &Adi_fis[473], adu_6strleft, NULL, NULL },	/* ADFI_299_LEFT_VARCHAR_I */
{ &Adi_fis[481], adu_7strlength, NULL, NULL },	/* ADFI_300_LENGTH_CHAR */
{ &Adi_fis[483], adu_7strlength, NULL, NULL },	/* ADFI_301_LENGTH_VARCHAR */
{ NULL, NULL, NULL, NULL },	/* No FI 302 */
{ NULL, NULL, NULL, NULL },	/* No FI 303 */
{ &Adi_fis[497], adu_9strlower, NULL, NULL },	/* ADFI_304_LOWERCASE_CHAR */
{ &Adi_fis[499], adu_9strlower, NULL, NULL },	/* ADFI_305_LOWERCASE_VARCHAR */
{ &Adi_fis[511], adu_2strtomny, NULL, NULL },	/* ADFI_306_MONEY_CHAR */
{ &Adi_fis[512], adu_2strtomny, NULL, NULL },	/* ADFI_307_MONEY_VARCHAR */
{ &Adi_fis[519], adu_pad, NULL, NULL },	/* ADFI_308_PAD_VARCHAR */
{ &Adi_fis[523], adu_10strright, NULL, NULL },	/* ADFI_309_RIGHT_CHAR_I */
{ &Adi_fis[525], adu_10strright, NULL, NULL },	/* ADFI_310_RIGHT_VARCHAR_I */
{ &Adi_fis[533], adu_11strshift, NULL, NULL },	/* ADFI_311_SHIFT_CHAR_I */
{ &Adi_fis[535], adu_11strshift, NULL, NULL },	/* ADFI_312_SHIFT_VARCHAR_I */
{ &Adi_fis[543], adu_12strsize, NULL, NULL },	/* ADFI_313_SIZE_CHAR */
{ &Adi_fis[545], adu_12strsize, NULL, NULL },	/* ADFI_314_SIZE_VARCHAR */
{ &Adi_fis[557], adu_squeezewhite, NULL, NULL },	/* ADFI_315_SQUEEZE_CHAR */
{ &Adi_fis[559], adu_squeezewhite, NULL, NULL },	/* ADFI_316_SQUEEZE_VARCHAR */
{ NULL, NULL, NULL, NULL },	/* No FI 317 */
{ NULL, NULL, NULL, NULL },	/* No FI 318 */
{ &Adi_fis[567], adu_trim, NULL, NULL },	/* ADFI_319_TRIM_CHAR */
{ &Adi_fis[569], adu_trim, NULL, NULL },	/* ADFI_320_TRIM_VARCHAR */
{ &Adi_fis[573], adu_15strupper, NULL, NULL },	/* ADFI_321_UPPERCASE_CHAR */
{ &Adi_fis[575], adu_15strupper, NULL, NULL },	/* ADFI_322_UPPERCASE_VARCHAR */
{ NULL, NULL, NULL, NULL },	/* No FI 323 */
{ NULL, NULL, NULL, NULL },	/* No FI 324 */
{ NULL, NULL, NULL, NULL },	/* No FI 325 */
{ NULL, NULL, NULL, NULL },	/* No FI 326 */
{ NULL, NULL, NULL, NULL },	/* No FI 327 */
{ NULL, NULL, NULL, NULL },	/* No FI 328 */
{ NULL, NULL, NULL, NULL },	/* No FI 329 */
{ NULL, NULL, NULL, NULL },	/* No FI 330 */
{ &Adi_fis[966], adu_1strtostr, NULL, NULL },	/* ADFI_331_C_TO_CHAR */
{ &Adi_fis[967], adu_1strtostr, NULL, NULL },	/* ADFI_332_C_TO_LONGTEXT */
{ &Adi_fis[968], adu_1strtostr, NULL, NULL },	/* ADFI_333_C_TO_VARCHAR */
{ &Adi_fis[969], adu_1strtostr, NULL, NULL },	/* ADFI_334_CHAR_TO_C */
{ &Adi_fis[970], adu_1strtostr, NULL, NULL },	/* ADFI_335_CHAR_TO_CHAR */
{ &Adi_fis[971], adu_14strtodate, NULL, NULL },	/* ADFI_336_CHAR_TO_DATE */
{ &Adi_fis[972], adu_1strtostr, NULL, NULL },	/* ADFI_337_CHAR_TO_LONGTEXT */
{ &Adi_fis[973], adu_2strtomny, NULL, NULL },	/* ADFI_338_CHAR_TO_MONEY */
{ &Adi_fis[974], adu_1strtostr, NULL, NULL },	/* ADFI_339_CHAR_TO_TEXT */
{ &Adi_fis[975], adu_1strtostr, NULL, NULL },	/* ADFI_340_CHAR_TO_VARCHAR */
{ &Adi_fis[978], adu_6datetostr, NULL, NULL },	/* ADFI_341_DATE_TO_CHAR */
{ &Adi_fis[979], adu_6datetostr, NULL, NULL },	/* ADFI_342_DATE_TO_LONGTEXT */
{ &Adi_fis[980], adu_6datetostr, NULL, NULL },	/* ADFI_343_DATE_TO_VARCHAR */
{ &Adi_fis[981], adu_ascii, NULL, NULL },	/* ADFI_344_F_TO_LONGTEXT */
{ &Adi_fis[983], adu_ascii, NULL, NULL },	/* ADFI_345_I_TO_LONGTEXT */
{ &Adi_fis[984], adu_1strtostr, NULL, NULL },	/* ADFI_346_LONGTEXT_TO_C */
{ &Adi_fis[985], adu_1strtostr, NULL, NULL },	/* ADFI_347_LONGTEXT_TO_CHAR */
{ &Adi_fis[986], adu_14strtodate, NULL, NULL },	/* ADFI_348_LONGTEXT_TO_DATE */
{ &Adi_fis[987], adu_1flt_coerce, NULL, NULL },	/* ADFI_349_LONGTEXT_TO_F */
{ &Adi_fis[989], adu_1int_coerce, NULL, NULL },	/* ADFI_350_LONGTEXT_TO_I */
{ &Adi_fis[993], adu_1strtostr, NULL, NULL },	/* ADFI_351_LONGTEXT_TO_LONGTEXT */
{ &Adi_fis[994], adu_2strtomny, NULL, NULL },	/* ADFI_352_LONGTEXT_TO_MONEY */
{ &Adi_fis[998], adu_1strtostr, NULL, NULL },	/* ADFI_353_LONGTEXT_TO_TEXT */
{ &Adi_fis[999], adu_1strtostr, NULL, NULL },	/* ADFI_354_LONGTEXT_TO_VARCHAR */
{ &Adi_fis[1000], adu_9mnytostr, NULL, NULL },	/* ADFI_355_MONEY_TO_CHAR */
{ &Adi_fis[1001], adu_9mnytostr, NULL, NULL },	/* ADFI_356_MONEY_TO_LONGTEXT */
{ &Adi_fis[1002], adu_9mnytostr, NULL, NULL },	/* ADFI_357_MONEY_TO_VARCHAR */
{ &Adi_fis[1003], adu_1strtostr, NULL, NULL },	/* ADFI_358_TEXT_TO_CHAR */
{ &Adi_fis[1004], adu_1strtostr, NULL, NULL },	/* ADFI_359_TEXT_TO_LONGTEXT */
{ &Adi_fis[1005], adu_1strtostr, NULL, NULL },	/* ADFI_360_TEXT_TO_VARCHAR */
{ &Adi_fis[1006], adu_1strtostr, NULL, NULL },	/* ADFI_361_VARCHAR_TO_C */
{ &Adi_fis[1007], adu_1strtostr, NULL, NULL },	/* ADFI_362_VARCHAR_TO_CHAR */
{ &Adi_fis[1008], adu_14strtodate, NULL, NULL },	/* ADFI_363_VARCHAR_TO_DATE */
{ &Adi_fis[1009], adu_1strtostr, NULL, NULL },	/* ADFI_364_VARCHAR_TO_LONGTEXT */
{ &Adi_fis[1010], adu_2strtomny, NULL, NULL },	/* ADFI_365_VARCHAR_TO_MONEY */
{ &Adi_fis[1011], adu_1strtostr, NULL, NULL },	/* ADFI_366_VARCHAR_TO_TEXT */
{ &Adi_fis[1012], adu_1strtostr, NULL, NULL },	/* ADFI_367_VARCHAR_TO_VARCHAR */
{ &Adi_fis[587], adu_hex, NULL, NULL },	/* ADFI_368_HEX_CHAR */
{ &Adi_fis[588], adu_hex, NULL, NULL },	/* ADFI_369_HEX_TEXT */
{ &Adi_fis[589], adu_hex, NULL, NULL },	/* ADFI_370_HEX_VARCHAR */
{ &Adi_fis[590], adu_hex, NULL, NULL },	/* ADFI_371_HEX_C */
{ &Adi_fis[591], adu_ifnull, NULL, NULL },	/* ADFI_372_IFNULL_C_C */
{ &Adi_fis[592], adu_ifnull, NULL, NULL },	/* ADFI_373_IFNULL_CHAR_CHAR */
{ &Adi_fis[593], adu_ifnull, NULL, NULL },	/* ADFI_374_IFNULL_DATE_DATE */
{ &Adi_fis[594], adu_ifnull, NULL, NULL },	/* ADFI_375_IFNULL_F_F */
{ &Adi_fis[596], adu_ifnull, NULL, NULL },	/* ADFI_376_IFNULL_I_I */
{ &Adi_fis[597], adu_ifnull, NULL, NULL },	/* ADFI_377_IFNULL_MONEY_MONEY */
{ &Adi_fis[598], adu_ifnull, NULL, NULL },	/* ADFI_378_IFNULL_TEXT_TEXT */
{ &Adi_fis[599], adu_ifnull, NULL, NULL },	/* ADFI_379_IFNULL_VARCHAR_VARCHAR */
{ &Adi_fis[134], NULL, NULL, NULL },	/* ADFI_380_I_ISNULL */
{ &Adi_fis[135], NULL, NULL, NULL },	/* ADFI_381_F_ISNULL */
{ &Adi_fis[136], NULL, NULL, NULL },	/* ADFI_382_MONEY_ISNULL */
{ &Adi_fis[137], NULL, NULL, NULL },	/* ADFI_383_DATE_ISNULL */
{ &Adi_fis[138], NULL, NULL, NULL },	/* ADFI_384_C_ISNULL */
{ &Adi_fis[139], NULL, NULL, NULL },	/* ADFI_385_CHAR_ISNULL */
{ &Adi_fis[140], NULL, NULL, NULL },	/* ADFI_386_TEXT_ISNULL */
{ &Adi_fis[141], NULL, NULL, NULL },	/* ADFI_387_VARCHAR_ISNULL */
{ &Adi_fis[147], NULL, NULL, NULL },	/* ADFI_388_I_ISNOTNULL */
{ &Adi_fis[148], NULL, NULL, NULL },	/* ADFI_389_F_ISNOTNULL */
{ &Adi_fis[149], NULL, NULL, NULL },	/* ADFI_390_MONEY_ISNOTNULL */
{ &Adi_fis[150], NULL, NULL, NULL },	/* ADFI_391_DATE_ISNOTNULL */
{ &Adi_fis[151], NULL, NULL, NULL },	/* ADFI_392_C_ISNOTNULL */
{ &Adi_fis[152], NULL, NULL, NULL },	/* ADFI_393_CHAR_ISNOTNULL */
{ &Adi_fis[153], NULL, NULL, NULL },	/* ADFI_394_TEXT_ISNOTNULL */
{ &Adi_fis[154], NULL, NULL, NULL },	/* ADFI_395_VARCHAR_ISNOTNULL */
{ &Adi_fis[297], adu_N2a_count, adu_B0a_noagwork, adu_E0a_anycount },	/* ADFI_396_COUNTSTAR */
{ &Adi_fis[372], adu_dbmsinfo, NULL, NULL },	/* ADFI_397_DBMSINFO_VARCHAR */
{ &Adi_fis[396], adu_typename, NULL, NULL },	/* ADFI_398_IITYPENAME_I */
{ &Adi_fis[602], adu_17structure, NULL, NULL },	/* ADFI_399_IISTRUCTURE_I */
{ &Adi_fis[581], adu_userlen, NULL, NULL },	/* ADFI_400_IIUSERLEN_I_I */
{ &Adi_fis[603], adu_dgmt, NULL, NULL },	/* ADFI_401_DATE_GMT_DATE */
{ &Adi_fis[604], adu_1strtostr, NULL, NULL },	/* ADFI_402_IICHAR12_C */
{ &Adi_fis[605], adu_1strtostr, NULL, NULL },	/* ADFI_403_IICHAR12_TEXT */
{ &Adi_fis[606], adu_1strtostr, NULL, NULL },	/* ADFI_404_IICHAR12_CHAR */
{ &Adi_fis[607], adu_1strtostr, NULL, NULL },	/* ADFI_405_IICHAR12_VARCHAR */
{ &Adi_fis[890], adu_16strindex, NULL, NULL },	/* ADFI_406_BYTEEXTRACT_C_I */
{ &Adi_fis[891], adu_16strindex, NULL, NULL },	/* ADFI_407_BYTEEXTRACT_TEXT_I */
{ &Adi_fis[892], adu_16strindex, NULL, NULL },	/* ADFI_408_BYTEEXTRACT_CHAR_I */
{ &Adi_fis[893], adu_16strindex, NULL, NULL },	/* ADFI_409_BYTEEXTRACT_VARCHAR_I */
{ &Adi_fis[177], NULL, NULL, NULL },	/* ADFI_410_F_MUL_I */
{ &Adi_fis[181], NULL, NULL, NULL },	/* ADFI_411_I_MUL_F */
{ &Adi_fis[240], NULL, NULL, NULL },	/* ADFI_412_F_DIV_I */
{ &Adi_fis[241], NULL, NULL, NULL },	/* ADFI_413_I_DIV_F */
{ &Adi_fis[614], adu_notrm, NULL, NULL },	/* ADFI_414_II_NOTRM_TXT_CHAR */
{ &Adi_fis[615], adu_notrm, NULL, NULL },	/* ADFI_415_II_NOTRM_VCH_CHAR */
{ &Adi_fis[894], adu_copascii, NULL, NULL },	/* ADFI_416_I_COPYTO_CHAR */
{ &Adi_fis[896], adu_copascii, NULL, NULL },	/* ADFI_417_F_COPYTO_CHAR */
{ &Adi_fis[897], adu_copascii, NULL, NULL },	/* ADFI_418_I_COPYTO_VARCHAR */
{ &Adi_fis[899], adu_copascii, NULL, NULL },	/* ADFI_419_F_COPYTO_VARCHAR */
{ &Adi_fis[900], adu_copascii, NULL, NULL },	/* ADFI_420_I_COPYTO_C */
{ &Adi_fis[902], adu_copascii, NULL, NULL },	/* ADFI_421_F_COPYTO_C */
{ &Adi_fis[903], adu_copascii, NULL, NULL },	/* ADFI_422_I_COPYTO_TEXT */
{ &Adi_fis[905], adu_copascii, NULL, NULL },	/* ADFI_423_F_COPYTO_TEXT */
{ &Adi_fis[906], adu_1int_coerce, NULL, NULL },	/* ADFI_424_MONEY_COPYTO_I */
{ &Adi_fis[907], adu_1int_coerce, NULL, NULL },	/* ADFI_425_CHAR_COPYTO_I */
{ &Adi_fis[908], adu_1int_coerce, NULL, NULL },	/* ADFI_426_VARCHAR_COPYTO_I */
{ &Adi_fis[909], adu_1int_coerce, NULL, NULL },	/* ADFI_427_C_COPYTO_I */
{ &Adi_fis[910], adu_1int_coerce, NULL, NULL },	/* ADFI_428_TEXT_COPYTO_I */
{ &Adi_fis[915], adu_1flt_coerce, NULL, NULL },	/* ADFI_429_CHAR_COPYTO_F */
{ &Adi_fis[916], adu_1flt_coerce, NULL, NULL },	/* ADFI_430_VARCHAR_COPYTO_F */
{ &Adi_fis[917], adu_1flt_coerce, NULL, NULL },	/* ADFI_431_C_COPYTO_F */
{ &Adi_fis[918], adu_1flt_coerce, NULL, NULL },	/* ADFI_432_TEXT_COPYTO_F */
{ &Adi_fis[616], adu_xyzzy, NULL, NULL },	/* ADFI_433_XYZZY_VARCHAR */
{ &Adi_fis[919], adu_cpmnytostr, NULL, NULL },	/* ADFI_434_MONEY_COPYTO_C */
{ &Adi_fis[920], adu_cpmnytostr, NULL, NULL },	/* ADFI_435_MONEY_COPYTO_CHAR */
{ &Adi_fis[921], adu_cpmnytostr, NULL, NULL },	/* ADFI_436_MONEY_COPYTO_TEXT */
{ &Adi_fis[922], adu_cpmnytostr, NULL, NULL },	/* ADFI_437_MONEY_COPYTO_VARCHAR */
{ &Adi_fis[617], adu_hxnum, NULL, NULL },	/* ADFI_438_IIHEXINT_TEXT */
{ &Adi_fis[618], adu_hxnum, NULL, NULL },	/* ADFI_439_IIHEXINT_VARCHAR */
{ &Adi_fis[515], adu_2mnytomny, NULL, NULL },	/* ADFI_440_MONEY_MONEY */
{ &Adi_fis[404], adu_2datetodate, NULL, NULL },	/* ADFI_441_DATE_DATE */
{ &Adi_fis[345], NULL, NULL, NULL },	/* ADFI_442__BINTIM */
{ &Adi_fis[619], adu_tb2di, NULL, NULL },	/* ADFI_443_II_TABID_DI_I_I */
{ &Adi_fis[620], adu_di2tb, NULL, NULL },	/* ADFI_444_II_DI_TABID_CHAR */
{ &Adi_fis[556], adu_squeezewhite, NULL, NULL },	/* ADFI_445_SQUEEZE_C */
{ &Adi_fis[516], adu_pad, NULL, NULL },	/* ADFI_446_PAD_C */
{ &Adi_fis[517], adu_pad, NULL, NULL },	/* ADFI_447_PAD_CHAR */
{ &Adi_fis[210], adu_4strconcat, NULL, NULL },	/* ADFI_448_TEXT_ADD_CHAR */
{ &Adi_fis[211], adu_4strconcat, NULL, NULL },	/* ADFI_449_TEXT_ADD_VARCHAR */
{ &Adi_fis[380], adu_4strconcat, NULL, NULL },	/* ADFI_450_CONCAT_TEXT_CHAR */
{ &Adi_fis[381], adu_4strconcat, NULL, NULL },	/* ADFI_451_CONCAT_TEXT_VARCHAR */
{ &Adi_fis[213], adu_4strconcat, NULL, NULL },	/* ADFI_452_CHAR_ADD_VARCHAR */
{ &Adi_fis[215], adu_4strconcat, NULL, NULL },	/* ADFI_453_VARCHAR_ADD_TEXT */
{ &Adi_fis[383], adu_4strconcat, NULL, NULL },	/* ADFI_454_CONCAT_CHAR_VARCHAR */
{ &Adi_fis[385], adu_4strconcat, NULL, NULL },	/* ADFI_455_CONCAT_VARCHAR_TEXT */
{ NULL, NULL, NULL, NULL },	/* No FI 456 */
{ NULL, NULL, NULL, NULL },	/* No FI 457 */
{ NULL, NULL, NULL, NULL },	/* No FI 458 */
{ NULL, NULL, NULL, NULL },	/* No FI 459 */
{ NULL, NULL, NULL, NULL },	/* No FI 460 */
{ NULL, NULL, NULL, NULL },	/* No FI 461 */
{ NULL, NULL, NULL, NULL },	/* No FI 462 */
{ NULL, NULL, NULL, NULL },	/* No FI 463 */
{ &Adi_fis[283], adu_N0d_tot_date, adu_B0a_noagwork, adu_E3d_avg_date },	/* ADFI_464_AVG_DATE */
{ &Adi_fis[328], adu_N0d_tot_date, adu_B0a_noagwork, adu_E4d_sum_date },	/* ADFI_465_SUM_DATE */
{ &Adi_fis[12], adu_1logkey_cmp, NULL, NULL },	/* ADFI_466_LOGKEY_NE_LOGKEY */
{ &Adi_fis[57], adu_1logkey_cmp, NULL, NULL },	/* ADFI_467_LOGKEY_LE_LOGKEY */
{ &Adi_fis[102], adu_1logkey_cmp, NULL, NULL },	/* ADFI_468_LOGKEY_GT_LOGKEY */
{ &Adi_fis[123], adu_1logkey_cmp, NULL, NULL },	/* ADFI_469_LOGKEY_GE_LOGKEY */
{ &Adi_fis[35], adu_1logkey_cmp, NULL, NULL },	/* ADFI_470_LOGKEY_LT_LOGKEY */
{ &Adi_fis[143], NULL, NULL, NULL },	/* ADFI_471_LOGKEY_ISNULL */
{ &Adi_fis[155], NULL, NULL, NULL },	/* ADFI_472_LOGKEY_ISNOTNULL */
{ NULL, NULL, NULL, NULL },	/* No FI 473 */
{ &Adi_fis[634], adu_4strtologkey, NULL, NULL },	/* ADFI_474_LOGKEY_CHAR */
{ &Adi_fis[635], adu_2logkeytologkey, NULL, NULL },	/* ADFI_475_LOGKEY_LOGKEY */
{ &Adi_fis[976], adu_4strtologkey, NULL, NULL },	/* ADFI_476_CHAR_TO_LOGKEY */
{ &Adi_fis[1020], adu_3logkeytostr, NULL, NULL },	/* ADFI_477_LOGKEY_TO_CHAR */
{ &Adi_fis[1024], adu_2logkeytologkey, NULL, NULL },	/* ADFI_478_LOGKEY_TO_LOGKEY */
{ &Adi_fis[275], adu_N1a_any, adu_B0a_noagwork, adu_E0a_anycount },	/* ADFI_479_ANY_LOGKEY */
{ &Adi_fis[293], adu_N2a_count, adu_B0a_noagwork, adu_E0a_anycount },	/* ADFI_480_COUNT_LOGKEY */
{ &Adi_fis[307], adu_N6l_max_logk, adu_B0l_minmax_logk, adu_E0l_minmax_logk },	/* ADFI_481_MAX_LOGKEY */
{ &Adi_fis[319], adu_N5l_min_logk, adu_B0l_minmax_logk, adu_E0l_minmax_logk },	/* ADFI_482_MIN_LOGKEY */
{ NULL, NULL, NULL, NULL },	/* No FI 483 */
{ &Adi_fis[79], adu_1logkey_cmp, NULL, NULL },	/* ADFI_484_TABKEY_EQ_TABKEY */
{ &Adi_fis[13], adu_1logkey_cmp, NULL, NULL },	/* ADFI_485_TABKEY_NE_TABKEY */
{ &Adi_fis[56], adu_1logkey_cmp, NULL, NULL },	/* ADFI_486_TABKEY_LE_TABKEY */
{ &Adi_fis[101], adu_1logkey_cmp, NULL, NULL },	/* ADFI_487_TABKEY_GT_TABKEY */
{ &Adi_fis[124], adu_1logkey_cmp, NULL, NULL },	/* ADFI_488_TABKEY_GE_TABKEY */
{ &Adi_fis[34], adu_1logkey_cmp, NULL, NULL },	/* ADFI_489_TABKEY_LT_TABKEY */
{ &Adi_fis[142], NULL, NULL, NULL },	/* ADFI_490_TABKEY_ISNULL */
{ &Adi_fis[156], NULL, NULL, NULL },	/* ADFI_491_TABKEY_ISNOTNULL */
{ NULL, NULL, NULL, NULL },	/* No FI 492 */
{ &Adi_fis[636], adu_4strtologkey, NULL, NULL },	/* ADFI_493_TABKEY_CHAR */
{ &Adi_fis[637], adu_2logkeytologkey, NULL, NULL },	/* ADFI_494_TABKEY_TABKEY */
{ &Adi_fis[977], adu_4strtologkey, NULL, NULL },	/* ADFI_495_CHAR_TO_TABKEY */
{ &Adi_fis[1025], adu_3logkeytostr, NULL, NULL },	/* ADFI_496_TABKEY_TO_CHAR */
{ &Adi_fis[1029], adu_2logkeytologkey, NULL, NULL },	/* ADFI_497_TABKEY_TO_TABKEY */
{ &Adi_fis[276], adu_N1a_any, adu_B0a_noagwork, adu_E0a_anycount },	/* ADFI_498_ANY_TABKEY */
{ &Adi_fis[294], adu_N2a_count, adu_B0a_noagwork, adu_E0a_anycount },	/* ADFI_499_COUNT_TABKEY */
{ &Adi_fis[308], adu_N6l_max_logk, adu_B0l_minmax_logk, adu_E0l_minmax_logk },	/* ADFI_500_MAX_TABKEY */
{ &Adi_fis[320], adu_N5l_min_logk, adu_B0l_minmax_logk, adu_E0l_minmax_logk },	/* ADFI_501_MIN_TABKEY */
{ NULL, NULL, NULL, NULL },	/* No FI 502 */
{ &Adi_fis[78], adu_1logkey_cmp, NULL, NULL },	/* ADFI_503_LOGKEY_EQ_LOGKEY */
{ &Adi_fis[632], adu_exttype, NULL, NULL },	/* ADFI_504_II_EXT_TYPE */
{ &Adi_fis[633], adu_extlength, NULL, NULL },	/* ADFI_505_II_EXT_LENGTH */
{ &Adi_fis[1023], adu_3logkeytostr, NULL, NULL },	/* ADFI_506_LOGKEY_TO_LONGTEXT */
{ &Adi_fis[992], adu_4strtologkey, NULL, NULL },	/* ADFI_507_LONGTEXT_TO_LOGKEY */
{ &Adi_fis[1028], adu_3logkeytostr, NULL, NULL },	/* ADFI_508_TABKEY_TO_LONGTEXT */
{ &Adi_fis[997], adu_4strtologkey, NULL, NULL },	/* ADFI_509_LONGTEXT_TO_TABKEY */
{ &Adi_fis[74], NULL, NULL, NULL },	/* ADFI_510_DEC_EQ_DEC */
{ &Adi_fis[8], NULL, NULL, NULL },	/* ADFI_511_DEC_NE_DEC */
{ &Adi_fis[52], NULL, NULL, NULL },	/* ADFI_512_DEC_LE_DEC */
{ &Adi_fis[97], NULL, NULL, NULL },	/* ADFI_513_DEC_GT_DEC */
{ &Adi_fis[120], NULL, NULL, NULL },	/* ADFI_514_DEC_GE_DEC */
{ &Adi_fis[30], NULL, NULL, NULL },	/* ADFI_515_DEC_LT_DEC */
{ &Adi_fis[133], NULL, NULL, NULL },	/* ADFI_516_ALL_ISNULL */
{ &Adi_fis[146], NULL, NULL, NULL },	/* ADFI_517_ALL_ISNOTNULL */
{ &Adi_fis[201], NULL, NULL, NULL },	/* ADFI_518_DEC_ADD_DEC */
{ &Adi_fis[235], NULL, NULL, NULL },	/* ADFI_519_DEC_SUB_DEC */
{ &Adi_fis[178], NULL, NULL, NULL },	/* ADFI_520_DEC_MUL_DEC */
{ &Adi_fis[176], NULL, NULL, NULL },	/* ADFI_521_F_MUL_DEC */
{ &Adi_fis[179], NULL, NULL, NULL },	/* ADFI_522_DEC_MUL_F */
{ &Adi_fis[182], NULL, NULL, NULL },	/* ADFI_523_I_MUL_DEC */
{ &Adi_fis[180], NULL, NULL, NULL },	/* ADFI_524_DEC_MUL_I */
{ &Adi_fis[243], NULL, NULL, NULL },	/* ADFI_525_DEC_DIV_DEC */
{ &Adi_fis[239], NULL, NULL, NULL },	/* ADFI_526_F_DIV_DEC */
{ &Adi_fis[242], NULL, NULL, NULL },	/* ADFI_527_DEC_DIV_F */
{ &Adi_fis[245], NULL, NULL, NULL },	/* ADFI_528_I_DIV_DEC */
{ &Adi_fis[244], NULL, NULL, NULL },	/* ADFI_529_DEC_DIV_I */
{ &Adi_fis[257], NULL, NULL, NULL },	/* ADFI_530_PLUS_DEC */
{ &Adi_fis[262], NULL, NULL, NULL },	/* ADFI_531_MINUS_DEC */
{ &Adi_fis[270], adu_N1a_any, adu_B0a_noagwork, adu_E0a_anycount },	/* ADFI_532_ANY_DEC */
{ &Adi_fis[280], adu_N3n_avg_dec, adu_B0n_agg_dec, adu_E3n_avg_dec },	/* ADFI_533_AVG_DEC */
{ &Adi_fis[288], adu_N2a_count, adu_B0a_noagwork, adu_E0a_anycount },	/* ADFI_534_COUNT_DEC */
{ &Adi_fis[302], adu_N6n_max_dec, adu_B0n_agg_dec, adu_E0n_minmax_dec },	/* ADFI_535_MAX_DEC */
{ &Adi_fis[315], adu_N5n_min_dec, adu_B0n_agg_dec, adu_E0n_minmax_dec },	/* ADFI_536_MIN_DEC */
{ &Adi_fis[325], adu_N4n_sum_dec, adu_B0n_agg_dec, adu_E4n_sum_dec },	/* ADFI_537_SUM_DEC */
{ &Adi_fis[364], NULL, NULL, NULL },	/* ADFI_538_ABS_DEC */
{ &Adi_fis[367], adu_ascii, NULL, NULL },	/* ADFI_539_ASCII_ALL */
{ &Adi_fis[373], adu_ascii, NULL, NULL },	/* ADFI_540_CHAR_ALL */
{ &Adi_fis[622], adu_2dec_convert, NULL, NULL },	/* ADFI_541_DEC_C_I */
{ &Adi_fis[623], adu_2dec_convert, NULL, NULL },	/* ADFI_542_DEC_CHAR_I */
{ &Adi_fis[624], adu_2dec_convert, NULL, NULL },	/* ADFI_543_DEC_DEC_I */
{ &Adi_fis[625], adu_2dec_convert, NULL, NULL },	/* ADFI_544_DEC_F_I */
{ &Adi_fis[626], adu_2dec_convert, NULL, NULL },	/* ADFI_545_DEC_I_I */
{ &Adi_fis[627], adu_2dec_convert, NULL, NULL },	/* ADFI_546_DEC_MONEY_I */
{ &Adi_fis[628], adu_2dec_convert, NULL, NULL },	/* ADFI_547_DEC_TEXT_I */
{ &Adi_fis[629], adu_2dec_convert, NULL, NULL },	/* ADFI_548_DEC_VARCHAR_I */
{ &Adi_fis[419], adu_1flt_coerce, NULL, NULL },	/* ADFI_549_FLOAT4_DEC */
{ &Adi_fis[429], adu_1flt_coerce, NULL, NULL },	/* ADFI_550_FLOAT8_DEC */
{ &Adi_fis[595], adu_ifnull, NULL, NULL },	/* ADFI_551_IFNULL_DEC_DEC */
{ NULL, NULL, NULL, NULL },	/* No FI 552 */
{ &Adi_fis[439], adu_1int_coerce, NULL, NULL },	/* ADFI_553_INT1_DEC */
{ &Adi_fis[449], adu_1int_coerce, NULL, NULL },	/* ADFI_554_INT2_DEC */
{ &Adi_fis[459], adu_1int_coerce, NULL, NULL },	/* ADFI_555_INT4_DEC */
{ &Adi_fis[507], adu_numtomny, NULL, NULL },	/* ADFI_556_MONEY_DEC */
{ &Adi_fis[562], adu_ascii, NULL, NULL },	/* ADFI_557_TEXT_ALL */
{ &Adi_fis[583], adu_ascii, NULL, NULL },	/* ADFI_558_VARCHAR_ALL */
{ &Adi_fis[901], adu_copascii, NULL, NULL },	/* ADFI_559_DEC_COPYTO_C */
{ &Adi_fis[904], adu_copascii, NULL, NULL },	/* ADFI_560_DEC_COPYTO_TEXT */
{ &Adi_fis[895], adu_copascii, NULL, NULL },	/* ADFI_561_DEC_COPYTO_CHAR */
{ &Adi_fis[898], adu_copascii, NULL, NULL },	/* ADFI_562_DEC_COPYTO_VARCHAR */
{ &Adi_fis[914], adu_1dec_coerce, NULL, NULL },	/* ADFI_563_C_COPYTO_DEC */
{ &Adi_fis[913], adu_1dec_coerce, NULL, NULL },	/* ADFI_564_TEXT_COPYTO_DEC */
{ &Adi_fis[911], adu_1dec_coerce, NULL, NULL },	/* ADFI_565_CHAR_COPYTO_DEC */
{ &Adi_fis[912], adu_1dec_coerce, NULL, NULL },	/* ADFI_566_VARCHAR_COPYTO_DEC */
{ &Adi_fis[958], adu_numtomny, NULL, NULL },	/* ADFI_567_DEC_TO_MONEY */
{ &Adi_fis[932], adu_1flt_coerce, NULL, NULL },	/* ADFI_568_DEC_TO_F */
{ &Adi_fis[942], adu_1int_coerce, NULL, NULL },	/* ADFI_569_DEC_TO_I */
{ &Adi_fis[1016], adu_1dec_coerce, NULL, NULL },	/* ADFI_570_DEC_TO_DEC */
{ &Adi_fis[982], adu_ascii, NULL, NULL },	/* ADFI_571_DEC_TO_LONGTEXT */
{ &Adi_fis[988], adu_1dec_coerce, NULL, NULL },	/* ADFI_572_LONGTEXT_TO_DEC */
{ &Adi_fis[961], adu_1mnytonum, NULL, NULL },	/* ADFI_573_MONEY_TO_DEC */
{ &Adi_fis[939], adu_1dec_coerce, NULL, NULL },	/* ADFI_574_F_TO_DEC */
{ &Adi_fis[940], adu_1dec_coerce, NULL, NULL },	/* ADFI_575_I_TO_DEC */
{ NULL, NULL, NULL, NULL },	/* No FI 576 */
{ NULL, NULL, NULL, NULL },	/* No FI 577 */
{ NULL, NULL, NULL, NULL },	/* No FI 578 */
{ NULL, NULL, NULL, NULL },	/* No FI 579 */
{ NULL, NULL, NULL, NULL },	/* No FI 580 */
{ NULL, NULL, NULL, NULL },	/* No FI 581 */
{ NULL, NULL, NULL, NULL },	/* No FI 582 */
{ NULL, NULL, NULL, NULL },	/* No FI 583 */
{ NULL, NULL, NULL, NULL },	/* No FI 584 */
{ NULL, NULL, NULL, NULL },	/* No FI 585 */
{ &Adi_fis[370], adu_ascii_2arg, NULL, NULL },	/* ADFI_586_ASCII_ALL_I */
{ &Adi_fis[565], adu_ascii_2arg, NULL, NULL },	/* ADFI_587_TEXT_ALL_I */
{ NULL, NULL, NULL, NULL },	/* No FI 588 */
{ NULL, NULL, NULL, NULL },	/* No FI 589 */
{ NULL, NULL, NULL, NULL },	/* No FI 590 */
{ NULL, NULL, NULL, NULL },	/* No FI 591 */
{ NULL, NULL, NULL, NULL },	/* No FI 592 */
{ NULL, NULL, NULL, NULL },	/* No FI 593 */
{ NULL, NULL, NULL, NULL },	/* No FI 594 */
{ &Adi_fis[651], NULL, NULL, NULL },	/* ADFI_595_ROWCNT_I */
{ &Adi_fis[653], NULL, NULL, NULL },	/* ADFI_596_SYSUSER */
{ NULL, NULL, NULL, NULL },	/* No FI 597 */
{ NULL, NULL, NULL, NULL },	/* No FI 598 */
{ NULL, NULL, NULL, NULL },	/* No FI 599 */
{ NULL, NULL, NULL, NULL },	/* No FI 600 */
{ NULL, NULL, NULL, NULL },	/* No FI 601 */
{ NULL, NULL, NULL, NULL },	/* No FI 602 */
{ NULL, NULL, NULL, NULL },	/* No FI 603 */
{ &Adi_fis[638], adu_iftrue, NULL, NULL },	/* ADFI_604_II_IFTRUE_I_ALL */
{ &Adi_fis[639], adu_restab, NULL, NULL },	/* ADFI_605_RESTAB_TEXT */
{ &Adi_fis[640], adu_restab, NULL, NULL },	/* ADFI_606_RESTAB_VARCHAR */
{ &Adi_fis[1021], adu_3logkeytostr, NULL, NULL },	/* ADFI_607_LOGKEY_TO_VARCHAR */
{ &Adi_fis[990], adu_4strtologkey, NULL, NULL },	/* ADFI_608_VARCHAR_TO_LOGKEY */
{ &Adi_fis[1026], adu_3logkeytostr, NULL, NULL },	/* ADFI_609_TABKEY_TO_VARCHAR */
{ &Adi_fis[995], adu_4strtologkey, NULL, NULL },	/* ADFI_610_VARCHAR_TO_TABKEY */
{ &Adi_fis[1022], adu_3logkeytostr, NULL, NULL },	/* ADFI_611_LOGKEY_TO_TEXT */
{ &Adi_fis[991], adu_4strtologkey, NULL, NULL },	/* ADFI_612_TEXT_TO_LOGKEY */
{ &Adi_fis[1027], adu_3logkeytostr, NULL, NULL },	/* ADFI_613_TABKEY_TO_TEXT */
{ &Adi_fis[996], adu_4strtologkey, NULL, NULL },	/* ADFI_614_TEXT_TO_TABKEY */
{ &Adi_fis[641], adu_18cvrt_gmt, NULL, NULL },	/* ADFI_615_GMT_TIMESTAMP_I */
{ &Adi_fis[586], adu_ascii_2arg, NULL, NULL },	/* ADFI_616_VARCHAR_ALL_I */
{ &Adi_fis[376], adu_ascii_2arg, NULL, NULL },	/* ADFI_617_CHAR_ALL_I */
{ &Adi_fis[652], NULL, NULL, NULL },	/* ADFI_618_SESSUSER */
{ &Adi_fis[642], adu_cpn_dump, NULL, NULL },	/* ADFI_619_CPNDMP_LVCH */
{ &Adi_fis[1030], adu_lvch_move, NULL, NULL },	/* ADFI_620_CHAR_TO_LVCH */
{ &Adi_fis[1031], adu_lvch_move, NULL, NULL },	/* ADFI_621_C_TO_LVCH */
{ &Adi_fis[1032], adu_lvch_move, NULL, NULL },	/* ADFI_622_TEXT_TO_LVCH */
{ &Adi_fis[1033], adu_lvch_move, NULL, NULL },	/* ADFI_623_VARCHAR_TO_LVCH */
{ &Adi_fis[1034], adu_lvch_move, NULL, NULL },	/* ADFI_624_LVCH_TO_LVCH */
{ &Adi_fis[1035], adu_lvch_move, NULL, NULL },	/* ADFI_625_LTXT_TO_LVCH */
{ &Adi_fis[1036], adu_lvch_move, NULL, NULL },	/* ADFI_626_LVCH_TO_LTXT */
{ &Adi_fis[14], adc_bit_compare, NULL, NULL },	/* ADFI_627_BIT_NE_BIT */
{ &Adi_fis[15], adc_bit_compare, NULL, NULL },	/* ADFI_628_BIT_NE_VBIT */
{ &Adi_fis[16], adc_bit_compare, NULL, NULL },	/* ADFI_629_VBIT_NE_VBIT */
{ &Adi_fis[17], adc_bit_compare, NULL, NULL },	/* ADFI_630_VBIT_NE_BIT */
{ &Adi_fis[36], adc_bit_compare, NULL, NULL },	/* ADFI_631_BIT_LT_BIT */
{ &Adi_fis[37], adc_bit_compare, NULL, NULL },	/* ADFI_632_BIT_LT_VBIT */
{ &Adi_fis[38], adc_bit_compare, NULL, NULL },	/* ADFI_633_VBIT_LT_VBIT */
{ &Adi_fis[39], adc_bit_compare, NULL, NULL },	/* ADFI_634_VBIT_LT_BIT */
{ &Adi_fis[58], adc_bit_compare, NULL, NULL },	/* ADFI_635_BIT_LE_BIT */
{ &Adi_fis[59], adc_bit_compare, NULL, NULL },	/* ADFI_636_BIT_LE_VBIT */
{ &Adi_fis[60], adc_bit_compare, NULL, NULL },	/* ADFI_637_VBIT_LE_VBIT */
{ &Adi_fis[63], adc_bit_compare, NULL, NULL },	/* ADFI_638_VBIT_LE_BIT */
{ &Adi_fis[80], adc_bit_compare, NULL, NULL },	/* ADFI_639_BIT_EQ_BIT */
{ &Adi_fis[81], adc_bit_compare, NULL, NULL },	/* ADFI_640_BIT_EQ_VBIT */
{ &Adi_fis[82], adc_bit_compare, NULL, NULL },	/* ADFI_641_VBIT_EQ_VBIT */
{ &Adi_fis[83], adc_bit_compare, NULL, NULL },	/* ADFI_642_VBIT_EQ_BIT */
{ &Adi_fis[103], adc_bit_compare, NULL, NULL },	/* ADFI_643_BIT_GT_BIT */
{ &Adi_fis[104], adc_bit_compare, NULL, NULL },	/* ADFI_644_BIT_GT_VBIT */
{ &Adi_fis[105], adc_bit_compare, NULL, NULL },	/* ADFI_645_VBIT_GT_VBIT */
{ &Adi_fis[106], adc_bit_compare, NULL, NULL },	/* ADFI_646_VBIT_GT_BIT */
{ &Adi_fis[125], adc_bit_compare, NULL, NULL },	/* ADFI_647_BIT_GE_BIT */
{ &Adi_fis[126], adc_bit_compare, NULL, NULL },	/* ADFI_648_BIT_GE_VBIT */
{ &Adi_fis[127], adc_bit_compare, NULL, NULL },	/* ADFI_649_VBIT_GE_VBIT */
{ &Adi_fis[128], adc_bit_compare, NULL, NULL },	/* ADFI_650_VBIT_GE_BIT */
{ &Adi_fis[654], NULL, NULL, NULL },	/* ADFI_651_INITUSER */
{ &Adi_fis[655], adu_allocated_pages, NULL, NULL },	/* ADFI_652_IITOTAL_ALLOC_PG */
{ &Adi_fis[656], adu_overflow_pages, NULL, NULL },	/* ADFI_653_IITOTAL_OVERFL_PG */
{ &Adi_fis[621], adu_dvdsc, NULL, NULL },	/* ADFI_654_II_DV_DESC_ALLTYPE */
{ NULL, NULL, NULL, NULL },	/* No FI 655 */
{ NULL, NULL, NULL, NULL },	/* No FI 656 */
{ &Adi_fis[1085], adu_1strtostr, NULL, NULL },	/* ADFI_657_BYTE_TO_LONGTEXT */
{ NULL, NULL, NULL, NULL },	/* No FI 658 */
{ &Adi_fis[1084], adu_1strtostr, NULL, NULL },	/* ADFI_659_VARBYTE_TO_VARCHAR */
{ &Adi_fis[1083], adu_1strtostr, NULL, NULL },	/* ADFI_660_BYTE_TO_VARCHAR */
{ &Adi_fis[1080], adu_1strtostr, NULL, NULL },	/* ADFI_661_VARBYTE_TO_CHAR */
{ &Adi_fis[1079], adu_1strtostr, NULL, NULL },	/* ADFI_662_BYTE_TO_CHAR */
{ &Adi_fis[1074], adu_lvch_move, NULL, NULL },	/* ADFI_663_LBYTE_TO_LBYTE */
{ &Adi_fis[1073], adu_lvch_move, NULL, NULL },	/* ADFI_664_VARBYTE_TO_LBYTE */
{ NULL, NULL, NULL, NULL },	/* No FI 665 */
{ &Adi_fis[1072], adu_lvch_move, NULL, NULL },	/* ADFI_666_BYTE_TO_LBYTE */
{ &Adi_fis[1071], adu_lvch_move, NULL, NULL },	/* ADFI_667_LONGTEXT_TO_LBYTE */
{ NULL, NULL, NULL, NULL },	/* No FI 668 */
{ &Adi_fis[1070], adu_lvch_move, NULL, NULL },	/* ADFI_669_TEXT_TO_LBYTE */
{ &Adi_fis[1069], adu_lvch_move, NULL, NULL },	/* ADFI_670_C_TO_LBYTE */
{ &Adi_fis[1068], adu_lvch_move, NULL, NULL },	/* ADFI_671_VARCHAR_TO_LBYTE */
{ &Adi_fis[1067], adu_lvch_move, NULL, NULL },	/* ADFI_672_CHAR_TO_LBYTE */
{ NULL, NULL, NULL, NULL },	/* No FI 673 */
{ NULL, NULL, NULL, NULL },	/* No FI 674 */
{ &Adi_fis[484], adu_bitlength, NULL, NULL },	/* ADFI_675_LENGTH_BIT */
{ &Adi_fis[485], adu_bitlength, NULL, NULL },	/* ADFI_676_LENGTH_VBIT */
{ NULL, NULL, NULL, NULL },	/* No FI 677 */
{ NULL, NULL, NULL, NULL },	/* No FI 678 */
{ &Adi_fis[546], adu_bitsize, NULL, NULL },	/* ADFI_679_SIZE_BIT */
{ &Adi_fis[547], adu_bitsize, NULL, NULL },	/* ADFI_680_SIZE_VBIT */
{ &Adi_fis[1066], adu_1strtostr, NULL, NULL },	/* ADFI_681_VARBYTE_TO_VARBYTE */
{ &Adi_fis[1065], adu_1strtostr, NULL, NULL },	/* ADFI_682_BYTE_TO_VARBYTE */
{ &Adi_fis[643], adu_str2bit, NULL, NULL },	/* ADFI_683_BIT_VARCHAR */
{ &Adi_fis[644], adu_str2bit, NULL, NULL },	/* ADFI_684_BIT_CHAR */
{ &Adi_fis[645], adu_mb_move_bits, NULL, NULL },	/* ADFI_685_BIT_BIT */
{ &Adi_fis[646], adu_mb_move_bits, NULL, NULL },	/* ADFI_686_BIT_VBIT */
{ &Adi_fis[647], adu_str2bit, NULL, NULL },	/* ADFI_687_VBIT_VARCHAR */
{ &Adi_fis[648], adu_str2bit, NULL, NULL },	/* ADFI_688_VBIT_CHAR */
{ &Adi_fis[649], adu_mb_move_bits, NULL, NULL },	/* ADFI_689_VBIT_BIT */
{ &Adi_fis[650], adu_mb_move_bits, NULL, NULL },	/* ADFI_690_VBIT_VBIT */
{ &Adi_fis[1037], adu_str2bit, NULL, NULL },	/* ADFI_691_CHAR_TO_BIT */
{ &Adi_fis[1038], adu_str2bit, NULL, NULL },	/* ADFI_692_VARCHAR_TO_BIT */
{ &Adi_fis[1039], adu_mb_move_bits, NULL, NULL },	/* ADFI_693_BIT_TO_BIT */
{ &Adi_fis[1052], adu_mb_move_bits, NULL, NULL },	/* ADFI_694_VBIT_TO_BIT */
{ &Adi_fis[1040], adu_str2bit, NULL, NULL },	/* ADFI_695_CHAR_TO_VBIT */
{ &Adi_fis[1041], adu_str2bit, NULL, NULL },	/* ADFI_696_VARCHAR_TO_VBIT */
{ &Adi_fis[1042], adu_mb_move_bits, NULL, NULL },	/* ADFI_697_BIT_TO_VBIT */
{ &Adi_fis[1043], adu_mb_move_bits, NULL, NULL },	/* ADFI_698_VBIT_TO_VBIT */
{ &Adi_fis[1044], adu_str2bit, NULL, NULL },	/* ADFI_699_LONGTEXT_TO_VBIT */
{ &Adi_fis[1045], adu_str2bit, NULL, NULL },	/* ADFI_700_LONGTEXT_TO_BIT */
{ &Adi_fis[1046], adu_bit2str, NULL, NULL },	/* ADFI_701_BIT_TO_VARCHAR */
{ &Adi_fis[1047], adu_bit2str, NULL, NULL },	/* ADFI_702_BIT_TO_CHAR */
{ &Adi_fis[1048], adu_bit2str, NULL, NULL },	/* ADFI_703_BIT_TO_LONGTEXT */
{ &Adi_fis[1049], adu_bit2str, NULL, NULL },	/* ADFI_704_VBIT_TO_VARCHAR */
{ &Adi_fis[1050], adu_bit2str, NULL, NULL },	/* ADFI_705_VBIT_TO_CHAR */
{ &Adi_fis[1051], adu_bit2str, NULL, NULL },	/* ADFI_706_VBIT_TO_LONGTEXT */
{ &Adi_fis[486], adu_7strlength, NULL, NULL },	/* ADFI_707_LENGTH_LVCH */
{ &Adi_fis[548], adu_12strsize, NULL, NULL },	/* ADFI_708_SIZE_LVCH */
{ &Adi_fis[474], adu_7lvch_left, NULL, NULL },	/* ADFI_709_LEFT_LVCH_INT */
{ &Adi_fis[526], adu_9lvch_right, NULL, NULL },	/* ADFI_710_RIGHT_LVCH_INT */
{ &Adi_fis[576], adu_3lvch_upper, NULL, NULL },	/* ADFI_711_UPPERCASE_LVCH */
{ &Adi_fis[500], adu_5lvch_lower, NULL, NULL },	/* ADFI_712_LOWERCASE_LVCH */
{ &Adi_fis[388], adu_11lvch_concat, NULL, NULL },	/* ADFI_713_CONCAT_LVCH_LVCH */
{ &Adi_fis[218], adu_11lvch_concat, NULL, NULL },	/* ADFI_714_LVCH_ADD_LVCH */
{ &Adi_fis[18], adc_byte_compare, NULL, NULL },	/* ADFI_715_BYTE_NE_BYTE */
{ &Adi_fis[19], adc_byte_compare, NULL, NULL },	/* ADFI_716_VBYTE_NE_VBYTE */
{ &Adi_fis[40], adc_byte_compare, NULL, NULL },	/* ADFI_717_BYTE_LT_BYTE */
{ &Adi_fis[41], adc_byte_compare, NULL, NULL },	/* ADFI_718_VBYTE_LT_VBYTE */
{ &Adi_fis[61], adc_byte_compare, NULL, NULL },	/* ADFI_719_BYTE_LE_BYTE */
{ &Adi_fis[62], adc_byte_compare, NULL, NULL },	/* ADFI_720_VBYTE_LE_VBYTE */
{ &Adi_fis[84], adc_byte_compare, NULL, NULL },	/* ADFI_721_BYTE_EQ_BYTE */
{ &Adi_fis[85], adc_byte_compare, NULL, NULL },	/* ADFI_722_VBYTE_EQ_VBYTE */
{ &Adi_fis[107], adc_byte_compare, NULL, NULL },	/* ADFI_723_BYTE_GT_BYTE */
{ &Adi_fis[108], adc_byte_compare, NULL, NULL },	/* ADFI_724_VBYTE_GT_VBYTE */
{ &Adi_fis[129], adc_byte_compare, NULL, NULL },	/* ADFI_725_BYTE_GE_BYTE */
{ &Adi_fis[130], adc_byte_compare, NULL, NULL },	/* ADFI_726_VBYTE_GE_VBYTE */
{ &Adi_fis[219], adu_4strconcat, NULL, NULL },	/* ADFI_727_BYTE_ADD_BYTE */
{ &Adi_fis[220], adu_4strconcat, NULL, NULL },	/* ADFI_728_VBYTE_ADD_VBYTE */
{ &Adi_fis[221], adu_11lvch_concat, NULL, NULL },	/* ADFI_729_LBYTE_ADD_LBYTE */
{ &Adi_fis[389], adu_4strconcat, NULL, NULL },	/* ADFI_730_CONCAT_BYTE_BYTE */
{ &Adi_fis[390], adu_4strconcat, NULL, NULL },	/* ADFI_731_CONCAT_VBYTE_VBYTE */
{ &Adi_fis[391], adu_11lvch_concat, NULL, NULL },	/* ADFI_732_CONCAT_LBYTE_LBYTE */
{ &Adi_fis[475], adu_6strleft, NULL, NULL },	/* ADFI_733_LEFT_BYTE_I */
{ &Adi_fis[476], adu_6strleft, NULL, NULL },	/* ADFI_734_LEFT_VBYTE_I */
{ &Adi_fis[477], adu_7lvch_left, NULL, NULL },	/* ADFI_735_LEFT_LBYTE_I */
{ &Adi_fis[487], adu_7strlength, NULL, NULL },	/* ADFI_736_LENGTH_BYTE */
{ &Adi_fis[488], adu_7strlength, NULL, NULL },	/* ADFI_737_LENGTH_VBYTE */
{ &Adi_fis[489], adu_7strlength, NULL, NULL },	/* ADFI_738_LENGTH_LBYTE */
{ &Adi_fis[527], adu_10strright, NULL, NULL },	/* ADFI_739_RIGHT_BYTE_I */
{ &Adi_fis[528], adu_10strright, NULL, NULL },	/* ADFI_740_RIGHT_VBYTE_I */
{ &Adi_fis[529], adu_9lvch_right, NULL, NULL },	/* ADFI_741_RIGHT_LBYTE_I */
{ &Adi_fis[536], adu_11strshift, NULL, NULL },	/* ADFI_742_SHIFT_BYTE_I */
{ &Adi_fis[537], adu_11strshift, NULL, NULL },	/* ADFI_743_SHIFT_VBYTE_I */
{ &Adi_fis[538], NULL, NULL, NULL },	/* ADFI_744_SHIFT_LBYTE_I */
{ &Adi_fis[549], adu_12strsize, NULL, NULL },	/* ADFI_745_SIZE_BYTE */
{ &Adi_fis[550], adu_12strsize, NULL, NULL },	/* ADFI_746_SIZE_VBYTE */
{ &Adi_fis[551], adu_12strsize, NULL, NULL },	/* ADFI_747_SIZE_LBYTE */
{ &Adi_fis[657], adu_notrm, NULL, NULL },	/* ADFI_748_BYTE_CHAR */
{ &Adi_fis[666], adu_1strtostr, NULL, NULL },	/* ADFI_749_VARBYTE_VARCHAR */
{ &Adi_fis[676], adu_lolk, NULL, NULL },	/* ADFI_750_II_LOLK_LVCH */
{ &Adi_fis[677], adu_lolk, NULL, NULL },	/* ADFI_751_II_LOLK_LBYTE */
{ &Adi_fis[1053], adu_1strtostr, NULL, NULL },	/* ADFI_752_CHAR_TO_BYTE */
{ &Adi_fis[1054], adu_1strtostr, NULL, NULL },	/* ADFI_753_VARCHAR_TO_BYTE */
{ &Adi_fis[1055], adu_1strtostr, NULL, NULL },	/* ADFI_754_C_TO_BYTE */
{ &Adi_fis[1056], adu_1strtostr, NULL, NULL },	/* ADFI_755_TEXT_TO_BYTE */
{ &Adi_fis[1057], adu_1strtostr, NULL, NULL },	/* ADFI_756_LONGTEXT_TO_BYTE */
{ &Adi_fis[1058], adu_1strtostr, NULL, NULL },	/* ADFI_757_BYTE_TO_BYTE */
{ &Adi_fis[1059], adu_1strtostr, NULL, NULL },	/* ADFI_758_VARBYTE_TO_BYTE */
{ &Adi_fis[1060], adu_1strtostr, NULL, NULL },	/* ADFI_759_CHAR_TO_VARBYTE */
{ &Adi_fis[1061], adu_1strtostr, NULL, NULL },	/* ADFI_760_VARCHAR_TO_VARBYTE */
{ &Adi_fis[1062], adu_1strtostr, NULL, NULL },	/* ADFI_761_C_TO_VARBYTE */
{ &Adi_fis[1063], adu_1strtostr, NULL, NULL },	/* ADFI_762_TEXT_TO_VARBYTE */
{ &Adi_fis[1064], adu_1strtostr, NULL, NULL },	/* ADFI_763_LONGTEXT_TO_VARBYTE */
{ &Adi_fis[222], adu_4strconcat, NULL, NULL },	/* ADFI_764_BYTE_ADD_VBYTE */
{ &Adi_fis[223], adu_4strconcat, NULL, NULL },	/* ADFI_765_VBYTE_ADD_BYTE */
{ &Adi_fis[392], adu_4strconcat, NULL, NULL },	/* ADFI_766_CONCAT_BYTE_VBYTE */
{ &Adi_fis[393], adu_4strconcat, NULL, NULL },	/* ADFI_767_CONCAT_VBYTE_BYTE */
{ &Adi_fis[658], adu_1strtostr, NULL, NULL },	/* ADFI_768_BYTE_VARCHAR */
{ &Adi_fis[659], adu_1strtostr, NULL, NULL },	/* ADFI_769_BYTE_BYTE */
{ &Adi_fis[660], adu_1strtostr, NULL, NULL },	/* ADFI_770_BYTE_VBYTE */
{ &Adi_fis[662], adu_ascii_2arg, NULL, NULL },	/* ADFI_771_BYTE_CHAR_I */
{ &Adi_fis[663], adu_ascii_2arg, NULL, NULL },	/* ADFI_772_BYTE_VARCHAR_I */
{ &Adi_fis[664], adu_ascii_2arg, NULL, NULL },	/* ADFI_773_BYTE_BYTE_I */
{ &Adi_fis[665], adu_ascii_2arg, NULL, NULL },	/* ADFI_774_BYTE_VBYTE_I */
{ &Adi_fis[667], adu_1strtostr, NULL, NULL },	/* ADFI_775_VARBYTE_CHAR */
{ &Adi_fis[668], adu_1strtostr, NULL, NULL },	/* ADFI_776_VARBYTE_BYTE */
{ &Adi_fis[669], adu_1strtostr, NULL, NULL },	/* ADFI_777_VARBYTE_VBYTE */
{ &Adi_fis[671], adu_varbyte_2arg, NULL, NULL },	/* ADFI_778_VARBYTE_CHAR_I */
{ &Adi_fis[672], adu_varbyte_2arg, NULL, NULL },	/* ADFI_779_VARBYTE_VARCHAR_I */
{ &Adi_fis[673], adu_varbyte_2arg, NULL, NULL },	/* ADFI_780_VARBYTE_BYTE_I */
{ &Adi_fis[674], adu_varbyte_2arg, NULL, NULL },	/* ADFI_781_VARBYTE_VBYTE_I */
{ &Adi_fis[1075], adu_lvch_move, NULL, NULL },	/* ADFI_782_LBYTE_TO_LVCH */
{ &Adi_fis[1076], adu_lvch_move, NULL, NULL },	/* ADFI_783_LVCH_TO_LBYTE */
{ &Adi_fis[187], adu_10mny_multu, NULL, NULL },	/* ADFI_784_DEC_MUL_MONEY */
{ &Adi_fis[190], adu_10mny_multu, NULL, NULL },	/* ADFI_785_MONEY_MUL_DEC */
{ &Adi_fis[250], adu_7mny_divf, NULL, NULL },	/* ADFI_786_DEC_DIV_MONEY */
{ &Adi_fis[253], adu_8mny_divu, NULL, NULL },	/* ADFI_787_MONEY_DIV_DEC */
{ &Adi_fis[1086], adu_1strtostr, NULL, NULL },	/* ADFI_788_VARBYTE_TO_LONGTEXT */
{ &Adi_fis[678], adu_perm_type, NULL, NULL },	/* ADFI_789_II_PERM_TYPE */
{ &Adi_fis[679], adu_19strsoundex, NULL, NULL },	/* ADFI_790_SOUNDEX_C */
{ &Adi_fis[680], adu_19strsoundex, NULL, NULL },	/* ADFI_791_SOUNDEX_CHAR */
{ &Adi_fis[681], adu_19strsoundex, NULL, NULL },	/* ADFI_792_SOUNDEX_TEXT */
{ &Adi_fis[682], adu_19strsoundex, NULL, NULL },	/* ADFI_793_SOUNDEX_VARCHAR */
{ &Adi_fis[683], adu_19strsoundex, NULL, NULL },	/* ADFI_794_SOUNDEX_BYTE */
{ &Adi_fis[684], adu_19strsoundex, NULL, NULL },	/* ADFI_795_SOUNDEX_VBYTE */
{ &Adi_fis[714], adu_bit_add, NULL, NULL },	/* ADFI_796_BIT_ADD_BYTE */
{ &Adi_fis[715], adu_bit_and, NULL, NULL },	/* ADFI_797_BIT_AND_BYTE */
{ &Adi_fis[716], adu_bit_not, NULL, NULL },	/* ADFI_798_BIT_NOT_BYTE */
{ &Adi_fis[717], adu_bit_or, NULL, NULL },	/* ADFI_799_BIT_OR_BYTE */
{ &Adi_fis[718], adu_bit_xor, NULL, NULL },	/* ADFI_800_BIT_XOR_BYTE */
{ &Adi_fis[719], adu_ipaddr, NULL, NULL },	/* ADFI_801_IPADDR_C */
{ &Adi_fis[720], adu_ipaddr, NULL, NULL },	/* ADFI_802_IPADDR_CHAR */
{ &Adi_fis[721], adu_ipaddr, NULL, NULL },	/* ADFI_803_IPADDR_TEXT */
{ &Adi_fis[722], adu_ipaddr, NULL, NULL },	/* ADFI_804_IPADDR_VARCHAR */
{ &Adi_fis[686], adu_intextract, NULL, NULL },	/* ADFI_805_INTEXTRACT_BYTE */
{ &Adi_fis[687], adu_intextract, NULL, NULL },	/* ADFI_806_INTEXTRACT_VARBYTE */
{ &Adi_fis[727], adu_hash, NULL, NULL },	/* ADFI_807_HASH_ALL */
{ &Adi_fis[728], adu_randomf, NULL, NULL },	/* ADFI_808_RANDOMF_VOID */
{ &Adi_fis[729], adu_randomf_range, NULL, NULL },	/* ADFI_809_RANDOMF_INT_INT */
{ &Adi_fis[730], adu_randomf_rangef, NULL, NULL },	/* ADFI_810_RANDOMF_FLT_FLT */
{ &Adi_fis[731], adu_random, NULL, NULL },	/* ADFI_811_RANDOM_VOID */
{ &Adi_fis[732], adu_random_range, NULL, NULL },	/* ADFI_812_RANDOM_INT_INT */
{ &Adi_fis[733], adu_uuid_create, NULL, NULL },	/* ADFI_813_UUID_CREATE_VOID */
{ &Adi_fis[734], adu_uuid_to_char, NULL, NULL },	/* ADFI_814_UUID_TO_CHAR_BYTE */
{ &Adi_fis[735], adu_uuid_from_char, NULL, NULL },	/* ADFI_815_UUID_FROM_CHAR_C */
{ &Adi_fis[736], adu_uuid_from_char, NULL, NULL },	/* ADFI_816_UUID_FROM_CHAR_CHAR */
{ &Adi_fis[737], adu_uuid_from_char, NULL, NULL },	/* ADFI_817_UUID_FROM_CHAR_TEXT */
{ &Adi_fis[738], adu_uuid_from_char, NULL, NULL },	/* ADFI_818_UUID_FROM_CHAR_VARCHAR */
{ &Adi_fis[739], adu_uuid_compare, NULL, NULL },	/* ADFI_819_UUID_COMPARE_BYTE_BYTE */
{ &Adi_fis[740], adu_20substr, NULL, NULL },	/* ADFI_820_SUBSTRING_C_INT */
{ &Adi_fis[741], adu_20substr, NULL, NULL },	/* ADFI_821_SUBSTRING_CHAR_INT */
{ &Adi_fis[742], adu_20substr, NULL, NULL },	/* ADFI_822_SUBSTRING_TEXT_INT */
{ &Adi_fis[743], adu_20substr, NULL, NULL },	/* ADFI_823_SUBSTRING_VARCHAR_INT */
{ &Adi_fis[746], adu_21substrlen, NULL, NULL },	/* ADFI_824_SUBSTRING_C_INT_INT */
{ &Adi_fis[747], adu_21substrlen, NULL, NULL },	/* ADFI_825_SUBSTRING_CHAR_INT_INT */
{ &Adi_fis[748], adu_21substrlen, NULL, NULL },	/* ADFI_826_SUBSTRING_TEXT_INT_INT */
{ &Adi_fis[749], adu_21substrlen, NULL, NULL },	/* ADFI_827_SUBSTRING_VARCHAR_INT_INT */
{ &Adi_fis[758], adu_unhex, NULL, NULL },	/* ADFI_828_UNHEX_CHAR */
{ &Adi_fis[759], adu_unhex, NULL, NULL },	/* ADFI_829_UNHEX_TEXT */
{ &Adi_fis[760], adu_unhex, NULL, NULL },	/* ADFI_830_UNHEX_VARCHAR */
{ &Adi_fis[761], adu_unhex, NULL, NULL },	/* ADFI_831_UNHEX_C */
{ &Adi_fis[329], adu_N7a_corrr2, adu_B0a_noagwork, adu_E7a_corr },	/* ADFI_832_CORR_FLT */
{ &Adi_fis[330], adu_N8a_covsxy, adu_B0a_noagwork, adu_E8a_cpop },	/* ADFI_833_COVAR_POP_FLT */
{ &Adi_fis[331], adu_N8a_covsxy, adu_B0a_noagwork, adu_E9a_csamp },	/* ADFI_834_COVAR_SAMP_FLT */
{ &Adi_fis[332], adu_N9a_ravgx, adu_B0a_noagwork, adu_E10a_ravgx },	/* ADFI_835_REGR_AVGX_FLT */
{ &Adi_fis[333], adu_N10a_ravgy, adu_B0a_noagwork, adu_E11a_ravgy },	/* ADFI_836_REGR_AVGY_FLT */
{ &Adi_fis[334], adu_N11a_rcnt, adu_B0a_noagwork, adu_E12a_rcnt },	/* ADFI_837_REGR_COUNT_ANY */
{ &Adi_fis[335], adu_N12a_rintsl, adu_B0a_noagwork, adu_E13a_rint },	/* ADFI_838_REGR_INTERCEPT_FLT */
{ &Adi_fis[336], adu_N7a_corrr2, adu_B0a_noagwork, adu_E14a_rr2 },	/* ADFI_839_REGR_R2_FLT */
{ &Adi_fis[337], adu_N12a_rintsl, adu_B0a_noagwork, adu_E15a_rslope },	/* ADFI_840_REGR_SLOPE_FLT */
{ &Adi_fis[338], adu_N13a_rsxx, adu_B0a_noagwork, adu_E16a_rsxx },	/* ADFI_841_REGR_SXX_FLT */
{ &Adi_fis[339], adu_N8a_covsxy, adu_B0a_noagwork, adu_E17a_rsxy },	/* ADFI_842_REGR_SXY_FLT */
{ &Adi_fis[340], adu_N14a_rsyy, adu_B0a_noagwork, adu_E18a_rsyy },	/* ADFI_843_REGR_SYY_FLT */
{ &Adi_fis[341], NULL, NULL, NULL },	/* ADFI_844_STDDEV_POP_FLT */
{ &Adi_fis[342], NULL, NULL, NULL },	/* ADFI_845_STDDEV_SAMP_FLT */
{ &Adi_fis[343], NULL, NULL, NULL },	/* ADFI_846_VAR_POP_FLT */
{ &Adi_fis[344], NULL, NULL, NULL },	/* ADFI_847_VAR_SAMP_FLT */
{ &Adi_fis[787], adu_22charlength, NULL, NULL },	/* ADFI_848_CHLEN_C */
{ &Adi_fis[788], adu_22charlength, NULL, NULL },	/* ADFI_849_CHLEN_CHAR */
{ &Adi_fis[789], adu_22charlength, NULL, NULL },	/* ADFI_850_CHLEN_TEXT */
{ &Adi_fis[790], adu_22charlength, NULL, NULL },	/* ADFI_851_CHLEN_VARCHAR */
{ &Adi_fis[791], adu_22charlength, NULL, NULL },	/* ADFI_852_CHLEN_BYTE */
{ &Adi_fis[792], adu_22charlength, NULL, NULL },	/* ADFI_853_CHLEN_VARBYTE */
{ &Adi_fis[793], adu_22charlength, NULL, NULL },	/* ADFI_854_CHLEN_LVCH */
{ &Adi_fis[794], adu_22charlength, NULL, NULL },	/* ADFI_855_CHLEN_LBYTE */
{ &Adi_fis[795], NULL, NULL, NULL },	/* ADFI_856_CHLEN_NCHAR */
{ &Adi_fis[796], NULL, NULL, NULL },	/* ADFI_857_CHLEN_NVCHAR */
{ &Adi_fis[797], adu_23octetlength, NULL, NULL },	/* ADFI_858_OCLEN_C */
{ &Adi_fis[798], adu_23octetlength, NULL, NULL },	/* ADFI_859_OCLEN_CHAR */
{ &Adi_fis[799], adu_23octetlength, NULL, NULL },	/* ADFI_860_OCLEN_TEXT */
{ &Adi_fis[800], adu_23octetlength, NULL, NULL },	/* ADFI_861_OCLEN_VARCHAR */
{ &Adi_fis[801], adu_23octetlength, NULL, NULL },	/* ADFI_862_OCLEN_BYTE */
{ &Adi_fis[802], adu_23octetlength, NULL, NULL },	/* ADFI_863_OCLEN_VARBYTE */
{ &Adi_fis[803], adu_23octetlength, NULL, NULL },	/* ADFI_864_OCLEN_LVCH */
{ &Adi_fis[804], adu_23octetlength, NULL, NULL },	/* ADFI_865_OCLEN_LBYTE */
{ &Adi_fis[805], NULL, NULL, NULL },	/* ADFI_866_OCLEN_NCHAR */
{ &Adi_fis[806], NULL, NULL, NULL },	/* ADFI_867_OCLEN_NVCHAR */
{ &Adi_fis[807], adu_24bitlength, NULL, NULL },	/* ADFI_868_BLEN_C */
{ &Adi_fis[808], adu_24bitlength, NULL, NULL },	/* ADFI_869_BLEN_CHAR */
{ &Adi_fis[809], adu_24bitlength, NULL, NULL },	/* ADFI_870_BLEN_TEXT */
{ &Adi_fis[810], adu_24bitlength, NULL, NULL },	/* ADFI_871_BLEN_VARCHAR */
{ &Adi_fis[811], adu_24bitlength, NULL, NULL },	/* ADFI_872_BLEN_BYTE */
{ &Adi_fis[812], adu_24bitlength, NULL, NULL },	/* ADFI_873_BLEN_VARBYTE */
{ &Adi_fis[813], adu_24bitlength, NULL, NULL },	/* ADFI_874_BLEN_LVCH */
{ &Adi_fis[814], adu_24bitlength, NULL, NULL },	/* ADFI_875_BLEN_LBYTE */
{ &Adi_fis[815], NULL, NULL, NULL },	/* ADFI_876_BLEN_NCHAR */
{ &Adi_fis[816], NULL, NULL, NULL },	/* ADFI_877_BLEN_NVCHAR */
{ &Adi_fis[763], adu_25strposition, NULL, NULL },	/* ADFI_878_POS_STR_STR */
{ &Adi_fis[764], adu_25strposition, NULL, NULL },	/* ADFI_879_POS_UNISTR_UNISTR */
{ &Adi_fis[765], adu_25strposition, NULL, NULL },	/* ADFI_880_POS_VBYTE_VBYTE */
{ &Adi_fis[766], adu_15lvch_position, NULL, NULL },	/* ADFI_881_POS_STR_LVCH */
{ &Adi_fis[767], adu_15lvch_position, NULL, NULL },	/* ADFI_882_POS_UNISTR_LNVCHR */
{ &Adi_fis[768], adu_15lvch_position, NULL, NULL },	/* ADFI_883_POS_VBYTE_LBYTE */
{ &Adi_fis[781], adu_atrim, NULL, NULL },	/* ADFI_884_ATRIM_C */
{ &Adi_fis[782], adu_atrim, NULL, NULL },	/* ADFI_885_ATRIM_TEXT */
{ &Adi_fis[783], adu_atrim, NULL, NULL },	/* ADFI_886_ATRIM_CHAR */
{ &Adi_fis[784], adu_atrim, NULL, NULL },	/* ADFI_887_ATRIM_VARCHAR */
{ &Adi_fis[785], NULL, NULL, NULL },	/* ADFI_888_ATRIM_NCHAR */
{ &Adi_fis[786], NULL, NULL, NULL },	/* ADFI_889_ATRIM_NVCHAR */
{ &Adi_fis[608], adu_16strindex, NULL, NULL },	/* ADFI_890_CHAREXTRACT_C_I */
{ &Adi_fis[609], adu_16strindex, NULL, NULL },	/* ADFI_891_CHAREXTRACT_TEXT_I */
{ &Adi_fis[610], adu_16strindex, NULL, NULL },	/* ADFI_892_CHAREXTRACT_CHAR_I */
{ &Adi_fis[611], adu_16strindex, NULL, NULL },	/* ADFI_893_CHAREXTRACT_VARCHAR_I */
{ NULL, NULL, NULL, NULL },	/* No FI 894 */
{ NULL, NULL, NULL, NULL },	/* No FI 895 */
{ NULL, NULL, NULL, NULL },	/* No FI 896 */
{ NULL, NULL, NULL, NULL },	/* No FI 897 */
{ NULL, NULL, NULL, NULL },	/* No FI 898 */
{ NULL, NULL, NULL, NULL },	/* No FI 899 */
{ NULL, NULL, NULL, NULL },	/* No FI 900 */
{ NULL, NULL, NULL, NULL },	/* No FI 901 */
{ NULL, NULL, NULL, NULL },	/* No FI 902 */
{ NULL, NULL, NULL, NULL },	/* No FI 903 */
{ NULL, NULL, NULL, NULL },	/* No FI 904 */
{ NULL, NULL, NULL, NULL },	/* No FI 905 */
{ NULL, NULL, NULL, NULL },	/* No FI 906 */
{ NULL, NULL, NULL, NULL },	/* No FI 907 */
{ NULL, NULL, NULL, NULL },	/* No FI 908 */
{ NULL, NULL, NULL, NULL },	/* No FI 909 */
{ NULL, NULL, NULL, NULL },	/* No FI 910 */
{ NULL, NULL, NULL, NULL },	/* No FI 911 */
{ NULL, NULL, NULL, NULL },	/* No FI 912 */
{ NULL, NULL, NULL, NULL },	/* No FI 913 */
{ NULL, NULL, NULL, NULL },	/* No FI 914 */
{ NULL, NULL, NULL, NULL },	/* No FI 915 */
{ NULL, NULL, NULL, NULL },	/* No FI 916 */
{ NULL, NULL, NULL, NULL },	/* No FI 917 */
{ NULL, NULL, NULL, NULL },	/* No FI 918 */
{ NULL, NULL, NULL, NULL },	/* No FI 919 */
{ NULL, NULL, NULL, NULL },	/* No FI 920 */
{ NULL, NULL, NULL, NULL },	/* No FI 921 */
{ NULL, NULL, NULL, NULL },	/* No FI 922 */
{ NULL, NULL, NULL, NULL },	/* No FI 923 */
{ NULL, NULL, NULL, NULL },	/* No FI 924 */
{ NULL, NULL, NULL, NULL },	/* No FI 925 */
{ NULL, NULL, NULL, NULL },	/* No FI 926 */
{ NULL, NULL, NULL, NULL },	/* No FI 927 */
{ NULL, NULL, NULL, NULL },	/* No FI 928 */
{ NULL, NULL, NULL, NULL },	/* No FI 929 */
{ NULL, NULL, NULL, NULL },	/* No FI 930 */
{ NULL, NULL, NULL, NULL },	/* No FI 931 */
{ NULL, NULL, NULL, NULL },	/* No FI 932 */
{ NULL, NULL, NULL, NULL },	/* No FI 933 */
{ NULL, NULL, NULL, NULL },	/* No FI 934 */
{ NULL, NULL, NULL, NULL },	/* No FI 935 */
{ NULL, NULL, NULL, NULL },	/* No FI 936 */
{ NULL, NULL, NULL, NULL },	/* No FI 937 */
{ NULL, NULL, NULL, NULL },	/* No FI 938 */
{ NULL, NULL, NULL, NULL },	/* No FI 939 */
{ NULL, NULL, NULL, NULL },	/* No FI 940 */
{ NULL, NULL, NULL, NULL },	/* No FI 941 */
{ NULL, NULL, NULL, NULL },	/* No FI 942 */
{ NULL, NULL, NULL, NULL },	/* No FI 943 */
{ NULL, NULL, NULL, NULL },	/* No FI 944 */
{ NULL, NULL, NULL, NULL },	/* No FI 945 */
{ NULL, NULL, NULL, NULL },	/* No FI 946 */
{ NULL, NULL, NULL, NULL },	/* No FI 947 */
{ NULL, NULL, NULL, NULL },	/* No FI 948 */
{ NULL, NULL, NULL, NULL },	/* No FI 949 */
{ NULL, NULL, NULL, NULL },	/* No FI 950 */
{ NULL, NULL, NULL, NULL },	/* No FI 951 */
{ NULL, NULL, NULL, NULL },	/* No FI 952 */
{ NULL, NULL, NULL, NULL },	/* No FI 953 */
{ &Adi_fis[694], adu_sesspriv, NULL, NULL },	/* ADFI_954_SESSION_PRIV */
{ &Adi_fis[695], adu_iitblstat, NULL, NULL },	/* ADFI_955_IITBLSTAT */
{ NULL, NULL, NULL, NULL },	/* No FI 956 */
{ NULL, NULL, NULL, NULL },	/* No FI 957 */
{ NULL, NULL, NULL, NULL },	/* No FI 958 */
{ NULL, NULL, NULL, NULL },	/* No FI 959 */
{ NULL, NULL, NULL, NULL },	/* No FI 960 */
{ NULL, NULL, NULL, NULL },	/* No FI 961 */
{ NULL, NULL, NULL, NULL },	/* No FI 962 */
{ NULL, NULL, NULL, NULL },	/* No FI 963 */
{ NULL, NULL, NULL, NULL },	/* No FI 964 */
{ NULL, NULL, NULL, NULL },	/* No FI 965 */
{ NULL, NULL, NULL, NULL },	/* No FI 966 */
{ NULL, NULL, NULL, NULL },	/* No FI 967 */
{ NULL, NULL, NULL, NULL },	/* No FI 968 */
{ NULL, NULL, NULL, NULL },	/* No FI 969 */
{ NULL, NULL, NULL, NULL },	/* No FI 970 */
{ &Adi_fis[1087], adu_1dec_coerce, NULL, NULL },	/* ADFI_971_CHA_TO_DEC */
{ &Adi_fis[1088], adu_1dec_coerce, NULL, NULL },	/* ADFI_972_VCH_TO_DEC */
{ &Adi_fis[1091], adu_1dec_coerce, NULL, NULL },	/* ADFI_973_TXT_TO_DEC */
{ &Adi_fis[1092], adu_1dec_coerce, NULL, NULL },	/* ADFI_974_C_TO_DEC */
{ &Adi_fis[1093], adu_ascii, NULL, NULL },	/* ADFI_975_DEC_TO_CHA */
{ &Adi_fis[1094], adu_ascii, NULL, NULL },	/* ADFI_976_DEC_TO_VCH */
{ &Adi_fis[1095], adu_ascii, NULL, NULL },	/* ADFI_977_DEC_TO_TXT */
{ &Adi_fis[1098], adu_ascii, NULL, NULL },	/* ADFI_978_DEC_TO_C */
{ &Adi_fis[661], adu_2alltobyte, NULL, NULL },	/* ADFI_979_BYTE_ALL */
{ &Adi_fis[670], adu_2alltobyte, NULL, NULL },	/* ADFI_980_VBYTE_ALL */
{ &Adi_fis[685], adu_1cvrt_date4, NULL, NULL },	/* ADFI_981__DATE4_I */
{ &Adi_fis[20], NULL, NULL, NULL },	/* ADFI_982_NCHAR_NE_NCHAR */
{ &Adi_fis[86], NULL, NULL, NULL },	/* ADFI_983_NCHAR_EQ_NCHAR */
{ &Adi_fis[21], NULL, NULL, NULL },	/* ADFI_984_NVCHAR_NE_NVCHAR */
{ &Adi_fis[87], NULL, NULL, NULL },	/* ADFI_985_NVCHAR_EQ_NVCHAR */
{ &Adi_fis[42], NULL, NULL, NULL },	/* ADFI_986_NCHAR_LT_NCHAR */
{ &Adi_fis[131], NULL, NULL, NULL },	/* ADFI_987_NCHAR_GE_NCHAR */
{ &Adi_fis[43], NULL, NULL, NULL },	/* ADFI_988_NVCHAR_LT_NVCHAR */
{ &Adi_fis[132], NULL, NULL, NULL },	/* ADFI_989_NVCHAR_GE_NVCHAR */
{ &Adi_fis[64], NULL, NULL, NULL },	/* ADFI_990_NCHAR_LE_NCHAR */
{ &Adi_fis[109], NULL, NULL, NULL },	/* ADFI_991_NCHAR_GT_NCHAR */
{ &Adi_fis[65], NULL, NULL, NULL },	/* ADFI_992_NVCHAR_LE_NVCHAR */
{ &Adi_fis[110], NULL, NULL, NULL },	/* ADFI_993_NVCHAR_GT_NVCHAR */
{ &Adi_fis[144], NULL, NULL, NULL },	/* ADFI_994_NCHAR_ISNULL */
{ &Adi_fis[157], NULL, NULL, NULL },	/* ADFI_995_NCHAR_ISNOTNULL */
{ &Adi_fis[145], NULL, NULL, NULL },	/* ADFI_996_NVCHAR_ISNULL */
{ &Adi_fis[158], NULL, NULL, NULL },	/* ADFI_997_NVCHAR_ISNOTNULL */
{ NULL, NULL, NULL, NULL },	/* No FI 998 */
{ NULL, NULL, NULL, NULL },	/* No FI 999 */
{ &Adi_fis[163], NULL, NULL, NULL },	/* ADFI_1000_NVCHAR_LIKE_NVCHAR */
{ &Adi_fis[168], NULL, NULL, NULL },	/* ADFI_1001_NVCHAR_NOTLIKE_NVCHAR */
{ NULL, NULL, NULL, NULL },	/* No FI 1002 */
{ NULL, NULL, NULL, NULL },	/* No FI 1003 */
{ NULL, NULL, NULL, NULL },	/* No FI 1004 */
{ &Adi_fis[394], adu_4strconcat, NULL, NULL },	/* ADFI_1005_CONCAT_NCHAR_NCHAR */
{ NULL, NULL, NULL, NULL },	/* No FI 1006 */
{ NULL, NULL, NULL, NULL },	/* No FI 1007 */
{ NULL, NULL, NULL, NULL },	/* No FI 1008 */
{ NULL, NULL, NULL, NULL },	/* No FI 1009 */
{ &Adi_fis[395], adu_4strconcat, NULL, NULL },	/* ADFI_1010_CONCAT_NVCHAR_NVCHAR */
{ NULL, NULL, NULL, NULL },	/* No FI 1011 */
{ &Adi_fis[478], adu_nvchr_left, NULL, NULL },	/* ADFI_1012_LEFT_NCHAR_I */
{ &Adi_fis[479], adu_nvchr_left, NULL, NULL },	/* ADFI_1013_LEFT_NVCHAR_I */
{ &Adi_fis[530], adu_nvchr_right, NULL, NULL },	/* ADFI_1014_RIGHT_NCHAR_I */
{ &Adi_fis[531], adu_nvchr_right, NULL, NULL },	/* ADFI_1015_RIGHT_NVCHAR_I */
{ &Adi_fis[490], adu_nvchr_length, NULL, NULL },	/* ADFI_1016_LENGTH_NCHAR */
{ &Adi_fis[491], adu_nvchr_length, NULL, NULL },	/* ADFI_1017_LENGTH_NVCHAR */
{ &Adi_fis[570], adu_nvchr_trim, NULL, NULL },	/* ADFI_1018_TRIM_NCHAR */
{ &Adi_fis[571], adu_nvchr_trim, NULL, NULL },	/* ADFI_1019_TRIM_NVCHAR */
{ NULL, NULL, NULL, NULL },	/* No FI 1020 */
{ NULL, NULL, NULL, NULL },	/* No FI 1021 */
{ NULL, NULL, NULL, NULL },	/* No FI 1022 */
{ NULL, NULL, NULL, NULL },	/* No FI 1023 */
{ &Adi_fis[1099], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1024_NCHAR_TO_NVCHAR */
{ &Adi_fis[1100], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1025_NVCHAR_TO_NCHAR */
{ &Adi_fis[1101], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1026_NCHAR_TO_NCHAR */
{ &Adi_fis[1102], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1027_NVCHAR_TO_NVCHAR */
{ &Adi_fis[277], adu_N1a_any, adu_B0a_noagwork, adu_E0a_anycount },	/* ADFI_1028_ANY_NCHAR */
{ &Adi_fis[278], adu_N1a_any, adu_B0a_noagwork, adu_E0a_anycount },	/* ADFI_1029_ANY_NVCHAR */
{ &Adi_fis[295], adu_N2a_count, adu_B0a_noagwork, adu_E0a_anycount },	/* ADFI_1030_COUNT_NCHAR */
{ &Adi_fis[296], adu_N2a_count, adu_B0a_noagwork, adu_E0a_anycount },	/* ADFI_1031_COUNT_NVCHAR */
{ &Adi_fis[309], adu_N6s_max_str, adu_B0s_minmax_str, adu_E0s_minmax_str },	/* ADFI_1032_MAX_NCHAR */
{ &Adi_fis[310], adu_N6s_max_str, adu_B0s_minmax_str, adu_E0s_minmax_str },	/* ADFI_1033_MAX_NVCHAR */
{ &Adi_fis[322], adu_N5s_min_str, adu_B0s_minmax_str, adu_E0s_minmax_str },	/* ADFI_1034_MIN_NCHAR */
{ &Adi_fis[323], adu_N5s_min_str, adu_B0s_minmax_str, adu_E0s_minmax_str },	/* ADFI_1035_MIN_NVCHAR */
{ NULL, NULL, NULL, NULL },	/* No FI 1036 */
{ NULL, NULL, NULL, NULL },	/* No FI 1037 */
{ &Adi_fis[501], adu_nvchr_tolower, NULL, NULL },	/* ADFI_1038_LOWERCASE_NCHAR */
{ &Adi_fis[502], adu_nvchr_tolower, NULL, NULL },	/* ADFI_1039_LOWERCASE_NVCHAR */
{ &Adi_fis[520], adu_nvchr_pad, NULL, NULL },	/* ADFI_1040_PAD_NCHAR */
{ &Adi_fis[521], adu_nvchr_pad, NULL, NULL },	/* ADFI_1041_PAD_NVARCHAR */
{ &Adi_fis[539], adu_nvchr_shift, NULL, NULL },	/* ADFI_1042_SHIFT_NCHAR_I */
{ &Adi_fis[540], adu_nvchr_shift, NULL, NULL },	/* ADFI_1043_SHIFT_NVCHAR_I */
{ &Adi_fis[552], adu_12strsize, NULL, NULL },	/* ADFI_1044_SIZE_NCHAR */
{ &Adi_fis[553], adu_12strsize, NULL, NULL },	/* ADFI_1045_SIZE_NVCHAR */
{ &Adi_fis[560], adu_nvchr_squeezewhite, NULL, NULL },	/* ADFI_1046_SQUEEZE_NCHAR */
{ &Adi_fis[561], adu_nvchr_squeezewhite, NULL, NULL },	/* ADFI_1047_SQUEEZE_NVARCHAR */
{ &Adi_fis[577], adu_nvchr_toupper, NULL, NULL },	/* ADFI_1048_UPPERCASE_NCHAR */
{ &Adi_fis[578], adu_nvchr_toupper, NULL, NULL },	/* ADFI_1049_UPPERCASE_NVCHAR */
{ &Adi_fis[612], adu_nvchr_charextract, NULL, NULL },	/* ADFI_1050_CHAREXTRACT_NCHAR_I */
{ &Adi_fis[613], adu_nvchr_charextract, NULL, NULL },	/* ADFI_1051_CHAREXTRACT_NVCHAR_I */
{ &Adi_fis[744], adu_nvchr_substr1, NULL, NULL },	/* ADFI_1052_SUBSTRING_NCHAR_INT */
{ &Adi_fis[745], adu_nvchr_substr1, NULL, NULL },	/* ADFI_1053_SUBSTRING_NVCHAR_INT */
{ &Adi_fis[750], adu_nvchr_substr2, NULL, NULL },	/* ADFI_1054_SUBSTRING_NCHAR_INT_INT */
{ &Adi_fis[751], adu_nvchr_substr2, NULL, NULL },	/* ADFI_1055_SUBSTRING_NVCHAR_INT_INT */
{ &Adi_fis[1117], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1056_NCHAR_TO_LONGTEXT */
{ &Adi_fis[1118], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1057_LONGTEXT_TO_NCHAR */
{ &Adi_fis[1119], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1058_NVCHAR_TO_LONGTEXT */
{ &Adi_fis[1120], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1059_LONGTEXT_TO_NVCHAR */
{ &Adi_fis[1121], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1060_NCHAR_TO_BYTE */
{ &Adi_fis[1122], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1061_NVCHAR_TO_BYTE */
{ &Adi_fis[1123], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1062_NCHAR_TO_VARBYTE */
{ &Adi_fis[1124], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1063_NVCHAR_TO_VARBYTE */
{ &Adi_fis[1125], adu_lvch_move, NULL, NULL },	/* ADFI_1064_NCHAR_TO_LBYTE */
{ &Adi_fis[1126], adu_lvch_move, NULL, NULL },	/* ADFI_1065_NVCHAR_TO_LBYTE */
{ &Adi_fis[1127], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1066_BYTE_TO_NCHAR */
{ &Adi_fis[1128], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1067_BYTE_TO_NVCHAR */
{ &Adi_fis[1129], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1068_VARBYTE_TO_NCHAR */
{ &Adi_fis[1130], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1069_VARBYTE_TO_NVCHAR */
{ &Adi_fis[1131], adu_lvch_move, NULL, NULL },	/* ADFI_1070_LONGNVCHAR_TO_LONGTEXT */
{ &Adi_fis[1132], adu_lvch_move, NULL, NULL },	/* ADFI_1071_LONGTEXT_TO_LONGNVCHAR */
{ &Adi_fis[1133], adu_lvch_move, NULL, NULL },	/* ADFI_1072_NCHAR_TO_LONGNVCHAR */
{ &Adi_fis[1134], adu_lvch_move, NULL, NULL },	/* ADFI_1073_NVCHAR_TO_LONGNVCHAR */
{ &Adi_fis[762], adu_tableinfo, NULL, NULL },	/* ADFI_1074_TABLEINFO */
{ &Adi_fis[1135], adu_lvch_move, NULL, NULL },	/* ADFI_1075_LNVARCHAR_TO_LNVARCHAR */
{ &Adi_fis[224], adu_4strconcat, NULL, NULL },	/* ADFI_1076_NCHAR_ADD_NCHAR */
{ &Adi_fis[225], adu_4strconcat, NULL, NULL },	/* ADFI_1077_NVARCHAR_ADD_NVARCHAR */
{ &Adi_fis[600], adu_ifnull, NULL, NULL },	/* ADFI_1078_IFNULL_NCHAR_NCHAR */
{ &Adi_fis[601], adu_ifnull, NULL, NULL },	/* ADFI_1079_IFNULL_NVARCHAR_NVARCHAR */
{ &Adi_fis[1103], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1080_NCHAR_TO_CHAR */
{ &Adi_fis[1104], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1081_NCHAR_TO_VARCHAR */
{ &Adi_fis[1105], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1082_NVCHAR_TO_CHAR */
{ &Adi_fis[1106], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1083_NVCHAR_TO_VARCHAR */
{ &Adi_fis[1107], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1084_CHAR_TO_NCHAR */
{ &Adi_fis[1108], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1085_CHAR_TO_NVCHAR */
{ &Adi_fis[1112], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1086_VARCHAR_TO_NCHAR */
{ &Adi_fis[1116], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1087_VARCHAR_TO_NVCHAR */
{ &Adi_fis[696], adu_lvch_move, NULL, NULL },	/* ADFI_1088_LVCH_CHAR */
{ &Adi_fis[697], adu_lvch_move, NULL, NULL },	/* ADFI_1089_LVCH_C */
{ &Adi_fis[698], adu_lvch_move, NULL, NULL },	/* ADFI_1090_LVCH_TEXT */
{ &Adi_fis[699], adu_lvch_move, NULL, NULL },	/* ADFI_1091_LVCH_VARCHAR */
{ &Adi_fis[700], adu_lvch_move, NULL, NULL },	/* ADFI_1092_LVCH_LVCH */
{ &Adi_fis[701], adu_lvch_move, NULL, NULL },	/* ADFI_1093_LVCH_LTXT */
{ &Adi_fis[702], adu_lvch_move, NULL, NULL },	/* ADFI_1094_LVCH_LBYTE */
{ &Adi_fis[703], adu_lvch_move, NULL, NULL },	/* ADFI_1095_LBYTE_C */
{ &Adi_fis[704], adu_lvch_move, NULL, NULL },	/* ADFI_1096_LBYTE_CHAR */
{ &Adi_fis[705], adu_lvch_move, NULL, NULL },	/* ADFI_1097_LBYTE_VARCHAR */
{ &Adi_fis[706], adu_lvch_move, NULL, NULL },	/* ADFI_1098_LBYTE_TEXT */
{ &Adi_fis[707], adu_lvch_move, NULL, NULL },	/* ADFI_1099_LBYTE_LONGTEXT */
{ &Adi_fis[708], adu_lvch_move, NULL, NULL },	/* ADFI_1100_LBYTE_BYTE */
{ &Adi_fis[709], adu_lvch_move, NULL, NULL },	/* ADFI_1101_LBYTE_VARBYTE */
{ &Adi_fis[710], adu_lvch_move, NULL, NULL },	/* ADFI_1102_LBYTE_LBYTE */
{ &Adi_fis[711], adu_lvch_move, NULL, NULL },	/* ADFI_1103_LBYTE_LVCH */
{ &Adi_fis[712], adu_lvch_move, NULL, NULL },	/* ADFI_1104_LBYTE_NCHAR */
{ &Adi_fis[713], adu_lvch_move, NULL, NULL },	/* ADFI_1105_LBYTE_NVCHAR */
{ &Adi_fis[817], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1106_NCHAR_ALL */
{ &Adi_fis[819], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1107_NVCHAR_ALL */
{ &Adi_fis[821], adu_nvchr_toutf8, NULL, NULL },	/* ADFI_1108_UTF16TOUTF8_NCHR */
{ &Adi_fis[822], adu_nvchr_toutf8, NULL, NULL },	/* ADFI_1109_UTF16TOUTF8_NVCHR */
{ &Adi_fis[823], adu_nvchr_fromutf8, NULL, NULL },	/* ADFI_1110_UTF8TOUTF16_NCHR */
{ &Adi_fis[824], adu_nvchr_fromutf8, NULL, NULL },	/* ADFI_1111_UTF8TOUTF16_NVCHR */
{ &Adi_fis[923], adu_nvchr_toutf8, NULL, NULL },	/* ADFI_1112_NCHAR_COPYTO_UTF8 */
{ &Adi_fis[924], adu_nvchr_toutf8, NULL, NULL },	/* ADFI_1113_NVCHAR_COPYTO_UTF8 */
{ &Adi_fis[925], adu_nvchr_fromutf8, NULL, NULL },	/* ADFI_1114_UTF8_COPYTO_NCHAR */
{ &Adi_fis[926], adu_nvchr_fromutf8, NULL, NULL },	/* ADFI_1115_UTF8_COPYTO_NVCHAR */
{ &Adi_fis[825], adu_1int_coerce, NULL, NULL },	/* ADFI_1116_INT8_C */
{ &Adi_fis[827], adu_1int_coerce, NULL, NULL },	/* ADFI_1117_INT8_F */
{ &Adi_fis[829], adu_1int_coerce, NULL, NULL },	/* ADFI_1118_INT8_I */
{ &Adi_fis[831], adu_1int_coerce, NULL, NULL },	/* ADFI_1119_INT8_TEXT */
{ &Adi_fis[830], adu_1int_coerce, NULL, NULL },	/* ADFI_1120_INT8_MONEY */
{ &Adi_fis[828], adu_1int_coerce, NULL, NULL },	/* ADFI_1121_INT8_DEC */
{ &Adi_fis[826], adu_1int_coerce, NULL, NULL },	/* ADFI_1122_INT8_CHAR */
{ &Adi_fis[832], adu_1int_coerce, NULL, NULL },	/* ADFI_1123_INT8_VARCHAR */
{ &Adi_fis[1136], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1124_NCHAR_TO_CHR */
{ &Adi_fis[1137], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1125_CHR_TO_NCHAR */
{ &Adi_fis[1138], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1126_NVCHAR_TO_CHR */
{ &Adi_fis[1139], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1127_CHR_TO_NVCHAR */
{ &Adi_fis[1140], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1128_NCHAR_TO_TXT */
{ &Adi_fis[1141], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1129_TXT_TO_NCHAR */
{ &Adi_fis[1142], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1130_NVCHAR_TO_TXT */
{ &Adi_fis[1143], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1131_TXT_TO_NVCHAR */
{ &Adi_fis[1144], adu_14strtodate, NULL, NULL },	/* ADFI_1132_NCHAR_TO_DATE */
{ &Adi_fis[1145], adu_6datetostr, NULL, NULL },	/* ADFI_1133_DATE_TO_NCHAR */
{ &Adi_fis[1146], adu_14strtodate, NULL, NULL },	/* ADFI_1134_NVCHAR_TO_DATE */
{ &Adi_fis[1147], adu_6datetostr, NULL, NULL },	/* ADFI_1135_DATE_TO_NVCHAR */
{ &Adi_fis[1148], adu_2strtomny, NULL, NULL },	/* ADFI_1136_NCHAR_TO_MONEY */
{ &Adi_fis[1149], adu_9mnytostr, NULL, NULL },	/* ADFI_1137_MONEY_TO_NCHAR */
{ &Adi_fis[1150], adu_2strtomny, NULL, NULL },	/* ADFI_1138_NVCHAR_TO_MONEY */
{ &Adi_fis[1151], adu_9mnytostr, NULL, NULL },	/* ADFI_1139_MONEY_TO_NVCHAR */
{ &Adi_fis[1152], adu_4strtologkey, NULL, NULL },	/* ADFI_1140_NCHAR_TO_LOGKEY */
{ &Adi_fis[1153], adu_3logkeytostr, NULL, NULL },	/* ADFI_1141_LOGKEY_TO_NCHAR */
{ &Adi_fis[1154], adu_4strtologkey, NULL, NULL },	/* ADFI_1142_NVCHAR_TO_LOGKEY */
{ &Adi_fis[1155], adu_3logkeytostr, NULL, NULL },	/* ADFI_1143_LOGKEY_TO_NVCHAR */
{ &Adi_fis[1156], adu_4strtologkey, NULL, NULL },	/* ADFI_1144_NCHAR_TO_TABKEY */
{ &Adi_fis[1157], adu_3logkeytostr, NULL, NULL },	/* ADFI_1145_TABKEY_TO_NCHAR */
{ &Adi_fis[1158], adu_4strtologkey, NULL, NULL },	/* ADFI_1146_NVCHAR_TO_TABKEY */
{ &Adi_fis[1159], adu_3logkeytostr, NULL, NULL },	/* ADFI_1147_TABKEY_TO_NVCHAR */
{ NULL, NULL, NULL, NULL },	/* No FI 1148 */
{ NULL, NULL, NULL, NULL },	/* No FI 1149 */
{ NULL, NULL, NULL, NULL },	/* No FI 1150 */
{ NULL, NULL, NULL, NULL },	/* No FI 1151 */
{ NULL, NULL, NULL, NULL },	/* No FI 1152 */
{ NULL, NULL, NULL, NULL },	/* No FI 1153 */
{ NULL, NULL, NULL, NULL },	/* No FI 1154 */
{ NULL, NULL, NULL, NULL },	/* No FI 1155 */
{ &Adi_fis[444], adu_1int_coerce, NULL, NULL },	/* ADFI_1156_INT1_NCHAR */
{ &Adi_fis[445], adu_1int_coerce, NULL, NULL },	/* ADFI_1157_INT1_NVCHAR */
{ &Adi_fis[454], adu_1int_coerce, NULL, NULL },	/* ADFI_1158_INT2_NCHAR */
{ &Adi_fis[455], adu_1int_coerce, NULL, NULL },	/* ADFI_1159_INT2_NVCHAR */
{ &Adi_fis[464], adu_1int_coerce, NULL, NULL },	/* ADFI_1160_INT4_NCHAR */
{ &Adi_fis[465], adu_1int_coerce, NULL, NULL },	/* ADFI_1161_INT4_NVCHAR */
{ &Adi_fis[833], adu_1int_coerce, NULL, NULL },	/* ADFI_1162_INT8_NCHAR */
{ &Adi_fis[834], adu_1int_coerce, NULL, NULL },	/* ADFI_1163_INT8_NVCHAR */
{ &Adi_fis[424], adu_1flt_coerce, NULL, NULL },	/* ADFI_1164_FLOAT4_NCHAR */
{ &Adi_fis[425], adu_1flt_coerce, NULL, NULL },	/* ADFI_1165_FLOAT4_NVCHAR */
{ &Adi_fis[434], adu_1flt_coerce, NULL, NULL },	/* ADFI_1166_FLOAT8_NCHAR */
{ &Adi_fis[435], adu_1flt_coerce, NULL, NULL },	/* ADFI_1167_FLOAT8_NVCHAR */
{ &Adi_fis[630], adu_2dec_convert, NULL, NULL },	/* ADFI_1168_DEC_NCHAR_I */
{ &Adi_fis[631], adu_2dec_convert, NULL, NULL },	/* ADFI_1169_DEC_NVCHAR_I */
{ &Adi_fis[374], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1170_CHAR_NCHAR */
{ &Adi_fis[375], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1171_CHAR_NVCHAR */
{ &Adi_fis[584], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1172_VARCHAR_NCHAR */
{ &Adi_fis[585], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1173_VARCHAR_NVCHAR */
{ &Adi_fis[563], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1174_TEXT_NCHAR */
{ &Adi_fis[564], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1175_TEXT_NVCHAR */
{ &Adi_fis[368], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1176_ASCII_NCHAR */
{ &Adi_fis[369], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1177_ASCII_NVCHAR */
{ &Adi_fis[402], adu_14strtodate, NULL, NULL },	/* ADFI_1178_DATE_NCHAR */
{ &Adi_fis[403], adu_14strtodate, NULL, NULL },	/* ADFI_1179_DATE_NVCHAR */
{ &Adi_fis[513], adu_2strtomny, NULL, NULL },	/* ADFI_1180_MONEY_NCHAR */
{ &Adi_fis[514], adu_2strtomny, NULL, NULL },	/* ADFI_1181_MONEY_NVCHAR */
{ &Adi_fis[927], adu_nvchr_strutf8conv, NULL, NULL },	/* ADFI_1182_CHAR_COPYTO_UTF8 */
{ &Adi_fis[928], adu_nvchr_strutf8conv, NULL, NULL },	/* ADFI_1183_VARCHAR_COPYTO_UTF8 */
{ &Adi_fis[929], adu_nvchr_strutf8conv, NULL, NULL },	/* ADFI_1184_UTF8_COPYTO_CHAR */
{ &Adi_fis[930], adu_nvchr_strutf8conv, NULL, NULL },	/* ADFI_1185_UTF8_COPYTO_VARCHAR */
{ &Adi_fis[1160], adu_nvchr_strutf8conv, NULL, NULL },	/* ADFI_1186_CHAR_UTF8 */
{ &Adi_fis[1161], adu_nvchr_strutf8conv, NULL, NULL },	/* ADFI_1187_VARCHAR_UTF8 */
{ &Adi_fis[1162], adu_nvchr_strutf8conv, NULL, NULL },	/* ADFI_1188_UTF8_CHAR */
{ &Adi_fis[1163], adu_nvchr_strutf8conv, NULL, NULL },	/* ADFI_1189_UTF8_VARCHAR */
{ &Adi_fis[835], adu_collweight, NULL, NULL },	/* ADFI_1190_COWGT_C */
{ &Adi_fis[836], adu_collweight, NULL, NULL },	/* ADFI_1191_COWGT_CHAR */
{ &Adi_fis[837], adu_collweight, NULL, NULL },	/* ADFI_1192_COWGT_VARCHAR */
{ &Adi_fis[838], adu_collweight, NULL, NULL },	/* ADFI_1193_COWGT_TEXT */
{ &Adi_fis[839], adu_ucollweight, NULL, NULL },	/* ADFI_1194_COWGT_NCHAR */
{ &Adi_fis[840], adu_ucollweightn, NULL, NULL },	/* ADFI_1195_COWGT_NCHAR_INT */
{ &Adi_fis[841], adu_ucollweight, NULL, NULL },	/* ADFI_1196_COWGT_NVCHAR */
{ &Adi_fis[842], adu_ucollweightn, NULL, NULL },	/* ADFI_1197_COWGT_NVCHAR_INT */
{ &Adi_fis[1164], adu_lvch_move, NULL, NULL },	/* ADFI_1198_CHAR_TO_LONGNVCHAR */
{ &Adi_fis[1165], adu_lvch_move, NULL, NULL },	/* ADFI_1199_VCHAR_TO_LONGNVCHAR */
{ &Adi_fis[492], adu_nvchr_length, NULL, NULL },	/* ADFI_1200_LENGTH_LONGNVCHAR */
{ &Adi_fis[554], adu_12strsize, NULL, NULL },	/* ADFI_1201_SIZE_LONGNVCHAR */
{ &Adi_fis[688], adu_date_isdst, NULL, NULL },	/* ADFI_1202_ISDST_DATE */
{ &Adi_fis[769], adu_17lvch_position, NULL, NULL },	/* ADFI_1203_POS_LVCH_LVCH */
{ &Adi_fis[770], adu_17lvch_position, NULL, NULL },	/* ADFI_1204_POS_LNVCHR_LNVCHR */
{ &Adi_fis[771], adu_17lvch_position, NULL, NULL },	/* ADFI_1205_POS_LBYTE_LBYTE */
{ &Adi_fis[772], adu_26positionfrom, NULL, NULL },	/* ADFI_1206_POS_STR_STR_I */
{ &Adi_fis[773], adu_26positionfrom, NULL, NULL },	/* ADFI_1207_POS_UNISTR_UNISTR_I */
{ &Adi_fis[774], adu_26positionfrom, NULL, NULL },	/* ADFI_1208_POS_VBYTE_VBYTE_I */
{ &Adi_fis[775], adu_16lvch_position, NULL, NULL },	/* ADFI_1209_POS_STR_LVCH_I */
{ &Adi_fis[776], adu_16lvch_position, NULL, NULL },	/* ADFI_1210_POS_UNISTR_LNVCHR_I */
{ &Adi_fis[777], adu_16lvch_position, NULL, NULL },	/* ADFI_1211_POS_BYTE_LBYTE_I */
{ &Adi_fis[778], adu_18lvch_position, NULL, NULL },	/* ADFI_1212_POS_LVCH_LVCH_I */
{ &Adi_fis[779], adu_18lvch_position, NULL, NULL },	/* ADFI_1213_POS_LNVCHR_LNVCHR_I */
{ &Adi_fis[780], adu_18lvch_position, NULL, NULL },	/* ADFI_1214_POS_LBYTE_LBYTE_I */
{ NULL, NULL, NULL, NULL },	/* No FI 1215 */
{ NULL, NULL, NULL, NULL },	/* No FI 1216 */
{ NULL, NULL, NULL, NULL },	/* No FI 1217 */
{ NULL, NULL, NULL, NULL },	/* No FI 1218 */
{ &Adi_fis[752], adu_13lvch_substr, NULL, NULL },	/* ADFI_1219_SUBSTRING_LVCH_I */
{ &Adi_fis[753], adu_13lvch_substr, NULL, NULL },	/* ADFI_1220_SUBSTRING_LBYTE_I */
{ &Adi_fis[754], adu_13lvch_substr, NULL, NULL },	/* ADFI_1221_SUBSTRING_LNVCHR_I */
{ &Adi_fis[755], adu_14lvch_substrlen, NULL, NULL },	/* ADFI_1222_SUBSTRING_LVCH_I_I */
{ &Adi_fis[756], adu_14lvch_substrlen, NULL, NULL },	/* ADFI_1223_SUBSTRING_LBYTE_I_I */
{ &Adi_fis[757], adu_14lvch_substrlen, NULL, NULL },	/* ADFI_1224_SUBSTRING_LNVCHR_I_I */
{ NULL, NULL, NULL, NULL },	/* No FI 1225 */
{ NULL, NULL, NULL, NULL },	/* No FI 1226 */
{ NULL, NULL, NULL, NULL },	/* No FI 1227 */
{ NULL, NULL, NULL, NULL },	/* No FI 1228 */
{ NULL, NULL, NULL, NULL },	/* No FI 1229 */
{ NULL, NULL, NULL, NULL },	/* No FI 1230 */
{ NULL, NULL, NULL, NULL },	/* No FI 1231 */
{ NULL, NULL, NULL, NULL },	/* No FI 1232 */
{ &Adi_fis[689], NULL, NULL, NULL },	/* ADFI_1233_CURRENT_DATE */
{ &Adi_fis[690], NULL, NULL, NULL },	/* ADFI_1234_CURRENT_TIME */
{ &Adi_fis[691], NULL, NULL, NULL },	/* ADFI_1235_CURRENT_TMSTMP */
{ &Adi_fis[692], NULL, NULL, NULL },	/* ADFI_1236_LOCAL_TIME */
{ &Adi_fis[693], NULL, NULL, NULL },	/* ADFI_1237_LOCAL_TMSTMP */
{ &Adi_fis[843], adu_21ansi_strtodt, NULL, NULL },	/* ADFI_1238_ANSI_DATE */
{ &Adi_fis[844], adu_21ansi_strtodt, NULL, NULL },	/* ADFI_1239_ANSI_TMWO */
{ &Adi_fis[845], adu_21ansi_strtodt, NULL, NULL },	/* ADFI_1240_ANSI_TMW */
{ &Adi_fis[846], adu_21ansi_strtodt, NULL, NULL },	/* ADFI_1241_ANSI_LCTME */
{ &Adi_fis[847], adu_21ansi_strtodt, NULL, NULL },	/* ADFI_1242_ANSI_TSWO */
{ &Adi_fis[848], adu_21ansi_strtodt, NULL, NULL },	/* ADFI_1243_ANSI_TSW */
{ &Adi_fis[849], adu_21ansi_strtodt, NULL, NULL },	/* ADFI_1244_ANSI_TSTMP */
{ &Adi_fis[850], adu_21ansi_strtodt, NULL, NULL },	/* ADFI_1245_ANSI_INDS */
{ &Adi_fis[851], adu_21ansi_strtodt, NULL, NULL },	/* ADFI_1246_ANSI_INYM */
{ &Adi_fis[852], adu_year_part, NULL, NULL },	/* ADFI_1247_YEARPART */
{ &Adi_fis[853], adu_quarter_part, NULL, NULL },	/* ADFI_1248_QUARTERPART */
{ &Adi_fis[854], adu_month_part, NULL, NULL },	/* ADFI_1249_MONTHPART */
{ &Adi_fis[855], adu_week_part, NULL, NULL },	/* ADFI_1250_WEEKPART */
{ &Adi_fis[856], adu_weekiso_part, NULL, NULL },	/* ADFI_1251_WEEKISOPART */
{ &Adi_fis[857], adu_day_part, NULL, NULL },	/* ADFI_1252_DAYPART */
{ &Adi_fis[858], adu_hour_part, NULL, NULL },	/* ADFI_1253_HOURPART */
{ &Adi_fis[859], adu_minute_part, NULL, NULL },	/* ADFI_1254_MINUTEPART */
{ &Adi_fis[860], adu_second_part, NULL, NULL },	/* ADFI_1255_SECONDPART */
{ &Adi_fis[861], adu_microsecond_part, NULL, NULL },	/* ADFI_1256_MICROSECONDPART */
{ &Adi_fis[862], adu_nanosecond_part, NULL, NULL },	/* ADFI_1257_NANOSECONDPART */
{ &Adi_fis[1166], adu_locator_to_cpn, NULL, NULL },	/* ADFI_1258_LNLOC_TO_LNVCHR */
{ &Adi_fis[1167], adu_locator_to_cpn, NULL, NULL },	/* ADFI_1259_LBLOC_TO_LBYTE */
{ &Adi_fis[1168], adu_locator_to_cpn, NULL, NULL },	/* ADFI_1260_LCLOC_TO_LVCH */
{ &Adi_fis[1169], adu_cpn_to_locator, NULL, NULL },	/* ADFI_1261_LNVCHR_TO_LNLOC */
{ &Adi_fis[1170], adu_cpn_to_locator, NULL, NULL },	/* ADFI_1262_LBYTE_TO_LBLOC */
{ &Adi_fis[1171], adu_cpn_to_locator, NULL, NULL },	/* ADFI_1263_LVCH_TO_LCLOC */
{ &Adi_fis[226], adu_7date_subu, NULL, NULL },	/* ADFI_1264_TMWO_SUB_TMWO */
{ &Adi_fis[227], adu_7date_subu, NULL, NULL },	/* ADFI_1265_TMW_SUB_TMW */
{ &Adi_fis[228], adu_7date_subu, NULL, NULL },	/* ADFI_1266_TME_SUB_TME */
{ &Adi_fis[229], adu_7date_subu, NULL, NULL },	/* ADFI_1267_TSWO_SUB_TSWO */
{ &Adi_fis[230], adu_7date_subu, NULL, NULL },	/* ADFI_1268_TSW_SUB_TSW */
{ &Adi_fis[231], adu_7date_subu, NULL, NULL },	/* ADFI_1269_TSTMP_SUB_TSTMP */
{ &Adi_fis[232], adu_7date_subu, NULL, NULL },	/* ADFI_1270_ANSIDATE_SUB_ANSIDATE */
{ &Adi_fis[1172], adu_1strtostr, NULL, NULL },	/* ADFI_1271_LTXT_TO_LTXT */
{ &Adi_fis[88], NULL, NULL, NULL },	/* ADFI_1272_LTXT_EQ_LTXT */
{ &Adi_fis[820], adu_nvchr_2args, NULL, NULL },	/* ADFI_1273_NVCHAR_ALL_I */
{ &Adi_fis[195], adu_1date_addu, NULL, NULL },	/* ADFI_1274_DATE_ADD_INYM */
{ &Adi_fis[196], adu_1date_addu, NULL, NULL },	/* ADFI_1275_DATE_ADD_INDS */
{ &Adi_fis[197], adu_1date_addu, NULL, NULL },	/* ADFI_1276_INYM_ADD_DATE */
{ &Adi_fis[198], adu_1date_addu, NULL, NULL },	/* ADFI_1277_INDS_ADD_DATE */
{ &Adi_fis[1173], adu_lvch_move, NULL, NULL },	/* ADFI_1278_NVCHAR_TO_LONGVCHAR */
{ &Adi_fis[1174], adu_lvch_move, NULL, NULL },	/* ADFI_1279_VCHAR_TO_LONGNVCHAR */
{ &Adi_fis[675], adu_varbyte_2arg, NULL, NULL },	/* ADFI_1280_VARBYTE_ALL_I */
{ &Adi_fis[503], adu_5lvch_lower, NULL, NULL },	/* ADFI_1281_LOWERCASE_LNVCHR */
{ &Adi_fis[579], adu_3lvch_upper, NULL, NULL },	/* ADFI_1282_UPPERCASE_LNVCHR */
{ &Adi_fis[863], adu_decround, NULL, NULL },	/* ADFI_1283_DECROUND */
{ &Adi_fis[864], adu_dectrunc, NULL, NULL },	/* ADFI_1284_DECTRUNC */
{ &Adi_fis[865], adu_decceil, NULL, NULL },	/* ADFI_1285_DECCEIL */
{ &Adi_fis[866], adu_decfloor, NULL, NULL },	/* ADFI_1286_DECFLOOR */
{ &Adi_fis[867], adu_chr, NULL, NULL },	/* ADFI_1287_CHR */
{ &Adi_fis[868], adu_ltrim, NULL, NULL },	/* ADFI_1288_LTRIM_CHAR */
{ &Adi_fis[869], adu_ltrim, NULL, NULL },	/* ADFI_1289_LTRIM_VARCHAR */
{ &Adi_fis[870], NULL, NULL, NULL },	/* ADFI_1290_LTRIM_NCHAR */
{ &Adi_fis[871], NULL, NULL, NULL },	/* ADFI_1291_LTRIM_NVCHR */
{ NULL, NULL, NULL, NULL },	/* No FI 1292 */
{ NULL, NULL, NULL, NULL },	/* No FI 1293 */
{ NULL, NULL, NULL, NULL },	/* No FI 1294 */
{ NULL, NULL, NULL, NULL },	/* No FI 1295 */
{ &Adi_fis[504], adu_ascii, NULL, NULL },	/* ADFI_1296_LOWERCASE_I */
{ &Adi_fis[580], adu_ascii, NULL, NULL },	/* ADFI_1297_UPPERCASE_I */
{ &Adi_fis[872], adu_27lpad, NULL, NULL },	/* ADFI_1298_LPAD_VARCHAR */
{ &Adi_fis[873], adu_27lpad, NULL, NULL },	/* ADFI_1299_LPAD_CHAR */
{ &Adi_fis[874], adu_27alpad, NULL, NULL },	/* ADFI_1300_LPAD_VARCHAR_VARCHAR */
{ &Adi_fis[875], adu_27alpad, NULL, NULL },	/* ADFI_1301_LPAD_CHAR_VARCHAR */
{ &Adi_fis[876], adu_28rpad, NULL, NULL },	/* ADFI_1302_RPAD_VARCHAR */
{ &Adi_fis[877], adu_28rpad, NULL, NULL },	/* ADFI_1303_RPAD_CHAR */
{ &Adi_fis[878], adu_28arpad, NULL, NULL },	/* ADFI_1304_RPAD_VARCHAR_VARCHAR */
{ &Adi_fis[879], adu_28arpad, NULL, NULL },	/* ADFI_1305_RPAD_CHAR_VARCHAR */
{ &Adi_fis[880], adu_29replace, NULL, NULL },	/* ADFI_1306_REPL_VARCH_VARCH_VARCH */
{ &Adi_fis[881], adu_29replace, NULL, NULL },	/* ADFI_1307_REPL_CHAR_VARCH_VARCH */
{ &Adi_fis[882], NULL, NULL, NULL },	/* ADFI_1308_ACOS_F */
{ &Adi_fis[883], NULL, NULL, NULL },	/* ADFI_1309_ASIN_F */
{ &Adi_fis[884], NULL, NULL, NULL },	/* ADFI_1310_TAN_F */
{ &Adi_fis[885], NULL, NULL, NULL },	/* ADFI_1311_PI */
{ &Adi_fis[886], NULL, NULL, NULL },	/* ADFI_1312_SIGN_I */
{ &Adi_fis[887], NULL, NULL, NULL },	/* ADFI_1313_SIGN_F */
{ &Adi_fis[888], NULL, NULL, NULL },	/* ADFI_1314_SIGN_DEC */
{ &Adi_fis[889], NULL, NULL, NULL },	/* ADFI_1315_ATAN2_F_F */
{ &Adi_fis[205], adu_4strconcat, NULL, NULL },	/* ADFI_1316_C_ADD_CHAR */
{ &Adi_fis[206], adu_4strconcat, NULL, NULL },	/* ADFI_1317_CHAR_ADD_C */
{ &Adi_fis[207], adu_4strconcat, NULL, NULL },	/* ADFI_1318_C_ADD_VARCHAR */
{ &Adi_fis[1089], adu_1dec_coerce, NULL, NULL },	/* ADFI_1319_NCHR_TO_DEC */
{ &Adi_fis[1090], adu_1dec_coerce, NULL, NULL },	/* ADFI_1320_NVCHR_TO_DEC */
{ &Adi_fis[943], adu_1int_coerce, NULL, NULL },	/* ADFI_1321_C_TO_I */
{ &Adi_fis[944], adu_1int_coerce, NULL, NULL },	/* ADFI_1322_TXT_TO_I */
{ &Adi_fis[945], adu_1int_coerce, NULL, NULL },	/* ADFI_1323_CHAR_TO_I */
{ &Adi_fis[946], adu_1int_coerce, NULL, NULL },	/* ADFI_1324_VARCHAR_TO_I */
{ &Adi_fis[947], adu_1int_coerce, NULL, NULL },	/* ADFI_1325_NCHAR_TO_I */
{ &Adi_fis[948], adu_1int_coerce, NULL, NULL },	/* ADFI_1326_NVARCHAR_TO_I */
{ &Adi_fis[933], adu_1flt_coerce, NULL, NULL },	/* ADFI_1327_C_TO_F */
{ &Adi_fis[934], adu_1flt_coerce, NULL, NULL },	/* ADFI_1328_TXT_TO_F */
{ &Adi_fis[935], adu_1flt_coerce, NULL, NULL },	/* ADFI_1329_CHAR_TO_F */
{ &Adi_fis[936], adu_1flt_coerce, NULL, NULL },	/* ADFI_1330_VARCHAR_TO_F */
{ &Adi_fis[937], adu_1flt_coerce, NULL, NULL },	/* ADFI_1331_NCHAR_TO_F */
{ &Adi_fis[938], adu_1flt_coerce, NULL, NULL },	/* ADFI_1332_NVARCHAR_TO_F */
{ &Adi_fis[964], adu_ascii, NULL, NULL },	/* ADFI_1333_I_TO_C */
{ &Adi_fis[965], adu_ascii, NULL, NULL },	/* ADFI_1334_F_TO_C */
{ &Adi_fis[1096], adu_ascii, NULL, NULL },	/* ADFI_1335_I_TO_TXT */
{ &Adi_fis[1097], adu_ascii, NULL, NULL },	/* ADFI_1336_F_TO_TXT */
{ &Adi_fis[1077], adu_ascii, NULL, NULL },	/* ADFI_1337_I_TO_CHAR */
{ &Adi_fis[1078], adu_ascii, NULL, NULL },	/* ADFI_1338_F_TO_CHAR */
{ &Adi_fis[1081], adu_ascii, NULL, NULL },	/* ADFI_1339_I_TO_VARCHAR */
{ &Adi_fis[1082], adu_ascii, NULL, NULL },	/* ADFI_1340_F_TO_VARCHAR */
{ &Adi_fis[1109], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1341_DEC_TO_NCHAR */
{ &Adi_fis[1110], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1342_I_TO_NCHAR */
{ &Adi_fis[1111], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1343_F_TO_NCHAR */
{ &Adi_fis[1113], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1344_DEC_TO_NVCHAR */
{ &Adi_fis[1114], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1345_I_TO_NVCHAR */
{ &Adi_fis[1115], adu_nvchr_coerce, NULL, NULL },	/* ADFI_1346_F_TO_NVCHAR */
{ &Adi_fis[66], NULL, NULL, NULL },	/* ADFI_1347_NUMERIC_EQ */
{ &Adi_fis[0], NULL, NULL, NULL },	/* ADFI_1348_NUMERIC_NE */
{ &Adi_fis[22], NULL, NULL, NULL },	/* ADFI_1349_NUMERIC_LT */
{ &Adi_fis[111], NULL, NULL, NULL },	/* ADFI_1350_NUMERIC_GE */
{ &Adi_fis[89], NULL, NULL, NULL },	/* ADFI_1351_NUMERIC_GT */
{ &Adi_fis[44], NULL, NULL, NULL },	/* ADFI_1352_NUMERIC_LE */
{ &Adi_fis[723], adu_ipaddr, NULL, NULL },	/* ADFI_1353_IPADDR_C */
{ &Adi_fis[724], adu_ipaddr, NULL, NULL },	/* ADFI_1354_IPADDR_CHAR */
{ &Adi_fis[725], adu_ipaddr, NULL, NULL },	/* ADFI_1355_IPADDR_TEXT */
{ &Adi_fis[726], adu_ipaddr, NULL, NULL },	/* ADFI_1356_IPADDR_VARCHAR */
{ &Adi_fis[169], NULL, NULL, NULL },	/* ADFI_1357_ALL_ISINT */
{ &Adi_fis[170], NULL, NULL, NULL },	/* ADFI_1358_ALL_ISNOTINT */
{ &Adi_fis[171], NULL, NULL, NULL },	/* ADFI_1359_ALL_ISDEC */
{ &Adi_fis[172], NULL, NULL, NULL },	/* ADFI_1360_ALL_ISNOTDEC */
{ &Adi_fis[173], NULL, NULL, NULL },	/* ADFI_1361_ALL_ISFLT */
{ &Adi_fis[174], NULL, NULL, NULL },	/* ADFI_1362_ALL_ISNOTFLT */
{ &Adi_fis[818], adu_nvchr_2args, NULL, NULL },	/* ADFI_1363_NCHAR_ALL_I */
{ &Adi_fis[191], adu_date_mul, NULL, NULL },	/* ADFI_1364_DTE_MUL_F */
{ &Adi_fis[192], adu_date_mul, NULL, NULL },	/* ADFI_1365_F_MUL_DTE */
{ &Adi_fis[254], adu_date_div, NULL, NULL },	/* ADFI_1366_DTE_DIV_F */
{ &Adi_fis[255], adu_date_div, NULL, NULL },	/* ADFI_1367_DTE_DIV_DTE */
{ &Adi_fis[265], adu_date_minus, NULL, NULL },	/* ADFI_1368_MINUS_DTE */
{ &Adi_fis[260], adu_date_plus, NULL, NULL },	/* ADFI_1369_PLUS_DTE */
{ NULL, NULL, NULL, NULL }
};

GLOBALDEF const i4 Adi_fil_size = sizeof(Adi_fi_lkup);
