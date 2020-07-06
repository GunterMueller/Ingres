# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**      ADDJOIN.QSC - This routine adds the join field range variables
**                  and column names to the to the structure containing 
**                  the join information.
**
**
**      written 7/22/83 (nml)
**              13-jul-87 (bab) Changed memory allocation to use [FM]Ereqmem.
**		07-dec-87 (kenl)
**			Turned module into a .QSC file in order to incorporate
**			the new header file: mqbf.qsh
**
**      Copyright (c) 2004 Ingres Corporation
*/
# include       <compat.h>
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include       <fe.h>
# include "mqbf.h"
# include	<me.h>
# include	<er.h>
# include 	"erqf.h"
mqaddjoin(qdef, k, j)
MQQDEF  *qdef;
i4      k;
i4      j;
{
        i4                      n;
        register ATTRIBINFO     *attk = qdef->mqatts[k];
        register ATTRIBINFO     *attj = qdef->mqatts[j];
        register JOININFO       *join;
        n = qdef->mqnumjoins;
        if (n >= MQ_MAXJOINS)
            return(-6);
        /* allocate space for join info */
        if ((qdef->mqjoins[n] = (JOININFO *)MEreqmem((u_i4)0,
            (u_i4)sizeof(JOININFO), TRUE, (STATUS *)NULL)) == NULL)
        {
            FDerror(ERRALLOC, 0, ERx(""));
            return(-7);
        }
        else
        {
            join = qdef->mqjoins[n];
            STcopy(attk->rvar, join->rv1); 
            STcopy(attk->col, join->col1); 
            STcopy(attj->rvar, join->rv2); 
            STcopy(attj->col, join->col2); 
            return(OK);
        }
}
