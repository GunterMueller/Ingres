# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<ex.h>
#include	<me.h>
#define _alloc(size)  MEreqmem(0, (size), FALSE, (STATUS *)NULL)
#define _calloc(size) MEreqmem(0, (size), TRUE, (STATUS *)NULL)
#include	<si.h>
#include	<st.h>
#include	<ol.h>
#include	<er.h>
#include	<gl.h>
#include	<sl.h>
#include	<iicommon.h>
#define DB_PS_TO_LEN_MACRO(ps) DB_PREC_TO_LEN_MACRO(DB_P_DECODE_MACRO(ps))
#include	<fe.h>
#include	<ug.h>
#include	<eqlang.h>
#include	<eqrun.h>
#include	<feconfig.h>
#include	<adf.h>
#include	<afe.h>
#include	<abclass.h>
#include	<abfcnsts.h>
#include	<abftrace.h>
#include	<fdesc.h>
#include	<abfrts.h>
#include	<rtsdata.h>
#include	<g4funcs.h>
#include	"rts.h"
#include        <ft.h>
#include        <fmt.h>
#include        <frame.h>
#include        <menu.h>
#include        <frsctblk.h>
/**
** Name:	abrtcall.qc -	ABF Run-Time System Call Module.
**
** Description:
**	Contains routines used to call the ABF RTS objects, that is frames and
**	procedures and local procedures.  Defines:
**
**	IIARstart()		initialize ABF run-time system.
**	IIARfrmCall()		call ABF frame object.
**	IIARisFrm()		is name for a frame?
**	abrtscall()		pre-6.1 entry point.
**	cabrtscal()		call 2.0 ABF frame object (WSC compatibility.)
**	abrtsiclose()		close ABF interpreter frame.
**	iiarOldReturns()
**	abrtsexit()		exit ABF application.
**	cabrtsexi()		exit 2.0 ABF application (WSC compatibility.)
**	abrtspcall()		call 2.0 ABF procedure object.
**	cabrtspca()		call 2.0 ABF proc' object (WSC compatibility.)
**	abrtsnpcall()		call 3.0 ABF known procedure object.
**	IIARlpcLocalProcCall()	call ABF local procedure.
**	IIARccdCopyCallersDbdvs() copy caller's DBDVs and adjust ABRTSPRM.
**	IIARdbProcCall()	call ABF database procedure.
**	IIARprocCall()		call ABF procedure object.
**	IIARisProc()		is name for a procedure?
**	abrtsretcall()		pre-6.1 entry point.
**	IIARgglGetGlobal()	get global dbv for a specified global name
**	IIARcglCopyGlobal()	copy a global dbv to a local dbv & verify types
**
** History:
**
**	Revision 6.0  87/07  arthur
**	Added 6.0 compatibility.
**	08/19/88 (dkh) - Changed static READONLY declarations to
**			 just static to regain shareability of
**			 shared libs on VMS.
**
**	Revision 6.1  88/05  wong
**	Move 'IIARstart()' here from "inirts.c" so that the RTS
**	table references can be static to this module.
**
**	Revision 6.3  90/03  wong
**	Added 'IIARisProc()'.
**
**	Revision 6.4
**	11/09/90 (emerson)
**		Modify function hostCall to fix bug 34389.
**	03/26/91 (emerson)
**		Added IIARisFrm (for abrtmain.c, so that starting procedures
**		don't need to be preceded by -p when invoking an image).
**		Also added logic to IIARfrmCall and IIARprocCall to handle
**		the case where the application is running in "batch" mode.
**	04/07/91 (emerson)
**		Modifications for local procedures:
**		Added IIARlpcLocalProcCall and IIARccdCopyCallersDbdvs.
**	07/24/91 (emerson)
**		Fix for bug 30660 in "undefined".
**
**	Revision 6.4/02
**	11/07/91 (emerson)
**		Added tagPush and tagPop, and revised IIARstart
**		for bugs 39581, 41013, and 41014.
**	14-jan-92 (davel)
**		Fixed bug 40983 in iiarGetGlobal().
**
**	19-feb-92 (leighb) DeskTop Porting Change:
**		adh_evcvtdb() has only 3 args, bogus 4th one deleted.
**	29-apr-1992 (mgw) Bug #37879
**		Check return value of OLpcall() and report error
**		E_AR0085_OLpcall_failed on failure.
**
**	Revision 6.5
**	25-aug-92 (davel)
**		Change IIARdbProcCall() to receive a procedure name argument
**		rather than a ABRTSPRO argument, so that the same routine
**		can be used for both CALLPROC and EXECUTE PROCEURE.
**	12-nov-92 (davel)
**		After returning from 3GL proc call, check current session
**		by calling IIARchsCheckSession().
**	21-dec-92 (davel)
**		Split iiarGetGlobal() into IIARgglGetGlobal() and 
**		IIARcglCopyGlobal() for EXEC 4GL support.
**	30-dec-92 (davel)
**		Add logic to pass complex objects as i4 handles to 3GL
**		procedures in IIARprocCall() and hostCall().  Also add 
**		EXEC 4GL initialization to IIARstart().
**	14-jan-93 (davel)
**		Small changes to prepare for OL re-architect: allocate
**		return buffer in IIARprocCall rather than relying on a
**		static buffer in OL. Also pad string parameters with extra
**		alignment space to prepare for eventual varchar support in OL.
**	20-jan-93 (davel)
**		Added support for passing decimal parameters as either float
**		or decimal.
**	16-feb-93 (davel)
**		Finish adding decimal support. Also now we pass string arguments
**		to OLpcall as varchar structures.
**	09-mar-93 (davel)
**		Fixed bug 50190 in hostCall() - DB_EMBEDDED_DATA's ed_length
**		member is supposed to hold the precision/scale instead of 
**		the length of the ed_data.
**	15-mar-93 (davel)
**		Make userFrame() return a valid status - EXEC 4GL stuff relies
**		on a valid status from IIARfrmCall().
**	14-Jul-93 (donc)
**		Altered hostCall. Added an abs() around the check for DECIMAL
**		datatype so that DECIMALs wouldn't be treated as f8s if they
**		are passed byref.  I also initialized 'align' so that when
**		DECIMALs are used a random bizarre number bytes of memory 
**		is not malloc'ed.
**	13-Aug-93 (donc) Bug 50410
**		Altered userFrame. Set global Pointer to the address of the
**		ABRTSPRM used for input parameters. This is required if an
**		EXEC 4GL calls a frame w/ parameters while in 'Go'. 
**	17-Nov-93 (donc) Bug 55057, 55059, 56911
**		Modified IIARprocCall() call to return status, otherwise
**		EXEC 4GL calls were not getting their byrefs and return
**		status copied back when invoked within g4call.c
**	20-dec-1993 (donc) Bug 58064
**		Pull references to Global IIarRtsprm. It is now set via a
**		wrapper routine in g4call.c
**      29-nov-1993 (smc)
**		Bug #58882
**		Return values from old frames (3.0 or earlier) are believed
**		to be integer only so earlier cast of int to PTR and then
**		back again in userFrame() should be OK even if the size of
**		a pointer exceeds the size of an int.
**	21-mar-94 (smc) Bug #60829
**		Added #include header(s) required to define types passed
**		in prototyped external function calls.
**	02-feb-96 (pchang)
**		In IIARlpcLocalProcCall(), use the dml that is active for the 
**		current frame for call stack access since a local procedure 
**		resides in the frame from which it is being called (B50652).
**      25-jul-96 (prida01,strpa01)
**              Make actual parameters have the correct number of bytes defined
**              for float4's and short ints (2 bytes). Fix bug 69871,60850
**	24-sep-96 (boama01) Bug #78728
**		Correct hostCall() so that DECIMAL parms cnvrtd to F8 (and
**		complex (DMY) parms cnvrtd to I4) for 3GL proc don't cause
**		divide-by-zero excptn in alignment calcs due to unassigned
**		edv->ed_length.
**      24-sep-96 (hanch04)
**              Global data moved to data.c
**	17/2/97 consi01 bug 80293
**		Added code to promote float4 to float8 when passing by value on
**		unix platforms, as this is what unix c compilers do.
**      10-mar-98 (rosga02)
**              Manually integrated changes 276159 (bug 79267), 276195 (bug
**		80090), 276214 (bug 80468), 276235 (bug 80293) from ingres63p.
**		These changes allow user numeric parameter passing from
**		abf 4gl to 3gls the way it was done in 6.4/05 by specifying
**		a special env/logical  II_PARAM_PASSING 
**      05-may-2000 (ashco01)
**		initialize align in second block for DB_DEC_TYPE. (100599)
**
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      18-sep-2000 (hanch04)
**          Replace STbcompare with STcasecmp,STncasecmp,STcmp,STncmp
**      09-oct-2007 (huazh01)
**          Call IILQprsProcStatus() with correct number of parameter. 
**          This fixes bug 119273. 
*/
# ifndef abs
# define	abs(x)	((x) >= 0 ? (x) : (-(x)))
# endif
/* this should be in ol.h */
# ifndef OL_MAX_RETLEN
# define	OL_MAX_RETLEN	4096+sizeof(i2)+1
# endif
char	*abinpname();
char	*abretstr();
char	*iiarOLstrType();
char	*iiarTypeName();
VOID	iiarCnfEnterFrame();
VOID	iiarCxfExitFrame();
STATUS	IIARfrmCall();
STATUS	IIARprocCall();
GLOBALREF	bool		IIarBatch;
GLOBALREF       bool            IIParamPassing;
static ABRTSFRM	*getFrame();
static VOID	tagPush();
static VOID	tagPop();
static VOID	rtsPush();
static VOID	rtsPop();
static VOID	retValue();
static ABRTSPRO	*getProc();
DB_DT_ID	iiarDbvType();
/*
** ABF RunTime Frame and Procedure Call Stack.
*/
GLOBALREF ABRTSSTK	*IIarRtsStack ;
/*
** ABF RunTime Frame, Procedure, and Globals Tables (and Hash Tables.)
**
**	The frame, procedure, and globals runtime definition tables and hash 
**	lookup tables.  These are references so that they may be dynamically 
**	allocated and also referenced when the ABF RTS is a shared library.
*/
static ABRTSFRM	*FrmTable = NULL;
static ABRTSPRO	*ProcTable = NULL;
static ABRTSGL	*GlobTable = NULL;
static i4	*FrmHash = NULL;	/* frame hash table */
static i4	*ProcHash = NULL;	/* procedure hash table */
static i4	*GlobHash = NULL;	/* procedure hash table */
#ifdef CMS
/*
**	This function reference is used only by CMS.  After an application has
**	been linked (either dynamically or not) it will contain the address of
**	the 'C_to_ln()' routine.  This routine is used to enable the ABF RTS
**	to branch (indirectly) to non-C language procedures (COBOL and FORTRAN,)
**	which cannot be called directly under VM/CMS.
*/
static i4 const	(*CtoLn)() = NULL;
#endif
/*{
** Name:	IIARstart() -	Initialize ABF Run-Time System.
**
** Description:
**	Initialize the RunTime system table references.  The tables
**	themselves are defined by ABF in a file that it generates.
**
** Called by:
**	'main()' (defined in "abfrts/abfmain.c".)
**
** Side Effects:
**	Initializes 'FrmTable', 'ProcTable', etc.
**	(Note:  'IIarDbname' is initialized in 'IIARinidb()'.)
**
** History:
**	Written 9/28/82 (jrc)
**	08/85 (jhw) -- Pass runtime tables and database name as para-
**			meters to provide a modular interface to the ABF
**			runtime system (and also allow implementation
**			of the runtime system as a shared library).
**	06/24/86 (a.dea) -- Put ifdef CMS back in for now.
**	01/88 (jhw) -- Removed initialization of 'IIarDbname'.
**	05/88 (jhw) -- Moved to "abrtcall.qsc" so RTS table references
**				can be static.
**	11/07/91 (emerson)
**		Pass new 3rd parm to iiarCicInitClass
**		(for bugs 39581, 41013, and 41014).
**	30-dec-92 (davel)(
**		Add initialization of EXEC 4GL access.
**      10-mar-98 rosga02
**              Read in environment variable II_PARAM_PASSING. A value of
**              FORCEMAX sets global bool variable IIParamPassing to TRUE
**              forcing all int types to be passed as I4 and all float types
**              to be passed as F8. IF II_PARAM_PASSING is any other value or
**              undefined then parameters will use the declared size.
*/
VOID
IIARstart ( obj )
register ABRTSOBJ *obj;
{
    char                *parampassing;
# ifdef CMS
	GLOBALREF const i4	(*ABCTOCLANG)();
# endif
	IIarRtsStack = NULL;
	FrmTable = obj->abroftab;
	ProcTable = obj->abroptab;
	GlobTable = obj->abrogltab;
	FrmHash = obj->abrofhash;
	ProcHash = obj->abrophash;
	GlobHash = obj->abroghash;
	/* pass these to the class-handling module. */
	iiarCicInitClass(obj->abrotytab, obj->abrothash, obj->abrotycnt);
	/* also initialize the EXEC 4GL access to 4GL objects from 3GL. */
	if (IIAG4Init() != OK)
	{
		/* only way this can FAIL is if unable to allocate memory */
		abproerr(ERx("IIAG4Init"), OUTOFMEM, (char *)NULL);  /* exit! */
	}
#ifdef CMS
	CtoLn = ABCTOCLANG;
#endif
/* Add new code to read in environment variable and set flag.
** Default is to pass using the required size (bug 79267).
*/
        IIParamPassing = FALSE;
        parampassing = ERx("");
        NMgtAt(ERx("II_PARAM_PASSING"),&parampassing);
        if(parampassing != NULL && *parampassing != EOS)
                if(STcompare(parampassing,ERx("FORCEMAX"))==0)
                        IIParamPassing = TRUE;
}
/*
** Name:	undefined() -	Undefined Frame/Procedure Frame.
**
** Description:
**	An undefined frame or procedure has been called.  So display a form
**	showing the runtime frame/procedure stack and allow the user to see
**	which frame or procedure called the undefined frame or procedure,
**	and to possibly call other frames or procedures.
**
** Input:
**	name	{char *}  The name of the frame or procedure.
**	type	{struct _objinfo *}  Object specific display information.  
**
** Called by:
**	a runtime routine
**
** Side Effects:
**	May call other frames or procedures.
**
** History:
**	Written 9/3/82 (jrc)
**	Deleted references to TBACC, and added display loops. (ncg)
**	05/17/88 (jhw) -- Merged 'abrtsufrm()' and 'abrtsuproc()' into
**						new routine 'undefined()'.
**	07/24/91 (emerson) Fix for bug 30660. Give the user the option
**		of selecting either a frame or a procedure.
*/
#define _Help_Expl	F_FE0100_ExplFrameHelp
#define _End_Expl	F_FE0102_ExplEnd
#define _FrameObj	ERget(F_AR0005_frame)
#define _ProcObj	ERget(F_AR0006_procedure)
  struct _objinfo {
    char *form;
		char	*helpfile;	/* help file name */
		i4	helptitle;	/* help title message ID */
		bool	inited;		/* is form inited? */
  };
static
	struct _objinfo	undef_frame = {
					ERx("abfrunfrm"),
					hlpUNFRM,
					F_AR0009_UndefinedFrame,
					FALSE
};
static
	struct _objinfo	undef_proc = {
					ERx("abfrunproc"),
					hlpUNPROC,
					F_AR000C_UndefinedProcedure,
					FALSE
};
#define undefined_frame(name) undefined( (name), &undef_frame )
#define undefined_proc(name) undefined( (name), &undef_proc )
static VOID
  undefined( name, op )
  char *name;
  register struct _objinfo *op;
  {
	bool	end;
	VOID	(*invoke_func)(); /* IIARfrmCall or IIARprocCall */
	if (!op->inited)
	{
		/* form not initialized.  do it now. */
		IIARiaForm(op->form);
		op->inited = TRUE;
	}
	for ( end = FALSE ; !end ; )
	{
		char	buf[FE_MAXNAME + 1];
		buf[0] = EOS;
/* # line 431 "abrtcall.qsc" */	/* display */
    {
      IIFRgpcontrol(1,0);
      IIFRgpsetio(6,(short *)0,1,30,sizeof(302),(void *)IILQint(302));
      IIFRgpcontrol(2,0);
      if (IIdispfrm(op->form,(char *)"u") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      IIputfldio((char *)"frmname",(short *)0,1,32,0,name);
      {
/* # line 434 "abrtcall.qsc" */	/* host code */
			iiarStkLoad( IIarRtsStack, op->form, FALSE );
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 440 "abrtcall.qsc" */	/* host code */
			invoke_func = (VOID(*)())IIARfrmCall;
			(VOID) abinpname( ERget(F_AR0008_Frame_to_call),
						buf, FE_MAXNAME );
			/* Break form display in case this frame gets called
			** again.  (A form can only have one display active
			** at a time).
			*/
/* # line 448 "abrtcall.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 454 "abrtcall.qsc" */	/* host code */
			invoke_func = (VOID(*)())IIARprocCall;
			(VOID) abinpname( ERget(F_AR000B_Proc_to_call),
						buf, FE_MAXNAME );
			/* Break form display in case this frame gets called
			** again.  (A form can only have one display active
			** at a time).
			*/
/* # line 462 "abrtcall.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 469 "abrtcall.qsc" */	/* host code */
			FEhelp( op->helpfile, ERget(F_AR0009_UndefinedFrame) );
          }
        } else if (IIretval() == 4) {
          {
/* # line 476 "abrtcall.qsc" */	/* host code */
			end = TRUE;
/* # line 477 "abrtcall.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
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
      if (IInmuact(ERget(F_AR0015_CallFrame),ERget(F_AR0007_CallFrameExpl),2,2
      ,1) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_AR0016_CallProc),ERget(F_AR000A_CallProcExpl),2,2,2)
       == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(_Help_Expl),0,2,3) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,2,3) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(_End_Expl),0,2,4) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,0,2,4) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 482 "abrtcall.qsc" */	/* host code */
		if ( !end && buf[0] != EOS )
		{
			(*invoke_func)( buf, (ABRTSPRM *)NULL );
		}
	} /* end for */
  }
/*
** Name:    retValue() -	Return Value from Frame/Procedure, V4.0.
**
** Description:
**	Used to return a value from an OL_RET (which is only from objects of
**	v4.0 or less) into the address specified in the parameter list.
**
** Inputs:
**	ret	{OL_RET *}  The OL_RET.	 NULL if the called frame or procedure
**			is >= v6.0.  (Note that host-language procedures will
**			not have a NULL return value.)
**	dbdv		Information on the type and length being returned
**			from the called frame.
**	name		Name of the called procedure or frame.
**	kind		String describing the kind of object.
**
** Outputs:
**	prm		The parameter to assign it to.
**			Must be a new parameter structure.
**
** History:
**	07/87 (jhw) -- Added support for `dynamic' strings for returns
**			to objects of version < 6.0.
**	03/90 (jhw) -- Only print RETTYPE errors if ADH could not convert the
**			return type.  JupBug #8425.
**	18-nov-93 (robf)
**          Allow for DB_SEC_TYPE return values, passed as a string value
**	    currently.
*/
static VOID
retValue ( ret, dbdv, name, kind, prm )
OL_RET		*ret;
DB_DATA_VALUE	*dbdv;
char		*name;
char		*kind;
ABRTSPRM	*prm;
{
    ADF_CB		*cb;
    DB_EMBEDDED_DATA	edv;
    AB_TYPENAME		tn;	/* input data value type name */
    ADF_CB	*FEadfcb();
    /*
    ** A NULL value returned from the called frame indicates that the
    ** callee was a version >= 6.0 and not a host-language procedure.  In
    ** this case, any return value has already been handled via a call to
    ** 'IIARrvlReturnVal()' from the frame or OSL procedure, itself.
    */
    if (ret == NULL || prm == NULL)
	return;
    cb = FEadfcb();
    if (prm->pr_version > 1)
    {
	/*
	** Calling frame is >= 6.0.  Called frame or OSL procedure
	** is < 6.0 or a host-language procedure.
	*/
	if (prm->pr_retvalue != NULL)
	{ /* Return value is expected. */
	    register DB_DATA_VALUE  *rdbv = (DB_DATA_VALUE *)prm->pr_retvalue;
	    AB_TYPENAME		    rn;	/* return value type name */
	    /*
	    ** Old frame has not been recompiled, and should only return
	    ** one of the 3 basic types.  But the new RTS structure
	    ** uses a DB_DATA_VALUE, with DB_xxx_TYPEs.
	    */
	    if (dbdv->db_datatype == DB_DYC_TYPE)
	    {
		/*
		** Host language procedure was defined as returning "string".
		** adh does not know about DB_DYC_TYPE.
		*/
		edv.ed_type = DB_CHR_TYPE;
	    }
	    else if (adh_dbtoev(cb, dbdv, &edv) != E_DB_OK)
	    {
		FEafeerr(cb);
		return;
	    }
	    edv.ed_null = (i2 *)NULL;
	    switch (edv.ed_type)
	    {
	      case DB_INT_TYPE:
		edv.ed_data = (PTR)&(ret->OL_int);
		edv.ed_length = sizeof(ret->OL_int);
		break;
	      case DB_FLT_TYPE:
		edv.ed_data = (PTR)&(ret->OL_float);
		edv.ed_length = sizeof(ret->OL_float);
		break;
	      case DB_CHR_TYPE:
		edv.ed_data = (PTR)ret->OL_string;
		edv.ed_length = STlength(ret->OL_string);
		if (rdbv->db_datatype == DB_VCH_TYPE &&
			(rdbv->db_length == ADE_LEN_UNKNOWN ||
				rdbv->db_length == 0))
		{ /* dynamic string -- allocate space. */
		    rdbv->db_length = edv.ed_length + DB_CNTSIZE;
		    if ((rdbv->db_data = _alloc(rdbv->db_length)) == NULL)
			abproerr(ERx("retValue"), OUTOFMEM, (char *)NULL);  /* exit! */
		}
		break;
	      case DB_NODT:
	      default:
		iiarUsrErr( RETNOTYPE, 3, name, kind, iiarTypeName(rdbv, rn) );
		adc_getempty(cb, rdbv);
		return;
	    } /* end switch */
	    if (adh_evcvtdb(cb, &edv, rdbv) != E_DB_OK)		 
	    {
		if ( cb->adf_errcb.ad_errcode != E_AD2009_NOCOERCION )
		{
			FEafeerr(cb);
		}
		else
		{
			iiarUsrErr( RETTYPE, 4, name, kind,
				iiarTypeName(dbdv, tn), iiarTypeName(rdbv, rn)
			);
		}
		adc_getempty(cb, rdbv);
	    }
	}
    }
    else
    {
	/*
	** Calling frame is < 6.0.
	*/
	if (RETEXPECT_MACRO(prm))
	{
	    ADI_FI_ID	junk;
	    if (dbdv->db_datatype == DB_NODT)
	    {
		iiarUsrErr( RETNOTYPE,
				3, name, kind, iiarOLstrType(prm->pr_rettype)
		);
		iiarDefaultRet(prm);
		return;
	    }
	    if ( adi_ficoerce(cb, dbdv->db_datatype,
			iiarDbvType(prm->pr_rettype), &junk) != E_DB_OK )
	    {
		if ( cb->adf_errcb.ad_errcode != E_AD2009_NOCOERCION )
		{
			FEafeerr(cb);
		}
		else
		{
			iiarUsrErr( RETTYPE, 4, name, kind,
					iiarTypeName(dbdv, tn),
					iiarOLstrType(prm->pr_rettype)
			);
		}
		iiarDefaultRet(prm);
		return;
	    }
	    if (dbdv->db_datatype == DB_CHR_TYPE)
	    {
		/*
		** If frame defined as returning a string
		** actually returned no value, return a null string.
		*/
		if (ret->OL_string == (char *)NULL)
		    ret->OL_string = ERx("");
		*((char **) prm->pr_retvalue) = ret->OL_string;
		return;
	    }
	    else
	    {
		DB_DATA_VALUE	temp;
		dbdv->db_data = (PTR)prm->pr_retvalue;
		temp.db_datatype = iiarDbvType(abs(prm->pr_rettype));
		temp.db_prec = 0;
		if (temp.db_datatype == DB_INT_TYPE)
		{
		    temp.db_data = (PTR)&(ret->OL_int);
		    temp.db_length = sizeof(ret->OL_int);
		}
		else
		{
		    temp.db_data = (PTR)&(ret->OL_float);
		    temp.db_length = sizeof(ret->OL_float);
		}
		(VOID)afe_cvinto(cb, &temp, dbdv);
	    }
	}
    }
}
/*{
** Name:	IIARformGet() -	Initialize Form for Frame.
**
** Description:
**	Given a frame name, initialize the form for the frame.
**
** Input:
**	frmname	{char *}  The name of the frame.
**
** History:
**	06/90 (jhw) -- Written for Bug #30774.
*/
VOID
IIARformGet ( frmname )
char	*frmname;
{
	register ABRTSFRM	*frm;
	if ( (frm = getFrame(frmname)) != NULL )
		_VOID_ iiarFormInit(frm);
}
/*{
** Name:	IIARfrmCall() -	Call a Frame.
**
** Description:
**	Call a frame within ABF and go and run the frame.
**	Must shuffle the frame off to the right place to run the frame.
**	Also, this procedure is recursively invoked each time
**	a frame is called.  This recursive nature is used to
**	store a stack of all called frames.  Each recursive invocation
**	declares an element of the runtime stack and is used
**	to chain together a list of runtime stack elements.
**	The advantage of this approach is that it doesn't necessatate
**	the full mechanics of a general list system.
**
**
** Input:
**	frmname		{char *}  The name of the frame to call.
**
**	param		{ABRTSPRM	*}
**			The parameter list for the frame.
**
** Error Messages:
**	ABNXTYPE
**
** History:
**	Written 7/20/82 (jrc)
**	added call to QG_free() in abrtsgen() for BUG 6187
**		8/9/85 (mgw)
**	19-aug-1986 (Joe) Added support for calling interpreted OSL.
**	12-aug-1987 (arthur) Added setting of DML for ADF_CB.
**	03/26/91 (emerson)
**		Added logic to handle the case where the application is
**		running in "batch" mode: a CALLFRAME cannot be implemented.
**		We terminate the application because we can't prompt the user
**		for a different procedure, and it might alter the logic
**		of the application too drastically to skip the callframe
**		and continue.
*/
DB_LANG	IIAFdsDmlSet();
#ifdef COMPILE_OSL
static VOID	_2userFrame();
static STATUS	userFrame();
#endif
STATUS
abrtscall ( frmname, param )
char		*frmname;
ABRTSPRM	*param;
{
	return IIARfrmCall( frmname, param );
}
STATUS
IIARfrmCall ( frmname, param )
char		*frmname;
ABRTSPRM	*param;
{
    register ABRTSFRM	*frm;
    ABRTSSTK		stkele;
    STATUS		rval = OK;
    bool		userfrm = FALSE;	/* Whether user frame called */
    ER_MSGID		frmtype;
    if ( IIarBatch )
    {
	IIUGerr( E_AR0083_BatchCallframe, UG_ERR_FATAL, 1, frmname );
	/* control does not return */
    }
    if ( (frm = getFrame(frmname)) == NULL )
    {
	undefined_frame(frmname);
	if (param != NULL && NEWPARAM_MACRO(param) && RETEXPECT_MACRO(param) )
	    IIARrvlReturnVal((DB_DATA_VALUE*)NULL, param, frmname, _FrameObj);
	return OK;
    }
# ifdef txRTS
    if (TRgettrace(ABTRTS, 0))
    {
	abtrcprint(ERx("Calling frame %s:%d"), frmname, frm->abrfrtype);
	abtrctime(ABTRTS);
    }
# endif
    rtsPush(&stkele, frm, param, frmname, frm->abrfrtype, frm->abrdml);
    switch (frm->abrfrtype)
    {
      case OC_OLDOSLFRAME:
#ifdef COMPILE_OSL
	_2userFrame(frm, param);
#endif
	userfrm = TRUE;
	break;
      case OC_OSLFRAME:
      {
	DB_LANG	adf_dml;		/* DML as defined in ADF_CB */
#ifdef COMPILE_OSL
	adf_dml = IIAFdsDmlSet(frm->abrdml);/* set DML for ADF_CB; may be SQL */
	rval = userFrame( frm, param );
	_VOID_ IIAFdsDmlSet(adf_dml);	/* reset DML for ADF_CB */
#endif
	userfrm = TRUE;
	break;
      }
      case OC_QBFFRAME:
	abrtsqbf(frm, param);
	frmtype = S_AR0040_QBF;
	break;
      case OC_RWFRAME:
	IIARrwfReport(frm, param);
	frmtype = S_AR0041_REPORT;
	break;
# ifndef NOGBF
      case OC_GBFFRAME:
	IIARgbfGraph(frm, param);
	frmtype = S_AR0042_GBF;
	break;
# endif /* NOGBF */
# ifndef NOVIGRAPH
      case OC_GRFRAME:
	IIARvgfGraph(frm, param);
	frmtype = S_AR0043_RUNGRAPH;
	break;
# endif /* NOVIGRAPH */
      default:
	abproerr(ERx("abrtscall"), ABNXTYPE, frm->abrfrtype, (char *)NULL);
    } /* end switch */
    if (!userfrm && param != NULL && NEWPARAM_MACRO(param) &&
		RETEXPECT_MACRO(param) )
    { /* no return value from sub-system frames */
	char	buf[16];
	STlcopy(ERget(frmtype), buf, sizeof(buf)-1);
	iiarUsrErr(ABRETFRM, 2, frmname, buf);
	IIARrvlReturnVal((DB_DATA_VALUE *)NULL, param, frmname, _FrameObj);
    }
    rtsPop();
# ifdef txRTS
    if (TRgettrace(ABTRTS, 0))
    {
	abtrcprint(ERx("Leaving frame %s:%d"), frmname, frm->abrfrtype);
	abtrctime(ABTRTS);
    }
# endif
    return rval;
}
/*{
** Name:	IIARisFrm() -	Is Named Object a Frame?
**
** Description:
**	Given a name returns whether it is a frame.  (That is, is found
**	in the frame table for the application.)
**
** Input:
**	name	{char *}  The object name.
**
** Returns:
**	{bool}  TRUE if name is matches a frame name.
**
** History:
**	03/26/91 (emerson)
**		Written (cribbed from IIARisProc).
*/
bool
IIARisFrm ( name )
char	*name;
{
	return (bool)( getFrame(name) != NULL );
}
# ifndef PCINGRES
# ifdef COMPILE_OSL
/*
** cabrtscal
**	For compatability.
*/
cabrtscal ( frmname, param )
char		*frmname;
ABRTSPRM	*param;
{
	(VOID)IIARfrmCall(frmname, param);
}
/*
** Special statics for the old Abrret which has been moved out
** of the runtime system to better support shared libraries.
**
** tempret is simply a variable to point to in case ABFMAIN
** does not call iiarOldReturns.
*/
static i4	tempret;
static i4	*Abrretp = &tempret;
static i4	*cAbrretp = &tempret;
/*
** Name:	_2userFrame() -	Call a User (OSL) Frame, V2.0.
**
** Description:
**	Call a user frame by running a display loop and displaying the menu.
**	When it returns do a menu get and check things out.
**
**	This routine is for old-style frames (still supported) less than or
**	equal to v2.0.
**
** Input:
**	frm	{ABRTSFRM *}  The runtime definition of the frame.
**	param	{ABRTSPRM *}  The parameters for the frame.
**
** Called by:
**	'IIARfrmCall()'
**
** History:
**	Written 7/20/82 (jrc)
*/
static VOID
_2userFrame ( frm, param )
ABRTSFRM	*frm;
ABRTSPRM	*param;
  {
	register ABRTSUSER	*userfrm = &(frm->abrfruser);
    char *mode;
    i4 *comfrm;
    char *frmname;
	mode = ERx("fill");
	if ( iiarFormInit(frm) != OK )
		return;
	frmname = (frm->abrform)->abrfoname;
/* # line 945 "abrtcall.qsc" */	/* clear */
    {
      IIclrscr();
    }
/* # line 946 "abrtcall.qsc" */	/* host code */
	for (;;)
	{
	/*
	** This is a dummy display loop so that the FRS will think that
	** the same form is getting displayed twice, and not clear the screen.
	** We do this because ABF is displaying a menu without using activates,
	** and FRS has no idea that the form has a menu.
	*/
/* # line 954 "abrtcall.qsc" */	/* display */
    {
      if (IIdispfrm(frmname,mode) == 0) goto IIfdE2;
      goto IImuI2;
IIfdI2:;
      if (IIfsetio((char *)0) == 0) goto IIfdE2;
      {
/* # line 957 "abrtcall.qsc" */	/* host code */
			mode = ERx("update");
			FTputmenu(userfrm->abrmenu);
			/*
			** should evaluate the parameters here
			*/
			IItputfrm(IIfindfrm(frmname));
/* # line 964 "abrtcall.qsc" */	/* breakdisplay */
        {
          if (1) goto IIfdE2;
        }
      }
IIfdB2:
      while (IIrunform() != 0) {
      } /* IIrunform */
      if (IIFRitIsTimeout() != 0) goto IIfdE2;
IIfdF2:
      if (IIchkfrm() == 0) goto IIfdB2;
      if (IIfsetio((char *)0) == 0) goto IIfdE2;
      goto IIfdE2;
IImuI2:
      goto IIfdI2;
IIfdE2:;
      IIendfrm();
    }
/* # line 968 "abrtcall.qsc" */	/* host code */
		/*
		**  NOTE...there's a warning message here from
		**  the compiler since this loop is never reached.
		**  This is due to breakdisplay above which is
		**  necessary so we don't display the same form twice.(nml)
		*/
		/* NOTREACHED */
		FTputmenu(userfrm->abrmenu);
		FTgetmenu(userfrm->abrmenu, NULL);
		if (*Abrretp == 1 || *cAbrretp == 1)
		{
			*Abrretp = 0;
			*cAbrretp = 0;
			return;
		}
		continue;
	}
  }
# endif /* COMPILE_OSL */
# endif /* PCINGRES */
/*{
** Name:    iiarOldReturns() -	Old return type stuff.
**
** Description:
**	The runtime system calls this to tell the runtime system
**	what the locations of Abrret and cAbrret are.
**
** Inputs:
**	aret	Address of Abrret
**	caret	Address of cAbrret
**
** Side Effects:
**	Sets Abrretp and cAbrretp
*/
VOID
iiarOldReturns(aret, caret)
i4	*aret;
i4	*caret;
{
# ifndef	PCINGRES
	Abrretp = aret;
	cAbrretp = caret;
# endif
}
# ifdef COMPILE_OSL
/*
** Name:	userFrame() -	Call a User (OSL) Frame.
**
** Description:
**	Call a user (OSL) frame for v4.0 or later by executing its routine.
**
** Input:
**	frm	{ABRTSFRM *}  The runtime definition of the frame.
**	param	{ABRTSPRM *}  The parameters for the frame.
**
** Called by:
**	'IIARfrmCall()'
**
** History:
**	2-feb-84 (jrc)
**		Written.
**	12-jan-87 (mgw) Bug # 10079
**		Don't call a null routine (probably indicates that frame
**		to be called has same name as some procedure).
**	11-jun-1987 (agh)
**		Changes for v6.0.
*/
static STATUS
userFrame ( frm, param )
ABRTSFRM	*frm;
ABRTSPRM	*param;
{
    register ABRTSNUSER	*userfrm = &(frm->abrfrnuser);
    OL_RET		ret;
    OL_RET		*rp;
    STATUS		stat = OK;
    i4			t;
    ABRTSPRM		rprm;
    FDESC		*fdesc;
    /* Fix for bug 10079 */
    if ( userfrm->abrfunc == NULL )
    {
	iiarUsrErr(ABNULLOBJ, 2, frm->abrfrname, _FrameObj);
	if ( param != NULL && NEWPARAM_MACRO(param) && RETEXPECT_MACRO(param) )
	    IIARrvlReturnVal((DB_DATA_VALUE *)NULL, param, frm->abrfrname,
				_FrameObj
	    );
	return OK;
    }
    /* End fix for bug 10079 */
    if ( iiarFormInit(frm) != OK )
    {
	if ( param != NULL && NEWPARAM_MACRO(param) && RETEXPECT_MACRO(param) )
	    IIARrvlReturnVal((DB_DATA_VALUE *)NULL, param, frm->abrfrname,
				_FrameObj
	    );
	return OK;
    }
    /*
    ** Call the function for the user's frame.  Frames from v4.0 and 5.0
    ** return a pointer to a static OL_RET; frames from 6.0 return NULL.
    */
    tagPush();
    rp = (OL_RET *) (*userfrm->abrfunc)(param, frm->abrfrname, frm);
    tagPop();
    if (param != NULL && param->pr_version > 1)
    {
	/* Calling frame is >= 6.0. */
	if (rp != NULL)
	{ /* Called frame is < 6.0. */
	    retValue(rp, &(userfrm->abrrettype), frm->abrfrname,
				_FrameObj, param
	    );
	}
	/*
	** For called frames from v6.0, a return value has already been
	** handled via a call to 'IIARrvlReturnVal()' from the frame's routine.
	*/
    }
    else
    { /* Calling frame is < 6.0. */
	fdesc = IIarRtsStack->abrfsfdesc;
	/*
	** If the fdesc is old, then this is a <=3.0 frame
	** so it returns an integer.
	*/
	if (fdesc != NULL && !(NEWFDESC_MACRO(fdesc)))
	{
	    /*
	    ** lint truncation warning if size of ptr > i4, but code
	    ** believed to be ok:
	    ** Return values from old frames (3.0 or earlier) are believed
	    ** to be integer only so earlier cast of int to PTR and then
	    ** back again here should be OK
	    */
	    ret.OL_int = (i4) rp;
	    rp = &ret;
	}
	/*
	** If the param is NULL then we are being called from
	** a <=3.0 frame, 'abrunfrm()' or 'undefined()'.
	** If the param is not a new one, then we are being called
	** from a  <= 3.0 frame.
	** In all these cases we don't really know if a return is
	** expected.  If we try to do the return we will run into
	** an error that might not be valid, so we don't do the return
	** if the type of the called procedure is not integer.
	** If it is not an integer, we return 0 as the default.
	*/
	if (param == NULL || !(NEWPARAM_MACRO(param)))
	{
	    if (userfrm->abrrettype.db_datatype != DB_INT_TYPE)
		return OK;
	    rprm.pr_zero = 0;
	    rprm.pr_version = 1;
	    rprm.pr_rettype = OL_I4;
	    rprm.pr_retvalue = (i4 *) &t;
	    param = &rprm;
	    stat = t;	/* For 3.0 compatibility */
	}
	retValue(rp, &(userfrm->abrrettype), frm->abrfrname, _FrameObj, param);
    }
    return stat;
}
# endif /* COMPILE_OSL */
/*
** Name:	getFrame() -	Find Frame Entry for Frame.
**
** Description:
**	Get the runtime definition of a frame from the frame table.  The frame
**	table must be set so the last frame entry is followed by a NULL name.
**
**	Note:  'iiarHash()' lowercases the name.
**
**Input:
**	name	{char *}  The name of the frame to find.
**
** Returns:
**	{ABRTSFRM *}	A reference to the frame structure for the frame,
**			NULL if not found.
**
** Called by:
**	'IIARfrmCall()'
**
** Trace Flags:
**	ROUT
**
** History:
**	Written 7/28/82 (jrc)
*/
static ABRTSFRM *
getFrame ( name )
register char	*name;
{
	i4	iiarHash();
	if ( FrmHash != NULL )
	{
		register ABRTSFRM	*frm;
		register i4		i;
		char			nbuf[ABBUFSIZE+1];
#	ifdef txROUT
		if (TRgettrace(ABTROUTENT, -1))
			abtrsrout(ERx(""), (char *) NULL);
#	endif
		STcopy( name, nbuf );
		name = nbuf;
		for ( i = FrmHash[iiarHash(name)] ; i != -1 ; i = frm->abrnext )
		{
			frm = &(FrmTable[i]);
			if ( STequal( name, frm->abrfrname ) )
				return frm;
		}
	}
#	ifdef txROUT
		if (TRgettrace(ABTROUTEXIT, -1))
			abtrerout(ERx(""), (char *) NULL);
#	endif
	return NULL;
}
/*{
** Name:    abrtsexit() -	Exit from Application.
**
** Description:
**	ABF run-time application exit interface.  Called from user's
**	application code to exit application.
**
*/
VOID
abrtsexit()
{
	abexcexit(OK);
}
VOID
cabrtsexi()
{
    abexcexit(OK);
}
/*
** Name:    tagPush(), and tagPop() - RTS get/free a memory tag for called frame
**
** Description:
**	tagPush gets a memory tag for a frame or procedure that's about to be
**	called.  It also calls the RTS class module for frame initiation.
**	tagPop is called just after a called frame or procedure terminates.
**	It frees the memory tag and its associated memory.  It also
**	calls the RTS class module for frame termination.
**
** History:
**	11/07/91 (emerson)
**		Created (part of fix for bugs 39581, 41013, and 41014).
**		Tags are acquired and released here instead of
**		in IIARfinFrmInit/IIARpinProcInit and iiarCObjClose
**		so that the called frame's records and arrays will be allocated
**		using the called frame's memory tag instead of the caller's.
*/
static VOID
tagPush ()
{
	iiarCnfEnterFrame(FEbegintag());
}
static VOID
tagPop ()
{
	iiarCxfExitFrame();
	IIUGtagFree(FEendtag());
}
/*
** Name:    rtsPush(), and rtsPop() - RTS Frame/Procedure Call Stack Access.
**
** Inputs:
**	ele	{ABRTSSTK *}  The stack element in which to store the new call
**			      frame.  (This is allocated by the caller, and may
**			      be on the stack.)
**
**	frm		{ABRTSFRM *}
**		The runtime frame currently being called. If
**		this is null then the frame is undefined.
**
**	param		{ABRTSPRM *}
**		The runtime parameters.
**
**	name		{char *}
**		The name of the frame.
**
**	usage
**
**	dml	The dml that was active before this frame was called.
**
** History:
**	Written 9/3/82 (jrc)
**	19-aug-1986 (Joe)
**		Added dml argument.
*/
static VOID
rtsPush ( ele, frm, param, name, usage, dml )
ABRTSSTK	*ele;
ABRTSFRM	*frm;
ABRTSPRM	*param;
char		*name;
i4		usage;
DB_LANG		dml;
{
	ele->abrfsnext = IIarRtsStack;
	IIarRtsStack = ele;
	ele->abrfsusage = usage;
	ele->abrfsfrm = frm;
	ele->abrfsname = (name != NULL && *name != EOS ? STalloc(name) : NULL);
	ele->abrfsprm = param;
	ele->abrfsfdesc = NULL;
	ele->abrfsdml = dml;
}
static VOID
rtsPop ()
{
	if ( IIarRtsStack == NULL )
		return;
	if (IIarRtsStack->abrfsname != NULL);
		MEfree((PTR) IIarRtsStack->abrfsname);
	IIarRtsStack = IIarRtsStack->abrfsnext;
}
# ifndef PCINGRES
/*
** abrtspcall
**	Call a known procedure.	 Used to set the language in EQUEL before
**	stepping off into the known.  All procedure calls from OSL go
**	through here.  If the procedure isn't here, assume C and hope
**	for the best.
**
** PARAMETERS
**	func		{nat (*)()}
**		The address of the func. If not 0 we assume it is
**		okay.
**
**	name		{char *}
**		The name of the func.
**
** RETURNS
**	NOTHING
**
** History:
**	9/27/82 (jrc)
**		Written
**	12/16/85 (joe)
**		Took out call to IIdml.	 Now the query language is
**		controlled by the first frame that is called.
*/
abrtspcall (func, name)
i4	(*func)();
char	*name;
{
	ABRTSPRO	*proc;
	if (func == NULL)
	{
		undefined_proc(name);
		return;
	}
	IIlang( ((proc = getProc(name)) == NULL) ? hostC : proc->abrplang );
#ifdef CMS
	/*
	**     In CMS we cannot call anything but C directly; instead
	**     we go through an interface routine called 'C_to_ln'.
	*/
	if (proc->abrplang != hostC)
	   {
	   (*CtoLn)( func, 0, 0, 0 );
	   }
	else
#endif
	(*func)(NULL);
	IIlang(hostC);
}
/*
** cabrtspca
**	For compatability.
*/
cabrtspca(func, name)
	i4	(*func)();
	char	*name;
{
	abrtspcall(func, name);
}
/*{
** Name:    abrtsnpcall() -	ABF Run-Time Call 3.0 Procedure.
**
**	Call a known procedure with arguments.  Sets the language in EQL before
**	stepping off into the procedure.  All procedure calls from 3.0 OSL go
**	through here.  If the procedure isn't here, assume C and hope for the
**	best.
**
**	This is for compatibility with 3.0 applications.
**
**	NOTE:  THIS ROUTINE SEEMS NOT TO BE CALLED BY USERS ANYMORE, AT LEAST
**	THEY'VE ALL PROBABLY RECOMPILED.  I FOUND A RATHER MAJOR BUG IN THIS
**	CODE WHILE DOING 6.0 DEVELOPMENT, WHICH NO-ONE HAS REPORTED.  01/88 jhw.
**
** Input:
**	func	{nat (*)()}  The address of the function. If not NULL we assume
**			     it is okay.
**
**	name	{char *}  The name of the function.
**
**	pv	{OABRTSPV *}  The parameter vector.
**
**	argcnt	{nat}  The number of arguments.
**
** History:
**	2/23/84 (joe)
**		Written
**	12/16/85 (joe)
**		Took out call to IIdml.	 See note in abrtspcall.
**	01/88 (jhw) -- Fixed bug with setting of parameter length
**			for strings (was setting 'OL_type' member to be length.)
*/
i4
abrtsnpcall ( name, func, pv, argcnt )
char		*name;
i4		(*func)();
OABRTSPV	*pv;
i4		argcnt;
{
    ABRTSPRO	*proc;
    i4		lang;
    if (func == NULL)
    {
	undefined_proc(name);
	return NULL;
    }
    IIlang( lang = ((proc = getProc(name)) == NULL) ? hostC : proc->abrplang );
    /*
    ** I don't really think an OSL procedure can be called through this
    ** procedure, but if it happens somehow, then it has to be the case
    ** that the OSL procedure has no arguments.
    */
    if (lang == OLOSL)
    {
	register OL_RET	*rp;
	ABRTSSTK	stkele;
	if (argcnt != 0)
	{
	    iiarUsrErr(POSTOOSL, 1, name);
	}
	else
	{
	    rtsPush( &stkele, (ABRTSFRM *) NULL,
			(ABRTSPRM *) NULL, name, OC_OSLPROC, DB_QUEL
	    );
	    tagPush();
	    rp = (OL_RET *) (*func)(NULL);
	    tagPop();
	    rtsPop();
	}
	return rp->OL_int;
    }
    else
    {
	register OL_PV	*tpv;
	register i4	i;
	register OL_PV	*npv;
	OL_RET		ret;
	OL_PV		bufpv[20];
	/*
	** Convert an old ABRTSPV to a new one.
	*/
	npv = (argcnt > 20 ? (OL_PV *)_alloc(argcnt * sizeof(*npv)) : bufpv);
	if ((tpv = npv) == NULL)
	    abproerr(ERx("abrtsnpcall"), OUTOFMEM, (char *)NULL);
	for (i = 0 ; i++ < argcnt ; ++tpv, ++pv)
	{
	    tpv->OL_type = pv->abpvtype;
	    tpv->OL_value = pv->abpvvalue;
	    tpv->OL_size = 0;
	    if (tpv->OL_type == OL_STR)
		tpv->OL_size = STlength((char *)tpv->OL_value);
	}
#ifdef CMS
	/*
	**     In CMS we cannot call anything but C directly; instead
	**     we go through an interface routine called 'C_to_ln'.
	*/
	if (proc->abrplang != hostC)
	{
	    (*CtoLn)( func, pv, argcnt, lang, OL_I4, &ret );
	}
	else
#endif
	if (OLpcall(func, npv, argcnt, lang, OL_I4, &ret) != OK)
		IIUGerr(E_AR0085_OLpcall_failed, 0, 1, name);
	IIlang(hostC);
	if (npv != bufpv)
	    MEfree((PTR) npv);
	return ret.OL_int;
    }
}
# endif /* PCINGRES */
/*{
** Name:	IIARlpcLocalProcCall() - ABF Run-Time Call Local Procedure.
**
** Description
**	Call a local procedure using a parameter structure.
**
** Inputs:
**	name	{char *}	The name of the procedure to call.
**	prm	{ABRTSPRM *}	The parameters for the call.
**	func	{nat (*)()}	The procedure to call.
**	dbvarr	{DB_DATA_VALUE *} The DBDV array for the local procedure.
**
** History:
**	04/07/91 (emerson)
**		Created for local procedures.
**	02/02/96 (pchang)
**		Use the dml that is active for the current frame for call stack
**		access since a local procedure resides in the frame from which
**		it is being called (B50652).
*/
VOID
IIARlpcLocalProcCall ( name, prm, func, dbvarr )
char		*name;
ABRTSPRM	*prm;
i4		(*func)();
DB_DATA_VALUE	*dbvarr;
{
	ABRTSSTK	stkele;
	rtsPush(&stkele, (ABRTSFRM *)NULL, prm, name, OC_OSLLOCALPROC, 
		IIarRtsStack->abrfsdml);
	tagPush();
	(VOID)(*func)(prm, dbvarr);
	tagPop();
	rtsPop();
}
/*{
** Name:	IIARccdCopyCallersDbdvs() - Copy Caller's DBDVs; Adjust ABRTSPRM
**
** Description
**	Copies a piece of an array of DBDVs and adjusts the DBDV pointers
**	in an ABRTSPRM (ABF runtime parameter) structure (and in the list
**	of actual parameters it points to) to point to the copies.
**
**	This function is called at entry to and exit from a local procedure.
**
**	Note that local procedures can only be called in code generated
**	under release 6.4 or higher, so the pr_retvalue field in ABRTSPRM
**	and the abpvvalue field in ABRTSPV are in fact always pointers
**	to a DB_DATA_VALUE.
**
** Inputs:
**	prm	{ABRTSPRM *}		The parameters to be adjusted.
**	bgn_old	{DB_DATA_VALUE *}	The begining of the array to be copied.
**	num_old	{nat}			The number of DBDVs to be copied.
**	bgn_new	{DB_DATA_VALUE *}	The destination of the copy.
**
** History:
**	04/07/91 (emerson)
**		Created for local procedures.
*/
VOID
IIARccdCopyCallersDbdvs( prm, bgn_old, num_old, bgn_new )
ABRTSPRM	*prm;
DB_DATA_VALUE	*bgn_old;
i4		num_old;
DB_DATA_VALUE	*bgn_new;
{
	ABRTSPV		*ap;
	i4		cnt;
	DB_DATA_VALUE	*dbdv_ptr;
	DB_DATA_VALUE	*end_old = bgn_old + num_old;
	if (num_old)
	{
	  MEcopy( (PTR)bgn_old, (u_i2)( sizeof( DB_DATA_VALUE ) * num_old ),
		(PTR)bgn_new );
	} /* num_old */
	if ( prm == NULL )
	{
		return;
	}
	for ( ap = prm->pr_actuals, cnt = prm->pr_argcnt; cnt > 0; ap++, cnt-- )
	{
		dbdv_ptr = (DB_DATA_VALUE *)ap->abpvvalue;
		if ( dbdv_ptr >= bgn_old && dbdv_ptr < end_old )
		{
			ap->abpvvalue =
				(char *)( bgn_new + ( dbdv_ptr - bgn_old ) );
		}
	}
	dbdv_ptr = (DB_DATA_VALUE *)prm->pr_retvalue;
	if ( dbdv_ptr >= bgn_old && dbdv_ptr < end_old )
	{
		prm->pr_retvalue = (i4 *)( bgn_new + ( dbdv_ptr - bgn_old ) );
	}
	return;
}
/*{
** Name:	IIARprocCall() -	ABF Run-Time Call Procedure.
**
** Description
**	Call a procedure using a parameter structure.  The procedure can be
**	either an OSL procedure or a host language procedure.  The parameter
**	structure is guaranteed to have 'pr_zero' set to NULL, indicating that
**	the procedure is v4.0 or later.
**
** Inputs:
**	name	{char *}  The name of the procedure to call.
**	prm	{ABRTSPRM *}  The parameters for the call.
**
** History:
**	(joe) Written
**	12/16/85 (joe) Took out calls to IIdml.	 See note in abrtspcall.
**	14-oct-1986 (Joe)  Allowed NULL prm structure.
**	06/87 (jhw) Modified to support passing data from DB_DATA_VALUEs
**			to host-language procedures for 6.0.
**	08/88 (jhw) Added support for DB procedures.
**	09/90 (jhw) Removed check for positional parameters for 4GL procedures;
**		procedure initialization also checks anyway (in 'iiarPassArg()'
**		via 'iiarIObjInit()').
**	03/26/91 (emerson)
**		Added logic to handle the case where the application is running
**		in "batch" mode: If we can't find the requested procedure,
**		we terminate the application because we can't prompt the user
**		for a different procedure, and it might alter the logic
**		of the application too drastically to skip the callproc
**		and continue.
**	14-dec-92 (davel)
**		Add byref support.
*/
VOID
abrtsretcall ( name, prm )
char		*name;
ABRTSPRM	*prm;
{
	IIARprocCall( name, prm );
}
STATUS
IIARdbProcCall ( procname, prm )
char		*procname;
register ABRTSPRM	*prm;
{
	i4		anybyref = 0;
	register i4	cnt;
	register ABRTSPV *ap;		/* Ref to array of actuals parameters */
	/* Assume version >= 6.0 ... */
	IIsqInit( (char *)NULL );
	IILQpriProcInit( IIPROC_EXEC, procname );
	if ( prm != NULL )
	{
		register char	**fp;	    /* Ref to array of formal names */
		register i4	isbyref;
		for ( fp = prm->pr_formals, ap = prm->pr_actuals, cnt = 0 ;
				fp != NULL && cnt++ < prm->pr_argcnt ;
				++fp, ++ap )
		{
			isbyref = (ap->abpvtype == -DB_DBV_TYPE);
			anybyref |= isbyref;
			IILQprvProcValio( *fp, isbyref, (i2 *)NULL, (i4)1,
					DB_DBV_TYPE, 0, (PTR)ap->abpvvalue );
		}
		if (anybyref)
		{
			IIsqMods(1);	/* put in "singleton select" mode */
		}
	}
	while ( IILQprsProcStatus(0) != 0 )
		;
	if (anybyref)
	{
		if (IInextget() != 0)
		{
			for ( ap = prm->pr_actuals, cnt = 0 ;
				cnt++ < prm->pr_argcnt ; ++ap )
			{
			if (ap->abpvtype == -DB_DBV_TYPE)
			    {
				IIgetdomio((i2 *)NULL, TRUE, DB_DBV_TYPE, 
						0, ap->abpvvalue);
			    }
			}
		}
		IIsqFlush((char *)NULL, 0);
	}
	if ( prm != NULL )
	{
		if ( RETEXPECT_MACRO(prm) )
		{
			IIeqiqio( (i2 *)NULL, TRUE, DB_DBV_TYPE, 0,
				(PTR)prm->pr_retvalue, ERx("iiret") );
		}
	}
	return OK;
}
static STATUS	hostCall();
STATUS
IIARprocCall ( name, prm )
char			*name;
ABRTSPRM	*prm;
{
	register ABRTSPRO	*proc;
	STATUS			stat = FAIL;
	OL_RET			*rp;
	OL_RET			ret;
	char			retbuffer[OL_MAX_RETLEN];
	if ( (proc = getProc(name)) == NULL || /* Host procedure ... */
		(proc->abrplang != hostDML && proc->abrplang != hostOSL &&
			proc->abrpfunc == NULL) )
	{
		if ( IIarBatch )
		{
			IIUGerr( E_AR0084_CalledProcNotFound,
				 UG_ERR_FATAL, 1, name );
			/* control does not return */
		}
		undefined_proc(name);
	}
	else if ( proc->abrplang == hostDML )
	{ /* DB procedure ... */
		rp = NULL;	/* no old-style return value */
		/*
		** Because of the grammar, formals will either be NULL or the
		** first one will be NULL if all arguments or some arguments
		** are positional.  DB procedures can only have keyword
		** parameters.
		*/
		if ( prm != NULL && prm->pr_argcnt != 0 &&
				(prm->pr_formals == NULL ||
					prm->pr_formals[0] == NULL) )
		{
			iiarUsrErr(POSTOSQL, 1, name);
		}
		else
		{
			stat = IIARdbProcCall( proc->abrpname, prm );
		}
	}
	else
	{
		IIlang(proc->abrplang);
		if ( proc->abrplang == OLOSL )
		{ /* 4GL procedure ... */
			ABRTSSTK	stkele;
			i4	(*func)();
			if ( (func = (i4 (*)())proc->abrpfunc) == NULL )
			{ /* Null routine reference for 4GL proc. */
				/* Note:  Since ABF will not build an
				** application unless it can compile all
				** the 4GL procedures, it should never
				** be possible to have a NULL routine
				** reference for any 4GL procedure.
				*/
				iiarUsrErr(ABNULLOBJ, 2, name, _ProcObj);
			}
			else
			{
				rtsPush( &stkele, (ABRTSFRM *)NULL,
					prm, name,
					OC_OSLPROC,
					DB_QUEL
				);
				tagPush();
				rp = (OL_RET *)(*func)(prm, name, proc);
				tagPop();
				rtsPop();
				stat = OK;
			}
		}
		else
		{ /* Host-language procedure ... */
			if ( prm != NULL && prm->pr_formals != NULL )
			{ /* Host-language procedures can only
			  ** have positional parameters.
			  */
				iiarUsrErr(KEYTOHOST, 1, name);
			}
			else
			{
			  /* before calling the 3GL procedure, save the
			  ** complex object stackpointer (for EXEC 4GL access),
			  ** so that we can pop back to the current point on
			  ** return from the procedure.
			  */
				i4 curr_sp, size;
				(void) IIAG4iosInqObjectStack(&curr_sp, &size);
				ret.OL_string = retbuffer;
				stat = hostCall( proc, prm, rp = &ret );
				(void) IIAG4fosFreeObjectStack(curr_sp);
			  /* also check our current session - 3GL proc may have
			  ** returned with the current session being one that
			  ** that was connected from 3GL, which wouldn't have
			  ** have the UI session data structures initialized.
			  */
				IIARchsCheckSession();
			}
		}
		IIlang(hostC);
	}
	/* Now do the return.  On error return a default value. */
	if ( stat != OK && prm != NULL &&
			NEWPARAM_MACRO(prm) && RETEXPECT_MACRO(prm) )
		IIARrvlReturnVal( (DB_DATA_VALUE*)NULL, prm, name, _ProcObj );
	else if ( rp != NULL )
		retValue(rp, &(proc->abrprettype), name, _ProcObj, prm);
	return stat;
}
/*
** History:
**	03/90 (jhw) -- Modified to use EOS-terminated length for all string
**		parameters returned by reference independent of language.
**		JupBug #6563.  ('OLpcall()' EOS-terminates all strings anyway.)
**	11/09/90 (emerson)
**		Check for complex datatypes erroneously being passed
**		to the 3GL procedure (bug 34389).  If found, emit error message
**		E_AR0065 and return FAIL (don't call the 3GL procedure).
**	30-dec-92 (davel)
**		Add logic to pass complex objects as i4 handles to 3GL
**		procedures.  Objects are freed in IIARprocCall() after
**		hostCall() returns.
**	14-jan-93 (davel)
**		pass strings as varchar-like structures (OL expects this
**		as of 6.5).  All allow passing decimal arguments as decimal.
**	14-Jul-93 (donc)
**		Altered hostCall. Added an abs() around the check for DECIMAL
**		datatype so that DECIMALs wouldn't be treated as f8s if they
**		are passed byref.  I also initialized 'align' so that when
**		DECIMALs are used a random bizarre number bytes of memory 
**		is not malloc'ed.
**	24-sep-96 (boama01) Bug #78728
**		Corrected so that DECIMAL parms cnvrtd to F8 (and complex (DMY)
**		parms cnvrtd to I4) for 3GL proc don't cause divide-by-zero
**		excptn in alignment calcs due to unassigned edv->ed_length.
**	17/2/97 consi01 bug 80293
**		added ifdef for unix so that a float4 being passed by value on
**		the stack is promoted to float8
**	10-mar-98 (rosga02)
**              Added code to test IIParamPassed to decide if parameters are
**              allocated the exact number of bytes needed or are converted
**              to I4 or F8 as per declared type.
**      05-may-2000 (ashco01)
**		initialize align in second block for DB_DEC_TYPE. (100599)
*/
static STATUS
hostCall ( proc, prm, rp )
register ABRTSPRO	*proc;
register ABRTSPRM	*prm;
OL_RET			*rp;
{
	i4			(*func)();
	i4			oltype;
	ADF_CB			*cb;
	DB_EMBEDDED_DATA	edvtemp;
	bool			passdec;
	ADF_CB	*FEadfcb();
	func = (i4 (*)())proc->abrpfunc;
	passdec = proc->abrpfid != OC_UNDEFINED && 
			(proc->abrpfid & APC_PASSDEC) != 0;
	/* Map return type to OL type.	Note this type is that which the ABFRTS
	** expects the procedure to return, not the type it may return!
	*/
	cb = FEadfcb();
	if ( (&proc->abrprettype)->db_datatype == DB_NODT )
	{
		oltype = OL_NOTYPE;
	}
	else if ( (&proc->abrprettype)->db_datatype == DB_DYC_TYPE )
	{
		/*
		** The host language procedure was defined as returning
		** "string" "string".  The ADH module does not know about
		** dynamic strings.
		*/
		oltype = OL_STR;
	}
	else
	{ /* Get embedded type of expected return value */
		if ( adh_dbtoev(cb, &proc->abrprettype, &edvtemp) != E_DB_OK )
		{ /* unlikely */
			FEafeerr(cb);
			return FAIL;
		}
		switch (edvtemp.ed_type)
		{
			case DB_INT_TYPE:
				oltype = OL_I4;
				break;
			case DB_FLT_TYPE:
			case DB_DEC_TYPE:
				/* Note: currently dbtoev doesn't return 
				** DB_DEC_TYPE.  But just in case it ever does,
				** we don't support rturn type of decimal yet.
				*/
				oltype = OL_F8;
				break;
			default:
				oltype = OL_STR;
		}
	}
	if ( prm == NULL || prm->pr_argcnt == 0 )
	{ /* no parameters ... */
		if (OLpcall(func, NULL, 0, proc->abrplang, oltype, rp) != OK)
		{
			IIUGerr(E_AR0085_OLpcall_failed, 0, 1, proc->abrpname);
			return FAIL;
		}
	}
	else
	{
		OL_PV			*actuals;
		DB_EMBEDDED_DATA	*edvs;
		PTR			data;
		if ( !NEWPARAM_MACRO(prm) || prm->pr_version < 2 )
		{ /* 3.0 4.0 5.0 */
			actuals = (OL_PV *)prm->pr_actuals;
		}
		else
		{ /* >= 6.0 */
			register DB_EMBEDDED_DATA	*edv;
			register ABRTSPV		*ap;
			register OL_PV			*argp;
			register i4			cnt;
			i4				size;
			/*
			** Convert actual DBV parameters to embedded values
			** to be passed to the host-language procedure.
			*/
			/* Allocate OL parameter & embedded descriptor arrays */
			if ( (actuals =
				(OL_PV*)_alloc(prm->pr_argcnt*sizeof(*actuals)))
						== NULL
				|| (edvs = (DB_EMBEDDED_DATA*)_alloc(
						prm->pr_argcnt*sizeof(*edvs)
					)) == NULL )
			{
				abproerr(ERx("hostCall"), OUTOFMEM,(char*)NULL);
				/*NOTREACHED*/
			}
			/* Determine embedded types and sizes (aligned) */
			edv = edvs;
			argp = actuals;
			size = 0;
			cnt = prm->pr_argcnt;
			for ( ap = prm->pr_actuals ; --cnt >= 0 ; ++ap )
			{ /* Determine embedded type/size for each DBV actual */
				i4 align = 0;
				DB_DATA_VALUE *dbv = 
						(DB_DATA_VALUE *)ap->abpvvalue;
				if (dbv->db_datatype == DB_DMY_TYPE )
				{
				    /* complex objects are passed as integers
				    ** to the 3GL.  the integer values are 
				    ** assigned later in this function.
				    */
				    edv->ed_type = DB_INT_TYPE;
				    /* Bug #78728: set ed_length for an i4
				    ** "handle", so subsequent alignment calcs
				    ** don't blow up. See later in this func
				    ** for "handle" assignment.
				    */
				    edv->ed_length = sizeof(i4);
				}
				else if ( abs(dbv->db_datatype) == DB_DEC_TYPE )
				{
				    /* currently, adhdbtoev doesn't return
				    ** DB_DEC_TYPE; but this could change.
				    ** So we need to explicitly set to 
				    ** decimal or float based on the
				    ** setting of the procedure's APC_PASSDEC
				    ** bit.
				    */
				    if (passdec) 
				    {
				    	edv->ed_type = DB_DEC_TYPE;
				    	edv->ed_length = dbv->db_prec;
				    }
				    else
				    {
					edv->ed_type = DB_FLT_TYPE;
				    /* Bug #78728: set ed_length as adh_dbtoev
				    ** would, so subsequent alignment calcs
				    ** don't blow up.
				    */
					edv->ed_length = sizeof(f8);
				    }
				}
				else if (adh_dbtoev( cb, dbv, edv) != E_DB_OK )
				{	/* invalid datatype (e.g. complex) */
					IIUGerr(E_AR0065_BadHostArgType, 0,
						1, proc->abrpname);
					MEfree((PTR) actuals);
					MEfree((PTR) edvs);
					return FAIL;
				}
				edv->ed_null = (i2 *)NULL;
				if ( edv->ed_type == DB_INT_TYPE )
				{
                                        /* Add extra types for sizes of 1,2 bytes 
                                        ** as not all ints are 4 bytes long no matter
                                        ** how much we would like them to be
                                        ** bug 60850
                                        */
			                /* Make fix for 60850 conditional on IIParamPassing,
			                ** true means that all integer types are passes as I4
			                ** (bug 79267)
					*/
                              	     if(IIParamPassing)
                                            argp->OL_size = edv->ed_length = sizeof(i4);
                                     else
                                            argp->OL_size = edv->ed_length = 
                                          	(edv->ed_length > sizeof(i4))?
	                                         sizeof(i4): edv->ed_length;
                                     /* Leave this type for historical reasons */
				     argp->OL_type = OL_I4;
                                     if ( (align = size % argp->OL_size) != 0 )
                                                align = argp->OL_size - align;
				}
				else if (edv->ed_type == DB_FLT_TYPE)
				{
                                        /* allow float4 and float8 
                                        ** bug 69871
                                        */
		                        /* Make fix for 69871 conditional on IIParamPassing
		                        ** True means that all float types are passed as F8
		                        ** (bug 79267)
		                        */
/* consi01 bug 80293 */
#ifdef UNIX
                                        if(IIParamPassing || ap->abpvtype>=0)
#else
                                        if(IIParamPassing)
#endif
						argp->OL_size =
						   edv->ed_length = sizeof(f8);
					else
						argp->OL_size = edv->ed_length =
						(edv->ed_length >sizeof(f8))?
						sizeof(f8):edv->ed_length;
                                        argp->OL_type = OL_F8;
                                        if ( (align = size % argp->OL_size) != 0 )
                                                align = argp->OL_size - align;
				}
				else if (edv->ed_type == DB_DEC_TYPE)
				{
					argp->OL_size = DB_PS_TO_LEN_MACRO(edv->ed_length);
					argp->OL_type = OL_DEC;
					/* no alignment needed for decimal */
				}
				else
				{ /* edv->ed_type == DB_CHR_TYPE */
					/* Pass string as varchar structures.
					** Set length to absolute length
					** but allocate an extra space for the
					** EOS terminator for the converted
					** embedded value. Also align on an
					** i2 boundary.
					*/
					argp->OL_size = edv->ed_length;
					argp->OL_type = OL_STR;
					if ( (align = size % sizeof(i2)) != 0 )
						align = sizeof(i2) - align;
					size += 1 + sizeof(i2);	/* + EOS */
				}
				if ( ap->abpvtype < 0 )
				{ /* by reference ... */
					argp->OL_type = -argp->OL_type;
				}
				size += align + argp->OL_size;
				++edv;
				++argp;
			} /* end for */
			/* Allocate data space and convert ... */
			if ( (data = _calloc(size)) == NULL )
			{
				abproerr(ERx("hostCall"), OUTOFMEM,(char*)NULL);
				/*NOTREACHED*/
			}
			edv = edvs;
			argp = actuals;
			size = 0;
			cnt = prm->pr_argcnt;
			for ( ap = prm->pr_actuals ; --cnt >= 0 ; ++ap )
			{ /* Allocate data space and convert each DBV actual */
				i4 align = 0;  /* Bug #100599 not used for DECIMALs */
				DB_DATA_VALUE *dbv = 
						(DB_DATA_VALUE *)ap->abpvvalue;
				if ( edv->ed_type == DB_INT_TYPE )
				{
                                        /* Set alignments up to work of sizes other
                                        ** than just 4 bytes when converting from
                                        ** actual to formal parameters it uses OL_size
                                        ** which should work for all platforms
                                        ** bug 60850
                                        */
                                        if ( (align = size % argp->OL_size) != 0 )
                                               align = argp->OL_size - align;
				}
				else if ( edv->ed_type == DB_FLT_TYPE )
				{
                                        /* Allow alignments to work for float4
                                        ** as well as float8 bug 69871
                                        */
                                        if ( (align = size % argp->OL_size) != 0 )
                                              align = argp->OL_size - align;
				}
				else if ( edv->ed_type == DB_DEC_TYPE )
				{
					/* no alignment for decimals */
					;
				}
				else
				{ /* edv->ed_type == DB_CHR_TYPE */
					if ( (align = size % sizeof(i2)) != 0 )
						align = sizeof(i2) - align;
				}
				/* special handling of string types */
				if ( edv->ed_type == DB_CHR_TYPE )
				{
				    argp->OL_value = (PTR)(data + size + align);
				    edv->ed_data = 
					&((OL_VSTR *)argp->OL_value)->string[0];
				    size += align + edv->ed_length;
			  	    size += 1 + sizeof(i2);	/* + EOS */
				}
				else
				{
				    edv->ed_data = data + size + align;
				    argp->OL_value = (PTR)edv->ed_data;
				    if (edv->ed_type == DB_DEC_TYPE)
				    {
					size += align + DB_PS_TO_LEN_MACRO(edv->ed_length);
				    }
				    else
				    {
					size += align + edv->ed_length;
				    }
				}
				if (dbv->db_datatype == DB_DMY_TYPE )
				{
				    /* assign a handle for the complex object.
				    */
				    i4 handle;
				    if (IIAG4aoAddObject(dbv->db_data, &handle)
					!= OK)
				    {
					/* assume mem alloc failure */
					abproerr(ERx("IIAG4aoAddObject"), 
							OUTOFMEM, (char*)NULL);
					/* NOTREACHED */
				    }
				    *(i4 *)edv->ed_data = handle;
				}
				else if (adh_dbcvtev( cb, dbv, edv) != E_DB_OK)
				{
					FEafeerr(cb);
				}
				/* one last special thing for strings: set
				** the length in the varchar structure for the
				** string.  For now, just use STlength, which
				** is equivalent to what OLpcall was doing.
				*/
				if ( edv->ed_type == DB_CHR_TYPE )
				{
				    ((OL_VSTR *)argp->OL_value)->slen = 
					STlength((char *)edv->ed_data);
				}
				++edv;
				++argp;
			} /* end for data allocation */
		}
		if (OLpcall(func, actuals, prm->pr_argcnt, proc->abrplang,
		            oltype,rp) != OK)
		{
			IIUGerr(E_AR0085_OLpcall_failed, 0, 1, proc->abrpname);
			if (NEWPARAM_MACRO(prm) && prm->pr_version >= 2)
			{ /* >= 6.0 */
				MEfree((PTR) actuals);
				MEfree((PTR) edvs);
				MEfree(data);
			}
			return FAIL;
		}
		if ( NEWPARAM_MACRO(prm) && prm->pr_version >= 2 )
		{ /* >= 6.0 */
			register DB_EMBEDDED_DATA	*edv;
			register ABRTSPV		*ap;
			register i4			cnt;
			/* Convert back any actuals passed by reference */
			edv = edvs;
			cnt = prm->pr_argcnt;
			for ( ap = prm->pr_actuals ; --cnt >= 0 ; ++ap )
			{
				DB_DATA_VALUE *dbv = 
						(DB_DATA_VALUE *)ap->abpvvalue;
				if ( ap->abpvtype == -DB_DBV_TYPE )
				{ /* a BYREF parameter */
					/* All strings returned by reference
					** will be EOS-terminated by 'OLpcall()'
					** use this length when converting the
					** actual parameter.  JupBug #6563.
					**
					** (OL should really set the actual size
					** passed back.)
					*/
					if ( edv->ed_type == DB_CHR_TYPE )
					{ /* get EOS terminated length string */
						edv->ed_length =
							STnlength(
							  (i4)edv->ed_length,
							  (char *)edv->ed_data
							);
					}
					if (dbv->db_datatype == DB_DMY_TYPE )
					{
					    /* it should be a compiler error
					    ** to pass complex objects byref.
					    ** If somehow it leaks through
					    ** here, let's not try to modify
					    ** the original complex object.
					    */
					    ;	/* do nothing */
					} 
					else if ( adh_evcvtdb(cb, edv, dbv)
							 != E_DB_OK )
					{
						FEafeerr(cb);
					}
				}
				++edv;
			} /* end for BYREF */
			MEfree((PTR) actuals);
			MEfree((PTR) edvs);
			MEfree(data);
		}
	}
	return OK;
}
/*
** Name:	getProc() -	Find Procedure Entry for Procedure.
**
** Description:
**	Get the runtime definition of a procedure from the procedure table.  The
**	procedure table must be set so the last procedure entry is followed by a
**	NULL name.
**
**	Note:  'iiarHash()' lowercases the name.
**
**Input:
**	name	{char *}  The name of the procedure to find.
**
** Returns:
**	{ABRTSPRO *}	A reference to the procedure structure for the procedure,
**			NULL if not found.
**
** Called by:
**	'abrtspcall()', 'abrtsnpcall()' and 'abrtsretcall()'.
**
** History:
**	Written 7/28/82 (jrc)
*/
static ABRTSPRO *
getProc ( name )
char	*name;
{
	i4	iiarHash();
	if ( ProcTable != NULL )
	{
		register ABRTSPRO	*proc;
		register i4		i;
		char				nbuf[ABBUFSIZE+1];
		STcopy( name, nbuf );
		name = nbuf;
		for ( i = ProcHash[iiarHash(name)] ; i != -1 ;
				i = proc->abrpnext )
		{
			proc = &(ProcTable[i]);
			if ( STequal( name, proc->abrpname ) )
				return proc;
		}
	}
	return NULL;
}
/*{
** Name:	IIARisProc() -	Is Named Object a Procedure?
**
** Description:
**	Given a name returns whether it is a procedure.  (That is, is found
**	in the procedure table for the application.)
**
** Input:
**	name	{char *}  The object name.
**
** Returns:
**	{bool}  TRUE if name is matches a procedure name.
**
** History:
**	03/90 (jhw) -- Written.
*/
bool
IIARisProc ( name )
char	*name;
{
	return (bool)( getProc(name) != NULL );
}
/*{
** Name:	IIARgglGetGlobal() -	Find Global Variable/Constant
**
** Description:
**	Get the runtime definition of an application variable or constant,
**	and return pointer to the DBV.
**
** Input:
**	name	{char *}  		The name of the global to find.
**
** Output:
**	gdbv	{DB_DATA_VALUE **} 	the global DB_DATA_VALUE *
**	kind	{nat *}			type of global: OC_CONST or OC_GLOBAL
**
** Returns:
**	{STATUS}
**		OK 	- found the global
**		FAIL	- didn't find the global	
**
** Side-effects:
**	Raises errors on the following conditions:
**		GLOBUNDEF	- global not defined
**		BADCLASS	- class not defined (if the global needed to be
**				  allocated)
**		OUTOFMEM	- memory allocation failure (if the global
**				  needed to be allocated)
**
** History:
**	21-dec-92 (davel)
**		Written. Split out iiarGetGlobal().
**      26-Oct-03 (zhahu02)
**      Updated to avoid SIGSEGV in iiinterp (b111188/INGCBT502) .
*/
STATUS
IIARgglGetGlobal ( char *name, DB_DATA_VALUE **gdbv, i4  *kind) 
{
	register ABRTSGL	*glob = NULL;
	register i4		i;
	char			nbuf[ABBUFSIZE+1];
	DB_DATA_VALUE		*dbv;
	i4	iiarHash();
	ADF_CB  *FEadfcb();
	if ( GlobTable == NULL )
		return FAIL;
	/* copy the name -- iiarHash lowercases it in place. */
	STlcopy( name, nbuf, sizeof(nbuf) );
	for ( i = GlobHash[iiarHash(nbuf)] ; i != -1 ; i = glob->abrgnext )
	{
		glob = &(GlobTable[i]);
		if ( STequal( nbuf, glob->abrgname ) )
			break;
	}
	if (glob == NULL || i == -1)
	{
		/* didn't find the global. */
		iiarUsrErr(GLOBUNDEF, 1, name);
		return FAIL;
	}
	/* found the DB_DATA_VALUE: set kind and initialize the db_data
	** if that hasn't ahppened yet for this global.
	*/
	if (kind != NULL)
		*kind = glob->abrgtype;
	dbv = *gdbv = &(glob->abrgdattype);
	if (dbv->db_data == NULL)
	{
		/* 
		** The global has not been initialized, so do it here.  This
		** only happens once-per-global in the whole application.
		*/
		if (dbv->db_datatype == DB_DMY_TYPE)
		{ /* Not a scalar type.  set up an object descriptor. */
			if (iiarCidInitDbdv(glob->abrgtname, dbv, TRUE) != OK)
			{
				iiarUsrErr(BADCLASS, 1, glob->abrgtname);
				return FAIL;
			}
		}
		else if (dbv->db_length > 0) 
		{ /* scalar type. allocate space and initialize. */
			if ( (dbv->db_data = _alloc(dbv->db_length)) == NULL )
			{
				abproerr( ERx("iiarGetGlobal"), OUTOFMEM, 
							(char*)NULL
				);
				return FAIL;
			}
			iiarInitDbv( FEadfcb(), dbv );
		}
	}
	return OK;
}
/*{
** Name:	IIARcglCopyGlobal() -	Verify & copy a global dbv to local one.
**
** Description:
**	Fill a local-variable dbv from a global dbv, if the types are
**	compatible.  This function is generally called after the global
**	dbv is fetched from a call to IIARgglGetGlobal(), when it is
**	necessary to fill a local dbv with the contents of a global dbv.
**	NOte that this isn't always needed: for example, when globals
**	are accessed from 3GL using the EXEC 4GL syntax, there is no
**	"local dbv" to verify; hence the global dbv returned from 
**	IIARgglGetGlobal() is all the 3GL access routines need.
**
**
** Input:
**	gdbv		{DB_DATA_VALUE *} the global dbv.
**	ldbv		{DB_DATA_VALUE *} the local dbv to verify.
**	globname	{char *}  The name of the global (for error msgs only).
**	localname	{char *}  The name of the local dbv's datatype.
**
** Outputs:
**	ldbv		{DB_DATA_VALUE *} db_data, db_length, and db_prec set.
**
** Returns:
**	{STATUS}
**		OK 	- local dbv filled
**		FAIL	- type mismatch between the local and global dbv's
**
** Side-effects:
**	Raises errors on the following conditions:
**		GLOBBADTYPE	- type mismatch between the 2 dbv's
**
** History:
**	21-dec-92 (davel)
**		Written. Split out iiarGetGlobal().
*/
STATUS
IIARcglCopyGlobal ( DB_DATA_VALUE *gdbv, DB_DATA_VALUE *ldbv, char *globname, 
			char *localname )
{
	/* now make sure we have a match. ignore "array of " for global array's
	** type name since the length check will catch any mis-matches between
	** a record and an array of the same record.  constants will have
	** length zero and should not have their lengths checked.
	*/
	if ( ldbv->db_datatype != gdbv->db_datatype
	  || ( ldbv->db_length != 0 && ldbv->db_length != gdbv->db_length )
	  || ldbv->db_datatype == DB_DMY_TYPE )
	{
		bool		err;
		char		*gtype;
		AB_TYPENAME	ltn;	/* local dbv type name */
		AB_TYPENAME	gtn;	/* global dbv type name */
		/* get the type name if one wasn't passed in. */
		if ( localname == NULL ||  *localname == EOS )
			localname = iiarTypeName(ldbv, ltn);
		gtype = iiarTypeName(gdbv, gtn); /* get global type name */
		if ( ldbv->db_datatype != gdbv->db_datatype
				|| ldbv->db_length != gdbv->db_length )
		{
			err = TRUE;
		}
		else
		{
			char	*cp;
			/* assert:  both types are DB_DMY_TYPE */
			if ( gdbv->db_length == 1
				&& (cp = STrchr(gtype, ' ')) != NULL )
			{ /* ignore "array of " . . . */
				cp += 1;	/* skip space */
			}
			else
			{
				cp = gtype;
			}
			err = (bool)(STbcompare(localname,0, cp,0, FALSE) != 0);
		}
		if ( err )
		{
			iiarUsrErr( GLOBBADTYPE, 3, globname, localname, gtn );
			return FAIL;
		}
	}
	ldbv->db_data = gdbv->db_data;
	ldbv->db_length = gdbv->db_length;
	ldbv->db_prec = gdbv->db_prec;
	return OK;
}