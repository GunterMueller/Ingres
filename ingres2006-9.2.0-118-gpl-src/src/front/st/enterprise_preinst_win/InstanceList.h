#pragma once
/*
**  Copyright (c) 2001, 2006 Ingres Corporation.
*/

/*
**  Name: InstanceList.h
**
**  Description:
**	Header file for InstanceList dialog.
**
**  History:
**  15-Nov-2006 (drivi01)
**		SIR 116599
**		Enhanced pre-installer in effort to improve installer usability.
*/


class InstanceList : public CPropertyPage
{
	DECLARE_DYNAMIC(InstanceList)

public:
	InstanceList();
	virtual ~InstanceList();

// Dialog Data
	enum { IDD = IDD_INSTANCELIST };
	CListCtrl	m_list;
	CButton		m_autoupgrade;
	CStatic		m_autoid;
	CStatic		m_autorecommend;
	CString		m_iicode;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void SetList();



	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnSetActive();
	afx_msg BOOL OnWizardFinish();
	afx_msg LRESULT OnWizardNext();
	afx_msg LRESULT OnWizardBack();
	afx_msg void OnLvnItemchangedInstanceList(NMHDR *pNMHDR, LRESULT *pResult);
};
