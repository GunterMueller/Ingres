# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<cv.h>		 
# include	<me.h>
# include	<st.h>
# include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include <ui.h>
/* this tells MING about ui.qsh dependency.
#include	<ui.h>
*/
# include	<erfe.h>
# include	<ug.h>
# include	<iisqlda.h>		 
# include	"ertu.h"
# include	"tuconst.h"
# include <uigdata.h>
# include       <lo.h>
# include       <uf.h>
/* this tells MING about uigdata.qsh dependency.
#include        <uigdata.h>
*/
/**
** Name:	creattbl.qsc -	Create table screen of Tables Utility.
**
** Description:
**	This module is called when the "Create" menu item is selected.
**	This file defines:
**
**	newtable()	routine used to drive "Create" table screen.
**
** History:
**	6/04/85		(prs)	Fix bug 5190.  Lowercase table names.
**	1/15/86		(prs)	Add refs to SQL help file.
**	25-mar-1987	(daver) Modified for Tables Utility use.
**	22-oct-1987 (peter)
**		Change 'y' and 'n' and change FEmsg to IIUGerr.
**	1-dec-1987 (peter)
**		Change call to tugetattr.
**	6/88 (bobm)	pass in title of form.
**	11/2/88 (pete)	changed to use new IIUIrel_help, rather than tutabbas.
**	2-feb-1989 (danielt)
**		tok out calls to IIUIxconn*() (not needed by DG anymore)
**	07-dec-1989 (seng)
**		Need to compile without optimization for PS/2. Symptoms
**		are dumping core upon exit of the "tables" utility form.
**      12-dec-1989     (pete)  Remove menuitems Top, Bottom and Find;
**			Add (validate=0, activate=0) to Help, End and Quit.
**	29-dec-1989	(pete)  Add Lookup menuitem for help on Datatypes.
**			Also, change the unique keys popup to use ListPick.
**			(and remove need for file "tukeys.frm")
**	1/8/90	(pete)	Set Defaultability and Nullability values to correct
**			default values when user creates a Logical key.
**	3/2/90	(pete)	Changed menuitems from Save/End to End/Cancel.
**	9/11/90 (owen)	Fix bug where cursor did not resume to top of table
**			form after selecting the save option. Fix taken from
**			Pyramid port. No change number (yet)...
**	31-oct-1990 (pete)
**		Fix bug 34244 - ListPick for datatypes doesn't work on
**		Gateways.
**	11/07/90 (emerson)
**		If table name is an SQL keyword, put out an error message
**		instead sending the back-end a messed-up CREATE TABLE
**		(bug 33846).
**	17-aug-91 (leighb) DeskTop Porting Change: include iisqlda.h
**	15-nov-91 (leighb) DeskTop Porting Change: 
**		added comment where HiC compiler generates bad code.
**	17-sep-1992 (mgw)
**		Took out tuprmptlst() - not used. Also Added new Defaults
**		handling.
**	12-jan-1993 (rogerl)
**		tblname might be del id, size now reflects; change \0 to EOS
**		Delete IIUGIsSQLKeyWord cruft.
**	21-jan-1993 (mgw)
**		Report the proper error when an illegal table name is given
**		to the GetTableDef prompt.
**	06-apr-1993 (rogerl)
**		Add hidden column defaults_lkey for keying defaults values in
**		defaults list.  Each row with defaults entry has a unique
**		value for that key which corresponds to the 'key' field in
**		TU_DLE.
**	16-apr-1993 (rogerl)
**		Check for running >= 6.5 && not gateways before putting up
**		new defaults listchoices goodies. (51040)
**	16-apr-1993 (rogerl)
**		Properly word error message if EditDefaults is chosen while
**		the cursor is not on the table field. (50751)
**	19-apr-1993 (rogerl)
**		Defaults 'suggestions' need to start out with NULLs == yes
**		and DEFAULT == null (49208).  'nullable' is now passed to
**		setdefstate as a ER_MSGID instead of char *.  Delete QUEL
**		anachronisms.
**	20-apr-1993 (rogerl)
**		Backward compatability: don't create 6.5 only (defaults value)
**		SQL syntax if connected to 6.4 or lower (50137).  Also, want
**		'defaults' to show 'n/a' if connected to 6.4, and make defaults
**		field read-only.  delete setdefstate().  Close logic hole
**		where attempt to create NOT NULL DEFAULT NULL would be sent to
**		DBMS in some cases.
**	23-apr-1993 (rogerl)
**		Also disable EditDefaults if connected to 6.4 DBMS.
**	7-jun-1993 (rogerl)
**		If user 'returns' out of the defaults field it is normally
**		replaced with its initial value.  Check to see if the user had
**		put 'no' for Nulls, and if so, emit error message if user
**		doesn't change the Defaults field to something other than
**		'null', and place them back on the field (52487).
**	27-aug-1993 (mgw) Bug #54557
**		Added error message E_TU000F_Bad_Defaultability to explain
**		about proper defaultability values when user enters an illegal
**		value in the Defaults column.
**	20-sep-1993 (mgw)
**		Add Datatypes BYTE, BYTE VARYING, LONG BYTE, and LONG VARCHAR
**		to the datatypes listpick. Also, the previous change for bug
**		54557 made it so user had to type over the previous value of
**		the defaultability indicator in order to get to the next row.
**		This fixes it so a carriage return in the defaultability
**		column just restores the previous defaultability and continues.
**	6-oct-1993 (rogerl)
**		lp_buf was used to store uniq key as well as defaults values.
**		If both had been 'loaded', then we really didn't know which
**		the buffer contained; confusion reigned.  Spend a couple'o
**		bytes for a separate ptr.  (54558)
**	08-oct-1993 (rogerl)
**		User may type owner specification (owner.table) into popup;
**		to ensure correct casing etc., ChkDlm must be applied as to
**		any other identifier. (54943)
**	08-oct-1993 (rogerl/rdrane)
**		Richard points out that identifier buffers may be overrun;
**		make sure they're as big as can be.
**	5-jan-1994 (mgw) Bug #58340
**		Be sure to RESUME after error E_TU0010 when user tries to
**		EditDefaults from the table name field (not on a column).
**	10-feb-1994 (mgw) Bug #59596
**		Can't have null default on a column with no nulls specified
**		so change the default defaultability to yes in that case.
**	08-apr-1994 (mgw) Bug #62186
**		Make Key columns default to 'no' Nullability and 'no'
**		Defaultability.
**      21-mar-94 (smc) Bug #60829
**              Added #include header(s) required to define types passed
**              in prototyped external function calls.
**	20-may-1997 (canor01)
**	    Cleaned up compiler warnings from CL prototypes.
**	10-may-1999 (walro03)
**		Remove obsolete version string ps2_us5.
**      02-Jul-1999 (hweho01)
**              Included uf.h header file, it contains the function     
**              prototype of IIUFlcfLocateForm(). Without the explicit
**              declaration, the default int return value for a  
**              function will truncate a 64-bit address on ris_u64 
**              platform.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
FUNC_EXTERN	i4	FEinqerr();
FUNC_EXTERN	DB_STATUS  IIUIrel_help();
FUNC_EXTERN	i4	tugetattr();
FUNC_EXTERN	STATUS	parsecreate();
FUNC_EXTERN	STATUS	tuchkname();
FUNC_EXTERN	i4	IIUIdml();
FUNC_EXTERN	VOID	FEdml() ;
FUNC_EXTERN     char    *IITUprTablePrompt();
FUNC_EXTERN     i4      IITUclkCheckLogKey();
FUNC_EXTERN     char    *IITUedEditDefault( i4  *dkey );
FUNC_EXTERN	VOID	IITUcdlClearDefaultList();
FUNC_EXTERN	ER_MSGID   IITUvunValueUserNull(char *response);
static bool	crtformready = FALSE;
static bool	keyformready = FALSE;
static bool	dfltformready = FALSE;
static char	*uniq_buf ;	/* buffer of unique key 
				** string for ListPick
				*/
static char	*defl_buf ;	/* buffer of default
				** string for ListPick
				*/
static char 	*load_lptypes();
static char	*load_lp();
static char	*tugetDtype();
static READONLY
  char _Form[]= ERx("tucreatefr"), _Table[]= ERx("table"), _Name[]= ERx("name")
  , _Tblfld[]= ERx("attributes"), _Key[]= ERx("key"), _Nullable[]= ERx("nullable")
  , _Defable[]= ERx("defable"), _KeysSet[]= ERx("keys_set"), _Deflkey[]= ERx("default_lkey")
  , _DataFmt[]= ERx("datafmt"), _Blank[]= ERx("");
static LISTVALS ukeys[] = {
	{F_TU0034_Row1_Col1,	F_TU0035_Row1_Col2},
	{F_TU0036_Row2_Col1,	F_TU0037_Row2_Col2},
};
#define uk_cnt (sizeof(ukeys)/sizeof(LISTVALS))
static DTYPE datatypes[] = {
	/* datatype,          OPEN/SQL? */
	{F_TU005A_Varchar,	TRUE},
	{F_TU005B_Char,		TRUE},
	{F_TU005C_Integer,	TRUE},
	{F_TU005D_Smallint,	TRUE},
	{F_TU0067_Integer1,	FALSE},
	{F_TU005E_Float,	TRUE},
	{F_TU0068_Float4,	FALSE},
	{F_TU005F_Date,		TRUE},
	{F_TU0060_Money,	FALSE},
	{F_TU0009_Decimal,	FALSE},
	{F_TU0086_Byte,		FALSE},
	{F_TU0087_ByteVarying,	FALSE},
	{F_TU0088_LongByte,	FALSE},
	{F_TU0089_LongVarchar,	FALSE},
	{F_TU0061_TblKeySysMnt, FALSE},
	{F_TU0062_TblKeyNoSysMnt,FALSE},
	{F_TU0065_ObjKeySysMnt, FALSE},
	{F_TU0066_ObjKeyNoSysMnt,FALSE}
};
#define dt_cnt (sizeof(datatypes)/sizeof(DTYPE))
static LISTVALS dfltlist[] = {
	{F_UG0007_No2,		F_TU0080_No_Desc},
	{F_UG0002_Yes2,		F_TU0081_Yes_Desc},
	{F_TU0079_User2,	F_TU0082_User_Desc},
	{F_TU007D_Null2,	F_TU0083_Null_Desc},
	{F_TU0074_Value2,	F_TU0084_Value_Desc}
};
#define dflt_cnt (sizeof(dfltlist)/sizeof(LISTVALS))
static char *lp_dtbuf ;	/* buffer of data types for ListPick */
static bool lpTypesReady = FALSE;	/* Has ListPick list of datatypes
					** been built yet?
					*/
VOID
  newtable(lasttable, title_prefix)
char	*lasttable;	/* set to name of successfully created table */
char	*title_prefix;	/* optional prefix for screen title */
  {
	ER_MSGID	nulldef;
    char *defdef;
    char title[TU_TITLE_SIZE +1];
    i4 state;
    i4 keys_set;
    char objname[FE_MAXNAME + 1];
    char mvformat[50];
    char mvnullable[6];
    char mvdefault[7];
    char startdefault[7];
    char keyno[5];
	char	*defval;
	if ( !crtformready )
	{
		if ( IIUFgtfGetForm(IIUFlcfLocateForm(), _Form) != OK )
		{
			IIUGerr( E_TU0041_Get_Form_Failed, UG_ERR_ERROR,
					1, _Form
			);
			return;
		}
		crtformready = TRUE;
	}
/* # line 286 "creattbl.qsc" */	/* display */
    {
      if (IIdispfrm(_Form,(char *)"f") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 289 "creattbl.qsc" */	/* host code */
		lasttable[0] = EOS;
		IITUcdlClearDefaultList();
		nulldef = F_UG0002_Yes2;
			/* n/a is only allowable response for < 6.5 */
			/* or gateways */
    		if ( ( STcompare( IIUIscl_StdCatLevel(), UI_LEVEL_65 ) >= 0 )
		   && ( IIUIdml() != UI_DML_GTWSQL ) )
		    defdef = ERget(F_TU007D_Null2);
		else
		    defdef = ERget(F_TU0001_n_a);
		/* note that defaults_lkey field is populated with
		** values from calls to tugetattr()
		*/
/* # line 307 "creattbl.qsc" */	/* inittable */
        {
          if (IItbinit(_Form,_Tblfld,(char *)"f") != 0) {
            IIthidecol(_KeysSet,(char *)"i4");
            IIthidecol(_Deflkey,(char *)"integer");
            IItfill();
          } /* IItbinit */
        }
/* # line 310 "creattbl.qsc" */	/* host code */
       		IITUstlSetTitle (title_prefix, ERget(F_TU0031_Create), title);
/* # line 311 "creattbl.qsc" */	/* putform */
        {
          if (IIfsetio(_Form) != 0) {
            IIputfldio((char *)"utitle",(short *)0,1,32,0,title);
          } /* IIfsetio */
        }
/* # line 313 "creattbl.qsc" */	/* host code */
		if ( ( IIUIdml() == UI_DML_GTWSQL )
		   ||
		   ( STcompare( IIUIscl_StdCatLevel(), UI_LEVEL_65 ) < 0 ) )
		{ /* in gateway SQL, default value info is not valid, so
		     don't allow user to enter it (ditto < 6.5) */
/* # line 318 "creattbl.qsc" */	/* set_frs */
        {
          if (IIiqset(4,0,_Form,_Tblfld,(char *)0) != 0) {
            IIstfsio(21,_Defable,0,(short *)0,1,32,0,(char *)"r");
          } /* IIiqset */
        }
/* # line 319 "creattbl.qsc" */	/* host code */
		}
/* # line 320 "creattbl.qsc" */	/* resume */
        {
          IIresfld(_Table);
          if (1) goto IIfdB1;
        }
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 325 "creattbl.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_Form,_Tblfld,-2) != 0) {
                IItcogetio((short *)0,1,30,4,&state,(char *)"_STATE");
                IItcogetio((short *)0,1,32,49,mvformat,_DataFmt);
                IItcogetio((short *)0,1,32,4,keyno,_Key);
                IItcogetio((short *)0,1,32,5,mvnullable,_Nullable);
                IItcogetio((short *)0,1,32,6,mvdefault,_Defable);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 332 "creattbl.qsc" */	/* host code */
		/*
		** Display the default nullable/defaultable settings if
		** we've added a new row, or if we've just inserted a row.
		*/
		if ( (state == stNEW &&
			mvnullable[0] == EOS && mvdefault[0] == EOS) ||
		     (mvformat[0] == EOS && keyno[0] == EOS &&
			mvnullable[0] == EOS && mvdefault[0] == EOS) )
		{
/* # line 341 "creattbl.qsc" */	/* putrow */
            {
              if (IItbsetio(3,_Form,_Tblfld,-2) != 0) {
                IItcoputio(_Nullable,(short *)0,1,32,0,ERget(nulldef));
                IItcoputio(_Defable,(short *)0,1,32,0,defdef);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 343 "creattbl.qsc" */	/* host code */
		}
/* # line 345 "creattbl.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 350 "creattbl.qsc" */	/* host code */
            if (IIUIdml() != UI_DML_GTWSQL)
            {
                /* Assertion: we're running against Ingres */
                /* check if user entered a Logical key data type WITH
                ** system_maintained. If so, set Nullability and Defaultability
                ** for the column to required values.
                */
/* # line 358 "creattbl.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_Form,_Tblfld,-2) != 0) {
                IItcogetio((short *)0,1,32,49,mvformat,_DataFmt);
                IItcogetio((short *)0,1,30,4,&keys_set,_KeysSet);
                IItcogetio((short *)0,1,30,4,&state,(char *)"_STATE");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 362 "creattbl.qsc" */	/* host code */
                if ( state != 2 && keys_set != 1 )
                {
                    /* changed or new row that we haven't yet set the
                    ** Null, Default info for.
                    */
                    char dmy[22+1];  /*sizeof("with system_maintained")+1*/
                    if (IITUclkCheckLogKey(mvformat, dmy) == TU_LOGKEY_WITH)
                    {
                        /* Assertion: Valid logical key
                        ** "WITH system_maintained".  Set Null to "No"
                        ** and Defaults to "Yes" -- these are required for
                        ** "WITH system_maintained" logical keys.
                        */
                        STcopy(ERget(F_UG0007_No2),  mvnullable);
                        STcopy(ERget(F_UG0002_Yes2), mvdefault);
/* # line 380 "creattbl.qsc" */	/* putrow */
            {
              if (IItbsetio(3,_Form,_Tblfld,-2) != 0) {
                IItcoputio(_Nullable,(short *)0,1,32,0,mvnullable);
                IItcoputio(_Defable,(short *)0,1,32,0,mvdefault);
                IItcoputio(_KeysSet,(short *)0,1,30,sizeof(1),(void *)IILQint(
                1));
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 385 "creattbl.qsc" */	/* host code */
                    }
                }
            }
/* # line 389 "creattbl.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 394 "creattbl.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_Form,_Tblfld,-2) != 0) {
                IItcogetio((short *)0,1,32,4,keyno,_Key);
                IItcogetio((short *)0,1,30,4,&keys_set,_KeysSet);
                IItcogetio((short *)0,1,32,6,mvdefault,_Defable);
                IItcogetio((short *)0,1,30,4,&state,(char *)"_STATE");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 400 "creattbl.qsc" */	/* host code */
		if (keyno[0] != EOS)	/* is keyno NULL ? */
		{
		    STATUS	rval;
		    i4		knum;
		    rval = CVan(keyno,&knum);	/* not necessary if NULLABLE */
		    if (rval != OK || knum <= 0)
		    {
			IIUGerr(E_TU0001_Bad_Key_Number, UG_ERR_ERROR,
				(i4) 1, (PTR) keyno);
/* # line 410 "creattbl.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 411 "creattbl.qsc" */	/* host code */
		    }
		    CVna(knum,keyno);
		    if ( state != 2 && keys_set != 1 )
		    {
			/* Only run this block for changed rows and only
		     	** run it one time for each row ("_KeysSet" indicates
			** if it has been run before).
			**
		        ** If key column, then reset default Nullability
		        ** to be "no" (CJDate says keys shouldn't take on Null
			** values; User can override if they insist on
			** bad design!)
			*/
		        STcopy(ERget(F_UG0007_No2),mvnullable);
			/* Bug 62186 - make key column 'no' NULLS, 'no' Dflt */
			if ( !STequal(mvdefault, ERget(F_UG0007_No2))
				&& IIUIdml() != UI_DML_GTWSQL )
			{
		            STcopy(ERget(F_UG0007_No2),mvdefault);
			}
/* # line 434 "creattbl.qsc" */	/* putrow */
            {
              if (IItbsetio(3,_Form,_Tblfld,-2) != 0) {
                IItcoputio(_Nullable,(short *)0,1,32,0,mvnullable);
                IItcoputio(_Defable,(short *)0,1,32,0,mvdefault);
                IItcoputio(_KeysSet,(short *)0,1,30,sizeof(1),(void *)IILQint(
                1));
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 439 "creattbl.qsc" */	/* host code */
		    }
		}
/* # line 442 "creattbl.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 447 "creattbl.qsc" */	/* host code */
		ER_MSGID	nullable;
		STATUS		retstat;
/* # line 450 "creattbl.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_Form,_Tblfld,-2) != 0) {
                IItcogetio((short *)0,1,32,4,keyno,_Key);
                IItcogetio((short *)0,1,32,5,mvnullable,_Nullable);
                IItcogetio((short *)0,1,32,6,mvdefault,_Defable);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 455 "creattbl.qsc" */	/* host code */
		_VOID_ IIUGyn(mvnullable, &retstat);
		if (retstat == E_UG0004_Yes_Response)
		{
			nullable = F_UG0002_Yes2;
		}
		else if (retstat == E_UG0005_No_Response)
		{
			nullable = F_UG0007_No2;
			/*
			** Fix bug #59596 - can't have null default on a
			** column with no nulls so change default to yes here.
			*/
			if (STequal(mvdefault, ERget(F_TU007D_Null2)))
			{
				STcopy(ERget(F_UG0002_Yes2), mvdefault);
			}
		}
		else
		{
			/* Nullable != (Yes or No). Probably empty. */
			i4	knum;
			_VOID_ CVan(keyno, &knum);
			/* bug 4011: if blank out Null column by hitting Return,
			** then set to "No" if Key column is set (i.e. don't
			** always set based on Query language default:
			** "nulldef").
			*/
			nullable = (knum > 0) ? F_UG0007_No2 : nulldef;
		}
/* # line 487 "creattbl.qsc" */	/* putrow */
            {
              if (IItbsetio(3,_Form,_Tblfld,-2) != 0) {
                IItcoputio(_Nullable,(short *)0,1,32,0,ERget(nullable));
                IItcoputio(_Defable,(short *)0,1,32,0,mvdefault);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 490 "creattbl.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 495 "creattbl.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_Form,_Tblfld,-2) != 0) {
                IItcogetio((short *)0,1,32,6,startdefault,_Defable);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 496 "creattbl.qsc" */	/* host code */
		STtrmwhite(startdefault);
/* # line 497 "creattbl.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 502 "creattbl.qsc" */	/* host code */
		STATUS	  retstat;
		ER_MSGID  rspns;
            i4 dkey = 0;
/* # line 506 "creattbl.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_Form,_Tblfld,-2) != 0) {
                IItcogetio((short *)0,1,32,5,mvnullable,_Nullable);
                IItcogetio((short *)0,1,32,6,mvdefault,_Defable);
                IItcogetio((short *)0,1,30,4,&dkey,_Deflkey);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 510 "creattbl.qsc" */	/* host code */
			/* if running gateways or < ingres6.5, don't allow
			** user defined defaults (no WITH DEFAULT, NOT DEFAULT)
			*/
		if ( (IIUIdml() == UI_DML_GTWSQL)
		  || 
    		  ( STcompare( IIUIscl_StdCatLevel(), UI_LEVEL_65 ) < 0 ) )
		{
			STcopy(ERget(F_TU0001_n_a), mvdefault);
/* # line 518 "creattbl.qsc" */	/* putrow */
            {
              if (IItbsetio(3,_Form,_Tblfld,-2) != 0) {
                IItcoputio(_Defable,(short *)0,1,32,0,mvdefault);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 519 "creattbl.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
/* # line 520 "creattbl.qsc" */	/* host code */
		}
		rspns = IITUvunValueUserNull(mvdefault);
		if (rspns == (ER_MSGID) NULL)	/* not value, user, or null */
		{				/*    look for yes or no    */
			_VOID_ IIUGyn(mvdefault, &retstat);
			if (retstat == E_UG0005_No_Response)	/* No */
			{
				STcopy(ERget(F_UG0007_No2), mvdefault);
			}
			else if (retstat == E_UG0004_Yes_Response) /* Yes */
			{
				STcopy(ERget(F_UG0002_Yes2), mvdefault);
			}
			else	/* Illegal defaultability */
			{
			    if (mvdefault[0] == EOS)
			    {
				/* Must've been a carriage return */
				/* restore previous value here    */
				STcopy(startdefault, mvdefault);
			    }
			    else
			    {
				/* Bogus defaultability */
				IIUGerr(E_TU000F_Bad_Defaultability,
				    	UG_ERR_ERROR, 1, mvdefault);
/* # line 549 "creattbl.qsc" */	/* resume */
            {
              IIrescol(_Tblfld,_Defable);
              if (1) goto IIfdB1;
            }
/* # line 550 "creattbl.qsc" */	/* host code */
			    }
			}
		}
		else if (rspns == F_TU0074_Value2)
		{
			if (!STequal(startdefault, ERget(F_TU0074_Value2)))
			{
				/* pick up defaults list key in case this
				** is a change and not a new row.
				** if dkey is 0, this is a new defaults
				** value.  if a defaults value is added,
				** the value for the key must be put into
				** the hidden column
				*/
				if (!IITUedEditDefault( &dkey ))
				{
					STcopy(startdefault, mvdefault);
				}
				else
				{
						/* dkey is 0 and TRUE return
						** from EditDefault implies
						** user has left default field
						** blank, so no default value
						*/
					if ( dkey == 0 )
					    STcopy(ERget(F_UG0007_No2),
					    			mvdefault);
					else
					    STcopy(ERget(F_TU0074_Value2),
								mvdefault);
				}
			}
			else
			{
				/* restore starting value so tabbing
				** through doesn't change field value
				*/
				STcopy( startdefault, mvdefault );
			}
		}
		else if (rspns == F_TU007D_Null2)
		{
			if (IIUGyn(mvnullable, &retstat))
			{
				STcopy(ERget(rspns), mvdefault);
			}
			else
			{
				IIUGerr(E_TU000B_Null_Default, UG_ERR_ERROR, 0);
				STcopy(startdefault, mvdefault);
/* # line 601 "creattbl.qsc" */	/* putrow */
            {
              if (IItbsetio(3,_Form,_Tblfld,-2) != 0) {
                IItcoputio(_Defable,(short *)0,1,32,0,mvdefault);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 602 "creattbl.qsc" */	/* resume */
            {
              IIrescol(_Tblfld,_Nullable);
              if (1) goto IIfdB1;
            }
/* # line 603 "creattbl.qsc" */	/* host code */
			}
		}
		else/* ( rspns == F_TU0079_User2 ) */
		{
			STcopy(ERget(rspns), mvdefault);
		}
			/* dkey or mvdefault may have changed
			*/
/* # line 611 "creattbl.qsc" */	/* putrow */
            {
              if (IItbsetio(3,_Form,_Tblfld,-2) != 0) {
                IItcoputio(_Defable,(short *)0,1,32,0,mvdefault);
                IItcoputio(_Deflkey,(short *)0,1,30,4,&dkey);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 613 "creattbl.qsc" */	/* resume */
            {
              IIresnext();
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 7) {
          {
            i4 ontable;
            i4 curcol;
/* # line 622 "creattbl.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,_Form,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&ontable,28,(char *)0,0);
              } /* IIiqset */
            }
/* # line 623 "creattbl.qsc" */	/* host code */
		if (ontable == 0)
		{
			IIUGerr(E_TU0002_Must_be_on_table_fiel, UG_ERR_ERROR,
				(i4) 0);
/* # line 627 "creattbl.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 628 "creattbl.qsc" */	/* host code */
		}
/* # line 630 "creattbl.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,_Form,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&curcol,29,_Tblfld,0);
              } /* IIiqset */
            }
/* # line 631 "creattbl.qsc" */	/* host code */
		--curcol;
/* # line 632 "creattbl.qsc" */	/* insertrow */
            {
              if (IItbsetio(5,_Form,_Tblfld,curcol) != 0) {
                if (IItinsert() != 0) {
                  IITBceColEnd();
                } /* IItinsert */
              } /* IItbsetio */
            }
/* # line 633 "creattbl.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
          }
        } else if (IIretval() == 8) {
          {
            i4 ontable;
            i4 dkey = 0;
/* # line 641 "creattbl.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,_Form,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&ontable,28,(char *)0,0);
              } /* IIiqset */
            }
/* # line 642 "creattbl.qsc" */	/* host code */
		if (ontable == 0)
		{
			IIUGerr(E_TU0003_Must_be_on_table_fiel, UG_ERR_ERROR,
				(i4) 0);
/* # line 646 "creattbl.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 647 "creattbl.qsc" */	/* host code */
		}
			/* get key into defaults list for this row  */
/* # line 650 "creattbl.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_Form,_Tblfld,-2) != 0) {
                IItcogetio((short *)0,1,30,4,&dkey,_Deflkey);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 652 "creattbl.qsc" */	/* host code */
			/* delete the defaults list entry if one exists */
		IITUdndDefaultNodeDelete( dkey );
/* # line 655 "creattbl.qsc" */	/* deleterow */
            {
              if (IItbsetio(4,_Form,_Tblfld,-2) != 0) {
                if (IItdelrow(0) != 0) {
                } /* IItdelrow */
              } /* IItbsetio */
            }
/* # line 656 "creattbl.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
          }
        } else if (IIretval() == 9) {
          {
/* # line 661 "creattbl.qsc" */	/* clear */
            {
              IIfldclear(_Tblfld);
            }
/* # line 662 "creattbl.qsc" */	/* putform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIputfldio((char *)"table",(short *)0,1,32,0,_Blank);
              } /* IIfsetio */
            }
/* # line 663 "creattbl.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(0),
                (void *)IILQint(0));
              } /* IIiqset */
            }
/* # line 664 "creattbl.qsc" */	/* host code */
			/* throw away any defaults values list */
		IITUcdlClearDefaultList();
/* # line 666 "creattbl.qsc" */	/* resume */
            {
              IIresfld(_Table);
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 10) {
          {
            i4 ontable;
            i4 currow;
            i4 moverec;
            i4 dkey;
/* # line 676 "creattbl.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,_Form,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&ontable,28,(char *)0,0);
              } /* IIiqset */
            }
/* # line 677 "creattbl.qsc" */	/* host code */
		if (ontable)
		{
/* # line 679 "creattbl.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_Form,_Tblfld,-2) != 0) {
                IItcogetio((short *)0,1,30,4,&state,(char *)"_STATE");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 680 "creattbl.qsc" */	/* host code */
			if (state == stUNDEF || state == stDELETE)
			{
				IIUGerr(E_TU0004_Cannot_Move_Empty_Col, 
					UG_ERR_ERROR, (i4) 0);
/* # line 684 "creattbl.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 685 "creattbl.qsc" */	/* host code */
			}
			else
			{
/* # line 688 "creattbl.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,_Form,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&currow,29,_Tblfld,0);
              } /* IIiqset */
            }
/* # line 690 "creattbl.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_Form,_Tblfld,currow) != 0) {
                IItcogetio((short *)0,1,30,4,&moverec,(char *)"_RECORD");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 692 "creattbl.qsc" */	/* host code */
			}
		}
		else
		{
			IIUGerr(E_TU0005_Must_be_on_table_fiel, UG_ERR_ERROR,
				(i4) 0);
/* # line 698 "creattbl.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 699 "creattbl.qsc" */	/* host code */
		}
/* # line 701 "creattbl.qsc" */	/* display */
            {
              if (IInestmu() == 0) goto IIfdE2;
              goto IImuI2;
IIfdI2:;
IIfdB2:
              while (IIrunnest() != 0) {
                if (IIretval() == 1) {
                  {
/* # line 705 "creattbl.qsc" */	/* inquire_frs */
                    {
                      if (IIiqset(2,0,_Form,(char *)0, (char *)0) != 0) {
                        IIiqfsio((short *)0,1,30,4,&ontable,28,(char *)0,0);
                      } /* IIiqset */
                    }
/* # line 706 "creattbl.qsc" */	/* host code */
			if (!ontable)
			{
				IIUGerr(E_TU0006_Dest_Must_Be_Column, 
					UG_ERR_ERROR, (i4) 0);
			}
			else
                    {
                      i4 destrec;
/* # line 715 "creattbl.qsc" */	/* inquire_frs */
                      {
                        if (IIiqset(3,0,_Form,(char *)0, (char *)0) != 0) {
                          IIiqfsio((short *)0,1,30,4,&currow,29,_Tblfld,0);
                        } /* IIiqset */
                      }
/* # line 717 "creattbl.qsc" */	/* getrow */
                      {
                        if (IItbsetio(2,_Form,_Tblfld,currow) != 0) {
                          IItcogetio((short *)0,1,30,4,&destrec,(char *)
"_RECORD");
                          IITBceColEnd();
                        } /* IItbsetio */
                      }
/* # line 718 "creattbl.qsc" */	/* host code */
				if (destrec == moverec)
				{
/* # line 720 "creattbl.qsc" */	/* breakdisplay */
                      {
                        if (1) goto IIfdE2;
                      }
/* # line 721 "creattbl.qsc" */	/* host code */
				}
/* # line 722 "creattbl.qsc" */	/* scroll */
                      {
                        if (IItbsetio(1,_Form,_Tblfld,-3) != 0) {
                          IItbsmode((char *)"to");
                          if (IItscroll(0,moverec) != 0) {
                          } /* IItscroll */
                        } /* IItbsetio */
                      }
/* # line 723 "creattbl.qsc" */	/* getrow */
                      {
                        if (IItbsetio(2,_Form,_Tblfld,-2) != 0) {
                          IItcogetio((short *)0,1,32,FE_MAXNAME + 1-1,objname,
                          _Name);
                          IItcogetio((short *)0,1,32,49,mvformat,_DataFmt);
                          IItcogetio((short *)0,1,32,4,keyno,_Key);
                          IItcogetio((short *)0,1,32,5,mvnullable,_Nullable);
                          IItcogetio((short *)0,1,32,6,mvdefault,_Defable);
                          IItcogetio((short *)0,1,30,4,&dkey,_Deflkey);
                          IITBceColEnd();
                        } /* IItbsetio */
                      }
/* # line 730 "creattbl.qsc" */	/* deleterow */
                      {
                        if (IItbsetio(4,_Form,_Tblfld,-2) != 0) {
                          if (IItdelrow(0) != 0) {
                          } /* IItdelrow */
                        } /* IItbsetio */
                      }
/* # line 731 "creattbl.qsc" */	/* host code */
				if (moverec < destrec)
				{
					--destrec;
				}
/* # line 735 "creattbl.qsc" */	/* scroll */
                      {
                        if (IItbsetio(1,_Form,_Tblfld,-3) != 0) {
                          IItbsmode((char *)"to");
                          if (IItscroll(0,destrec) != 0) {
                          } /* IItscroll */
                        } /* IItbsetio */
                      }
/* # line 736 "creattbl.qsc" */	/* inquire_frs */
                      {
                        if (IIiqset(3,0,_Form,(char *)0, (char *)0) != 0) {
                          IIiqfsio((short *)0,1,30,4,&currow,29,_Tblfld,0);
                        } /* IIiqset */
                      }
/* # line 738 "creattbl.qsc" */	/* host code */
				--currow;
/* # line 739 "creattbl.qsc" */	/* insertrow */
                      {
                        if (IItbsetio(5,_Form,_Tblfld,currow) != 0) {
                          if (IItinsert() != 0) {
                            IItcoputio(_Name,(short *)0,1,32,0,objname);
                            IItcoputio(_DataFmt,(short *)0,1,32,0,mvformat);
                            IItcoputio(_Key,(short *)0,1,32,0,keyno);
                            IItcoputio(_Nullable,(short *)0,1,32,0,mvnullable);
                            IItcoputio(_Defable,(short *)0,1,32,0,mvdefault);
                            IItcoputio(_Deflkey,(short *)0,1,30,4,&dkey);
                            IITBceColEnd();
                          } /* IItinsert */
                        } /* IItbsetio */
                      }
/* # line 747 "creattbl.qsc" */	/* set_frs */
                      {
                        if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0
                        ) {
                          IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(1),
                          (void *)IILQint(1));
                        } /* IIiqset */
                      }
                    }
/* # line 749 "creattbl.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE2;
                    }
                  }
                } else if (IIretval() == 2) {
                  {
/* # line 755 "creattbl.qsc" */	/* host code */
			FEhelp(ERx("tumovesm.hlp"), 
				ERget(F_TU0027_Move_Column_Submenu));
                  }
                } else if (IIretval() == 3) {
                  {
/* # line 762 "creattbl.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE2;
                    }
                  }
                } else {
                  if (1) goto IIfdE2;
                } /* IIretval */
              } /* IIrunnest */
IIfdF2:
              if (IIchkfrm() == 0) goto IIfdB2;
              goto IIfdE2;
IImuI2:
              if (IIinitmu() == 0) goto IIfdE2;
              if (IInmuact(ERget(FE_Place),ERget(F_TU0052_ExplPlace),2,2,1) ==
               0) goto IIfdE2;
              if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,0,2)
               == 0) goto IIfdE2;
              IIFRInternal(0);
              if (IInfrskact(1,(char *)0,0,0,2) == 0) goto IIfdE2;
              if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),0,0,3) == 0) 
              goto IIfdE2;
              IIFRInternal(0);
              if (IInfrskact(3,(char *)0,0,0,3) == 0) goto IIfdE2;
              if (IIendmu() == 0) goto IIfdE2;
              goto IIfdI2;
IIfdE2:;
              IIendnest();
            }
          }
        } else if (IIretval() == 11) {
          {
/* # line 768 "creattbl.qsc" */	/* host code */
	    UI_HELP_INFO	*rel_info;
	    char		ldtblwidth[20];
            char *p_tbl = (char *)NULL;
	    DB_STATUS		rval;
	    FE_RSLV_NAME	rname;
	    char		get_own[FE_UNRML_MAXNAME + 1];
	    char		get_name[FE_UNRML_MAXNAME + 1];
            while (1)   /* loop till good table name is entered */
            {
                p_tbl = IITUprTablePrompt(p_tbl); /* prompt user for table name
						  ** with popup Help available
						  */
                if ( (p_tbl == NULL) || (*p_tbl == EOS))
/* # line 782 "creattbl.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 783 "creattbl.qsc" */	/* host code */
                else
                {
		    rname.name = p_tbl;
		    rname.owner_dest = get_own;
		    rname.name_dest = get_name;
		    rname.is_nrml = FALSE;
		    /* FE_fullresolve(&rname); ??? */
		    FE_decompose(&rname);
		    if (IIUGdlm_ChkdlmBEobject(get_name, get_name, FALSE) ==
			UI_BOGUS_ID)
		    {
			IIUGerr(E_TU0037_Name_contain_bad_char, UG_ERR_ERROR,
				(i4) 1, get_name);
			continue;
		    }
		    if (rname.owner_spec == FALSE)
		    {
		    	STcopy(IIUIdbdata()->suser, get_own);
		    }	
			  /* ensure correct casing etc of owner which may
			  ** depend upon the 'kind' of database we's
			  ** connected to; if suser is used this isn't
			  ** necessary since it is assumed to be proper already
			  */
		    else if (IIUGdlm_ChkdlmBEobject(get_own, get_own, FALSE)
			== UI_BOGUS_ID)
		    {
			  IIUGerr(E_TU0009_Owner_name_illegal,
			      UG_ERR_ERROR, (i4) 1, get_own);
			  continue;
												    }
	    	    rval = IIUIrel_help(&get_name, &get_own, &rel_info);
	    	    if (rval != OK)
	    	    {
			IIUGerr(E_TU004D_Error_in_IIUIrel_help, UG_ERR_ERROR,
				(i4) 1, (PTR) &rval);
                        continue;       /* Internal error. prompt again. */
	    	    }
	    	    else if (rel_info == NULL)
		    {
			IIUGerr(E_TU000E_Own_dot_Tab_notexist, UG_ERR_ERROR,
				(i4) 2, (PTR) get_name, (PTR) get_own);
                        continue;       /* no such table. prompt again. */
	    	    }
		}
                break;              /* exit while() loop */
            }
	    /* note that any user-defined defaults associated with
	    ** the columns in the table being gotten are added (cumulative)
	    ** to the defaults list.  the defaults list is only cleared
	    ** upon entry to this routine, because a user may repetitively
	    ** GetTableDef - rows are added to those already on the screen
	    ** possibly from previous GetTableDef's, possibly to rows which
	    ** the user just created on the table field
	    */
	    /* assertion: table specified above exists */
	    if (tugetattr(rel_info, _Form, _Tblfld, ldtblwidth) == 0)
	    {
		IIUGerr(E_TU0015_No_attributes_found, UG_ERR_ERROR,
			(i4) 1, (PTR) p_tbl);
	    }
	    else
	    {
/* # line 852 "creattbl.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 853 "creattbl.qsc" */	/* resume */
            {
              IIresfld(_Tblfld);
              if (1) goto IIfdB1;
            }
/* # line 854 "creattbl.qsc" */	/* host code */
	    }
          }
        } else if (IIretval() == 12) {
          {
            i4 ontab;
            i4 dkey;
/* # line 863 "creattbl.qsc" */	/* host code */
			/* user defined defaults not avail in GW's or < 6.5 */
    		if ( ( STcompare( IIUIscl_StdCatLevel(), UI_LEVEL_65 ) < 0 )
		   || ( IIUIdml() == UI_DML_GTWSQL ) )
		{
			IIUGerr(E_TU000A_NA_Defaults, UG_ERR_ERROR, (i4) 0);
/* # line 868 "creattbl.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 869 "creattbl.qsc" */	/* host code */
		}
/* # line 871 "creattbl.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,_Form,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&ontab,28,(char *)0,0);
              } /* IIiqset */
            }
/* # line 872 "creattbl.qsc" */	/* host code */
		if (ontab == 0)
		{
			IIUGerr(E_TU0010_Must_be_on_table_fiel, UG_ERR_ERROR,
					(i4) 0);
/* # line 876 "creattbl.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 877 "creattbl.qsc" */	/* host code */
		}
		/* pick up the key to the defaults list */
/* # line 880 "creattbl.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_Form,_Tblfld,-2) != 0) {
                IItcogetio((short *)0,1,30,4,&dkey,_Deflkey);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 882 "creattbl.qsc" */	/* host code */
		/* note that 
		** 1) EditDefault will install a value if one didn't
		**	already exist (dkey will be 0 in that case).
		**	dkey contains new value upon return.
		** 2) EditDefault will change an existing value if
		**	that value already exists; retaining key.
		** 3) If dkey is 0 upon return, the defaults value
		**	was left blank, so user wants to delete it.
		*/
		if (IITUedEditDefault( &dkey ))
		{
			/* TRUE from EditDefault with 0 key indicates
			** deleted defaults value; restart with 'no'
			*/
			if ( dkey == 0 )
			    STcopy(ERget(F_UG0007_No2), mvdefault);
			else
			    STcopy(ERget(F_TU0074_Value2), mvdefault);
			/*
			** Reset entry value of 'Defaults' field in case
			** we're currently on that field.
			*/
			STcopy(mvdefault, startdefault);
/* # line 909 "creattbl.qsc" */	/* putrow */
            {
              if (IItbsetio(3,_Form,_Tblfld,-2) != 0) {
                IItcoputio(_Defable,(short *)0,1,32,0,mvdefault);
                IItcoputio(_Deflkey,(short *)0,1,30,4,&dkey);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 911 "creattbl.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 13) {
          {
/* # line 918 "creattbl.qsc" */	/* host code */
		FEtabfnd(_Form, _Tblfld);
          }
        } else if (IIretval() == 14) {
          {
/* # line 923 "creattbl.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_Form,_Tblfld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 15) {
          {
/* # line 928 "creattbl.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_Form,_Tblfld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 16) {
          {
            i4 item;
            char name[FE_MAXNAME+1];
            char *chosen;
/* # line 939 "creattbl.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,_Form,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&item,28,(char *)0,0);
                IIiqfsio((short *)0,1,32,FE_MAXNAME+1-1,name,19,(char *)0,0);
              } /* IIiqset */
            }
/* # line 940 "creattbl.qsc" */	/* host code */
		if (item == 1)	/* cursor is in table field */
/* # line 941 "creattbl.qsc" */	/* inquire_frs */
            {
              if (IIiqset(4,0,_Form,_Tblfld,(char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,objname,19,(char *)0
                ,0);
              } /* IIiqset */
            }
/* # line 942 "creattbl.qsc" */	/* host code */
		else
		    objname[0] = EOS;
                if ((item == 1) && STequal(objname, _DataFmt))
                {
		    /* Put up ListChoices for "Data Type" field */
		    if (!lpTypesReady)
		    {
			if ((lp_dtbuf = load_lptypes(datatypes,dt_cnt)) == NULL)
/* # line 952 "creattbl.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 953 "creattbl.qsc" */	/* host code */
			else
			    lpTypesReady = TRUE;
		    }
                    item = IIFDlpListPick(ERget(F_TU0063_DtypeTitle),
			lp_dtbuf, 0, -1, -1,
                        ERget(F_TU0064_HlpTitle),
			ERx("tudtypes.hlp"), NULL, NULL);
                    if (item >= 0)	/* user chose a row */
                    {
			chosen = tugetDtype(item);
/* # line 965 "creattbl.qsc" */	/* putrow */
            {
              if (IItbsetio(3,_Form,_Tblfld,-2) != 0) {
                IItcoputio(_DataFmt,(short *)0,1,32,0,chosen);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 966 "creattbl.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 967 "creattbl.qsc" */	/* host code */
                    }
        	}
		else if ( ( (item == 1) && STequal(objname, _Defable) )
		    &&  /* running 6.5 or above */
		    ( STcompare( IIUIscl_StdCatLevel(), UI_LEVEL_65 ) >= 0 )
		    &&  /* not gateway */
		    ( IIUIdml() != UI_DML_GTWSQL ) )
		{
		    /* Put up ListChoices for "Defaults" field */
		    if (!dfltformready)
                    {
                        if ((defl_buf = load_lp(dfltlist, dflt_cnt)) == NULL)
			{
/* # line 985 "creattbl.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 986 "creattbl.qsc" */	/* host code */
			}
                        else
			    dfltformready = TRUE ;
		    }
                    item = IIFDlpListPick(ERget(F_TU007F_Select_Defaultability),
                        	defl_buf, 0, -1, -1,
                        	ERget(F_TU0085_Default_Val),
                        	ERx("tudefaults.hlp"), NULL, NULL);
		    switch (item)
		    {
		      case 0:
			chosen = ERget(F_UG0007_No2);
			break;
		      case 1:
			chosen = ERget(F_UG0002_Yes2);
			break;
		      case 2:
			chosen = ERget(F_TU0079_User2);
			break;
		      case 3:
			chosen = ERget(F_TU007D_Null2);
			break;
		      case 4:
			chosen = ERget(F_TU0074_Value2);
			break;
		      default:
/* # line 1014 "creattbl.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1015 "creattbl.qsc" */	/* host code */
		    }
/* # line 1017 "creattbl.qsc" */	/* putrow */
            {
              if (IItbsetio(3,_Form,_Tblfld,-2) != 0) {
                IItcoputio(_Defable,(short *)0,1,32,0,chosen);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 1018 "creattbl.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 1019 "creattbl.qsc" */	/* host code */
		}
                else
                {
                    /* lookups not supported on current field */
		    IIUGerr( E_TU0053_No_Lookup, UG_ERR_ERROR,
					1, ERget(FE_Lookup));
                }
          }
        } else if (IIretval() == 17) {
          {
/* # line 1033 "creattbl.qsc" */	/* host code */
		FEhelp(ERx("tucreate.hlp"), 
			ERget(F_TU0028_New_Table));
          }
        } else if (IIretval() == 18) {
          {
/* # line 1041 "creattbl.qsc" */	/* host code */
		IITUcdlClearDefaultList();
/* # line 1042 "creattbl.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 19) {
          {
/* # line 1049 "creattbl.qsc" */	/* host code */
		bool	key_unique;
            i4 errcol = 0;
            char tblname[ FE_UNRML_MAXNAME + 1 ];
		char	*attrs;
		char	*modcols;
  char *sql_buf;
/* # line 1059 "creattbl.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&state,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 1060 "creattbl.qsc" */	/* host code */
		if (state == 0)
		{
			/* pressing End without making changes just returns */
/* # line 1063 "creattbl.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 1064 "creattbl.qsc" */	/* host code */
		}
/* # line 1065 "creattbl.qsc" */	/* redisplay */
            {
              IIredisp();
            }
/* # line 1067 "creattbl.qsc" */	/* host code */
		if ( (sql_buf = (char *)MEreqmem((u_i4)0,
		   (u_i4) FE_MAXNAME * DB_GW2_MAX_COLS + 2 * ( TUBUFSIZE + 1 ),
					FALSE, (STATUS *)NULL)) == (char *)NULL)
		{
			IIUGerr(E_TU0033_tunmadd_bad_mem_alloc, UG_ERR_ERROR,0);
/* # line 1072 "creattbl.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1073 "creattbl.qsc" */	/* host code */
		}
		attrs = sql_buf + FE_MAXNAME*DB_GW2_MAX_COLS;
		modcols = attrs + TUBUFSIZE + 1;
# ifdef DGC_AOS
		/*
		** Until INGRES supports a readonly mode, DG will use the
		** following code to avoid ugly error messages that occur
		** when trying to write to the database when we are only
		** connected for readonly access.
		*/
		if (IIUIcro_CheckReadOnly())
		{
			IIUGerr(E_UI0016_ReadOnly_err, 0, 0);
/* # line 1087 "creattbl.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 1088 "creattbl.qsc" */	/* host code */
		}
# endif
/* # line 1091 "creattbl.qsc" */	/* getform */
            {
              if (IIfsetio((char *)0) != 0) {
                IIgetfldio((short *)0,1,32,FE_UNRML_MAXNAME + 1-1,tblname,
                _Table);
              } /* IIfsetio */
            }
/* # line 1092 "creattbl.qsc" */	/* host code */
		if (tuchkname(tblname) != OK)
		{	/* Errors will be given by tuchkname */
			goto _resumeTblfld;
		}
		modcols[0] = EOS;
		if (parsecreate(attrs, TUBUFSIZE, modcols, TUBUFSIZE, &errcol)
			!= OK)
		{
			if (errcol != 0)
			{
/* # line 1103 "creattbl.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_Form,_Tblfld,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,errcol) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
/* # line 1104 "creattbl.qsc" */	/* host code */
			}
			goto _resume;
		}
		if ( modcols[0] != EOS && IIUIdcn_ingres() )
		{
		    /* keys specified on ingres dbms. see if key unique */
		    i4	row;
		    if (!keyformready)
                    {
                        if ((uniq_buf = load_lp(ukeys, uk_cnt)) == NULL)
			    goto _resume;	/* error */
                        else
			    keyformready = TRUE ;
		    }
                    row = IIFDlpListPick(ERget(F_TU0069_Title),
                        	uniq_buf, 0, -1, -1,
                        	ERget(F_TU0033_Unique_Keys),
                        	ERx("tukeys.hlp"), NULL, NULL);
		    if (row == 0)
			key_unique = TRUE ;
		    else if (row == 1)
			key_unique = FALSE ;
		    else
		    {
			goto _resume;
		    }	
		}
		if ( modcols[0] != EOS && ! IIUIdcn_ingres() )
		{
			/*
			** Keys specified, and dbms != ingres.
			*/
			STprintf( sql_buf,
				ERx(IIUIdck_req() == UI_KEY_Y
				    ? "CREATE TABLE %s ( %s ) WITH KEY = (%s)"
			    	    : "CREATE TABLE %s ( %s )"
				),
				tblname, attrs, modcols
			);
		}
		else
		{
			STprintf(sql_buf,ERx("CREATE TABLE %s ( %s )"),
							tblname, attrs);
		}
		IIUGmsg(ERget(F_TU003B_Creating_table), FALSE, 1, (PTR)tblname);
/* # line 1539 "creattbl.sc" */	/* prepare */
  {
    IIsqInit((char *)0);
    IIsqPrepare(0,(char *)"tutc_s1",(char *)0,0,sql_buf);
  }
/* # line 1540 "creattbl.sc" */	/* host code */
		/* fix for bug 3131 -- if use keywords for object names,
		** then PREPARE can return an error. check for it.
		*/
		if ( FEinqerr() != OK )
		{	/* Error messages will come from the server */
			goto _resume;
		}
/* # line 1547 "creattbl.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExStmt((char *)"tutc_s1",0);
    IIsyncup((char *)0,0);
  }
/* # line 1548 "creattbl.sc" */	/* host code */
		if ( FEinqerr() != OK )
		{	/* Errors will come from the server */
			goto _resume;
		}
		else
		{
			IIUGmsg( ERget(F_TU003C_Table_created), FALSE,
					1, (PTR)tblname
			);
/* # line 1174 "creattbl.qsc" */	/* sleep */
            {
              IIsleep(2);
            }
/* # line 1176 "creattbl.qsc" */	/* host code */
			STcopy(tblname, lasttable);
			if ( modcols[0] != EOS && IIUIdcn_ingres() )
			{
				_VOID_ STprintf(sql_buf,
					ERx(key_unique
					    ? "MODIFY %s TO btree UNIQUE ON %s"
					    : "MODIFY %s TO btree ON %s"
					),
					tblname, modcols
				);
/* # line 1572 "creattbl.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(sql_buf);
    IIsyncup((char *)0,0);
  }
/* # line 1573 "creattbl.sc" */	/* host code */
				if ( FEinqerr() != OK )
				{
					/* jupbug 4196 -- if modify failed,
					then table should be dropped to give
					user chance to fix up table def'n --
					probably char key column is too long */
					STprintf(sql_buf,
					    ERx("DROP TABLE %s"), tblname);
/* # line 1581 "creattbl.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExImmed(sql_buf);
    IIsyncup((char *)0,0);
  }
/* # line 1582 "creattbl.sc" */	/* host code */
					IIUGerr(E_TU0011_Error_modifying_table,
						UG_ERR_ERROR, (i4) 1,
						(PTR) tblname);
					goto _resume;
				}
				else
				{
					IIUGmsg(ERget(F_TU0040_Table_modified),
						FALSE, 2, (PTR) tblname,
						(PTR) modcols) ;
/* # line 1210 "creattbl.qsc" */	/* sleep */
            {
              IIsleep(2);
            }
/* # line 1211 "creattbl.qsc" */	/* host code */
				}
			}
		}
		/* Assertion: CREATE TABLE was successful.
		** Free memory, and return to caller.
		*/
		MEfree(sql_buf);
/* # line 1219 "creattbl.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 1221 "creattbl.qsc" */	/* host code */
_resumeTblfld:
		MEfree(sql_buf);
/* # line 1223 "creattbl.qsc" */	/* resume */
            {
              IIresfld(_Table);
              if (1) goto IIfdB1;
            }
/* # line 1225 "creattbl.qsc" */	/* host code */
_resume:
		MEfree(sql_buf);
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
      if (IITBcaClmAct(_Tblfld,_Name,0,1) == 0) goto IIfdE1;
      if (IITBcaClmAct(_Tblfld,_DataFmt,0,2) == 0) goto IIfdE1;
      if (IITBcaClmAct(_Tblfld,_Key,0,3) == 0) goto IIfdE1;
      if (IITBcaClmAct(_Tblfld,_Nullable,0,4) == 0) goto IIfdE1;
      if (IITBcaClmAct(_Tblfld,_Defable,1,5) == 0) goto IIfdE1;
      if (IITBcaClmAct(_Tblfld,_Defable,0,6) == 0) goto IIfdE1;
      if (IIinitmu() == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Insert),ERget(F_TU004E_ExplInsert),2,2,7) == 0) 
      goto IIfdE1;
      if (IInmuact(ERget(FE_Delete),ERget(F_TU004F_ExplDelete),2,2,8) == 0) 
      goto IIfdE1;
      if (IInmuact(ERget(FE_Blank),ERget(F_TU0050_ExplBlank),0,2,9) == 0) 
      goto IIfdE1;
      if (IInmuact(ERget(FE_Move),ERget(F_TU0051_ExplMove),2,2,10) == 0) goto 
      IIfdE1;
      if (IInmuact(ERget(FE_GetTableDef),ERget(F_TU004D_ExplGetTblDef),2,2,11)
       == 0) goto IIfdE1;
      if (IInmuact(ERget(F_TU0017_EditDefaults),ERget(F_TU0070_ExplEdtDflt),2,
      2,12) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,13) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,14) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,15) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Lookup),(char *)0,0,0,16) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(10,(char *)0,0,0,16) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,0,17) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,0,17) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),0,0,18) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,0,0,18) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_TU004C_ExplSave),2,1,19) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,1,19) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 1230 "creattbl.qsc" */	/* host code */
	return;
  }
/*{
** Name:	load_lp - build character array for ListPick.
**
** Description:
**	Load listpick unique key or default info into a character array.
**
** Inputs:
**	LISTVALS   *lstvals	array of unique key descriptions
**	i4        cnt		number of items in above array
**
** Outputs:
**
**	Returns:
**		Pointer to character string, all set for ListPick's use.
**		If error, returns NULL.
**
**	Exceptions:
**		NONE
**
** Side Effects:
**	Allocates memory.
**
** History:
**	12/29/89	(pete)	Initial Version.
*/
static char *
load_lp (lstvals, cnt)
LISTVALS  *lstvals;	/* array of unique key descriptions */
i4	  cnt;		/* number of items in above array */	
{
	i4 size, i;
	char *buf;
	LISTVALS *p_uk = lstvals;
	char *nl = ERx("\n");
	char *separate = ERx(";");
	i4 nl_size, sep_size;
	nl_size = STlength(nl);
	sep_size = STlength(separate);
	for (i=0, size=0; i < cnt; i++, p_uk++)
	{
	    size += (STlength(ERget(p_uk->abbrev)) + sep_size);
	    size += (STlength(ERget(p_uk->descrip)) + nl_size);
	}
	if ( (buf = (char *)MEreqmem((u_i4)0, (u_i4) (size +1),
		FALSE, (STATUS *)NULL)) == NULL)
	{
    	    IIUGerr(E_TU0033_tunmadd_bad_mem_alloc, UG_ERR_ERROR, 0);
	    return (char *)NULL;
	}
	*buf = EOS;
	p_uk = lstvals;
	for (i=0; i < cnt; i++, p_uk++)
	{
	    STcat (buf, ERget(p_uk->abbrev));
	    STcat (buf, separate);
	    STcat (buf, ERget(p_uk->descrip));
	    STcat (buf, nl);
	}
	return buf;
}
/*{
** Name:	load_lptypes - build character array for ListPick of data types.
**
** Description:
**	Load listpick data types into a character array.
**
** Inputs:
**	DTYPE *datatypes	array of data type descriptions
**	i4	cnt		number of items in above array
**
** Outputs:
**
**	Returns:
**		Pointer to character string, all set for ListPick's use.
**		If error, returns NULL.
**
**	Exceptions:
**		NONE
**
** Side Effects:
**	Allocates memory.
**
** History:
**	12/29/89	(pete)	Initial Version.
*/
static char *
load_lptypes (datatypes, cnt)
DTYPE *datatypes;	/* array of data type descriptions */
i4	cnt;		/* number of items in above array */	
{
	i4 size, i;
	char *buf;
	DTYPE *p_dt = datatypes;
	char *nl = ERx("\n");
	i4 nl_size;
	nl_size = STlength(nl);
	for (i=0, size=0; i < cnt; i++, p_dt++)
	{
		if (IIUIdcn_ingres())
		{
		    /* Ingres: include all data types */
	            size += (STlength(ERget(p_dt->type)) + nl_size);
		}
		else
		{
		    /* Not INGRES; only include Open SQL data types */
		    if (p_dt->openSql)
	                size += (STlength(ERget(p_dt->type)) + nl_size);
		}
	}
	if ( (buf = (char *)MEreqmem((u_i4)0, (u_i4) (size +1),
		FALSE, (STATUS *)NULL)) == NULL)
	{
    	    IIUGerr(E_TU0033_tunmadd_bad_mem_alloc, UG_ERR_ERROR, 0);
	    return (char *)NULL;
	}
	*buf = EOS;
	p_dt = datatypes;
	for (i=0; i < cnt; i++, p_dt++)
	{
		if (IIUIdcn_ingres())
		{
		    /* Ingres: include all data types */
	    	    STcat (buf, ERget(p_dt->type));
	    	    STcat (buf, nl);
		}
		else
		{
		    /* Not INGRES; only include Open SQL data types */
		    if (p_dt->openSql)
		    {
	    	        STcat (buf, ERget(p_dt->type));
	    	        STcat (buf, nl);
		    }
		}
	}
	return buf;
}
/*{
** Name:	tugetDtype - get data type name picked from list.
**
** Description:
**	return the string name of the data type the user picked from ListPick.
**
** Inputs:
**	item	i4		item picked by user in table field.
**
** Outputs:
**
**	Returns:
**		Pointer to name of data type.
**
**	Exceptions:
**		NONE
**
** Side Effects:
**	NONE
**
** History:
**	10-oct-1990 (pete)
**		Initial version (for bug 34244)
*/
static char *
tugetDtype (item)
i4	item;
{
	char *typename = ERx("");
	if (IIUIdcn_ingres())
	{
	    /* Ingres: all data types were loaded */
	    typename = ERget(datatypes[item].type);
	}
	else
	{
	    i4  i = 0;
	    i4  gtw_item = 0;
	    DTYPE *p_dt = datatypes;
	    /* Gateway: not all data types were loaded. Loop thru the array of
	    ** types looking for the "item"th one loaded.
	    */
	    for (i=0; i < dt_cnt; i++, p_dt++)
	    {
		if (p_dt->openSql)
		{
		    /* this one was loaded into ListPick for Gateway */
		    if (gtw_item == item)
		    {
			typename = ERget(datatypes[i].type);
			break;
		    }
		    gtw_item++;
		}
	    }
	}
	return typename;
}
