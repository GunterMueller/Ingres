/*
**Copyright (c) 2004 Ingres Corporation
*/

/**
** Name: DM2U.H - Definitions for modify table utility routines.
**
** Description:
**      This file decribes the routines that modify table structures.
**
** History:
**      27-jan-1986 (Jennifer)
**          Created new for Jupiter.
**	10-apr-1989 (mikem)
**	    Logical key development.  Added definition of DM2U_SYS_MAINTAINED.
**      08-aug-1989 (teg)
**          Added new definitions to support Table Check/Patch operations.
**	15-aug-1989 (rogerk)
**	    Added support for Non-SQL Gateway.  Added DM2U_GATEWAY attribute.
**	02-mar-90 (andre)
**	    Defined DM2U_VGRANT_OK.  Will be used to indicate to dm2u_create
**	    that DMT_VGRANT_OK must be set in relstat.
**	24-jan-1990 (fred)
**	    Added definitions for use in creating table extensions
**	    (DM2U_TABLE_EXTENSION).
**	25-jan-1990 (fred)
**	    Added definition for use in creating tables which have extensions.
**	    (DM2U_HAS_EXTENSIONS).
**	24-sep-1990 (rogerk)
**	    Merged 6.3 changes into 6.5.
**	6-jul-1992 (bryanp)
**	    Prototyping DMF.
**      04-sep-1992 (fred)
**          Added has_extensions arguments to prototypes for dm2u_destroy()
**	    and dm2u_modify().
**	17-aug-92 (rickh)
**	    Added an argument to dm2u_create for FIPS CONSTRAINTS.
**	29-August-1992 (rmuth)
**	   Add prototype information for new 65 conversion routines.
**	2-oct-1992 (bryanp)
**	    Add char_array argument to dm2u_create() and dm2u_index()
**	    for gateway use.
**	23-oct-92 (teresa)
**	    add DM2U_T_VERBOSE for sir 42489.
**	30-October-1992 (rmuth)
**	   Add prototypes for dm2u_convert and dm2u_conv_rel_update.
**	25-feb-93 (rickh)
**	    Added relstat2 argument to dm2u_index.
**	30-mar-93 (rmuth)
**	   Add mod_options2 field to dm2u_modify.
**	7-apr-93 (rickh)
**	    Added relstat2 argument to dm2u_modify.
**	15-may-1993 (rmuth)
**	   Add support for readonly tables.
**	      - Add DM2U_2_READONLY and DM2U_2_NOREADONLY.
**	      - Add prototype for dm2u_readonly_upd_cats.
**	23-jul-93 (robf)
**	   Add secid param to dm2u_create
**	26-jul-1993 (rmuth)
**	    Add prototype for dm2u_calc_hash_buckets.
**	23-aug-1993 (rogerk)
**	    Add prototype for dm2u_file_create.
**      09-jan-1994 (forky01)
**          Add operation codes for recovery modifies which include
**          LOG_[IN]CONSISTENT, PHYS_[IN]CONSISTENT, and
**          TABLE_RECOVERY_[DIS]ALLOWED
**	      - Add prototype for dm2u_readonly_upd_cats.
**	08-feb-1995 (cohmi01)
**	    For RAW I/O, added DB_RAWEXT_NAME parm to dm2u_create,dm2u_modify
**	    and dm2u_index.
**	05-may-1995 (shero03)
**	    For RTREE, added dimension, range, and  hilbert_size
**	06-mar-1996 (stial01 for bryanp)
**	    Update prototypes for dm2u_create, dm2u_index, dm2u_modify, and
**		dm2u_file_create to pass page_size.
**	    Add page_size argument to dm2u_calc_hash_buckets.
**	06-mar-1996 (stial01 for bryanp)
**	    Add prototype for dm2u_maxreclen.
**      06-mar-1996 (stial01)
**          Changed prototype for dm2u_maxreclen.
**      06-may-1996 (nanpr01)
**          Changed prototype for dm2u_maxreclen.
**      22-jul-1996 (ramra01 for bryanp)
**          Add prototype for dm2u_atable().
**	13-aug-1996 (somsa01)
**	    Added DM2U_ETAB_JOURNAL for enabling journaling after next
**	    checkpoint for etab tables.
**	16-oct-1996 (ramra01)
**	    Add parm to dm2u_atable to pass down the relstat
**	    information.
**      06-dec-1996 (nanpr01)
**          bug 79484 : to let caller know which location failed in the
**          location array we need to pass an extra argument.
**	21-Mar-1997 (jenjo02)
**	    Table priority project:
**	    Added tbl_pri to dm2u_create(), dm2u_modify(), dm2u_index()
**	    prototypes.
**	    Added DM2U_2_TABLE_PRIORITY, DM2U_2_TO_TABLE_PRIORITY flags.
**	15-apr-1997 (shero03)
**	    Remove dimension and hilbertsize from dmuindex
**	31-mar-1998 (nanpr01)
**	    define a structure to pass back and forth all the elements.
**	9-apr-98 (inkdo01)
**	    Add flags for "to persistence/unique_scope=statement" options.
**	30-apr-1998 (nanpr01)
**	    Added the header information for the index control block so 
**	    that dm0m calls can be used.
**     18-may-98 (stial01)
**          Support VPS system catalogs, changed prototype for dm2u_sysmod
**      15-mar-1999 (stial01)
**          Added DM2U_RFP_ENTRY, also changed prototype for dm2u_modify.
**	19-mar-1999 (nanpr01)
**	    Get rid of extent names since only one table is supported per
**	    raw location.
**	25-apr-99 (stephenb)
**	    Add DM2U_T_PERIPHERAL for verifydb peripheral (coupon) checking
**	28-may-1999 (nanpr01)
**	    Added table name and owner name in parameter list to display
**	    sensible error messages in errlog.log.
**      23-oct-2000 (stial01)
**          Variable Page Size - Variable Page Type support (SIR 102955)
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	22-Feb-2001 (jenjo02)
**	    Added indxcb_idx_key to DM2U_INDEX_CB.
**      08-Oct-2002 (hanch04)
**          Replace the i4 for length with a SIZE_TYPE (size_t) to allow
**          64-bit memory allocation.
**      24-jul-2002 (hanal04) Bug 108330 INGSRV 1847
**          Added indxcb_dmveredo to DM2U_INDEX_CB to indicate
**          that we are in redo recovery.
**      02-jan-2004 (stial01)
**          Removed unused DM2U_ETAB_JOURNAL
**	6-Feb-2004 (schka24)
**	    Added name-id to rollforward info block.
**	    Update prototype for create table.
**	10-Feb-2004 (jenjo02)
**	    Added new partitioning parms for 
**	    dm2u_modify.
**	10-Mar-2004 (schka24)
**	    Need one more flag for DROP.
**	    Try to clean up verify flag mess.
**	2-Apr-2004 (schka24)
**	    Add partdef size parameter to modify; add partdef pointerizing
**	    prototype.
**      07-apr-2004 (horda03) Bug 96820/INGSRV850
**          Use DM_ATT for index key's base table attribute reference.
**      14-apr-2004 (stial01)
**	    Added indxcb_maxklen, for E_DM007D_BTREE_BAD_KEY_LENGTH (SIR 108315)
**      22-apr-2004 (stial01)
**          Use unused CB number for DM2U_IND_CB
**	22-apr-2004 (thaju02)
**	    For online modify: Added DM2U_ONLINE_MODIFY. Added ptr rfp_octx to
**	    DM2U_RFP_ENTRY. Added indxcb_dupchk_tabid to DM2U_INDEX_CB.
**	    Added dm2u_put_dupchktbl and dm2u_rfp_load_table.
**	30-apr-2004 (thaju02)
**	    Changed dm2u_put_dupchktbl prototype.
**	30-Apr-2004 (jenjo02)
**	    Added dm2u_check_for_interrupt() prototype.
**	10-may-2004 (thaju02)
**	    Removed dm2u_rfp_load_table. Incorporated into dm2u_load_table.
**	08-jul-2004 (thaju02)
**	    Online Modify - added ptr indxcb_online_fhdr_pageno
**	    to DM2U_INDEX_CB. (B112610)
**	16-jul-2004 (thaju02)
**	    Added DM2U_FREEPG_CB for online_modify. (B112658)
**	    Cleaned up DMU_RFP_ENTRY/DM2U_RFP_ENTRY.
**	10-jan-2005 (thaju02)
**	    Added freepg_tabid to DM2U_FREEPG_CB to identify partition.
**	    Modified dm2u_modify prototype - removed input_rcb and
**	    tmp_tabid, replaced with omcb.
**	    Added dm2u_init_btree_read prototype.
**	11-feb-2005 (thaju02)
**	    Added DM2U_ONLINE_REPART.
**      11-mar-2005 (thaju02)
**          Replace *indxcb_online_fhdr_pageno with indxcb_on_relfhdr.
**	    Added indxcb_on_{reltups, relpages, relprim, relmain}.
**      05-Aug-2005 (hanje04)
**          Back out change 478041 as it should never have been X-integrated
**          into main in the first place.
**	15-Aug-2005 (jenjo02)
**	    Implement multiple DMF LongTerm memory pools and
**	    ShortTerm session memory.
**	    Define DM2U_FPG_CB as DM_DM2U_FPG_CB,
**		   DM2U_IND_CB as DM_DM2U_IND_CB.
**	26-May-2006 (jenjo02)
**	    Added function prototype for dm2uMakeIndAtts()
**	25-Aug-2006 (jonj)
**	    Add dm2uGetTempTableId(), dm2uGetTempTableLocs()
**	    dm2uCreateTempIndexTCB()
**	    for Indexes on TempTables support.
**	26-sep-2007 (kibro01) b119080
**	    Allow modify to nopersistence
*/

/*
**  Forward typedef references.
*/

typedef struct  _DM2U_KEY_ENTRY DM2U_KEY_ENTRY;
typedef struct  _DM2U_INDEX_CB  DM2U_INDEX_CB;
typedef struct  _DM2U_RFP_ENTRY DM2U_RFP_ENTRY;
typedef struct  _DM2U_FREEPG_CB	DM2U_FREEPG_CB;
typedef struct  _DM2U_MOD_CB	DM2U_MOD_CB;


/* 
**  The following flags are used by the dm2u.c code to
**  determine where the request came from.
*/
#define         DM2U_USER_REQUEST   0L
#define         DM2U_MODIFY_REQUEST 1L
#define		DM2U_DROP_NOFILE    2L	/* See dmucb.h DMU_DROP_NOFILE */
#define         DM2U_ONLINE_REPART  0x0004L

/*
**  The following flags are use in the mod_options2 flag
*/
#define                 DM2U_2_ADD_EXTEND               0x0001L
#define                 DM2U_2_READONLY                 0x0002L
#define                 DM2U_2_NOREADONLY               0x0004L
#define                 DM2U_2_LOG_INCONSISTENT         0x0008L
#define                 DM2U_2_LOG_CONSISTENT           0x0010L
#define                 DM2U_2_PHYS_INCONSISTENT        0x0020L
#define                 DM2U_2_PHYS_CONSISTENT          0x0040L
#define                 DM2U_2_TBL_RECOVERY_DISALLOWED  0x0080L
#define                 DM2U_2_TBL_RECOVERY_ALLOWED     0x0100L
#define                 DM2U_2_TBL_RECOVERY_DEFAULT	0x0200L
#define                 DM2U_2_TABLE_PRIORITY		0x0400L
#define                 DM2U_2_TO_TABLE_PRIORITY	0x0800L
#define                 DM2U_2_PERSISTS_OVER_MODIFIES	0x1000L
#define                 DM2U_2_STATEMENT_LEVEL_UNIQUE	0x2000L
#define                 DM2U_2_NOPERSIST_OVER_MODIFIES	0x4000L


/* 
**  The following flags are used by the dm2u.c code to
**  determine create characteristics.
**  The flags marked (*) are also used in the modoptions flag
**  word for modify.
*/
#define                 DM2U_JOURNAL        0x0001L
#define                 DM2U_DUPLICATES     0x0002L  /* (*) */
#define                 DM2U_NODUPLICATES   0x0004L  /* (*) */
#define                 DM2U_TRUNCATE       0x0008L  /* (*) */
#define                 DM2U_CATALOG        0x0010L
#define                 DM2U_EXTCATALOG     0x0020L
#define                 DM2U_REORG          0x0040L  /* (*) */
#define                 DM2U_SYS_MAINTAINED 0x0080L
#define			DM2U_TABLE_EXTENSION 0x0100L
#define			DM2U_HAS_EXTENSIONS 0x0200L
/* 
**  The following flags are used by the dm2u.c code to
**  determine create characteristics.
*/
#define                 DM2U_GATEWAY	    0x10000L
#define			DM2U_SYSMOD	    0x20000L
#define			DM2U_GW_UPDT	    0x40000L
#define			DM2U_ONLINE_END	    0x80000L
#define			DM2U_ONLINE_MODIFY  0x100000L

/*
** DM2U_VGRANT_OK is used to let dm2u_create know that DMT_VGRANT_OK
** must be set in relstat
*/
#define			DM2U_VGRANT_OK	    0x80000L

struct _DM2U_FREEPG_CB
{
    DM2U_FREEPG_CB	*q_next;        /* Next item on free list. */
    DM2U_FREEPG_CB	*q_prev;        /* Previous item on free list. */
    SIZE_TYPE           length;         /* Size of object. */
    i2                  type;           /* Type of object. */
#define         DM2U_FPG_CB     DM_DM2U_FPG_CB
    i2                  s_reserved;     /* Set if pool list is corrupt. */
    PTR                 l_reserved;     /* NOT USED. */
    PTR                 owner;          /* Control block owner. */
    i4                  ascii_id;       /* Hex dump visible tag. */
#define                 DM2U_FPGCB_ASCII_ID      CV_C_CONST_MACRO('F','P','G','B')
    DB_TAB_ID		freepg_tabid;
    DM_PAGENO		freepg_no;
};

/*}
** Name:  DM2U_RFP_ENTRY - RFP transaction information.
**
** Description:
**      This structure is used to describe the session/transaction information
**      when a DM2U operation is done from rollforwarddb.
**
**	This info is needed for DO (rollforwarddb apply of journals.)
**	The INDEX or MODIFY log record is preceded by FCREATE of the temp
**	file(s) that index/modify is to use, and followed by file rename
**	operations.  Index/modify had better use the same file name(s), so
**	we pass the name generator info.  Also, we need to pass the lock
**	and log context, as well as any other info that index or modify
**	would normally compute on its own instead of taking from the
**	index/modify control block.
**
** History:
**      03-15-99 (stial01)
**          Created.
**	6-Feb-2004 (schka24)
**	    Add name-gen ID, rename stmt count.
*/
struct _DM2U_RFP_ENTRY
{
    i4              rfp_log_id;
    i4              rfp_lk_id;
    DB_TRAN_ID      rfp_tran_id;
    i4              rfp_name_gen;	/* Temp filename generator */
    i4              rfp_hash_buckets;
    i2		    rfp_name_id;	/* Name generator ID */
    bool            rfp_upcase;
    PTR		    rfp_octx;
};

/*}
** Name:  DM2U_KEY_ENTRY - Description of key for a database table.
**
** Description:
**      This structure is used to describe the key attributes of
**      the table being created, modified or indexed.
**	This structure is the same as the logical interface(DMU_KEY_ENTRY),
**	if either changes, need to apply to both places.
**
** History:
**      02-jun-85 (ac)
**          Created for Jupiter.
**	30-jan-1992 (bonobo)
**	    Removed the redundant typedef to quiesce the ANSI C 
**	    compiler warnings.
**	03-may-95 (morayf)
**	    Prototyping probs on odt_us5 since the C compiler on SCO has a
**	    limit of 31 prototyped function args. Thus use K&R declaration
**	    (i.e without argument definitions) on dm2u_index().
**	    Must do this with any others which exceed 31 args in future.
*/

struct _DM2U_KEY_ENTRY
{
    DB_ATT_NAME     key_attr_name;          /* Name of table attribute. */
    i4         	    key_order;              /* Sort order of attribute. */
#define                 DM2U_ASCENDING       1L
#define                 DM2U_DESCENDING      2L
};

struct _DM2U_INDEX_CB
{
    DM2U_INDEX_CB       *q_next;        /* Next item on free list. */
    DM2U_INDEX_CB       *q_prev;        /* Previous item on free list. */
    SIZE_TYPE           length;         /* Size of object. */
    i2                  type;           /* Type of object. */
#define         DM2U_IND_CB     DM_DM2U_IND_CB
    i2                  s_reserved;     /* Set if pool list is corrupt. */
    PTR                 l_reserved;     /* NOT USED. */
    PTR                 owner;          /* Control block owner. */
    i4             	ascii_id;       /* Hex dump visible tag. */
#define                 DM2U_IND_ASCII_ID      CV_C_CONST_MACRO('D','M','2','U')
    DMP_DCB             *indxcb_dcb;
    DML_XCB             *indxcb_xcb;
    DB_TAB_NAME         *indxcb_index_name;
    DB_LOC_NAME         *indxcb_location;
    i4             	indxcb_l_count;
    DB_TAB_ID           *indxcb_tbl_id;
    i4             	indxcb_structure;
    i4             	indxcb_i_fill;
    i4             	indxcb_l_fill;
    i4             	indxcb_d_fill;
    i4             	indxcb_compressed;
    i4             	indxcb_index_compressed;
    i4             	indxcb_unique;
    i4             	indxcb_dmveredo;
    i4             	indxcb_min_pages;
    i4             	indxcb_max_pages;
    i4             	indxcb_kcount;
    DM2U_KEY_ENTRY      **indxcb_key;
    i4             	indxcb_acount;
    i4             	indxcb_db_lockmode;
    i4             	indxcb_allocation;
    i4             	indxcb_extend;
    i4             	indxcb_page_type;
    i4             	indxcb_page_size;
    DB_TAB_ID           *indxcb_idx_id;
    f8			*indxcb_range;
    i4             	indxcb_index_flags;
    DM_PTR              *indxcb_gwattr_array;
    DM_DATA             *indxcb_gwchar_array;
    DMU_FROM_PATH_ENTRY *indxcb_gwsource;
    DB_QRY_ID           *indxcb_qry_id;
    i4             	*indxcb_tup_info;
    DB_TAB_NAME         *indxcb_tab_name;
    DB_OWN_NAME         *indxcb_tab_owner;
    i4             	indxcb_reltups;
    i4             	indxcb_gw_id;
    DM_DATA             *indxcb_char_array;
    u_i4                indxcb_relstat2;
    i4             	indxcb_tbl_pri;
    DB_ERROR            *indxcb_errcb;
    i4			indxcb_maxklen; /* returned if btree klen error */
    DM2U_RFP_ENTRY      indxcb_rfp;
    DM_ATT		indxcb_idx_key[DB_MAXIXATTS+2];
    DB_TAB_ID		indxcb_online_tabid;
    DB_TAB_ID		*indxcb_dupchk_tabid;
    i4                  indxcb_on_reltups;
    DM_PAGENO           indxcb_on_relfhdr;
    i4                  indxcb_on_relpages;
    i4                  indxcb_on_relprim;
    i4                  indxcb_on_relmain;
};


/*}
** Name:  DM2U_MOD_CB - Parameters for dm2u_modify()
**
** Description:
**	Describes everything needed by dm2u_modify.
**
** History:
**	25-Apr-2006 (jenjo02)
**	    Created, finally, to get rid of outlandish
**	    parm list, added mcb_clustered for
**	    CLUSTERED Btrees.
*/
struct _DM2U_MOD_CB
{
    DMP_DCB		*mcb_dcb;
    DML_XCB		*mcb_xcb;
    DB_TAB_ID       	*mcb_tbl_id;
    DM2U_OMCB		*mcb_omcb;
    DMU_CB		*mcb_dmu;
    i4			mcb_structure;
    DB_LOC_NAME		*mcb_location;
    i4         		mcb_l_count;
    i4			mcb_i_fill;
    i4			mcb_l_fill;
    i4			mcb_d_fill;
    bool		mcb_compressed;
    bool		mcb_index_compressed;
    bool		mcb_temporary;
    bool		mcb_unique;
    bool		mcb_merge;
    bool		mcb_clustered;
    i4			mcb_min_pages;
    i4			mcb_max_pages;
    i4         		mcb_modoptions;
    i4         		mcb_mod_options2;
    i4         		mcb_kcount;
    DM2U_KEY_ENTRY  	**mcb_key;
    i4         		mcb_db_lockmode;
    i4			mcb_allocation;
    i4			mcb_extend;
    i4			mcb_page_type;
    i4			mcb_page_size;
    i4			*mcb_tup_info;
    i4			mcb_reltups;
    DB_TAB_NAME		*mcb_tab_name;
    DB_OWN_NAME		*mcb_tab_owner;
    i4			*mcb_has_extensions;
    u_i4		mcb_relstat2;
    i4			mcb_flagsmask;
    i4			mcb_tbl_pri;
    DM2U_RFP_ENTRY  	*mcb_rfp_entry;
    DB_PART_DEF		*mcb_new_part_def;
    i4			mcb_new_partdef_size;
    DMU_PHYPART_CHAR 	*mcb_partitions;
    i4			mcb_nparts;
    i4			mcb_verify;
};


/*
**  Forward and/or External function references.
*/

FUNC_EXTERN DB_STATUS	dm2u_ckp_lock(
		DMP_DCB	    *dcb,
		DB_TAB_NAME *tab_name,
		DB_OWN_NAME *owner_name,
		DML_XCB	    *xcb,
		i4	    *err_code);

FUNC_EXTERN DB_STATUS	dm2u_create(
		DMP_DCB		    *dcb,
		DML_XCB		    *xcb,
		DB_TAB_NAME	    *table_name,
		DB_OWN_NAME	    *owner,
		DB_LOC_NAME	    *location,
		i4		    l_count,
		DB_TAB_ID	    *tbl_id,
		DB_TAB_ID	    *idx_id,
		i4		    index,
		i4		    view,
		i4		    table_flags,
		u_i4		    relstat2,
		i4		    structure,
		i4		    ntab_width,
		i4		    attr_count,
		DM2T_ATTR_ENTRY	    **attr_entry,
		i4		    db_lockmode,
		i4		    allocation,
		i4		    extend,
		i4		    page_type,
		i4		    page_size,
		DB_QRY_ID	    *qry_id,
		DM_PTR		    *gwattr_array,
		DM_DATA		    *gwchar_array,
		i4		    gw_id,
		i4		    gwrowcount,
		DMU_FROM_PATH_ENTRY *gwsource,
		DM_DATA		    *char_array,
		i4		    dimension,
		i4		    hilbertsize,
		f8		    *range,
		i4		    tbl_pri,
		DB_PART_DEF	    *dmu_part_def,
		i4		    dmu_partno,
		DB_ERROR	    *errcb);

FUNC_EXTERN DB_STATUS	dm2u_destroy(
		DMP_DCB		*dcb,	
		DML_XCB		*xcb,	
		DB_TAB_ID       *tbl_id,
		i4         db_lockmode,
		i4         flag,
		DB_TAB_NAME	*tab_name,
		DB_OWN_NAME	*tab_owner,
		i4		*is_view,
		i4		*has_extensions,
		i4		*err_code);

FUNC_EXTERN DB_STATUS	dm2u_get_tabid(
		DMP_DCB		*dcb,
		DML_XCB		*xcb,
		DB_TAB_ID	*base_tbl_id,
		bool	        index,
		i4		numids,
		DB_TAB_ID	*new_tab_id,
		i4		*err_code);

FUNC_EXTERN DB_STATUS	dm2u_index(
		DM2U_INDEX_CB	*index_cb);

FUNC_EXTERN DB_STATUS dm2u_load_table(
		DM2U_MXCB	    *mxcb,
		i4		    *err_code);

FUNC_EXTERN DB_STATUS dm2u_bbox_range(            
                DM2U_MXCB       *mxcb,
                ADF_CB          *adf_cb,
                i4         *err_code);

FUNC_EXTERN DB_STATUS	dm2u_modify(
		DM2U_MOD_CB	*mcb,
		DB_ERROR        *errcb);

FUNC_EXTERN DB_STATUS	dm2u_relocate(
		DMP_DCB		*dcb,
		DML_XCB		*xcb,
		DB_TAB_ID       *tbl_id,
		DB_LOC_NAME	*location,
		DB_LOC_NAME	*oldlocation,
		i4         l_count,
		i4         db_lockmode,
		DB_TAB_NAME	*tab_name,
		DB_OWN_NAME	*tab_owner,
		i4		*err_data,
		i4		*err_code);

FUNC_EXTERN DB_STATUS	dm2u_reorg_table(
		DM2U_MXCB	    *mxcb,
		i4		    *err_code);

FUNC_EXTERN DB_STATUS	dm2u_sysmod(
		DMP_DCB		*dcb,
		DML_XCB		*xcb,
		DB_TAB_ID       *tbl_id,
		i4		structure,
		i4		newpgtype,
		i4		newpgsize,
		i4		i_fill,
		i4		l_fill,
		i4		d_fill,
		i4		compressed,
		i4		min_pages,
		i4		max_pages,
		i4		unique,
		i4		merge,
		i4         truncate,
		i4         kcount,
		DM2U_KEY_ENTRY  **key,
		i4         db_lockmode,
		i4		allocation,
		i4		extend,
		i4		*tup_cnt,
		i4		reltups,
		i4		*err_code);

FUNC_EXTERN DB_STATUS dm2u_update_catalogs(
		DM2U_MXCB           *mxcb,
		i4		    journal,
		i4             *err_code);

FUNC_EXTERN DB_STATUS dm2u_readonly_upd_cats(
		DM2U_MXCB           *mxcb,
		i4		    journal,
		i4             *err_code);


FUNC_EXTERN DB_STATUS	dm2u_convert(
		DMP_DCB		*dcb,
		DML_XCB		*xcb,
		DB_TAB_ID	*tbl_id,
		i4		*err_code);

FUNC_EXTERN DB_STATUS	dm2u_conv_rel_update(
		DMP_TCB		*tcb,
		i4		lock_list,
		DB_TRAN_ID	*tran_id,
		i4		*err_code);

FUNC_EXTERN i4	dm2u_calc_hash_buckets(
		i4		page_type,
		i4		page_size,
		i4         fillfactor,
		i4         width,
		i4              row_count,
		i4         min_pages,
		i4         max_pages);

FUNC_EXTERN DB_STATUS	dm2u_file_create(
		DMP_DCB		*dcb,
		i4		log_id,
		i4		lock_id,
		DB_TAB_ID	*tbl_id,
		u_i4	db_sync_flag,
		DMP_LOCATION	*loc_array,
		i4		loc_count,
		i4		page_size,
		i4		logging,
		i4		journal,
		i4		*err_code);
FUNC_EXTERN DB_STATUS dm2u_alterstatus_upd_cats(
		DM2U_MXCB           *mxcb,
		i4		    journal,
		i4             *err_code);

FUNC_EXTERN i4 dm2u_maxreclen(i4 page_type, i4 page_size);

FUNC_EXTERN DB_STATUS	dm2u_atable(
		DMP_DCB		*dcb,
		DML_XCB		*xcb,
		DB_TAB_ID       *tbl_id,
		i4		operation,
		i4		cascade,
		DM2T_ATTR_ENTRY	**attr_entry,
		i4		table_flags,
		i4         db_lockmode,
		DB_TAB_NAME	*tab_name,
		DB_OWN_NAME	*tab_owner,
		i4		*err_code);
FUNC_EXTERN STATUS dm2u_raw_device_lock(
		DMP_DCB             *dcb,
		DML_XCB             *xcb,
		DB_LOC_NAME         *location,
		LK_LKID             *lockid,
		i4                  *err_code);
FUNC_EXTERN DB_STATUS dm2u_raw_location_free(
		DMP_DCB             *dcb,
		DML_XCB             *xcb,
		DB_LOC_NAME         *location,
		i4                  *err_code);

FUNC_EXTERN DB_STATUS dm2u_put_dupchktbl(
		DM2U_TPCB	    *tp,
		char		    *record,
		i4		    *err_code);

FUNC_EXTERN void dm2u_repointerize_partdef(
		DB_PART_DEF	*part_def);

FUNC_EXTERN DMU_PHYPART_CHAR *dm2u_repointerize_ppchar(
		PTR		rawdata);

FUNC_EXTERN DB_STATUS dm2u_check_for_interrupt(
		DML_XCB             *xcb,
		i4                  *err_code);

FUNC_EXTERN DB_STATUS dm2u_init_btree_read(
		DMP_TCB		    *tcb,
		DMP_RNL_ONLINE	    *rnl,
		DM_PAGENO	    lastused_pageno,
		i4		    *err_code);

FUNC_EXTERN DB_STATUS dm2uMakeIndAtts(
		DM2U_INDEX_CB	*index_cb,
		DM2U_MXCB	*m,
		DMP_RCB		*r,
		DM2T_ATTR_ENTRY	**CreAtts,
		i4		*NumCreAtts,
		bool		KeyProjection,
		i4		ProjColMap[],
		i4		*error,
		i4		*error_data);

FUNC_EXTERN DB_STATUS dm2uGetTempTableId(
		DMP_DCB		*dcb,
		i4		lock_list,
		DB_TAB_ID	*base_tbl_id,
		bool		index,
		DB_TAB_ID	*new_tab_id,
		i4		*err_code);

FUNC_EXTERN DB_STATUS dm2uGetTempTableLocs(
		DMP_DCB		*dcb,
		DML_SCB		*scb,
		DB_LOC_NAME	**locs,
		i4		*loc_count,
		DM_OBJECT	**locs_mem,
		i4		*err_code);

FUNC_EXTERN DB_STATUS dm2uCreateTempIndexTCB(
		DMP_TCB		*BaseTCB,
		DM2U_MXCB	*m,
		DM2U_INDEX_CB	*idx,
		i4		AttCount,
		DM2T_ATTR_ENTRY	**AttList,
		i4		*error);
