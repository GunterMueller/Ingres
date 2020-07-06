/*
**Copyright (c) 2004 Ingres Corporation
*/

/**
** Name: DM1H.H - Hash Access Method interface definitions.
**
** Description:
**      This file describes the interface to the Hash Access Method
**	services.
**
** History:
**	 7-jul-1992 (rogerk)
**	    Created for DMF Prototyping.
**	22-oct-92 (rickh)
**	    Replaced all references to DMP_ATTS with DB_ATTS.
**	13-sep-1996 (canor01)
**	    Pass tuple buffer to dm1h_hash and dm1h_newhash.
**      22-nov-96 (stial01,dilma04)
**          Row Locking Project:
**          Added prototype for dm1h_keyhash
**      10-mar-97 (stial01)
**          Pass tuple buffer to dm1h_keyhash
**      7-jan-98 (stial01)
**          Removed buffer parameter from dm1h_hash,dm1h_newhash,dm1h_keyhash
**      30-oct-2000 (stial01)
**          Changed prototype for dm1h_allocate
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      31-Jan-2002 (hweho01)
**          Added prototype of dm1h_hash_uchar().
**/

FUNC_EXTERN DB_STATUS	dm1h_allocate(
				    DMP_RCB        *rcb,
				    DMPP_PAGE      **data,
				    DM_TID         *tid,
				    char           *record,
				    char	   *crecord,
				    i4        need,
				    i4        dupflag,
				    i4        *err_code);

FUNC_EXTERN DB_STATUS	dm1h_search(
				    DMP_RCB         *rcb,
				    DMPP_PAGE       **data,
				    char            *tuple,
				    i4         mode,
				    i4         direction,
				    DM_TID          *tid,
				    i4         *err_code);

FUNC_EXTERN i4	dm1h_hash(
				    DB_ATTS        **att,
				    i4         count,
				    char            *tuple,
				    i4         prime);

FUNC_EXTERN i4	dm1h_newhash(
				    DB_ATTS        **att,
				    i4         count,
				    char            *tuple,
				    i4         buckets);

FUNC_EXTERN VOID	dm1h_nofull(DMP_RCB *rcb, DM_PAGENO bucket);

FUNC_EXTERN i4     dm1h_keyhash(
				    DB_ATTS        **att,
				    i4         count,
				    char            *tuple,
				    i4              *key4,
				    i4              *key5,
				    i4              *key6);
FUNC_EXTERN i4	dm1h_hash_uchar(
				    DB_ATTS        **att,
				    i4         count,
				    char            *tuple,
				    i4         prime);
