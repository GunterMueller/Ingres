/*
**Copyright (c) 2004 Ingres Corporation
*/

#include        <compat.h>
#include	<si.h>
#include        <gl.h>
#include        <sl.h>
#include        <iicommon.h>
#include        <dbdbms.h>
#include        <lo.h>
#include        <er.h>
#include        <pc.h>
#include        <duerr.h>
#include        <duvfiles.h>
#include        <dudbms.h>
#include        <duve.h>
#include        <duustrings.h>
#include        <cs.h>
#include        <dm.h>

/*
** Name:        duvedata.c
**
** Description: Global data for duve facility.
**
** History:
**
**      28-sep-96 (mcgem01)
**          Created.
**	19-dec-96 (chech02)
**	    Added #include si.h.
**	19-jan-2005 (abbjo03)
**	    Remove dmf_svcb, Duv_statf, Duv_dbf and Duv_conv_status.
*/

/* duverify.c */

GLOBALDEF DUVE_CB       duvecb; /* verifydb control block */

