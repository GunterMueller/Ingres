/*
** Copyright (c) 2004 Ingres Corporation
*/

# include       <compat.h>
# include       <gl.h>
# include       <sl.h>
# include       <iicommon.h>
# include       <fe.h>
# include       <si.h>
# include       <qr.h>
# include       "qrhelp.h"

/*
** Name:        qrdata.c
**
** Description: Global data for qr facility.
**
** History:
**
**      26-sep-96 (mcgem01)
**          Created.
**	16-dec-1996 (donc)
**	    Added connect_chgd
*/

/* qrhelp.qsc */

GLOBALDEF bool  synonym_yes = FALSE;


/* qrinit.c */

GLOBALDEF QRSD  noStmtDesc = { NULL,    NOstmt, NULL};

/* qrmklist.c */

GLOBALDEF IIQRlist      *IIQRnml_NameList = NULL;

/* qrrun.c */

GLOBALDEF QRB   *QRqrb;

/* qrsql.c */

GLOBALDEF bool  Runtime_INGRES;

/* go.c */
GLOBALDEF bool connect_chgd = FALSE;
