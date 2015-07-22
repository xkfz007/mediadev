// KDlgIP.cpp : implementation file
//

#include "stdafx.h"
#include "KMP.h"
#include "KDlgIP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKDlgIP dialog


CKDlgIP::CKDlgIP(CWnd* pParent /*=NULL*/)
	: CDialog(CKDlgIP::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKDlgIP)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDI_KICON1);
	hbrush=(HBRUSH)GetStockObject(HOLLOW_BRUSH);

	bstored=FALSE;
	yfix=-3;
}


void CKDlgIP::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKDlgIP)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKDlgIP, CDialog)
	//{{AFX_MSG_MAP(CKDlgIP)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_EN_CHANGE(IDC_EDIT20, OnChangeEdit1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKDlgIP message handlers

BOOL CKDlgIP::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);

	pDC->BitBlt(0,0,rc.Width(),rc.Height(),&bkdc,
				60,40,
				SRCCOPY);

	if(bstored==FALSE)
	{
		bstored=TRUE;

		CBitmap	bitmap;
		ip1.bkdc.CreateCompatibleDC (pDC);
		bitmap.CreateCompatibleBitmap (pDC,ip1.rc.right-ip1.rc.left,ip1.rc.bottom-ip1.rc.top);
		ip1.bkdc.SelectObject (&bitmap);
		ip1.bkdc.BitBlt (0,0,ip1.rc.right-ip1.rc.left,ip1.rc.bottom-ip1.rc.top,pDC,ip1.rc.left,ip1.rc.top+yfix,SRCCOPY);
	}

	CKEdit* pwin=(CKEdit*)GetDlgItem(IDC_EDIT20);
	pwin->drawtedit3d();

	return 1;
}


BOOL CKDlgIP::OnInitDialog() 
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

	btn1.SubclassDlgItem(IDOK,this);
	btn2.SubclassDlgItem(IDCANCEL,this);
	
	WINDOWPLACEMENT wp;
	CWnd *pwin;
	pwin=GetDlgItem(IDC_EDIT20);
	pwin->GetWindowPlacement(&wp);	
	ip1.rc.left =wp.rcNormalPosition.left ;
	ip1.rc.top =wp.rcNormalPosition.top;
	ip1.rc.right =wp.rcNormalPosition.right ;
	ip1.rc.bottom =wp.rcNormalPosition.bottom ;
	ip1.SubclassDlgItem(IDC_EDIT20,this);
/*
	RECT textrc;

    ::SendMessage(ip1.m_hWnd,EM_GETRECT,0,(LPARAM)&textrc);

	textrc.left=10;
	textrc.right=40;

	::SendMessage(ip1.m_hWnd,EM_SETRECT,0,(LPARAM)&textrc);

    ::SendMessage(ip1.m_hWnd,EM_GETRECT,0,(LPARAM)&textrc);
*/
	ip1.SetWindowText("192.168.0.1");

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CKDlgIP::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	pDC->SetBkMode(1);

	return hbrush;
}

void CKDlgIP::OnChangeEdit1() 
{
	CRect rc;
	GetClientRect(&rc);

	CClientDC dc(this);

	dc.BitBlt(ip1.rc.left ,ip1.rc.top +yfix,
			ip1.rc.right -ip1.rc.left ,ip1.rc.bottom -ip1.rc.top ,
			&(ip1.bkdc),0,0,SRCCOPY);

	CKEdit* pwin=(CKEdit*)GetDlgItem(IDC_EDIT20);
	pwin->InvalidateRect(NULL,TRUE);
	pwin->drawtedit3d();	
}
