/*
**  Copyright (C) 2005-2006 Ingres Corporation. All Rights Reserved.
*/

/*
**    Source   : stdafx.h
**    Project  : INGRES II/ Visual Schema Diff Control (vdda.exe).
**    Author   : Sotheavut UK (uk$so01)
**    Purpose  : include file for standard system include files,
**               or project specific include files that are used frequently, but 
**               are changed infrequently
**
** History:
**
** 23-Sep-2002 (uk$so01)
**    Created
** 10-Mar-2004 (schph01)
**    (BUG #111014), defined WINVER for fixing various problems when building
**    with recent version(s) of compiler
**/

#if !defined(AFX_STDAFX_H__2F5E5357_CF0B_11D6_87E7_00C04F1F754A__INCLUDED_)
#define AFX_STDAFX_H__2F5E5357_CF0B_11D6_87E7_00C04F1F754A__INCLUDED_

#define WINVER 0x0400

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>       // MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <afxtempl.h>       // MFC template
#include <afxole.h>         // MFC Ole
#include "portto64.h"


//#define _IGNORE_STAR_DATABASE  // star database will not be compared.

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__2F5E5357_CF0B_11D6_87E7_00C04F1F754A__INCLUDED_)
