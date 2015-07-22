// KDlgChan.cpp : implementation file
//

#include "stdafx.h"
#include "KMP.h"
#include "KDlgChan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKDlgChan dialog

CKDlgChan::CKDlgChan(CWnd* pParent /*=NULL*/)
	: CDialog(CKDlgChan::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKDlgChan)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_hIcon = AfxGetApp()->LoadIcon(IDI_KICON1);
	hbrush=(HBRUSH)GetStockObject(HOLLOW_BRUSH);
}


void CKDlgChan::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKDlgChan)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BOOL CKDlgChan::OnInitDialog() 
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
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_MESSAGE_MAP(CKDlgChan, CDialog)
	//{{AFX_MSG_MAP(CKDlgChan)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKDlgChan message handlers

BOOL CKDlgChan::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);

	pDC->BitBlt(0,0,rc.Width(),rc.Height(),&bkdc,
				60,40,  
				SRCCOPY);
	return 1;
}

HBRUSH CKDlgChan::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
//	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO: Return a different brush if the default is not desired
//	return hbr;

	pDC->SetBkMode(1);

	return hbrush;
}
