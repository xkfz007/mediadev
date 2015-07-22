/* 雷霄骅
 * 中国传媒大学/数字电视技术
 * leixiaohua1020@126.com
 *
 */
// SimplePlayerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SimplePlayer.h"
#include "SimplePlayerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSimplePlayerDlg dialog

CSimplePlayerDlg::CSimplePlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSimplePlayerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSimplePlayerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	mFilterGraph = NULL;
	mSourceFile  = "";
	mSliderTimer = 0;
}

CSimplePlayerDlg::~CSimplePlayerDlg()
{
	DestroyGraph();
}

void CSimplePlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSimplePlayerDlg)
	DDX_Control(pDX, IDC_SLIDER_GRAPH, mSliderGraph);
	DDX_Control(pDX, IDC_VIDEO_WINDOW, mVideoWindow);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSimplePlayerDlg, CDialog)
	//{{AFX_MSG_MAP(CSimplePlayerDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, OnButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, OnButtonPause)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_GRAB, OnButtonGrab)
	ON_BN_CLICKED(IDC_BUTTON_FULLSCREEN, OnButtonFullscreen)
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_TEST, OnButtonTest)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_GRAPHNOTIFY, OnGraphNotify)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimplePlayerDlg message handlers

BOOL CSimplePlayerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	// Change the window style for video window, or video window
	// will not be painted properly.
	mVideoWindow.ModifyStyle(0, WS_CLIPCHILDREN);

	mSliderGraph.SetRange(0, 1000);
	mSliderGraph.SetPos(0);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSimplePlayerDlg::OnPaint() 
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
HCURSOR CSimplePlayerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
//打开
void CSimplePlayerDlg::OnButtonOpen() 
{
	// TODO: Add your control notification handler code here
	CString    strFilter = "AVI File (*.avi)|*.avi|";
	strFilter += "MPEG File (*.mpg;*.mpeg)|*.mpg;*.mpeg|";
	strFilter += "Mp3 File (*.mp3)|*.mp3|";
	strFilter += "Wave File (*.wav)|*.wav|";
	strFilter += "All Files (*.*)|*.*|";
	CFileDialog dlgOpen(TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, 
		strFilter, this);
	if (IDOK == dlgOpen.DoModal()) 
	{
		mSourceFile = dlgOpen.GetPathName();
		// Rebuild the file playback filter graph
		//创建Graph
		CreateGraph();
	}
}
//播放
void CSimplePlayerDlg::OnButtonPlay() 
{
	if (mFilterGraph)
	{
		mFilterGraph->Run();
		// Start a timer
		if (mSliderTimer == 0)
		{
			mSliderTimer = SetTimer(SLIDER_TIMER, 100, NULL);
		}
	}
}

void CSimplePlayerDlg::OnButtonPause() 
{
	if (mFilterGraph)
	{
		mFilterGraph->Pause();
		// Start a timer
		if (mSliderTimer == 0)
		{
			mSliderTimer = SetTimer(SLIDER_TIMER, 100, NULL);
		}
	}
}

void CSimplePlayerDlg::OnButtonStop() 
{
	if (mFilterGraph)
	{
		mFilterGraph->SetCurrentPosition(0);
		mFilterGraph->Stop();
		// Stop the timer
		if (mSliderTimer)
		{
			KillTimer(mSliderTimer);
			mSliderTimer = 0;
		}
	}
}

void CSimplePlayerDlg::OnButtonGrab() 
{
	if (mFilterGraph)
	{
		// Firstly grab a bitmap to a temp file
		char  szTemp[] = "C:\\mysnapshot.bmp";
		if (mFilterGraph->SnapshotBitmap(szTemp))
		{
			// User can browser for a new file here
			CString   strFilter = "BMP File (*.bmp)|*.bmp|";
			CFileDialog dlgOpen(FALSE, ".bmp", NULL, OFN_HIDEREADONLY, strFilter, NULL);
			if (IDOK == dlgOpen.DoModal()) 
			{
				::CopyFile(szTemp, dlgOpen.GetPathName(), FALSE);
				::DeleteFile(szTemp);
			}
		}
		
	}
}

void CSimplePlayerDlg::OnButtonFullscreen() 
{
	if (mFilterGraph)
	{
		mFilterGraph->SetFullScreen(TRUE);
	}
}
//创建Graph
void CSimplePlayerDlg::CreateGraph(void)
{
	//（如果有）销毁Graph
	DestroyGraph();
	//新建一个核心类
	mFilterGraph = new CDXGraph();
	if (mFilterGraph->Create())
	{
		// Render the source clip
		mFilterGraph->RenderFile(mSourceFile);
		// Set video window and notification window
		mFilterGraph->SetDisplayWindow(mVideoWindow.GetSafeHwnd());
		mFilterGraph->SetNotifyWindow(this->GetSafeHwnd());
		// Show the first frame
		mFilterGraph->Pause();
	}
}
//（如果有）销毁Graph
void CSimplePlayerDlg::DestroyGraph(void)
{
	if (mFilterGraph)
	{
		// Stop the filter graph first
		mFilterGraph->Stop();
		mFilterGraph->SetNotifyWindow(NULL);

		delete mFilterGraph;
		mFilterGraph = NULL;
	}
}

BOOL CSimplePlayerDlg::OnEraseBkgnd(CDC* pDC)
{
	// Intercept background erasing for the movie window, since the
    // video renderer will keep the screen painted.  Without this code,
    // your video window might get painted over with gray (the default
    // background brush) when it is obscured by another window and redrawn.
    CRect rc;
    // Get the bounding rectangle for the movie screen
    mVideoWindow.GetWindowRect(&rc);
    ScreenToClient(&rc);
    // Exclude the clipping region occupied by our movie screen
    pDC->ExcludeClipRect(&rc);

	// Erase the remainder of the dialog as usual
    return CDialog::OnEraseBkgnd(pDC);
}

// We use "Return" key and "Esc" key to restore from fullscreen mode
BOOL CSimplePlayerDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			// Restore form fullscreen mode
			RestoreFromFullScreen();
			return 1;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CSimplePlayerDlg::RestoreFromFullScreen(void)
{
	if (mFilterGraph)
	{
		if (mFilterGraph->GetFullScreen())
		{
			mFilterGraph->SetFullScreen(FALSE);
		}
	}
}

void CSimplePlayerDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == mSliderTimer && mFilterGraph)
	{
		double pos = 0, duration = 1.;
		mFilterGraph->GetCurrentPosition(&pos);
		mFilterGraph->GetDuration(&duration);
		// Get the new position, and update the slider
		int newPos = int(pos * 1000 / duration);
		if (mSliderGraph.GetPos() != newPos)
		{
			mSliderGraph.SetPos(newPos);
		}
	}

	CDialog::OnTimer(nIDEvent);
}

BOOL CSimplePlayerDlg::DestroyWindow() 
{
	if (mSliderTimer)
	{
		KillTimer(mSliderTimer);
		mSliderTimer = 0;
	}
	
	return CDialog::DestroyWindow();
}

void CSimplePlayerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (pScrollBar->GetSafeHwnd() == mSliderGraph.GetSafeHwnd())
	{
		if (mFilterGraph)
		{
			// Calculate the current seeking position
			double duration = 1.;
			mFilterGraph->GetDuration(&duration);
			double pos = duration * mSliderGraph.GetPos() / 1000.;
			mFilterGraph->SetCurrentPosition(pos);
		}		
	}
	else
	{
		CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	}
}

// Deal with the graph events 
LRESULT CSimplePlayerDlg::OnGraphNotify(WPARAM inWParam, LPARAM inLParam)
{
	IMediaEventEx * pEvent = NULL;
	if (mFilterGraph && (pEvent = mFilterGraph->GetEventHandle()))
	{
		LONG   eventCode = 0, eventParam1 = 0, eventParam2 = 0;
		while (SUCCEEDED(pEvent->GetEvent(&eventCode, &eventParam1, &eventParam2, 0)))
		{ 
			// Spin through the events
			pEvent->FreeEventParams(eventCode, eventParam1, eventParam2);
			switch (eventCode)
			{
			case EC_COMPLETE:
				OnButtonPause();
				mFilterGraph->SetCurrentPosition(0);
				break;
			
			case EC_USERABORT:
			case EC_ERRORABORT:
				OnButtonStop();
				break;

			default:
				break;
			}
		}
	}
	return 0;
}






//////////////////////////////////////////////////////////////////////////////////
// Locate a filter within the graph by searching (from renderers upstream)
// looking for a specific interface on the filter
HRESULT CSimplePlayerDlg::FindFilterByInterface(REFIID riid, IBaseFilter** ppFilter)
{
    *ppFilter = NULL;
	if (!mFilterGraph)
	{
		return E_FAIL;
	}

    IEnumFilters* pEnum;
    HRESULT hr = mFilterGraph->GetGraph()->EnumFilters(&pEnum);
    if (FAILED(hr)) 
	{
		return hr;
    }

    IBaseFilter* pFilter = NULL;
    while (pEnum->Next(1, &pFilter, NULL) == S_OK) 
	{
		// Check for required interface
		IUnknown* pUnk;
		HRESULT hrQuery = pFilter->QueryInterface(riid, (void**)&pUnk);
		if (SUCCEEDED(hrQuery)) 
		{
			pUnk->Release();
			pEnum->Release();
			*ppFilter = pFilter;
			return S_OK;
		}
		pFilter->Release();
    }
    pEnum->Release();

    return E_FAIL;
}

void CSimplePlayerDlg::ShowVRPropertyPage(void)
{
	IBaseFilter *pFilter = NULL;
	if (FAILED(FindFilterByInterface(IID_IVideoWindow, &pFilter)))
	{
		return;
	}
	pFilter->Release();

	ISpecifyPropertyPages *pProp = NULL;
	HRESULT hr = pFilter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&pProp);
	if (SUCCEEDED(hr)) 
	{
		// Get the filter's name and IUnknown pointer.
		FILTER_INFO FilterInfo;
		hr = pFilter->QueryFilterInfo(&FilterInfo); 
		IUnknown *pFilterUnk;
		pFilter->QueryInterface(IID_IUnknown, (void **)&pFilterUnk);

		// Show the page. 
		CAUUID caGUID;
		pProp->GetPages(&caGUID);
		pProp->Release();
		OleCreatePropertyFrame(
			this->GetSafeHwnd(),                   // Parent window
			0, 0,                   // Reserved
			FilterInfo.achName,     // Caption for the dialog box
			1,                      // Number of objects (just the filter)
			&pFilterUnk,            // Array of object pointers. 
			caGUID.cElems,          // Number of property pages
			caGUID.pElems,          // Array of property page CLSIDs
			0,                      // Locale identifier
			0, NULL                 // Reserved
		);

		// Clean up.
		pFilterUnk->Release();
		FilterInfo.pGraph->Release(); 
		CoTaskMemFree(caGUID.pElems);
	}
}

void CSimplePlayerDlg::OnButtonTest() 
{
	ShowVRPropertyPage();
}
