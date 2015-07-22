#if !defined(AFX_KDLGCHAN_H__03D0196F_493D_11D5_BC2D_5254AB36B90A__INCLUDED_)
#define AFX_KDLGCHAN_H__03D0196F_493D_11D5_BC2D_5254AB36B90A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KDlgChan.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKDlgChan dialog

class CKDlgChan : public CDialog
{
// Construction
public:
	CKDlgChan(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CKDlgChan)
	enum { IDD = IDD_KCHANDLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKDlgChan)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON	 m_hIcon;
	HBRUSH   hbrush;
	CDC      bkdc;
	CKButton btn1;
	CKButton btn2;

	// Generated message map functions
	//{{AFX_MSG(CKDlgChan)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDLGCHAN_H__03D0196F_493D_11D5_BC2D_5254AB36B90A__INCLUDED_)
