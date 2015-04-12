// VideoCompareDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VideoCompare.h"
#include "VideoCompareDlg.h"
#include "Data.h"
#include "FrameCopare.h"

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
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVideoCompareDlg dialog

CVideoCompareDlg::CVideoCompareDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVideoCompareDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVideoCompareDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVideoCompareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVideoCompareDlg)
	DDX_Control(pDX, IDC_EDIT_OUTPUT, m_Eoutput);
	DDX_Control(pDX, IDC_PROGRESS, m_Progress);
	DDX_Control(pDX, IDC_EDIT_PATH2, m_EPath2);
	DDX_Control(pDX, IDC_EDIT_PATH1, m_EPath1);
	DDX_Control(pDX, IDC_COMBO_FRAMEWH, m_ComboWH);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CVideoCompareDlg, CDialog)
	//{{AFX_MSG_MAP(CVideoCompareDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_COMPARE, OnButtonCompare)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE1, OnButtonBrowse1)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE2, OnButtonBrowse2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVideoCompareDlg message handlers

BOOL CVideoCompareDlg::OnInitDialog()
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
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	CString tmps;
	for(int i=0; i<5; i++)
	{
		tmps.Format("%d*%d",Frame_W_H[i][0],Frame_W_H[i][1]) ;
		m_ComboWH.AddString(tmps);
		tmps.Empty();
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CVideoCompareDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
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

void CVideoCompareDlg::OnPaint() 
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
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVideoCompareDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CVideoCompareDlg::OnButtonCompare() 
{
	// TODO: Add your control notification handler code here
	//if()
	CString path1;
	m_EPath1.GetWindowText(path1);
	CString path2;
	m_EPath2.GetWindowText(path2);
	int index = m_ComboWH.GetCurSel();
	if(index < 0)
		return;
	if(path1.IsEmpty() || path2.IsEmpty())
		return;
	CFrameCopare cp(Frame_W_H[index][0],Frame_W_H[index][1], this);
	cp.FrameCompare(path1, path2, &m_Progress, &m_Eoutput);
	return;
}

void CVideoCompareDlg::OnButtonBrowse1() 
{
	// TODO: Add your control notification handler code here
	CFileDialog FileDlg(TRUE, "", NULL, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT, 
		"yuv(*.yuv)|*.yuv|所有文件(*.*)|*.*||",
		AfxGetMainWnd());
	
	if (FileDlg.DoModal()!= IDOK)
		return;
	m_path1 = FileDlg.GetPathName();
	m_EPath1.SetWindowText(m_path1);

}

void CVideoCompareDlg::OnButtonBrowse2() 
{
	// TODO: Add your control notification handler code here
	CFileDialog FileDlg(TRUE, "", NULL, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT, 
		"yuv(*.yuv)|*.yuv|所有文件(*.*)|*.*||",
		AfxGetMainWnd());
	
	if (FileDlg.DoModal()!= IDOK)
		return;
	m_path2 = FileDlg.GetPathName();
	m_EPath2.SetWindowText(m_path2);
}
