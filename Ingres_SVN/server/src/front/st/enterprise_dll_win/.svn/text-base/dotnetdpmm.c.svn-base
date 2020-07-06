/*
**  Copyright (c) 2001 Ingres Corporation
*/

/*
**  Name: dotnetdpmm.c
**
**  Description:
**	This file becomes the DLL for the IngresDotNetDataProvider Merge Module.
**	It contains custom actions that are required to be executed.
**
**  History:
**	21-Apr-2004 (penga03)
**	    Created.
**	29-Apr-2004 (penga03)
**	    Use INGRES_LIC_LOC instead of SourceDir because for the installation 
**	    id other than II, SourceDir won't work.
**	22-sep-2004 (penga03)
**	    Changed INGRES_LIC_LOC to INGRES_CDIMAGE.
**	08-march-2005 (penga03)
**	    Get the value of Cdimage from the registry if INGRES_CDIMAGE is not 
**	    defined.
**	14-june-2005 (penga03)
**	    Moved IsDotNETFXInstalled() to commonmm.c.
**	16-Nov-2006 (drivi01)
**	    Expanded functionality of ingres_install_dotnetfx
**	    to detect and install .NET Framework 2.0.
**	10-Jan-2007 (drivi01)
**	    If user has no .NET Framework installed and are prompted to install 1.1
**	    first and agree to install it, they will never get a prompt for 2.0 even
**	    though they should.  Fixed the routine to continue on inside the function
**	    instead of returning on successful install completion of 1.1 Framework.
**	04-Oct-2007 (drivi01)
**	    Update routines to generate only one message for required Frameworks.
**          The versions of Framework(s) will be updated dynamically.  Update
**	    message to specify that user needs to exit install before attempting
**	    to install the Framework(s).
*/

#include <windows.h>
#include <msi.h>
#include <msiquery.h>
#include <string.h>
#include <stdio.h>
#include "commonmm.h"

/*
**  Name:ingres_install_dotnetfx
**	
**  Description:
**	Install Microsoft .NET Framework 1.1.
**
**  History:
**	21-apr-2004 (penga03)
**	    Created.
**	10-Jul-2007 (drivi01)
**	    If .NET Data Provider Package is selected when Ingres 
**	    is being installed from a network image, User is
**	    never prompted to install .NET Framework packages.
**	    This change forces .NET Framework prompts even
**	    if installation isn't local.
**	    Fixed spelling error for INGRES_CONTINUE property.
**	03-Dec-2007 (drivi01)
**	    Modified routines to eliminate requirement for 
**	    .NET Framework 1.1.  It is no longer needed.
**	    Support for Data Provider 1.1 is being dropped.
*/

UINT __stdcall 
ingres_install_dotnetfx(MSIHANDLE hInstall) 
{
    INSTALLSTATE iInstalled, iAction;
    char buf[MAX_PATH+1], cmd[MAX_PATH+1], cmd2[MAX_PATH+1], 
			version[MAX_PATH], message[MAX_PATH];
    DWORD nret;
    HKEY hRegKey;
    char id[3], guid[128];
    int idx;
    char *p;
    BOOL bAnswer;
    nret=sizeof(buf); *buf=0;
    *version = '\0';

    sprintf(cmd, "http://www.microsoft.com/downloads");

    MsiSetProperty(hInstall, "INGRES_CONTINUE", "YES");

    if (!MsiGetFeatureState(hInstall, "IngresDotNETDataProvider", &iInstalled, &iAction))
    {
		if ((iAction==INSTALLSTATE_LOCAL || iAction == INSTALLSTATE_SOURCE 
			|| iAction == INSTALLSTATE_DEFAULT) && !IsDotNETFX20Installed())
		{
			MsiSetProperty(hInstall, "INGRES_CONTINUE", "NO");
			if (*version == NULL)
				sprintf(version, "%s", "2.0");
			else
			{
				sprintf(buf, " and %s", "2.0");
				strcat(version, buf);
			}
		}
		if (IsDotNETFX20Installed())
		{
			MsiSetProperty(hInstall, "INGRES_CONTINUE", "YES");
			goto complete;
		}

		sprintf(message, "The Ingres .NET Data Provider component requires Microsoft .NET Framework %s which could not be found on your system.\r\nTo obtain the Microsoft .NET Framework %s Package(s) from the Microsoft website click OK and exit Ingres installation \r\nwizard, or click Cancel and uncheck the .NET Data Provider component to continue.\r\n", version, version);
		bAnswer = AskUserOkCancel(hInstall, message);
		if (bAnswer)
		{
			ExecuteShellEx(cmd);
		}
		
	}
    else
	    MsiSetProperty(hInstall, "INGRES_CONTINUE", "YES");

complete:
    return ERROR_SUCCESS;
}
