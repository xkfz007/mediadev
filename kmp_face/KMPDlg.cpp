// KMPDlg.cpp : implementation file
//

#include "stdafx.h"
#include "KMP.h"
#include "KMPDlg.h"
#include "kdlgchan.h"
#include "kdlgip.h"
#include "kdlgmip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON	 m_hIcon;
	HBRUSH   hbrush;
	CDC      bkdc;
	CKButton btn1;
	// Generated message map functions
	//{{AFX_MSG(CAboutDlg)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT

	m_hIcon = AfxGetApp()->LoadIcon(IDI_KICON1);
	hbrush=(HBRUSH)GetStockObject(HOLLOW_BRUSH);
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BOOL CAboutDlg::OnInitDialog() 
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

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	pDC->SetBkMode(1);

	return hbrush;
}

BOOL CAboutDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);

	pDC->BitBlt(0,0,rc.Width(),rc.Height(),&bkdc, 60,30, SRCCOPY);
	return 1;
}

/////////////////////////////////////////////////////////////////////////////
// CKMPDlg dialog

CKMPDlg::CKMPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKMPDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKMPDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32

	hIcon = AfxGetApp()->LoadIcon(IDI_KICON1);

	menuflag=0;

	status = 2;

 	bstored=FALSE;

	focus1=-100;
	focus2=-100;

	int i=0;

	for(i=0;i<9;i++)
	{
		hIconList[i][0]=AfxGetApp()->LoadIcon(IDI_KICON1);
		hIconList[i][1]=AfxGetApp()->LoadIcon(IDI_KICON2);
	}

	shadow = GetSysColor(COLOR_3DSHADOW);
	hilight = GetSysColor(COLOR_3DHILIGHT);	
}

void CKMPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKMPDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CKMPDlg, CDialog)
	//{{AFX_MSG_MAP(CKMPDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCRBUTTONDOWN()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKMPDlg message handlers

BOOL CKMPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(hIcon, TRUE);		// Set big icon
	SetIcon(hIcon, FALSE);		// Set small icon

	//-----------------------------------------------------------------

	CBitmap  bitmap ;
	CDC* pDC=GetDC();
	bitmap.LoadBitmap(IDB_BITMAP3);
	bkdc.CreateCompatibleDC(pDC);
	bkdc.SelectObject(bitmap);
	ReleaseDC(pDC);

	CRect boardRect;
	boardRect.left = 20;
	boardRect.right =140;
	boardRect.top = 20;
	boardRect.bottom = 200;

	menu[0].appendmenu(ID_KNETSTREAM,"选择播放频道");
	menu[0].appendmenu(ID_KLOCALFILE,"选择播放文件");
	menu[0].appendmenu(MF_SEPARATOR,"");
	menu[0].appendmenu(ID_KCHANNEL,"频道参数设置");
	menu[0].appendmenu(ID_KIPADDR,"IP地址设置");
	menu[0].appendmenu(MF_SEPARATOR,"");
	menu[0].appendmenu(ID_KEXIT,"退出");
	menu[0].appendmenu(0,"退出");
	menu[0].setparent(this->m_hWnd);
	menu[0].CreateEx(0,AfxRegisterWndClass(0),"menu",WS_POPUP,boardRect.left ,boardRect.top ,boardRect.right ,boardRect.bottom ,this->m_hWnd,NULL,NULL);
	menu[0].ShowWindow(SW_HIDE);

	menu[1].appendmenu(ID_KPLAY,"正常播放");
	menu[1].appendmenu(ID_KPAUSE,"暂停播放");
	menu[1].appendmenu(ID_KSTOP,"停止播放");
	menu[1].appendmenu(MF_SEPARATOR,"");
	menu[1].appendmenu(ID_KFORWARD,"向前快进");
	menu[1].appendmenu(ID_KBACKWARD,"向后快退");
	menu[1].appendmenu(ID_KBACKWARD,"顺序(循环)");
	menu[1].appendmenu(0,"快退");
	menu[1].setparent(this->m_hWnd);
	menu[1].CreateEx(0,AfxRegisterWndClass(0),"menu",WS_POPUP,boardRect.left ,boardRect.top ,boardRect.right ,boardRect.bottom ,this->m_hWnd,NULL,NULL);
	menu[1].ShowWindow(SW_HIDE);

	menu[2].appendmenu(ID_KCOLOR,"单色/彩色");
	menu[2].appendmenu(MF_SEPARATOR,"");
	menu[2].appendmenu(ID_KHALFSCREEN,"一半窗口");
	menu[2].appendmenu(ID_KSCREEN,"原始窗口");
	menu[2].appendmenu(ID_KDOUBLESCREEN,"双倍窗口");
	menu[2].appendmenu(ID_KFULLSCREEN,"全屏");
	menu[2].appendmenu(0,"全屏");
	menu[2].setparent(this->m_hWnd);
	menu[2].CreateEx(0,AfxRegisterWndClass(0),"menu",WS_POPUP,boardRect.left ,boardRect.top ,boardRect.right ,boardRect.bottom ,this->m_hWnd,NULL,NULL);
	menu[2].ShowWindow(SW_HIDE);

	menu[3].appendmenu(ID_KADDVOC,"加大声音");
	menu[3].appendmenu(ID_KDELVOC,"减小声音");
	menu[3].appendmenu(MF_SEPARATOR,"");
	menu[3].appendmenu(ID_KLEFTSOUND,"左声道");
	menu[3].appendmenu(ID_KRIGHTSOUND,"右声道");
	menu[3].appendmenu(ID_KSTERO,"立体声");
	menu[3].appendmenu(0,"立体声");
	menu[3].setparent(this->m_hWnd);
	menu[3].CreateEx(0,AfxRegisterWndClass(0),"menu",WS_POPUP,boardRect.left ,boardRect.top ,boardRect.right ,boardRect.bottom ,this->m_hWnd,NULL,NULL);
	menu[3].ShowWindow(SW_HIDE);

	menu[4].appendmenu(ID_KISOITU,"支持标准");
	menu[4].appendmenu(ID_KABOUT,"关于...");
	menu[4].appendmenu(0,"关于");
	menu[4].setparent(this->m_hWnd);
	menu[4].CreateEx(0,AfxRegisterWndClass(0),"menu",WS_POPUP,boardRect.left ,boardRect.top ,boardRect.right ,boardRect.bottom ,this->m_hWnd,NULL,NULL);
	menu[4].ShowWindow(SW_HIDE);

	CFont* pfont=GetFont();
	for(int i=0;i<5;i++)
	{
		menu[i].setfont((HFONT)pfont->m_hObject);
	}

	barID[0]=ID_KDLGCHAN;
	barID[1]=ID_KLOCALFILE;
	barID[2]=ID_KPLAY;
	barID[3]=ID_KPAUSE;
	barID[4]=ID_KSTOP;
	barID[5]=ID_KFORWARD;
	barID[6]=ID_KBACKWARD;
	barID[7]=ID_KBACKWARD;
	barID[8]=ID_KFULLSCREEN;

	memset((void*)ktmenu,0,sizeof(ktmenu));
	memset((void*)khelp, 0,sizeof(khelp));
	memset((void*)kmenu, 0,sizeof(kmenu));
	memset((void*)kbar,0,sizeof(kbar));
	memset((void*)knum,0,sizeof(knum));

	strcpy(ktmenu[0],"文件(F)");
	strcpy(ktmenu[1],"控制(C)");
	strcpy(ktmenu[2],"视频(V)");
	strcpy(ktmenu[3],"音频(A)");
	strcpy(ktmenu[4],"帮助(H)");

	strcpy(khelp[0],"选择播放频道");
	strcpy(khelp[1],"选择播放文件");
	strcpy(khelp[2],"播放");
	strcpy(khelp[3],"暂停");
	strcpy(khelp[4],"停止");
	strcpy(khelp[5],"快进");
	strcpy(khelp[6],"快退");
	strcpy(khelp[7],"顺序(循环)");
	strcpy(khelp[8],"全屏");
	strcpy(khelp[9],"准备好 ...");
	strcpy(khelp[10],"正在播放 ...");
	strcpy(khelp[11],"暂停 ...");

	CWnd* pwin;
	WINDOWPLACEMENT wp;

	pwin=GetDlgItem(IDC_BUTTON11);
	pwin->GetWindowPlacement(&wp);
	kmenu[0].left = wp.rcNormalPosition.left;
	kmenu[0].right =wp.rcNormalPosition .right ;
	kmenu[0].top =wp.rcNormalPosition .top ;
	kmenu[0].bottom =wp.rcNormalPosition .bottom ;

	pwin=GetDlgItem(IDC_BUTTON12);
	pwin->GetWindowPlacement(&wp);
	kmenu[1].left = wp.rcNormalPosition.left;
	kmenu[1].right =wp.rcNormalPosition .right ;
	kmenu[1].top =wp.rcNormalPosition .top ;
	kmenu[1].bottom =wp.rcNormalPosition .bottom ;

	pwin=GetDlgItem(IDC_BUTTON13);
	pwin->GetWindowPlacement(&wp);
	kmenu[2].left = wp.rcNormalPosition.left;
	kmenu[2].right =wp.rcNormalPosition .right ;
	kmenu[2].top =wp.rcNormalPosition .top ;
	kmenu[2].bottom =wp.rcNormalPosition .bottom ;

	pwin=GetDlgItem(IDC_BUTTON14);
	pwin->GetWindowPlacement(&wp);
	kmenu[3].left = wp.rcNormalPosition.left;
	kmenu[3].right =wp.rcNormalPosition .right ;
	kmenu[3].top =wp.rcNormalPosition .top ;
	kmenu[3].bottom =wp.rcNormalPosition .bottom ;

	pwin=GetDlgItem(IDC_BUTTON15);
	pwin->GetWindowPlacement(&wp);
	kmenu[4].left = wp.rcNormalPosition.left;
	kmenu[4].right =wp.rcNormalPosition .right ;
	kmenu[4].top =wp.rcNormalPosition .top ;
	kmenu[4].bottom =wp.rcNormalPosition .bottom ;

	pwin=GetDlgItem(IDC_BUTTON21);
	pwin->GetWindowPlacement(&wp);
	kbar[0].left = wp.rcNormalPosition.left;
	kbar[0].right =wp.rcNormalPosition .right ;
	kbar[0].top =wp.rcNormalPosition .top ;
	kbar[0].bottom =wp.rcNormalPosition .bottom ;

	pwin=GetDlgItem(IDC_BUTTON22);
	pwin->GetWindowPlacement(&wp);
	kbar[1].left = wp.rcNormalPosition.left;
	kbar[1].right =wp.rcNormalPosition .right ;
	kbar[1].top =wp.rcNormalPosition .top ;
	kbar[1].bottom =wp.rcNormalPosition .bottom ;

	pwin=GetDlgItem(IDC_BUTTON23);
	pwin->GetWindowPlacement(&wp);
	kbar[2].left = wp.rcNormalPosition.left;
	kbar[2].right =wp.rcNormalPosition .right ;
	kbar[2].top =wp.rcNormalPosition .top ;
	kbar[2].bottom =wp.rcNormalPosition .bottom ;

	pwin=GetDlgItem(IDC_BUTTON24);
	pwin->GetWindowPlacement(&wp);
	kbar[3].left = wp.rcNormalPosition.left;
	kbar[3].right =wp.rcNormalPosition .right ;
	kbar[3].top =wp.rcNormalPosition .top ;
	kbar[3].bottom =wp.rcNormalPosition .bottom ;

	pwin=GetDlgItem(IDC_BUTTON25);
	pwin->GetWindowPlacement(&wp);
	kbar[4].left = wp.rcNormalPosition.left;
	kbar[4].right =wp.rcNormalPosition .right ;
	kbar[4].top =wp.rcNormalPosition .top ;
	kbar[4].bottom =wp.rcNormalPosition .bottom ;

	pwin=GetDlgItem(IDC_BUTTON26);
	pwin->GetWindowPlacement(&wp);
	kbar[5].left = wp.rcNormalPosition.left;
	kbar[5].right =wp.rcNormalPosition .right ;
	kbar[5].top =wp.rcNormalPosition .top ;
	kbar[5].bottom =wp.rcNormalPosition .bottom ;

	pwin=GetDlgItem(IDC_BUTTON27);
	pwin->GetWindowPlacement(&wp);
	kbar[6].left = wp.rcNormalPosition.left;
	kbar[6].right =wp.rcNormalPosition .right ;
	kbar[6].top =wp.rcNormalPosition .top ;
	kbar[6].bottom =wp.rcNormalPosition .bottom ;

	pwin=GetDlgItem(IDC_BUTTON28);
	pwin->GetWindowPlacement(&wp);
	kbar[7].left = wp.rcNormalPosition.left;
	kbar[7].right =wp.rcNormalPosition .right ;
	kbar[7].top =wp.rcNormalPosition .top ;
	kbar[7].bottom =wp.rcNormalPosition .bottom ;

	pwin=GetDlgItem(IDC_BUTTON29);
	pwin->GetWindowPlacement(&wp);
	kbar[8].left = wp.rcNormalPosition.left;
	kbar[8].right =wp.rcNormalPosition .right ;
	kbar[8].top =wp.rcNormalPosition .top ;
	kbar[8].bottom =wp.rcNormalPosition .bottom ;

	pwin=GetDlgItem(IDC_BUTTON30);
	pwin->GetWindowPlacement(&wp);
	kbar[9].left = wp.rcNormalPosition.left;
	kbar[9].right =wp.rcNormalPosition .right ;
	kbar[9].top =wp.rcNormalPosition .top ;
	kbar[9].bottom =wp.rcNormalPosition .bottom ;

	pwin=GetDlgItem(IDC_BUTTON41);
	pwin->GetWindowPlacement(&wp);
	knum[0].left = wp.rcNormalPosition.left;
	knum[0].right =wp.rcNormalPosition .right ;
	knum[0].top =wp.rcNormalPosition .top ;
	knum[0].bottom =wp.rcNormalPosition .bottom ;

	pwin=GetDlgItem(IDC_BUTTON42);
	pwin->GetWindowPlacement(&wp);
	knum[1].left = wp.rcNormalPosition.left;
	knum[1].right =wp.rcNormalPosition .right ;
	knum[1].top =wp.rcNormalPosition .top ;
	knum[1].bottom =wp.rcNormalPosition .bottom ;

	pwin=GetDlgItem(IDC_BUTTON43);
	pwin->GetWindowPlacement(&wp);
	knum[2].left = wp.rcNormalPosition.left;
	knum[2].right =wp.rcNormalPosition .right ;
	knum[2].top =wp.rcNormalPosition .top ;
	knum[2].bottom =wp.rcNormalPosition .bottom ;

	pwin=GetDlgItem(IDC_BUTTON44);
	pwin->GetWindowPlacement(&wp);
	knum[3].left = wp.rcNormalPosition.left;
	knum[3].right =wp.rcNormalPosition .right ;
	knum[3].top =wp.rcNormalPosition .top ;
	knum[3].bottom =wp.rcNormalPosition .bottom ;

	pwin=GetDlgItem(IDC_BUTTON45);
	pwin->GetWindowPlacement(&wp);
	knum[4].left = wp.rcNormalPosition.left;
	knum[4].right =wp.rcNormalPosition .right ;
	knum[4].top =wp.rcNormalPosition .top ;
	knum[4].bottom =wp.rcNormalPosition .bottom ;

	pwin=GetDlgItem(IDC_BUTTON46);
	pwin->GetWindowPlacement(&wp);
	knum[5].left = wp.rcNormalPosition.left;
	knum[5].right =wp.rcNormalPosition .right ;
	knum[5].top =wp.rcNormalPosition .top ;
	knum[5].bottom =wp.rcNormalPosition .bottom ;

	xoff=10;
	yoff=6;

	pwin=GetDlgItem(IDC_STATIC200);
	pwin->GetWindowPlacement(&wp);
	sliderrc.left = wp.rcNormalPosition.left-1;
	sliderrc.right =wp.rcNormalPosition .right ;
	sliderrc.top =wp.rcNormalPosition .top ;
	sliderrc.bottom =wp.rcNormalPosition .bottom ;

	sliderx=sliderrc.left;
	slidery=sliderrc.top-5;
	sliderylen=8;
	sliderflag=FALSE;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CKMPDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if(focus1>=0)
	{
		CClientDC dc(this);
		dc.SetBkMode(1);
		CFont* font=GetFont();
		CFont* lfont=dc.SelectObject(font);

		drawmenu(&dc,focus1,0);
		menu[focus1].ShowWindow(SW_HIDE);
		focus1=-100;
		menuflag=0;

		dc.SelectObject(lfont);
	}

	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}

}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CKMPDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CKMPDlg::OnQueryDragIcon()
{
	return (HCURSOR) hIcon;
}

//----------------------------- step 1 : draw ui -----------------------------------------------

HBRUSH CKMPDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{	
	pDC->SetBkMode(TRANSPARENT);

	return hbrush;
}

BOOL CKMPDlg::OnEraseBkgnd(CDC* pDC)
{
	CRect rc;
	GetClientRect(rc);	

	pDC->SetBkMode(1);
	CFont* font = this->GetFont();
	CFont* oldfont= pDC->SelectObject(font);

	pDC->BitBlt(0,0,rc.Width(),rc.Height(),&bkdc,30,61,SRCCOPY);
	int i;
	int cx;
	int cy;

	if(bstored==FALSE)
	{
		bstored=TRUE;

		for(i=0;i<5;i++)
		{
			cx=kmenu[i].right-kmenu[i].left;
			cy=kmenu[i].bottom-kmenu[i].top;
	
			CBitmap	bitmap;
			menudc[i].CreateCompatibleDC (pDC);
			bitmap.CreateCompatibleBitmap (pDC,cx,cy);
			menudc[i].SelectObject (&bitmap);
			menudc[i].BitBlt (0,0,cx,cy,pDC,kmenu[i].left,kmenu[i].top,SRCCOPY);
		}

		for(i=0;i<9;i++)
		{
 			cx=kbar[i].right-kbar[i].left;
			cy=kbar[i].bottom-kbar[i].top;
	
			CBitmap	bitmap;
			bardc[i].CreateCompatibleDC (pDC);
			bitmap.CreateCompatibleBitmap (pDC,cx,cy);
			bardc[i].SelectObject (&bitmap);
			bardc[i].BitBlt (0,0,cx,cy,pDC,kbar[i].left,kbar[i].top,SRCCOPY);
		}

		for(i=0;i<5;i++)	
		{
 			cx=knum[i].right-knum[i].left;
			cy=knum[i].bottom-knum[i].top;
	
			CBitmap	bitmap;
			numdc[i].CreateCompatibleDC (pDC);
			bitmap.CreateCompatibleBitmap (pDC,cx,cy);
			numdc[i].SelectObject (&bitmap);
			numdc[i].BitBlt (0,0,cx,cy,pDC,knum[i].left,knum[i].top,SRCCOPY);
		}
	}

	int x1,y1,x2,y2;

	for(i=0;i<5;i++)
	{
		x1=kmenu[i].left+2;
		y1=kmenu[i].top;
		x2=kmenu[i].right;
		y2=kmenu[i].bottom;

		pDC->TextOut(x1+7,y1+5,(LPCTSTR)(ktmenu[i]));
	}

	HICON hIcon = AfxGetApp()->LoadIcon(IDI_KICON1);

	for(i=0;i<9;i++)
	{
		x1=kbar[i].left;
		y1=kbar[i].top;
		x2=kbar[i].right;
		y2=kbar[i].bottom;

		pDC->DrawIcon(x1-6,y1-5,hIcon);   
	}

	char statictext[40];

	memset((void*)statictext,0,40);
	sprintf(statictext,"00:00");
	pDC->TextOut(knum[0].left+xoff+2,knum[0].top+yoff,statictext,strlen(statictext));

	memset((void*)statictext,0,40);
	sprintf(statictext,"30%%");
	pDC->TextOut(knum[0].left+xoff+80,knum[0].top+yoff,statictext,strlen(statictext));

	memset((void*)statictext,0,40);
	sprintf(statictext,"30:30");
	pDC->TextOut(knum[0].left+xoff+156,knum[0].top+yoff,statictext,strlen(statictext));

	CPen  pen1(PS_SOLID,1,::GetSysColor(COLOR_BTNSHADOW));
	CPen  pen2(PS_SOLID,1,::GetSysColor(COLOR_WINDOWFRAME));
	CPen  pen3(PS_SOLID,1,::GetSysColor(COLOR_BTNHILIGHT));
	CPen  pen4(PS_SOLID,1,::GetSysColor(COLOR_BTNFACE));
	CPen  *lpen=pDC->SelectObject(&pen4);
	pDC->MoveTo(sliderrc.left,sliderrc.top);
	pDC->LineTo(sliderrc.right,sliderrc.top);
	pDC->MoveTo(sliderrc.left,sliderrc.top+1);
	pDC->LineTo(sliderrc.right,sliderrc.top+1);
	pDC->MoveTo(sliderrc.left,sliderrc.top-5);
	pDC->LineTo(sliderrc.left,sliderrc.top+2);
	pDC->MoveTo(sliderrc.left+1,sliderrc.top-5);
	pDC->LineTo(sliderrc.left+1,sliderrc.top+2);
	pDC->MoveTo(sliderrc.right-1,sliderrc.top-5);
	pDC->LineTo(sliderrc.right-1,sliderrc.top+2);
	pDC->MoveTo(sliderrc.right,sliderrc.top-5);
	pDC->LineTo(sliderrc.right,sliderrc.top+2);
	pDC->SelectObject(&pen1);
	pDC->MoveTo(sliderx-1,slidery);
	pDC->LineTo(sliderx-1,slidery+sliderylen);
	pDC->SelectObject(&pen2);
	pDC->MoveTo(sliderx,slidery);
	pDC->LineTo(sliderx,slidery+sliderylen);
	pDC->SelectObject(&pen3);
	pDC->MoveTo(sliderx+1,slidery);
	pDC->LineTo(sliderx+1,slidery+sliderylen);
	pDC->SelectObject(lpen);						

	memset((void*)statictext,0,40);
	sprintf(statictext,"数据传输速率:1441k");
	pDC->TextOut(knum[1].left+xoff,knum[1].top+yoff,statictext,strlen(statictext));

	memset((void*)statictext,0,40);
	sprintf(statictext,"包数:14496");
	pDC->TextOut(knum[2].left+xoff,knum[2].top+yoff,statictext,strlen(statictext));

	memset((void*)statictext,0,40);
	sprintf(statictext,"准备好");
	drawnum(pDC,3);
	pDC->TextOut(knum[3].left+xoff,knum[3].top+yoff,statictext,strlen(statictext));

	memset((void*)statictext,0,40);
	sprintf(statictext,"时间 10:20:30");
	pDC->TextOut(knum[4].left+xoff,knum[4].top+yoff,statictext,strlen(statictext));

	memset((void*)statictext,0,40);
	sprintf(statictext,"欢迎使用美柯放影机");
	pDC->TextOut(knum[5].left+xoff,knum[5].top+yoff,statictext,strlen(statictext));

	return 1;
}

void CKMPDlg::drawmenu(CDC* pDC,int i,int j)
{
	CRect	rect;

	pDC->BitBlt (kmenu[i].left,
			   kmenu[i].top,
			   kmenu[i].right-kmenu[i].left,
			   kmenu[i].bottom-kmenu[i].top,
			   &(menudc[i]),
			   0,
			   0,
			   SRCCOPY);

	int x=kmenu[i].left+2;
	int y=kmenu[i].top;

	switch(j)
	{
	case -1:    // down
		x++;
		y++;
		pDC->Draw3dRect (&(kmenu[i]),
					::GetSysColor(COLOR_BTNSHADOW),
					::GetSysColor(COLOR_BTNHILIGHT));
		break;
	case 0:
		break;
	case 1:     // up

		pDC->Draw3dRect (&(kmenu[i]),
					::GetSysColor(COLOR_BTNHILIGHT),
					::GetSysColor(COLOR_BTNSHADOW));
		break;
	default :
		break;
	}
	
	pDC->TextOut(x+7,y+5,(LPCTSTR)(ktmenu[i]));

	return ;
}

void CKMPDlg::drawbar(CDC* pDC,int i,int j)  // j =-1 down; j=0 normal ;j=1 up
{
	CRect	rect;
	
	pDC->BitBlt (kbar[i].left,
			   kbar[i].top,
			   kbar[i].right-kbar[i].left,
			   kbar[i].bottom-kbar[i].top,
			   &(bardc[i]),
			   0,
			   0,
			   SRCCOPY);

	switch(j)  // 
	{
	case -1:
		if(barflag[i])
		{		
			pDC->Draw3dRect (&(kbar[i]),
						::GetSysColor(COLOR_BTNSHADOW),
						::GetSysColor(COLOR_BTNHILIGHT));

			pDC->DrawIcon (kbar[i].left-5,kbar[i].top-4,hIconList[i][0]);
		}
		else
		{	
   			pDC->DrawIcon (kbar[i].left-5,kbar[i].top-4,hIconList[i][1]);
		}

		break;
	case 0:
		if(barflag[i])
		{
			pDC->DrawIcon (kbar[i].left-6,kbar[i].top-5,hIconList[i][0]);
		}
		else
		{	
   			pDC->DrawIcon (kbar[i].left-6,kbar[i].top-5,hIconList[i][1]);
		}
		break;

	case 1:		
		if(barflag[i])
		{
			pDC->Draw3dRect (&(kbar[i]),
						::GetSysColor(COLOR_BTNHILIGHT),
						::GetSysColor(COLOR_BTNSHADOW) );
			pDC->DrawIcon (kbar[i].left-6,kbar[i].top-5,hIconList[i][0]);
		}
		else
		{	
			pDC->DrawIcon (kbar[i].left-6,kbar[i].top-5,hIconList[i][1]);
		}
		break;

	default:
		break;
	}

	return ;
}

void CKMPDlg::drawnum(CDC* pDC,int i)
{
	pDC->BitBlt (knum[i].left,
			   knum[i].top,
			   knum[i].right-knum[i].left,
			   knum[i].bottom-knum[i].top,
			   &(numdc[i]),
			   0,
			   0,
			   SRCCOPY);
	return ;
}

//---------------------------------step 2: mouse ------------------------------------

void CKMPDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect	rect;

	BOOL    flag=FALSE;

	CClientDC dc(this);
	dc.SetBkMode(1);
	CFont* font=GetFont();
	CFont* lfont=dc.SelectObject(font);

	int  i=0;
	for(i=0;i<5;i++)    // 菜单
	{
		rect.bottom=kmenu[i].bottom-1;
		rect.top=kmenu[i].top+1;
		rect.left=kmenu[i].left+1;
		rect.right=kmenu[i].right-1;

		if(rect.PtInRect(point))
		{
			flag=TRUE;

			if(focus1>=0 && focus1<5)
			{
				drawmenu(&dc,focus1,0);
			}

			GetWindowPlacement(&wp);
			focus1=i;

			if(menuflag==0)
			{
				menuflag=1;
				drawmenu(&dc,focus1,-1);
	
				menu[i].setwp(&wp);
				setsubmenupos(&menu[i],i);
				menu[i].ModifyStyle(WS_POPUP,WS_CHILD,0);
				menu[i].ShowWindow(SW_SHOW);
			}
			else
			{
				drawmenu(&dc,focus1,0);	
			}
		}
	}

	if(flag==FALSE)  // 工具条
	{
		if(focus1<5 && focus1>=0 && menuflag==1)
		{
			drawmenu(&dc,focus1,0);
			menu[focus1].focus=-2;
			menu[focus1].ShowWindow(SW_HIDE);
			focus1=-100;
		}

		for(i=0;i<9;i++)
		{
			rect.bottom=kbar[i].bottom-1;
			rect.top=kbar[i].top+1;
			rect.left=kbar[i].left+1;
			rect.right=kbar[i].right-1;
		
			if(rect.PtInRect(point))
			{
				flag=TRUE;	
				if(focus2!=i && focus2>=0 && focus2 < 9)
				{
					drawbar(&dc,focus2,0);
				}
				focus2=i;
				drawbar(&dc,focus2,-1);
			}
		}
	}

	if(flag==FALSE)   // slider bar and other area
	{
		if(focus2<9 && focus2>=0)  // other area and slider bar common handle
		{
			drawbar(&dc,focus2,0);
			focus2=-100;
		}

		if(focus1<5 && focus1>=0)
		{
			menu[focus1].focus=-2;
			menu[focus1].ShowWindow(SW_HIDE);
			drawmenu(&dc,focus1,0);
			focus1=-100;
		}		
		menuflag=0;

		rect.bottom =slidery+sliderylen;  // only slider bar handle
		rect.top =slidery;
		rect.left =sliderx-1;
		rect.right =sliderx+3;

		if(rect.PtInRect(point))
		{
			sliderflag=TRUE;
			SetCapture();
		}
	}

	dc.SelectObject(lfont);
}

void CKMPDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CRect	rect;

	CClientDC dc(this);
	dc.SetBkMode(1);
	CFont* font=GetFont();
	CFont* lfont=dc.SelectObject(font);

	int i=0;
	BOOL flag;

	for(i=0;i<9;i++)
	{
		rect.bottom=kbar[i].bottom-1;
		rect.top=kbar[i].top+1;
		rect.left=kbar[i].left+1;
		rect.right=kbar[i].right-1;

		if(rect.PtInRect(point))
		{
			flag=TRUE;

			if(focus2!=i && focus2>=0 && focus2<9)
			{
				drawbar(&dc,focus2,0);
				focus2=i;
			}

			drawbar(&dc,focus2,1);

			if((i>=0) &&(i<9))
			{
				drawbar(&dc, i, -1);
				SendMessage(barID[i], 0, 0);
				drawbar(&dc, i, 0);
			}
			break;
		}
	}

	if(flag==FALSE)  // 其他地方
	{
		if(focus2<9 && focus2>=0)
		{
			drawbar(&dc,focus2,0);
			focus2=-100;
		}
	}

	if(sliderflag==TRUE)
	{
		sliderflag=FALSE;
		ReleaseCapture();
	}

	dc.SelectObject(lfont);
}

void CKMPDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect	rect;
	BOOL    flag=FALSE;

	CClientDC dc(this);
	dc.SetBkMode(1);
	CFont* font=this->GetFont();
	CFont* lfont=dc.SelectObject(font);

	int i=0;

	for(i=0;i<5;i++)   ////  菜单
	{
		rect.bottom=kmenu[i].bottom-1;
		rect.top=kmenu[i].top+1;
		rect.left=kmenu[i].left+1;
		rect.right=kmenu[i].right-1;

		if(rect.PtInRect(point))
		{
			flag=TRUE;
			if(focus2>=0)
			{
				drawbar(&dc,focus2,0);
				focus2=-100;
			}

			if(focus1!=i)
			{
				if(focus1>=0 && focus1<5)
				{
					drawmenu(&dc,focus1,0);
					menu[focus1].focus=-2;
					menu[focus1].ShowWindow(SW_HIDE);
				}

				focus1=i;
				GetWindowPlacement(&wp);

				if(menuflag==1)
				{
					menu[i].setwp(&wp);
					setsubmenupos(&menu[i],i);
					menu[i].ModifyStyle(WS_POPUP,WS_CHILD,0);
					menu[i].ShowWindow(SW_SHOW);
					drawmenu(&dc,focus1,-1);	
				}
				else
				{
					drawmenu(&dc,focus1,1);
				}
			}
			break;
		}
	}

	if(menuflag==0) 
	{
		for(i=0;i<9;i++)      //// 工具条
		{
			rect.bottom=kbar[i].bottom-1;
			rect.top=kbar[i].top+1;
			rect.left=kbar[i].left+1;
			rect.right=kbar[i].right-1;

			if(rect.PtInRect(point))
			{
				flag=TRUE;

				if(focus1>=0 && focus1<5 )
				{
					menu[focus1].focus=-2;
					menu[focus1].ShowWindow(SW_HIDE);
					drawmenu(&dc,focus1,0);
					focus1=-100;
				}

				if(focus2!=i )
				{
					if( focus2>=0 && focus2<9)
					{
						drawbar(&dc,focus2,0);	
					}

					if(barflag[i])
					{	
						focus2=i;
						drawbar(&dc,focus2,1);

						drawnum(&dc,3);
						dc.TextOut(knum[3].left+xoff,knum[3].top+yoff,khelp[i],strlen(khelp[i]));
					}
				}
				break;
			}
		}
		if(flag==FALSE && sliderflag==TRUE)   // slider bar 
		{
			if(point.x>=sliderrc.right )
				sliderx=sliderrc.right-1 ;
			else if(point.x <=sliderrc.left)
				sliderx=sliderrc.left+1;
			else 
				sliderx=point.x;

			dc.SetBkMode(1);
			CFont* font = this->GetFont();
			CFont* oldfont= dc.SelectObject(font);

			dc.BitBlt(knum[0].left ,knum[0].top ,knum[0].right-knum[0].left ,knum[0].bottom -knum[0].top ,&numdc[0],
						0,0,
						SRCCOPY);

			char statictext[40];

			memset((void*)statictext,0,40);
			sprintf(statictext,"00:00");
			dc.TextOut(knum[0].left+xoff+2,knum[0].top+yoff,statictext,strlen(statictext));

			memset((void*)statictext,0,40);
			sprintf(statictext,"30%%");
			dc.TextOut(knum[0].left+xoff+80,knum[0].top+yoff,statictext,strlen(statictext));

			memset((void*)statictext,0,40);
			sprintf(statictext,"30:30");
			dc.TextOut(knum[0].left+xoff+156,knum[0].top+yoff,statictext,strlen(statictext));
			
			CPen  pen1(PS_SOLID,1,::GetSysColor(COLOR_BTNSHADOW));
			CPen  pen2(PS_SOLID,1,::GetSysColor(COLOR_WINDOWFRAME));
			CPen  pen3(PS_SOLID,1,::GetSysColor(COLOR_BTNHILIGHT));
			CPen  pen4(PS_SOLID,1,::GetSysColor(COLOR_BTNFACE));
			CPen  *lpen=dc.SelectObject(&pen4);
			dc.MoveTo(sliderrc.left,sliderrc.top);
			dc.LineTo(sliderrc.right,sliderrc.top);
			dc.MoveTo(sliderrc.left,sliderrc.top+1);
			dc.LineTo(sliderrc.right,sliderrc.top+1);
			dc.MoveTo(sliderrc.left,sliderrc.top-5);
			dc.LineTo(sliderrc.left,sliderrc.top+2);
			dc.MoveTo(sliderrc.left+1,sliderrc.top-5);
			dc.LineTo(sliderrc.left+1,sliderrc.top+2);
			dc.MoveTo(sliderrc.right-1,sliderrc.top-5);
			dc.LineTo(sliderrc.right-1,sliderrc.top+2);
			dc.MoveTo(sliderrc.right,sliderrc.top-5);
			dc.LineTo(sliderrc.right,sliderrc.top+2);
			dc.SelectObject(&pen1);
			dc.MoveTo(sliderx-1,slidery);
			dc.LineTo(sliderx-1,slidery+sliderylen);
			dc.SelectObject(&pen2);
			dc.MoveTo(sliderx,slidery);
			dc.LineTo(sliderx,slidery+sliderylen);
			dc.SelectObject(&pen3);
			dc.MoveTo(sliderx+1,slidery);
			dc.LineTo(sliderx+1,slidery+sliderylen);
			dc.SelectObject(lpen);					
		}
	}

	if(flag==FALSE)  // 其他地方
	{
		if((focus2<9 && focus2>=0)||(focus1<5 && focus1>=0))
		{
			drawnum(&dc,3);
			if(status==0)
				dc.TextOut(knum[3].left+xoff,knum[3].top+yoff,khelp[9],strlen(khelp[9]));
			else if(status==1)
				dc.TextOut(knum[3].left+xoff,knum[3].top+yoff,khelp[10],strlen(khelp[10]));
			else if(status==2)
				dc.TextOut(knum[3].left+xoff,knum[3].top+yoff,khelp[11],strlen(khelp[11]));
		}

		if(focus2<9 && focus2>=0)
		{
			drawbar(&dc,focus2,0);
			focus2=-100;
		}

		if(focus1<5 && focus1>=0 && menuflag==0)
		{
			drawmenu(&dc,focus1,0);
			focus1=-100;
		}
	}
	dc.SelectObject(lfont);
}

void CKMPDlg::setsubmenupos(CWnd* pwnd,int i)
{
	int icap=GetSystemMetrics(SM_CYCAPTION);  // 20050320 修正在XP中菜单定位问题

	CKMenu* pmenu = (CKMenu*)pwnd;
	pmenu->index =i;
	CSize rs;
	rs.cx=pmenu->itemw ;
	rs.cy=pmenu->menurc[pmenu->items-1].bottom + 3;
	pmenu->MoveWindow(wp.rcNormalPosition.left+kmenu[i].left+3,wp.rcNormalPosition.top+26+icap,rs.cx,rs.cy );
}

//----------------------------- step3 : ui function -----------------------------------------

void CKMPDlg::OnKDlgChan()   // select channel
{
	CKDlgChan dia;
	dia.DoModal();
}

void CKMPDlg::OnKMIP()   // set channel
{
	CKDlgMIP dia;
	dia.ShowDialog();
}

void CKMPDlg::OnKIP()   // set ip addr
{
	CKDlgIP dia;
	dia.DoModal();
}

void CKMPDlg::OnKAbout() 
{
	CAboutDlg dia;
	dia.DoModal();
}
//-------------------------step 4 : other ----------------------------------------------------

LRESULT CKMPDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class

	if(message==0x313)
	{
		GetWindowPlacement(&wp);

		if(wp.showCmd ==SW_SHOWMINIMIZED)
			ShowWindow(SW_RESTORE);
		
		return 1;
	}

	else if(message == WM_USER200)
	{
		focus1=-100;
		menuflag=0;
		CClientDC dc(this);
		dc.SetBkMode(1);
		CFont* font=GetFont();
		CFont* lfont=dc.SelectObject(font);
		int i=(int)lParam;
		if(i>=0 && i<=4)
			drawmenu(&dc,i,0);
		dc.SelectObject(lfont);
	}

	else if(message ==ID_KDLGCHAN)	OnKDlgChan();
	else if(message ==ID_KDLGFILE)	OnKDlgFile();
	else if(message ==ID_KIP)		OnKIP();
	else if(message ==ID_KEXIT)		OnKExit();
	else if(message ==ID_KFORWARD)	OnKForward();
	else if(message ==ID_KBACKWARD) OnKBackward();
	else if(message ==ID_KCOLOR)	OnKColor();
	else if(message ==ID_KHALFSCREEN)	OnKHalfscreen();
	else if(message ==ID_KDOUBLESCREEN) OnKDoublescreen();
	else if(message ==ID_KFULLSCREEN)	OnKFullscreen();
	else if(message ==ID_KADDVOC)		OnKAddvoc();
	else if(message ==ID_KDELVOC)		OnKDelvoc();
	else if(message ==ID_KISOITU)		OnKISOITU();
	else if(message ==ID_KABOUT)		OnKAbout();
	else if(message ==ID_KLEFTSOUND)	OnKLeftsound();
	else if(message ==ID_KLOOP)			OnKLoop();
	else if(message ==ID_KMIP)			OnKMIP();
	else if(message ==ID_KPAUSE)		OnKPause();
	else if(message ==ID_KPLAY)			OnKPlay();
	else if(message ==ID_KRIGHTSOUND)	OnKRightsound();
	else if(message ==ID_KSCREEN)		OnKScreen();
	else if(message ==ID_KSKIN)			OnKSkin();
	else if(message ==ID_KSTERO)		OnKStero();
	else if(message ==ID_KSTOP)			OnKStop();

	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CKMPDlg::resetmenu()
{
	if(focus1>=0)
	{
		CClientDC dc(this);
		dc.SetBkMode(1);
		CFont* font=GetFont();
		CFont* lfont=dc.SelectObject(font);

		drawmenu(&dc,focus1,0);
		menu[focus1].ShowWindow(SW_HIDE);
		focus1=-100;
		menuflag=0;

		dc.SelectObject(lfont);
	}
}

void CKMPDlg::OnKillFocus(CWnd* pNewWnd) 
{
	CDialog::OnKillFocus(pNewWnd);

	resetmenu();
}

void CKMPDlg::OnRButtonDown(UINT nFlags, CPoint point) 
{
	resetmenu();

	CDialog::OnRButtonDown(nFlags, point);
}

void CKMPDlg::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	resetmenu();

	CDialog::OnNcLButtonDown(nHitTest, point);
}

void CKMPDlg::OnNcRButtonDown(UINT nHitTest, CPoint point)
{
	resetmenu();

	CDialog::OnNcRButtonDown(nHitTest, point);
}
