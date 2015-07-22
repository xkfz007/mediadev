#if !defined(AFX_KDLGMIP_H__03D01972_493D_11D5_BC2D_5254AB36B90A__INCLUDED_)
#define AFX_KDLGMIP_H__03D01972_493D_11D5_BC2D_5254AB36B90A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KDlgMIP.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKDlgMIP dialog

class CKDlgMIP : public CDialog
{
// Construction
public:
	CKDlgMIP(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CKDlgMIP)
	enum { IDD = IDD_KMIPDLG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKDlgMIP)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	int  ShowDialog();

protected:
	BOOL     bstored;
	HICON    hIcon1;
	HICON	 m_hIcon;
	HBRUSH   hbrush;
	CDC      bkdc;

	CDC		 staticdc[5];
	CRect    staticrc[5];
	CKEdit   tedit[10];

	int      focus;
	int      xoff1;
	int      yoff1;
	int      xoff2;
	int      yoff2;

	int      yfix;

	void     ChangeEdit(UINT nID,int i);
	void     Static10X(int j);

	// Generated message map functions
	//{{AFX_MSG(CKDlgMIP)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEdit1();
	afx_msg void OnChangeEdit2();
	afx_msg void OnChangeEdit3();
	afx_msg void OnChangeEdit4();
	afx_msg void OnChangeEdit5();
	afx_msg void OnChangeEdit6();
	afx_msg void OnChangeEdit7();
	afx_msg void OnChangeEdit8();
	afx_msg void OnChangeEdit9();
	afx_msg void OnChangeEdit10();
	afx_msg void OnStatic101();
	afx_msg void OnStatic102();
	afx_msg void OnStatic103();
	afx_msg void OnStatic104();
	afx_msg void OnStatic105();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDLGMIP_H__03D01972_493D_11D5_BC2D_5254AB36B90A__INCLUDED_)
