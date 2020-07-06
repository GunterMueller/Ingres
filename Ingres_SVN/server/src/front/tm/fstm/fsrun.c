# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<cv.h>		/* 6-x_PC_80x86 */
# include	<nm.h>		/* 6-x_PC_80x86 */
# include	<st.h>		/* 6-x_PC_80x86 */
# include	<me.h>
# include	<ci.h>
# include	<ex.h>
# include	<cm.h>
# include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ug.h>
# include	<uf.h>
# include	<fstm.h>
# include	<qr.h>
# include	<ermf.h>
/**
** Name:	fsrun.qsc -	Interactive Terminal Monitor Run Module.
**
** Description:
**	Contains the routine that runs the interactive terminal monitor
**	frame (the only one.)  Defines:
**
**	FSrun()
**
** History:
**	mm/dd/yy (RTI) -- created for 5.0.
**	10/20/86 (KY)  -- Changed CH.h to CM.h.
**	08/25/87 (scl) -- Convert FTprompt to ##prompt
**	10-aug-88 (bruceb)
**		Changed ##abort to EXEC SQL ROLLBACK WORK and
**		##end transaction to EXEC SQL COMMIT WORK.
**	12-oct-88 (kathryn)	Bugs 2673,3269
**		End help transaction when all output has not been scrolled 
**		through.
**	26-oct-88 (bruceb)	Fix for bug 2603.
**		Call IIMFdcl_DisplayCleanup when an interrupt causes movement
**		from output display to input form.
**		Interrupts react in the following fashion:
**		  If on the input form and a query is in progress, than
**			add a message to output buffer, but stay on input
**			form.
**		  If on input form with no query in progress, do nothing.
**		  If on output display and query in progress, add message to
**			output buffer and stay on output display.
**		  If on output display and no query in progress, return to
**			input form.
**	24-apr-89 (kathryn)  Added OnError option and  FSonError() function.
**	04-may-89 (kathryn)  Bug# 5632
**		Changed Quit option to be FRSkey2 ONLY - FRSkey3 no longer
**		activates the QUIT menuitem.
**	23-may-89 (kathryn)  Bug# 5631
**		Always commit when quitting if language was SQL.
**	25-may-89 (kathryn) Added IIFSend_qry().
**      25-may-89 (teresal)
**              Add new activations InsertLine and DeleteLine.
**	21-jul-89 (teresal)
**		Bug fix for #6597.  Put out an error message if the query
**		buffer is full.
**	27-jul-89 (teresal)
**		Changed back to having FRSkey3 also activate the QUIT menuitem.
**		See FRC minutes for 7/3/89.
**      02-aug-89 (sylviap)
**		Added _VOID_ to FSmore and FSdisp calls.
**		Added help file parameters to FSdisp.
**	04-oct-89 (teresal)
**		Added check for Runtime INGRES installation.
**	15-oct-89 (teresal)
**		The tablefield editing functions have been removed from the
**		FSTM directory and put into a general facility (uf) so emerald
**		can use them as well.  The function calls FSedit,
**		FSload, and FSunload have been replaced by calls to 
**		IIUFtfeTblFldEdit, IIUFtflTblFldLoad, and IIUFtfuTblFldUnload.
**		The InsertLine and DeleteLine menu items have been replaced
**		by a LineEdit menu item that brings up a menu item containing
**		various line editing functions.
**      16-oct-89 (sylviap)
**		Changed IIMFdcl_DisplayCleanup to IIUFdcl_DisplayCleanup, and
**		   IIUFoof_OnOutputForm to IIMFoof_OnOutputForm.
**		Changed routine calls from FS to IIUF.
**	21-dec-89 (teresal)
**		IBM porting change.  Moved NMgtAt call out of if stmt.
**		
**	28-nov-89 (teresal)
**		Changed global Runtime_INGRES to be referenced rather than
**		defined here.
**	08-feb-90 (sylviap)
**		Added a parameter to IIUFdsp_Display to control scrolling.
**      02-mar-1990 (Mike S)
**              Use function interface for "on output form" flag
**	13-mar-90 (teresal)
**		Fix for bug 9141.  Only put out SQL committed msg when
**		COMMIT was sucessful.
**	20-mar-90 (teresal)
**		Eliminated 'Query buffer is full' message - Query buffer is
**		now dynamically allocated. Bug 8389.
**      01-may-90 (sylviap)
**		Added new parameters to IIUFdsp_Display for popup titles.
**	15-may-1990 (teresal)
**		Added top and bottom frskeys so control keys will appear
**		in the help keys display.
**	12-jun-1990 (teresal)
**		Added help for LineEdit menu item.  Changed Read and Write
**		menuitems to be ReadFile and WriteFile to make this more
**		consistent with other frontends and added help explanations
**		for these menu items.
**	29-aug-1990 (kathryn) - Integration of Porting changes:
**		Remove Declaration of inMST() which is an obsolete function.
**	17-mar-92 (seg)	Exception handlers return STATUS, not EX.
**	21-apr-92 (seg)	"errno" reserved by ANSI.  Changed to "errnum" for OS/2.
**	10-oct-92 (rogerl) 
**		Allocate output buffer dynamically, double it's initial size.
**	29-jan-93 (rogerl) popform in interrupt handler, reorder calls after
**		[ backed out ]
**	5-feb-93 (rogerl) rework above 'fix'.
**		[ backed out ]
**	8-mar-93 (rogerl) backed out 29-jan-93, 5-feb-93 changes
**		Ignore interrupts on subforms as per FRC decision.
**		Bugs 42020,40968,40449
**	01/19/94 (dkh) - Fixed bug 58914.  Moved buffer allocation
**			 for the output buffer to be before the calls
**			 to FSsetup().  This is needed since the
**			 completion of a query causes the buffer to be freed.
**	01/20/94 (ricka)
**		added "include <me.h>", needed because of MEreqmem added as
**		part of above fix.
**	03/21/94 (donc)
**		replace CI_xxxx_RUNTIME in favor of new CI_CREATE_TABLE
**	04/23/96 (chech02)
**		inclued uf.h for windows 3.1 port.
**      03-oct-1996 (rodjo04)
**              Changed "ermf.h" to <ermf.h>
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      27-oct-2004 (huazh01)
**          Call IIhelp() if the query is a detailed 'help' statement. 
**          b64679, INGCBT545. 
**      11-Jun-2004 (hanch04)
**          Removed reference to CI for the open source release.
**      11-Jan-2005 (wansh01)
**          INGNET 161 bug # 113737
**	    Supressed login info[uid,pwd] in vnodeless connection string  
**	    to display in Database field. 
**	01-Feb-2005 
**          correct win32 compiler error 	
**	28-Mar-2005 (lakvi01)
**	    Corrected function prototypes.
**      24-Apr-2007 (kiria01) b118184
**          Renamed IIhelp to IIhelptoggle to avoid naming conflict
**          runsys routines.
**
*/
GLOBALDEF QRB	*FS_qrb = NULL;
GLOBALREF bool	Runtime_INGRES;
FUNC_EXTERN	bool	IIUFver();
FUNC_EXTERN	VOID	IIUFdcl_DisplayCleanup();
FUNC_EXTERN	VOID	IIUFtfbTblFldBreak();
FUNC_EXTERN 	VOID    IIUFtfdTblFldDelete();
FUNC_EXTERN 	STATUS  IIUFtfeTblFldEdit();
FUNC_EXTERN 	VOID    IIUFtfiTblFldInsert();
FUNC_EXTERN	VOID	IIUFtfjTblFldJoin();
FUNC_EXTERN 	STATUS  IIUFtflTblFldLoad();
FUNC_EXTERN 	STATUS  IIUFtfuTblFldUnload();
FUNC_EXTERN     VOID    IIUFoosOnOutputSet();
FUNC_EXTERN     bool    IIUFoogOnOutputGet();
FUNC_EXTERN     VOID    FTfrminvis();
FUNC_EXTERN     VOID    FTfrmvis();
GLOBALREF 	ADF_CB	*FS_adfscb;
static STATUS	trap();
#define _Help_Expl	F_FE0100_ExplFrameHelp
#define _End_Expl	F_FE0102_ExplEnd
#define _Select_Expl	F_FE0108_ExplSelect
STATUS
FSrun ( bcb, dbname, dml )
BCB		*bcb;
  char *dbname;
DB_LANG	dml;	/* display DML */
{
	static READONLY
  char _Form[]= ERx("quelcmds"), _TabFld[]= ERx("qin"), _Column[]= ERx("quelin")
  ;
  char *editmenu;
	EX_CONTEXT	ctx;
	QRB		qrb;		/* QR control block structure */
	DB_DATA_VALUE	dv;		/* QRB output buffer structure */
	char		*qryerror;
	char		*err_cont;
	char		*msg;		/* Error message text */
	char		title[80];	/* the help title buffer */
	char		help_scr[80];	/* the help screen buffer */
	char		file_title[80];	/* the browser popup title */ 
	char		print_title[80];/* the browser popup title */
  i4 errnum;
	if ( IIUFgtfGetForm(IIUFlcfLocateForm(), _Form) != OK )
	{
		IIUGerr(E_MF0026_Bad_Get_Form, UG_ERR_ERROR, 0);
		return FAIL;
	}
# ifdef	PCINGRES
	editmenu = NULL;
# else
	editmenu = ERget(FE_Edit);
# endif	/* PCINGRES */
	/* allocate output buffer */
	dv.db_length = DB_MAXSTRING;
	qrb.qrptr = NULL;	/* bugs 2673,3269 */
	qrb.severity = 1;
        qrb.help_stmt = FALSE; 
	NMgtAt(ERx("II_TM_ON_ERROR"), &qryerror);
	if ((qryerror != NULL) && (*qryerror != EOS))
	{
		CVlower(qryerror);
		if (STcompare(qryerror,ERget(F_MF0039_continue)) == 0)
		{
			qrb.severity = 0;
		}
	}
	/* Check if this is a Runtime INGRES installation.  Note: the TM
	** treats fully restrictive and partially restrictive Runtime
	** INGRES in the same way - the TM doesn't allow tables to be created
	** (or dropped, if SQL) when running Runtime INGRES.
	*/
	Runtime_INGRES = FALSE;
	STcopy (ERx("fsoutput.hlp"), help_scr);
	STcopy (ERget(S_MF0002_FSTM_output_helptitle), title);
	/* Get the correct title based whether it is ISQL or IQUEL */
	STcopy (ERget
	   (dml==DB_SQL ? S_MF2103_SQL_print_title : S_MF2105_QUEL_print_title),
	    print_title);
	STcopy (ERget
	    (dml==DB_SQL ? S_MF2104_SQL_file_title : S_MF2106_QUEL_file_title), 
	     file_title);
/* # line 260 "fsrun.qsc" */	/* display */
  {
    if (IIdispfrm(_Form,(char *)"u") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
/* # line 263 "fsrun.qsc" */	/* host code */
		ER_MSGID	trim;
		char *pleft,*pright, *pcomma;
		int  len;
		char buffer[80];
/* # line 268 "fsrun.qsc" */	/* inittable */
      {
        if (IItbinit(_Form,_TabFld,(char *)"f") != 0) {
          IItfill();
        } /* IItbinit */
      }
/* # line 270 "fsrun.qsc" */	/* host code */
		trim = dml == DB_SQL ? F_MF000E_Enter_SQL_Statements
				: F_MF000F_Enter_QUEL_Statements;
		/*
		**    Display name of database on main form.
		*/
		/*
		**    for vnodeless connection, supress [uid,pwd] info 
		*/
		if (( dbname[0] == '@' ) &&
		    (pleft = STchr(dbname,'[')) &&
		    (pright = STchr(dbname,']')) && 
		    ((pleft) && (pcomma= STchr(pleft,','))))
		{
		    len = pcomma - dbname +1;
		    STncpy(buffer,dbname, len);
		    buffer[len]='\0';
		    STcat( buffer,"****");
		    STcat(buffer,pright);
		    STcpy(dbname,buffer);
		}
/* # line 291 "fsrun.qsc" */	/* putform */
      {
        if (IIfsetio((char *)0) != 0) {
          IIputfldio((char *)"database",(short *)0,1,32,0,dbname);
          IIputfldio((char *)"language",(short *)0,1,32,0,ERget(trim));
        } /* IIfsetio */
      }
/* # line 293 "fsrun.qsc" */	/* host code */
		IIUFoosOnOutputSet(FALSE);
		if ( EXdeclare(trap, &ctx) != OK )
		{
			/*
			** req_complete is also TRUE before the first
			** query is started, and TRUE again when no query
			** is still partially run.
			*/
			if (!bcb->req_complete)
			{
			    IIUFadd_AddRec(bcb,
				ERget(F_MF0010_Terminated_Interrupt), TRUE);
			    bcb->req_complete = TRUE;
			    IIUFbot_Bottom(bcb);
			    /* reset counts; 'RunToCompletion' may be
			    ** two down
			    */
			    EXinterrupt(EX_RESET);
			    if (IIUFoogOnOutputGet())
				goto output_done;	/* Display output */
			    else
				goto begin_input;	/* Display input form */
			}
			if (IIUFoogOnOutputGet())
			    IIUFdcl_DisplayCleanup();
			IIUFoosOnOutputSet(FALSE);
			goto begin_input;	/* Display input form */
		}
		EXinterrupt(EX_ON);
				/* default is interruptable; explicitly shut
				** interrupts off when necessary
				*/
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 338 "fsrun.qsc" */	/* host code */
		i4		nmrows;
		IIFSend_qry(&qrb,bcb);
		if ( (nmrows = FScount(_Form, _TabFld, _Column)) == 0 )
		{
			FTswinerr(ERget(S_MF001C_no_query_req_ignored), FALSE);
/* # line 345 "fsrun.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 346 "fsrun.qsc" */	/* host code */
		}
/* # line 348 "fsrun.qsc" */	/* message */
          {
            IImessage(ERget(S_MF001D_Run_the_request));
          }
/* # line 350 "fsrun.qsc" */	/* host code */
		bcb->req_complete = FALSE;
		bcb->eor_warning  = FALSE;
		IIUFoosOnOutputSet(TRUE);
		IIUFclb_ClearBcb(bcb);
		/* initialize DB_DATA_VALUE representing QRB output buffer*/
		dv.db_data = (PTR)MEreqmem( (u_i4)0, (u_i4) DB_MAXSTRING,
			FALSE, (STATUS *)0 );
		if (FSsetup(bcb, _Form, _TabFld, _Column, nmrows, &qrb, &dv)
			    != OK)
		{
			/* Unable to allocate more memory for query buffer */
			bcb->req_complete = TRUE;
/* # line 365 "fsrun.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 366 "fsrun.qsc" */	/* host code */
		}
		_VOID_ IIUFmro_MoreOutput(bcb, bcb->mx_rows-3, &qrb);
output_done:   ;
		/* 
		** Make the parent(input frame) invisible to allow the popups
		** to appear over the scrollable output
		*/
		FTfrminvis();	
		_VOID_ IIUFdsp_Display(bcb, &qrb, help_scr, title, TRUE,
			print_title, file_title, TRUE);
                /* b64679, INGCBT545 */
                if (qrb.help_stmt)
                {
                   IIhelptoggle(TRUE);
                   qrb.help_stmt = FALSE; 
                }
                else
                {
                   IIhelptoggle(FALSE);
                }
		FTfrmvis();
begin_input:   ;
        }
      } else if (IIretval() == 2) {
        {
/* # line 397 "fsrun.qsc" */	/* host code */
		if (bcb->nrows == 0)
		{
			FTswinerr(ERget(S_MF001E_No_active_request), FALSE);
		}
		else
		{
/* # line 403 "fsrun.qsc" */	/* message */
          {
            IImessage(ERget(S_MF001F_Resume_the_request));
          }
/* # line 404 "fsrun.qsc" */	/* host code */
			IIUFoosOnOutputSet(TRUE);
			/* 
			** Make the parent(input frame) invisible to allow the 
			** popups to appear over the scrollable output
			*/
			FTfrminvis();	
			_VOID_ IIUFdsp_Display(bcb, &qrb, help_scr, title, 
				TRUE, print_title, file_title, TRUE);
			FTfrmvis();
		}
        }
      } else if (IIretval() == 3) {
        {
/* # line 420 "fsrun.qsc" */	/* host code */
		i4		nmrows;
		IIFSend_qry(&qrb,bcb);
		if ( (nmrows = FScount(_Form, _TabFld, _Column)) == 0 )
		{
			FTswinerr(ERget(S_MF001C_no_query_req_ignored), FALSE);
/* # line 427 "fsrun.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 428 "fsrun.qsc" */	/* host code */
		}
/* # line 430 "fsrun.qsc" */	/* message */
          {
            IImessage(ERget(S_MF0003_Run_to_completion));
          }
/* # line 432 "fsrun.qsc" */	/* host code */
		bcb->req_complete = FALSE;
		bcb->eor_warning  = FALSE;
		IIUFoosOnOutputSet(TRUE);
		IIUFclb_ClearBcb(bcb);
		/* initialize DB_DATA_VALUE representing QRB output buffer*/
		dv.db_data = (PTR)MEreqmem( (u_i4)0, (u_i4) DB_MAXSTRING,
			FALSE, (STATUS *)0 );
		if (FSsetup(bcb, _Form, _TabFld, _Column, nmrows, &qrb, &dv)
			    != OK)
		{
			/* Unable to allocate more memory for query buffer */
			bcb->req_complete = TRUE;
/* # line 447 "fsrun.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 448 "fsrun.qsc" */	/* host code */
		}
		_VOID_ IIUFmro_MoreOutput(bcb, 0, &qrb);
		IIUFbot_Bottom(bcb);
		/* 
		** Make the parent(input frame) invisible to allow the 
		** popups to appear over the scrollable output
		*/
		FTfrminvis();
		_VOID_ IIUFdsp_Display(bcb, &qrb, help_scr, title, TRUE,
			print_title, file_title, TRUE);
		FTfrmvis();
        }
      } else if (IIretval() == 4) {
        {
/* # line 467 "fsrun.qsc" */	/* inittable */
          {
            if (IItbinit(_Form,_TabFld,(char *)"f") != 0) {
              IItfill();
            } /* IItbinit */
          }
        }
      } else if (IIretval() == 5) {
        {
/* # line 472 "fsrun.qsc" */	/* host code */
# ifndef	PCINGRES
		if (IIUFtfeTblFldEdit(_Form, _TabFld, _Column, &msg) != OK)
			IIUFpan_Panic(bcb, msg);
# endif	/* PCINGRES */
        }
      } else if (IIretval() == 6) {
        {
/* # line 481 "fsrun.qsc" */	/* host code */
		EXinterrupt(EX_OFF);
		for (;;)
		{
/* # line 485 "fsrun.qsc" */	/* submenu */
          {
            goto IImuI2;
IIfdB2:
            IIrunmu(0);
            if (IIretval() == 1) {
              {
/* # line 489 "fsrun.qsc" */	/* host code */
				if (IIUFtflTblFldLoad(_Form, _TabFld, _Column, 
					&msg) != OK)
					IIUFpan_Panic(bcb, msg);
				break;
              }
            } else if (IIretval() == 2) {
              {
/* # line 497 "fsrun.qsc" */	/* host code */
				bool	dummy;
				if (IIUFtfuTblFldUnload(_Form, _TabFld, _Column,
					&msg, &dummy) != OK)
					IIUFpan_Panic(bcb, msg);
				break;
              }
            } else if (IIretval() == 3) {
              {
/* # line 507 "fsrun.qsc" */	/* host code */
				FEhelp(ERx("fsfile.hlp"),
					ERget(S_MF0020_FSTM_File_help_banner));
				continue;
              }
            } else if (IIretval() == 4) {
              {
/* # line 514 "fsrun.qsc" */	/* host code */
				break;
              }
            } else {
              if (1) goto IIfdE2;
            } /* IIretval */
            goto IIfdE2;
IImuI2:
            IImuonly();
            if (IIinitmu() == 0) goto IIfdE2;
            if (IInmuact(ERget(FE_ReadFile),ERget(F_FE010D_ReadExpl),2,2,1) ==
             0) goto IIfdE2;
            if (IInmuact(ERget(FE_WriteFile),ERget(F_FE010E_WriteExpl),2,2,2) 
            == 0) goto IIfdE2;
            if (IInmuact(ERget(FE_Help),ERget(_Help_Expl),2,2,3) == 0) goto 
            IIfdE2;
            IIFRInternal(0);
            if (IInfrskact(1,(char *)0,2,2,3) == 0) goto IIfdE2;
            if (IInmuact(ERget(FE_End),ERget(_End_Expl),2,2,4) == 0) goto 
            IIfdE2;
            IIFRInternal(0);
            if (IInfrskact(3,(char *)0,2,2,4) == 0) goto IIfdE2;
            if (IIendmu() == 0) goto IIfdE2;
            goto IIfdB2;
IIfdE2:;
          }
/* # line 516 "fsrun.qsc" */	/* host code */
		}
		EXinterrupt(EX_ON);
        }
      } else if (IIretval() == 7) {
        {
/* # line 523 "fsrun.qsc" */	/* host code */
		EXinterrupt(EX_OFF);
		qrb.severity = FSonError(qrb.severity);
		EXinterrupt(EX_ON);
        }
      } else if (IIretval() == 8) {
        {
/* # line 531 "fsrun.qsc" */	/* host code */
		EXinterrupt(EX_OFF);
		for (;;)
		{
/* # line 535 "fsrun.qsc" */	/* submenu */
          {
            goto IImuI3;
IIfdB3:
            IIrunmu(0);
            if (IIretval() == 1) {
              {
/* # line 539 "fsrun.qsc" */	/* host code */
				IIUFtfiTblFldInsert(_Form, _TabFld);
				break;
              }
            } else if (IIretval() == 2) {
              {
/* # line 546 "fsrun.qsc" */	/* host code */
				IIUFtfdTblFldDelete(_Form, _TabFld);
				break;
              }
            } else if (IIretval() == 3) {
              {
/* # line 553 "fsrun.qsc" */	/* host code */
				IIUFtfbTblFldBreak(_Form, _TabFld, _Column);
				break;
              }
            } else if (IIretval() == 4) {
              {
/* # line 560 "fsrun.qsc" */	/* host code */
				IIUFtfjTblFldJoin(_Form, _TabFld, _Column);
				break;
              }
            } else if (IIretval() == 5) {
              {
/* # line 568 "fsrun.qsc" */	/* host code */
				FEhelp(ERx("fslinedt.hlp"),
					ERget(S_MF0028_LineEdit_help_banner));
				continue;
              }
            } else if (IIretval() == 6) {
              {
/* # line 577 "fsrun.qsc" */	/* host code */
				break;
              }
            } else {
              if (1) goto IIfdE3;
            } /* IIretval */
            goto IIfdE3;
IImuI3:
            IImuonly();
            if (IIinitmu() == 0) goto IIfdE3;
            if (IInmuact(ERget(FE_InsLine),ERget(F_FE0110_InsertExpl),2,2,1) 
            == 0) goto IIfdE3;
            if (IInmuact(ERget(FE_DelLine),ERget(F_FE0111_DeleteExpl),2,2,2) 
            == 0) goto IIfdE3;
            if (IInmuact(ERget(FE_SplitLine),ERget(F_FE0113_SplitExpl),2,2,3) 
            == 0) goto IIfdE3;
            if (IInmuact(ERget(FE_JoinLines),ERget(F_FE0112_JoinExpl),2,2,4) 
            == 0) goto IIfdE3;
            if (IInmuact(ERget(FE_Help),ERget(F_FE0101_ExplKeyHelp),2,2,5) == 
            0) goto IIfdE3;
            IIFRInternal(0);
            if (IInfrskact(1,(char *)0,2,2,5) == 0) goto IIfdE3;
            if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),2,2,6) == 0) 
            goto IIfdE3;
            IIFRInternal(0);
            if (IInfrskact(3,(char *)0,2,2,6) == 0) goto IIfdE3;
            if (IIendmu() == 0) goto IIfdE3;
            goto IIfdB3;
IIfdE3:;
          }
/* # line 579 "fsrun.qsc" */	/* host code */
		}
		EXinterrupt(EX_ON);
        }
      } else if (IIretval() == 9) {
        {
/* # line 586 "fsrun.qsc" */	/* host code */
	    EXinterrupt(EX_OFF);
	    FEhelp(dml == DB_SQL ? ERx("fssinput.hlp") : ERx("fsqinput.hlp"),
				    ERget(S_MF0021_FSTM_input_helpbanner));
	    EXinterrupt(EX_ON);
        }
      } else if (IIretval() == 10) {
        {
/* # line 594 "fsrun.qsc" */	/* host code */
		/* Top */
/* # line 595 "fsrun.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_Form,_TabFld,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,1) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
        }
      } else if (IIretval() == 11) {
        {
/* # line 600 "fsrun.qsc" */	/* host code */
		/* Bottom */
/* # line 601 "fsrun.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_Form,_TabFld,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,-1) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
        }
      } else if (IIretval() == 12) {
        {
/* # line 608 "fsrun.qsc" */	/* host code */
		/* flush out any resident unfinished queries */
		/* inMST - no need for a routine for this -  now defunct */
		IIFSend_qry(&qrb,bcb);
		IIUFclb_ClearBcb(bcb);
		/* BEGIN FIX FOR BUG 7149 */
		/* Use ADF_CB in case '\sql' or '\quel' switched the DML
		** (even though such switching is not supported.)
		*/
		if (FS_adfscb->adf_qlang == DB_QUEL)
		{
		    if (FEinMST())
		    {
			if (!IIUFver(ERget(S_MF0022_in_a_trans_wanna_quit), 0))
/* # line 624 "fsrun.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 626 "fsrun.qsc" */	/* host code */
			/* put out a message that the transaction
			** has been aborted... (especially if TM
			** was run from a script and inadvertantly
			** quit with an abort, so there's some
			** output about it.)
			*/
/* # line 687 "fsrun.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 688 "fsrun.sc" */	/* host code */
/* # line 633 "fsrun.qsc" */	/* message */
          {
            IImessage(ERget(S_MF2101_Pending_trans_aborted));
          }
/* # line 634 "fsrun.qsc" */	/* host code */
		     }
		}
		/* END FIX FOR BUG 7149 */
		/* BUG 5631 - ALWAYS COMMIT when exiting if language was SQL.*/
		else	/* assume SQL */
		{
/* # line 699 "fsrun.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 700 "fsrun.sc" */	/* host code */
			/*
			** Make sure commit was successful before
			** displaying "Your SQL statement(s) have been
			** committed." message. (bug 9141)
			*/
/* # line 649 "fsrun.qsc" */	/* inquire_ingres */
          {
            IIeqiqio((short *)0,1,30,4,&errnum,(char *)"ERRORNO");
          }
/* # line 650 "fsrun.qsc" */	/* host code */
			if (errnum == 0)
			{
/* # line 652 "fsrun.qsc" */	/* message */
          {
            IImessage(ERget(S_MF2102_SQL_stmt_committed));
          }
/* # line 653 "fsrun.qsc" */	/* host code */
			}
		}
/* # line 655 "fsrun.qsc" */	/* breakdisplay */
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
    goto IIfdE1;
IImuI1:
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Go),ERget(S_MF001D_Run_the_request),2,2,1) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Resume),ERget(S_MF001F_Resume_the_request),2,2,2) ==
     0) goto IIfdE1;
    if (IInmuact(ERget(F_MF0011_Complete),ERget(S_MF0003_Run_to_completion),2,
    2,3) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Blank),ERget(S_MF0051_ExplBlank),2,2,4) == 0) goto 
    IIfdE1;
    if (IInmuact(editmenu,ERget(S_MF0052_ExplEdit),2,2,5) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_File),ERget(S_MF0053_ExplFile),2,2,6) == 0) goto 
    IIfdE1;
    if (IInmuact(ERget(F_MF0030_OnError),ERget(S_MF0040_ExplOnError),0,2,7) ==
     0) goto IIfdE1;
    if (IInmuact(ERget(FE_LineEdit),ERget(F_FE010F_LineEditExpl),2,2,8) == 0) 
    goto IIfdE1;
    if (IInmuact(ERget(FE_Help),ERget(_Help_Expl),0,2,9) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,0,2,9) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,10) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,11) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Quit),ERget(S_MF0054_ExplQuit),2,2,12) == 0) goto 
    IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(2,(char *)0,0,2,12) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,12) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 658 "fsrun.qsc" */	/* host code */
	EXdelete();
	return OK;
}
/**
** Name:	FSonError -	Interactive Terminal Monitor On_Error option.
**
** Description:
**	Contains the routine that runs the pop-up form for the On_Error option
**	frame of the main FSrun frame.
**
**	FSonError()
**
** History:
**	24-apr-89 (kathryn). Created.
**/
STATUS
FSonError(err_terminate)
i4	err_terminate;
{
	static READONLY
  char _errForm[]= ERx("onErrForm"), _errTbl[]= ERx("errchoice"), _errAct[]= ERx("erraction")
  , _errDesc[]= ERx("errdescript"), _errCurr[]= ERx("errcurr"), _errNew[]= ERx("errnew")
  ;
	i4	err_term;
  i4 curr_row;
	i4	last_row = 2;
	err_term = err_terminate;
	if ( IIUFgtfGetForm(IIUFlcfLocateForm(), _errForm) != OK )
	{
		IIUGerr(E_MF0026_Bad_Get_Form, UG_ERR_ERROR, 0);
		return FAIL;
	}
/* # line 697 "fsrun.qsc" */	/* display */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    if (IIdispfrm(_errForm,(char *)"r") == 0) goto IIfdE4;
    goto IImuI4;
IIfdI4:;
    if (IIfsetio((char *)0) == 0) goto IIfdE4;
    {
/* # line 700 "fsrun.qsc" */	/* inittable */
      {
        if (IItbinit(_errForm,_errTbl,(char *)"f") != 0) {
          IItfill();
        } /* IItbinit */
      }
/* # line 701 "fsrun.qsc" */	/* loadtable */
      {
        if (IItbact(_errForm,_errTbl,1) != 0) {
          IItcoputio((char *)"erraction",(short *)0,1,32,0,
          ERget(S_MF0031_TERMINATE));
          IItcoputio((char *)"errdescript",(short *)0,1,32,0,
          ERget(S_MF0037_Choice_Terminate));
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 705 "fsrun.qsc" */	/* loadtable */
      {
        if (IItbact(_errForm,_errTbl,1) != 0) {
          IItcoputio((char *)"erraction",(short *)0,1,32,0,
          ERget(S_MF0032_CONTINUE));
          IItcoputio((char *)"errdescript",(short *)0,1,32,0,
          ERget(S_MF0038_Choice_Continue));
          IITBceColEnd();
        } /* IItbact */
      }
/* # line 710 "fsrun.qsc" */	/* host code */
		if (err_terminate == 1)		
		{
/* # line 712 "fsrun.qsc" */	/* putform */
      {
        if (IIfsetio(_errForm) != 0) {
          IIputfldio((char *)"errcurr",(short *)0,1,32,0,
          ERget(S_MF0034_Curr_Terminate));
        } /* IIfsetio */
      }
/* # line 713 "fsrun.qsc" */	/* putform */
      {
        if (IIfsetio(_errForm) != 0) {
          IIputfldio((char *)"errnew",(short *)0,1,32,0,
          ERget(S_MF0035_Select_Continue));
        } /* IIfsetio */
      }
/* # line 714 "fsrun.qsc" */	/* scroll */
      {
        if (IItbsetio(1,_errForm,_errTbl,-3) != 0) {
          IItbsmode((char *)"to");
          if (IItscroll(0,1) != 0) {
          } /* IItscroll */
        } /* IItbsetio */
      }
/* # line 715 "fsrun.qsc" */	/* resume */
      {
        IIresfld(_errTbl);
        if (1) goto IIfdB4;
      }
/* # line 716 "fsrun.qsc" */	/* host code */
		}
		else
		{
/* # line 719 "fsrun.qsc" */	/* putform */
      {
        if (IIfsetio(_errForm) != 0) {
          IIputfldio((char *)"errcurr",(short *)0,1,32,0,
          ERget(S_MF0033_Curr_Continue));
        } /* IIfsetio */
      }
/* # line 720 "fsrun.qsc" */	/* putform */
      {
        if (IIfsetio(_errForm) != 0) {
          IIputfldio((char *)"errnew",(short *)0,1,32,0,
          ERget(S_MF0036_Select_Terminate));
        } /* IIfsetio */
      }
/* # line 721 "fsrun.qsc" */	/* scroll */
      {
        if (IItbsetio(1,_errForm,_errTbl,-3) != 0) {
          IItbsmode((char *)"to");
          if (IItscroll(0,2) != 0) {
          } /* IItscroll */
        } /* IItbsetio */
      }
/* # line 722 "fsrun.qsc" */	/* resume */
      {
        IIresfld(_errTbl);
        if (1) goto IIfdB4;
      }
/* # line 723 "fsrun.qsc" */	/* host code */
		}
    }
IIfdB4:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 728 "fsrun.qsc" */	/* inquire_frs */
          {
            if (IIiqset(3,0,_errForm,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&curr_row,29,_errTbl,0);
            } /* IIiqset */
          }
/* # line 729 "fsrun.qsc" */	/* host code */
		if (curr_row == 1)
			err_terminate = 1;
		else
			err_terminate = 0;
/* # line 733 "fsrun.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE4;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 739 "fsrun.qsc" */	/* host code */
		err_terminate = 1;
/* # line 740 "fsrun.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE4;
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 745 "fsrun.qsc" */	/* host code */
		err_terminate = 0;
/* # line 746 "fsrun.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE4;
          }
        }
      } else if (IIretval() == 4) {
        {
/* # line 750 "fsrun.qsc" */	/* host code */
	       FEhelp(ERx("fserr.hlp"), ERget(S_MF0024_OnError_help_banner));
		continue;
        }
      } else if (IIretval() == 5) {
        {
/* # line 756 "fsrun.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE4;
          }
        }
      } else if (IIretval() == 6) {
        {
/* # line 760 "fsrun.qsc" */	/* inquire_frs */
          {
            if (IIiqset(3,0,_errForm,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&curr_row,29,_errTbl,0);
            } /* IIiqset */
          }
/* # line 761 "fsrun.qsc" */	/* host code */
		if (curr_row < last_row)
		{
/* # line 763 "fsrun.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB4;
          }
/* # line 764 "fsrun.qsc" */	/* host code */
		}
		else
		{
/* # line 767 "fsrun.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_errForm,_errTbl,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,1) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
/* # line 768 "fsrun.qsc" */	/* host code */
		}
        }
      } else if (IIretval() == 7) {
        {
/* # line 772 "fsrun.qsc" */	/* inquire_frs */
          {
            if (IIiqset(3,0,_errForm,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&curr_row,29,_errTbl,0);
            } /* IIiqset */
          }
/* # line 773 "fsrun.qsc" */	/* host code */
		if (curr_row == 1)
		{
/* # line 775 "fsrun.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_errForm,_errTbl,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,-1) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
/* # line 776 "fsrun.qsc" */	/* host code */
		}
		else
		{
/* # line 779 "fsrun.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB4;
          }
/* # line 780 "fsrun.qsc" */	/* host code */
		}
        }
      } else {
        if (1) goto IIfdE4;
      } /* IIretval */
    } /* IIrunform */
IIfdF4:
    if (IIchkfrm() == 0) goto IIfdB4;
    goto IIfdE4;
IImuI4:
    if (IIinitmu() == 0) goto IIfdE4;
    if (IInmuact(ERget(FE_Go),ERget(_Select_Expl),2,2,1) == 0) goto IIfdE4;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE4;
    if (IInmuact(ERget(F_MF0031_Terminate),ERget(S_MF0042_ExplTerminate),2,2,2)
     == 0) goto IIfdE4;
    if (IInmuact(ERget(F_MF0032_Continue),ERget(S_MF0043_ExplContinue),2,2,3) 
    == 0) goto IIfdE4;
    if (IInmuact(ERget(FE_Help),ERget(_Help_Expl),2,2,4) == 0) goto IIfdE4;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,4) == 0) goto IIfdE4;
    if (IInmuact(ERget(FE_End),ERget(_End_Expl),2,2,5) == 0) goto IIfdE4;
    IIFRInternal(0);
    if (IInfrskact(3,(char *)0,2,2,5) == 0) goto IIfdE4;
    if (IIactscrl(_errTbl,0,6) == 0) goto IIfdE4;
    if (IIactscrl(_errTbl,1,7) == 0) goto IIfdE4;
    if (IIendmu() == 0) goto IIfdE4;
    goto IIfdI4;
IIfdE4:;
    IIendfrm();
  }
/* # line 782 "fsrun.qsc" */	/* host code */
	return (err_terminate);
}
/***********************************************************************
**								      **
**    trap  --  Capture PA1 and abort active request		      **
**								      **
**    Parameters:						      **
**	none							      **
**								      **
***********************************************************************/
static STATUS
trap ( ex )
EX_ARGS *ex;
{
	if (ex->exarg_num == EXINTR)
	{
		IIresync();
		/*
		** take care of case where QR has started a transaction itself
		** internally, such as in HELP ALL before the OPEN CURSOR
		*/
		/*
		** NOTE: The call to FEinMST() is here temporarily to work
		** around a 6.0 BE bug, in which if immediately after a
 		** BEGIN TRANSACTION statement is completed, an interrupt is
		** sent to the BE, the BEGIN TRANSACTION appears to get backed
		** out.  The FEinMST ensures that in fact a transaction is
		** active and the ABORT will not fail.
		*/
		if ( FS_qrb != NULL && FS_qrb->endtrans && FEinMST() )
/* # line 1074 "fsrun.sc" */	/* rollback */
  {
    IIsqInit((char *)0);
    IIxact(2);
  }
/* # line 1075 "fsrun.sc" */	/* host code */
		/*
		** reset FS_qrb so that if another interrupt occurs before the
		** next call to QR, we won't try to look at a meaningless QRB.
		*/
		FS_qrb = NULL;
		return EXDECLARE;
	}
	return EXRESIGNAL;
}
/*
** Name:  	IIFSend_qry - Flush out any resident unfinished queries.
**
** Description:
**	Flush out any resident unfinished queries.  If a request was begun, 
**	but not completed (because all output had not been scrolled through),
**	end the request. If the query was a HELP statement, and needs to
**	be committed then do so.  Otherwise, call IIbreak() to interrupt
**	the query.
**	
**
** History:
**	24-may-1989 (kathryn) created. -- Code Cleanup -- 
**		Moved this code from fsrun(), where it was repeated under
**		most menu items. Bugs 2673,3269.
**		Moved IIbreak() from FSsetup() so only called when
**		necessary.
**      06-nov-1990 (kathryn) Bug 34247
**              Always call IIbreak() when query was begun but not completed.
**
*/
VOID
IIFSend_qry (qrb,bcb)
QRB	*qrb;
BCB	*bcb;
{
	if (bcb->req_begin && !bcb->req_complete)
	{
		IIbreak();
		if (qrb->qrptr) 
		{
			if (qrb->endtrans)
			{
/* # line 1117 "fsrun.sc" */	/* commit */
  {
    IIsqInit((char *)0);
    IIxact(3);
  }
/* # line 1118 "fsrun.sc" */	/* host code */
				qrb->endtrans = FALSE;
			}
			FS_qrb = NULL;
			qrb->qrptr = NULL;
		}
	}
}
