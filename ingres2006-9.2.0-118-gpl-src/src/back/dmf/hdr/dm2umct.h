/*
**Copyright (c) 2004 Ingres Corporation
*/

/**
** Name: DM2UMCT.H - Definitions for modify table utility routines.
**
** Description:
**      This file decribes the routines that modify table structures.
**
** History:
**      25-mar-1987 (Jennifer)
**          Split from dm2u.h.
**      17-may-90 (jennifer)
**          Added MCT_KEEPDATA and MCT_USEFHDR to mct_rebuild flag.
**	17-dec-1990 (bryanp)
**	    Added new fields to support Btree Index Compression
**	14-jun-1991 (Derek)
**	    Reorganized a little in support for new build interface.
**	10-mar-1992 (bryanp)
**	    Added new fields for in-memory builds.
**	7-July-1992 (rmuth)
**	    Prototyping DMF, changed mct_fhdrnext and mct_fmapnext into
**	    DM_PAGENO from i4s
**    	05-jun-1992 (kwatts)
**          Added mct_acc_tlv to DM2U_M_CONTEXT and changed mct_atts_ptr
**          to mct_data_atts and gave one more level of indirection for
**          new compression interface that bryanp first introduced.
**	29-August-1992 (rmuth)
**	    New build code add mct_lastused_data,mct_fhdr_pageno and
**	    mct_last_dpage removed mct_fmapnext and mct_fhdrnext.
**	22-oct-92 (rickh)
**	    Replaced all references to DMP_ATTS with DB_ATTS.
**	30-October-1992 (rmuth)
**	    Add flag mct_guarantee_on_disk to the mct control_block.
**	26-jul-1993 (bryanp)
**	    Replace all uses of DM_LKID with LK_LKID.
**	26-July-1993 (rmuth)
**	    Add mct_override_alloc, whilst building a table we usually
**	    preallocate mct_allocation amount. Sometimes we know we are
**	    going to use more than this amount this field allows us to
**	    allocate accordingly. For example if a user has specified an
**	    allocation of x pages for a HASH table but we calculate that
**	    we will use x+N hash buckets. This allows us to pre-allocate x+N 
**	    pages hopefully reducing fragmentation on extent based file 
**	    systems.
**	08-may-1995 (shero03)
**	    Added RTREE support
**	06-mar-1996 (stial01 for bryanp)
**	    Add mct_page_size to hold the page size for build routines.
**	06-may-1996 (nanpr01)
**	    Add mct_acc_plv for supporting different page accessor function.
**	22-nov-1996 (nanpr01)
**	    Add mct_ver_number for loading data with appropriate version 
**	    number.
**      10-mar-1997 (stial01)
**          Added mct_crecord, a buffer to use for compression
**      07-Dec-1998 (stial01)
**          Added mct_kperleaf to distinguish from mct_kperpage
**      12-apr-1999 (stial01)
**          Different att/key info in DM2U_M_CONTEXT for (btree) 
**          LEAF vs. INDEX pages
**      23-oct-2000 (stial01)
**          Variable Page Size - Variable Page Type support (SIR 102955)
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      10-feb-2004 (stial01)
**          Support for 256K rows, rows spanning pages.
**/

/*
**  Forward typedef references.
*/

typedef struct  _DM2U_M_CONTEXT	DM2U_M_CONTEXT;


typedef struct _MCT_BUFFER
{
    i4		    mct_start;	    /* Offset into mct_data buffer. */
    i4		    mct_last;	    /* Current used offset for this window. */
    i4		    mct_finish;	    /* Maximum offset for this window. */
    DMPP_PAGE	    *mct_pages;	    /* Window of file. */
}   MCT_BUFFER;

/*}
** Name: DM2U_M_CONTEXT - Context block used by modify.
**
** Description:
**      This typedef is used by the modify dmu to keep context
**      between calls to the lower level routines which are 
**      building the new table one record at a time.  The 
**      normal high level put routines are not used to provide
**      for a more efficient method of restructuring tables.
**      Since this context is used by all access methods, it
**      contains many entries which may be ignored by any
**      one access method.
**
** History:
**     27-jan-1986 (Jennifer)
**          Created new for Jupiter.
**	19-aug-1991 (bryanp)
**	    Added new fields to support Btree Index Compression: mct_index_comp,
**	    mct_index_atts, mct_index_attcount. We need attribute information
**	    and a count of the number of attributes in order to pass to the
**	    compression routine, and we need to know what type of index
**	    compression (if any) is in effect.
**	    We use mct_index_maxlen to determine the worst case length of an
**	    entry on a page, to determine when a Btree index page is full.
**	20-aug-1991 (bryanp)
**	    Removed mct_parent. It's obsolete in 6.5.
**	30-jan-1992 (bonobo)
**	    Removed the redundant typedef to quiesce the ANSI C 
**	    compiler warnings.
**	10-mar-1992 (bryanp)
**	    Added new fields for in-memory builds.
**    	05-jun-1992 (kwatts)
**          Added mct_acc_tlv to DM2U_M_CONTEXT and changed mct_atts_ptr
**          to mct_data_atts and gave one more level of indirection for
**          new compression interface that bryanp first introduced.
**	29-August-1992 (rmuth)
**	    New build code add mct_lastused_data,mct_fhdr_pageno and
**	    mct_last_dpage removed mct_fmapnext and mct_fhdrnext.
**	30-October-1992 (rmuth)
**	    Add flag mct_guarantee_on_disk to the mct control_block.
**	26-jul-1993 (bryanp)
**	    Replace all uses of DM_LKID with LK_LKID.
**	06-mar-1996 (stial01 for bryanp)
**	    Add mct_page_size to hold the page size for build routines.
**	06-may-1996 (nanpr01)
**	    Add mct_acc_plv for supporting different page accessor function.
**	22-nov-1996 (nanpr01)
**	    Add mct_ver_number for loading data with appropriate version 
**	    number.
**	06-Jan-2004 (jenjo02)
**	    Added mct_tidsize, mct_partno for Global Indexes.
**	28-Feb-2004 (jenjo02)
**	    Deleted some unused defines, fields, realigned stuff
**	    to make it easier on the eyes.
**	12-Aug-2004 (schka24)
**	    Build data structure cleanup: delete unused members.
**	13-Apr-2006 (jenjo02)
**	    Add mct_cx_ixkey_atts, mct_clustered for CLUSTERED
**	    primary Btree tables.
*/
struct _DM2U_M_CONTEXT
{
    DMP_RCB         *mct_oldrcb;        /* Pointer to the rcb of table
                                        ** that is being modified. */

    /* mct_buildrcb == mct_oldrcb normally, but if we're doing an inmemory
    ** build, buildrcb points to a temporary table rcb used for holding
    ** buffer cache pages for the table being built.  For non-inmemory,
    ** it doesn't matter which you use, oldrcb or buildrcb.
    */
    DMP_RCB         *mct_buildrcb;      /* RCB for running the build into */
    DB_ATTS	    **mct_key_atts;	/* Pointer to pointers to attributes. */
    DB_ATTS	    **mct_index_atts;	/* Pointer to pointers to attributes. */
    DB_ATTS         **mct_data_atts;    /* Pointer to pointers to attributes. */
    i4         	    mct_keys;           /* Number of attributes in key. */
    i4         	    mct_klen;           /* Key length in bytes. */
    i4	    	    mct_relatts;	/* Number of attributes in tuple. */
    i4	    	    mct_relwid;		/* Tuple length in bytes. */
    i4	    	    mct_index_attcount;	/* Number of attributes in index entry*/
    i4	    	    mct_structure;	/* Structure of table */
    i4         	    mct_kperpage;       /* Max BTREE/ISAM keys per index page */
    i4         	    mct_kperleaf;       /* Max BTREE/RTREE keys per leaf page */
    i4         	    mct_compressed;     /* Type of compression for modifying 
                                        ** to compressed. */
    i4	    	    mct_index_comp;	/* Indicates whether index compression
					** is in use. */
    i4	    	    mct_index_maxlen;	/* Worst case index entry length */
    i4	    	    mct_allocation;	/* File allocation information. */
    i4	    	    mct_override_alloc; /* File allocation override */
    i4	    	    mct_extend;		/* File extend information. */
    i4	    	    mct_page_size;	/* page size for new table */
    i2      	    mct_page_type;      /* page type for new table */
#define MCT_PLEAF  0
#define MCT_PINDEX 1
    /*
    ** BTREE sec index: index entries may differ from leaf entries
    ** In dm1bbbegin() btree specific mct fields are init
    */
    DB_ATTS         mct_cx_tidp;	/* Att entry for index page tidp */
    DB_ATTS         mct_cx_ixkey_atts[DB_MAXKEYS];
					/* Copies of keys for CLUSTERED */
    DB_ATTS         *mct_cx_ix_keys[DB_MAXKEYS]; /* Atts/keys for Index */

    DB_ATTS         **mct_cx_atts[2];    /* Atts for Leaf/Index */
    i4              mct_cx_attcnt[2];    /* Number of atts on Leaf/Index */
    DB_ATTS         **mct_cx_keys[2];    /* Keys for Leaf/Index */
    i4              mct_cx_klen[2];      /* Entry len for Leaf/Index */
    i4              mct_cx_kperpage[2];  /* Kperpage for Leaf/Index */
    i4              mct_cx_maxentry[2];  /* Max entry len (includes
					    compression adjustment */
    BITFLD	    mct_guarantee_on_disk:1;
				/* Guarantee what we just built is on disc */
    BITFLD	    mct_free_bits:31;
    i4		    mct_inmemory;	/* Indicates in-memory build yes/no */
    i4		    mct_unique;         /* Indicator for modifying to unique. */
    i4		    mct_rebuild;	/* Indicates table is being built over
					** an already existing table. */
#define             MCT_REBUILD         0x1
#define             MCT_KEEPDATA        0x2
#define             MCT_USEFHDR         0x4
    i4		    mct_recreate;	/* Indicates new file is to be built and
					** renamed when complete. */
    i4		    mct_heapdirect;	/* Indicates to bypass sorter */
    i4		    mct_index;		/* Indicator for secondary index. */
    i4		    mct_clustered;	/* Indicator for clustered primary btree. */
    i2		    mct_partno;		/* Partition number */
    i2		    mct_tidsize;	/* Size of TIDs */
    DMP_LOCATION    *mct_location;	/* Locations for new files.*/
    i4         	    mct_loc_count;      /* Count of number of locations. */
    i4         	    mct_d_fill;         /* Data page fill factor, in percent. */
    i4         	    mct_i_fill;         /* Index page fill factor, in percent */
    i4         	    mct_l_fill;         /* Leaf page fill factor, in percent. */
    i4         	    mct_iperpage;       /* Max keys per index page from fill. */
    i4         	    mct_lperpage;       /* Max keys per leaf page from fill. */
    i4	    	    mct_db_fill;	/* Data page fill amount in bytes. */
    i4         	    mct_buckets;        /* Number of Hash buckets. */
    i4         	    mct_curbucket;	/* Current hash bucket being used. */
    i4	    	    mct_startmain;	/* Starting page# of main segment. */
    i4         	    mct_main;           /* Number of main pages in ISAM. */
    i4         	    mct_prim;           /* Number of primary pages in ISAM. */
    i4         	    mct_dupstart;       /* Position of start of duplicates
                                        ** in BTREE leaf page. */
    i4	    	    mct_top;            /* Top of BTREE. */
    DM_PAGENO       mct_lastleaf;       /* Last leaf page. */
    i4		    mct_mwrite_blocks;	/* Number of pages per IO in build. */
    i4	    	    mct_oldpages;	/* Number of allocated pages in file
					** before start of build. */
    i4	    	    mct_allocated;	/* Current file allocation. */
    DM_PAGENO	    mct_lastused;	/* Last page used in heap. */
    i4         	    mct_relpages;	/* Count of number of pages in file. */
    DM_PAGENO       mct_last_data_pageno;/* last data page pageno used in a HEAP
 					 */
    DM_PAGENO       mct_fhdr_pageno;    /* Location of FHDR page. */
    DMPP_ACC_PLV    *mct_acc_plv;       /* Page accessors for new table */
    DMPP_ACC_KLV    *mct_acc_klv;       /* Key accessors for new table */
    DMPP_ACC_TLV    *mct_acc_tlv;       /* Tuple accessors for new table */
    DMPP_PAGE	    *mct_curdata;	/* Points to current build data page */
    DM1B_INDEX	    *mct_curleaf;	/* Points to current build leaf page */
    DM1B_INDEX	    *mct_curindex;	/* Points to current build index page */
    DMPP_PAGE	    *mct_curovfl;	/* Points to current build ovfl page. */
    DMPP_PAGE	    *mct_curseg;
    DMPP_PAGE	    *mct_dtsave;	/* Buffer for BTREE data page */
    DMPP_PAGE	    *mct_ovsave;
    DM1B_INDEX      *mct_lfsave;	/* Buffer for BTREE leaf page */
    DM1B_INDEX      *mct_nxsave;	/* Buffer for BTREE index page */
    DMPP_PAGE       *mct_last_dpage;    /* Points to saved last data used when
					** loading data to HEAP eof */
    char            *mct_crecord;       /* Tuple buffer for compression */
    char	    *mct_segbuf;	/* Row buffer in case row spans pages*/
    i4		    mct_hilbertsize;	/* RTREE hilbert size */
    MCT_BUFFER	    mct_data;		/* Data segment window. */
    MCT_BUFFER	    mct_ovfl;		/* Overflow segment window. */
    PTR		    mct_buffer;		/* Holds buffer of pages. */
    u_i2	    mct_ver_number;	/* curr. version number of the table */
    u_i2	    filler1;		/* filler for alignment */
    DM_PAGENO       mct_ancestors[sizeof(DM_TID) * 8 + 1];
};
