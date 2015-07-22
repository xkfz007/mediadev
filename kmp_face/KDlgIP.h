#if !defined(AFX_KDLGIP_H__03D01970_493D_11D5_BC2D_5254AB36B90A__INCLUDED_)
#define AFX_KDLGIP_H__03D01970_493D_11D5_BC2D_5254AB36B90A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KDlgIP.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKDlgIP dialog

class CKDlgIP : public CDialog
{
// Construction
public:
	CKDlgIP(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CKDlgIP)
	enum { IDD = IDD_KIPDLG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKDlgIP)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int      yfix;
	HICON	 m_hIcon;
	HBRUSH   hbrush;
	CDC      bkdc;
	CKButton btn1;
	CKButton btn2;
	CKEdit   ip1;
	BOOL     bstored;

	// Generated message map functions
	//{{AFX_MSG(CKDlgIP)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnChangeEdit1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDLGIP_H__03D01970_493D_11D5_BC2D_5254AB36B90A__INCLUDED_)
