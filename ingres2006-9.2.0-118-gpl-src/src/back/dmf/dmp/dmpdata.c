/*
**Copyright (c) 2004 Ingres Corporation
*/

/*
**
LIBRARY = IMPDMFLIBDATA
**
*/

#include    <compat.h>
#include    <gl.h>
#include    <me.h>
#include    <pc.h>
#include    <tr.h>
#include    <st.h>
#include    <cs.h>
#include    <sl.h>
#include    <iicommon.h>
#include    <dbdbms.h>
#include    <adf.h>
#include    <ulf.h>
#include    <dmf.h>
#include    <lg.h>
#include    <lk.h>
#include    <dm.h>
#include    <dmp.h>
#include    <dmpl.h>
#include    <dmpp.h>
#include    <dm0p.h>
#include    <dm1c.h>
#include    <dm1ch.h>
#include    <dm1cn.h>
#include    <dm1cnv2.h>
#include    <dm1u.h>
#include    <dm1b.h>
#include    <dmtcb.h>
#include    <dm2t.h>
#include    <dmftrace.h>

/*
** Name:	dmpdata.c
**
** Description:	Global data for dmp facility.
**
** History:
**
**	23-sep-96 (canor01)
**	    Created.
**      05-oct-98 (stial01) 
**          Update dmpp_dput parameters, Added dmpp_put_hdr
**	01-dec-1998 (nanpr01)
**	    Get rid of DMPP_TUPLE_INFO structure. Use DMPP_TUPLE_HDR instead.
**      09-feb-1999 (stial01)
**          Added relcmptlvl to dm1c*_ixsearch prototypes
**      17-may-1999 (nanpr01)
**          Corrected the prototype.
**      21-dec-1999 (stial01)
**          Added dm1cnv3*
**      15-may-2000 (stial01)
**          Remove Smart Disk code
**      25-may-2000 (stial01)
**         Deleted System Catalog Accessors (we will use normal page accessors)
**      23-oct-2000 (stial01)
**          Variable Page Size - Variable Page Type support (SIR 102955)
**      30-oct-2000 (stial01)
**          Change args to dmpp_allocate, dmpp_isnew accessors (add rcb).
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      01-feb-2001 (stial01)
**          Deleted dm1cnv3* accessors (Variable Page Type SIR 102955)
**	06-nov-2001 (kinte01)
**	    Add a globalconstdef of dmpp_pagetype
**	06-jun-2002 (devjo01)
**	    Back out above change.
**	29-Sep-2004 (drivi01)
**	    Added LIBRARY jam hint to put this file into IMPDMFLIBDATA
**	    in the Jamfile.
*/

GLOBALDEF DMPP_ACC_TLV dm1ch_tlv =
{
        /* "Normal" page format, high compression */
        dm1ch_compress, dm1ch_uncompress,
        dm1ch_compexpand, dm1cn_compress,
        dm1cn_uncompress, dm1cn_compexpand
};


/* dm1ccmpn.c */
/*{
** Name: dm1cn_tlv - Normal format tuple level vector of accessors
**  
** Description:
**      This the the accessor structure that dm1c_getaccessors will use when
**      passed parameters asking for standard accessors without the
**      special forward compatibility compresion scheme.
**       
** History:
**      03_jun_92 (kwatts)
**          Created.
*/
  
  
GLOBALDEF DMPP_ACC_TLV dm1cn_tlv =
{
    /* "Normal" page format */
    dm1cn_compress,     dm1cn_uncompress,
    dm1cn_compexpand,   dm1cn_compress,  
    dm1cn_uncompress,   dm1cn_compexpand 
};


/* dm1cn.c */
/*{
** Name: dm1cn_plv - Normal format page level vector of accessors
**               
** Description:
**      This the the accessor structure that dm1c_getaccessors will use when
**      passed parameters asking for standard page accessors.
**       
** History:
**      03_jun_92 (kwatts)
**          Created.
**      16-oct-1992 (jnash)
**          Reduced logging project.  Eliminate dm1cn_isempty.
**      03-nov-1992 (jnash)
**          Reduced logging project.  Change params to dm1cn_reclen,
**          modify dm1cn_reallocate.
*/
 
GLOBALDEF DMPP_ACC_PLV dm1cn_plv =
{
    /* "Normal" page format */
    dm1cn_allocate,
    dm1cn_isempty,
    dm1cn_check_page,
    dm1cn_delete,
    dm1cn_format,
    dm1cn_get,
    dm1cn_getfree,
    dm1cn_get_offset,
    dm1cn_isnew,
    dm1cn_ixsearch,
    dm1cn_load,
    dm1cn_put,
    dm1cn_reallocate,
    dm1cn_reclen,
    dm1cn_tuplecount,
    dm1cn_dput
};


/* dm1cnv2.c */
/*{
** Name: dm1cnv2_plv - Normal format page level vector of accessors    
**           
** Description:
**      This the the accessor structure that dm1c_getaccessors will use when
**      passed parameters asking for standard page accessors.
**       
** History:
**      06-may-1996 (thaju02)
**          Created.
*/
   
GLOBALDEF DMPP_ACC_PLV dm1cnv2_plv =
{ 
    /* "Normal" page format */
    dm1cnv2_allocate,
    dm1cnv2_isempty,
    dm1cnv2_check_page,
    dm1cnv2_delete,
    dm1cnv2_format,
    dm1cnv2_get,
    dm1cnv2_getfree,
    dm1cnv2_get_offset,
    dm1cnv2_isnew,
    dm1cnv2_ixsearch,
    dm1cnv2_load,
    dm1cnv2_put,
    dm1cnv2_reallocate,
    dm1cnv2_reclen,
    dm1cnv2_tuplecount,
    dm1cnv2_dput
};

