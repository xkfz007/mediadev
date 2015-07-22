// KButton.cpp : implementation file
//

#include "stdafx.h"
#include "KMP.h"
#include "kButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKButton

CKButton::CKButton()
{
	bfocus=FALSE;
	bmove=FALSE;
	bstored=FALSE;
	status=2;

	font.CreateFont(-12,0,0,0,400,FALSE,FALSE,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH & FF_SWISS,"ËÎÌו");//Time New Roman");//""ËÎÌו");
}

CKButton::~CKButton()
{
	DeleteObject(font.m_hObject);
}

BEGIN_MESSAGE_MAP(CKButton, CButton)
	//{{AFX_MSG_MAP(CKButton)
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKButton message handlers

void CKButton::PreSubclassWindow() 
{
	CButton::PreSubclassWindow();
   
	SetButtonStyle(GetButtonStyle() | BS_OWNERDRAW);	
}

BOOL CKButton::OnEraseBkgnd(CDC* pDC) 
{
	if(!bstored)
	{
    	CRect  clientRect;
	    GetClientRect(&clientRect);
	    int cx=clientRect.Width ();
	    int cy=clientRect.Height ();
		CBitmap	bitmap;
		bkdc.CreateCompatibleDC (pDC);
		bitmap.CreateCompatibleBitmap (pDC,cx,cy);
		bkdc.SelectObject (&bitmap);
		bkdc.BitBlt (0,0,cx,cy,pDC,0,0,SRCCOPY);
		bstored=true;
	}
	return TRUE;
}

void CKButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if(lpDrawItemStruct->itemState & ODS_DISABLED )
	{
		status=2;
	}
	if(lpDrawItemStruct->itemState & ODS_FOCUS)
	{
		bfocus=TRUE;
	}
	if( !(lpDrawItemStruct->itemState & ODS_FOCUS) )
	{
		bfocus=FALSE;
	}
	draw();
}

void CKButton::draw()
{
	CRect	rect;              // bkdc
	GetClientRect(&rect);	
	CClientDC  dc(this);
	dc.BitBlt (0,0,rect.Width (),rect.Height (),&bkdc,0,0,SRCCOPY);

	COLORREF  clface=::GetSysColor(COLOR_BTNFACE);
	COLORREF  clhilight=::GetSysColor(COLOR_BTNHILIGHT);  // border
	COLORREF  clshadow=::GetSysColor(COLOR_BTNSHADOW);
	COLORREF  clframe=::GetSysColor(COLOR_WINDOWFRAME);

	CFont* pfont=dc.SelectObject(&font);

	switch(status)
	{
		case 2:
		case 1:
		case 0:
			dc.Draw3dRect (&rect,clface,clframe);
			rect.DeflateRect(1, 1);
			dc.Draw3dRect (&rect,clhilight,clshadow);
			rect.InflateRect(1, 1);
			break;
		case 3:
			dc.Draw3dRect (&rect,clshadow,clhilight);
			rect.DeflateRect(1, 1);
			dc.Draw3dRect (&rect,clframe,clface);
			rect.InflateRect(1, 1);
			break;
		default:
			break;
	}
	if(bfocus)
	{
		CBrush br(RGB(0,0,0));  
		dc.FrameRect(&rect, &br);
		CPen  pen(PS_SOLID,0,clshadow);
		dc.SelectObject (&pen);
		dc.MoveTo(rect.right-2,rect.top );
		dc.LineTo(rect.right-2,rect.bottom-2 );
		dc.LineTo(rect.left+2,rect.bottom-2 );
		rect.DeflateRect(4, 4);
		dc.DrawFocusRect(&rect);
	}
	
	CString		itemString;      // text
	GetWindowText(itemString);

	CSize size=dc.GetTextExtent (itemString);
	GetClientRect(&rect);
	int rectwidth=rect.Width();
	int rectheight=rect.Height();
	int textwidth=size.cx ;
	int textheight=size.cy ;
	int x,y; 

	y=(rectheight-textheight)/2 + 1;		
	x=(rectwidth-textwidth)/2 ;
	
	if(status==3)
	{
		x++;
		y++;
	}
	dc.SetBkMode(1);
	dc.TextOut (x,y,itemString);

	dc.SelectObject(pfont);
}

void CKButton::OnMouseMove(UINT nFlags, CPoint point) 
{
//	if (nFlags & MK_LBUTTON)
//		return;
	if(!bmove)
	{
		bmove=TRUE;
		status=0;
		draw();
		SetTimer(1,10,NULL);
	}
	CButton::OnMouseMove(nFlags, point);
}

void CKButton::OnLButtonDown(UINT nFlags, CPoint point) 
{
	status=3;
    draw();
	CButton::OnLButtonDown(nFlags, point);
}

void CKButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
	status=2;
    draw();

	CButton::OnLButtonUp(nFlags, point);
}

void CKButton::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent!=1)
		return;
	CPoint	point;
	CRect	rect;
	GetWindowRect(&rect);	
	GetCursorPos(&point);

	if (!rect.PtInRect(point) && bmove )
	{
		KillTimer (1);
		status=1;
		bmove=FALSE;
		draw();
	}
//	CButton::OnTimer(nIDEvent);
}
