# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/* 
**	Copyright (c) 2004 Ingres Corporation  
**
**	History:
**	   31-aug-93 (kchin)
**		Changed type of nat_frame_ptr to PTR in IIVQvqsVqShow(),
**		since it holds pointer.  Also changed the cast of 
**		vq to PTR when assigns it to nat_frame_ptr.  Cast to i4  
**		will result in truncation of 64-bit address.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
*/
#include	<compat.h> 
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h> 
#include 	<er.h>
#include 	<st.h>
#include 	<adf.h>
#include 	<ft.h>
#include 	<fmt.h>
#include 	<frame.h>
#include 	<uf.h>
#include 	<ug.h>
#include 	<cm.h>
#include 	<ooclass.h>
#include 	<abclass.h>
# include <metafrm.h>
/*
#include	<metafrm.h>
  ..so ming knows about the dependancy 
*/
#include	"tabinfo.h"
#include	"ervq.h"
#include	"vqloc.h"
/**
** Name:	vqshow -	visual query display loop
**
** Description:
**	Process the display loop for simple viewing of the visual query.
**
**	This file defines:
**		IIVQvqsVqShow - run the vq display loop
**
** History:
**	12/27/89  (tom) - created
**/
/* # define's */
/* GLOBALDEF's */
/* extern's */
  GLOBALREF char *IIVQform;
GLOBALREF char *IIVQtfname_fmt; /* STprintf string to format name */
GLOBALREF char IIVQinc_col[4];
GLOBALREF char *IIabExename;
FUNC_EXTERN FRAME *IIVQvmfVqMakeFrame(); /* vq dynamic frame creation */
FUNC_EXTERN VOID IIdelfrm();
FUNC_EXTERN i4  IIVQcfCurFld();
FUNC_EXTERN i4  IIVQdcvDisplayCompView();
FUNC_EXTERN VOID IIVQfmFormMode();
/* static's */
/*{
** Name:	IIVQvqsVqShow	- show Vq
**
** Description:
**	This display loop allows the user to see the visual query
**	No editing, and only limited operations are allowed.
**
** Inputs:
**	METAFRAME *mf;	- metaframe to be edited
**
** Outputs:
**	Returns:
**	Exceptions:
**
** Side Effects:
**
** History:
**	12/27/89  (tom) - created
*/
VOID
IIVQvqsVqShow(mf)
METAFRAME *mf;
  {
	FRAME *vq;
    PTR nat_frame_ptr;
	register i4  i;
    char fname[20];
	/* dynamically create the vq frame */
	vq = IIVQvmfVqMakeFrame(mf, FALSE, 0);
	/* add the form which was created */
	nat_frame_ptr = (PTR) vq;
/* # line 112 "vqshow.qsc" */	/* addform */
    {
      IIaddform(nat_frame_ptr);
    }
/* # line 114 "vqshow.qsc" */	/* display */
    {
      if (IIdispfrm(IIVQform,(char *)"u") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 121 "vqshow.qsc" */	/* host code */
		/* load the current column and field values to the form */
		IIVQvlfVqLoadFrame(mf);
		/* put form into simple, readonly mode */
		IIVQfmFormMode(mf, TRUE);
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
/* # line 134 "vqshow.qsc" */	/* host code */
		i = IIVQcfCurFld() + 1;
		if (i >= mf->numtabs)
		{
			i = 0;
		}
		STprintf(fname, IIVQtfname_fmt, i);
/* # line 140 "vqshow.qsc" */	/* resume */
            {
              IIresfld(fname);
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 145 "vqshow.qsc" */	/* host code */
		MFCOL *col;
		i4 tabidx;
            i4 colidx;
            i4 tbl;
		char  table_name[FE_MAXTABNAME+1];
		/* get table and column indices of user's current position */
		tabidx = IIVQcfCurFld();
/* # line 154 "vqshow.qsc" */	/* inquire_frs */
            {
              if (IIiqset(2,0,IIVQform,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&tbl,28,(char *)0,0);
              } /* IIiqset */
            }
/* # line 155 "vqshow.qsc" */	/* host code */
		if (tbl == 1)	/* cursor in table field */
		{
			/* set which column name to scroll to 
			   in Examine-a-Table */
/* # line 159 "vqshow.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,IIVQform,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&colidx,29,(char *)0,0);
              } /* IIiqset */
            }
/* # line 160 "vqshow.qsc" */	/* host code */
			colidx--;       /* zero based */
			col = mf->tabs[tabidx]->cols[colidx];
		}
		else
		{
			col = NULL;
		}
		/* Can't pass METAFRAME version of table name to Tables/Examine
		** cause Tables writes to it if user selects NewTable.
		*/
		STcopy (mf->tabs[tabidx]->name, table_name);
		/* call Tables Utility Examine-a-Table */
		IITUtbiTableinfo(table_name,
				NULL,		/* should be table owner */
				IIabExename,
				((col == NULL) ? (char *)NULL : col->name));
          }
        } else if (IIretval() == 3) {
          {
/* # line 183 "vqshow.qsc" */	/* host code */
		/* give user the compressed view of the tables */
		/* form field name to resume to */
		STprintf(fname, IIVQtfname_fmt, 
			IIVQdcvDisplayCompView(mf, IIVQcfCurFld()));
		/* resume to the field that the user was last on in
		   the compressed view. */
/* # line 190 "vqshow.qsc" */	/* resume */
            {
              IIresfld(fname);
              if (1) goto IIfdB1;
            }
          }
        } else if (IIretval() == 4) {
          {
/* # line 195 "vqshow.qsc" */	/* host code */
		FEhelp(ERx("vqshow.hlp"), ERget(F_VQ00BC_vq_prmpt));
          }
        } else if (IIretval() == 5) {
          {
/* # line 200 "vqshow.qsc" */	/* breakdisplay */
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
      if (IInmuact(ERget(F_VQ002D_NextTable),ERget(F_VQ0061_NextTableExpl),2,2
      ,1) == 0) goto IIfdE1;
      if (IInmuact(ERget(F_VQ002C_TableDef),ERget(F_VQ0062_TableDefExpl),2,2,2)
       == 0) goto IIfdE1;
      if (IInmuact(ERget(F_VQ0027_Compress),ERget(F_VQ0064_CompressExpl),2,2,3)
       == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),(char *)0,2,2,4) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,ERget(F_VQ0065_HelpExpl),2,2,4) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),(char *)0,2,2,5) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,ERget(F_VQ0087_EndExpl),2,2,5) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
/* # line 204 "vqshow.qsc" */	/* host code */
	IIdelfrm(IIVQform);
  }
/*{
** Name:	IIVQfmFormMode	- set form mode
**
** Description:
**	Set the mode of the visual query form to readonly or 
**	read/write before.. or after join insertion.
**
** Inputs:
**	METAFRAME *mf;	- pointer to the metaframe structure
**	bool ro;	- flag:
**				TRUE - set form to read only
**				FALSE - set form to read/write 
**
** Outputs:
**	Returns:
**		VOID
**
**	Exceptions:
**
** Side Effects:
**
** History:
**	05/10/89	- tom created
*/
VOID
IIVQfmFormMode(mf, ro)
register METAFRAME *mf;
  bool ro;
  {
	register i4  i;
	register i4  j;
	register TABINFO *ti;
	register TCINFO *tc;
	register MFTAB *tab;
    char *fr_mode;
    char fname[6];
    char *name;
	if (ro)
		fr_mode = ERx("read");
	else
		fr_mode = ERx("update");
/* # line 252 "vqshow.qsc" */	/* set_frs */
    {
      if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
        IIstfsio(21,IIVQform,0,(short *)0,1,32,0,fr_mode);
      } /* IIiqset */
    }
/* # line 254 "vqshow.qsc" */	/* host code */
	for (i = 0; i < mf->numtabs; i++)
	{
		tab = mf->tabs[i];
		ti = &tab_info[tab->usage];
		/* form the base name of the table's field */
		STprintf(fname, IIVQtfname_fmt, i);
		fname[5] = EOS;
		/* loop on the formatting information and set 
		   the displayonly attribute of all but
		   the form inclusion field */
		for (j = 0; j < ti->numcols; j++)
		{
			tc = &ti->cols[j];
			if (	(tc->flags & fdtfCOLREAD) == 0
			   &&	STcompare(tc->name, IIVQinc_col) != 0
			   )
			{
				name = tc->name;
/* # line 274 "vqshow.qsc" */	/* set_frs */
    {
      if (IIiqset(4,0,IIVQform,fname,(char *)0) != 0) {
        IIstfsio(53,name,0,(short *)0,1,30,1,&ro);
      } /* IIiqset */
    }
/* # line 276 "vqshow.qsc" */	/* host code */
			}
		}
		/* now handle the exception fields associated with the 
		   table type. */
		switch (tab->usage)
		{
		case TAB_UPDATE:
			fname[4] = 'i';
/* # line 285 "vqshow.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,IIVQform,(char *)0, (char *)0) != 0) {
        IIstfsio(53,fname,0,(short *)0,1,30,1,&ro);
      } /* IIiqset */
    }
/* # line 287 "vqshow.qsc" */	/* host code */
			fname[4] = 'd';
/* # line 288 "vqshow.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,IIVQform,(char *)0, (char *)0) != 0) {
        IIstfsio(53,fname,0,(short *)0,1,30,1,&ro);
      } /* IIiqset */
    }
/* # line 290 "vqshow.qsc" */	/* host code */
			break;
		case TAB_LOOKUP:
			fname[4] = 'q';
/* # line 294 "vqshow.qsc" */	/* set_frs */
    {
      if (IIiqset(2,0,IIVQform,(char *)0, (char *)0) != 0) {
        IIstfsio(53,fname,0,(short *)0,1,30,1,&ro);
      } /* IIiqset */
    }
/* # line 296 "vqshow.qsc" */	/* host code */
		}
	}
  }
