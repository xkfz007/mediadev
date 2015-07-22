// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__03D01969_493D_11D5_BC2D_5254AB36B90A__INCLUDED_)
#define AFX_STDAFX_H__03D01969_493D_11D5_BC2D_5254AB36B90A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#define WM_USER100 WM_USER+100
#define WM_USER101 WM_USER+101
#define WM_USER102 WM_USER+102
#define WM_USER103 WM_USER+103

#define WM_USER200 WM_USER+200

#include "kbutton.h"
#include "kedit.h"


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__03D01969_493D_11D5_BC2D_5254AB36B90A__INCLUDED_)
