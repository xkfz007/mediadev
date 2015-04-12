// VideoCompareDlg.h : header file
//

#if !defined(AFX_VIDEOCOMPAREDLG_H__A75CB1B5_4B3D_4657_BC7C_4A67A0D7F4A1__INCLUDED_)
#define AFX_VIDEOCOMPAREDLG_H__A75CB1B5_4B3D_4657_BC7C_4A67A0D7F4A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CVideoCompareDlg dialog

class CVideoCompareDlg : public CDialog
{
// Construction
public:
	CVideoCompareDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CVideoCompareDlg)
	enum { IDD = IDD_VIDEOCOMPARE_DIALOG };
	CEdit	m_Eoutput;
	CProgressCtrl	m_Progress;
	CEdit	m_EPath2;
	CEdit	m_EPath1;
	CComboBox	m_ComboWH;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVideoCompareDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CVideoCompareDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonCompare();
	afx_msg void OnButtonBrowse1();
	afx_msg void OnButtonBrowse2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CString m_path1;
	CString m_path2;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDEOCOMPAREDLG_H__A75CB1B5_4B3D_4657_BC7C_4A67A0D7F4A1__INCLUDED_)
