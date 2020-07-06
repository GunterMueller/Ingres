/*
**  Copyright (C) 2005-2006 Ingres Corporation. All Rights Reserved.		       
*/

/*
**    Source   : stdafx.h : include file for standard system include files,
**               or project specific include files that are used frequently, but 
**               are changed infrequently
**    Project  : Meta data library 
**    Author   : Sotheavut UK (uk$so01)
**    Purpose  : QUIDS Management
**
** History:
**
** 09-Mar-2004 (noifr01)
**    defined WINVER for fixing various problems when building with recent
**    version(s) of compiler
**/

#if !defined(AFX_STDAFX_H__88E497F8_45E5_11D5_8751_00C04F1F754A__INCLUDED_)
#define AFX_STDAFX_H__88E497F8_45E5_11D5_8751_00C04F1F754A__INCLUDED_

#define WINVER 0x0400

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afx.h>
#include <afxwin.h>

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__88E497F8_45E5_11D5_8751_00C04F1F754A__INCLUDED_)
