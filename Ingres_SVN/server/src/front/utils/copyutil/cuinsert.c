# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**	Copyright (c) 2004 Ingres Corporation
**	All rights reserved.
*/
# include	<compat.h>
# include	<st.h>
# include	<gl.h>
# include	<sl.h>
# include	<iicommon.h>
# include	<fe.h>
# include	<ooclass.h>
# include	<cu.h>
# include	<er.h>
# include	"ercu.h"
/**
** Name:	cuinsert.c -	Insert a record.
**
** Description:
**	IICUidlInsertDetailLine	 Insert a record.
**
** History:
**	25-jun-1987 (Joe)
**		Initial Version
**/
/*{
** Name:	IICUidlInsertDetailLine	- Insert a record.
**
** Description:
**	This does a QUEL param append to insert a record
**	into a table.
**
** Inputs:
**	curec		The CURECORD to insert a tuple for.
**
**	linebuf		The line from the file.
**
**	ids		The id(s) for the parent object(s).
**
** History:
**	25-jun-1987 (Joe)
*/
/*
**  The following define is used when allocating space for a dynamic query
**  string that needs to be PREPAREd.  The majority of the string is built
**  from existing strings.  However, certain words must be interspersed with
**  these strings:
**
**	For INSERTs:  'INSERT INTO '
**
**  This define must be set to cover the largest possible case.
*/
#define LEN_SQL_KEYWORDS 15
STATUS
IICUidlInsertDetailLine(curec, linebuf, ids)
CURECORD	*curec;
char		*linebuf;
IDSTACK		*ids;
{
    char	*table;
    char	*string;
    IISQLDA	*argv;
  char buf[LEN_SQL_KEYWORDS+((FE_MAXNAME+5)*(CU_TOKMAX+CU_LEVELMAX))];
    IICUltdLineToDescriptor(curec, linebuf, ids);
    table = curec->cutable;
    argv = curec->cuargv;
    argv->sqln = curec->cunoelms;
    argv->sqld = curec->cunoelms;
    string = curec->cuinsstr;
    iiuicw1_CatWriteOn();
    STpolycat(3, ERx("INSERT INTO "), table, string, buf);
/* # line 74 "cuinsert.sc" */	/* prepare */
  {
    IIsqInit((char *)0);
    IIsqPrepare(0,(char *)"insert_stmt",(char *)0,0,buf);
  }
/* # line 75 "cuinsert.sc" */	/* execute */
  {
    IIsqInit((char *)0);
    IIsqExStmt((char *)"insert_stmt",0);
    IIsqDaIn(0,argv);
    IIsyncup((char *)0,0);
  }
/* # line 76 "cuinsert.sc" */	/* host code */
    return FEinqerr();
}
