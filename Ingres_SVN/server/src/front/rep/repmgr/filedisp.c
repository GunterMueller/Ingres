# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include <compat.h>
# include <cm.h>
# include <st.h>
# include <lo.h>
# include <er.h>
# include <si.h>
# include <gl.h>
# include <iicommon.h>
# include <fe.h>
# include <erfe.h>
# include <uf.h>
# include <ug.h>
# include "errm.h"
/**
** Name:	filedisp.qsc - file display
**
** Description:
**	Defines
**		file_display	- file display
**
** History:
**	16-dec-96 (joea)
**		Created based on cdisplay.sc and loadtbl.sc in replicator
**		library.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
# define BUF_SZ		256
static bool	form_initd;
static READONLY
  char *form_name = ERx("rmdisplayer"), *tf_name = ERx("tblfield"), *fline = ERx("line")
  ;
/*{
** Name:	file_display	- file display
**
** Description:
**	Display a report file.
**
** Inputs:
**	file_name	- name of file to display
**
** Outputs:
**	none
**
** Returns:
**	none
*/
void
file_display(
char	*file_name)
# if 0
  char *file_name;
# endif
  {
    char buff[SI_MAX_TXT_REC+1];
	FILE	*filptr;
	char	open_file[MAX_LOC+1];
	LOCATION	loc;
	bool	found;
	if (!form_initd)
	{
		if (IIUFgtfGetForm(IIUFlcfLocateForm(), form_name) != OK)
		{
			IIUGerr(E_RM0006_Get_form_failed, UG_ERR_ERROR, 1,
				form_name);
			return;
		}
		form_initd = TRUE;
	}
/* # line 85 "filedisp.qsc" */	/* inittable */
    {
      if (IItbinit(form_name,tf_name,(char *)"r") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 86 "filedisp.qsc" */	/* host code */
	STcopy(file_name, open_file);
	LOfroms(PATH & FILENAME, open_file, &loc);
	if (SIfopen(&loc, ERx("r"), SI_TXT, SI_MAX_TXT_REC, &filptr) != OK)
	{
		IIUGerr(E_RM0074_Error_open_file_read, UG_ERR_ERROR, 1,
			file_name);
		return;
	}
	while (SIgetrec(buff, (i4)sizeof(buff), filptr) != ENDFILE)
/* # line 96 "filedisp.qsc" */	/* loadtable */
    {
      if (IItbact(form_name,tf_name,1) != 0) {
        IItcoputio(fline,(short *)0,1,32,0,buff);
        IITBceColEnd();
      } /* IItbact */
    }
/* # line 98 "filedisp.qsc" */	/* host code */
	SIclose(filptr);
/* # line 100 "filedisp.qsc" */	/* display */
    {
      if (IIdispfrm(form_name,(char *)"r") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      IIputfldio((char *)"filename",(short *)0,1,32,0,file_name);
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 105 "filedisp.qsc" */	/* host code */
		FEtabfnd(form_name, tf_name);
          }
        } else if (IIretval() == 2) {
          {
/* # line 110 "filedisp.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 3) {
          {
/* # line 115 "filedisp.qsc" */	/* scroll */
            {
              if (IItbsetio(1,form_name,tf_name,-3) != 0) {
                IItbsmode((char *)"to");
                if (IItscroll(0,-1) != 0) {
                } /* IItscroll */
              } /* IItbsetio */
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 121 "filedisp.qsc" */	/* breakdisplay */
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
      if (IInmuact(ERget(FE_Find),ERget(F_FE0104_ExplFind),2,2,1) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(7,(char *)0,2,2,1) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,2) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,3) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),ERget(F_FE0102_ExplEnd),2,2,4) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,(char *)0,2,2,4) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
  }
