# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<st.h>
# include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include       <ooclass.h>
# include <metafrm.h>
# include <abclass.h>
# include       <erug.h>
# include       <erfg.h>
# include       "vqsecnam.h"
# include       "ervq.h"
/**
** Name:	vqqparts.qsc -	Edit query parts.
**
** Description:
**	This file defines:
**
**	IIVQeqpEditQueryParts	- Edit Query Parts.
**
** History:
**	11/20/89 (Mike S) 	Initial Version	
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
/* # define's */
/* GLOBALDEF's */
/* extern's */
FUNC_EXTERN VOID	FEhelp();
FUNC_EXTERN STATUS	IIAMmfMetaFree();
FUNC_EXTERN STATUS	IIAMmpMetaPopulate();
FUNC_EXTERN STATUS	IIAMmuMetaUpdate();
FUNC_EXTERN MFTAB 	*IIFGpt_pritbl();
FUNC_EXTERN VOID        IIUGerr();
FUNC_EXTERN bool 	IIVQcsqCheckSeq();
FUNC_EXTERN char 	*IIVQgivGetIncVal();
/* static's */
static	init_done = FALSE;
/* form, field, and column names */
static READONLY
  char _form[]= 	ERx("vqqparts"), _title[]= 	ERx("title"), _table[]= 	ERx("table")
  , _frame[]= 	ERx("frame"), _info[]= 	ERx("info"), _info_title[]= ERx("info_title")
  , _column[]= 	ERx("column"), _display[]= 	ERx("display"), _index[]= 	ERx("index")
  , _blank[]=	ERx(" ");
/* Text pointers */
static 
  char *astitle, *dftitle, *qrtitle, *ascol, *dfcol, *qrcol;
/*{
** Name:	IIVQeqpEditQueryParts   - Edit Query Parts.
**
** Description:
**	This frame allows the user to edit the query restrictions and	
**	assignments/defaults from the Visual Query.  It is called from	
**	the Compilation Error frame to fix an error with a query part.
**
** Inputs:
**	frame		USER_FRAME *	the frame whose VQ should be fixed
**	esection	i4		error section (INSERT, SELECT, or
**					DEFAULT)
**	qsection	i4		query section (master or detail)
**
** Outputs:
**	none
**
**	Returns:
**		none
**
**	Exceptions:
**		none
**
** Side Effects:
**	The VQ may be loaded from the database.  It will be updated if
**	the user changes anything.
**
** History:
**	11/20/89 (Mike S)	Initial Version
*/
VOID IIVQeqpEditQueryParts(frame, esection, qsection)
USER_FRAME	*frame;
i4		esection;
i4		qsection;
{
	/* Frame's metaframe */
	METAFRAME	*metaframe = frame->mf;
  char *framename = frame->name;
	bool		wasinmem;	/* Was VQ in memory */
	MFTAB		*table;		/* Table we're displaying info for */
	MFCOL		*column;	/* Current column */
	i4		num_changes = 0;/* Number of changes made */
  i4 i;
  char *colname;
  char *info;
  char *displayed;
  char *tabname;
  char *disp;
	TAGID	tag;			/* Tag for memory allocation */
	/* Load the VQ into memory, if need be */
	wasinmem = (metaframe->popmask & MF_P_VQ) != 0;
	if (!wasinmem)
	{
		if (IIAMmpMetaPopulate(metaframe, MF_P_VQ) != OK)
		{
			return;
		}
	}
	tag = metaframe->ptag[MF_PT_VQ];
	/* Init the form, if need be, and get some text pointers */
	if (!init_done)
	{
		IIARiaForm(_form);
              	astitle = ERget(F_VQ003E_assgns_text);
              	dftitle = ERget(F_VQ003F_defaults_text);
              	qrtitle = ERget(F_VQ003C_rests_text);
              	ascol = ERget(F_VQ003D_assgn_text);
              	dfcol = ERget(F_VQ0040_detault_text);
              	qrcol = ERget(F_VQ00C9_rest_text);
		init_done = TRUE;
	}
	/* Fund table we're displaying */
	if (qsection == EST_QMASTER)
		table = IIFGpt_pritbl(metaframe, TAB_MASTER);
	else
		table = IIFGpt_pritbl(metaframe, TAB_DETAIL);
	if (table == NULL)
	{
		IIUGerr(E_VQ0084_NoSuchSection, 0, 2, framename, 
			ERget((qsection == EST_QMASTER) ? 
			       F_FG0018_Master : 
			       F_FG0019_Detail));
		return;
	}
	tabname = table->name;
	/* Set up titles and load the tablefield */
/* # line 172 "vqqparts.qsc" */	/* inittable */
  {
    if (IItbinit(_form,_info,(char *)"u") != 0) {
      IIthidecol(_index,(char *)"i4");
      IItfill();
    } /* IItbinit */
  }
/* # line 173 "vqqparts.qsc" */	/* putform */
  {
    if (IIfsetio(_form) != 0) {
      IIputfldio(_frame,(short *)0,1,32,0,framename);
      IIputfldio(_table,(short *)0,1,32,0,tabname);
    } /* IIfsetio */
  }
/* # line 175 "vqqparts.qsc" */	/* host code */
	switch (esection)
	{
	    case EST_DEFAULT:
		/* 
		** We're doing defaults -- only displayed columns and those
		** in variables
		*/
/* # line 182 "vqqparts.qsc" */	/* putform */
  {
    if (IIfsetio(_form) != 0) {
      IIputfldio(_title,(short *)0,1,32,0,dftitle);
      IIputfldio(_info_title,(short *)0,1,32,0,dfcol);
    } /* IIfsetio */
  }
/* # line 183 "vqqparts.qsc" */	/* host code */
		for (i = 0; i < table->numcols; i++)
		{
			column = table->cols[i];
			if ((column->flags & (COL_USED|COL_VARIABLE)) == 0)
				continue;
			colname = column->name;
			info = column->info;
			disp = IIVQgivGetIncVal(column);
/* # line 193 "vqqparts.qsc" */	/* loadtable */
  {
    if (IItbact(_form,_info,1) != 0) {
      IItcoputio(_column,(short *)0,1,32,0,colname);
      IItcoputio(_display,(short *)0,1,32,0,disp);
      IItcoputio(_info,(short *)0,1,32,0,info);
      IItcoputio(_index,(short *)0,1,30,4,&i);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 196 "vqqparts.qsc" */	/* host code */
		}
		break;
	    case EST_INSERT:
		/* We're doing assignments -- only non-used columns */
/* # line 201 "vqqparts.qsc" */	/* putform */
  {
    if (IIfsetio(_form) != 0) {
      IIputfldio(_title,(short *)0,1,32,0,astitle);
      IIputfldio(_info_title,(short *)0,1,32,0,ascol);
    } /* IIfsetio */
  }
/* # line 202 "vqqparts.qsc" */	/* host code */
		for (i = 0; i < table->numcols; i++)
		{
			column = table->cols[i];
			if ((column->flags & (COL_USED|COL_VARIABLE)) != 0)
				continue;
			colname = column->name;
			info = column->info;
			disp = IIVQgivGetIncVal(column);
/* # line 212 "vqqparts.qsc" */	/* loadtable */
  {
    if (IItbact(_form,_info,1) != 0) {
      IItcoputio(_column,(short *)0,1,32,0,colname);
      IItcoputio(_display,(short *)0,1,32,0,disp);
      IItcoputio(_info,(short *)0,1,32,0,info);
      IItcoputio(_index,(short *)0,1,30,4,&i);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 215 "vqqparts.qsc" */	/* host code */
		}
		break;
	    case EST_SELECT:
		/* We're doing query restrictions */
/* # line 220 "vqqparts.qsc" */	/* putform */
  {
    if (IIfsetio(_form) != 0) {
      IIputfldio(_title,(short *)0,1,32,0,qrtitle);
      IIputfldio(_info_title,(short *)0,1,32,0,qrcol);
    } /* IIfsetio */
  }
/* # line 221 "vqqparts.qsc" */	/* host code */
		for (i = 0; i < table->numcols; i++)
		{
			column = table->cols[i];
			colname = column->name;
			info = column->info;
			disp = IIVQgivGetIncVal(column);
/* # line 229 "vqqparts.qsc" */	/* loadtable */
  {
    if (IItbact(_form,_info,1) != 0) {
      IItcoputio(_column,(short *)0,1,32,0,colname);
      IItcoputio(_display,(short *)0,1,32,0,disp);
      IItcoputio(_info,(short *)0,1,32,0,info);
      IItcoputio(_index,(short *)0,1,30,4,&i);
      IITBceColEnd();
    } /* IItbact */
  }
/* # line 232 "vqqparts.qsc" */	/* host code */
		}
	}
	/* Run the display loop */
/* # line 236 "vqqparts.qsc" */	/* display */
  {
    if (IIdispfrm(_form,(char *)"u") == 0) goto IIfdE1;
    goto IImuI1;
IIfdI1:;
IIfdB1:
    while (IIrunform() != 0) {
      if (IIretval() == 1) {
        {
/* # line 240 "vqqparts.qsc" */	/* host code */
		/* Avoid out-of-data message */
          i4 tfrec;
/* # line 243 "vqqparts.qsc" */	/* getrow */
          {
            if (IItbsetio(2,_form,_info,-2) != 0) {
              IItcogetio((short *)0,1,30,4,&tfrec,(char *)"_RECORD");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 245 "vqqparts.qsc" */	/* host code */
		if (1 != tfrec)
		{
/* # line 247 "vqqparts.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_form,_info,-3) != 0) {
              IItbsmode((char *)"down");
              if (IItscroll(0,-3) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
/* # line 248 "vqqparts.qsc" */	/* host code */
		}
        }
      } else if (IIretval() == 2) {
        {
/* # line 253 "vqqparts.qsc" */	/* host code */
		/* Avoid out-of-data message */
          i4 tfrec, tfrows;
/* # line 256 "vqqparts.qsc" */	/* inquire_frs */
          {
            if (IIiqset(3,0,_form,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&tfrows,32,_info,0);
            } /* IIiqset */
          }
/* # line 257 "vqqparts.qsc" */	/* getrow */
          {
            if (IItbsetio(2,_form,_info,-2) != 0) {
              IItcogetio((short *)0,1,30,4,&tfrec,(char *)"_RECORD");
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 259 "vqqparts.qsc" */	/* host code */
		if (tfrows != tfrec)
		{
/* # line 261 "vqqparts.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_form,_info,-3) != 0) {
              IItbsmode((char *)"up");
              if (IItscroll(0,-3) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
/* # line 262 "vqqparts.qsc" */	/* host code */
		}
        }
      } else if (IIretval() == 3) {
        {
          char buffer[133];
          i4 colindex;
/* # line 271 "vqqparts.qsc" */	/* getrow */
          {
            if (IItbsetio(2,_form,_info,-2) != 0) {
              IItcogetio((short *)0,1,32,132,buffer,_info);
              IItcogetio((short *)0,1,30,4,&colindex,_index);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 272 "vqqparts.qsc" */	/* host code */
		table->cols[colindex]->info = STtalloc(tag, buffer);
		/* Check if "sequenced" was entered invalidly */
		if (!IIVQcsqCheckSeq(metaframe, table, table->cols[colindex]))
		{
/* # line 277 "vqqparts.qsc" */	/* putrow */
          {
            if (IItbsetio(3,_form,_info,-2) != 0) {
              IItcoputio(_info,(short *)0,1,32,0,_blank);
              IITBceColEnd();
            } /* IItbsetio */
          }
/* # line 278 "vqqparts.qsc" */	/* set_frs */
          {
            if (IIiqset(5,0,_form,_info,(char *)0) != 0) {
              IIstfsio(20,_info,0,(short *)0,1,30,sizeof(1),(void *)IILQint(1));
            } /* IIiqset */
          }
/* # line 279 "vqqparts.qsc" */	/* resume */
          {
            if (1) goto IIfdB1;
          }
/* # line 280 "vqqparts.qsc" */	/* host code */
		}
		else
		{
/* # line 283 "vqqparts.qsc" */	/* resume */
          {
            IIresnext();
            if (1) goto IIfdB1;
          }
/* # line 284 "vqqparts.qsc" */	/* host code */
		}
        }
      } else if (IIretval() == 4) {
        {
          i4 changed;
          char buffer[133];
/* # line 294 "vqqparts.qsc" */	/* host code */
		/* Replace anything which changed */
/* # line 295 "vqqparts.qsc" */	/* unloadtable */
          {
            if (IItbact(_form,_info,0) == 0) goto IItbE1;
            while (IItunload() != 0) {
              IItcogetio((short *)0,1,32,132,buffer,_info);
              IItcogetio((short *)0,1,30,4,&i,_index);
              IITBceColEnd();
              {
/* # line 297 "vqqparts.qsc" */	/* inquire_frs */
                {
                  if (IIiqset(5,0,_form,_info,(char *)0) != 0) {
                    IIiqfsio((short *)0,1,30,4,&changed,20,_info,0);
                  } /* IIiqset */
                }
/* # line 298 "vqqparts.qsc" */	/* host code */
			if (changed)
			{
				table->cols[i]->info = STtalloc(tag, buffer);
				num_changes++;
			}	
              }
            } /* IItunload */
IItbE1:
            IItunend();
          }
/* # line 305 "vqqparts.qsc" */	/* clear */
          {
            IIfldclear(_info);
          }
/* # line 306 "vqqparts.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 5) {
        {
/* # line 313 "vqqparts.qsc" */	/* clear */
          {
            IIfldclear(_info);
          }
/* # line 314 "vqqparts.qsc" */	/* breakdisplay */
          {
            if (1) goto IIfdE1;
          }
        }
      } else if (IIretval() == 6) {
        {
/* # line 321 "vqqparts.qsc" */	/* host code */
		FEhelp(ERx("vqqparts.hlp"), ERget(S_VQ0027_EditVQInfo));
        }
      } else if (IIretval() == 7) {
        {
/* # line 326 "vqqparts.qsc" */	/* host code */
                /* top */
/* # line 327 "vqqparts.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_form,_info,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,1) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
        }
      } else if (IIretval() == 8) {
        {
/* # line 332 "vqqparts.qsc" */	/* host code */
                /* bottom */
          i4 no_rows;
/* # line 335 "vqqparts.qsc" */	/* inquire_frs */
          {
            if (IIiqset(3,0,_form,(char *)0, (char *)0) != 0) {
              IIiqfsio((short *)0,1,30,4,&no_rows,32,_info,0);
            } /* IIiqset */
          }
/* # line 337 "vqqparts.qsc" */	/* scroll */
          {
            if (IItbsetio(1,_form,_info,-3) != 0) {
              IItbsmode((char *)"to");
              if (IItscroll(0,no_rows) != 0) {
              } /* IItscroll */
            } /* IItbsetio */
          }
        }
      } else if (IIretval() == 9) {
        {
/* # line 342 "vqqparts.qsc" */	/* host code */
                /* find */
                FEtabfnd(_form, _info);
        }
      } else {
        if (1) goto IIfdE1;
      } /* IIretval */
    } /* IIrunform */
IIfdF1:
    if (IIchkfrm() == 0) goto IIfdB1;
    goto IIfdE1;
IImuI1:
    if (IIactscrl(_info,1,1) == 0) goto IIfdE1;
    if (IIactscrl(_info,0,2) == 0) goto IIfdE1;
    if (IITBcaClmAct(_info,_info,0,3) == 0) goto IIfdE1;
    if (IIinitmu() == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_OK),ERget(F_VQ0038_ChangesOK),2,2,4) == 0) goto 
    IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(4,(char *)0,2,2,4) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Cancel),ERget(F_FE0109_ExplForget),2,2,5) == 0) 
    goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(9,(char *)0,2,2,5) == 0) goto IIfdE1;
    if (IInmuact(ERget(FE_Help),ERget(F_FE0101_ExplKeyHelp),2,2,6) == 0) goto 
    IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(1,(char *)0,2,2,6) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(5,ERget(F_FE0105_ExplTop),2,2,7) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(6,ERget(F_FE0106_ExplBottom),2,2,8) == 0) goto IIfdE1;
    IIFRInternal(0);
    if (IInfrskact(7,ERget(F_FE0104_ExplFind),2,2,9) == 0) goto IIfdE1;
    if (IIendmu() == 0) goto IIfdE1;
    goto IIfdI1;
IIfdE1:;
    IIendfrm();
  }
/* # line 347 "vqqparts.qsc" */	/* host code */
	/* Update VQ, if anything changed */
	if (num_changes > 0)
	{
		metaframe->state |= MFST_DOGEN;
		_VOID_ IIAMmuMetaUpdate(metaframe, MF_P_VQ);
		IIAMufqUserFrameQuick(metaframe->apobj);
	}
	/* Free it, if it wasn't in memory in the first place*/
	if (!wasinmem)
		_VOID_ IIAMmfMetaFree(metaframe, MF_P_VQ);
	return;
}		
