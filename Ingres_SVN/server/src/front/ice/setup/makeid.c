# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
/*
** Name: makeid.sc
**
** Description:
**      Simple program to insert a range of integers into a column.
**      This is so blobstor can update the table and negates the need for
**      a filename column.
**
** History:
**      09-Oct-98 (fanra01)
**          Created.
**      05-Mar-1999 (peeje01)
**          Branched from NT.
**	    MING hints added.
**	21-jan-1999 (hanch04)
**	    replace nat and longnat with i4
**	31-aug-2000 (hanch04)
**	    cross change to main
**	    replace nat and longnat with i4
**      22-May-2007 (horda03) Bug 118375
**          Delete all rows in the table before inserting the new rows
**          to prevent duplicates.
*/
# include <compat.h>
# include <cv.h>
# include <si.h>
# include <st.h>
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
/*
** Name: Usage
**
** Description:
**      Display a usage message
**
** Inputs:
**      progname    pointer to program name string.
**
** Outputs:
**      none
**
** Returns:
**      none
**
** History:
**      09-Oct-98 (fanra01)
**          Created.
*/
void
usage (char* progname)
{
    SIprintf ("Usage:\n");
    SIprintf ("      %s <start> <end> <column> <table> <database name>\n",
        progname);
}
/*
** Name: main
**
** Description:
**      Insert the range of values passed from the command line into the
**      specified column, table and database.
**
** Inputs:
**      argv[1]     start of range
**      argv[2]     end of range
**      argv[3]     column name
**      argv[4]     table name
**      argv[5]     database name
**
** Outputs:
**      none
**
** Returns:
**      OK      successfully completed
**      FAIL    on error
**
** History:
**      09-Oct-98 (fanra01)
**          Created.
PROGRAM =	makeid
**
NEEDLIBS =	LIBINGRES
*/
STATUS
main (i4 argc, char** argv)
{
    STATUS  status = FAIL;
    i4      start = 0;
    i4      end = 0;
    i4      i;
    char*   colname;
    char*   tbname;
  char *dbname;
  char statement[80];
    if (argc < 6)
    {
        usage(argv[0]);
    }
    else
    {
        CVan (argv[1], &start);
        CVan (argv[2], &end);
        colname= argv[3];
        tbname = argv[4];
        dbname = argv[5];
        status = OK;
    }
    if (status == OK)
    {
/* # line 121 "makeid.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IIsqConnect(0,dbname,(char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 123 "makeid.sc" */	/* host code */
        STprintf (statement, "delete from %s", tbname);
/* # line 125 "makeid.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(statement);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 127 "makeid.sc" */	/* host code */
        for (i=start; i < (end+1); i++)
        {
            STprintf (statement, "insert into %s (%s) values (%d)", tbname,
                colname, i);
/* # line 131 "makeid.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(statement);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 132 "makeid.sc" */	/* host code */
        }
/* # line 133 "makeid.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
    if (sqlca.sqlcode < 0) 
      IIsqPrint(&sqlca);
  }
/* # line 134 "makeid.sc" */	/* host code */
    }
    return (status);
}
