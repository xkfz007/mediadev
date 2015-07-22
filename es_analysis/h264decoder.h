#ifndef _H264DECODER_H_
#define _H264DECODER_H_
#include "es_defines.h"
//#include "h264typedefs.h"
#define MAXSPS  32
#define MAXPPS  256
class ES_API  H264Decoder
{
public:
	H264Decoder();
	~H264Decoder();

	int NextPesPack(PesPack*pes);
	void ShowSeqMeg();

private:

	int ProcessNALU();
	void ProcessSPS();
	void ProcessPPS();
	void ProcessSH();

	int GetNaluFromPES();
	void SetPES(PesPack*pes);

	string GetSPSMsg(int& infoType);
	string GetPPSMsg(int& infoType);
	string GetSHMsg(int& infoType);
	string GetSeqMsg(int& infoType);

private:
	seq_parameter_set_rbsp_t SeqParSet[MAXSPS];
	pic_parameter_set_rbsp_t PicParSet[MAXPPS];

	NALU_t *nalu_t;
	PesPack*hPack;
	int pesPos;

	int NALUCount;
	int totalUsedBits;
	pic_parameter_set_rbsp_t *active_pps;
	seq_parameter_set_rbsp_t *active_sps;
	pic_parameter_set_rbsp_t *pNextPPS;

private:
	int profileIdc;//Profile   
	int levelIdc;//Level  

	int sliceType;
	//string sliceType2;
	int idrFlag;
	int idrPicId;

	int entropyCodingMode;
	//string entropyCodingMode2;
	int structure;
	//string structure2;

	int PicWidthInMbs;
	int PicHeightInMapUnits;
	int FrameHeightInMbs;
	int PicHeightInMbs;
	int FrameSizeInMbs;
	int PicSizeInMbs;

	int chromaFormatIdc;

	int SPSID;
	int PPSID;
	
	unsigned int iInterval;
	unsigned int rapInterval;
	unsigned int maxI;//I或IDR最大间隔
	unsigned int maxRap;//RAP最大间隔

	unsigned int seqLen;

	int pesOnePic;

public:
	H264Info h264Info;
	Es_ErrorType errType;
public:
	ESCALLBACK escb;
	ESERRCALLBACK eserrcb;
	HANDLE ptsHandle;
};


#endif