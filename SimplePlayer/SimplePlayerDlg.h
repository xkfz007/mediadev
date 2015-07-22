/* 雷霄骅
 * 中国传媒大学/数字电视技术
 * leixiaohua1020@126.com
 *
 */
// SimplePlayerDlg.h : header file
//

#if !defined(AFX_SIMPLEPLAYERDLG_H__3599FE35_3322_4CC7_B30B_6D6050C2EDFF__INCLUDED_)
#define AFX_SIMPLEPLAYERDLG_H__3599FE35_3322_4CC7_B30B_6D6050C2EDFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSimplePlayerDlg dialog

#include <streams.h>
#include "CDXGraph.h"

#define SLIDER_TIMER   100

class CSimplePlayerDlg : public CDialog
{
// Construction
public:
	CSimplePlayerDlg(CWnd* pParent = NULL);	// standard constructor
	~CSimplePlayerDlg();

// Dialog Data
	//{{AFX_DATA(CSimplePlayerDlg)
	enum { IDD = IDD_SIMPLEPLAYER_DIALOG };
	CSliderCtrl	mSliderGraph;
	CStatic	mVideoWindow;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimplePlayerDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	CDXGraph *   mFilterGraph;     // Filter Graph封装
	CString      mSourceFile;      // 源文件
	UINT         mSliderTimer;     // 定时器ID
	//创建Graph
	void CreateGraph(void);        // 创建Filter Graph
	void DestroyGraph(void);       // 析构Filter Graph
	void RestoreFromFullScreen(void);

	// Just for testing...
	HRESULT FindFilterByInterface(REFIID riid, IBaseFilter** ppFilter);
	void ShowVRPropertyPage(void);

	// Generated message map functions
	//{{AFX_MSG(CSimplePlayerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//打开
	afx_msg void OnButtonOpen();
	//播放
	afx_msg void OnButtonPlay();
	//暂停
	afx_msg void OnButtonPause();
	//停止
	afx_msg void OnButtonStop();
	afx_msg void OnButtonGrab();
	afx_msg void OnButtonFullscreen();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnButtonTest();
	//}}AFX_MSG
	afx_msg LRESULT OnGraphNotify(WPARAM inWParam, LPARAM inLParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMPLEPLAYERDLG_H__3599FE35_3322_4CC7_B30B_6D6050C2EDFF__INCLUDED_)
