// KMP.h : main header file for the KMP application
//

#if !defined(AFX_KMP_H__03D01965_493D_11D5_BC2D_5254AB36B90A__INCLUDED_)
#define AFX_KMP_H__03D01965_493D_11D5_BC2D_5254AB36B90A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CKMPApp:
// See KMP.cpp for the implementation of this class
//

class CKMPApp : public CWinApp
{
public:
	CKMPApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKMPApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CKMPApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KMP_H__03D01965_493D_11D5_BC2D_5254AB36B90A__INCLUDED_)
