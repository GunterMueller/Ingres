/*
**  Copyright (C) 2005-2006 Ingres Corporation. All Rights Reserved.
*/

/*
**    Source   : dbedoc.h, Header File   (Document of MDI Child Frame)
**    Project  : CA-OpenIngres/Monitoring.
**    Author   : EMB, Standard Skeleton Generated by MFC Widzard.
**               UK Sotheavut, Detail implementation.
**    Purpose  : The Document Data for the DBEvent Trace.
**
** History (after 26-Sep-2000)
** 
** 26-Sep-2000 (uk$so01)
**    SIR #102711: Callable in context command line improvement (for Manage-IT)
**    Select the input database if specified.
** 26-Mar-2001 (noifr01)
**    (sir 104270) removal of code for managing Ingres/Desktop
*/

#ifndef DBEDOC_HEADER
#define DBEDOC_HEADER

#include <afxadv.h>

#include "calsctrl.h"

//
// Used to Implement the Serialized Mechanism
class CuDataRegisteredDBevent: public CObject
{
    DECLARE_SERIAL (CuDataRegisteredDBevent)
public:
    CuDataRegisteredDBevent();
    CuDataRegisteredDBevent(const CuDataRegisteredDBevent& c);
    CuDataRegisteredDBevent(LPCTSTR lpszDBEvent, LPCTSTR lpszOwner, BOOL bRegistered);
    CuDataRegisteredDBevent& operator= (const CuDataRegisteredDBevent& c);
    virtual ~CuDataRegisteredDBevent(){}
    
    CString m_strDBEvent;   // The name of the DB Event
    CString m_strOwner;     // The owner of the DB Event
    BOOL    m_bRegistered;  // TRUE if it is registered.

    virtual void Serialize (CArchive& ar);
};

//
// Used to Implement the Serialized Mechanism
class CuDataRaisedDBevent: public CObject
{
    DECLARE_SERIAL (CuDataRaisedDBevent)
public:
    CuDataRaisedDBevent();
    CuDataRaisedDBevent(const CuDataRaisedDBevent& c);
    CuDataRaisedDBevent(
        LPCTSTR lpszNum,
        LPCTSTR lpszTime, 
        LPCTSTR lpszDBEvent, 
        LPCTSTR lpszOwner,
        LPCTSTR lpszText);
    CuDataRaisedDBevent& operator= (const CuDataRaisedDBevent& c);
   virtual ~CuDataRaisedDBevent(){}
    CString m_strNum;       // The Counter;
    CString m_strTime;      // The time when raised
    CString m_strDBEvent;   // The name of the DB Event
    CString m_strDBEOwner;  // The owner of the DB Event
    CString m_strDBEText;   // The DB Event text

    virtual void Serialize (CArchive& ar);
};


class CDbeventDoc : public CDocument
{
protected:
	CDbeventDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDbeventDoc)

    void CleanUp();
    void SetInputDatabase(LPCTSTR lpszDatabase);
    //
    // Data to be stored.
    // Current Selected Database
    // List of DB Events (with state Registered/Not Registered) of the Current Database. 
    // List of Raised DB Events

public:
	void SetSeqNum(int val);
	int m_OIVers;
	int m_hNode;
    int m_nHandle; 
    int m_nMaxLine;
    int m_nCounter1;    // Double Counter ex:m_nCounter1/m_nCounter2
    int m_nCounter2;    // Double Counter ex:m_nCounter1/m_nCounter2
    BOOL    m_bDBInit;
    BOOL    m_bSysDBEvent;
    BOOL    m_bPopupOnRaise;
    BOOL    m_bClearFirst;
    CString m_strDBName;
    CString m_strRaisedSince;
    CTypedPtrList<CObList, CuDataRegisteredDBevent*>    m_listRegisteredDBEvent;
    CTypedPtrList<CObList, CuDataRaisedDBevent*>        m_listRaisedDBEvent;

    // serialization
    BOOL        m_bToolbarVisible;    // for Load only!
    BOOL        m_bLoaded;
    CDockState  m_toolbarState;
    WINDOWPLACEMENT m_wplj;
    int             m_cxCur;    // splitter
    int             m_cxMin;    // splitter

    void SetRegisteredDBEventList (CCheckListBox* pList){m_pListRegisteredDBEvent = pList;}
    void SetRaisedDBEventList     (CuListCtrl*    pList){m_pListRaisedDBEvent     = pList;}
    CCheckListBox* GetRegisteredDBEventList (){return m_pListRegisteredDBEvent;}
    CuListCtrl*    GetRaisedDBEventList     (){return m_pListRaisedDBEvent;}

    // serialization
    BOOL  IsLoadedDoc()           { return m_bLoaded; }
    CDockState& GetToolbarState() { return m_toolbarState; }
    WINDOWPLACEMENT *GetWPLJ()    { return &m_wplj; }
    int GetSplitterCxMin()        { return m_cxMin; }
    int GetSplitterCxCur()        { return m_cxCur; }

    // Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDbeventDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual void DeleteContents();
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

    // Implementation
public:
	virtual ~CDbeventDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
    void DisplayDoc();
#endif
    CString m_strInputDatabase;

	// Generated message map functions
protected:
    CCheckListBox*  m_pListRegisteredDBEvent;
    CuListCtrl*     m_pListRaisedDBEvent;

	//{{AFX_MSG(CDbeventDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int m_SeqNum;
};

#endif  // DBEDOC_HEADER