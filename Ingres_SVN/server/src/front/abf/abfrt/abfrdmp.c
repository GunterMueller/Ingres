# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<ex.h>
# include	<nm.h>
# include	<lo.h>
# include	<ol.h>
# include	<er.h> 
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<abftrace.h>
# include	<fdesc.h>
# include	<abfrts.h>
# include	<erar.h>
# ifdef txDEBUG
/**
** Name:	abfrdmp.qc -	Frame to do runtime system dumps.
**
** Description:
**	Frame to do runtime system data structure dumps.  Calls data structure
**	specific routines to do dumps.
**
** History:
**	Revision 6.4
**	04/07/91 (emerson)
**		Fixed typo in abdmpfdesc.
**
**	Revision 6.3  90/03  wong
**	Moved 'aboltstr()' to "abrtutil.qsc" as 'iiarOLstrType()'.
**
**	06/24/86 (a.dea) -- removed first clause from:
**		for (arr; arr->fdsc_name != NULL; arr++)
**		because Whitesmiths compiler won't accept.
**		Unnecessary anyway, since arr == f, which has
**		been validated != NULL already.
**	16-apr-87 (arthur)
**		Restored first clause as (arr != NULL), since (arr == f)
**		only in one branch of the code.
**	11-Nov-1993 (tad)
**		Bug #56449
**		Replace %x with %p for pointer values where necessary.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
/*{
** Name:	abrtsdmp() -	Frame to do rts data structure dumps.
**
** Description:
**	This routine implements a frame that is a user interface for dumping
**	various data structures at runtime.  It is intended to be used
**	by RTI developers and support people, NOT USERS!!!.
**
**	The data structures are dumped to a help file for leisurely viewing.
**
**	This uses some data structures currently in abfrts.qc.
**
** Side Effects:
**	Calls various routines to dump data structures.	 If the data structures
**	are messed up then it can cause bombs.
**
** History:
*/
/*
** Operations currently supported.
*/
# define	DUMP	1
# define	ALL	2
# define	VIEW	3
static READONLY
  char _Form[]= ERx("abfrrtsdmp");
static bool	initfrm = FALSE;
abrtsdmp()
  {
	i4		op;
	STATUS		stat;
    char buf[MAX_LOC+1];
	if (!initfrm)
	{
		IIARiaForm(_Form);
		initfrm = TRUE;
	}
/* # line 100 "abfrdmp.qsc" */	/* display */
    {
      if (IIdispfrm(_Form,(char *)"f") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 103 "abfrdmp.qsc" */	/* host code */
		abrtsstk(_Form, TRUE);
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 107 "abfrdmp.qsc" */	/* host code */
		abrdmpop(DUMP);
          }
        } else if (IIretval() == 2) {
          {
/* # line 111 "abfrdmp.qsc" */	/* host code */
		abrdmpop(ALL);
          }
        } else if (IIretval() == 3) {
          {
/* # line 115 "abfrdmp.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,MAX_LOC+1-1,buf,(char *)"tf\
ile");
              } /* IIfsetio */
            }
/* # line 116 "abfrdmp.qsc" */	/* host code */
		abtrcopen(buf);
          }
        } else if (IIretval() == 4) {
          {
/* # line 120 "abfrdmp.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,MAX_LOC+1-1,buf,(char *)"tf\
lags");
              } /* IIfsetio */
            }
/* # line 121 "abfrdmp.qsc" */	/* host code */
		abtrcreset(buf);
          }
        } else if (IIretval() == 5) {
          {
/* # line 125 "abfrdmp.qsc" */	/* host code */
		abrdmpop(VIEW);
          }
        } else if (IIretval() == 6) {
          {
/* # line 129 "abfrdmp.qsc" */	/* enddisplay */
            {
              if (1) goto IIfdF1;
            }
          }
        } else {
          if (1) goto IIfdE1;
        } /* IIretval */
      } /* IIrunform */
IIfdF1:
      if (IIchkfrm() == 0) goto IIfdB1;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      goto IIfdE1;
IImuI1:
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERx("Dump"),(char *)0,2,2,1) == 0) goto IIfdE1;
      if (IInmuact(ERx("All"),(char *)0,2,2,2) == 0) goto IIfdE1;
      if (IInmuact(ERx("SetTraceFile"),(char *)0,2,2,3) == 0) goto IIfdE1;
      if (IInmuact(ERx("TraceFlags"),(char *)0,2,2,4) == 0) goto IIfdE1;
      if (IInmuact(ERx("ViewTrace"),(char *)0,2,2,5) == 0) goto IIfdE1;
      if (IInmuact(ERx("End"),(char *)0,2,2,6) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
  }
/* # line 135 "abfrdmp.qsc" */	/* host code */
/*
** Name:	abrdmpop() -	Dump based on info in frame and op.
**
** Description:
**	Dumps the data structures described in the frame using the operation
**	that was passed in.
**	Currently, the data structure type is given in the field: type
**	and can be one of FDESC, QUERY or PARAM
**
**	The name(s) of the data structure(s) to dump is given in the table
**	field names.
**
**	The operation can by DUMP or ALL.
**	DUMP means dump the ones named, ALL means dump all of that type.
**
** Inputs:
**	op	-	The operation to perform.
**
** History:
**	23-sep-1985 (joe)
**		Written.
**      18-Feb-1999 (hweho01)
**          Changed EXCONTINUE to EXCONTINUES to avoid compile error of
**          redefinition on AIX 4.3, it is used in <sys/context.h>.
*/
static i4	getobj();
static
abrdmpop(op)
i4	op;
  {
	i4		type;
	i4		(*func)() = NULL;
	i4		abdmpfdesc(), abdmpqry(), abdmpparam();
	i4		*(*getfunc)();
	FDESC		*abstkfdesc();
	QDESC		*abstkqdesc();
	ABRTSPRM	*abstkparam();
    char *cp;
    i4 ele;
	ABRTSSTK	*stk;
	FILE		*fp;
	LOCATION	tloc;
	LOCATION	loc;
	char		buf[MAX_LOC+1];
	if (op != VIEW && (type = getobj()) == 0)
	{
		IIUGmsg( ERx("Bad object type. (FDESC, PARAM only)"), TRUE, 0 );
		return;
	}
	switch (type)
	{
	  case ABOFDESC:
		func = abdmpfdesc;
		getfunc = (i4*(*)()) abstkfdesc;
		break;
	  case ABOPARAM:
		func = abdmpparam;
		getfunc = (i4*(*)()) abstkparam;
		break;
	}
	if (op != VIEW && func == NULL)
	{
		IIUGmsg( ERx("Type not found"), TRUE, 0 );
		return;
	}
	/*
	** Open a temp file to use
	*/
	if ( NMloc(TEMP, PATH, (char *)NULL, &tloc) != OK
		|| LOuniq(ERx("abf"), ERx("dmp"), &tloc) != OK
			|| ( LOcopy(&tloc, buf, &loc),
			SIopen(&loc, ERx("w"), &fp)) != OK )
	{
		LOtos(&loc, &cp);
		EXsignal( EXCONTINUES,
			2, ERx("abrdmpop: Can't open temp file %s.\n"), cp
		);
		return;
	}
	if (op == VIEW)
		abtrctofile(fp);
	else if (op == ALL)
	{
/* # line 223 "abfrdmp.qsc" */	/* unloadtable */
    {
      if (IItbact(_Form,ERx("frmstk"),0) == 0) goto IItbE1;
      while (IItunload() != 0) {
        IItcogetio((short *)0,1,30,4,&ele,(char *)"stkele");
        IITBceColEnd();
        {
/* # line 225 "abfrdmp.qsc" */	/* host code */
			stk = (ABRTSSTK *) ele;
			(*func)((*getfunc)(stk), fp);
        }
      } /* IItunload */
IItbE1:
      IItunend();
    }
/* # line 228 "abfrdmp.qsc" */	/* host code */
	}
	else
	{
    char name[FE_MAXNAME+1];
/* # line 233 "abfrdmp.qsc" */	/* inquire_frs */
    {
      if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,name,22,_Form,0);
      } /* IIiqset */
    }
/* # line 234 "abfrdmp.qsc" */	/* host code */
		if ( !STequal(name, ERx("frmstk")) )
		{
			IIUGmsg( ERx("You must select a row on the frame stack."),
				TRUE, 0);
			return;
		}
/* # line 240 "abfrdmp.qsc" */	/* getrow */
    {
      if (IItbsetio(2,_Form,ERx("frmstk"),-2) != 0) {
        IItcogetio((short *)0,1,30,4,&ele,(char *)"stkele");
        IITBceColEnd();
      } /* IItbsetio */
    }
/* # line 241 "abfrdmp.qsc" */	/* host code */
		stk = (ABRTSSTK *) ele;
		(*func)((*getfunc)(stk), fp);
	}
	SIclose(fp);
	LOtos(&loc, &cp);
/* # line 246 "abfrdmp.qsc" */	/* helpfile */
    {
      IIhelpfile(ERx("ABF RTS DUMPER"),cp);
    }
/* # line 247 "abfrdmp.qsc" */	/* host code */
	LOdelete(&loc);
  }
static i4
getobj()
  {
    char objname[13];
/* # line 255 "abfrdmp.qsc" */	/* getform */
    {
      if (IIfsetio((char *)"abfrrtsdmp") != 0) {
        IIgetfldio((short *)0,1,32,12,objname,(char *)"type");
      } /* IIfsetio */
    }
/* # line 256 "abfrdmp.qsc" */	/* host code */
	CVlower(objname);
	if (FEprefix(objname, ERx("fdesc")) == 0)
		return ABOFDESC;
	else if (FEprefix(objname, ERx("param")) == 0)
		return ABOPARAM;
	return 0;
  }
char	*abpvstr();
char	*abfdscstr();
/*{
** Name:	abdmpfdesc	-	Dump an fdesc.
**
** Description:
**	Dump the fdesc for a frame.
**
** Inputs:
**	f	-	The fdesc to dump.
**	fp	-	The file to dump the fdesc to.
**
** History:
**	27-sep-1985 (joe)
**		Written.
**	04/07/91 (emerson)
**		Fixed typo in FOR loop: fp++ ==> fdp++.
*/
abdmpfdesc(f, fp)
FDESC	*f;
FILE	*fp;
{
	i4	version;
	FDESCV2 *fdp;
	if (f == NULL)
	{
		SIfprintf(fp, ERx("Empty FDESC"));
		return;
	}
	SIfprintf(fp, ERx("FDESC (assuming version %d): \n"), FD_VERSION);
	for (fdp = (FDESCV2*)fdesc->fdsc_ofdesc; fdp->fdv2_name != NULL ; fdp++)
	{
		SIfprintf(fp, ERx("  Name : %s\n"), fdp->fdv2_name);
		SIfprintf(fp, ERx("  Table : %s\n"), fdp->fdv2_tblname);
		SIfprintf(fp, ERx("  Type (%c) size (%d)\n"),
				stype, ssize );
		SIfprintf(fp, ERx("  Offset : 0x%x\n"), fdp->fdv2_dbdvoff);
		SIfprintf(fp, ERx("  Visiblity : %c\n"), fdp->fdv2_visible);
		SIfprintf(fp, ERx("  Order : %d\n"), fdp->fdv2_order);
		SIfprintf(fp, ERx("  Flags : 0x%2x"), fdp->fdv2_flags);
	}
}
/*{
** Name:	abdmpqry	-	Dump an QRY.
**
** Description:
**	Dump the qdesc for a frame.
**
** Inputs:
**	q	-	The qry to dump.
**	fp	-	The file to dump the fdesc to.
**
** History:
**	27-sep-1985 (joe)
**		Written.
*/
abdmpqry(q, fp)
QRY	*q;
FILE	*fp;
{
	if (q == NULL)
	{
		SIfprintf(fp, ERx("Empty QRY\n"));
		return;
	}
	SIfprintf(fp, ERx("QUERY:\n"));
	SIfprintf(fp, ERx("  putparam : %s\n"), q->qr_putparam);
	SIfprintf(fp, ERx("  wparam : %s\n"), q->qr_wparam);
	SIfprintf(fp, ERx("  form : %s\n"), q->qr_form);
	SIfprintf(fp, ERx("  table : %s\n"), q->qr_table);
	abdmpqdesc(q->qr_qdesc, fp);
	abdmpqry(q->qr_child, fp);
}
abdmpqdesc(q, fp)
QDESC	*q;
FILE	*fp;
{
	SIfprintf(fp, ERx("QDESC not implemented\n"));
}
/*{
** Name:	abdmpparam	-	Dump a param.
**
** Description:
**	Dump the param for a frame.
**
** Inputs:
**	p	-	The param to dump.
**	fp	-	The file to dump the fdesc to.
**
** History:
**	27-sep-1985 (joe)
**		Written.
*/
abdmpparam(p, fp)
ABRTSPRM	*p;
FILE		*fp;
{
	ABRTSOPRM	*op;
	i4		version;
	if (p == NULL)
	{
		SIfprintf(fp, ERx("Empty PARAM"));
		return;
	}
	if (NEWPARAM_MACRO(p))
	{
		op = p->pr_oldprm;
		version = p->pr_version;
	}
	else
	{
		op = (ABRTSOPRM *) p;
		version = 0;
	}
	if (version > 0)
	{
		char	**f;
		char	*cp;
		ABRTSPV *a;
		i4	i;
		SIfprintf(fp, ERx("New parameter:\n"));
		SIfprintf(fp, ERx("  Version : %d\n"), version);
		SIfprintf(fp, ERx("  Args : %d\n"), p->pr_argcnt);
		for (i = 0, a = p->pr_actuals, f = p->pr_formals;
			i < p->pr_argcnt; i++, a++, f++)
		{
			if ((cp = *f) == NULL)
				cp = ERx("<NULL>");
			SIfprintf(fp, ERx("    Formal : %s\n"), cp);
			SIfprintf(fp,
			     ERx("    Actual : type(%d), size(%d), value(%s)\n"),
			     a->abpvtype, a->abpvsize, abpvstr(a));
		}
		SIfprintf(fp, ERx("  Returns : type(%d), addr(%p)\n"), p->pr_rettype,
					p->pr_retvalue);
	}
	else
	{
		SIfprintf(fp, ERx("Old parameter:\n"));
		SIfprintf(fp, ERx("  putparam : %s"), op->pr_tlist);
	}
	abdmpqry(op->pr_qry, fp);
}
static char buf[256] ZERO_FILL;
char *
abpvstr(pv)
ABRTSPV *pv;
{
	i2	dum;
	switch (abs(pv->abpvtype))
	{
	  case OL_I4:
		CVla(*((i4 *) pv->abpvvalue), buf);
		return buf;
	  case OL_F8:
		CVfa(*((f8 *) pv->abpvvalue), 28, 18, 'e', '.', buf, &dum);
		return buf;
	  case OL_STR:
		return pv->abpvvalue;
	}
}
char *
abfdscstr(fd)
OFDESC	*fd;
{
	i2	dum;
	switch (fd->fdsc_type)
	{
	  case 'i':
		CVla(*((i4 *) fd->fdsc_ptr), buf);
		return buf;
	  case 'f':
		CVfa(*((f8 *) fd->fdsc_ptr), 28, 18, 'e', '.', buf, &dum);
		return buf;
	  case 'c':
		return fd->fdsc_ptr;
	}
}
char *
abretstr(type, ret)
i4	type;
OL_RET	*ret;
{
	i2	dum;
	switch (type)
	{
	  case OL_I4:
		CVla(ret->OL_int, buf);
		return buf;
	  case OL_F8:
		CVfa(ret->OL_float, 28, 18, 'e', '.', buf, &dum);
		return buf;
	  case OL_STR:
		return ret->OL_string;
	  case OL_NOTYPE:
		return ERx("No type");
	}
}
FDESC *
abstkfdesc(stk)
ABRTSSTK	*stk;
{
	return stk->abrfsfdesc;
}
ABRTSPRM *
abstkparam(stk)
ABRTSSTK	*stk;
{
	return stk->abrfsprm;
}
# endif /* txDEBUG */
