#ifndef _MPEG2DECODER_H_
#define _MPEG2DECODER_H_
#include "es_defines.h"

class ES_API  Mpeg2Decoder
{
public:
	Mpeg2Decoder(void);
	~Mpeg2Decoder(void);
    int NextPesPack(PesPack*pes);
	int processMPack();
	void SetPesPack(PesPack*pes);
	int GetMpackFromPES();
	void ShowAllMsg();
	
private:
	void sequence_header(MPack*mpk);
	void extension_and_user_data(MPack*mpk);
	void sequence_extension(MPack*mpk);
	void sequence_display_extension(MPack*mpk);
	void group_of_pictures_header(MPack*mpk);
	void picture_header(MPack*mpk);
	void picture_coding_extension(MPack*mpk);

	string getSeqMsg(int& infoType);
	string getGopMsg(int& infoType);
	string getPicMsg(int& infoType);
	string getMPackMsg(int& infoType);

public:
	ESCALLBACK escb;
	ESERRCALLBACK eserrcb;
	HANDLE ptsHandle;

private:
	MPack*mpack;
	PesPack*mPes;
	int pesPos;

	int picFirstime;
	int gopFirstime;
	int seqFirstime;
		
public:
	MPEG2Info mpg2Info;
	Es_ErrorType errType;
private:
	int MPEG2_Flag;
	int profileID, levelID;
	int horizontal_size;
	int vertical_size;
	double bit_rate;
	double frame_rate; 
	int frame_rate_code; 
	int bit_rate_value;
	int chroma_format;
	int video_format;  
	int picture_coding_type;
	int picture_structure;
	int top_field_first;

	unsigned int iInterval;
	unsigned int minI;

	unsigned int seqInterval;
	unsigned int minSeq;

	unsigned int gopInterval;
	unsigned int minGop;

	int pesOnePic;

	int vbv_buffer_size;
	int profile_and_level_indication;

	int closed_gop;
	int I_flag;

};


#endif

