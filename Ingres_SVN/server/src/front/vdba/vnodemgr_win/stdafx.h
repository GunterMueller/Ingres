/*
**  Copyright (C) 2005-2006 Ingres Corporation. All Rights Reserved.
*/

/*
**    Source   : stdafx.h , Header File
**    Purpose  : include file for standard system include files,
**               or project specific include files that are used frequently,
**               but are changed infrequently
**
** History:
**
** 10-Mar-2004 (schph01)
**    (BUG #111014), defined WINVER for fixing various problems when building
**    with recent version(s) of compiler
**/

#if !defined(AFX_STDAFX_H__2D0C26C8_E5AC_11D2_A2D9_00609725DDAF__INCLUDED_)
#define AFX_STDAFX_H__2D0C26C8_E5AC_11D2_A2D9_00609725DDAF__INCLUDED_

#define WINVER 0x0400

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcview.h>
#include <afxdisp.h>        // MFC OLE automation classes
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxtempl.h>       // MFC Template classes
#include "portto64.h"
#include "ingobdml.h"
#include "vnodedml.h"



//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__2D0C26C8_E5AC_11D2_A2D9_00609725DDAF__INCLUDED_)
