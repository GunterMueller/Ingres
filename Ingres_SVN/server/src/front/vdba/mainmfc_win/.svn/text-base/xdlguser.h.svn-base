/*
**  Copyright (C) 2005-2006 Ingres Corporation. All Rights Reserved.
*/

/*
**    Source   : xdlguser.h , Header File
**    Project  : Ingres II / VDBA
**    Author   : Sotheavut UK (uk$so01)
**    Purpose  : Dialog box for Create/Alter User
**
** History:
**
** 27-Oct-1999 (uk$so01)
**    created, Rewrite the dialog code, in C++/MFC.
** 14-Sep-2000 (uk$so01)
**    Bug #102609: Missing Pop help ID when destroying the dialog
**  07-Apr-2005 (srisu02)
**   Add functionality for 'Delete password' checkbox in 'Alter User' dialog box
**
*/
#if !defined(AFX_XDLGUSER_H__CCC5A422_8B7B_11D3_A316_00609725DDAF__INCLUDED_)
#define AFX_XDLGUSER_H__CCC5A422_8B7B_11D3_A316_00609725DDAF__INCLUDED_
#include "urpectrl.h" // User, Role, Profile list control of privileges
#include "uchklbox.h" // Check list box.
#include "dmlurp.h"


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CxDlgUser : public CDialog
{
public:
	CxDlgUser(CWnd* pParent = NULL);
	CxDlgUser(CWnd* pParent , BOOL bAlter = TRUE);
	void SetParam (CaUser* pParam)
	{
		m_pParam = pParam;
		InitClassMember();
	}
	void InitClassMember(BOOL bInitMember = TRUE);

	// Dialog Data
	//{{AFX_DATA(CxDlgUser)
	enum { IDD = IDD_XUSER };
	CButton	m_RmcmdTitle;
	CButton	m_ctrlCheckGrantRMCMD;
	CButton	m_ctrlRemovePwd;
	CStatic	m_ctrlStaticPartial;
	CStatic	m_ctrlIconPartial;
	CEdit	m_cEditConfirmPassword;
	CEdit	m_cEditPassword;
	CButton	m_cCheckExternalPassword;
	CButton	m_cButtonOK;
	CEdit	m_cEditUserName;
	CComboBox	m_cComboProfile;
	CComboBox	m_cComboGroup;
	BOOL	m_bAllEvents;
	BOOL	m_bQueryText;
	BOOL	m_bExternalPassword;
	CString	m_strGroup;
	CString	m_strProfile;
	CString	m_strUserName;
	CString	m_strExpireDate;
	CString	m_strLimitSecurityLabel;
	CString	m_strPassword;
	CString	m_strConfirmPassword;
	BOOL	m_bGrantRmcmd;
	BOOL	m_bRemovePwd;
	//}}AFX_DATA
	CString m_strOldPassword;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CxDlgUser)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	CaUser* m_pParam;
	CImageList m_ImageCheck;
	CuCheckListBox m_cCheckListBoxDatabase;
	CuEditableListCtrlURPPrivileges m_cListPrivilege;

	void FillPrivileges();
	void FillDatabases();
	void FillGroups();
	void FillProfiles();


	// Generated message map functions
	//{{AFX_MSG(CxDlgUser)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	virtual void OnOK();
	afx_msg void OnEditChangeUserName();
	afx_msg void OnCheckExternalPassword();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XDLGUSER_H__CCC5A422_8B7B_11D3_A316_00609725DDAF__INCLUDED_)
