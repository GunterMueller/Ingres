/*
**Copyright (c) 2004 Ingres Corporation
*/


/* Jam hints
**
LIBRARY = IMPDMFLIBDATA
**
*/

#include    <compat.h>
#include    <gl.h>
#include    <cs.h>
#include    <di.h>
#include    <lo.h>
#include    <me.h>
#include    <pc.h>
#include    <sl.h>
#include    <tm.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <lk.h>
#include    <lg.h>
#include    <st.h>
#include    <adf.h>
#include    <ulf.h>
#include    <dmf.h>
#include    <dmccb.h>
#include    <dmmcb.h>
#include    <dmrcb.h>
#include    <dmtcb.h>
#include    <dmxcb.h>
#include    <dm.h>
#include    <dml.h>
#include    <dmp.h>
#include    <dmpp.h>
#include    <dm1c.h>
#include    <dm1p.h>
#include    <dm2t.h>
#include    <dm2f.h>
#include    <dm0c.h>
#include    <dm0m.h>
#include    <dm1b.h>
#include    <dm0l.h>
#include    <dm0s.h>
#include    <dmm.h>
#include    <dudbms.h>



/*
** Name:	dmmdata.c
**
** Description:	Global data for dmm facility.
**
** History:
**
**	23-sep-96 (canor01)
**	    Created.
**	24-Mar-1997 (jenjo02)
**	    Table priority project:
**	    Added reltcpri (table cache priority) to DMP_RELATION
**      23-oct-2000 (stial01)
**          Variable Page Size - Variable Page Type support (SIR 102955)
**	18-Dec-2003 (schka24)
**	    Added iirelation stuff for partitioned tables.
**	18-feb-2004 (stial01)
**          iirelation changes for 256k rows, relwid, reltotwid i2->i4
**          iiattribute changes for 256k rows, attoff i2->i4
**          Also changed attfml i2->i4
**	23-Feb-2004 (gupsh01)
**	    Added alter table alter column support.
**	30-Jun-2004 (schka24)
**	    relsecid -> relfree1.
**	29-Sep-2004 (drivi01)
**	    Added LIBRARY jam hint to put this file into IMPDMFLIBDATA
**	    in the Jamfile.
**	13-dec-04 (inkdo01)
**	    Added attcollID.
**	30-May-2006 (jenjo02)
**	    Add 32 more idom attributes to IIINDEX for indexes on
**	    clustered tables.
*/

/* dmmcre.c */
/*
**  This table contains the initialized IIRELATION records for the
**  core system tables.  At server startup, we space pad names to
**  DB_MAXNAME positions and fill in the number of attributes per relation.
*/

/* core table ids */

#define NOT_AN_INDEX            0
#define IIRELATION_BASE_ID      1
#define IIREL_IDX_INDEX_ID      2
#define IIATTRIBUTE_BASE_ID     3
#define IIINDEX_BASE_ID         4

#define IIRELATION_TABLE_ID     { IIRELATION_BASE_ID, NOT_AN_INDEX }
#define IIREL_IDX_TABLE_ID      { IIRELATION_BASE_ID, IIREL_IDX_INDEX_ID  }
#define IIATTRIBUTE_TABLE_ID    { IIATTRIBUTE_BASE_ID, NOT_AN_INDEX }
#define IIINDEX_TABLE_ID        { IIINDEX_BASE_ID, NOT_AN_INDEX }

/* covers for CL_OFFSETOF */

#define REL_OFFSET( fieldName ) ( CL_OFFSETOF( DMP_RELATION, fieldName ) )
#define REL_IDX_OFFSET( fieldName ) ( CL_OFFSETOF( DMP_RINDEX, fieldName ) )
#define ATT_OFFSET( fieldName ) ( CL_OFFSETOF( DMP_ATTRIBUTE, fieldName ) )
#define IIINDEX_OFFSET( fieldName ) ( CL_OFFSETOF( DMP_INDEX, fieldName ) )

/* uninitialized fields should identify themselves to the developer */

#define DUMMY_ATTRIBUTE_COUNT   -1
#define DUMMY_ATTRIBUTE_NUMBER  -1
#define DUMMY_TUPLE_COUNT       -1


GLOBALDEF DMP_RELATION DM_core_relations[] =
{
    {	/*  iirelation	*/
    	IIRELATION_TABLE_ID,
	{ 'i', 'i', 'r', 'e', 'l', 'a', 't', 'i', 'o', 'n', 0 },
	{ '$', 'i', 'n', 'g', 'r', 'e', 's', 0 },
	DUMMY_ATTRIBUTE_COUNT, 
	0 /* reltcpri */, 
	1, TCB_HASH,
	TCB_CATALOG | TCB_NOUPDT | TCB_CONCUR | TCB_PROALL | TCB_SECURE | TCB_IDXD
	,
	DUMMY_TUPLE_COUNT, 1, 1, 1, 0, { 0, 0 },
	{ '$', 'd', 'e', 'f', 'a', 'u', 'l', 't', 0 },
	DMF_T_VERSION, 0, 0, 0, 0, 1, 0, 50, 0, 16, 0, 
	DM_PG_SIZE, 
	0, 0, 1, 65536,
        DM_TBL_DEFAULT_FHDR_PAGENO,
        DM_TBL_DEFAULT_ALLOCATION,
        DM_TBL_DEFAULT_EXTEND,
	TCB_C_NONE, TCB_PG_V1,
	0 /* relstat2 */, 
	{ ' ' } /* relfree1 */,
	1 /* relloccount */,
	0 /* relversion */, 
	sizeof(DMP_RELATION) /* relwid */,
	sizeof(DMP_RELATION) /* reltotwid */,
	0 /* relnparts */, 0 /* relnpartlevels */,
	{ ' ' }
    },
    {	/*  iirel_idx	*/
    	IIREL_IDX_TABLE_ID,
	{ 'i', 'i', 'r', 'e', 'l', '_', 'i', 'd', 'x', 0 },
	{ '$', 'i', 'n', 'g', 'r', 'e', 's', 0 },
	DUMMY_ATTRIBUTE_COUNT, 
	0 /* reltcpri */,
	2, TCB_HASH,
	TCB_CATALOG | TCB_NOUPDT | TCB_CONCUR | TCB_PROALL | TCB_SECURE | TCB_INDEX,
	DUMMY_TUPLE_COUNT, 1, 1, 1, 0, { 0, 0 },
	{ '$', 'd', 'e', 'f', 'a', 'u', 'l', 't', 0 },
	DMF_T_VERSION, 0, 0, 0, 0, 0, 0, 50, 0, 8, 0, 
	DM_PG_SIZE, 
	0, 0, 1, 65536,
        DM_TBL_DEFAULT_FHDR_PAGENO,
        DM_TBL_DEFAULT_ALLOCATION,
        DM_TBL_DEFAULT_EXTEND,
	TCB_C_NONE, TCB_PG_V1,
	0 /* relstat2 */, 
	{ ' ' } /* relfree1 */,
	1 /* rellocount */,
	0 /* relversion */, 
	sizeof(DMP_RINDEX) /* relwid */,
	sizeof(DMP_RINDEX) /* reltotwid */,
	0 /* relnparts */, 0 /* relnpartlevels */,
	{ ' ' }
    },
    {	/*  iiattribute	*/
    	IIATTRIBUTE_TABLE_ID,
	{ 'i', 'i', 'a', 't', 't', 'r', 'i', 'b', 'u', 't', 'e', 0 },
	{ '$', 'i', 'n', 'g', 'r', 'e', 's', 0 },
	DUMMY_ATTRIBUTE_COUNT,
	0 /* reltcpri */,
	2, TCB_HASH,
	TCB_CATALOG | TCB_NOUPDT | TCB_CONCUR | TCB_SECURE | TCB_PROALL
	,
	DUMMY_TUPLE_COUNT, 1, 1, 1, 0, { 0, 0 },
	{ '$', 'd', 'e', 'f', 'a', 'u', 'l', 't', 0 },
	DMF_T_VERSION, 0, 0, 0, 0, 0, 0, 50, 0, 32, 0, 
	DM_PG_SIZE, 
	0, 0, 1, 65536,
        DM_TBL_DEFAULT_FHDR_PAGENO,
        DM_TBL_DEFAULT_ALLOCATION,
        DM_TBL_DEFAULT_EXTEND,
	TCB_C_NONE, TCB_PG_V1,
	0 /* relstat2 */, 
	{ ' ' } /* relfree1 */,
	1 /* relloccount */,
	0 /* relversion */, 
	sizeof(DMP_ATTRIBUTE) /* relwid */,
	sizeof(DMP_ATTRIBUTE) /* reltotwid */,
	0 /* relnparts */, 0 /* relnpartlevels */,
	 { ' ' }
    },
    {	/*  iiindex	*/
    	IIINDEX_TABLE_ID,
	{ 'i', 'i', 'i', 'n', 'd', 'e', 'x', 0 },
	{ '$', 'i', 'n', 'g', 'r', 'e', 's', 0 },
	DUMMY_ATTRIBUTE_COUNT, 
	0 /* reltcpri */, 
	1, TCB_HASH,
	TCB_CATALOG | TCB_NOUPDT | TCB_CONCUR | TCB_SECURE | TCB_PROALL
	,
	DUMMY_TUPLE_COUNT, 1, 1, 1, 0, { 0, 0 },
	{ '$', 'd', 'e', 'f', 'a', 'u', 'l', 't', 0 },
	DMF_T_VERSION, 0, 0, 0, 0, 0, 0, 50, 0, 8, 0, 
	DM_PG_SIZE, 
	0, 0, 1, 65536,
        DM_TBL_DEFAULT_FHDR_PAGENO,
        DM_TBL_DEFAULT_ALLOCATION,
        DM_TBL_DEFAULT_EXTEND,
	TCB_C_NONE, TCB_PG_V1,
	0 /* relstat2 */, 
	{ ' ' } /* relfree1 */,
	1 /* relloccount */,
	0 /* relversion */, 
	DMP_INDEX_SIZE /* relwid */,
	DMP_INDEX_SIZE /* reltotwid */,
	0 /* relnparts */, 0 /* relnpartlevels */,
 	{ ' ' }
    },
};

/*
**  This table is the same as DM_core_relations, except it contains
**  upper-cased identifier fields for databases with upper-cased
**  regular identifiers.
*/
GLOBALDEF DMP_RELATION DM_ucore_relations[] =
{
    {	/*  IIRELATION	*/
    	IIRELATION_TABLE_ID,
	{ 'I', 'I', 'R', 'E', 'L', 'A', 'T', 'I', 'O', 'N', 0 },
	{ '$', 'I', 'N', 'G', 'R', 'E', 'S', 0 },
	DUMMY_ATTRIBUTE_COUNT, sizeof(DMP_RELATION), 1, TCB_HASH,
	TCB_CATALOG | TCB_NOUPDT | TCB_CONCUR | TCB_PROALL | TCB_SECURE | TCB_IDXD
	,
	DUMMY_TUPLE_COUNT, 1, 1, 1, 0, { 0, 0 },
	{ '$', 'd', 'e', 'f', 'a', 'u', 'l', 't', 0 },
	DMF_T_VERSION, 0, 0, 0, 0, 1, 0, 50, 0, 16, 0, 
	DM_PG_SIZE, 
	0, 0, 1, 65536,
        DM_TBL_DEFAULT_FHDR_PAGENO,
        DM_TBL_DEFAULT_ALLOCATION,
        DM_TBL_DEFAULT_EXTEND,
	TCB_C_NONE, TCB_PG_V1,
	0 /* relstat2 */, 
	{ ' ' } /* relfree1 */,
	1 /* relloccount */,
	0 /* relversion */, 
	sizeof(DMP_RELATION) /* relwid */,
	sizeof(DMP_RELATION) /* reltotwid */,
	0 /* relnparts */, 0 /* relnpartlevels */,
	{ ' ' }
    },
    {	/*  IIREL_IDX	*/
    	IIREL_IDX_TABLE_ID,
	{ 'I', 'I', 'R', 'E', 'L', '_', 'I', 'D', 'X', 0 },
	{ '$', 'I', 'N', 'G', 'R', 'E', 'S', 0 },
	DUMMY_ATTRIBUTE_COUNT, sizeof(DMP_RINDEX), 2, TCB_HASH,
	TCB_CATALOG | TCB_NOUPDT | TCB_CONCUR | TCB_PROALL | TCB_SECURE | TCB_INDEX,
	DUMMY_TUPLE_COUNT, 1, 1, 1, 0, { 0, 0 },
	{ '$', 'd', 'e', 'f', 'a', 'u', 'l', 't', 0 },
	DMF_T_VERSION, 0, 0, 0, 0, 0, 0, 50, 0, 8, 0, 
	DM_PG_SIZE, 
	0, 0, 1, 65536,
        DM_TBL_DEFAULT_FHDR_PAGENO,
        DM_TBL_DEFAULT_ALLOCATION,
        DM_TBL_DEFAULT_EXTEND,
	TCB_C_NONE, TCB_PG_V1,
	0 /* relstat2 */, 
	{ ' ' } /* relfree1 */,
	1 /* relloccount */,
	0 /* relversion */, 
	sizeof(DMP_RINDEX) /* relwid */,
	sizeof(DMP_RINDEX) /* reltotwid */,
	0 /* relnparts */, 0 /* relnpartlevels */,
	{ ' ' }
    },
    {	/*  IIATTRIBUTE	*/
    	IIATTRIBUTE_TABLE_ID,
	{ 'I', 'I', 'A', 'T', 'T', 'R', 'I', 'B', 'U', 'T', 'E', 0 },
	{ '$', 'I', 'N', 'G', 'R', 'E', 'S', 0 },
	DUMMY_ATTRIBUTE_COUNT, sizeof(DMP_ATTRIBUTE), 2, TCB_HASH,
	TCB_CATALOG | TCB_NOUPDT | TCB_CONCUR | TCB_SECURE | TCB_PROALL
	,
	DUMMY_TUPLE_COUNT, 1, 1, 1, 0, { 0, 0 },
	{ '$', 'd', 'e', 'f', 'a', 'u', 'l', 't', 0 },
	DMF_T_VERSION, 0, 0, 0, 0, 0, 0, 50, 0, 32, 0, 
	DM_PG_SIZE, 
	0, 0, 1, 65536,
        DM_TBL_DEFAULT_FHDR_PAGENO,
        DM_TBL_DEFAULT_ALLOCATION,
        DM_TBL_DEFAULT_EXTEND,
	TCB_C_NONE, TCB_PG_V1,
	0 /* relstat2 */, 
	{ ' ' } /* relfree1 */,
	1 /* relloccount */,
	0 /* relversion */, 
	sizeof(DMP_ATTRIBUTE) /* relwid */,
	sizeof(DMP_ATTRIBUTE) /* reltotwid */,
	0 /* relnparts */, 0 /* relnpartlevels */,
	{ ' ' }
    },
    {	/*  IIINDEX	*/
    	IIINDEX_TABLE_ID,
	{ 'I', 'I', 'I', 'N', 'D', 'E', 'X', 0 },
	{ '$', 'I', 'N', 'G', 'R', 'E', 'S', 0 },
	DUMMY_ATTRIBUTE_COUNT, DMP_INDEX_SIZE, 1, TCB_HASH,
	TCB_CATALOG | TCB_NOUPDT | TCB_CONCUR | TCB_SECURE | TCB_PROALL
	,
	DUMMY_TUPLE_COUNT, 1, 1, 1, 0, { 0, 0 },
	{ '$', 'd', 'e', 'f', 'a', 'u', 'l', 't', 0 },
	DMF_T_VERSION, 0, 0, 0, 0, 0, 0, 50, 0, 8, 0, 
	DM_PG_SIZE, 
	0, 0, 1, 65536,
        DM_TBL_DEFAULT_FHDR_PAGENO,
        DM_TBL_DEFAULT_ALLOCATION,
        DM_TBL_DEFAULT_EXTEND,
	TCB_C_NONE, TCB_PG_V1,
	0 /* relstat2 */, 
	{ ' ' } /* relfree1 */,
	1 /* relloccount */,
	0 /* relversion */, 
	DMP_INDEX_SIZE /* relwid */,
	DMP_INDEX_SIZE /* reltotwid */,
	0 /* relnparts */, 0 /* relnpartlevels */,
	{ ' ' }
    },
};

/*
**  This table contains the initialized IIATTRIBUTE records for the
**  core system tables.
*/

GLOBALDEF DMP_ATTRIBUTE DM_core_attributes[] =
{
    {
	IIRELATION_TABLE_ID,
	DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 't', 'i', 'd', 0 },
	REL_OFFSET( reltid ), 
	4, 1, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 't', 'i', 'd', 'x', 0 },
	REL_OFFSET( reltid ) + sizeof( u_i4 ), 
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'i', 'd', 0 },
	REL_OFFSET( relid ), 
	DB_MAXNAME, ATT_NOTKEY, 0, { DB_DEF_ID_BLANK, 0 }, 0,0,0,0, ATT_CHA
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'o', 'w', 'n', 'e', 'r', 0 },
	REL_OFFSET( relowner ), 
	DB_MAXNAME, ATT_NOTKEY, 0, { DB_DEF_ID_BLANK, 0 }, 0,0,0,0, ATT_CHA
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'a', 't', 't', 's', 0 },
	REL_OFFSET( relatts ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
        IIRELATION_TABLE_ID,
         DUMMY_ATTRIBUTE_NUMBER, 0,
        { 'r', 'e', 'l', 't', 'c', 'p', 'r', 'i', 0 },
        REL_OFFSET( reltcpri ),
        2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'k', 'e', 'y', 's', 0 },
	REL_OFFSET( relkeys ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 's', 'p', 'e', 'c', 0 },
	REL_OFFSET( relspec ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 's', 't', 'a', 't', 0 },
	REL_OFFSET( relstat ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 't', 'u', 'p', 's', 0 },
	REL_OFFSET( reltups ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'p', 'a', 'g', 'e', 's', 0 },
	REL_OFFSET( relpages ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'p', 'r', 'i', 'm', 0 },
	REL_OFFSET( relprim ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'm', 'a', 'i', 'n', 0 },
	REL_OFFSET( relmain ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 's', 'a', 'v', 'e', 0 },
	REL_OFFSET( relsave ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 's', 't', 'a', 'm', 'p', '1', 0 },
	REL_OFFSET( relstamp12 ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 's', 't', 'a', 'm', 'p', '2', 0 },
	REL_OFFSET( relstamp12 ) + sizeof( u_i4 ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'l', 'o', 'c', 0 },
	REL_OFFSET( relloc ),
	DB_MAXNAME, ATT_NOTKEY, 0, { DB_DEF_ID_BLANK, 0 }, 0,0,0,0, ATT_CHA
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'c', 'm', 'p', 't', 'l', 'v', 'l', 0 },
	REL_OFFSET( relcmptlvl ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_BLANK, 0 }, 0,0,0,0, ATT_CHA
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'c', 'r', 'e', 'a', 't', 'e', 0 },
	REL_OFFSET( relcreate ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'q', 'i', 'd', '1', 0 },
	REL_OFFSET( relqid1 ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'q', 'i', 'd', '2', 0 },
	REL_OFFSET( relqid2 ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'm', 'o', 'd', 'd', 'a', 't', 'e', 0 },
	REL_OFFSET( relmoddate ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'i', 'd', 'x', 'c', 'o', 'u', 'n', 't', 0 },
	REL_OFFSET( relidxcount ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'i', 'f', 'i', 'l', 'l', 0 },
	REL_OFFSET( relifill ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'd', 'f', 'i', 'l', 'l', 0 },
	REL_OFFSET( reldfill ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'l', 'f', 'i', 'l', 'l', 0 },
	REL_OFFSET( rellfill ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'm', 'i', 'n', 0 },
	REL_OFFSET( relmin ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'm', 'a', 'x', 0 },
	REL_OFFSET( relmax ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'p', 'g', 's', 'i', 'z', 'e', 0 },
	REL_OFFSET( relpgsize ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'g', 'w', 'i', 'd', 0 },
	REL_OFFSET( relgwid ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'g', 'w', 'o', 't', 'h', 'e', 'r', 0 },
	REL_OFFSET( relgwother ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'h', 'i', 'g', 'h', '_', 'l', 'o', 'g', 'k',
	  'e', 'y', 0 },
	REL_OFFSET( relhigh_logkey ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'l', 'o', 'w', '_', 'l', 'o', 'g', 'k',
	  'e', 'y', 0 },
	REL_OFFSET( rellow_logkey ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'f', 'h', 'd', 'r', 0 },
	REL_OFFSET( relfhdr ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'a', 'l', 'l', 'o', 'c', 'a', 't', 'i',
	  'o', 'n', 0 },
	REL_OFFSET( relallocation ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'e', 'x', 't', 'e', 'n', 'd', 0 },
	REL_OFFSET( relextend ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'c', 'o', 'm', 'p', 't', 'y', 'p', 'e', 0 },
	REL_OFFSET( relcomptype ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'p', 'g', 't', 'y', 'p', 'e', 0 },
	REL_OFFSET( relpgtype ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 's', 't', 'a', 't', '2', 0 },
	REL_OFFSET( relstat2 ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'f', 'r', 'e', 'e', '1', 0 },
	REL_OFFSET( relfree1 ),
	8, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_CHA
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'l', 'o', 'c', 'c', 'o', 'u', 'n', 't', 0 },
	REL_OFFSET( relloccount ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'v', 'e', 'r', 's', 'i', 'o', 'n', 0 },
	REL_OFFSET( relversion ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'w', 'i', 'd', 0 },
	REL_OFFSET( relwid ), 
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
        IIRELATION_TABLE_ID,
         DUMMY_ATTRIBUTE_NUMBER, 0,
        { 'r', 'e', 'l', 't', 'o', 't', 'w', 'i', 'd', 0 },
        REL_OFFSET( reltotwid ),
        4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
        IIRELATION_TABLE_ID,
         DUMMY_ATTRIBUTE_NUMBER, 0,
        { 'r', 'e', 'l', 'n', 'p', 'a', 'r', 't', 's',  0 },
        REL_OFFSET( relnparts ),
        2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
        IIRELATION_TABLE_ID,
         DUMMY_ATTRIBUTE_NUMBER, 0,
        { 'r', 'e', 'l', 'n', 'p', 'a', 'r', 't', 'l', 'e', 'v', 'e', 'l', 's',  0 },
        REL_OFFSET( relnpartlevels ),
        2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'f', 'r', 'e', 'e', 0 },
	REL_OFFSET( relfree ),
	8, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_CHA
    },
    {
	IIREL_IDX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'i', 'd', 0 },
	REL_IDX_OFFSET( relname ), 
	DB_MAXNAME, 1, 0, { DB_DEF_ID_BLANK, 0 }, 0,0,0,0, ATT_CHA
    },
    {
	IIREL_IDX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'r', 'e', 'l', 'o', 'w', 'n', 'e', 'r', 0 },
	REL_IDX_OFFSET( relowner ), 
	DB_MAXNAME, 2, 0, { DB_DEF_ID_BLANK, 0 }, 0,0,0,0, ATT_CHA
    },
    {
	IIREL_IDX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 't', 'i', 'd', 'p', 0 },
	REL_IDX_OFFSET( tidp ), 
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'a', 't', 't', 'r', 'e', 'l', 'i', 'd', 0 },
	ATT_OFFSET( attrelid ), 
	4, 1, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'a', 't', 't', 'r', 'e', 'l', 'i', 'd', 'x', 0 },
	ATT_OFFSET( attrelid ) + sizeof( u_i4 ), 
	4, 2, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'a', 't', 't', 'i', 'd', 0 },
	ATT_OFFSET( attid ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'a', 't', 't', 'x', 't', 'r', 'a', 0 },
	ATT_OFFSET( attxtra ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'a', 't', 't', 'n', 'a', 'm', 'e', 0 },
	ATT_OFFSET( attname ), 
	DB_MAXNAME, ATT_NOTKEY, 0, { DB_DEF_ID_BLANK, 0 }, 0,0,0,0, ATT_CHA
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'a', 't', 't', 'o', 'f', 'f', 0 },
	ATT_OFFSET( attoff ), 
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'a', 't', 't', 'f', 'r', 'm', 'l', 0 },
	ATT_OFFSET( attfml ), 
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'a', 't', 't', 'k', 'd', 'o', 'm', 0 },
	ATT_OFFSET( attkey ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'a', 't', 't', 'f', 'l', 'a', 'g', 0 },
	ATT_OFFSET( attflag ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'a', 't', 't', 'd', 'e', 'f', 'i', 'd', '1', 0 },
	ATT_OFFSET( attDefaultID ), 
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'a', 't', 't', 'd', 'e', 'f', 'i', 'd', '2', 0 },
	ATT_OFFSET( attDefaultID ) + sizeof( u_i4 ), 
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'a', 't', 't', 'i', 'n', 't', 'l', '_', 'i', 'd', 0 },
	ATT_OFFSET( attintl_id ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'a', 't', 't', 'v', 'e', 'r', '_', 'a', 'd', 'd', 'e', 'd', 0 },
	ATT_OFFSET( attver_added ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'a', 't', 't', 'v', 'e', 'r', '_', 'd', 'r', 'o', 'p', 'p', 'e', 'd', 0 },
	ATT_OFFSET( attver_dropped ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'a', 't', 't', 'v', 'a', 'l', '_', 'f', 'r', 'o', 'm', 0 },
	ATT_OFFSET( attval_from ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'a', 't', 't', 'f', 'r', 'm', 't', 0 },
	ATT_OFFSET( attfmt ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'a', 't', 't', 'f', 'r', 'm', 'p', 0 },
	ATT_OFFSET( attfmp ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'a', 't', 't', 'v', 'e', 'r', '_', 'a', 'l', 't', 'c', 'o', 'l', 0 },
	ATT_OFFSET( attver_altcol ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'a', 't', 't', 'c', 'o', 'l', 'l', 'i', 'd', 0},
	ATT_OFFSET( attcollID ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'a', 't', 't', 'f', 'r', 'e', 'e', 0 },
	ATT_OFFSET( attfree ),
	16, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_CHA
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'b', 'a', 's', 'e', 'i', 'd', 0 },
	IIINDEX_OFFSET( baseid ), 
	4, 1, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'n', 'd', 'e', 'x', 'i', 'd', 0 },
	IIINDEX_OFFSET( indexid ), 
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 's', 'e', 'q', 'u', 'e', 'n', 'c', 'e', 0 },
	IIINDEX_OFFSET( sequence ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '1', 0 },
	IIINDEX_OFFSET( idom[ 0 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '2', 0 },
	IIINDEX_OFFSET( idom[ 1 ] ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '3', 0 },
	IIINDEX_OFFSET( idom[ 2 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '4', 0 },
	IIINDEX_OFFSET( idom[ 3 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '5', 0 },
	IIINDEX_OFFSET( idom[ 4 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '6', 0 },
	IIINDEX_OFFSET( idom[ 5 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '7', 0 },
	IIINDEX_OFFSET( idom[ 6 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '8', 0 },
	IIINDEX_OFFSET( idom[ 7 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '9', 0 },
	IIINDEX_OFFSET( idom[ 8 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '1', '0', 0 },
	IIINDEX_OFFSET( idom[ 9 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '1', '1', 0 },
	IIINDEX_OFFSET( idom[ 10 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '1', '2', 0 },
	IIINDEX_OFFSET( idom[ 11 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '1', '3', 0 },
	IIINDEX_OFFSET( idom[ 12 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '1', '4', 0 },
	IIINDEX_OFFSET( idom[ 13 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '1', '5', 0 },
	IIINDEX_OFFSET( idom[ 14 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '1', '6', 0 },
	IIINDEX_OFFSET( idom[ 15 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '1', '7', 0 },
	IIINDEX_OFFSET( idom[ 16 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '1', '8', 0 },
	IIINDEX_OFFSET( idom[ 17 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '1', '9', 0 },
	IIINDEX_OFFSET( idom[ 18 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '2', '0', 0 },
	IIINDEX_OFFSET( idom[ 19 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '2', '1', 0 },
	IIINDEX_OFFSET( idom[ 20 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '2', '2', 0 },
	IIINDEX_OFFSET( idom[ 21 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '2', '3', 0 },
	IIINDEX_OFFSET( idom[ 22 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '2', '4', 0 },
	IIINDEX_OFFSET( idom[ 23 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '2', '5', 0 },
	IIINDEX_OFFSET( idom[ 24 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '2', '6', 0 },
	IIINDEX_OFFSET( idom[ 25 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '2', '7', 0 },
	IIINDEX_OFFSET( idom[ 26 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '2', '8', 0 },
	IIINDEX_OFFSET( idom[ 27 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '2', '9', 0 },
	IIINDEX_OFFSET( idom[ 28 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '3', '0', 0 },
	IIINDEX_OFFSET( idom[ 29 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '3', '1', 0 },
	IIINDEX_OFFSET( idom[ 30 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '3', '2', 0 },
	IIINDEX_OFFSET( idom[ 31 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
#ifdef NOT_UNTIL_CLUSTERED_INDEXES
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '3', '3', 0 },
	IIINDEX_OFFSET( idom[ 32 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '3', '4', 0 },
	IIINDEX_OFFSET( idom[ 33 ] ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '3', '5', 0 },
	IIINDEX_OFFSET( idom[ 34 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '3', '6', 0 },
	IIINDEX_OFFSET( idom[ 35 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '3', '7', 0 },
	IIINDEX_OFFSET( idom[ 36 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '3', '8', 0 },
	IIINDEX_OFFSET( idom[ 37 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '3', '9', 0 },
	IIINDEX_OFFSET( idom[ 38 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '4', '0', 0 },
	IIINDEX_OFFSET( idom[ 39 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '4', '1', 0 },
	IIINDEX_OFFSET( idom[ 40 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '4', '2', 0 },
	IIINDEX_OFFSET( idom[ 41 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '4', '3', 0 },
	IIINDEX_OFFSET( idom[ 42 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '4', '4', 0 },
	IIINDEX_OFFSET( idom[ 43 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '4', '5', 0 },
	IIINDEX_OFFSET( idom[ 44 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '4', '6', 0 },
	IIINDEX_OFFSET( idom[ 45 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '4', '7', 0 },
	IIINDEX_OFFSET( idom[ 46 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '4', '8', 0 },
	IIINDEX_OFFSET( idom[ 47 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '4', '9', 0 },
	IIINDEX_OFFSET( idom[ 48 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '5', '0', 0 },
	IIINDEX_OFFSET( idom[ 49 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '5', '1', 0 },
	IIINDEX_OFFSET( idom[ 50 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '5', '2', 0 },
	IIINDEX_OFFSET( idom[ 51 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '5', '3', 0 },
	IIINDEX_OFFSET( idom[ 52 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '5', '4', 0 },
	IIINDEX_OFFSET( idom[ 53 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '5', '5', 0 },
	IIINDEX_OFFSET( idom[ 54 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '5', '6', 0 },
	IIINDEX_OFFSET( idom[ 55 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '5', '7', 0 },
	IIINDEX_OFFSET( idom[ 56 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '5', '8', 0 },
	IIINDEX_OFFSET( idom[ 57 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '5', '9', 0 },
	IIINDEX_OFFSET( idom[ 58 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '6', '0', 0 },
	IIINDEX_OFFSET( idom[ 59 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '6', '1', 0 },
	IIINDEX_OFFSET( idom[ 60 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '6', '2', 0 },
	IIINDEX_OFFSET( idom[ 61 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '6', '3', 0 },
	IIINDEX_OFFSET( idom[ 62 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'i', 'd', 'o', 'm', '6', '4', 0 },
	IIINDEX_OFFSET( idom[ 63 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
#endif  /* NOT_UNTIL_CLUSTERED_INDEXES */
};

/*
**  This table is the same as DM_core_attributes, except it contains
**  upper-cased identifier fields for databases with upper-cased
**  regular identifiers.
*/
GLOBALDEF DMP_ATTRIBUTE DM_ucore_attributes[] =
{
    {
	IIRELATION_TABLE_ID,
	DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'T', 'I', 'D', 0 },
	REL_OFFSET( reltid ), 
	4, 1, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'T', 'I', 'D', 'X', 0 },
	REL_OFFSET( reltid ) + sizeof( u_i4 ), 
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'I', 'D', 0 },
	REL_OFFSET( relid ), 
	DB_MAXNAME, ATT_NOTKEY, 0, { DB_DEF_ID_BLANK, 0 }, 0,0,0,0, ATT_CHA
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'O', 'W', 'N', 'E', 'R', 0 },
	REL_OFFSET( relowner ), 
	DB_MAXNAME, ATT_NOTKEY, 0, { DB_DEF_ID_BLANK, 0 }, 0,0,0,0, ATT_CHA
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'A', 'T', 'T', 'S', 0 },
	REL_OFFSET( relatts ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
        IIRELATION_TABLE_ID,
         DUMMY_ATTRIBUTE_NUMBER, 0,
        { 'R', 'E', 'L', 'T', 'C', 'P', 'R', 'I', 0 },
        REL_OFFSET( reltcpri ),
        2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'K', 'E', 'Y', 'S', 0 },
	REL_OFFSET( relkeys ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'S', 'P', 'E', 'C', 0 },
	REL_OFFSET( relspec ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'S', 'T', 'A', 'T', 0 },
	REL_OFFSET( relstat ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'T', 'U', 'P', 'S', 0 },
	REL_OFFSET( reltups ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'P', 'A', 'G', 'E', 'S', 0 },
	REL_OFFSET( relpages ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'P', 'R', 'I', 'M', 0 },
	REL_OFFSET( relprim ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'M', 'A', 'I', 'N', 0 },
	REL_OFFSET( relmain ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'S', 'A', 'V', 'E', 0 },
	REL_OFFSET( relsave ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'S', 'T', 'A', 'M', 'P', '1', 0 },
	REL_OFFSET( relstamp12 ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'S', 'T', 'A', 'M', 'P', '2', 0 },
	REL_OFFSET( relstamp12 ) + sizeof( u_i4 ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'L', 'O', 'C', 0 },
	REL_OFFSET( relloc ),
	DB_MAXNAME, ATT_NOTKEY, 0, { DB_DEF_ID_BLANK, 0 }, 0,0,0,0, ATT_CHA
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'C', 'M', 'P', 'T', 'L', 'V', 'L', 0 },
	REL_OFFSET( relcmptlvl ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_BLANK, 0 }, 0,0,0,0, ATT_CHA
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'C', 'R', 'E', 'A', 'T', 'E', 0 },
	REL_OFFSET( relcreate ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'Q', 'I', 'D', '1', 0 },
	REL_OFFSET( relqid1 ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'Q', 'I', 'D', '2', 0 },
	REL_OFFSET( relqid2 ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'M', 'O', 'D', 'D', 'A', 'T', 'E', 0 },
	REL_OFFSET( relmoddate ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'I', 'D', 'X', 'C', 'O', 'U', 'N', 'T', 0 },
	REL_OFFSET( relidxcount ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'I', 'F', 'I', 'L', 'L', 0 },
	REL_OFFSET( relifill ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'D', 'F', 'I', 'L', 'L', 0 },
	REL_OFFSET( reldfill ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'L', 'F', 'I', 'L', 'L', 0 },
	REL_OFFSET( rellfill ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'M', 'I', 'N', 0 },
	REL_OFFSET( relmin ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'M', 'A', 'X', 0 },
	REL_OFFSET( relmax ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'P', 'G', 'S', 'I', 'Z', 'E', 0 },
	REL_OFFSET( relpgsize ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'G', 'W', 'I', 'D', 0 },
	REL_OFFSET( relgwid ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'G', 'W', 'O', 'T', 'H', 'E', 'R', 0 },
	REL_OFFSET( relgwother ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'H', 'I', 'G', 'H', '_', 'L', 'O', 'G', 'K',
	  'E', 'Y', 0 },
	REL_OFFSET( relhigh_logkey ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'L', 'O', 'W', '_', 'L', 'O', 'G', 'K',
	  'E', 'Y', 0 },
	REL_OFFSET( rellow_logkey ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'F', 'H', 'D', 'R', 0 },
	REL_OFFSET( relfhdr ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'A', 'L', 'L', 'O', 'C', 'A', 'T', 'I',
	  'O', 'N', 0 },
	REL_OFFSET( relallocation ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'E', 'X', 'T', 'E', 'N', 'D', 0 },
	REL_OFFSET( relextend ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'C', 'O', 'M', 'P', 'T', 'Y', 'P', 'E', 0 },
	REL_OFFSET( relcomptype ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'P', 'G', 'T', 'Y', 'P', 'E', 0 },
	REL_OFFSET( relpgtype ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'S', 'T', 'A', 'T', '2', 0 },
	REL_OFFSET( relstat2 ),
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'F', 'R', 'E', 'E', '1', 0 },
	REL_OFFSET( relfree1 ),
	8, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_CHA
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'L', 'O', 'C', 'C', 'O', 'U', 'N', 'T', 0 },
	REL_OFFSET( relloccount ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'V', 'E', 'R', 'S', 'I', 'O', 'N', 0 },
	REL_OFFSET( relversion ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'W', 'I', 'D', 0 },
	REL_OFFSET( relwid ), 
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
        IIRELATION_TABLE_ID,
         DUMMY_ATTRIBUTE_NUMBER, 0,
        { 'R', 'E', 'L', 'T', 'O', 'T', 'W', 'I', 'D', 0 },
        REL_OFFSET( reltotwid ),
        4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
        IIRELATION_TABLE_ID,
         DUMMY_ATTRIBUTE_NUMBER, 0,
        { 'R', 'E', 'L', 'N', 'P', 'A', 'R', 'T', 'S',  0 },
        REL_OFFSET( relnparts ),
        2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
        IIRELATION_TABLE_ID,
         DUMMY_ATTRIBUTE_NUMBER, 0,
        { 'R', 'E', 'L', 'N', 'P', 'A', 'R', 'T', 'L', 'E', 'V', 'E', 'L', 'S',  0 },
        REL_OFFSET( relnpartlevels ),
        2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIRELATION_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'F', 'R', 'E', 'E', 0 },
	REL_OFFSET( relfree ),
	8, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_CHA
    },
    {
	IIREL_IDX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'I', 'D', 0 },
	REL_IDX_OFFSET( relname ), 
	DB_MAXNAME, 1, 0, { DB_DEF_ID_BLANK, 0 }, 0,0,0,0, ATT_CHA
    },
    {
	IIREL_IDX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'R', 'E', 'L', 'O', 'W', 'N', 'E', 'R', 0 },
	REL_IDX_OFFSET( relowner ), 
	DB_MAXNAME, 2, 0, { DB_DEF_ID_BLANK, 0 }, 0,0,0,0, ATT_CHA
    },
    {
	IIREL_IDX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'T', 'I', 'D', 'P', 0 },
	REL_IDX_OFFSET( tidp ), 
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'A', 'T', 'T', 'R', 'E', 'L', 'I', 'D', 0 },
	ATT_OFFSET( attrelid ), 
	4, 1, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'A', 'T', 'T', 'R', 'E', 'L', 'I', 'D', 'X', 0 },
	ATT_OFFSET( attrelid ) + sizeof( u_i4 ), 
	4, 2, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'A', 'T', 'T', 'I', 'D', 0 },
	ATT_OFFSET( attid ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'A', 'T', 'T', 'X', 'T', 'R', 'A', 0 },
	ATT_OFFSET( attxtra ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'A', 'T', 'T', 'N', 'A', 'M', 'E', 0 },
	ATT_OFFSET( attname ), 
	DB_MAXNAME, ATT_NOTKEY, 0, { DB_DEF_ID_BLANK, 0 }, 0,0,0,0, ATT_CHA
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'A', 'T', 'T', 'O', 'F', 'F', 0 },
	ATT_OFFSET( attoff ), 
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'A', 'T', 'T', 'F', 'R', 'M', 'L', 0 },
	ATT_OFFSET( attfml ), 
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'A', 'T', 'T', 'K', 'D', 'O', 'M', 0 },
	ATT_OFFSET( attkey ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'A', 'T', 'T', 'F', 'L', 'A', 'G', 0 },
	ATT_OFFSET( attflag ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'A', 'T', 'T', 'D', 'E', 'F', 'I', 'D', '1', 0 },
	ATT_OFFSET( attDefaultID ), 
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'A', 'T', 'T', 'D', 'E', 'F', 'I', 'D', '2', 0 },
	ATT_OFFSET( attDefaultID ) + sizeof( u_i4 ), 
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'A', 'T', 'T', 'I', 'N', 'T', 'L', '_', 'I', 'D', 0 },
	ATT_OFFSET( attintl_id ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'A', 'T', 'T', 'V', 'E', 'R', '_', 'A', 'D', 'D', 'E', 'D', 0 },
	ATT_OFFSET( attver_added ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'A', 'T', 'T', 'V', 'E', 'R', '_', 'D', 'R', 'O', 'P', 'P', 'E', 'D', 0 },
	ATT_OFFSET( attver_dropped ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'A', 'T', 'T', 'V', 'A', 'L', '_', 'F', 'R', 'O', 'M', 0 },
	ATT_OFFSET( attval_from ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'A', 'T', 'T', 'F', 'R', 'M', 'T', 0 },
	ATT_OFFSET( attfmt ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'A', 'T', 'T', 'F', 'R', 'M', 'P', 0 },
	ATT_OFFSET( attfmp ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'A', 'T', 'T', 'V', 'E', 'R', '_', 'A', 'L', 'T', 'C', 'O', 'L', 0 },
	ATT_OFFSET( attver_altcol ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'A', 'T', 'T', 'C', 'O', 'L', 'L', 'I', 'D', 0},
	ATT_OFFSET( attcollID ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIATTRIBUTE_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'A', 'T', 'T', 'F', 'R', 'E', 'E', 0 },
	ATT_OFFSET( attfree ),
	16, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_CHA
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'B', 'A', 'S', 'E', 'I', 'D', 0 },
	IIINDEX_OFFSET( baseid ), 
	4, 1, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'N', 'D', 'E', 'X', 'I', 'D', 0 },
	IIINDEX_OFFSET( indexid ), 
	4, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'S', 'E', 'Q', 'U', 'E', 'N', 'C', 'E', 0 },
	IIINDEX_OFFSET( sequence ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '1', 0 },
	IIINDEX_OFFSET( idom[ 0 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '2', 0 },
	IIINDEX_OFFSET( idom[ 1 ] ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '3', 0 },
	IIINDEX_OFFSET( idom[ 2 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '4', 0 },
	IIINDEX_OFFSET( idom[ 3 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '5', 0 },
	IIINDEX_OFFSET( idom[ 4 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '6', 0 },
	IIINDEX_OFFSET( idom[ 5 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '7', 0 },
	IIINDEX_OFFSET( idom[ 6 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '8', 0 },
	IIINDEX_OFFSET( idom[ 7 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '9', 0 },
	IIINDEX_OFFSET( idom[ 8 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '1', '0', 0 },
	IIINDEX_OFFSET( idom[ 9 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '1', '1', 0 },
	IIINDEX_OFFSET( idom[ 10 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '1', '2', 0 },
	IIINDEX_OFFSET( idom[ 11 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '1', '3', 0 },
	IIINDEX_OFFSET( idom[ 12 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '1', '4', 0 },
	IIINDEX_OFFSET( idom[ 13 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '1', '5', 0 },
	IIINDEX_OFFSET( idom[ 14 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '1', '6', 0 },
	IIINDEX_OFFSET( idom[ 15 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '1', '7', 0 },
	IIINDEX_OFFSET( idom[ 16 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '1', '8', 0 },
	IIINDEX_OFFSET( idom[ 17 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '1', '9', 0 },
	IIINDEX_OFFSET( idom[ 18 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '2', '0', 0 },
	IIINDEX_OFFSET( idom[ 19 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '2', '1', 0 },
	IIINDEX_OFFSET( idom[ 20 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '2', '2', 0 },
	IIINDEX_OFFSET( idom[ 21 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '2', '3', 0 },
	IIINDEX_OFFSET( idom[ 22 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '2', '4', 0 },
	IIINDEX_OFFSET( idom[ 23 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '2', '5', 0 },
	IIINDEX_OFFSET( idom[ 24 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '2', '6', 0 },
	IIINDEX_OFFSET( idom[ 25 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '2', '7', 0 },
	IIINDEX_OFFSET( idom[ 26 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '2', '8', 0 },
	IIINDEX_OFFSET( idom[ 27 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '2', '9', 0 },
	IIINDEX_OFFSET( idom[ 28 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '3', '0', 0 },
	IIINDEX_OFFSET( idom[ 29 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '3', '1', 0 },
	IIINDEX_OFFSET( idom[ 30 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '3', '2', 0 },
	IIINDEX_OFFSET( idom[ 31 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
#ifdef NOT_UNTIL_CLUSTERED_INDEXES
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '3', '3', 0 },
	IIINDEX_OFFSET( idom[ 32 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '3', '4', 0 },
	IIINDEX_OFFSET( idom[ 33 ] ),
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '3', '5', 0 },
	IIINDEX_OFFSET( idom[ 34 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '3', '6', 0 },
	IIINDEX_OFFSET( idom[ 35 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '3', '7', 0 },
	IIINDEX_OFFSET( idom[ 36 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '3', '8', 0 },
	IIINDEX_OFFSET( idom[ 37 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '3', '9', 0 },
	IIINDEX_OFFSET( idom[ 38 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '4', '0', 0 },
	IIINDEX_OFFSET( idom[ 39 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '4', '1', 0 },
	IIINDEX_OFFSET( idom[ 40 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '4', '2', 0 },
	IIINDEX_OFFSET( idom[ 41 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '4', '3', 0 },
	IIINDEX_OFFSET( idom[ 42 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '4', '4', 0 },
	IIINDEX_OFFSET( idom[ 43 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '4', '5', 0 },
	IIINDEX_OFFSET( idom[ 44 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '4', '6', 0 },
	IIINDEX_OFFSET( idom[ 45 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '4', '7', 0 },
	IIINDEX_OFFSET( idom[ 46 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '4', '8', 0 },
	IIINDEX_OFFSET( idom[ 47 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '4', '9', 0 },
	IIINDEX_OFFSET( idom[ 48 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '5', '0', 0 },
	IIINDEX_OFFSET( idom[ 49 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '5', '1', 0 },
	IIINDEX_OFFSET( idom[ 50 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '5', '2', 0 },
	IIINDEX_OFFSET( idom[ 51 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '5', '3', 0 },
	IIINDEX_OFFSET( idom[ 52 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '5', '4', 0 },
	IIINDEX_OFFSET( idom[ 53 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '5', '5', 0 },
	IIINDEX_OFFSET( idom[ 54 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '5', '6', 0 },
	IIINDEX_OFFSET( idom[ 55 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '5', '7', 0 },
	IIINDEX_OFFSET( idom[ 56 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '5', '8', 0 },
	IIINDEX_OFFSET( idom[ 57 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '5', '9', 0 },
	IIINDEX_OFFSET( idom[ 58 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '6', '0', 0 },
	IIINDEX_OFFSET( idom[ 59 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '6', '1', 0 },
	IIINDEX_OFFSET( idom[ 60 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '6', '2', 0 },
	IIINDEX_OFFSET( idom[ 61 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '6', '3', 0 },
	IIINDEX_OFFSET( idom[ 62 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
    {
	IIINDEX_TABLE_ID,
	 DUMMY_ATTRIBUTE_NUMBER, 0,
	{ 'I', 'D', 'O', 'M', '6', '4', 0 },
	IIINDEX_OFFSET( idom[ 63 ] ), 
	2, ATT_NOTKEY, 0, { DB_DEF_ID_0, 0 }, 0,0,0,0, ATT_INT
    },
#endif /* NOT_UNTIL_CLUSTERED_INDEXES */
};

/*
**  This table contains the initialized Relation index records for the
**  core system tables.
*/

GLOBALDEF DMP_RINDEX core_rindex[] =
{
    {
        {{ 'i', 'i', 'r', 'e', 'l', 'a', 't', 'i', 'o', 'n', 0 }},
        {{ '$', 'i', 'n', 'g', 'r', 'e', 's', 0 }},
        0
    },
    {
        {{ 'i', 'i', 'r', 'e', 'l', '_', 'i', 'd', 'x', 0 }},
        {{ '$', 'i', 'n', 'g', 'r', 'e', 's', 0 }},
        1
    },
    {
        {{ 'i', 'i', 'a', 't', 't', 'r', 'i', 'b', 'u', 't', 'e', 0 }},
        {{ '$', 'i', 'n', 'g', 'r', 'e', 's', 0 }},
        2
    },
    {
        {{ 'i', 'i', 'i', 'n', 'd', 'e', 'x', 0 }},
        {{ '$', 'i', 'n', 'g', 'r', 'e', 's', 0 }},
        3
    }
};

/*
**  This table is the same as core_rindex, except it contains
**  upper-cased identifier fields for databases with upper-cased
**  regular identifiers.
*/
GLOBALDEF DMP_RINDEX ucore_rindex[] =
{
    {
        {{ 'I', 'I', 'R', 'E', 'L', 'A', 'T', 'I', 'O', 'N', 0 }},
        {{ '$', 'I', 'N', 'G', 'R', 'E', 'S', 0 }},
        0
    },
    {
        {{ 'I', 'I', 'R', 'E', 'L', '_', 'I', 'D', 'X', 0 }},
        {{ '$', 'I', 'N', 'G', 'R', 'E', 'S', 0 }},
        1
    },
    {
        {{ 'I', 'I', 'A', 'T', 'T', 'R', 'I', 'B', 'U', 'T', 'E', 0 }},
        {{ '$', 'I', 'N', 'G', 'R', 'E', 'S', 0 }},
        2
    },
    {
        {{ 'I', 'I', 'I', 'N', 'D', 'E', 'X', 0 }},
        {{ '$', 'I', 'N', 'G', 'R', 'E', 'S', 0 }},
        3
    }
};

/*
** Number of elements in DM_core_relations
*/
GLOBALDEF i4 DM_sizeof_core_rel = sizeof(DM_core_relations)/
                                        sizeof(DMP_RELATION);

/*
** Number of elements in DM_core_attributes
** 27-aug-1996 (wilan06)
**      added
*/
GLOBALDEF i4 DM_sizeof_core_attr = sizeof(DM_core_attributes)/
                                   sizeof(DMP_ATTRIBUTE);


/*
** Number of elements in core_rindex structure
** 27-aug-1996 (wilan06)
**      added
*/
GLOBALDEF i4 DM_sizeof_core_rindex = sizeof(core_rindex)/sizeof(DMP_RINDEX);

/*
**  This table contains the initialized IIINDEX records for the
**  core system tables.
*/


GLOBALDEF DMP_INDEX core_index[] =
{
    {
        1, 2, 0,
        {
            3, 4
        }
    }
};


/*
** Number of elements in core_index array
** 27-aug-1996 (wilan06)
**      added
*/
GLOBALDEF i4 DM_sizeof_core_index = sizeof(core_index)/
                                    DMP_INDEX_SIZE;


