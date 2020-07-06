/*
** Copyright (c) 2007 Ingres Corporation
*/
#include <compat.h>
#include <gl.h>
#include <iicommon.h>
#include <adf.h>
#include <adfops.h>
#include <ade.h>
#include <adp.h>
#include <ulf.h>
#include <adfint.h>
#include <adffiids.h>
#include <adudate.h>
#include <aduint.h>

/**
**
**  Name: ADGFI_DEFN.ROC - ADF's function instance table.
**
**  Description:
**	This file contains the function instance table initialization which is a
**	table of ADI_FI_DESC structures, one per f.i.  These structures hold
**	everything ADF callers will need to see for any f.i. This file used to
**	also initialize the function instance lookup table, an ADF internal
**	structure used for quick access to a particular function instance
**	given the function instance ID
**
**	** DO NOT MAKE MANUAL CHANGES TO THIS FILE **
**
**	This file is generated from fi_defn.txt by the AWK program fi_defn.awk.
**	To change function instance info, modify fi_defn.txt and run fi_defn.awk;
*/

GLOBALDEF READONLY ADI_FI_DESC Adi_fis[]= {
    /*
    ** This is the definition of ADF's function instance table.  Each element
    ** of the table is an ADI_FI_DESC structure which carries all information
    ** visible to the outside world for its corresponding function instance.
    **
    **      You will notice that some function instances have  as
    **  their input or result datatype.  This is valid only for non-coercion
    **  function instances.  If a function instance has an input type of
    **  DB_ALL_TYPE, all types are valid for this function instance although
    **  this function instance will be less preferable than those which
    **  specifically mention the input types.  If a function instance has a
    **  result type of DB_ALL_TYPE, this means that the result datatype could be
    **  anything and must be determined by calling adi_res_type().
    */

/*----------------------------------------------------------------------------*/
/*  Each array element in the function instance table looks like this:        */
/*----------------------------------------------------------------------------*/
/* /* [nnnn]:  Comment describing the function instance ...                   */
/*                                                                            */
/*  {   fi_ID,                          complment_fi_ID,                      */
/*          type_of_fi,         fi_flags,               op_ID,                */
/*              {     length_spec    },   aggr_w/s_dv_length, null_pre-instr, */
/*                  #_args,  result_dt,                                       */
/*                           arg_#1_dt,                                       */
/*                           arg_#2_dt,                                       */
/*                           arg_#3_dt,                                       */
/*                           arg_#4_dt    }                                   */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*
** [0]:  boolean  :  all != all
*/
{ ADFI_1348_NUMERIC_NE, ADFI_1347_NUMERIC_EQ,
  ADI_COMPARISON, ADI_F8192_QUEL_CLOAKED, ADI_NE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_ALL_TYPE, DB_ALL_TYPE, DB_NODT, DB_NODT
},
/*
** [1]:  boolean  :  c != c
*/
{ ADFI_008_C_NE_C, ADFI_000_C_EQ_C,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_CHR_TYPE, DB_CHR_TYPE, DB_NODT, DB_NODT
},
/*
** [2]:  boolean  :  text != text
*/
{ ADFI_009_TEXT_NE_TEXT, ADFI_001_TEXT_EQ_TEXT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_TXT_TYPE, DB_TXT_TYPE, DB_NODT, DB_NODT
},
/*
** [3]:  boolean  :  char != char
*/
{ ADFI_236_CHAR_NE_CHAR, ADFI_234_CHAR_EQ_CHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_CHA_TYPE, DB_CHA_TYPE, DB_NODT, DB_NODT
},
/*
** [4]:  boolean  :  varchar != varchar
*/
{ ADFI_237_VARCHAR_NE_VARCHAR, ADFI_235_VARCHAR_EQ_VARCHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_VCH_TYPE, DB_VCH_TYPE, DB_NODT, DB_NODT
},
/*
** [5]:  boolean  :  date != date
*/
{ ADFI_010_DATE_NE_DATE, ADFI_002_DATE_EQ_DATE,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_DTE_TYPE, DB_DTE_TYPE, DB_NODT, DB_NODT
},
/*
** [6]:  boolean  :  f != f
*/
{ ADFI_011_F_NE_F, ADFI_003_F_EQ_F,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_FLT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [7]:  boolean  :  f != i
*/
{ ADFI_012_F_NE_I, ADFI_004_F_EQ_I,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_FLT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [8]:  boolean  :  dec != dec
*/
{ ADFI_511_DEC_NE_DEC, ADFI_510_DEC_EQ_DEC,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_DEC_TYPE, DB_DEC_TYPE, DB_NODT, DB_NODT
},
/*
** [9]:  boolean  :  i != f
*/
{ ADFI_013_I_NE_F, ADFI_005_I_EQ_F,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_INT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [10]:  boolean  :  i != i
*/
{ ADFI_014_I_NE_I, ADFI_006_I_EQ_I,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_INT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [11]:  boolean  :  money != money
*/
{ ADFI_015_MONEY_NE_MONEY, ADFI_007_MONEY_EQ_MONEY,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_MNY_TYPE, DB_MNY_TYPE, DB_NODT, DB_NODT
},
/*
** [12]:  boolean  :  logical_key != logical_key
*/
{ ADFI_466_LOGKEY_NE_LOGKEY, ADFI_503_LOGKEY_EQ_LOGKEY,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_LOGKEY_TYPE, DB_LOGKEY_TYPE, DB_NODT, DB_NODT
},
/*
** [13]:  boolean  :  table_key != table_key
*/
{ ADFI_485_TABKEY_NE_TABKEY, ADFI_484_TABKEY_EQ_TABKEY,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_TABKEY_TYPE, DB_TABKEY_TYPE, DB_NODT, DB_NODT
},
/*
** [14]:  boolean  :  bit != bit
*/
{ ADFI_627_BIT_NE_BIT, ADFI_639_BIT_EQ_BIT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_BIT_TYPE, DB_BIT_TYPE, DB_NODT, DB_NODT
},
/*
** [15]:  boolean  :  bit != vbit
*/
{ ADFI_628_BIT_NE_VBIT, ADFI_640_BIT_EQ_VBIT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_BIT_TYPE, DB_VBIT_TYPE, DB_NODT, DB_NODT
},
/*
** [16]:  boolean  :  vbit != vbit
*/
{ ADFI_629_VBIT_NE_VBIT, ADFI_641_VBIT_EQ_VBIT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_VBIT_TYPE, DB_VBIT_TYPE, DB_NODT, DB_NODT
},
/*
** [17]:  boolean  :  vbit != bit
*/
{ ADFI_630_VBIT_NE_BIT, ADFI_642_VBIT_EQ_BIT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_VBIT_TYPE, DB_BIT_TYPE, DB_NODT, DB_NODT
},
/*
** [18]:  boolean  :  byte != byte
*/
{ ADFI_715_BYTE_NE_BYTE, ADFI_721_BYTE_EQ_BYTE,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_BYTE_TYPE, DB_BYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [19]:  boolean  :  vbyte != vbyte
*/
{ ADFI_716_VBYTE_NE_VBYTE, ADFI_722_VBYTE_EQ_VBYTE,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_VBYTE_TYPE, DB_VBYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [20]:  boolean  :  nchar != nchar
*/
{ ADFI_982_NCHAR_NE_NCHAR, ADFI_983_NCHAR_EQ_NCHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_NCHR_TYPE, DB_NCHR_TYPE, DB_NODT, DB_NODT
},
/*
** [21]:  boolean  :  nvarchar != nvarchar
*/
{ ADFI_984_NVCHAR_NE_NVCHAR, ADFI_985_NVCHAR_EQ_NVCHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_NVCHR_TYPE, DB_NVCHR_TYPE, DB_NODT, DB_NODT
},
/*
** [22]:  boolean  :  all < all
*/
{ ADFI_1349_NUMERIC_LT, ADFI_1350_NUMERIC_GE,
  ADI_COMPARISON, ADI_F8192_QUEL_CLOAKED, ADI_LT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_ALL_TYPE, DB_ALL_TYPE, DB_NODT, DB_NODT
},
/*
** [23]:  boolean  :  c < c
*/
{ ADFI_040_C_LT_C, ADFI_032_C_GE_C,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_CHR_TYPE, DB_CHR_TYPE, DB_NODT, DB_NODT
},
/*
** [24]:  boolean  :  text < text
*/
{ ADFI_041_TEXT_LT_TEXT, ADFI_033_TEXT_GE_TEXT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_TXT_TYPE, DB_TXT_TYPE, DB_NODT, DB_NODT
},
/*
** [25]:  boolean  :  char < char
*/
{ ADFI_244_CHAR_LT_CHAR, ADFI_242_CHAR_GE_CHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_CHA_TYPE, DB_CHA_TYPE, DB_NODT, DB_NODT
},
/*
** [26]:  boolean  :  varchar < varchar
*/
{ ADFI_245_VARCHAR_LT_VARCHAR, ADFI_243_VARCHAR_GE_VARCHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_VCH_TYPE, DB_VCH_TYPE, DB_NODT, DB_NODT
},
/*
** [27]:  boolean  :  date < date
*/
{ ADFI_042_DATE_LT_DATE, ADFI_034_DATE_GE_DATE,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_DTE_TYPE, DB_DTE_TYPE, DB_NODT, DB_NODT
},
/*
** [28]:  boolean  :  f < f
*/
{ ADFI_043_F_LT_F, ADFI_035_F_GE_F,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_FLT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [29]:  boolean  :  f < i
*/
{ ADFI_044_F_LT_I, ADFI_036_F_GE_I,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_FLT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [30]:  boolean  :  dec < dec
*/
{ ADFI_515_DEC_LT_DEC, ADFI_514_DEC_GE_DEC,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_DEC_TYPE, DB_DEC_TYPE, DB_NODT, DB_NODT
},
/*
** [31]:  boolean  :  i < f
*/
{ ADFI_045_I_LT_F, ADFI_037_I_GE_F,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_INT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [32]:  boolean  :  i < i
*/
{ ADFI_046_I_LT_I, ADFI_038_I_GE_I,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_INT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [33]:  boolean  :  money < money
*/
{ ADFI_047_MONEY_LT_MONEY, ADFI_039_MONEY_GE_MONEY,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_MNY_TYPE, DB_MNY_TYPE, DB_NODT, DB_NODT
},
/*
** [34]:  boolean  :  table_key < table_key
*/
{ ADFI_489_TABKEY_LT_TABKEY, ADFI_488_TABKEY_GE_TABKEY,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_TABKEY_TYPE, DB_TABKEY_TYPE, DB_NODT, DB_NODT
},
/*
** [35]:  boolean  :  logical_key < logical_key
*/
{ ADFI_470_LOGKEY_LT_LOGKEY, ADFI_469_LOGKEY_GE_LOGKEY,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_LOGKEY_TYPE, DB_LOGKEY_TYPE, DB_NODT, DB_NODT
},
/*
** [36]:  boolean  :  bit < bit
*/
{ ADFI_631_BIT_LT_BIT, ADFI_647_BIT_GE_BIT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_BIT_TYPE, DB_BIT_TYPE, DB_NODT, DB_NODT
},
/*
** [37]:  boolean  :  bit < vbit
*/
{ ADFI_632_BIT_LT_VBIT, ADFI_648_BIT_GE_VBIT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_BIT_TYPE, DB_VBIT_TYPE, DB_NODT, DB_NODT
},
/*
** [38]:  boolean  :  vbit < vbit
*/
{ ADFI_633_VBIT_LT_VBIT, ADFI_649_VBIT_GE_VBIT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_VBIT_TYPE, DB_VBIT_TYPE, DB_NODT, DB_NODT
},
/*
** [39]:  boolean  :  vbit < bit
*/
{ ADFI_634_VBIT_LT_BIT, ADFI_650_VBIT_GE_BIT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_VBIT_TYPE, DB_BIT_TYPE, DB_NODT, DB_NODT
},
/*
** [40]:  boolean  :  byte < byte
*/
{ ADFI_717_BYTE_LT_BYTE, ADFI_725_BYTE_GE_BYTE,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_BYTE_TYPE, DB_BYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [41]:  boolean  :  vbyte < vbyte
*/
{ ADFI_718_VBYTE_LT_VBYTE, ADFI_726_VBYTE_GE_VBYTE,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_VBYTE_TYPE, DB_VBYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [42]:  boolean  :  nchar < nchar
*/
{ ADFI_986_NCHAR_LT_NCHAR, ADFI_987_NCHAR_GE_NCHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_NCHR_TYPE, DB_NCHR_TYPE, DB_NODT, DB_NODT
},
/*
** [43]:  boolean  :  nvarchar < nvarchar
*/
{ ADFI_988_NVCHAR_LT_NVCHAR, ADFI_989_NVCHAR_GE_NVCHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_NVCHR_TYPE, DB_NVCHR_TYPE, DB_NODT, DB_NODT
},
/*
** [44]:  boolean  :  all <= all
*/
{ ADFI_1352_NUMERIC_LE, ADFI_1351_NUMERIC_GT,
  ADI_COMPARISON, ADI_F8192_QUEL_CLOAKED, ADI_LE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_ALL_TYPE, DB_ALL_TYPE, DB_NODT, DB_NODT
},
/*
** [45]:  boolean  :  c <= c
*/
{ ADFI_016_C_LE_C, ADFI_024_C_GT_C,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_CHR_TYPE, DB_CHR_TYPE, DB_NODT, DB_NODT
},
/*
** [46]:  boolean  :  text <= text
*/
{ ADFI_017_TEXT_LE_TEXT, ADFI_025_TEXT_GT_TEXT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_TXT_TYPE, DB_TXT_TYPE, DB_NODT, DB_NODT
},
/*
** [47]:  boolean  :  char <= char
*/
{ ADFI_238_CHAR_LE_CHAR, ADFI_240_CHAR_GT_CHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_CHA_TYPE, DB_CHA_TYPE, DB_NODT, DB_NODT
},
/*
** [48]:  boolean  :  varchar <= varchar
*/
{ ADFI_239_VARCHAR_LE_VARCHAR, ADFI_241_VARCHAR_GT_VARCHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_VCH_TYPE, DB_VCH_TYPE, DB_NODT, DB_NODT
},
/*
** [49]:  boolean  :  date <= date
*/
{ ADFI_018_DATE_LE_DATE, ADFI_026_DATE_GT_DATE,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_DTE_TYPE, DB_DTE_TYPE, DB_NODT, DB_NODT
},
/*
** [50]:  boolean  :  f <= f
*/
{ ADFI_019_F_LE_F, ADFI_027_F_GT_F,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_FLT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [51]:  boolean  :  f <= i
*/
{ ADFI_020_F_LE_I, ADFI_028_F_GT_I,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_FLT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [52]:  boolean  :  dec <= dec
*/
{ ADFI_512_DEC_LE_DEC, ADFI_513_DEC_GT_DEC,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_DEC_TYPE, DB_DEC_TYPE, DB_NODT, DB_NODT
},
/*
** [53]:  boolean  :  i <= f
*/
{ ADFI_021_I_LE_F, ADFI_029_I_GT_F,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_INT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [54]:  boolean  :  i <= i
*/
{ ADFI_022_I_LE_I, ADFI_030_I_GT_I,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_INT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [55]:  boolean  :  money <= money
*/
{ ADFI_023_MONEY_LE_MONEY, ADFI_031_MONEY_GT_MONEY,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_MNY_TYPE, DB_MNY_TYPE, DB_NODT, DB_NODT
},
/*
** [56]:  boolean  :  table_key <= table_key
*/
{ ADFI_486_TABKEY_LE_TABKEY, ADFI_487_TABKEY_GT_TABKEY,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_TABKEY_TYPE, DB_TABKEY_TYPE, DB_NODT, DB_NODT
},
/*
** [57]:  boolean  :  logical_key <= logical_key
*/
{ ADFI_467_LOGKEY_LE_LOGKEY, ADFI_468_LOGKEY_GT_LOGKEY,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_LOGKEY_TYPE, DB_LOGKEY_TYPE, DB_NODT, DB_NODT
},
/*
** [58]:  boolean  :  bit <= bit
*/
{ ADFI_635_BIT_LE_BIT, ADFI_643_BIT_GT_BIT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_BIT_TYPE, DB_BIT_TYPE, DB_NODT, DB_NODT
},
/*
** [59]:  boolean  :  bit <= vbit
*/
{ ADFI_636_BIT_LE_VBIT, ADFI_644_BIT_GT_VBIT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_BIT_TYPE, DB_VBIT_TYPE, DB_NODT, DB_NODT
},
/*
** [60]:  boolean  :  vbit <= vbit
*/
{ ADFI_637_VBIT_LE_VBIT, ADFI_645_VBIT_GT_VBIT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_VBIT_TYPE, DB_VBIT_TYPE, DB_NODT, DB_NODT
},
/*
** [61]:  boolean  :  byte <= byte
*/
{ ADFI_719_BYTE_LE_BYTE, ADFI_723_BYTE_GT_BYTE,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_BYTE_TYPE, DB_BYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [62]:  boolean  :  vbyte <= vbyte
*/
{ ADFI_720_VBYTE_LE_VBYTE, ADFI_724_VBYTE_GT_VBYTE,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_VBYTE_TYPE, DB_VBYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [63]:  boolean  :  vbit <= bit
*/
{ ADFI_638_VBIT_LE_BIT, ADFI_646_VBIT_GT_BIT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_VBIT_TYPE, DB_BIT_TYPE, DB_NODT, DB_NODT
},
/*
** [64]:  boolean  :  nchar <= nchar
*/
{ ADFI_990_NCHAR_LE_NCHAR, ADFI_991_NCHAR_GT_NCHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_NCHR_TYPE, DB_NCHR_TYPE, DB_NODT, DB_NODT
},
/*
** [65]:  boolean  :  nvarchar <= nvarchar
*/
{ ADFI_992_NVCHAR_LE_NVCHAR, ADFI_993_NVCHAR_GT_NVCHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_NVCHR_TYPE, DB_NVCHR_TYPE, DB_NODT, DB_NODT
},
/*
** [66]:  boolean  :  all = all
*/
{ ADFI_1347_NUMERIC_EQ, ADFI_1348_NUMERIC_NE,
  ADI_COMPARISON, ADI_F8192_QUEL_CLOAKED, ADI_EQ_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_ALL_TYPE, DB_ALL_TYPE, DB_NODT, DB_NODT
},
/*
** [67]:  boolean  :  c = c
*/
{ ADFI_000_C_EQ_C, ADFI_008_C_NE_C,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_EQ_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_CHR_TYPE, DB_CHR_TYPE, DB_NODT, DB_NODT
},
/*
** [68]:  boolean  :  text = text
*/
{ ADFI_001_TEXT_EQ_TEXT, ADFI_009_TEXT_NE_TEXT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_EQ_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_TXT_TYPE, DB_TXT_TYPE, DB_NODT, DB_NODT
},
/*
** [69]:  boolean  :  char = char
*/
{ ADFI_234_CHAR_EQ_CHAR, ADFI_236_CHAR_NE_CHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_EQ_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_CHA_TYPE, DB_CHA_TYPE, DB_NODT, DB_NODT
},
/*
** [70]:  boolean  :  varchar = varchar
*/
{ ADFI_235_VARCHAR_EQ_VARCHAR, ADFI_237_VARCHAR_NE_VARCHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_EQ_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_VCH_TYPE, DB_VCH_TYPE, DB_NODT, DB_NODT
},
/*
** [71]:  boolean  :  date = date
*/
{ ADFI_002_DATE_EQ_DATE, ADFI_010_DATE_NE_DATE,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_EQ_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_DTE_TYPE, DB_DTE_TYPE, DB_NODT, DB_NODT
},
/*
** [72]:  boolean  :  f = f
*/
{ ADFI_003_F_EQ_F, ADFI_011_F_NE_F,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_EQ_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_FLT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [73]:  boolean  :  f = i
*/
{ ADFI_004_F_EQ_I, ADFI_012_F_NE_I,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_EQ_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_FLT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [74]:  boolean  :  dec = dec
*/
{ ADFI_510_DEC_EQ_DEC, ADFI_511_DEC_NE_DEC,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_EQ_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_DEC_TYPE, DB_DEC_TYPE, DB_NODT, DB_NODT
},
/*
** [75]:  boolean  :  i = f
*/
{ ADFI_005_I_EQ_F, ADFI_013_I_NE_F,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_EQ_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_INT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [76]:  boolean  :  i = i
*/
{ ADFI_006_I_EQ_I, ADFI_014_I_NE_I,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_EQ_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_INT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [77]:  boolean  :  money = money
*/
{ ADFI_007_MONEY_EQ_MONEY, ADFI_015_MONEY_NE_MONEY,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_EQ_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_MNY_TYPE, DB_MNY_TYPE, DB_NODT, DB_NODT
},
/*
** [78]:  boolean  :  logical_key = logical_key
*/
{ ADFI_503_LOGKEY_EQ_LOGKEY, ADFI_466_LOGKEY_NE_LOGKEY,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_EQ_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_LOGKEY_TYPE, DB_LOGKEY_TYPE, DB_NODT, DB_NODT
},
/*
** [79]:  boolean  :  table_key = table_key
*/
{ ADFI_484_TABKEY_EQ_TABKEY, ADFI_485_TABKEY_NE_TABKEY,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_EQ_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_TABKEY_TYPE, DB_TABKEY_TYPE, DB_NODT, DB_NODT
},
/*
** [80]:  boolean  :  bit = bit
*/
{ ADFI_639_BIT_EQ_BIT, ADFI_627_BIT_NE_BIT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_EQ_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_BIT_TYPE, DB_BIT_TYPE, DB_NODT, DB_NODT
},
/*
** [81]:  boolean  :  bit = vbit
*/
{ ADFI_640_BIT_EQ_VBIT, ADFI_628_BIT_NE_VBIT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_EQ_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_BIT_TYPE, DB_VBIT_TYPE, DB_NODT, DB_NODT
},
/*
** [82]:  boolean  :  vbit = vbit
*/
{ ADFI_641_VBIT_EQ_VBIT, ADFI_629_VBIT_NE_VBIT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_EQ_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_VBIT_TYPE, DB_VBIT_TYPE, DB_NODT, DB_NODT
},
/*
** [83]:  boolean  :  vbit = bit
*/
{ ADFI_642_VBIT_EQ_BIT, ADFI_630_VBIT_NE_BIT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_EQ_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_VBIT_TYPE, DB_BIT_TYPE, DB_NODT, DB_NODT
},
/*
** [84]:  boolean  :  byte = byte
*/
{ ADFI_721_BYTE_EQ_BYTE, ADFI_715_BYTE_NE_BYTE,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_EQ_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_BYTE_TYPE, DB_BYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [85]:  boolean  :  vbyte = vbyte
*/
{ ADFI_722_VBYTE_EQ_VBYTE, ADFI_716_VBYTE_NE_VBYTE,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_EQ_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_VBYTE_TYPE, DB_VBYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [86]:  boolean  :  nchar = nchar
*/
{ ADFI_983_NCHAR_EQ_NCHAR, ADFI_982_NCHAR_NE_NCHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_EQ_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_NCHR_TYPE, DB_NCHR_TYPE, DB_NODT, DB_NODT
},
/*
** [87]:  boolean  :  nvarchar = nvarchar
*/
{ ADFI_985_NVCHAR_EQ_NVCHAR, ADFI_984_NVCHAR_NE_NVCHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_EQ_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_NVCHR_TYPE, DB_NVCHR_TYPE, DB_NODT, DB_NODT
},
/*
** [88]:  boolean  :  longtext = longtext
*/
{ ADFI_1272_LTXT_EQ_LTXT, ADFI_1272_LTXT_EQ_LTXT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_EQ_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_LTXT_TYPE, DB_LTXT_TYPE, DB_NODT, DB_NODT
},
/*
** [89]:  boolean  :  all > all
*/
{ ADFI_1351_NUMERIC_GT, ADFI_1352_NUMERIC_LE,
  ADI_COMPARISON, ADI_F8192_QUEL_CLOAKED, ADI_GT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_ALL_TYPE, DB_ALL_TYPE, DB_NODT, DB_NODT
},
/*
** [90]:  boolean  :  c > c
*/
{ ADFI_024_C_GT_C, ADFI_016_C_LE_C,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_CHR_TYPE, DB_CHR_TYPE, DB_NODT, DB_NODT
},
/*
** [91]:  boolean  :  text > text
*/
{ ADFI_025_TEXT_GT_TEXT, ADFI_017_TEXT_LE_TEXT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_TXT_TYPE, DB_TXT_TYPE, DB_NODT, DB_NODT
},
/*
** [92]:  boolean  :  char > char
*/
{ ADFI_240_CHAR_GT_CHAR, ADFI_238_CHAR_LE_CHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_CHA_TYPE, DB_CHA_TYPE, DB_NODT, DB_NODT
},
/*
** [93]:  boolean  :  varchar > varchar
*/
{ ADFI_241_VARCHAR_GT_VARCHAR, ADFI_239_VARCHAR_LE_VARCHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_VCH_TYPE, DB_VCH_TYPE, DB_NODT, DB_NODT
},
/*
** [94]:  boolean  :  date > date
*/
{ ADFI_026_DATE_GT_DATE, ADFI_018_DATE_LE_DATE,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_DTE_TYPE, DB_DTE_TYPE, DB_NODT, DB_NODT
},
/*
** [95]:  boolean  :  f > f
*/
{ ADFI_027_F_GT_F, ADFI_019_F_LE_F,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_FLT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [96]:  boolean  :  f > i
*/
{ ADFI_028_F_GT_I, ADFI_020_F_LE_I,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_FLT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [97]:  boolean  :  dec > dec
*/
{ ADFI_513_DEC_GT_DEC, ADFI_512_DEC_LE_DEC,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_DEC_TYPE, DB_DEC_TYPE, DB_NODT, DB_NODT
},
/*
** [98]:  boolean  :  i > f
*/
{ ADFI_029_I_GT_F, ADFI_021_I_LE_F,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_INT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [99]:  boolean  :  i > i
*/
{ ADFI_030_I_GT_I, ADFI_022_I_LE_I,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_INT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [100]:  boolean  :  money > money
*/
{ ADFI_031_MONEY_GT_MONEY, ADFI_023_MONEY_LE_MONEY,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_MNY_TYPE, DB_MNY_TYPE, DB_NODT, DB_NODT
},
/*
** [101]:  boolean  :  table_key > table_key
*/
{ ADFI_487_TABKEY_GT_TABKEY, ADFI_486_TABKEY_LE_TABKEY,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_TABKEY_TYPE, DB_TABKEY_TYPE, DB_NODT, DB_NODT
},
/*
** [102]:  boolean  :  logical_key > logical_key
*/
{ ADFI_468_LOGKEY_GT_LOGKEY, ADFI_467_LOGKEY_LE_LOGKEY,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_LOGKEY_TYPE, DB_LOGKEY_TYPE, DB_NODT, DB_NODT
},
/*
** [103]:  boolean  :  bit > bit
*/
{ ADFI_643_BIT_GT_BIT, ADFI_635_BIT_LE_BIT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_BIT_TYPE, DB_BIT_TYPE, DB_NODT, DB_NODT
},
/*
** [104]:  boolean  :  bit > vbit
*/
{ ADFI_644_BIT_GT_VBIT, ADFI_636_BIT_LE_VBIT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_BIT_TYPE, DB_VBIT_TYPE, DB_NODT, DB_NODT
},
/*
** [105]:  boolean  :  vbit > vbit
*/
{ ADFI_645_VBIT_GT_VBIT, ADFI_637_VBIT_LE_VBIT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_VBIT_TYPE, DB_VBIT_TYPE, DB_NODT, DB_NODT
},
/*
** [106]:  boolean  :  vbit > bit
*/
{ ADFI_646_VBIT_GT_BIT, ADFI_638_VBIT_LE_BIT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_VBIT_TYPE, DB_BIT_TYPE, DB_NODT, DB_NODT
},
/*
** [107]:  boolean  :  byte > byte
*/
{ ADFI_723_BYTE_GT_BYTE, ADFI_719_BYTE_LE_BYTE,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_BYTE_TYPE, DB_BYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [108]:  boolean  :  vbyte > vbyte
*/
{ ADFI_724_VBYTE_GT_VBYTE, ADFI_720_VBYTE_LE_VBYTE,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_VBYTE_TYPE, DB_VBYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [109]:  boolean  :  nchar > nchar
*/
{ ADFI_991_NCHAR_GT_NCHAR, ADFI_990_NCHAR_LE_NCHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_NCHR_TYPE, DB_NCHR_TYPE, DB_NODT, DB_NODT
},
/*
** [110]:  boolean  :  nvarchar > nvarchar
*/
{ ADFI_993_NVCHAR_GT_NVCHAR, ADFI_992_NVCHAR_LE_NVCHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_NVCHR_TYPE, DB_NVCHR_TYPE, DB_NODT, DB_NODT
},
/*
** [111]:  boolean  :  all >= all
*/
{ ADFI_1350_NUMERIC_GE, ADFI_1349_NUMERIC_LT,
  ADI_COMPARISON, ADI_F8192_QUEL_CLOAKED, ADI_GE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_ALL_TYPE, DB_ALL_TYPE, DB_NODT, DB_NODT
},
/*
** [112]:  boolean  :  c >= c
*/
{ ADFI_032_C_GE_C, ADFI_040_C_LT_C,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_CHR_TYPE, DB_CHR_TYPE, DB_NODT, DB_NODT
},
/*
** [113]:  boolean  :  text >= text
*/
{ ADFI_033_TEXT_GE_TEXT, ADFI_041_TEXT_LT_TEXT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_TXT_TYPE, DB_TXT_TYPE, DB_NODT, DB_NODT
},
/*
** [114]:  boolean  :  char >= char
*/
{ ADFI_242_CHAR_GE_CHAR, ADFI_244_CHAR_LT_CHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_CHA_TYPE, DB_CHA_TYPE, DB_NODT, DB_NODT
},
/*
** [115]:  boolean  :  varchar >= varchar
*/
{ ADFI_243_VARCHAR_GE_VARCHAR, ADFI_245_VARCHAR_LT_VARCHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_VCH_TYPE, DB_VCH_TYPE, DB_NODT, DB_NODT
},
/*
** [116]:  boolean  :  date >= date
*/
{ ADFI_034_DATE_GE_DATE, ADFI_042_DATE_LT_DATE,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_DTE_TYPE, DB_DTE_TYPE, DB_NODT, DB_NODT
},
/*
** [117]:  boolean  :  f >= f
*/
{ ADFI_035_F_GE_F, ADFI_043_F_LT_F,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_FLT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [118]:  boolean  :  f >= i
*/
{ ADFI_036_F_GE_I, ADFI_044_F_LT_I,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_FLT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [119]:  boolean  :  i >= f
*/
{ ADFI_037_I_GE_F, ADFI_045_I_LT_F,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_INT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [120]:  boolean  :  dec >= dec
*/
{ ADFI_514_DEC_GE_DEC, ADFI_515_DEC_LT_DEC,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_DEC_TYPE, DB_DEC_TYPE, DB_NODT, DB_NODT
},
/*
** [121]:  boolean  :  i >= i
*/
{ ADFI_038_I_GE_I, ADFI_046_I_LT_I,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_INT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [122]:  boolean  :  money >= money
*/
{ ADFI_039_MONEY_GE_MONEY, ADFI_047_MONEY_LT_MONEY,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_MNY_TYPE, DB_MNY_TYPE, DB_NODT, DB_NODT
},
/*
** [123]:  boolean  :  logical_key >= logical_key
*/
{ ADFI_469_LOGKEY_GE_LOGKEY, ADFI_470_LOGKEY_LT_LOGKEY,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_LOGKEY_TYPE, DB_LOGKEY_TYPE, DB_NODT, DB_NODT
},
/*
** [124]:  boolean  :  table_key >= table_key
*/
{ ADFI_488_TABKEY_GE_TABKEY, ADFI_489_TABKEY_LT_TABKEY,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_TABKEY_TYPE, DB_TABKEY_TYPE, DB_NODT, DB_NODT
},
/*
** [125]:  boolean  :  bit >= bit
*/
{ ADFI_647_BIT_GE_BIT, ADFI_631_BIT_LT_BIT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_BIT_TYPE, DB_BIT_TYPE, DB_NODT, DB_NODT
},
/*
** [126]:  boolean  :  bit >= vbit
*/
{ ADFI_648_BIT_GE_VBIT, ADFI_632_BIT_LT_VBIT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_BIT_TYPE, DB_VBIT_TYPE, DB_NODT, DB_NODT
},
/*
** [127]:  boolean  :  vbit >= vbit
*/
{ ADFI_649_VBIT_GE_VBIT, ADFI_633_VBIT_LT_VBIT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_VBIT_TYPE, DB_VBIT_TYPE, DB_NODT, DB_NODT
},
/*
** [128]:  boolean  :  vbit >= bit
*/
{ ADFI_650_VBIT_GE_BIT, ADFI_634_VBIT_LT_BIT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_VBIT_TYPE, DB_BIT_TYPE, DB_NODT, DB_NODT
},
/*
** [129]:  boolean  :  byte >= byte
*/
{ ADFI_725_BYTE_GE_BYTE, ADFI_717_BYTE_LT_BYTE,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_BYTE_TYPE, DB_BYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [130]:  boolean  :  vbyte >= vbyte
*/
{ ADFI_726_VBYTE_GE_VBYTE, ADFI_718_VBYTE_LT_VBYTE,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_VBYTE_TYPE, DB_VBYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [131]:  boolean  :  nchar >= nchar
*/
{ ADFI_987_NCHAR_GE_NCHAR, ADFI_986_NCHAR_LT_NCHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_NCHR_TYPE, DB_NCHR_TYPE, DB_NODT, DB_NODT
},
/*
** [132]:  boolean  :  nvarchar >= nvarchar
*/
{ ADFI_989_NVCHAR_GE_NVCHAR, ADFI_988_NVCHAR_LT_NVCHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_GE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_NVCHR_TYPE, DB_NVCHR_TYPE, DB_NODT, DB_NODT
},
/*
** [133]:  boolean  :  all isnull
*/
{ ADFI_516_ALL_ISNULL, ADFI_517_ALL_ISNOTNULL,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_ISNUL_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_0CXI_IGNORE,
  1, DB_BOO_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [134]:  boolean  :  i isnull
*/
{ ADFI_380_I_ISNULL, ADFI_388_I_ISNOTNULL,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_ISNUL_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_0CXI_IGNORE,
  1, DB_BOO_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [135]:  boolean  :  f isnull
*/
{ ADFI_381_F_ISNULL, ADFI_389_F_ISNOTNULL,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_ISNUL_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_0CXI_IGNORE,
  1, DB_BOO_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [136]:  boolean  :  money isnull
*/
{ ADFI_382_MONEY_ISNULL, ADFI_390_MONEY_ISNOTNULL,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_ISNUL_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_0CXI_IGNORE,
  1, DB_BOO_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [137]:  boolean  :  date isnull
*/
{ ADFI_383_DATE_ISNULL, ADFI_391_DATE_ISNOTNULL,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_ISNUL_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_0CXI_IGNORE,
  1, DB_BOO_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [138]:  boolean  :  c isnull
*/
{ ADFI_384_C_ISNULL, ADFI_392_C_ISNOTNULL,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_ISNUL_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_0CXI_IGNORE,
  1, DB_BOO_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [139]:  boolean  :  char isnull
*/
{ ADFI_385_CHAR_ISNULL, ADFI_393_CHAR_ISNOTNULL,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_ISNUL_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_0CXI_IGNORE,
  1, DB_BOO_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [140]:  boolean  :  text isnull
*/
{ ADFI_386_TEXT_ISNULL, ADFI_394_TEXT_ISNOTNULL,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_ISNUL_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_0CXI_IGNORE,
  1, DB_BOO_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [141]:  boolean  :  varchar isnull
*/
{ ADFI_387_VARCHAR_ISNULL, ADFI_395_VARCHAR_ISNOTNULL,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_ISNUL_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_0CXI_IGNORE,
  1, DB_BOO_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [142]:  boolean  :  table_key isnull
*/
{ ADFI_490_TABKEY_ISNULL, ADFI_491_TABKEY_ISNOTNULL,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_ISNUL_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_0CXI_IGNORE,
  1, DB_BOO_TYPE,
    DB_TABKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [143]:  boolean  :  logical_key isnull
*/
{ ADFI_471_LOGKEY_ISNULL, ADFI_472_LOGKEY_ISNOTNULL,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_ISNUL_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_0CXI_IGNORE,
  1, DB_BOO_TYPE,
    DB_LOGKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [144]:  boolean  :  nchar isnull
*/
{ ADFI_994_NCHAR_ISNULL, ADFI_995_NCHAR_ISNOTNULL,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_ISNUL_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_0CXI_IGNORE,
  1, DB_BOO_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [145]:  boolean  :  nvarchar isnull
*/
{ ADFI_996_NVCHAR_ISNULL, ADFI_997_NVCHAR_ISNOTNULL,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_ISNUL_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_0CXI_IGNORE,
  1, DB_BOO_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [146]:  boolean  :  all isnotnull
*/
{ ADFI_517_ALL_ISNOTNULL, ADFI_516_ALL_ISNULL,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NONUL_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_0CXI_IGNORE,
  1, DB_BOO_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [147]:  boolean  :  i isnotnull
*/
{ ADFI_388_I_ISNOTNULL, ADFI_380_I_ISNULL,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NONUL_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_0CXI_IGNORE,
  1, DB_BOO_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [148]:  boolean  :  f isnotnull
*/
{ ADFI_389_F_ISNOTNULL, ADFI_381_F_ISNULL,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NONUL_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_0CXI_IGNORE,
  1, DB_BOO_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [149]:  boolean  :  money isnotnull
*/
{ ADFI_390_MONEY_ISNOTNULL, ADFI_382_MONEY_ISNULL,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NONUL_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_0CXI_IGNORE,
  1, DB_BOO_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [150]:  boolean  :  date isnotnull
*/
{ ADFI_391_DATE_ISNOTNULL, ADFI_383_DATE_ISNULL,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NONUL_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_0CXI_IGNORE,
  1, DB_BOO_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [151]:  boolean  :  c isnotnull
*/
{ ADFI_392_C_ISNOTNULL, ADFI_384_C_ISNULL,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NONUL_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_0CXI_IGNORE,
  1, DB_BOO_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [152]:  boolean  :  char isnotnull
*/
{ ADFI_393_CHAR_ISNOTNULL, ADFI_385_CHAR_ISNULL,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NONUL_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_0CXI_IGNORE,
  1, DB_BOO_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [153]:  boolean  :  text isnotnull
*/
{ ADFI_394_TEXT_ISNOTNULL, ADFI_386_TEXT_ISNULL,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NONUL_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_0CXI_IGNORE,
  1, DB_BOO_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [154]:  boolean  :  varchar isnotnull
*/
{ ADFI_395_VARCHAR_ISNOTNULL, ADFI_387_VARCHAR_ISNULL,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NONUL_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_0CXI_IGNORE,
  1, DB_BOO_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [155]:  boolean  :  logical_key isnotnull
*/
{ ADFI_472_LOGKEY_ISNOTNULL, ADFI_471_LOGKEY_ISNULL,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NONUL_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_0CXI_IGNORE,
  1, DB_BOO_TYPE,
    DB_LOGKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [156]:  boolean  :  table_key isnotnull
*/
{ ADFI_491_TABKEY_ISNOTNULL, ADFI_490_TABKEY_ISNULL,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NONUL_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_0CXI_IGNORE,
  1, DB_BOO_TYPE,
    DB_TABKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [157]:  boolean  :  nchar isnotnull
*/
{ ADFI_995_NCHAR_ISNOTNULL, ADFI_994_NCHAR_ISNULL,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NONUL_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_0CXI_IGNORE,
  1, DB_BOO_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [158]:  boolean  :  nvarchar isnotnull
*/
{ ADFI_997_NVCHAR_ISNOTNULL, ADFI_996_NVCHAR_ISNULL,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NONUL_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_0CXI_IGNORE,
  1, DB_BOO_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [159]:  boolean  :  c like c
*/
{ ADFI_246_C_LIKE_C, ADFI_250_C_NOTLIKE_C,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LIKE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_CHR_TYPE, DB_CHR_TYPE, DB_NODT, DB_NODT
},
/*
** [160]:  boolean  :  char like char
*/
{ ADFI_247_CHAR_LIKE_CHAR, ADFI_251_CHAR_NOTLIKE_CHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LIKE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_CHA_TYPE, DB_CHA_TYPE, DB_NODT, DB_NODT
},
/*
** [161]:  boolean  :  text like text
*/
{ ADFI_248_TEXT_LIKE_TEXT, ADFI_252_TEXT_NOTLIKE_TEXT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LIKE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_TXT_TYPE, DB_TXT_TYPE, DB_NODT, DB_NODT
},
/*
** [162]:  boolean  :  varchar like varchar
*/
{ ADFI_249_VARCHAR_LIKE_VARCHAR, ADFI_253_VARCHAR_NOTLIKE_VARCHA,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LIKE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_VCH_TYPE, DB_VCH_TYPE, DB_NODT, DB_NODT
},
/*
** [163]:  boolean  :  nvarchar like nvarchar
*/
{ ADFI_1000_NVCHAR_LIKE_NVCHAR, ADFI_1001_NVCHAR_NOTLIKE_NVCHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_LIKE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_NVCHR_TYPE, DB_NVCHR_TYPE, DB_NODT, DB_NODT
},
/*
** [164]:  boolean  :  c notlike c
*/
{ ADFI_250_C_NOTLIKE_C, ADFI_246_C_LIKE_C,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NLIKE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_CHR_TYPE, DB_CHR_TYPE, DB_NODT, DB_NODT
},
/*
** [165]:  boolean  :  char notlike char
*/
{ ADFI_251_CHAR_NOTLIKE_CHAR, ADFI_247_CHAR_LIKE_CHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NLIKE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_CHA_TYPE, DB_CHA_TYPE, DB_NODT, DB_NODT
},
/*
** [166]:  boolean  :  text notlike text
*/
{ ADFI_252_TEXT_NOTLIKE_TEXT, ADFI_248_TEXT_LIKE_TEXT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NLIKE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_TXT_TYPE, DB_TXT_TYPE, DB_NODT, DB_NODT
},
/*
** [167]:  boolean  :  varchar notlike varchar
*/
{ ADFI_253_VARCHAR_NOTLIKE_VARCHA, ADFI_249_VARCHAR_LIKE_VARCHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NLIKE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_VCH_TYPE, DB_VCH_TYPE, DB_NODT, DB_NODT
},
/*
** [168]:  boolean  :  nvarchar notlike nvarchar
*/
{ ADFI_1001_NVCHAR_NOTLIKE_NVCHAR, ADFI_1000_NVCHAR_LIKE_NVCHAR,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NLIKE_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  2, DB_BOO_TYPE,
    DB_NVCHR_TYPE, DB_NVCHR_TYPE, DB_NODT, DB_NODT
},
/*
** [169]:  boolean  :  all is integer
*/
{ ADFI_1357_ALL_ISINT, ADFI_1358_ALL_ISNOTINT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_ISINT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  1, DB_BOO_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [170]:  boolean  :  all is integer
*/
{ ADFI_1358_ALL_ISNOTINT, ADFI_1357_ALL_ISINT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NOINT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  1, DB_BOO_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [171]:  boolean  :  all is decimal
*/
{ ADFI_1359_ALL_ISDEC, ADFI_1360_ALL_ISNOTDEC,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_ISDEC_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  1, DB_BOO_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [172]:  boolean  :  all is decimal
*/
{ ADFI_1360_ALL_ISNOTDEC, ADFI_1359_ALL_ISDEC,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NODEC_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  1, DB_BOO_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [173]:  boolean  :  ALL is float
*/
{ ADFI_1361_ALL_ISFLT, ADFI_1362_ALL_ISNOTFLT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_ISFLT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  1, DB_BOO_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [174]:  boolean  :  ALL is float
*/
{ ADFI_1362_ALL_ISNOTFLT, ADFI_1361_ALL_ISFLT,
  ADI_COMPARISON, ADI_F0_NOFIFLAGS, ADI_NOFLT_OP,
  {ADI_FIXED, sizeof(bool), 0}, 0, ADE_3CXI_CMP_SET_SKIP,
  1, DB_BOO_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [175]:  f  :  f * f
*/
{ ADFI_059_F_MUL_F, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_MUL_OP,
  {ADI_SHORTER, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [176]:  f  :  f * dec
*/
{ ADFI_521_F_MUL_DEC, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_MUL_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_DEC_TYPE, DB_NODT, DB_NODT
},
/*
** [177]:  f  :  f * i
*/
{ ADFI_410_F_MUL_I, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_MUL_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [178]:  dec  :  dec * dec
*/
{ ADFI_520_DEC_MUL_DEC, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_MUL_OP,
  {ADI_DECMUL, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DEC_TYPE,
    DB_DEC_TYPE, DB_DEC_TYPE, DB_NODT, DB_NODT
},
/*
** [179]:  f  :  dec * f
*/
{ ADFI_522_DEC_MUL_F, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_MUL_OP,
  {ADI_O2, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_FLT_TYPE,
    DB_DEC_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [180]:  dec  :  dec * i
*/
{ ADFI_524_DEC_MUL_I, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_MUL_OP,
  {ADI_DECIMUL, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DEC_TYPE,
    DB_DEC_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [181]:  f  :  i * f
*/
{ ADFI_411_I_MUL_F, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_MUL_OP,
  {ADI_O2, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_FLT_TYPE,
    DB_INT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [182]:  dec  :  i * dec
*/
{ ADFI_523_I_MUL_DEC, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_MUL_OP,
  {ADI_IDECMUL, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DEC_TYPE,
    DB_INT_TYPE, DB_DEC_TYPE, DB_NODT, DB_NODT
},
/*
** [183]:  i  :  i * i
*/
{ ADFI_060_I_MUL_I, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_MUL_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_INT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [184]:  money  :  money * money
*/
{ ADFI_065_MONEY_MUL_MONEY, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_MUL_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_MNY_TYPE,
    DB_MNY_TYPE, DB_MNY_TYPE, DB_NODT, DB_NODT
},
/*
** [185]:  money  :  f * money
*/
{ ADFI_061_F_MUL_MONEY, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_MUL_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_MNY_TYPE,
    DB_FLT_TYPE, DB_MNY_TYPE, DB_NODT, DB_NODT
},
/*
** [186]:  money  :  i * money
*/
{ ADFI_062_I_MUL_MONEY, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_MUL_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_MNY_TYPE,
    DB_INT_TYPE, DB_MNY_TYPE, DB_NODT, DB_NODT
},
/*
** [187]:  money  :  dec * money
*/
{ ADFI_784_DEC_MUL_MONEY, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_MUL_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_MNY_TYPE,
    DB_DEC_TYPE, DB_MNY_TYPE, DB_NODT, DB_NODT
},
/*
** [188]:  money  :  money * f
*/
{ ADFI_063_MONEY_MUL_F, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_MUL_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_MNY_TYPE,
    DB_MNY_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [189]:  money  :  money * i
*/
{ ADFI_064_MONEY_MUL_I, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_MUL_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_MNY_TYPE,
    DB_MNY_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [190]:  money  :  money * dec
*/
{ ADFI_785_MONEY_MUL_DEC, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_MUL_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_MNY_TYPE,
    DB_MNY_TYPE, DB_DEC_TYPE, DB_NODT, DB_NODT
},
/*
** [191]:  dte  :  dte * f
*/
{ ADFI_1364_DTE_MUL_F, ADI_NOFI,
  ADI_OPERATOR, ADI_F512_RES1STARG, ADI_MUL_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DTE_TYPE,
    DB_DTE_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [192]:  dte  :  f * dte
*/
{ ADFI_1365_F_MUL_DTE, ADI_NOFI,
  ADI_OPERATOR, ADI_F1024_RES2NDARG, ADI_MUL_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DTE_TYPE,
    DB_FLT_TYPE, DB_DTE_TYPE, DB_NODT, DB_NODT
},
/*
** [193]:  f  :  f ** f
*/
{ ADFI_073_F_POW_F, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_POW_OP,
  {ADI_LONGER, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [194]:  f  :  f ** i
*/
{ ADFI_074_F_POW_I, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_POW_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [195]:  date  :  date + interval_ytom
*/
{ ADFI_1274_DATE_ADD_INYM, ADI_NOFI,
  ADI_OPERATOR, ADI_F512_RES1STARG, ADI_ADD_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DTE_TYPE,
    DB_DTE_TYPE, DB_INYM_TYPE, DB_NODT, DB_NODT
},
/*
** [196]:  date  :  date + interval_dtos
*/
{ ADFI_1275_DATE_ADD_INDS, ADI_NOFI,
  ADI_OPERATOR, ADI_F512_RES1STARG, ADI_ADD_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DTE_TYPE,
    DB_DTE_TYPE, DB_INDS_TYPE, DB_NODT, DB_NODT
},
/*
** [197]:  date  :  interval_ytom + date
*/
{ ADFI_1276_INYM_ADD_DATE, ADI_NOFI,
  ADI_OPERATOR, ADI_F1024_RES2NDARG, ADI_ADD_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DTE_TYPE,
    DB_INYM_TYPE, DB_DTE_TYPE, DB_NODT, DB_NODT
},
/*
** [198]:  date  :  interval_dtos + date
*/
{ ADFI_1277_INDS_ADD_DATE, ADI_NOFI,
  ADI_OPERATOR, ADI_F1024_RES2NDARG, ADI_ADD_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DTE_TYPE,
    DB_INDS_TYPE, DB_DTE_TYPE, DB_NODT, DB_NODT
},
/*
** [199]:  date  :  date + date
*/
{ ADFI_048_DATE_ADD_DATE, ADI_NOFI,
  ADI_OPERATOR, ADI_F512_RES1STARG, ADI_ADD_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DTE_TYPE,
    DB_DTE_TYPE, DB_DTE_TYPE, DB_NODT, DB_NODT
},
/*
** [200]:  f  :  f + f
*/
{ ADFI_049_F_ADD_F, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_ADD_OP,
  {ADI_SHORTER, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [201]:  dec  :  dec + dec
*/
{ ADFI_518_DEC_ADD_DEC, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_ADD_OP,
  {ADI_DECADD, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DEC_TYPE,
    DB_DEC_TYPE, DB_DEC_TYPE, DB_NODT, DB_NODT
},
/*
** [202]:  i  :  i + i
*/
{ ADFI_050_I_ADD_I, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_ADD_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_INT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [203]:  money  :  money + money
*/
{ ADFI_051_MONEY_ADD_MONEY, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_ADD_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_MNY_TYPE,
    DB_MNY_TYPE, DB_MNY_TYPE, DB_NODT, DB_NODT
},
/*
** [204]:  c  :  c + c
*/
{ ADFI_052_C_ADD_C, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_ADD_OP,
  {ADI_CSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHR_TYPE,
    DB_CHR_TYPE, DB_CHR_TYPE, DB_NODT, DB_NODT
},
/*
** [205]:  c  :  c + char
*/
{ ADFI_1316_C_ADD_CHAR, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_ADD_OP,
  {ADI_CSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHR_TYPE,
    DB_CHR_TYPE, DB_CHA_TYPE, DB_NODT, DB_NODT
},
/*
** [206]:  c  :  char + c
*/
{ ADFI_1317_CHAR_ADD_C, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_ADD_OP,
  {ADI_CSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHR_TYPE,
    DB_CHA_TYPE, DB_CHR_TYPE, DB_NODT, DB_NODT
},
/*
** [207]:  c  :  c + varchar
*/
{ ADFI_1318_C_ADD_VARCHAR, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_ADD_OP,
  {ADI_CTSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHR_TYPE,
    DB_CHR_TYPE, DB_VCH_TYPE, DB_NODT, DB_NODT
},
/*
** [208]:  c  :  text + c
*/
{ ADFI_053_TEXT_ADD_C, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_ADD_OP,
  {ADI_CTSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHR_TYPE,
    DB_TXT_TYPE, DB_CHR_TYPE, DB_NODT, DB_NODT
},
/*
** [209]:  text  :  text + text
*/
{ ADFI_054_TEXT_ADD_TEXT, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_ADD_OP,
  {ADI_TSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_TXT_TYPE,
    DB_TXT_TYPE, DB_TXT_TYPE, DB_NODT, DB_NODT
},
/*
** [210]:  text  :  text + char
*/
{ ADFI_448_TEXT_ADD_CHAR, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_ADD_OP,
  {ADI_SUMV, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_TXT_TYPE,
    DB_TXT_TYPE, DB_CHA_TYPE, DB_NODT, DB_NODT
},
/*
** [211]:  text  :  text + varchar
*/
{ ADFI_449_TEXT_ADD_VARCHAR, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_ADD_OP,
  {ADI_TSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_TXT_TYPE,
    DB_TXT_TYPE, DB_VCH_TYPE, DB_NODT, DB_NODT
},
/*
** [212]:  char  :  char + char
*/
{ ADFI_254_CHAR_ADD_CHAR, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_ADD_OP,
  {ADI_CSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHA_TYPE,
    DB_CHA_TYPE, DB_CHA_TYPE, DB_NODT, DB_NODT
},
/*
** [213]:  char  :  char + varchar
*/
{ ADFI_452_CHAR_ADD_VARCHAR, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_ADD_OP,
  {ADI_CTSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHA_TYPE,
    DB_CHA_TYPE, DB_VCH_TYPE, DB_NODT, DB_NODT
},
/*
** [214]:  c  :  varchar + c
*/
{ ADFI_255_VARCHAR_ADD_C, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_ADD_OP,
  {ADI_CTSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHR_TYPE,
    DB_VCH_TYPE, DB_CHR_TYPE, DB_NODT, DB_NODT
},
/*
** [215]:  text  :  varchar + text
*/
{ ADFI_453_VARCHAR_ADD_TEXT, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_ADD_OP,
  {ADI_TSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_TXT_TYPE,
    DB_VCH_TYPE, DB_TXT_TYPE, DB_NODT, DB_NODT
},
/*
** [216]:  char  :  varchar + char
*/
{ ADFI_256_VARCHAR_ADD_CHAR, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_ADD_OP,
  {ADI_CTSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHA_TYPE,
    DB_VCH_TYPE, DB_CHA_TYPE, DB_NODT, DB_NODT
},
/*
** [217]:  varchar  :  varchar + varchar
*/
{ ADFI_257_VARCHAR_ADD_VARCHAR, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_ADD_OP,
  {ADI_TSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_VCH_TYPE, DB_NODT, DB_NODT
},
/*
** [218]:  lvch  :  lvch + lvch
*/
{ ADFI_714_LVCH_ADD_LVCH, ADI_NOFI,
  ADI_OPERATOR, ADI_F4_WORKSPACE, ADI_ADD_OP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  2, DB_LVCH_TYPE,
    DB_LVCH_TYPE, DB_LVCH_TYPE, DB_NODT, DB_NODT
},
/*
** [219]:  byte  :  byte + byte
*/
{ ADFI_727_BYTE_ADD_BYTE, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_ADD_OP,
  {ADI_CSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_BYTE_TYPE,
    DB_BYTE_TYPE, DB_BYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [220]:  vbyte  :  vbyte + vbyte
*/
{ ADFI_728_VBYTE_ADD_VBYTE, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_ADD_OP,
  {ADI_TSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VBYTE_TYPE,
    DB_VBYTE_TYPE, DB_VBYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [221]:  lbyte  :  lbyte + lbyte
*/
{ ADFI_729_LBYTE_ADD_LBYTE, ADI_NOFI,
  ADI_OPERATOR, ADI_F4_WORKSPACE, ADI_ADD_OP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  2, DB_LBYTE_TYPE,
    DB_LBYTE_TYPE, DB_LBYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [222]:  byte  :  byte + vbyte
*/
{ ADFI_764_BYTE_ADD_VBYTE, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_ADD_OP,
  {ADI_CTSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_BYTE_TYPE,
    DB_BYTE_TYPE, DB_VBYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [223]:  byte  :  vbyte + byte
*/
{ ADFI_765_VBYTE_ADD_BYTE, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_ADD_OP,
  {ADI_CTSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_BYTE_TYPE,
    DB_VBYTE_TYPE, DB_BYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [224]:  nchar  :  nchar + nchar
*/
{ ADFI_1076_NCHAR_ADD_NCHAR, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_ADD_OP,
  {ADI_CSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_NCHR_TYPE,
    DB_NCHR_TYPE, DB_NCHR_TYPE, DB_NODT, DB_NODT
},
/*
** [225]:  nvarchar  :  nvarchar + nvarchar
*/
{ ADFI_1077_NVARCHAR_ADD_NVARCHAR, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_ADD_OP,
  {ADI_TSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_NVCHR_TYPE,
    DB_NVCHR_TYPE, DB_NVCHR_TYPE, DB_NODT, DB_NODT
},
/*
** [226]:  interval_dtos :  time_wo_tz - time_wo_tz 
*/
{ ADFI_1264_TMWO_SUB_TMWO, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_SUB_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INDS_TYPE,
    DB_TMWO_TYPE, DB_TMWO_TYPE, DB_NODT, DB_NODT
},
/*
** [227]:  interval_dtos :  time_w_tz - time_w_tz 
*/
{ ADFI_1265_TMW_SUB_TMW, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_SUB_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INDS_TYPE,
    DB_TMW_TYPE, DB_TMW_TYPE, DB_NODT, DB_NODT
},
/*
** [228]:  interval_dtos :  time_local - time_local 
*/
{ ADFI_1266_TME_SUB_TME, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_SUB_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INDS_TYPE,
    DB_TME_TYPE, DB_TME_TYPE, DB_NODT, DB_NODT
},
/*
** [229]:  interval_dtos :  timestamp_wo_tz - timestamp_wo_tz 
*/
{ ADFI_1267_TSWO_SUB_TSWO, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_SUB_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INDS_TYPE,
    DB_TSWO_TYPE, DB_TSWO_TYPE, DB_NODT, DB_NODT
},
/*
** [230]:  interval_dtos :  timestamp_w_tz - timestamp_w_tz 
*/
{ ADFI_1268_TSW_SUB_TSW, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_SUB_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INDS_TYPE,
    DB_TSW_TYPE, DB_TSW_TYPE, DB_NODT, DB_NODT
},
/*
** [231]:  interval_dtos :  timestamp_local - timestamp_local 
*/
{ ADFI_1269_TSTMP_SUB_TSTMP, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_SUB_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INDS_TYPE,
    DB_TSTMP_TYPE, DB_TSTMP_TYPE, DB_NODT, DB_NODT
},
/*
** [232]:  interval_ytom :  ansidate - ansidate 
*/
{ ADFI_1270_ANSIDATE_SUB_ANSIDATE, ADI_NOFI,
  ADI_OPERATOR, ADI_F512_RES1STARG, ADI_SUB_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INDS_TYPE,
    DB_ADTE_TYPE, DB_ADTE_TYPE, DB_NODT, DB_NODT
},
/*
** [233]:  date  :  date - date
*/
{ ADFI_055_DATE_SUB_DATE, ADI_NOFI,
  ADI_OPERATOR, ADI_F512_RES1STARG, ADI_SUB_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DTE_TYPE,
    DB_DTE_TYPE, DB_DTE_TYPE, DB_NODT, DB_NODT
},
/*
** [234]:  f  :  f - f
*/
{ ADFI_056_F_SUB_F, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_SUB_OP,
  {ADI_SHORTER, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [235]:  dec  :  dec - dec
*/
{ ADFI_519_DEC_SUB_DEC, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_SUB_OP,
  {ADI_DECADD, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DEC_TYPE,
    DB_DEC_TYPE, DB_DEC_TYPE, DB_NODT, DB_NODT
},
/*
** [236]:  i  :  i - i
*/
{ ADFI_057_I_SUB_I, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_SUB_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_INT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [237]:  money  :  money - money
*/
{ ADFI_058_MONEY_SUB_MONEY, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_SUB_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_MNY_TYPE,
    DB_MNY_TYPE, DB_MNY_TYPE, DB_NODT, DB_NODT
},
/*
** [238]:  f  :  f / f
*/
{ ADFI_066_F_DIV_F, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_DIV_OP,
  {ADI_SHORTER, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [239]:  f  :  f / dec
*/
{ ADFI_526_F_DIV_DEC, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_DIV_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_DEC_TYPE, DB_NODT, DB_NODT
},
/*
** [240]:  f  :  f / i
*/
{ ADFI_412_F_DIV_I, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_DIV_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [241]:  f  :  i / f
*/
{ ADFI_413_I_DIV_F, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_DIV_OP,
  {ADI_O2, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_FLT_TYPE,
    DB_INT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [242]:  f  :  dec / f
*/
{ ADFI_527_DEC_DIV_F, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_DIV_OP,
  {ADI_O2, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_FLT_TYPE,
    DB_DEC_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [243]:  dec  :  dec / dec
*/
{ ADFI_525_DEC_DIV_DEC, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_DIV_OP,
  {ADI_DECDIV, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DEC_TYPE,
    DB_DEC_TYPE, DB_DEC_TYPE, DB_NODT, DB_NODT
},
/*
** [244]:  dec  :  dec / i
*/
{ ADFI_529_DEC_DIV_I, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_DIV_OP,
  {ADI_DECIDIV, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DEC_TYPE,
    DB_DEC_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [245]:  dec  :  i / dec
*/
{ ADFI_528_I_DIV_DEC, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_DIV_OP,
  {ADI_IDECDIV, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DEC_TYPE,
    DB_INT_TYPE, DB_DEC_TYPE, DB_NODT, DB_NODT
},
/*
** [246]:  i  :  i / i
*/
{ ADFI_067_I_DIV_I, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_DIV_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_INT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [247]:  money  :  money / money
*/
{ ADFI_072_MONEY_DIV_MONEY, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_DIV_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_MNY_TYPE,
    DB_MNY_TYPE, DB_MNY_TYPE, DB_NODT, DB_NODT
},
/*
** [248]:  f  :  f / money
*/
{ ADFI_068_F_DIV_MONEY, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_DIV_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_MNY_TYPE, DB_NODT, DB_NODT
},
/*
** [249]:  f  :  i / money
*/
{ ADFI_069_I_DIV_MONEY, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_DIV_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_FLT_TYPE,
    DB_INT_TYPE, DB_MNY_TYPE, DB_NODT, DB_NODT
},
/*
** [250]:  f  :  dec / money
*/
{ ADFI_786_DEC_DIV_MONEY, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_DIV_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_FLT_TYPE,
    DB_DEC_TYPE, DB_MNY_TYPE, DB_NODT, DB_NODT
},
/*
** [251]:  money  :  money / f
*/
{ ADFI_070_MONEY_DIV_F, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_DIV_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_MNY_TYPE,
    DB_MNY_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [252]:  money  :  money / i
*/
{ ADFI_071_MONEY_DIV_I, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_DIV_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_MNY_TYPE,
    DB_MNY_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [253]:  money  :  money / dec
*/
{ ADFI_787_MONEY_DIV_DEC, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_DIV_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_MNY_TYPE,
    DB_MNY_TYPE, DB_DEC_TYPE, DB_NODT, DB_NODT
},
/*
** [254]:  dte  :  dte / f
*/
{ ADFI_1366_DTE_DIV_F, ADI_NOFI,
  ADI_OPERATOR, ADI_F512_RES1STARG, ADI_DIV_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DTE_TYPE,
    DB_DTE_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [255]:  f  : dte / dte
*/
{ ADFI_1367_DTE_DIV_DTE, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_DIV_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_FLT_TYPE,
    DB_DTE_TYPE, DB_DTE_TYPE, DB_NODT, DB_NODT
},
/*
** [256]:  f  :  + f
*/
{ ADFI_075_PLUS_F, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_PLUS_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [257]:  dec  :  + dec
*/
{ ADFI_530_PLUS_DEC, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_PLUS_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DEC_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [258]:  i  :  + i
*/
{ ADFI_076_PLUS_I, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_PLUS_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [259]:  money  :  + money
*/
{ ADFI_077_PLUS_MONEY, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_PLUS_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_MNY_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [260]:  dte  :  + dte
*/
{ ADFI_1369_PLUS_DTE, ADI_NOFI,
  ADI_OPERATOR, ADI_F512_RES1STARG, ADI_PLUS_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DTE_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [261]:  f  :  - f
*/
{ ADFI_078_MINUS_F, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_MINUS_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [262]:  dec  :  - dec
*/
{ ADFI_531_MINUS_DEC, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_MINUS_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DEC_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [263]:  i  :  - i
*/
{ ADFI_079_MINUS_I, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_MINUS_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [264]:  money  :  - money
*/
{ ADFI_080_MINUS_MONEY, ADI_NOFI,
  ADI_OPERATOR, ADI_F0_NOFIFLAGS, ADI_MINUS_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_MNY_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [265]:  dte  :  - dte
*/
{ ADFI_1368_MINUS_DTE, ADI_NOFI,
  ADI_OPERATOR, ADI_F512_RES1STARG, ADI_MINUS_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DTE_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [266]:  i  :  any(c) (or any other type)
*/
{ ADFI_081_ANY_C_ALL, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_ANY_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_2CXI_SKIP,
  1, DB_INT_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [267]:  i  :  any(char)
*/
{ ADFI_258_ANY_CHAR, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_ANY_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_2CXI_SKIP,
  1, DB_INT_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [268]:  i  :  any(date)
*/
{ ADFI_082_ANY_DATE, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_ANY_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_2CXI_SKIP,
  1, DB_INT_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [269]:  i  :  any(f)
*/
{ ADFI_083_ANY_F, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_ANY_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_2CXI_SKIP,
  1, DB_INT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [270]:  i  :  any(dec)
*/
{ ADFI_532_ANY_DEC, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_ANY_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_2CXI_SKIP,
  1, DB_INT_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [271]:  i  :  any(i)
*/
{ ADFI_084_ANY_I, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_ANY_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_2CXI_SKIP,
  1, DB_INT_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [272]:  i  :  any(money)
*/
{ ADFI_085_ANY_MONEY, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_ANY_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_2CXI_SKIP,
  1, DB_INT_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [273]:  i  :  any(text)
*/
{ ADFI_086_ANY_TEXT, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_ANY_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_2CXI_SKIP,
  1, DB_INT_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [274]:  i  :  any(varchar)
*/
{ ADFI_259_ANY_VARCHAR, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_ANY_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_2CXI_SKIP,
  1, DB_INT_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [275]:  i  :  any(logical_key)
*/
{ ADFI_479_ANY_LOGKEY, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_ANY_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_2CXI_SKIP,
  1, DB_INT_TYPE,
    DB_LOGKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [276]:  i  :  any(table_key)
*/
{ ADFI_498_ANY_TABKEY, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_ANY_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_2CXI_SKIP,
  1, DB_INT_TYPE,
    DB_TABKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [277]:  i  :  any(nchar)
*/
{ ADFI_1028_ANY_NCHAR, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_ANY_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_2CXI_SKIP,
  1, DB_INT_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [278]:  i  :  any(nvarchar)
*/
{ ADFI_1029_ANY_NVCHAR, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_ANY_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_2CXI_SKIP,
  1, DB_INT_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [279]:  f  :  avg(f)
*/
{ ADFI_087_AVG_F, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_AVG_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_2CXI_SKIP,
  1, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [280]:  dec  :  avg(dec)
*/
{ ADFI_533_AVG_DEC, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_AVG_OP,
  {ADI_DECAVG, 0, 0}, DB_MAX_DECLEN, ADE_2CXI_SKIP,
  1, DB_DEC_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [281]:  f  :  avg(i)
*/
{ ADFI_088_AVG_I, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_AVG_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_2CXI_SKIP,
  1, DB_FLT_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [282]:  money  :  avg(money)
*/
{ ADFI_089_AVG_MONEY, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_AVG_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_2CXI_SKIP,
  1, DB_MNY_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [283]:  date  :  avg(date)
*/
{ ADFI_464_AVG_DATE, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F4_WORKSPACE|ADI_F512_RES1STARG, ADI_AVG_OP,
  {ADI_FIXED, 12, 0}, sizeof(AD_AGGDATE), ADE_2CXI_SKIP,
  1, DB_DTE_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [284]:  i  :  count(c) (or any other type)
*/
{ ADFI_090_COUNT_C_ALL, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_CNT_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_2CXI_SKIP,
  1, DB_INT_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [285]:  i  :  count(char)
*/
{ ADFI_260_COUNT_CHAR, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_CNT_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_2CXI_SKIP,
  1, DB_INT_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [286]:  i  :  count(date)
*/
{ ADFI_091_COUNT_DATE, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_CNT_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_2CXI_SKIP,
  1, DB_INT_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [287]:  i  :  count(f)
*/
{ ADFI_092_COUNT_F, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_CNT_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_2CXI_SKIP,
  1, DB_INT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [288]:  i  :  count(dec)
*/
{ ADFI_534_COUNT_DEC, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_CNT_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_2CXI_SKIP,
  1, DB_INT_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [289]:  i  :  count(i)
*/
{ ADFI_093_COUNT_I, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_CNT_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_2CXI_SKIP,
  1, DB_INT_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [290]:  i  :  count(money)
*/
{ ADFI_094_COUNT_MONEY, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_CNT_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_2CXI_SKIP,
  1, DB_INT_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [291]:  i  :  count(text)
*/
{ ADFI_095_COUNT_TEXT, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_CNT_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_2CXI_SKIP,
  1, DB_INT_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [292]:  i  :  count(varchar)
*/
{ ADFI_261_COUNT_VARCHAR, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_CNT_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_2CXI_SKIP,
  1, DB_INT_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [293]:  i  :  count(logical_key)
*/
{ ADFI_480_COUNT_LOGKEY, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_CNT_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_2CXI_SKIP,
  1, DB_INT_TYPE,
    DB_LOGKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [294]:  i  :  count(table_key)
*/
{ ADFI_499_COUNT_TABKEY, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_CNT_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_2CXI_SKIP,
  1, DB_INT_TYPE,
    DB_TABKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [295]:  i  :  count(nchar)
*/
{ ADFI_1030_COUNT_NCHAR, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_CNT_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_2CXI_SKIP,
  1, DB_INT_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [296]:  i  :  count(nvarchar)
*/
{ ADFI_1031_COUNT_NVCHAR, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_CNT_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_2CXI_SKIP,
  1, DB_INT_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [297]:  i  :  count(*)
*/
{ ADFI_396_COUNTSTAR, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_CNTAL_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_0CXI_IGNORE,
  0, DB_INT_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [298]:  c  :  max(c)
*/
{ ADFI_096_MAX_C, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_MAX_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_CHR_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [299]:  char  :  max(char)
*/
{ ADFI_262_MAX_CHAR, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_MAX_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_CHA_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [300]:  date  :  max(date)
*/
{ ADFI_097_MAX_DATE, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F512_RES1STARG, ADI_MAX_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_DTE_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [301]:  f  :  max(f)
*/
{ ADFI_098_MAX_F, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_MAX_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [302]:  dec  :  max(dec)
*/
{ ADFI_535_MAX_DEC, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_MAX_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_DEC_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [303]:  i  :  max(i)
*/
{ ADFI_099_MAX_I, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_MAX_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_INT_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [304]:  money  :  max(money)
*/
{ ADFI_100_MAX_MONEY, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_MAX_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_MNY_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [305]:  text  :  max(text)
*/
{ ADFI_101_MAX_TEXT, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_MAX_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_TXT_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [306]:  varchar  :  max(varchar)
*/
{ ADFI_263_MAX_VARCHAR, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_MAX_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [307]:  logical_key  :  max(logical_key)
*/
{ ADFI_481_MAX_LOGKEY, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_MAX_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_LOGKEY_TYPE,
    DB_LOGKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [308]:  table_key  :  max(table_key)
*/
{ ADFI_500_MAX_TABKEY, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_MAX_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_TABKEY_TYPE,
    DB_TABKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [309]:  nchar  :  max(nchar)
*/
{ ADFI_1032_MAX_NCHAR, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_MAX_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_NCHR_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [310]:  nvarchar  :  max(nvarchar)
*/
{ ADFI_1033_MAX_NVCHAR, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_MAX_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_NVCHR_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [311]:  c  :  min(c)
*/
{ ADFI_102_MIN_C, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_MIN_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_CHR_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [312]:  char  :  min(char)
*/
{ ADFI_264_MIN_CHAR, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_MIN_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_CHA_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [313]:  date  :  min(date)
*/
{ ADFI_103_MIN_DATE, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F512_RES1STARG, ADI_MIN_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_DTE_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [314]:  f  :  min(f)
*/
{ ADFI_104_MIN_F, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_MIN_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [315]:  dec  :  min(dec)
*/
{ ADFI_536_MIN_DEC, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_MIN_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_DEC_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [316]:  i  :  min(i)
*/
{ ADFI_105_MIN_I, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_MIN_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_INT_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [317]:  money  :  min(money)
*/
{ ADFI_106_MIN_MONEY, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_MIN_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_MNY_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [318]:  text  :  min(text)
*/
{ ADFI_107_MIN_TEXT, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_MIN_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_TXT_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [319]:  logical_key  :  min(logical_key)
*/
{ ADFI_482_MIN_LOGKEY, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_MIN_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_LOGKEY_TYPE,
    DB_LOGKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [320]:  table_key  :  min(table_key)
*/
{ ADFI_501_MIN_TABKEY, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_MIN_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_TABKEY_TYPE,
    DB_TABKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [321]:  varchar  :  min(varchar)
*/
{ ADFI_265_MIN_VARCHAR, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_MIN_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [322]:  nchar  :  min(nchar)
*/
{ ADFI_1034_MIN_NCHAR, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_MIN_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_NCHR_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [323]:  nvarchar  :  min(nvarchar)
*/
{ ADFI_1035_MIN_NVCHAR, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_MIN_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_NVCHR_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [324]:  f  :  sum(f)
*/
{ ADFI_108_SUM_F, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_SUM_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [325]:  dec  :  sum(dec)
*/
{ ADFI_537_SUM_DEC, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F512_RES1STARG, ADI_SUM_OP,
  {ADI_DECSUM, 0, 0}, DB_MAX_DECLEN, ADE_5CXI_CLR_SKIP,
  1, DB_DEC_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [326]:  i  :  sum(i)
*/
{ ADFI_109_SUM_I, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_SUM_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_INT_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [327]:  money  :  sum(money)
*/
{ ADFI_110_SUM_MONEY, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F0_NOFIFLAGS, ADI_SUM_OP,
  {ADI_O1, 0, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_MNY_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [328]:  date  :  sum(date)
*/
{ ADFI_465_SUM_DATE, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F512_RES1STARG, ADI_SUM_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_5CXI_CLR_SKIP,
  1, DB_DTE_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [329]:  float :  corr(float, float)
*/
{ ADFI_832_CORR_FLT, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F4_WORKSPACE|ADI_F16_OLAPAGG, ADI_CORR_OP,
  {ADI_FIXED, 8, 0}, sizeof(ADF_AG_OLAP), ADE_0CXI_IGNORE,
  2, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [330]:  float :  covar_pop(float, float)
*/
{ ADFI_833_COVAR_POP_FLT, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F4_WORKSPACE|ADI_F16_OLAPAGG, ADI_COVAR_POP_OP,
  {ADI_FIXED, 8, 0}, sizeof(ADF_AG_OLAP), ADE_0CXI_IGNORE,
  2, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [331]:  float :  covar_samp(float, float)
*/
{ ADFI_834_COVAR_SAMP_FLT, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F4_WORKSPACE|ADI_F16_OLAPAGG, ADI_COVAR_SAMP_OP,
  {ADI_FIXED, 8, 0}, sizeof(ADF_AG_OLAP), ADE_0CXI_IGNORE,
  2, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [332]:  float :  regr_avgx(float, float)
*/
{ ADFI_835_REGR_AVGX_FLT, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F4_WORKSPACE|ADI_F16_OLAPAGG, ADI_REGR_AVGX_OP,
  {ADI_FIXED, 8, 0}, sizeof(ADF_AG_OLAP), ADE_0CXI_IGNORE,
  2, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [333]:  float :  regr_avgy(float, float)
*/
{ ADFI_836_REGR_AVGY_FLT, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F4_WORKSPACE|ADI_F16_OLAPAGG, ADI_REGR_AVGY_OP,
  {ADI_FIXED, 8, 0}, sizeof(ADF_AG_OLAP), ADE_0CXI_IGNORE,
  2, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [334]:  i  :  regr_count(all, all)
*/
{ ADFI_837_REGR_COUNT_ANY, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F4_WORKSPACE|ADI_F16_OLAPAGG, ADI_REGR_COUNT_OP,
  {ADI_FIXED, 4, 0}, sizeof(ADF_AG_OLAP), ADE_0CXI_IGNORE,
  2, DB_INT_TYPE,
    DB_ALL_TYPE, DB_ALL_TYPE, DB_NODT, DB_NODT
},
/*
** [335]:  float :  regr_intercept(float, float)
*/
{ ADFI_838_REGR_INTERCEPT_FLT, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F4_WORKSPACE|ADI_F16_OLAPAGG, ADI_REGR_INTERCEPT_OP,
  {ADI_FIXED, 8, 0}, sizeof(ADF_AG_OLAP), ADE_0CXI_IGNORE,
  2, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [336]:  float :  regr_r2(float, float)
*/
{ ADFI_839_REGR_R2_FLT, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F4_WORKSPACE|ADI_F16_OLAPAGG, ADI_REGR_R2_OP,
  {ADI_FIXED, 8, 0}, sizeof(ADF_AG_OLAP), ADE_0CXI_IGNORE,
  2, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [337]:  float :  regr_slope(float, float)
*/
{ ADFI_840_REGR_SLOPE_FLT, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F4_WORKSPACE|ADI_F16_OLAPAGG, ADI_REGR_SLOPE_OP,
  {ADI_FIXED, 8, 0}, sizeof(ADF_AG_OLAP), ADE_0CXI_IGNORE,
  2, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [338]:  float :  regr_sxx(float, float)
*/
{ ADFI_841_REGR_SXX_FLT, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F4_WORKSPACE|ADI_F16_OLAPAGG, ADI_REGR_SXX_OP,
  {ADI_FIXED, 8, 0}, sizeof(ADF_AG_OLAP), ADE_0CXI_IGNORE,
  2, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [339]:  float :  regr_sxy(float, float)
*/
{ ADFI_842_REGR_SXY_FLT, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F4_WORKSPACE|ADI_F16_OLAPAGG, ADI_REGR_SXY_OP,
  {ADI_FIXED, 8, 0}, sizeof(ADF_AG_OLAP), ADE_0CXI_IGNORE,
  2, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [340]:  float :  regr_syy(float, float)
*/
{ ADFI_843_REGR_SYY_FLT, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F4_WORKSPACE|ADI_F16_OLAPAGG, ADI_REGR_SYY_OP,
  {ADI_FIXED, 8, 0}, sizeof(ADF_AG_OLAP), ADE_0CXI_IGNORE,
  2, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [341]:  float :  stddev_pop(float)
*/
{ ADFI_844_STDDEV_POP_FLT, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F16_OLAPAGG, ADI_STDDEV_POP_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_0CXI_IGNORE,
  1, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [342]:  float :  stddev_samp(float)
*/
{ ADFI_845_STDDEV_SAMP_FLT, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F16_OLAPAGG, ADI_STDDEV_SAMP_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_0CXI_IGNORE,
  1, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [343]:  float :  var_pop(float)
*/
{ ADFI_846_VAR_POP_FLT, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F16_OLAPAGG, ADI_VAR_POP_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_0CXI_IGNORE,
  1, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [344]:  float :  var_samp(float)
*/
{ ADFI_847_VAR_SAMP_FLT, ADI_NOFI,
  ADI_AGG_FUNC, ADI_F16_OLAPAGG, ADI_VAR_SAMP_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_0CXI_IGNORE,
  1, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [345]:  i  :  _bintim()
*/
{ ADFI_442__BINTIM, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_BNTIM_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_0CXI_IGNORE,
  0, DB_INT_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [346]:  i  :  _bintim(i)
*/
{ ADFI_194__BINTIM_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_BNTIM_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_0CXI_IGNORE,
  1, DB_INT_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [347]:  i  :  _bio_cnt()
*/
{ ADFI_200__BIO_CNT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_BIOC_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_0CXI_IGNORE,
  0, DB_INT_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [348]:  i  :  _cache_read()
*/
{ ADFI_205__CACHE_READ, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_CHRD_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_0CXI_IGNORE,
  0, DB_INT_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [349]:  i  :  _cache_req()
*/
{ ADFI_204__CACHE_REQ, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_CHREQ_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_0CXI_IGNORE,
  0, DB_INT_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [350]:  i  :  _cache_rread()
*/
{ ADFI_207__CACHE_RREAD, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_CHRRD_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_0CXI_IGNORE,
  0, DB_INT_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [351]:  i  :  _cache_size()
*/
{ ADFI_208__CACHE_SIZE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CHSIZ_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_0CXI_IGNORE,
  0, DB_INT_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [352]:  i  :  _cache_used()
*/
{ ADFI_209__CACHE_USED, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_CHUSD_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_0CXI_IGNORE,
  0, DB_INT_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [353]:  i  :  _cache_write()
*/
{ ADFI_206__CACHE_WRITE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_CHWR_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_0CXI_IGNORE,
  0, DB_INT_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [354]:  i  :  _cpu_ms()
*/
{ ADFI_197__CPU_MS, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_CPUMS_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_0CXI_IGNORE,
  0, DB_INT_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [355]:  c  :  _date(i)
*/
{ ADFI_195__DATE_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_BDATE_OP,
  {ADI_FIXED, 9, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHR_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [356]:  i  :  _dio_cnt()
*/
{ ADFI_199__DIO_CNT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_DIOC_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_0CXI_IGNORE,
  0, DB_INT_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [357]:  i  :  _et_sec()
*/
{ ADFI_198__ET_SEC, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_ETSEC_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_0CXI_IGNORE,
  0, DB_INT_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [358]:  i  :  _pfault_cnt()
*/
{ ADFI_201__PFAULT_CNT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_PFLTC_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_0CXI_IGNORE,
  0, DB_INT_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [359]:  i  :  _phys_page()
*/
{ ADFI_203__PHYS_PAGE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_PHPAG_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_0CXI_IGNORE,
  0, DB_INT_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [360]:  c  :  _time(i)
*/
{ ADFI_196__TIME_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_BTIME_OP,
  {ADI_FIXED, 5, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHR_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [361]:  c  :  _version()
*/
{ ADFI_210__VERSION, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_VERSN_OP,
  {ADI_FIXED, 32, 0}, 0, ADE_0CXI_IGNORE,
  0, DB_CHR_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [362]:  i  :  _ws_page()
*/
{ ADFI_202__WS_PAGE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_WSPAG_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_0CXI_IGNORE,
  0, DB_INT_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [363]:  f  :  abs(f)
*/
{ ADFI_111_ABS_F, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_ABS_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [364]:  dec  :  abs(dec)
*/
{ ADFI_538_ABS_DEC, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_ABS_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DEC_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [365]:  i  :  abs(i)
*/
{ ADFI_112_ABS_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_ABS_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [366]:  money  :  abs(money)
*/
{ ADFI_113_ABS_MONEY, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_ABS_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_MNY_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [367]:  c  :  ascii(all)
*/
{ ADFI_539_ASCII_ALL, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI|ADI_F64_ALLSKIPUNI, ADI_ASCII_OP,
  {ADI_LEN_INDIRECT, 539, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHR_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [368]:  c  :  ascii(nchar)
*/
{ ADFI_1176_ASCII_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_ASCII_OP,
  {ADI_UTOC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHR_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [369]:  c  :  ascii(nvarchar)
*/
{ ADFI_1177_ASCII_NVCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_ASCII_OP,
  {ADI_VARUTOC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHR_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [370]:  c  :  ascii(all, i)
*/
{ ADFI_586_ASCII_ALL_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI|ADI_F64_ALLSKIPUNI, ADI_ASCII_OP,
  {ADI_LEN_INDIRECT, 586, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHR_TYPE,
    DB_ALL_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [371]:  f  :  atan(f)
*/
{ ADFI_120_ATAN_F, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_ATAN_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [372]:  varchar  :  dbmsinfo(varchar)
*/
{ ADFI_397_DBMSINFO_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_DBMSI_OP,
  {ADI_FIXED, 64+DB_CNTSIZE, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [373]:  char  :  char(all)
*/
{ ADFI_540_CHAR_ALL, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F64_ALLSKIPUNI, ADI_CHAR_OP,
  {ADI_LEN_INDIRECT, 540, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [374]:  char  :  char(nchar)
*/
{ ADFI_1170_CHAR_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CHAR_OP,
  {ADI_UTOC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [375]:  char  :  char(nvarchar)
*/
{ ADFI_1171_CHAR_NVCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CHAR_OP,
  {ADI_VARUTOC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [376]:  char  :  char(all, i)
*/
{ ADFI_617_CHAR_ALL_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F64_ALLSKIPUNI, ADI_CHAR_OP,
  {ADI_LEN_INDIRECT, 617, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHA_TYPE,
    DB_ALL_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [377]:  c  :  concat(c,c)
*/
{ ADFI_121_CONCAT_C_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CNCAT_OP,
  {ADI_CSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHR_TYPE,
    DB_CHR_TYPE, DB_CHR_TYPE, DB_NODT, DB_NODT
},
/*
** [378]:  c  :  concat(text,c)
*/
{ ADFI_122_CONCAT_TEXT_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CNCAT_OP,
  {ADI_CTSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHR_TYPE,
    DB_TXT_TYPE, DB_CHR_TYPE, DB_NODT, DB_NODT
},
/*
** [379]:  text  :  concat(text,text)
*/
{ ADFI_123_CONCAT_TEXT_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CNCAT_OP,
  {ADI_TSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_TXT_TYPE,
    DB_TXT_TYPE, DB_TXT_TYPE, DB_NODT, DB_NODT
},
/*
** [380]:  text  :  concat(text,char)
*/
{ ADFI_450_CONCAT_TEXT_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CNCAT_OP,
  {ADI_SUMV, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_TXT_TYPE,
    DB_TXT_TYPE, DB_CHA_TYPE, DB_NODT, DB_NODT
},
/*
** [381]:  text  :  concat(text,varchar)
*/
{ ADFI_451_CONCAT_TEXT_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CNCAT_OP,
  {ADI_TSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_TXT_TYPE,
    DB_TXT_TYPE, DB_VCH_TYPE, DB_NODT, DB_NODT
},
/*
** [382]:  char  :  concat(char,char)
*/
{ ADFI_276_CONCAT_CHAR_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CNCAT_OP,
  {ADI_CSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHA_TYPE,
    DB_CHA_TYPE, DB_CHA_TYPE, DB_NODT, DB_NODT
},
/*
** [383]:  char  :  concat(char,varchar)
*/
{ ADFI_454_CONCAT_CHAR_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CNCAT_OP,
  {ADI_CTSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHA_TYPE,
    DB_CHA_TYPE, DB_VCH_TYPE, DB_NODT, DB_NODT
},
/*
** [384]:  c  :  concat(varchar,c)
*/
{ ADFI_277_CONCAT_VARCHAR_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CNCAT_OP,
  {ADI_CTSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHR_TYPE,
    DB_VCH_TYPE, DB_CHR_TYPE, DB_NODT, DB_NODT
},
/*
** [385]:  text  :  concat(varchar,text)
*/
{ ADFI_455_CONCAT_VARCHAR_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CNCAT_OP,
  {ADI_TSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_TXT_TYPE,
    DB_VCH_TYPE, DB_TXT_TYPE, DB_NODT, DB_NODT
},
/*
** [386]:  char  :  concat(varchar,char)
*/
{ ADFI_278_CONCAT_VARCHAR_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CNCAT_OP,
  {ADI_CTSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHA_TYPE,
    DB_VCH_TYPE, DB_CHA_TYPE, DB_NODT, DB_NODT
},
/*
** [387]:  varchar  :  concat(varchar,varchar)
*/
{ ADFI_279_CONCAT_VARCHAR_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CNCAT_OP,
  {ADI_TSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_VCH_TYPE, DB_NODT, DB_NODT
},
/*
** [388]:  lvch  :  concat(lvch,lvch)
*/
{ ADFI_713_CONCAT_LVCH_LVCH, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F4_WORKSPACE, ADI_CNCAT_OP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  2, DB_LVCH_TYPE,
    DB_LVCH_TYPE, DB_LVCH_TYPE, DB_NODT, DB_NODT
},
/*
** [389]:  byte  :  concat(byte,byte)
*/
{ ADFI_730_CONCAT_BYTE_BYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CNCAT_OP,
  {ADI_CSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_BYTE_TYPE,
    DB_BYTE_TYPE, DB_BYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [390]:  vbyte  :  concat(vbyte,vbyte)
*/
{ ADFI_731_CONCAT_VBYTE_VBYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CNCAT_OP,
  {ADI_TSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VBYTE_TYPE,
    DB_VBYTE_TYPE, DB_VBYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [391]:  lbyte  :  concat(lbyte,lbyte)
*/
{ ADFI_732_CONCAT_LBYTE_LBYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F4_WORKSPACE, ADI_CNCAT_OP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  2, DB_LBYTE_TYPE,
    DB_LBYTE_TYPE, DB_LBYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [392]:  byte  :  concat(byte,vbyte)
*/
{ ADFI_766_CONCAT_BYTE_VBYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CNCAT_OP,
  {ADI_CTSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_BYTE_TYPE,
    DB_BYTE_TYPE, DB_VBYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [393]:  byte  :  concat(vbyte,byte)
*/
{ ADFI_767_CONCAT_VBYTE_BYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CNCAT_OP,
  {ADI_CTSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_BYTE_TYPE,
    DB_VBYTE_TYPE, DB_BYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [394]:  nchar  :  concat(nchar,nchar)
*/
{ ADFI_1005_CONCAT_NCHAR_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CNCAT_OP,
  {ADI_CSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_NCHR_TYPE,
    DB_NCHR_TYPE, DB_NCHR_TYPE, DB_NODT, DB_NODT
},
/*
** [395]:  nvarchar  :  concat(nvarchar,nvarchar)
*/
{ ADFI_1010_CONCAT_NVCHAR_NVCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CNCAT_OP,
  {ADI_TSUM, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_NVCHR_TYPE,
    DB_NVCHR_TYPE, DB_NVCHR_TYPE, DB_NODT, DB_NODT
},
/*
** [396]:  char  :  iitypename(i)
*/
{ ADFI_398_IITYPENAME_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_TYNAM_OP,
  {ADI_FIXED, 32, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [397]:  f  :  cos(f)
*/
{ ADFI_124_COS_F, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_COS_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [398]:  date  :  date(c)
*/
{ ADFI_125_DATE_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_DATE_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DTE_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [399]:  date  :  date(char)
*/
{ ADFI_280_DATE_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_DATE_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DTE_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [400]:  date  :  date(text)
*/
{ ADFI_126_DATE_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_DATE_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DTE_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [401]:  date  :  date(varchar)
*/
{ ADFI_281_DATE_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_DATE_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DTE_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [402]:  date  :  date(nchar)
*/
{ ADFI_1178_DATE_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_DATE_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DTE_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [403]:  date  :  date(nvarchar)
*/
{ ADFI_1179_DATE_NVCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_DATE_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DTE_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [404]:  date  :  date(date)
*/
{ ADFI_441_DATE_DATE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_DATE_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DTE_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [405]:  i  :  date_part(c,date)
*/
{ ADFI_128_DATE_PART_C_DATE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_DPART_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_CHR_TYPE, DB_DTE_TYPE, DB_NODT, DB_NODT
},
/*
** [406]:  i  :  date_part(char,date)
*/
{ ADFI_282_DATE_PART_CHAR_DATE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_DPART_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_CHA_TYPE, DB_DTE_TYPE, DB_NODT, DB_NODT
},
/*
** [407]:  i  :  date_part(text,date)
*/
{ ADFI_129_DATE_PART_TEXT_DATE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_DPART_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_TXT_TYPE, DB_DTE_TYPE, DB_NODT, DB_NODT
},
/*
** [408]:  i  :  date_part(varchar,date)
*/
{ ADFI_283_DATE_PART_VARCHAR_DATE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_DPART_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_VCH_TYPE, DB_DTE_TYPE, DB_NODT, DB_NODT
},
/*
** [409]:  date  :  date_trunc(c,date)
*/
{ ADFI_132_DATE_TRUNC_C_DATE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1024_RES2NDARG, ADI_DTRUN_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DTE_TYPE,
    DB_CHR_TYPE, DB_DTE_TYPE, DB_NODT, DB_NODT
},
/*
** [410]:  date  :  date_trunc(char,date)
*/
{ ADFI_286_DATE_TRUNC_CHAR_DATE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1024_RES2NDARG, ADI_DTRUN_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DTE_TYPE,
    DB_CHA_TYPE, DB_DTE_TYPE, DB_NODT, DB_NODT
},
/*
** [411]:  date  :  date_trunc(text,date)
*/
{ ADFI_133_DATE_TRUNC_TEXT_DATE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1024_RES2NDARG, ADI_DTRUN_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DTE_TYPE,
    DB_TXT_TYPE, DB_DTE_TYPE, DB_NODT, DB_NODT
},
/*
** [412]:  date  :  date_trunc(varchar,date)
*/
{ ADFI_287_DATE_TRUNC_VARCHAR_DAT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1024_RES2NDARG, ADI_DTRUN_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DTE_TYPE,
    DB_VCH_TYPE, DB_DTE_TYPE, DB_NODT, DB_NODT
},
/*
** [413]:  c  :  dba()
*/
{ ADFI_211_DBA, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_DBA_OP,
  {ADI_FIXED, DB_MAXNAME, 0}, 0, ADE_0CXI_IGNORE,
  0, DB_CHR_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [414]:  c  :  dow(date)
*/
{ ADFI_127_DOW_DATE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_DOW_OP,
  {ADI_FIXED, 3, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHR_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [415]:  f  :  exp(f)
*/
{ ADFI_134_EXP_F, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_EXP_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [416]:  f  :  float4(c)
*/
{ ADFI_135_FLOAT4_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_F4_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [417]:  f  :  float4(char)
*/
{ ADFI_288_FLOAT4_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_F4_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [418]:  f  :  float4(f)
*/
{ ADFI_136_FLOAT4_F, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_F4_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [419]:  f  :  float4(dec)
*/
{ ADFI_549_FLOAT4_DEC, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_F4_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [420]:  f  :  float4(i)
*/
{ ADFI_137_FLOAT4_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_F4_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [421]:  f  :  float4(money)
*/
{ ADFI_139_FLOAT4_MONEY, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_F4_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [422]:  f  :  float4(text)
*/
{ ADFI_138_FLOAT4_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_F4_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [423]:  f  :  float4(varchar)
*/
{ ADFI_289_FLOAT4_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_F4_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [424]:  f  :  float4(nchar)
*/
{ ADFI_1164_FLOAT4_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_F4_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [425]:  f  :  float4(nvarchar)
*/
{ ADFI_1165_FLOAT4_NVCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_F4_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [426]:  f  :  float8(c)
*/
{ ADFI_140_FLOAT8_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_F8_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [427]:  f  :  float8(char)
*/
{ ADFI_290_FLOAT8_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_F8_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [428]:  f  :  float8(f)
*/
{ ADFI_141_FLOAT8_F, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_F8_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [429]:  f  :  float8(dec)
*/
{ ADFI_550_FLOAT8_DEC, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_F8_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [430]:  f  :  float8(i)
*/
{ ADFI_142_FLOAT8_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_F8_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [431]:  f  :  float8(money)
*/
{ ADFI_144_FLOAT8_MONEY, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_F8_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [432]:  f  :  float8(text)
*/
{ ADFI_143_FLOAT8_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_F8_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [433]:  f  :  float8(varchar)
*/
{ ADFI_291_FLOAT8_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_F8_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [434]:  f  :  float8(nchar)
*/
{ ADFI_1166_FLOAT8_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_F8_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [435]:  f  :  float8(nvarchar)
*/
{ ADFI_1167_FLOAT8_NVCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_F8_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [436]:  i  :  int1(c)
*/
{ ADFI_145_INT1_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I1_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [437]:  i  :  int1(char)
*/
{ ADFI_292_INT1_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I1_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [438]:  i  :  int1(f)
*/
{ ADFI_146_INT1_F, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I1_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [439]:  i  :  int1(dec)
*/
{ ADFI_553_INT1_DEC, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I1_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [440]:  i  :  int1(i)
*/
{ ADFI_147_INT1_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I1_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [441]:  i  :  int1(money)
*/
{ ADFI_149_INT1_MONEY, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I1_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [442]:  i  :  int1(text)
*/
{ ADFI_148_INT1_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I1_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [443]:  i  :  int1(varchar)
*/
{ ADFI_293_INT1_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I1_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [444]:  i  :  int1(nchar)
*/
{ ADFI_1156_INT1_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I1_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [445]:  i  :  int1(nvarchar)
*/
{ ADFI_1157_INT1_NVCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I1_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [446]:  i  :  int2(c)
*/
{ ADFI_150_INT2_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I2_OP,
  {ADI_FIXED, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [447]:  i  :  int2(char)
*/
{ ADFI_294_INT2_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I2_OP,
  {ADI_FIXED, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [448]:  i  :  int2(f)
*/
{ ADFI_151_INT2_F, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I2_OP,
  {ADI_FIXED, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [449]:  i  :  int2(dec)
*/
{ ADFI_554_INT2_DEC, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I2_OP,
  {ADI_FIXED, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [450]:  i  :  int2(i)
*/
{ ADFI_152_INT2_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I2_OP,
  {ADI_FIXED, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [451]:  i  :  int2(money)
*/
{ ADFI_154_INT2_MONEY, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I2_OP,
  {ADI_FIXED, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [452]:  i  :  int2(text)
*/
{ ADFI_153_INT2_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I2_OP,
  {ADI_FIXED, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [453]:  i  :  int2(varchar)
*/
{ ADFI_295_INT2_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I2_OP,
  {ADI_FIXED, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [454]:  i  :  int2(nchar)
*/
{ ADFI_1158_INT2_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I2_OP,
  {ADI_FIXED, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [455]:  i  :  int2(nvarchar)
*/
{ ADFI_1159_INT2_NVCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I2_OP,
  {ADI_FIXED, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [456]:  i  :  int4(c)
*/
{ ADFI_155_INT4_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I4_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [457]:  i  :  int4(char)
*/
{ ADFI_296_INT4_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I4_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [458]:  i  :  int4(f)
*/
{ ADFI_156_INT4_F, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I4_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [459]:  i  :  int4(dec)
*/
{ ADFI_555_INT4_DEC, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I4_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [460]:  i  :  int4(i)
*/
{ ADFI_157_INT4_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I4_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [461]:  i  :  int4(money)
*/
{ ADFI_159_INT4_MONEY, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I4_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [462]:  i  :  int4(text)
*/
{ ADFI_158_INT4_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I4_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [463]:  i  :  int4(varchar)
*/
{ ADFI_297_INT4_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I4_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [464]:  i  :  int4(nchar)
*/
{ ADFI_1160_INT4_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I4_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [465]:  i  :  int4(nvarchar)
*/
{ ADFI_1161_INT4_NVCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I4_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [466]:  f  :  interval(c,date)
*/
{ ADFI_130_INTERVAL_C_DATE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_INTVL_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_FLT_TYPE,
    DB_CHR_TYPE, DB_DTE_TYPE, DB_NODT, DB_NODT
},
/*
** [467]:  f  :  interval(char,date)
*/
{ ADFI_284_INTERVAL_CHAR_DATE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_INTVL_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_FLT_TYPE,
    DB_CHA_TYPE, DB_DTE_TYPE, DB_NODT, DB_NODT
},
/*
** [468]:  f  :  interval(text,date)
*/
{ ADFI_131_INTERVAL_TEXT_DATE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_INTVL_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_FLT_TYPE,
    DB_TXT_TYPE, DB_DTE_TYPE, DB_NODT, DB_NODT
},
/*
** [469]:  f  :  interval(varchar,date)
*/
{ ADFI_285_INTERVAL_VARCHAR_DATE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_INTVL_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_FLT_TYPE,
    DB_VCH_TYPE, DB_DTE_TYPE, DB_NODT, DB_NODT
},
/*
** [470]:  c  :  left(c,i)
*/
{ ADFI_160_LEFT_C_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LEFT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHR_TYPE,
    DB_CHR_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [471]:  char  :  left(char,i)
*/
{ ADFI_298_LEFT_CHAR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LEFT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHA_TYPE,
    DB_CHA_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [472]:  text  :  left(text,i)
*/
{ ADFI_161_LEFT_TEXT_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LEFT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_TXT_TYPE,
    DB_TXT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [473]:  varchar  :  left(varchar,i)
*/
{ ADFI_299_LEFT_VARCHAR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LEFT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [474]:  lvch  :  left(lvch,int)
*/
{ ADFI_709_LEFT_LVCH_INT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F4_WORKSPACE, ADI_LEFT_OP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  2, DB_LVCH_TYPE,
    DB_LVCH_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [475]:  byte  :  left(byte,i)
*/
{ ADFI_733_LEFT_BYTE_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LEFT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_BYTE_TYPE,
    DB_BYTE_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [476]:  vbyte  :  left(vbyte,i)
*/
{ ADFI_734_LEFT_VBYTE_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LEFT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VBYTE_TYPE,
    DB_VBYTE_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [477]:  lbyte  :  left(lbyte,i)
*/
{ ADFI_735_LEFT_LBYTE_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F4_WORKSPACE, ADI_LEFT_OP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  2, DB_LBYTE_TYPE,
    DB_LBYTE_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [478]:  nchar  :  left(nchar,i)
*/
{ ADFI_1012_LEFT_NCHAR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LEFT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_NCHR_TYPE,
    DB_NCHR_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [479]:  nvarchar  :  left(nvarchar,i)
*/
{ ADFI_1013_LEFT_NVCHAR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LEFT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_NVCHR_TYPE,
    DB_NVCHR_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [480]:  i  :  length(c)
*/
{ ADFI_162_LENGTH_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [481]:  i  :  length(char)
*/
{ ADFI_300_LENGTH_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [482]:  i  :  length(text)
*/
{ ADFI_163_LENGTH_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [483]:  i  :  length(varchar)
*/
{ ADFI_301_LENGTH_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [484]:  int  :  length(bit)
*/
{ ADFI_675_LENGTH_BIT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_BIT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [485]:  int  :  length(vbit)
*/
{ ADFI_676_LENGTH_VBIT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_VBIT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [486]:  int  :  length(lvch)
*/
{ ADFI_707_LENGTH_LVCH, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_LVCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [487]:  int  :  length(byte)
*/
{ ADFI_736_LENGTH_BYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_BYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [488]:  int  :  length(vbyte)
*/
{ ADFI_737_LENGTH_VBYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_VBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [489]:  int  :  length(lbyte)
*/
{ ADFI_738_LENGTH_LBYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_LBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [490]:  i  :  length(nchar)
*/
{ ADFI_1016_LENGTH_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [491]:  i  :  length(nvarchar)
*/
{ ADFI_1017_LENGTH_NVCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [492]:  i  :  length(long_nvarchar)
*/
{ ADFI_1200_LENGTH_LONGNVCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_LNVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [493]:  i  :  locate(str,str)
*/
{ ADFI_164_LOCATE_STR_STR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_COMPAT_IDT, ADI_LOC_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_VCH_TYPE, DB_VCH_TYPE, DB_NODT, DB_NODT
},
/*
** [494]:  i  :  locate(unistr,unistr)
*/
{ ADFI_165_LOCATE_UNISTR_UNISTR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_COMPAT_IDT, ADI_LOC_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_NVCHR_TYPE, DB_NVCHR_TYPE, DB_NODT, DB_NODT
},
/*
** [495]:  f  :  log(f)
*/
{ ADFI_166_LOG_F, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LOG_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [496]:  c  :  lowercase(c)
*/
{ ADFI_167_LOWERCASE_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LOWER_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHR_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [497]:  char  :  lowercase(char)
*/
{ ADFI_304_LOWERCASE_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LOWER_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [498]:  text  :  lowercase(text)
*/
{ ADFI_168_LOWERCASE_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LOWER_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [499]:  varchar  :  lowercase(varchar)
*/
{ ADFI_305_LOWERCASE_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LOWER_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [500]:  lvch  :  lowercase(lvch)
*/
{ ADFI_712_LOWERCASE_LVCH, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F4_WORKSPACE, ADI_LOWER_OP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  1, DB_LVCH_TYPE,
    DB_LVCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [501]:  nchar  :  lowercase(nchar)
*/
{ ADFI_1038_LOWERCASE_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LOWER_OP,
  {ADI_O1UNIDBL, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NCHR_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [502]:  nvarchar  :  lowercase(nvarchar)
*/
{ ADFI_1039_LOWERCASE_NVCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LOWER_OP,
  {ADI_O1UNIDBL, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [503]:  lvch  :  lowercase(long nvarchar)
*/
{ ADFI_1281_LOWERCASE_LNVCHR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F4_WORKSPACE, ADI_LOWER_OP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  1, DB_LNVCHR_TYPE,
    DB_LNVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [504]:  varchar  :  lowercase(i)
*/
{ ADFI_1296_LOWERCASE_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LOWER_OP,
  {ADI_LEN_INDIRECT, 558, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [505]:  i  :  mod(i,i)
*/
{ ADFI_169_MOD_I_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_MOD_OP,
  {ADI_O2, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_INT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [506]:  money  :  money(f)
*/
{ ADFI_170_MONEY_F, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_MONEY_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_MNY_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [507]:  money  :  money(dec)
*/
{ ADFI_556_MONEY_DEC, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_MONEY_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_MNY_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [508]:  money  :  money(i)
*/
{ ADFI_171_MONEY_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_MONEY_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_MNY_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [509]:  money  :  money(c)
*/
{ ADFI_172_MONEY_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_MONEY_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_MNY_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [510]:  money  :  money(text)
*/
{ ADFI_173_MONEY_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_MONEY_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_MNY_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [511]:  money  :  money(char)
*/
{ ADFI_306_MONEY_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_MONEY_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_MNY_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [512]:  money  :  money(varchar)
*/
{ ADFI_307_MONEY_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_MONEY_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_MNY_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [513]:  money  :  money(nchar)
*/
{ ADFI_1180_MONEY_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_MONEY_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_MNY_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [514]:  money  :  money(nvarchar)
*/
{ ADFI_1181_MONEY_NVCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_MONEY_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_MNY_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [515]:  money  :  money(money)
*/
{ ADFI_440_MONEY_MONEY, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_MONEY_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_MNY_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [516]:  text  :  pad(c)
*/
{ ADFI_446_PAD_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_PAD_OP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [517]:  varchar  :  pad(char)
*/
{ ADFI_447_PAD_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_PAD_OP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [518]:  text  :  pad(text)
*/
{ ADFI_174_PAD_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_PAD_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [519]:  varchar  :  pad(varchar)
*/
{ ADFI_308_PAD_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_PAD_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [520]:  nvarchar  :  pad(nchar)
*/
{ ADFI_1040_PAD_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_PAD_OP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [521]:  nvarchar  :  pad(nvarchar)
*/
{ ADFI_1041_PAD_NVARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_PAD_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [522]:  c  :  right(c,i)
*/
{ ADFI_175_RIGHT_C_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_RIGHT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHR_TYPE,
    DB_CHR_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [523]:  char  :  right(char,i)
*/
{ ADFI_309_RIGHT_CHAR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_RIGHT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHA_TYPE,
    DB_CHA_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [524]:  text  :  right(text,i)
*/
{ ADFI_176_RIGHT_TEXT_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_RIGHT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_TXT_TYPE,
    DB_TXT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [525]:  varchar  :  right(varchar,i)
*/
{ ADFI_310_RIGHT_VARCHAR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_RIGHT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [526]:  lvch  :  right(lvch,int)
*/
{ ADFI_710_RIGHT_LVCH_INT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F4_WORKSPACE, ADI_RIGHT_OP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  2, DB_LVCH_TYPE,
    DB_LVCH_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [527]:  byte  :  right(byte,i)
*/
{ ADFI_739_RIGHT_BYTE_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_RIGHT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_BYTE_TYPE,
    DB_BYTE_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [528]:  vbyte  :  right(vbyte,i)
*/
{ ADFI_740_RIGHT_VBYTE_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_RIGHT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VBYTE_TYPE,
    DB_VBYTE_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [529]:  lbyte  :  right(lbyte,i)
*/
{ ADFI_741_RIGHT_LBYTE_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F4_WORKSPACE, ADI_RIGHT_OP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  2, DB_LBYTE_TYPE,
    DB_LBYTE_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [530]:  nchar  :  right(nchar,i)
*/
{ ADFI_1014_RIGHT_NCHAR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_RIGHT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_NCHR_TYPE,
    DB_NCHR_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [531]:  nvarchar  :  right(nvarchar,i)
*/
{ ADFI_1015_RIGHT_NVCHAR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_RIGHT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_NVCHR_TYPE,
    DB_NVCHR_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [532]:  c  :  shift(c,i)
*/
{ ADFI_177_SHIFT_C_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SHIFT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHR_TYPE,
    DB_CHR_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [533]:  char  :  shift(char,i)
*/
{ ADFI_311_SHIFT_CHAR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SHIFT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHA_TYPE,
    DB_CHA_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [534]:  text  :  shift(text,i)
*/
{ ADFI_178_SHIFT_TEXT_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SHIFT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_TXT_TYPE,
    DB_TXT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [535]:  varchar  :  shift(varchar,i)
*/
{ ADFI_312_SHIFT_VARCHAR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SHIFT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [536]:  byte  :  shift(byte,i)
*/
{ ADFI_742_SHIFT_BYTE_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SHIFT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_BYTE_TYPE,
    DB_BYTE_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [537]:  vbyte  :  shift(vbyte,i)
*/
{ ADFI_743_SHIFT_VBYTE_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SHIFT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VBYTE_TYPE,
    DB_VBYTE_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [538]:  lbyte  :  shift(lbyte,i)
*/
{ ADFI_744_SHIFT_LBYTE_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F4_WORKSPACE, ADI_SHIFT_OP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  2, DB_LBYTE_TYPE,
    DB_LBYTE_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [539]:  nchar  :  shift(nchar,i)
*/
{ ADFI_1042_SHIFT_NCHAR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SHIFT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_NCHR_TYPE,
    DB_NCHR_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [540]:  nvarchar  :  shift(nvarchar,i)
*/
{ ADFI_1043_SHIFT_NVCHAR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SHIFT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_NVCHR_TYPE,
    DB_NVCHR_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [541]:  f  :  sin(f)
*/
{ ADFI_179_SIN_F, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SIN_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [542]:  i  :  size(c)
*/
{ ADFI_180_SIZE_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SIZE_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [543]:  i  :  size(char)
*/
{ ADFI_313_SIZE_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SIZE_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [544]:  i  :  size(text)
*/
{ ADFI_181_SIZE_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SIZE_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [545]:  i  :  size(varchar)
*/
{ ADFI_314_SIZE_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SIZE_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [546]:  i  :  size(bit)
*/
{ ADFI_679_SIZE_BIT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SIZE_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_BIT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [547]:  i  :  size(vbit)
*/
{ ADFI_680_SIZE_VBIT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SIZE_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_VBIT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [548]:  int  :  size(lvch)
*/
{ ADFI_708_SIZE_LVCH, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SIZE_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_LVCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [549]:  i  :  size(byte)
*/
{ ADFI_745_SIZE_BYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SIZE_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_BYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [550]:  i  :  size(vbyte)
*/
{ ADFI_746_SIZE_VBYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SIZE_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_VBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [551]:  i  :  size(lbyte)
*/
{ ADFI_747_SIZE_LBYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SIZE_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_LBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [552]:  i  :  size(nchar)
*/
{ ADFI_1044_SIZE_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SIZE_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [553]:  i  :  size(nvarchar)
*/
{ ADFI_1045_SIZE_NVCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SIZE_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [554]:  i  :  size(long_nvarchar)
*/
{ ADFI_1201_SIZE_LONGNVCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SIZE_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_LNVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [555]:  f  :  sqrt(f)
*/
{ ADFI_182_SQRT_F, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SQRT_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [556]:  text  :  squeeze(c)
*/
{ ADFI_445_SQUEEZE_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SQZ_OP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [557]:  varchar  :  squeeze(char)
*/
{ ADFI_315_SQUEEZE_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SQZ_OP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [558]:  text  :  squeeze(text)
*/
{ ADFI_183_SQUEEZE_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SQZ_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [559]:  varchar  :  squeeze(varchar)
*/
{ ADFI_316_SQUEEZE_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SQZ_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [560]:  nvarchar  :  squeeze(nchar)
*/
{ ADFI_1046_SQUEEZE_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SQZ_OP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [561]:  nvarchar  :  squeeze(nvarchar)
*/
{ ADFI_1047_SQUEEZE_NVARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SQZ_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [562]:  text  :  text(all)
*/
{ ADFI_557_TEXT_ALL, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI|ADI_F64_ALLSKIPUNI, ADI_TEXT_OP,
  {ADI_LEN_INDIRECT, 557, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [563]:  text  :  text(nchar)
*/
{ ADFI_1174_TEXT_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_TEXT_OP,
  {ADI_UTOVARC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [564]:  text  :  text(nvarchar)
*/
{ ADFI_1175_TEXT_NVCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_TEXT_OP,
  {ADI_UTOC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [565]:  text  :  text(all, i)
*/
{ ADFI_587_TEXT_ALL_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI|ADI_F64_ALLSKIPUNI, ADI_TEXT_OP,
  {ADI_LEN_INDIRECT, 587, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_TXT_TYPE,
    DB_ALL_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [566]:  text  :  trim(c)
*/
{ ADFI_190_TRIM_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_TRIM_OP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [567]:  varchar  :  trim(char)
*/
{ ADFI_319_TRIM_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_TRIM_OP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [568]:  text  :  trim(text)
*/
{ ADFI_191_TRIM_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_TRIM_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [569]:  varchar  :  trim(varchar)
*/
{ ADFI_320_TRIM_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_TRIM_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [570]:  nvarchar  :  trim(nchar)
*/
{ ADFI_1018_TRIM_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_TRIM_OP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [571]:  nvarchar  :  trim(nvarchar)
*/
{ ADFI_1019_TRIM_NVCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_TRIM_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [572]:  c  :  uppercase(c)
*/
{ ADFI_192_UPPERCASE_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_UPPER_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHR_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [573]:  char  :  uppercase(char)
*/
{ ADFI_321_UPPERCASE_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_UPPER_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [574]:  text  :  uppercase(text)
*/
{ ADFI_193_UPPERCASE_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_UPPER_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [575]:  varchar  :  uppercase(varchar)
*/
{ ADFI_322_UPPERCASE_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_UPPER_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [576]:  lvch  :  uppercase(lvch)
*/
{ ADFI_711_UPPERCASE_LVCH, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F4_WORKSPACE, ADI_UPPER_OP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  1, DB_LVCH_TYPE,
    DB_LVCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [577]:  nchar  :  uppercase(nchar)
*/
{ ADFI_1048_UPPERCASE_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_UPPER_OP,
  {ADI_O1UNIDBL, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NCHR_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [578]:  nvarchar  :  uppercase(nvarchar)
*/
{ ADFI_1049_UPPERCASE_NVCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_UPPER_OP,
  {ADI_O1UNIDBL, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [579]:  lvch  :  uppercase(long nvarchar)
*/
{ ADFI_1282_UPPERCASE_LNVCHR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F4_WORKSPACE, ADI_UPPER_OP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  1, DB_LNVCHR_TYPE,
    DB_LNVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [580]:  varchar  :  uppercase(i)
*/
{ ADFI_1297_UPPERCASE_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_UPPER_OP,
  {ADI_LEN_INDIRECT, 558, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [581]:  i  :  iiuserlen(i,i)
*/
{ ADFI_400_IIUSERLEN_I_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_USRLN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_INT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [582]:  c  :  username()
*/
{ ADFI_212_USERNAME, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_USRNM_OP,
  {ADI_FIXED, DB_MAXNAME, 0}, 0, ADE_0CXI_IGNORE,
  0, DB_CHR_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [583]:  varchar  :  varchar(all)
*/
{ ADFI_558_VARCHAR_ALL, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F64_ALLSKIPUNI, ADI_VARCH_OP,
  {ADI_LEN_INDIRECT, 558, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [584]:  varchar  :  varchar(nchar)
*/
{ ADFI_1172_VARCHAR_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_VARCH_OP,
  {ADI_UTOVARC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [585]:  varchar  :  varchar(nvarchar)
*/
{ ADFI_1173_VARCHAR_NVCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_VARCH_OP,
  {ADI_UTOC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [586]:  varchar  :  varchar(all, i)
*/
{ ADFI_616_VARCHAR_ALL_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F64_ALLSKIPUNI, ADI_VARCH_OP,
  {ADI_LEN_INDIRECT, 616, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VCH_TYPE,
    DB_ALL_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [587]:  varchar  :  hex(char)
*/
{ ADFI_368_HEX_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_HEX_OP,
  {ADI_2XP2, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [588]:  varchar  :  hex(text)
*/
{ ADFI_369_HEX_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_HEX_OP,
  {ADI_2XM2, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [589]:  varchar  :  hex(varchar)
*/
{ ADFI_370_HEX_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_HEX_OP,
  {ADI_2XM2, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [590]:  varchar  :  hex(all)
*/
{ ADFI_371_HEX_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_HEX_OP,
  {ADI_2XP2, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [591]:  c  :  ifnull(c,c)
*/
{ ADFI_372_IFNULL_C_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_IFNUL_OP,
  {ADI_LONGER, 0, 0}, 0, ADE_0CXI_IGNORE,
  2, DB_CHR_TYPE,
    DB_CHR_TYPE, DB_CHR_TYPE, DB_NODT, DB_NODT
},
/*
** [592]:  char  :  ifnull(char,char)
*/
{ ADFI_373_IFNULL_CHAR_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_IFNUL_OP,
  {ADI_LONGER, 0, 0}, 0, ADE_0CXI_IGNORE,
  2, DB_CHA_TYPE,
    DB_CHA_TYPE, DB_CHA_TYPE, DB_NODT, DB_NODT
},
/*
** [593]:  date  :  ifnull(date,date)
*/
{ ADFI_374_IFNULL_DATE_DATE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1024_RES2NDARG, ADI_IFNUL_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_0CXI_IGNORE,
  2, DB_DTE_TYPE,
    DB_DTE_TYPE, DB_DTE_TYPE, DB_NODT, DB_NODT
},
/*
** [594]:  f  :  ifnull(f,f)
*/
{ ADFI_375_IFNULL_F_F, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_IFNUL_OP,
  {ADI_LONGER, 0, 0}, 0, ADE_0CXI_IGNORE,
  2, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [595]:  dec  :  ifnull(dec,dec)
*/
{ ADFI_551_IFNULL_DEC_DEC, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_IFNUL_OP,
  {ADI_DECBLEND, 0, 0}, 0, ADE_0CXI_IGNORE,
  2, DB_DEC_TYPE,
    DB_DEC_TYPE, DB_DEC_TYPE, DB_NODT, DB_NODT
},
/*
** [596]:  i  :  ifnull(i,i)
*/
{ ADFI_376_IFNULL_I_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_IFNUL_OP,
  {ADI_LONGER, 0, 0}, 0, ADE_0CXI_IGNORE,
  2, DB_INT_TYPE,
    DB_INT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [597]:  money  :  ifnull(money,money)
*/
{ ADFI_377_IFNULL_MONEY_MONEY, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_IFNUL_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_0CXI_IGNORE,
  2, DB_MNY_TYPE,
    DB_MNY_TYPE, DB_MNY_TYPE, DB_NODT, DB_NODT
},
/*
** [598]:  text  :  ifnull(text,text)
*/
{ ADFI_378_IFNULL_TEXT_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_IFNUL_OP,
  {ADI_LONGER, 0, 0}, 0, ADE_0CXI_IGNORE,
  2, DB_TXT_TYPE,
    DB_TXT_TYPE, DB_TXT_TYPE, DB_NODT, DB_NODT
},
/*
** [599]:  varchar  :  ifnull(varchar,varchar)
*/
{ ADFI_379_IFNULL_VARCHAR_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_IFNUL_OP,
  {ADI_LONGER, 0, 0}, 0, ADE_0CXI_IGNORE,
  2, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_VCH_TYPE, DB_NODT, DB_NODT
},
/*
** [600]:  nchar  :  ifnull(nchar,nchar)
*/
{ ADFI_1078_IFNULL_NCHAR_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_IFNUL_OP,
  {ADI_LONGER, 0, 0}, 0, ADE_0CXI_IGNORE,
  2, DB_NCHR_TYPE,
    DB_NCHR_TYPE, DB_NCHR_TYPE, DB_NODT, DB_NODT
},
/*
** [601]:  nvarchar  :  ifnull(nvarchar,nvarchar)
*/
{ ADFI_1079_IFNULL_NVARCHAR_NVARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_IFNUL_OP,
  {ADI_LONGER, 0, 0}, 0, ADE_0CXI_IGNORE,
  2, DB_NVCHR_TYPE,
    DB_NVCHR_TYPE, DB_NVCHR_TYPE, DB_NODT, DB_NODT
},
/*
** [602]:  char  :  iistructure(i)
*/
{ ADFI_399_IISTRUCTURE_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_STRUC_OP,
  {ADI_FIXED, 16, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [603]:  char  :  date_gmt(date)
*/
{ ADFI_401_DATE_GMT_DATE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_DGMT_OP,
  {ADI_FIXED, 25, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [604]:  char  :  iichar12(c)
*/
{ ADFI_402_IICHAR12_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CHA12_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [605]:  char  :  iichar12(text)
*/
{ ADFI_403_IICHAR12_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CHA12_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [606]:  char  :  iichar12(char)
*/
{ ADFI_404_IICHAR12_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CHA12_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [607]:  char  :  iichar12(varchar)
*/
{ ADFI_405_IICHAR12_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CHA12_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [608]:  varchar  :  charextract(c,i)
*/
{ ADFI_890_CHAREXTRACT_C_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CHEXT_OP,
  {ADI_FIXED, 4+DB_CNTSIZE, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VCH_TYPE,
    DB_CHR_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [609]:  varchar  :  charextract(text,i)
*/
{ ADFI_891_CHAREXTRACT_TEXT_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CHEXT_OP,
  {ADI_FIXED, 4+DB_CNTSIZE, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VCH_TYPE,
    DB_TXT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [610]:  varchar  :  charextract(char,i)
*/
{ ADFI_892_CHAREXTRACT_CHAR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CHEXT_OP,
  {ADI_FIXED, 4+DB_CNTSIZE, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VCH_TYPE,
    DB_CHA_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [611]:  varchar  :  charextract(varchar,i)
*/
{ ADFI_893_CHAREXTRACT_VARCHAR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CHEXT_OP,
  {ADI_FIXED, 4+DB_CNTSIZE, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [612]:  nchar  :  charextract(nchar,i)
*/
{ ADFI_1050_CHAREXTRACT_NCHAR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CHEXT_OP,
  {ADI_FIXED, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_NCHR_TYPE,
    DB_NCHR_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [613]:  nchar  :  charextract(nvarchar,i)
*/
{ ADFI_1051_CHAREXTRACT_NVCHAR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CHEXT_OP,
  {ADI_FIXED, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_NCHR_TYPE,
    DB_NVCHR_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [614]:  text  :  ii_notrm_txt(char)
*/
{ ADFI_414_II_NOTRM_TXT_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_NTRMT_OP,
  {ADI_RSLNO1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [615]:  varchar  :  ii_notrm_vch(char)
*/
{ ADFI_415_II_NOTRM_VCH_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_NTRMV_OP,
  {ADI_RSLNO1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [616]:  varchar  :  xyzzy(all)
*/
{ ADFI_433_XYZZY_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_XYZZY_OP,
  {ADI_FIXED, 32, 0}, 0, ADE_0CXI_IGNORE,
  1, DB_VCH_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [617]:  i  :  iihexint(text)
*/
{ ADFI_438_IIHEXINT_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_HXINT_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [618]:  i  :  iihexint(varchar)
*/
{ ADFI_439_IIHEXINT_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_HXINT_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [619]:  char  :  ii_tabid_di(i,i)
*/
{ ADFI_443_II_TABID_DI_I_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_TB2DI_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHA_TYPE,
    DB_INT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [620]:  i  :  ii_di_tabid(char)
*/
{ ADFI_444_II_DI_TABID_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_DI2TB_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [621]:  varchar  :  ii_dv_desc(alltype)
*/
{ ADFI_654_II_DV_DESC_ALLTYPE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_DVDSC_OP,
  {ADI_FIXED, 64, 0}, 0, ADE_0CXI_IGNORE,
  1, DB_VCH_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [622]:  dec  :  decimal(c,i)
*/
{ ADFI_541_DEC_C_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_DEC_OP,
  {ADI_DECFUNC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DEC_TYPE,
    DB_CHR_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [623]:  dec  :  decimal(char,i)
*/
{ ADFI_542_DEC_CHAR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_DEC_OP,
  {ADI_DECFUNC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DEC_TYPE,
    DB_CHA_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [624]:  dec  :  decimal(dec,i)
*/
{ ADFI_543_DEC_DEC_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_DEC_OP,
  {ADI_DECFUNC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DEC_TYPE,
    DB_DEC_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [625]:  dec  :  decimal(f,i)
*/
{ ADFI_544_DEC_F_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_DEC_OP,
  {ADI_DECFUNC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DEC_TYPE,
    DB_FLT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [626]:  dec  :  decimal(i,i)
*/
{ ADFI_545_DEC_I_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_DEC_OP,
  {ADI_DECFUNC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DEC_TYPE,
    DB_INT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [627]:  dec  :  decimal(money,i)
*/
{ ADFI_546_DEC_MONEY_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_DEC_OP,
  {ADI_DECFUNC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DEC_TYPE,
    DB_MNY_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [628]:  dec  :  decimal(text,i)
*/
{ ADFI_547_DEC_TEXT_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_DEC_OP,
  {ADI_DECFUNC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DEC_TYPE,
    DB_TXT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [629]:  dec  :  decimal(varchar,i)
*/
{ ADFI_548_DEC_VARCHAR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_DEC_OP,
  {ADI_DECFUNC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DEC_TYPE,
    DB_VCH_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [630]:  dec  :  decimal(nchar,i)
*/
{ ADFI_1168_DEC_NCHAR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_DEC_OP,
  {ADI_DECFUNC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DEC_TYPE,
    DB_NCHR_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [631]:  dec  :  decimal(nvarchar,i)
*/
{ ADFI_1169_DEC_NVCHAR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_DEC_OP,
  {ADI_DECFUNC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DEC_TYPE,
    DB_NVCHR_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [632]:  int  :  ii_ext_typename(i, i)
*/
{ ADFI_504_II_EXT_TYPE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_ETYPE_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_INT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [633]:  int	  :  ii_ext_length(i, i)
*/
{ ADFI_505_II_EXT_LENGTH, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_ELENGTH_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_INT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [634]:  logical_key  :  logical_key(char)
*/
{ ADFI_474_LOGKEY_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LOGKEY_OP,
  {ADI_FIXED, DB_LEN_OBJ_LOGKEY, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LOGKEY_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [635]:  logical_key  :  logical_key(logical_key)
*/
{ ADFI_475_LOGKEY_LOGKEY, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LOGKEY_OP,
  {ADI_FIXED, DB_LEN_OBJ_LOGKEY, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LOGKEY_TYPE,
    DB_LOGKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [636]:  table_key  :  table_key(char)
*/
{ ADFI_493_TABKEY_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_TABKEY_OP,
  {ADI_FIXED, DB_LEN_TAB_LOGKEY, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TABKEY_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [637]:  table_key  :  table_key(table_key)
*/
{ ADFI_494_TABKEY_TABKEY, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_TABKEY_OP,
  {ADI_FIXED, DB_LEN_TAB_LOGKEY, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TABKEY_TYPE,
    DB_TABKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [638]:  all  :  ii_iftrue(i,all)
*/
{ ADFI_604_II_IFTRUE_I_ALL, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_IFTRUE_OP,
  {ADI_O2, 0, 0}, 0, ADE_0CXI_IGNORE,
  2, DB_ALL_TYPE,
    DB_INT_TYPE, DB_ALL_TYPE, DB_NODT, DB_NODT
},
/*
** [639]:  varchar  :  resolve_table(text, text)
*/
{ ADFI_605_RESTAB_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_RESTAB_OP,
  {ADI_FIXED, 2*DB_GW1_MAXNAME+DB_CNTSIZE, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VCH_TYPE,
    DB_TXT_TYPE, DB_TXT_TYPE, DB_NODT, DB_NODT
},
/*
** [640]:  varchar  :  resolve_table(varchar, varchar)
*/
{ ADFI_606_RESTAB_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_RESTAB_OP,
  {ADI_FIXED, 2*DB_GW1_MAXNAME+DB_CNTSIZE, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_VCH_TYPE, DB_NODT, DB_NODT
},
/*
** [641]:  char  :  gmt_timestamp(i)
*/
{ ADFI_615_GMT_TIMESTAMP_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_GMTSTAMP_OP,
  {ADI_FIXED, 25, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [642]:  varchar   :  ii_cpn_dump(long_varchar)
*/
{ ADFI_619_CPNDMP_LVCH, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CPNDMP_OP,
  {ADI_FIXED, 100, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_LVCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [643]:  bit  :  bit(varchar)
*/
{ ADFI_683_BIT_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BIT_OP,
  {ADI_CHAR2BIT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BIT_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [644]:  bit  :  bit(char)
*/
{ ADFI_684_BIT_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BIT_OP,
  {ADI_CHAR2BIT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BIT_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [645]:  bit  :  bit(bit)
*/
{ ADFI_685_BIT_BIT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BIT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BIT_TYPE,
    DB_BIT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [646]:  bit  :  bit(vbit)
*/
{ ADFI_686_BIT_VBIT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BIT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BIT_TYPE,
    DB_VBIT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [647]:  vbit  :  vbit(varchar)
*/
{ ADFI_687_VBIT_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_VBIT_OP,
  {ADI_CHAR2BIT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBIT_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [648]:  vbit  :  vbit(char)
*/
{ ADFI_688_VBIT_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_VBIT_OP,
  {ADI_CHAR2BIT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBIT_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [649]:  vbit  :  vbit(bit)
*/
{ ADFI_689_VBIT_BIT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_VBIT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBIT_TYPE,
    DB_BIT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [650]:  vbit  :  vbit(vbit)
*/
{ ADFI_690_VBIT_VBIT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_VBIT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBIT_TYPE,
    DB_VBIT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [651]:  i  :  ii_row_count(i,i)
*/
{ ADFI_595_ROWCNT_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_ROWCNT_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_INT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [652]:  char  :  session_user()
*/
{ ADFI_618_SESSUSER, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_SESSUSER_OP,
  {ADI_FIXED, DB_MAXNAME, 0}, 0, ADE_0CXI_IGNORE,
  0, DB_CHA_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [653]:  char  :  system_user()
*/
{ ADFI_596_SYSUSER, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_SYSUSER_OP,
  {ADI_FIXED, DB_MAXNAME, 0}, 0, ADE_0CXI_IGNORE,
  0, DB_CHA_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [654]:  char  :  initial_user()
*/
{ ADFI_651_INITUSER, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_INITUSER_OP,
  {ADI_FIXED, DB_MAXNAME, 0}, 0, ADE_0CXI_IGNORE,
  0, DB_CHA_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [655]:  i  :  iitotal_allocated_pages(i,i)
*/
{ ADFI_652_IITOTAL_ALLOC_PG, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_ALLOCPG_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_INT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [656]:  i  :  iitotal_overflow_pages(i,i)
*/
{ ADFI_653_IITOTAL_OVERFL_PG, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_OVERPG_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_INT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [657]:  byte  :  byte(char)
*/
{ ADFI_748_BYTE_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BYTE_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BYTE_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [658]:  byte  :  byte(varchar)
*/
{ ADFI_768_BYTE_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BYTE_OP,
  {ADI_O1TC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BYTE_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [659]:  byte  :  byte(byte)
*/
{ ADFI_769_BYTE_BYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BYTE_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BYTE_TYPE,
    DB_BYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [660]:  byte  :  byte(vbyte)
*/
{ ADFI_770_BYTE_VBYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BYTE_OP,
  {ADI_O1TC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BYTE_TYPE,
    DB_VBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [661]:  byte  :  byte(all)
*/
{ ADFI_979_BYTE_ALL, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F64_ALLSKIPUNI, ADI_BYTE_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BYTE_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [662]:  byte  :  byte(char,i)
*/
{ ADFI_771_BYTE_CHAR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BYTE_OP,
  {ADI_LEN_INDIRECT, 771, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_BYTE_TYPE,
    DB_CHA_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [663]:  byte  :  byte(varchar,i)
*/
{ ADFI_772_BYTE_VARCHAR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BYTE_OP,
  {ADI_LEN_INDIRECT, 772, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_BYTE_TYPE,
    DB_VCH_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [664]:  byte  :  byte(byte,i)
*/
{ ADFI_773_BYTE_BYTE_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BYTE_OP,
  {ADI_LEN_INDIRECT, 773, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_BYTE_TYPE,
    DB_BYTE_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [665]:  byte  :  byte(vbyte,i)
*/
{ ADFI_774_BYTE_VBYTE_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BYTE_OP,
  {ADI_LEN_INDIRECT, 774, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_BYTE_TYPE,
    DB_VBYTE_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [666]:  vbyte  :  varbyte(varchar)
*/
{ ADFI_749_VARBYTE_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_VBYTE_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBYTE_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [667]:  vbyte  :  varbyte(char)
*/
{ ADFI_775_VARBYTE_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_VBYTE_OP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBYTE_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [668]:  vbyte  :  varbyte(byte)
*/
{ ADFI_776_VARBYTE_BYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_VBYTE_OP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBYTE_TYPE,
    DB_BYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [669]:  vbyte  :  varbyte(vbyte)
*/
{ ADFI_777_VARBYTE_VBYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_VBYTE_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBYTE_TYPE,
    DB_VBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [670]:  vbyte  :  varbyte(all)
*/
{ ADFI_980_VBYTE_ALL, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F64_ALLSKIPUNI, ADI_VBYTE_OP,
  {ADI_LEN_INDIRECT, 980, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBYTE_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [671]:  vbyte  :  varbyte(char,i)
*/
{ ADFI_778_VARBYTE_CHAR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_VBYTE_OP,
  {ADI_LEN_INDIRECT, 778, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VBYTE_TYPE,
    DB_CHA_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [672]:  vbyte  :  varbyte(varchar,i)
*/
{ ADFI_779_VARBYTE_VARCHAR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_VBYTE_OP,
  {ADI_LEN_INDIRECT, 779, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VBYTE_TYPE,
    DB_VCH_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [673]:  vbyte  :  varbyte(byte,i)
*/
{ ADFI_780_VARBYTE_BYTE_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_VBYTE_OP,
  {ADI_LEN_INDIRECT, 780, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VBYTE_TYPE,
    DB_BYTE_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [674]:  vbyte  :  varbyte(vbyte,i)
*/
{ ADFI_781_VARBYTE_VBYTE_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_VBYTE_OP,
  {ADI_LEN_INDIRECT, 781, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VBYTE_TYPE,
    DB_VBYTE_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [675]:  varchar  :  varbyte(all, i)
*/
{ ADFI_1280_VARBYTE_ALL_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F64_ALLSKIPUNI, ADI_VBYTE_OP,
  {ADI_LEN_INDIRECT, 1280, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VBYTE_TYPE,
    DB_ALL_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [676]:  table_key  :  ii_lolk(all)
*/
{ ADFI_750_II_LOLK_LVCH, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LOLK_OP,
  {ADI_FIXED, DB_LEN_TAB_LOGKEY, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TABKEY_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [677]:  table_key  :  ii_lolk(lbyte)
*/
{ ADFI_751_II_LOLK_LBYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LOLK_OP,
  {ADI_FIXED, DB_LEN_TAB_LOGKEY, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TABKEY_TYPE,
    DB_LBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [678]:  char	   : iipermittype(int, int)
*/
{ ADFI_789_II_PERM_TYPE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_PTYPE_OP,
  {ADI_FIXED, 2*DB_MAXNAME, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHA_TYPE,
    DB_INT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [679]:  char  :  soundex(c)
*/
{ ADFI_790_SOUNDEX_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SOUNDEX_OP,
  {ADI_FIXED, AD_LEN_SOUNDEX, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [680]:  char  :  soundex(char)
*/
{ ADFI_791_SOUNDEX_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SOUNDEX_OP,
  {ADI_FIXED, AD_LEN_SOUNDEX, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [681]:  char  :  soundex(text)
*/
{ ADFI_792_SOUNDEX_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SOUNDEX_OP,
  {ADI_FIXED, AD_LEN_SOUNDEX, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [682]:  char  :  soundex(varchar)
*/
{ ADFI_793_SOUNDEX_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SOUNDEX_OP,
  {ADI_FIXED, AD_LEN_SOUNDEX, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [683]:  char  :  soundex(byte)
*/
{ ADFI_794_SOUNDEX_BYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SOUNDEX_OP,
  {ADI_FIXED, AD_LEN_SOUNDEX, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_BYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [684]:  char  :  soundex(vbyte)
*/
{ ADFI_795_SOUNDEX_VBYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SOUNDEX_OP,
  {ADI_FIXED, AD_LEN_SOUNDEX, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_VBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [685]:  c  :  _date4(i)
*/
{ ADFI_981__DATE4_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_BDATE4_OP,
  {ADI_FIXED, 11, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHR_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [686]:  int2  :  intextract(byte,i)
*/
{ ADFI_805_INTEXTRACT_BYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_INTEXT_OP,
  {ADI_FIXED, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_BYTE_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [687]:  int2  :  intextract(varbyte,i)
*/
{ ADFI_806_INTEXTRACT_VARBYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_INTEXT_OP,
  {ADI_FIXED, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_VBYTE_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [688]:  i  :  isdst(date)
*/
{ ADFI_1202_ISDST_DATE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_ISDST_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [689]:  ansichar  :  current_date()
*/
{ ADFI_1233_CURRENT_DATE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_CURDATE_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_0CXI_IGNORE,
  0, DB_ADTE_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [690]:  timewtz :  current_time()
*/
{ ADFI_1234_CURRENT_TIME, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_CURTIME_OP,
  {ADI_FIXED, 10, 0}, 0, ADE_0CXI_IGNORE,
  0, DB_TMW_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [691]:  tmstmpwtz  :  current_timestamp()
*/
{ ADFI_1235_CURRENT_TMSTMP, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_CURTMSTMP_OP,
  {ADI_FIXED, 14, 0}, 0, ADE_0CXI_IGNORE,
  0, DB_TSW_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [692]:  timewotz :  local_time()
*/
{ ADFI_1236_LOCAL_TIME, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_LCLTIME_OP,
  {ADI_FIXED, 10, 0}, 0, ADE_0CXI_IGNORE,
  0, DB_TMWO_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [693]:  tmstmpwotz :  local_timestamp()
*/
{ ADFI_1237_LOCAL_TMSTMP, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_LCLTMSTMP_OP,
  {ADI_FIXED, 14, 0}, 0, ADE_0CXI_IGNORE,
  0, DB_TSWO_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [694]:  char  :  session_priv(all)
*/
{ ADFI_954_SESSION_PRIV, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SESSION_PRIV_OP,
  {ADI_FIXED, 32, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [695]:  int  :  iitblstat(int)
*/
{ ADFI_955_IITBLSTAT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_IITBLSTAT_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [696]:  long_varchar  :  long_varchar(char)
*/
{ ADFI_1088_LVCH_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LONG_VARCHAR,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LVCH_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [697]:  long_varchar  :  long_varchar(c)
*/
{ ADFI_1089_LVCH_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LONG_VARCHAR,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LVCH_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [698]:  long_varchar  :  long_varchar(text)
*/
{ ADFI_1090_LVCH_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LONG_VARCHAR,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LVCH_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [699]:  long_varchar  :  long_varchar(varchar)
*/
{ ADFI_1091_LVCH_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LONG_VARCHAR,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LVCH_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [700]:  long_varchar  :  long_varchar(long_varchar)
*/
{ ADFI_1092_LVCH_LVCH, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LONG_VARCHAR,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LVCH_TYPE,
    DB_LVCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [701]:  long_varchar  :  long_varchar(longtext)
*/
{ ADFI_1093_LVCH_LTXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LONG_VARCHAR,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LVCH_TYPE,
    DB_LTXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [702]:  long_varchar  :  long_varchar(lbyte)
*/
{ ADFI_1094_LVCH_LBYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LONG_VARCHAR,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LVCH_TYPE,
    DB_LBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [703]:  lbyte  : lbye(c)
*/
{ ADFI_1095_LBYTE_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LONG_BYTE,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LBYTE_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [704]:  lbyte  :  lbyte(char)
*/
{ ADFI_1096_LBYTE_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LONG_BYTE,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LBYTE_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [705]:  lbyte  :  lbyte(varchar)
*/
{ ADFI_1097_LBYTE_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LONG_BYTE,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LBYTE_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [706]:  lbyte  :  lbyte(text)
*/
{ ADFI_1098_LBYTE_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LONG_BYTE,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LBYTE_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [707]:  lbyte  :  lbyte(longtext)
*/
{ ADFI_1099_LBYTE_LONGTEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LONG_BYTE,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LBYTE_TYPE,
    DB_LTXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [708]:  lbyte  :  lbyte(byte)
*/
{ ADFI_1100_LBYTE_BYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LONG_BYTE,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LBYTE_TYPE,
    DB_BYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [709]:  lbyte  :  lbyte(varbyte)
*/
{ ADFI_1101_LBYTE_VARBYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LONG_BYTE,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LBYTE_TYPE,
    DB_VBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [710]:  lbyte  :  lbyte(lbyte)
*/
{ ADFI_1102_LBYTE_LBYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LONG_BYTE,
  {ADI_RESLEN, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LBYTE_TYPE,
    DB_LBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [711]:  lbyte  :  lbyte(lvch)
*/
{ ADFI_1103_LBYTE_LVCH, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LONG_BYTE,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LBYTE_TYPE,
    DB_LVCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [712]:  lbyte  :  lbyte(nchar)
*/
{ ADFI_1104_LBYTE_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LONG_BYTE,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LBYTE_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [713]:  lbyte  :  lbyte(nvarchar)
*/
{ ADFI_1105_LBYTE_NVCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LONG_BYTE,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LBYTE_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [714]:  byte  :  bit_add(byte, byte)
*/
{ ADFI_796_BIT_ADD_BYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BIT_ADD_OP,
  {ADI_LONGER, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_BYTE_TYPE,
    DB_BYTE_TYPE, DB_BYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [715]:  byte  :  bit_and(byte, byte)
*/
{ ADFI_797_BIT_AND_BYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BIT_AND_OP,
  {ADI_LONGER, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_BYTE_TYPE,
    DB_BYTE_TYPE, DB_BYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [716]:  byte  :  bit_not(byte)
*/
{ ADFI_798_BIT_NOT_BYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BIT_NOT_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BYTE_TYPE,
    DB_BYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [717]:  byte  :  bit_or(byte, byte)
*/
{ ADFI_799_BIT_OR_BYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BIT_OR_OP,
  {ADI_LONGER, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_BYTE_TYPE,
    DB_BYTE_TYPE, DB_BYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [718]:  byte  :  bit_xor(byte, byte)
*/
{ ADFI_800_BIT_XOR_BYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BIT_XOR_OP,
  {ADI_LONGER, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_BYTE_TYPE,
    DB_BYTE_TYPE, DB_BYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [719]:  byte  :  ii_ipaddr(c)
*/
{ ADFI_801_IPADDR_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F4096_SQL_CLOAKED|ADI_F8192_QUEL_CLOAKED, ADI_IPADDR_OP,
  {ADI_FIXED, AD_LEN_IPV4ADDR, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BYTE_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [720]:  byte  :  ii_ipaddr(char)
*/
{ ADFI_802_IPADDR_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F4096_SQL_CLOAKED|ADI_F8192_QUEL_CLOAKED, ADI_IPADDR_OP,
  {ADI_FIXED, AD_LEN_IPV4ADDR, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BYTE_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [721]:  byte  :  ii_ipaddr(text)
*/
{ ADFI_803_IPADDR_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F4096_SQL_CLOAKED|ADI_F8192_QUEL_CLOAKED, ADI_IPADDR_OP,
  {ADI_FIXED, AD_LEN_IPV4ADDR, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BYTE_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [722]:  byte  :  ii_ipaddr(varchar)
*/
{ ADFI_804_IPADDR_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F4096_SQL_CLOAKED|ADI_F8192_QUEL_CLOAKED, ADI_IPADDR_OP,
  {ADI_FIXED, AD_LEN_IPV4ADDR, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BYTE_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [723]:  varbyte :  ii_ipaddr(c)
*/
{ ADFI_1353_IPADDR_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_IPADDR_OP,
  {ADI_FIXED, AD_LEN_IPADDR+DB_CNTSIZE, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBYTE_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [724]:  varbyte :  ii_ipaddr(char)
*/
{ ADFI_1354_IPADDR_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_IPADDR_OP,
  {ADI_FIXED, AD_LEN_IPADDR+DB_CNTSIZE, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBYTE_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [725]:  varbyte :  ii_ipaddr(text)
*/
{ ADFI_1355_IPADDR_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_IPADDR_OP,
  {ADI_FIXED, AD_LEN_IPADDR+DB_CNTSIZE, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBYTE_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [726]:  varbyte :  ii_ipaddr(varchar)
*/
{ ADFI_1356_IPADDR_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_IPADDR_OP,
  {ADI_FIXED, AD_LEN_IPADDR+DB_CNTSIZE, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBYTE_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [727]:  int4  :  hash(all)
*/
{ ADFI_807_HASH_ALL, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_HASH_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [728]:  f  :  randomf(void)
*/
{ ADFI_808_RANDOMF_VOID, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F32_NOTVIRGIN, ADI_RANDOMF_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  0, DB_FLT_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [729]:  i  :  randomf(i, i)
*/
{ ADFI_809_RANDOMF_INT_INT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F32_NOTVIRGIN, ADI_RANDOMF_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_INT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [730]:  f  :  randomf(f, f)
*/
{ ADFI_810_RANDOMF_FLT_FLT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F32_NOTVIRGIN, ADI_RANDOMF_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [731]:  int  :  random(void)
*/
{ ADFI_811_RANDOM_VOID, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F32_NOTVIRGIN, ADI_RANDOM_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  0, DB_INT_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [732]:  int :  random(i, i)
*/
{ ADFI_812_RANDOM_INT_INT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F32_NOTVIRGIN, ADI_RANDOM_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_INT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [733]:  byte  :  uuid_create(void)
*/
{ ADFI_813_UUID_CREATE_VOID, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F32_NOTVIRGIN, ADI_UUID_CREATE_OP,
  {ADI_FIXED, AD_LEN_UUID, 0}, 0, ADE_1CXI_SET_SKIP,
  0, DB_BYTE_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [734]:  char :  uuid_to_char(byte)
*/
{ ADFI_814_UUID_TO_CHAR_BYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_UUID_TO_CHAR_OP,
  {ADI_FIXED, AD_LEN_CHR_UUID, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_BYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [735]:  byte :  uuid_from_char(c)
*/
{ ADFI_815_UUID_FROM_CHAR_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_UUID_FROM_CHAR_OP,
  {ADI_FIXED, AD_LEN_UUID, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BYTE_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [736]:  byte :  uuid_from_char(char)
*/
{ ADFI_816_UUID_FROM_CHAR_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_UUID_FROM_CHAR_OP,
  {ADI_FIXED, AD_LEN_UUID, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BYTE_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [737]:  byte :  uuid_from_char(text)
*/
{ ADFI_817_UUID_FROM_CHAR_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_UUID_FROM_CHAR_OP,
  {ADI_FIXED, AD_LEN_UUID, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BYTE_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [738]:  byte :  uuid_from_char(varchar)
*/
{ ADFI_818_UUID_FROM_CHAR_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_UUID_FROM_CHAR_OP,
  {ADI_FIXED, AD_LEN_UUID, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BYTE_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [739]:  int :  uuid_compare(byte,byte)
*/
{ ADFI_819_UUID_COMPARE_BYTE_BYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_UUID_COMPARE_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_BYTE_TYPE, DB_BYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [740]:  varchar  :  substring(c, int)
*/
{ ADFI_820_SUBSTRING_C_INT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SUBSTRING_OP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VCH_TYPE,
    DB_CHR_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [741]:  varchar  :  substring(char, int)
*/
{ ADFI_821_SUBSTRING_CHAR_INT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SUBSTRING_OP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VCH_TYPE,
    DB_CHA_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [742]:  varchar  :  substring(text, int)
*/
{ ADFI_822_SUBSTRING_TEXT_INT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SUBSTRING_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VCH_TYPE,
    DB_TXT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [743]:  varchar  :  substring(varchar, int)
*/
{ ADFI_823_SUBSTRING_VARCHAR_INT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SUBSTRING_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [744]:  nvarchar  :  substring(nchar, int)
*/
{ ADFI_1052_SUBSTRING_NCHAR_INT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SUBSTRING_OP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_NVCHR_TYPE,
    DB_NCHR_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [745]:  nvarchar  :  substring(nvarchar, int)
*/
{ ADFI_1053_SUBSTRING_NVCHAR_INT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SUBSTRING_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_NVCHR_TYPE,
    DB_NVCHR_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [746]:  varchar  :  substring(c, int, int)
*/
{ ADFI_824_SUBSTRING_C_INT_INT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SUBSTRING_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  3, DB_VCH_TYPE,
    DB_CHR_TYPE, DB_INT_TYPE, DB_INT_TYPE, DB_NODT
},
/*
** [747]:  varchar  :  substring(char, int, int)
*/
{ ADFI_825_SUBSTRING_CHAR_INT_INT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SUBSTRING_OP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  3, DB_VCH_TYPE,
    DB_CHA_TYPE, DB_INT_TYPE, DB_INT_TYPE, DB_NODT
},
/*
** [748]:  varchar  :  substring(text, int, int)
*/
{ ADFI_826_SUBSTRING_TEXT_INT_INT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SUBSTRING_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  3, DB_VCH_TYPE,
    DB_TXT_TYPE, DB_INT_TYPE, DB_INT_TYPE, DB_NODT
},
/*
** [749]:  varchar  :  substring(varchar, int, int)
*/
{ ADFI_827_SUBSTRING_VARCHAR_INT_INT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SUBSTRING_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  3, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_INT_TYPE, DB_INT_TYPE, DB_NODT
},
/*
** [750]:  nvarchar  :  substring(nchar, int, int)
*/
{ ADFI_1054_SUBSTRING_NCHAR_INT_INT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SUBSTRING_OP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  3, DB_NVCHR_TYPE,
    DB_NCHR_TYPE, DB_INT_TYPE, DB_INT_TYPE, DB_NODT
},
/*
** [751]:  nvarchar  :  substring(nvarchar, int, int)
*/
{ ADFI_1055_SUBSTRING_NVCHAR_INT_INT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SUBSTRING_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  3, DB_NVCHR_TYPE,
    DB_NVCHR_TYPE, DB_INT_TYPE, DB_INT_TYPE, DB_NODT
},
/*
** [752]:  lvch  :  substring(lvch,int)
*/
{ ADFI_1219_SUBSTRING_LVCH_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F4_WORKSPACE, ADI_SUBSTRING_OP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  2, DB_LVCH_TYPE,
    DB_LVCH_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [753]:  lbyte  :  substring(lbyte,int)
*/
{ ADFI_1220_SUBSTRING_LBYTE_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F4_WORKSPACE, ADI_SUBSTRING_OP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  2, DB_LBYTE_TYPE,
    DB_LBYTE_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [754]:  lvch  :  substring(lnvchr,int)
*/
{ ADFI_1221_SUBSTRING_LNVCHR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F4_WORKSPACE, ADI_SUBSTRING_OP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  2, DB_LNVCHR_TYPE,
    DB_LNVCHR_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [755]:  lvch  :  substring(lvch,int,int)
*/
{ ADFI_1222_SUBSTRING_LVCH_I_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F4_WORKSPACE, ADI_SUBSTRING_OP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  3, DB_LVCH_TYPE,
    DB_LVCH_TYPE, DB_INT_TYPE, DB_INT_TYPE, DB_NODT
},
/*
** [756]:  lbyte  :  substring(lbyte,int,int)
*/
{ ADFI_1223_SUBSTRING_LBYTE_I_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F4_WORKSPACE, ADI_SUBSTRING_OP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  3, DB_LBYTE_TYPE,
    DB_LBYTE_TYPE, DB_INT_TYPE, DB_INT_TYPE, DB_NODT
},
/*
** [757]:  lvch  :  substring(lnvchr,int,int)
*/
{ ADFI_1224_SUBSTRING_LNVCHR_I_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F4_WORKSPACE, ADI_SUBSTRING_OP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  3, DB_LNVCHR_TYPE,
    DB_LNVCHR_TYPE, DB_INT_TYPE, DB_INT_TYPE, DB_NODT
},
/*
** [758]:  varchar  :  unhex(char)
*/
{ ADFI_828_UNHEX_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_UNHEX_OP,
  {ADI_X2P2, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [759]:  varchar  :  unhex(text)
*/
{ ADFI_829_UNHEX_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_UNHEX_OP,
  {ADI_X2P2, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [760]:  varchar  :  unhex(varchar)
*/
{ ADFI_830_UNHEX_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_UNHEX_OP,
  {ADI_X2P2, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [761]:  varchar  :  unhex(c)
*/
{ ADFI_831_UNHEX_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_UNHEX_OP,
  {ADI_X2P2, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [762]:  i  :  iitableinfo(varchar, i,i)
*/
{ ADFI_1074_TABLEINFO, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_TABLEINFO_OP,
  {ADI_FIXED, 32+DB_CNTSIZE, 0}, 0, ADE_1CXI_SET_SKIP,
  3, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_INT_TYPE, DB_INT_TYPE, DB_NODT
},
/*
** [763]:  i  :  position(str,str)
*/
{ ADFI_878_POS_STR_STR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_COMPAT_IDT, ADI_POS_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_VCH_TYPE, DB_VCH_TYPE, DB_NODT, DB_NODT
},
/*
** [764]:  i  :  position(unistr,unistr)
*/
{ ADFI_879_POS_UNISTR_UNISTR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_COMPAT_IDT, ADI_POS_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_NVCHR_TYPE, DB_NVCHR_TYPE, DB_NODT, DB_NODT
},
/*
** [765]:  i  :  position(vbyte,vbyte)
*/
{ ADFI_880_POS_VBYTE_VBYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_COMPAT_IDT, ADI_POS_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_VBYTE_TYPE, DB_VBYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [766]:  i  :  position(str,lvch)
*/
{ ADFI_881_POS_STR_LVCH, ADI_NOFI,
  ADI_NORM_FUNC, ADI_COMPAT_IDT|ADI_F4_WORKSPACE, ADI_POS_OP,
  {ADI_FIXED, 4, 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_VCH_TYPE, DB_LVCH_TYPE, DB_NODT, DB_NODT
},
/*
** [767]:  i  :  position(unistr,lnvchr)
*/
{ ADFI_882_POS_UNISTR_LNVCHR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_COMPAT_IDT|ADI_F4_WORKSPACE, ADI_POS_OP,
  {ADI_FIXED, 4, 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_NVCHR_TYPE, DB_LNVCHR_TYPE, DB_NODT, DB_NODT
},
/*
** [768]:  i  :  position(vbyte,lbyte)
*/
{ ADFI_883_POS_VBYTE_LBYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_COMPAT_IDT|ADI_F4_WORKSPACE, ADI_POS_OP,
  {ADI_FIXED, 4, 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_VBYTE_TYPE, DB_LBYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [769]:  i  :  position(lvch,lvch)
*/
{ ADFI_1203_POS_LVCH_LVCH, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F4_WORKSPACE, ADI_POS_OP,
  {ADI_FIXED, 4, 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_LVCH_TYPE, DB_LVCH_TYPE, DB_NODT, DB_NODT
},
/*
** [770]:  i  :  position(lnvchr,lnvchr)
*/
{ ADFI_1204_POS_LNVCHR_LNVCHR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F4_WORKSPACE, ADI_POS_OP,
  {ADI_FIXED, 4, 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_LNVCHR_TYPE, DB_LNVCHR_TYPE, DB_NODT, DB_NODT
},
/*
** [771]:  i  :  position(lbyte,lbyte)
*/
{ ADFI_1205_POS_LBYTE_LBYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F4_WORKSPACE, ADI_POS_OP,
  {ADI_FIXED, 4, 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  2, DB_INT_TYPE,
    DB_LBYTE_TYPE, DB_LBYTE_TYPE, DB_NODT, DB_NODT
},
/*
** [772]:  i  :  position(str,str,i)
*/
{ ADFI_1206_POS_STR_STR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_COMPAT_IDT, ADI_POS_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  3, DB_INT_TYPE,
    DB_VCH_TYPE, DB_VCH_TYPE, DB_INT_TYPE, DB_NODT
},
/*
** [773]:  i  :  position(unistr,unistr,i)
*/
{ ADFI_1207_POS_UNISTR_UNISTR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_COMPAT_IDT, ADI_POS_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  3, DB_INT_TYPE,
    DB_NVCHR_TYPE, DB_NVCHR_TYPE, DB_INT_TYPE, DB_NODT
},
/*
** [774]:  i  :  position(vbyte,vbyte,i)
*/
{ ADFI_1208_POS_VBYTE_VBYTE_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_COMPAT_IDT, ADI_POS_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  3, DB_INT_TYPE,
    DB_VBYTE_TYPE, DB_VBYTE_TYPE, DB_INT_TYPE, DB_NODT
},
/*
** [775]:  i  :  position(str,lvch,i)
*/
{ ADFI_1209_POS_STR_LVCH_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_COMPAT_IDT|ADI_F4_WORKSPACE, ADI_POS_OP,
  {ADI_FIXED, 4, 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  3, DB_INT_TYPE,
    DB_VCH_TYPE, DB_LVCH_TYPE, DB_INT_TYPE, DB_NODT
},
/*
** [776]:  i  :  position(unistr,lnvchr,i)
*/
{ ADFI_1210_POS_UNISTR_LNVCHR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_COMPAT_IDT|ADI_F4_WORKSPACE, ADI_POS_OP,
  {ADI_FIXED, 4, 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  3, DB_INT_TYPE,
    DB_NVCHR_TYPE, DB_LNVCHR_TYPE, DB_INT_TYPE, DB_NODT
},
/*
** [777]:  i  :  position(byte,lbyte,i)
*/
{ ADFI_1211_POS_BYTE_LBYTE_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_COMPAT_IDT|ADI_F4_WORKSPACE, ADI_POS_OP,
  {ADI_FIXED, 4, 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  3, DB_INT_TYPE,
    DB_VBYTE_TYPE, DB_LBYTE_TYPE, DB_INT_TYPE, DB_NODT
},
/*
** [778]:  i  :  position(lvch,lvch,i)
*/
{ ADFI_1212_POS_LVCH_LVCH_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F4_WORKSPACE, ADI_POS_OP,
  {ADI_FIXED, 4, 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  3, DB_INT_TYPE,
    DB_LVCH_TYPE, DB_LVCH_TYPE, DB_INT_TYPE, DB_NODT
},
/*
** [779]:  i  :  position(lnvchr,lnvchr,i)
*/
{ ADFI_1213_POS_LNVCHR_LNVCHR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F4_WORKSPACE, ADI_POS_OP,
  {ADI_FIXED, 4, 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  3, DB_INT_TYPE,
    DB_LNVCHR_TYPE, DB_LNVCHR_TYPE, DB_INT_TYPE, DB_NODT
},
/*
** [780]:  i  :  position(lbyte,lbyte,i)
*/
{ ADFI_1214_POS_LBYTE_LBYTE_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F4_WORKSPACE, ADI_POS_OP,
  {ADI_FIXED, 4, 0}, (2*DB_MAXTUP), ADE_1CXI_SET_SKIP,
  3, DB_INT_TYPE,
    DB_LBYTE_TYPE, DB_LBYTE_TYPE, DB_INT_TYPE, DB_NODT
},
/*
** [781]:  text  :  ANSI trim(c)
*/
{ ADFI_884_ATRIM_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_ATRIM_OP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_TXT_TYPE,
    DB_CHR_TYPE, DB_CHA_TYPE, DB_NODT, DB_NODT
},
/*
** [782]:  text  :  ANSI trim(text)
*/
{ ADFI_885_ATRIM_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_ATRIM_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_TXT_TYPE,
    DB_TXT_TYPE, DB_CHA_TYPE, DB_NODT, DB_NODT
},
/*
** [783]:  varchar  :  ANSI trim(char)
*/
{ ADFI_886_ATRIM_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_ATRIM_OP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VCH_TYPE,
    DB_CHA_TYPE, DB_CHA_TYPE, DB_NODT, DB_NODT
},
/*
** [784]:  varchar  :  ANSI trim(varchar)
*/
{ ADFI_887_ATRIM_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_ATRIM_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_CHA_TYPE, DB_NODT, DB_NODT
},
/*
** [785]:  nvarchar  :  ANSI trim(nchar)
*/
{ ADFI_888_ATRIM_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_ATRIM_OP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_NVCHR_TYPE,
    DB_NCHR_TYPE, DB_CHA_TYPE, DB_NODT, DB_NODT
},
/*
** [786]:  nvarchar  :  ANSI trim(nvarchar)
*/
{ ADFI_889_ATRIM_NVCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_ATRIM_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_NVCHR_TYPE,
    DB_NVCHR_TYPE, DB_CHA_TYPE, DB_NODT, DB_NODT
},
/*
** [787]:  i  :  char_length(c)
*/
{ ADFI_848_CHLEN_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CHLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [788]:  i  :  char_length(char)
*/
{ ADFI_849_CHLEN_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CHLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [789]:  i  :  char_length(text)
*/
{ ADFI_850_CHLEN_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CHLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [790]:  i  :  char_length(varchar)
*/
{ ADFI_851_CHLEN_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CHLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [791]:  int  :  char_length(byte)
*/
{ ADFI_852_CHLEN_BYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CHLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_BYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [792]:  int  :  char_length(vbyte)
*/
{ ADFI_853_CHLEN_VARBYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CHLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_VBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [793]:  int  :  char_length(lvch)
*/
{ ADFI_854_CHLEN_LVCH, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CHLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_LVCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [794]:  int  :  char_length(lbyte)
*/
{ ADFI_855_CHLEN_LBYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CHLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_LBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [795]:  i  :  char_length(nchar)
*/
{ ADFI_856_CHLEN_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CHLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [796]:  i  :  char_length(nvarchar)
*/
{ ADFI_857_CHLEN_NVCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CHLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [797]:  i  :  octet_length(c)
*/
{ ADFI_858_OCLEN_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_OCLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [798]:  i  :  octet_length(char)
*/
{ ADFI_859_OCLEN_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_OCLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [799]:  i  :  octet_length(text)
*/
{ ADFI_860_OCLEN_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_OCLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [800]:  i  :  octet_length(varchar)
*/
{ ADFI_861_OCLEN_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_OCLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [801]:  int  :  octet_length(byte)
*/
{ ADFI_862_OCLEN_BYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_OCLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_BYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [802]:  int  :  octet_length(vbyte)
*/
{ ADFI_863_OCLEN_VARBYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_OCLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_VBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [803]:  int  :  octet_length(lvch)
*/
{ ADFI_864_OCLEN_LVCH, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_OCLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_LVCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [804]:  int  :  octet_length(lbyte)
*/
{ ADFI_865_OCLEN_LBYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_OCLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_LBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [805]:  i  :  octet_length(nchar)
*/
{ ADFI_866_OCLEN_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_OCLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [806]:  i  :  octet_length(nvarchar)
*/
{ ADFI_867_OCLEN_NVCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_OCLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [807]:  i  :  bit_length(c)
*/
{ ADFI_868_BLEN_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [808]:  i  :  bit_length(char)
*/
{ ADFI_869_BLEN_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [809]:  i  :  bit_length(text)
*/
{ ADFI_870_BLEN_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [810]:  i  :  bit_length(varchar)
*/
{ ADFI_871_BLEN_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [811]:  int  :  bit_length(byte)
*/
{ ADFI_872_BLEN_BYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_BYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [812]:  int  :  bit_length(vbyte)
*/
{ ADFI_873_BLEN_VARBYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_VBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [813]:  int  :  bit_length(lvch)
*/
{ ADFI_874_BLEN_LVCH, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_LVCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [814]:  int  :  bit_length(lbyte)
*/
{ ADFI_875_BLEN_LBYTE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_LBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [815]:  i  :  bit_length(nchar)
*/
{ ADFI_876_BLEN_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [816]:  i  :  bit_length(nvarchar)
*/
{ ADFI_877_BLEN_NVCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BLEN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [817]:  nchar  :  nchar(all)
*/
{ ADFI_1106_NCHAR_ALL, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_NCHAR_OP,
  {ADI_LEN_INDIRECT, 1106, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NCHR_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [818]:  nchar  :  nchar(all,i)
*/
{ ADFI_1363_NCHAR_ALL_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_NCHAR_OP,
  {ADI_LEN_INDIRECT, 1363, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_NCHR_TYPE,
    DB_ALL_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [819]:  nvarchar  :  nvarchar(all)
*/
{ ADFI_1107_NVCHAR_ALL, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_NVCHAR_OP,
  {ADI_LEN_INDIRECT, 1107, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [820]:  nvarchar  :  nvarchar(all,i)
*/
{ ADFI_1273_NVCHAR_ALL_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_NVCHAR_OP,
  {ADI_LEN_INDIRECT, 1273, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_NVCHR_TYPE,
    DB_ALL_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [821]:  varchar  :  iiutf16_to_utf8(nchar)
*/
{ ADFI_1108_UTF16TOUTF8_NCHR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_UTF16TOUTF8_OP,
  {ADI_UTOVARC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [822]:  varchar  :  iiutf16_to_utf8(nvarchar)
*/
{ ADFI_1109_UTF16TOUTF8_NVCHR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_UTF16TOUTF8_OP,
  {ADI_UTOC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [823]:  nchar  :  iiutf8_to_utf16(varchar)
*/
{ ADFI_1110_UTF8TOUTF16_NCHR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_UTF8TOUTF16_OP,
  {ADI_VARCTOU, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NCHR_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [824]:  nvarchar  :  iiutf8_to_utf16(varchar)
*/
{ ADFI_1111_UTF8TOUTF16_NVCHR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_UTF8TOUTF16_OP,
  {ADI_CTOU, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [825]:  i  :  int8(c)
*/
{ ADFI_1116_INT8_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I8_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [826]:  i  :  int8(char)
*/
{ ADFI_1122_INT8_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I8_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [827]:  i  :  int8(f)
*/
{ ADFI_1117_INT8_F, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I8_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [828]:  i  :  int8(dec)
*/
{ ADFI_1121_INT8_DEC, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I8_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [829]:  i  :  int8(i)
*/
{ ADFI_1118_INT8_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I8_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [830]:  i  :  int8(money)
*/
{ ADFI_1120_INT8_MONEY, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I8_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [831]:  i  :  int8(text)
*/
{ ADFI_1119_INT8_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I8_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [832]:  i  :  int8(varchar)
*/
{ ADFI_1123_INT8_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I8_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [833]:  i  :  int8(nchar)
*/
{ ADFI_1162_INT8_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I8_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [834]:  i  :  int8(nvarchar)
*/
{ ADFI_1163_INT8_NVCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_I8_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [835]:  varbyte : collation_weight(c)
*/
{ ADFI_1190_COWGT_C, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_COWGT_OP,
  {ADI_CWTOVB, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBYTE_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [836]:  varbyte : collation_weight(char)
*/
{ ADFI_1191_COWGT_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_COWGT_OP,
  {ADI_CWTOVB, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBYTE_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [837]:  varbyte : collation_weight(varchar)
*/
{ ADFI_1192_COWGT_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_COWGT_OP,
  {ADI_CWTOVB, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBYTE_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [838]:  varbyte : collation_weight(text)
*/
{ ADFI_1193_COWGT_TEXT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_COWGT_OP,
  {ADI_CWTOVB, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBYTE_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [839]:  varbyte : collation_weight(nchr)
*/
{ ADFI_1194_COWGT_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_COWGT_OP,
  {ADI_CWTOVB, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBYTE_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [840]:  varbyte : collation_weight(nchr, int)
*/
{ ADFI_1195_COWGT_NCHAR_INT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_COWGT_OP,
  {ADI_CWTOVB, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VBYTE_TYPE,
    DB_NCHR_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [841]:  varbyte : collation_weight(nvchr)
*/
{ ADFI_1196_COWGT_NVCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_COWGT_OP,
  {ADI_CWTOVB, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBYTE_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [842]:  varbyte : collation_weight(nvchr, int)
*/
{ ADFI_1197_COWGT_NVCHAR_INT, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_COWGT_OP,
  {ADI_CWTOVB, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VBYTE_TYPE,
    DB_NVCHR_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [843]:  ansidate  :  ansidate(all)
*/
{ ADFI_1238_ANSI_DATE, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_ADTE_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_ADTE_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [844]:  timewotz  :  time_wo_tz(all)
*/
{ ADFI_1239_ANSI_TMWO, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_TMWO_OP,
  {ADI_FIXED, 10, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TMWO_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [845]:  timewtz  :  time_with_tz(all)
*/
{ ADFI_1240_ANSI_TMW, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_TMW_OP,
  {ADI_FIXED, 10, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TMW_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [846]:  time  :  time_local(all)
*/
{ ADFI_1241_ANSI_LCTME, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_TME_OP,
  {ADI_FIXED, 10, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TME_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [847]:  tmstmpwotz  :  timestamp_wo_tz(all)
*/
{ ADFI_1242_ANSI_TSWO, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_TSWO_OP,
  {ADI_FIXED, 14, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TSWO_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [848]:  tmstmpwtz  :  timestamp_with_tz(all)
*/
{ ADFI_1243_ANSI_TSW, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_TSW_OP,
  {ADI_FIXED, 14, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TSW_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [849]:  tmstmp  :  timestamp_local(all)
*/
{ ADFI_1244_ANSI_TSTMP, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_TSTMP_OP,
  {ADI_FIXED, 14, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TSTMP_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [850]:  tmstmp  :  interval_dtos(all)
*/
{ ADFI_1245_ANSI_INDS, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_INDS_OP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INDS_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [851]:  tmstmp  :  interval_ytom(all)
*/
{ ADFI_1246_ANSI_INYM, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_INYM_OP,
  {ADI_FIXED, 3, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INYM_TYPE,
    DB_ALL_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [852]:  i  :  year(date/time)
*/
{ ADFI_1247_YEARPART, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_YPART_OP,
  {ADI_FIXED, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [853]:  i  :  quarter(date/time)
*/
{ ADFI_1248_QUARTERPART, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_QPART_OP,
  {ADI_FIXED, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [854]:  i  :  month(date/time)
*/
{ ADFI_1249_MONTHPART, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_MPART_OP,
  {ADI_FIXED, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [855]:  i  :  week(date/time)
*/
{ ADFI_1250_WEEKPART, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_WPART_OP,
  {ADI_FIXED, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [856]:  i  :  week_iso(date/time)
*/
{ ADFI_1251_WEEKISOPART, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_WIPART_OP,
  {ADI_FIXED, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [857]:  i  :  day(date/time)
*/
{ ADFI_1252_DAYPART, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_DYPART_OP,
  {ADI_FIXED, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [858]:  i  :  hour(date/time)
*/
{ ADFI_1253_HOURPART, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_HPART_OP,
  {ADI_FIXED, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [859]:  i  :  minute(date/time)
*/
{ ADFI_1254_MINUTEPART, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_MIPART_OP,
  {ADI_FIXED, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [860]:  i  :  second(date/time)
*/
{ ADFI_1255_SECONDPART, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_SPART_OP,
  {ADI_FIXED, 2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [861]:  i  :  microsecond(date/time)
*/
{ ADFI_1256_MICROSECONDPART, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_MSPART_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [862]:  i  :  nanosecond(date/time)
*/
{ ADFI_1257_NANOSECONDPART, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F1_VARFI, ADI_NPART_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [863]:  dec  :  round(dec, i)
*/
{ ADFI_1283_DECROUND, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_ROUND_OP,
  {ADI_DECROUND, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DEC_TYPE,
    DB_DEC_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [864]:  dec  :  trunc[ate](dec, i)
*/
{ ADFI_1284_DECTRUNC, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_TRUNC_OP,
  {ADI_DECROUND, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_DEC_TYPE,
    DB_DEC_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [865]:  dec  :  ceil[ing](dec)
*/
{ ADFI_1285_DECCEIL, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CEIL_OP,
  {ADI_DECCEILFL, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DEC_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [866]:  dec  :  floor(dec)
*/
{ ADFI_1286_DECFLOOR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_FLOOR_OP,
  {ADI_DECCEILFL, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DEC_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [867]:  dec  :  chr(int)
*/
{ ADFI_1287_CHR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_CHR_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [868]:  char  :  ltrim(char)
*/
{ ADFI_1288_LTRIM_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LTRIM_OP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [869]:  varchar  :  ltrim(varchar)
*/
{ ADFI_1289_LTRIM_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LTRIM_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [870]:  char  :  ltrim(nchar)
*/
{ ADFI_1290_LTRIM_NCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LTRIM_OP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [871]:  char  :  ltrim(nvarchar)
*/
{ ADFI_1291_LTRIM_NVCHR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LTRIM_OP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [872]:  varchar  :  lpad(varchar,n)
*/
{ ADFI_1298_LPAD_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LPAD_OP,
  {ADI_FIXED, 4000, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [873]:  varchar  :  lpad(char,n)
*/
{ ADFI_1299_LPAD_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LPAD_OP,
  {ADI_FIXED, 4000, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VCH_TYPE,
    DB_CHA_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [874]:  varchar  :  lpad(varchar,n,varchar)
*/
{ ADFI_1300_LPAD_VARCHAR_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LPAD_OP,
  {ADI_FIXED, 4000, 0}, 0, ADE_1CXI_SET_SKIP,
  3, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_INT_TYPE, DB_VCH_TYPE, DB_NODT
},
/*
** [875]:  varchar  :  lpad(char,n,varchar)
*/
{ ADFI_1301_LPAD_CHAR_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_LPAD_OP,
  {ADI_FIXED, 4000, 0}, 0, ADE_1CXI_SET_SKIP,
  3, DB_VCH_TYPE,
    DB_CHA_TYPE, DB_INT_TYPE, DB_VCH_TYPE, DB_NODT
},
/*
** [876]:  varchar  :  rpad(varchar,n)
*/
{ ADFI_1302_RPAD_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_RPAD_OP,
  {ADI_FIXED, 4000, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [877]:  varchar  :  rpad(char,n)
*/
{ ADFI_1303_RPAD_CHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_RPAD_OP,
  {ADI_FIXED, 4000, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_VCH_TYPE,
    DB_CHA_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [878]:  varchar  :  rpad(varchar,n,varchar)
*/
{ ADFI_1304_RPAD_VARCHAR_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_RPAD_OP,
  {ADI_FIXED, 4000, 0}, 0, ADE_1CXI_SET_SKIP,
  3, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_INT_TYPE, DB_VCH_TYPE, DB_NODT
},
/*
** [879]:  varchar  :  rpad(char,n,varchar)
*/
{ ADFI_1305_RPAD_CHAR_VARCHAR, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_RPAD_OP,
  {ADI_FIXED, 4000, 0}, 0, ADE_1CXI_SET_SKIP,
  3, DB_VCH_TYPE,
    DB_CHA_TYPE, DB_INT_TYPE, DB_VCH_TYPE, DB_NODT
},
/*
** [880]:  varchar  :  replace(varchar,varchar)
*/
{ ADFI_1306_REPL_VARCH_VARCH_VARCH, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_REPL_OP,
  {ADI_FIXED, 4000, 0}, 0, ADE_1CXI_SET_SKIP,
  3, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_VCH_TYPE, DB_VCH_TYPE, DB_NODT
},
/*
** [881]:  varchar  :  replace(char,varchar)
*/
{ ADFI_1307_REPL_CHAR_VARCH_VARCH, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_REPL_OP,
  {ADI_FIXED, 4000, 0}, 0, ADE_1CXI_SET_SKIP,
  3, DB_VCH_TYPE,
    DB_CHA_TYPE, DB_VCH_TYPE, DB_VCH_TYPE, DB_NODT
},
/*
** [882]:  f  :  acos(f)
*/
{ ADFI_1308_ACOS_F, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_ACOS_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [883]:  f  :  asin(f)
*/
{ ADFI_1309_ASIN_F, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_ASIN_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [884]:  f  :  tan(f)
*/
{ ADFI_1310_TAN_F, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_TAN_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [885]:  f  :  pi()
*/
{ ADFI_1311_PI, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_PI_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  0, DB_FLT_TYPE,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
},
/*
** [886]:  i  :  sign(i)
*/
{ ADFI_1312_SIGN_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SIGN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [887]:  i  :  sign(f)
*/
{ ADFI_1313_SIGN_F, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SIGN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [888]:  i  :  sign(dec)
*/
{ ADFI_1314_SIGN_DEC, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_SIGN_OP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [889]:  f  :  atan2(f, f)
*/
{ ADFI_1315_ATAN2_F_F, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_ATAN2_OP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_FLT_TYPE, DB_NODT, DB_NODT
},
/*
** [890]:  char  :  byteextract(c,i)
*/
{ ADFI_406_BYTEEXTRACT_C_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BYTEXT_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHA_TYPE,
    DB_CHR_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [891]:  char  :  byteextract(text,i)
*/
{ ADFI_407_BYTEEXTRACT_TEXT_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BYTEXT_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHA_TYPE,
    DB_TXT_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [892]:  char  :  byteextract(char,i)
*/
{ ADFI_408_BYTEEXTRACT_CHAR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BYTEXT_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHA_TYPE,
    DB_CHA_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [893]:  char  :  byteextract(varchar,i)
*/
{ ADFI_409_BYTEEXTRACT_VARCHAR_I, ADI_NOFI,
  ADI_NORM_FUNC, ADI_F0_NOFIFLAGS, ADI_BYTEXT_OP,
  {ADI_FIXED, 1, 0}, 0, ADE_1CXI_SET_SKIP,
  2, DB_CHA_TYPE,
    DB_VCH_TYPE, DB_INT_TYPE, DB_NODT, DB_NODT
},
/*
** [894]:  char  :  i -copy-> char
*/
{ ADFI_416_I_COPYTO_CHAR, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_PRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [895]:  char  :  dec -copy-> char
*/
{ ADFI_561_DEC_COPYTO_CHAR, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_PRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [896]:  char  :  f -copy-> char
*/
{ ADFI_417_F_COPYTO_CHAR, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_PRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [897]:  varchar  :  i -copy-> varchar
*/
{ ADFI_418_I_COPYTO_VARCHAR, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_TPRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [898]:  varchar  :  dec -copy-> varchar
*/
{ ADFI_562_DEC_COPYTO_VARCHAR, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_TPRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [899]:  varchar  :  f -copy-> varchar
*/
{ ADFI_419_F_COPYTO_VARCHAR, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_TPRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [900]:  c  :  i -copy-> c
*/
{ ADFI_420_I_COPYTO_C, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_PRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHR_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [901]:  c  :  dec -copy-> c
*/
{ ADFI_559_DEC_COPYTO_C, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_PRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHR_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [902]:  c  :  f -copy-> c
*/
{ ADFI_421_F_COPYTO_C, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_PRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHR_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [903]:  text  :  i -copy-> text
*/
{ ADFI_422_I_COPYTO_TEXT, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_TPRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [904]:  text  :  dec -copy-> text
*/
{ ADFI_560_DEC_COPYTO_TEXT, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_TPRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [905]:  text  :  f -copy-> text
*/
{ ADFI_423_F_COPYTO_TEXT, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_TPRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [906]:  i  :  money -copy-> i
*/
{ ADFI_424_MONEY_COPYTO_I, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [907]:  i  :  char -copy-> i
*/
{ ADFI_425_CHAR_COPYTO_I, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [908]:  i  :  varchar -copy-> i
*/
{ ADFI_426_VARCHAR_COPYTO_I, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [909]:  i  :  c -copy-> i
*/
{ ADFI_427_C_COPYTO_I, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [910]:  i  :  text -copy-> i
*/
{ ADFI_428_TEXT_COPYTO_I, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [911]:  dec  :  char -copy-> dec
*/
{ ADFI_565_CHAR_COPYTO_DEC, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, 16, 0x1f0f}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DEC_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [912]:  dec  :  varchar -copy-> dec
*/
{ ADFI_566_VARCHAR_COPYTO_DEC, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, 16, 0x1f0f}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DEC_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [913]:  dec  :  text -copy-> dec
*/
{ ADFI_564_TEXT_COPYTO_DEC, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, 16, 0x1f0f}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DEC_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [914]:  dec  :  c -copy-> dec
*/
{ ADFI_563_C_COPYTO_DEC, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, 16, 0x1f0f}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DEC_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [915]:  f  :  char -copy-> f
*/
{ ADFI_429_CHAR_COPYTO_F, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [916]:  f  :  varchar -copy-> f
*/
{ ADFI_430_VARCHAR_COPYTO_F, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [917]:  f  :  c -copy-> f
*/
{ ADFI_431_C_COPYTO_F, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [918]:  f  :  text -copy-> f
*/
{ ADFI_432_TEXT_COPYTO_F, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [919]:  c  :  money -copy-> c
*/
{ ADFI_434_MONEY_COPYTO_C, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 20, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHR_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [920]:  char  :  money -copy-> char
*/
{ ADFI_435_MONEY_COPYTO_CHAR, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 20, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [921]:  text  :  money -copy-> text
*/
{ ADFI_436_MONEY_COPYTO_TEXT, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 20+DB_CNTSIZE, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [922]:  varchar  :  money -copy-> varchar
*/
{ ADFI_437_MONEY_COPYTO_VARCHAR, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 20+DB_CNTSIZE, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [923]:  utf8  :  nchar -copy-> utf8
*/
{ ADFI_1112_NCHAR_COPYTO_UTF8, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_UTOC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_UTF8_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [924]:  utf8  :  nvarchar -copy-> utf8
*/
{ ADFI_1113_NVCHAR_COPYTO_UTF8, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_VARUTOC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_UTF8_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [925]:  nchar  :  utf8 -copy-> nchar
*/
{ ADFI_1114_UTF8_COPYTO_NCHAR, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_CTOU, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NCHR_TYPE,
    DB_UTF8_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [926]:  nvarchar  :  utf8 -copy-> nvarchar
*/
{ ADFI_1115_UTF8_COPYTO_NVCHAR, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_CTOVARU, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_UTF8_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [927]:  utf8  :  char -copy-> utf8
*/
{ ADFI_1182_CHAR_COPYTO_UTF8, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_CTOU, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_UTF8_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [928]:  utf8  :  varchar -copy-> utf8
*/
{ ADFI_1183_VARCHAR_COPYTO_UTF8, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_VARCTOU, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_UTF8_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [929]:  char  :  utf8 -copy-> char
*/
{ ADFI_1184_UTF8_COPYTO_CHAR, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_UTOC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHR_TYPE,
    DB_UTF8_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [930]:  varchar  :  utf8 -copy-> varchar
*/
{ ADFI_1185_UTF8_COPYTO_VARCHAR, ADI_NOFI,
  ADI_COPY_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_UTOVARC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_UTF8_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [931]:  f  :  i -> f
*/
{ ADFI_213_I_TO_F, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [932]:  f  :  dec -> f
*/
{ ADFI_568_DEC_TO_F, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [933]:  f  :  c -> f
*/
{ ADFI_1327_C_TO_F, ADI_NOFI,
  ADI_COERCION, ADI_F8192_QUEL_CLOAKED, ADI_NOOP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [934]:  f  :  text -> f
*/
{ ADFI_1328_TXT_TO_F, ADI_NOFI,
  ADI_COERCION, ADI_F8192_QUEL_CLOAKED, ADI_NOOP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [935]:  f  :  char -> f
*/
{ ADFI_1329_CHAR_TO_F, ADI_NOFI,
  ADI_COERCION, ADI_F8192_QUEL_CLOAKED, ADI_NOOP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [936]:  f  :  varchar -> f
*/
{ ADFI_1330_VARCHAR_TO_F, ADI_NOFI,
  ADI_COERCION, ADI_F8192_QUEL_CLOAKED, ADI_NOOP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [937]:  f  :  nchar -> f
*/
{ ADFI_1331_NCHAR_TO_F, ADI_NOFI,
  ADI_COERCION, ADI_F8192_QUEL_CLOAKED, ADI_NOOP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [938]:  f  :  nvarchar -> f
*/
{ ADFI_1332_NVARCHAR_TO_F, ADI_NOFI,
  ADI_COERCION, ADI_F8192_QUEL_CLOAKED, ADI_NOOP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [939]:  dec  :  f -> dec
*/
{ ADFI_574_F_TO_DEC, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, 8, 0x0f00}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DEC_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [940]:  dec  :  i -> dec
*/
{ ADFI_575_I_TO_DEC, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_DECINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DEC_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [941]:  i  :  f -> i
*/
{ ADFI_214_F_TO_I, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [942]:  i  :  dec -> i
*/
{ ADFI_569_DEC_TO_I, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [943]:  i  :  c -> i
*/
{ ADFI_1321_C_TO_I, ADI_NOFI,
  ADI_COERCION, ADI_F8192_QUEL_CLOAKED, ADI_NOOP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [944]:  i  :  txt -> i
*/
{ ADFI_1322_TXT_TO_I, ADI_NOFI,
  ADI_COERCION, ADI_F8192_QUEL_CLOAKED, ADI_NOOP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [945]:  i  :  char -> i
*/
{ ADFI_1323_CHAR_TO_I, ADI_NOFI,
  ADI_COERCION, ADI_F8192_QUEL_CLOAKED, ADI_NOOP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [946]:  i  :  varchar -> i
*/
{ ADFI_1324_VARCHAR_TO_I, ADI_NOFI,
  ADI_COERCION, ADI_F8192_QUEL_CLOAKED, ADI_NOOP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [947]:  i  :  nchar -> i
*/
{ ADFI_1325_NCHAR_TO_I, ADI_NOFI,
  ADI_COERCION, ADI_F8192_QUEL_CLOAKED, ADI_NOOP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [948]:  i  :  nvarchar -> i
*/
{ ADFI_1326_NVARCHAR_TO_I, ADI_NOFI,
  ADI_COERCION, ADI_F8192_QUEL_CLOAKED, ADI_NOOP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [949]:  date  :  c -> date
*/
{ ADFI_215_C_TO_DATE, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DTE_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [950]:  date  :  text -> date
*/
{ ADFI_216_TEXT_TO_DATE, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DTE_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [951]:  c  :  date -> c
*/
{ ADFI_217_DATE_TO_C, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 25, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHR_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [952]:  text  :  date -> text
*/
{ ADFI_218_DATE_TO_TEXT, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 25+DB_CNTSIZE, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [953]:  c  :  money -> c
*/
{ ADFI_219_MONEY_TO_C, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 20, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHR_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [954]:  text  :  money -> text
*/
{ ADFI_220_MONEY_TO_TEXT, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 20+DB_CNTSIZE, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [955]:  money  :  c -> money
*/
{ ADFI_221_C_TO_MONEY, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_MNY_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [956]:  money  :  text -> money
*/
{ ADFI_222_TEXT_TO_MONEY, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_MNY_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [957]:  money  :  f -> money
*/
{ ADFI_223_F_TO_MONEY, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_MNY_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [958]:  money  :  dec -> money
*/
{ ADFI_567_DEC_TO_MONEY, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_MNY_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [959]:  money  :  i -> money
*/
{ ADFI_224_I_TO_MONEY, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_MNY_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [960]:  f  :  money -> f
*/
{ ADFI_225_MONEY_TO_F, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [961]:  dec  :  money -> dec
*/
{ ADFI_573_MONEY_TO_DEC, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, 9, 0x0f02}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DEC_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [962]:  text  :  c -> text
*/
{ ADFI_226_C_TO_TEXT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [963]:  c  :  text -> c
*/
{ ADFI_227_TEXT_TO_C, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1TC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHR_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [964]:  char  :  i -> c
*/
{ ADFI_1333_I_TO_C, ADI_NOFI,
  ADI_COERCION, ADI_F8192_QUEL_CLOAKED, ADI_NOOP,
  {ADI_PRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHR_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [965]:  c  :  f -> c
*/
{ ADFI_1334_F_TO_C, ADI_NOFI,
  ADI_COERCION, ADI_F8192_QUEL_CLOAKED, ADI_NOOP,
  {ADI_PRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHR_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [966]:  char  :  c -> char
*/
{ ADFI_331_C_TO_CHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [967]:  longtext  :  c -> longtext
*/
{ ADFI_332_C_TO_LONGTEXT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LTXT_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [968]:  varchar  :  c -> varchar
*/
{ ADFI_333_C_TO_VARCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [969]:  c  :  char -> c
*/
{ ADFI_334_CHAR_TO_C, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHR_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [970]:  char  :  char -> char
*/
{ ADFI_335_CHAR_TO_CHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_RESLEN, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [971]:  date  :  char -> date
*/
{ ADFI_336_CHAR_TO_DATE, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DTE_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [972]:  longtext  :  char -> longtext
*/
{ ADFI_337_CHAR_TO_LONGTEXT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LTXT_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [973]:  money  :  char -> money
*/
{ ADFI_338_CHAR_TO_MONEY, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_MNY_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [974]:  text  :  char -> text
*/
{ ADFI_339_CHAR_TO_TEXT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [975]:  varchar  :  char -> varchar
*/
{ ADFI_340_CHAR_TO_VARCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [976]:  logical_key  :  char -> logical_key
*/
{ ADFI_476_CHAR_TO_LOGKEY, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, DB_LEN_OBJ_LOGKEY, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LOGKEY_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [977]:  table_key  :  char -> table_key
*/
{ ADFI_495_CHAR_TO_TABKEY, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, DB_LEN_TAB_LOGKEY, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TABKEY_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [978]:  char  :  date -> char
*/
{ ADFI_341_DATE_TO_CHAR, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 25, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [979]:  longtext  :  date -> longtext
*/
{ ADFI_342_DATE_TO_LONGTEXT, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 25+DB_CNTSIZE, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LTXT_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [980]:  varchar  :  date -> varchar
*/
{ ADFI_343_DATE_TO_VARCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 25+DB_CNTSIZE, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [981]:  longtext  :  f -> longtext
*/
{ ADFI_344_F_TO_LONGTEXT, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_TPRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LTXT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [982]:  longtext  :  dec -> longtext
*/
{ ADFI_571_DEC_TO_LONGTEXT, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_TPRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LTXT_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [983]:  longtext  :  i -> longtext
*/
{ ADFI_345_I_TO_LONGTEXT, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_TPRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LTXT_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [984]:  c  :  longtext -> c
*/
{ ADFI_346_LONGTEXT_TO_C, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1TC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHR_TYPE,
    DB_LTXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [985]:  char  :  longtext -> char
*/
{ ADFI_347_LONGTEXT_TO_CHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1TC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_LTXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [986]:  date  :  longtext -> date
*/
{ ADFI_348_LONGTEXT_TO_DATE, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DTE_TYPE,
    DB_LTXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [987]:  f  :  longtext -> f
*/
{ ADFI_349_LONGTEXT_TO_F, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_LTXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [988]:  dec  :  longtext -> dec
*/
{ ADFI_572_LONGTEXT_TO_DEC, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, 16, 0x1f0f}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DEC_TYPE,
    DB_LTXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [989]:  i  :  longtext -> i
*/
{ ADFI_350_LONGTEXT_TO_I, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, 4, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_LTXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [990]:  logical_key  :  varchar -> logical_key
*/
{ ADFI_608_VARCHAR_TO_LOGKEY, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, DB_LEN_OBJ_LOGKEY, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LOGKEY_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [991]:  logical_key  :  text -> logical_key
*/
{ ADFI_612_TEXT_TO_LOGKEY, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, DB_LEN_OBJ_LOGKEY, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LOGKEY_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [992]:  logical_key  :  longtext -> logical_key
*/
{ ADFI_507_LONGTEXT_TO_LOGKEY, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, DB_LEN_OBJ_LOGKEY, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LOGKEY_TYPE,
    DB_LTXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [993]:  longtext  :  longtext -> longtext
*/
{ ADFI_351_LONGTEXT_TO_LONGTEXT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_RESLEN, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LTXT_TYPE,
    DB_LTXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [994]:  money  :  longtext -> money
*/
{ ADFI_352_LONGTEXT_TO_MONEY, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_MNY_TYPE,
    DB_LTXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [995]:  table_key  :  varchar -> table_key
*/
{ ADFI_610_VARCHAR_TO_TABKEY, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, DB_LEN_TAB_LOGKEY, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TABKEY_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [996]:  table_key  :  text -> table_key
*/
{ ADFI_614_TEXT_TO_TABKEY, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, DB_LEN_TAB_LOGKEY, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TABKEY_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [997]:  table_key  :  longtext -> table_key
*/
{ ADFI_509_LONGTEXT_TO_TABKEY, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, DB_LEN_TAB_LOGKEY, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TABKEY_TYPE,
    DB_LTXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [998]:  text  :  longtext -> text
*/
{ ADFI_353_LONGTEXT_TO_TEXT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_LTXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [999]:  varchar  :  longtext -> varchar
*/
{ ADFI_354_LONGTEXT_TO_VARCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_LTXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1000]:  char  :  money -> char
*/
{ ADFI_355_MONEY_TO_CHAR, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 20, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1001]:  longtext  :  money -> longtext
*/
{ ADFI_356_MONEY_TO_LONGTEXT, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 20+DB_CNTSIZE, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LTXT_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1002]:  varchar  :  money -> varchar
*/
{ ADFI_357_MONEY_TO_VARCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 20+DB_CNTSIZE, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1003]:  char  :  text -> char
*/
{ ADFI_358_TEXT_TO_CHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1TC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1004]:  longtext  :  text -> longtext
*/
{ ADFI_359_TEXT_TO_LONGTEXT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LTXT_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1005]:  varchar  :  text -> varchar
*/
{ ADFI_360_TEXT_TO_VARCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1006]:  c  :  varchar -> c
*/
{ ADFI_361_VARCHAR_TO_C, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1TC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHR_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1007]:  char  :  varchar -> char
*/
{ ADFI_362_VARCHAR_TO_CHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1TC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1008]:  date  :  varchar -> date
*/
{ ADFI_363_VARCHAR_TO_DATE, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DTE_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1009]:  longtext  :  varchar -> longtext
*/
{ ADFI_364_VARCHAR_TO_LONGTEXT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LTXT_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1010]:  money  :  varchar -> money
*/
{ ADFI_365_VARCHAR_TO_MONEY, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_MNY_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1011]:  text  :  varchar -> text
*/
{ ADFI_366_VARCHAR_TO_TEXT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1012]:  varchar  :  varchar -> varchar
*/
{ ADFI_367_VARCHAR_TO_VARCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_RESLEN, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1013]:  c  :  c -> c
*/
{ ADFI_228_C_TO_C, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_RESLEN, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHR_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1014]:  text  :  text -> text
*/
{ ADFI_229_TEXT_TO_TEXT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_RESLEN, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1015]:  f  :  f -> f
*/
{ ADFI_230_F_TO_F, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_RESLEN, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_FLT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1016]:  dec  :  dec -> dec
*/
{ ADFI_570_DEC_TO_DEC, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_RESLEN, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DEC_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1017]:  i  :  i -> i
*/
{ ADFI_231_I_TO_I, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_RESLEN, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_INT_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1018]:  money  :  money -> money
*/
{ ADFI_232_MONEY_TO_MONEY, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_RESLEN, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_MNY_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1019]:  date  :  date -> date
*/
{ ADFI_233_DATE_TO_DATE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_DATE2DATE, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DTE_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1020]:  char  :  logical_key -> char
*/
{ ADFI_477_LOGKEY_TO_CHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, DB_LEN_OBJ_LOGKEY, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_LOGKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1021]:  varchar  :  logical_key -> varchar
*/
{ ADFI_607_LOGKEY_TO_VARCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, DB_LEN_OBJ_LOGKEY+2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_LOGKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1022]:  text     :  logical_key -> text
*/
{ ADFI_611_LOGKEY_TO_TEXT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, DB_LEN_OBJ_LOGKEY+2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_LOGKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1023]:  longtext  :  logical_key -> longtext
*/
{ ADFI_506_LOGKEY_TO_LONGTEXT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, DB_LEN_OBJ_LOGKEY+2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LTXT_TYPE,
    DB_LOGKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1024]:  logical_key  :  logical_key -> logical_key
*/
{ ADFI_478_LOGKEY_TO_LOGKEY, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, DB_LEN_OBJ_LOGKEY, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LOGKEY_TYPE,
    DB_LOGKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1025]:  char  :  table_key -> char
*/
{ ADFI_496_TABKEY_TO_CHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, DB_LEN_TAB_LOGKEY, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_TABKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1026]:  varchar  :  table_key -> varchar
*/
{ ADFI_609_TABKEY_TO_VARCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, DB_LEN_TAB_LOGKEY+2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_TABKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1027]:  text     :  table_key -> text
*/
{ ADFI_613_TABKEY_TO_TEXT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, DB_LEN_TAB_LOGKEY+2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_TABKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1028]:  longtext  :  table_key -> longtext
*/
{ ADFI_508_TABKEY_TO_LONGTEXT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, DB_LEN_TAB_LOGKEY+2, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LTXT_TYPE,
    DB_TABKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1029]:  table_key  :  table_key -> table_key
*/
{ ADFI_497_TABKEY_TO_TABKEY, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, DB_LEN_TAB_LOGKEY, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TABKEY_TYPE,
    DB_TABKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1030]:  long_varchar  :  char -> long_varchar
*/
{ ADFI_620_CHAR_TO_LVCH, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LVCH_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1031]:  long_varchar  :  c -> long_varchar
*/
{ ADFI_621_C_TO_LVCH, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LVCH_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1032]:  long_varchar  :  text -> long_varchar
*/
{ ADFI_622_TEXT_TO_LVCH, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LVCH_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1033]:  long_varchar  :  varchar -> long_varchar
*/
{ ADFI_623_VARCHAR_TO_LVCH, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LVCH_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1034]:  long_varchar  :  long_varchar -> long_varchar
*/
{ ADFI_624_LVCH_TO_LVCH, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LVCH_TYPE,
    DB_LVCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1035]:  long_varchar  :  longtext -> long_varchar
*/
{ ADFI_625_LTXT_TO_LVCH, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LVCH_TYPE,
    DB_LTXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1036]:  long_varchar  :  long_varchar -> longtext
*/
{ ADFI_626_LVCH_TO_LTXT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LTXT_TYPE,
    DB_LVCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1037]:  bit  :  char -> bit
*/
{ ADFI_691_CHAR_TO_BIT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_CHAR2BIT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BIT_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1038]:  bit  :  varchar -> bit
*/
{ ADFI_692_VARCHAR_TO_BIT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_CHAR2BIT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BIT_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1039]:  bit  :  bit -> bit
*/
{ ADFI_693_BIT_TO_BIT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_RESLEN, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BIT_TYPE,
    DB_BIT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1040]:  vbit  :  char -> vbit
*/
{ ADFI_695_CHAR_TO_VBIT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_CHAR2BIT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBIT_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1041]:  vbit  :  varchar -> vbit
*/
{ ADFI_696_VARCHAR_TO_VBIT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_CHAR2BIT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBIT_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1042]:  vbit  :  bit -> vbit
*/
{ ADFI_697_BIT_TO_VBIT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_RESLEN, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBIT_TYPE,
    DB_BIT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1043]:  vbit  :  vbit -> vbit
*/
{ ADFI_698_VBIT_TO_VBIT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_RESLEN, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBIT_TYPE,
    DB_VBIT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1044]:  vbit  :  longtext -> vbit
*/
{ ADFI_699_LONGTEXT_TO_VBIT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_CHAR2BIT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBIT_TYPE,
    DB_LTXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1045]:  bit  :  longtext -> bit
*/
{ ADFI_700_LONGTEXT_TO_BIT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_CHAR2BIT, 100, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BIT_TYPE,
    DB_LTXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1046]:  varchar  :  bit -> varchar
*/
{ ADFI_701_BIT_TO_VARCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_BIT2CHAR, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_BIT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1047]:  char  :  bit -> char
*/
{ ADFI_702_BIT_TO_CHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_BIT2CHAR, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_BIT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1048]:  longtext  :  bit -> longtext
*/
{ ADFI_703_BIT_TO_LONGTEXT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_BIT2CHAR, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LTXT_TYPE,
    DB_BIT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1049]:  varchar  :  vbit -> varchar
*/
{ ADFI_704_VBIT_TO_VARCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_BIT2CHAR, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_VBIT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1050]:  char  :  vbit -> char
*/
{ ADFI_705_VBIT_TO_CHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_BIT2CHAR, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_VBIT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1051]:  longtext  :  vbit -> longtext
*/
{ ADFI_706_VBIT_TO_LONGTEXT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_BIT2CHAR, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LTXT_TYPE,
    DB_VBIT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1052]:  vbit  :  vbit -> bit
*/
{ ADFI_694_VBIT_TO_BIT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_RESLEN, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBIT_TYPE,
    DB_VBIT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1053]:  byte  :  char -> byte
*/
{ ADFI_752_CHAR_TO_BYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BYTE_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1054]:  byte  :  varchar -> byte
*/
{ ADFI_753_VARCHAR_TO_BYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1TC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BYTE_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1055]:  byte  :  c -> byte
*/
{ ADFI_754_C_TO_BYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BYTE_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1056]:  byte  :  text -> byte
*/
{ ADFI_755_TEXT_TO_BYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1TC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BYTE_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1057]:  byte  :  longtext -> byte
*/
{ ADFI_756_LONGTEXT_TO_BYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1TC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BYTE_TYPE,
    DB_LTXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1058]:  byte  :  byte -> byte
*/
{ ADFI_757_BYTE_TO_BYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_RESLEN, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BYTE_TYPE,
    DB_BYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1059]:  byte  :  varbyte -> byte
*/
{ ADFI_758_VARBYTE_TO_BYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1TC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BYTE_TYPE,
    DB_VBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1060]:  varbyte  :  char -> varbyte
*/
{ ADFI_759_CHAR_TO_VARBYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBYTE_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1061]:  varbyte  :  varchar -> varbyte
*/
{ ADFI_760_VARCHAR_TO_VARBYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBYTE_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1062]:  varbyte  :  c -> varbyte
*/
{ ADFI_761_C_TO_VARBYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBYTE_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1063]:  varbyte  :  text -> varbyte
*/
{ ADFI_762_TEXT_TO_VARBYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBYTE_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1064]:  varbyte  :  longtext -> varbyte
*/
{ ADFI_763_LONGTEXT_TO_VARBYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBYTE_TYPE,
    DB_LTXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1065]:  varbyte  :  byte -> varbyte
*/
{ ADFI_682_BYTE_TO_VARBYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBYTE_TYPE,
    DB_BYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1066]:  varbyte  :  varbyte -> varbyte
*/
{ ADFI_681_VARBYTE_TO_VARBYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_RESLEN, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBYTE_TYPE,
    DB_VBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1067]:  lbyte  :  char -> lbyte
*/
{ ADFI_672_CHAR_TO_LBYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LBYTE_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1068]:  lbyte  :  varchar -> lbyte
*/
{ ADFI_671_VARCHAR_TO_LBYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LBYTE_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1069]:  lbyte  :  c -> lbyte
*/
{ ADFI_670_C_TO_LBYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LBYTE_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1070]:  lbyte  :  text -> lbyte
*/
{ ADFI_669_TEXT_TO_LBYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LBYTE_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1071]:  lbyte  :  longtext -> lbyte
*/
{ ADFI_667_LONGTEXT_TO_LBYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LBYTE_TYPE,
    DB_LTXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1072]:  lbyte  :  byte -> lbyte
*/
{ ADFI_666_BYTE_TO_LBYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LBYTE_TYPE,
    DB_BYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1073]:  lbyte  :  varbyte -> lbyte
*/
{ ADFI_664_VARBYTE_TO_LBYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LBYTE_TYPE,
    DB_VBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1074]:  lbyte  :  lbyte -> lbyte
*/
{ ADFI_663_LBYTE_TO_LBYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_RESLEN, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LBYTE_TYPE,
    DB_LBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1075]:  lvch  :  lbyte -> lvch
*/
{ ADFI_782_LBYTE_TO_LVCH, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LVCH_TYPE,
    DB_LBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1076]:  lbyte  :  lvch -> lbyte
*/
{ ADFI_783_LVCH_TO_LBYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LBYTE_TYPE,
    DB_LVCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1077]:  char  :  i -> char
*/
{ ADFI_1337_I_TO_CHAR, ADI_NOFI,
  ADI_COERCION, ADI_F8192_QUEL_CLOAKED, ADI_NOOP,
  {ADI_PRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1078]:  char  :  f -> char
*/
{ ADFI_1338_F_TO_CHAR, ADI_NOFI,
  ADI_COERCION, ADI_F8192_QUEL_CLOAKED, ADI_NOOP,
  {ADI_PRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1079]:  char  :  byte -> char
*/
{ ADFI_662_BYTE_TO_CHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_BYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1080]:  char  :  varbyte -> char
*/
{ ADFI_661_VARBYTE_TO_CHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1TC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_VBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1081]:  varchar  :  i -> varchar
*/
{ ADFI_1339_I_TO_VARCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F8192_QUEL_CLOAKED, ADI_NOOP,
  {ADI_TPRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1082]:  varchar  :  f -> varchar
*/
{ ADFI_1340_F_TO_VARCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F8192_QUEL_CLOAKED, ADI_NOOP,
  {ADI_TPRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1083]:  varchar  :  byte -> varchar
*/
{ ADFI_660_BYTE_TO_VARCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_BYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1084]:  varchar  :  varbyte -> varchar
*/
{ ADFI_659_VARBYTE_TO_VARCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_VBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1085]:  longtext  :  byte -> longtext
*/
{ ADFI_657_BYTE_TO_LONGTEXT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LTXT_TYPE,
    DB_BYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1086]:  longtext  :  varbyte -> longtext
*/
{ ADFI_788_VARBYTE_TO_LONGTEXT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LTXT_TYPE,
    DB_VBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1087]:  dec  :  char -> dec
*/
{ ADFI_971_CHA_TO_DEC, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, 16, 0x1f0f}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DEC_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1088]:  dec  :  varchar -> dec
*/
{ ADFI_972_VCH_TO_DEC, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, 16, 0x1f0f}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DEC_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1089]:  dec  :  nchar -> dec
*/
{ ADFI_1319_NCHR_TO_DEC, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, 16, 0x1f0f}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DEC_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1090]:  dec  :  nvarchar -> dec
*/
{ ADFI_1320_NVCHR_TO_DEC, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, 16, 0x1f0f}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DEC_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1091]:  dec  :  text -> dec
*/
{ ADFI_973_TXT_TO_DEC, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, 16, 0x1f0f}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DEC_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1092]:  dec  :  c -> dec
*/
{ ADFI_974_C_TO_DEC, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, 16, 0x1f0f}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DEC_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1093]:  char  :  dec -> char
*/
{ ADFI_975_DEC_TO_CHA, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_TPRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1094]:  varchar  :  dec -> varchar
*/
{ ADFI_976_DEC_TO_VCH, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_TPRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1095]:  text  :  dec -> text
*/
{ ADFI_977_DEC_TO_TXT, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_TPRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1096]:  text  :  i -> text
*/
{ ADFI_1335_I_TO_TXT, ADI_NOFI,
  ADI_COERCION, ADI_F8192_QUEL_CLOAKED, ADI_NOOP,
  {ADI_TPRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1097]:  text  :  f -> text
*/
{ ADFI_1336_F_TO_TXT, ADI_NOFI,
  ADI_COERCION, ADI_F8192_QUEL_CLOAKED, ADI_NOOP,
  {ADI_TPRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1098]:  c  :  dec -> c
*/
{ ADFI_978_DEC_TO_C, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_TPRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHR_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1099]:  nvarchar  :  nchar -> nvarchar
*/
{ ADFI_1024_NCHAR_TO_NVCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1100]:  nchar  :  nvarchar -> nchar
*/
{ ADFI_1025_NVCHAR_TO_NCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_O1TC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NCHR_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1101]:  nchar  :  nchar -> nchar
*/
{ ADFI_1026_NCHAR_TO_NCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_RESLEN, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NCHR_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1102]:  nvarchar  :  nvarchar -> nvarchar
*/
{ ADFI_1027_NVCHAR_TO_NVCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_RESLEN, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1103]:  char  :  nchar -> char
*/
{ ADFI_1080_NCHAR_TO_CHAR, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_UTOC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1104]:  varchar  :  nchar -> varchar
*/
{ ADFI_1081_NCHAR_TO_VARCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_UTOVARC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1105]:  char  :  nvarchar -> char
*/
{ ADFI_1082_NVCHAR_TO_CHAR, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_VARUTOC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHA_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1106]:  nvarchar  :  nvarchar -> varchar
*/
{ ADFI_1083_NVCHAR_TO_VARCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_UTOC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1107]:  nchar  :  char -> nchar
*/
{ ADFI_1084_CHAR_TO_NCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_CTOU, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NCHR_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1108]:  nvarchar  :  char -> nvarchar
*/
{ ADFI_1085_CHAR_TO_NVCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_CTOVARU, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1109]:  nchar  :  decimal -> nchar
*/
{ ADFI_1341_DEC_TO_NCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F8192_QUEL_CLOAKED, ADI_NOOP,
  {ADI_NPRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NCHR_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1110]:  nchar  :  i -> nchar
*/
{ ADFI_1342_I_TO_NCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F8192_QUEL_CLOAKED, ADI_NOOP,
  {ADI_NPRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NCHR_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1111]:  nchar  :  f -> nchar
*/
{ ADFI_1343_F_TO_NCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F8192_QUEL_CLOAKED, ADI_NOOP,
  {ADI_NPRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NCHR_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1112]:  nchar  :  varchar -> nchar
*/
{ ADFI_1086_VARCHAR_TO_NCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_VARCTOU, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NCHR_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1113]:  nvarchar  :  decimal -> nvarchar
*/
{ ADFI_1344_DEC_TO_NVCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F8192_QUEL_CLOAKED, ADI_NOOP,
  {ADI_NTPRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_DEC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1114]:  nvarchar  :  i -> nvarchar
*/
{ ADFI_1345_I_TO_NVCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F8192_QUEL_CLOAKED, ADI_NOOP,
  {ADI_NTPRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_INT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1115]:  nvarchar  :  f -> nvarchar
*/
{ ADFI_1346_F_TO_NVCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F8192_QUEL_CLOAKED, ADI_NOOP,
  {ADI_NTPRINT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_FLT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1116]:  nvarchar  :  varchar -> nvarchar
*/
{ ADFI_1087_VARCHAR_TO_NVCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_CTOU, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1117]:  longtext  :  nchar -> longtext
*/
{ ADFI_1056_NCHAR_TO_LONGTEXT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_UTOVARC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LTXT_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1118]:  char  :  longtext -> nchar
*/
{ ADFI_1057_LONGTEXT_TO_NCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_VARCTOU, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NCHR_TYPE,
    DB_LTXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1119]:  nvarchar  :  longtext -> nvarchar
*/
{ ADFI_1058_NVCHAR_TO_LONGTEXT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_CTOU, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_LTXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1120]:  longtext  :  nvarchar -> longtext
*/
{ ADFI_1059_LONGTEXT_TO_NVCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_UTOC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LTXT_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1121]:  byte  :  nchar -> byte
*/
{ ADFI_1060_NCHAR_TO_BYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_CTOU, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BYTE_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1122]:  byte  :  nvarchar -> byte
*/
{ ADFI_1061_NVCHAR_TO_BYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_VARUTOC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_BYTE_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1123]:  varbyte  :  nchar -> varbyte
*/
{ ADFI_1062_NCHAR_TO_VARBYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_UTOVARC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBYTE_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1124]:  varbyte  :  nvarchar -> varbyte
*/
{ ADFI_1063_NVCHAR_TO_VARBYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_UTOC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VBYTE_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1125]:  lbyte  :  nchar -> lbyte
*/
{ ADFI_1064_NCHAR_TO_LBYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LBYTE_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1126]:  lbyte  :  nvarchar -> lbyte
*/
{ ADFI_1065_NVCHAR_TO_LBYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LBYTE_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1127]:  nchar  :  byte -> nchar
*/
{ ADFI_1066_BYTE_TO_NCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_CTOU, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NCHR_TYPE,
    DB_BYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1128]:  nvarchar  :  byte -> nvarchar
*/
{ ADFI_1067_BYTE_TO_NVCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_CTOVARU, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_BYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1129]:  nchar  :  varbyte -> nchar
*/
{ ADFI_1068_VARBYTE_TO_NCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_VARCTOU, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NCHR_TYPE,
    DB_VBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1130]:  nvarchar  :  varbyte -> nvarchar
*/
{ ADFI_1069_VARBYTE_TO_NVCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_CTOU, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_VBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1131]:  longtext  :  long_nvarchar -> longtext
*/
{ ADFI_1070_LONGNVCHAR_TO_LONGTEXT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LNVCHR_TYPE,
    DB_LTXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1132]:  long_nvarchar  :  longtext -> long_nvarchar
*/
{ ADFI_1071_LONGTEXT_TO_LONGNVCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_UTOC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LTXT_TYPE,
    DB_LNVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1133]:  long_nvarchar  :  nchar -> long_nvarchar
*/
{ ADFI_1072_NCHAR_TO_LONGNVCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LNVCHR_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1134]:  long_nvarchar  :  nvarchar -> long_nvarchar
*/
{ ADFI_1073_NVCHAR_TO_LONGNVCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LNVCHR_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1135]:  long_nvarchar  :  long_nvarchar -> long_nvarchar
*/
{ ADFI_1075_LNVARCHAR_TO_LNVARCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LNVCHR_TYPE,
    DB_LNVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1136]:  c  :  nchar -> c
*/
{ ADFI_1124_NCHAR_TO_CHR, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_UTOC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHR_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1137]:  nchar  :  c -> nchar
*/
{ ADFI_1125_CHR_TO_NCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_CTOU, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NCHR_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1138]:  c  :  nvarchar -> c
*/
{ ADFI_1126_NVCHAR_TO_CHR, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_VARUTOC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHR_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1139]:  nvarchar  :  c -> nvarchar
*/
{ ADFI_1127_CHR_TO_NVCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_CTOVARU, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_CHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1140]:  text  :  nchar -> text
*/
{ ADFI_1128_NCHAR_TO_TXT, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_UTOVARC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1141]:  nchar  :  text -> nchar
*/
{ ADFI_1129_TXT_TO_NCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_VARCTOU, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NCHR_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1142]:  text  :  nvarchar -> text
*/
{ ADFI_1130_NVCHAR_TO_TXT, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_UTOC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TXT_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1143]:  nvarchar  :  text -> nvarchar
*/
{ ADFI_1131_TXT_TO_NVCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_CTOU, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_TXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1144]:  date  :  nchar -> date
*/
{ ADFI_1132_NCHAR_TO_DATE, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DTE_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1145]:  nchar  :  date -> nchar
*/
{ ADFI_1133_DATE_TO_NCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 25, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NCHR_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1146]:  date  :  nvarchar -> date
*/
{ ADFI_1134_NVCHAR_TO_DATE, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 12, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_DTE_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1147]:  nvarchar  :  date -> nvarchar
*/
{ ADFI_1135_DATE_TO_NVCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 25+DB_CNTSIZE, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_DTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1148]:  money  :  nchar -> money
*/
{ ADFI_1136_NCHAR_TO_MONEY, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_MNY_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1149]:  nchar  :  money -> nchar
*/
{ ADFI_1137_MONEY_TO_NCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 20, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NCHR_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1150]:  money  :  nvarchar -> money
*/
{ ADFI_1138_NVCHAR_TO_MONEY, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 8, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_MNY_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1151]:  nvarchar  :  money -> nvarchar
*/
{ ADFI_1139_MONEY_TO_NVCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F1_VARFI, ADI_NOOP,
  {ADI_FIXED, 20+DB_CNTSIZE, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_MNY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1152]:  logical_key  :  nchar -> logical_key
*/
{ ADFI_1140_NCHAR_TO_LOGKEY, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, DB_LEN_OBJ_LOGKEY, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LOGKEY_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1153]:  nchar  :  logical_key -> nchar
*/
{ ADFI_1141_LOGKEY_TO_NCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, DB_LEN_OBJ_LOGKEY, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NCHR_TYPE,
    DB_LOGKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1154]:  logical_key  :  nvarchar -> logical_key
*/
{ ADFI_1142_NVCHAR_TO_LOGKEY, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, DB_LEN_OBJ_LOGKEY, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LOGKEY_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1155]:  nvarchar  :  logical_key -> nvarchar
*/
{ ADFI_1143_LOGKEY_TO_NVCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, DB_LEN_OBJ_LOGKEY, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_LOGKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1156]:  table_key  :  nchar -> table_key
*/
{ ADFI_1144_NCHAR_TO_TABKEY, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, DB_LEN_TAB_LOGKEY, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TABKEY_TYPE,
    DB_NCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1157]:  nchar  :  table_key -> nchar
*/
{ ADFI_1145_TABKEY_TO_NCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, DB_LEN_TAB_LOGKEY, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NCHR_TYPE,
    DB_TABKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1158]:  table_key  :  nvarchar -> table_key
*/
{ ADFI_1146_NVCHAR_TO_TABKEY, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, DB_LEN_TAB_LOGKEY, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_TABKEY_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1159]:  nvarchar  :  table_key -> nvarchar
*/
{ ADFI_1147_TABKEY_TO_NVCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, DB_LEN_TAB_LOGKEY, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_NVCHR_TYPE,
    DB_TABKEY_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1160]:  utf8  :  char -> utf8
*/
{ ADFI_1186_CHAR_UTF8, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_CTOU, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_UTF8_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1161]:  utf8  :  varchar -> utf8
*/
{ ADFI_1187_VARCHAR_UTF8, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_VARCTOU, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_UTF8_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1162]:  char  :  utf8 -> char
*/
{ ADFI_1188_UTF8_CHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_UTOC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_CHR_TYPE,
    DB_UTF8_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1163]:  varchar  :  utf8 -> varchar
*/
{ ADFI_1189_UTF8_VARCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_UTOVARC, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_VCH_TYPE,
    DB_UTF8_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1164]:  long_nvarchar  :  char -> long_nvarchar
*/
{ ADFI_1198_CHAR_TO_LONGNVCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LNVCHR_TYPE,
    DB_CHA_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1165]:  long_nvarchar  :  varchar -> long_nvarchar
*/
{ ADFI_1199_VCHAR_TO_LONGNVCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LNVCHR_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1166]:  long nvarchar : lnloc -> lnvchr
*/
{ ADFI_1258_LNLOC_TO_LNVCHR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LNVCHR_TYPE,
    DB_LNLOC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1167]:  long byte : lbloc -> lbyte
*/
{ ADFI_1259_LBLOC_TO_LBYTE, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LBYTE_TYPE,
    DB_LBLOC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1168]:  long varchar : lcloc -> lvch
*/
{ ADFI_1260_LCLOC_TO_LVCH, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LVCH_TYPE,
    DB_LCLOC_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1169]:  long nvarchar locator : lnvchr -> lnloc
*/
{ ADFI_1261_LNVCHR_TO_LNLOC, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, ADP_LOC_PERIPH_SIZE, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LNLOC_TYPE,
    DB_LNVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1170]:  long byte locator : lbyte -> lbloc
*/
{ ADFI_1262_LBYTE_TO_LBLOC, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, ADP_LOC_PERIPH_SIZE, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LBLOC_TYPE,
    DB_LBYTE_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1171]:  long varchar locator : lvch-> lcloc
*/
{ ADFI_1263_LVCH_TO_LCLOC, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, ADP_LOC_PERIPH_SIZE, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LCLOC_TYPE,
    DB_LVCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1172]:  longtext  :  longtext -> longtext
*/
{ ADFI_1271_LTXT_TO_LTXT, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_O1CT, 0, 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LTXT_TYPE,
    DB_LTXT_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1173]:  long_nvarchar  :  nvarchar -> long_varchar
*/
{ ADFI_1278_NVCHAR_TO_LONGVCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LVCH_TYPE,
    DB_NVCHR_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** [1174]:  long_nvarchar  :  varchar -> long_nvarchar
*/
{ ADFI_1279_VCHAR_TO_LONGNVCHAR, ADI_NOFI,
  ADI_COERCION, ADI_F0_NOFIFLAGS, ADI_NOOP,
  {ADI_FIXED, sizeof(ADP_PERIPHERAL), 0}, 0, ADE_1CXI_SET_SKIP,
  1, DB_LNVCHR_TYPE,
    DB_VCH_TYPE, DB_NODT, DB_NODT, DB_NODT
},
/*
** End-of-table null entry
*/
{ ADFI_ENDTAB, ADFI_ENDTAB,
  0, 0, ADI_NOOP,
  {0, 0, 0}, 0, 0,
  0, DB_NODT,
    DB_NODT, DB_NODT, DB_NODT, DB_NODT
}
};

GLOBALDEF const i4 Adi_fis_size = sizeof(Adi_fis);
