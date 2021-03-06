# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<cv.h>		/* 6-x_PC_80x86 */
# include	<me.h>
# include	<cm.h>
# include	<st.h>
# include	<ol.h>
# include	<er.h> 
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<abfcnsts.h>
# include	<abftrace.h>
# include	<fdesc.h>
# include	<abfrts.h>
/**
** Name:	abrtqry.qc -	ABF RunTime Frame/Procedure Initialization
**					and Close Module.
** Description:
**	Contains routines used to initialize and close frames and procedures.
**	This includes assigning the parameters and setting up and running
**	any querys passes as parameters.  Defines:
**
**	abrtsfinit()		initialize an OSL frame.
**	abrtspinit()		initialize an OSL procedure.
**	iiarIObjInit()	  	initialize an OSL object (frame or procedure.)
**	iiarOLtype()		return OL type for OFDESC type.
**	abrtsfclose()		close an OSL frame.
**	abrtspclose()		close an OSL procedure.
**	iiarCObjClose()		close an OSL object (frame or procedure)
**	iiarRtsgenV0()		interface for query generation
**
** History:
**	Revision 6.5
**	08/11/93 (donc)
**		Fix for bug 52908 in iiarIObjInit.  EXEC 4GL CALLPROC generates
**		calls that have a nulled out rtsprm->pr_oldprm. This was causing
**		SEG VIOS. I put a check in for this condition.
**	08/13/93 (donc)
**		Fix for bug 52908 and 50410 in g4call, made the last change
**		unnecessary. Instead I am going to set pr_oldprm correctly 
**		in g4call and let this routine run as it did.
**
**	Revision 6.4/02
**	10/25/91 (emerson)
**		Fix for bug 40619 in iiarCObjClose.
**	11/07/91 (emerson)
**		Part of fix for bugs 39581, 41013, and 41014 in iiarCObjClose,
**		abrtsfinit and abrtspinit (supersedes previous fix).
**
**	Revision 6.4
**	05-apr-1991 (mgw) 35186
**		In iiarIObjInit(), call iiarPassArg() with the framename,
**		not formname, in the ABOFRM case.
**
**	Revision 6.1  88/10  wong
**	Changed to call 'IIUGtagFree()' on close instead of 'FEfree()'.
**	21-sep-88 (kenl)
**		Moved declaration of param_type() out of routine to the
**		front of this file.  Changed MEcalloc() calls to MEreqmem().
**
**	Revision 6.0  86/05  arthur
**	Added support and backwards compatibility for version 6.0 and prior.
**
**	Revision 5.0  85/12
**	18-jul-86 (mgw)	Bug # 8918
**		Do STbcompare()'s instead of STcompare()'s to prevent
**		case sensitivity. Must also CVlower() the qry->putparam
**		string for putting query parameters to the proper fields
**		in the called frame, again for case sensitive reasons.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
GLOBALREF ABRTSSTK	*IIarRtsStack;
char	*abpvstr();
OFDESC	*iiar1GetOFdescV1();
bool	abrtsgen();
FDESCV2	*iiarGetFdesc();
static FDESC	*_convertFdesc();
static char	_Empty[1] = {EOS};
/*{
** Name:	abrtsfinit() -	Initialize an OSL Frame.
**
** Description:
**	Initializes an OSL frame.
**
** Inputs:
**	name		Name of the form used in the OSL frame.
**	nrtsprm		A parameter structure built by the caller.
**			This and the fdescarr are NOT necessarily from the
**			same version.
**	fdescarr	An fdesc built by the caller.
**
** History:
**	11/07/91 (emerson)
**		Part of fix for bugs 39581, 41013, and 41014:
**		Move the call to FEbegintag to tagPush in abrtcall.qsc
**		(so objects will be allocated on this frame's tag
**		instead of the parent frame's tag).
*/
STATUS
abrtsfinit(name, nrtsprm, fdescarr)
char		*name;
ABRTSPRM	*nrtsprm;
FDESC		*fdescarr;
{
	FDESC *fdesc;
	DB_DATA_VALUE *dbdv;
	fdesc = _convertFdesc(fdescarr, &dbdv); 
	return iiarIObjInit(name, nrtsprm, fdesc, dbdv, ABOFRM);
}
/*
** Name:	abrtspinit() -	Initialize an OSL procedure.
**
** Description:
**	Initializes an OSL procedure.
**
** Inputs
**	name		Name of the procedure.
**	nrtsprm		A parameter structure built by the caller.
**			This and the fdescarr are NOT necessarily from the
**			same version.
**	fdescarr	An fdesc built by the caller.
**
** History:
**	11/07/91 (emerson)
**		Part of fix for bugs 39581, 41013, and 41014:
**		Move the call to FEbegintag to tagPush in abrtcall.qsc
**		(so objects will be allocated on this procedure's tag
**		instead of the parent frame's tag).
*/
STATUS
abrtspinit(name, nrtsprm, fdescarr)
char		*name;
ABRTSPRM	*nrtsprm;
FDESC		*fdescarr;
{
	FDESC *fdesc;
	DB_DATA_VALUE *dbdv;
	fdesc = _convertFdesc(fdescarr, &dbdv); 
	return iiarIObjInit(name, nrtsprm, fdesc, dbdv, ABOPROC);
}
/*{
** Name:	iiarIObjInit() -  Initialize an OSL object (frame or procedure.)
**
** Description:
**	Initializes an OSL object (a frame or procedure).  This involves passing
**	the arguments and running any queries for the frame or procedure.
**
** Inputs:
**	name		The name of the form for a frame or the name of
**			a procedure.
**	nrtsprm		A parameter structure built by the caller.
**			This and the fdescarr are NOT necessarily from the
**			same version.
**	fdescarr	An fdesc built by the caller.
**	dbdvarr		The base array of DB_DATA_VALUEs for the frame or
**			procedure.  This argument will be NULL (and ignored)
**			for pre-6.0 frames.
**	type		One of ABOFRM, ABOPROC.
**
** History:
**	30-sep-1985 (joe)
**		Extracted from abrtsfinit
**	14-may-1987 (agh)
**		Adapted for v6.0.  Must check version of ABRTSPRM structure
**		in order to determine if pr_qry member is a (QRYVO *) from
**		v4.0/5.0 or a (QRY *) from v6.0.
**	28-may-1987 (agh)
**		Changed name from abrtsoinit().  Added dbdvarr parameter.
**	02/17/91 (emerson)
**		If the FDESCV2's appear to be in version 2 format,
**		call IIARhfiHidFldInit to convert them to version 3 format
**		(this should only happen if there are no fields in the object
**		being initialized; otherwise, IIARhfiHidFldInit should have
**		already been called). (bug 35946; see fdesc.h).
**	05-apr-1991 (mgw) 35186
**		call iiarPassArg() with the framename, not formname, in the
**		ABOFRM case.
*/
static VOID	_QryExpand5();
static STATUS	_FrmAttach5();
STATUS
iiarIObjInit ( name, nrtsprm, fdescarr, dbdvarr, type )
char		*name;
ABRTSPRM	*nrtsprm;
FDESC		*fdescarr;
DB_DATA_VALUE	*dbdvarr;
i4		type;
  {
	ABRTSOPRM	*rtsprm;
	QRY		*qry;		/* used if prmver > 1 */
	QRYV0		*qryv0;		/* used if prmver == 1 */
    char *prm;
    char **argv;
    char *formname = name;
	char		*framename;
	i4		prmver = 0;
	STATUS		rval;
# ifdef txRTS
	if (TRgettrace(ABTRTS, 1))
	{
		abtrcprint(ERx("Initializing %s:%d"), name, type);
		abtrctime(ABTRTS);
	}
# endif
	IIarRtsStack->abrfsfdesc = fdescarr;
	if (NEWFDESC_MACRO(fdescarr) && fdescarr->fdsc_version == FD_V2)
	{
		IIARhfiHidFldInit(fdescarr, dbdvarr);
	}
	if (nrtsprm == NULL) /* no parameters */
		return (OK);
	/*
	** If this is a new rtsprm then
	** get pointer to oldrtsprm.
	*/
	if (NEWPARAM_MACRO(nrtsprm))
	{
		rtsprm = nrtsprm->pr_oldprm;
		prmver = nrtsprm->pr_version;
	}
	else
	{
		rtsprm = (ABRTSOPRM *) nrtsprm;
	}
	argv = rtsprm->pr_argv;
	prm = rtsprm->pr_tlist;
	if (type == ABOFRM && rtsprm->pr_tlist != NULL) /* prmver == 0 */
	{
/* # line 253 "abrtqry.qsc" */	/* putform */
    {
      if (IIfsetio(formname) != 0) {
        IIsf_param(prm, argv,(int *)0);
      } /* IIfsetio */
    }
/* # line 254 "abrtqry.qsc" */	/* host code */
	}
	else 
	{
		if (type == ABOFRM)
		{
			iiarPassArg( formname, nrtsprm, fdescarr, dbdvarr, 1,
					IIarRtsStack->abrfsfrm->abrfrname,
					ERget(F_AR0005_frame)
			);
		}
		else
		{
			iiarPassArg( formname, nrtsprm, fdescarr, dbdvarr, 1,
					name, ERget(F_AR0006_procedure)
			);
		}
	}
# ifdef txRTS
	if (TRgettrace(ABTRTS, 1))
	{
		abtrcprint(ERx("Done passing params"));
		abtrctime(ABTRTS);
	}
# endif
	if (type == ABOFRM)
	{
		framename = IIarRtsStack->abrfsfrm->abrfrname;
		/*
		** The query structure may be either a QRYV0 (if prmver == 1)
		** or a QRY (if prmver > 1).
		*/
		if (rtsprm->pr_qry != NULL)
		{
			if ( prmver == 1 )
			{ /* < 6.0 */
				qryv0 = (QRYV0 *) rtsprm->pr_qry;
				_QryExpand5( qryv0, fdescarr, dbdvarr );
				if (qryv0->qr_child != NULL)
					_QryExpand5( qryv0->qr_child,
							fdescarr, dbdvarr
					);
# ifdef txRTS
				if (TRgettrace(ABTRTS, 1))
				{
					abtrcprint(ERx("Done with expand"));
					abtrctime(ABTRTS);
				}
# endif
				rval = _FrmAttach5( qryv0, fdescarr, framename,
					formname, dbdvarr
				);
				/*
				** BUG 3756
				**
				** If nested query is passed to called frame,
				** we must call '_FrmAttach5()' twice.
				*/
				if (rval == OK && qryv0->qr_child != NULL)
				{
					rval = _FrmAttach5( qryv0->qr_child,
							fdescarr, framename,
							formname, dbdvarr
					);
				}
			}
			else if (prmver > 1)
			{
				qry = rtsprm->pr_qry;
				iiarQryExpand(qry, fdescarr);
				if (qry->qr_child != NULL)
				{
					iiarQryExpand(qry->qr_child, fdescarr);
				}
# ifdef txRTS
				if (TRgettrace(ABTRTS, 1))
				{
					abtrcprint(ERx("Done with expand"));
					abtrctime(ABTRTS);
				}
# endif
				rval = iiarAttachQry(qry, fdescarr, dbdvarr,
					framename, formname);
				if (rval == OK && qry->qr_child != NULL)
				{
					rval = iiarAttachQry(qry->qr_child,
						fdescarr, dbdvarr,
						framename, formname);
				}
			}
# ifdef txRTS
			if (TRgettrace(ABTRTS, 1))
			{
				abtrcprint(ERx("Done with attach"));
				abtrctime(ABTRTS);
			}
# endif
			/*
			** opening call to abrtsgen (which in turn
			** calls QG_generate).
			*/
			if (rval == OK)
			{
				/*
				** bug 10053
				**	change QI_START to QI_OSL so callframe
				** 	with a retrieve goes to a file.
				**
				** Changed back to QI_START for v6.0, since
				**	QI_OSL is no longer needed.
				*/
				/*
				** The query structure may be either a QRYV0
				** (if prmver == 1) or a QRY (if prmver > 1).
				*/
				_VOID_ abrtsgen( ( prmver == 1 )
							? (QRY *)qryv0 : qry,
						QI_START
				);
			}
# ifdef txRTS
			if (TRgettrace(ABTRTS, 1))
			{
				abtrcprint(ERx("Done with first gen"));
				abtrctime(ABTRTS);
			}
# endif
		}
	}
	return OK;
  }
/*
** Name:	 _FrmAttach5() -	Attach a Query Passed as Parameter.
**						(version < 6.0)
** Description:
**	This is called when a query is passed as a parameter to a frame.
**	It has to resolve some things that OSL can't do at compile time
**	because OSL doesn't know the fields of the frame being called.
**	This routine is called to handle pre-6.0 QRYs (now knows as QRYV0s).
**	The 6.0 equivalent of this routine is 'iiarAttachQry()'.
**
** Inputs:
**	qry		The query.
**	nfdesc		The fdesc for the called frame.
**			This can be a new or an old fdesc.
**	framename	The name of the called frame.
**	formname	The called form's frame name.
**
** Returns:
**	{STATUS}  OK if successful.
**		  FAIL otherwise.
**
** History:
**	? (joe)
**		First Written.
**	13-jan-1986 (joe)
**		Added framename and formname and did checks for right form
**		and table names being used.
**	19-may-1986 (agh)
**		First argument changed from QRY to QRYV0.  Local variable
**		changed from QDESC to QDV0.
*/
static bool	_FdescGet();
static STATUS	_tlInsert5();
static STATUS
_FrmAttach5 ( qry, nfdesc, framename, formname, dbdvarr )
QRYV0		*qry;
FDESC		*nfdesc;
char		*framename;
char		*formname;
DB_DATA_VALUE	*dbdvarr;
{
	QDV0		*qdesc = (QDV0 *) qry->qr_qdesc;
	char		**ql;
	char		*tblname;
	char		fd_typelist[DB_GW2_MAX_COLS];
	register char	*tl = fd_typelist;
	char		fd_puttype[DB_GW2_MAX_COLS];
	register char	*pt = fd_puttype;
	register i4	*sl = qdesc->qg_sizelist;
	register char	**ag = qdesc->qg_argv;
	i4		count = 0;
	bool	iiarTblfld();
	if (STbcompare(qry->qr_form, 0, formname, 0, TRUE) != 0)
	{
		abusrerr(NOQRYFRM, framename, qry->qr_form, formname, NULL);
		qry->qr_form = formname;
	}
	ql = qry->qr_qlist;
	tblname = *ql++;
	if (tblname != NULL && *tblname != EOS)
	{
		if (!iiarTblfld(tblname, nfdesc))
		{
			abusrerr(NOQRYTBL, framename, formname, tblname, NULL);
			return FAIL;
		}
	}
	for (; *ql; ql++)
	{
		FDESCV2	*fp;
		if ( (fp = iiarGetFdesc(*ql, tblname, nfdesc)) != NULL )
		{
			if (++count >= DB_GW2_MAX_COLS)
			{
				abusrerr(DOMOVER, NULL);
				return FAIL;
			}
			*ag++ = (char *) (dbdvarr + fp->fdv2_dbdvoff);
			*tl++ = 'a';
			if (fp->fdv2_visible != 'v')
				*pt++ = 'd';
			else
				*pt++ = 'a';
			*sl++ = sizeof(DB_DATA_VALUE);
		}
		else
		{
			/*
			** BUG 3738
			**
			** Generate error message at runtime if query
			** passed to new frame referenced a non-existent
			** field or table field column.
			*/
			if (*tblname == EOS)
				abusrerr(ABNOFIELD, *ql, qry->qr_form, NULL);
			else
				abusrerr(ABNOTFCOL, *ql, tblname, qry->qr_form,
					NULL);
		}
	}
	_tlInsert5(qdesc->qg_param, fd_typelist, qdesc->qg_sizelist, TRUE);
	CVlower(qry->qr_putparam);
	_tlInsert5(qry->qr_putparam, fd_puttype, qdesc->qg_sizelist, FALSE);
	return OK;
}
/*
** Name:	_tlInsert5() -	Insert info into a QDV0's param target list.
**
** Description:
**	This routine inserts information into the param target list of
**	a QDV0.  The information from the arguments 'typelist' and 'sizelist'
**	is inserted into 'paramlist'.
**	This routine is called only for 4.0/5.0 QRY structures.  There is
**	no 6.0 equivalent.
**
** Inputs:
**	paramlist	The param target list.  Elements to be filled in
**			appear as "%    " within the list.
**	typelist	Types of the elements to be filled in:  e.g., 'i',
**			'f' or 'c'.
**	sizelist	Sizes of the elements to be filled in.
**	addcharsize	TRUE if a %c element should have its size filled in;
**			else FALSE.
**
*/
static STATUS
_tlInsert5 ( paramlist, typelist, sizelist, addcharsize )
register char	*paramlist;
register char	*typelist;
register i4	*sizelist;
bool		addcharsize;
{
	char	sizestring[8];
	char	*cp;
	while (*paramlist)
	{
		if (*paramlist++ == '%' && *paramlist == ' ')
		{
			if (*typelist == NULL || *sizelist == NULL)
				return (FAIL);
			else
			{
				*paramlist++ = *typelist;
				/*
				** BUG 9553
				** Also put size if for character string
				** and addcharsize is TRUE.  Addcharsize will
				** be FALSE for putform param statement because
				** those don't take %c with a size.
				** Make sure there is space for it in
				** param string.  If not, generate
				** error message.
				*/
				if (*typelist != 'd' && *typelist != 'a' &&
				    (*typelist != 'c' || addcharsize))
				{
					CVna(*sizelist, sizestring);
					for (cp = sizestring; *cp;)
					{
						if (*paramlist == ' ')
							*paramlist++ = *cp++;
						else
						{
							abusrerr(PARAMSPACE, 0);
							break;
						}
					}
				}
			}
			typelist++;
			sizelist++;
		}
	}
	return (OK);
}
/*
** Name:	_QryExpand5() -	Expand ".all" in Passed in Query (version < 6.0)
**
** Description:
**	Expand a ".all" in a passed in query for when the parameter list
**	object is prior to version 6.0.
*/
static char	*_RelationName();
static VOID	_ExpArgvSet();
static VOID	_ExpParamSet();
static VOID	_SortExpand();
static VOID
_QryExpand5 ( qry, nfdesc, dbdvarr )
QRYV0		*qry;
FDESC		*nfdesc;
DB_DATA_VALUE	*dbdvarr;
{
	char			*rcp;	/* pointer into rparam buffer */
	char			*pcp;	/* pointer into putparam buffer */
	QDV0			*qdesc = (QDV0 *) qry->qr_qdesc;
	char			*relname;
	char			*tfldname;
	char			**qgargv;
	i4			*qgsizelist;
	char			**qrqlist;
	char			**qgav;
	i4			*qgsl;
	char			**qrql;
	char			rparam[ABBIGBUFSIZE];
	char			putparam[ABBIGBUFSIZE];
	i4			count = 0;
	FDESCV2 *fp;
	FDESCV2 *firstfp;
	if ( (relname = _RelationName(qdesc->qg_param)) == NULL )
		return;		/* no '.all' references in target list */
	rparam[0] = EOS;
	putparam[0] = EOS;
	rcp = rparam;
	pcp = putparam;
	tfldname = *(qry->qr_qlist);
	firstfp = (FDESCV2*) nfdesc->fdsc_ofdesc;
	for (fp = firstfp; fp->fdv2_name != NULL; fp++)
	{
		if (fp->fdv2_visible != 'v' && fp->fdv2_visible != 'h')
			continue;
		_ExpParamSet( fp->fdv2_name, fp->fdv2_tblname, tfldname,
			relname, fp->fdv2_visible, (bool)(fp == firstfp), 
			&rcp, &pcp, &count
		);
	}
	if (count == 0)		/* for bug 9734 */
	{
		return;		/* can't MEcalloc with a count of 0 */
	}			/* end bug 9734 */
	*rcp = EOS;
	*pcp = EOS;
	qdesc->qg_param = STalloc(rparam);
	qry->qr_putparam = STalloc(putparam);
	qgargv = (char **) MEreqmem(0, count * sizeof(char **), 
					TRUE, (STATUS *) NULL);
	if ( qgargv == (char **) NULL ||
	    (qgsizelist = (i4 *) MEreqmem(0, count * sizeof(i4 *), 
				TRUE, (STATUS *) NULL)) == (i4 *) NULL ||
	    (qrqlist = (char **) MEreqmem(0, (count + 2) * sizeof(char **),
				TRUE, (STATUS *) NULL)) == (char **) NULL )
	{
		abproerr(ERx("_QryExpand5()"), OUTOFMEM, NULL);
	}
	qgav = qgargv;
	qgsl = qgsizelist;
	qrql = qrqlist;
	*qrql++ = STalloc(tfldname);
	firstfp = (FDESCV2*) nfdesc->fdsc_ofdesc;
	for (fp = firstfp; fp->fdv2_name != NULL; fp++)
	{
		if (fp->fdv2_visible != 'v' && fp->fdv2_visible != 'h')
			continue;
		_ExpArgvSet( fp->fdv2_name, fp->fdv2_tblname, tfldname,
			fp->fdv2_visible, (PTR) (dbdvarr + fp->fdv2_dbdvoff), 
			0, &qgav, &qgsl, &qrql
		);
	}
	*qrql = NULL;
	qdesc->qg_argv = qgargv;
	qdesc->qg_sizelist = qgsizelist;
	qry->qr_qlist = qrqlist;
	qry->qr_argv = qgargv;
	_SortExpand(qry);
}
/*
** Name:	_RelationName() -	Return Name of Relation
**						to Which a '.all' Applies.
** Description:
**	Determines if a '.all' construct (indicated by a "%A" in the
**	query's param target list) appears in a query passed between frames.
**	If so, returns the name of the table being retrieved from;
**	if not, returns NULL.
**	If a '%A' does appear in the query's param list, it is guaranteed
**	to be the only element there (for pre-6.0 OSL).
**
** Inputs:
**	qg_param	The param target list for the query
**
** Returns:
**	{char *}  A reference to the name of the relation to which the '.all'
**			applies, if any; else NULL.
*/
static char *
_RelationName ( qg_param )
char	*qg_param;
{
	register char	*cp;
	for (cp = qg_param; *cp != NULL; cp++)
	{
		if (*cp == '%' && *(cp+1) == 'A')
			return (cp+2);
	}
	return (NULL);
}
/*
** Name:	_ExpArgvSet() -	Set up argvs on expand.
**
** Description:
**	This routine sets up the argv, sizelist and the qlist
**	for the expand routine.
**
** Inputs:
**	field		The field to place in the param string.
**
**	fldstbl		The field's parent according to the FDESC.
**
**	tfldname	The parent we are looking for.
**
**	visible		This field's visibility.
**
**	addr		The address of this field
**
**	size		The size of this field.
**
** Outputs:
**	qgav		A pointer to a pointer to the argv.
**
**	qgsl		A pointer to a pointer to the sizelist.
**
**	qrql		A pointer to a pointer to the query name list.
**
** History:
**	16-jun-1987 (Joe)
**		Extracted from expand for backward compatibility.
*/
static VOID
_ExpArgvSet ( field, fldstbl, tfldname, visible, addr, size, qgav, qgsl, qrql )
char	*field;
char	*fldstbl;
char	*tfldname;
char	visible;
PTR	addr;
i4	size;
char	***qgav;
i4	**qgsl;
char	***qrql;
{
    if (STbcompare(fldstbl, 0, tfldname, 0, TRUE) == 0 && visible == 'v')
    {
	*(*qgav)++ = (char *) addr;
	*(*qgsl)++ = size;
	*(*qrql)++ = field;
    }
}
/*
** Name:	_ExpParamSet() -	Set up param on expand.
**
** Description:
**	This routine sets up the parameter strings for the
**	expand routine.
**
** Inputs:
**	field		The field to place in the param string.
**
**	fldstbl		The field's parent according to the FDESC.
**
**	tfldname	The parent we are looking for.
**
**	relname		The relation to use for this .all.
**
**	visible		This field's visibility.
**
**	first		Whether this is the first element added
**			to the param lists.
**
** Outputs:
**	rcp		A pointer to a pointer into the retrive param
**			string.
**
**	pcp		A pointer to a pointer into the putform param
**			string.
**	
**	count		The number of elements added to the param strings.
**			Will be incremented by one if an element is added on
**			this call.
** History:
**	16-jun-1987 (Joe)
**		Extracted from expand for backward compatibility.
*/
static VOID
_ExpParamSet ( field, fldstbl, tfldname, relname, visible, first, rcp, pcp,
		count )
char	*field;
char	*fldstbl;
char	*tfldname;
char	*relname;
char	visible;
bool	first;
char	**rcp;
char	**pcp;
i4	*count;
{
    char			buf[ABBUFSIZE];
    if (STbcompare(fldstbl, 0, tfldname, 0, TRUE) == 0 && visible == 'v')
    {
	(*count)++;
	STprintf(buf, ERx("%%      = %s.%s"), relname, field);
	if (!first)
	{
	    *(*rcp)++ = ',';
	    *(*rcp)++ = ' ';
	}
	STcopy(buf, *rcp);
	*rcp += STlength(*rcp);
	STprintf(buf, ERx("%s = %%     "), field);
	if (!first)
	{
	    *(*pcp)++ = ',';
	    *(*pcp)++ = ' ';
	}
	STcopy(buf, *pcp);
	*pcp += STlength(*pcp);
    }
}
/*
** Name:	_SortExpand() -
*/
static VOID
_SortExpand ( qry )
register QRYV0	*qry;
{
    register char	**sortlist;
    register char	**qrqlist;
    QDV0		*qdesc = (QDV0 *) qry->qr_qdesc;
    for (sortlist = qdesc->qg_sortlist; *sortlist != NULL; ++sortlist)
    {
	for (qrqlist = &(qry->qr_qlist[1]) ; *qrqlist != NULL ; ++qrqlist)
	{
	    if (STbcompare(*sortlist, 0, *qrqlist, 0, TRUE) == 0)
	    {
		char	posstring[8];
		CVna((qrqlist - qry->qr_qlist), posstring);
		*sortlist = STalloc(posstring);
		break;
	    }
	}
    }
    return;
}
/*{
** Name:	iiarOLtype() -	Return OL Type for Character.
**
** Description:
**	Maps an OFDESC type entry (version < 6.0) to an OL type.
**
** Input:
**	type	{char}  The OFDESC type (in the 'fdsc_type' member.)
**
** Returns:
**	{nat}  The OL type (defined in <ol.h>.)
*/
i4
iiarOLtype ( type )
char	type;
{
	switch (type)
	{
	  case 'i':
		return OL_I4;
	  case 'f':
		return OL_F8;
	  case 'c':
		return OL_STR;
	}
	return OL_NOTYPE;
}
VOID	iiarCObjClose();
/*{
** Name:	abrtsfclose() -	Close an OSL Frame.
**
** Description:
**	Closes an OSL frame.
**
** Inputs:
**	nrtsprm		A parameter structure built by the caller
**			of this frame.
*/
VOID
abrtsfclose(nrtsprm)
ABRTSPRM	*nrtsprm;
{
	iiarCObjClose(nrtsprm, (DB_DATA_VALUE *) NULL, ABOFRM);
}
/*{
** Name:	abrtspclose() -	Close an OSL Procedure.
**
** Description:
**	Closes an OSL procedure.
**
** Inputs:
**	nrtsprm		A parameter structure built by the caller
**			of this procedure.
*/
VOID
abrtpclose(nrtsprm)
ABRTSPRM	*nrtsprm;
{
	iiarCObjClose(nrtsprm, (DB_DATA_VALUE *) NULL, ABOPROC);
}
/*
** Name:	iiarCObjClose() -     Close an OSL Object (Frame or Procedure.)
**
** Description:
**	Closes an OSL object (either a frame or procedure).
**
** Inputs:
**	nrtsprm		The param structure for this object
**	dbdvarr		The base array of DB_DATA_VALUEs for the frame or
**			procedure.  This argument will be NULL (and ignored)
**			for pre-6.0 frames.
**	type		Type of object:  either ABOFRM or ABOPROC.
**
** History:
**	10/25/91 (emerson)
**		Fix for bug 40619: issue IIUGtagFree(FEendtag())
**		even if nrtsprm is NULL.
**	11/07/91 (emerson)
**		Part of fix for bugs 39581, 41013, and 41014:
**		Move the IIUGtagFree(FEendtag()) to tagPop in abrtcall.qsc
**		(for symmetry with the movement of FEbegintag to tagPush).
**		Supersedes previous fix.
*/
VOID
iiarCObjClose ( nrtsprm, dbdvarr, type )
ABRTSPRM	*nrtsprm;
DB_DATA_VALUE	*dbdvarr;
i4		type;
{
	ABRTSOPRM	*rtsprm;
	FDESC		*fdesc;
	i4		prmver = 0;
	char		*form = NULL;
# ifdef txRTS
	if (TRgettrace(ABTRTS, 1))
	{
		abtrcprint(ERx("Closing %d", type));
		abtrctime(ABTRTS);
	}
# endif
	if (nrtsprm != NULL)
	{
		if (NEWPARAM_MACRO(nrtsprm))
		{
			rtsprm = nrtsprm->pr_oldprm;
			prmver = nrtsprm->pr_version;
		}
		else
		{
			rtsprm = (ABRTSOPRM *) nrtsprm;
		}
		/*
		** closing call to abqg (which in turn calls QG_generate).
		** Pass rtsprm->pr_qry->qr_qdesc, QI_BREAK.
		*/
		if (type == ABOFRM && rtsprm->pr_qry != NULL)
		{
			_VOID_ abrtsgen(rtsprm->pr_qry, QI_BREAK);
# ifdef txRTS
			if (TRgettrace(ABTRTS, 1))
			{
				abtrcprint(ERx("Done closing query"));
				abtrctime(ABTRTS);
			}
# endif
		}
		/*
		** If this is a frame get the form name.
		*/
		if (type == ABOFRM)
		{
			form = ((IIarRtsStack->abrfsfrm)->abrform)->abrfoname;
		}
		fdesc = IIarRtsStack->abrfsfdesc;
		if (fdesc != NULL && prmver > 0)
		{
			iiarPassArg( form, nrtsprm, fdesc, dbdvarr, -1, _Empty,
					(type == ABOFRM
						? ERget(F_AR0005_frame)
						: ERget(F_AR0006_procedure))
				);
# ifdef txRTS
			if (TRgettrace(ABTRTS, 1))
			{
				abtrcprint(ERx("BYREF parameters updated"));
				abtrctime(ABTRTS);
			}
# endif
		}
	}
# ifdef txRTS
	if (TRgettrace(ABTRTS, 1))
	{
		abtrcprint(ERx("Done closing %d", type));
		abtrctime(ABTRTS);
	}
# endif
}
/*{
** Name:	iiarRtsgenV0() -	Interface for query generation
**
** Description:
**	This is ABF's interface to QG, the query generation module,
**	for v4.0/5.0.
**	We don't call QG directly because the runtime code would do a
**	substantial amount of checking.
**
**	In v4.0/5.0, this routine was known as abrtsgen().
**	In 6.0, abrtsgen() has become a cover for the old and new
**	query generate routines, which it calls based on the type
**	of its first argument.  If this argument is an old-style
**	(up to 5.0) QRY (now called a QRYV0), the cover routine calls
**	this routine iiarRtsgenV0().  If the argument is a new
**	QRY, then IIARgn1RtsgenV1() is called.
**
** Inputs:
**	qry	A 4.0/5.0 QRY structure (now known as a QRYV0)
**	mode	Mode of the query:  e.g., QI_NMASTER.
**
*/
STATUS
iiarRtsgenV0 ( qry, mode )
QRYV0	*qry;
i4	mode;
  {
    char *plist;
    char **argv;
    char *form = qry->qr_form;
	QRYV0	*child = qry->qr_child;
    char *table;
	i4	output;
	STATUS	err;
	/*
	** BUG 4514
	** Call separate QG routine to find status of current query in order
	** to handle retrieve statements within submenus.
	*/
	if (mode == QI_NMASTER && QG_inquery(qry->qr_qdesc) == FALSE)
	{
		/*
		** Attempt to get next tuple when in fact
		** none were retrieved.
		*/
		return (bool)FALSE;
	}
	err = QG_generate(qry->qr_qdesc, mode, &output);
	QG_trmwhite(qry->qr_qdesc);
	if (output == QO_QDONE || err != OK)
	{
		/*
		** BUG 6187
		** Free memory allocated in the QDV0 when query is done.
		*/
		QG_free(qry->qr_qdesc);
		return (bool)FALSE;
	}
	/*
	** If the query has a child then we need to do the putform
	** for it.  If it is a table field then we fill up the table.
	*/
	if (child != NULL)
	{
		plist = child->qr_putparam;
		argv = child->qr_argv;
		table = child->qr_table;
		if ( *table != EOS )
		{
/* # line 1097 "abrtqry.qsc" */	/* clear */
    {
      IIfldclear(table);
    }
/* # line 1098 "abrtqry.qsc" */	/* host code */
			while ( output != QO_DDONE && output != QO_QDONE &&
					err == OK )
			{
/* # line 1101 "abrtqry.qsc" */	/* loadtable */
    {
      if (IItbact(form,table,1) != 0) {
        IItsc_param(plist, argv,(int *)0);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 1102 "abrtqry.qsc" */	/* host code */
				err = QG_generate(qry->qr_qdesc, QI_GET,
							&output
				);
				QG_trmwhite(qry->qr_qdesc);
			}
		}
		else if ( *plist != EOS )
		{
/* # line 1110 "abrtqry.qsc" */	/* putform */
    {
      if (IIfsetio(form) != 0) {
        IIsf_param(plist, argv,(int *)0);
      } /* IIfsetio */
    }
/* # line 1111 "abrtqry.qsc" */	/* host code */
		}
	}
	plist = qry->qr_putparam;
	argv = qry->qr_argv;
	table = qry->qr_table;
	if ( *table != EOS )
	{
/* # line 1118 "abrtqry.qsc" */	/* clear */
    {
      IIfldclear(table);
    }
/* # line 1119 "abrtqry.qsc" */	/* host code */
		while (output != QO_QDONE && err == OK)
		{
/* # line 1121 "abrtqry.qsc" */	/* loadtable */
    {
      if (IItbact(form,table,1) != 0) {
        IItsc_param(plist, argv,(int *)0);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 1122 "abrtqry.qsc" */	/* host code */
			err = QG_generate(qry->qr_qdesc, QI_GET, &output);
			QG_trmwhite(qry->qr_qdesc);
		}
	}
	else if ( *plist != EOS )
/* # line 1127 "abrtqry.qsc" */	/* putform */
    {
      if (IIfsetio(form) != 0) {
        IIsf_param(plist, argv,(int *)0);
      } /* IIfsetio */
    }
/* # line 1129 "abrtqry.qsc" */	/* host code */
	return (bool)TRUE;
  }
/*
** Name:	_convertFdesc() -	convert pre-6.0 FDESC to modern format.
**
** Description:
**
** Inputs:
**	fdesc		The FDESC.
**
** Rerturns:
**	{FDESC*}	The converted FDESC..
*/
static FDESC *
_convertFdesc(oldfdesc, dbvp)
FDESC *oldfdesc;
DB_DATA_VALUE **dbvp;
{
	OFDESC *fp;
	OFDESC *oldfp;
	FDESC *ret;
	FDESCV2 *nfp;
	DB_DATA_VALUE *dp;
	i4 fcount;
	i4 version = 0;
	if (!NEWFDESC_MACRO(oldfdesc))
	{
		/* == 3.0 */
		oldfp = (OFDESC *) oldfdesc;
	}
	else if ((version = oldfdesc->fdsc_version) == 1)
	{
		/* 4.0 or 5.0 */
		oldfp = oldfdesc->fdsc_ofdesc;
	}
	else 
	{
		/* 6.0 or higher, needs no conversion. */
		return oldfdesc;
	}
	for (fp = oldfp, fcount = 0; fp->fdsc_name != NULL; fp++)
		fcount++;
	/* setup the FDESC */
	ret = (FDESC*) FEreqmem(0, sizeof(FDESC), TRUE, (STATUS*)NULL);
	ret->fdsc_cnt = fcount;
	ret->fdsc_version = FD_VERSION;
	ret->fdsc_ofdesc = (OFDESC*) FEreqmem(0, (sizeof(FDESCV2) * fcount), 
			TRUE, (STATUS*)NULL);
	*dbvp = (DB_DATA_VALUE*) FEreqmem(0, sizeof(DB_DATA_VALUE) * fcount,
			TRUE, (STATUS*)NULL);
	dp = *dbvp;
	nfp = (FDESCV2 *) ret->fdsc_ofdesc;
	for ( fp = oldfp, fcount = 0; 
			fp->fdsc_name != NULL; 
			fp++, nfp++, dp++, fcount++)
	{
		DB_DT_ID iiarDbvType();
		nfp->fdv2_name = fp->fdsc_name;
		nfp->fdv2_tblname = fp->fdsc_tblname;
		if (version == 0)
			nfp->fdv2_visible = 'v';
		else
			nfp->fdv2_visible = fp->fdsc_visible;
		nfp->fdv2_order = fp->fdsc_order;
		nfp->fdv2_dbdvoff = fcount;
		/* set up the dbv */
		dp->db_data = (PTR)fp->fdsc_ptr;
		dp->db_datatype = iiarDbvType( iiarOLtype(fp->fdsc_type) );
		dp->db_length = fp->fdsc_size;
		dp->db_prec = 0;
	}
	return ret;
}
/*
** Name:	iiarGetFdesc() - Get FDESCV2.
**
** Description:
**	This routine is the 8.0 equivalent of 'iiar1GetOFdescV1()'
**	and 'iiar2GetFdescV2'.
**
** Inputs:
**	field		Name of the field (or table field column).
**	tblname		Name of the table field (if any).
**	fdesc		The FDESC.
*/
FDESCV2 *
iiarGetFdesc ( field, tblname, fdesc )
char	*field;
char	*tblname;
FDESC	*fdesc;
{
	register FDESCV2 *fp;
	for (fp = (FDESCV2 *)fdesc->fdsc_ofdesc; fp->fdv2_name != NULL; fp++)
	{
		/* only find visible and hidden items. */
		if (fp->fdv2_visible != 'v' && fp->fdv2_visible != 'h')
			continue;
		if (field == NULL
		  || (CMcmpnocase(field, fp->fdv2_name) == 0 
		    && STbcompare(field, 0, fp->fdv2_name, 0, TRUE) == 0 
		    )
		  && CMcmpnocase(tblname, fp->fdv2_tblname) == 0 
		  && STbcompare(tblname, 0, fp->fdv2_tblname, 0, TRUE) == 0)
		{
			return fp;
		}
	}
	return NULL;
}
