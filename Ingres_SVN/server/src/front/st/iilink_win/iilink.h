#if !defined(AFX_IILINK_H__E4AA1CB7_ECF9_11D2_B840_AA000400CF10__INCLUDED_)
#define AFX_IILINK_H__E4AA1CB7_ECF9_11D2_B840_AA000400CF10__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif /* _MSC_VER > 1000 */

/*
**
**  Name: iilink.h
**
**  Description:
**	This is the header file for the iilink.cpp file.
**
**  History:
**	23-apr-1999 (somsa01)
**	    Created.
**	06-nov-2001 (somsa01)
**	    Added SPLASH_DURATION define.
**	07-nov-2001 (somsa01)
**	    Changed SPLASH_DURATION to 3000.
**	19-sep-2002 (somsa01)
**	    Added SetupVC7Environment().
**	22-jun-2004 (somsa01)
**	    Cleaned up code for Open Source.
*/

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		/* main symbols */

struct node
{
    char filename[48];
    struct node *next;
};

extern HPALETTE hSystemPalette;
extern BOOL Backup, UseUserUdts, IncludeDemo, LocChanged;
extern BOOL IngresUp;
extern CString BackupExt, UserLoc, CompileCmd, LinkCmd;
extern struct node *FileSrcList, *FileObjList;
extern HWND MainWnd;

extern "C" LONG	ping_iigcn(VOID);
extern INT ExecuteCmds();
extern INT CompareVersion(char *FileName);
extern VOID SetupVC7Environment(HKEY hKeyVC7);

#define SPLASH_DURATION (3000)

/*
** CIilinkApp
*/
class CIilinkApp : public CWinApp
{
    public:
	CIilinkApp();

/* Overrides */
	/* ClassWizard generated virtual function overrides */
	//{{AFX_VIRTUAL(CIilinkApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

/* Implementation */
	//{{AFX_MSG(CIilinkApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IILINK_H__E4AA1CB7_ECF9_11D2_B840_AA000400CF10__INCLUDED_)
