/*
**Copyright (c) 2004 Ingres Corporation
*/

/**
** Name: DM1I.H - Isam Access Method interface definitions.
**
** Description:
**      This file describes the interface to the Isam Access Method
**	services.
**
** History:
**	 7-jul-1992 (rogerk)
**	    Created for DMF Prototyping.
**      30-oct-2000 (stial01)
**          Changed prototype for dm1i_allocate
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/

FUNC_EXTERN DB_STATUS	dm1i_allocate(
				DMP_RCB        *rcb,
				DMPP_PAGE      **data,
				DM_TID         *tid,
				char           *record,
				char	       *crecord,
				i4        need,
				i4        dupflag,
				i4        *err_code);

FUNC_EXTERN DB_STATUS	dm1i_search(
				DMP_RCB         *rcb,
				DMPP_PAGE       **data,
				char            *key,
				i4         keys_given,
				i4         mode,
				i4         direction,
				DM_TID          *tid,
				i4         *err_code);
