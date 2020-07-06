# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<adf.h>
# include	<fmt.h>
# include	<cm.h>
# include	<me.h>
# include	<ug.h>
# include	<ui.h>
# include	<qr.h>
# include	<er.h>
# include <uigdata.h>
# include	"erqr.h"
# include	"qrhelp.h"
/**
** Name:	qrtxthlp.qsc -	process help command for mostly textual output.
**
** Description:
**	Help command routines for those objects for which most of the
**	information is stored as text in the system catalogs.
**	Print some sort of object-specific header, then format information
**	stored in the database out to the screen.  The routines are:
**
**	qrviewhelp		retrieve view info
**	qrintghelp		retrieve intregrity info
**	qrpermhelp		retrieve permit info
**	IIQRcnh_ConstraintHelp	retrieve constraint info
**	IIQRdfh_DefaultHelp	retrieve table defaults info
**	IIQRprh_ProcedureHelp	retrieve procedure info
**	IIQRlkh_LinkHelp	retrieve registered object info
**	IIQRrlh_RuleHelp	retrieve rule object info
**	IIQRsah_SecAlarmHelp	retrieve security alarm info
**	IIQRseh_SequenceHelp	retrieve sequence info
**	IIQRsyh_SynonymHelp	retrieve synonym info
**	IIQRcth_CommTblHelp	retrieve comment table info
**	IIQRcch_CommColHelp	retrieve comment column info
**
**	Internal routines:
**	inittext		initialize the output/format buffer
**	addtext			add currently retrieved text to buffer
**	cleartext		call qrfmttext to dump the buffer
**	qrfmttext		print text info from the backend
**	qrprttype		print description of type for register command
**
** History:
**	28-sep-88 (bruceb)
**		File created.  Code from qrviphlp.qsc copied here, then
**		modified.  Help for procedures and links added.
**	03-oct-88 (bruceb)
**		Use dynamically sized buffer for view help.  This buffer may
**		be implemented with non-standard text width on gateways
**		(the other textual help types are only available on INGRES.)
**	05-oct-88 (bruceb)
**		Made type of the help routines be VOID since no one ever
**		looks at the return values.
**	19-oct-88 (bruceb)
**		Only show text of a procedure if owned by the user.  If owned
**		by someone else (the DBA) then merely state existence.
**	31-oct-88 (bruceb)
**		Look for registered objects, not just links.  Still pertains
**		just to tables, views and indexes.
**	03-mar-89 (kathryn) 	Bug #4487
**		Changed calls FErelexists to IIQR_relexists.
**	07-apr-89 (teresal)
**		Modifed the 'help register' command to display more info
**		for STAR.  Also, fixed bug for gateways where database objects 
**		'iiviews' and 'text_segment' were not being uppercased for an
**		uppercase DBMS.
**	27-apr-89 (teresal)
**		Replaced obsolete catalog iiingres_tables with iitables.
**	18-may-89 (teresal)
**		Added help rule.
**	28-jul-89 (marian)
**		Added arg (FALSE) to fmt_multi call because of fmt changes.
**	13-nov-89 (teresal)
**		Added help security_alarm.
**	29-dec-89 (teresal)
**		Add help synonym, help comment table and help comment column.
**	30-mar-90 (teresal)
**		Removed short_remark from help comment display because it
**		is not going to be supported in 6.4 release.
**      3/21/91 (elein)         (b35574) Add FALSE parameter to call to
**                              fmt_multi. TRUE is used internally for boxed
**                              messages only.  They need to have control
**                              characters suppressed.
**	22-mar-91 (kathryn)
**		Added function: IIQRphp_Permit_Help().
**      26-sep-91 (jillb/jroy--DGC) (from 6.3)
**            Changed fmt_init to IIfmt_init since it is a fairly global
**            routine.  The old name, fmt_init, conflicts with a Green Hills
**            Fortran function name.
**      26-jun-92 (rogerl)
**		Added field to contain the 'Type:' of procedures and rules.
**		The code providing a value is commented out (DBMS support
**		not available now), and marked with @COMMENTED@.  Until
**		this is uncommented, we'll always display 'user object'.
**		Took out checks for DBA/current owner - we want info if we
**		get to here.
**	9-dec-1992 (rogerl)
**		Change qrprintf's involving object/owner names to qrdiprintf()
**		Which will ensure display of a delimited ident is quoted OK.
**      07-feb-92 (rogerl)
**		Allocate buffer in addtext() with MEreqlng(), since buffers
**		may be big.  Don't bother 0'ing.
**	11-feb-1993 (rogerl)
**		Uncommented COMMENTED sections now that DBMS support exists.
**	24-may-1993 (rogerl)
**		If rev is <65, support old 'help permit <object>' as usual;
**		if >=65, ditto except don't check relstat value  - if the
**		name and the owner are OK, get anything available. (qrpermhelp)
**	24-may-1993 (rogerl)
**		Yow.  Still need to do some relstat bits checking;
**		'yow' may exist in iipermits, for example, as a table as
**		well as a procedure.  Limits usefulness to views and tables.
**	10-dec-93 (robf)
**              Add support for HELP SECURITY_ALARM ON DATABASE|
**					CURRENT INSTALLATION
**	11-apr-1994 (timt) bug 58802
** 		For HELP CONSTRAINT & HELP DEFAULT, change qrprintf's
**		involving owner/table/column names to qrdiprintf().
**		This will ensure display of a delimited ident is quoted OK.
**		Search for b58802 to see all related changes.
**      2-mar-1995 (inkdo01)
**              Fix 1st arg to qrdiprintf call for QR00A3 (secalarm stuff)
**	24-jul-1996 (chech02)
**		Fix "%d" arguments to qrprintf and STprintf call. 
**      09-jul-97 (i4jo01)
**              Changed routine used for case conversion in qrviewhelp. 
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      29-Nov-1999 (hanch04)
**          First stage of updating ME calls to use OS memory management.
**          Make sure me.h is included.  Use SIZE_TYPE for lengths.
**	05-apr-2002 (somsa01)
**	    In qrfmttext(), when dealing with DB_TEXT_STRING, one must note
**	    that the db_t_text structure member will NOT be null-terminated.
**	    Therefore, this should NOT be passed to functions which require
**	    arguments to be null-terminated (such as STprintf()). Otherwise,
**	    a SEGV will result.
**	17-apr-02 (inkdo01)
**	    Added 'help sequence'.
**	02-feb-07 (upake01)
**	    Modified to use SYSTEM_CAT_PREFIX instead of &SystemCatPrefix
**          because the latter requires global visibility.
*/
/* # define's */
# define	MSGWIDTH	70
/* size of text_segment column in the standard catalogs */
# define	TEXTSEGWIDTH	240
/* size of short_remark and long_remark columns in the comments catalogs */
# define	SREMARKWIDTH	60
# define	LREMARKWIDTH	1600
/* arbitrary initial size for text of single views, permits, ... */
# define	INITBUFSIZE	1000
/* GLOBALDEF's */
/* extern's */
FUNC_EXTERN	ADF_CB	*FEadfcb();
FUNC_EXTERN	VOID	qrdiprintf( u_i4 nth, QRB *qrb, char *fmt,
			... );
/* static's */
static	FMT		*IIhelpfmt = NULL;
static	DB_DATA_VALUE	IIhelpdbv = {0};
static	char		*text = NULL;
static	i4		textbufsiz = INITBUFSIZE;
static	STATUS		qrfmttext();
static	char		*addtext();
static	VOID		inittext(), cleartext(), qrprttype();
  static char *view_buf = NULL;
/*{
** Name:	qrviewhelp	- get info from iiviews.
**
** Description:
**	Performs the actual retrieval from iiviews catalog of the
**	definition of the view. Prints to the screen via qrfmttext,
**	which places the output in the qrb's output buffer.
**
** Inputs:
**	qrb		the QRB
**	v		the view name
**	vowner		the view's owner
**
** Outputs:
**	none
**
**	Returns:
**		None
**
** Side Effects:
**	eventually fills the qrb's output buffer with the view definition.
**
** History:
**	17-aug-87 (daver)	First written.
**	14-apr-89 (teresal)	Fixed 'iiviews' and 'text_segment' to 
**				be the appropriate case.
**	10-may-89 (kathryn) - Bug #5382
**		Changed select to repeated select.
**	09-jul-97 (i4jo01) 
**		Changed routine used for case conversion of 'iiviews'
**		and 'text_segment'. Use it since we can indicate that
**		these are regular non-delimited ids. (b75223).
*/
VOID
qrviewhelp(qrb, v, vowner)
QRB	*qrb;
  char *v;
  char *vowner;
{
    bool	are_some = FALSE;
  i4 seq;
  char *retbuf;
  char chkopt[9];
  i4 rows;
  i4 vwidth;
  char views[8];
  char textseg[13];
    char	*checkopt_value = NULL;
    bool	bad_add = FALSE;
    if (!view_buf)
    {
	/*
	** For views (only), determine the width of the text_segment column
	** in iiviews.  This is currently the only textual standard catalog
	** that is not INGRES-only, and may be of a size other than 240
	** chars wide.
	*/
	STcopy(ERx("iiviews"),views);	/* lowercase so MIXED case is correct */
	STcopy(ERx("text_segment"),textseg);
	IIUGdbo_dlmBEobject(views,FALSE);	/* Adjust case for DBMS  */
	IIUGdbo_dlmBEobject(textseg,FALSE);
	/* bug #5382 - Changed to Repeated select */
/* # line 247 "qrtxthlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtxthlp1",7484,6908);
      IIputdomio((short *)0,1,32,0,views);
      IIputdomio((short *)0,1,32,0,textseg);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtxthlp1",7484,6908);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select column_length from iicolumns where table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,views);
        IIwritio(0,(short *)0,1,32,0,(char *)"and column_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,textseg);
        IIexDefine(0,(char *)"qrtxthlp1",7484,6908);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE1;
IIrtB1:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,30,4,&vwidth);
      if (IIerrtest() != 0) goto IIrtB1;
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE1:;
  }
/* # line 254 "qrtxthlp.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&rows,8);
  }
/* # line 255 "qrtxthlp.sc" */	/* host code */
	if (FEinqerr() || (rows == 0))
	{
	    qrprintf(qrb, ERget(F_QR0040_ret_err_iicolumns));
	    return;
	}
	if ((view_buf = (char *)MEreqmem((u_i4)0, (u_i4)(vwidth + 1), TRUE,
	    (STATUS *)NULL)) == NULL)
	{
	    qrprintf(qrb, ERget(E_QR000F_Error_allocing_mem));
	    return;
	}
    }
    inittext(qrb);
/* # line 268 "qrtxthlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtxthlp2",7485,6907);
      IIputdomio((short *)0,1,32,0,v);
      IIputdomio((short *)0,1,32,0,vowner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtxthlp2",7485,6907);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select text_segment, text_sequence, check_option from iiviews where t\
able_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,v);
        IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,vowner);
        IIwritio(0,(short *)0,1,32,0,(char *)"order by text_sequence");
        IIexDefine(0,(char *)"qrtxthlp2",7485,6907);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE2;
IIrtB2:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,0,view_buf);
      IIgetdomio((short *)0,1,30,4,&seq);
      IIgetdomio((short *)0,1,32,8,chkopt);
      if (IIerrtest() != 0) goto IIrtB2;
/* # line 275 "qrtxthlp.sc" */	/* host code */
	if (!are_some)
	{
	    qrdiprintf( QRARG1, qrb, ERget(F_QR000B_View_def_heading), v );
	    qrdiprintf( QRARG1, qrb, ERget(F_QR0013_Owner), vowner );
	    if (chkopt[0] == 'Y')
	    {
		checkopt_value = ERget(F_QR0076_on);
	    }
	    else if (chkopt[0] == 'N')
	    {
		checkopt_value = ERget(F_QR0077_off);
	    }
	    if (checkopt_value != NULL)
	    {
		qrprintf(qrb, ERget(F_QR0075_Check_Option), checkopt_value);
	    }
	    qrprintf(qrb, ERget(F_QR0078_View_Def));
	    are_some = TRUE;
	}
	if (addtext(qrb, view_buf) == NULL)
	{
	    bad_add = TRUE;
/* # line 297 "qrtxthlp.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE2;
      }
/* # line 298 "qrtxthlp.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE2:;
  }
    if (FEinqerr())
    {
	qrprintf(qrb, ERget(F_QR000A_Err_ret_iiview));
    }
    else if (!are_some)
    {
	/* Should never happen--should be caught in IIQRmdh */
	qrdiprintf( QRARG1, qrb, ERget(F_QR000C_Table_not_a_view), v );
    }
    else if (!bad_add)
    {
	cleartext(qrb);
    }
}
/*{
** Name:	qrintghelp	- get info from iiintegrity.
**
** Description:
**	Performs the actual retrieval from the iiintegrities catalog of the
**	definition of the integrity. Prints to the screen via qrfmttext,
**	which places the output in the qrb's output buffer.
**
** Inputs:
**	qrb		the QRB
**	i		the integrity name
**	iowner		the integrity's owner
**
** Outputs:
**	none
**
**	Returns:
**		None
**
** Side Effects:
**	eventually fills the qrb's output buffer with the integrity definition.
**
** History:
**	17-aug-87 (daver)	First written.
*/
VOID
qrintghelp(qrb, i, iowner)
QRB	*qrb;
  char *i;
  char *iowner;
{
    bool	are_some = FALSE;
  char itext[TEXTSEGWIDTH + 1];
  i4 inum;
  i4 iseq;
    bool	bad_add = FALSE;
    inittext(qrb);
/* # line 355 "qrtxthlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtxthlp3",7486,28090);
      IIputdomio((short *)0,1,32,0,i);
      IIputdomio((short *)0,1,32,0,iowner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtxthlp3",7486,28090);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select text_segment, integrity_number, text_sequence from iiintegriti\
es where table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,i);
        IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,iowner);
        IIwritio(0,(short *)0,1,32,0,(char *)
"order by integrity_number, text_sequence");
        IIexDefine(0,(char *)"qrtxthlp3",7486,28090);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE3;
IIrtB3:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,TEXTSEGWIDTH + 1-1,itext);
      IIgetdomio((short *)0,1,30,4,&inum);
      IIgetdomio((short *)0,1,30,4,&iseq);
      if (IIerrtest() != 0) goto IIrtB3;
/* # line 362 "qrtxthlp.sc" */	/* host code */
	/* only if its the first row, print the integrity header */
	if (!are_some)
	{
	    qrdiprintf( QRARG1, qrb, ERget(F_QR0048_Integrity_header), i );
	    are_some = TRUE;
	}
	/* if iseq > 1, then it's the next part of a long integrity */
	if (iseq <= 1)
	{
	    /* output any previous integrity text */
	    cleartext(qrb);
	    /* next integrity */
	    qrprintf(qrb, ERget(F_QR0049_Integ_Constraint_num), inum);
	}
	if (addtext(qrb, itext) == NULL)
	{
	    bad_add = TRUE;
/* # line 379 "qrtxthlp.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE3;
      }
/* # line 380 "qrtxthlp.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE3:;
  }
    if (FEinqerr())
    {
	qrprintf(qrb, ERget(F_QR004A_Err_ret_iiintegrities));
    }
    else if (!are_some)
    {
	qrdiprintf( QRARG1, qrb, ERget(F_QR004B_Table_has_no_integs), i);
    }
    else if (!bad_add)
    {
	cleartext(qrb);
    }
}
/*{
** Name:	qrpermhelp	- get info from iipermits
**
** Description:
**	Performs the actual retrieval from the iipermits catalog for the
**	definition of the permit. Prints to the screen via qrfmttext,
**	which places the output in the qrb's output buffer.
**
** Inputs:
**	qrb		the QRB
**	p		the permit name
**	powner		the permit's owner
**
** Outputs:
**	none
**
**	Returns:
**		None
**
** Side Effects:
**	eventually fills the qrb's output buffer with the permit definition.
**
** History:
**	17-aug-87 (daver)	First written.
**	26-jul-93 (rogerl)	Don't check all_to_all or ret_to_all for >=6.5
**				for 'help permit <object>' and help permit on
**				table <name>'
*/
VOID
qrpermhelp(qrb, p, powner)
QRB	*qrb;
  char *p;
  char *powner;
{
  char ptext[TEXTSEGWIDTH + 1];
  char allto[2];
  char retto[2];
  i4 pnum;
  i4 pseq;
    bool	headprinted = FALSE;	/* if TRUE, there are some permits */
    char	*t = NULL;
    bool	bad_add = FALSE;
    inittext(qrb);
	/* if < 6.5, check the special all_to_all and ret_to_all cols */
    if  ( STcompare( IIUIscl_StdCatLevel(), UI_LEVEL_65 ) < 0 )
    {
/* # line 445 "qrtxthlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtxthlp4",7487,25018);
      IIputdomio((short *)0,1,32,0,p);
      IIputdomio((short *)0,1,32,0,powner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtxthlp4",7487,25018);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select all_to_all, ret_to_all from iitables where table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,p);
        IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,powner);
        IIexDefine(0,(char *)"qrtxthlp4",7487,25018);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE4;
IIrtB4:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,1,allto);
      IIgetdomio((short *)0,1,32,1,retto);
      if (IIerrtest() != 0) goto IIrtB4;
/* # line 451 "qrtxthlp.sc" */	/* host code */
	if ( (allto[0] == 'Y') || (retto[0] == 'Y') )
	{
	    qrdiprintf( QRARG1, qrb, ERget(F_QR004C_Permissions_header), p );
	    headprinted = TRUE;
	    if (allto[0] == 'Y')
	    {
		qrprintf(qrb, ERget(F_QR004E_Permission_header), (i4)0);
		if (qrb->lang == DB_SQL)
		{
		    STprintf(ptext, ERget(F_QR0067_def_permit_alltoall), p);
		}
		else
		{
		    STprintf(ptext, ERget(F_QR0051_def_permit_alltoall), p);
		}
		qrfmttext(qrb, ptext);
	    }
	    if (retto[0] == 'Y')
	    {
		qrprintf(qrb, ERget(F_QR004E_Permission_header), (i4)1);
		if (qrb->lang == DB_SQL)
		{
		    STprintf(ptext, ERget(F_QR0068_def_permit_rettoall), p);
		}
		else
		{
		    STprintf(ptext, ERget(F_QR0052_def_permit_rettoall), p);
		}
		qrfmttext(qrb, ptext);
	    }
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE4:;
  }
    }   /* end if all-to-all or ret-to-all special permits */
    if  (STcompare(IIUIscl_StdCatLevel(),UI_LEVEL_65) >= 0)
    {
/* # line 486 "qrtxthlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtxthlp5",7488,25274);
      IIputdomio((short *)0,1,32,0,p);
      IIputdomio((short *)0,1,32,0,powner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtxthlp5",7488,25274);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select text_segment, permit_number, text_sequence from iipermits wher\
e object_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,p);
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,powner);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and(object_type='T' or object_type='V')order by permit_number, text_s\
equence");
        IIexDefine(0,(char *)"qrtxthlp5",7488,25274);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE5;
IIrtB5:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,TEXTSEGWIDTH + 1-1,ptext);
      IIgetdomio((short *)0,1,30,4,&pnum);
      IIgetdomio((short *)0,1,30,4,&pseq);
      if (IIerrtest() != 0) goto IIrtB5;
/* # line 496 "qrtxthlp.sc" */	/* host code */
	/* only if it's the first row, print the permissions header */
	if (!headprinted)
	{
	    qrdiprintf( QRARG1, qrb, ERget(F_QR004C_Permissions_header), p);
	    headprinted = TRUE;
	}
	/* if pseq > 1, then it's in the middle of a big permit definition */
	if (pseq <= 1)
	{
	    /* output any previous permit definition */
	    cleartext(qrb);
	    /* next permit */
	    qrprintf(qrb, ERget(F_QR004E_Permission_header), pnum);
	}
	if (addtext(qrb, ptext) == NULL)
	{
	    bad_add = TRUE;
/* # line 513 "qrtxthlp.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE5;
      }
/* # line 514 "qrtxthlp.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE5:;
  }
    }
    else
    {
/* # line 519 "qrtxthlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtxthlp6",7489,25273);
      IIputdomio((short *)0,1,32,0,p);
      IIputdomio((short *)0,1,32,0,powner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtxthlp6",7489,25273);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select text_segment, permit_number, text_sequence from iipermits wher\
e object_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,p);
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,powner);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and object_type='T' order by permit_number, text_sequence");
        IIexDefine(0,(char *)"qrtxthlp6",7489,25273);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE6;
IIrtB6:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,TEXTSEGWIDTH + 1-1,ptext);
      IIgetdomio((short *)0,1,30,4,&pnum);
      IIgetdomio((short *)0,1,30,4,&pseq);
      if (IIerrtest() != 0) goto IIrtB6;
/* # line 527 "qrtxthlp.sc" */	/* host code */
	/* only if it's the first row, print the permissions header */
	if (!headprinted)
	{
	    qrdiprintf( QRARG1, qrb, ERget(F_QR004C_Permissions_header), p);
	    headprinted = TRUE;
	}
	/* if pseq > 1, then it's in the middle of a big permit definition */
	if (pseq <= 1)
	{
	    /* output any previous permit definition */
	    cleartext(qrb);
	    /* next permit */
	    qrprintf(qrb, ERget(F_QR004E_Permission_header), pnum);
	}
	if (addtext(qrb, ptext) == NULL)
	{
	    bad_add = TRUE;
/* # line 544 "qrtxthlp.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE6;
      }
/* # line 545 "qrtxthlp.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE6:;
  }
    }
    if (FEinqerr())
    {
	qrprintf(qrb, ERget(F_QR004D_Error_retrieving_data));
    }
    else if (!headprinted)
    {
	 qrdiprintf( QRARG1, qrb, ERget(F_QR004F_Table_has_no_permits), p );
    }
    else if (!bad_add)
    {
	cleartext(qrb);
    }
}
/*{
** Name:	IIQRcnh_ConstraintHelp	- get info from iiintegrity.
**
** Description:
**	Performs the actual retrieval from the iiconstraints catalog of the
**	definition of the constraints defined for the given table. Prints 
**	to the screen via qrfmttext, which places the output in the 
**	qrb's output buffer.
**
** Inputs:
**	qrb		the QRB
**	ctable		the table name
**	ctab_owner	the table owner
**
** Outputs:
**	none
**
** Returns:
**	None
**
** Side Effects:
**	eventually fills the qrb's output buffer with the constraints
**	defined for this table
**
** History:
**	17-aug-87 (daver)	First written.
*/
VOID
IIQRcnh_ConstraintHelp(qrb, ctable, ctab_owner)
QRB	*qrb;
  char *ctable;
  char *ctab_owner;
{
  char cname[DB_MAXNAME +1];
  char ctext[TEXTSEGWIDTH + 1];
  i4 cseq;
    bool	printed_header = FALSE;
    bool	bad_add = FALSE;
    inittext(qrb);
/* # line 604 "qrtxthlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtxthlp7",7490,25782);
      IIputdomio((short *)0,1,32,0,ctable);
      IIputdomio((short *)0,1,32,0,ctab_owner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtxthlp7",7490,25782);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select constraint_name, text_segment, text_sequence from iiconstraint\
s where table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,ctable);
        IIwritio(0,(short *)0,1,32,0,(char *)"and schema_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,ctab_owner);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and system_use='U' order by constraint_name, text_sequence");
        IIexDefine(0,(char *)"qrtxthlp7",7490,25782);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE7;
IIrtB7:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME +1-1,cname);
      IIgetdomio((short *)0,1,32,TEXTSEGWIDTH + 1-1,ctext);
      IIgetdomio((short *)0,1,30,4,&cseq);
      if (IIerrtest() != 0) goto IIrtB7;
/* # line 612 "qrtxthlp.sc" */	/* host code */
	/* only if its the first row, print the constraint header */
	if (!printed_header)
	{
	    /* change qrprintf to qrdiprintf         	 b58802 */
	    qrdiprintf( QRARG1 | QRARG2, qrb,
	        ERx("Contraints defined for %s.%s are:\n"), 
		ctab_owner, ctable);
	    printed_header = TRUE;
	}
	/* if cseq > 1, then it's the next part of a long integrity */
	if (cseq <= 1)
	{
	    /* output any previous integrity text */
	    cleartext(qrb);
	    /* next integrity */
	    /* change qrprintf to qrdiprintf         	 b58802 */
	    qrdiprintf( QRARG1, qrb, ERx("Constraint Name: %s\n"), cname);
	}
	if (addtext(qrb, ctext) == NULL)
	{
	    bad_add = TRUE;
/* # line 633 "qrtxthlp.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE7;
      }
/* # line 634 "qrtxthlp.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE7:;
  }
    if (FEinqerr())
    {
	qrprintf(qrb, ERx("Error retrieving data from iiconstraints\n"));
    }
    else if (!printed_header)
    /* if we haven't printed the header, we don't have any constraints! */
    {
	/* change qrprintf to qrdiprintf         	 b58802 */
	qrdiprintf( QRARG1 | QRARG2, qrb,
	    ERx("Table %s.%s has no constraints defined for it\n"),
	    ctab_owner, ctable);
    }
    else if (!bad_add)
    {
	cleartext(qrb);
    }
}
/*{
** Name:	IIQRdfh_DefaultHelp	- get info from iidefault
**
** Descriptioniidefault
**	Retrieves information from iicolumns about the user-defined
**	defaults for the given table. Prints to the screen via qrfmttext,
**	which places the output in the qrb's output buffer.
**
** Inputs:
**	qrb		the QRB
**	dtable		table name
**	dtab_owner	table owner
**
** Outputs:
**	none
**
** Returns:
**	None
**
**
** Side Effects:
**	Fills up the qrb with information on those user-defined defaults
**	for this table.
**
**
** History:
**	17-aug-87 (daver)	First written.
*/
VOID
IIQRdfh_DefaultHelp(qrb, dtable, dtab_owner)
QRB	*qrb;
  char *dtable;
  char *dtab_owner;
{
  char dcol_name[DB_MAXNAME +1];
  char dvalue[1500 + 1];
  int dseq;
  char dcol_hasd[2];
  char dcol_nulls[2];
  i2 nulli;
    bool	has_defaults_yet = FALSE;
    bool	bad_add = FALSE;
    char	def_type[11];
    inittext(qrb);
    /* 
    ** With each column retrieved, determine the type of the default via
    ** IIUIgdt_GetDefaultType. IF the return of this routine indicates 
    ** its a user-defined default ("value"), then go ahead and print the
    ** column name and the default. Otherwise, its just another vanilla
    ** default, and we skip it.
    **
    ** NEED to update this query once the DBMS has the left(20,thang) 
    ** feature once working, which will only require a buffer for the
    ** most interesting 20 or so bytes rather than 1501 to hold the 
    ** whole default!
    */
/* # line 713 "qrtxthlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtxthlp8",7491,22709);
      IIputdomio((short *)0,1,32,0,dtable);
      IIputdomio((short *)0,1,32,0,dtab_owner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtxthlp8",7491,22709);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select column_name, column_sequence, column_default_val, column_has_d\
efault, column_nulls from iicolumns where table_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,dtable);
        IIwritio(0,(short *)0,1,32,0,(char *)"and table_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,dtab_owner);
        IIwritio(0,(short *)0,1,32,0,(char *)"order by column_sequence");
        IIexDefine(0,(char *)"qrtxthlp8",7491,22709);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE8;
IIrtB8:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME +1-1,dcol_name);
      IIgetdomio((short *)0,1,30,4,&dseq);
      IIgetdomio(&nulli,1,32,1500 + 1-1,dvalue);
      IIgetdomio((short *)0,1,32,1,dcol_hasd);
      IIgetdomio((short *)0,1,32,1,dcol_nulls);
      if (IIerrtest() != 0) goto IIrtB8;
/* # line 722 "qrtxthlp.sc" */	/* host code */
	IIUIgdt_GetDefaultType(dvalue, nulli, dcol_hasd, dcol_nulls, def_type);
	if (STscompare(def_type, 0, "value", 0) == 0)
	{
	    /* only if its the first row, print the constraint header */
	    if (!has_defaults_yet)
	    {
	        /* change qrprintf to qrdiprintf       	 b58802 */
		qrdiprintf( QRARG1 | QRARG2, qrb, 
		    ERx("User defined Defaults for %s.%s are:\n"),
		    dtab_owner, dtable);
		has_defaults_yet = TRUE;
	    }
	    /* output any previous integrity text */
	    cleartext(qrb);
	    /* next integrity */
	    /* change qrprintf to qrdiprintf         	 b58802 */
	    qrdiprintf( QRARG1, qrb, ERx("Column Name: %s\n"), dcol_name);
	    if (addtext(qrb, dvalue) == NULL)
	    {
		bad_add = TRUE;
/* # line 742 "qrtxthlp.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE8;
      }
/* # line 743 "qrtxthlp.sc" */	/* host code */
	    }
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE8:;
  }
	if (FEinqerr())
	{
	    qrprintf(qrb, ERx("Error retrieving data from iiconstraints\n"));
	}
	else if (!has_defaults_yet)
	/* if we haven't printed the header, we don't have any constraints! */
	{
	    /* change qrprintf to qrdiprintf         	 b58802 */
	    qrdiprintf( QRARG1 | QRARG2, qrb, 
		ERx("Table %s.%s has no user defined defaults \n"),
	        dtab_owner, dtable);
	}
	else if (!bad_add)
	{
	    cleartext(qrb);
	}
}
/*{
** Name:	IIQRprh_ProcedureHelp	- get info from iiprocedures.
**
** Description:
**	Performs the actual retrieval from iiprocedures catalog of the
**	definition of the procedure. Prints to the screen via qrfmttext,
**	which places the output in the qrb's output buffer.
**
** Inputs:
**	qrb		the QRB
**	prname		the procedure name
**	prowner		the procedure's owner
**
** Outputs:
**	none
**
**	Returns:
**		None
**
** Side Effects:
**	eventually fills the qrb's output buffer with the procedure definition.
**
** History:
**	28-sep-88 (bruceb)	First written.
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
**	24-jun-1992 (rogerl)
**	    Support for displaying help about procedure 'type' as pertains
**	    to procedures created by the system to support integritities
**	    constraints having 'system' type where user-procedures are that.
**	    this necessitates the deletion of the check to only show the
**	    procedures owned by the owner of this db session, and a similar
**	    permits check.  Add 'prtype' to extract the user/system
**	    type into.
**	01-sep-06 (toumi01) SIR 116145
**	    For "help procedure" print out user dbprocs with line numbers
**	    and one statement per line, to make it easier to locate error
**	    lines when debugging.
*/
VOID
IIQRprh_ProcedureHelp(qrb, prname, prowner)
QRB	*qrb;
  char *prname;
  char *prowner;
{
    u_char	*next_text;
    u_char	*next_stmt;
    bool	are_some = FALSE;
    bool	bad_add = FALSE;
    bool	new_cat;
    bool	prettyprint = TRUE;
    i4		stmtcnt = 1;
    i4		quotecnt;
  i4 prseq;
  i4 prnum;
  char prcdate[26];
  char prtype[9];
  char pruse[9];
  u_char prtext[TEXTSEGWIDTH + 1];
    u_char	prstmt[TEXTSEGWIDTH + 1 + sizeof("/* 0000 */ ")];
    /*
    ** Determine the catalog level.
    */
    if (STcompare(IIUIscl_StdCatLevel(), UI_LEVEL_65) >= 0)
	new_cat = TRUE;
    else
	new_cat = FALSE;
    inittext(qrb);
    /*
    ** prepare to pretty print first statement
    */
    STprintf(prstmt,"/* %4d */ ",stmtcnt);
    next_stmt = prstmt+sizeof("/* 0000 */ ")-1;
    quotecnt = 0;
    if (new_cat == TRUE)
    {
/* # line 844 "qrtxthlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtxthlp9",7492,12214);
      IIputdomio((short *)0,1,32,0,prname);
      IIputdomio((short *)0,1,32,0,prowner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtxthlp9",7492,12214);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select text_segment, text_sequence, create_date, proc_subtype, system\
_use from iiprocedures where procedure_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,prname);
        IIwritio(0,(short *)0,1,32,0,(char *)"and procedure_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,prowner);
        IIwritio(0,(short *)0,1,32,0,(char *)"order by text_sequence");
        IIexDefine(0,(char *)"qrtxthlp9",7492,12214);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE9;
IIrtB9:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,TEXTSEGWIDTH + 1-1,prtext);
      IIgetdomio((short *)0,1,30,4,&prseq);
      IIgetdomio((short *)0,1,32,25,prcdate);
      IIgetdomio((short *)0,1,32,8,prtype);
      IIgetdomio((short *)0,1,32,8,pruse);
      if (IIerrtest() != 0) goto IIrtB9;
/* # line 852 "qrtxthlp.sc" */	/* host code */
	if (!are_some)
	{
	    qrdiprintf( QRARG1, qrb, ERget(F_QR0079_Proc_def_heading), prname );
	    qrdiprintf( QRARG1, qrb, ERget(F_QR0013_Owner), prowner );
	    /*
	    ** Print procedure type
	    */
	    switch ( prtype[0] )
	    {
		case 'N':
		    qrprintf(qrb, ERget(F_QR0115_Proc_type), 
				ERget(F_QR0118_Native_proc));
		    break;
		case 'I':
		    qrprintf(qrb, ERget(F_QR0115_Proc_type), 
				ERget(F_QR0119_Imported_proc));
		    break;
		case 'E':
		    qrprintf(qrb, ERget(F_QR0115_Proc_type), 
				ERget(F_QR0120_Enterprise_proc));
		    break;
		case 'L':
		    qrprintf(qrb, ERget(F_QR0115_Proc_type), 
				ERget(F_QR0121_Star_proc));
		    break;
		default:
		    qrprintf(qrb, ERget(F_QR0115_Proc_type), 
				ERget(F_QR0005_unknown));
		    break;
	    }
	    /* 
	    ** print 'user' or 'system' object depending on catalog 
	    ** level and pruse (system_use)
	    */
	    switch (pruse[0])
	    {
		case 'G':
		    qrprintf(qrb, ERget( F_QR0122_Obj_type ), 
					ERget( F_QR0116_System_object ));
		    break;
		case 'U':
		    qrprintf(qrb, ERget( F_QR0122_Obj_type ), 
					ERget( F_QR0117_User_object ));
		    break;
		default:
		    qrprintf(qrb, ERget(F_QR0122_Obj_type), 
					ERget(F_QR0005_unknown));
		    break;
	    }
	    if (prcdate[0] != ' ')
	    {
		if (IIQRgtl_GmtToLocal(prcdate, prcdate) == OK)
		    qrprintf(qrb, ERget(F_QR0070_Created), prcdate);
	    }
	    qrprintf(qrb, ERget(F_QR007A_Proc_Def));
	    are_some = TRUE;
	}
	/*
	** don't pretty print system procedures
	*/
	if ((!CMcmpnocase(&prname[0], SYSTEM_CAT_PREFIX) &&
	     !CMcmpnocase(&prname[1], SYSTEM_CAT_PREFIX + 1)) ||
	     pruse[0] == 'G')
	    prettyprint = FALSE;
	if (prettyprint)
	{
	    /*
	    ** format the dbproc text as separate numbered statements
	    */
	    next_text = prtext;
	    while (*next_text)
	    {
		switch (*next_text)
		{
		    case '\'':
			/*
			** keep track of quotes and copy text to statement
			*/
			quotecnt++;
			CMcpychar(next_text, next_stmt);
			CMnext(next_text);
			CMnext(next_stmt);
			break;
		    case ';':
			/*
			** if parsing quoted string just copy text to statement
			*/
			if (quotecnt % 2)
			{
			    CMcpychar(next_text, next_stmt);
			    CMnext(next_text);
			    CMnext(next_stmt);
			    break;
			}
			/*
			** copy ';' to statement and finish off end of line
			*/
			*next_stmt = *next_text;
			*(next_stmt+1) = '\n';
			*(next_stmt+2) = '\0';
			/*
			** copy the formatted statment to the output buffer
			*/
			if (addtext(qrb, prstmt) == NULL)
			{
			    bad_add = TRUE;
/* # line 958 "qrtxthlp.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE9;
      }
/* # line 959 "qrtxthlp.sc" */	/* host code */
			}
			/*
			** prepare to print next statement
			*/
			stmtcnt++;
			STprintf(prstmt,"/* %4d */",stmtcnt);
			next_stmt = prstmt+sizeof("/* 0000 */")-1;
			CMnext(next_text);
			break;
		    default:
			/*
			** just copy text to statement
			*/
			CMcpychar(next_text, next_stmt);
			CMnext(next_text);
			CMnext(next_stmt);
			break;
		}
	    }
	}
	else
	{
	    if (addtext(qrb, prtext) == NULL)
	    {
		bad_add = TRUE;
/* # line 984 "qrtxthlp.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE9;
      }
/* # line 985 "qrtxthlp.sc" */	/* host code */
	    }
	}
	if (prettyprint)
	{
	    /*
	    ** flush what we have left and start a new line
	    */
	    CMcpychar(next_text, next_stmt);
	    if (addtext(qrb, prstmt) == NULL)
	    {
		bad_add = TRUE;
/* # line 996 "qrtxthlp.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE9;
      }
/* # line 997 "qrtxthlp.sc" */	/* host code */
	    }
	    next_stmt = prstmt;
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE9:;
  }
    }
    else
    {
/* # line 1004 "qrtxthlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtxthlp10",7493,7413);
      IIputdomio((short *)0,1,32,0,prname);
      IIputdomio((short *)0,1,32,0,prowner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtxthlp10",7493,7413);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select text_segment, text_sequence, create_date, proc_subtype from ii\
procedures where procedure_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,prname);
        IIwritio(0,(short *)0,1,32,0,(char *)"and procedure_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,prowner);
        IIwritio(0,(short *)0,1,32,0,(char *)"order by text_sequence");
        IIexDefine(0,(char *)"qrtxthlp10",7493,7413);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE10;
IIrtB10:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,TEXTSEGWIDTH + 1-1,prtext);
      IIgetdomio((short *)0,1,30,4,&prseq);
      IIgetdomio((short *)0,1,32,25,prcdate);
      IIgetdomio((short *)0,1,32,8,prtype);
      if (IIerrtest() != 0) goto IIrtB10;
/* # line 1012 "qrtxthlp.sc" */	/* host code */
	if (!are_some)
	{
	    qrdiprintf( QRARG1, qrb, ERget(F_QR0079_Proc_def_heading), prname );
	    qrdiprintf( QRARG1, qrb, ERget(F_QR0013_Owner), prowner );
	    /*
	    ** Print procedure type
	    */
	    switch ( prtype[0] )
	    {
		case 'N':
		    qrprintf(qrb, ERget(F_QR0115_Proc_type), 
				ERget(F_QR0118_Native_proc));
		    break;
		case 'I':
		    qrprintf(qrb, ERget(F_QR0115_Proc_type), 
				ERget(F_QR0119_Imported_proc));
		    break;
		case 'E':
		    qrprintf(qrb, ERget(F_QR0115_Proc_type), 
				ERget(F_QR0120_Enterprise_proc));
		    break;
		case 'L':
		    qrprintf(qrb, ERget(F_QR0115_Proc_type), 
				ERget(F_QR0121_Star_proc));
		    break;
		default:
		    qrprintf(qrb, ERget(F_QR0115_Proc_type), 
				ERget(F_QR0005_unknown));
		    break;
	    }
	    if (prcdate[0] != ' ')
	    {
		if (IIQRgtl_GmtToLocal(prcdate, prcdate) == OK)
		    qrprintf(qrb, ERget(F_QR0070_Created), prcdate);
	    }
	    qrprintf(qrb, ERget(F_QR007A_Proc_Def));
	    are_some = TRUE;
	}
	if (addtext(qrb, prtext) == NULL)
	{
	    bad_add = TRUE;
/* # line 1053 "qrtxthlp.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE10;
      }
/* # line 1054 "qrtxthlp.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE10:;
  }
    }
    if (FEinqerr() || (!are_some))
    {
	/*
	** Retrieving 0 rows is also an error, since have already verified
	** the existence of a procedure with this name/owner.
	*/
	qrprintf(qrb, ERget(E_QR001C_err_ret_iiproc));
	return;
    }
    else if (bad_add)
    {
	return;
    }
    cleartext(qrb);
    /*
    ** Only display permits on the procedure if iipermits exists.
    */
    if ( IIQR_relexists( qrb->lang, ERx( "iipermits" ) ) == OK )
    {
	are_some = FALSE;
/* # line 1077 "qrtxthlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtxthlp11",7494,24756);
      IIputdomio((short *)0,1,32,0,prname);
      IIputdomio((short *)0,1,32,0,prowner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtxthlp11",7494,24756);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select text_segment, permit_number, text_sequence from iipermits wher\
e object_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,prname);
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,prowner);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and object_type='P' order by permit_number, text_sequence");
        IIexDefine(0,(char *)"qrtxthlp11",7494,24756);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE11;
IIrtB11:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,TEXTSEGWIDTH + 1-1,prtext);
      IIgetdomio((short *)0,1,30,4,&prnum);
      IIgetdomio((short *)0,1,30,4,&prseq);
      if (IIerrtest() != 0) goto IIrtB11;
/* # line 1085 "qrtxthlp.sc" */	/* host code */
	    /* only if it's the first row, print the permissions header */
	    if (!are_some)
	    {
		qrdiprintf( QRARG1, qrb, ERget(F_QR007D_Permissions_header),
		    prname);
		are_some = TRUE;
	    }
	    /* if prseq > 1, then in the middle of a permit definition */
	    if (prseq <= 1)
	    {
		/* output any previous permit definition */
		cleartext(qrb);
		/* next permit */
		qrprintf(qrb, ERget(F_QR004E_Permission_header), prnum);
	    }
	    if (addtext(qrb, prtext) == NULL)
	    {
		bad_add = TRUE;
/* # line 1103 "qrtxthlp.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE11;
      }
/* # line 1104 "qrtxthlp.sc" */	/* host code */
	    }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE11:;
  }
	if (FEinqerr())
	{
	    qrprintf(qrb, ERget(F_QR004D_Error_retrieving_data));
	    return;
	}
	else if ((are_some) && (!bad_add))
	{
	    /* there are some permits, so flush */
		cleartext(qrb);
	}
    }
}
/*{
** Name:	IIQRlkh_LinkHelp	- get info from iiregistrations.
**
** Description:
**	Performs the actual retrieval from iiregistrations catalog of the
**	definition of the link or import. Prints to the screen via qrfmttext,
**	which places the output in the qrb's output buffer.
**
**	Additional information is displayed for STAR from the iiddb_tableinfo
**	and iiddb_ldbids catalogs.
**
** Inputs:
**	qrb		the QRB
**	linkname	the link name
**	linkowner	the link's owner
**
** Outputs:
**	none
**
**	Returns:
**		None
**
** Side Effects:
**	eventually fills the qrb's output buffer with the link definition.
**
** History:
**	28-sep-88 (bruceb)	First written.
**	07-apr-89 (teresal)	Display additional information for STAR.
*/
VOID
IIQRlkh_LinkHelp(qrb, linkname, linkowner)
QRB	*qrb;
  char *linkname;
  char *linkowner;
{
    bool	are_some = FALSE;
  i4 rows;
  i4 linkseq;
  char linktext[TEXTSEGWIDTH + 1];
  char linktype[9];
  char localtype[9];
  char localname[FE_MAXNAME + 1];
  char localowner[FE_MAXNAME + 1];
  char node[FE_MAXNAME + 1];
  char dbms[FE_MAXNAME + 1];
  char database[FE_MAXNAME + 1];
    bool	bad_add = FALSE;
    inittext(qrb);
/* # line 1170 "qrtxthlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtxthlp12",7495,25520);
      IIputdomio((short *)0,1,32,0,linkname);
      IIputdomio((short *)0,1,32,0,linkowner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtxthlp12",7495,25520);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select text_segment, text_sequence, object_type from iiregistrations \
where object_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,linkname);
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,linkowner);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and(object_subtype='L' or object_subtype='I')order by text_sequence");
        IIexDefine(0,(char *)"qrtxthlp12",7495,25520);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE12;
IIrtB12:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,TEXTSEGWIDTH + 1-1,linktext);
      IIgetdomio((short *)0,1,30,4,&linkseq);
      IIgetdomio((short *)0,1,32,8,linktype);
      if (IIerrtest() != 0) goto IIrtB12;
/* # line 1179 "qrtxthlp.sc" */	/* host code */
	if (!are_some)
	{
	    qrdiprintf( QRARG1, qrb, ERget(F_QR0012_Name), linkname );
	    qrdiprintf( QRARG1, qrb, ERget(F_QR0013_Owner), linkowner );
	    are_some = TRUE;
	}
	if (addtext(qrb, linktext) == NULL)
	{
	    bad_add = TRUE;
/* # line 1188 "qrtxthlp.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE12;
      }
/* # line 1189 "qrtxthlp.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE12:;
  }
    if (FEinqerr())
    {
	qrprintf(qrb, ERget(E_QR001D_err_ret_iireg));
	return;
    }
    else if (!are_some)
    {
	/* Should never happen--should be caught in IIQRmdh */
	qrdiprintf( QRARG1, qrb, ERget(E_QR001E_Table_not_a_link), linkname );
	return;
    }
    if (IIUIdcd_dist())		/* get additional information for STAR */
    {
/* # line 1204 "qrtxthlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtxthlp13",7496,31984);
      IIputdomio((short *)0,1,32,0,linkname);
      IIputdomio((short *)0,1,32,0,linkowner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIsqMods(1);
        IIexDefine(1,(char *)"qrtxthlp13",7496,31984);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select ldb_node, ldb_dbms, ldb_database, table_name, table_owner, loc\
al_type from iiddb_objects, iiddb_tableinfo, iiddb_ldbids where object\
_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,linkname);
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,linkowner);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and iiddb_objects.object_base=iiddb_tableinfo.object_base and iiddb_o\
bjects.object_index=iiddb_tableinfo.object_index and iiddb_tableinfo.l\
db_id=iiddb_ldbids.ldb_id");
        IIexDefine(0,(char *)"qrtxthlp13",7496,31984);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,node);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,dbms);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,database);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,localname);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,localowner);
      IIgetdomio((short *)0,1,32,8,localtype);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 1213 "qrtxthlp.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,30,4,&rows,8);
  }
/* # line 1214 "qrtxthlp.sc" */	/* host code */
	if (FEinqerr() || (rows == 0))
	{
	    qrprintf(qrb, ERget(E_QR0026_err_ret_star_info));
	}
	else
	{
	    qrprttype(qrb,localtype[0]);	/* Print local type */
	    qrprintf(qrb, ERget(F_QR008E_Node), node);
	    qrprintf(qrb, ERget(F_QR008F_Database), database);
	    qrdiprintf( QRARG1, qrb, ERget(F_QR0090_Local_Table), localname );
	    qrdiprintf( QRARG1, qrb, ERget(F_QR0091_Local_Owner), localowner );
	    qrprintf(qrb, ERget(F_QR0092_DBMS), dbms);
	}
    }
    else
    {
	qrprttype(qrb,linktype[0]);	/* Print iiregistrations type */ 
    }	
    if (!bad_add)
    {
    	qrprintf(qrb, ERget(F_QR007C_Link_Def)); /* "Registration Text" */
	cleartext(qrb);
    }
}
/*{
** Name:	qrprttype	-  Print the description for type
**
** Description:
**	Expand and display the type value.
**
** Inputs:
**	qrb		the QRB
**	type		type value from either iiregistrations or
**			iiddb_tableinfo.
**
** Outputs:
**	none
**
**	Returns:
**		None
**
** Side Effects:
**	none
**
** History:
**	07-apr-89 (teresal)	First written.
*/
static VOID
qrprttype(qrb,type,focus)
QRB	*qrb;
char	type;
char	focus;
{
    if (type != ' ')
    {
	qrprintf(qrb, ERget(F_QR0015_Type));
	if (type == 'T')
	{
	    qrprintf(qrb, ERget(F_QR0017_user_table));
	}
	else if (type == 'I')
	{
	    qrprintf(qrb, ERget(F_QR006E_secondary_index));
	}
	else if (type == 'V')
	{
	    qrprintf(qrb, ERget(F_QR0019_view));
	}
	else if (type == 'P')
	{
	    qrprintf(qrb, ERget(F_QR012A_procedure));
	}
	else
	{
	    qrprintf(qrb, ERget(F_QR001A_unknown));
	}
    }
}
/*{
** Name:	IIQRrlh_RuleHelp	- get info from iirules.
**
** Description:
**	Performs the actual retrieval from iirules catalog of the
**	definition of a rule. Prints to the screen via qrfmttext,
**	which places the output in the qrb's output buffer.
**
** Inputs:
**	qrb		the QRB
**	linkname	the link name
**	linkowner	the link's owner
**
** Outputs:
**	none
**
**	Returns:
**		None
**
** Side Effects:
**	eventually fills the qrb's output buffer with the link definition.
**
** History:
**	18-may-89 (teresal)	First written.
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
**	24-jun-1992 (rogerl)
**	    Support for displaying help about rule 'type' as pertains
**	    to rule created by the system to support integritities
**	    constraints having 'system' type where user-rule are that.
**	    this necessitates the deletion of the check to only show the
**	    rule owned by the owner of this db session, and a similar
**	    permits check.  Add 'rtype' to extract the user/system
**	    type into.
*/
VOID
IIQRrlh_RuleHelp(qrb, rulename, ruleowner)
QRB	*qrb;
  char *rulename;
  char *ruleowner;
{
    bool	are_some = FALSE;
  i4 rows;
  i4 ruleseq;
  char ruletext[TEXTSEGWIDTH + 1];
  char ruletable[FE_MAXNAME + 1];
  char rtype[2];
    bool	bad_add = FALSE;
    inittext(qrb);
/* # line 1346 "qrtxthlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtxthlp14",7497,27310);
      IIputdomio((short *)0,1,32,0,rulename);
      IIputdomio((short *)0,1,32,0,ruleowner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtxthlp14",7497,27310);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select table_name, text_segment, text_sequence, system_use from iirul\
es where rule_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,rulename);
        IIwritio(0,(short *)0,1,32,0,(char *)"and rule_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,ruleowner);
        IIwritio(0,(short *)0,1,32,0,(char *)"order by text_sequence");
        IIexDefine(0,(char *)"qrtxthlp14",7497,27310);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE13;
IIrtB13:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,ruletable);
      IIgetdomio((short *)0,1,32,TEXTSEGWIDTH + 1-1,ruletext);
      IIgetdomio((short *)0,1,30,4,&ruleseq);
      IIgetdomio((short *)0,1,32,1,rtype);
      if (IIerrtest() != 0) goto IIrtB13;
/* # line 1354 "qrtxthlp.sc" */	/* host code */
	if (!are_some)
	{
	    qrdiprintf( QRARG1, qrb, ERget(F_QR0093_Rule), rulename );
	    qrprintf( qrb, ERget( F_QR0115_Proc_type ),
		( *rtype == 'G' )
		 ? ERget( F_QR0116_System_object )
		 : ERget( F_QR0117_User_object ) );
	    qrdiprintf( QRARG1, qrb, ERget(F_QR0013_Owner), ruleowner );
	    qrdiprintf( QRARG1, qrb, ERget(F_QR0094_Table), ruletable );
	    are_some = TRUE;
	}
	if (addtext(qrb, ruletext) == NULL)
	{
	    bad_add = TRUE;
/* # line 1368 "qrtxthlp.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE13;
      }
/* # line 1369 "qrtxthlp.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE13:;
  }
    if (FEinqerr() || (!are_some))
    {
	/*
	** Retrieving 0 rows is also an error, since have already verified
	** the existence of a rule with this name/owner.
	*/
	qrprintf(qrb, ERget(E_QR002A_err_ret_iirul));
	return;
    }
    else if (!bad_add)
    {
	qrprintf(qrb, ERget(F_QR0095_Rule_Def)); /* "Rule Definition" */
	cleartext(qrb);
    }
}
/*{
** Name:	IIQRsah_SecAlarmHelp	- Get info from iisecurity_alarms
**
** Description:
**	Performs the actual retrieval from the iisecurity_alarms catalog for the
**	definition of the security alarm. Prints to the screen via qrfmttext,
**	which places the output in the qrb's output buffer.
**
** Inputs:
**	qrb		the QRB
**	sname		the security alarm name
**	sowner		the security alarm's owner
**
** Outputs:
**	none
**
**	Returns:
**		None
**
** Side Effects:
**	eventually fills the qrb's output buffer with the security alarm
**	definition.
**
** History:
**	13-nov-89 (teresal)	First written.
**	28-jan-1994 (mgw) Bug #59199
**		Security_alarms can be on views too. Add object_type = 'V'
**		case in the select query.
*/
VOID
IIQRsah_SecAlarmHelp(qrb, sname, sowner)
QRB	*qrb;
  char *sname;
  char *sowner;
{
  char stext[TEXTSEGWIDTH + 1];
  i4 snum;
  i4 sseq;
    bool	are_some = FALSE;	/* if TRUE, there are security alarms */
    bool	bad_add = FALSE;
    inittext(qrb);
/* # line 1431 "qrtxthlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtxthlp15",7498,22000);
      IIputdomio((short *)0,1,32,0,sname);
      IIputdomio((short *)0,1,32,0,sowner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtxthlp15",7498,22000);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select text_segment, security_number, text_sequence from iisecurity_a\
larms where object_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,sname);
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,sowner);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and(object_type='T' or object_type='V')order by security_number, text\
_sequence");
        IIexDefine(0,(char *)"qrtxthlp15",7498,22000);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE14;
IIrtB14:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,TEXTSEGWIDTH + 1-1,stext);
      IIgetdomio((short *)0,1,30,4,&snum);
      IIgetdomio((short *)0,1,30,4,&sseq);
      if (IIerrtest() != 0) goto IIrtB14;
/* # line 1439 "qrtxthlp.sc" */	/* host code */
	/* only if it's the first row, print the security alarm header */
	if (!are_some)
	{
	    qrdiprintf( QRARG1, qrb, ERget(F_QR009C_Sec_Alarm_header), sname );
	    are_some = TRUE;
	}
	/* if sseq > 1, then it's in the middle of a big security alarm definition */
	if (sseq <= 1)
	{
	    /* output any previous security alarm definition */
	    cleartext(qrb);
	    /* next security alarm */
	    qrprintf(qrb, ERget(F_QR009E_Sec_Alarm_number), snum);
	}
	if (addtext(qrb, stext) == NULL)
	{
	    bad_add = TRUE;
/* # line 1456 "qrtxthlp.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE14;
      }
/* # line 1457 "qrtxthlp.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE14:;
  }
    if (FEinqerr())
    {
	qrprintf(qrb, ERget(F_QR009D_Error_iisecurity));
    }
    else if (!are_some)
    {
	 qrdiprintf( QRARG1, qrb, ERget(F_QR009F_Table_has_no_alarms), sname);
    }
    else if (!bad_add)
    {
	cleartext(qrb);
    }
}
/*{
** Name:	IIQRsah_AlarmDbHelp	- Get info from iisecurity_alarms
**					  for database alarms
**
** Description:
**	Performs the actual retrieval from the iisecurity_alarms catalog for the
**	definition of the db security alarm. Prints to the screen via qrfmttext,
**	which places the output in the qrb's output buffer.
**
** Inputs:
**	qrb		the QRB
**	sname		the security alarm database name
**	sowner		the security alarm's owner
**
** Outputs:
**	none
**
**	Returns:
**		None
**
** Side Effects:
**	eventually fills the qrb's output buffer with the security alarm
**	definition.
**
** History:
**	10-dec-93 (robf)	First written
*/
VOID
IIQRsah_AlarmDbHelp(qrb, sname, sowner)
QRB	*qrb;
  char *sname;
  char *sowner;
{
  char stext[TEXTSEGWIDTH + 1];
  i4 snum;
  i4 sseq;
    bool	are_some = FALSE;	/* if TRUE, there are security alarms */
    bool	bad_add = FALSE;
    inittext(qrb);
/* # line 1515 "qrtxthlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtxthlp16",7499,20653);
      IIputdomio((short *)0,1,32,0,sname);
      IIputdomio((short *)0,1,32,0,sowner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtxthlp16",7499,20653);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select text_segment, security_number, text_sequence from iisecurity_a\
larms where object_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,sname);
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,sowner);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and object_type='D' order by security_number, text_sequence");
        IIexDefine(0,(char *)"qrtxthlp16",7499,20653);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE15;
IIrtB15:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,TEXTSEGWIDTH + 1-1,stext);
      IIgetdomio((short *)0,1,30,4,&snum);
      IIgetdomio((short *)0,1,30,4,&sseq);
      if (IIerrtest() != 0) goto IIrtB15;
/* # line 1523 "qrtxthlp.sc" */	/* host code */
	/* only if it's the first row, print the security alarm header */
	if (!are_some)
	{
	    qrdiprintf( QRARG1, qrb, ERget(F_QR009C_Sec_Alarm_header), sname );
	    are_some = TRUE;
	}
	/* if sseq > 1, then it's in the middle of a big security alarm definition */
	if (sseq <= 1)
	{
	    /* output any previous security alarm definition */
	    cleartext(qrb);
	    /* next security alarm */
	    qrprintf(qrb, ERget(F_QR009E_Sec_Alarm_number), snum);
	}
	if (addtext(qrb, stext) == NULL)
	{
	    bad_add = TRUE;
/* # line 1540 "qrtxthlp.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE15;
      }
/* # line 1541 "qrtxthlp.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE15:;
  }
    if (FEinqerr())
    {
	qrprintf(qrb, ERget(F_QR009D_Error_iisecurity));
    }
    else if (!are_some)
    {
	 qrdiprintf( QRARG1, qrb, ERget(F_QR00A0_Database_has_no_alarm), 
			sname);
    }
    else if (!bad_add)
    {
	cleartext(qrb);
    }
}
/*{
** Name:	IIQRsah_AlarmInsHelp	- Get info from iisecurity_alarms
**					  for installation alarms
**
** Description:
**	Performs the actual retrieval from the iisecurity_alarms catalog for the
**	current installation. Prints to the screen via qrfmttext,
**	which places the output in the qrb's output buffer.
**
** Inputs:
**	qrb		the QRB
**
** Outputs:
**	none
**
**	Returns:
**		None
**
** Side Effects:
**	eventually fills the qrb's output buffer with the security alarm
**	definition.
**
** History:
**	10-dec-93 (robf)	First written
**      2-mar-1995 (inkdo01)
**              Fix 1st arg to qrdiprintf call for QR00A3 (secalarm stuff)
*/
VOID
IIQRsah_AlarmInsHelp(qrb)
QRB	*qrb;
{
  char stext[TEXTSEGWIDTH + 1];
  i4 snum;
  i4 sseq;
    bool	are_some = FALSE;	/* if TRUE, there are security alarms */
    bool	bad_add = FALSE;
    inittext(qrb);
/* # line 1596 "qrtxthlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtxthlp17",7500,21165);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtxthlp17",7500,21165);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select text_segment, security_number, text_sequence from iisecurity_a\
larms where object_type='I' order by security_number, text_sequence");
        IIexDefine(0,(char *)"qrtxthlp17",7500,21165);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE16;
IIrtB16:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,TEXTSEGWIDTH + 1-1,stext);
      IIgetdomio((short *)0,1,30,4,&snum);
      IIgetdomio((short *)0,1,30,4,&sseq);
      if (IIerrtest() != 0) goto IIrtB16;
/* # line 1602 "qrtxthlp.sc" */	/* host code */
	/* only if it's the first row, print the security alarm header */
	if (!are_some)
	{
	    qrdiprintf( 0, qrb, ERget(F_QR009C_Sec_Alarm_header), 
			ERx("current installation") );
	    are_some = TRUE;
	}
	/* if sseq > 1, then it's in the middle of a big security alarm definition */
	if (sseq <= 1)
	{
	    /* output any previous security alarm definition */
	    cleartext(qrb);
	    /* next security alarm */
	    qrprintf(qrb, ERget(F_QR009E_Sec_Alarm_number), snum);
	}
	if (addtext(qrb, stext) == NULL)
	{
	    bad_add = TRUE;
/* # line 1620 "qrtxthlp.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE16;
      }
/* # line 1621 "qrtxthlp.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE16:;
  }
    if (FEinqerr())
    {
	qrprintf(qrb, ERget(F_QR009D_Error_iisecurity));
    }
    else if (!are_some)
    {
	 qrdiprintf( 0, qrb, ERget(F_QR00A3_Installation_has_no_a));
    }
    else if (!bad_add)
    {
	cleartext(qrb);
    }
}
/*{
** Name:	IIQRseh_SequenceHelp	- get info from iisequences
**
** Description:
**	Performs the actual retrieval from iiisequences catalog of the
**	definition of a sequence. Prints to the screen via qrfmttext,
**	which places the output in the qrb's output buffer.
**
** Inputs:
**	qrb		the QRB
**	linkname	the link name
**	linkowner	the link's owner
**
** Outputs:
**	none
**
**	Returns:
**		None
**
** Side Effects:
**	eventually fills the qrb's output buffer with the link definition.
**
** History:
**	17-apr-02 (inkdo01)
**	    Written for 'help sequence'.
**	29-jan-03 (inkdo01)
**	    Changed strategy for reading numbers to allow big dec values
**	    and modified to handle new compound column names in iisequences.
**	26-feb-03 (inkdo01)
**	    Display start, increment even if not coded in sequence definition.
**	24-sep-2007 (dougi)
**	    Needed one more character to display big decimal numbers.
*/
VOID
IIQRseh_SequenceHelp(qrb, seqname, seqowner)
QRB	*qrb;
  char *seqname;
  char *seqowner;
{
    bool	are_some = FALSE;
  char seqtext[TEXTSEGWIDTH + 1];
  char seqtable[FE_MAXNAME + 1];
  char seqtype[8];
  int seqlen, seqprec, seqcache;
  char seqstart[32], seqincr[32], seqmin[32], seqmax[32];
  char seqstartf[2], seqincrf[2], seqminf[2], seqmaxf[2];
  char seqrestartf[2], seqcachef[2], seqcyclef[2], seqorderf[2];
    int		seqtlen;
    bool	bad_add = FALSE;
    inittext(qrb);
/* # line 1690 "qrtxthlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtxthlp18",7501,13995);
      IIputdomio((short *)0,1,32,0,seqname);
      IIputdomio((short *)0,1,32,0,seqowner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtxthlp18",7501,13995);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select data_type, seq_length, seq_precision, trim(char(start_value)),\
 trim(char(increment_value)), trim(char(min_value)), trim(char(max_val\
ue)), cache_size, start_flag, incr_flag, min_flag, max_flag, restart_f\
lag, cache_flag, cycle_flag, order_flag from ");
        IIwritio(0,(short *)0,1,32,0,(char *)"iisequences where seq_na\
me=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,seqname);
        IIwritio(0,(short *)0,1,32,0,(char *)"and seq_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,seqowner);
        IIexDefine(0,(char *)"qrtxthlp18",7501,13995);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE17;
IIrtB17:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,7,seqtype);
      IIgetdomio((short *)0,1,30,4,&seqlen);
      IIgetdomio((short *)0,1,30,4,&seqprec);
      IIgetdomio((short *)0,1,32,31,seqstart);
      IIgetdomio((short *)0,1,32,31,seqincr);
      IIgetdomio((short *)0,1,32,31,seqmin);
      IIgetdomio((short *)0,1,32,31,seqmax);
      IIgetdomio((short *)0,1,30,4,&seqcache);
      IIgetdomio((short *)0,1,32,1,seqstartf);
      IIgetdomio((short *)0,1,32,1,seqincrf);
      IIgetdomio((short *)0,1,32,1,seqminf);
      IIgetdomio((short *)0,1,32,1,seqmaxf);
      IIgetdomio((short *)0,1,32,1,seqrestartf);
      IIgetdomio((short *)0,1,32,1,seqcachef);
      IIgetdomio((short *)0,1,32,1,seqcyclef);
      IIgetdomio((short *)0,1,32,1,seqorderf);
      if (IIerrtest() != 0) goto IIrtB17;
/* # line 1702 "qrtxthlp.sc" */	/* host code */
	are_some = TRUE;
	qrdiprintf( QRARG1, qrb, ERget(F_QR00AC_Sequence), seqname );
	qrdiprintf( QRARG1, qrb, ERget(F_QR0013_Owner), seqowner );
	qrprintf(qrb, ERget(F_QR00AD_Seq_Def)); /* "Sequence Definition" */
	/* Compose sequence definition text (no iiqrytext rows). */
	STprintf(seqtext, "Create sequence %s.%s as %s\n", seqowner, seqname, seqtype);
	seqtlen = STlength(seqtext);
	if (seqtype[0] == 'd')
	{
	    STprintf(&seqtext[seqtlen-1], "(%d)\n", seqprec);
	    seqtlen = STlength(seqtext);
	}
	/* Used to only display start/incr when they were explicitly coded
	** in sequence definition. But it seems reasonable even to display
	** the defaults. */
	{
	    STprintf(&seqtext[seqtlen-1], " start with %s\n", seqstart);
	    seqtlen = STlength(seqtext);
	}
	if (addtext(qrb, seqtext) == NULL)
	{
	    bad_add = TRUE;
/* # line 1724 "qrtxthlp.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE17;
      }
/* # line 1725 "qrtxthlp.sc" */	/* host code */
	}
	STprintf(seqtext, "    \n");
	seqtlen = STlength(seqtext);
	{
	    STprintf(&seqtext[seqtlen-1], " increment by %s\n", seqincr);
	    seqtlen = STlength(seqtext);
	}
	if (seqminf[0] == 'Y')
	{
	    STprintf(&seqtext[seqtlen-1], " minvalue %s\n", seqmin);
	    seqtlen = STlength(seqtext);
	}
	else if (seqminf[0] = 'N')
	{
	    STprintf(&seqtext[seqtlen-1], " no minvalue\n");
	    seqtlen = STlength(seqtext);
	}
	if (seqmaxf[0] == 'Y')
	{
	    STprintf(&seqtext[seqtlen-1], " maxvalue %s\n", seqmax);
	    seqtlen = STlength(seqtext);
	}
	else if (seqmaxf[0] = 'N')
	{
	    STprintf(&seqtext[seqtlen-1], " no maxvalue\n");
	    seqtlen = STlength(seqtext);
	}
	if (addtext(qrb, seqtext) == NULL)
	{
	    bad_add = TRUE;
/* # line 1755 "qrtxthlp.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE17;
      }
/* # line 1756 "qrtxthlp.sc" */	/* host code */
	}
	STprintf(seqtext, "    \n");
	seqtlen = STlength(seqtext);
	if (seqcachef[0] == 'Y')
	{
	    STprintf(&seqtext[seqtlen-1], " cache %d\n", seqcache);
	    seqtlen = STlength(seqtext);
	}
	if (seqcachef[0] == 'N')
	{
	    STprintf(&seqtext[seqtlen-1], " no cache\n");
	    seqtlen = STlength(seqtext);
	}
	STprintf(&seqtext[seqtlen-1], "%scycle\n", 
	    (seqcyclef[0] == 'N') ? " no " : " ");
	seqtlen = STlength(seqtext);
	STprintf(&seqtext[seqtlen-1], "%sorder\n", 
	    (seqorderf[0] == 'N') ? " no " : " ");
	if (addtext(qrb, seqtext) == NULL)
	{
	    bad_add = TRUE;
/* # line 1777 "qrtxthlp.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE17;
      }
/* # line 1778 "qrtxthlp.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE17:;
  }
    if (FEinqerr() || (!are_some))
    {
	/*
	** Retrieving 0 rows is also an error, since have already verified
	** the existence of a sequence with this name/owner.
	*/
	qrprintf(qrb, ERget(E_QR0063_err_ret_iiseq));
	return;
    }
    else if (!bad_add)
    {
	cleartext(qrb);
    }
}
/*{
** Name:	IIQRsyh_SynonymHelp	- Get info from iisynonyms
**
** Description:
**	Performs the actual retrieval from the iisynonyms catalog. Note:
**	If a wildcard is used, only those synonyms owned by the current
**	user are shown.  If a specific synonym name is requested, that synonym
**	info is displayed only if the synonym is owned by the user or the DBA. 
**
** Inputs:
**	qrb		the QRB
**	syname		the synonym name
**	syowner		the synonym's owner - this is NULL if a wildcard
**			was not used.
**
** Outputs:
**	none
**
**	Returns:
**		None
**
** Side Effects:
**	eventually fills the qrb's output buffer with the synonym
**	definition.
**
** History:
**	29-dec-89 (teresal)	First written.
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
*/
VOID
IIQRsyh_SynonymHelp(qrb, syname, syowner)
QRB	*qrb;
  char *syname;
  char *syowner;
{
  char sytblname[FE_MAXNAME + 1];
  char sytblowner[FE_MAXNAME + 1];
  char syown[FE_MAXNAME + 1];
  UIDBDATA *uidbdata;
    bool	are_some = FALSE;	/* if TRUE, there are synonyms */
    uidbdata = IIUIdbdata();
    if (syowner == NULL)	/* Display user or dba owned synonym */ 
    {
/* # line 1843 "qrtxthlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtxthlp19",7502,16620);
      IIputdomio((short *)0,1,32,0,syname);
      IIputdomio((short *)0,1,32,0,uidbdata->suser);
      IIputdomio((short *)0,1,32,0,uidbdata->sdba);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtxthlp19",7502,16620);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select synonym_owner, table_name, table_owner from iisynonyms where s\
ynonym_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,syname);
        IIwritio(0,(short *)0,1,32,0,(char *)"and(synonym_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,uidbdata->suser);
        IIwritio(0,(short *)0,1,32,0,(char *)"or synonym_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,uidbdata->sdba);
        IIwritio(0,(short *)0,1,32,0,(char *)")");
        IIexDefine(0,(char *)"qrtxthlp19",7502,16620);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE18;
IIrtB18:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,syown);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,sytblname);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,sytblowner);
      if (IIerrtest() != 0) goto IIrtB18;
/* # line 1850 "qrtxthlp.sc" */	/* host code */
	if (!are_some)
	{
	    are_some = TRUE;
	}
	syowner = syown;
	if (syowner == uidbdata->suser) /* User synonym takes precedence */
/* # line 1856 "qrtxthlp.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE18;
      }
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE18:;
  }
/* # line 1858 "qrtxthlp.sc" */	/* host code */
    }
    else	/* Wildcard given, only display user owned synonyms */
    {
/* # line 1861 "qrtxthlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtxthlp20",7503,11432);
      IIputdomio((short *)0,1,32,0,syname);
      IIputdomio((short *)0,1,32,0,syowner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtxthlp20",7503,11432);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select table_name, table_owner from iisynonyms where synonym_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,syname);
        IIwritio(0,(short *)0,1,32,0,(char *)"and synonym_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,syowner);
        IIexDefine(0,(char *)"qrtxthlp20",7503,11432);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE19;
IIrtB19:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,sytblname);
      IIgetdomio((short *)0,1,32,FE_MAXNAME + 1-1,sytblowner);
      if (IIerrtest() != 0) goto IIrtB19;
/* # line 1867 "qrtxthlp.sc" */	/* host code */
	if (!are_some)
	{
	    are_some = TRUE;
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE19:;
  }
    }	
    if (FEinqerr())
    {
	qrprintf(qrb, ERget(E_QR0035_err_ret), ERx("iisynonyms"));
    }
    else if (!are_some)
    {
	qrdiprintf( QRARG1, qrb, ERget(E_QR0036_Synonym_does_not), syname );
    }
    else
    {
	qrdiprintf( QRARG1, qrb, ERget(F_QR0103_Synonym_Name), syname );
	qrdiprintf( QRARG1, qrb, ERget(F_QR0104_Synonym_Owner), syowner );
	qrdiprintf( QRARG1, qrb, ERget(F_QR0105_Table_Name), sytblname );
	qrdiprintf( QRARG1, qrb, ERget(F_QR0106_Table_Owner), sytblowner );
	qrprintf(qrb, ERx("\n"));
    }
}
/*{
** Name:	IIQRcth_CommTblHelp	- Get info from iidb_comments
**
** Description:
**	Performs the actual retrieval from the iidb_comments catalog for
**	displaying long and short remarks on a primary object (table, view or 
**	index). Prints to the screen via qrfmttext, which places the output in 
**	the qrb's output buffer.
**
** Inputs:
**	qrb		the QRB
**	oname		the object name
**	oowner		the object's owner
**
** Outputs:
**	none
**
**	Returns:
**		None
**
** Side Effects:
**	eventually fills the qrb's output buffer with the long remark
**	definition.
**
** History:
**	03-jan-89 (teresal)	First written.
*/
VOID
IIQRcth_CommTblHelp(qrb, oname, oowner)
QRB	*qrb;
  char *oname;
  char *oowner;
{
  char olremark[LREMARKWIDTH + 1];
  char osremark[SREMARKWIDTH + 1];
  i4 oseq;
    bool	are_some = FALSE;	/* if TRUE, there are remarks */
    bool	bad_add = FALSE;
    inittext(qrb);
/* # line 1933 "qrtxthlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtxthlp21",7504,9961);
      IIputdomio((short *)0,1,32,0,oname);
      IIputdomio((short *)0,1,32,0,oowner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtxthlp21",7504,9961);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select short_remark, long_remark, text_sequence from iidb_comments wh\
ere object_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,oname);
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,oowner);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and object_type='T' order by text_sequence");
        IIexDefine(0,(char *)"qrtxthlp21",7504,9961);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE20;
IIrtB20:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,SREMARKWIDTH + 1-1,osremark);
      IIgetdomio((short *)0,1,32,LREMARKWIDTH + 1-1,olremark);
      IIgetdomio((short *)0,1,30,4,&oseq);
      if (IIerrtest() != 0) goto IIrtB20;
/* # line 1941 "qrtxthlp.sc" */	/* host code */
	/* only if it's the first row, print the header and short remark */
	if (!are_some)
	{
	    qrdiprintf( QRARG1, qrb, ERget(F_QR0094_Table), oname );
	    qrdiprintf( QRARG1, qrb, ERget(F_QR0013_Owner), oowner );
	    /* Short_remark not supported in 6.4 - comment out display for now 
	    ** qrprintf(qrb, ERget(F_QR0108_Remark), osremark);
	    */
	    qrprintf(qrb, ERget(F_QR0109_Long_Remark));
	    are_some = TRUE;
	}
	if (addtext(qrb, olremark) == NULL)
	{
	    bad_add = TRUE;
/* # line 1955 "qrtxthlp.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE20;
      }
/* # line 1956 "qrtxthlp.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE20:;
  }
    if (FEinqerr())
    {
	qrprintf(qrb, ERget(E_QR0035_err_ret), ERx("iidb_comments"));
    }
    else if (!are_some)
    {
	 qrdiprintf( QRARG2, qrb, ERget(F_QR0102_has_no_remarks),
	     ERx("Table"), oname );
    }
    else if (!bad_add)
    {
	cleartext(qrb);
    }
}
/*{
** Name:	IIQRcch_CommColHelp	- Get info from iidb_subcomments
**
** Description:
**	Performs the actual retrieval from the iidb_subcomments catalog for
**	displaying long and short remarks on a column. Prints to the screen via
**	qrfmttext, which places the output in the qrb's output buffer.
**
** Inputs:
**	qrb		the QRB
**	obname		the object name
**	obowner		the object's owner
**	colname		the column name
**
** Outputs:
**	none
**
**	Returns:
**		None
**
** Side Effects:
**	eventually fills the qrb's output buffer with the long remark
**	definition.
**
** History:
**	03-jan-89 (teresal)	First written.
*/
VOID
IIQRcch_CommColHelp(qrb, obname, obowner, colname)
QRB	*qrb;
  char *obname;
  char *obowner;
  char *colname;
{
  char collremark[LREMARKWIDTH + 1];
  char colsremark[SREMARKWIDTH + 1];
  i4 colseq;
    bool	are_some = FALSE;	/* if TRUE, there are remarks */
    bool	bad_add = FALSE;
    inittext(qrb);
/* # line 2016 "qrtxthlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtxthlp22",7505,14759);
      IIputdomio((short *)0,1,32,0,obname);
      IIputdomio((short *)0,1,32,0,obowner);
      IIputdomio((short *)0,1,32,0,colname);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtxthlp22",7505,14759);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select short_remark, long_remark, text_sequence from iidb_subcomments\
 where object_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,obname);
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,obowner);
        IIwritio(0,(short *)0,1,32,0,(char *)"and subobject_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $2=");
        IIputdomio((short *)0,1,32,0,colname);
        IIwritio(0,(short *)0,1,32,0,(char *)
"and subobject_type='C' order by text_sequence");
        IIexDefine(0,(char *)"qrtxthlp22",7505,14759);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE21;
IIrtB21:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,SREMARKWIDTH + 1-1,colsremark);
      IIgetdomio((short *)0,1,32,LREMARKWIDTH + 1-1,collremark);
      IIgetdomio((short *)0,1,30,4,&colseq);
      if (IIerrtest() != 0) goto IIrtB21;
/* # line 2025 "qrtxthlp.sc" */	/* host code */
	/* only if it's the first row, print the header and short remark */
	if (!are_some)
	{
	    qrdiprintf( QRARG1, qrb, ERget(F_QR0094_Table), obname );
	    qrdiprintf( QRARG1, qrb, ERget(F_QR0013_Owner), obowner );
	    qrdiprintf( QRARG1, qrb, ERget(F_QR0107_Column), colname );
	    /* Short_remark not supported in 6.4 - comment out display for now 
	    ** qrprintf(qrb, ERget(F_QR0108_Remark), colsremark);
	    */
	    qrprintf(qrb, ERget(F_QR0109_Long_Remark));
	    are_some = TRUE;
	}
	if (addtext(qrb, collremark) == NULL)
	{
	    bad_add = TRUE;
/* # line 2040 "qrtxthlp.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE21;
      }
/* # line 2041 "qrtxthlp.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE21:;
  }
    if (FEinqerr())
    {
	qrprintf(qrb, ERget(E_QR0035_err_ret), ERx("iidb_subcomments"));
    }
    else if (!are_some)
    {
	 qrdiprintf( QRARG2, qrb, ERget(F_QR0102_has_no_remarks),
	     ERx("Column"), colname );
    }
    else if (!bad_add)
    {
	cleartext(qrb);
    }
}
/*{
** Name:	IIQRphp_Permit_Help - get info from iipermits
**
** Description:
**	Performs the actual retrieval from the iipermits catalog for the
**	definition of the permit. Prints to the screen via qrfmttext,
**	which places the output in the qrb's output buffer.
**
** Inputs:
**	qrb		the QRB
**	p		the permit name
**	powner		the permit's owner
**
** Outputs:
**	none
**
**	Returns:
**		None
**
** Side Effects:
**	eventually fills the qrb's output buffer with the permit definition.
**
** History:
**	22-mar-1991 (kathryn)	Written.
*/
VOID
IIQRphp_Permit_Help(qrb, p, powner)
QRB	*qrb;
  char *p;
  char *powner;
{
  char ptext[TEXTSEGWIDTH + 1];
  i4 pnum;
  i4 pseq;
    bool	headprinted = FALSE;	/* if TRUE, there are some permits */
    char	*t = NULL;
    bool	bad_add = FALSE;
    inittext(qrb);
/* # line 2099 "qrtxthlp.sc" */	/* repeated select */
  {
    IIsexec();
    while (IInexec() == 0) {
      IIsqInit((char *)0);
      IIexExec(1,(char *)"qrtxthlp23",7506,11430);
      IIputdomio((short *)0,1,32,0,p);
      IIputdomio((short *)0,1,32,0,powner);
      IIretinit((char *)0,0);
      if (IInexec() == 0) {
        IIsqInit((char *)0);
        IIexDefine(1,(char *)"qrtxthlp23",7506,11430);
        IIwritio(0,(short *)0,1,32,0,(char *)
"select text_segment, permit_number, text_sequence from iipermits wher\
e object_name=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $0=");
        IIputdomio((short *)0,1,32,0,p);
        IIwritio(0,(short *)0,1,32,0,(char *)"and object_owner=");
        IIwritio(0,(short *)0,1,32,0,(char *)" $1=");
        IIputdomio((short *)0,1,32,0,powner);
        IIwritio(0,(short *)0,1,32,0,(char *)
"order by permit_number, text_sequence");
        IIexDefine(0,(char *)"qrtxthlp23",7506,11430);
        if (IIerrtest() == 0) {
          IIsexec();
        } /* IIerrtest */
      } /* IInexec */
    } /* IInexec */
    if (IIerrtest() != 0) goto IIrtE22;
IIrtB22:
    while (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,TEXTSEGWIDTH + 1-1,ptext);
      IIgetdomio((short *)0,1,30,4,&pnum);
      IIgetdomio((short *)0,1,30,4,&pseq);
      if (IIerrtest() != 0) goto IIrtB22;
/* # line 2106 "qrtxthlp.sc" */	/* host code */
	/* only if it's the first row, print the permissions header */
	if (!headprinted)
	{
		qrdiprintf( QRARG1, qrb, ERget(F_QR004C_Permissions_header), p);
		headprinted = TRUE;
        }
	/* if pseq > 1, then it's in the middle of a big permit definition */
	if (pseq <= 1)
	{
	    /* output any previous permit definition */
	    cleartext(qrb);
	    /* next permit */
	    qrprintf(qrb, ERget(F_QR004E_Permission_header), pnum);
	}
	if (addtext(qrb, ptext) == NULL)
	{
	    bad_add = TRUE;
/* # line 2123 "qrtxthlp.sc" */	/* endselect */
      {
        IIbreak();
        if (1) goto IIrtE22;
      }
/* # line 2124 "qrtxthlp.sc" */	/* host code */
	}
    } /* IInextget */
    IIflush((char *)0,0);
IIrtE22:;
  }
    if (FEinqerr())
    {
	qrprintf(qrb, ERget(F_QR004D_Error_retrieving_data));
    }
    else if (!headprinted)
    {
	 qrdiprintf( QRARG1, qrb, ERget(F_QR0114_Object_has_no_permits), p );
    }
    else if (!bad_add)
    {
	cleartext(qrb);
    }
}
/*{
** Name:	qrfmttext	- formatting text for help commands
**
** Description:
**	This routine takes a string, and uses the c0.70 wrapping formats.
**
** Inputs:
**	qrb		the qrb.
**	text		the string to be printed
**
** Outputs:
**	none
**
**	Returns:
**		OK		Success!
**		FAIL		General failure.
**
** Side Effects:
**	places the formatted string on the qrb's output buffer via qrprintf.
**
** History:
**	17-aug-87 (daver) First Written.
*/
static STATUS
qrfmttext(qrb, text)
QRB	*qrb;
char	*text;
{
    ADF_CB		*adfcb;
    char		fmtstr[60];
    i4			fmtsize;
    PTR			blk;
    DB_DATA_VALUE	dbv;
    DB_TEXT_STRING	*textstr;
    i4			len;
    PTR			wksp;
    bool		more = TRUE;
    if (text == NULL)
	return OK;
    adfcb = FEadfcb();
    /*
    ** only allocate this FMT once. after that, the wrapping
    ** format here will ALWAYS be cf0.70 (for help view, permit, integrity)
    ** so we can reuse this fmt once it's initialized
    */
    if (IIhelpfmt == NULL)
    {
	STprintf(fmtstr, ERx("cf0.%d"), (i4)MSGWIDTH);
	if ((fmt_areasize(adfcb, fmtstr, &fmtsize) == OK) &&
	    ((blk = MEreqmem((u_i4)0, (u_i4)fmtsize, TRUE,
		(STATUS *)NULL)) != NULL) &&
	    (fmt_setfmt(adfcb, fmtstr, blk, &IIhelpfmt,
		&len) == OK))
	{
	    ;
	}
	else
	{
	    return(FAIL);
	}
	IIhelpdbv.db_datatype = DB_LTXT_TYPE;
	IIhelpdbv.db_prec = 0;
	IIhelpdbv.db_length = MSGWIDTH + DB_CNTSIZE;
	if ((IIhelpdbv.db_data = MEreqmem((u_i4)0,
	    (u_i4)IIhelpdbv.db_length, TRUE, (STATUS *)NULL)) == NULL)
	{
	    return(FAIL);
	}
    }
    _VOID_ FEbegintag();
    /*
    **	Now set up the DB_DATA_VALUE for the message string
    **	to use in the formatting routines.
    */
    dbv.db_datatype = DB_CHR_TYPE;
    dbv.db_length = STlength(text);
    dbv.db_prec = 0;
    dbv.db_data = (PTR) text;
    fmt_workspace(adfcb, IIhelpfmt, &dbv, &len);
    if ((wksp = FEreqmem((u_i4)0, (u_i4)len, TRUE,
	(STATUS *)NULL)) == NULL)
    {
	_VOID_ FEfree(FEendtag());
	return(FAIL);
    }
    /*
    ** Now actually call the fmt routines for the
    ** wrapping formats
    */
    textstr = (DB_TEXT_STRING *)IIhelpdbv.db_data;
    IIfmt_init(adfcb, IIhelpfmt, &dbv, wksp);
    while (more)
    {
	if (fmt_multi(adfcb, IIhelpfmt, &dbv, wksp,
	    &IIhelpdbv, &more, FALSE, FALSE) != OK)
	{
	    _VOID_ FEfree(FEendtag());
	    return(FAIL);
	}
	if (more)
	{
	    char	*txt_str;
	    txt_str = MEreqmem( NULL, (u_i4)textstr->db_t_count + 1, FALSE,
				(STATUS *)NULL );
	    STlcopy(textstr->db_t_text, txt_str, textstr->db_t_count);
	    qrprintf(qrb, ERx("%.*s\n"), (i4)textstr->db_t_count, txt_str);
	    MEfree((PTR)txt_str);
	}
    }
    qrprintf(qrb, ERx("\n"));
    /* free up memory */
    _VOID_ FEfree(FEendtag());
    return(OK);
}
static char *
addtext(qrb, retbuf)
QRB	*qrb;
char	*retbuf;
{
    i4	retlen;
    /* see if allocation of a bigger buffer is necessary */
    retlen = STlength(retbuf);
    if (STlength(text) + retlen > textbufsiz )
    {
	PTR t = (PTR) text;
	if ((text = MEreqmem((u_i4)0, (SIZE_TYPE)textbufsiz*2 + 1, FALSE,
	    (STATUS *)NULL)) == NULL)
	{
	    qrprintf(qrb, ERget(E_QR000F_Error_allocing_mem));
	    return NULL;
	}
	STcopy(t, text);
	textbufsiz *= 2;
	MEfree(t);
    }
    STcat(text, retbuf);
    return text;
}
static VOID
inittext(qrb)
QRB	*qrb;
{
    if (text == NULL)
    {
	if ((text = (char *)MEreqmem((u_i4)0, (u_i4)(INITBUFSIZE + 1),
	    TRUE, (STATUS *)NULL)) == NULL)
	{
	    qrprintf(qrb, ERget(E_QR000F_Error_allocing_mem));
	}
    }
}
static VOID
cleartext(qrb)
QRB	*qrb;
{
    qrfmttext(qrb, text);
    *text = EOS;
}
