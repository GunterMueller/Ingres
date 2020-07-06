/*
**  Copyright (C) 2005-2006 Ingres Corporation. All Rights Reserved.		       
*/

/*
**    Source   : stdafx.h : include file for standard system include files,
**               or project specific include files that are used frequently, but 
**               are changed infrequently
**    Project  : Meta data library / Libingll.lib
**    Author   : Sotheavut UK (uk$so01)
**    Purpose  : QUIDS Management
**
** History:
**
** 29-Nov-2000 (uk$so01)
**    Created
** 23-Oct-2001 (uk$so01)
**    SIR #106057 (sqltest as ActiveX & Sql Assistant as In-Process COM Server)
** 09-Mar-2004 (noifr01)
**    defined WINVER for fixing various problems when building with recent
**    version(s) of compiler
**/


#if !defined(AFX_STDAFX_H__E5A2DB53_C79C_11D4_872D_00C04F1F754A__INCLUDED_)
#define AFX_STDAFX_H__E5A2DB53_C79C_11D4_872D_00C04F1F754A__INCLUDED_

#define WINVER 0x0400

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN  // Exclude rarely-used stuff from Windows headers

#include <afx.h>
#include <afxwin.h>

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxtempl.h>       // MFC Template classes
#include <afxpriv.h>
#include <afxole.h>
#include "constdef.h"
#include "usrexcep.h"

#if !defined (MAINWIN)
#define DESKTOP          // Define target for Ingres compat library
#define NT_GENERIC       // Define target for Ingres compat library
#define IMPORT_DLL_DATA  // Define target for Ingres compat library
#endif
#define INGRESII         // Define target for Ingres compat library

#define _INGRES_DATA_UNICODE // INGRES supports only columns unicode.
#if !defined (MAINWIN)
#define _ANIMATION_      // Activate Animate Dialog if not mainwin
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__E5A2DB53_C79C_11D4_872D_00C04F1F754A__INCLUDED_)
