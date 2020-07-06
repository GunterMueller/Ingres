# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<st.h>
# include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ug.h>
# include	<ft.h>
# include	<fmt.h>
# include	<adf.h>
# include	<frame.h>
# include	<menu.h>
# include	<runtime.h>
# include	<fsicnsts.h>
# include	<help.h>
# include	<uf.h>
# include	"helphlp.h"
# include	"eruf.h"
/**
** Name:	fehkeys.qc -	help on command keys.
**
** Description:
**	This contains the help for keys, which is called up by the
**	standard help utility for RTI.
**
**	This file defines:
**
**	FEhkeys		- help for keys in effect.
**
** History:
**	Revision 5.0
**	07/03/86 (scl) Added code to supress commands not supported in 3270 mode
**	8/26/86 (bruceb)
**		Removed entries for Startfield and Endfield '[',']'
**		keys since equel programs really can't make use of those keys.
**	6-nov-86 (bruceb)	Fix for bug 9700
**		shorten text for Bottomedge; change 'bottom' to 'lower'.
**	07/23/87 (dkh) - Fixed jup bug 591.
**	08/14/87 (dkh) - ER changes.
**	09/17/87 (dkh) - Fixed problem with loading menu explanations.
**	10/05/87 (dkh) - Help file changes.
**	02/27/88 (dkh) - Added support for nextitem command.
**	05/27/88 (dkh) - Enabled display of nextitem command.
**	24-apr-89 (bruceb)
**		Added display of shell command.
**	27-sep-89 (bruceb)
**		Added code for 'disabled' message on editor key.
**	27-nov-89 (bruceb)
**		Find/top/bottom no longer visible menuitems.
**	29-nov-89 (bruceb)
**		Now display information on frskeys.  Also, help is
**		now displayed for menuitems, frskeys and frs commands,
**		in that order.
**	16-jan-90 (bruceb)
**		Improved scroll code.
**	22-jan-90 (bruceb)
**		Only display information on frskeys if label text exists.
**	16-mar-90 (swilson)
**		Eliminated overlap between menu keys and frs keys.
**	21-jan-92 (seg)
**		needed to change NULL to 0 in initialization due to OS2
**		porting peculiarity.
**	10-mar-92 (leighb) Check for MU_GUIDIVIDER character in menu item
**		name and replace with a '\0' if not using GUI menus.
**	29-mar-93 (fraser)
**		Removed ifdef on GUIMENU. 
**	23-sep-93 (vijay)
**		Remove STcompare defn. st.h is already included.
**	15-Feb-1995 (canor01)
**		Not finding the keys help form is non-fatal.
**	23-apr-1996 (chech02)
**		changed to FAR ptr for windows 3.1 port.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
# define	TITLELEN    80
# define	LABELWIDTH  60
# define	EXPLWIDTH   80
	/* Define masks used for various combinations of keys */
# define	HM_EQ	1		/* for EQUEL */
# define	HM_VFRBF 2		/* for VIFRED */
# define	HM_IQUEL 4		/* for IQUEL */
# define	HM_VIG	 8		/* for VIGRAPH */
# define	HM_ALL	15		/* for all of them */
# define	HM_NONE 0		/* for none of them */
# define	HM_FTASCII 1		/* supported in FT ASCII mode */
# define	HM_FT3270  2		/* supported in FT 3270	 mode */
# define	HM_FTBOTH  3		/* supported in both FT modes */
	/* Typedef */
  typedef struct {
    i4 key_labseq;
    ER_MSGID key_name;
    char *key_label;
    i4 key_mask;
    i4 key_FTmode;
    ER_MSGID key_desc;
  } KEYDATA;
typedef struct
{
	i4	val_vfrfiq;		/* For value of vfrfiq */
	i4	val_mask;		/* ..use this mask */
} VAL2HM;
	/* Forward Refs and Externs */
GLOBALREF	i4	form_mu;
#ifdef WIN16
GLOBALREF	FRS_CB	*FAR IIfrscb;
#else
GLOBALREF	FRS_CB	*IIfrscb;
#endif /* WIN16 */
VOID	FEhkeys();
	/* Statics */
static bool	keysfadded = FALSE;
static char	mu_guidivider[2] = {MU_GUIDIVIDER, '\0'};
static READONLY
  char _KeysForm[]= ERx("keysform"), _KeysTable[]= ERx("keystable"), _Command
  []= ERx("command"), _Key[]= ERx("key"), _Expl[]= ERx("explanation"), _Empty
  []= ERx("");
static VAL2HM Masktab[] = {
	H_EQ,		HM_EQ,
	H_VFRBF,	HM_VFRBF,
	H_IQUEL,	HM_IQUEL,
	H_VIG,		HM_VIG,
	H_QBFSRT,	HM_EQ,
	H_GREQ,		HM_EQ
};
  static KEYDATA Keytab[]= {
    {1, F_UF000E_Menu,	_Empty,	HM_ALL,			HM_FTBOTH,
	    F_UF000F_Menu_Desc},
    {2, F_UF0010_Scup,	_Empty,	HM_ALL & ~HM_VIG,	HM_FTBOTH,
	    F_UF0011_Scup_Desc},
    {3, F_UF0012_Scdn,	_Empty,	HM_ALL & ~HM_VIG,	HM_FTBOTH,
	    F_UF0013_Scdn_Desc},
    {4, F_UF0014_Nfld,	_Empty,	HM_EQ|HM_VFRBF,		HM_FTASCII,
	    F_UF0015_Nfld_Desc},
    {4, F_UF0016_Next,	_Empty,	HM_VIG,			HM_FTASCII,
	    F_UF0017_Next_Desc},
    {5, F_UF0018_Pfld,	_Empty,	HM_EQ|HM_VFRBF,		HM_FTASCII,
	    F_UF0019_Pfld_Desc},
    {5, F_UF001A_Prev,	_Empty,	HM_VIG,			HM_FTASCII,
	    F_UF001B_Prev_Desc},
    {6, F_UF001C_Sclt,	_Empty,	HM_ALL & ~HM_VIG,	HM_FTBOTH,
	    F_UF001D_Sclt_Desc},
    {7, F_UF001E_Scrt,	_Empty,	HM_ALL & ~HM_VIG,	HM_FTBOTH,
	    F_UF001F_Scrt_Desc},
    {0, F_UF0020_Ledg,	ERx("["),	HM_VFRBF,		HM_FTASCII,
	    F_UF0021_Ledg_Desc},
    {0, F_UF0022_Ledg,	ERx("["),	HM_VIG,			HM_FTASCII,
	    F_UF0023_Ledg_Desc},
    {0, F_UF0024_Tedg,	ERx("{"),	HM_VIG,			HM_FTASCII,
	    F_UF0025_Tedg_Desc},
    {0, F_UF0026_Redg,	ERx("]"),	HM_VFRBF,		HM_FTASCII,
	    F_UF0027_Redg_Desc},
    {0, F_UF0028_Redg,	ERx("]"),	HM_VIG,			HM_FTASCII,
	    F_UF0029_Redg_Desc},
    {0, F_UF002A_Bedg,	ERx("}"),	HM_VIG,			HM_FTASCII,
	    F_UF002B_Bedg_Desc},
    {12, F_UF002C_Lchr,	_Empty,	HM_ALL,			HM_FTASCII,
	    F_UF002D_Lchr_Desc},
    {13, F_UF002E_Rchr,	_Empty,	HM_ALL,			HM_FTASCII,
	    F_UF002F_Rchr_Desc},
    {14, F_UF0030_Dnln,	_Empty,	HM_ALL,			HM_FTASCII,
	    F_UF0031_Dnln_Desc},
    {15, F_UF0032_Upln,	_Empty,	HM_ALL,			HM_FTASCII,
	    F_UF0033_Upln_Desc},
    {0, F_UF0034_Gcur,	ERx("^"),	HM_VIG,			HM_FTASCII,
	    F_UF0035_Gcur_Desc},
    {0, F_UF0036_Lerr,	ERx("="),	HM_VIG,			HM_FTASCII,
	    F_UF0037_Lerr_Desc},
    {16, F_UF0038_Nxwd,	_Empty,	HM_EQ|HM_VFRBF|HM_VIG,  HM_FTASCII,
	    F_UF0039_Nxwd_Desc},
    {17, F_UF003A_Pvwd,	_Empty,	HM_EQ|HM_VFRBF|HM_VIG,  HM_FTASCII,
	    F_UF003B_Pvwd_Desc},
    {18, F_UF003C_Mode,	_Empty,	HM_EQ|HM_VFRBF|HM_VIG,  HM_FTASCII,
	    F_UF003D_Mode_Desc},
    {19, F_UF003E_Redr,	_Empty,	HM_ALL,			HM_FTASCII,
	    F_UF003F_Redr_Desc},
    {20, F_UF0040_Dchr,	_Empty,	HM_EQ|HM_VFRBF|HM_VIG,  HM_FTASCII,
	    F_UF0041_Dchr_Desc},
    {21, F_UF0042_Rubout, _Empty,	HM_EQ|HM_VFRBF|HM_VIG,  HM_FTASCII,
	    F_UF0043_Rubout_Desc},
    {22, F_UF0044_Edit,	_Empty,	HM_EQ,			HM_FTBOTH,
	    F_UF0045_Edit_Desc},
    {23, F_UF0046_Nrow,	_Empty,	HM_EQ,			HM_FTASCII,
	    F_UF0047_Nrow_Desc},
    {24, F_UF0048_Clear,_Empty,	HM_EQ|HM_VFRBF|HM_VIG,  HM_FTASCII,
	    F_UF0049_Clear_Desc},
    {25, F_UF004A_Clst,	_Empty,	HM_EQ|HM_VFRBF|HM_VIG,  HM_FTASCII,
	    F_UF004B_Clst_Desc},
    {26, F_UF004C_Dup,	_Empty,	HM_EQ,			HM_FTASCII,
	    F_UF004D_Dup_Desc},
    {27, F_UF004E_Pscr,	_Empty,	HM_ALL & ~HM_VIG,	HM_FTBOTH,
	    F_UF004F_Pscr_Desc},
    {28, F_UF0050_Nitm, _Empty,	HM_ALL,			HM_FTASCII,
	    F_UF0051_Nitm_Desc},
    {29, F_UF0052_Shell, _Empty,	HM_ALL,			HM_FTASCII,
	    F_UF0053_Shell_Desc},
    {-1, 0,		_Empty,	HM_NONE,	HM_FTASCII, 0}
};
/*
**  Maximum number of labels allocated in Keytab structure above.
**  Please increase this number if you add entries to Keytab above.
*/
# define	HM_MAXLABEL	30
# define	HM_SHELL	29
# define	HM_EDITOR	22
/*{
** Name:	FEhkeys - print help on control and function keys.
**
** Description:
**	Print help on the set of control keys, mapping menu items
**	and FRS functions in the process.  This is called through
**	the HELP/KEYS combination in the "help_frs" EQUEL
**	command.
**
**	Depending on the value for vfrviq, different keys are
**	available, and displayed.
**
** Inputs:
**	subj		a name to be used at the top of the help screen, or
**			printed as a message if the file is not found.
**	menu		The current menu structure to use in mapping.
**	frs_labels	Array of labels for the menu key mappings.
**	frskey_expls	Array of explanations for the FRS keys.
**	vfrfiq		special help processing flag from help.h.
**			Values are H_EQ, H_VFRBF, H_IQUEL, H_VIG or
**			H_QBFSRT (same as H_EQ for this)
**
** Outputs:
**	Returns:
**		VOID
**
** Side Effects:
**
** History:
**	01-sep-1985 (grant)	Written.
**	19-oct-1985 (peter)	Add differences for vifred/rbf and
**				support of printscreen.
**	13-jan-1986 (peter)	Changed last param to special case IQUEL.
**	30-jan-1986 (peter)	Changed after code review.
*/
VOID
FEhkeys(subj, menu, frs_labels, frskey_expls, vfrfiq)
char	*subj;
MENU	menu;
char	**frs_labels;
char	**frskey_expls;
i4	vfrfiq;
  {
    char subject[TITLELEN+1];
	i4	i;
    char menulabel[LABELWIDTH+1];
	char	explanation[EXPLWIDTH+1];
    char *expl;
    char klabel[HM_MAXLABEL][LABELWIDTH+1];
    i4 numrecs;
    i4 numrows;
    i4 currrec;
    char upmap[20];
    char downmap[20];
    char menu1map[20];
    char menu2map[20];
    char uplabel[50];
    char downlabel[50];
    char menu1label[50];
    char menu2label[50];
	i4	mask,FTmask;
	bool	separator_needed = FALSE;
	bool	match;
	_VOID_ STlcopy(subj, subject, (i4) TITLELEN);
	_VOID_ STtrmwhite(subject);
	if (!keysfadded)
	{
		if (IIUFgtfGetForm(IIUFlcfLocateForm(), _KeysForm) != OK)
		{
			/* 15-Feb-1995 (canor01) - not fatal */
			/*IIUGerr(E_UF0026_BadForm,UG_ERR_FATAL,1,_KeysForm);*/
			IIUGerr( E_UF0026_BadForm, UG_ERR_ERROR, 1, _KeysForm );
			return;
		}
		keysfadded = TRUE;
	}
/* # line 323 "fehkeys.qsc" */	/* putform */
    {
      if (IIfsetio(_KeysForm) != 0) {
        IIputfldio((char *)"title",(short *)0,1,32,0,subject);
      } /* IIfsetio */
    }
/* # line 324 "fehkeys.qsc" */	/* inittable */
    {
      if (IItbinit(_KeysForm,_KeysTable,(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 326 "fehkeys.qsc" */	/* host code */
	/* First load the menu items */
	if (menu != NULL)
	{
	    register i4	    mi;
	    register struct com_tab *mutab;
	    mutab = menu->mu_coms;
	    for ( mi = 0 ; mutab[mi].ct_name != NULL ; ++mi )
    {
      char *menuname;
      char *menuexpl;
		char	ct_name[101];				 
		char	*p;					 
	    /*  The following code must not be ifdef-ed  */
		STlcopy(mutab[mi].ct_name, ct_name, 100);	 
		if (p = STindex(ct_name, mu_guidivider, STlength(ct_name)))	 
		{						 
			*p = EOS;				 
			menuname = ct_name;			 
		}						 
		else						 
			menuname = mutab[mi].ct_name;
		if ((menuexpl = mutab[mi].description) == NULL)
		{
			menuexpl = _Empty;
		}
		if (frs_labels[mi] != NULL)
		   STcopy(frs_labels[mi], menulabel);
		else switch(mi+1)
		{
		  case 1:
/* # line 361 "fehkeys.qsc" */	/* inquire_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,3000);
        } /* IIiqset */
      }
/* # line 362 "fehkeys.qsc" */	/* host code */
		    break;
		  case 2:
/* # line 364 "fehkeys.qsc" */	/* inquire_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,3001);
        } /* IIiqset */
      }
/* # line 365 "fehkeys.qsc" */	/* host code */
		    break;
		  case 3:
/* # line 367 "fehkeys.qsc" */	/* inquire_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,3002);
        } /* IIiqset */
      }
/* # line 368 "fehkeys.qsc" */	/* host code */
		    break;
		  case 4:
/* # line 370 "fehkeys.qsc" */	/* inquire_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,3003);
        } /* IIiqset */
      }
/* # line 371 "fehkeys.qsc" */	/* host code */
		    break;
		  case 5:
/* # line 373 "fehkeys.qsc" */	/* inquire_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,3004);
        } /* IIiqset */
      }
/* # line 374 "fehkeys.qsc" */	/* host code */
		    break;
		  case 6:
/* # line 376 "fehkeys.qsc" */	/* inquire_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,3005);
        } /* IIiqset */
      }
/* # line 377 "fehkeys.qsc" */	/* host code */
		    break;
		  case 7:
/* # line 379 "fehkeys.qsc" */	/* inquire_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,3006);
        } /* IIiqset */
      }
/* # line 380 "fehkeys.qsc" */	/* host code */
		    break;
		  case 8:
/* # line 382 "fehkeys.qsc" */	/* inquire_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,3007);
        } /* IIiqset */
      }
/* # line 383 "fehkeys.qsc" */	/* host code */
		    break;
		  case 9:
/* # line 385 "fehkeys.qsc" */	/* inquire_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,3008);
        } /* IIiqset */
      }
/* # line 386 "fehkeys.qsc" */	/* host code */
		    break;
		  case 10:
/* # line 388 "fehkeys.qsc" */	/* inquire_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,3009);
        } /* IIiqset */
      }
/* # line 389 "fehkeys.qsc" */	/* host code */
		    break;
		  case 11:
/* # line 391 "fehkeys.qsc" */	/* inquire_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,3010);
        } /* IIiqset */
      }
/* # line 392 "fehkeys.qsc" */	/* host code */
		    break;
		  case 12:
/* # line 394 "fehkeys.qsc" */	/* inquire_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,3011);
        } /* IIiqset */
      }
/* # line 395 "fehkeys.qsc" */	/* host code */
		    break;
		  case 13:
/* # line 397 "fehkeys.qsc" */	/* inquire_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,3012);
        } /* IIiqset */
      }
/* # line 398 "fehkeys.qsc" */	/* host code */
		    break;
		  case 14:
/* # line 400 "fehkeys.qsc" */	/* inquire_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,3013);
        } /* IIiqset */
      }
/* # line 401 "fehkeys.qsc" */	/* host code */
		    break;
		  case 15:
/* # line 403 "fehkeys.qsc" */	/* inquire_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,3014);
        } /* IIiqset */
      }
/* # line 404 "fehkeys.qsc" */	/* host code */
		    break;
		  case 16:
/* # line 406 "fehkeys.qsc" */	/* inquire_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,3015);
        } /* IIiqset */
      }
/* # line 407 "fehkeys.qsc" */	/* host code */
		    break;
		  case 17:
/* # line 409 "fehkeys.qsc" */	/* inquire_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,3016);
        } /* IIiqset */
      }
/* # line 410 "fehkeys.qsc" */	/* host code */
		    break;
		  case 18:
/* # line 412 "fehkeys.qsc" */	/* inquire_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,3017);
        } /* IIiqset */
      }
/* # line 413 "fehkeys.qsc" */	/* host code */
		    break;
		  case 19:
/* # line 415 "fehkeys.qsc" */	/* inquire_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,3018);
        } /* IIiqset */
      }
/* # line 416 "fehkeys.qsc" */	/* host code */
		    break;
		  case 20:
/* # line 418 "fehkeys.qsc" */	/* inquire_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,3019);
        } /* IIiqset */
      }
/* # line 419 "fehkeys.qsc" */	/* host code */
		    break;
		  case 21:
/* # line 421 "fehkeys.qsc" */	/* inquire_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,3020);
        } /* IIiqset */
      }
/* # line 422 "fehkeys.qsc" */	/* host code */
		    break;
		  case 22:
/* # line 424 "fehkeys.qsc" */	/* inquire_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,3021);
        } /* IIiqset */
      }
/* # line 425 "fehkeys.qsc" */	/* host code */
		    break;
		  case 23:
/* # line 427 "fehkeys.qsc" */	/* inquire_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,3022);
        } /* IIiqset */
      }
/* # line 428 "fehkeys.qsc" */	/* host code */
		    break;
		  case 24:
/* # line 430 "fehkeys.qsc" */	/* inquire_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,3023);
        } /* IIiqset */
      }
/* # line 431 "fehkeys.qsc" */	/* host code */
		    break;
		  case 25:
/* # line 433 "fehkeys.qsc" */	/* inquire_frs */
      {
        if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
          IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,3024);
        } /* IIiqset */
      }
/* # line 434 "fehkeys.qsc" */	/* host code */
		    break;
		} /* end else-switch */
/* # line 437 "fehkeys.qsc" */	/* loadtable */
      {
        if (IItbact(_KeysForm,_KeysTable,1) != 0) {
          IItcoputio(_Command,(short *)0,1,32,0,menuname);
          IItcoputio(_Key,(short *)0,1,32,0,menulabel);
          IItcoputio(_Expl,(short *)0,1,32,0,menuexpl);
          IITBceColEnd();
        } /* IItbact */
      }
    }
/* # line 443 "fehkeys.qsc" */	/* loadtable */
    {
      if (IItbact(_KeysForm,_KeysTable,1) != 0) {
        IItcoputio(_Command,(short *)0,1,32,0,_Empty);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 444 "fehkeys.qsc" */	/* host code */
	}
	/* Then load the frskeys */
	for ( i = Fsi_KEY_PF_MAX ; --i >= 0 ; )
	{
	    if ((expl = frskey_expls[i]) != NULL)
	    {
		switch(i+1)
		{
		  case 1:
/* # line 454 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4000);
      } /* IIiqset */
    }
/* # line 455 "fehkeys.qsc" */	/* host code */
		    break;
		  case 2:
/* # line 457 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4001);
      } /* IIiqset */
    }
/* # line 458 "fehkeys.qsc" */	/* host code */
		    break;
		  case 3:
/* # line 460 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4002);
      } /* IIiqset */
    }
/* # line 461 "fehkeys.qsc" */	/* host code */
		    break;
		  case 4:
/* # line 463 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4003);
      } /* IIiqset */
    }
/* # line 464 "fehkeys.qsc" */	/* host code */
		    break;
		  case 5:
/* # line 466 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4004);
      } /* IIiqset */
    }
/* # line 467 "fehkeys.qsc" */	/* host code */
		    break;
		  case 6:
/* # line 469 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4005);
      } /* IIiqset */
    }
/* # line 470 "fehkeys.qsc" */	/* host code */
		    break;
		  case 7:
/* # line 472 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4006);
      } /* IIiqset */
    }
/* # line 473 "fehkeys.qsc" */	/* host code */
		    break;
		  case 8:
/* # line 475 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4007);
      } /* IIiqset */
    }
/* # line 476 "fehkeys.qsc" */	/* host code */
		    break;
		  case 9:
/* # line 478 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4008);
      } /* IIiqset */
    }
/* # line 479 "fehkeys.qsc" */	/* host code */
		    break;
		  case 10:
/* # line 481 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4009);
      } /* IIiqset */
    }
/* # line 482 "fehkeys.qsc" */	/* host code */
		    break;
		  case 11:
/* # line 484 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4010);
      } /* IIiqset */
    }
/* # line 485 "fehkeys.qsc" */	/* host code */
		    break;
		  case 12:
/* # line 487 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4011);
      } /* IIiqset */
    }
/* # line 488 "fehkeys.qsc" */	/* host code */
		    break;
		  case 13:
/* # line 490 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4012);
      } /* IIiqset */
    }
/* # line 491 "fehkeys.qsc" */	/* host code */
		    break;
		  case 14:
/* # line 493 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4013);
      } /* IIiqset */
    }
/* # line 494 "fehkeys.qsc" */	/* host code */
		    break;
		  case 15:
/* # line 496 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4014);
      } /* IIiqset */
    }
/* # line 497 "fehkeys.qsc" */	/* host code */
		    break;
		  case 16:
/* # line 499 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4015);
      } /* IIiqset */
    }
/* # line 500 "fehkeys.qsc" */	/* host code */
		    break;
		  case 17:
/* # line 502 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4016);
      } /* IIiqset */
    }
/* # line 503 "fehkeys.qsc" */	/* host code */
		    break;
		  case 18:
/* # line 505 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4017);
      } /* IIiqset */
    }
/* # line 506 "fehkeys.qsc" */	/* host code */
		    break;
		  case 19:
/* # line 508 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4018);
      } /* IIiqset */
    }
/* # line 509 "fehkeys.qsc" */	/* host code */
		    break;
		  case 20:
/* # line 511 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4019);
      } /* IIiqset */
    }
/* # line 512 "fehkeys.qsc" */	/* host code */
		    break;
		  case 21:
/* # line 514 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4020);
      } /* IIiqset */
    }
/* # line 515 "fehkeys.qsc" */	/* host code */
		    break;
		  case 22:
/* # line 517 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4021);
      } /* IIiqset */
    }
/* # line 518 "fehkeys.qsc" */	/* host code */
		    break;
		  case 23:
/* # line 520 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4022);
      } /* IIiqset */
    }
/* # line 521 "fehkeys.qsc" */	/* host code */
		    break;
		  case 24:
/* # line 523 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4023);
      } /* IIiqset */
    }
/* # line 524 "fehkeys.qsc" */	/* host code */
		    break;
		  case 25:
/* # line 526 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4024);
      } /* IIiqset */
    }
/* # line 527 "fehkeys.qsc" */	/* host code */
		    break;
		  case 26:
/* # line 529 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4025);
      } /* IIiqset */
    }
/* # line 530 "fehkeys.qsc" */	/* host code */
		    break;
		  case 27:
/* # line 532 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4026);
      } /* IIiqset */
    }
/* # line 533 "fehkeys.qsc" */	/* host code */
		    break;
		  case 28:
/* # line 535 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4027);
      } /* IIiqset */
    }
/* # line 536 "fehkeys.qsc" */	/* host code */
		    break;
		  case 29:
/* # line 538 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4028);
      } /* IIiqset */
    }
/* # line 539 "fehkeys.qsc" */	/* host code */
		    break;
		  case 30:
/* # line 541 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4029);
      } /* IIiqset */
    }
/* # line 542 "fehkeys.qsc" */	/* host code */
		    break;
		  case 31:
/* # line 544 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4030);
      } /* IIiqset */
    }
/* # line 545 "fehkeys.qsc" */	/* host code */
		    break;
		  case 32:
/* # line 547 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4031);
      } /* IIiqset */
    }
/* # line 548 "fehkeys.qsc" */	/* host code */
		    break;
		  case 33:
/* # line 550 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4032);
      } /* IIiqset */
    }
/* # line 551 "fehkeys.qsc" */	/* host code */
		    break;
		  case 34:
/* # line 553 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4033);
      } /* IIiqset */
    }
/* # line 554 "fehkeys.qsc" */	/* host code */
		    break;
		  case 35:
/* # line 556 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4034);
      } /* IIiqset */
    }
/* # line 557 "fehkeys.qsc" */	/* host code */
		    break;
		  case 36:
/* # line 559 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4035);
      } /* IIiqset */
    }
/* # line 560 "fehkeys.qsc" */	/* host code */
		    break;
		  case 37:
/* # line 562 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4036);
      } /* IIiqset */
    }
/* # line 563 "fehkeys.qsc" */	/* host code */
		    break;
		  case 38:
/* # line 565 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4037);
      } /* IIiqset */
    }
/* # line 566 "fehkeys.qsc" */	/* host code */
		    break;
		  case 39:
/* # line 568 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4038);
      } /* IIiqset */
    }
/* # line 569 "fehkeys.qsc" */	/* host code */
		    break;
		  case 40:
/* # line 571 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,menulabel,9,(char *)0,4039);
      } /* IIiqset */
    }
/* # line 572 "fehkeys.qsc" */	/* host code */
		    break;
		} /* end else-switch */
		if ( STtrmwhite(menulabel) > 0 )
		{
		    /*
		    ** Check for overlap between FRSkey definitions and
		    ** standard menuitems; eliminate those that share the
		    ** key labels.
		    */
		    match = FALSE;
		    if (menu != NULL)
		    {
			register i4	    mi;
			register struct com_tab *mutab;
			mutab = menu->mu_coms;
			for ( mi = 0 ; mutab[mi].ct_name != NULL ; ++mi )
			{
			    if (frs_labels[mi] != NULL)
			    {
				if (STcompare(frs_labels[mi],menulabel) == 0)
				{
				    match = TRUE;
				    break;
				}
			    }
			}
		    }
		    if (!match)
		    {
			/*
			** 'Activate frskey i' exists for this display loop
			** with a non-blank label.
			*/
			separator_needed = TRUE;
/* # line 613 "fehkeys.qsc" */	/* loadtable */
    {
      if (IItbact(_KeysForm,_KeysTable,1) != 0) {
        IItcoputio(_Command,(short *)0,1,32,0,_Empty);
        IItcoputio(_Key,(short *)0,1,32,0,menulabel);
        IItcoputio(_Expl,(short *)0,1,32,0,expl);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 617 "fehkeys.qsc" */	/* host code */
		    }
		}
	    }
	}
	if (separator_needed)
	{
/* # line 623 "fehkeys.qsc" */	/* loadtable */
    {
      if (IItbact(_KeysForm,_KeysTable,1) != 0) {
        IItcoputio(_Command,(short *)0,1,32,0,_Empty);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 624 "fehkeys.qsc" */	/* host code */
	}
	/* Then load the commands */
	mask = HM_EQ;
	for (i=0; i < (sizeof(Masktab)/sizeof(VAL2HM)); i++)
	{	/* Find appropriate mask */
		if (Masktab[i].val_vfrfiq == vfrfiq)
		{
			mask = Masktab[i].val_mask;
			break;
		}
	}
/* # line 637 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,klabel[1],9,(char *)0,2016);
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,klabel[2],9,(char *)0,2017);
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,klabel[3],9,(char *)0,2018);
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,klabel[4],9,(char *)0,2000);
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,klabel[5],9,(char *)0,2001);
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,klabel[6],9,(char *)0,2019);
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,klabel[7],9,(char *)0,2020);
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,klabel[12],9,(char *)0,2009);
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,klabel[13],9,(char *)0,2010);
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,klabel[14],9,(char *)0,2011);
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,klabel[15],9,(char *)0,2012);
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,klabel[16],9,(char *)0,2002);
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,klabel[17],9,(char *)0,2003);
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,klabel[18],9,(char *)0,2004);
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,klabel[19],9,(char *)0,2005);
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,klabel[20],9,(char *)0,2006);
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,klabel[21],9,(char *)0,2007);
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,klabel[22],9,(char *)0,2008);
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,klabel[23],9,(char *)0,2013);
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,klabel[24],9,(char *)0,2014);
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,klabel[25],9,(char *)0,2015);
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,klabel[26],9,(char *)0,2021);
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,klabel[27],9,(char *)0,2022);
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,klabel[28],9,(char *)0,2029);
        IIiqfsio((short *)0,1,32,LABELWIDTH+1-1,klabel[29],9,(char *)0,2028);
      } /* IIiqset */
    }
/* # line 665 "fehkeys.qsc" */	/* host code */
	/*
	**   Only display commands valid for this FT mode
	*/
#ifdef FT3270
	FTmask = HM_FT3270;
#else
	FTmask = HM_FTASCII;
#endif
	for (i=0; Keytab[i].key_labseq>=0; i++)
	{	/* Keytab is ordered */
		if ( (Keytab[i].key_FTmode & FTmask) &&
		     (Keytab[i].key_mask & mask) )
		{	/* this row should be entered */
			STcopy(Keytab[i].key_label, menulabel);
			if (Keytab[i].key_labseq >0)
			{	/* Move label from inquire above */
				STcopy(klabel[Keytab[i].key_labseq], menulabel);
			}
			if (((Keytab[i].key_labseq == HM_SHELL)
				&& !(IIfrscb->frs_globs->enabled & SHELL_FN))
			    || ((Keytab[i].key_labseq == HM_EDITOR)
				&& !(IIfrscb->frs_globs->enabled & EDITOR_FN)))
			{
			    STcopy(ERget(Keytab[i].key_desc), explanation);
			    expl = STcat(explanation, ERget(F_UF0054_disabled));
			}
			else
			{
			    expl = ERget(Keytab[i].key_desc);
			}
/* # line 697 "fehkeys.qsc" */	/* loadtable */
    {
      if (IItbact(_KeysForm,_KeysTable,1) != 0) {
        IItcoputio(_Command,(short *)0,1,32,0,ERget(Keytab[i].key_name));
        IItcoputio(_Key,(short *)0,1,32,0,menulabel);
        IItcoputio(_Expl,(short *)0,1,32,0,expl);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 702 "fehkeys.qsc" */	/* host code */
		}
	}
/* # line 705 "fehkeys.qsc" */	/* inquire_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIiqfsio((short *)0,1,32,19,upmap,8,(char *)0,2017);
        IIiqfsio((short *)0,1,32,19,downmap,8,(char *)0,2018);
        IIiqfsio((short *)0,1,32,19,menu1map,8,(char *)0,3000);
        IIiqfsio((short *)0,1,32,19,menu2map,8,(char *)0,3001);
        IIiqfsio((short *)0,1,32,49,uplabel,9,(char *)0,2017);
        IIiqfsio((short *)0,1,32,49,downlabel,9,(char *)0,2018);
        IIiqfsio((short *)0,1,32,49,menu1label,9,(char *)0,3000);
        IIiqfsio((short *)0,1,32,49,menu2label,9,(char *)0,3001);
      } /* IIiqset */
    }
/* # line 713 "fehkeys.qsc" */	/* host code */
	if (upmap[0] != EOS)
	{
/* # line 715 "fehkeys.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,3000,(short *)0,1,32,0,upmap);
        IIstfsio(9,(char *)0,3000,(short *)0,1,32,0,uplabel);
      } /* IIiqset */
    }
/* # line 716 "fehkeys.qsc" */	/* host code */
	}
	if (downmap[0] != EOS)
	{
/* # line 719 "fehkeys.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,3001,(short *)0,1,32,0,downmap);
        IIstfsio(9,(char *)0,3001,(short *)0,1,32,0,downlabel);
      } /* IIiqset */
    }
/* # line 720 "fehkeys.qsc" */	/* host code */
	}
/* # line 722 "fehkeys.qsc" */	/* display */
    {
      if (IIdispfrm(_KeysForm,(char *)"r") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 725 "fehkeys.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,_KeysForm,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&numrecs,32,_KeysTable,0);
                IIiqfsio((short *)0,1,30,4,&numrows,30,_KeysTable,0);
              } /* IIiqset */
            }
/* # line 728 "fehkeys.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_KeysForm,_KeysTable,1) != 0) {
                IItcogetio((short *)0,1,30,4,&currrec,(char *)"_RECORD");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 729 "fehkeys.qsc" */	/* host code */
		currrec = min(currrec+2*(numrows-1), numrecs);
/* # line 730 "fehkeys.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_KeysForm,_KeysTable,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,currrec) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 734 "fehkeys.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,_KeysForm,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&numrows,30,_KeysTable,0);
              } /* IIiqset */
            }
/* # line 736 "fehkeys.qsc" */	/* getrow */
            {
              if (IItbsetio(2,_KeysForm,_KeysTable,1) != 0) {
                IItcogetio((short *)0,1,30,4,&currrec,(char *)"_RECORD");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 737 "fehkeys.qsc" */	/* host code */
		currrec = max(1, currrec-(numrows-1));
/* # line 738 "fehkeys.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_KeysForm,_KeysTable,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,currrec) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 742 "fehkeys.qsc" */	/* host code */
		/* Implement the Find command. */
		FEtabfnd(_KeysForm, _KeysTable);
          }
        } else if (IIretval() == 4) {
          {
/* # line 747 "fehkeys.qsc" */	/* host code */
		/* Implement the Top command. */
/* # line 748 "fehkeys.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_KeysForm,_KeysTable,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 5) {
          {
/* # line 752 "fehkeys.qsc" */	/* host code */
		/* Implement the Bottom command. */
/* # line 753 "fehkeys.qsc" */	/* scroll */
            {
              if (IItbsetio(1,_KeysForm,_KeysTable,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 6) {
          {
/* # line 757 "fehkeys.qsc" */	/* host code */
		if (upmap[0] != EOS)
		{
/* # line 759 "fehkeys.qsc" */	/* set_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(8,(char *)0,2017,(short *)0,1,32,0,upmap);
                IIstfsio(9,(char *)0,2017,(short *)0,1,32,0,uplabel);
              } /* IIiqset */
            }
/* # line 761 "fehkeys.qsc" */	/* host code */
		}
		if (menu1map[0] != EOS)
		{
/* # line 764 "fehkeys.qsc" */	/* set_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(8,(char *)0,3000,(short *)0,1,32,0,menu1map);
                IIstfsio(9,(char *)0,3000,(short *)0,1,32,0,menu1label);
              } /* IIiqset */
            }
/* # line 766 "fehkeys.qsc" */	/* host code */
		}
		if (downmap[0] != EOS)
		{
/* # line 769 "fehkeys.qsc" */	/* set_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(8,(char *)0,2018,(short *)0,1,32,0,downmap);
                IIstfsio(9,(char *)0,2018,(short *)0,1,32,0,downlabel);
              } /* IIiqset */
            }
/* # line 771 "fehkeys.qsc" */	/* host code */
		}
		if (menu2map[0] != EOS)
		{
/* # line 774 "fehkeys.qsc" */	/* set_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(8,(char *)0,3001,(short *)0,1,32,0,menu2map);
                IIstfsio(9,(char *)0,3001,(short *)0,1,32,0,menu2label);
              } /* IIiqset */
            }
/* # line 776 "fehkeys.qsc" */	/* host code */
		}
		FEhhelp(HLPKEYS_HLP, ERget(S_UF0048_HELP_Command_Key));
		if (upmap[0] != EOS)
		{
/* # line 782 "fehkeys.qsc" */	/* set_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(8,(char *)0,3000,(short *)0,1,32,0,upmap);
                IIstfsio(9,(char *)0,3000,(short *)0,1,32,0,uplabel);
              } /* IIiqset */
            }
/* # line 784 "fehkeys.qsc" */	/* host code */
		}
		if (downmap[0] != EOS)
		{
/* # line 787 "fehkeys.qsc" */	/* set_frs */
            {
              if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(8,(char *)0,3001,(short *)0,1,32,0,downmap);
                IIstfsio(9,(char *)0,3001,(short *)0,1,32,0,downlabel);
              } /* IIiqset */
            }
/* # line 789 "fehkeys.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 7) {
          {
/* # line 793 "fehkeys.qsc" */	/* breakdisplay */
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
      if (IInmuact(ERget(FE_NextPage),(char *)0,2,2,1) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_PrevPage),(char *)0,2,2,2) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,3) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,4) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),(char *)0,2,2,6) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,2,2,6) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),(char *)0,2,2,7) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,7) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 797 "fehkeys.qsc" */	/* host code */
	if (upmap[0] != EOS)
	{
/* # line 799 "fehkeys.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,2017,(short *)0,1,32,0,upmap);
        IIstfsio(9,(char *)0,2017,(short *)0,1,32,0,uplabel);
      } /* IIiqset */
    }
/* # line 801 "fehkeys.qsc" */	/* host code */
	}
	if (menu1map[0] != EOS)
	{
/* # line 804 "fehkeys.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,3000,(short *)0,1,32,0,menu1map);
        IIstfsio(9,(char *)0,3000,(short *)0,1,32,0,menu1label);
      } /* IIiqset */
    }
/* # line 806 "fehkeys.qsc" */	/* host code */
	}
	if (downmap[0] != EOS)
	{
/* # line 809 "fehkeys.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,2018,(short *)0,1,32,0,downmap);
        IIstfsio(9,(char *)0,2018,(short *)0,1,32,0,downlabel);
      } /* IIiqset */
    }
/* # line 811 "fehkeys.qsc" */	/* host code */
	}
	if (menu2map[0] != EOS)
	{
/* # line 814 "fehkeys.qsc" */	/* set_frs */
    {
      if (IIiqset(0,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(8,(char *)0,3001,(short *)0,1,32,0,menu2map);
        IIstfsio(9,(char *)0,3001,(short *)0,1,32,0,menu2label);
      } /* IIiqset */
    }
/* # line 816 "fehkeys.qsc" */	/* host code */
	}
/* # line 818 "fehkeys.qsc" */	/* inittable */
    {
      if (IItbinit(_KeysForm,_KeysTable,(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
  }
