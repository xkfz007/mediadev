// KMPDlg.h : header file
//

#if !defined(AFX_KMPDLG_H__03D01967_493D_11D5_BC2D_5254AB36B90A__INCLUDED_)
#define AFX_KMPDLG_H__03D01967_493D_11D5_BC2D_5254AB36B90A__INCLUDED_

#include "kmenu.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CKMPDlg dialog

class CKMPDlg : public CDialog
{
// Construction
public:
	CKMPDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CKMPDlg)
	enum { IDD = IDD_KMPDLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKMPDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	int     menuflag; // 标记菜单是否显示
	int     status;   // 程序状态

	WINDOWPLACEMENT  wp; 

	HBRUSH  hbrush;
	DWORD	shadow;
	DWORD	hilight;

	CKMenu  menu[5];

	int     focus1;  // menu focus
	int     focus2;  // bar  focus

	bool    bstored;
	CDC     bkdc;
	CDC		menudc[5];
	CDC     bardc[9];
	CDC     numdc[5];

	bool    barflag[9];      // is enable.
	HICON   hIconList[9][2];
	UINT    barID[9];

	void    drawbar(CDC* pDC, int i,int j);  // i ,index ; j , status ;
	void    drawmenu(CDC* pDC, int i,int j); // i ,index ; j , status ;
	void    drawnum(CDC* pDC, int i);		 // i ,index ; j , status ;	
	void    setsubmenupos(CWnd* pwnd,int i);

	void    resetmenu();

public:	
	char    ktmenu[12][10];
	char    khelp[20][24];
	RECT    kmenu[5];
	RECT	kbar[9] ;
	RECT	knum[6];
	int     xoff;    // used for textout of num[6];
	int     yoff;
	RECT    sliderrc;
	int     sliderx;    // x-off
	int     slidery;    // y-off
	int     sliderylen; // y-len
	BOOL    sliderflag;

protected:
	HICON hIcon;

	// Generated message map functions
	//{{AFX_MSG(CKMPDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL    OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH  OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKDlgChan();
	afx_msg void OnKDlgFile(){};
	afx_msg void OnKMIP();
	afx_msg void OnKIP();
	afx_msg void OnKSkin(){};
	afx_msg void OnKExit(){};
	afx_msg void OnKPlay(){};
	afx_msg void OnKPause(){};
	afx_msg void OnKStop(){};
	afx_msg void OnKForward(){};
	afx_msg void OnKBackward(){};
	afx_msg void OnKLoop(){};
	afx_msg void OnKColor(){};
	afx_msg void OnKHalfscreen(){};
	afx_msg void OnKScreen(){};
	afx_msg void OnKDoublescreen(){};
	afx_msg void OnKFullscreen(){};
	afx_msg void OnKAddvoc(){};
	afx_msg void OnKDelvoc(){};
	afx_msg void OnKLeftsound(){};
	afx_msg void OnKRightsound(){};
	afx_msg void OnKStero(){};
	afx_msg void OnKISOITU(){};
	afx_msg void OnKAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KMPDLG_H__03D01967_493D_11D5_BC2D_5254AB36B90A__INCLUDED_)
