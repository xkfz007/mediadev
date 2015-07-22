#ifndef _H264ANALYSIS_H_
#define _H264ANALYSIS_H_
#include "h264typedefs.h"
#include "es_defines.h"

int GetRBSP(NALU_t*);
int NALU2RBSP (NALU_t *nalu);
int EBSP2RBSP(unsigned char   *streamBuffer, int end_bytepos, int begin_bytepos);	
int RBSP2SODB(unsigned char   *streamBuffer, int last_byte_pos);

seq_parameter_set_rbsp_t *AllocSPS ();
pic_parameter_set_rbsp_t *AllocPPS ();

void FreePPS (pic_parameter_set_rbsp_t *pps);
void FreeSPS (seq_parameter_set_rbsp_t *sps);

Bitstream *AllocBitStream();
void FreeBitStream(Bitstream*bitstream);

int sps_is_equal(seq_parameter_set_rbsp_t *sps1, seq_parameter_set_rbsp_t *sps2);
int pps_is_equal(pic_parameter_set_rbsp_t *pps1, pic_parameter_set_rbsp_t *pps2);

unsigned CeilLog2( unsigned uiVal);

NALU_t *AllocNALU(int buffersize);
void FreeNALU(NALU_t *n);


int FindStartCode_h264 (unsigned char *Buf, int zeros_in_startcode);
SliceHeader*AllocSH();
void FreeSH(SliceHeader*);


int InterpretSPS (Bitstream *s ,seq_parameter_set_rbsp_t *sps,int&UsedBits);
int InterpretPPS (Bitstream *s, pic_parameter_set_rbsp_t *pps,int&UsedBits,seq_parameter_set_rbsp_t SeqParSet[]);

int FirstPartOfSliceHeader(Bitstream *bitstream,SliceHeader *sliceH,int&UsedBits,
						   seq_parameter_set_rbsp_t* &active_sps,
						   pic_parameter_set_rbsp_t* &active_pps,
						   seq_parameter_set_rbsp_t SeqParSet[],
						   pic_parameter_set_rbsp_t PicParSet[]);
//void UseParameterSet (int pic_parameter_set_id);
//int RestOfSliceHeader(Bitstream*currStream,SliceHeader *sliceH,int&UsedBits);

int ReadVUI(Bitstream *s, seq_parameter_set_rbsp_t *sps,int &UsedBits);
int ReadHRDParameters(Bitstream *s , hrd_parameters_t *hrd,int &UsedBits);

void Scaling_List(int *scalingList, int sizeOfScalingList, int *UseDefaultScalingMatrix, Bitstream *s,int &UsedBits);

void dec_ref_pic_marking(Bitstream *currStream,SliceHeader *sliceH,int &UsedBits);
void pred_weight_table(Bitstream *currStream,SliceHeader *sliceH,int &UsedBits,seq_parameter_set_rbsp_t*active_sps, pic_parameter_set_rbsp_t*active_pps);
void ref_pic_list_reordering(Bitstream *currStream,SliceHeader *sliceH,int &UsedBits);



#endif