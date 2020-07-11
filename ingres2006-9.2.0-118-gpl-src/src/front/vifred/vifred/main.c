/*
**	Copyright (c) 2004 Ingres Corporation
*/

# include	<compat.h>
# include	<pc.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<me.h>
# include	<ci.h>
# include	<ex.h>
# include	<er.h>
# include	"decls.h"

/**
** Name:	main.c		- vifred main program.
**
** Description:
**	This file defines:
**
**	main		main program for VIFRED.
**
** History:
**	22-dec-1986 (peter)	Added new MKMFIN hints.
**	2/18/87 (daveb)	 -- Call to MEadvise, added MALLOCLIB hint.
**	03/23/87 (dkh) -Added ADFLIB to needlibs list.
**	05/01/87 (dkh) - Added OOLIB to needlibs list.
**	08/14/87 (dkh) - ER changes.
**	14-apr-88 (bruceb)
**		Added, if DG, a send of the terminal's 'is' string at
**		the very beginning of the program.  Needed for CEO.
**      9/21/89 (elein) PC Integration
**              Remove i4 cast of FDhandler();
**	03-nov-89 (bruceb)
**		Added TBLUTILLIB for ABF/VIFRED interface use of IITUtpTblPik.
**	29-jan-90 (ricka)
**		Replaced i4 cast of FDhandler();
**	09/05/90 (dkh) - Integrated round 3 of MACWS changes.
**      09/28/90 (stevet) - Add call to IIUGinit
**      16-nov-1992 (larrym)
**          Added CUFLIB to NEEDLIBS.  CUF is the new common utility facility.
**	26-jan-93 (leighb) - Added i4  return type to main().
**	03/25/94 (dkh) - Added call to EXsetclient().
**	16-jun-95 (emmag)
**	    Define main as ii_user_main on NT.
**	24-sep-95 (tutto01)
**	    Restored main, run as a console app on NT.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      11-Jun-2004 (hanch04)
**          Removed reference to CI for the open source release.
**	29-Sep-2004 (drivi01)
**	    Updated NEEDLIBS to link dynamic library SHQLIB and SHFRAMELIB
**	    to replace its static libraries. Added NEEDLIBSW hint which is 
**	    used for compiling on windows and compliments NEEDLIBS.
**/

# ifdef DGC_AOS
# define main IIVFrsmRingSevenMain
# endif

/*
**	MKMFIN Hints
**
PROGRAM =	vifred

NEEDLIBS =	VIFREDLIB PRINTFORMLIB QBFLIB TBLUTILLIB 
		COMPFRMLIB SHFRAMELIB SHQLIB SHCOMPATLIB

NEEDLIBSW = 	SHEMBEDLIB SHADFLIB

UNDEFS =	II_copyright
*/

i4
main(argc, argv)
i4	argc;
char	**argv;
{

	FUNC_EXTERN i4	FDhandler();	/* interrupt handler for front-ends */
	EX_CONTEXT	context;

	/* Tell EX this is an ingres tool. */
	(void) EXsetclient(EX_INGRES_TOOL);

	/* Use the ingres allocator instead of malloc/free default (daveb) */
	MEadvise( ME_INGRES_ALLOC );

	if (EXdeclare(FDhandler, &context) != OK)
	{
		EXdelete();
		PCexit(-1);
	}

        /* Add call to IIUGinit to initialize character set table and others */

        if ( IIUGinit() != OK )
	{
                PCexit(-1);
        }


# ifdef DATAVIEW
	IIMWomwOverrideMW();
# endif /* DATAVIEW */

	vifred(argc, argv, FALSE);
	EXdelete();
}