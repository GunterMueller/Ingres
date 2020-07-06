/*
**  Copyright (C) 2005-2006 Ingres Corporation. All Rights Reserved.
*/

/*
**    Source   : xdlgrole.h , Header File
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
**
*/
#if !defined(AFX_XDLGROLE_H__28A94563_8BC1_11D3_A317_00609725DDAF__INCLUDED_)
#define AFX_XDLGROLE_H__28A94563_8BC1_11D3_A317_00609725DDAF__INCLUDED_
#include "urpectrl.h" // User, Role, Profile list control of privileges
#include "uchklbox.h" // Check list box.
#include "dmlurp.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CxDlgRole : public CDialog
{
public:
	CxDlgRole(CWnd* pParent = NULL);
	CxDlgRole(CWnd* pParent /*=NULL*/, BOOL bAlter);
	void SetParam (CaRole* pParam)
	{
		m_pParam = pParam;
		InitClassMembers();
	}
	void InitClassMembers(BOOL bInitMember = TRUE);

	// Dialog Data
	//{{AFX_DATA(CxDlgRole)
	enum { IDD = IDD_XROLE };
	CButton	m_cButtonOK;
	CEdit	m_cEditConfirmPassword;
	CEdit	m_cEditPassword;
	CEdit	m_cEditRoleName;
	CButton	m_cCheckExternalPassword;
	BOOL	m_bExternalPassword;
	BOOL	m_bAllEvents;
	BOOL	m_bQueryText;
	CString	m_strRoleName;
	CString	m_strLimitSecurityLabel;
	CString	m_strPassword;
	CString	m_strConfirmPassword;
	//}}AFX_DATA


	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CxDlgRole)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	CaRole* m_pParam;
	CImageList m_ImageCheck;
	CuCheckListBox m_cCheckListBoxDatabase;
	CuEditableListCtrlURPPrivileges m_cListPrivilege;

	void FillPrivileges();
	void FillDatabases();
	// Generated message map functions
	//{{AFX_MSG(CxDlgRole)
	afx_msg void OnCheckExternalPassword();
	afx_msg void OnChangeEditRoleName();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_XDLGROLE_H__28A94563_8BC1_11D3_A317_00609725DDAF__INCLUDED_)
