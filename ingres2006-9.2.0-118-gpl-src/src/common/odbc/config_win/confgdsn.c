/*
** Copyright 2007 Ingres Corporation. All Rights Reserved.
*/

/*
** Name: CONFGDSN.C
**
** Description:
**	Define Ingres data sources
**
** History:
**	11-may-1993 (rosda01)
**	    Initial coding
**	27-jul-1993 (rosda01)
**	    Rewrote for "standard"
**	05-jan-1993 (rosda01)
**	    Merged with cnfg.
**	29-nov-1994 (rosda01)
**	    Moved all user visible strings to RC
**	22-oct-1994 (rosda01)
**	    Moved common routines to caidcn32.c.
**	06-mar-1997 (tenje01)
**	    Added check box IDC_PROMPTUIDPWD
**	14-apr-1997 (thoda04)
**	    Added OPINGDT and other serverclasses support
**	    Added error reporting for TestConnection
**	08-may-1997 (tenje01)
**	    Changed IDC_VNODE to a combobox
**	30-may-1997 (thoda04)
**	    Added winhelp support
**	04-jun-1997 (thoda04)
**	    Open vnode files in binary mode
**	18-jun-1997 (thoda04)
**	    Added gateways' WITH options support
**	12-sep-1997 (thoda04)
**	    Change serverclass DBD to STAR
**	30-oct-1997 (tenje01)
**	    fix bug 85857
**	07-nov-1997 (tenje01)
**	    Added support of role name and pwd 
**	01-dec-1997 (tenje01)
**	    Added support of SelectLoop/CursorLoop
**	10-dec-1997 (thoda04)
**	    Treat Role pwd from odbc.ini as character string
**	11-dec-1997 (tenje01)
**	    convert c functions to cl functions
**	15-dec-1997 (thoda04)
**	    Added support for converting 3 part names
**	14-jan-1998 (thoda04)
**	    Added RMS server class  
**	20-may-1998 (Dmitriy Bubis)
**	    Fixed Bug #90880 (ConfigDSN)
**	21-dec-1998 (thoda04)
**	    Added INGDSK, MSSQL, ODBC server classes
**	19-mar-1999 (Bobby Ward)
**	    Added code to fix Bug# 93886 (II_HOSTNAME)	
**	29-mar-1999 (thoda04)
**	    Use SYSTEM_LOCATION_VARIABLE instead of "II_SYSTEM".
**	21-sep-1999 (thoda04)
**	    Added database name browse drop-down list and logo
**	15-nov-1999 (thoda04)
**	    Added Blank Date return NULL support
**	12-jan-2000 (thoda04)
**	    Added Date 1582 returned as NULL support
**	27-apr-2000 (thoda04)
**	    Use CB_FINDSTRINGEXACT for list searches
**	04-may-2000 (thoda04)
**	    Use correct ServerClass for dbname dropdown
**	23-may-2000 (thoda04)
**	    Display user-defined server class on dialog
**	05-jun-2000 (thoda04)
**	    Added numeric_overflow= support
**	06-jun-2000 (thoda04)
**	    Correct role password encryption
**	16-oct-2000 (loera01)
**	    Added support for II_DECIMAL (Bug 102666).
**	17-oct-2000 (thoda04)
**	    Clear and build server capabilities cache
**	    so that driver does not need to keep
**	    asking the same questions with the same
**	    answers.
**	30-oct-2000 (thoda04)
**	    Added CatSchemaNULL support
**	08-mar-2001 (thoda04)
**	    Removed hard-coded DCOM database name
**	27-mar-2001 (weife01)
**	    Fix bug104356. Add check for the existence 
**	    of the registry key before trying to delete it
**	02-may-2001 (thoda04)
**	    CreateDIBPalette returns HPALETTE, not HBITMAP
**	03-may-2001 (thoda04)
**	    Default prompt for UID/PWD to 'N'
**	05-jul-2001 (somsa01)
**	    Define DWL_MSGRESULT to DWLP_MSGRESULT on NT_IA64. Also
**	    corrected 64-bit compiler warnings.
**	06-sep-2001 (thoda04)
**	    Add VANT server class.
**	10-sep-2001 (thoda04)
**	    Change default for "Prompt for UID/PWD".
**	19-nov-2001 (thoda04)
**	    Change GetNextVNodeInLoginFile to use SYSTEM_LOCATION_VARIABLE.
**	01-jan-2002 (thoda04)
**	    Add support for DisableCatUnderscore search char.
**	18-apr-2002 (weife01) Bug #106831
**          Add option in advanced tab of ODBC admin which allows the 
**          procedure update with odbc read only driver 
**      22-may-2002 (loera01) Bug 107814
**          Pull DSN configuration constants out of this file and into
**          caiioc32.h.
**      27-jun-2002 (weife01) Add DB2UDB to the supported server classes.  
**      30-jul-2002 (weife01) SIR #108411: made change to functions SQLGetInfo and
**          SQLGetTypeInfo to support DBMS change to allow the VARCHAR 
**          limit to 32k; added internal function GetSQLMaxRowSize.    
**      14-feb-2003 (loera01) SIR 109643
**          Minimize platform-dependency in code.  Make backward-compatible
**          with earlier Ingres versions.
**      07-mar-2003 (loera01) SIR 109789
**          Add read-only option to data source DSN defintion.  Check
**          ODBCINST_INI to see if a default read-only definition was
**          provided.
**      01-jul-2003 (weife01) SIR #110511
**          Add support for group ID in DSN.
**	02-oct-2003 (somsa01)
**	    Ported to NT_AMD64.
**	11-Jun-2004 (somsa01)
**	    Cleaned up code for Open Source.
**      10-aug-2004 (loera01)
**          Removed obsolete SQLLEN reference.
** 	10-jan-2007 (wonst02) Bug 117741
** 	    Fix so server type 'STAR' displays STAR databases.
**      10-Mar-2006 (Ralph Loen) Bug 115833
**          Added support for new DSN configuration option: "Fill character
**          for failed Unicode/multibyte conversions"
**      20-Sep-2007 (weife01) SIR 119140
**          Added support for new DSN/connection string configuration
**          option:"Convert int8 from DBMS to int4".
*/

#include <compat.h>
#include <windowsx.h>              // More Windows stuff
#include <sql.h>                   // ODBC prototypes 
#include <sqlext.h>
#include <odbcinst.h>              // ODBC Installer definitions
#include "resource.h"              // Resource : MS Devleoper Studio
#include "caiioc32.h"              // Global definitions.
#include <idmseini.h>              // Ini file definitions.
#include <idmsucry.h>

/* CL functions */
#include <nm.h>
#include <st.h> 
#include <cm.h>
#include <er.h>
#include <me.h>
#include <erodbc.h>

#define MAX_VALUE_NAME              128
#define VNODE_SIZE_32               376
#define VNODE_SIZE_16               372
#define VNODE_NAME_OFFSET           44
#define VNODE_UIDPWD_OFFSET         304  /* "userid,encrptpassword */
#define VNODE_VALID_OFFSET          372
#define VNODE_EMPTY_OFFSET          0

#define LEN_GATEWAY_OPTION          255

#define isInIINodeFile              0x01
#define isInIILoginFile             0x02

#if defined(NT_IA64) || defined(NT_AMD64)
#define DWL_MSGRESULT DWLP_MSGRESULT
#endif  /* NT_IA64 || NT_AMD64 */

//
//  Externals:
//
extern HINSTANCE  hInstance;      /* module handle */

//
//  Internal constants:
//
static const char DRIVER[]				= ",DRIVER=";
static const char HELP[]				= ",HELP=";

static const char DSN_CACHE_ServerVersion[]           ="ServerVersion";
static const char DSN_CACHE_ServerNameCase[]          ="ServerNameCase";
static const char DSN_CACHE_ServerDelimitedCase[]     ="ServerDelimitedCase";
static const char DSN_CACHE_ServerMaxColumns[]        ="ServerMaxColumns";
static const char DSN_CACHE_ServerMaxSchemaNameLen[]  ="ServerMaxSchemaNameLen";
static const char DSN_CACHE_ServerMaxTableNameLen[]   ="ServerMaxTableNameLen";
static const char DSN_CACHE_ServerMaxColumnNameLen[]  ="ServerMaxColumnNameLen";
static const char DSN_CACHE_ServerMaxRowLen[]         ="ServerMaxRowLen";
static const char DSN_CACHE_ServerMaxCharColumnLen[]  ="ServerMaxCharColumnLen";
static const char DSN_CACHE_ServerMaxVchrColumnLen[]  ="ServerMaxVchrColumnLen";
static const char DSN_CACHE_ServerMaxByteColumnLen[]  ="ServerMaxByteColumnLen";
static const char DSN_CACHE_ServerMaxVbytColumnLen[]  ="ServerMaxVbytColumnLen";

static       char CacheServerName[LEN_SERVER + 1]     ="";
static       char CacheServerType[LEN_SERVERTYPE + 1] ="";
static       char CacheServerVersion[16]              ="";
static       char CacheServerNameCase[2]              ="";
static       char CacheServerDelimitedCase[2]         ="";
static       char CacheServerMaxColumns[18]           ="";
static       char CacheServerMaxSchemaNameLen[18]     ="";
static       char CacheServerMaxTableNameLen[18]      ="";
static       char CacheServerMaxColumnNameLen[18]     ="";
static       char CacheServerMaxRowLen[18]            ="";
static       char CacheServerMaxCharColumnLen[18]     ="";
static       char CacheServerMaxVchrColumnLen[18]     ="";
static       char CacheServerMaxByteColumnLen[18]     ="";
static       char CacheServerMaxVbytColumnLen[18]     ="";
static LPCTSTR ServerClass[] = KEY_SERVERCLASSES;
/* {"Ingres", "OPINGDT (desktop)", "DCOM (Datacom)", "IDMS", 
    "DB2", "IMS", "VSAM", "RDB (Rdb/VMS)", "STAR", "RMS", 
    "Oracle", "Informix", "Sybase", "ALB (ALLBASE/SQL)", ..., NULL} */

static HBITMAP    hLogo         = NULL;  /* logo bit map  */
static HPALETTE   hPalette      = NULL;  /* palette for logo bit map */
static char *     pDatabaseList = NULL;  /* Database list retrieved from iidatabases */
static char       DatabaseListszServer       [LEN_SERVER + 1]     = "";
static char       DatabaseListDlgszServerType[LEN_SERVERTYPE + 1] = "";
static OFFSET_TYPE        DatabaseListSize = 0;
#define DatabaseListMAX   1000           /* max number of characters in list */

#define VnodeListMAX      1000           /* max number of characters in VNODE list */
static char *     pVnodeList = NULL;     /* VNODE list retrieved from iidatabases */
static int        VnodeListSize[] = {0,VnodeListMAX};

//
//  Internal function prototypes:
//
static void TestConnection (HWND hwnd, char *szDSN, 
                            char *szServer, char *szServerType);
static void BrowseConnectionForDBNames (HWND);
static void PaintLogo(HWND hDlg, int control,
                      LPRECT prect, HDC hdc, HBITMAP hbitmap);
static HBITMAP LoadResourceBitmap(HINSTANCE      hinst, 
                                  LPSTR          lpszName,
                                  HPALETTE * lphPalette);
static HPALETTE CreateDIBPalette(LPBITMAPINFOHEADER lpbi, LPINT lpnumcolors);
BOOL ValidateDataSource (HWND hDlg);
//static void EncryptPWD(char * name, char * pwd);
void EncryptPWD(char * name, char * pwd);
static void DecryptPWD(char * name, char * pwd);
static void GetCapabilitiesFromDSN(char * szDSN);
static void GetCapabilitiesFromServer(HDBC hdbc, 
                                      char * ServerName, char * ServerType);
static char * AddListEntry (char * plist, int plistsize[2], char * pname);
static char * FindListEntry(char * plist, int plistsize[2], char * pname);
static BOOL   VnodeSetsPromptUID(HWND hDlg, char * plist, int plistsize[2]);
static VOID   HideOrShowRole(HWND hDlg, char *szServerType);
static VOID   HideOrShowGroup(HWND hDlg, char *szServerType);
static VOID   HideOrShowProcUpdt(HWND hDlg);
static BOOL isIngresServerType(char *szServerType);

//
//  About box dialog procedure.
//
//  On entry: hdlg   = dialog window handle
//            wMsg   = message id
//            wParam = control id
//            lParam = not used
//
//  On exit:  Nothing.
//
//  Returns:  TRUE
//
BOOL CALLBACK AboutProc(
    HWND    hDlg,
    UINT    wMsg,
    WPARAM  wParam,
    LPARAM  lParam)
{
    switch (wMsg)
    {
    case WM_INITDIALOG:
        
		SetWindowText  (hDlg, ERget(F_OD0119_CONFIG_ABOUT));
		SetDlgItemText (hDlg, IDC_DS_ADMIN,  ERget(F_OD0120_IDC_DS_ADMIN));
//		SetDlgItemText (hDlg, IDC_COPYRIGHT, ERget(F_OD0121_IDC_COPYRIGHT));
		SetDlgItemText (hDlg, IDC_CA,        ERget(F_OD0122_IDC_CA));

        SetDlgItemText (hDlg, IDC_PRODUCT, szProduct);
        SetDlgItemText (hDlg, IDC_VERSION, szProductVersion);
        return (TRUE);

    case WM_COMMAND:

        switch (wParam)
        {
        case IDOK:

            EndDialog (hDlg, TRUE);
            return (TRUE);
        }
    }
    return (FALSE);
}
/*--------------------------------------------------------------*/
BOOL FixComputerName(char * szPath)
{
char * lpChar = szPath;
char * lpSave = NULL; 

	if (lpChar)
	{
		while(*lpChar != EOS)       /* find the last occurrence of back slash */
		{
			if (! CMcmpcase(lpChar,"\\") )
				lpSave = lpChar;
			CMnext(lpChar);
		}
		if (lpSave)
        {
			lpChar = lpSave;
			while(*lpChar != EOS)   /* replace space by underscore */       
		    {
				if (CMspace(lpChar))
					CMcpychar("_",lpChar); 
				CMnext(lpChar);
			}
        }
	}
	return(TRUE);
}
/*--------------------------------------------------------------*/
BOOL GetNextVNodeInNodeFile(FILE * * pFileNode, char * pNodeName, char * szNodeUser)
{ 
char * lpIngDir;
char szPath[_MAX_PATH] = "C:";
char * HostName;
FILE * fileNode; 
#ifdef WIN32
char   buff[VNODE_SIZE_32]; // size of each vnode is 376 in Ingres  	   
char   szComputerName[MAX_COMPUTERNAME_LENGTH+1];
DWORD  dwSize=MAX_COMPUTERNAME_LENGTH+1;
#else
char   buff[VNODE_SIZE_16]; // size of each vnode is 128 in Ingres
#endif

     MEfill(MAX_OBJECT_NAME,0,pNodeName); 
     if (szNodeUser)
        *szNodeUser = '\0';

	 fileNode = *pFileNode; 

     if (fileNode == NULL)    // the first time
     {
         NMgtAt(SYSTEM_LOCATION_VARIABLE,&lpIngDir);  /* usually II_SYSTEM */
         if (lpIngDir != NULL)
             strcpy(szPath,lpIngDir);
         strcat(szPath,"\\");
         strcat(szPath,SYSTEM_LOCATION_SUBDIRECTORY);  /* usually "ingres"  */
#ifdef WIN32
         /* 
            II_HOSTNAME will be checked 1st and if there is not		
            one then it will default to the ComputerName
         */
		   	
		 
         strcat(szPath,"\\files\\name\\IINODE_");
		 NMgtAt(SYSTEM_LOCATION_HOSTNAME, &HostName); /* usually II_HOSTNAME */
         if (HostName  &&  *HostName)
         {
	     strcpy(szComputerName,HostName);
         }

         else
	     if (!GetComputerName(szComputerName,&dwSize))
			 return(FALSE);
	     STcat(szPath,szComputerName);
#else 
	     strcat(szPath,"\\files\\name\\iinode");   
#endif
         fileNode = fopen(szPath,"rb");

#ifdef WIN32 
		 if (fileNode == NULL)
		 {
			 /* 
			 if computer name contains space, Ingres 2.0 changes the
			 space to '_'. Prior to 2.0, it would just leave the space 
			 alone. So, if open fails, we will replace space by '_', then 
			 try again. 
			 */
			 FixComputerName(szPath);
			 fileNode = fopen(szPath,"rb");
		 }
#endif
		 *pFileNode = fileNode;
     }

     if (fileNode == NULL)
         return(FALSE); 

     fread(buff,sizeof(char),sizeof(buff),fileNode); 
     if (ferror(fileNode))
	 {
         fclose(fileNode);
         return(FALSE);       
	 }

     if (feof(fileNode))
     {
         fclose(fileNode);
         return(FALSE);
     }
#ifdef WIN32
     if ((buff[VNODE_VALID_OFFSET] == EOS) &&  // vnode is valid, not destroyed
         (buff[VNODE_EMPTY_OFFSET] == EOS))    // this record is valid, not an empty slot.
     {
          STcopy(&buff[VNODE_NAME_OFFSET],pNodeName);
          if (szNodeUser)
              strcpy(szNodeUser,&buff[VNODE_UIDPWD_OFFSET]);
     }
#else 
     STcopy(&buff[VNODE_NAME_OFFSET],pNodeName);
     if (szNodeUser)
         strcpy(szNodeUser,&buff[VNODE_UIDPWD_OFFSET]);
#endif

     return(TRUE);
}
/*--------------------------------------------------------------*/
BOOL GetNextVNodeInLoginFile(FILE * * pFileNode, char * pNodeName, char * szNodeUser)
{ 
char * lpIngDir;
char szPath[_MAX_PATH] = "C:";
char * HostName;	
FILE * fileNode; 
#ifdef WIN32
char   buff[VNODE_SIZE_32]; // size of each vnode is 376 in Ingres  	   
char   szComputerName[MAX_COMPUTERNAME_LENGTH+1];
DWORD  dwSize=MAX_COMPUTERNAME_LENGTH+1;
#else
char   buff[VNODE_SIZE_16]; // size of each vnode is 128 in Ingres
#endif
  
     MEfill(MAX_OBJECT_NAME,0,pNodeName); 
     if (szNodeUser)
        *szNodeUser = '\0';

	 fileNode = *pFileNode; 

     if (fileNode == NULL)    // the first time
     {
         NMgtAt(SYSTEM_LOCATION_VARIABLE,&lpIngDir);  /* usually II_SYSTEM */
         if (lpIngDir != NULL)
             strcpy(szPath,lpIngDir);
         strcat(szPath,"\\");
         strcat(szPath,SYSTEM_LOCATION_SUBDIRECTORY);  /* usually "ingres"  */
#ifdef WIN32
         STcat(szPath,"\\files\\name\\IILOGIN_");
         /* 
            II_HOSTNAME will be checked 1st and if there is not		
            one then it will default to the ComputerName
         */
		 NMgtAt(SYSTEM_LOCATION_HOSTNAME, &HostName); /* usually II_HOSTNAME */
         if (HostName  &&  *HostName)
         {
	     strcpy(szComputerName,HostName);
         }

         else
	     if (!GetComputerName(szComputerName,&dwSize))
			 return(FALSE);
	     STcat(szPath,szComputerName);
#else 
	     STcat(szPath,"\\files\\name\\iilogin");   
#endif
         fileNode = fopen(szPath,"rb");

#ifdef WIN32 
		 if (fileNode == NULL)
		 {
			 /* 
			 if computer name contains space, Ingres 2.0 changes the
			 space to '_'. Prior to 2.0, it would just leave the space 
			 alone. So, if open fails, we will replace space by '_', then 
			 try again. 
			 */
			 FixComputerName(szPath);
			 fileNode = fopen(szPath,"rb");
		 }
#endif
		 *pFileNode = fileNode;
     }

     if (fileNode == NULL)
         return(FALSE); 

     fread(buff,sizeof(char),sizeof(buff),fileNode); 
     if (ferror(fileNode))
	 {
         fclose(fileNode);
         return(FALSE);       
	 }

     if (feof(fileNode))
     {
         fclose(fileNode);
         return(FALSE);
     }
#ifdef WIN32
     if ((buff[VNODE_VALID_OFFSET] == EOS) &&  // vnode is valid, not destroyed
         (buff[VNODE_EMPTY_OFFSET] == EOS))    // this record is valid, not an empty slot.
     {
          STcopy(&buff[VNODE_NAME_OFFSET],pNodeName);
          if (szNodeUser)
              strcpy(szNodeUser,&buff[VNODE_UIDPWD_OFFSET]);
     }
#else
     STcopy(&buff[VNODE_NAME_OFFSET],pNodeName);
     if (szNodeUser)
         strcpy(szNodeUser,&buff[VNODE_UIDPWD_OFFSET]);
#endif
     return(TRUE);
}
/*-----------------------------------------------------------------*/ 
//
// DataSourceProc
//
// The DataSource PropertySheet executes this function.
//
BOOL CALLBACK DataSourceProc(
    HWND    hDlg,
    UINT    wMsg,
    WPARAM  wParam,
    LPARAM  lParam)
{
    // LRESULT rc;
    LPSTR   lpsz;
    int     wmId, wmEvent;
    LRESULT CurSel;
    int     bitmapid;
    UINT    cb_findstringxxx;
    HWND    hwndParent = GetParent (hDlg);
	LPCTSTR *lpTmp;
	BOOL    bRC = TRUE;
	UCHAR   szNodeName[MAX_OBJECT_NAME];
	char    szNodeUser[MAX_OBJECT_NAME*2];
	FILE *  fileNode = NULL;
	char	szPromptTemp[LEN_PROMPT + 1];
	char *  pVnode;

    SQLGetPrivateProfileString(
        szDriver, DSN_OPTIONS_DRIVER_READONLY, "", szPromptTemp, LEN_PROMPT + 1,
        ODBCINST_INI);
    if (!CMcmpnocase(&szPromptTemp[0],"Y")) 
        STcopy(szPromptTemp,szDriverReadOnly);
    else
        STcopy("N",szDriverReadOnly);
    *szPromptTemp = '\0';

    switch (wMsg)
    {
    case WM_INITDIALOG:

		SetWindowText  (hDlg, ERget(F_OD0138_CONFIG_DATASOURCE));
		SetDlgItemText (hDlg, IDC_DS, ERget(F_OD0139_IDC_DS));
		SetDlgItemText (hDlg, IDC_DESCRIPTION_TEXT, ERget(F_OD0140_IDC_DESCRIPTION_TEXT));
		SetDlgItemText (hDlg, IDC_SERVER, ERget(F_OD0141_IDC_SERVER));
		SetDlgItemText (hDlg, IDC_VNODE_TEXT, ERget(F_OD0142_IDC_VNODE_TEXT));
		SetDlgItemText (hDlg, IDC_DATABASE, ERget(F_OD0143_IDC_DATABASE));
		SetDlgItemText (hDlg, IDC_TYPE, ERget(F_OD0144_IDC_TYPE));
		SetDlgItemText (hDlg, IDC_CONN_OPTS, ERget(F_OD0145_IDC_CONN_OPTS));
		SetDlgItemText (hDlg, IDC_PROMPTUIDPWD, ERget(F_OD0146_IDC_PROMPTUIDPWD));
		SetDlgItemText (hDlg, IDC_GATEWAY_OPTION_TEXT, ERget(F_OD0147_IDC_GATEWAY_OPTION_TE));
		SetDlgItemText (hDlg, IDC_ROLENAME_TEXT, ERget(F_OD0148_IDC_ROLENAME_TEXT));
		SetDlgItemText (hDlg, IDC_ROLEPWD_TEXT, ERget(F_OD0149_IDC_ROLEPWD_TEXT));
		SetDlgItemText (hDlg, IDC_TEST, ERget(F_OD0150_IDC_TEST));

		EnableWindow (GetDlgItem (hDlg, IDC_NAME), (fRequest == ODBC_ADD_DSN));
		SendDlgItemMessage (hDlg, IDC_NAME,        EM_LIMITTEXT, LEN_DSN,    0L);
        SendDlgItemMessage (hDlg, IDC_DESCRIPTION, EM_LIMITTEXT, LEN_DESCRIPTION,  0L);
        SendDlgItemMessage (hDlg, IDC_VNODE,       EM_LIMITTEXT, LEN_SERVER, 0L);
		SendDlgItemMessage (hDlg, IDC_DBNAME,      EM_LIMITTEXT, LEN_DBNAME, 0L);
		SendDlgItemMessage (hDlg, IDC_SERVERTYPE,  EM_LIMITTEXT, LEN_SERVERTYPE, 0L);
		SendDlgItemMessage (hDlg, IDC_GATEWAY_OPTION,EM_LIMITTEXT, LEN_GATEWAY_OPTION,0L);
		SendDlgItemMessage (hDlg, IDC_ROLENAME, EM_LIMITTEXT, LEN_ROLENAME, 0L);
		SendDlgItemMessage (hDlg, IDC_ROLEPWD,  EM_LIMITTEXT, LEN_ROLEPWD,  0L);
        SendDlgItemMessage (hDlg, IDC_GROUP, EM_LIMITTEXT, LEN_GROUP, 0L);

		STcopy("N",szPrompt);  

        if (*szDSN)   /* if not an ADD operation */
        {
			char szDefaultDesc[LEN_DESCRIPTION+1];
			STcopy(szDescription, szDefaultDesc );
			if (!*szDescription)
				SQLGetPrivateProfileString(
					szDSN, DSN_OPTIONS_DESCRIPTION,
					szDefaultDesc, szDescription, LEN_DESCRIPTION + 1, ODBC_INI);
			if (!*szServer)
				SQLGetPrivateProfileString(
					szDSN, DSN_OPTIONS_SERVER,
					"", szServer, LEN_SERVER + 1, ODBC_INI);
			if (!*szDBName)
				SQLGetPrivateProfileString(
					szDSN, DSN_OPTIONS_DBNAME,
					"", szDBName, LEN_DBNAME + 1, ODBC_INI);
			if (!*szServerType)
				SQLGetPrivateProfileString(
					szDSN, DSN_OPTIONS_SERVERTYPE,
					SERVER_TYPE_DEFAULT, szServerType, LEN_SERVERTYPE + 1, 
					                              ODBC_INI);
			
			SQLGetPrivateProfileString(
				szDSN, DSN_OPTIONS_PROMPT,
				"", szPromptTemp, LEN_PROMPT + 1, ODBC_INI);
			if (*szPromptTemp == 'Y'  ||  *szPromptTemp == 'N')
				STcopy(szPromptTemp,szPrompt);
		        *szPromptTemp = '\0';	
			SQLGetPrivateProfileString(
				szDSN, DSN_OPTIONS_READONLY,
				"", szPromptTemp, LEN_PROMPT + 1, ODBC_INI);
                        if (!CMcmpnocase(&szPromptTemp[0],"Y") )
				STcopy(szPromptTemp,szReadOnly);
                        else
                                STcopy("N",szReadOnly);
			if(!*szWithOption)
				SQLGetPrivateProfileString(
					szDSN, DSN_OPTIONS_WITHOPTION,
					"", szWithOption, LEN_GATEWAY_OPTION + 1, ODBC_INI);
			if (!*szRoleName)
				SQLGetPrivateProfileString(
					szDSN, DSN_OPTIONS_ROLENAME,
					"", szRoleName, LEN_ROLENAME + 1, ODBC_INI);
			if (!*szRolePWD)
				SQLGetPrivateProfileString(
					szDSN, DSN_OPTIONS_ROLEPWD,
					"", szRolePWD, LEN_ROLEPWD + 1, ODBC_INI);
			if (STlength(szRolePWD) > 0)
				DecryptPWD(szRoleName, szRolePWD); /* decrypts the pw */
            if (!*szGroup)
                SQLGetPrivateProfileString(
                    szDSN, DSN_OPTIONS_GROUP,
                    "", szGroup, LEN_GROUP + 1, ODBC_INI);
			if(!*szCatUnderscoreDisabled)
				SQLGetPrivateProfileString(
					szDSN, DSN_OPTIONS_CATUNDERSCORE,
					"", szCatUnderscoreDisabled, sizeof(szCatUnderscoreDisabled)-1, ODBC_INI);
			if(!*szAllowProcedureUpdate)
				SQLGetPrivateProfileString(
					szDSN, DSN_OPTIONS_ALLOWPROCEDUREUPDATE,
					"", szAllowProcedureUpdate, sizeof(szAllowProcedureUpdate)-1, ODBC_INI);

                        if(!*szMultibyteFillChar)
                                SQLGetPrivateProfileString(
                                        szDSN, DSN_OPTIONS_MULTIBYTEFILLCHAR,
                                        "", szMultibyteFillChar, sizeof(szMultibyteFillChar)-1, ODBC_INI);
			if(!*szConvertInt8ToInt4)
				SQLGetPrivateProfileString(
					szDSN, DSN_OPTIONS_CONVERTINT8TOINT4,
					"", szConvertInt8ToInt4, sizeof(szConvertInt8ToInt4)-1, ODBC_INI);

			GetCapabilitiesFromDSN(szDSN);
        }
        else
			if (!*szServerType)
				strcpy(szServerType, SERVER_TYPE_DEFAULT); 
             /* for new DSN, default ServerType="INGRES" */

        //
        //  Note: for some reason, setting the text in an edit control
        //  cause the EN_CHANGED message to be sent to the control if
        //  done in the PSN_SETACTIVE case, so they must be done here
        //  or flags are needed to keep the APPLY button disabled when
        //  the page is first displayed.  See server sheet...
        //
        SetDlgItemText (hDlg, IDC_NAME, szDSN);
        SetDlgItemText (hDlg, IDC_DESCRIPTION, szDescription);
        SetDlgItemText (hDlg, IDC_DBNAME, szDBName);
        SetDlgItemText (hDlg, IDC_GATEWAY_OPTION, szWithOption);
        SetDlgItemText (hDlg, IDC_ROLENAME, szRoleName);
        SetDlgItemText (hDlg, IDC_ROLEPWD,  szRolePWD);
        SetDlgItemText (hDlg, IDC_GROUP, szGroup);

        if (pVnodeList == NULL)
            pVnodeList=GlobalAllocPtr(GMEM_MOVEABLE | GMEM_ZEROINIT, VnodeListMAX+64);

		/* fill the vnode combo box */
        SendDlgItemMessage(hDlg, IDC_VNODE, CB_ADDSTRING, (WPARAM) 0, (LPARAM) "(LOCAL)"); 
        pVnode = AddListEntry(pVnodeList, VnodeListSize, "(LOCAL)");
        if (pVnode)
           *pVnode |= isInIILoginFile;  /* use flag byte to mark Ingres "(LOCAL)" vnode
                as (psuedo) in iilogin file to induce IDC_PROMPTUIDPWD unchecked*/

		/* get vnode names from iinode file */
		while (bRC)
		{
			bRC = GetNextVNodeInNodeFile(&fileNode, szNodeName, NULL);
			if (bRC)
			{
				if (szNodeName[0] != 0)
				{
		            pVnode = AddListEntry(pVnodeList, VnodeListSize, szNodeName);
		            if (pVnode)
		               *pVnode |= isInIINodeFile;
		                  /* use flag byte to mark vnode as in iinode file */

		            CurSel = SendDlgItemMessage(hDlg, IDC_VNODE, CB_FINDSTRINGEXACT,
			                 (WPARAM) -1, (LPARAM) (LPCSTR) szNodeName);
					if (CurSel == CB_ERR)
			            SendDlgItemMessage(hDlg, IDC_VNODE, CB_ADDSTRING, (WPARAM) 0, (LPARAM) szNodeName); 
				}
			}
		}
		/* get vnode names from iilogin file */
		bRC = TRUE;
		fileNode = NULL;
		while (bRC)
		{
			bRC = GetNextVNodeInLoginFile(&fileNode, szNodeName, szNodeUser);
			if (bRC)
			{
				if (szNodeName[0] != EOS)
				{
		            char * pVnode = FindListEntry(pVnodeList, VnodeListSize, szNodeName);
		            if (pVnode == NULL)  /* if not in list yet, add it now */
		                pVnode = AddListEntry(pVnodeList, VnodeListSize, szNodeName);
		            if (pVnode  &&  *szNodeUser  &&  *szNodeUser != ',')
		                                            /* if node has username/pwd */
		               *pVnode |= isInIILoginFile;  /* use flag byte to mark vnode as in iilogin file */

		            CurSel = SendDlgItemMessage(hDlg, IDC_VNODE, CB_FINDSTRINGEXACT,
			                 (WPARAM) -1, (LPARAM) (LPCSTR) szNodeName);
					if (CurSel == CB_ERR)
			            SendDlgItemMessage(hDlg, IDC_VNODE, CB_ADDSTRING, (WPARAM) 0, (LPARAM) szNodeName); 
				}
			}
		}
		CurSel = SendDlgItemMessage(hDlg, IDC_VNODE, CB_FINDSTRINGEXACT,
			(WPARAM) -1, (LPARAM) (LPCSTR) szServer);
		if (CurSel == CB_ERR)
		    CurSel = 0;        /* default to first vnode in list */
		if (CurSel != CB_ERR)
			SendDlgItemMessage(hDlg, IDC_VNODE, CB_SETCURSEL, (WPARAM) CurSel, (LPARAM) 0);

		/* fill the server type combo box */
		lpTmp = ServerClass;
		while (*lpTmp)
		{
			SendDlgItemMessage(hDlg, IDC_SERVERTYPE, CB_ADDSTRING, (WPARAM) 0, (LPARAM) *lpTmp); 
			lpTmp++;
		}
		if (_stricmp(szServerType, "ALB")     == 0  ||  /* if one of the server */
		    _stricmp(szServerType, "DCOM")    == 0  ||  /* in the list with     */
		    _stricmp(szServerType, "INGDSK")  == 0  ||  /* a comment following  */
		    _stricmp(szServerType, "OPINGDT") == 0  ||  /* then search using    */
		    _stricmp(szServerType, "MSSQL")   == 0  ||  /* prefix as a special  */
		    _stricmp(szServerType, "RDB")     == 0  ||  /* case.                */
		    _stricmp(szServerType, "VANT")    == 0   )
		       cb_findstringxxx = CB_FINDSTRING;       /* search using prefix */
		else   cb_findstringxxx = CB_FINDSTRINGEXACT;  /* search using exact  */
		CurSel = SendDlgItemMessage(hDlg, IDC_SERVERTYPE, cb_findstringxxx,
		       (WPARAM) -1, (LPARAM) (LPCSTR) szServerType);   /* search  list*/
		if (CurSel == CB_ERR)  /* must be a user defined class like INGBATCH */
		    CurSel = SendDlgItemMessage(hDlg, IDC_SERVERTYPE, CB_ADDSTRING,
		       (WPARAM) 0, (LPARAM) szServerType);  /* add user defined class */
		SendDlgItemMessage(hDlg, IDC_SERVERTYPE, CB_SETCURSEL,
		       (WPARAM) CurSel, (LPARAM) 0);        /* select the item in list*/

		if (!CMcmpcase(&szPrompt[0],"N")) 
			CheckDlgButton(hDlg, IDC_PROMPTUIDPWD, BST_UNCHECKED);
		else
		if (szPrompt[0] == 'Y') 
		    CheckDlgButton(hDlg, IDC_PROMPTUIDPWD, BST_CHECKED);
		else
		    VnodeSetsPromptUID(hDlg, pVnodeList, VnodeListSize);

		if (!CMcmpnocase(&szReadOnly[0],"Y") || 
                    !CMcmpnocase(&szDriverReadOnly[0],"Y")) 
		    CheckDlgButton(hDlg, IDC_READ_ONLY, BST_CHECKED);
		else
		    CheckDlgButton(hDlg, IDC_READ_ONLY, BST_UNCHECKED);
                if (!CMcmpnocase(&szDriverReadOnly[0],"Y"))
                    EnableWindow(GetDlgItem(hDlg,IDC_READ_ONLY),FALSE);
                    
        HideOrShowRole(hDlg, szServerType);
           /* hide or show rolename control based on server class */
        HideOrShowGroup(hDlg, szServerType);

        SetWindowLong (hDlg, DWL_MSGRESULT, TRUE);
		EnableWindow (GetDlgItem (hDlg, IDC_TEST), (fRequest == ODBC_CONFIG_DSN));

        bitmapid = IDB_INGRES;
        hLogo = LoadResourceBitmap(hInstance, MAKEINTRESOURCE(bitmapid),&hPalette);
        return TRUE;

    case WM_DESTROY:
        { 
          if (hLogo)       /* delete the logo bitmap if loaded */
             {DeleteObject(hLogo);
              hLogo = NULL;
             }
          if (hPalette)    /* delete the logo bitmap palette */
             {DeleteObject(hPalette);
              hPalette = NULL;
             }
          return TRUE;   /* let processing of the message continue */
        }

    case WM_DRAWITEM:
        { 
          LPDRAWITEMSTRUCT pdis  = (LPDRAWITEMSTRUCT)lParam;
          UINT             idctl = (UINT) wParam;

          if (idctl == IDC_BITMAPLOCATION  && 
              pdis->itemAction == ODA_DRAWENTIRE)
             {
              PaintLogo(hDlg, IDC_BITMAPLOCATION, &pdis->rcItem, 
                                                   pdis->hDC, hLogo);
              return TRUE;
             }
          break;
        }

    case WM_NOTIFY:

        switch (((NMHDR  *) lParam)->code)
        {
        case PSN_SETACTIVE:

           // CreateServerList (hDlg);        // set server text after this
           // SetDlgItemText (hDlg, IDC_SERVER, szServer);
            return FALSE;

        case PSN_KILLACTIVE:

            SetWindowLong (hDlg, DWL_MSGRESULT, ValidateDataSource (hDlg));
            return TRUE;

        case PSN_APPLY:
            //
            //  If adding data source, call ODBC installer to do it:
            //
            if (fRequest == ODBC_ADD_DSN)
                if (!SQLWriteDSNToIni (szDSN, szDriver))
                    return TRUE;

            //
            //  Add our info to ODBC.INI section of registry:
            //
            if ((lpsz = STindex(szServerType," ",0)) != NULL)
               CMcpychar("\0",lpsz);      /* strip any doc from servertype */

			EnableWindow (GetDlgItem (hDlg, IDC_TEST), TRUE);
			SQLWritePrivateProfileString(szDSN, DSN_OPTIONS_DESCRIPTION, szDescription, ODBC_INI);
			SQLWritePrivateProfileString(szDSN, DSN_OPTIONS_SERVER, szServer, ODBC_INI);
			SQLWritePrivateProfileString(szDSN, DSN_OPTIONS_DBNAME, szDBName, ODBC_INI);
			SQLWritePrivateProfileString(szDSN, DSN_OPTIONS_SERVERTYPE, szServerType, ODBC_INI);
			SQLWritePrivateProfileString(szDSN, DSN_OPTIONS_PROMPT, szPrompt, ODBC_INI);
			SQLWritePrivateProfileString(szDSN, DSN_OPTIONS_PROMPT, szPrompt, ODBC_INI);
			SQLWritePrivateProfileString(szDSN, DSN_OPTIONS_READONLY, szReadOnly, ODBC_INI);
			SQLWritePrivateProfileString(szDSN, DSN_OPTIONS_ROLENAME, szRoleName, ODBC_INI);
			if (STlength(szRolePWD) > 0)
				EncryptPWD(szRoleName, szRolePWD); /* encrypts the pw */
			SQLWritePrivateProfileString(szDSN, DSN_OPTIONS_ROLEPWD,  szRolePWD, ODBC_INI);
                        SQLWritePrivateProfileString(szDSN, DSN_OPTIONS_GROUP, szGroup, ODBC_INI);
			if(!*szCatUnderscoreDisabled)
			    SQLGetPrivateProfileString(szDSN, DSN_OPTIONS_CATUNDERSCORE,"", 
			            szCatUnderscoreDisabled, sizeof(szCatUnderscoreDisabled)-1, ODBC_INI);
			if(!*szCatUnderscoreDisabled)  /* if still not set, set default */
			    if (isIngresServerType(szServerType))  /* based on server type */
			          *szCatUnderscoreDisabled = 'N';
			    else  *szCatUnderscoreDisabled = 'Y';
			SQLWritePrivateProfileString(szDSN, DSN_OPTIONS_CATUNDERSCORE,szCatUnderscoreDisabled,
                                                                                        ODBC_INI);

			if(!*szAllowProcedureUpdate)
			    SQLGetPrivateProfileString(szDSN, DSN_OPTIONS_ALLOWPROCEDUREUPDATE,"", 
			            szAllowProcedureUpdate, sizeof(szAllowProcedureUpdate)-1, ODBC_INI);
			if(!*szAllowProcedureUpdate)  /* if still not set, set default */
			    *szAllowProcedureUpdate = 'N';
			SQLWritePrivateProfileString(szDSN, DSN_OPTIONS_ALLOWPROCEDUREUPDATE,szAllowProcedureUpdate,
                                                                                        ODBC_INI);
                        if(!*szMultibyteFillChar)
                            SQLGetPrivateProfileString(szDSN, DSN_OPTIONS_MULTIBYTEFILLCHAR,"",
                                    szMultibyteFillChar, sizeof(szMultibyteFillChar)-1, ODBC_INI);
                        if(!*szMultibyteFillChar)  /* if still not set, set default */
                            *szMultibyteFillChar = ' ';
                        SQLWritePrivateProfileString(szDSN, DSN_OPTIONS_MULTIBYTEFILLCHAR, szMultibyteFillChar, 
                  ODBC_INI);
			if(!*szConvertInt8ToInt4)
			    SQLGetPrivateProfileString(szDSN, DSN_OPTIONS_CONVERTINT8TOINT4,"", 
			            szAllowProcedureUpdate, sizeof(szConvertInt8ToInt4)-1, ODBC_INI);
			if(!*szConvertInt8ToInt4)  /* if still not set, set default */
			    *szConvertInt8ToInt4 = 'N';
			SQLWritePrivateProfileString(szDSN, DSN_OPTIONS_CONVERTINT8TOINT4,szConvertInt8ToInt4, ODBC_INI);

			WriteDSNCacheCapabilities(szDSN, szServer, szServerType);
            return TRUE;

        case PSN_HELP:

            WinHelp (hDlg, szHelp, HELP_CONTENTS, (DWORD)0);

        case PSN_RESET:

            return TRUE;
        }
        break;

    case WM_COMMAND:

        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case IDC_NAME:

            switch (wmEvent)
            {
            case CBN_EDITCHANGE:
				EnableWindow (GetDlgItem (hDlg, IDC_TEST), FALSE);
                PropSheet_Changed (hwndParent, hDlg);
                return FALSE;
            }
            break;

        case IDC_VNODE:
			switch (wmEvent)
			{
			case CBN_SELCHANGE:
				EnableWindow (GetDlgItem (hDlg, IDC_TEST), FALSE);
				PropSheet_Changed(hwndParent, hDlg);
				*szPrompt = '\0';
				VnodeSetsPromptUID(hDlg, pVnodeList, VnodeListSize);
				return FALSE;
			}
			break;

		case IDC_DBNAME :
			switch (wmEvent)
			{
			case CBN_SELCHANGE:
			case CBN_EDITCHANGE:
				EnableWindow (GetDlgItem (hDlg, IDC_TEST), FALSE);
				PropSheet_Changed(hwndParent, hDlg);
				return FALSE;
			case CBN_DROPDOWN:
				BrowseConnectionForDBNames (hDlg);
				return FALSE;
			}
			break;

		case IDC_ROLENAME:
		case IDC_ROLEPWD:
        case IDC_GROUP:
			switch (wmEvent)
			{
			case EN_CHANGE :
				EnableWindow (GetDlgItem (hDlg, IDC_TEST), FALSE);
				PropSheet_Changed (hwndParent, hDlg);
                return FALSE;
			}
			break;

        case IDC_DESCRIPTION:

            switch (wmEvent)
            {
            case EN_CHANGE:
                PropSheet_Changed(hwndParent, hDlg);
                return FALSE;
            }
            break;

		case IDC_SERVERTYPE :

			switch (wmEvent)
			{
			char szServerType[LEN_SERVERTYPE + 1] = "";

			case CBN_SELCHANGE:   /* user has made his selection */
			case CBN_EDITCHANGE:  /* user has altered text */
				if (wmEvent == CBN_SELCHANGE)
				   {LONG  CurSel;
				    CurSel = SendDlgItemMessage(hDlg, IDC_SERVERTYPE,
				                CB_GETCURSEL, (WPARAM) 0, (LPARAM) 0);
				    if (CurSel != CB_ERR)
				             SendDlgItemMessage(hDlg, IDC_SERVERTYPE,
				                CB_GETLBTEXT, (WPARAM) CurSel, (LPARAM) szServerType);
				   }
				else /* wmEvent == CBN_EDITCHANGE */
				    GetDlgItemText (hDlg, IDC_SERVERTYPE, szServerType, LEN_SERVERTYPE + 1);
				HideOrShowRole(hDlg, szServerType);
                HideOrShowGroup(hDlg, szServerType);

				EnableWindow (GetDlgItem (hDlg, IDC_TEST), FALSE);
				PropSheet_Changed(hwndParent, hDlg);
				return FALSE;
			}
			break;

		case IDC_PROMPTUIDPWD :

			switch (wmEvent)
			{
			case BN_CLICKED:
				EnableWindow (GetDlgItem(hDlg, IDC_TEST), FALSE);
				PropSheet_Changed(hwndParent, hDlg);
				return FALSE;
			}
			break;

	case IDC_READ_ONLY :

	    switch (wmEvent)
	    {
	        case BN_CLICKED:
	        EnableWindow (GetDlgItem(hDlg, IDC_TEST), FALSE);
	        PropSheet_Changed(hwndParent, hDlg);
	        return FALSE;
	    }
	    break;

        case IDC_GATEWAY_OPTION:

            switch (wmEvent)
            {
            case EN_CHANGE:
                PropSheet_Changed(hwndParent, hDlg);
                return FALSE;
            }
            break;

        case IDC_TEST:

            TestConnection (hDlg, szDSN, szServer, szServerType);
            return (TRUE);

        }
        break;
    }
    return FALSE;
}

/*-----------------------------------------------------------------*/ 
//
// AdvancedProc
//
// The Advanced PropertySheet executes this function.
//
BOOL CALLBACK AdvancedProc(
    HWND    hDlg,
    UINT    wMsg,
    WPARAM  wParam,
    LPARAM  lParam)
{
    int     wmId, wmEvent;
    HWND    hwndParent = GetParent (hDlg);
    BOOL    bRC = TRUE;

    switch (wMsg)
    {
    case WM_INITDIALOG:

		SetWindowText  (hDlg, ERget(F_OD0134_CONFIG_ADVANCED));
		SetDlgItemText (hDlg, IDC_SELECT_CURSOR, ERget(F_OD0135_IDC_SELECT_CURSOR));
		SetDlgItemText (hDlg, IDC_SELECT_LOOPS,  ERget(F_OD0136_IDC_SELECT_LOOPS));
		SetDlgItemText (hDlg, IDC_CURSORS,       ERget(F_OD0137_IDC_CURSORS));

		SQLGetPrivateProfileString(szDSN, DSN_OPTIONS_SELECTLOOPS,"Y", szSelectLoops, LEN_SELECTLOOPS + 1, ODBC_INI);
        if (!CMcmpcase(&szSelectLoops[0],"Y") )
			CheckDlgButton(hDlg, IDC_SELECT_LOOPS, BST_CHECKED);
		else
			CheckDlgButton(hDlg, IDC_CURSORS, BST_CHECKED);

		SQLGetPrivateProfileString(szDSN, DSN_OPTIONS_3_PART_NAMES,"N", sz3PartNames, 6, ODBC_INI);
        sz3PartNames[1]='\0';          /* bulletproof length of string */
        if (sz3PartNames[0] == 'Y')    /* Convert 3 part names? */
			CheckDlgButton(hDlg, IDC_3_PART_NAMES, BST_CHECKED);

		SQLGetPrivateProfileString(szDSN, DSN_OPTIONS_SYSTABLES,"N", szSYSTABLES, 6, ODBC_INI);
        szSYSTABLES[1]='\0';          /* bulletproof length of string */
        if (szSYSTABLES[0] == 'Y')    /* Use System Tables? */
			CheckDlgButton(hDlg, IDC_SYSTABLES, BST_CHECKED);

        if(!*szBlankDate)
            SQLGetPrivateProfileString(szDSN, DSN_OPTIONS_BLANKDATE,"", 
                                   szBlankDate, sizeof(szBlankDate)-1, ODBC_INI);
        if (memcmp(szBlankDate, "NULL", 4)==0) /* "Return blank dates as NULL" */
            CheckDlgButton(hDlg, IDC_BLANKDATE, BST_CHECKED);

        if(!*szDate1582)
            SQLGetPrivateProfileString(szDSN, DSN_OPTIONS_DATE1582,"", 
                                   szDate1582, sizeof(szDate1582)-1, ODBC_INI);
        if (memcmp(szDate1582, "NULL", 4)==0) /* "Return blank dates as NULL" */
            CheckDlgButton(hDlg, IDC_DATE1582, BST_CHECKED);

        if(!*szCatConnect)
            SQLGetPrivateProfileString(szDSN, DSN_OPTIONS_CATCONNECT,"N", 
                                   szCatConnect, sizeof(szCatConnect)-1, ODBC_INI);
        if (szCatConnect[0] == 'Y')           /* Force separate session for catalog */
            CheckDlgButton(hDlg, IDC_CATCONNECT, BST_CHECKED);

        if(!*szNumOverflow)
            SQLGetPrivateProfileString(szDSN, DSN_OPTIONS_NUMOVERFLOW,"", 
                                   szNumOverflow, sizeof(szNumOverflow)-1, ODBC_INI);
        if (szNumOverflow[0] == 'I'  ||        /* Ignore divide by zero */
            szNumOverflow[0] == 'i')           /* and overflow/underflow*/
           {strcpy(szNumOverflow, "Ignore");
            CheckDlgButton(hDlg, IDC_NUMOVERFLOW, BST_CHECKED);
           }

        SQLGetPrivateProfileString(szDSN, DSN_OPTIONS_IIDECIMAL,"N", 
            szIIDECIMAL, 6, ODBC_INI);
        szIIDECIMAL[1]='\0';          /* bulletproof length of string */
        if (szIIDECIMAL[0] == 'Y')    /* Use II_DECIMAL? */
            CheckDlgButton(hDlg, IDC_SUPPORT_II_DECIMAL, BST_CHECKED);

        if(!*szCatSchemaNULL)
            SQLGetPrivateProfileString(szDSN, DSN_OPTIONS_CATSCHEMANULL,"N", 
                                   szCatSchemaNULL, sizeof(szCatSchemaNULL)-1, ODBC_INI);
        if (*szCatSchemaNULL == 'Y')           /* Force separate session for catalog */
            CheckDlgButton(hDlg, IDC_CATSCHEMANULL, BST_CHECKED);

        if(!*szCatUnderscoreDisabled)
            SQLGetPrivateProfileString(szDSN, DSN_OPTIONS_CATUNDERSCORE,"", 
                    szCatUnderscoreDisabled, sizeof(szCatUnderscoreDisabled)-1, ODBC_INI);
        if(!*szCatUnderscoreDisabled)  /* if still not set, set default */
            if (isIngresServerType(szServerType))  /* based on server type */
                  *szCatUnderscoreDisabled = 'N';
            else  *szCatUnderscoreDisabled = 'Y';
        if (*szCatUnderscoreDisabled == 'Y')   /* Underscore search char disabled */
           {CheckDlgButton(hDlg, IDC_UNDERSCORESEARCH, BST_CHECKED);
            if (strcmp(szServerType, "IDMS") == 0)  /* if servertype == "IDMS" */
                EnableWindow(GetDlgItem (hDlg, IDC_UNDERSCORESEARCH), FALSE);
                      /* disable it since IDMS can't support underscore search */
           }

        if(!*szAllowProcedureUpdate)
            SQLGetPrivateProfileString(szDSN, DSN_OPTIONS_ALLOWPROCEDUREUPDATE,"", 
                    szAllowProcedureUpdate, sizeof(szAllowProcedureUpdate)-1, ODBC_INI);
        if(!*szAllowProcedureUpdate)  /* if still not set, set default */
            *szAllowProcedureUpdate = 'N';
        if (*szAllowProcedureUpdate == 'Y')   /* Underscore search char disabled */
           CheckDlgButton(hDlg, IDC_ALLOWPROCEDUREUPDATE, BST_CHECKED);
                    

        if(!*szMultibyteFillChar)
            SQLGetPrivateProfileString(szDSN, DSN_OPTIONS_MULTIBYTEFILLCHAR,"",
                    szMultibyteFillChar, sizeof(szMultibyteFillChar)-1, ODBC_INI);


		if(!*szConvertInt8ToInt4)
            SQLGetPrivateProfileString(szDSN, DSN_OPTIONS_CONVERTINT8TOINT4,"", 
                    szAllowProcedureUpdate, sizeof(szConvertInt8ToInt4)-1, ODBC_INI);
        if(!*szConvertInt8ToInt4)  /* if still not set, set default */
            *szConvertInt8ToInt4 = 'N';
        if (*szConvertInt8ToInt4 == 'Y')  
           CheckDlgButton(hDlg, IDC_CONVERTINT8TOINT4, BST_CHECKED);
        /*
        ** If the current setting of is a white character, such as <TAB>
        ** or a blank, empty the contents so that a new character can be
        ** inserted without having to hit the "delete" key.  Allow only
        ** one character to be inserted.
        */
        if (CMwhite(szMultibyteFillChar))
              *szMultibyteFillChar = '\0';
        SetDlgItemText (hDlg, IDC_MULTIBYTE_FILL_CHAR, szMultibyteFillChar);
        SendDlgItemMessage (hDlg, IDC_MULTIBYTE_FILL_CHAR, 
            EM_LIMITTEXT, 1L, 0L);

        return TRUE;

    case WM_NOTIFY:
        switch (((NMHDR  *) lParam)->code)
        {        
        case PSN_SETACTIVE:

            HideOrShowProcUpdt(hDlg);
            return FALSE;
    
        case PSN_KILLACTIVE:
			if (IsDlgButtonChecked(hDlg, IDC_SELECT_LOOPS) == BST_CHECKED)
				CMcpychar("Y",&szSelectLoops[0]); 
			else
				CMcpychar("N",&szSelectLoops[0]); 

			if (IsDlgButtonChecked(hDlg, IDC_3_PART_NAMES) == BST_CHECKED)
				sz3PartNames[0] = 'Y';
			else
				sz3PartNames[0] = 'N';

			if (IsDlgButtonChecked(hDlg, IDC_SYSTABLES) == BST_CHECKED)
				szSYSTABLES[0] = 'Y';
			else
				szSYSTABLES[0] = 'N';

			if (IsDlgButtonChecked(hDlg, IDC_BLANKDATE) == BST_CHECKED)
				strcpy(szBlankDate, "NULL");
			else
				szBlankDate[0] = '\0';

			if (IsDlgButtonChecked(hDlg, IDC_DATE1582) == BST_CHECKED)
				strcpy(szDate1582, "NULL");
			else
				szDate1582[0] = '\0';

			if (IsDlgButtonChecked(hDlg, IDC_CATCONNECT) == BST_CHECKED)
				szCatConnect[0] = 'Y';
			else
				szCatConnect[0] = 'N';

			if (IsDlgButtonChecked(hDlg, IDC_NUMOVERFLOW) == BST_CHECKED)
				strcpy(szNumOverflow, "Ignore");
			else
				szNumOverflow[0] = '\0';

			if (IsDlgButtonChecked(hDlg, IDC_SUPPORT_II_DECIMAL) == BST_CHECKED)
				szIIDECIMAL[0] = 'Y';
			else
				szIIDECIMAL[0] = 'N';

			if (IsDlgButtonChecked(hDlg, IDC_CATSCHEMANULL) == BST_CHECKED)
				szCatSchemaNULL[0] = 'Y';
			else
				szCatSchemaNULL[0] = 'N';

			if (IsDlgButtonChecked(hDlg, IDC_UNDERSCORESEARCH) == BST_CHECKED)
				szCatUnderscoreDisabled[0] = 'Y';
			else
				szCatUnderscoreDisabled[0] = 'N';

			if (IsDlgButtonChecked(hDlg, IDC_ALLOWPROCEDUREUPDATE) == BST_CHECKED)
				szAllowProcedureUpdate[0] = 'Y';
			else
				szAllowProcedureUpdate[0] = 'N';

            GetDlgItemText (hDlg, IDC_MULTIBYTE_FILL_CHAR,
                szMultibyteFillChar, sizeof(szMultibyteFillChar));
            if (!*szMultibyteFillChar)
                *szMultibyteFillChar = ' ';

			if (IsDlgButtonChecked(hDlg, IDC_CONVERTINT8TOINT4) == BST_CHECKED)
				szConvertInt8ToInt4[0] = 'Y';
			else
				szConvertInt8ToInt4[0] = 'N';
            return TRUE;

        case PSN_APPLY:
			if (*szBlankDate == ' ')  /* if connection string supplied strange value */
			    *szBlankDate  = '\0'; /* then write out no value supplied */
			if (*szDate1582  == ' ')  /* if connection string supplied strange value */
			    *szDate1582   = '\0'; /* then write out no value supplied */
			SQLWritePrivateProfileString(szDSN, DSN_OPTIONS_SELECTLOOPS, szSelectLoops, ODBC_INI);
			SQLWritePrivateProfileString(szDSN, DSN_OPTIONS_3_PART_NAMES,sz3PartNames,  ODBC_INI);
			SQLWritePrivateProfileString(szDSN, DSN_OPTIONS_SYSTABLES,   szSYSTABLES,   ODBC_INI);
			SQLWritePrivateProfileString(szDSN, DSN_OPTIONS_BLANKDATE,   szBlankDate,   ODBC_INI);
			SQLWritePrivateProfileString(szDSN, DSN_OPTIONS_DATE1582,    szDate1582,    ODBC_INI);
			SQLWritePrivateProfileString(szDSN, DSN_OPTIONS_CATCONNECT,  szCatConnect,  ODBC_INI);
			SQLWritePrivateProfileString(szDSN, DSN_OPTIONS_NUMOVERFLOW, szNumOverflow, ODBC_INI);
			SQLWritePrivateProfileString(szDSN, DSN_OPTIONS_IIDECIMAL,   szIIDECIMAL,   ODBC_INI);
			SQLWritePrivateProfileString(szDSN, DSN_OPTIONS_CATSCHEMANULL,szCatSchemaNULL,ODBC_INI);
			SQLWritePrivateProfileString(szDSN, DSN_OPTIONS_CATUNDERSCORE,szCatUnderscoreDisabled,
                                                                                        ODBC_INI);
			SQLWritePrivateProfileString(szDSN, DSN_OPTIONS_ALLOWPROCEDUREUPDATE,szAllowProcedureUpdate,
                                                                                        ODBC_INI);
                        SQLWritePrivateProfileString(szDSN, DSN_OPTIONS_MULTIBYTEFILLCHAR,szMultibyteFillChar,
        ODBC_INI);
			SQLWritePrivateProfileString(szDSN, DSN_OPTIONS_CONVERTINT8TOINT4,szConvertInt8ToInt4,ODBC_INI);

            return TRUE;

        case PSN_HELP:

            WinHelp (hDlg, szHelp, HELP_CONTENTS, (DWORD)0);

        case PSN_RESET:

            return TRUE;
        }
        break;

    case WM_COMMAND:
 
        wmId    = LOWORD(wParam);
        wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case IDC_SELECT_LOOPS:
		case IDC_CURSORS:
		case IDC_3_PART_NAMES:
		case IDC_SYSTABLES:
		case IDC_BLANKDATE:
		case IDC_DATE1582:
		case IDC_CATCONNECT:
		case IDC_NUMOVERFLOW:
		case IDC_SUPPORT_II_DECIMAL:
		case IDC_CATSCHEMANULL:
		case IDC_UNDERSCORESEARCH:
		case IDC_ALLOWPROCEDUREUPDATE:
                case IDC_MULTIBYTE_FILL_CHAR:
		case IDC_CONVERTINT8TOINT4:
			switch (wmEvent)
			{
			case BN_CLICKED:
			case BN_DBLCLK:
				PropSheet_Changed(hwndParent, hDlg);
				return FALSE;
			}
			break;
        }
        break;
    }
    return FALSE;
}
//
//  TestConnection
//
//  On entry:  szDSN = data source name
//
//  Returns:  nothing.
//
static void TestConnection (
    HWND hDlg, char *szDSN, char *szServer, char *szServerType)
{
      
	HCURSOR hCursor;      
	HENV    henv;
	HDBC    hdbc;
	RETCODE retcode;
	UCHAR   szSqlState[6];
	SDWORD  fNativeError;
	char  * szErrorMsg;
	SWORD   pcbErrorMsg;
	int     DlgbPrompt;
	SQLCHAR szConnStrIn[256];

	DlgbPrompt = (IsDlgButtonChecked(hDlg, IDC_PROMPTUIDPWD) == BST_CHECKED);

	ClearDSNCacheCapabilities(szDSN);  /* clear the ODBC.INI DSN of cached
	                                      capabilities to force the driver to
	                                      re-issue the queries to get them */

	retcode = SQLAllocEnv(&henv);              /* Environment handle */
	if (retcode == SQL_SUCCESS) 
	{
	   retcode = SQLAllocConnect(henv, &hdbc); /* Connection handle */
	   if (retcode == SQL_SUCCESS) 
		 {

		    hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));  /* set cursor = HOURGLASS */

			SQLSetConnectOption(hdbc, SQL_ACCESS_MODE,   SQL_MODE_READ_ONLY);
			SQLSetConnectOption(hdbc, SQL_AUTOCOMMIT,    SQL_AUTOCOMMIT_OFF);
			SQLSetConnectOption(hdbc, SQL_LOGIN_TIMEOUT, 30); /* login timeout */

			/* Connect to data source */

			wsprintf(szConnStrIn, "DSN=%s;", szDSN);

#ifdef DISABLE_PROMPT_DIALOG
			DlgbPrompt = FALSE; 
			   /* let's never prompt, for right now, in order to keep it simple.*/
#endif

			retcode = SQLDriverConnect(hdbc, hDlg, szConnStrIn, SQL_NTS,
			    NULL, 0, NULL, (UWORD)(DlgbPrompt?SQL_DRIVER_COMPLETE
			                                     :SQL_DRIVER_NOPROMPT));

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			    GetCapabilitiesFromServer(hdbc, szServer, szServerType);
			      /* fill in capabilities cache from server */

			SetCursor(hCursor);      /* set cursor back to ARROW */

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
			{
		
				/* Successful connection */
				MessageBox(hDlg, (LPCTSTR) ERget(F_OD0132_IDS_MSG_CONNECTOK), szMsgTitle, MB_OK);
				SQLDisconnect(hdbc);
			}
			else
			{   szErrorMsg=GlobalAllocPtr(GMEM_MOVEABLE, SQL_MAX_MESSAGE_LENGTH);
				if (szErrorMsg)
				   {
					CMcpychar("\0",szErrorMsg); 
				    SQLError(henv, hdbc, NULL, szSqlState, &fNativeError,
				             szErrorMsg, SQL_MAX_MESSAGE_LENGTH-1, 
				             &pcbErrorMsg);
				    MessageBox(hDlg, szErrorMsg, (LPCTSTR) ERget(F_OD0133_IDS_MSG_CONNECTFAILED), 
				               MB_OK | MB_ICONERROR);
				    GlobalFreePtr(szErrorMsg);
				   }
				else
				    MessageBox(hDlg, (LPCTSTR) ERget(F_OD0133_IDS_MSG_CONNECTFAILED), 
				              szMsgTitle, MB_OK | MB_ICONERROR);
			}
			SQLFreeConnect(hdbc);
		}
		SQLFreeEnv(henv);
	}

    return;
}

//
//  BrowseConnectionForDBNames
//
//  On entry:  hDlg = Dialog handle
//
//  Returns:  nothing.
//
static void BrowseConnectionForDBNames (HWND hDlg)
{

	HCURSOR hCursor;      
	HENV    henv  = NULL;
	HDBC    hdbc  = NULL;
	HSTMT   hstmt = NULL;
	RETCODE rc;
	LRESULT lrc;
	UCHAR   szSqlState[6];
	SDWORD  fNativeError;
	char  * szErrorMsg;
	SWORD   pcbErrorMsg;
	SQLCHAR szConnStrIn[256];
	char    szCaption[256];
	char    rgbValue[64];
	SQLINTEGER  cbValue;
	OFFSET_TYPE     i;
	char  * p;
	char    szServerType[LEN_SERVERTYPE + 1];

	char    DlgszServer    [LEN_SERVER + 1];
	char    DlgszServerType[LEN_SERVERTYPE + 1];
	int     DlgbPrompt;

#define MSG_DATABASEBROWSE " - Database Name Browse"

	GetDlgItemText (hDlg, IDC_VNODE,      DlgszServer,     LEN_SERVER     + 1);
	GetDlgItemText (hDlg, IDC_SERVERTYPE, DlgszServerType, LEN_SERVERTYPE + 1);
	DlgbPrompt = (IsDlgButtonChecked(hDlg, IDC_PROMPTUIDPWD) == BST_CHECKED);

	for (i=0; DlgszServerType[i] != '\0' && DlgszServerType[i] != ' '; i++);
	DlgszServerType[i] = '\0';   /* strip comment information */

	/* build the Database name list */
	if (pDatabaseList == NULL                                      ||
		strcmp(DatabaseListszServer,        DlgszServer)     != 0  ||
		strcmp(DatabaseListDlgszServerType, DlgszServerType) != 0)
	{
		strcpy(DatabaseListszServer,        DlgszServer);
		strcpy(DatabaseListDlgszServerType, DlgszServerType);

	if (pDatabaseList == NULL)
	    pDatabaseList=GlobalAllocPtr(GMEM_MOVEABLE, DatabaseListMAX+64);

	*pDatabaseList = '\0';    /* clear first byte */
	DatabaseListSize = 0;     /* current number of chars in list */

	while (1)    /* convenient loop to break out of */ 
	{
		rc = SQLAllocEnv(&henv);           /* Environment handle */
		if (rc == SQL_ERROR)
			break;
		rc = SQLAllocConnect(henv, &hdbc); /* Connection handle */
		if (rc == SQL_ERROR)
			break;

		hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));  /* set cursor = HOURGLASS */

		SQLSetConnectOption(hdbc, SQL_ACCESS_MODE,   SQL_MODE_READ_ONLY);
		SQLSetConnectOption(hdbc, SQL_AUTOCOMMIT,    SQL_AUTOCOMMIT_OFF);
		SQLSetConnectOption(hdbc, SQL_LOGIN_TIMEOUT, 30); /* login timeout */

		/* Connect to data source */

		if (strcmp(DlgszServerType, "STAR") == 0)
		{
		    strcpy(szServerType, "INGRES");
		}
		else
		{
		    strcpy(szServerType, DlgszServerType);
		}		      
		wsprintf(szConnStrIn, 
		           "DRIVER={%s};SERVER=%s;SERVERTYPE=%s;DATABASE=iidbdb",
                   szDriver,   DlgszServer, szServerType);

#ifdef DISABLE_PROMPT_DIALOG
		DlgbPrompt = FALSE; 
		   /* let's never prompt, for right now, in order to keep it simple.
		      Otherwise, users may be confused in filling the
		      DriverConnect dialog that says an iidbdb database.*/
#endif

		rc = SQLDriverConnect(hdbc, hDlg, szConnStrIn, SQL_NTS,
			NULL, 0, NULL, (UWORD)(DlgbPrompt?SQL_DRIVER_PROMPT
			                                 :SQL_DRIVER_NOPROMPT));

		SetCursor(hCursor);      /* set cursor back to ARROW */

		if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
		{
			GetCapabilitiesFromServer(hdbc, DlgszServer, szServerType);
			  /* fill in capabilities cache from server */

			/* Successful connection */
			rc = SQLAllocStmt(hdbc, &hstmt);
			            SQLBindCol(hstmt, 1, SQL_C_CHAR, 
			                    rgbValue, sizeof(rgbValue), &cbValue);
			if (strcmp(DlgszServerType, "STAR") == 0)
			{
			    rc = SQLExecDirect(hstmt, "select ddb_name from iistar_cdbinfo", SQL_NTS);
			}
			else
			{	
			    rc = SQLExecDirect(hstmt, "select name from iidatabase", SQL_NTS);
			}
			if (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
				rc = SQLFetch(hstmt);
			while (rc == SQL_SUCCESS || rc == SQL_SUCCESS_WITH_INFO)
				{
				  i = cbValue;
				  rgbValue[i--] = '\0';
				  while (i >= 0  && rgbValue[i] == ' ') 
				     rgbValue[i--] = '\0';
				  i = strlen(rgbValue);
				  if (i)
				     { strcpy(pDatabaseList+DatabaseListSize, rgbValue);
				       DatabaseListSize += i+1;
				       *(pDatabaseList+DatabaseListSize) = '\0'; /*mark EOL*/
				     }
				  if (DatabaseListSize > DatabaseListMAX)
				     break;
				  rc = SQLFetch(hstmt);
				}  /* end while loop through rows of iidatabase */
		}
		else   /* connect failed for database browsing */
		{   szErrorMsg=GlobalAllocPtr(GMEM_MOVEABLE, SQL_MAX_MESSAGE_LENGTH);
			if (szErrorMsg)
			   {*szErrorMsg='\0';
			    SQLError(henv, hdbc, NULL, szSqlState, &fNativeError,
			             szErrorMsg, SQL_MAX_MESSAGE_LENGTH-1, 
			             &pcbErrorMsg);
			    strcpy(szCaption, (LPCTSTR) ERget(F_OD0133_IDS_MSG_CONNECTFAILED));
			    strcat(szCaption, MSG_DATABASEBROWSE);
			    MessageBox(hDlg, szErrorMsg, 
			              szCaption, MB_OK | MB_ICONERROR);
			    GlobalFreePtr(szErrorMsg);
			   }
			else
			    MessageBox(hDlg, (LPCTSTR) ERget(F_OD0133_IDS_MSG_CONNECTFAILED), 
			              szMsgTitle, MB_OK | MB_ICONERROR);
		}
		break;
	}   /* end convenient while loop */

	if (hstmt)
		{ SQLFreeStmt(hstmt, SQL_DROP);
		  SQLDisconnect(hdbc);
		}
	if (hdbc)
		SQLFreeConnect(hdbc);
	if (henv)
		SQLFreeEnv(henv);

	}  /* endif pDatabaseList ... need to build database list */

	do  /* clear out old list */
	{ lrc = SendDlgItemMessage(hDlg, IDC_DBNAME, CB_DELETESTRING,
	                         (WPARAM) 0, (LPARAM) 0L);
	} while (lrc != CB_ERR  &&  lrc > 0);

	for (p = pDatabaseList; *p; p = p + strlen(p) + 1)
		{
		 if (CB_ERR == SendDlgItemMessage(hDlg, IDC_DBNAME, CB_FINDSTRINGEXACT,
		                     (WPARAM) -1, (LPARAM) (LPCSTR) p))
		    SendDlgItemMessage(hDlg, IDC_DBNAME, CB_ADDSTRING, 
		                      (WPARAM) 0, (LPARAM) ((LPCSTR)p)); 
		}
    return;
}

//
//  Validate data source control values
//
//  On entry: hDlg = dialog window handle
//
//  Returns:  return value for PSN_KILLACTIVE message
//            
//            TRUE  = some error, sheet stays active
//            FALSE = OK, can leave sheet, maybe update registry
//
BOOL ValidateDataSource (HWND hDlg)
{
	char * lpM;

    GetDlgItemText (hDlg, IDC_NAME, szDSN, LEN_DSN + 1);
	lpM = ERget(F_OD0123_IDS_MSG_REQUIRED);
	STcopy(lpM,(char *)MSG_REQUIRED);

    if (*szDSN == EOS)
    {
        wsprintf (szMessage, MSG_REQUIRED, ERget(F_OD0125_IDS_MSG_DSN));
        MessageBox (hDlg, szMessage, szMsgTitle, MB_OK | MB_ICONSTOP);
        SetFocus (GetDlgItem (hDlg, IDC_NAME));
        return TRUE;
    }
    
    if (!SQLValidDSN (szDSN))
    {
        CMcpychar("\0",szDSN); 
        MessageString (hDlg, F_OD0118_IDS_ERR_DSN, MB_OK | MB_ICONSTOP);
        SetFocus (GetDlgItem (hDlg, IDC_NAME));
        return TRUE;
    }

	GetDlgItemText (hDlg, IDC_VNODE, szServer, LEN_SERVER + 1);
	if (*szServer == EOS)
    {
        wsprintf (szMessage, MSG_REQUIRED, ERget(F_OD0127_IDS_MSG_SERVER));
        MessageBox (hDlg, szMessage, szMsgTitle, MB_OK | MB_ICONSTOP);
        SetFocus (GetDlgItem (hDlg, IDC_VNODE));
        return TRUE;
    }

	GetDlgItemText (hDlg, IDC_DBNAME, szDBName, LEN_DBNAME + 1);
	if (*szDBName == EOS)
    {
        wsprintf (szMessage, MSG_REQUIRED, ERget(F_OD0130_IDS_MSG_DBNAME));
        MessageBox (hDlg, szMessage, szMsgTitle, MB_OK | MB_ICONSTOP);
        SetFocus (GetDlgItem (hDlg, IDC_DBNAME));
        return TRUE;
    }

	GetDlgItemText (hDlg, IDC_SERVERTYPE, szServerType, LEN_SERVERTYPE + 1);
	if (*szServerType == EOS)
	{
		wsprintf (szMessage, MSG_REQUIRED, ERget(F_OD0131_IDS_MSG_SERVERTYPE));
		MessageBox (hDlg, szMessage, szMsgTitle, MB_OK | MB_ICONSTOP);
		SetFocus (GetDlgItem (hDlg, IDC_SERVERTYPE));
		return TRUE;
	}
	GetDlgItemText (hDlg, IDC_DESCRIPTION, szDescription, LEN_DESCRIPTION + 1);
	GetDlgItemText (hDlg, IDC_GATEWAY_OPTION, szWithOption, LEN_GATEWAY_OPTION + 1);
	  if (STbcompare(szWithOption,5,"with ",5,TRUE)==0)  /* strip any with clause */
	{ 
	     char * s;
             char * t;
             char bogus[6];

	     strcpy(bogus,szWithOption);
	     t=szWithOption+0;
             s=szWithOption+5;
             /* skip leading whitespace */
             while (*s != EOS) 
				 if (CMwhite(s))
				     CMnext(s);  /* skip leading whitespace */
             while (*s != EOS)
	     {
	         CMcpychar(s,t);            /* copy the string */
                 CMnext(s);
		 CMnext(t);
	     }
        }	
	if (IsDlgButtonChecked(hDlg, IDC_PROMPTUIDPWD) == BST_CHECKED)
            CMcpychar("Y",&szPrompt[0]); 
	else
            CMcpychar("N",&szPrompt[0]); 

	if (IsDlgButtonChecked(hDlg, IDC_READ_ONLY) == BST_CHECKED)
            CMcpychar("Y",&szReadOnly[0]); 
	else
            CMcpychar("N",&szReadOnly[0]); 

	GetDlgItemText (hDlg, IDC_ROLENAME, szRoleName, LEN_ROLENAME + 1);
	GetDlgItemText (hDlg, IDC_ROLEPWD,  szRolePWD,  LEN_ROLEPWD  + 1);
    GetDlgItemText (hDlg, IDC_GROUP,  szGroup,  LEN_GROUP  + 1);

    return FALSE;
}

/*--------------------------------------------------------------*/
//
//  EncryptPWD
//
//  Encrypt the password and convert to character string
//  since .ini files only work with valid ascii strings 
//  and can't handle embedded binary characters. The Registry
//  on WIN32 systems can but we need to support 16-bit also.
//
//  On entry: name-->user name
//            pwd -->clear text password
//
//  Returns:  *pwd is overlaid with the character string.
//
void EncryptPWD(char * name, char * pwd)
{ char * s;
  char * t;
  char   newpwd[LEN_ROLEPWD+1]="";
  int i, ismoke=0;
  OFFSET_TYPE j;
  static int TranslateTab[]={'0','1','2','3','4','5','6','7',
                             '8','9','A','B','C','D','E','F'};

  j = strlen(pwd);           /* seed the background noise */
  for (i=0; i<j; i++)
       ismoke += pwd[i];
  ismoke %= 256;
  for (i++; i<32; i++)       /* add smoke after the real pwd */
       pwd[i] = (ismoke+i) & 0xff;
  
  pwcrypt(name,pwd); /*encrypt the pwd into the binary string*/

  for (s=pwd, t=newpwd, j=0; j<32; s++, j++)  /* convert binary to char string */
      { i = *s & 0xff;
        *(t++)=TranslateTab[i/16];
        *(t++)=TranslateTab[i&15];
      }  /* end for loop */
  *t='\0';
  STcopy(newpwd,pwd); /* copy new character string pwd to pwd */

}

/*--------------------------------------------------------------*/
static int DecryptTrans(int c)
{
  if (c>='0'  &&  c<='9')
     return(0x00 + (c-'0'));

  if (c>='A'  &&  c<='F')
     return(0x0a + (c-'A'));

  if (c>='a'  &&  c<='f')
     return(0x0a + (c-'a'));

  return(0);
}

/*--------------------------------------------------------------*/
//
//  DecryptPWD
//
//  Decrypt the password from a encrypted character string.to
//                              encrypted binary string to
//                              cleartext character string
//
//  On entry: name-->user name
//            pwd -->Encrypted password character string
//
//  Returns:  *pwd is overlaid with the character string.
//
static void DecryptPWD(char * name, char * pwd)
{ char * s;
  char * t;
  char   newpwd[LEN_ROLEPWD+1];
  int i;

  for (s=pwd, t=newpwd; *s; s++)  /* convert char string to binary*/
      { i = DecryptTrans(*s)*16;
        if (*(++s))
           i += DecryptTrans(*s);
        else s--;    /* shouldn't happen; '\0' in trailing char */
        *(t++)=i;
      }  /* end for loop */

  memcpy(pwd,newpwd,32); /* copy new character string pwd to pwd */
  
  pwcrypt(name,pwd); /*decrypt the pwd into cleartext*/
}


//
//  PaintLogo
//
//  On entry:  hDlg      = Dialog handle
//             control   = identifier of the destination control
//             prectdest = destination rectangle
//             hdcDest   = destination DC
//             hbitmap   = source bitmap handle
//
//  Returns:  nothing.
//
static void PaintLogo(HWND hDlg, int control,
                      LPRECT prectdest, HDC hdcDest, HBITMAP hbitmap)
{
   HDC     hdcmem;
   HBITMAP hbmold;
   BITMAP  bitmap;
   int     DestWidth, DestHeight, xDest, yDest;

   if (hbitmap == NULL)
      return;

   xDest      = prectdest->left;
   yDest      = prectdest->top;
   DestWidth  = prectdest->right -  prectdest->left;
   DestHeight = prectdest->bottom - prectdest->top;

   GetObject(hbitmap, sizeof(BITMAP), &bitmap);

   xDest     += (DestWidth  - bitmap.bmWidth) /2;  /* just center the map to */
// yDest     += (DestHeight - bitmap.bmHeight)/2;  /* avoid stretch distortion */

   hdcmem = CreateCompatibleDC(hdcDest);  /* create a memory device context (DC) */

   /* select the logical palette into the DCs as a foreground palette*/
   SelectPalette (hdcDest, hPalette, FALSE);
   RealizePalette(hdcDest);      /* map entries from logical to system palette*/
   SelectPalette (hdcmem,  hPalette, FALSE);
   RealizePalette(hdcmem);   /* map entries from logical to system palette*/

   hbmold = SelectObject(hdcmem, hbitmap); /* select the bitmap into the DC */
// BitBlt(hdcDest, xDest, yDest, DestWidth, DestHeight,
//        hdcmem, 0, 0, SRCCOPY);
   SetStretchBltMode(hdcDest, COLORONCOLOR);
   StretchBlt(hdcDest, 0, 0,      DestWidth,      DestHeight,
               hdcmem, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);  
   SelectObject(hdcmem, hbmold);
   DeleteDC(hdcmem);
}


//
//  LoadResourceBitmap
//
//  On entry: hinst      =  module handle of the DLL
//            lpszName   -> resource name
//            lphPalette -> where to return palette handle
//
//  Returns:  handle of bitmap
//
//  Notes:    This routine is necessary because LoadBitmap
//            loses the pallette colors.  This function loads 
//            a device-independent bitmap (DIB) from the 
//            resource (.rc), create a palette and a device-
//            dependent bitmap (DDB) out of it.
//            Idea is from MS KB article Q124947.
//
static HBITMAP LoadResourceBitmap(HINSTANCE      hinst, 
                                  LPSTR          lpszName,
                                  HPALETTE  * lphPalette)
{
   HGLOBAL            hglobal;
   HBITMAP            hbitmap = NULL;
   LPBITMAPINFOHEADER lpbminfohdr;
   HDC                hdc;
   int                numcolors;

   if (hglobal = LoadResource(hinst, FindResource(hinst, lpszName, RT_BITMAP)))
      {
       lpbminfohdr = (LPBITMAPINFOHEADER) LockResource(hglobal);

       hdc = GetDC(NULL);   /* get a working device context */

       *lphPalette = CreateDIBPalette(lpbminfohdr, &numcolors);
       if (*lphPalette)
          {
           SelectPalette(hdc, *lphPalette, FALSE);
              /* select the logical palette into the DC as a foreground palette*/
           RealizePalette(hdc);   /* map entries from logical to system palette*/
          }

       hbitmap = CreateDIBitmap(hdc,   /* create the DDB from DIB */
                                lpbminfohdr,
                                (LONG) CBM_INIT,
                                (LPSTR) lpbminfohdr + lpbminfohdr->biSize
                                                    + numcolors * sizeof(RGBQUAD),
                                (LPBITMAPINFO)lpbminfohdr,
                                DIB_RGB_COLORS);

       ReleaseDC(NULL, hdc);   /* clean up */
       UnlockResource(hglobal);
       FreeResource  (hglobal);
      }  /* end if hglobal = LoadResource...*/
   
   return(hbitmap);
}

//
//  CreateDIBPalette
//
//  On entry: hpbmi      -> module handle of the DLL
//            lpszName   -> resource name
//            lphPalette -> where to return palette handle
//
//  Returns:  handle of bitmap
//
//  Notes:    This function creates a device-independent 
//            bitmap (DIB) palette.
//
static HPALETTE CreateDIBPalette(LPBITMAPINFOHEADER lpbi, LPINT lpnumcolors)
{
   LPBITMAPINFO lpbmi;
   LPLOGPALETTE lpPal;
   HANDLE       hndl;
   HPALETTE     hPal = NULL;
   int          i;

   lpbmi = (LPBITMAPINFO) lpbi;

   if (lpbi->biBitCount <= 8)
        *lpnumcolors = (1 << lpbi->biBitCount);
   else *lpnumcolors = 0;  /* no palette for 24 bpp DIB */

   if (lpbi->biClrUsed > 0)
        *lpnumcolors = lpbi->biClrUsed;

   if (*lpnumcolors)
      {
       hndl = GlobalAlloc(GHND, sizeof(LOGPALETTE) +
                                sizeof(PALETTEENTRY) * (*lpnumcolors));
       lpPal = (LPLOGPALETTE) GlobalLock(hndl);

       lpPal->palVersion    = 0x300;        /* fill in the logical palette */
       lpPal->palNumEntries = *lpnumcolors;
       for (i=0; i < *lpnumcolors; i++)
           {
            lpPal->palPalEntry[i].peRed   = lpbmi->bmiColors[i].rgbRed;
            lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
            lpPal->palPalEntry[i].peBlue  = lpbmi->bmiColors[i].rgbBlue;
            lpPal->palPalEntry[i].peFlags = 0;
           }  /* end for loop */

       hPal = CreatePalette(lpPal);  /* create the logical color palatte */
       GlobalUnlock(hndl);
       GlobalFree  (hndl);
      }  /* end if (*lpnumcolors) */
   
   return(hPal);
}

/*
**  ClearDSNCacheCapabilities
**
**  On entry: szDSN      -> DSN name
**
**  Returns:  nothing
**
**  Notes:    Delete all of the DSN capabilities cache in the ODBC.INI.
*/
void ClearDSNCacheCapabilities(char * szDSN)
{
   BOOL   rc;

   if (szDSN == NULL  ||  *szDSN == '\0')
      return;

   rc = SQLGetPrivateProfileString(szDSN, DSN_CACHE_ServerVersion, "", CacheServerVersion,
	      sizeof(CacheServerVersion), ODBC_INI);
   if(rc)
	   rc = SQLWritePrivateProfileString(szDSN, DSN_CACHE_ServerVersion,
          NULL, ODBC_INI);

   rc = SQLGetPrivateProfileString(szDSN, DSN_CACHE_ServerNameCase, "", CacheServerNameCase,
	      sizeof(CacheServerNameCase), ODBC_INI);
   if(rc)
	   rc = SQLWritePrivateProfileString(szDSN, DSN_CACHE_ServerNameCase,
          NULL, ODBC_INI);

   rc = SQLGetPrivateProfileString(szDSN, DSN_CACHE_ServerDelimitedCase, "", CacheServerDelimitedCase,
	      sizeof(CacheServerDelimitedCase), ODBC_INI);
   if(rc)
	   rc = SQLWritePrivateProfileString(szDSN, DSN_CACHE_ServerDelimitedCase,
          NULL, ODBC_INI);

   rc = SQLGetPrivateProfileString(szDSN, DSN_CACHE_ServerMaxColumns, "", CacheServerMaxColumns,
	      sizeof(CacheServerMaxColumns), ODBC_INI);
   if(rc)
	   rc = SQLWritePrivateProfileString(szDSN, DSN_CACHE_ServerMaxColumns,
          NULL, ODBC_INI);

   rc = SQLGetPrivateProfileString(szDSN, DSN_CACHE_ServerMaxSchemaNameLen, "", CacheServerMaxSchemaNameLen,
	      sizeof(CacheServerMaxSchemaNameLen), ODBC_INI);
   if(rc)
	   rc = SQLWritePrivateProfileString(szDSN, DSN_CACHE_ServerMaxSchemaNameLen,
          NULL, ODBC_INI);

   rc = SQLGetPrivateProfileString(szDSN, DSN_CACHE_ServerMaxTableNameLen, "", CacheServerMaxTableNameLen,
	      sizeof(CacheServerMaxTableNameLen), ODBC_INI);
   if(rc)
	   rc = SQLWritePrivateProfileString(szDSN, DSN_CACHE_ServerMaxTableNameLen,
          NULL, ODBC_INI);

   rc = SQLGetPrivateProfileString(szDSN, DSN_CACHE_ServerMaxColumnNameLen, "", CacheServerMaxColumnNameLen,
	      sizeof(CacheServerMaxColumnNameLen), ODBC_INI);
   if(rc)
	   rc = SQLWritePrivateProfileString(szDSN, DSN_CACHE_ServerMaxColumnNameLen,
          NULL, ODBC_INI);

   rc = SQLGetPrivateProfileString(szDSN, DSN_CACHE_ServerMaxRowLen, "", CacheServerMaxRowLen,
	      sizeof(CacheServerMaxRowLen), ODBC_INI);
   if(rc)
	   rc = SQLWritePrivateProfileString(szDSN, DSN_CACHE_ServerMaxRowLen,
          NULL, ODBC_INI);
   
   rc = SQLGetPrivateProfileString(szDSN, DSN_CACHE_ServerMaxCharColumnLen, "", CacheServerMaxCharColumnLen,
	      sizeof(CacheServerMaxCharColumnLen), ODBC_INI);
   if(rc)
	   rc = SQLWritePrivateProfileString(szDSN, DSN_CACHE_ServerMaxCharColumnLen,
          NULL, ODBC_INI);
   return;

   rc = SQLGetPrivateProfileString(szDSN, DSN_CACHE_ServerMaxByteColumnLen, "", CacheServerMaxByteColumnLen,
	      sizeof(CacheServerMaxByteColumnLen), ODBC_INI);
   if(rc)
	   rc = SQLWritePrivateProfileString(szDSN, DSN_CACHE_ServerMaxByteColumnLen,
          NULL, ODBC_INI);
   return;

   rc = SQLGetPrivateProfileString(szDSN, DSN_CACHE_ServerMaxVchrColumnLen, "", CacheServerMaxVchrColumnLen,
	      sizeof(CacheServerMaxVchrColumnLen), ODBC_INI);
   if(rc)
	   rc = SQLWritePrivateProfileString(szDSN, DSN_CACHE_ServerMaxVchrColumnLen,
          NULL, ODBC_INI);
   return;

   rc = SQLGetPrivateProfileString(szDSN, DSN_CACHE_ServerMaxVbytColumnLen, "", CacheServerMaxVbytColumnLen,
	      sizeof(CacheServerMaxVbytColumnLen), ODBC_INI);
   if(rc)
	   rc = SQLWritePrivateProfileString(szDSN, DSN_CACHE_ServerMaxVbytColumnLen,
          NULL, ODBC_INI);
   return;
}

/*
**  WriteDSNCacheCapabilities
**
**  On entry: szDSN      -> DSN name
**
**  Returns:  nothing
**
**  Notes:    Delete all of the DSN capabilities cache in the ODBC.INI.
*/
void WriteDSNCacheCapabilities(char * szDSN, char * ServerName, char * ServerType)
{
   BOOL   rc;

   if (szDSN == NULL  ||  *szDSN == '\0'  ||  *CacheServerType == '\0')
      return;    /* return if no DSN or cache never filled in */

   if (strcmp(CacheServerName, ServerName) != 0  ||
       strcmp(CacheServerType, ServerType) != 0)
          /* if cache name/type differs from DSN name/type */
      { ClearDSNCacheCapabilities(szDSN);   /* clear the DSN cache */
        return;
      }

   rc = SQLWritePrivateProfileString(szDSN,
           DSN_CACHE_ServerVersion,          CacheServerVersion,          ODBC_INI);
   rc = SQLWritePrivateProfileString(szDSN,
           DSN_CACHE_ServerNameCase,         CacheServerNameCase,         ODBC_INI);
   rc = SQLWritePrivateProfileString(szDSN,
           DSN_CACHE_ServerDelimitedCase,    CacheServerDelimitedCase,    ODBC_INI);
   rc = SQLWritePrivateProfileString(szDSN,
           DSN_CACHE_ServerMaxColumns,       CacheServerMaxColumns,       ODBC_INI);
   rc = SQLWritePrivateProfileString(szDSN,
           DSN_CACHE_ServerMaxSchemaNameLen, CacheServerMaxSchemaNameLen, ODBC_INI);
   rc = SQLWritePrivateProfileString(szDSN,
           DSN_CACHE_ServerMaxTableNameLen,  CacheServerMaxTableNameLen,  ODBC_INI);
   rc = SQLWritePrivateProfileString(szDSN,
           DSN_CACHE_ServerMaxColumnNameLen, CacheServerMaxColumnNameLen, ODBC_INI);
   rc = SQLWritePrivateProfileString(szDSN,
           DSN_CACHE_ServerMaxRowLen,        CacheServerMaxRowLen,        ODBC_INI);
   rc = SQLWritePrivateProfileString(szDSN,
           DSN_CACHE_ServerMaxCharColumnLen, CacheServerMaxCharColumnLen, ODBC_INI);
   rc = SQLWritePrivateProfileString(szDSN,
           DSN_CACHE_ServerMaxVchrColumnLen, CacheServerMaxVchrColumnLen, ODBC_INI);
   rc = SQLWritePrivateProfileString(szDSN,
           DSN_CACHE_ServerMaxByteColumnLen, CacheServerMaxByteColumnLen, ODBC_INI);
   rc = SQLWritePrivateProfileString(szDSN,
           DSN_CACHE_ServerMaxVbytColumnLen, CacheServerMaxVbytColumnLen, ODBC_INI);
   return;
}

/*
**  GetCapabilitiesFromDSN
**
**  On entry: szDSN      -> Data Source Name
**
**  Returns:  nothing
**
**  Notes:    Get the namecase, tablename length, etc., into memory from DSN
**            before we start editting in changes to servername and servertype.
*/
static void GetCapabilitiesFromDSN(char * szDSN)
{
   BOOL   rc;

   if (szDSN == NULL  ||  *szDSN == '\0')
      return;    /* return if no DSN */

   rc = SQLGetPrivateProfileString(szDSN,
           DSN_OPTIONS_SERVER,
           "", CacheServerName,             sizeof(CacheServerName),
           ODBC_INI);
   rc = SQLGetPrivateProfileString(szDSN,
           DSN_OPTIONS_SERVERTYPE, SERVER_TYPE_DEFAULT,
               CacheServerType,             sizeof(CacheServerType),
           ODBC_INI);
   rc = SQLGetPrivateProfileString(szDSN,
           DSN_CACHE_ServerVersion,
           "", CacheServerVersion,          sizeof(CacheServerVersion),
           ODBC_INI);

   if (*CacheServerVersion == '\0')    /* skip the rest if no entries */
      {
       *CacheServerNameCase         = '\0';
       *CacheServerDelimitedCase    = '\0';
       *CacheServerMaxColumns       = '\0';
       *CacheServerMaxSchemaNameLen = '\0';
       *CacheServerMaxTableNameLen  = '\0';
       *CacheServerMaxColumnNameLen = '\0';
       *CacheServerMaxRowLen        = '\0';
       *CacheServerMaxCharColumnLen = '\0';
       *CacheServerMaxVchrColumnLen = '\0';
       *CacheServerMaxByteColumnLen = '\0';
       *CacheServerMaxVbytColumnLen = '\0';
       return;
      }

   rc = SQLGetPrivateProfileString(szDSN,
           DSN_CACHE_ServerNameCase,
           "", CacheServerNameCase,         sizeof(CacheServerNameCase),
           ODBC_INI);
   rc = SQLGetPrivateProfileString(szDSN,
           DSN_CACHE_ServerDelimitedCase,
           "", CacheServerDelimitedCase,    sizeof(CacheServerDelimitedCase),
           ODBC_INI);
   rc = SQLGetPrivateProfileString(szDSN,
           DSN_CACHE_ServerMaxColumns,
           "", CacheServerMaxColumns,       sizeof(CacheServerMaxColumns),
           ODBC_INI);
   rc = SQLGetPrivateProfileString(szDSN,
           DSN_CACHE_ServerMaxSchemaNameLen,
           "", CacheServerMaxSchemaNameLen, sizeof(CacheServerMaxSchemaNameLen),
           ODBC_INI);
   rc = SQLGetPrivateProfileString(szDSN,
           DSN_CACHE_ServerMaxTableNameLen,
           "", CacheServerMaxTableNameLen,  sizeof(CacheServerMaxTableNameLen),
           ODBC_INI);
   rc = SQLGetPrivateProfileString(szDSN,
           DSN_CACHE_ServerMaxColumnNameLen,
           "", CacheServerMaxColumnNameLen, sizeof(CacheServerMaxColumnNameLen),
           ODBC_INI);
   rc = SQLGetPrivateProfileString(szDSN,
           DSN_CACHE_ServerMaxRowLen,
           "", CacheServerMaxRowLen, sizeof(CacheServerMaxRowLen),
           ODBC_INI);
   rc = SQLGetPrivateProfileString(szDSN,
           DSN_CACHE_ServerMaxCharColumnLen,
           "", CacheServerMaxCharColumnLen, sizeof(CacheServerMaxCharColumnLen),
           ODBC_INI);
   rc = SQLGetPrivateProfileString(szDSN,
           DSN_CACHE_ServerMaxVchrColumnLen,
           "", CacheServerMaxVchrColumnLen, sizeof(CacheServerMaxVchrColumnLen),
           ODBC_INI);
   rc = SQLGetPrivateProfileString(szDSN,
           DSN_CACHE_ServerMaxByteColumnLen,
           "", CacheServerMaxByteColumnLen, sizeof(CacheServerMaxByteColumnLen),
           ODBC_INI);
   rc = SQLGetPrivateProfileString(szDSN,
           DSN_CACHE_ServerMaxVbytColumnLen,
           "", CacheServerMaxVbytColumnLen, sizeof(CacheServerMaxVbytColumnLen),
           ODBC_INI);
   return;
}

/*
**  GetCapabilitiesFromServer
**
**  On entry: hdbc       =  ODBC connection handle (HDBC)
**            ServerName -> Name of server that we are connected to
**            ServerType -> Class of server that we are connected to ("INGRES")
**
**  Returns:  nothing
**
**  Notes:    Get the namecase, tablename length, etc., into memory from connect
*/
static void GetCapabilitiesFromServer(HDBC hdbc, char * ServerName, char * ServerType)
{
   SQLRETURN   rc;
   SQLUSMALLINT num;
   HSTMT   hstmt = NULL;
   char typeName[23];

   strcpy(CacheServerName, ServerName);
   strcpy(CacheServerType, ServerType);

   rc = SQLGetInfo(hdbc, SQL_DBMS_VER, 
             (SQLPOINTER) CacheServerVersion, sizeof(CacheServerVersion), NULL);

   rc = SQLGetInfo(hdbc, SQL_IDENTIFIER_CASE,
             (SQLPOINTER) &num, sizeof(num), NULL);
   if      (num == SQL_IC_UPPER)  *CacheServerNameCase      = 'U';
   else if (num == SQL_IC_LOWER)  *CacheServerNameCase      = 'L';
   else                           *CacheServerNameCase      = 'M';

   rc = SQLGetInfo(hdbc, SQL_QUOTED_IDENTIFIER_CASE,
             (SQLPOINTER) &num, sizeof(num), NULL);
   if      (num == SQL_IC_UPPER)  *CacheServerDelimitedCase = 'U';
   else if (num == SQL_IC_LOWER)  *CacheServerDelimitedCase = 'L';
   else                           *CacheServerDelimitedCase = 'M';

   rc = SQLGetInfo(hdbc, SQL_MAX_COLUMNS_IN_TABLE,
             (SQLPOINTER) &num, sizeof(num), NULL);
   _itoa(num, CacheServerMaxColumns,       10);

   rc = SQLGetInfo(hdbc, SQL_MAX_OWNER_NAME_LEN,
             (SQLPOINTER) &num, sizeof(num), NULL);
   _itoa(num, CacheServerMaxSchemaNameLen, 10);

   rc = SQLGetInfo(hdbc, SQL_MAX_TABLE_NAME_LEN,
             (SQLPOINTER) &num, sizeof(num), NULL);
   _itoa(num, CacheServerMaxTableNameLen,  10);

   rc = SQLGetInfo(hdbc, SQL_MAX_COLUMN_NAME_LEN,
             (SQLPOINTER) &num, sizeof(num), NULL);
   _itoa(num, CacheServerMaxColumnNameLen, 10);

   rc = SQLGetInfo(hdbc, SQL_MAX_ROW_SIZE, 
             (SQLPOINTER) &num, sizeof(num), NULL);
   _itoa(num, CacheServerMaxRowLen, 10);

   rc = SQLAllocStmt(hdbc, &hstmt);
			            
   rc = SQLGetTypeInfo(hstmt, SQL_ALL_TYPES);   
   while(TRUE) 
    {
        rc = SQLFetch(hstmt);
        if (rc == SQL_NO_DATA_FOUND || rc == SQL_ERROR)
	        break;
        rc = SQLGetData(hstmt, 1, SQL_C_CHAR, typeName, 23, NULL);
        if(strcmp(typeName, "CHAR")==0)
            rc = SQLGetData(hstmt, 3, SQL_C_CHAR, CacheServerMaxCharColumnLen, 32, NULL);    
        else if(strcmp(typeName, "VARCHAR")==0)
            rc = SQLGetData(hstmt, 3, SQL_C_CHAR, CacheServerMaxVchrColumnLen, 32, NULL);
        else if(strcmp(typeName, "BINARY")==0)       
            rc = SQLGetData(hstmt, 3, SQL_C_CHAR, CacheServerMaxByteColumnLen, 32, NULL);
        else if(strcmp(typeName, "VARBINARY")==0)       
            rc = SQLGetData(hstmt, 3, SQL_C_CHAR, CacheServerMaxVbytColumnLen, 32, NULL);  
    }
   if (hstmt)
       rc = SQLFreeStmt(hstmt, SQL_DROP);
 
   return;
}


/*
**  AddListEntry
**
**  On entry: plist     -> list
**            plistsize -> current size of list, max size of list
**            pname     -> to be added
**
**  Returns:  -> list entry or NULL if it could not be added
**
**  Notes:    creates an entry in list of
**               a flag byte that can be used by the caller
**               a character string of the name
**               a null terminator ('\0')
*/
static char * AddListEntry(char * plist, int plistsize[2], char * pname)
{
    int   length = strlen(pname);
    char *pitem;

    if (length == 0)
        return NULL;

    if (plistsize[0] + length + 2 > plistsize[1])
        return NULL;

    pitem = plist+plistsize[0];

    strcpy(pitem+1, pname);

    plistsize[0] += (length+2);

    return pitem;
}


/*
**  FindListEntry using pname as the key (case insensitive)
**
**  On entry: plist     -> list
**            plistsize -> current size of list, max size of list
**            pname     -> to be added
**
**  Returns:  -> list entry found or NULL if it could not be added
**
**  Notes:    locates an entry in list of
**               a flag byte that can be used by the caller
**               a character string of the name
**               a null terminator ('\0')
*/
static char * FindListEntry(char * plist, int plistsize[2], char * pname)
{
    int   length = 0;
    int   listsize = 0;
    char *pitem;

    for (;;)  /* convenient for loop to break out of */
       {
        if (listsize >= plistsize[0])
            return NULL;

        pitem = plist + listsize;

        if (_stricmp(pitem+1, pname) == 0)
            break;

        length = strlen(pitem+1);
        listsize += (length+2);

       }  /* end convenient for loop to break out of */

    return pitem;
}


/*
**  VnodeSetsPromptUID
**
**  On entry: hDlg     =  dialog handle
**            plist     -> list
**            plistsize -> current size of list, max size of list
**
**  Returns:  -> FALSE if PromptUID has been set UNCHECKED
**               TRUE  if PromptUID has been set CHECKED
**
**  Notes:    locates an entry in list of
**               a flag byte that can be used by the caller
**               a character string of the name
**               a null terminator ('\0')
*/
static BOOL VnodeSetsPromptUID(HWND hDlg, char * plist, int plistsize[2])
{
    int   length = 0;
    char  szNodeName[MAX_OBJECT_NAME] = "";
    char *pitem;

    GetDlgItemText(hDlg, IDC_VNODE, szNodeName, sizeof(szNodeName));
       /* get vnode server name from edit box control */

    if (*szNodeName == '\0'  ||  *szNodeName == ' ')  /* if none, default */
       {CheckDlgButton(hDlg, IDC_PROMPTUIDPWD, BST_UNCHECKED);
        return 0;
       }

    pitem = FindListEntry(plist, plistsize, szNodeName);
    if (pitem  && (*pitem & isInIILoginFile))
       {CheckDlgButton(hDlg, IDC_PROMPTUIDPWD, BST_UNCHECKED);
        return 0;
       }
    else
       {CheckDlgButton(hDlg, IDC_PROMPTUIDPWD, BST_CHECKED);
        return 1;
       }
}

/*
**  HideOrShowRole
**
**  On entry: hDlg          =  dialog handle
**            szServerType  -> to be added
**
**  Returns:  nothing
**
**  Notes:    locates an entry in list of
**               a flag byte that can be used by the caller
**               a character string of the name
**               a null terminator ('\0')
*/
static VOID HideOrShowRole(HWND hDlg, char *szServerTypeParm)
{
    char szServerType[LEN_SERVERTYPE + 1];
    char *lpsz;
    int   nCmdShow;
    int   nCurShow = IsWindowVisible(GetDlgItem (hDlg, IDC_ROLENAME))?
                         SW_SHOWNORMAL : SW_HIDE;  /* current state of control */

    strcpy(szServerType, szServerTypeParm);  /* make a work copy */
    if ((lpsz=strchr(szServerType,' ')) != NULL)
       *lpsz = '\0';    /* strip any doc from servertype */

    if (strcmp(szServerType, "DB2")  == 0     ||
        strcmp(szServerType, "DCOM") == 0     ||
        strcmp(szServerType, "IDMS") == 0     ||
        strcmp(szServerType, "IMS")  == 0     ||
        strcmp(szServerType, "VANT") == 0     ||
        strcmp(szServerType, "VSAM") == 0)
            nCmdShow = SW_HIDE;
    else  /* szServerType == "INGRES" or others */
            nCmdShow = SW_SHOWNORMAL;

/*  if (nCmdShow == nCurShow)   ** if target visibility already there then return */
/*      return; */

    if (nCmdShow == SW_HIDE)   /* if target visibility == HIDE */
       {SetDlgItemText (hDlg, IDC_ROLENAME, "");   /* clear out the text so that */
        SetDlgItemText (hDlg, IDC_ROLEPWD,  "");   /* is no "bleed-through" from an */
                                                   /* invisible control to a visible one*/
       }

    ShowWindow(GetDlgItem (hDlg, IDC_ROLENAME),       nCmdShow); /* hide or show  */
    ShowWindow(GetDlgItem (hDlg, IDC_ROLENAME_TEXT),  nCmdShow); /*  the controls */
    ShowWindow(GetDlgItem (hDlg, IDC_ROLEPWD),        nCmdShow);
    ShowWindow(GetDlgItem (hDlg, IDC_ROLEPWD_TEXT),   nCmdShow);

    return;
}

/*
**  HideOrShowGroup
**
**  On entry: hDlg          =  dialog handle
**            szServerType  -> to be added
**
**  Returns:  nothing
**
**  Notes:    locates an entry in list of
**               a flag byte that can be used by the caller
**               a character string of the name
**               a null terminator ('\0')
*/
static VOID HideOrShowGroup(HWND hDlg, char *szServerTypeParm)
{
    char szServerType[LEN_SERVERTYPE + 1];
    char *lpsz;
    int   nCmdShow;
 
    strcpy(szServerType, szServerTypeParm);  /* make a work copy */
    if ((lpsz=strchr(szServerType,' ')) != NULL)
       *lpsz = '\0';    /* strip any doc from servertype */

    if (strcmp(szServerType, "DB2")   == 0     ||
        strcmp(szServerType, "DCOM")  == 0     ||
        strcmp(szServerType, "IDMS")  == 0     ||
        strcmp(szServerType, "IMS")   == 0     ||
        strcmp(szServerType, "VANT")  == 0     ||
        strcmp(szServerType, "VSAM")  == 0     ||
        strcmp(szServerType, "ALB")   == 0     ||
        strcmp(szServerType, "RDB")   == 0     ||
        strcmp(szServerType, "SYBASE")== 0     ||
        strcmp(szServerType, "ORACLE")== 0     ||
        strcmp(szServerType, "MSSQL") == 0     ||
        strcmp(szServerType, "INFORMIX") == 0  ||
        strcmp(szServerType, "DB2UDB")== 0)

            nCmdShow = SW_HIDE;
    else  /* szServerType == "INGRES" or others */
            nCmdShow = SW_SHOWNORMAL;

/*  if (nCmdShow == nCurShow)   ** if target visibility already there then return */
/*      return; */

    if (nCmdShow == SW_HIDE)   /* if target visibility == HIDE */
        SetDlgItemText (hDlg, IDC_GROUP, "");   /* clear out the text so that */
                                                /* is no "bleed-through" from an */
                                                   /* invisible control to a visible one*/

    ShowWindow(GetDlgItem (hDlg, IDC_GROUP),       nCmdShow); /* hide or show  */
    ShowWindow(GetDlgItem (hDlg, IDC_GROUP_TEXT),  nCmdShow); /*  the controls */

    return;
}

/*
**  HideOrShowProcUpdt
**
**  On entry: hDlg          =  dialog handle
**
**  Returns:  nothing
**
*/
static VOID HideOrShowProcUpdt(HWND hDlg)
{
    int   nCmdShow;
    int   nCurShow = IsWindowVisible(GetDlgItem (hDlg, 
              IDC_ALLOWPROCEDUREUPDATE))? SW_SHOWNORMAL : SW_HIDE;  

    if (!CMcmpnocase(&szReadOnly[0],"Y"))
        nCmdShow = SW_SHOWNORMAL;
    else
        nCmdShow = SW_HIDE;

    if (nCmdShow == SW_HIDE)   /* if target visibility == HIDE */
        SetDlgItemText (hDlg, IDC_ALLOWPROCEDUREUPDATE, ""); 
    else
        SetDlgItemText (hDlg, IDC_ALLOWPROCEDUREUPDATE, "Allow update in database procedure (Read Only Driver only)"); 


    ShowWindow(GetDlgItem (hDlg, IDC_ALLOWPROCEDUREUPDATE), nCmdShow); 
    return;
}

/*
**  isIngresServerType
**
**  On entry: szServerType
**
**  Returns:  TRUE if Ingres or user-defined Ingres server class
**
*/
static BOOL isIngresServerType(char *szServerTypeParm)
{
    char szServerType[LEN_SERVERTYPE + 1];
    char *lpsz;

    strcpy(szServerType, szServerTypeParm);  /* make a work copy */
    if ((lpsz=strchr(szServerType,' ')) != NULL)
       *lpsz = '\0';    /* strip any doc from servertype */

    if (strcmp(szServerType, "ALB")     == 0  ||  /* based on server type */
        strcmp(szServerType, "DB2")     == 0  ||
        strcmp(szServerType, "DCOM")    == 0  ||
        strcmp(szServerType, "IDMS")    == 0  ||
        strcmp(szServerType, "IMS")     == 0  ||
        strcmp(szServerType, "INFORMIX")== 0  ||
        strcmp(szServerType, "MSSQL")   == 0  ||
        strcmp(szServerType, "ORACLE")  == 0  ||
        strcmp(szServerType, "RMS")     == 0  ||
        strcmp(szServerType, "RDB")     == 0  ||
        strcmp(szServerType, "SYBASE")  == 0  ||
        strcmp(szServerType, "VANT")    == 0  ||
        strcmp(szServerType, "VSAM")    == 0  ||
        strcmp(szServerType, "DB2UDB")  == 0 )
           return FALSE;

     return TRUE;
}
