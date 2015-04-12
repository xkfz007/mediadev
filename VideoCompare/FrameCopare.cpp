// FrameCopare.cpp: implementation of the CFrameCopare class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "VideoCompare.h"
#include "FrameCopare.h"
#include<math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFrameCopare::CFrameCopare(const int frame_w, const int frame_h, CDialog* pdlg)
{
	m_yuvlen = frame_w * frame_h * 3/2;
	m_ylen = frame_w * frame_h;
	m_uvlen = m_ylen/4;

	m_framew = frame_w;
	m_frameh = frame_h;
	//多出四个字节放置信号兵:)
	m_yuvbuff1 = new unsigned char[m_yuvlen + 4];
	m_ybuff1 = m_yuvbuff1;
	m_ubuff1 = m_yuvbuff1 + m_ylen;
	m_vbuff1 = m_ubuff1 + m_uvlen;
	m_yuvbuff1[m_yuvlen] = 0xff;
	//多出四个字节放置信号兵:)
	m_yuvbuff2 = new unsigned char[m_yuvlen + 4];
	m_ybuff2 = m_yuvbuff2;
	m_ubuff2 = m_yuvbuff2 + m_ylen;
	m_vbuff2 = m_ubuff2 + m_uvlen;
	m_yuvbuff2[m_yuvlen] = 0x00;
	m_pCurDlg = pdlg;
	
	m_snr.snr_u = 0;
	m_snr.snr_ua = 0;

	m_snr.snr_v = 0;
	m_snr.snr_va = 0;

	m_snr.snr_y = 0;
	m_snr.snr_ya = 0;

	for (int i=0; i <  256; i++)
	{
		m_quad[i]=i*i; // fix from TML 1, truncation removed
	}
	m_frameNum = 0;
	return;
}

CFrameCopare::~CFrameCopare()
{
	if(NULL != m_yuvbuff1)
		delete[] m_yuvbuff1;
	if(NULL != m_yuvbuff1)
		delete[] m_yuvbuff2;
}

int CFrameCopare::FrameCompare(const CString path1, const CString path2, CProgressCtrl *pProgress, CEdit* pEoutput)
{
	CFile file1;
	CFile file11;
	CFile file2;
	CFile file22;
	CFile filelog;
	CFileException e;
	int ret = 0;
	long file_len1;
	long file_len2;
	long compare_size;
	int dif_fame_num = 0;
	if( !file1.Open( path1, CFile::modeRead, &e ) )
    {
		return -1;
    }
	CString file11name = path1+".dif";
	if( !file11.Open( file11name, CFile::modeCreate | CFile::modeWrite, &e ) )
    {
		file1.Close();
		return -1;
    }
	if( !file2.Open( path2, CFile::modeRead, &e ) )
    {
		file1.Close();
		file2.Close();
		return -1;
    }
	CString file22name = path2+".dif";
	if( !file22.Open( file22name, CFile::modeCreate | CFile::modeWrite, &e ) )
    {
		file1.Close();
		file11.Close();
		file2.Close();	
		return -1;
    }
	CString logfilename = file1.GetFilePath() + "_vs_" + file2.GetFileTitle()+".log";
	if( !filelog.Open( logfilename, CFile::modeCreate | CFile::modeWrite, &e ) )
    {
		file1.Close();
		file2.Close();
		file11.Close();
		file22.Close();	
		return -1;
    }

	file_len1 = file1.GetLength();
	file_len2 = file1.GetLength();
	compare_size = file_len1 > file_len2 ?  file_len2:file_len1;

	pProgress->SetRange(0, (int)(compare_size/m_yuvlen));
	pProgress->SetStep(1);

	CString log_str;
	log_str.Format("The frame NO.   Y_SNR   U_SNR   V_SNR\n");

	filelog.Write(log_str, log_str.GetLength());
    unsigned int *yuvbuff1int;
	unsigned int *yuvbuff2int;
	unsigned int *EndSignalAdress = (unsigned int *)(m_yuvbuff1 + m_yuvlen);
	while(file1.Read(m_yuvbuff1, m_yuvlen) && file2.Read(m_yuvbuff2, m_yuvlen))
	{
		m_frameNum++;

		yuvbuff1int = (unsigned int *)m_yuvbuff1;
		yuvbuff2int = (unsigned int *)m_yuvbuff2;
		while(*yuvbuff1int++ ==  *yuvbuff2int++);

		if(yuvbuff1int < EndSignalAdress)
		{
			dif_fame_num++;
			SNRCaculate();
			log_str.Empty();
			log_str.Format(" NO.%d    %f    %f    %f\n", m_frameNum, m_snr.snr_y, m_snr.snr_u, m_snr.snr_v);
			filelog.Write(log_str, log_str.GetLength());
			file11.Write(m_yuvbuff1, m_yuvlen);
			file22.Write(m_yuvbuff2, m_yuvlen);
		}
		pProgress->StepIt();
	}
	CString str_output;

	file1.Close();
	file2.Close();
	file11.Close();
	file22.Close();
	filelog.Close();

	if(dif_fame_num > 0)
	{
		
		str_output.Format("%d frames with different bytes have been writen in :\r\n%s \r\n%s \r\nSNR are recoded in:\r\n%s "
			,dif_fame_num,path1+".dif", path2+".dif", logfilename);
	}
	else
	{
		str_output.Format("There are no different frames!");
		CFile::Remove(file11name); 
		CFile::Remove(file22name); 
		CFile::Remove(logfilename); 
	}
	pEoutput->SetWindowText(str_output);
	m_pCurDlg->UpdateData();

	return ret;
}
void CFrameCopare::SNRCaculate()
{
	int diff_y,diff_u,diff_v;
	diff_y = 0;
	for(int i=0; i<m_ylen; i++)
	{
		diff_y += m_quad[abs(m_ybuff1[i]-m_ybuff2[i])];
	}
	diff_u = 0;
	diff_v = 0;
	for(int i=0; i<m_uvlen; i++)
	{
		diff_u += m_quad[abs(m_ubuff1[i]-m_ubuff2[i])];
		diff_v += m_quad[abs(m_vbuff1[i]-m_vbuff2[i])];
	}
	if (diff_y != 0)
		m_snr.snr_y=(float)(10*log10(65025*(float)m_ylen/(float)diff_y));        // luma snr for current frame
	else
		m_snr.snr_y=0;
	if (diff_u != 0)
		m_snr.snr_u=(float)(10*log10(65025*(float)m_uvlen/(float)(diff_u)));    //  chroma snr for current frame
	else
		m_snr.snr_u=0;
	if (diff_v != 0)
		m_snr.snr_v=(float)(10*log10(65025*(float)m_uvlen/(float)(diff_v)));    //  chroma snr for current frame
	else
		m_snr.snr_v=0;


}
