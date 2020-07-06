# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** WRITETODB.QSC - This file contains the routines used
**		 to write a query definition out to the
**		 database.
**
**	written 11/7/83 (nml)
**	07/14/86 (scl) Remove CPaddnode for 5.0
**	07-dec-87 (kenl)
**		Turned module into a .QSC file in order to incorporate
**		the new header file: mqbf.qsh and mqsymbol.qsh.  All QUEL
**		code has been changed to SQL.  If talking to an INGRES
**		backend, this routine will first write the query definition
**		to a file and then COPY it into the database.  Otherwise, a
**		a series of SQL INSERT statements will save the definition.
**	05-jan-88 (kenl)
**		Added null character string to SQL INSERT to accommodate a
**		backed that does not allow defaults for columns (mqdumpQdef).
**	29-jan-1988 (peter)
**		Add repeat queries.
**	09-nov-88 (marian)
**		Modified column names for extended catalogs to allow them
**		to work with gateways.
**	01-sep-90 (esd/bryanp)
**              The MPE/XL port requires that the temporary file which is
**              created by this routine be created by SIfopen, rather 
**              than through NMt_open (on the MPE/XL, NMt_open opens files
**              in 'binary' mode, which is unacceptable for this temp file).
**              An MPE/XL-specific change has been added (ifdef hp9_mpe)
**              to use NMloc and SIfopen rather than NMt_open.
**	05-oct-92 (daver)
**		Modified IIQFsqz_sqlInsertJoindef() and its callers to
**		support owner names. This includes an extra parameter 
**		for the currently unused ii_joindefs column qinfo5,
**		where the owner name will be stored.
**
** NEED TO check: (1) why qinfo5 is nullable and (2) what EOS puts in
**			the column.
**
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<lo.h>
# include	<nm.h>
# include	<si.h>
# include "mqbf.h"
# include "mqsymbol.h"
# include	<er.h>
# include	<ug.h>
# include	"erqf.h"
# include	<ui.h>
GLOBALREF   i4	    MQdblang;	/* will be set to SQL or Gateway SQL */
VOID	IIQFsqz_sqlInsertJoindef();
static	LOCATION	mqcatloc = {0};
static mqnonode();
STATUS
mqwritetodb(id, qdef)
  OOID id;
MQQDEF	*qdef;
{
    if (MQdblang != UI_DML_GTWSQL)
    {
	/* Open temporary file needed to write out qdef */
	if (mqfileopen() != OK)
	{
		IIUGerr(E_QF00C4__rQBF__ERROR__Unable_,UG_ERR_FATAL,0);
	}
    }
    iiuicw1_CatWriteOn();
    /* first delete old qdef entries */
/* # line 79 "wrttodb.sc" */	/* repeat delete */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"wrttodb1",7161,14046);
      IIputdomio((short *)0,1,30,4,&id);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"wrttodb1",7161,14046);
        IIwritio(0,(short *)0,1,32,0,(char *)
"delete from ii_joindefs where object_id=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&id);
        IIexDefine(0,(char *)"wrttodb1",7161,14046);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 80 "wrttodb.sc" */	/* host code */
    if (FEinqerr())
    {
	iiuicw0_CatWriteOff();
	return(FAIL);
    }
    if (mqdumpTbls(qdef) != OK)
    {
	iiuicw0_CatWriteOff();
	return(FAIL);
    }
    if (mqdumpAttribs(qdef) != OK)
    {
	iiuicw0_CatWriteOff();
	return(FAIL);
    }
    if (mqdumpJoins(qdef) != OK)
    {
	iiuicw0_CatWriteOff();
	return(FAIL);
    }
    if (MQdblang != UI_DML_GTWSQL)
    {
	mqfileclose();
    }
    if (mqdumpQdef(id) != OK)
    {
	/* problem updating database info */
	FDerror(DUMPQDEFBAD, 0, ERx(""));
	iiuicw0_CatWriteOff();
	return(FAIL);
    }
    if (MQdblang != UI_DML_GTWSQL)
    {
        if (mqcopyQdef() != OK)
	{
	    /* problem updating detail info for this QDEF */
	    FDerror(COPYQDEFBAD, 0, ERx(""));
	    iiuicw0_CatWriteOff();
	    return(FAIL);
	}
    }
    /* all went well for writing out qdef */
    iiuicw0_CatWriteOff();
    mqLast(qdef->mqcr.name);
    return(OK);
}
/*
** mqfileopen - create file in ING_TEMP to
**		use for dumping QDEF info.
** History:
**	01-sep-90 (esd/bryanp)
**              The MPE/XL port requires that the temporary file which is
**              created by this routine be created by SIfopen, rather 
**              than through NMt_open (on the MPE/XL, NMt_open opens files
**              in 'binary' mode, which is unacceptable for this temp file).
**              An MPE/XL-specific change has been added (ifdef hp9_mpe)
**              to use NMloc and SIfopen rather than NMt_open.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
STATUS
mqfileopen()
{
	LOCATION	loc;
	i4		status;
# ifdef hp9_mpe
	if ((status = NMloc(TEMP, PATH, (char *)NULL, &loc)) != OK)
	{
		return(status);
	}
	if ((status = LOuniq(ERx("mqbf"), ERx("tmp"), &loc)) != OK)
	{
		return(status);
	}
	LOcopy(&loc, mqflnm, &mqcatloc);
	if ((status = SIfopen(&mqcatloc, ERx("w"), SI_TXT, 256, &mq_tfile)) != OK)
	{
		return(status);
	}
# else  /* not hp9_mpe */
	if ((status = NMt_open(ERx("w"), ERx("mqbf"), ERx("tmp"), &loc, &mq_tfile)) != OK)
	{
		return(status);
	}
	LOcopy(&loc, mqflnm, &mqcatloc);
# endif /* hp9_mpe */
# ifdef lint
	status = status;	/* use variable to keep lint happy */
# endif
	return(OK);
}
/*
** mqfileclose - close temp file used to write out QDEF
*/
mqfileclose()
{
	SIclose(mq_tfile);
}
/*
** mqdumpTbls - dump out information about the tables
**	      used in this qdef to the file.
*/
STATUS
mqdumpTbls(qdef)
MQQDEF	*qdef;
{
	i4	i;
	register	RELINFO	    *rel;
	i4	type;
	char	tbuf1[FE_MAXNAME];
	char	tbuf2[FE_MAXNAME];
	for (i = 0; i < qdef->mqnumtabs; i++)
	{
	    rel = qdef->mqtabs[i];
	    type = i*SEQ_OFFSET + 1;
	    /* 
	    ** added line to write owner to file, in qinfo5, as well (daver)
	    */
	    if (MQdblang != UI_DML_GTWSQL)
	    {
		SIfprintf(mq_tfile, ERx("%d"), qdef->mqcr.id);
		SIfprintf(mq_tfile, ERx("\t%d"), type);
		SIfprintf(mq_tfile, ERx("\t%s"), rel->mqrelid);
		SIfprintf(mq_tfile, ERx("\t%d"), rel->mqtype);
		SIfprintf(mq_tfile, ERx("\t%s"), rel->mqrangevar);
		SIfprintf(mq_tfile, ERx("\t%d"), rel->opdelete);
		SIfprintf(mq_tfile, ERx("\t%s\n"), rel->mqrelown);
	    }
	    else
	    {
		STprintf(tbuf1, ERx("%d"), rel->mqtype);
		STprintf(tbuf2, ERx("%d"), rel->opdelete);
		/*
		** Now IIQFsqz_sqlInsertJoindef() takes an additional
		** parameter to store the owner of the table as well
		*/
		IIQFsqz_sqlInsertJoindef(qdef->mqcr.id, type, rel->mqrelid,
			tbuf1, rel->mqrangevar, tbuf2, rel->mqrelown);
		if (FEinqerr())
		{
		    return(FAIL);
		}
	    }	
	}
	return(OK);
}
/*
** mqdumpAttribs - dump out the attribute info for this qdef
**		 to the file.
*/
STATUS
mqdumpAttribs(qdef)
MQQDEF	*qdef;
{
	register i4	i;
	i4	jinfo1, jinfo2;
	register	ATTRIBINFO	*att;
	i4	type;
	char	tbuf1[FE_MAXNAME];
	char	tbuf2[FE_MAXNAME];
	char	nchar[2];		/* new dummy field for qinfo5 */
	nchar[0] = EOS;
	for (i = 0; i < qdef->mqnumatts; i++)
	{
	    att = qdef->mqatts[i];
	    type = i*SEQ_OFFSET + 2;
	    jinfo1 = att->jpart < 0? 0: 1;
	    jinfo2 = att->jpart == 0? 0: 1;
	    if (MQdblang != UI_DML_GTWSQL)
	    {
		SIfprintf(mq_tfile, ERx("%d"), qdef->mqcr.id);
		SIfprintf(mq_tfile, ERx("\t%d"), type);
		SIfprintf(mq_tfile, ERx("\t%s"), att->rvar);
		SIfprintf(mq_tfile, ERx("\t%s"), att->col);
		SIfprintf(mq_tfile, ERx("\t%s"), att->formfield);
		SIfprintf(mq_tfile, ERx("\t%d%d%d"), att->opupdate,
			jinfo1, jinfo2);
		SIfprintf(mq_tfile, ERx("\t \n"));  /* nothing for new qinfo5 */
	    }
	    else
	    {
		STprintf(tbuf1, ERx("%d%d%d"), att->opupdate, jinfo1, jinfo2);
		IIQFsqz_sqlInsertJoindef(qdef->mqcr.id, type, att->rvar,
			att->col, att->formfield, tbuf1, nchar);
		if (FEinqerr())
		{
		    return(FAIL);
		}
	    }	
	}
	return(OK);
}
/*
** mqdumpJoins - dump out the join info for this qdef
**		 to the file.
*/
STATUS
mqdumpJoins(qdef)
MQQDEF	*qdef;
{
	register i4	i;
	register	JOININFO	*join;
	i4		type;
	char	nchar[2];		/* new dummy field for qinfo5 */
	nchar[0] = EOS;
	for (i = 0; i < qdef->mqnumjoins; i++)
	{
	    join = qdef->mqjoins[i];
	    type = i*SEQ_OFFSET + 3;
	    if (MQdblang != UI_DML_GTWSQL)
	    {
		SIfprintf(mq_tfile, ERx("%d"), qdef->mqcr.id);
		SIfprintf(mq_tfile, ERx("\t%d"), type);
		SIfprintf(mq_tfile, ERx("\t%s"), join->rv1);
		SIfprintf(mq_tfile, ERx("\t%s"), join->col1);
		SIfprintf(mq_tfile, ERx("\t%s"), join->rv2);
		SIfprintf(mq_tfile, ERx("\t%s"), join->col2);
		SIfprintf(mq_tfile, ERx("\t \n"));	/* for new qinfo5 */
	    }
	    else
	    {
		IIQFsqz_sqlInsertJoindef(qdef->mqcr.id, type, join->rv1,
			join->col1, join->rv2, join->col2, nchar);
		if (FEinqerr())
		{
		    return(FAIL);
		}
	    }	
	}
	return(OK);
}
/*
** mqdumpQdef - dump the qdef to the database. This routine
**	      deletes the old qdef entry in the database
**	      and then appends the new one.
**
**		This routine DOES NOT affect the status of the current
**		transaction (ie. commit or rollback).  This routine will
**		return a status of OK or FAIL and it is the responsibility
**		of the caller to determine what to do with the transaction.
*/
STATUS
mqdumpQdef(id)
OOID	id;
{
	i4	type;
	char	q_tblfld[2];
	char	nchar[2];
	nchar[0] = EOS;
	type = 0;
	if (tblfield == 0)
		STcopy(ERx("n"), q_tblfld);
	else
		STcopy(ERx("y"), q_tblfld);
	/* add a nchar for new qinfo5 */
	IIQFsqz_sqlInsertJoindef(id, type, q_tblfld, 
					nchar, nchar, nchar, nchar);
	if (FEinqerr())
	{
	    return(FAIL);
	}
	return(OK);
}
/*
** mqcopyQdef - copy the qdef info in from the file
**
** History:
**	10/27/87 (peterk) - removed INnetrequest code, no longer needed.
*/
STATUS
mqcopyQdef()
{
  char *filename;
	LOtos(&mqcatloc, &filename);
#ifdef CMS
	/*
	**    Since there is no such thing as a null (empty) file in CMS
	**    we must check to make sure the location physically exists.
	**    If it does not, if was probably supposed to be a null
	**    dataset, so we will just return and skip the 'copy from'.
	*/
	if (LOexist(&mqcatloc)!=OK) return(TRUE);
#endif
/* # line 368 "wrttodb.sc" */	/* copy table */
  {
    IIsqInit((char *)0);
    IIwritio(0,(short *)0,1,32,0,(char *)
"copy table ii_joindefs(object_id=c0tab, qtype=c0tab, qinfo1=c0tab, qi\
nfo2=c0tab, qinfo3=c0tab, qinfo4=c0tab, qinfo5=c0nl)from ");
    IIvarstrio((short *)0,1,32,0,filename);
    IIsyncup((char *)0,0);
  }
/* # line 373 "wrttodb.sc" */	/* host code */
	if (FEinqerr())
		return(FAIL);
	LOdelete(&mqcatloc);
	return(OK);
}
static
mqnonode()
{
	IIUGerr(E_QF00C5_Could_not_get_current,UG_ERR_FATAL,0);
}
VOID
IIQFsqz_sqlInsertJoindef(jid, jqtype, 
				jqinfo1, jqinfo2, jqinfo3, jqinfo4, jqinfo5)
  i4 jid;
  i4 jqtype;
  char *jqinfo1;
  char *jqinfo2;
  char *jqinfo3;
  char *jqinfo4;
  char *jqinfo5;
{
/* # line 396 "wrttodb.sc" */	/* repeated insert */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"wrttodb2",7162,25597);
      IIputdomio((short *)0,1,30,4,&jid);
      IIputdomio((short *)0,1,30,4,&jqtype);
      IIputdomio((short *)0,1,32,0,jqinfo1);
      IIputdomio((short *)0,1,32,0,jqinfo2);
      IIputdomio((short *)0,1,32,0,jqinfo3);
      IIputdomio((short *)0,1,32,0,jqinfo4);
      IIputdomio((short *)0,1,32,0,jqinfo5);
      IIsyncup((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"wrttodb2",7162,25597);
        IIwritio(0,(short *)0,1,32,0,(char *)
"insert into ii_joindefs(object_id, qtype, qinfo1, qinfo2, qinfo3, qin\
fo4, qinfo5)values(");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,30,4,&jid);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,30,4,&jqtype);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,jqinfo1);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $3=");
        IIputdomio((short *)0,1,32,0,jqinfo2);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $4=");
        IIputdomio((short *)0,1,32,0,jqinfo3);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $5=");
        IIputdomio((short *)0,1,32,0,jqinfo4);
        IIwritio(0,(short *)0,1,32,0,(char *)", ");
        IIwritio(0,(short *)0,1,32,0,(char *)" $6=");
        IIputdomio((short *)0,1,32,0,jqinfo5);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"wrttodb2",7162,25597);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
  }
/* # line 401 "wrttodb.sc" */	/* host code */
}
