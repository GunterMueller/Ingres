/*
**
**  Name: perfwiz.cpp
**
**  Description:
**	This file defines the class behaviors for the application.
**
**  History:
**	06-sep-1999 (somsa01)
**	    Created.
**	18-oct-1999 (somsa01)
**	    Added NumCountersSelected, a global variable which keeps track of
**	    counters selected for charting.
**	29-oct-1999 (somsa01)
**	    Added the global variable PersonalCounterID for keeping track
**	    of the counter id form which the personal stuff starts.
*/

#include "stdafx.h"
#include "perfwiz.h"
#include "PropSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HPALETTE		hSystemPalette=0;
HWND			MainWnd;
CString			ChosenVnode, ChosenServer, ChartName="";
CString			ObjectName, ObjectDescription;
BOOL			PersonalGroup, InitFileRead=FALSE, UseExisting=FALSE;
int			CounterID, PersonalCounterID;
DWORD			NumCountersSelected=0;
struct grouplist	*GroupList=NULL;

GroupClass		GroupNames[Num_Groups];
Group			CacheGroup[NUM_CACHE_GROUPS*Num_Cache_Counters];
Group			LockGroup[NUM_LOCK_GROUPS*Num_Lock_Counters];
Group			ThreadGroup[NUM_THREAD_GROUPS*Num_Thread_Counters];
Group			SamplerGroup[NUM_SAMPLER_GROUPS*Num_Sampler_Counters];
PERFCTR			PersonalCtr_Init[NUM_PERSONAL_COUNTERS];

/*
** CPerfwizApp
*/
BEGIN_MESSAGE_MAP(CPerfwizApp, CWinApp)
    //{{AFX_MSG_MAP(CPerfwizApp)
	    // NOTE - the ClassWizard will add and remove mapping macros here.
	    //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/*
** CPerfwizApp construction
*/
CPerfwizApp::CPerfwizApp()
{
}

/*
** The one and only CPerfwizApp object
*/
CPerfwizApp theApp;

/*
** CPerfwizApp initialization
*/
BOOL CPerfwizApp::InitInstance()
{
    INT_PTR	nResponse;

    AfxEnableControlContainer();

    hSystemPalette=0;
#ifdef _AFXDLL
    Enable3dControls();	  /* Call this when using MFC in a shared DLL */
#else
    Enable3dControlsStatic();  /* Call this when linking to MFC statically */
#endif

    PropSheet psDlg("Ingres Performance Monitor Wizard");
    m_pMainWnd = &psDlg;
    nResponse = psDlg.DoModal();
    if (nResponse == IDOK)
    {
    }
    else if (nResponse == IDCANCEL)
    {
    }

    return FALSE;
}
