/****************************************************************************************
//                                                                                     //
//  Copyright (C) 2005-2006 Ingres Corporation. All Rights Reserved.                   //
//                                                                                     //
//    Source   : seauddlg.h, Header File                                               //
//                                                                                     //
//                                                                                     //
//    Project  : OpenIngres Configuration Manager                                      //
//    Author   : UK Sotheavut                                                          //
//                                                                                     //
//    Purpose  : Modeless Dialog, Page (Auditing) of Security                          //
//               See the CONCEPT.DOC for more detail                                   //
****************************************************************************************/
#if !defined(SEAUDDLG_HEADER)
#define SEAUDDLG_HEADER

// seauddlg.h : header file
//
#include "editlsgn.h"

/////////////////////////////////////////////////////////////////////////////
// CuDlgSecurityAuditing dialog

class CuDlgSecurityAuditing : public CDialog
{
public:
	CuDlgSecurityAuditing(CWnd* pParent = NULL);
	void OnOK() {return;}
	void OnCancel() {return;}

	// Dialog Data
	//{{AFX_DATA(CuDlgSecurityAuditing)
	enum { IDD = IDD_SECURITY_PAGE_AUDITING };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CuDlgSecurityAuditing)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

	//
	// Implementation
protected:
	CImageList m_ImageList;
	CImageList m_ImageListCheck;
	CuEditableListCtrlGeneric m_ListCtrl;

	// Generated message map functions
	//{{AFX_MSG(CuDlgSecurityAuditing)
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg LONG OnButton1   (UINT wParam, LONG lParam);     // Edit Value	
	afx_msg LONG OnButton2   (UINT wParam, LONG lParam);     // Default
	afx_msg LONG OnButton3   (UINT wParam, LONG lParam);     // Not Used
	afx_msg LONG OnButton4   (UINT wParam, LONG lParam);     // Not Used
	afx_msg LONG OnButton5   (UINT wParam, LONG lParam);     // Not Used
	afx_msg LONG OnUpdateData(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

#endif
