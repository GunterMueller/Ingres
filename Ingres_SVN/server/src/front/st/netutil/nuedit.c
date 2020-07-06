# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 1992, 2004 Ingres Corporation
*/
#include	<compat.h>
#include	<si.h>
#include	<st.h>
#include	<me.h>
#include	<gc.h>
#include	<ex.h>
#include        <er.h>
#include	<cm.h>
#include	<gl.h>
#include	<sl.h>
#include	<iicommon.h>
#include        <fe.h>
#include        <ug.h>
#include        <uf.h>
#include        <ui.h>
#include        <ci.h>
#include        <pc.h>
#include	<iplist.h>
#include	<erst.h>
#include        <uigdata.h>
#include	<stdprmpt.h>
#include	<gcccl.h>
#include	"nu.h"
/*
**  Name: nuedit.sc -- editing functions for netutil
**
**  Description:
** 	The functions in this module control the popup windows used for
**	editing detail information.  These functions are used exclusively
**	by the netutil utility.
**
**  Entry points:
**	nu_authedit() -- Edit or create login information
**	nu_connedit() -- Edit or create connection information
**	nu_attredit() -- Edit or create attribute information
** 	nu_locate_conn() -- Find a specified connection entry
** 	nu_locate_attr() -- Find a specified attribute entry
**	nu_is_private()  -- Distinguish between "Private" and "Global" words
**
**  History:
**	30-jun-92 (jonb)
**	    Created.
**	03-Oct-1993 (joplin)
**	    Fixed truncation of listen address problems by enlarging
**          the local character arrays used to hold the values,  Finalized
**          protocol selection list,  Got rid of "superuser" detection code.
**	21-mar-94 (smc) Bug #60829
**		Added #include header(s) required to define types passed
**		in prototyped external function calls.
**	08-aug-95 (reijo01) #70384
**		Changed the procotols for the DESKTOP platform.
**      19-mar-96 (chech02)
**              deleted #include <locl.h>. LOCATION is redefined.
**              Added win 3.1 protocol drivers to protocol_listpick().
**	13-jun-96 (emmag)
**	    Add decnet to the list of protocols for NT.
**	21-Aug-97 (rajus01)
**	    Added nu_attredit(), nu_locate_attr() routines. Renamed 
**	    "logpass" to "logpassedit".
**      14-oct-97 (funka01)
**          Change hard-coded Netutil messages to depend on SystemNetutilName.
**          Modified to use correct form for JASMINE/INGRES.
**	23-Jan-98 (rajus01)
**	   Changed the order of field names so that the cursor is positioned
**	   at Login field.(logpassedit.frm). Fixed problems with the display
**	   of login type on the netutil form.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**	29-sep-2003 (somsa01)
**	    Added tcp_ip to Windows.
**	14-jun-2004 (somsa01)
**	    Cleaned up code for Open Source.
*/
bool	nu_is_private();
CONN *	nu_locate_conn();
ATTR *	nu_locate_attr();
static char *protocol_listpick();
  GLOBALREF char global_word[];
  GLOBALREF char private_word[];
  GLOBALREF bool edit_global;
  GLOBALREF char username[];
  static char logpassFormName[ MAX_TXT ];
  static char conneditFormName[ MAX_TXT ];
/*
**  nu_authedit() -- edit popup for login (authorization) entries
**
**  Inputs:
**
**    function -- AUTH_NEW ==> get new login
**                AUTH_EDIT ==> edit existing login
**                AUTH_EDITPW ==> edit password of existing login only
**    vnode -- the vnode name this entry is associated with (not edited)
**    login -- the login name.  May be changed by editing.
**    password -- points to location to store the password.  Edited but
**                not displayed, i.e. its input value doesn't matter.
**    prompt_word -- "Private" or "Global" depending on what kind of login
**		     this is.  Not Edited.
**
**  Returns:
**
**    EDIT_OK -- Edit approved by the user
**    EDIT_CANCEL -- Edit was cancelled by the user
**    EDIT_EMPTY -- User didn't change anything
**
**  Note that you can't change a private entry to a global by editing, or
**  vice versa.  Note also that a password must _always_ be entered.
*/
STATUS
nu_authedit(i4 function, char *vn, 
	    char *login, char *password, char *prompt_word)
{
	char lc_prompt_word[20];/* The prompt word in lowercase for messages */
	STATUS rtn;
	/* Make sure the sizes of these variables match form definitions */
  char sav_login[32 + 1];
  char lcl_login[32 + 1];
  char lcl_logintype[10 + 1];
  char lcl_pass[32 + 1];
  char lcl_pass_conf[32 + 1];
  char msg[40];
  i4 chflag, len;
  char *vnode = vn;
  char *type = prompt_word;
	char msg1[200];
 	STprintf(msg1, "%s - Login/password data", SystemNetutilName); 
	*lcl_pass = *sav_login = *lcl_logintype = EOS;
	STcopy(prompt_word, lc_prompt_word);
	CMtolower(lc_prompt_word, lc_prompt_word);
	/* Find the form */
	STcopy(ERx("logpassedit"),logpassFormName);
	if (IIUFgtfGetForm(IIUFlcfLocateForm(), logpassFormName) != OK)
	{
    	    PCexit(FAIL);
	}
/* # line 157 "nuedit.sc" */	/* set_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(5,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      IIstfsio(4,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      IIstfsio(6,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      IIstfsio(2,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      IIstfsio(3,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 160 "nuedit.sc" */	/* set_frs */
  {
    if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
      IIstfsio(41,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      IIstfsio(39,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      IIstfsio(40,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      IIstfsio(36,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
      IIstfsio(37,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
    } /* IIiqset */
  }
/* # line 164 "nuedit.sc" */	/* host code */
	/* Display the form.  Beginning of display loop. */
/* # line 166 "nuedit.sc" */	/* display */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    if (IIdispfrm(logpassFormName,(char *)"f") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    IIputfldio((char *)"f_node",(short *)0,1,32,0,vnode);
/* # line 170 "nuedit.sc" */	/* host code */
	    if (function == AUTH_NEW)
	    {
	        if (*login == EOS)
		{
		    STcopy(username,sav_login);
/* # line 175 "nuedit.sc" */	/* set_frs */
    {
      if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(20,logpassFormName,0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
      } /* IIiqset */
    }
/* # line 176 "nuedit.sc" */	/* host code */
		}
/* # line 177 "nuedit.sc" */	/* putform */
    {
      if (IIfsetio(logpassFormName) != 0) {
        IIputfldio((char *)"infoline",(short *)0,1,32,0,
        ERget(S_ST001B_define_new_auth));
      } /* IIfsetio */
    }
/* # line 179 "nuedit.sc" */	/* host code */
	    }
	    else if (function == AUTH_EDIT)
	    {
/* # line 182 "nuedit.sc" */	/* putform */
    {
      if (IIfsetio(logpassFormName) != 0) {
        IIputfldio((char *)"infoline",(short *)0,1,32,0,
        ERget(S_ST001C_edit_auth));
      } /* IIfsetio */
    }
/* # line 184 "nuedit.sc" */	/* host code */
	    }
	    else
	    {
	        STprintf(msg,ERget(S_ST0030_enter_pw),lc_prompt_word );
/* # line 188 "nuedit.sc" */	/* putform */
    {
      if (IIfsetio(logpassFormName) != 0) {
        IIputfldio((char *)"infoline",(short *)0,1,32,0,msg);
      } /* IIfsetio */
    }
/* # line 189 "nuedit.sc" */	/* set_frs */
    {
      if (IIiqset(2,0,logpassFormName,(char *)0, (char *)0) != 0) {
        IIstfsio(53,(char *)"f_login",0,(short *)0,1,30,sizeof(1),
        (void *)IILQint(1));
        IIstfsio(17,(char *)"f_login",0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 191 "nuedit.sc" */	/* host code */
	    }
	    if (*sav_login == EOS)
	        STcopy(login,sav_login);
/* # line 196 "nuedit.sc" */	/* inquire_frs */
    {
      if (IIiqset(2,0,logpassFormName,(char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,30,4,&len,24,(char *)"f_login_type",0);
      } /* IIiqset */
    }
/* # line 197 "nuedit.sc" */	/* host code */
	    MEfill(len,' ',msg);
	    STcopy(prompt_word,&msg[len-STlength(prompt_word)]);
/* # line 201 "nuedit.sc" */	/* putform */
    {
      if (IIfsetio(logpassFormName) != 0) {
        IIputfldio((char *)"f_login",(short *)0,1,32,0,sav_login);
        IIputfldio((char *)"f_login_type",(short *)0,1,32,0,type);
        IIputfldio((char *)"f_password",(short *)0,1,32,0,(char *)"");
        IIputfldio((char *)"f_password_conf",(short *)0,1,32,0,(char *)"");
      } /* IIfsetio */
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 210 "nuedit.sc" */	/* inquire_frs */
        {
          if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIiqfsio((short *)0,1,30,4,&chflag,20,logpassFormName,0);
          } /* IIiqset */
        }
/* # line 211 "nuedit.sc" */	/* host code */
	    if (!chflag)   /* Has anything changed on the form? */
	    { 
		rtn = EDIT_EMPTY;   /* Return a code that says so */
/* # line 214 "nuedit.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
/* # line 215 "nuedit.sc" */	/* host code */
	    } 
	    /* Get all the potentially changed fields from the form */
/* # line 219 "nuedit.sc" */	/* getform */
        {
          if (IIfsetio(logpassFormName) != 0) {
            IIgetfldio((short *)0,1,32,10 + 1-1,lcl_logintype,(char *)
"f_login_type");
            IIgetfldio((short *)0,1,32,32 + 1-1,lcl_login,(char *)"f_l\
ogin");
            IIgetfldio((short *)0,1,32,32 + 1-1,lcl_pass,(char *)
"f_password");
            IIgetfldio((short *)0,1,32,32 + 1-1,lcl_pass_conf,(char *)
"f_password_conf");
          } /* IIfsetio */
        }
/* # line 224 "nuedit.sc" */	/* host code */
	    if (*lcl_login != EOS) 
	    {
		if (*lcl_pass == EOS)  
		{
		    IIUGerr(E_ST002C_must_enter_pw,0,0);
/* # line 229 "nuedit.sc" */	/* resume */
        {
          IIresfld((char *)"f_password");
          if (1) goto IIfdB1;
        }
/* # line 230 "nuedit.sc" */	/* host code */
		}
		if (*lcl_pass_conf == EOS)
		{
		    IIUGerr(E_ST002D_must_confirm_pw,0,0);
/* # line 234 "nuedit.sc" */	/* resume */
        {
          IIresfld((char *)"f_password_conf");
          if (1) goto IIfdB1;
        }
/* # line 235 "nuedit.sc" */	/* host code */
		}
		if (!STequal(lcl_pass,lcl_pass_conf))
		{
		    IIUGerr(E_ST002F_password_mismatch,0,0);
/* # line 239 "nuedit.sc" */	/* putform */
        {
          if (IIfsetio(logpassFormName) != 0) {
            IIputfldio((char *)"f_password",(short *)0,1,32,0,(char *)"");
            IIputfldio((char *)"f_password_conf",(short *)0,1,32,0,(char *)"");
          } /* IIfsetio */
        }
/* # line 241 "nuedit.sc" */	/* resume */
        {
          IIresfld((char *)"f_password");
          if (1) goto IIfdB1;
        }
/* # line 242 "nuedit.sc" */	/* host code */
		}
	        if (*lcl_logintype != EOS) 
		{
	          if( STcompare( lcl_logintype, private_word ) == 0) 
		      edit_global = FALSE;
  		  else
		      edit_global = TRUE;
		}
                STcopy(lcl_login, login); 
	        STcopy(lcl_pass, password); 
            }
	    rtn = OK;
/* # line 254 "nuedit.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
      } else if (IIretval() == 2) {
/* # line 259 "nuedit.sc" */	/* host code */
	    rtn = EDIT_CANCEL;
/* # line 260 "nuedit.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE1;
        }
      } else if (IIretval() == 3) {
/* # line 265 "nuedit.sc" */	/* host code */
            FEhelp(ERx("logpass.hlp"), msg1);
      } else if (IIretval() == 4) {
        char privglob[12];
        bool lcl_private = nu_is_private( lc_prompt_word );
/* # line 274 "nuedit.sc" */	/* getform */
        {
          if (IIfsetio((char *)"logpassedit") != 0) {
            IIgetfldio((short *)0,1,32,11,privglob,(char *)"f_login_ty\
pe");
          } /* IIfsetio */
        }
/* # line 276 "nuedit.sc" */	/* host code */
	    if (nu_is_private(privglob))
	    {
/* # line 278 "nuedit.sc" */	/* putform */
        {
          if (IIfsetio((char *)"logpassedit") != 0) {
            IIputfldio((char *)"f_login_type",(short *)0,1,32,0,private_word);
          } /* IIfsetio */
        }
/* # line 279 "nuedit.sc" */	/* host code */
	    	lcl_private = TRUE;
	    }
	    else if (!CMcmpnocase(privglob,global_word))
	    {
	    	lcl_private = FALSE;
/* # line 284 "nuedit.sc" */	/* putform */
        {
          if (IIfsetio((char *)"logpassedit") != 0) {
            IIputfldio((char *)"f_login_type",(short *)0,1,32,0,global_word);
          } /* IIfsetio */
        }
/* # line 285 "nuedit.sc" */	/* host code */
	    }
	    else
	    {
	    	IIUGerr(E_ST000B_p_or_g,0,0);
/* # line 289 "nuedit.sc" */	/* resume */
        {
          IIresfld((char *)"f_login_type");
          if (1) goto IIfdB1;
        }
/* # line 290 "nuedit.sc" */	/* host code */
	    }
/* # line 291 "nuedit.sc" */	/* resume */
        {
          IIresnext();
          if (1) goto IIfdB1;
        }
      } else if (IIretval() == 5) {
/* # line 296 "nuedit.sc" */	/* getform */
        {
          if (IIfsetio(logpassFormName) != 0) {
            IIgetfldio((short *)0,1,32,10 + 1-1,lcl_logintype,(char *)
"f_login_type");
          } /* IIfsetio */
        }
/* # line 298 "nuedit.sc" */	/* host code */
	    STcopy(lcl_logintype, lc_prompt_word);
	    CMtolower(lc_prompt_word, lc_prompt_word);
	    if (*lcl_logintype != EOS) 
	       STprintf( msg, STequal(lcl_login,INST_PWD_LOGIN)? 
	                 ERget(S_ST0031_enter_inst_pw):
	                 ERget(S_ST0030_enter_pw), lc_prompt_word );
/* # line 304 "nuedit.sc" */	/* putform */
        {
          if (IIfsetio(logpassFormName) != 0) {
            IIputfldio((char *)"prompt_line",(short *)0,1,32,0,msg);
            IIputfldio((char *)"noecho_reminder",(short *)0,1,32,0,
            ERget(S_ST0033_password_wont_echo));
          } /* IIfsetio */
        }
/* # line 307 "nuedit.sc" */	/* resume */
        {
          IIresnext();
          if (1) goto IIfdB1;
        }
      } else if (IIretval() == 6) {
/* # line 312 "nuedit.sc" */	/* putform */
        {
          if (IIfsetio(logpassFormName) != 0) {
            IIputfldio((char *)"prompt_line",(short *)0,1,32,0,
            ERget(S_ST0029_confirm_pw));
          } /* IIfsetio */
        }
/* # line 314 "nuedit.sc" */	/* resume */
        {
          IIresnext();
          if (1) goto IIfdB1;
        }
      } else if (IIretval() == 7) {
/* # line 319 "nuedit.sc" */	/* getform */
        {
          if (IIfsetio(logpassFormName) != 0) {
            IIgetfldio((short *)0,1,32,32 + 1-1,lcl_login,(char *)"f_l\
ogin");
            IIgetfldio((short *)0,1,32,32 + 1-1,lcl_pass,(char *)
"f_password");
          } /* IIfsetio */
        }
/* # line 321 "nuedit.sc" */	/* host code */
	    if (*lcl_pass == EOS && *lcl_login != EOS)
/* # line 322 "nuedit.sc" */	/* resume */
        {
          IIresfld((char *)"f_password");
          if (1) goto IIfdB1;
        }
/* # line 323 "nuedit.sc" */	/* resume */
        {
          IIresnext();
          if (1) goto IIfdB1;
        }
      } else if (IIretval() == 8) {
/* # line 328 "nuedit.sc" */	/* putform */
        {
          if (IIfsetio(logpassFormName) != 0) {
            IIputfldio((char *)"noecho_reminder",(short *)0,1,32,0,(char *)"");
            IIputfldio((char *)"prompt_line",(short *)0,1,32,0,(char *)"");
          } /* IIfsetio */
        }
/* # line 330 "nuedit.sc" */	/* resume */
        {
          IIresnext();
          if (1) goto IIfdB1;
        }
      } else if (IIretval() == 9) {
/* # line 335 "nuedit.sc" */	/* putform */
        {
          if (IIfsetio(logpassFormName) != 0) {
            IIputfldio((char *)"noecho_reminder",(short *)0,1,32,0,(char *)"");
            IIputfldio((char *)"prompt_line",(short *)0,1,32,0,(char *)"");
          } /* IIfsetio */
        }
/* # line 336 "nuedit.sc" */	/* resume */
        {
          IIresnext();
          if (1) goto IIfdB1;
        }
      } else if (IIretval() == 10) {
/* # line 341 "nuedit.sc" */	/* getform */
        {
          if (IIfsetio(logpassFormName) != 0) {
            IIgetfldio((short *)0,1,32,32 + 1-1,lcl_login,(char *)"f_l\
ogin");
          } /* IIfsetio */
        }
/* # line 342 "nuedit.sc" */	/* host code */
	    if (*lcl_login == EOS)
		*lcl_pass = EOS;
/* # line 344 "nuedit.sc" */	/* resume */
        {
          IIresnext();
          if (1) goto IIfdB1;
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
    if (IInmuact(ERget(FE_Save),(char *)0,2,2,1) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Cancel),(char *)0,2,2,2) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,2,0,2) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Help),(char *)0,2,2,3) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,3) == 0) goto IIfdE1;
    if (IIFRafActFld((char *)"f_login_type",0,4) == 0) goto IIfdE1;
    if (IIFRafActFld((char *)"f_password",1,5) == 0) goto IIfdE1;
    if (IIFRafActFld((char *)"f_password",0,6) == 0) goto IIfdE1;
    if (IIFRafActFld((char *)"f_password_conf",1,7) == 0) goto IIfdE1;
    if (IIFRafActFld((char *)"f_password_conf",0,8) == 0) goto IIfdE1;
    if (IIFRafActFld((char *)"f_login",1,9) == 0) goto IIfdE1;
    if (IIFRafActFld((char *)"f_login",0,10) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 348 "nuedit.sc" */	/* host code */
	return rtn;
}
/*
**  nu_connedit() -- Popup form for editing or entering connection data
**
**  Inputs:
**
**    new -- boolean, TRUE if this is a new entry, FALSE if we're editing
**           an existing entry.
**    cnode -- the virtual node name.  Not edited.
**    cpriv -- pointer to boolean, TRUE for private, FALSE for global.  May
**             be changed by editing.
**    caddr -- the network address.  May be changed by editing.
**    clist -- the listen address.  May be changed by editing.
**    cprot -- the protocol name.  May be changed by editing.
**
**  Returns:
**
**    EDIT_OK -- Edit approved by the user
**    EDIT_CANCEL -- Edit was cancelled by the user
**    EDIT_EMPTY -- User didn't change anything
**    < 0 -- This is a duplicate entry.  The value returned is the negative
**           of the row number for the entry that was duplicated.
** History:
**	23-Jan-98 (rajus01)
**	   Changed the order of field names so that the cursor is positioned
**	   at Network address field.(connedit.frm). Fixed problems with the
**	   display of connection type on the netutil form.
*/
i4  nu_connedit(new, cnode, cpriv, caddr, clist, cprot) 
bool new, *cpriv;
  char *cnode, *caddr, *clist, *cprot;
{ 
    i4  rtn; 
    /* Make sure the sizes of these variables match form definitions */
  char fldname[64];
  bool lcl_private = *cpriv;
  char lcl_privglob[12];
  char lcl_address[128 + 1];
  char lcl_listen[64 + 1];
  char lcl_protocol[64 + 1];
  bool changed;
  char *disp;
  GLOBALREF char *def_protocol;
    char msg[200];
    STprintf(msg, "%s - Connection data", SystemNetutilName); 
    /* Find the form... */
    STcopy(ERx("connedit"), conneditFormName);
    if (IIUFgtfGetForm(IIUFlcfLocateForm(), conneditFormName) != OK)
        PCexit(FAIL);
    /* ...and display it.  Top of display loop. */
/* # line 412 "nuedit.sc" */	/* display */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    if (IIdispfrm(conneditFormName,(char *)"f") == 0) goto IIfdE2;
    goto IImuI2;
IIfdI2:;
    if (IIfsetio((char *)0) == 0) goto IIfdE2;
    IIputfldio((char *)"f_node",(short *)0,1,32,0,cnode);
/* # line 417 "nuedit.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(5,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        IIstfsio(4,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        IIstfsio(6,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        IIstfsio(2,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        IIstfsio(3,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 420 "nuedit.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(41,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(39,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(40,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(36,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(37,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 424 "nuedit.sc" */	/* set_frs */
    {
      if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
        IIstfsio(67,ERget(F_ST0001_list_choices),0,(short *)0,1,30,sizeof(0),
        (void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 426 "nuedit.sc" */	/* host code */
        if (new)
        {
	    disp = ERget(S_ST0014_define_new_conn);
/* # line 429 "nuedit.sc" */	/* putform */
    {
      if (IIfsetio(conneditFormName) != 0) {
        IIputfldio((char *)"f_address",(short *)0,1,32,0,cnode);
        IIputfldio((char *)"f_protocol",(short *)0,1,32,0,def_protocol);
      } /* IIfsetio */
    }
/* # line 431 "nuedit.sc" */	/* host code */
        }
        else
        {
	    disp = ERget(S_ST0015_edit_conn);
/* # line 435 "nuedit.sc" */	/* putform */
    {
      if (IIfsetio(conneditFormName) != 0) {
        IIputfldio((char *)"f_listen",(short *)0,1,32,0,clist);
        IIputfldio((char *)"f_protocol",(short *)0,1,32,0,cprot);
        IIputfldio((char *)"f_address",(short *)0,1,32,0,caddr);
      } /* IIfsetio */
    }
/* # line 439 "nuedit.sc" */	/* host code */
        }
/* # line 441 "nuedit.sc" */	/* putform */
    {
      if (IIfsetio(conneditFormName) != 0) {
        IIputfldio((char *)"infoline",(short *)0,1,32,0,disp);
      } /* IIfsetio */
    }
/* # line 443 "nuedit.sc" */	/* host code */
	 if( new )
            disp = edit_global ? global_word: private_word;
	 else
            disp = *cpriv ? private_word: global_word;
/* # line 447 "nuedit.sc" */	/* putform */
    {
      if (IIfsetio(conneditFormName) != 0) {
        IIputfldio((char *)"f_privglob",(short *)0,1,32,0,disp);
      } /* IIfsetio */
    }
IIfdB2:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 453 "nuedit.sc" */	/* inquire_frs */
        {
          if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIiqfsio((short *)0,1,30,1,&changed,20,conneditFormName,0);
          } /* IIiqset */
        }
/* # line 454 "nuedit.sc" */	/* host code */
	if (!changed)
        { 
	    *caddr = *clist = *cprot = EOS;
	    rtn = EDIT_EMPTY;
/* # line 458 "nuedit.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE2;
        }
/* # line 459 "nuedit.sc" */	/* host code */
	}
/* # line 461 "nuedit.sc" */	/* getform */
        {
          if (IIfsetio(conneditFormName) != 0) {
            IIgetfldio((short *)0,1,32,11,lcl_privglob,(char *)"f_priv\
glob");
            IIgetfldio((short *)0,1,32,128 + 1-1,lcl_address,(char *)
"f_address");
            IIgetfldio((short *)0,1,32,64 + 1-1,lcl_protocol,(char *)
"f_protocol");
            IIgetfldio((short *)0,1,32,64 + 1-1,lcl_listen,(char *)
"f_listen");
          } /* IIfsetio */
        }
/* # line 467 "nuedit.sc" */	/* host code */
	if (*lcl_privglob == EOS)  
	{ 
	    IIUGerr(E_ST000B_p_or_g,0,0);
/* # line 470 "nuedit.sc" */	/* resume */
        {
          IIresfld((char *)"f_privglob");
          if (1) goto IIfdB2;
        }
/* # line 471 "nuedit.sc" */	/* host code */
	} 
	if (*lcl_address == EOS)  
	{
            IIUGerr(E_ST000C_net_address_req,0,0);
/* # line 476 "nuedit.sc" */	/* resume */
        {
          IIresfld((char *)"f_address");
          if (1) goto IIfdB2;
        }
/* # line 477 "nuedit.sc" */	/* host code */
	} 
	if (*lcl_protocol == EOS) 
	{
            IIUGerr(E_ST000D_protocol_req,0,0);
/* # line 482 "nuedit.sc" */	/* resume */
        {
          IIresfld((char *)"f_protocol");
          if (1) goto IIfdB2;
        }
/* # line 483 "nuedit.sc" */	/* host code */
        } 
	if (*lcl_listen == EOS)  
	{ 
	    IIUGerr(E_ST000E_listen_req,0,0);
/* # line 488 "nuedit.sc" */	/* resume */
        {
          IIresfld((char *)"f_listen");
          if (1) goto IIfdB2;
        }
/* # line 489 "nuedit.sc" */	/* host code */
	} 
	*cpriv = lcl_private;
        STcopy(lcl_address,caddr);
	STcopy(lcl_listen,clist);
        STcopy(lcl_protocol,cprot);
	if (NULL != nu_locate_conn(cnode,lcl_private,caddr,clist,cprot,&rtn))
        { 
	    rtn = -rtn; 
	    IIUGerr(E_ST000F_duplicate_conn,0,0);
	} 
	else 
	    rtn = EDIT_OK; 
/* # line 503 "nuedit.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE2;
        }
      } else if (IIretval() == 2) {
/* # line 508 "nuedit.sc" */	/* host code */
        rtn = EDIT_CANCEL;
/* # line 509 "nuedit.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE2;
        }
      } else if (IIretval() == 3) {
        char *prot;
/* # line 518 "nuedit.sc" */	/* host code */
	if (NULL != (prot = protocol_listpick(TRUE)))
	{
/* # line 520 "nuedit.sc" */	/* putform */
        {
          if (IIfsetio(conneditFormName) != 0) {
            IIputfldio((char *)"f_protocol",(short *)0,1,32,0,prot);
          } /* IIfsetio */
        }
/* # line 521 "nuedit.sc" */	/* set_frs */
        {
          if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIstfsio(20,conneditFormName,0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 522 "nuedit.sc" */	/* set_frs */
        {
          if (IIiqset(2,0,conneditFormName,(char *)0, (char *)0) != 0) {
            IIstfsio(20,(char *)"f_protocol",0,(short *)0,1,30,sizeof(1),
            (void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 523 "nuedit.sc" */	/* redisplay */
        {
          IIredisp();
        }
/* # line 524 "nuedit.sc" */	/* host code */
	}
      } else if (IIretval() == 4) {
        FEhelp(ERx("connedit.hlp"), msg);
      } else if (IIretval() == 5) {
/* # line 535 "nuedit.sc" */	/* inquire_frs */
        {
          if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIiqfsio((short *)0,1,32,63,fldname,22,(char *)0,0);
          } /* IIiqset */
        }
/* # line 536 "nuedit.sc" */	/* host code */
	if (STcompare(fldname, ERx("f_protocol")) == 0) 
	{
/* # line 538 "nuedit.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget(F_ST0001_list_choices),0,(short *)0,1,30,sizeof(
            1),(void *)IILQint(1));
          } /* IIiqset */
        }
/* # line 539 "nuedit.sc" */	/* host code */
	}
	else
	{
/* # line 542 "nuedit.sc" */	/* set_frs */
        {
          if (IIiqset(7,0,(char *)"",(char *)0, (char *)0) != 0) {
            IIstfsio(67,ERget(F_ST0001_list_choices),0,(short *)0,1,30,sizeof(
            0),(void *)IILQint(0));
          } /* IIiqset */
        }
/* # line 543 "nuedit.sc" */	/* host code */
	}
/* # line 544 "nuedit.sc" */	/* resume */
        {
          IIresnext();
          if (1) goto IIfdB2;
        }
      } else if (IIretval() == 6) {
        char privglob[12];
/* # line 553 "nuedit.sc" */	/* getform */
        {
          if (IIfsetio(conneditFormName) != 0) {
            IIgetfldio((short *)0,1,32,11,privglob,(char *)"f_privglob");
          } /* IIfsetio */
        }
/* # line 555 "nuedit.sc" */	/* host code */
	if (nu_is_private(privglob))
	{
/* # line 557 "nuedit.sc" */	/* putform */
        {
          if (IIfsetio(conneditFormName) != 0) {
            IIputfldio((char *)"f_privglob",(short *)0,1,32,0,private_word);
          } /* IIfsetio */
        }
/* # line 558 "nuedit.sc" */	/* host code */
	    lcl_private = TRUE;
	}
	else if (!CMcmpnocase(privglob,global_word))
	{
	    lcl_private = FALSE;
/* # line 563 "nuedit.sc" */	/* putform */
        {
          if (IIfsetio(conneditFormName) != 0) {
            IIputfldio((char *)"f_privglob",(short *)0,1,32,0,global_word);
          } /* IIfsetio */
        }
/* # line 564 "nuedit.sc" */	/* host code */
	}
	else
	{
	    IIUGerr(E_ST000B_p_or_g,0,0);
/* # line 568 "nuedit.sc" */	/* resume */
        {
          IIresfld((char *)"f_privglob");
          if (1) goto IIfdB2;
        }
/* # line 569 "nuedit.sc" */	/* host code */
	}
/* # line 570 "nuedit.sc" */	/* resume */
        {
          IIresnext();
          if (1) goto IIfdB2;
        }
      } else if (IIretval() == 7) {
        char privglob[12];
/* # line 579 "nuedit.sc" */	/* getform */
        {
          if (IIfsetio(conneditFormName) != 0) {
            IIgetfldio((short *)0,1,32,11,privglob,(char *)"f_privglob");
          } /* IIfsetio */
        }
/* # line 581 "nuedit.sc" */	/* host code */
	if (nu_is_private(privglob))
	{
/* # line 583 "nuedit.sc" */	/* putform */
        {
          if (IIfsetio(conneditFormName) != 0) {
            IIputfldio((char *)"f_privglob",(short *)0,1,32,0,private_word);
          } /* IIfsetio */
        }
/* # line 584 "nuedit.sc" */	/* host code */
	    lcl_private = TRUE;
	}
	else if (!CMcmpnocase(privglob,global_word))
	{
	    lcl_private = FALSE;
/* # line 589 "nuedit.sc" */	/* putform */
        {
          if (IIfsetio(conneditFormName) != 0) {
            IIputfldio((char *)"f_privglob",(short *)0,1,32,0,global_word);
          } /* IIfsetio */
        }
/* # line 590 "nuedit.sc" */	/* host code */
	}
	else
	{
	    IIUGerr(E_ST000B_p_or_g,0,0);
/* # line 594 "nuedit.sc" */	/* resume */
        {
          IIresfld((char *)"f_privglob");
          if (1) goto IIfdB2;
        }
/* # line 595 "nuedit.sc" */	/* host code */
	}
/* # line 596 "nuedit.sc" */	/* resume */
        {
          IIresnext();
          if (1) goto IIfdB2;
        }
      } else {
        if (1) goto IIfdE2;
      } /* IIretval */
    } /* IIrunform */
IIfdF2:
    if (IIchkfrm() == 0) goto IIfdB2;
    if (IIfsetio((char *)0) == 0) goto IIfdE2;
    goto IIfdE2;
IImuI2:
    if (IIinitmu() == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_Save),(char *)0,2,2,1) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE2;
    if (IInmuact(ERget(FE_Cancel),(char *)0,2,2,2) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,2,0,2) == 0) goto IIfdE2;
    if (IInmuact(ERget(F_ST0001_list_choices),(char *)0,2,0,3) == 0) goto 
    IIfdE2;
    if (IInmuact(ERget(FE_Help),(char *)0,2,2,4) == 0) goto IIfdE2;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,4) == 0) goto IIfdE2;
    if (IIFRafActFld((char *)"all",1,5) == 0) goto IIfdE2;
    if (IIFRafActFld((char *)"f_privglob",0,6) == 0) goto IIfdE2;
    if (IIFRafActFld((char *)"f_address",1,7) == 0) goto IIfdE2;
    if (IIendmu() == 0) goto IIfdE2;
    goto IIfdI2;
IIfdE2:;
    IIendfrm();
  }
/* # line 600 "nuedit.sc" */	/* host code */
    return rtn;
}
/*
**  protocol_listpick() -- Gets a ListChoices for possible protocols
**
**  Returns a character string that contains the chosen item out of a
**  a list of possible protocols.
** 
**  (joplin) Note:  There were several methods that were attempted by
**  the original author (jonb).  However, what I finally settled on
**  was a simple regurgatation of the protocol values listed in the
**  documentation for NETU,NETUTIL.  Please check out the older 
**  revisions of this file to see the programatic methods used.
**
**  Inputs:
**
**    really  -- flag to tell us if we *really* want to do this
**
**  Returns:
**
**    A Pointer to a character string representing a protocal.
**
*/
static char *
protocol_listpick(really)
bool really;
{
    i4  lpx;
    static char lpstr[256];
    char *sp, *rtn;
/*
**  Straight from the original documentation, here is the 
**  List of Protocols and the amazing dancing bear, Checkers.
*/
    *lpstr = EOS;
#  ifdef WIN16
    STcat(lpstr, ERx("winsock\n"));
    STcat(lpstr, ERx("netbios\n"));
    STcat(lpstr, ERx("decnet\n"));
    STcat(lpstr, ERx("nvl_spx\n"));
    STcat(lpstr, ERx("tcp_dec\n"));
    STcat(lpstr, ERx("tcp_ftp\n"));
    STcat(lpstr, ERx("tcp_lmx\n"));
    STcat(lpstr, ERx("tcp_nfs\n"));
    STcat(lpstr, ERx("tcp_wol\n"));
    STcat(lpstr, ERx("lan_workplace\n"));
#  else
# ifdef NT_GENERIC
    STcat(lpstr, ERx("wintcp\n"));
    STcat(lpstr, ERx("tcp_ip\n"));
    STcat(lpstr, ERx("lanman\n"));
    STcat(lpstr, ERx("nvlspx\n"));
    STcat(lpstr, ERx("decnet\n"));
# else
    STcat(lpstr, ERx("decnet\n"));
    STcat(lpstr, ERx("sna_lu0\n"));
    STcat(lpstr, ERx("sna_lu62\n"));
    STcat(lpstr, ERx("tcp_dec\n"));
    STcat(lpstr, ERx("tcp_ibm\n"));
    STcat(lpstr, ERx("tcp_ip\n"));
    STcat(lpstr, ERx("tcp_knet\n"));
    STcat(lpstr, ERx("tcp_wol\n"));
# endif
#  endif
    if (*lpstr == EOS)
	return NULL;
    if (!really)
	return lpstr;  /* It's ok, it won't be used.  Any non-null will do. */
    /* Display the ListChoices for selection */
    lpx = IIFDlpListPick( ERget(S_ST0041_protocol_prompt), lpstr, 
			  0, -1, -1, NULL, NULL, NULL, NULL );
    /* If nothing was picked just return an empty string */
    if (lpx < 0)
	return NULL;
    /* Find the selection in the delimitted character string */
    for (rtn = lpstr; lpx; lpx--)
    {
	rtn = STindex(rtn, ERx("\n"), 0);
	if (rtn == NULL)
	    return NULL;
	CMnext(rtn);
    }
    /* return the results of the string search */
    if (NULL != (sp = STindex(rtn, ERx("\n"), 0)))
	*sp = EOS;
    return rtn;
}
/*
**  nu_locate_conn() -- Locate a connection entry in the internal data
**
**  Loops through the internal data structure looking for a connection
**  entry that matches the criteria specified by the input arguments.
**  This has the effect of making the corresponding member of the internal
**  list the "current" member, which is convenient if it needs to be
**  deleted or modified.  This function can also be used as a simple
**  predicate to see if the specified entry currently exists or not.
**  
**  Inputs:
**
**    vnode -- the virtual node name
**    private -- TRUE for a private entry, FALSE for a global
**    address -- the network address \
**    listen -- the listen address    > of the entry being searched for
**    protocol -- the protocol name  /
**    row -- may be NULL; if not, points to i4  which receives the row
**           number of the match, if any.
**
**  Returns:
**
**    Pointer to connection entry in internal data structure.
**    
*/
CONN *
nu_locate_conn(char *vnode, i4  private, 
	       char *address, char *listen, char *protocol, i4  *row)
{
    bool flag;
    char *ad, *li, *pr;
    i4  pv, lrow;
    CONN *rtn;
    /* Loop through the data structure looking for a match... */
    for ( flag=TRUE, lrow=1; /* "flag" just says to rewind list the 1st time */
	  NULL != (rtn = nu_vnode_conn(flag, &pv, vnode, &ad, &li, &pr)); 
          flag=FALSE, lrow++ )
    {
        if ( pv == private  &&
             !STbcompare(address,  0, ad, 0, FALSE) &&
	     !STbcompare(listen,   0, li, 0, FALSE) &&
	     !STbcompare(protocol, 0, pr, 0, FALSE) ) 
	{
	    /* Found one. */
	    if (row != NULL) 
		*row = lrow;
	    return rtn;
	}
    }
    return NULL;
}
/*
**  nu_is_private() 
**
**  Converts the words "Private" and "Global" to a boolean.  Input argument
**  is the word to be examined.  Returns TRUE for private, FALSE for global.
*/
bool
nu_is_private(char *pg)
{
    return (!CMcmpnocase(pg,private_word));
}
/*
**  Name: nu_attredit() 
**
**  Description:
**	Edit popup for attribute information
**
**  Input:
**    new		boolean for new attribute or edit existing attribute.
**    vnode 		vnode name this entry is associated with (not edited)
**    privglob		"Private or Global"
**    name		attribute name
**    value		attribute value
**
**  Output:
**	None.
** 
**  Returns:
**    EDIT_OK 		Edit approved by the user
**    EDIT_CANCEL 	Edit was cancelled by the user
**    EDIT_EMPTY 	User didn't change anything
**
** History:
**	08-Aug-97 (rajus01)
**	    Created.
**	23-Jan-98 (rajus01)
**	   Changed the order of field names so that the cursor is positioned
**	   at attribute value field.(attribedit.frm). Fixed problems with the
**	   display of attribute type on the netutil form.
*/
i4  nu_attredit( new, vnode, privglob, name, value ) 
bool 	new, *privglob;
  char *vnode, *name, *value;
{ 
    i4  rtn; 
    /* Make sure the sizes of these variables match form definitions */
  bool lcl_type = *privglob;
  char lcl_privglob[12];
  char lcl_name[64 + 1];
  char lcl_value[64 + 1];
  bool changed;
  char *disp;
    /* Find the form... */
    if( IIUFgtfGetForm( IIUFlcfLocateForm(), ERx( "attribedit" ) ) != OK )
        PCexit(FAIL);
    /* ...and display it.  Top of display loop. */
/* # line 822 "nuedit.sc" */	/* display */
  {
    IIFRgpcontrol(1,0);
    IIFRgpsetio(6,(short *)0,1,30,sizeof(301),(void *)IILQint(301));
    IIFRgpcontrol(2,0);
    if (IIdispfrm((char *)"attribedit",(char *)"f") == 0) goto IIfdE3;
    goto IImuI3;
IIfdI3:;
    if (IIfsetio((char *)0) == 0) goto IIfdE3;
    IIputfldio((char *)"f_node",(short *)0,1,32,0,vnode);
/* # line 827 "nuedit.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(5,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        IIstfsio(4,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        IIstfsio(6,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        IIstfsio(2,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
        IIstfsio(3,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 830 "nuedit.sc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(41,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(39,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(40,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(36,(char *)0,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
        IIstfsio(37,(char *)0,0,(short *)0,1,30,sizeof(0),(void *)IILQint(0));
      } /* IIiqset */
    }
/* # line 834 "nuedit.sc" */	/* host code */
        if( new )
        {
	    disp = ERget( S_ST0535_define_new_attr );
/* # line 837 "nuedit.sc" */	/* putform */
    {
      if (IIfsetio((char *)"attribedit") != 0) {
        IIputfldio((char *)"attributename",(short *)0,1,32,0,(char *)"");
        IIputfldio((char *)"attributevalue",(short *)0,1,32,0,(char *)"");
      } /* IIfsetio */
    }
/* # line 839 "nuedit.sc" */	/* host code */
        }
        else
        {
	    disp = ERget( S_ST0536_edit_attr );
/* # line 843 "nuedit.sc" */	/* putform */
    {
      if (IIfsetio((char *)"attribedit") != 0) {
        IIputfldio((char *)"attributename",(short *)0,1,32,0,name);
        IIputfldio((char *)"attributevalue",(short *)0,1,32,0,value);
      } /* IIfsetio */
    }
/* # line 846 "nuedit.sc" */	/* host code */
        }
/* # line 848 "nuedit.sc" */	/* putform */
    {
      if (IIfsetio((char *)"attribedit") != 0) {
        IIputfldio((char *)"infoline",(short *)0,1,32,0,disp);
      } /* IIfsetio */
    }
/* # line 850 "nuedit.sc" */	/* host code */
	if( new )
           disp = edit_global ? global_word: private_word;
	else
           disp = *privglob ? private_word: global_word;
/* # line 854 "nuedit.sc" */	/* putform */
    {
      if (IIfsetio((char *)"attribedit") != 0) {
        IIputfldio((char *)"f_privglob",(short *)0,1,32,0,disp);
      } /* IIfsetio */
    }
IIfdB3:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
/* # line 859 "nuedit.sc" */	/* inquire_frs */
        {
          if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
            IIiqfsio((short *)0,1,30,1,&changed,20,(char *)"attribedit",0);
          } /* IIiqset */
        }
/* # line 860 "nuedit.sc" */	/* host code */
	if( !changed )
        { 
	    *name = *value = EOS;
	    rtn   = EDIT_EMPTY;
/* # line 864 "nuedit.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE3;
        }
/* # line 865 "nuedit.sc" */	/* host code */
	}
/* # line 867 "nuedit.sc" */	/* getform */
        {
          if (IIfsetio((char *)"attribedit") != 0) {
            IIgetfldio((short *)0,1,32,11,lcl_privglob,(char *)"f_priv\
glob");
            IIgetfldio((short *)0,1,32,64 + 1-1,lcl_name,(char *)
"attributename");
            IIgetfldio((short *)0,1,32,64 + 1-1,lcl_value,(char *)
"attributevalue");
          } /* IIfsetio */
        }
/* # line 872 "nuedit.sc" */	/* host code */
	if( *lcl_privglob == EOS )  
	{ 
	    IIUGerr( E_ST000B_p_or_g, 0, 0 );
/* # line 875 "nuedit.sc" */	/* resume */
        {
          IIresfld((char *)"f_privglob");
          if (1) goto IIfdB3;
        }
/* # line 876 "nuedit.sc" */	/* host code */
	} 
	if( *lcl_name == EOS )  
	{
            IIUGerr( E_ST0137_attribname_req, 0, 0 );
/* # line 881 "nuedit.sc" */	/* resume */
        {
          IIresfld((char *)"attributename");
          if (1) goto IIfdB3;
        }
/* # line 882 "nuedit.sc" */	/* host code */
	} 
	if( *lcl_value == EOS ) 
	{
            IIUGerr( E_ST0138_attribval_req, 0, 0 );
/* # line 887 "nuedit.sc" */	/* resume */
        {
          IIresfld((char *)"attributevalue");
          if (1) goto IIfdB3;
        }
/* # line 888 "nuedit.sc" */	/* host code */
        } 
	*privglob = lcl_type;
        STcopy( lcl_name, name );
	STcopy( lcl_value, value );
	if( nu_locate_attr( vnode, lcl_type, name, value, &rtn ) != NULL )
        { 
	    rtn = -rtn; 
	    IIUGerr( E_ST020C_duplicate_attr, 0, 0 );
	} 
	else 
	    rtn = EDIT_OK; 
/* # line 901 "nuedit.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE3;
        }
      } else if (IIretval() == 2) {
/* # line 906 "nuedit.sc" */	/* host code */
        rtn = EDIT_CANCEL;
/* # line 907 "nuedit.sc" */	/* breakdisplay */
        {
          if (1) goto IIfdE3;
        }
      } else if (IIretval() == 3) {
/* # line 912 "nuedit.sc" */	/* host code */
        FEhelp( ERx( "attredit.hlp" ), ERx( "Netutil - Attribute data" ) );
      } else if (IIretval() == 4) {
        char privglob[12];
/* # line 921 "nuedit.sc" */	/* getform */
        {
          if (IIfsetio((char *)"attribedit") != 0) {
            IIgetfldio((short *)0,1,32,11,privglob,(char *)"f_privglob");
          } /* IIfsetio */
        }
/* # line 923 "nuedit.sc" */	/* host code */
	if( nu_is_private( privglob ) )
	{
/* # line 925 "nuedit.sc" */	/* putform */
        {
          if (IIfsetio((char *)"attribedit") != 0) {
            IIputfldio((char *)"f_privglob",(short *)0,1,32,0,private_word);
          } /* IIfsetio */
        }
/* # line 926 "nuedit.sc" */	/* host code */
	    lcl_type = TRUE;
	}
	else if( !CMcmpnocase( privglob, global_word ) )
	{
	    lcl_type = FALSE;
/* # line 931 "nuedit.sc" */	/* putform */
        {
          if (IIfsetio((char *)"attribedit") != 0) {
            IIputfldio((char *)"f_privglob",(short *)0,1,32,0,global_word);
          } /* IIfsetio */
        }
/* # line 932 "nuedit.sc" */	/* host code */
	}
	else
	{
	    IIUGerr( E_ST000B_p_or_g, 0, 0 );
/* # line 936 "nuedit.sc" */	/* resume */
        {
          IIresfld((char *)"f_privglob");
          if (1) goto IIfdB3;
        }
/* # line 937 "nuedit.sc" */	/* host code */
	}
/* # line 938 "nuedit.sc" */	/* resume */
        {
          IIresnext();
          if (1) goto IIfdB3;
        }
      } else if (IIretval() == 5) {
        char privglob[12];
/* # line 947 "nuedit.sc" */	/* getform */
        {
          if (IIfsetio((char *)"attribedit") != 0) {
            IIgetfldio((short *)0,1,32,11,privglob,(char *)"f_privglob");
          } /* IIfsetio */
        }
/* # line 949 "nuedit.sc" */	/* host code */
	if( nu_is_private( privglob ) )
	{
/* # line 951 "nuedit.sc" */	/* putform */
        {
          if (IIfsetio((char *)"attribedit") != 0) {
            IIputfldio((char *)"f_privglob",(short *)0,1,32,0,private_word);
          } /* IIfsetio */
        }
/* # line 952 "nuedit.sc" */	/* host code */
	    lcl_type = TRUE;
	}
	else if( !CMcmpnocase( privglob, global_word ) )
	{
	    lcl_type = FALSE;
/* # line 957 "nuedit.sc" */	/* putform */
        {
          if (IIfsetio((char *)"attribedit") != 0) {
            IIputfldio((char *)"f_privglob",(short *)0,1,32,0,global_word);
          } /* IIfsetio */
        }
/* # line 958 "nuedit.sc" */	/* host code */
	}
	else
	{
	    IIUGerr( E_ST000B_p_or_g, 0, 0 );
/* # line 962 "nuedit.sc" */	/* resume */
        {
          IIresfld((char *)"f_privglob");
          if (1) goto IIfdB3;
        }
/* # line 963 "nuedit.sc" */	/* host code */
	}
/* # line 964 "nuedit.sc" */	/* resume */
        {
          IIresnext();
          if (1) goto IIfdB3;
        }
      } else {
        if (1) goto IIfdE3;
      } /* IIretval */
    } /* IIrunform */
IIfdF3:
    if (IIchkfrm() == 0) goto IIfdB3;
    if (IIfsetio((char *)0) == 0) goto IIfdE3;
    goto IIfdE3;
IImuI3:
    if (IIinitmu() == 0) goto IIfdE3;
    if (IInmuact(ERget( FE_Save ),(char *)0,2,2,1) == 0) goto IIfdE3;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE3;
    if (IInmuact(ERget( FE_Cancel ),(char *)0,2,2,2) == 0) goto IIfdE3;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,2,0,2) == 0) goto IIfdE3;
    if (IInmuact(ERget( FE_Help ),(char *)0,2,2,3) == 0) goto IIfdE3;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,3) == 0) goto IIfdE3;
    if (IIFRafActFld((char *)"f_privglob",0,4) == 0) goto IIfdE3;
    if (IIFRafActFld((char *)"attributename",1,5) == 0) goto IIfdE3;
    if (IIendmu() == 0) goto IIfdE3;
    goto IIfdI3;
IIfdE3:;
    IIendfrm();
  }
/* # line 968 "nuedit.sc" */	/* host code */
    return rtn;
}
/*
**  Name: nu_locate_attr() 
**
**  Description:
**  	Loops through the internal data structure looking for an attribute
**  	entry that matches the criteria specified by the input arguments.
**  	This has the effect of making the corresponding member of the internal
**  	list the "current" member, which is convenient if it needs to be
**  	deleted or modified.  This function can also be used as a simple
**  	predicate to see if the specified entry currently exists or not.
**  
**  Input:
**    vnode 		The virtual node name
**    private 		TRUE for a private entry, FALSE for a global
**    name 		the attr_name }
**    value		the attr_value}--> of the entry being searched for
**    row 		may be NULL; if not, points to i4  which receives 
**			the row number of the match, if any.
**
**  Output:
**	None.
**
**  Returns:
**    Pointer to attribute entry in internal data structure.
**    
** History:
**	08-Aug-97 (rajus01)
**	    Created.
*/
ATTR *
nu_locate_attr( char *vnode, i4  private, 
	       char *name, char *value, i4  *row )
{
    bool 	flag;
    char 	*nam, *val;
    i4  	pv, lrow;
    ATTR 	*rtn;
    /* Loop through the data structure looking for a match... */
    for( flag=TRUE, lrow=1; /* "flag" just says to rewind list the 1st time */
	  NULL != ( rtn = nu_vnode_attr( flag, &pv, vnode, &nam, &val ) ); 
          flag=FALSE, lrow++ )
    {
        if ( pv == private  &&
             !STbcompare( name,  0, nam, 0, FALSE ) &&
	     !STbcompare( value,   0, val, 0, FALSE ) ) 
	{
	    /* Found one. */
	    if( row != NULL ) 
		*row = lrow;
	    return rtn;
	}
    }
    return NULL;
}
