# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
/**
** Name: dutform.qsh  Starview forms header
**
** Description:
**
** History:
**      23-oct-88 (alexc)
**          Creation for this header file.
**	08-mar-1989 (alexc)
**	    Alpha test version.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/*
**  Forward and/or External typedef/struct references.
*/
#define	   DDB_MAXNAME		        (32 + 8)
#define	   DDB_2MAXNAME			(64 + 8)
/*
**	Ingres form declarations.
*/
  typedef char *FORM;
#ifdef	DUT_MAIN
  GLOBALDEF FORM dut_f1_intro;
  GLOBALDEF FORM dut_f2_ldblist;
  GLOBALDEF FORM dut_f3_objlist;
  GLOBALDEF FORM dut_p2_select;
  GLOBALDEF FORM dut_p3_register;
  GLOBALDEF FORM dut_p6_nodelist;
  GLOBALDEF FORM dut_p7_dbattr;
  GLOBALDEF FORM dut_p7_1dbname;
  GLOBALDEF FORM dut_p8_objattr;
  GLOBALDEF FORM dut_p9_viewattr;
  GLOBALDEF FORM dut_p11_perror;
  GLOBALDEF FORM dut_p12_ldblist;
  GLOBALDEF FORM dut_p13_ownlist;
  GLOBALDEF FORM dut_p14_node;
  GLOBALDEF FORM dut_p15_ldb;
  GLOBALDEF FORM dut_p16_table;
#endif
#ifndef	DUT_MAIN
  GLOBALREF FORM dut_f1_intro;
  GLOBALREF FORM dut_f2_ldblist;
  GLOBALREF FORM dut_f3_objlist;
  GLOBALREF FORM dut_p2_select;
  GLOBALREF FORM dut_p3_register;
  GLOBALREF FORM dut_p6_nodelist;
  GLOBALREF FORM dut_p7_dbattr;
  GLOBALREF FORM dut_p7_1dbname;
  GLOBALREF FORM dut_p8_objattr;
  GLOBALREF FORM dut_p9_viewattr;
  GLOBALREF FORM dut_p11_perror;
  GLOBALREF FORM dut_p12_ldblist;
  GLOBALREF FORM dut_p13_ownlist;
  GLOBALREF FORM dut_p14_node;
  GLOBALREF FORM dut_p15_ldb;
  GLOBALREF FORM dut_p16_table;
#endif
/*************************************************************************
       S Q L   B E G I N   D E C L A R A T I O N   S E C T I O N.
*************************************************************************/
  typedef struct _DUT_T1_DDBLIST {
    char dut_t1_1ddbname[DDB_MAXNAME];
    char dut_t1_2ddbowner[DDB_MAXNAME];
    char dut_3cdbname[DDB_MAXNAME];
    char dut_4nodename[DDB_MAXNAME];
    struct _DUT_T1_DDBLIST *dut_t1_4next;
  } DUT_T1_DDBLIST;
  typedef struct _DUT_F1_INTRO {
    int dut_f1_0ddb_num_entry;
    int dut_f1_0ddb_size;
    char dut_f1_1ddbname[DDB_MAXNAME];
    char dut_f1_2nodename[DDB_MAXNAME];
    char dut_f1_3nodename[DDB_MAXNAME];
    DUT_T1_DDBLIST *dut_f1_4ddblist;
    DUT_T1_DDBLIST *dut_f1_4ddblist_curr_p;
  } DUT_F1_INTRO;
  typedef struct _DUT_T2_LDBLIST {
    char dut_t2_3ldbname[DDB_MAXNAME];
    char dut_t2_4ldbtype[DDB_MAXNAME];
    int dut_t2_0ldbid;
    char dut_t2_1status[DDB_MAXNAME];
    char dut_t2_2nodename[DDB_MAXNAME];
  } DUT_T2_LDBLIST;
  typedef struct _DUT_F2_LDBLIST {
    char dut_f2_1ddbname[DDB_MAXNAME];
    char dut_f2_2nodename[DDB_MAXNAME];
    i4 dut_f2_0ldb_num_entry;
    i4 dut_f2_0ldb_size;
    i4 dut_f2_0modify;
    DUT_T2_LDBLIST *dut_f2_3ldblist;
    DUT_T2_LDBLIST *dut_f2_3ldblist_curr_p;
  } DUT_F2_LDBLIST;
  typedef struct _DUT_P3_REGISTER {
    char dut_p3_1table[DDB_MAXNAME];
    char dut_p3_2source[DDB_MAXNAME];
    char dut_p3_3dbms[DDB_MAXNAME];
    char dut_p3_4node[DDB_MAXNAME];
    char dut_p3_5database[DDB_MAXNAME];
    char dut_p3_0objtype[DDB_MAXNAME];
    char dut_p3_0objowner[DDB_MAXNAME];
  } DUT_P3_REGISTER;
  typedef struct _DUT_T3_OBJLIST {
    char dut_t3_1objname[DDB_MAXNAME];
    char dut_t3_2objtype[DDB_MAXNAME];
    i4 dut_t3_0delete;
    i4 dut_t3_0select;
    i4 dut_t3_0objbase;
    char dut_t3_0objtype[DDB_MAXNAME];
    char dut_t3_0subtype[DDB_MAXNAME];
    char dut_t3_0system[DDB_MAXNAME];
    char dut_t3_0ldbname[DDB_MAXNAME];
    char dut_t3_0ldbnode[DDB_MAXNAME];
    char dut_t3_0objowner[DDB_MAXNAME];
  } DUT_T3_OBJLIST;
  typedef struct _DUT_F3_OBJLIST {
    i4 dut_f3_0obj_num_entry;
    i4 dut_f3_0obj_size;
    DUT_P3_REGISTER *dut_f3_0register;
    char dut_f3_1nodename[DDB_MAXNAME];
    char dut_f3_2dbname[DDB_MAXNAME];
    char dut_f3_3owner[DDB_MAXNAME];
    char dut_f3_3show_regist[DDB_MAXNAME];
    char dut_f3_4show_natives[DDB_MAXNAME];
    char dut_f3_5show_view[DDB_MAXNAME];
    char dut_f3_6show_table[DDB_MAXNAME];
    char dut_f3_7show_system[DDB_MAXNAME];
    char dut_f3_8ddbname[DDB_MAXNAME];
    DUT_T3_OBJLIST *dut_f3_9objlist;
    DUT_T3_OBJLIST *dut_f3_9objlist_curr_p;
  } DUT_F3_OBJLIST;
  typedef struct _DUT_P2_SELECT {
    char dut_p2_1nodename[DDB_MAXNAME];
    char dut_p2_2dbname[DDB_MAXNAME];
    char dut_p2_3show_regist[DDB_MAXNAME];
    char dut_p2_4show_natives[DDB_MAXNAME];
    char dut_p2_5show_view[DDB_MAXNAME];
    char dut_p2_6show_table[DDB_MAXNAME];
    char dut_p2_7show_system[DDB_MAXNAME];
  } DUT_P2_SELECT;
  typedef struct _DUT_T4_DBLIST {
    char dut_t4_1dbname[DDB_MAXNAME];
    char dut_t4_2owner[DDB_MAXNAME];
    struct _DUT_T4_DBLIST *dut_t4_3next;
  } DUT_T4_DBLIST;
  typedef struct _DUT_P4_DBLIST {
    int dut_p4_0num_entry;
    int dut_p4_0size;
    char dut_p4_2nodename[DDB_MAXNAME];
    DUT_T4_DBLIST *dut_p4_1dblist;
    DUT_T4_DBLIST *dut_p4_1dblist_curr_p;
  } DUT_P4_DBLIST;
  typedef struct _DUT_T5_TABLELIST {
    char dut_t5_1tablename[DDB_MAXNAME];
    char dut_t5_2owner[DDB_MAXNAME];
    struct _DUT_T5_TABLELIST *dut_t5_3next;
  } DUT_T5_TABLELIST;
  typedef struct _DUT_P5_TABLELIST {
    int dut_p5_0num_entry;
    int dut_p5_0size;
    char dut_p5_2nodename[DDB_MAXNAME];
    char dut_p5_3ldbname[DDB_MAXNAME];
    DUT_T5_TABLELIST *dut_p5_1tablelist;
    DUT_T5_TABLELIST *dut_p5_1tablelist_curr_p;
  } DUT_P5_TABLELIST;
  typedef struct _DUT_T6_NODELIST {
    char dut_t6_1nodename[DDB_MAXNAME];
  } DUT_T6_NODELIST;
  typedef struct _DUT_P6_NODELIST {
    int dut_p6_0num_entry;
    int dut_p6_0size;
    DUT_T6_NODELIST *dut_p6_1nodelist;
    DUT_T6_NODELIST *dut_p6_1nodelist_curr_p;
  } DUT_P6_NODELIST;
  typedef struct _DUT_P7_DBATTR {
    int dut_p7_0access;
    int dut_p7_0dbservice;
    char dut_p7_0nodename[DDB_MAXNAME];
    char dut_p7_1dbname[DDB_MAXNAME];
    char dut_p7_2own[DDB_MAXNAME];
    char dut_p7_3dbdev[DDB_MAXNAME];
    char dut_p7_4ckpdev[DDB_MAXNAME];
    char dut_p7_5jnldev[DDB_MAXNAME];
    char dut_p7_6sortdev[DDB_MAXNAME];
    char dut_p7_7access[DDB_MAXNAME];
    char dut_p7_8dbservice[DDB_MAXNAME];
  } DUT_P7_DBATTR;
  typedef struct _DUT_P8_OBJATTR {
    char dut_p8_1objtype[DDB_MAXNAME];
    char dut_p8_2objname[DDB_MAXNAME];
    char dut_p8_3objowner[DDB_MAXNAME];
    char dut_p8_4create_date[DDB_MAXNAME];
    char dut_p8_5alter_date[DDB_MAXNAME];
    char dut_p8_6objsubtype[DDB_MAXNAME];
    char dut_p8_7objversion[DDB_MAXNAME];
    char dut_p8_8system_use[DDB_MAXNAME];
    char dut_p8_9local_name[DDB_MAXNAME];
    char dut_p8_10dbmstype[DDB_MAXNAME];
    char dut_p8_11nodename[DDB_MAXNAME];
    char dut_p8_12ldbname[DDB_MAXNAME];
  } DUT_P8_OBJATTR;
  typedef struct _DUT_P9_VIEWATTR {
    char dut_p9_1dbname[DDB_MAXNAME];
    char dut_p9_2nodename[DDB_MAXNAME];
    char dut_p9_3view_name[DDB_MAXNAME];
    char dut_p9_4view_owner[DDB_MAXNAME];
    char dut_p9_5view_dml[DDB_MAXNAME];
    char dut_p9_6check_option[DDB_MAXNAME];
    i4 dut_p9_7text_sequence;
    char dut_p9_8text_segment[(DB_MAXNAME << 2)];
  } DUT_P9_VIEWATTR;
  typedef struct _DUT_T12_LDBLIST {
    char dut_t12_1ldbname[DDB_MAXNAME];
    struct _DUT_T12_LDBLIST *dut_t12_2next;
  } DUT_T12_LDBLIST;
  typedef struct _DUT_P12_LDBLIST {
    int dut_p12_0num_entry;
    int dut_p12_0size;
    DUT_T12_LDBLIST *dut_p12_1ldblist;
    DUT_T12_LDBLIST *dut_p12_1ldblist_curr_p;
  } DUT_P12_LDBLIST;
  typedef struct _DUT_T13_OWNLIST {
    char dut_t13_1owner[DDB_MAXNAME];
    struct _DUT_T13_OWNLIST *dut_t13_next;
  } DUT_T13_OWNLIST;
  typedef struct _DUT_P13_OWNLIST {
    int dut_p13_0num_entry;
    int dut_p13_0size;
    DUT_T13_OWNLIST *dut_p13_1ownlist;
    DUT_T13_OWNLIST *dut_p13_1ownlist_curr_p;
  } DUT_P13_OWNLIST;
  typedef struct _DUT_T14_NODE {
    char dut_t14_1node[DDB_MAXNAME];
    struct _DUT_T14_NODE *dut_t14_next;
  } DUT_T14_NODE;
  typedef struct _DUT_P14_NODE {
    int dut_p14_0num_entry;
    int dut_p14_0size;
    DUT_T14_NODE *dut_p14_1node;
    DUT_T14_NODE *dut_p14_1node_curr_p;
  } DUT_P14_NODE;
  typedef struct _DUT_T15_LDB {
    char dut_t15_1ldb[DDB_MAXNAME];
    char dut_t15_2dbms_type[DDB_MAXNAME];
    struct _DUT_T15_LDB *dut_t15_next;
  } DUT_T15_LDB;
  typedef struct _DUT_P15_LDB {
    int dut_p15_0num_entry;
    int dut_p15_0size;
    DUT_T15_LDB *dut_p15_1ldb;
    DUT_T15_LDB *dut_p15_1ldb_curr_p;
  } DUT_P15_LDB;
  typedef struct _DUT_T16_TABLE {
    char dut_t16_1table[DDB_MAXNAME];
    char dut_t16_2own[DDB_MAXNAME];
    char dut_t16_3type[DDB_MAXNAME];
    char dut_t16_4system[DDB_MAXNAME];
    struct _DUT_T16_TABLE *dut_t16_next;
  } DUT_T16_TABLE;
  typedef struct _DUT_P16_TABLE {
    int dut_p16_0num_entry;
    int dut_p16_0size;
    DUT_T16_TABLE *dut_p16_1table;
    DUT_T16_TABLE *dut_p16_1table_curr_p;
  } DUT_P16_TABLE;
/**
** Name: dut.qsh - INGRES/Star database management utility (StarView)
**		    header file.
**
** Description:
**      Define StarView system constants, and Control block structures
**	for DUT_ERRCB, DUT_CB, and DUT_HELPCB;
**
** History:
**      23-oct-88 (alexc)
**          Creation for this header file.
**	08-mar-1989 (alexc)
**	    Alpha test version.
**	25-jul-91 (fpang)
**	    1. Constants to recognize forced consistent databases.
**	    2. Change text of some strings.
**/
/*
**  Forward and/or External typedef/struct references.
*/
/*
**  Defines of other constants.
*/
#define	DUT_MASK		    (0x0000)
/* Dut error */
#define	   DUT_ERROR			(!E_DU_OK)
/*
** Name: DUT_STATUS - Status return by the DUT functions.
**
** Description:
**
** History:
**	02-jul-91 (johnr)
**		Removed excess digits from constants.
*/
#define	   DB_2MAXNAME		        (DB_MAXNAME << 1)
#define	   DB_4MAXNAME		        (DB_MAXNAME << 2)
#define	   DB_8MAXNAME		        (DB_MAXNAME << 3)
#define	   DB_SHORTMSG			(256)
#define	   DDB_MAXPATH			(256)
#define	   DB_LONGMSG			(512)
#define	   DUT_ME_EXTRA			(2)
/*
** Definitions for DUT_FLAGS.
*/
#define	   DUT_FLAG_MASK		(0x000)
#define	   DUT_IIDBDBOPEN		(DUT_FLAG_MASK + 0x001)
#define	   DUT_IIDBDBCLOSE		(DUT_FLAG_MASK + 0x002)
#define	   DUT_DDBOPEN			(DUT_FLAG_MASK + 0x003)
#define	   DUT_DDBCLOSE			(DUT_FLAG_MASK + 0x004)
#define	   DUT_DIRECT_CONNECT		(DUT_FLAG_MASK + 0x005)
#define	   DUT_DIRECT_DISCONNECT	(DUT_FLAG_MASK + 0x006)
#define	   DUT_DIRECT_CONNECT_CDB	(DUT_FLAG_MASK + 0x007)
#define	   DUT_DISCONNECT		(DUT_FLAG_MASK + 0x008)
#define	   DUT_CK_NODENAME		(DUT_FLAG_MASK + 0x011)
#define	   DUT_CK_DDBNAME		(DUT_FLAG_MASK + 0x012)
#define	   DUT_STAR_SERVER_TYPE		(DUT_FLAG_MASK + 0x0F0)
/**************************************************************************
**      Object type for function dut_uu11_chk_criteria.
**************************************************************************/
#define		DUT_UNKNOWN_OBJ		(0xFFF) /* Unknown object type */
#define		DUT_VIEW_MASK		(0x000) /* Mask for Views */
#define		DUT_TABLE_MASK		(0x001) /* Mask for Tables */
#define		DUT_INDEX_MASK		(0x002) /* Mask for Indexes */
#define		DUT_PART_MASK		(0x004) /* Mask for Partition tables */
#define		DUT_REPLIC_MASK		(0x008) /* Mask for Replicate tables */
#define		DUT_REGISTER_MASK	(0x000) /* Mask for Registered 
						* Objects.
						*/
#define		DUT_DD_MASK		(0x040) /* Mask for Distributed
						* Objects.
						*/
#define		DUT_REG_VIEW		(DUT_VIEW_MASK + DUT_REGISTER_MASK)
						/* Registered View */
#define		DUT_REG_TABLE		(DUT_TABLE_MASK + DUT_REGISTER_MASK)
						/* Registered Table */
#define		DUT_DD_VIEW		(DUT_VIEW_MASK + DUT_DD_MASK)
						/* Distributed View */
#define		DUT_DD_TABLE		(DUT_TABLE_MASK + DUT_DD_MASK)
						/* Distributed Table */
#define		DUT_PART_TABLE		(DUT_PART_MASK)
#define		DUT_INDEX		(DUT_INDEX_MASK)
#define		DUT_REPLIC_TABLE	(DUT_REPLIC_MASK)
/**************************************************************************
**      DB connect status.(DUT_S_STATUS)
**************************************************************************/
#define		DUT_S_MASK			(0x000)
#define		DUT_S_NULL			(DUT_S_MASK + 0x000)
#define		DUT_S_CONNECT_IIDBDB		(DUT_S_MASK + 0x001)
#define		DUT_S_DIRCONT_IIDBDB		(DUT_S_MASK + 0x002)
#define		DUT_S_CONNECT_DDB		(DUT_S_MASK + 0x004)
#define		DUT_S_DIRCONT_LDB		(DUT_S_MASK + 0x008)
#define		DUT_S_DIRCONT_CDB		(DUT_S_MASK + 0x010)
#define		DUT_S_NEW_OBJ_SELECT		(DUT_S_MASK + 0x020)
/**************************************************************************
**      DB connect status.(DUT_F_STATUS)
**************************************************************************/
#define		DUT_F_MASK			(0x000)
#define		DUT_P_MASK			(0x010)
#define		DUT_F_NULL			(E_DU_OK)
#define		DUT_F_FORM_1			(DUT_F_MASK + 0x001)
#define		DUT_F_FORM_2			(DUT_F_MASK + 0x002)
#define		DUT_F_FORM_3			(DUT_F_MASK + 0x003)
#define		DUT_P_POPUP_2			(DUT_P_MASK + 0x002)
#define		DUT_P_POPUP_3			(DUT_P_MASK + 0x003)
/**************************************************************************
**	System wide constants.
**************************************************************************/
#define		DUT_X_MASK			(0x000)
#define		DUT_NONE_STAR_SERVER		(DUT_X_MASK + 0xFFFFF63C)
#define		DUT_IS_STAR_SERVER		(DUT_X_MASK + 0x01000000)
#define		DUT_OBJLIST_CHANGED		(DUT_X_MASK + 0x00000010)
#define		DUT_NO_OBJLIST_CHANGE		(DUT_X_MASK + 0x00000100)
/* access field in iidatabase */
#define		DUT_GLOBAL			1
#define		DUT_RES_DDB			2
#define		DUT_DESTROYDB			4
#define		DUT_CREATEDB			8
#define		DUT_OPERATIVE			16
#define		DUT_CONVERTING			32
/* dbservice field in iidatabase */
#define		DUT_LDB_TYPE			0
#define		DUT_DDB_TYPE			1
#define		DUT_CDB_TYPE			2
#define		DUT_RMS_TYPE			4
#define		DUT_FRCD_CNST_TYPE		0x40000000
/**************************************************************************
**	DUT_DATATYPE
**************************************************************************/
#define		DUT_3_DATE	(3)
#define		DUT_5_MONEY	(5)
#define		DUT_20_CHAR	(20)
#define		DUT_21_VARCHAR	(21)
#define		DUT_30_INTEGER	(30)
#define		DUT_31_FLOAT	(31)
#define		DUT_32_C	(32)
#define		DUT_37_VCHAR	(37)
#define		DUT_NULL_3_DATE		(-3)
#define		DUT_NULL_5_MONEY	(-5)
#define		DUT_NULL_20_CHAR	(-20)
#define		DUT_NULL_21_VARCHAR	(-21)
#define		DUT_NULL_30_INTEGER	(-30)
#define		DUT_NULL_31_FLOAT	(-31)
#define		DUT_NULL_32_C		(-32)
#define		DUT_NULL_37_VCHAR	(-37)
#define		DUT_0_NONE_TABLE_TYPE	0
#define		DUT_1_TABLE_TYPE	1
/***************************************************************************
**	B E G I N   S Q L   D E C L A R E   S E C T I O N 
***************************************************************************/
  typedef long DUT_STATUS;
  typedef long DUT_S_STATUS;
  typedef long DUT_F_STATUS;
  typedef long DUT_X_STATUS;
  typedef long DUT_FLAG;
  typedef struct _DUT_HELPCB {
    char dut_hf1_1name[DDB_MAXPATH];
    char dut_hf2_1name[DDB_MAXPATH];
    char dut_hf2_2name[DDB_MAXPATH];
    char dut_hf2_5name[DDB_MAXPATH];
    char dut_hf3_1name[DDB_MAXPATH];
    char dut_hp2_1name[DDB_MAXPATH];
    char dut_hp3_1name[DDB_MAXPATH];
    char dut_hp4_1name[DDB_MAXPATH];
    char dut_hp5_1name[DDB_MAXPATH];
    char dut_hp6_1name[DDB_MAXPATH];
    char dut_hp7_1name[DDB_MAXPATH];
    char dut_hp7_2name[DDB_MAXPATH];
    char dut_hp12_1name[DDB_MAXPATH];
    char dut_hp13_1name[DDB_MAXPATH];
    char dut_hu1_1name[DDB_MAXPATH];
    char dut_hp14_1name[DDB_MAXPATH];
    char dut_hp15_1name[DDB_MAXPATH];
    char dut_hp16_1name[DDB_MAXPATH];
    char dut_hp16_2name[DDB_MAXPATH];
  } DUT_HELPCB;
  typedef struct _DUT_CB {
    i4 dut_c0_server_type;
    char dut_c0_buffer[DDB_MAXNAME];
    char dut_c0_buffer2[DDB_MAXNAME];
    char dut_c0_buffer3[DDB_MAXNAME];
    char dut_c0_iidbdbnode[DDB_MAXNAME];
    char dut_curr_iidbdbnode[DDB_MAXNAME];
    char dut_c1_ddbname[DDB_MAXNAME];
    char dut_c2_nodename[DDB_MAXNAME];
    char dut_c3_ldbname[DDB_MAXNAME];
    char dut_c4_ldbnode[DDB_MAXNAME];
    char dut_c5_cdbname[DDB_MAXNAME];
    char dut_c6_cdbnode[DDB_MAXNAME];
    char dut_c7_currnode[DDB_MAXNAME];
    char dut_c8_prev_ldbnode[DDB_MAXNAME];
    char dut_c9_prev_ldbname[DDB_MAXNAME];
    char dut_c10_prev_ddbnode[DDB_MAXNAME];
    char dut_c11_prev_ddbname[DDB_MAXNAME];
    char dut_c12_username[DDB_MAXNAME];
    char dut_c13_vnodename[DDB_MAXNAME];
    char dut_c15_username[DDB_MAXNAME];
    char dut_c16_browse_node[DDB_MAXNAME];
    char dut_c17_browse_database[DDB_MAXNAME];
    char dut_c18_browse_dbms[DDB_MAXNAME];
    char dut_c19_browse_table[DDB_MAXNAME];
    char dut_c20_browse_own[DDB_MAXNAME];
    char frs_message[DB_SHORTMSG];
    DUT_S_STATUS dut_c7_status;
    DUT_X_STATUS dut_c8_status;
    DUT_F_STATUS dut_c9_status;
    DUT_HELPCB *dut_helpcb;
    DUT_F1_INTRO *dut_form_intro;
    DUT_F2_LDBLIST *dut_form_ldblist;
    DUT_F3_OBJLIST *dut_form_objlist;
    DUT_P2_SELECT *dut_popup_select;
    DUT_P3_REGISTER *dut_popup_register;
    DUT_P6_NODELIST *dut_popup_nodelist;
    DUT_P7_DBATTR *dut_popup_dbattr;
    DUT_P8_OBJATTR *dut_popup_objattr;
    DUT_P9_VIEWATTR *dut_popup_viewattr;
    DUT_P12_LDBLIST *dut_popup_ldblist;
    DUT_P13_OWNLIST *dut_popup_ownlist;
    DUT_P14_NODE *dut_popup_b_node;
    DUT_P15_LDB *dut_popup_b_ldb;
    DUT_P16_TABLE *dut_popup_b_table;
    i2 dut_null_1;
    i2 dut_null_2;
    i2 dut_null_3;
    i2 dut_null_4;
    i2 dut_null_5;
    i2 dut_null_6;
    i2 dut_null_7;
    DUT_STATUS dut_on_error_exit;
    DUT_STATUS dut_on_error_noop;
  } DUT_CB;
  typedef struct _DUT_ERRCB {
    DUT_STATUS dut_e1_errno;
    DUT_STATUS dut_e2_erstatus;
    DUT_STATUS dut_e3_status;
    i4 dut_e4_ingerr;
    i4 dut_e5_utilerr;
    STATUS dut_e6_clerr;
    char *dut_e7_clsyserr;
    char dut_e8_errmsg[ER_MAX_LEN];
    i4 dut_e9_flags;
    i4 dut_e10_form_init;
    DUT_CB *dut_cb;
  } DUT_ERRCB;
# define DUT_M00_DDBNAME "Distributed database name"
# define DUT_M01_LOADDDB "Loading a list of Distributed Database(s) fr\
om node %s. . ."
# define DUT_M02_NOTACCESS "'%s::%s' is not accessible"
# define DUT_M03_ACCESS "'%s::%s' is accessible"
# define DUT_M04_OBJNAME "Object name"
# define DUT_M05_LOAD_DOBJ "Loading Distributed Object: %s. . ."
# define DUT_M06_CONFIRM_REMOVE "Confirm to remove object '%s'(y/n):"
# define DUT_M07_REMOVING_DOBJ "Removing Object '%s'..."
# define DUT_M08_NODENAME "Node Name"
# define DUT_M09_UNKNOWN_ACCESS "Unknown value for access field in iid\
bdb(%d)"
# define DUT_M10_DB_IS_DDB "Database is a Distributed database"
# define DUT_M11_DB_IS_DESTROY "Database is being destroyed"
# define DUT_M12_DB_IS_CREATE "Database is being create"
# define DUT_M13_DB_IS_OPERATE "Database is marked operational"
# define DUT_M14_DB_IS_CONVERTING "Database is being converted"
# define DUT_M15_LDB "Local database"
# define DUT_M16_DDB "Distributed database"
# define DUT_M17_CDB "Coordinator database"
# define DUT_M18_UNKNOWN_DBSERV "Unknown dbservice value (%d)"
# define DUT_M19_LDB_OR_CDB "Local or Distributed Database"
# define DUT_M20_UNKNOWN "Unknown"
# define DUT_M21_REGTABLE "Registered Table"
# define DUT_M22_TABLE "Table"
# define DUT_M23_REGVIEW "Registered View"
# define DUT_M24_VIEW "View"
# define DUT_M25_REGINDEX "registered Index"
# define DUT_M26_INDEX "Index"
# define DUT_M27_STAR_TABLE "Star-Level Table"
# define DUT_M28_DIST_VIEW "Distributed View"
# define DUT_M29_PART_TABLE "Partitioned Table"
# define DUT_M30_REPLICATE_TABLE "Replicate Table"
# define DUT_M31_NATIVE_INDEX "Native Index"
# define DUT_M32_SYSTEM_OBJ "(S) System object"
# define DUT_M33_USER_OBJ "(U) User created object"
# define DUT_M34_NATIVE_OBJ "(N) Native object"
# define DUT_M35_REG_OBJ "(L) Registered object"
# define DUT_M36_LDB_NAME "LDB Name"
# define DUT_M37_OWNER_NAME "Owner Name"
# define DUT_M38_SYSTEM "System"
# define DUT_M39_USER "User"
# define DUT_M40_CREATE_REGIST "Creating Register Object '%s'. . ."
# define DUT_M41_PROMPT00 "Enter %s: "
# define DUT_M42_PARTITION "Partition"
# define DUT_M43_REPLICATE "Replicate"
# define DUT_M44_VNODE00 "II_INS_VNODE"
# define DUT_M45_II_INSTALL "II_INSTALLATION"
# define DUT_M46_II_GCN "II_GCN"
# define DUT_M47_LCL_VNODE "_LCL_VNODE"
# define DUT_M48_PROMPT01 "Please enter VNODE name:"
# define DUT_M49_TEST_DB "Testing Access to database: %s::%s"
# define DUT_M50_UP "up"
# define DUT_M51_DOWN "down"
# define DUT_M52_TEST_NODE "Testing Access to node: %s"
# define DUT_M53_REG_SYNTAX "REGISTER %s AS LINK FROM %s WITH DBMS = %\
s, NODE = %s, DATABASE = %s"
# define DUT_M54_RMS "RMS database"
# define DUT_M55_FRCD_CNST "Forced Consistent database"
#ifdef  DUT_MAIN
GLOBALDEF       DUT_CB  *Dut_gcb;
GLOBALDEF       DUT_ERRCB  *Dut_gerrcb;
#endif
#ifndef DUT_MAIN
GLOBALREF       DUT_CB  *Dut_gcb;
GLOBALREF       DUT_ERRCB  *Dut_gerrcb;
#endif
