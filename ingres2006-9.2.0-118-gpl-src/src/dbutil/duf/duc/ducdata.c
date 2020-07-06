/*
**Copyright (c) 2004 Ingres Corporation
*/

# include	<compat.h>
# include	<pc.h>
# include	<gl.h>
# include	<sl.h>
# include	<cs.h>
# include	<er.h>
# include	<iicommon.h>
# include	<dbdbms.h>
# include	<duf.h>
# include	<lk.h>
# include	<usererror.h>
# include	<duerr.h>
# include	<duenv.h>
# include	<dustar.h>

/*
** Name:        ducdata.c
**
** Description: Global data for duc facility.
**
** History:
**
**      28-sep-96 (mcgem01)
**          Created.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/

/* ducommon.qsc */

GLOBALDEF       DU_ERROR        *Duc_errcb ZERO_FILL;
GLOBALDEF       bool            upg_dbg_out = FALSE;

/* ducreate.qsc */

GLOBALDEF DU_ENV       *Duc_dbenv ZERO_FILL;
GLOBALDEF i4            Duc_deadlock    ZERO_FILL;
GLOBALDEF i4            Duc_dlock_cnt   ZERO_FILL;
GLOBALDEF i4            Duc_1dlock_retry ZERO_FILL;
GLOBALDEF i4            Duc_iidbdb_read ZERO_FILL;
GLOBALDEF i4            Duc_destroydb_msg ZERO_FILL;
GLOBALDEF i4            Duc_distributed_flag ZERO_FILL;
GLOBALDEF DU_STARCB     Duc_star_cb;


