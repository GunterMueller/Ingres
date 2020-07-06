/*
**  Copyright (c) 2005 Ingres Corporation. All Rights Reserved.
*/

/*
**  Name: patchcl.c: Functions used by custom actions and 
**                   Patch Installer.
**
**  History:
**		01-Dec-2005 (drivi01)
**			Created.
**		03-Mar-2006 (drivi01)
**			If AppendToLog fails to retrieve II_SYSTEM then
**			we will output logging info to %TEMP% directory.
**
*/

#include <stdio.h>
#include <windows.h>
#include "patchcl.h"


/*
**  Name: checksum
**  
**  Description: Calculates checksum for binaries.
**
**	Parameters:  filename - Path to binary.
**
**  Return Code: Checksum
*/
unsigned long 
checksum(char *filename) 
{ 
  int ch; 
  unsigned long sum = 0; 
  FILE *fp;

  fp = fopen(filename, "r");
  if(fp) 
  { 
    while((ch = fgetc(fp)) != EOF) 
    { 
      sum <<= 1; 
      sum ^= ch; 
    } 
  } 
  return sum; 

} 

/*
**  Name: AppendToLog
**  
**  Description: Prints log information to patch.log and
**               places it in II_SYSTEM/ingres/files assuming
**               II_SYSTEM is defined.
**
**	Parameters:  str - Information to be written to the log.
**
**  Return Code:  
*/
void 
AppendToLog(char *str)
{
    DWORD   dw, rv;
    HANDLE	hLogFile;
    char 	ii_system[MAX_PATH+1];
    char	log_path[MAX_PATH+1];
    char*	pstr = str;
    char	ii_id[II_SIZE];
    char 	szBuf[MAX_PATH];
    HKEY	hRegKey;
        
    if ( !( rv = GetEnvironmentVariable("II_SYSTEM", ii_system, sizeof(ii_system)) ) )
    {
	rv = GetEnvironmentVariable("TEMP", ii_system, sizeof(ii_system));
	sprintf(log_path, "%s\\patch.log", ii_system);
    }
    else
    {
	sprintf(log_path, "%s\\ingres\\files\\patch.log", ii_system);
    }


    if (rv)
    {
	    
    	hLogFile = CreateFile( log_path, GENERIC_READ|GENERIC_WRITE , 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
        if (hLogFile != NULL)
		{
		     SetFilePointer(hLogFile, 0, NULL, FILE_END);
		     WriteFile(hLogFile, str, strlen(str), &dw, NULL); 
		}
		FlushFileBuffers(hLogFile);
		CloseHandle(hLogFile);
    }
}