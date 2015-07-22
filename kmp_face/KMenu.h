#if !defined(AFX_KMENU_H__00241E78_C335_437A_8E72_310E36E54B26__INCLUDED_)
#define AFX_KMENU_H__00241E78_C335_437A_8E72_310E36E54B26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KMenu.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKMenu window

class CKMenu : public CWnd
{
// Construction
public:
	CKMenu();

// Attributes
public:
	int     focus;     // 获得焦点子菜单项
	HBRUSH  hbrush;
	HFONT   hfont;

	HWND    hparent;
	WINDOWPLACEMENT wp;

	int     index;     // 第几个子菜单
	int     itemh;     // menu item height;
	int     itemw;
	int     sph;       // splitter item height;

	int     items;
	UINT	menuids[8];
	char    menutexts[8][20];
	CRect   menurc[8];

	CDC     bkdc;
	CDC     fkdc;

// Operations
public:
	int     setfont(HFONT hft);
	int     redrawmenu(CDC* pDC);
	int     setwp(WINDOWPLACEMENT* lpwp);
	int     setparent(HWND hwnd);
	int     appendmenu(UINT menuid,LPCTSTR menutext);  // menuid =0 ,end;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKMenu)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CKMenu();

	// Generated message map functions
protected:
	//{{AFX_MSG(CKMenu)
	afx_msg HBRUSH  OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KMENU_H__00241E78_C335_437A_8E72_310E36E54B26__INCLUDED_)
