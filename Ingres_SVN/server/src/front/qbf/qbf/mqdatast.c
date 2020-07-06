# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include "mqbf.h"
# include	<me.h>
# include	<er.h>
# include	"erqf.h"
/*
**	MQDATAST.QSC -
**
**	Mqdatast() allocates and initializes qdefinfo struct
**	for transfering global data between qdef and qexec.
**
**	written - 1/10/85 (kira)
**	13-jul-87 (bab)	Changed memory allocation to use [FM]Ereqmem.
**	07-dec-87 (kenl)
**			Turned module into a .QSC file in order to incorporate
**			the new header file: mqbf.qsh
**/
QDEFINFO *
mqdatast(tblwidth)
i4	tblwidth;
{
	QDEFINFO	*temp_data;
	if ((temp_data = (QDEFINFO *)MEreqmem((u_i4)0,
	    (u_i4)sizeof(struct qdefinfo), TRUE, (STATUS *)NULL)) == NULL)
	{
		FDerror(ERRALLOC, 0, ERx(""));
		return(NULL);
	}
	temp_data->d_tblfield = tblfield;
	temp_data->d_qdef_type = qdef_type;
	temp_data->d_mq_func = mq_func;
	temp_data->d_mq_debug = mq_debug;
	temp_data->d_mq_deffrm = mq_deffrm;
	temp_data->d_tblwidth = tblwidth;
	STcopy( mq_frame, temp_data->d_mq_frame );
	STcopy( mq_tbl, temp_data->d_mq_tbl );
	STcopy( mqflnm, temp_data->d_mqflnm );
	temp_data->d_onetbl = onetbl;
	temp_data->d_any_views = any_views;
	temp_data->d_any_cats = any_cats;
	temp_data->d_mqnummasters = mqnummasters;
	temp_data->d_mqnumdetails = mqnumdetails;
	temp_data->d_mq_name = mq_name;
	return(temp_data);
}
