// KDlgMIP.cpp : implementation file
//

#include "stdafx.h"
#include "KMP.h"
#include "KDlgMIP.h"
#include "afxpriv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKDlgMIP dialog

CKDlgMIP::CKDlgMIP(CWnd* pParent /*=NULL*/)
	: CDialog(CKDlgMIP::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKDlgMIP)
	//}}AFX_DATA_INIT

	hIcon1 = AfxGetApp()->LoadIcon(IDI_KICON90);

	m_hIcon = AfxGetApp()->LoadIcon(IDI_KICON1);
	hbrush=(HBRUSH)GetStockObject(HOLLOW_BRUSH);

	focus=0;
	bstored=FALSE;

	xoff1=26;
	yoff1=6;
	xoff2=-2;
	yoff2=-6;

	yfix=-3;
}

void CKDlgMIP::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKDlgMIP)
	//}}AFX_DATA_MAP
}

BOOL CKDlgMIP::OnInitDialog() 
{
	CDialog::OnInitDialog();
		
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CBitmap bitmap ;
	CDC* pDC=GetDC();
	bitmap.LoadBitmap(IDB_BITMAP3);
	bkdc.CreateCompatibleDC (pDC);
	bkdc.SelectObject (&bitmap);
	ReleaseDC(pDC);	

	CRect rc;
	GetClientRect(rc);

	int i;
	int y1=24;
	int x1=rc.Width()/4;
	int x2=rc.Width()*5/8;

	for(i=0;i<5;i++)
	{
		staticrc[i].left=0;
		staticrc[i].right=rc.Width()/4;
		staticrc[i].top=(i+1)*y1+1;
		staticrc[i].bottom=(i+2)*y1;
	}
	staticrc[4].bottom=rc.bottom;

	int yoff=6;

	for(i=0;i<5;i++)
	{
		tedit[i*2].rc.left=x1+2;
		tedit[i*2].rc.right=x2;
		tedit[i*2+1].rc.left=x2+2;
		tedit[i*2+1].rc.right=rc.right;

		tedit[i*2].rc.top=(i+1)*y1+yoff;
		tedit[i*2].rc.bottom=(i+2)*y1+yoff-1;
		tedit[i*2+1].rc.top=(i+1)*y1+yoff;
		tedit[i*2+1].rc.bottom=(i+2)*y1+yoff-1;
	}

//	tedit[
	tedit[8].rc.bottom+=1;
	tedit[9].rc.bottom+=1;

	int ret;
	ret=tedit[0].SubclassDlgItem(IDC_EDIT1,this);
	ret=tedit[1].SubclassDlgItem(IDC_EDIT2,this);
	ret=tedit[2].SubclassDlgItem(IDC_EDIT3,this);
	ret=tedit[3].SubclassDlgItem(IDC_EDIT4,this);
	ret=tedit[4].SubclassDlgItem(IDC_EDIT5,this);
	ret=tedit[5].SubclassDlgItem(IDC_EDIT6,this);
	ret=tedit[6].SubclassDlgItem(IDC_EDIT7,this);
	ret=tedit[7].SubclassDlgItem(IDC_EDIT8,this);
	ret=tedit[8].SubclassDlgItem(IDC_EDIT9,this);
	ret=tedit[9].SubclassDlgItem(IDC_EDIT10,this);

	tedit[0].SetWindowText("192.168.0.1");
	tedit[1].SetWindowText("5000");
	tedit[2].SetWindowText("192.168.0.1");

	CFont* font=GetFont();
	
	for(i=0;i<10;i++)
	{
		tedit[i].setfont((HFONT)font->m_hObject);
		tedit[i].setface(2);
		tedit[i].MoveWindow(&(tedit[i].rc),TRUE);
	}

	tedit[8].fixflag =TRUE;
	tedit[9].fixflag =TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_MESSAGE_MAP(CKDlgMIP, CDialog)
	//{{AFX_MSG_MAP(CKDlgMIP)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	ON_EN_CHANGE(IDC_EDIT2, OnChangeEdit2)
	ON_EN_CHANGE(IDC_EDIT3, OnChangeEdit3)
	ON_EN_CHANGE(IDC_EDIT4, OnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT5, OnChangeEdit5)
	ON_EN_CHANGE(IDC_EDIT6, OnChangeEdit6)
	ON_EN_CHANGE(IDC_EDIT7, OnChangeEdit7)
	ON_EN_CHANGE(IDC_EDIT8, OnChangeEdit8)
	ON_EN_CHANGE(IDC_EDIT9, OnChangeEdit9)
	ON_EN_CHANGE(IDC_EDIT10, OnChangeEdit10)
	ON_BN_CLICKED(IDC_STATIC101, OnStatic101)
	ON_BN_CLICKED(IDC_STATIC102, OnStatic102)
	ON_BN_CLICKED(IDC_STATIC103, OnStatic103)
	ON_BN_CLICKED(IDC_STATIC104, OnStatic104)
	ON_BN_CLICKED(IDC_STATIC105, OnStatic105)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKDlgMIP message handlers

HBRUSH CKDlgMIP::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
//	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	
	// TODO: Return a different brush if the default is not desired
//	return hbr;

	pDC->SetBkMode(1);

	return hbrush;
}

BOOL CKDlgMIP::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);

	pDC->BitBlt(0,0,rc.Width(),rc.Height(),&bkdc,
				60,40,
				SRCCOPY);
	int i;

	if(bstored==FALSE)
	{
		bstored=TRUE;

		for(i=0;i<5;i++)
		{
			CBitmap	bitmap;
			staticdc[i].CreateCompatibleDC (pDC);
			bitmap.CreateCompatibleBitmap (pDC,staticrc[i].right-staticrc[i].left,staticrc[i].bottom-staticrc[i].top);
			staticdc[i].SelectObject (&bitmap);
			staticdc[i].BitBlt (0,0,staticrc[i].right-staticrc[i].left,staticrc[i].bottom-staticrc[i].top,pDC,staticrc[i].left,staticrc[i].top,SRCCOPY);
		}

		for(i=0;i<8;i++)
		{
			CBitmap	bitmap;
			tedit[i].bkdc.CreateCompatibleDC (pDC);
			bitmap.CreateCompatibleBitmap (pDC,tedit[i].rc.right-tedit[i].rc.left+1,tedit[i].rc.bottom-tedit[i].rc.top);
			tedit[i].bkdc.SelectObject (&bitmap);
			tedit[i].bkdc.BitBlt (0,0,tedit[i].rc.right-tedit[i].rc.left+1,tedit[i].rc.bottom-tedit[i].rc.top,pDC,tedit[i].rc.left-1,tedit[i].rc.top+yfix-2,SRCCOPY);
		}

		CBitmap	bitmap8;
		tedit[8].bkdc.CreateCompatibleDC (pDC);
		bitmap8.CreateCompatibleBitmap (pDC,tedit[8].rc.right-tedit[8].rc.left+1,tedit[8].rc.bottom-tedit[8].rc.top-yfix);
		tedit[8].bkdc.SelectObject (&bitmap8);
		tedit[8].bkdc.BitBlt (0,0,tedit[8].rc.right-tedit[8].rc.left+1,tedit[8].rc.bottom-tedit[8].rc.top-yfix,pDC,tedit[8].rc.left-1,tedit[8].rc.top+yfix-2,SRCCOPY);

		CBitmap	bitmap9;
		tedit[9].bkdc.CreateCompatibleDC (pDC);
		bitmap9.CreateCompatibleBitmap (pDC,tedit[9].rc.right-tedit[9].rc.left+1,tedit[9].rc.bottom-tedit[9].rc.top-yfix);
		tedit[9].bkdc.SelectObject (&bitmap9);
		tedit[9].bkdc.BitBlt (0,0,tedit[9].rc.right-tedit[9].rc.left+1,tedit[9].rc.bottom-tedit[9].rc.top-yfix,pDC,tedit[9].rc.left-1,tedit[9].rc.top+yfix-2,SRCCOPY);
	}
	
	CPen mpen(PS_SOLID,1,::GetSysColor(COLOR_BTNSHADOW));
	CPen* lpen=pDC->SelectObject(&mpen);
	CFont* lfont=GetFont();
	CFont* pfont=pDC->SelectObject(lfont);
	pDC->SetBkMode(1);

//	int y1=18;
//	int y2=20;
//	int y3=5;
//	int y4=5;

//	int i;
	int y1=24;
	int x1=rc.Width()/4;
	int x2=rc.Width()*5/8;

	pDC->MoveTo(x1,0);
	pDC->LineTo(x1,rc.bottom);
	pDC->MoveTo(x2,0);
	pDC->LineTo(x2,rc.bottom);

	pDC->MoveTo(0,y1);
	pDC->LineTo(rc.right,y1);
	pDC->MoveTo(0,2*y1);
	pDC->LineTo(rc.right,2*y1);
	pDC->MoveTo(0,3*y1);
	pDC->LineTo(rc.right,3*y1);
	pDC->MoveTo(0,4*y1);
	pDC->LineTo(rc.right,4*y1);
	pDC->MoveTo(0,5*y1);
	pDC->LineTo(rc.right,5*y1);
	pDC->MoveTo(0,6*y1);
	pDC->LineTo(rc.right,6*y1);
	pDC->MoveTo(0,7*y1);
	pDC->LineTo(rc.right,7*y1);

	pDC->TextOut(7,6,"频道号",6);
	pDC->TextOut(x1+2,6,"IP地址",6);
	pDC->TextOut(x2+2,6,"端口号",6);	

	pDC->DrawIcon(staticrc[0].left+xoff2 ,staticrc[0].top+yoff2+2 ,hIcon1);

	pDC->TextOut(staticrc[0].left+xoff1 ,staticrc[0].top+yoff1 ,"1 频道");
	pDC->TextOut(staticrc[1].left+xoff1 ,staticrc[1].top+yoff1 ,"2 频道");
	pDC->TextOut(staticrc[2].left+xoff1 ,staticrc[2].top+yoff1 ,"3 频道");
	pDC->TextOut(staticrc[3].left+xoff1 ,staticrc[3].top+yoff1 ,"4 频道");
	pDC->TextOut(staticrc[4].left+xoff1 ,staticrc[4].top+yoff1 ,"5 频道");

	pDC->SelectObject(pfont);

	return 1;
}

void CKDlgMIP::ChangeEdit(UINT nID,int i)
{
//	CRect rc;
//	GetClientRect(&rc);

	CClientDC dc(this);

	if(i<8)
	{
		dc.BitBlt(tedit[i].rc.left-1 ,tedit[i].rc.top+yfix-2,
				tedit[i].rc.right -tedit[i].rc.left ,tedit[i].rc.bottom -tedit[i].rc.top ,
				&(tedit[i].bkdc),0,0,SRCCOPY);
	}
	else
	{
		dc.BitBlt(tedit[i].rc.left-1 ,tedit[i].rc.top+yfix-2,
				tedit[i].rc.right -tedit[i].rc.left ,tedit[i].rc.bottom -tedit[i].rc.top-yfix ,
				&(tedit[i].bkdc),0,0,SRCCOPY);
	}

	CKEdit* pwin=(CKEdit*)GetDlgItem(nID);
	pwin->InvalidateRect(NULL,TRUE);
	pwin->drawtedit2d();	
}

void CKDlgMIP::OnChangeEdit1() 
{
	ChangeEdit(IDC_EDIT1,0);
}

void CKDlgMIP::OnChangeEdit2() 
{
	ChangeEdit(IDC_EDIT2,1);
}

void CKDlgMIP::OnChangeEdit3() 
{
	ChangeEdit(IDC_EDIT3,2);
}

void CKDlgMIP::OnChangeEdit4() 
{
	ChangeEdit(IDC_EDIT4,3);
}

void CKDlgMIP::OnChangeEdit5() 
{
	ChangeEdit(IDC_EDIT5,4);
}

void CKDlgMIP::OnChangeEdit6() 
{
	ChangeEdit(IDC_EDIT6,5);	
}

void CKDlgMIP::OnChangeEdit7() 
{
	ChangeEdit(IDC_EDIT7,6);
}

void CKDlgMIP::OnChangeEdit8() 
{
	ChangeEdit(IDC_EDIT8,7);
}

void CKDlgMIP::OnChangeEdit9() 
{
	ChangeEdit(IDC_EDIT9,8);
}

void CKDlgMIP::OnChangeEdit10() 
{
	ChangeEdit(IDC_EDIT10,9);
}

void CKDlgMIP::Static10X(int j)
{
	int i=focus;
	CClientDC dc(this);
	CFont* lfont=GetFont();
	CFont* pfont=dc.SelectObject(lfont);
	dc.SetBkMode(1);

	dc.BitBlt(staticrc[i].left ,staticrc[i].top ,
			staticrc[i].right -staticrc[i].left ,staticrc[i].bottom -staticrc[i].top ,
			&(staticdc[i]),0,0,SRCCOPY);
	char text[50];
	memset((void*)text,0,sizeof(text));
	sprintf(text,"%d %s",i+1,"频道");
	dc.TextOut(staticrc[i].left+xoff1 ,staticrc[i].top+yoff1 ,text,6);

	i=j;
	focus=j;
	dc.BitBlt(staticrc[i].left ,staticrc[i].top ,
			staticrc[i].right -staticrc[i].left ,staticrc[i].bottom -staticrc[i].top ,
			&(staticdc[i]),0,0,SRCCOPY);
	dc.DrawIcon(staticrc[i].left+xoff2 ,staticrc[i].top+yoff2+2 ,hIcon1);
	memset((void*)text,0,sizeof(text));
	sprintf(text,"%d %s",i+1,"频道");
	dc.TextOut(staticrc[i].left+xoff1 ,staticrc[i].top+yoff1 ,text,6);	dc.SelectObject(pfont);
	return ;
}

void CKDlgMIP::OnStatic101() 
{
	Static10X(0);
}

void CKDlgMIP::OnStatic102() 
{
	Static10X(1);
}

void CKDlgMIP::OnStatic103() 
{
	Static10X(2);
}

void CKDlgMIP::OnStatic104() 
{
	Static10X(3);
}

void CKDlgMIP::OnStatic105() 
{
	Static10X(4);
}

BOOL CKDlgMIP::PreTranslateMessage(MSG* pMsg) 
{
	int i;

	CClientDC dc(this);
	dc.SetBkMode(1);
	CFont* font=GetFont();
	dc.SelectObject(font);
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_UP) 
	{
		i=focus;
		i--;
		if(i<0)
			i=4;
		Static10X(i);
	}
	else if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DOWN) 
	{
		i=focus;
		i++;
		if(i>4)
			i=0;
		Static10X(i);
	}	
	return CDialog::PreTranslateMessage(pMsg);
}

int CKDlgMIP::ShowDialog()
{
	// can be constructed with a resource template or InitModalIndirect
	ASSERT(m_lpszTemplateName != NULL || m_hDialogTemplate != NULL ||
		m_lpDialogTemplate != NULL);

	// load resource as necessary
	LPCDLGTEMPLATE lpDialogTemplate = m_lpDialogTemplate;
	HGLOBAL hDialogTemplate = m_hDialogTemplate;
	HINSTANCE hInst = AfxGetResourceHandle();
	if (m_lpszTemplateName != NULL)
	{
		hInst = AfxFindResourceHandle(m_lpszTemplateName, RT_DIALOG);
		HRSRC hResource = ::FindResource(hInst, m_lpszTemplateName, RT_DIALOG);
		hDialogTemplate = LoadResource(hInst, hResource);
	}
	if (hDialogTemplate != NULL)
		lpDialogTemplate = (LPCDLGTEMPLATE)LockResource(hDialogTemplate);

	// return -1 in case of failure to load the dialog template resource
	if (lpDialogTemplate == NULL)
		return -1;

	// disable parent (before creating dialog)
	HWND hWndParent = PreModal();
	AfxUnhookWindowCreate();
	BOOL bEnableParent = FALSE;
	if (hWndParent != NULL && ::IsWindowEnabled(hWndParent))
	{
		::EnableWindow(hWndParent, FALSE);
		bEnableParent = TRUE;
	}

	TRY
	{
		// create modeless dialog
		AfxHookWindowCreate(this);
		if (CreateDlgIndirect(lpDialogTemplate,
						CWnd::FromHandle(hWndParent), hInst))
		{
			if (m_nFlags & WF_CONTINUEMODAL)
			{
				// enter modal loop
				DWORD dwFlags = MLF_SHOWONIDLE;
				if (GetStyle() & DS_NOIDLEMSG)
					dwFlags |= MLF_NOIDLEMSG;
				
				CKEdit* pwin=(CKEdit*)GetDlgItem(IDC_EDIT1);  // addition
				pwin->focus =1;
				pwin->SetFocus();
				pwin->drawtedit2d();

				VERIFY(RunModalLoop(dwFlags) == m_nModalResult);
			}

			// hide the window before enabling the parent, etc.
			if (m_hWnd != NULL)
				SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW|
					SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);
		}
	}
	CATCH_ALL(e)
	{
		m_nModalResult = -1;
	}
	END_CATCH_ALL

	if (bEnableParent)
		::EnableWindow(hWndParent, TRUE);
	if (hWndParent != NULL && ::GetActiveWindow() == m_hWnd)
		::SetActiveWindow(hWndParent);

	// destroy modal window
	DestroyWindow();
	PostModal();

	// unlock/free resources as necessary
	if (m_lpszTemplateName != NULL || m_hDialogTemplate != NULL)
		UnlockResource(hDialogTemplate);
	if (m_lpszTemplateName != NULL)
		FreeResource(hDialogTemplate);

	return m_nModalResult;
}
