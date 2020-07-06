/*
**  Copyright (C) 2005-2006 Ingres Corporation. All Rights Reserved.
*/

/*
**  Source   : xdlguser.cpp , Implementation File
**
**  Project  : Ingres II / VDBA
**
**  Author   : Sotheavut UK (uk$so01)
**
**  Purpose  : Dialog box for Create/Alter User
**
** History:
** 26-Oct-1999 (uk$so01)
**    Created. Rewrite the dialog code, in C++/MFC.
** 16-feb-2000 (somsa01)
**    In InitClassMember(), modified setting of
**    m_pParam->m_strDefaultGroup and m_pParam->m_strProfileName to
**    prevent compiler errors.
** 24-Feb-2000 (uk$so01)
**    Bug #100557: Remove the (default profile) from the create user dialog box
** 14-Sep-2000 (uk$so01)
**    Bug #102609: Missing Pop help ID when destroying the dialog
**    15-Jun-2001(schph01)
**      SIR 104991 initialize and manage new controles and variables to manage
**      the grants for rmcmd
** 30-Jul-2002 (schph01)
**    Bug #108381 On OK button verify that the password and confirm password
**    are identical.
** 07-Oct-2002 (schph01)
**    Bug 108883 Disable the m_ctrlCheckGrantRMCMD control if current user is
**    the owner of rmcmd objects.
** 10-Sep-2004 (uk$so01)
**    BUG #113002 / ISSUE #13671785 & 13671608, Removal of B1 Security
**    from the back-end implies the removal of some equivalent functionalities
**    from VDBA.
**  07-Apr-2005 (srisu02)
**   Add functionality for 'Delete password' checkbox in 'Alter User' dialog box
** 30-May-2005 (lakvi01)
**    In InitClassMember(), make sure that the database list is empty before adding
**    the name of the database.
**/

#include "stdafx.h"
#include "rcdepend.h"
#include "xdlguser.h"
extern "C" 
{
#include "dbaset.h" // GetOIVers()
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
static const int LAYOUT_NUMBER = 3;

CxDlgUser::CxDlgUser(CWnd* pParent /*=NULL*/)
	: CDialog(CxDlgUser::IDD, pParent)
{
	m_pParam = NULL;
	//{{AFX_DATA_INIT(CxDlgUser)
	m_bAllEvents = FALSE;
	m_bQueryText = FALSE;
	m_bExternalPassword = FALSE;
	m_strGroup = _T("");
	m_strProfile = _T("");
	m_strUserName = _T("");
	m_strExpireDate = _T("");
	m_strLimitSecurityLabel = _T("");
	m_strPassword = _T("");
	m_strConfirmPassword = _T("");
	m_bGrantRmcmd = FALSE;
	m_bRemovePwd = FALSE;
	//}}AFX_DATA_INIT
	m_strOldPassword = _T("");
}

CxDlgUser::CxDlgUser(CWnd* pParent /*=NULL*/, BOOL bAlter)
	: CDialog(IDD_XUSER_ALTER, pParent)
{
	//
	// It should be the same as the first constructor:
	m_pParam = NULL;
	m_bAllEvents = FALSE;
	m_bQueryText = FALSE;
	m_bExternalPassword = FALSE;
	m_strGroup = _T("");
	m_strProfile = _T("");
	m_strUserName = _T("");
	m_strExpireDate = _T("");
	m_strLimitSecurityLabel = _T("");
	m_strPassword = _T("");
	m_strConfirmPassword = _T("");

	m_strOldPassword = _T("");
}

void CxDlgUser::InitClassMember(BOOL bInitMember)
{
	//
	// Must be set first:
	ASSERT (m_pParam);
	if (!m_pParam)
		return;
	if (bInitMember)
	{
		m_bGrantRmcmd = m_pParam->m_bGrantUser4Rmcmd;
		m_bRemovePwd = m_pParam->m_bRemovePwd;
		m_bAllEvents = m_pParam->GetAllEvent();
		m_bQueryText = m_pParam->GetQueryText();
		m_bExternalPassword = m_pParam->m_bExternalPassword;

		if (m_pParam->m_strDefaultGroup.CompareNoCase(m_pParam->m_strNoGroup) == 0 ||
			m_pParam->m_strDefaultGroup.IsEmpty())
		{
			m_strGroup = _T("");
		}
		else
			m_strGroup = m_pParam->m_strDefaultGroup;
		if (m_pParam->m_strProfileName.CompareNoCase(m_pParam->m_strNoProfile) == 0 ||
			m_pParam->m_strProfileName.IsEmpty())
		{
			m_strProfile = m_pParam->m_strNoProfile;
		}
		else
			m_strProfile = m_pParam->m_strProfileName;
		m_strUserName = m_pParam->m_strName;
		m_strExpireDate = m_pParam->m_strExpireDate;
		m_strLimitSecurityLabel = m_pParam->m_strLimitSecLabels;
		m_strPassword = m_pParam->m_strPassword;
		m_strConfirmPassword = m_pParam->m_strPassword;

		if (!m_pParam->m_bCreate)
			m_strOldPassword = m_pParam->m_strOldPassword;

		int i = 0;
		POSITION pos = m_pParam->m_listPrivileges.GetHeadPosition();
		while (pos != NULL && i < m_pParam->GetMaxPrivilege())
		{
			CaURPPrivilegesItemData* pObj = m_pParam->m_listPrivileges.GetNext(pos);
			pObj->m_bUser = m_pParam->m_Privilege[i];
			pObj->m_bDefault = m_pParam->m_bDefaultPrivilege[i];
			i++;
		}
	}
	else
	{
		m_pParam->m_bGrantUser4Rmcmd = m_bGrantRmcmd ;
		m_pParam->m_bRemovePwd = m_bRemovePwd ;
		m_pParam->m_strName = m_strUserName;
		if (m_strGroup.CompareNoCase(m_pParam->m_strNoGroup) == 0)
		    m_pParam->m_strDefaultGroup = (LPCTSTR)_T("");
		else
		    m_pParam->m_strDefaultGroup = m_strGroup;
		if (m_strProfile.CompareNoCase(m_pParam->m_strNoProfile) == 0)
		    m_pParam->m_strProfileName = (LPCTSTR)_T("");
		else
		    m_pParam->m_strProfileName = m_strProfile;

		m_pParam->m_strPassword   = m_strPassword;
		m_pParam->m_strExpireDate = m_strExpireDate;
		m_pParam->m_strLimitSecLabels = m_strLimitSecurityLabel;
		m_pParam->m_bExternalPassword = m_bExternalPassword;

		if (!m_pParam->m_bCreate)
			m_pParam->m_strOldPassword= m_strOldPassword;
		
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
		
		if (m_pParam->m_bCreate)
		{
			CString strObj;
			int nCount = m_cCheckListBoxDatabase.GetCount();
			for (i = 0; i < nCount; i++)
			{
				if (m_cCheckListBoxDatabase.GetCheck(i))
				{
					m_cCheckListBoxDatabase.GetText (i, strObj);
					if(!m_pParam->m_strListDatabase.IsEmpty()) 
							m_pParam->m_strListDatabase.RemoveAll();
					m_pParam->m_strListDatabase.AddTail(strObj);
				}
			}
		}
	}
}



void CxDlgUser::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CxDlgUser)
	DDX_Control(pDX, IDC_STATIC_RMCMD_TITLE, m_RmcmdTitle);
	DDX_Control(pDX, IDC_CHECK_GRANT_REVOKE_RMCMD, m_ctrlCheckGrantRMCMD);
	DDX_Control(pDX, DELETE_OLD_PASSWD, m_ctrlRemovePwd);
	DDX_Control(pDX, IDC_STATIC_PARTIAL_GRANT, m_ctrlStaticPartial);
	DDX_Control(pDX, IDC_STATIC_ICON_PARTIALGRANT, m_ctrlIconPartial);
	DDX_Control(pDX, IDC_MFC_EDIT5, m_cEditConfirmPassword);
	DDX_Control(pDX, IDC_MFC_EDIT4, m_cEditPassword);
	DDX_Control(pDX, IDC_MFC_CHECK4, m_cCheckExternalPassword);
	DDX_Control(pDX, IDOK, m_cButtonOK);
	DDX_Control(pDX, IDC_MFC_EDIT1, m_cEditUserName);
	DDX_Control(pDX, IDC_MFC_COMBO2, m_cComboProfile);
	DDX_Control(pDX, IDC_MFC_COMBO1, m_cComboGroup);
	DDX_Check(pDX, IDC_MFC_CHECK1, m_bAllEvents);
	DDX_Check(pDX, IDC_MFC_CHECK2, m_bQueryText);
	DDX_Check(pDX, IDC_MFC_CHECK4, m_bExternalPassword);
	DDX_CBString(pDX, IDC_MFC_COMBO1, m_strGroup);
	DDX_CBString(pDX, IDC_MFC_COMBO2, m_strProfile);
	DDX_Text(pDX, IDC_MFC_EDIT1, m_strUserName);
	DDX_Text(pDX, IDC_MFC_EDIT2, m_strExpireDate);
	DDX_Text(pDX, IDC_MFC_EDIT3, m_strLimitSecurityLabel);
	DDX_Text(pDX, IDC_MFC_EDIT4, m_strPassword);
	DDX_Text(pDX, IDC_MFC_EDIT5, m_strConfirmPassword);
	DDX_Check(pDX, IDC_CHECK_GRANT_REVOKE_RMCMD, m_bGrantRmcmd);
	DDX_Check(pDX, DELETE_OLD_PASSWD, m_bRemovePwd);
	//}}AFX_DATA_MAP
	if (!m_pParam->m_bCreate)
	{
		DDX_Text(pDX, IDC_MFC_EDIT6, m_strOldPassword);
	}
}


BEGIN_MESSAGE_MAP(CxDlgUser, CDialog)
	//{{AFX_MSG_MAP(CxDlgUser)
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_MFC_EDIT1, OnEditChangeUserName)
	ON_BN_CLICKED(IDC_MFC_CHECK4, OnCheckExternalPassword)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CxDlgUser message handlers

BOOL CxDlgUser::OnInitDialog() 
{
	CDialog::OnInitDialog();
	ASSERT (m_pParam && m_pParam->m_nNodeHandle != -1);
	if (!m_pParam || m_pParam->m_nNodeHandle == -1)
	{
		EndDialog (IDCANCEL);
		return FALSE;
	}

	m_cListPrivilege.SetFullRowSelected(FALSE);
	VERIFY (m_cListPrivilege.SubclassDlgItem (IDC_MFC_LIST1, this));
	m_ImageCheck.Create (IDB_CHECK, 16, 1, RGB (255, 0, 0));
	m_cListPrivilege.SetCheckImageList(&m_ImageCheck);
	
	if (GetOIVers() >= OIVERS_30)
	{
		CWnd* pWnd = GetDlgItem(IDC_MFC_STATIC1);
		if (pWnd && pWnd->m_hWnd)
			pWnd->ShowWindow(SW_HIDE);
		pWnd = GetDlgItem(IDC_MFC_EDIT3);
		if (pWnd && pWnd->m_hWnd)
			pWnd->ShowWindow(SW_HIDE);
	}

	if (m_pParam->m_bCreate)
	{
		VERIFY (m_cCheckListBoxDatabase.SubclassDlgItem (IDC_MFC_LIST2, this));
		HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		if (hFont == NULL)
			hFont = (HFONT)GetStockObject(ANSI_VAR_FONT);
		//m_cCheckListBoxDatabase.SendMessage(WM_SETFONT, (WPARAM)hFont);
	}
	if (m_pParam->m_bPartialGrant && !m_pParam->m_bCreate)
	{
		HICON hIcon = theApp.LoadStandardIcon(IDI_EXCLAMATION);
		m_ctrlIconPartial.SetIcon(hIcon);
		m_ctrlIconPartial.Invalidate();
		DestroyIcon(hIcon);
		m_ctrlIconPartial.ShowWindow(SW_SHOW);
		m_ctrlStaticPartial.ShowWindow(SW_SHOW);
		m_RmcmdTitle.ShowWindow(SW_SHOW);
	}
	else
	{
		m_ctrlIconPartial.ShowWindow(SW_HIDE);
		m_ctrlStaticPartial.ShowWindow(SW_HIDE);
		m_RmcmdTitle.ShowWindow(SW_HIDE);
	}

	//
	// Initalize the Column Header of CListCtrl (CuListCtrl)
	//
	LV_COLUMN lvcolumn;
	LSCTRLHEADERPARAMS2 lsp[LAYOUT_NUMBER] =
	{
		{IDS_URP_PRIVILEGE,   128,  LVCFMT_LEFT,   FALSE},
		{IDS_URP_USER,         50,  LVCFMT_CENTER, TRUE },
		{IDS_URP_DEFAULT,      60,  LVCFMT_CENTER, TRUE },
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
	FillGroups();
	FillProfiles();

	OnEditChangeUserName();
	OnCheckExternalPassword();
	if (!m_pParam->m_bCreate)
		m_cEditUserName.EnableWindow (FALSE);
	if ( m_pParam->m_bGrantUser4Rmcmd )
		m_ctrlCheckGrantRMCMD.SetCheck(BST_CHECKED);
	else
		m_ctrlCheckGrantRMCMD.SetCheck(BST_UNCHECKED);
	if ( m_pParam->m_bRemovePwd )
		m_ctrlRemovePwd.SetCheck(BST_CHECKED);
	else
		m_ctrlRemovePwd.SetCheck(BST_UNCHECKED);
	if ( m_pParam->m_bOwnerRmcmdObjects )
		m_ctrlCheckGrantRMCMD.EnableWindow(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CxDlgUser::OnDestroy() 
{
	CDialog::OnDestroy();
	if (m_pParam)
		m_pParam->PopHelpID();
}

void CxDlgUser::OnOK() 
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

	if ((x_strlen (m_strPassword) > 0) && m_bRemovePwd == 1)
	{
		AfxMessageBox (IDS_E_DEL_OLD_PWD, MB_OK|MB_ICONEXCLAMATION); //"The passwords do not match"
		return;
	}
	
	

	InitClassMember (FALSE);

	BOOL bOk = FALSE;
	if (m_pParam->m_bCreate)
		bOk = m_pParam->CreateObject(this);
	else
		bOk = m_pParam->AlterObject(this);
	if (!bOk)
		return;

	bOk = m_pParam->GrantRevoke4Rmcmd(this);
	if (!bOk)
		return;

	CDialog::OnOK();
}


void CxDlgUser::FillPrivileges()
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

void CxDlgUser::FillDatabases()
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

void CxDlgUser::FillGroups()
{
	CStringList listObj;
	m_cComboGroup.ResetContent();
	m_pParam->GetGroups (listObj);
	m_cComboGroup.AddString(m_pParam->m_strNoGroup);

	POSITION pos = listObj.GetHeadPosition();
	while (pos != NULL)
	{
		CString strObj = listObj.GetNext (pos);
		m_cComboGroup.AddString (strObj);
	}
	int idx = 0;
	if (!m_pParam->m_bCreate)
	{
		idx = m_cComboGroup.FindStringExact (-1, m_strGroup);
		if (idx == -1)
			idx = 0;
	}
	m_cComboGroup.SetCurSel(idx);
}

void CxDlgUser::FillProfiles()
{
	CStringList listObj;
	m_cComboProfile.ResetContent();
	m_pParam->GetProfiles (listObj);
	m_cComboProfile.AddString(m_pParam->m_strNoProfile);

	POSITION pos = listObj.GetHeadPosition();
	while (pos != NULL)
	{
		CString strObj = listObj.GetNext (pos);
		//
		// Ignore the string "(default profile)":
		if (INGRESII_IsDefaultProfile (strObj))
			continue;
		m_cComboProfile.AddString (strObj);
	}
	int idx = m_cComboProfile.FindStringExact (-1, m_pParam->m_strNoProfile);

	if (!m_pParam->m_bCreate)
	{
		int idef = m_cComboProfile.FindStringExact (-1, m_strProfile);
		if (idef != -1)
			idx = idef;
	}
	m_cComboProfile.SetCurSel(idx);
}

void CxDlgUser::OnEditChangeUserName() 
{
	CString strItem;
	m_cEditUserName.GetWindowText (strItem);
	strItem.TrimLeft();
	strItem.TrimRight();
	if (strItem.IsEmpty())
		m_cButtonOK.EnableWindow(FALSE);
	else
		m_cButtonOK.EnableWindow(TRUE);
}

void CxDlgUser::OnCheckExternalPassword() 
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
