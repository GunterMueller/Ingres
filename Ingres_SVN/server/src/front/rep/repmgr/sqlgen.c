# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
** Copyright (c) 2004 Ingres Corporation
*/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    extern IISQLCA sqlca;   /* SQL Communications Area */
# include <compat.h>
# include <st.h>
# include <cm.h>
# include <nm.h>
# include <lo.h>
# include <si.h>
# include <pc.h>
# include <er.h>
# include <gl.h>
# include <iicommon.h>
# include <fe.h>
# include <ug.h>
# include <generr.h>
# include "repgen.h"
# include "errm.h"
/**
** Name:	sqlgen.sc - Generate SQL replication objects
**
** Description:
**	Defines
**		sql_gen - Generate SQL replication objects
**
** History:
**	16-dec-96 (joea)
**		Created based on sqlgen.sc in replicator library.
**	25-sep-98 (abbjo03)
**		Replace ddba_messageit with IIUGerr.
**/
static char	directory[MAX_LOC+1] ZERO_FILL;	/* read directory or path */
/*{
** Name:	sql_gen - Generate SQL replication objects
**
** Description:
**	Expands tokens in input file and creates (or appends to) a file
**	&/or executes the result immediately.
**
** Inputs:
**	tablename	- table name
**	table_no	- table number
**	table_owner	- table owner
**	template_filename - template file name
**	output_filename	- output file name
**
** Outputs:
**	none
**
** Returns:
**	0		success
**	-1		bad parameter(s)
**	-N		messageit message number
**	GE_NO_RESOURCE	SQL code executed via TM
**	GE_NO_PRIVILEGE	SQL code executed via TM
**	> 0		Ingres error
**
** Side effects:
**	The output filename is created and executed.
*/
STATUS
sql_gen(
char	*tablename,
char	*table_owner,
i4	table_no,
char	*template_filename,
char	*output_filename)
{
  char execute_string[MAX_EXEC_SIZE];
  char error_text[1000];
  i4 err_ret = 0;
  char localdb[DB_MAXNAME+1];
  char tmp[MAX_SIZE];
  char vnode_name[DB_MAXNAME+1];
	STATUS	return_code = OK;
	FILE	*in_file;
	FILE	*out_file;
	LOCATION	loc;
	LOCATION	temp_loc;
	char	loc_name[MAX_LOC+1];
	char	temp_loc_name[MAX_LOC+1];
	LOCATION dir_loc;
	char	*p;
	CL_ERR_DESC	err_code;
	if (tablename == NULL || *tablename == EOS)
	{
		IIUGerr(E_RM00EE_Null_empty_arg, UG_ERR_ERROR, 1, 
			ERx("sql_gen"));
		return (-1);
	}
	if (*directory == EOS)
	{
		if (NMloc(FILES, PATH, NULL, &dir_loc) != OK)
			return (-1);
		if (LOfaddpath(&dir_loc, ERx("rep"), &dir_loc) != OK)
			return (-1);
		LOtos(&dir_loc, &p);
		STcopy(p, directory);
	}
	if ((in_file = open_infile(directory, template_filename)) == NULL)
		return (-1);
	if (NMloc(TEMP, PATH, NULL, &temp_loc) != OK)
		return (-1);
	LOtos(&temp_loc, &p);
	STcopy(p, temp_loc_name);
	if ((out_file = open_outfile(output_filename, temp_loc_name, ERx("c"),
			loc_name, TRUE)) == NULL)
		return (-1);
	if (exp_token(tablename, table_owner, table_no, directory, in_file,
			out_file, TRUE, execute_string))
		return (-1);
	SIclose(in_file);
	SIfprintf(out_file, ERx("\\p\\g\nCOMMIT\\g\n\\q\n"));
	SIclose(out_file);
	/* Try to exec immediate first, then save to file if GE_NO_RESOURCE */
/* # line 132 "sqlgen.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(execute_string);
    IIsyncup((char *)0,0);
  }
/* # line 133 "sqlgen.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err_ret,2);
  }
/* # line 134 "sqlgen.sc" */	/* host code */
	return_code = err_ret;
	switch (err_ret)
	{
	case OK:
/* # line 138 "sqlgen.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 139 "sqlgen.sc" */	/* host code */
		break;
	case GE_NO_RESOURCE:
/* # line 142 "sqlgen.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
  }
/* # line 143 "sqlgen.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select vnode_name, database_name from dd_databases where local_db=1");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,vnode_name);
      IIgetdomio((short *)0,1,32,DB_MAXNAME+1-1,localdb);
    } /* IInextget */
    IIsqFlush((char *)0,0);
  }
/* # line 147 "sqlgen.sc" */	/* inquire_sql */
  {
    IILQisInqSqlio((short *)0,1,30,4,&err_ret,2);
    IILQisInqSqlio((short *)0,1,32,999,error_text,63);
  }
/* # line 149 "sqlgen.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 150 "sqlgen.sc" */	/* host code */
		if (err_ret)
		{
			IIUGerr(E_RM00EF_Err_rtrv_node, UG_ERR_ERROR, 1,
				error_text);
			return_code = err_ret;
			break;
		}
		STtrmwhite(vnode_name);
		STtrmwhite(localdb);
		STprintf(tmp, ERx("rpgensql %s %s %s %s"), table_owner,
			vnode_name, localdb, loc_name);
		err_ret = PCcmdline(NULL, tmp, PC_WAIT, NULL, &err_code);
		if (err_ret)
			return_code = err_ret;
		break;
	default:
/* # line 167 "sqlgen.sc" */	/* rollback */
  {
    IIsqInit(&sqlca);
    IIxact(2);
  }
/* # line 168 "sqlgen.sc" */	/* host code */
		break;
	}
	if (return_code == OK)
	{
		LOfroms(PATH & FILENAME, loc_name, &loc);
		LOdelete(&loc);
	}
	return (return_code);
}
