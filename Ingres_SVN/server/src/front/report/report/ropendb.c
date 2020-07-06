# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include	<compat.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<adf.h>
# include	<fmt.h>
# include	 <rtype.h> 
# include	 <rglob.h> 
# include	<st.h>
# include	<er.h>
# include <uigdata.h>
/*
**   R_OPEN_DB - open the specified database, using EQUEL. 
**
**	Parameters:
**		none.
**
**	Returns:
**		none.  (Will abort if no database exists.)
**
**	Trace Flags:
**		1.3.
**
**	History:
**		3/5/81 (ps) - written.
**		
**		9/8/82 (jrc) - Added call to IIxingres if called as subproc.
**
**		11/4/85 (jrc) - Added funny init for internation stuff.
**
**		10-oct-1986 (Joe)
**			Added Saturn changes.  Uses FEingres now
**			to start DB.
**		16-june-1988 (danielt)
**			fix for bug 12070 really abort if error with
**			FEingres.
**		3/21/90 (martym)
**			Added code to pass down -P (password), and -G 
**			(group ID) flags down to the DBMS, if set by 
**			the user.
**	28-aug-1990 (Joe)
**	    Changed references to IIUIgdata to the UIDBDATA structure
**	    returned from IIUIdbdata().
**	31-aug-90 (sylviap)
**	    Changed include to be uigdata.qsh.
**	16-sep-1993 (rdrane)
**		Call FEningres() directly, rather than through the
**		FEingres() cover.  We need to run with level 3
**		system catalogs, and FEingres() only requests level 2.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      28-apr-2004 (srisu02)
**             Changes for addition of new flag "numeric_overflow=ignore|fail|warn"
**	23-aug-2004 (lazro01)
**	    BUG 112819
**	    Changes to invoke FEningres() with the dictionary client
**	    version as 4. 
**      28-Mar-2005 (lakvi01)
**          Properly prototype r_open_db.
*/
VOID
r_open_db ()
{
	/* external declarations */
  extern char *En_database;
  extern char *En_uflag;
	/* internal declarations */
  char *uname;
	char	*pipe;
	char	*uflag;
	char	*gidflag;
	char 	*password;
        char    num_ostr[40];
	/* start of routine */
	if (!St_called)
	{
		pipe = r_xpipe == NULL ? ERx("") : r_xpipe;
		uflag = En_uflag == NULL ? ERx("") : En_uflag;
		gidflag = En_gidflag == NULL ? ERx("") : En_gidflag;
		password = En_passflag == NULL ? ERx("") : En_passflag;
                if(En_num_oflag == NUM_O_FAIL)
                {
	          STcopy("-numeric_overflow=fail",num_ostr);
                }
                else if(En_num_oflag == NUM_O_WARN)
                {
	          STcopy("-numeric_overflow=warn",num_ostr);
                }
                else if(En_num_oflag == NUM_O_IGNORE)
                {
	          STcopy("-numeric_overflow=ignore",num_ostr);
                }
		if  (FEningres(ERx("INGRES"),(i4)4,
				En_database, uflag, pipe, 
				gidflag, password, num_ostr,NULL) != OK)
		{
			r_error(1000, FATAL, En_database, NULL);
		}
	}
	St_db_open = TRUE;
	/* get the user and dba code */
	STcopy(IIUIdbdata()->user, Usercode);
	STcopy(IIUIdbdata()->dba, Dbacode);
	return;
}
