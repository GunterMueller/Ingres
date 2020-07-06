# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	fetnames.qc
**
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
#include	<compat.h>
#include	<st.h>
#include	<er.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
#include	<fe.h>
# include	<nm.h>
# define KFE
# ifdef FT3270
# undef KFE
# endif
# ifdef VMS
# undef KFE
# endif
/**
** Name:    fetnames.qc -	Front-End Utility Input Test File Names Module.
**
** Descripition:
**	Contains routines used to prompt for file names for testing
**	the Front-Ends.  Defines:
**
**	FEinames()	prompt for input test file name.
**	FEtnames()	prompt for test file names.
**
** History:
**	Revision 3.0  84
**	Initial revision.
**	12/19/87 (dkh) - Fixed jup bug 1648.
**	14-sep-1995 (sweeney)
**		WECOV purge -- it seems to have been SYSV on VAX (?).
**      10-jun-1999 (kinte01)
**         Change VAX to VMS as VAX should not be defined on AlphaVMS
**         systems. The definition of VAX on AlphaVMS systems causes
**         problems with the RMS GW (98236).
*/
/*{
** Name:    FEinames() -	Prompt for Input Test File Name.
*/
VOID
FEinames (cp)
char	*cp;
  {
    char infile[FE_PROMPTSIZE];
    char outfile[FE_PROMPTSIZE];
    char frmfile[FE_PROMPTSIZE];
    char	*kfe = NULL;
    *cp = EOS;
/* # line 62 "fetnames.qsc" */	/* prompt */
    {
      IIprmptio(0,ERx("Enter name of input file: "),(short *)0,1,32,
      FE_PROMPTSIZE-1,infile);
    }
/* # line 63 "fetnames.qsc" */	/* prompt */
    {
      IIprmptio(0,ERx("Enter name of output file: "),(short *)0,1,32,
      FE_PROMPTSIZE-1,outfile);
    }
/* # line 64 "fetnames.qsc" */	/* prompt */
    {
      IIprmptio(0,ERx("Enter name of file for frames: "),(short *)0,1,32,
      FE_PROMPTSIZE-1,frmfile);
    }
/* # line 66 "fetnames.qsc" */	/* host code */
# ifdef KFE
    NMgtAt(ERx("II_TT_KFE"), &kfe);
    if (kfe == NULL)
    {
	_VOID_ STprintf(cp, ERx("-I%s"), infile);
    }
    else
    {
	_VOID_ STprintf(cp, ERx("-I%s -Z%s"), infile, infile);
    }
# else
    _VOID_ STprintf(cp, ERx("-I%s"), infile);
# endif /* KFE */
  }
/*{
** Name:    FEtnames() -	Prompt for Test File Names.
*/
VOID
FEtnames(cp)
char	*cp;
  {
    char infile[FE_PROMPTSIZE];
    char outfile[FE_PROMPTSIZE];
    char frmfile[FE_PROMPTSIZE];
    *cp = EOS;
/* # line 93 "fetnames.qsc" */	/* prompt */
    {
      IIprmptio(0,ERx("Enter name of input file: "),(short *)0,1,32,
      FE_PROMPTSIZE-1,infile);
    }
/* # line 94 "fetnames.qsc" */	/* prompt */
    {
      IIprmptio(0,ERx("Enter name of output file: "),(short *)0,1,32,
      FE_PROMPTSIZE-1,outfile);
    }
/* # line 95 "fetnames.qsc" */	/* prompt */
    {
      IIprmptio(0,ERx("Enter name of file for frames: "),(short *)0,1,32,
      FE_PROMPTSIZE-1,frmfile);
    }
/* # line 96 "fetnames.qsc" */	/* host code */
    _VOID_ STprintf(cp, ERx("-Z%s,%s -D%s"), infile, outfile, frmfile);
  }
