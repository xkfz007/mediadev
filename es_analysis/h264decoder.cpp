#include "h264Decoder.h"
#include "errorHandle.h"
#include "vlc.h"
#include "h264analysis.h"

// #include <cstdio>
// #include <cstdlib>
#include <cassert>
// #include <cstring>
#include <sstream>
#include <string>
#include <iostream>

using std::stringstream;
using std::endl;
using std::cout;



H264Decoder::H264Decoder()
{
	NALUCount=0;
	totalUsedBits=0;

	active_pps=NULL;
	active_sps=NULL;
	pNextPPS=NULL;

	nalu_t=AllocNALU(MAX_CODED_FRAME_SIZE);
	hPack=NULL;
	pesPos=0;

	PicHeightInMbs=0;
	PicHeightInMapUnits=0;
	FrameHeightInMbs=0;
	PicHeightInMbs=0;
	FrameSizeInMbs=0;
	PicSizeInMbs=0;

	iInterval=0;
	rapInterval=0;
	maxI=0;
	maxRap=0;

	seqLen=0;

	pesOnePic=0;

	escb=NULL;
	eserrcb=NULL;
	ptsHandle=NULL;

	h264Info.maxIInterval=0;
	h264Info.maxRapInterval=0;
	h264Info.profile=0;
	h264Info.level=0;
	h264Info.horizontal=0;
	h264Info.vertical=0;
	h264Info.resolution="";
	h264Info.chromaFormat="";
	h264Info.entropyType="";
	h264Info.picType="";
	h264Info.picStruct="";
	h264Info.streamStruct="";
}
H264Decoder::~H264Decoder()
{	
	FreeNALU(nalu_t);
}

int H264Decoder::NextPesPack(PesPack*pes)
{
	//int hdLen=sizeof(pes_header);
	int tolLen=pes->i_len;
	
	SetPES(pes);
	while(pesPos<tolLen-1)
	{
		if((GetNaluFromPES())<0)
		{
// 			infoType=0;
// 			string tmp;
// 			tmp=GetSeqMsg(infoType);
// #ifdef _DEBUG
// 			cout<<tmp<<endl;
// #endif
// 			escb(ptsHandle,this,infoType);
			//没有找到NALU, 丢帧
			eserrcb(ptsHandle,this,ES_DROP_FRAME);
			return -1;		
		}
		//hDec->SetNalu(nalu);
		if(ProcessNALU()>1)
			break;
	}
	//escb(GetSeqMsg());
	return 1;

}

void H264Decoder::ShowSeqMeg()
{
	int infoType=0;
	string tmp;
	tmp=GetSeqMsg(infoType);
#ifdef _DEBUG
	cout<<tmp<<endl;
#endif
	escb(ptsHandle,this,infoType);

}
void H264Decoder::SetPES(PesPack*pes)
{
	hPack=pes;
	pesPos=0;
	pesOnePic=0;
}

int H264Decoder::ProcessNALU()
{

	if(0==GetRBSP(nalu_t))
	{
		//printf("ERRor!!!\n");
		//eserrcb(NALU_LEN0);
		errType=STREAM_ERROR;
		eserrcb(ptsHandle,this,STREAM_ERROR);
		exit(-1);
	}

	int infoType;
	string tmp;
	switch (nalu_t->nal_unit_type)
	{
	case NALU_TYPE_SLICE:
	case NALU_TYPE_IDR:
		rapInterval++;

		seqLen++;
		idrFlag=nalu_t->nal_unit_type==NALU_TYPE_IDR?1:0;

		if(pesOnePic)
		{
#ifdef _DEBUG
			printf("一个PES只能包含一幅图像\n");
#endif
			eserrcb(ptsHandle,this,NAL_EVERY_PIC_ERROR);
			//exit(-1);	
			return 2;
		}
		pesOnePic=1;
		//printf("We can processSliceHeader\n");
		ProcessSH();
		infoType=0;
		tmp=GetSHMsg(infoType);
#ifdef _DEBUG
		std::cout<<tmp<<endl;
#endif
		escb(ptsHandle,this,infoType);
		break;
	case NALU_TYPE_PPS:
		//printf ("Found NALU_TYPE_PPS\n");
		if(maxRap<rapInterval)
			maxRap=rapInterval;
		rapInterval=0;
		ProcessPPS();
		infoType=0;
		tmp=GetPPSMsg(infoType);
#ifdef _DEBUG
		std::cout<<tmp<<endl;
#endif
		escb(ptsHandle,this,infoType);
		break;
	case NALU_TYPE_SPS:
		//printf ("Found NALU_TYPE_SPS\n");
		if(maxRap<rapInterval)
			maxRap=rapInterval;
		rapInterval=0;
		ProcessSPS();
		infoType=0;
		tmp=GetSPSMsg(infoType);
#ifdef _DEBUG
		std::cout<<tmp<<endl;
#endif
		escb(ptsHandle,this,infoType);
		break;
	case NALU_TYPE_EOSEQ:
		infoType=0;
		tmp=GetSeqMsg(infoType);
#ifdef _DEBUG
		std::cout<<tmp<<endl;
#endif
		escb(ptsHandle,this,infoType);
		break;
	case NALU_TYPE_AUD:
		if(maxRap<rapInterval)
			maxRap=rapInterval;
		rapInterval=0;

		break;
	case NALU_TYPE_SPS_EXTENSION:
		eserrcb(ptsHandle,this,SPSEXTENSION_ERROR);
	default:
		if(maxRap<rapInterval)
			maxRap=rapInterval;
		rapInterval=0;
		break;
	}
	NALUCount++;
	return 1;

}
string H264Decoder::GetSPSMsg(int&infoType)
{
	stringstream spsMsg;
	spsMsg<<"SPS: profile_idc= "<<profileIdc<<" ";
	h264Info.profile=profileIdc;
	infoType|=ES_H264_profile;

	switch(profileIdc)
	{
	case FREXT_CAVLC444:     //!< YUV 4:4:4/14 "CAVLC 4:4:4"
		spsMsg<<" FREXT_CAVLC444"<<endl;
		break;
	case BASELINE:       //!< YUV 4:2:0/8  "Baseline"
		spsMsg<<" BASELINE"<<endl;
		break;
	case MAIN://           = 77,       //!< YUV 4:2:0/8  "Main"
		spsMsg<<" MAIN"<<endl;
		break;
	case EXTENDED://       = 88,       //!< YUV 4:2:0/8  "Extended"
		spsMsg<<" EXTENDED"<<endl;
		break;
	case FREXT_HP://       = 100,      //!< YUV 4:2:0/8  "High"
		spsMsg<<" FREXT_HP"<<endl;
		break;
	case FREXT_Hi10P://    = 110,      //!< YUV 4:2:0/10 "High 10"
		spsMsg<<" FREXT_Hi10P"<<endl;
		break;
	case FREXT_Hi422://    = 122,      //!< YUV 4:2:2/10 "High 4:2:2"
		spsMsg<<" FREXT_Hi422"<<endl;
		break;
	case FREXT_Hi444://    = 244,      //!< YUV 4:4:4/14 "High 4:4:4"
		spsMsg<<" FREXT_Hi444"<<endl;
		break;
	case MVC_HIGH://       = 118,      //!< YUV 4:2:0/8  "Multiview High"
		spsMsg<<" MVC_HIGH"<<endl;
		break;
	case STEREO_HIGH://    = 128       //!< YUV 4:2:0/8  "Stereo High"
		spsMsg<<" STEREO_HIGH"<<endl;
		break; 	
	default:
		error("ERRor profile\n",-1);
		break;
	}

	spsMsg<<"SPS: level_idc= "<<levelIdc<<endl;
	h264Info.level=levelIdc;
	infoType|=ES_H264_level;

	spsMsg<<"SPS: chroma_format_idc= "<<chromaFormatIdc;
	switch(chromaFormatIdc)
	{
	case YUV400://     =  0,     //!< Monochrome
		spsMsg<<" YUV=4:0:0"<<endl;
		h264Info.chromaFormat="YUV400";
		break;
	case YUV420://     =  1,     //!< 4:2:0
		spsMsg<<" YUV=4:2:0"<<endl;
		h264Info.chromaFormat="YUV420";
		break;
	case YUV422://     =  2,     //!< 4:2:2
		spsMsg<<" YUV=4:2:2"<<endl;
		h264Info.chromaFormat="YUV422";
		break;
	case YUV444://     =  3      //!< 4:4:4
		spsMsg<<" YUV=4:4:4"<<endl;
		h264Info.chromaFormat="YUV444";
		break;
	}
	infoType|=ES_H264_chromaFormat;
#if 1
	cout<<"SPS: profile_idc= "<<h264Info.profile<<endl;
	cout<<"SPS: level_idc= "<<h264Info.level<<endl;
	cout<<"YUV="<<h264Info.chromaFormat<<endl;
#endif
	
	return spsMsg.str();
}
void H264Decoder::ProcessSPS ()
{  
	seq_parameter_set_rbsp_t *sps = AllocSPS();


	Bitstream *bitstream=AllocBitStream();

	memcpy (bitstream->streamBuffer, &nalu_t->buf[1], nalu_t->len-1);
	bitstream->bitstream_length = RBSP2SODB (bitstream->streamBuffer, nalu_t->len-1);
	bitstream->frame_bitoffset = 0;

	int UsedBits=0;
	
	InterpretSPS (bitstream, sps,UsedBits);
	
	//get the info
	{
		profileIdc=sps->profile_idc;
		levelIdc=sps->level_idc;
		chromaFormatIdc=sps->chroma_format_idc;
		if(sps->profile_idc==BASELINE&&sps->profile_idc==EXTENDED)
			eserrcb(ptsHandle,this,ASO_ERROR);
	}



	if (sps->Valid)
	{
		if (active_sps)
		{
			if (sps->seq_parameter_set_id == active_sps->seq_parameter_set_id)
			{
				if (!sps_is_equal(sps,active_sps))
				{
					active_sps=NULL;
				}
			}
		}
		// SPSConsistencyCheck (pps);
		//MakeSPSavailable (sps->seq_parameter_set_id, sps);
		//assert (sps->Valid == 1);//TRUE);
		int id=sps->seq_parameter_set_id;
		memcpy (&SeqParSet[id], sps, sizeof (seq_parameter_set_rbsp_t));
	}
	FreeBitStream(bitstream);

	FreeSPS (sps);
}

void H264Decoder::ProcessPPS ()
{
	pic_parameter_set_rbsp_t *pps = AllocPPS();

	Bitstream *bitstream=AllocBitStream();

	memcpy (bitstream->streamBuffer, &nalu_t->buf[1], nalu_t->len-1);
	bitstream->bitstream_length = RBSP2SODB (bitstream->streamBuffer, nalu_t->len-1);
	bitstream->frame_bitoffset = 0;

	int UsedBits=0;

	InterpretPPS (bitstream, pps,UsedBits,SeqParSet);
	//get Info
	{
		SPSID=pps->seq_parameter_set_id;
		entropyCodingMode=pps->entropy_coding_mode_flag;
	}
	// PPSConsistencyCheck (pps);
	if (active_pps)
	{
		if (pps->pic_parameter_set_id == active_pps->pic_parameter_set_id)
		{
			if(!pps_is_equal(pps, active_pps))
			{
				active_pps = NULL;

			}
		}
	}
	//MakePPSavailable (pps->pic_parameter_set_id, pps);
	int id=pps->pic_parameter_set_id;
	{
		assert (pps->Valid == 1);//TRUE);

		if (PicParSet[id].Valid == 1/*TRUE */&& PicParSet[id].slice_group_id != NULL)
			free (PicParSet[id].slice_group_id);

		memcpy (&PicParSet[id], pps, sizeof (pic_parameter_set_rbsp_t));

		// we can simply use the memory provided with the pps. the PPS is destroyed after this function
		// call and will not try to free if pps->slice_group_id == NULL
		PicParSet[id].slice_group_id = pps->slice_group_id;
		pps->slice_group_id          = NULL;
	}
	FreeBitStream(bitstream);
	FreePPS (pps);
}

string  H264Decoder::GetPPSMsg(int&infoType)
{
	stringstream ppsMsg;
	ppsMsg<<"Refer SPS="<<SPSID<<endl;
	ppsMsg<<"PPS: entropy_coding_mode_flag="<<entropyCodingMode;

	switch(entropyCodingMode)
	{
	case CAVLC:
		ppsMsg<<" CAVLC"<<endl;
		h264Info.entropyType="CAVLC";
		break;
	case CABAC:
		ppsMsg<<" CABAC"<<endl;
		h264Info.entropyType="CABAC";
		break;
	}
	infoType|=ES_H264_entropyType;
	return ppsMsg.str();
}

void H264Decoder::ProcessSH()
{
	SliceHeader *sliceH=AllocSH();
	sliceH->nal_reference_idc=nalu_t->nal_reference_idc;
	sliceH->idrFlag=idrFlag;
	Bitstream *bitstream=AllocBitStream();

	memcpy (bitstream->streamBuffer, &nalu_t->buf[1], nalu_t->len-1);
	bitstream->bitstream_length = RBSP2SODB(bitstream->streamBuffer, nalu_t->len-1);
	bitstream->frame_bitoffset = 0;
	int UsedBits=0;

	FirstPartOfSliceHeader(bitstream,sliceH,UsedBits,active_sps,active_pps,SeqParSet,PicParSet);

	//RestOfSliceHeader(bitstream,sliceH,UsedBits);

	//get info
	{
		sliceType=sliceH->sliceType;

		PPSID=sliceH->pic_parameter_set_id;

		structure=sliceH->structure;

		//idrFlag=sliceH->idrFlag;

		idrPicId=sliceH->idrPicId;

		PicWidthInMbs = (active_sps->pic_width_in_mbs_minus1 +1);
		PicHeightInMapUnits = (active_sps->pic_height_in_map_units_minus1 +1);
		FrameHeightInMbs = ( 2 - active_sps->frame_mbs_only_flag ) * PicHeightInMapUnits;

		FrameSizeInMbs = PicWidthInMbs * FrameHeightInMbs;

		PicHeightInMbs = FrameHeightInMbs / ( 1 + sliceH->field_pic_flag );
		PicSizeInMbs   = PicWidthInMbs * PicHeightInMbs;

	}

	FreeBitStream(bitstream);
	FreeSH(sliceH);

}


string  H264Decoder::GetSHMsg(int&infoType)
{
	stringstream shMsg;
	shMsg<<"Refer PPS="<<PPSID<<endl;
	shMsg<<"SH: slice_type= "<<sliceType;
	switch(sliceType)
	{
	case P_SLICE:// = 0,
		shMsg<<" P"<<endl;
		h264Info.picType="P";
		h264Info.streamStruct+="P";
		iInterval++;
		break;
	case B_SLICE:// = 1,
		shMsg<<" B"<<endl;
		h264Info.picType="B";
		h264Info.streamStruct+="B";
		iInterval++;
		break;
	case I_SLICE://= 2,

		if(h264Info.streamStruct.length()>1000)
			h264Info.streamStruct="";
		if(idrFlag)
		{
			shMsg<<" IDR idr_id="<<idrPicId<<endl;
			h264Info.streamStruct+="IDR";
			h264Info.picType="IDR";
		}
		else
		{
			shMsg<<" I"<<endl;
			h264Info.streamStruct+="I";
			h264Info.picType="I";
		}
		shMsg<<"The interval between this IDR and the One before is "<<iInterval<<endl;

		if(maxI<iInterval)
		{
			maxI=iInterval;
		}
		iInterval=0;

		break;
	case SP_SLICE:// = 3,
		shMsg<<" SP"<<endl;
		h264Info.picType="SP";
		h264Info.streamStruct+="SP";
		break;
	case SI_SLICE:// = 4,
		shMsg<<" SI"<<endl;
		h264Info.picType="SI";
		h264Info.streamStruct+="SI";
		break;
	default:
		error("ERRor slice_type!!!",-1);
		break;
	}
	if(!idrFlag&&sliceType==I_SLICE)
	{
		eserrcb(ptsHandle,this,I2IDR_ERROR);
	}
	infoType|=ES_H264_picType;

	shMsg<<"SH:STRUCTURE="<<structure;
	switch(structure)
	{
	case FRAME:
		shMsg<<" FRAME"<<endl;
		h264Info.picStruct="FRAME";
		break;
	case TOP_FIELD:
		shMsg<<" TOP_FIELD"<<endl;
		h264Info.picStruct="TOP_FIELD";
		break;
	case BOTTOM_FIELD:
		shMsg<<" BOTTOM_FIELD"<<endl;
		h264Info.picStruct="BOTTOM_FIELD";
		break;
	default:
		printf("undefined structure\n");
		break;
	}
	infoType|=ES_H264_picStruct;

	h264Info.horizontal=PicWidthInMbs*16;
	h264Info.vertical=PicHeightInMbs*16;
	infoType|=ES_H264_horizontal;
	infoType|=ES_H264_vertical;
	stringstream tmpSS;

	tmpSS<<PicWidthInMbs*16<<"x"<<PicHeightInMbs*16;

	h264Info.resolution=tmpSS.str();
	infoType|=ES_H264_resolution;

	shMsg<<"SH:WIDTH="<<h264Info.horizontal<<endl;
	shMsg<<"SH:HEIGHT="<<h264Info.vertical<<endl;
	shMsg<<"SH:RESOLUTION="<<h264Info.resolution<<endl;
#if 1
	cout<<"SH:WIDTH="<<h264Info.horizontal<<endl;
	cout<<"SH:HEIGHT="<<h264Info.vertical<<endl;
	cout<<"SH:RESOLUTION="<<h264Info.resolution<<endl;

#endif
	return shMsg.str();

}

int H264Decoder::GetNaluFromPES()
{
	int info2 = 0, info3 = 0, pos = 0;
	int StartCodeFound = 0;
	int LeadingZero8BitsCount = 0;
	//int hdLen=sizeof(pes_header);
	int tolLen=hPack->i_len;
	int kk=pesPos;//+hdLen;

	unsigned char*pBuf=hPack->p_data+pesPos;

	int startcodeprefix_len;
	int nextstartcodebytes;

	while(kk<tolLen-1)
	{
		pos++;
		if (*(pBuf++)!= 0)
			break;
		kk++;
	}
	if(*(pBuf - 1) != 1 || pos < 3||kk==tolLen-1)
	{
		//printf ("GetAnnexbNALU: no Start Code at the beginning of the NALU, return -1\n");
		return -1;
	}

	if (pos == 3)
	{
		startcodeprefix_len = 3;
	}
	else
	{
		startcodeprefix_len = 4;
	}
	LeadingZero8BitsCount = pos;

	while ((pos+pesPos)<tolLen&&!StartCodeFound)
	{

		pos++;
		pBuf++;    
		info3 = FindStartCode_h264(pBuf - 4, 3);
		if(info3 != 1)
		{
			info2 = FindStartCode_h264(pBuf - 3, 2);
			StartCodeFound = info2 & 0x01;
		}
		else
			StartCodeFound = 1;
		//pBuf++;
	}

	if((pos+pesPos)==tolLen)
	{
		//NALU_t*nalu=AllocNALU(MAX_CODED_FRAME_SIZE);

		nalu_t->len = pos-LeadingZero8BitsCount;
		memcpy (nalu_t->buf, hPack->p_data+pesPos+ LeadingZero8BitsCount, nalu_t->len);
		nalu_t->forbidden_bit     = (*(nalu_t->buf) >> 7) & 1;
		nalu_t->nal_reference_idc = (NalRefIdc) ((*(nalu_t->buf) >> 5) & 3);
		nalu_t->nal_unit_type     = (NaluType) ((*(nalu_t->buf)) & 0x1f);

		pesPos+=pos-1;

		return 1;

	}
	// Here, we have found another start code (and read length of startcode bytes more than we should
	// have.  Hence, go back in the file
	if(info3 == 1)  //if the detected start code is 00 00 01, trailing_zero_8bits is sure not to be present
	{
		pBuf -= 5;
		while(*(pBuf--) == 0)
			pos--;
		nextstartcodebytes = 4;
	}
	else if (info2 == 1)
		nextstartcodebytes = 3;
	else
	{
		printf(" Panic: Error in next start code search \n");
		return -1;
	}

	pos -= nextstartcodebytes;

	// Here the leading zeros(if any), Start code, the complete NALU, trailing zeros(if any)
	// and the next start code is in the Buf.
	// The size of Buf is pos - rewind, pos are the number of bytes excluding the next
	// start code, and (pos) - LeadingZero8BitsCount
	// is the size of the NALU.

	//NALU_t*nalu=AllocNALU(MAX_CODED_FRAME_SIZE);

	nalu_t->len = pos - LeadingZero8BitsCount;
	memcpy (nalu_t->buf, hPack->p_data+pesPos+ LeadingZero8BitsCount, nalu_t->len);
	nalu_t->forbidden_bit     = (*(nalu_t->buf) >> 7) & 1;
	nalu_t->nal_reference_idc = (NalRefIdc) ((*(nalu_t->buf) >> 5) & 3);
	nalu_t->nal_unit_type     = (NaluType) ((*(nalu_t->buf)) & 0x1f);

	pesPos+=pos;

	return 1;
}
string H264Decoder::GetSeqMsg(int&infoType)
{
	string seqMsg;

	stringstream ss;
	h264Info.maxIInterval=maxI;
	h264Info.maxRapInterval=maxRap;	
	infoType|=ES_H264_maxIInterval;
	infoType|=ES_H264_maxRapInterval;
	ss<<"The max interval between two IDRs is "<<maxI<<endl;

	ss<<"The max interval between two RAPs is "<<maxRap<<endl;
	ss<<"The sequence length is "<<seqLen<<endl;
	ss<<"The StreamType is:"<<h264Info.streamStruct<<endl;

	infoType|=ES_H264_streamStruct;
	seqMsg=ss.str();

	return seqMsg;
}



