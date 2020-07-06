# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqdef.h"
/*
**  Copyright (C) 2005-2006 Ingres Corporation. All Rights Reserved.
*/
/*
** Source   : sqlcopy.sc
** Project  : Data Loader
** Author   : UK Sotheavut (uk$so01) 
** Purpose  : SQL Session & SQL Copy
**
** History:
**
** 02-Jan-2004 (uk$so01)
**    created
** 21-Jun-2004 (uk$so01)
**    SIR #111688, additional change to display ingres
**    error message when sqlca.sqlcode = -33000 and the 
**    GetRow handler has never been called.
** 29-Jun-2004 (uk$so01)
**    SIR #111688, additional change to support -u<username> option
**    at the command line.
** 12-May-2006 (thaju02)
**    In INGRESII_llConnectSession(), determine if database is NFC/NFD
**    and intialize collation table for adf_cb.
*/
/*
** compile:
** esqlc -multi -fsqlcopy.inc sqlcopy.sc
*/
/*
** Declare the SQLCA structure and the SQLDA typedef
*/
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlca.h"
    IISQLCA *IIsqlca();
# define sqlca (*(IIsqlca()))
# include "/home/ingres/SANDBOX/Ingres_SVN/server/install/build/ingres/files/eqsqlda.h"
static int g_nNewSession4Sequence = 0;
static char* g_strSqlError = (char*)0;
static void FormatSQLError (char* pBuffer)
{
}
char*INGRESII_llGetLastSQLError ()
{
	FormatSQLError (g_strSqlError);
	return g_strSqlError;
}
static int nFirstError = 1;
static void SQLError()
{
  char error_buf[1024];
	int nErr = sqlca.sqlcode;
	if (nFirstError == 1)
	{
		nFirstError = 0;
		g_strSqlError = malloc (1024);
	}
	error_buf[0] = '\0';
/* # line 71 "sqlcopy.sc" */	/* inquire_ingres */
  {
    IILQisInqSqlio((short *)0,1,32,1023,error_buf,63);
  }
/* # line 72 "sqlcopy.sc" */	/* host code */
	STcopy (error_buf, g_strSqlError);
}
void INGRESII_llCleanSql()
{
	if (!nFirstError && g_strSqlError)
	{
		nFirstError = 1;
		free (g_strSqlError);
		g_strSqlError = NULL;
	}
	if (g_nNewSession4Sequence > 0)
	{
		INGRESII_llDisconnectSession(g_nNewSession4Sequence, 1);
		g_nNewSession4Sequence = 0;
	}
}
int INGRESII_llSetCopyHandler(PfnIISQLHandler handler)
{
/* # line 93 "sqlcopy.sc" */	/* set_sql */
  {
    IILQshSetHandler(12,handler);
  }
/* # line 94 "sqlcopy.sc" */	/* host code */
	return 1;
}
int INGRESII_llActivateSession (int nSessionNumer)
{
  int nSession = nSessionNumer;
/* # line 105 "sqlcopy.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(nSession),&nSession);
  }
/* # line 106 "sqlcopy.sc" */	/* host code */
	return 1;
}
static int nSessionNumber = 0;
int INGRESII_llConnectSession(char* strFullVnode)
{
  char szSessionDescription[64];
  char *connection = strFullVnode;
  char *o[12];
  int nSession = 1;
  char unic[10];
	o[0] = CMD_GetUFlag(&g_cmd);
	nSessionNumber++;
	nSession = nSessionNumber;
/* # line 124 "sqlcopy.sc" */	/* set connection */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(-97),(void *)IILQint(-97));
  }
/* # line 125 "sqlcopy.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(-97),(void *)IILQint(-97));
  }
/* # line 127 "sqlcopy.sc" */	/* host code */
	if (o[0] && STlength(o[0]) > 0)
/* # line 128 "sqlcopy.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(nSession);
    IIsqConnect(0,connection,o[0],(char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0);
    if (sqlca.sqlcode < 0) 
      goto CONNECT_ERROR;
  }
/* # line 129 "sqlcopy.sc" */	/* host code */
	else
/* # line 130 "sqlcopy.sc" */	/* connect */
  {
    IIsqInit(&sqlca);
    IILQsidSessID(nSession);
    IIsqConnect(0,connection,(char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, (char *)0, 
    (char *)0, (char *)0, (char *)0);
    if (sqlca.sqlcode < 0) 
      goto CONNECT_ERROR;
  }
/* # line 131 "sqlcopy.sc" */	/* host code */
	STcopy ("Ingres Data Loader", szSessionDescription);
/* # line 132 "sqlcopy.sc" */	/* set */
  {
    IIsqInit(&sqlca);
    IIwritio(0,(short *)0,1,32,0,(char *)"set session with DESCRIPTION\
=");
    IIvarstrio((short *)0,1,32,0,szSessionDescription);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto CONNECT_ERROR;
  }
/* # line 133 "sqlcopy.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      goto CONNECT_ERROR;
  }
/* # line 135 "sqlcopy.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)
"select dbmsinfo('UNICODE_NORMALIZATION')");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,9,unic);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto CONNECT_ERROR;
  }
/* # line 136 "sqlcopy.sc" */	/* host code */
        if (STlength(unic) >= 3)
        {
            if (STbcompare( unic, 3, "NFC",3,0) == 0)
            {
              IILQucolinit(1);
            }
            if (STbcompare( unic, 3, "NFD",3,0) == 0)
            {
              IILQucolinit(0);
            }
        }
	return nSession;
CONNECT_ERROR:
	SQLError();
	return 0;
}
int INGRESII_llDisconnectSession(int nSession, int nCommit)
{
  int nSession2Disconnect = nSession;
/* # line 162 "sqlcopy.sc" */	/* host code */
	/*
	** Disconnect the session:
	*/
	if (nSession2Disconnect > 0)
	{
		INGRESII_llActivateSession (nSession);
		if(nCommit == 1)
/* # line 169 "sqlcopy.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
    if (sqlca.sqlcode < 0) 
      goto DISCONNECTSESSION_ERROR;
  }
/* # line 170 "sqlcopy.sc" */	/* disconnect */
  {
    IIsqInit(&sqlca);
    IIsqDisconnect();
    if (sqlca.sqlcode < 0) 
      goto DISCONNECTSESSION_ERROR;
  }
/* # line 171 "sqlcopy.sc" */	/* host code */
	}
/* # line 172 "sqlcopy.sc" */	/* set connection */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(-97),(void *)IILQint(-97));
  }
/* # line 173 "sqlcopy.sc" */	/* set_sql */
  {
    IILQssSetSqlio(11,(short *)0,1,30,sizeof(-97),(void *)IILQint(-97));
  }
/* # line 174 "sqlcopy.sc" */	/* host code */
	return 1;
DISCONNECTSESSION_ERROR:
	SQLError();
	return 0;
}
int INGRESII_llExecuteImmediate (char* strCopyStatement)
{
  char *statement = strCopyStatement;
/* # line 189 "sqlcopy.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(statement);
    IIsyncup((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto EXECUTEIMMEDIATE_LERROR;
  }
/* # line 190 "sqlcopy.sc" */	/* host code */
	return 1;
EXECUTEIMMEDIATE_LERROR:
	SQLError();
	return 0;
}
int INGRESII_llExecuteCopyFromProgram (char* strCopyStatement, long* lAffectRows)
{
  int nErrorCode = 0;
  int nCallbackInvoked = 0;
  char *statement = strCopyStatement;
	if (lAffectRows)
		*lAffectRows = 0;
/* # line 208 "sqlcopy.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqExImmed(statement);
    IIsyncup((char *)0,0);
  }
/* # line 210 "sqlcopy.sc" */	/* host code */
	nCallbackInvoked = g_nGetRecordLineIvoked;
	g_nGetRecordLineIvoked = 0;
	nErrorCode = sqlca.sqlcode;
	switch (nErrorCode)
	{
	case -33000: /* Run-time logical error */
		if (nCallbackInvoked != 1)
		{
			SQLError();
			return 0;
		}
		break;
	default:
		if (nErrorCode < 0)
		{
			SQLError();
			return 0;
		}
		break;
	}
	if (lAffectRows)
		*lAffectRows = (long)sqlca.sqlerrd[2];
	return 1;
}
void INGRESII_llCommitSequence()
{
	if (g_nNewSession4Sequence <= 0)
		return;
	INGRESII_llActivateSession (g_nNewSession4Sequence);
/* # line 241 "sqlcopy.sc" */	/* commit */
  {
    IIsqInit(&sqlca);
    IIxact(3);
  }
/* # line 242 "sqlcopy.sc" */	/* host code */
	INGRESII_llActivateSession (1);
}
int INGRESII_llSequenceNext(char* strSequenceName, char* szNextValue)
{
  char szStatement[128];
  char szSeqValue[128];
  char *strName = strSequenceName;
  int nSess = 0;
	szSeqValue[0]='\0';
	STprintf (szStatement, "SELECT VARCHAR (NEXT VALUE FOR %s)", strSequenceName);
/* # line 258 "sqlcopy.sc" */	/* host code */
	if (g_nNewSession4Sequence == 0)
	{
		g_nNewSession4Sequence = INGRESII_llConnectSession(CMD_GetVnode(&g_cmd));
		if (g_nNewSession4Sequence == 0)
		{
			SQLError();
			return 0;
		}
	}
	INGRESII_llActivateSession (g_nNewSession4Sequence);
/* # line 269 "sqlcopy.sc" */	/* execute */
  {
    IIsqInit(&sqlca);
    IIsqMods(3);
    IIsqMods(1);
    IIsqExImmed(szStatement);
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,127,szSeqValue);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto NEXT_SEQUENCE_LERROR;
  }
/* # line 270 "sqlcopy.sc" */	/* host code */
	TrimRight(szSeqValue);
	STcopy (szSeqValue, szNextValue);
	INGRESII_llActivateSession (1);
	return 1;
NEXT_SEQUENCE_LERROR:
	SQLError();
	return 0;
}
long INGRESII_llGetCPUSession()
{
  char szResult[64];
  long lResult;
/* # line 289 "sqlcopy.sc" */	/* select */
  {
    IIsqInit(&sqlca);
    IIsqMods(1);
    IIwritio(0,(short *)0,1,32,0,(char *)"select DBMSINFO('_cpu_ms')");
    IIretinit((char *)0,0);
    if (IInextget() != 0) {
      IIgetdomio((short *)0,1,32,63,szResult);
    } /* IInextget */
    IIsqFlush((char *)0,0);
    if (sqlca.sqlcode < 0) 
      goto DBMSINFO_LERROR;
  }
/* # line 290 "sqlcopy.sc" */	/* host code */
	lResult = atol(szResult);
	return lResult;
DBMSINFO_LERROR:
	SQLError();
	return -1;
}
