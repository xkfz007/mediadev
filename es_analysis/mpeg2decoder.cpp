#include "Mpeg2Decoder.h"
#include "mpeg2typedefs.h"
#include "mpeg2analysis.h"
#include <sstream>
#include <iostream>
using std::stringstream;
using std::endl;
using std::cout;
static double frame_rate_Table[16] =
{
	0.0,
	((23.0*1000.0)/1001.0),
	24.0,
	25.0,
	((30.0*1000.0)/1001.0),
	30.0,
	50.0,
	((60.0*1000.0)/1001.0),
	60.0,

	RESERVED,
	RESERVED,
	RESERVED,
	RESERVED,
	RESERVED,
	RESERVED,
	RESERVED
};
Mpeg2Decoder::Mpeg2Decoder()
{	
	mpack=AllocMpack(MAX_CODED_FRAME_SIZE);
	mPes=NULL;
	pesPos=0;
	vbv_buffer_size=0;
	profile_and_level_indication=0;

	picFirstime=1;
	gopFirstime=1;
	seqFirstime=1;

	iInterval=0;
	minI=0xffffffff;

	seqInterval=0;
	minSeq=0xfffffff;

	gopInterval=0;
	minGop=0xffffffff;

	pesOnePic=0;

	escb=NULL;
	eserrcb=NULL;
	ptsHandle=NULL;

	I_flag=0;

	mpg2Info.minIInterval=0xffffffff;
	mpg2Info.minSeqInterval=0xffffffff;
	mpg2Info.minGopInterval=0xffffffff;
	mpg2Info.streamStruct="";
	mpg2Info.horizontal=0;
	mpg2Info.vertical=0;
	mpg2Info.resolution="";
	mpg2Info.profile=0;
	mpg2Info.level=0;
	mpg2Info.frameRate=0;
	mpg2Info.videoFormat="";
	mpg2Info.chromaFormat="";
	mpg2Info.picType="";
	mpg2Info.picStruct="";
}

Mpeg2Decoder::~Mpeg2Decoder(void)
{
	FreeMpack(mpack);
}
int Mpeg2Decoder::NextPesPack(PesPack*pes)
{
	//int hdLen=sizeof(pes_header);
	int tolLen=pes->i_len;

	//Mpeg2Decoder*mpg2Dec=(Mpeg2Decoder*)pesHandle;

	//mpg2Dec->
	SetPesPack(pes);
	if(GetMpackFromPES()<0)
	{
		//没有找到帧, 丢帧报错
		eserrcb(ptsHandle,this,ES_DROP_FRAME);
		return -1;
	}
	int flag;
	do
	{
		flag=processMPack();
		if(flag>1)
			break;
	}while(flag>0&&pesPos<tolLen-1);
	// 	int flag=1;
	// 	while(flag>0)
	// 	{
	// 		flag=processMPack();
	// 	}	
	return flag;
}
string Mpeg2Decoder::getSeqMsg(int& infoType)
{
	stringstream seqMsg;
	if(MPEG2_Flag)
	{
		seqMsg<<"MPEG2_Flag=TRUE"<<endl;
	}
	seqMsg<<"Profile="<<profileID<<endl;
	mpg2Info.profile=profileID;
	infoType|=ES_MPEG2_profile;

	seqMsg<<"Level="<<levelID<<endl;
	mpg2Info.level=levelID;
	infoType|=ES_MPEG2_level;

	seqMsg<<"horizontal_size="<<horizontal_size<<endl;
	mpg2Info.horizontal=horizontal_size;
	infoType|=ES_MPEG2_horizontal;

	seqMsg<<"vertical_size="<<vertical_size<<endl;
	mpg2Info.vertical=vertical_size;
	infoType|=ES_MPEG2_vertical;

	seqMsg<<"bit_rate="<<bit_rate<<endl;
	seqMsg<<"frame_rate="<<frame_rate<<endl;
	mpg2Info.frameRate=frame_rate;
	infoType|=ES_MPEG2_frameRate;
	seqMsg<<"chroma_format="<<chroma_format;
	switch (chroma_format)
	{
	case CHROMA420:
		seqMsg<<" CHROMA420"<<endl;
		mpg2Info.chromaFormat="YUV420";
		break;
	case CHROMA422:
		seqMsg<<" CHROMA422"<<endl;
		mpg2Info.chromaFormat="YUV422";
		break;
	case CHROMA444:
		seqMsg<<" CHROMA444"<<endl;
		mpg2Info.chromaFormat="YUV444";
		break;
	default:
		printf("ERRor CHROMA\n");
		break;
	}	
	infoType|=ES_MPEG2_chromaFormat;

	seqMsg<<"video_format="<<video_format;
	switch (video_format)
	{
	case 0:
		seqMsg<<" comp"<<endl;;
		mpg2Info.videoFormat="comp";
		break;
	case 1:
		seqMsg<<" PAL"<<endl;
		mpg2Info.videoFormat="PAL";
		break;
	case 2:
		seqMsg<<" NTSC"<<endl;
		mpg2Info.videoFormat="NTSC";
		break;
	case 3:
		seqMsg<<" SECAM"<<endl;
		mpg2Info.videoFormat="SECAM";
		break;
	case 4:
		seqMsg<<" MAC"<<endl;
		mpg2Info.videoFormat="MAC";
		break;
	default:
		printf("undefined video foramt\n");
		break;
	}
	infoType|=ES_MPEG2_videoFormat;

	return seqMsg.str();
}

string Mpeg2Decoder::getGopMsg(int& infoType)
{
	stringstream gopMsg;
	gopMsg<<"Get a GOP Message"<<endl;
	return gopMsg.str();

}
string Mpeg2Decoder::getPicMsg(int& infoType)
{
	stringstream picMsg;
	picMsg<<"picture_coding_type="<<picture_coding_type;
	switch(picture_coding_type)
	{
	case I_TYPE:
		I_flag=1;
		if(mpg2Info.streamStruct.length()>1000)
			mpg2Info.streamStruct="";
		mpg2Info.streamStruct+="I";
		mpg2Info.picType="I";
		picMsg<<" I"<<endl;
		if (!picFirstime&&minI>iInterval)
		{
			minI=iInterval;
		}
		picFirstime=0;
		iInterval=0;
		//minI=0xffffff;
		break;
	case P_TYPE:
		I_flag=0;
		mpg2Info.streamStruct+="P";
		mpg2Info.picType="P";
		picMsg<<" P"<<endl;
		iInterval++;
		break;
	case B_TYPE:
		if(I_flag&&!closed_gop)
		{			
			eserrcb(ptsHandle,this,CLOSE_GOP_ERROR);
			//exit(-1);
		}
		I_flag=0;
		mpg2Info.streamStruct+="B";
		mpg2Info.picType="B";
		picMsg<<" B"<<endl;
		iInterval++;
		break;
	default:
		printf("undefined coding type\n");
		break;
	}
	infoType|=ES_MPEG2_picType;

	picMsg<<"picture_structure="<<picture_structure;
	switch(picture_structure)
	{
	case FRAME_PICTURE:
		picMsg<<" FRAME"<<endl;
		mpg2Info.picStruct="FRAME";
		break;
	case TOP_FIELD:
		picMsg<<" TOP_FIELD"<<endl;
		mpg2Info.picStruct="TOP_FIELD";
		break;
	case BOTTOM_FIELD:
		picMsg<<" BOTTOM_FIELD"<<endl;
		mpg2Info.picStruct="BOTTOM_FIELD";
		break;
	default:
		printf("undefined structure\n");
		break;
	}
	infoType|=ES_MPEG2_picStruct;

	picMsg<<"top_field_first="<<top_field_first<<endl;

	return picMsg.str();

}
string Mpeg2Decoder::getMPackMsg(int&infoType)
{
	stringstream mpkMsg;

	mpkMsg<<"The min interval between two I-frames is "<<minI<<endl;
	mpg2Info.minIInterval=minI;
	infoType|=ES_MPEG2_minIInterval;

	mpkMsg<<"The min interval between two Sequencess is "<<minSeq<<endl;
	mpg2Info.minSeqInterval=minSeq;
	infoType|=ES_MPEG2_minSeqInterval;

	mpkMsg<<"The min interval between two Gops is "<<minGop<<endl;
	mpg2Info.minGopInterval=minGop;
	infoType|=ES_MPEG2_minGopInterval;

	mpkMsg<<"VideoStructure:"<<mpg2Info.streamStruct<<endl;
	infoType|=ES_MPEG2_streamStruct;	

	return mpkMsg.str();
}

void Mpeg2Decoder::ShowAllMsg()
{
	string tmp;
	int infoType=0;
	tmp=getMPackMsg(infoType);
	escb(ptsHandle,this,infoType);
#ifdef _DEBUG
	cout<<tmp<<endl;
#endif
}
void Mpeg2Decoder::SetPesPack(PesPack*pes)
{
	mPes=pes;
	pesPos=0;
	pesOnePic=0;
}
int Mpeg2Decoder::processMPack()
{
	unsigned int code;
	code=mPackStartCode(mpack);
	//code=GetStartCodeBits(mpack);
	int infoType;
	string tmp;
	switch (code)
	{
	case SEQUENCE_HEADER_CODE:
#ifdef _DEBUG
		printf("\nFound SEQUENCE_HEADER\n");
#endif
		if(!seqFirstime&&minSeq>seqInterval)
		{
			minSeq=seqInterval;
		}
		seqFirstime=0;
		seqInterval=0;
		sequence_header(mpack);
		infoType=0;
		tmp=getSeqMsg(infoType);
#ifdef _DEBUG
		cout<<tmp<<endl;
#endif
		escb(ptsHandle,this,infoType);
		return 1;
		break;
	case GROUP_START_CODE:

#ifdef _DEBUG
		printf("\nFound GOP_HEADER\n");
#endif
		if(!gopFirstime&&minGop>gopInterval)
		{
			minGop=gopInterval;
			mpg2Info.minGopInterval=gopInterval;
		}
		gopFirstime=0;
		gopInterval=0;

		group_of_pictures_header(mpack);
// 		if(!closed_gop)
// 		{
// 			eserrcb(ptsHandle,this,CLOSE_GOP_ERROR);
// 			exit(-1);
// 		}
		infoType=0;
		tmp=getGopMsg(infoType);
#ifdef _DEBUG
		cout<<tmp<<endl;
#endif
		escb(ptsHandle,this,infoType);
		return 1;
		break;
	case PICTURE_START_CODE:
#ifdef _DEBUG
		printf("\nFound PICTURE_HEADER\n");
#endif
		gopInterval++;

		seqInterval++;
		if(pesOnePic)
		{
			printf("一个PES只能包含一幅图像\n");
			eserrcb(ptsHandle,this,PES_EVERY_PIC_ERROR);
			//exit(-1);	
			return 2;
		}
		pesOnePic=1;
		picture_header(mpack);

		infoType=0;
		tmp=getPicMsg(infoType);
#ifdef _DEBUG
		cout<<tmp<<endl;
#endif
		escb(ptsHandle,this,infoType);
		return 1;
		break;
	case SEQUENCE_END_CODE:
#ifdef _DEBUG
		printf("\nFound EQUENCE_END\n");
#endif
		if(minSeq>seqInterval)
		{
			minSeq=seqInterval;
		}
		seqInterval=0;
		ShowAllMsg();
		return 0;
		break;
	default:
		if(code>=SLICE_START_CODE_MIN&&code<=SLICE_START_CODE_MAX)
		{

#ifdef _DEBUG
			printf(".");
#endif
			if(GetMpackFromPES()>0)
				return 1;
			else
				return -1;
		}
		else
		{
			printf("Unexpected next_start_code %08x (ignored)\n",code);
			GetMpackFromPES();
			return -1;
		}	//exit(-1);		
		break;
	}
}

int Mpeg2Decoder::GetMpackFromPES()
{
	int info2 = 0, info3 = 0, pos = 0;
	int StartCodeFound = 0;
	//int hdLen=sizeof(pes_header);
	int tolLen=mPes->i_len;
	int kk=pesPos;//+hdLen;

	unsigned char*pBuf=mPes->p_data+pesPos;

	while(kk<tolLen-1)
	{
		pos++;
		if (*(pBuf++)!= 0)
			break;
		kk++;
	}
	if(*(pBuf - 1) != 1 || pos != 3||kk==tolLen-1)
	{
		//printf ("GetMpackFromPES: no Start Code at the beginning of the Mpack, return -1\n");
		return -1;
	}

	while ((pos+pesPos)<tolLen&&!StartCodeFound)
	{

		pos++;
		pBuf++;    
		info3 = FindStartCode(pBuf - 3, 2);
		if(info3==1)
			StartCodeFound = 1;
		//pBuf++;
	}

	if((pos+pesPos)==tolLen)
	{
		mpack->len = pos;
		memcpy (mpack->buf, mPes->p_data+pesPos, mpack->len);
		mpack->bitOffset=0;
		pesPos+=pos-1;
		return 1;
	}
	pos -=3;

	mpack->len = pos;
	memcpy (mpack->buf, mPes->p_data+pesPos, mpack->len);
	mpack->bitOffset=0;

	pesPos+=pos;
	return 1;

}

void Mpeg2Decoder::sequence_header(MPack*mpk)
{
	int i;
	int marker_bit;
	int load_intra_quantizer_matrix;
	int intra_quantizer_matrix[64];
	int load_non_intra_quantizer_matrix;
	int non_intra_quantizer_matrix[64];
	int startcode;
	int aspect_ratio_information;
	int constrained_parameters_flag;

	startcode                   = GetStartCodeBits(mpk);	
	horizontal_size             = GetBits(12,"SeqHeader:horizontal_size",mpk);
	vertical_size               = GetBits(12,"vertical_size",mpk);
	aspect_ratio_information    = GetBits(4,"aspect_ratio_information",mpk);
	frame_rate_code             = GetBits(4,"frame_rate_code",mpk);
	bit_rate_value              = GetBits(18,"bit_rate_value",mpk);

	marker_bit                  = GetBits(1,"marker_bit",mpk);

	vbv_buffer_size             = GetBits(10,"vbv_buffer_size",mpk);
	constrained_parameters_flag = GetBits(1,"constrained_parameters_flag",mpk);

	if((load_intra_quantizer_matrix = GetBits(1,"load_intra_quantizer_matrix",mpk)))
	{
		for (i=0; i<64; i++)
			intra_quantizer_matrix[i] = GetBits(8,"intra_quantizer_matrix",mpk);
	}


	if((load_non_intra_quantizer_matrix = GetBits(1,"load_non_intra_quantizer_matrix",mpk)))
	{
		for (i=0; i<64; i++)
			non_intra_quantizer_matrix[i] = GetBits(8,"non_intra_quantizer_matrix",mpk);
	}

	extension_and_user_data(mpk);
}

void Mpeg2Decoder::extension_and_user_data(MPack*mpk)
{
	int code,ext_ID;
	int startcode;

	if(GetMpackFromPES()<0)
		return;

	while ((code=mPackStartCode(mpk))==EXTENSION_START_CODE || code==USER_DATA_START_CODE)
	{
		if (code==EXTENSION_START_CODE)
		{
			//Flush_Buffer32();
			startcode                   = GetStartCodeBits(mpk);
			ext_ID = GetBits(4,"extension_start_code_identifier",mpk);
			switch (ext_ID)
			{
			case SEQUENCE_EXTENSION_ID:
				sequence_extension(mpk);
				break;
			case SEQUENCE_DISPLAY_EXTENSION_ID:
				sequence_display_extension(mpk);
				break;
			case QUANT_MATRIX_EXTENSION_ID:
				//quant_matrix_extension();
				break;
			case SEQUENCE_SCALABLE_EXTENSION_ID:
				//sequence_scalable_extension();
				break;
			case PICTURE_DISPLAY_EXTENSION_ID:
				//picture_display_extension();
				break;
			case PICTURE_CODING_EXTENSION_ID:
				picture_coding_extension(mpk);
				break;
			case PICTURE_SPATIAL_SCALABLE_EXTENSION_ID:
				//picture_spatial_scalable_extension();
				break;
			case PICTURE_TEMPORAL_SCALABLE_EXTENSION_ID:
				//picture_temporal_scalable_extension();
				break;
			case COPYRIGHT_EXTENSION_ID:
				//copyright_extension();
				break;
			default:
				fprintf(stderr,"reserved extension start code ID %d\n",ext_ID);
				break;
			}
			//next_start_code();
			if(GetMpackFromPES()<0)
				return;
		}
		else
		{
			//Flush_Buffer32();
			user_data(mpk);
			//break;
			if(GetMpackFromPES()<0)
				return;
		}
	}
}

void Mpeg2Decoder::sequence_extension(MPack*mpk)
{
	int horizontal_size_extension;
	int vertical_size_extension;
	int bit_rate_extension;
	int vbv_buffer_size_extension;
	int low_delay;
	int frame_rate_extension_n;
	int frame_rate_extension_d;

	int progressive_sequence;
	// int pos;
	int marker_bit;

	/* derive bit position for trace */
	MPEG2_Flag = 1;
	profile_and_level_indication = GetBits(8,"profile_and_level_indication",mpk);
	progressive_sequence         = GetBits(1,"progressive_sequence",mpk);
	chroma_format                = GetBits(2,"chroma_format",mpk);
	horizontal_size_extension    = GetBits(2,"horizontal_size_extension",mpk);
	vertical_size_extension      = GetBits(2,"vertical_size_extension",mpk);
	bit_rate_extension           = GetBits(12,"bit_rate_extension",mpk);
	marker_bit                   =GetBits(1,"sequence_extension",mpk);
	vbv_buffer_size_extension    = GetBits(8,"vbv_buffer_size_extension",mpk);
	low_delay                    = GetBits(1,"low_delay",mpk);
	frame_rate_extension_n       = GetBits(2,"frame_rate_extension_n",mpk);
	frame_rate_extension_d       = GetBits(5,"frame_rate_extension_d",mpk);

	frame_rate = frame_rate_Table[ frame_rate_code] *
		((frame_rate_extension_n+1)/(frame_rate_extension_d+1));

	/* special case for 422 profile & level must be made */
	if((profile_and_level_indication>>7) & 1)
	{  /* escape bit of profile_and_level_indication set */

		/* 4:2:2 Profile @ Main Level */
		if((profile_and_level_indication&15)==5)
		{
			mpg2Info.profile = PROFILE_422;
			mpg2Info.level   = MAIN_LEVEL;  
		}
	}
	else
	{
		mpg2Info.profile = profile_and_level_indication >> 4;  /* Profile is upper nibble */
		mpg2Info.level   = profile_and_level_indication & 0xF;  /* Level is lower nibble */
	}


	horizontal_size = (horizontal_size_extension<<12) | (horizontal_size&0x0fff);
	vertical_size = (vertical_size_extension<<12) | (vertical_size&0x0fff);


	/* ISO/IEC 13818-2 does not define bit_rate_value to be composed of
	* both the original bit_rate_value parsed in sequence_header() and
	* the optional bit_rate_extension in sequence_extension_header(). 
	* However, we use it for bitstream verification purposes. 
	*/
	bit_rate_value += (bit_rate_extension << 18);
	bit_rate = ((double) bit_rate_value) * 400.0;
	vbv_buffer_size += (vbv_buffer_size_extension << 10);

}


void Mpeg2Decoder::sequence_display_extension(MPack*mpk)
{
	int marker_bit;
	/* ISO/IEC 13818-2 section 6.2.2.4:  sequence_display_extension() */
	int color_description;
	int color_primaries;
	int transfer_characteristics;
	int matrix_coefficients;
	int display_horizontal_size;
	int display_vertical_size;

	video_format      = GetBits(3,"video_format",mpk);
	color_description = GetBits(1,"color_description",mpk);

	if (color_description)
	{
		color_primaries          = GetBits(8,"color_primaries",mpk);
		transfer_characteristics = GetBits(8,"transfer_characteristics",mpk);
		matrix_coefficients      = GetBits(8,"matrix_coefficients",mpk);
	}

	display_horizontal_size = GetBits(14,"display_horizontal_size",mpk);
	marker_bit=GetBits(1,"sequence_display_extension",mpk);
	display_vertical_size   = GetBits(14,"display_vertical_size",mpk);

}

void Mpeg2Decoder::group_of_pictures_header(MPack*mpk)
{
	int marker_bit;
	int startcode;
	/* ISO/IEC 13818-2 section 6.2.2.6: group_of_pictures_header()  */
	int drop_flag;
	int hour;
	int minute;
	int sec;
	int frame;
	//int closed_gop;
	int broken_link;
	startcode   = GetStartCodeBits(mpk);
	drop_flag   = GetBits(1,"drop_flag",mpk);
	hour        = GetBits(5,"hour",mpk);
	minute      = GetBits(6,"minute",mpk);
	marker_bit  = GetBits(1,"group_of_pictures_header()",mpk);
	sec         = GetBits(6,"sec",mpk);
	frame       = GetBits(6,"frame",mpk);
	closed_gop  = GetBits(1,"closed_gop",mpk);
	broken_link = GetBits(1,"broken_link",mpk);

	extension_and_user_data(mpk);

}
void Mpeg2Decoder::picture_header(MPack*mpk)
{
	int startcode;

	int vbv_delay;
	int full_pel_forward_vector;
	int forward_f_code;
	int full_pel_backward_vector;
	int backward_f_code;
	int temporal_reference;

	startcode           = GetStartCodeBits(mpk);

	temporal_reference  = GetBits(10,"temporal_reference",mpk);
	picture_coding_type = GetBits(3,"picture_coding_type",mpk);
	vbv_delay           = GetBits(16,"vbv_delay",mpk);

	if (picture_coding_type==P_TYPE || picture_coding_type==B_TYPE)
	{
		full_pel_forward_vector = GetBits(1,"full_pel_forward_vector",mpk);
		forward_f_code = GetBits(3,"forward_f_code",mpk);
	}
	if (picture_coding_type==B_TYPE)
	{
		full_pel_backward_vector = GetBits(1,"full_pel_backward_vector",mpk);
		backward_f_code = GetBits(3,"backward_f_code",mpk);
	}


	//Extra_Information_Byte_Count = 	extra_bit_information();

	extension_and_user_data(mpk);

	/* update tracking information used to assist spatial scalability */
	//Update_Temporal_Reference_Tacking_Data();
}
void Mpeg2Decoder::picture_coding_extension(MPack*mpk)
{
	int q_scale_type;
	int alternate_scan;
	int f_code[2][2];
	int intra_dc_precision;

	int frame_pred_frame_dct;
	int concealment_motion_vectors;

	int intra_vlc_format;

	int repeat_first_field;

	int chroma_420_type;
	int progressive_frame;
	int composite_display_flag;
	int v_axis;
	int field_sequence;
	int sub_carrier;
	int burst_amplitude;
	int sub_carrier_phase;

	f_code[0][0] = GetBits(4,"f_code[0,0]",mpk);
	f_code[0][1] = GetBits(4,"f_code[0,1]",mpk);
	f_code[1][0] = GetBits(4,"f_code[1,0]",mpk);
	f_code[1][1] = GetBits(4,"f_code[1,1]",mpk);

	intra_dc_precision         = GetBits(2,"intra_dc_precision",mpk);
	picture_structure          = GetBits(2,"picture_structure",mpk);
	top_field_first            = GetBits(1,"top_field_first",mpk);
	frame_pred_frame_dct       = GetBits(1,"frame_pred_frame_dct",mpk);
	concealment_motion_vectors = GetBits(1,"concealment_motion_vectors",mpk);
	q_scale_type           = GetBits(1,"q_scale_type",mpk);
	intra_vlc_format           = GetBits(1,"intra_vlc_format",mpk);
	alternate_scan         = GetBits(1,"alternate_scan",mpk);
	repeat_first_field         = GetBits(1,"repeat_first_field",mpk);
	chroma_420_type            = GetBits(1,"chroma_420_type",mpk);
	progressive_frame          = GetBits(1,"progressive_frame",mpk);
	composite_display_flag     = GetBits(1,"composite_display_flag",mpk);

	if (composite_display_flag)
	{
		v_axis            = GetBits(1,"v_axis",mpk);
		field_sequence    = GetBits(3,"field_sequence",mpk);
		sub_carrier       = GetBits(1,"sub_carrier",mpk);
		burst_amplitude   = GetBits(7,"burst_amplitude",mpk);
		sub_carrier_phase = GetBits(8,"sub_carrier_phase",mpk);
	}


}


