// FrameCopare.h: interface for the CFrameCopare class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRAMECOPARE_H__164BA9B6_76DC_47A3_BDF6_1536512EBFAB__INCLUDED_)
#define AFX_FRAMECOPARE_H__164BA9B6_76DC_47A3_BDF6_1536512EBFAB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
struct snr_par
{
  float snr_y;                                 //!< current Y SNR
  float snr_u;                                 //!< current U SNR
  float snr_v;                                 //!< current V SNR

  float snr_ya;                                //!< Average SNR Y(dB) remaining frames
  float snr_ua;                                //!< Average SNR U(dB) remaining frames
  float snr_va;                                //!< Average SNR V(dB) remaining frames
};
class CFrameCopare  
{
public:
	CFrameCopare(const int frame_w, const int frame_h,  CDialog* pdlg);
	virtual ~CFrameCopare();
	int FrameCompare(const CString path1, const CString path2
		, CProgressCtrl *pProgress, CEdit* pEoutput);
private:
	CFrameCopare();
	void SNRCaculate();
private:

	unsigned char *m_yuvbuff1;
	unsigned char *m_ybuff1;
	unsigned char *m_ubuff1;
	unsigned char *m_vbuff1;

	unsigned char *m_yuvbuff2;
	unsigned char *m_ybuff2;
	unsigned char *m_ubuff2;
	unsigned char *m_vbuff2;
	int m_framew;
	int m_frameh;
	int m_yuvlen;
	int m_uvlen;
	int m_ylen;
	CDialog *m_pCurDlg;
	snr_par m_snr;
	int m_quad[256];
	int m_frameNum;
};

#endif // !defined(AFX_FRAMECOPARE_H__164BA9B6_76DC_47A3_BDF6_1536512EBFAB__INCLUDED_)
