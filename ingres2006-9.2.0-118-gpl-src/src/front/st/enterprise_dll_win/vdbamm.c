/*
**  Copyright (c) 2001, 2006 Ingres Corporation.
*/

/*
**  Name: vdbamm.c
**
**  Description:
**	This file becomes the DLL for the IngresIIVdba Merge Module. It
**	contains custom actions that are required to be executed.
**
**  History:
**	22-Feb-2007 (drivi01)
**	    Created.
**
*/




#include <windows.h>
#include <msi.h>
#include <msiquery.h>
#include <string.h>
#include <stdio.h>


/*
**  Name: ingres_remove_ivmfromstartup
**
**  Description:
**	Removes IVM from Startup folder on Vista.
**
**  History:
**	22-Feb-2007 (drivi01)
**	    Created.
*/
ingres_remove_ivmfromstartup(MSIHANDLE hInstall)
{
	MSIHANDLE msih, hView, hRec;
	char cmd[MAX_PATH+1];

	if (IsVista())
	{
	    msih = MsiGetActiveDatabase(hInstall);
   	    if (!msih)
		return FALSE;
	    sprintf(cmd, "SELECT * from Shortcut WHERE Directory_ = 'StartupFolder'");
	
	    if (!(MsiDatabaseOpenView(msih, cmd, &hView) == ERROR_SUCCESS))
		return FALSE;

	    if (!(MsiViewExecute(hView, 0) == ERROR_SUCCESS))
		return FALSE;
		
	    while ((MsiViewFetch(hView, &hRec) == ERROR_SUCCESS))
	        if (!(MsiViewModify(hView, MSIMODIFY_DELETE, hRec) == ERROR_SUCCESS))
				return FALSE;


	    if (!(MsiCloseHandle(hRec) == ERROR_SUCCESS))
		return FALSE;

	    if (!(MsiViewClose( hView ) == ERROR_SUCCESS))
		return FALSE;

	    if (!(MsiCloseHandle(hView) == ERROR_SUCCESS))
		return FALSE;
		/* Commit changes to the MSI database */
	    if (!(MsiDatabaseCommit(msih)==ERROR_SUCCESS))
	       return FALSE;
	
	    if (!(MsiCloseHandle(msih)==ERROR_SUCCESS))
	       return FALSE;
	}

	return ERROR_SUCCESS;


}
