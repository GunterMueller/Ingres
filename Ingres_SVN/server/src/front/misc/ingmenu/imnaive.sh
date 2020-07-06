# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
*/
/*
**	header file used only by "imnaive.qsc"
*/
# define FORM_NAME "imtopfr"
# define	MAXDBNAME_DISP	70	/* max db name display length */
/*	tells top_frame() how to behave */
# define	IM_TOP_MENU		2
# define	IM_QUERY_MENU		3
# define	IM_EXIT			4
/*
** Structure definition for the tables of menu options.
*/
  typedef struct {
    ER_MSGID item;
    ER_MSGID expl;
  } IM_MU_STRUCT;
