# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<lo.h>
#include	<st.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
# include <abclass.h>
/*
#include	<abclass.h>
*/
#include	<fdesc.h>
#include	<abfrts.h>
#include	<abfcnsts.h>
#include	<abfcompl.h>
#include	"abfgolnk.h"
#include	"ablink.h"
#include	"erab.h"
/**
** Name:	abfimage.qsc -	ABF Link Image Frame.
**
** Description:
**  Contains the frame for the link image frame of ABF and an
**  auxilliary routine fetching an application's default role.
**  Defines:
**
**	IIABimage()	ABF link image frame.
**  	IIABgrGtRole()  Fetches default role for an application.
**
** History:
**	Revision 6.2  89/02  wong
**	Added application object parameter and free of application.
**
**	Revision 6.0  88/01  wong
**	Created from EQUEL source, "abimgfrm.qc".
**  20/02/95  cwaldman (x-int angusm)
**  Created IIABgrGtRole().
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
FUNC_EXTERN char	*FEtsalloc();
/*{
** Name:	IIABimage() -	ABF Link Image Frame.
**
** Description:
**	Allow a user to build an executable image for an application.
**	The resulting executable is a stand alone program that the user
**	can run outside of ABF.
**
** Input:
**	app	{APPL *}  The application object to be linked; may not
**				be instantiated (i.e., ID is OC_UNDEFINED.)
**	appname	{char *} The application to be linked.
**	id	{OOID}  The application object ID.
**
** History:
**	01/88 (jhw) -- Modified from EQUEL source for "abimgfrm.qc".
**	11/89 (wolf) - Accept CMS format file ID "fn ft" and convert to "fn.ft".
**	11/89 (jhw) -- Modified to pass error display function to
**			'iiabImgLink()'.
**	10/90 (Mike S) -- Removed previous change.  Error display logic is now
**			in abapplnk.qsc
**	11/90 (Mike S) -- Ignore roleid if Knowledge Management Extensions
**			are of.
**	19-sep-91 (leighb) Added ifdef so that when INTERPIMG is defined,
**			Interpreted Images are the default.
**	08-jun-92 (leighb) DeskTop Porting Change: 
**			Changed 'INTERPIMG' to 'INTERPIMG_DEF' as per emerson.
**	11-jan-93 (fraser)
**			Restored 19-sep-91 history comment.  (Now, if 
**			INTERPIMG_DEF is defined, interpreted images are
**			the default.)
**      17-jun-97 (dacri01)
**                      Initialized *rolename to EOS in func IIABgrGtRole().
**                      So now it returns NULL or a valid rolename.
**                      Bug#83125.
**      03-nov-97 (hweho01)
**                      Fix the memory access violation error in   
**                      IIABgrGtRole() routine by setting char. pointer
**                      rol to a local static buffer space.
**                      
*/
GLOBALREF 	char *IIabExename;
GLOBALREF	char *IIabRoleID;
GLOBALREF	bool IIabKME;
GLOBALREF	i4  IIOIimageBld;		 
#define _ImageExpl	F_AB0011_Image_expl
#define _BatchImgExpl	F_AB0013_Bimg_expl
#define _HelpExpl	F_FE0100_ExplFrameHelp
#define _EndExpl	F_FE0102_ExplEnd
static READONLY
  char _ImgForm[]= ERx("abfimage");
VOID
IIABimage(app, appname, caller)
  APPL *app;
  char *appname;
i4	caller;
{
	STATUS	stat;
  char executable[MAX_LOC+1];
  char roleid[FE_MAXNAME+1];
	char 	password[FE_MAXNAME+1];
	char	fullrole[2*FE_MAXNAME+2];
	iiabifiImgFrmInit(_ImgForm);
	/* if we are called from the application flow diagram.. then
	   we need not read the application components.. they are 
	   already up-to-date */
	stat = 	(caller != ABUT_AFD)
		? IIABappFetch(app, appname, OC_UNDEFINED, TRUE) 
		: OK;
	/* if we have bad status or there is no source directory.. return */
	if (stat != OK || !IIABchkSrcDir(app->source, FALSE))
	{
		return;
	}
/* # line 137 "abimage.qsc" */	/* display */
  {
    if (IIdispfrm(_ImgForm,(char *)"f") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
    if (IIfsetio((char *)0) == 0) goto IIfdE1;
    {
      char titlebuf[80];
/* # line 143 "abimage.qsc" */	/* host code */
		IIUGfmt(titlebuf, sizeof(titlebuf), ERget(F_AB00FF_BuildTitle), 
			1, IIabExename);
		if ( app->executable == NULL || *app->executable == EOS )
		{
			_VOID_ STprintf(executable, "%s%s", appname, ABIMGEXT);
		}
		else
		{
			STcopy(app->executable, executable);
		}
/* # line 153 "abimage.qsc" */	/* putform */
      {
        if (IIfsetio(_ImgForm) != 0) {
          IIputfldio((char *)"file",(short *)0,1,32,0,executable);
          IIputfldio((char *)"title",(short *)0,1,32,0,titlebuf);
        } /* IIfsetio */
      }
/* # line 154 "abimage.qsc" */	/* host code */
		if (IIabKME)
		{
/* # line 156 "abimage.qsc" */	/* putform */
      {
        if (IIfsetio(_ImgForm) != 0) {
          IIputfldio((char *)"role",(short *)0,1,32,0,app->roleid);
        } /* IIfsetio */
      }
/* # line 157 "abimage.qsc" */	/* host code */
		}
		else
		{
/* # line 160 "abimage.qsc" */	/* set_frs */
      {
        if (IIiqset(2,0,_ImgForm,(char *)0, (char *)0) != 0) {
          IIstfsio(54,(char *)"role",0,(short *)0,1,30,sizeof(1),
          (void *)IILQint(1));
        } /* IIiqset */
      }
/* # line 161 "abimage.qsc" */	/* clear */
      {
        IIfldclear((char *)"role");
      }
/* # line 162 "abimage.qsc" */	/* host code */
		}
    }
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 167 "abimage.qsc" */	/* host code */
		ABLNKARGS link_args;
/* # line 169 "abimage.qsc" */	/* getform */
          {
            if (IIfsetio(_ImgForm) != 0) {
              IIgetfldio((short *)0,1,32,MAX_LOC+1-1,executable,(char *)
"file");
              IIgetfldio((short *)0,1,32,FE_MAXNAME+1-1,roleid,(char *)
"role");
            } /* IIfsetio */
          }
/* # line 170 "abimage.qsc" */	/* host code */
#ifdef CMS
		TOwscnme(executable);	/* accept "fn ft"; convert to "fn.ft" */
#endif
		if (*roleid == EOS)
		{
				*password = EOS;
				IIabRoleID = ERx("");
		}
		else
		{
			bool newrole = !STequal(roleid, app->roleid);
			char *prev_pwd = newrole ? NULL : app->password;
			if (IIABgrpGetRolePwd(roleid, prev_pwd, password) != OK)
/* # line 184 "abimage.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 185 "abimage.qsc" */	/* host code */
			if (!newrole)
			{
				if (app->password == NULL ||
				    !STequal(app->password, password))
				{
					app->password = FEtsalloc(app->data.tag,
							  	  password);
				}
			}
			IIabRoleID = STprintf(fullrole, ERx("-R%s/%s"), 
					      roleid, password);
		}
# ifdef	INTERPIMG_DEF					  
		IIOIimageBld = 1;		/* Build Interpreted Image */
		_VOID_ IIABilnkImage(appname, OC_UNDEFINED, 
						executable, ABF_OBJECTCODE);
		IIOIimageBld = 0;		/* Reset Image-type flag */
# else							 
		link_args.link_type = ABLNK_IMAGE;
		link_args.app = app;
		link_args.executable = executable;
		link_args.options = ABF_OBJECTCODE;
		_VOID_ IIABlkaLinKApplication(&link_args);
# endif	/* INTERPIMG_DEF */				 
/* # line 210 "abimage.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 2) {
        {
/* # line 224 "abimage.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 3) {
        {
/* # line 230 "abimage.qsc" */	/* host code */
		FEhelp(ERx("abfimage.hlp"), ERget(S_AB0031_Building_Image));
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
    if (IInmuact(ERget(FE_OK),ERget(_ImageExpl),2,2,1) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,1) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Cancel),ERget(F_FE0103_ExplCancel),0,2,2) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,0,2,2) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Help),ERget(_HelpExpl),0,2,3) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,0,2,3) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 233 "abimage.qsc" */	/* host code */
	/* if the caller is the application catalog.. then we can
	   free the structures.. all other cases expect the structures
	   to be in memory */
	if (caller == ABUT_APPCAT)
	{
		IIAMappFree(app);
	}
}
/*
** Cache initialization of this form.
*/
static bool _FormInit = FALSE;
iiabifiImgFrmInit(fname)
char *fname;
{
	if (!_FormInit)
	{
		_FormInit = TRUE;
		IIARiaForm(_ImgForm);
	}
}
char *
IIABgrGtRole (app)
char *app;
{
   char *rol;
   static char buffer[FE_MAXNAME+1];
  char appname[FE_MAXNAME+1];
  char rolename[FE_MAXNAME+1];
   _VOID_ STlcopy(app, appname, sizeof(appname)-1);
   CVlower(appname);
   *buffer = EOS;
   rol = buffer;
   *rolename = EOS;
/* # line 314 "abimage.sc" */	/* select */
  {
    IIsqInit((char *)0);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select a.abf_arg5 from ii_abfobjects a, ii_objects o where o.object_n\
ame=");
    IIputdomio((short *)0,1,32,0,appname);
    IIwritio(0,(short *)0,1,32,0,(char *)"and o.object_class=");
    IIputdomio((short *)0,1,30,sizeof(OC_APPL),(void *)IILQint(OC_APPL));
    IIwritio(0,(short *)0,1,32,0,(char *)"and o.object_id=a.object_id");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,FE_MAXNAME+1-1,rolename);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 319 "abimage.sc" */	/* host code */
   STcopy(rolename, rol);
   return (char *)rol;
}
