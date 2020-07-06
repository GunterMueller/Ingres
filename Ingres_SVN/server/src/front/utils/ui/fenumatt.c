# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ui.h>
/**
** Name:	fenumatts.sc -  determine the maximum number of attributes in
**				a relation.
**
** Description:
**	This file defines:
**
**	FEnumatts()	determine the number of attributes of a relation.
**
** History:
**	Revision 6.3  89/11  wong
**	Modified to use aggregate for performance.
**
**	Revision 6.0  87/09/04  rdesmond
**	Initial revision.
**	11-feb-88 rdesmond  modified to use FErel_ffetch() instead of
**		FErel_open(), FErel_fetch(), FErel_close sequence for
**		performance.
**	19-aug-1988 (peter)
**		update column names for standard catalogs for 6.1.
**      22-jul-1992 (rdrane)
**              Add 6.5 support for owner.tablename, delimited identifiers,
**              and synonyms.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/*{
** Name:	FEnumatts -  determine the number of attributes of a relation.
**
** Description:
**	Returns the number of attributes of a relation.
**
** Inputs:
**	 relname	Name of the relation you want the attributes of.
**			This is expected to be in normalized form.
**
**	 relowner	Name of the owner of the relation you want the
**			attributes of.  This is also expected to be in
**			normalized form, and may be NULL or an empty string
**			(for pre-6.5 compatibility or when the true owner is
**			unknown or unimportant).  Note that this parameter is
**			ignored for connections to pre-6.5 databases.
**
** Returns:
**	numatts		The number of attributes of the relation.
**			If the relation doesn't exist or other DB error,
**			then returns 0.
**
** History:
**	25-jul-87 (rdesmond) written.
**	11-feb-88 (rdesmond)
**		modified to use FErel_ffetch() instead of FErel_open(),
**		FErel_fetch(), FErel_close sequence for performance.
**		Removed range statement.
**	19-aug-1988 (peter)
**		update column names for standard catalogs for 6.1.
**	30-may-1989 (danielt)
**		changed CVlower to IIUGlbo_lowerBEobject()
**		(my last bug fix)  (sob)
**	11/89 (jhw)  Modified to use aggregate for performance.
**	22-jul-1992 (rdrane)
**		Add 6.5 support for owner.tablename, delimited identifiers,
**		and synonyms.  If 6.5, can bypass call to FErel_ffetch()
**		since we don't require any of the information which is
**		returned in FE_REL_INFO.  Note the implication that both
**		the owner and tablename strings are normalized since they
**		are specified discretely.  Thus, the routine no longer
**		cases the strings.  Be paranoid and split if relname is
**		NULL or an empty string.
**	31-dec-1993 (rdrane)
**		Use OpenSQL level to determine 6.5 syntax features support.
*/
i4
FEnumatts ( relname, relowner )
char	*relname;
char	*relowner;
{
  i4 numatts = 0;
  char *relname_ptr;
  char *relowner_ptr;
    char		relname_buf[(FE_MAXNAME + 1)];
    char		relowner_buf[(FE_MAXNAME + 1)];
    FE_REL_INFO		relinfo;
    FE_RSLV_NAME	w_frn;
    if  ((relname == NULL) || (*relname == EOS))
    {
	return(0);
    }
    if  (STcompare(IIUIcsl_CommonSQLLevel(),UI_LEVEL_65) < 0)
    {
	relname_ptr = relname;
	/* determine owner */
	if  (FErel_ffetch(relname,relowner,&relinfo) != OK)
	{
	    return(0);
	}
	relowner_ptr = relinfo.owner;
    }
    else
    {
	w_frn.owner_dest = &relowner_buf[0];
	w_frn.name_dest = &relname_buf[0];
	if  (!FE_resolve(&w_frn,relname,relowner))
	{
	    return(0);
	}
    	relname_ptr = &relname_buf[0];
    	relowner_ptr = &relowner_buf[0];
    }
/* # line 127 "fenumatt.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"fenumatt1",6505,10845);
      IIputdomio((short *)0,1,32,0,relname_ptr);
      IIputdomio((short *)0,1,32,0,relowner_ptr);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"fenumatt1",6505,10845);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select count(*)from iicolumns where table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,relname_ptr);
        IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,relowner_ptr);
        IIexDefine(0,(char *)"fenumatt1",6505,10845);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&numatts);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 130 "fenumatt.sc" */	/* host code */
    return(numatts);
}
