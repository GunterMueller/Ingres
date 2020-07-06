// dgdpdvns.cpp : implementation file
//

#include "stdafx.h"
#include "mainmfc.h"
#include "dgdpdvns.h"
#include "vtree.h"
#include "starsel.h"

extern "C" {
  #include "dba.h"
  #include "domdata.h"
  #include "main.h"
  #include "dom.h"
  #include "tree.h"
  #include "dbaset.h"
  #include "monitor.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define LAYOUT_NUMBER   2

IMPLEMENT_DYNCREATE(CuDlgDomPropViewNativeSource, CFormView)

CuDlgDomPropViewNativeSource::CuDlgDomPropViewNativeSource()
: CFormView(CuDlgDomPropViewNativeSource::IDD)
{
    //{{AFX_DATA_INIT(CuDlgDomPropViewNativeSource)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
  m_Data.m_refreshParams.InitializeRefreshParamsType(OT_VIEW);
}

CuDlgDomPropViewNativeSource::~CuDlgDomPropViewNativeSource()
{
}

void CuDlgDomPropViewNativeSource::DoDataExchange(CDataExchange* pDX)
{
  CFormView::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CuDlgDomPropViewNativeSource)
	DDX_Control(pDX, IDC_EDIT_LDB_OWNER, m_edLdbObjOwner);
	DDX_Control(pDX, IDC_EDIT_LDB_NODE, m_edLdbNode);
	DDX_Control(pDX, IDC_EDIT_LDB_NAME, m_edLdbObjName);
	DDX_Control(pDX, IDC_EDIT_LDB_DBMSTYPE, m_edLdbDbmsType);
	DDX_Control(pDX, IDC_EDIT_LDB_DB, m_edLdbDatabase);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CuDlgDomPropViewNativeSource, CFormView)
    //{{AFX_MSG_MAP(CuDlgDomPropViewNativeSource)
    ON_WM_SIZE()
	//}}AFX_MSG_MAP
    ON_MESSAGE (WM_USER_IPMPAGE_UPDATEING, OnUpdateData)
    ON_MESSAGE (WM_USER_IPMPAGE_LOADING,   OnLoad)
    ON_MESSAGE (WM_USER_IPMPAGE_GETDATA,   OnGetData)
    ON_MESSAGE (WM_USER_DOMPAGE_QUERYTYPE, OnQueryType)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CuDlgDomPropViewNativeSource diagnostics

#ifdef _DEBUG
void CuDlgDomPropViewNativeSource::AssertValid() const
{
    CFormView::AssertValid();
}

void CuDlgDomPropViewNativeSource::Dump(CDumpContext& dc) const
{
    CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CuDlgDomPropViewNativeSource message handlers

void CuDlgDomPropViewNativeSource::OnSize(UINT nType, int cx, int cy) 
{
    CFormView::OnSize(nType, cx, cy);
}


LONG CuDlgDomPropViewNativeSource::OnQueryType(WPARAM wParam, LPARAM lParam)
{
  ASSERT (wParam == 0);
  ASSERT (lParam == 0);
  return DOMPAGE_DETAIL;
}

LONG CuDlgDomPropViewNativeSource::OnUpdateData (WPARAM wParam, LPARAM lParam)
{
  // cast received parameters
  int nNodeHandle = (int)wParam;
  LPIPMUPDATEPARAMS pUps = (LPIPMUPDATEPARAMS)lParam;
  ASSERT (nNodeHandle != -1);
  ASSERT (pUps);

  // ignore selected actions on filters
  switch (pUps->nIpmHint)
  {
    case 0:
    //case FILTER_DOM_SYSTEMOBJECTS:
    //case FILTER_DOM_BASEOWNER:
    //case FILTER_DOM_OTHEROWNER:
      break;

    case FILTER_DOM_BKREFRESH_DETAIL:
      if (m_Data.m_refreshParams.MustRefresh(pUps->pSFilter->bOnLoad, pUps->pSFilter->refreshtime))
        break;    // need to update
      else
        return 0; // no need to update
      break;

    default:
      return 0L;    // nothing to change on the display
  }

  // Get info on the object
  STARSOURCEINFO sSrcInfo;
  memset (&sSrcInfo, 0, sizeof (sSrcInfo));

  LPTREERECORD  lpRecord = (LPTREERECORD)pUps->pStruct;
  ASSERT (lpRecord);

  x_strcpy ((char *)sSrcInfo.DBName,     (const char *)lpRecord->extra);
  x_strcpy ((char *)sSrcInfo.objectname, (const char *)lpRecord->objName);
  x_strcpy ((char *)sSrcInfo.szSchema,   (const char *)lpRecord->ownerName);

  // view native
  sSrcInfo.bRegisteredAsLink  = FALSE;
  sSrcInfo.objType = 'V';

  int iResult = GetStarObjSourceInfo ((LPUCHAR)GetVirtNodeName(nNodeHandle), &sSrcInfo);
  if (iResult != RES_SUCCESS) {
    ASSERT (FALSE);

    // Reset m_Data
    CuDomPropDataViewNativeSource tempData;
    tempData.m_refreshParams = m_Data.m_refreshParams;
    m_Data = tempData;

    // Refresh display
    RefreshDisplay();

    return 0L;
  }

  // Update refresh info
  m_Data.m_refreshParams.UpdateRefreshParams();

  // update member variables, for display/load/save purpose
  m_Data.m_csLdbObjName   = sSrcInfo.szLdbObjName  ;
  m_Data.m_csLdbObjOwner  = sSrcInfo.szLdbObjOwner ;
  m_Data.m_csLdbDatabase  = sSrcInfo.szLdbDatabase ;
  m_Data.m_csLdbNode      = sSrcInfo.szLdbNode     ;
  m_Data.m_csLdbDbmsType  = sSrcInfo.szLdbDbmsType ;

  // Refresh display
  RefreshDisplay();

  return 0L;
}

LONG CuDlgDomPropViewNativeSource::OnLoad (WPARAM wParam, LPARAM lParam)
{
  // Precheck consistency of class name
  LPCTSTR pClass = (LPCTSTR)wParam;
  ASSERT (lstrcmp (pClass, "CuDomPropDataViewNativeSource") == 0);

  // Get class pointer, and check for it's class runtime type
  CuDomPropDataViewNativeSource* pData = (CuDomPropDataViewNativeSource*)lParam;
  ASSERT (pData->IsKindOf(RUNTIME_CLASS(CuDomPropDataViewNativeSource)));

  // Update current m_Data using operator = overloading
  m_Data = *pData;

  // Refresh display
  RefreshDisplay();

  return 0L;

}

LONG CuDlgDomPropViewNativeSource::OnGetData (WPARAM wParam, LPARAM lParam)
{
  // Allocate data class instance WITH NEW, and
  // DUPLICATE current m_Data in the allocated instance using copy constructor
  CuDomPropDataViewNativeSource* pData = new CuDomPropDataViewNativeSource(m_Data);

  // return it WITHOUT FREEING - will be freed by caller
  return (LRESULT)pData;
}


void CuDlgDomPropViewNativeSource::RefreshDisplay()
{
  UpdateData (FALSE);   // Mandatory!

  //
  // Exclusively use member variables of m_Data for display refresh
  //
  m_edLdbObjName.SetWindowText  (m_Data.m_csLdbObjName ) ;
  m_edLdbObjOwner.SetWindowText (m_Data.m_csLdbObjOwner) ;
  m_edLdbDatabase.SetWindowText (m_Data.m_csLdbDatabase) ;
  m_edLdbNode.SetWindowText     (m_Data.m_csLdbNode    ) ;
  m_edLdbDbmsType.SetWindowText (m_Data.m_csLdbDbmsType) ;
}

