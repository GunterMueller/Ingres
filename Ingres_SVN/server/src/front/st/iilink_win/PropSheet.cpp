/*
**
**  Name: PropSheet.cpp
**
**  Description:
**	This file contains all functions needed For the Property Sheet.
**
**  History:
**	23-apr-1999 (somsa01)
**	    Created.
*/

#include "stdafx.h"
#include "iilink.h"
#include "PropSheet.h"
#include "MainPage.h"
#include "OptionsPage.h"
#include "UserUdts.h"
#include "FinalPage.h"

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
    AddPage(new MainPage(this));
    AddPage(new COptionsPage(this));
    AddPage(new UserUdts(this));
    AddPage(new FinalPage(this));
    m_psh.dwFlags &= ~(PSH_HASHELP);
    SetWizardMode();
}

PropSheet::~PropSheet()
{
}

BEGIN_MESSAGE_MAP(PropSheet, CPropertySheet)
    //{{AFX_MSG_MAP(PropSheet)
	// NOTE - the ClassWizard will add and remove mapping macros here.
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*
** PropSheet message handlers
*/
