# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<er.h>
# include	<abclass.h>
# include	<adf.h>
# include	<ft.h>
# include	<fmt.h>
# include	<frame.h>
# include	<oocat.h>
# include	<ooclass.h>
# include	<st.h>
# include	<ug.h>
# include	<runtime.h>
# include	<cm.h>
# include	<cv.h>
# include	<metafrm.h>
# include	"ervq.h"
/**
** Name:	vqparms.c - User interface for parameter handling
**
** Description:
**	This file has routines to display existing paramters that are being
**	passed to a given frame, allows the user to modify the paramters,
**	and saves the parameters back into the database.
*	diagram frame.
**
**	This file defines:
**
**	IIVQparams		Displays and updates parameters for frames
**
** History:
**	14-sep-89 (kenl) - created
**	20-feb-90 (kenl)
**		Added call to listpick routines for ListChoices.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**/
# define _param_prmpt           (ERget(F_VQ0107_param_prmpt))
# define LOC_UNSET	-1
# define ON_PARENT	1
# define ON_CHILD	0
/* GLOBALDEF's */
GLOBALREF APPL *IIVQappl; /* argument of frame flow entry, the current
			   application we are editing in abf */
/* extern's */
FUNC_EXTERN VOID IIVQfplFreeParamLP();
FUNC_EXTERN VOID IIVQlgLocGlobEdit();
FUNC_EXTERN STATUS IIAMmpMetaPopulate();
/* static's */
static 		PTR	loc_alloc();
  static char *vqparms_form = ERx("vq4glparms");
  static char *vqhlparms_form = ERx("vqhlparms");
  static char *p_from_column = ERx("p_from");
  static char *p_to_column = ERx("p_to");
static	READONLY char	_empty[] = ERx("");
static		bool	first_call = TRUE;
VOID
  IIVQparams(pmf, idx)
METAFRAME	*pmf;
i4		idx;
  {
	MFCFARG		**margs;
	MFCFARG		*cfarg;
    char *pfrom, *pto;
    char from_temp[240], to_temp[65];
    char *pform;
    char frame_name[FE_MAXNAME + 1];
	MFMENU		*cmenu;
	TAGID		tag;
	OOID		frame_type;
	bool		frame;
    i4 changed;
	i4		i, j;
	i4		form_side;
	u_i2		flags = 0;
	if (first_call)
	{
		first_call = FALSE;
		IIARiaForm(vqparms_form);
		IIARiaForm(vqhlparms_form);
	}
	cmenu = pmf->menus[idx];
	if (IIAMmpMetaPopulate(pmf, MF_P_ARG) != OK)
	{
		return;
	}
	margs = cmenu->args;
	frame_type = (*(cmenu->apobj)).class;
	switch (frame_type)
	{
	  case OC_MUFRAME:
	  case OC_APPFRAME:
	  case OC_UPDFRAME:
	  case OC_BRWFRAME:
	  case OC_OSLFRAME:
	  case OC_RWFRAME:
	  case OC_OSLPROC:
	  case OC_DBPROC:
		frame = TRUE;
		pform = vqparms_form;
		break;
	  case OC_HLPROC:
		frame = FALSE;
		pform = vqhlparms_form;
		break;
	  case OC_QBFFRAME:
		IIUGerr(E_VQ0088_No_Params_Allowed, 0, 1, ERx("QBF"));
		return;
	  case OC_GRFRAME:
	  case OC_GBFFRAME:
		IIUGerr(E_VQ0088_No_Params_Allowed, 0, 1, ERx("VIGRAPH"));
		return;
	  default:
		IIUGerr(E_VQ0089_Unknown_Frame_Type, 0, 0);
		return;
	}
/* # line 148 "vqparms.qsc" */	/* inittable */
    {
      if (IItbinit(pform,(char *)"vqparms",(char *)"f") != 0) {
        IItfill();
      } /* IItbinit */
    }
/* # line 149 "vqparms.qsc" */	/* display */
    {
      if (IIdispfrm(pform,(char *)"f") == 0) goto IIfdE1;
      goto IImuI1;
IIfdI1:;
      if (IIfsetio((char *)0) == 0) goto IIfdE1;
      {
/* # line 152 "vqparms.qsc" */	/* host code */
		STcopy(pmf->apobj->name, frame_name);
/* # line 153 "vqparms.qsc" */	/* putform */
        {
          if (IIfsetio((char *)0) != 0) {
            IIputfldio((char *)"parentframe",(short *)0,1,32,0,frame_name);
          } /* IIfsetio */
        }
/* # line 154 "vqparms.qsc" */	/* host code */
		STcopy(cmenu->apobj->name, frame_name);
/* # line 155 "vqparms.qsc" */	/* putform */
        {
          if (IIfsetio((char *)0) != 0) {
            IIputfldio((char *)"currentframe",(short *)0,1,32,0,frame_name);
          } /* IIfsetio */
        }
/* # line 156 "vqparms.qsc" */	/* host code */
		for (i = 0; i < cmenu->numargs; i++)
		{
			pfrom = (*margs)->expr;
			pto = (*margs)->var;
			if (frame)
			{
/* # line 162 "vqparms.qsc" */	/* loadtable */
        {
          if (IItbact(pform,(char *)"vqparms",1) != 0) {
            IItcoputio(p_from_column,(short *)0,1,32,0,pfrom);
            IItcoputio(p_to_column,(short *)0,1,32,0,pto);
            IITBceColEnd();
          } /* IItbact */
        }
/* # line 166 "vqparms.qsc" */	/* host code */
			}
			else
			{
/* # line 169 "vqparms.qsc" */	/* loadtable */
        {
          if (IItbact(pform,(char *)"vqparms",1) != 0) {
            IItcoputio(p_from_column,(short *)0,1,32,0,pfrom);
            IITBceColEnd();
          } /* IItbact */
        }
/* # line 172 "vqparms.qsc" */	/* host code */
			}
			margs++;
		}
      }
IIfdB1:
      while (IIrunform() != 0) {
        if (IIretval() == 1) {
          {
            i4 curcol;
/* # line 183 "vqparms.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,pform,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&curcol,29,(char *)"vqparms",0);
              } /* IIiqset */
            }
/* # line 184 "vqparms.qsc" */	/* host code */
		--curcol;
/* # line 185 "vqparms.qsc" */	/* insertrow */
            {
              if (IItbsetio(5,pform,(char *)"vqparms",curcol) != 0) {
                if (IItinsert() != 0) {
                  IITBceColEnd();
                } /* IItinsert */
              } /* IItbsetio */
            }
/* # line 186 "vqparms.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
          }
        } else if (IIretval() == 2) {
          {
/* # line 192 "vqparms.qsc" */	/* deleterow */
            {
              if (IItbsetio(4,pform,(char *)"vqparms",-2) != 0) {
                if (IItdelrow(0) != 0) {
                } /* IItdelrow */
              } /* IItbsetio */
            }
/* # line 193 "vqparms.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
          }
        } else if (IIretval() == 3) {
          {
            i4 currow;
            i4 moverec;
            i4 state;
/* # line 203 "vqparms.qsc" */	/* getrow */
            {
              if (IItbsetio(2,pform,(char *)"vqparms",-2) != 0) {
                IItcogetio((short *)0,1,30,4,&state,(char *)"_STATE");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 204 "vqparms.qsc" */	/* host code */
		if (state == stUNDEF || state == stDELETE)
		{
			IIUGerr(E_VQ0095_Cannot_Move_Empty_Row,
				UG_ERR_ERROR, (i4) 0);
/* # line 208 "vqparms.qsc" */	/* resume */
            {
              if (1) goto IIfdB1;
            }
/* # line 209 "vqparms.qsc" */	/* host code */
		}
		else
		{
/* # line 212 "vqparms.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,pform,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&currow,29,(char *)"vqparms",0);
              } /* IIiqset */
            }
/* # line 213 "vqparms.qsc" */	/* getrow */
            {
              if (IItbsetio(2,pform,(char *)"vqparms",currow) != 0) {
                IItcogetio((short *)0,1,30,4,&moverec,(char *)"_RECORD");
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 214 "vqparms.qsc" */	/* host code */
		}
/* # line 216 "vqparms.qsc" */	/* display */
            {
              if (IInestmu() == 0) goto IIfdE2;
              goto IImuI2;
IIfdI2:;
IIfdB2:
              while (IIrunnest() != 0) {
                if (IIretval() == 1) {
                  {
                    i4 destrec;
/* # line 222 "vqparms.qsc" */	/* inquire_frs */
                    {
                      if (IIiqset(3,0,pform,(char *)0, (char *)0) != 0) {
                        IIiqfsio((short *)0,1,30,4,&currow,29,(char *)
"vqparms",0);
                      } /* IIiqset */
                    }
/* # line 223 "vqparms.qsc" */	/* getrow */
                    {
                      if (IItbsetio(2,pform,(char *)"vqparms",currow) != 0) {
                        IItcogetio((short *)0,1,30,4,&destrec,(char *)
"_RECORD");
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 224 "vqparms.qsc" */	/* host code */
			if (destrec == moverec)
			{
/* # line 226 "vqparms.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE2;
                    }
/* # line 227 "vqparms.qsc" */	/* host code */
			}
/* # line 228 "vqparms.qsc" */	/* scroll */
                    {
                      if (IItbsetio(1,pform,(char *)"vqparms",-3) != 0) {
                        IItbsmode((char *)"to");
                        if (IItscroll(0,moverec) != 0) {
                        } /* IItscroll */
                      } /* IItbsetio */
                    }
/* # line 230 "vqparms.qsc" */	/* host code */
			if (frame)
			{
/* # line 232 "vqparms.qsc" */	/* getrow */
                    {
                      if (IItbsetio(2,pform,(char *)"vqparms",-2) != 0) {
                        IItcogetio((short *)0,1,32,239,from_temp,p_from_column);
                        IItcogetio((short *)0,1,32,64,to_temp,p_to_column);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 236 "vqparms.qsc" */	/* host code */
			}
			else
			{
/* # line 239 "vqparms.qsc" */	/* getrow */
                    {
                      if (IItbsetio(2,pform,(char *)"vqparms",-2) != 0) {
                        IItcogetio((short *)0,1,32,239,from_temp,p_from_column);
                        IITBceColEnd();
                      } /* IItbsetio */
                    }
/* # line 240 "vqparms.qsc" */	/* host code */
			}
/* # line 242 "vqparms.qsc" */	/* deleterow */
                    {
                      if (IItbsetio(4,pform,(char *)"vqparms",-2) != 0) {
                        if (IItdelrow(0) != 0) {
                        } /* IItdelrow */
                      } /* IItbsetio */
                    }
/* # line 243 "vqparms.qsc" */	/* host code */
			if (moverec < destrec)
			{
				--destrec;
			}
/* # line 247 "vqparms.qsc" */	/* scroll */
                    {
                      if (IItbsetio(1,pform,(char *)"vqparms",-3) != 0) {
                        IItbsmode((char *)"to");
                        if (IItscroll(0,destrec) != 0) {
                        } /* IItscroll */
                      } /* IItbsetio */
                    }
/* # line 248 "vqparms.qsc" */	/* inquire_frs */
                    {
                      if (IIiqset(3,0,pform,(char *)0, (char *)0) != 0) {
                        IIiqfsio((short *)0,1,30,4,&currow,29,(char *)
"vqparms",0);
                      } /* IIiqset */
                    }
/* # line 249 "vqparms.qsc" */	/* host code */
			--currow;
			if (frame)
			{
/* # line 253 "vqparms.qsc" */	/* insertrow */
                    {
                      if (IItbsetio(5,pform,(char *)"vqparms",currow) != 0) {
                        if (IItinsert() != 0) {
                          IItcoputio(p_from_column,(short *)0,1,32,0,from_temp);
                          IItcoputio(p_to_column,(short *)0,1,32,0,to_temp);
                          IITBceColEnd();
                        } /* IItinsert */
                      } /* IItbsetio */
                    }
/* # line 257 "vqparms.qsc" */	/* host code */
			}
			else
			{
/* # line 260 "vqparms.qsc" */	/* insertrow */
                    {
                      if (IItbsetio(5,pform,(char *)"vqparms",currow) != 0) {
                        if (IItinsert() != 0) {
                          IItcoputio(p_from_column,(short *)0,1,32,0,from_temp);
                          IITBceColEnd();
                        } /* IItinsert */
                      } /* IItbsetio */
                    }
/* # line 263 "vqparms.qsc" */	/* host code */
			}
/* # line 265 "vqparms.qsc" */	/* set_frs */
                    {
                      if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) 
                      {
                        IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(1),
                        (void *)IILQint(1));
                      } /* IIiqset */
                    }
/* # line 266 "vqparms.qsc" */	/* breakdisplay */
                    {
                      if (1) goto IIfdE2;
                    }
                  }
                } else if (IIretval() == 2) {
                  {
/* # line 273 "vqparms.qsc" */	/* host code */
			FEhelp(ERx("vqmvparm.hlp"),
					ERget(F_VQ0115_Move_Column_Submenu));
                  }
                } else if (IIretval() == 3) {
                  {
/* # line 281 "vqparms.qsc" */	/* breakdisplay */
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
              if (IInmuact(ERget(FE_Place),ERget(F_VQ0114_ExplPlace),2,2,1) ==
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
        } else if (IIretval() == 4) {
          {
            char formfield[FE_MAXNAME + 1];
/* # line 290 "vqparms.qsc" */	/* host code */
		OO_OBJECT	*frame;
		/* 
		** See what side we're on, so we know whether to
		** display local variables for the parent or the child.
		*/
/* # line 296 "vqparms.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,pform,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,formfield,34,
                (char *)"vqparms",0);
              } /* IIiqset */
            }
/* # line 297 "vqparms.qsc" */	/* host code */
		if (STequal(formfield, p_from_column))
			frame = pmf->apobj;	/* parent */
		else
			frame = cmenu->apobj;	/* child */
		IIVQlgLocGlobEdit(IIVQappl, NULL, frame);
          }
        } else if (IIretval() == 5) {
          {
            char formfield[FE_MAXNAME + 1];
            char ret_val[FE_MAXNAME + 1];
/* # line 312 "vqparms.qsc" */	/* host code */
		ret_val[0]  = EOS;
/* # line 313 "vqparms.qsc" */	/* inquire_frs */
            {
              if (IIiqset(3,0,pform,(char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,32,FE_MAXNAME + 1-1,formfield,34,
                (char *)"vqparms",0);
              } /* IIiqset */
            }
/* # line 314 "vqparms.qsc" */	/* host code */
		if (STequal(formfield, p_from_column))
		{
			if (form_side == ON_CHILD)
			{
				if (flags != 0)
				{
					_VOID_ IIVQfplFreeParamLP();
				}
			}
			IIVQfveFrameVarList(pmf->apobj, ret_val, TRUE, &flags);
			form_side = ON_PARENT;
		}
		else
		{
			if (form_side == ON_PARENT)
			{
				if (flags != 0)
				{
					_VOID_ IIVQfplFreeParamLP();
				}
			}
			IIVQfveFrameVarList((USER_FRAME *)cmenu->apobj,
				ret_val, FALSE, &flags);
			form_side = ON_CHILD;
		}
		if (ret_val[0] != EOS)
		{
/* # line 342 "vqparms.qsc" */	/* set_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIstfsio(20,(char *)0,0,(short *)0,1,30,sizeof(1),
                (void *)IILQint(1));
              } /* IIiqset */
            }
/* # line 343 "vqparms.qsc" */	/* putrow */
            {
              if (IItbsetio(3,pform,(char *)"vqparms",-2) != 0) {
                IItcoputio(formfield,(short *)0,1,32,0,ret_val);
                IITBceColEnd();
              } /* IItbsetio */
            }
/* # line 344 "vqparms.qsc" */	/* host code */
		}
          }
        } else if (IIretval() == 6) {
          {
/* # line 352 "vqparms.qsc" */	/* host code */
		FEhelp(ERx("vqparam.hlp"), _param_prmpt);  
          }
        } else if (IIretval() == 7) {
          {
/* # line 359 "vqparms.qsc" */	/* host code */
		if (flags != 0)
		{
			_VOID_ IIVQfplFreeParamLP();
		}
/* # line 363 "vqparms.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
          }
        } else if (IIretval() == 8) {
          {
            i4 urow = 0;
            i4 state;
/* # line 372 "vqparms.qsc" */	/* host code */
		if (flags != 0)
		{
			_VOID_ IIVQfplFreeParamLP();
		}
/* # line 377 "vqparms.qsc" */	/* inquire_frs */
            {
              if (IIiqset(1,0,(char *)0, (char *)0, (char *)0) != 0) {
                IIiqfsio((short *)0,1,30,4,&changed,20,(char *)0,0);
              } /* IIiqset */
            }
/* # line 378 "vqparms.qsc" */	/* host code */
		if (changed == 0)
		{
			IIAMmfMetaFree(pmf, MF_P_ARG);
/* # line 381 "vqparms.qsc" */	/* breakdisplay */
            {
              if (1) goto IIfdE1;
            }
/* # line 382 "vqparms.qsc" */	/* host code */
		}
		tag = (pmf->ptag)[MF_PT_ARG];
                if (margs == (MFCFARG **)NULL);
		{
			cmenu->args = (MFCFARG **) loc_alloc(tag,
					MF_MAXARGS*sizeof(MFCFARG *), NULL);
		}
                cmenu->numargs = 0;
		if (frame)
		{
/* # line 395 "vqparms.qsc" */	/* unloadtable */
            {
              if (IItbact(pform,(char *)"vqparms",0) == 0) goto IItbE1;
              while (IItunload() != 0) {
                IItcogetio((short *)0,1,32,239,from_temp,p_from_column);
                IItcogetio((short *)0,1,32,64,to_temp,p_to_column);
                IItcogetio((short *)0,1,30,4,&urow,(char *)"_RECORD");
                IItcogetio((short *)0,1,30,4,&state,(char *)"_STATE");
                IITBceColEnd();
                {
/* # line 402 "vqparms.qsc" */	/* host code */
			    if ((state != stDELETE) && (state != stUNDEF))
			    {
                		i = STtrmwhite(from_temp);
				j = STtrmwhite(to_temp);
                		if ((i != 0) && (j != 0))
				{
					cfarg = (MFCFARG *) loc_alloc(tag,
						sizeof(MFCFARG),NULL);
					(cmenu->args)[cmenu->numargs] = cfarg;
                       		 	++(cmenu->numargs);
					cfarg->var = loc_alloc(tag,0,to_temp);
					cfarg->expr =
						loc_alloc(tag,0,from_temp);
				}
				else if ((i == 0) && (j != 0))
				{
					IIUGerr(E_VQ008A_No_Source_Param, 0, 0);
/* # line 419 "vqparms.qsc" */	/* scroll */
                  {
                    if (IItbsetio(1,pform,(char *)"vqparms",-3) != 0) {
                      IItbsmode((char *)"to");
                      if (IItscroll(0,urow) != 0) {
                      } /* IItscroll */
                    } /* IItbsetio */
                  }
/* # line 420 "vqparms.qsc" */	/* resume */
                  {
                    IIrescol((char *)"vqparms",p_from_column);
                    IItunend();
                    if (1) goto IIfdB1;
                  }
/* # line 421 "vqparms.qsc" */	/* host code */
				}
				else if ((i != 0) && (j == 0))
				{
					IIUGerr(E_VQ008B_No_Destin_Param, 0, 0);
/* # line 425 "vqparms.qsc" */	/* scroll */
                  {
                    if (IItbsetio(1,pform,(char *)"vqparms",-3) != 0) {
                      IItbsmode((char *)"to");
                      if (IItscroll(0,urow) != 0) {
                      } /* IItscroll */
                    } /* IItbsetio */
                  }
/* # line 426 "vqparms.qsc" */	/* resume */
                  {
                    IIrescol((char *)"vqparms",p_to_column);
                    IItunend();
                    if (1) goto IIfdB1;
                  }
/* # line 427 "vqparms.qsc" */	/* host code */
				}
			    }
                }
              } /* IItunload */
IItbE1:
              IItunend();
            }
/* # line 430 "vqparms.qsc" */	/* host code */
		}
		else
		{
/* # line 433 "vqparms.qsc" */	/* unloadtable */
            {
              if (IItbact(pform,(char *)"vqparms",0) == 0) goto IItbE2;
              while (IItunload() != 0) {
                IItcogetio((short *)0,1,32,239,from_temp,p_from_column);
                IItcogetio((short *)0,1,30,4,&urow,(char *)"_RECORD");
                IItcogetio((short *)0,1,30,4,&state,(char *)"_STATE");
                IITBceColEnd();
                {
/* # line 439 "vqparms.qsc" */	/* host code */
				if ((state != stDELETE) && (state != stUNDEF) &&
                			(STtrmwhite(from_temp) != 0))
				{
					cfarg = (MFCFARG *) loc_alloc(tag,
						sizeof(MFCFARG),NULL);
					(cmenu->args)[cmenu->numargs] = cfarg;
                       		 	++(cmenu->numargs);
					cfarg->var = ERx("");
					cfarg->expr =
						loc_alloc(tag,0,from_temp);
				}
                }
              } /* IItunload */
IItbE2:
              IItunend();
            }
/* # line 451 "vqparms.qsc" */	/* host code */
		}
		/* set the metaframe state to say that it needs regen */
		pmf->state |= MFST_DOGEN;
	        /* write the flags to the database */
		IIAMufqUserFrameQuick(pmf->apobj);
		/* update the frame parameters */
                IIAMmuMetaUpdate(pmf, MF_P_ARG);
		/* free the frame parameter structures */
		IIAMmfMetaFree(pmf, MF_P_ARG);
/* # line 463 "vqparms.qsc" */	/* breakdisplay */
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
      if (IInmuact(ERget(FE_Insert),ERget(F_FE0110_InsertExpl),2,2,1) == 0) 
      goto IIfdE1;
      if (IInmuact(ERget(FE_Delete),ERget(F_FE0111_DeleteExpl),2,2,2) == 0) 
      goto IIfdE1;
      if (IInmuact(ERget(FE_Move),ERget(F_VQ0113_ExplMove),2,2,3) == 0) goto 
      IIfdE1;
      if (IInmuact(ERget(F_VQ00A8_vqloc_glob_menu),
      ERget(F_VQ00A9_vqloc_glob_expl),0,0,4) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Lookup),ERget(FE_LookupExpl),0,0,5) == 0) goto 
      IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(10,(char *)0,0,0,5) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Help),ERget(F_FE0100_ExplFrameHelp),0,0,6) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(1,(char *)0,0,0,6) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_Cancel),ERget(F_FE0109_ExplForget),0,0,7) == 0) 
      goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(9,(char *)0,0,0,7) == 0) goto IIfdE1;
      if (IInmuact(ERget(FE_End),(char *)0,2,2,8) == 0) goto IIfdE1;
      IIFRInternal(0);
      if (IInfrskact(3,ERget(F_FE0102_ExplEnd),2,2,8) == 0) goto IIfdE1;
      if (IIendmu() == 0) goto IIfdE1;
      goto IIfdI1;
IIfdE1:;
      IIendfrm();
    }
  }
/* # line 470 "vqparms.qsc" */	/* host code */
static PTR
loc_alloc(tag,size,str)
TAGID   tag;
i4      size;
char    *str;
{
	PTR mem;
	if (str != NULL)
		size = STlength(str)+1;
	mem = FEreqmem((u_i4)tag, (u_i4)size, FALSE, (STATUS *)NULL);
	if (mem == NULL)
	{
		return (NULL);
	}
	if (str != NULL)
		STcopy(str,(char *) mem);
	return (mem);
}
