// TEdit.cpp : implementation file
//

#include "stdafx.h"
#include "KMP.h"
#include "KEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKEdit

CKEdit::CKEdit()
{
	fixflag=FALSE;
	yfix=-3;

	bstored=FALSE;
	focus=0;
	face=3;
}

CKEdit::~CKEdit()
{
//	ON_WM_PAINT()
}

BEGIN_MESSAGE_MAP(CKEdit, CEdit)
	//{{AFX_MSG_MAP(CKEdit)
	ON_WM_ERASEBKGND()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKEdit message handlers

void CKEdit::setface(int i)
{
	face=i;
}

void CKEdit::setfont(HFONT hft)
{
	hfont=hft;
}

void CKEdit::update()
{
	CClientDC dc(this);

	int yoff=yfix-2;
	if(face==2)
	{
		dc.BitBlt (-1,
				   yoff,
				   rc.right-rc.left+1,
				   rc.bottom-rc.top,
				   &(bkdc),
				   0,
				   0,
				   SRCCOPY);
	}
	else
	{
		dc.BitBlt (0,
				   yoff,
				   rc.right-rc.left+1,
				   rc.bottom-rc.top,
				   &(bkdc),
				   0,
				   0,
				   SRCCOPY);
	}

	InvalidateRect(NULL,TRUE);

	if(face==3)
		drawtedit3d();
	else if(focus==1)
		drawtedit2d();
}

BOOL CKEdit::OnEraseBkgnd(CDC* pDC) 
{
/*	if(!bstored)
	{
    	CRect  clientRect;
	    GetClientRect(&clientRect);
	    int cx=clientRect.Width ();
	    int cy=clientRect.Height ();
		CBitmap	bitmap;
		bkdc.CreateCompatibleDC (pDC);
		bitmap.CreateCompatibleBitmap (pDC,cx,cy);
		bkdc.SelectObject (&bitmap);
		bkdc.BitBlt (0,-2,cx,cy,pDC,0,0,SRCCOPY);
		bstored=true;
	}
*/
	return TRUE;
}

void CKEdit::OnKillFocus(CWnd* pNewWnd) 
{
	focus=0;	
	SetSel(-1,0);

	update();

	CEdit::OnKillFocus(pNewWnd);
}

void CKEdit::OnSetFocus(CWnd* pOldWnd) 
{
	CEdit::OnSetFocus(pOldWnd);	

	focus=1;

	update();

}
/*
void CKEdit::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	dc.SetBkMode(1);

	dc.BitBlt(0,0,rc.right-rc.left,rc.bottom-rc.top,&bkdc,0,0,SRCCOPY);

	if(face==3)
		drawtedit3d();
	else if(focus==1)
		drawtedit2d();

	char buf[100];
	memset((void*)buf,0,sizeof(buf));
	GetWindowText(buf,sizeof(buf));

	int ilen=strlen(buf);
	if(ilen>20)
		ilen=20;

	CFont* ft=CFont::FromHandle(hfont);

	CFont *pfont=dc.SelectObject(ft);


	dc.TextOut(0,0,buf,ilen);

	dc.SelectObject(pfont);

	// Do not call CWnd::OnPaint() for painting messages
}
*/
void CKEdit::drawtedit2d()
{
	CClientDC dc(this);
	CPen  pen1(PS_SOLID,1,::GetSysColor(COLOR_BTNSHADOW));
	CPen  pen2(PS_SOLID,1,::GetSysColor(COLOR_BTNHILIGHT));

	int bottomfix;
	if(fixflag)
		bottomfix=-2;
	else
		bottomfix=0;

	CPen  *lpen=dc.SelectObject(&pen1);

	int yoff=yfix-2;
	dc.MoveTo(rc.right-rc.left-1,yoff);
	dc.LineTo(-1,yoff);
	dc.LineTo(-1,rc.bottom-rc.top-1+yoff+bottomfix);

	dc.SelectObject(&pen2);
	dc.MoveTo(rc.right-rc.left-1, yoff);
	dc.LineTo(rc.right-rc.left-1, rc.bottom-rc.top-1+yoff+bottomfix);
	dc.LineTo(0,rc.bottom-rc.top-1+yoff+bottomfix);

	dc.SelectObject(lpen);
}

void CKEdit::drawtedit3d()
{
	CRect rc;
	CClientDC dc(this);
	GetClientRect(&rc);

	rc.left -=2;
	rc.top -=5;
	rc.bottom-=4;

	CPen  pen1(PS_SOLID,1,::GetSysColor(COLOR_BTNSHADOW));
	CPen  pen2(PS_SOLID,1,::GetSysColor(COLOR_WINDOWFRAME));
	CPen  pen3(PS_SOLID,1,::GetSysColor(COLOR_BTNHILIGHT));
	CPen  pen4(PS_SOLID,1,::GetSysColor(COLOR_BTNFACE));

	CPen  *lpen=dc.SelectObject(&pen1);
	dc.MoveTo(rc.right ,rc.top );
	dc.LineTo(rc.left , rc.top );
	dc.LineTo(rc.left ,rc.bottom );

	dc.SelectObject(&pen2);		
	dc.MoveTo(rc.right-1,rc.top +1);
	dc.LineTo(rc.left +1,rc.top +1);
	dc.LineTo(rc.left +1,rc.bottom -1);

	dc.SelectObject(&pen3);
	dc.MoveTo(rc.right ,rc.top );
	dc.LineTo(rc.right ,rc.bottom );
	dc.LineTo(rc.left ,rc.bottom );

	dc.SelectObject(&pen4);
	dc.MoveTo(rc.right -1,rc.top +1);
	dc.MoveTo(rc.right -1,rc.bottom -1);
	dc.MoveTo(rc.left +1,rc.bottom -1);

	dc.SelectObject(lpen);
}

void CKEdit::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CEdit::OnLButtonDown(nFlags,point);
	
	focus=1;

	update();
}

void CKEdit::OnMouseMove(UINT nFlags, CPoint point) 
{
	CEdit::OnMouseMove(nFlags, point);

	if(nFlags & MK_LBUTTON )
	{
		update();

		return ;
	}

	int startchar=-1;
	int endchar=0;

	GetSel(startchar,endchar);
	SetSel(startchar,endchar,FALSE);
}

void CKEdit::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CEdit::OnRButtonDown(nFlags, point);

	focus=1;

	update();
}

void CKEdit::OnMButtonDown(UINT nFlags, CPoint point) 
{
	CEdit::OnMButtonDown(nFlags, point);

	focus=1;

	update();
}

void CKEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// 35 end
	// 36 home 
	// 37 left 
	// 38 up
	// 39 right
	// 40 down

	if(nChar==VK_UP || nChar==VK_DOWN )  
		return ;

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);

	if(nChar==VK_END || nChar==VK_HOME || nChar==VK_LEFT || nChar==VK_RIGHT	)
	{
		update();
	}

	return ;
}

