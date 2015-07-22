// KMenu.cpp : implementation file
//

#include "stdafx.h"
#include "KMP.h"
#include "KMenu.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern RECT kmenu[5];

/////////////////////////////////////////////////////////////////////////////
// CKMenu

CKMenu::CKMenu()
{
	memset((void*)menuids,0,8*sizeof(UINT));
	memset((void*)menutexts,0,8*20*sizeof(char));

	focus=-2;
	items =0;
	itemh=20;
	itemw=120;
	sph=12;

	hbrush=(HBRUSH)GetStockObject(HOLLOW_BRUSH);

	index=-1;
}

CKMenu::~CKMenu()
{
}

BEGIN_MESSAGE_MAP(CKMenu, CWnd)
	//{{AFX_MSG_MAP(CKMenu)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKMenu message handlers

int  CKMenu::setfont(HFONT hft)
{
	hfont=hft;

	return 1;
}

void CKMenu::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect	rect;
	int		i=0;

	ShowWindow(SW_HIDE);
	::SendMessage(hparent,WM_USER200,0,index);
	index=-1;

	for(i=0;i<8;i++) 
	{
		rect.bottom=menurc[i].bottom-1;
		rect.top=menurc[i].top+1;
		rect.left=menurc[i].left+1;
		rect.right=menurc[i].right-1;

		if(rect.PtInRect(point))
		{
			::SendMessage(hparent,menuids[i],0,0);
			break;
		}
	}
}

void CKMenu::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CRect	rect;
	int		i=0;

	ShowWindow(SW_HIDE);
	::SendMessage(hparent,WM_USER200,0,index);
	index=-1;

	for(i=0;i<8;i++) 
	{
		rect.bottom=menurc[i].bottom-1;
		rect.top=menurc[i].top+1;
		rect.left=menurc[i].left+1;
		rect.right=menurc[i].right-1;

		if(rect.PtInRect(point))
		{
			::SendMessage(hparent,menuids[i],1,1);
			break;
		}
	}
}

void CKMenu::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect	rect;
	int		i=0;

	for(i=0;i<8;i++) 
	{
		rect.bottom=menurc[i].bottom-1;
		rect.top=menurc[i].top+1;
		rect.left=menurc[i].left+1;
		rect.right=menurc[i].right-1;

		if(rect.PtInRect(point) && focus!=i && menuids[i]!=MF_SEPARATOR && menuids[i]!=0)
		{
			focus=i;

			CDC* pDC=GetDC();

			CFont *font=CFont::FromHandle(hfont);
			CFont *pfont=pDC->SelectObject(font);
			pDC->SetBkMode(1);

			redrawmenu(pDC);

			pDC->BitBlt(menurc[i].left+2 ,
						menurc[i].top,
						menurc[i].right-menurc[i].left -5,
						menurc[i].bottom - menurc[i].top ,
						&fkdc,
						0,
						0,
						SRCCOPY);

			pDC->SetBkMode(TRANSPARENT);
			pDC->TextOut(9,menurc[i].top + 4,menutexts[i]);
			
			pDC->SelectObject(pfont);

			ReleaseDC(pDC);

			break;
		}
	}

	return;
}

//-----------------------------------------------------------------------------

int CKMenu::redrawmenu(CDC* pDC)
{
	CPen *lpen;

	CPen  pen1(PS_SOLID,1,::GetSysColor(COLOR_BTNFACE));
	CPen  pen2(PS_SOLID,1,::GetSysColor(COLOR_BTNHILIGHT));
	CPen  pen3(PS_SOLID,1,::GetSysColor(COLOR_BTNSHADOW));
	CPen  pen4(PS_SOLID,1,::GetSysColor(COLOR_WINDOWFRAME));

	CRect rc;
	GetClientRect(&rc);

	pDC->BitBlt(0,0,rc.Width(),rc.Height(),&bkdc,
				0,0,SRCCOPY);

	int xitem,yitem;

	xitem=3;
	yitem=7;

	lpen=pDC->SelectObject(&pen3);

	CRect   itemrect[8];
	for(int mm=0; mm<items ; mm++)
	{		
		if(menuids[mm] != 0 && menuids[mm]!=MF_SEPARATOR )
		{
			pDC->TextOut(9,yitem,menutexts[mm]);
		}

		if(menuids[mm]==MF_SEPARATOR)
		{
			pDC->SelectObject(&pen3);
			pDC->MoveTo(5,yitem+1);
			pDC->LineTo(rc.right-5 ,yitem+1);

			pDC->SelectObject(&pen2);
			pDC->MoveTo(5,yitem+2);
			pDC->LineTo(rc.right-5 ,yitem+2);

			yitem+=sph;
		}
		else if (menuids[mm]!= 0)
		{
			yitem+=itemh;
		}

		itemrect[mm].left =0;
		itemrect[mm].right =itemw ;
	}

	pDC->SelectObject(&pen2);

	pDC->MoveTo(rc.left,0);
	pDC->LineTo(rc.right-2,0);
	pDC->MoveTo(0,rc.left);
	pDC->LineTo(0,rc.bottom);

	pDC->SelectObject(&pen1);
	pDC->MoveTo(rc.left,1);
	pDC->LineTo(rc.right-2,1);
	pDC->MoveTo(1,rc.left);
	pDC->LineTo(1,rc.bottom);

//	pDC->SelectObject(&pen1);
	pDC->MoveTo(rc.right-2,rc.bottom);
	pDC->LineTo(rc.right-2,rc.top);
	pDC->MoveTo(rc.right-2,rc.bottom-2);
	pDC->LineTo(rc.left,rc.bottom-2);

	pDC->SelectObject(&pen3);
	pDC->MoveTo(rc.right-1,rc.bottom);
	pDC->LineTo(rc.right-1,rc.top);
	pDC->MoveTo(rc.right-1,rc.bottom-1);
	pDC->LineTo(rc.left,rc.bottom-1);

	pDC->SelectObject(lpen);

	return 1;
}

int CKMenu::setwp(WINDOWPLACEMENT* lpwp)
{
	wp.rcNormalPosition.left =lpwp->rcNormalPosition.left ;
	wp.rcNormalPosition.right = lpwp->rcNormalPosition.right ;
	wp.rcNormalPosition.top = lpwp->rcNormalPosition.top;
	wp.rcNormalPosition.bottom = lpwp->rcNormalPosition.bottom ;

	return 1;
}

int CKMenu::setparent(HWND hwnd)
{
	hparent=hwnd;
	return 1;
}

int CKMenu::appendmenu(UINT menuid,LPCTSTR menutext)
{
	if(menuid!=0)
	{
		menuids[items]=menuid;
		strcpy(menutexts[items],menutext);

		items++;
	}
	else
	{
		menurc[0].top =3;
		menurc[0].bottom =menurc[0].top + itemh;
		menurc[0].left = 0;
		menurc[0].right =itemw;

		for(int i=1; i<8;i++)
		{	
			if(menuids[i]==MF_SEPARATOR)
			{
				menurc[i].top =menurc[i-1].bottom ;
				menurc[i].bottom =menurc[i].top+12;
			}
			else if (menuids[i]!= 0)
			{
				menurc[i].top =menurc[i-1].bottom ;
				menurc[i].bottom =menurc[i].top+20;
			}
			else   
			{
				menurc[i].top =0;
				menurc[i].bottom =0;
			}

			menurc[i].left =0;
			menurc[i].right =itemw;
		}
	}

	return 1;
}

void CKMenu::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CFont *font=CFont::FromHandle(hfont);
	CFont *pfont=dc.SelectObject(font);
	dc.SetBkMode(1);
	redrawmenu(&dc);
	dc.SelectObject(pfont);

	return  ;
}

HBRUSH CKMenu::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	pDC->SetBkMode(TRANSPARENT);

	return hbrush;
}

int CKMenu::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CDC* pdc=GetDC();

	CBitmap bitmap1;
	bitmap1.LoadBitmap(IDB_BITMAP1);
	fkdc.CreateCompatibleDC(pdc);
	fkdc.SelectObject(&bitmap1);
	
	CBitmap bitmap2;
	bitmap2.LoadBitmap(IDB_BITMAP2);
	bkdc.CreateCompatibleDC(pdc);
	bkdc.SelectObject(&bitmap2);

	ReleaseDC(pdc);
		
	return 0;
}

