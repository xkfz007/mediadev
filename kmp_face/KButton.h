#if !defined(AFX_KBUTTON_H__D421F861_4913_11D5_BC2D_5254AB36B90A__INCLUDED_)
#define AFX_KBUTTON_H__D421F861_4913_11D5_BC2D_5254AB36B90A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKButton window

class CKButton : public CButton
{
// Construction
public:
	CKButton();

// Attributes
public:
	CFont font;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void draw();
	virtual ~CKButton();

	// Generated message map functions
protected:
	CDC      bkdc;
	BOOL     bstored;
	BOOL     bmove;
	BOOL	 bfocus;
	int		 status;     // 0,in ;1,out;2,up;3,down
	//{{AFX_MSG(CKButton)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KBUTTON_H__D421F861_4913_11D5_BC2D_5254AB36B90A__INCLUDED_)
