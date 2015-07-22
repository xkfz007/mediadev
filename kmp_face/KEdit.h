
#if !defined(AFX_KEdit_H__89F5D8C1_491C_11D5_BC2D_5254AB36B90A__INCLUDED_)
#define AFX_KEdit_H__89F5D8C1_491C_11D5_BC2D_5254AB36B90A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKEdit window

class CKEdit : public CEdit
{
// Construction
public:
	CKEdit();

// Attributes
public:
	BOOL     fixflag;
	HFONT    hfont;
	int      yfix;

// Operations
public:
	void     setfont(HFONT hft);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CKEdit();

public:
	BOOL    bstored;
	CDC     bkdc;
	RECT    rc;
	RECT    facerc;
	int     focus;
	int     face;   // 2 , ¶þÎ¬; 3, ÈýÎ¬
	void    setface(int i);
	void    drawtedit2d();
	void    drawtedit3d();
	void    update();

	// Generated message map functions

//	afx_msg void OnPaint();

protected:
	//{{AFX_MSG(CKEdit)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KEdit_H__89F5D8C1_491C_11D5_BC2D_5254AB36B90A__INCLUDED_)
