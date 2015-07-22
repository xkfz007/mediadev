#ifndef _H264TYPEDEFS_H_
#define _H264TYPEDEFS_H_

#ifndef _TYPEDEFS_H_
#define _TYPEDEFS_H_
#include <string>

//using std::string;

typedef enum {
	CF_UNKNOWN = -1,     //!< Unknown color format
	YUV400     =  0,     //!< Monochrome
	YUV420     =  1,     //!< 4:2:0
	YUV422     =  2,     //!< 4:2:2
	YUV444     =  3      //!< 4:4:4
} ColorFormat;

typedef enum
{
	CAVLC,
	CABAC
} SymbolMode;

typedef enum
{
	FRAME,
	TOP_FIELD,
	BOTTOM_FIELD
} PictureStructure;           //!< New enum for field processing

typedef enum
{
	P_SLICE = 0,
	B_SLICE = 1,
	I_SLICE = 2,
	SP_SLICE = 3,
	SI_SLICE = 4,
	NUM_SLICE_TYPES = 5
} SliceType;

//typedef unsigned char  unsigned char  ;     //!< unsigned char   type definition

typedef struct bit_stream_dec
{
	// CABAC Decoding
	//int           read_len;           //!< actual position in the codebuffer, CABAC only
	//int           code_len;           //!< overall codebuffer length, CABAC only
	// CAVLC Decoding
	int           frame_bitoffset;    //!< actual position in the codebuffer, bit-oriented, CAVLC only
	int           bitstream_length;   //!< over codebuffer lnegth, unsigned char   oriented, CAVLC only
	// ErrorConcealment
	unsigned char            *streamBuffer;      //!< actual codebuffer for read bytes
	//int           ei_flag;            //!< error indication, 0: no error, else unspecified error
}Bitstream;

typedef struct syntaxelement_dec
{
	//	int           type;                  //!< type of syntax element for data part.
	int           value1;                //!< numerical value of syntax element
	int           value2;                //!< for blocked symbols, e.g. run/level
	int           len;                   //!< length of code
	int           inf;                   //!< info part of CAVLC code
	// 	unsigned int  bitpattern;            //!< CAVLC bitpattern
	// 	int           context;               //!< CABAC context
	// 	int           k;                     //!< CABAC context for coeff_count,uv

	//! for mapping of CAVLC to syntaxElement
	void  (*mapping)(int len, int info, int *value1, int *value2);
	//! used for CABAC: refers to actual coding method of each individual syntax element type
	//	void  (*reading)(struct macroblock_dec *currMB, struct syntaxelement_dec *, DecodingEnvironmentPtr);
} SyntaxElement;

typedef enum {
	FREXT_CAVLC444 = 44,       //!< YUV 4:4:4/14 "CAVLC 4:4:4"
	BASELINE       = 66,       //!< YUV 4:2:0/8  "Baseline"
	MAIN           = 77,       //!< YUV 4:2:0/8  "Main"
	EXTENDED       = 88,       //!< YUV 4:2:0/8  "Extended"
	FREXT_HP       = 100,      //!< YUV 4:2:0/8  "High"
	FREXT_Hi10P    = 110,      //!< YUV 4:2:0/10 "High 10"
	FREXT_Hi422    = 122,      //!< YUV 4:2:2/10 "High 4:2:2"
	FREXT_Hi444    = 244,      //!< YUV 4:4:4/14 "High 4:4:4"
	MVC_HIGH       = 118,      //!< YUV 4:2:0/8  "Multiview High"
	STEREO_HIGH    = 128       //!< YUV 4:2:0/8  "Stereo High"
} ProfileIDC;

static const unsigned char   ZZ_SCAN[16]  =
{  0,  1,  4,  8,  5,  2,  3,  6,  9, 12, 13, 10,  7, 11, 14, 15
};

static const unsigned char   ZZ_SCAN8[64] =
{  0,  1,  8, 16,  9,  2,  3, 10, 17, 24, 32, 25, 18, 11,  4,  5,
12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13,  6,  7, 14, 21, 28,
35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
};

typedef struct slice
{
  int first_mb_in_slice;
  int pic_parameter_set_id;
  int sliceType;
  int colour_plane_id;
  
  int field_pic_flag,bottom_field_flag, mb_aff_frame_flag;
  int  pic_order_cnt_lsb, delta_pic_order_cnt_bottom;

  int delta_pic_order_cnt[2];
  int redundant_pic_cnt;
  int direct_spatial_mv_pred_flag;
  int frame_num;
  int num_ref_idx_override_flag;
  int cabac_init_idc;
  int slice_qp_delta;
  int sp_for_switch_flag;
  int slice_qs_delta;
  short               DFDisableIdc;     //!< Disable deblocking filter on slice
  short               DFAlphaC0Offset;  //!< Alpha and C0 offset for filtering slice
  short               DFBetaOffset;     //!< Beta offset for filtering slice
  int slice_group_change_cycle;   
  int structure;

  int idrFlag;
  int idrPicId;

  int num_ref_idx_activel0;
  int num_ref_idx_activel1;

  int luma_weight_flag_l0, luma_weight_flag_l1, chroma_weight_flag_l0, chroma_weight_flag_l1;
  unsigned short luma_log2_weight_denom;
  unsigned short chroma_log2_weight_denom;
  int luma_weight_l0,luma_offset_l0;
  int luma_weight_l1,luma_offset_l1;
  int chroma_weight_l0,chroma_offset_l0;
  int chroma_weight_l1,chroma_offset_l1;

  int nal_reference_idc;

} SliceHeader;


#endif
#endif