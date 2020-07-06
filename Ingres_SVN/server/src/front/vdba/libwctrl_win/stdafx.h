/*
**  Copyright (C) 2005-2006 Ingres Corporation. All Rights Reserved.
*/

/*
**    Source   : istdafx.h : Header File.
**    Project  : Import Assistant (IIA)
**    Author   : UK Sotheavut (uk$so01)
**    Purpose  : include file for standard system include files
**               or project specific include files that are used frequently,
**               but are changed infrequently
**
** History:
**
** 12-Mar-2001 (uk$so01)
**    Created
** 17-Dec-2002 (uk$so01)
**    SIR #109220, Enhance the library.
** 28-Feb-2003 (uk$so01)
**    SIR #109718, Avoiding the duplicated source files by integrating the libraries.
** 11-Feb-2004 (noifr01)
**    defined WINVER for fixing various problems when building with recent
**    version(s) of compiler
*/


#if !defined(AFX_STDAFX_H__3BFB8EB5_B55B_11D4_8728_00C04F1F754A__INCLUDED_)
#define AFX_STDAFX_H__3BFB8EB5_B55B_11D4_8728_00C04F1F754A__INCLUDED_

#define WINVER 0x0400

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN         // Exclude rarely-used stuff from Windows headers

#include <afx.h>
#include <afxwin.h>

// TODO: reference additional headers your program requires here
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxtempl.h>       // MFC Template classes
#include <afxdlgs.h>
#include "portto64.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__3BFB8EB5_B55B_11D4_8728_00C04F1F754A__INCLUDED_)
