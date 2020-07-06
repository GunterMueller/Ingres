/*
**  Copyright (C) 2005-2006 Ingres Corporation. All Rights Reserved.
*/

/*
**    Source   : xdlgrole.cpp , Implementation File 
**    Project  : Ingres II / VDBA
**    Author   : Sotheavut UK (uk$so01)
**    Purpose  : Dialog box for Create/Alter Role
**
** History:
**
** 27-Oct-1999 (uk$so01)
**    created, Rewrite the dialog code, in C++/MFC.
** 14-Sep-2000 (uk$so01)
**    Bug #102609: Missing Pop help ID when destroying the dialog
** 30-Jul-2002 (schph01)
**    Bug #108381 On OK button verify that the password and confirm password
**    are identical.
** 10-Sep-2004 (uk$so01)
**    BUG #113002 / ISSUE #13671785 & 13671608, Removal of B1 Security
**    from the back-end implies the removal of some equivalent functionalities
**    from VDBA.
*/

#include "stdafx.h"
#include "rcdepend.h"
#include "xdlgrole.h"
extern "C" 
{
#include "dbaset.h" // GetOIVers()
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
static const int LAYOUT_NUMBER = 2;


CxDlgRole::CxDlgRole(CWnd* pParent /*=NULL*/)
	: CDialog(CxDlgRole::IDD, pParent)
{
	//{{AFX_DATA_INIT(CxDlgRole)
	m_bExternalPassword = FALSE;
	m_strRoleName = _T("");
	m_strLimitSecurityLabel = _T("");
	m_strPassword = _T("");
	m_strConfirmPassword = _T("");
	//}}AFX_DATA_INIT
	m_pParam = NULL;
}

CxDlgRole::CxDlgRole(CWnd* pParent /*=NULL*/, BOOL bAlter)
	: CDialog(IDD_XROLE_ALTER, pParent)
{
	m_bExternalPassword = FALSE;
	m_strRoleName = _T("");
	m_strLimitSecurityLabel = _T("");
	m_strPassword = _T("");
	m_strConfirmPassword = _T("");

	m_pParam = NULL;
}


void CxDlgRole::InitClassMembers(BOOL bInitMember)
{
	//
	// Must be set first:
	ASSERT (m_pParam);
	if (!m_pParam)
		return;
	if (bInitMember)
	{
		m_bAllEvents = m_pParam->GetAllEvent();
		m_bQueryText = m_pParam->GetQueryText();
		m_bExternalPassword = m_pParam->m_bExternalPassword;
		m_strRoleName = m_pParam->m_strName;
		m_strLimitSecurityLabel = m_pParam->m_strLimitSecLabels;
		m_strPassword = m_pParam->m_strPassword;
		m_strConfirmPassword = m_pParam->m_strPassword;
		int i = 0;
		POSITION pos = m_pParam->m_listPrivileges.GetHeadPosition();
		while (pos != NULL && i < m_pParam->GetMaxPrivilege())
		{
			CaURPPrivilegesItemData* pObj = m_pParam->m_listPrivileges.GetNext(pos);
			pObj->m_bUser = m_pParam->m_Privilege[i];
			pObj->m_bDefault = m_pParam->m_bDefaultPrivilege[i];
			i++;
		}
		if (IsWindow (m_hWnd))
		{
			pos = m_pParam->m_strListDatabase.GetHeadPosition();
			while (pos != NULL)
			{
				CString strObj = m_pParam->m_strListDatabase.GetNext(pos);
				m_cCheckListBoxDatabase.SetCheck(strObj);
			}
		}
	}
	else
	{
		m_pParam->m_strName = m_strRoleName;
		m_pParam->m_strPassword   = m_strPassword;
		m_pParam->m_strLimitSecLabels = m_strLimitSecurityLabel;
		m_pParam->m_bExternalPassword = m_bExternalPassword;
		
		int i = 0;
		POSITION pos = m_pParam->m_listPrivileges.GetHeadPosition();
		while (pos != NULL && i < m_pParam->GetMaxPrivilege())
		{
			CaURPPrivilegesItemData* pObj = m_pParam->m_listPrivileges.GetNext(pos);
			m_pParam->m_Privilege[i] = pObj->m_bUser;
			m_pParam->m_bDefaultPrivilege[i] = pObj->m_bDefault;
			i++;
		}

		m_pParam->SetAllEvent(m_bAllEvents);
		m_pParam->SetQueryText(m_bQueryText);
		
		CString strObj;
		if (m_pParam->m_bCreate)
		{
			int nCount = m_cCheckListBoxDatabase.GetCount();
			for (i = 0; i < nCount; i++)
			{
				if (m_cCheckListBoxDatabase.GetCheck(i))
				{
					m_cCheckListBoxDatabase.GetText (i, strObj);
					m_pParam->m_strListDatabase.AddTail(strObj);
				}
			}
		}
	}
}


void CxDlgRole::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CxDlgRole)
	DDX_Control(pDX, IDOK, m_cButtonOK);
	DDX_Control(pDX, IDC_MFC_EDIT5, m_cEditConfirmPassword);
	DDX_Control(pDX, IDC_MFC_EDIT4, m_cEditPassword);
	DDX_Control(pDX, IDC_MFC_EDIT1, m_cEditRoleName);
	DDX_Control(pDX, IDC_MFC_CHECK4, m_cCheckExternalPassword);
	DDX_Check(pDX, IDC_MFC_CHECK1, m_bAllEvents);
	DDX_Check(pDX, IDC_MFC_CHECK2, m_bQueryText);
	DDX_Check(pDX, IDC_MFC_CHECK4, m_bExternalPassword);
	DDX_Text(pDX, IDC_MFC_EDIT1, m_strRoleName);
	DDX_Text(pDX, IDC_MFC_EDIT3, m_strLimitSecurityLabel);
	DDX_Text(pDX, IDC_MFC_EDIT4, m_strPassword);
	DDX_Text(pDX, IDC_MFC_EDIT5, m_strConfirmPassword);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CxDlgRole, CDialog)
	//{{AFX_MSG_MAP(CxDlgRole)
	ON_BN_CLICKED(IDC_MFC_CHECK4, OnCheckExternalPassword)
	ON_EN_CHANGE(IDC_MFC_EDIT1, OnChangeEditRoleName)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CxDlgRole message handlers

BOOL CxDlgRole::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	ASSERT (m_pParam && m_pParam->m_nNodeHandle != -1);
	if (!m_pParam || m_pParam->m_nNodeHandle == -1)
	{
		EndDialog (IDCANCEL);
		return FALSE;
	}
	if (GetOIVers() >= OIVERS_30)
	{
		CWnd* pWnd = GetDlgItem(IDC_MFC_STATIC1);
		if (pWnd && pWnd->m_hWnd)
			pWnd->ShowWindow(SW_HIDE);
		pWnd = GetDlgItem(IDC_MFC_EDIT3);
		if (pWnd && pWnd->m_hWnd)
			pWnd->ShowWindow(SW_HIDE);
	}
	if (!m_pParam->m_bCreate)
		m_cEditRoleName.EnableWindow (FALSE);
	
	m_cListPrivilege.SetFullRowSelected(FALSE);
	VERIFY (m_cListPrivilege.SubclassDlgItem (IDC_MFC_LIST1, this));
	m_ImageCheck.Create (IDB_CHECK, 16, 1, RGB (255, 0, 0));
	m_cListPrivilege.SetCheckImageList(&m_ImageCheck);

	if (m_pParam->m_bCreate)
	{
		VERIFY (m_cCheckListBoxDatabase.SubclassDlgItem (IDC_MFC_LIST2, this));
		HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		if (hFont == NULL)
			hFont = (HFONT)GetStockObject(ANSI_VAR_FONT);
		//m_cCheckListBoxDatabase.SendMessage(WM_SETFONT, (WPARAM)hFont);
	}
	
	//
	// Initalize the Column Header of CListCtrl (CuListCtrl)
	//
	LV_COLUMN lvcolumn;
	LSCTRLHEADERPARAMS2 lsp[LAYOUT_NUMBER] =
	{
		{IDS_URP_PRIVILEGE,   170,  LVCFMT_LEFT,   FALSE},
		{IDS_URP_ROLE,         60,  LVCFMT_CENTER, TRUE },
	};

	CString strHeader;
	memset (&lvcolumn, 0, sizeof (LV_COLUMN));
	lvcolumn.mask = LVCF_FMT|LVCF_SUBITEM|LVCF_TEXT|LVCF_WIDTH;
	for (int i=0; i<LAYOUT_NUMBER; i++)
	{
		strHeader.LoadString (lsp[i].m_nIDS);
		lvcolumn.fmt      = lsp[i].m_fmt;
		lvcolumn.pszText  = (LPTSTR)(LPCTSTR)strHeader;
		lvcolumn.iSubItem = i;
		lvcolumn.cx       = lsp[i].m_cxWidth;
		m_cListPrivilege.InsertColumn (i, &lvcolumn, lsp[i].m_bUseCheckMark);
	}

	m_pParam->PushHelpID();
	SetWindowText(m_pParam->GetTitle());

	FillDatabases();
	FillPrivileges();
	OnChangeEditRoleName();
	OnCheckExternalPassword();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CxDlgRole::OnOK() 
{
	//
	// Construct the parameter for creating object:
	//
	UpdateData (TRUE);

	if ( m_strConfirmPassword != m_strPassword)
	{
		AfxMessageBox (IDS_E_PASSWORD_ERROR, MB_OK|MB_ICONEXCLAMATION); //"The passwords do not match"
		return;
	}

	InitClassMembers(FALSE);

	BOOL bOk = FALSE;
	if (m_pParam->m_bCreate)
		bOk = m_pParam->CreateObject(this);
	else
		bOk = m_pParam->AlterObject(this);
	if (!bOk)
		return;
	CDialog::OnOK();
}

void CxDlgRole::OnCheckExternalPassword() 
{
	int nCheck = m_cCheckExternalPassword.GetCheck();
	if (nCheck == 1)
	{
		m_cEditConfirmPassword.EnableWindow(FALSE);
		m_cEditPassword.EnableWindow(FALSE);
	}
	else
	{
		m_cEditConfirmPassword.EnableWindow(TRUE);
		m_cEditPassword.EnableWindow(TRUE);
	}
}

void CxDlgRole::OnChangeEditRoleName() 
{
	CString strItem;
	m_cEditRoleName.GetWindowText (strItem);
	strItem.TrimLeft();
	strItem.TrimRight();
	if (strItem.IsEmpty())
		m_cButtonOK.EnableWindow(FALSE);
	else
		m_cButtonOK.EnableWindow(TRUE);
}


void CxDlgRole::FillPrivileges()
{
	TCHAR tchszText[] =_T("");
	int nCount = 0;
	LV_ITEM lvitem;
	CaURPPrivilegesItemData* pItem = NULL;
	CTypedPtrList< CObList, CaURPPrivilegesItemData* >& lp = m_pParam->m_listPrivileges;
	memset (&lvitem, 0, sizeof(lvitem));
	m_cListPrivilege.DeleteAllItems();
	POSITION pos = lp.GetHeadPosition();
	while (pos != NULL)
	{
		pItem = lp.GetNext (pos);

		lvitem.mask   = LVIF_PARAM|LVIF_TEXT;
		lvitem.pszText= tchszText;
		lvitem.iItem  = nCount;
		lvitem.lParam = (LPARAM)pItem;

		m_cListPrivilege.InsertItem (&lvitem);
		nCount++;
	}
	m_cListPrivilege.DisplayPrivileges();
}

void CxDlgRole::FillDatabases()
{
	if (!m_pParam->m_bCreate)
		return;
	CStringList listObj;
	m_cCheckListBoxDatabase.ResetContent();
	m_pParam->GetDatabases (listObj);

	POSITION pos = listObj.GetHeadPosition();
	while (pos != NULL)
	{
		CString strObj = listObj.GetNext (pos);
		m_cCheckListBoxDatabase.AddString (strObj);
	}
}


void CxDlgRole::OnDestroy() 
{
	CDialog::OnDestroy();
	if (m_pParam)
		m_pParam->PopHelpID();
}
