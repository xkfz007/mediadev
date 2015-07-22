// SimplePlayer.h : main header file for the SIMPLEPLAYER application
//

#if !defined(AFX_SIMPLEPLAYER_H__3FCE9E02_08D5_47BB_B5A1_2708D494BF50__INCLUDED_)
#define AFX_SIMPLEPLAYER_H__3FCE9E02_08D5_47BB_B5A1_2708D494BF50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSimplePlayerApp:
// See SimplePlayer.cpp for the implementation of this class
//

class CSimplePlayerApp : public CWinApp
{
public:
	CSimplePlayerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimplePlayerApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSimplePlayerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMPLEPLAYER_H__3FCE9E02_08D5_47BB_B5A1_2708D494BF50__INCLUDED_)
