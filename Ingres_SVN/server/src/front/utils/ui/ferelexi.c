# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<st.h>
# include	<iicommon.h>
# include	<generr.h>
# include	<fe.h>		/* unused! */
# include <ui.h>
# include <uigdata.h>
# include	<erlq.h>	/* for LIBQ error define */
/*{
** Name:    FErelexists() -	Check for Relation (Table) Existence.
**
** Description:
**  Fast way to check if a table exists.
**  We suppress printing of error message if error is
**  that table does not exists, otherwise we return
**  non-zero to print error message.
**
** Inputs:
**	fetable		Name of the relation to be searched for.  The
**			string is expected to be normalized.
**
**	feowner		Name of the owner of the relation to further qualify
**			query.  This is also expected to be in normalized form.
**			However, it may also be specified as an empty string
**			for pre-6.5 compatibility or when the true owner is
**			unknown or unimportant.  Unused when connected to a
**			pre-6.5 database!
** Outputs:
**	None.
** Returns:
**	OK -		relation found and is accessable.
**	other -		relation not found, not accessable, name formed
**			badly, etc.
**		
**
**  5/12/86 - Now, also suppress printing of error message
**            if 'name too long' or 'invalid operator',
**            both common errors for users entering names (bab)
**
**  iiseterr() now returns pointer to previous
**  handling routine so we can reset properly at end.
**  Like signal handling on UNIX.
**
**	14-oct-1986 (sandyd)
**		Changed to also intercept new error codes generated
**		by the distributed backend.  Also covered case where
**		table name is an empty string (syntax error caught).
**	09/02/87 (dkh) - Fixed jup bug 964.  Now using "iijunk" on
**			 range statement.  It used to be "iirelation".
**	13-dec-1988 (neil) 
**		Changed to check for generic error as well.
**	15-mar-1989 (kathryn)	
**		Use QUEL if backend has ANY QUEL capability 
**		(quelLevel != NOLEVEL).  NOTE: Bug found in terminal monitor
**		- 4487 - Need to change dml to QUEL before calling FErelexists.
**		IIQR_relexists was added to  tm/.../qr/qrmlthlp.qsc
**	26-may-1989 (danielt)
**		fix for bug 6268
**	22-jul-1992 (rdrane)
**		Add 6.5 support for owner.tablename, delimited identifiers,
**		and synonyms.  Change parameterization to include owner.
**		Note that owner is ignored for pre-6.5 database connections,
**		and may be NULL or an empty string.  Indicate that both
**		tablename and owner are expected to be normalized.
**		Replace reference to IIUGlbo_lowerBEobject() with reference
**		to IIUGdbo_dlmBEobject().  Removed unused local variables
**		rcount and err_num.  Note that if 6.5, will NEVER use the QUEL!
**		Note also that 6.5 is not specific on failure ...
**	31-dec-1993 (rdrane)
**		Use OpenSQL level to determine 6.5 syntax features support,
**		and check dlm_Case capability directly to determine if
**		delimited identifiers are suppported.
**      21-apr-1999 (hanch04)
**          Added st.h
*/
# define	NOTABLE		2117
# define	BADOP		2702
# define	NAMELONG	2703
# define	BAD_SYNTAX	2612	/* can happen if tablename is null */
# define	DDB_INVLINK	2651	/* invalid link name in RANGE OF */
# define	DDB_SYNTAX	2652	/* syntax error in RANGE OF */
i4  	    (*IIseterr())();
i4	    IIret_err();
i4	    FEinqerr();
static i4   FEerrproc();
STATUS
FErelexists(fetable,feowner)
  char *fetable;
  char *feowner;
{
	i4		(*oldproc)();
  UIDBDATA *uidbdata;
  char relname_buf[(FE_MAXNAME + 1)];
	STATUS		retval;
	FE_RSLV_NAME	w_frn;
    if  (STcompare(IIUIcsl_CommonSQLLevel(),UI_LEVEL_65) >= 0)
    {
	w_frn.owner_dest = NULL;	/* Not interested in "real" name(s) */
	w_frn.name_dest = NULL;
	if  (FE_resolve(&w_frn,fetable,feowner))
	{
	    return(OK);
	}
	else
	{
	    return(FAIL);
	}
    }
    /*
    **  Fix for BUG 8441. (dkh)
    */
    uidbdata = IIUIdbdata();
    if ((IIUIdml() != UI_DML_GTWSQL) && (IIUIdcq_quelLevel() != UI_DML_NOLEVEL))
    { 
        oldproc = IIseterr(FEerrproc);
/* # line 137 "ferelexi.qsc" */	/* range */
  {
    IIwritio(0,(short *)0,1,32,0,(char *)"range of iijunk=");
    IIwritio(1,(short *)0,1,32,0,fetable);
    IIsyncup((char *)0,0);
  }
/* # line 139 "ferelexi.qsc" */	/* host code */
    	IIseterr(oldproc);
    	return (FEinqerr() != OK) ? FAIL : OK;
    }
    /*fix for bug # 6268 */
    IIUGdbo_dlmBEobject(fetable,FALSE);
    retval = FAIL;
/* # line 140 "ferelexi.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"ferelexi1",6504,26363);
      IIputdomio((short *)0,1,32,0,fetable);
      IIputdomio((short *)0,1,32,0,uidbdata->sdba);
      IIputdomio((short *)0,1,32,0,uidbdata->suser);
      IIputdomio((short *)0,1,32,0,uidbdata->catowner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"ferelexi1",6504,26363);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select table_name from iitables where table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,fetable);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,uidbdata->sdba);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,uidbdata->suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,uidbdata->catowner);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"ferelexi1",6504,26363);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,(FE_MAXNAME + 1)-1,relname_buf);
      if (IIerrtest() != 0) goto IIrtB1;
/* # line 148 "ferelexi.sc" */	/* host code */
	retval = OK;
/* # line 149 "ferelexi.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE1;
      }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 151 "ferelexi.sc" */	/* host code */
    return (retval);
}
static i4
FEerrproc(errnum)
i4	*errnum;
{
    if (*errnum == NOTABLE || *errnum == BADOP ||
	    *errnum == NAMELONG || *errnum == BAD_SYNTAX ||
	    *errnum == DDB_INVLINK || *errnum == DDB_SYNTAX)
    {
	return 0;
    }
    return *errnum;
}
