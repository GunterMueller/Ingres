/*
**
** Name: PropSheet.cpp
**
** Description:
**	This file contains the routines for implememting the application's
**	Property sheet.
**
** History:
**	06-sep-1999 (somsa01)
**	    Created.
**	15-oct-1999 (somsa01)
**	    The Server selection page is now the final page.
**	29-oct-1999 (somsa01)
**	    Added the Counter Creation Property Page.
*/

#include "stdafx.h"
#include "perfwiz.h"
#include "PropSheet.h"
#include "IntroPage.h"
#include "ServerSelect.h"
#include "FinalPage.h"
#include "PersonalGroup.h"
#include "CounterCreation.h"
#include "PersonalCounter.h"
#include "PersonalHelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
** PropSheet
*/
IMPLEMENT_DYNAMIC(PropSheet, CPropertySheet)

PropSheet::PropSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

PropSheet::PropSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
    AddPage(new CIntroPage(this));
    AddPage(new CPersonalGroup(this));
    AddPage(new CCounterCreation(this));
    AddPage(new CPersonalCounter(this));
    AddPage(new CPersonalHelp(this));
    AddPage(new CFinalPage(this));
    AddPage(new CServerSelect(this));
    m_psh.dwFlags |= PSH_HASHELP;
    SetWizardMode();
}

PropSheet::~PropSheet()
{
}

BEGIN_MESSAGE_MAP(PropSheet, CPropertySheet)
    //{{AFX_MSG_MAP(PropSheet)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*
** PropSheet message handlers
*/
