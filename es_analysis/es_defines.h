#ifndef _DEFINES_H_
#define _DEFINES_H_

#ifdef _DEBUG
#define TRACE 0
#else
#define TRACE 0
#endif
#include <string>
using std::string;

#ifdef ES_ANALYSIS_EXPORTS 
#define ES_API __declspec(dllexport)
#else
#define ES_API __declspec(dllimport)
#endif
typedef unsigned long       DWORD;
typedef unsigned int        UINT;
//pes头定义
struct pes_header
{
	DWORD		           packet_start_code_prefix;
	unsigned char          stream_id;
	UINT		           PES_packet_length;
	unsigned char          PES_scrambling_control;
	unsigned char          PES_priority;
	unsigned char          data_alignment_indicator;
	unsigned char          copyrignt;
	unsigned char          original_or_copy;
	unsigned char          PTS_DTS_flags;
	unsigned char          ESCR_flag;
	unsigned char          ES_rate_flag;
	unsigned char          DSM_trick_mode_flag;
	unsigned char          additional_copy_info_flag;
	unsigned char          PES_CRC_flag;
	unsigned char          PES_extension_flag;
	unsigned char          PES_header_data_length;
	__int64 PTS;
	__int64 DTS;
	__int64 ESCR_base;
	DWORD ES_rate;
	DWORD ESCR_extension;
	unsigned char          trick_mode_control;
	unsigned char          field_id;
	unsigned char          intra_slice_refresh;
	unsigned char          frequency_truncation;
	unsigned char          field_rep_cntrl;
	unsigned char          additional_copy_info;
	UINT                   previous_PES_packet_CRC;
	unsigned char          PES_private_data_flag;
	unsigned char          pack_header_field_flag;
	unsigned char          program_packet_sequence_counter_flag;
	unsigned char          P_STD_buffer_flag;
	unsigned char          PES_extension_flag_2;
	unsigned char          PES_private_data[128];
	unsigned char          packet_field_length;
	unsigned char          pack_header[128];
	unsigned char          program_packet_sequence_counter;
	unsigned char          MPEG1_MPEG2_indentifier;
	unsigned char          original_stuff_length;
	unsigned char          P_STD_buffer_scale;
	unsigned short         P_STD_buffer_size;
	unsigned char          PES_extension_field_length;

};

//pes包定义
typedef struct pes_packet
{
	pes_header *p_header;
	unsigned char *p_data;
	UINT  i_len;
}PesPack;


//回调函数的定义
typedef void *HANDLE;
typedef void (*ESCALLBACK)(HANDLE ptsHandle,HANDLE pesHandle,int msgType);
typedef void (*ESERRCALLBACK)(HANDLE ptsHandle,HANDLE pesHandle,int eType);

typedef enum
{
	ALLINFO=0xffffffff,
	ES_NO_INFO = 0,
	ES_MPEG2_minIInterval=0x0001,
	ES_MPEG2_minSeqInterval=0x0002,
	ES_MPEG2_minGopInterval=0x0004,
	ES_MPEG2_streamStruct=0x0008,

	ES_MPEG2_horizontal=0x0010,
	ES_MPEG2_vertical=0x0020,
	ES_MPEG2_resolution=0x0040,
	ES_MPEG2_profile=0x0080,

	ES_MPEG2_level=0x0100,
	ES_MPEG2_frameRate=0x0200,
	ES_MPEG2_videoFormat=0x0400,
	ES_MPEG2_chromaFormat=0x0800,

	ES_MPEG2_picType=0x1000,
	ES_MPEG2_picStruct=0x2000,
	ES_MPEG2_TYPE15=0x4000,
	ES_MPEG2_TYPE16=0x8000,


	ES_H264_maxIInterval=0x00010000,
	ES_H264_maxRapInterval=0x00020000,
	ES_H264_profile=0x00040000,
	ES_H264_level=0x00080000,

	ES_H264_horizontal=0x00100000,
	ES_H264_vertical=0x00200000,
	ES_H264_resolution=0x00400000,
	ES_H264_chromaFormat=0x00800000,

	ES_H264_entropyType=0x01000000,
	ES_H264_picType=0x02000000,
	ES_H264_picStruct=0x04000000,
	ES_H264_streamStruct=0x08000000,

	ES_H264_TYPE13=0x10000000,
	ES_H264_TYPE14=0x20000000,
	ES_H264_TYPE15=0x40000000,
	ES_H264_TYPE16=0x80000000,

}es_info_type_e;
//mpeg2返回的结构信息
typedef struct
{
	unsigned int minIInterval;//最小I帧间隔
	unsigned int minSeqInterval;//最小序列头间隔
	unsigned int minGopInterval;//GOP长度
	string streamStruct;//视频序列结构 "I P B B"

	int horizontal;//水平采样率
	int vertical;//垂直采样率
	string resolution;//分辨率"356x288"

	int profile;//Profile
	int level;//Level
	double frameRate;//帧率 30.0/25.0

	string videoFormat;//视频格式 PAL/NTSC
	string chromaFormat;//色度 YUV420/YUV422

	string picType;//I P B帧 :  I/P/B 
	string picStruct;//图像结构		FRAME/TOP_FIELD/BOTTOM_FIELD
	//14

}MPEG2Info;
//h.264返回信息
typedef struct  
{
	int maxIInterval;//I或IDR最大间隔
	int maxRapInterval;//RAP最大间隔

	int profile;//Profile   
	int level;//Level  

	int horizontal;//水平采样率
	int vertical;//垂直采样率	

	string resolution;//分辨率 176x144
	string chromaFormat;//视频格式 YUV420/YUV422
	string entropyType;//熵编码方式 CAVLC/CABAC

	string picType;//图像类型 I P B
	string picStruct;//图像结构 FRAME/TOP_FIELD/BOTTOM_FIELD
	string streamStruct;//序列结构 "IDR P B B "
	
	//12

}H264Info;

//错误类型
enum Es_ErrorType
{
	STREAM_ERROR               = 0x1001,//ES码流错误,无法继续分析	
	//mpeg2中主要错误
	PES_WITH_SEQ_PTSDTS_ERROR  = 0x1002,//序列头出现在一个携带PTS/DTS的PES包头
	CLOSE_GOP_ERROR            = 0x1003,//封闭GOP
	PES_WITH_PCR_ERROR         = 0x1004, //PCR出现在PES包头
	PES_RANDOW_INDI_ERROR      = 0x1005, //随机指示位
	PES_EVERY_PIC_ERROR        = 0x1006, //一个PES头表示每个图像

	//h.264中主要错误
	PES_STREAMID_ERROR         = 0x1007, //PES流ID为0xE0
	PES_WITHOUT_PTSDTS_ERR0R   = 0x1008, //每个PES必须携带PTS/DTS
	PES_NO_SCRAMBLE_ERROR      = 0x1009, //PES无加扰
	DATA_SLICE_ERROR           = 0x100a, //数据片分析
	PES_SYN_ERROR              = 0x100b, //PES语法正确
	NAL_EVERY_PIC_ERROR        = 0x100c, //每个图像必须一个完整NAL单元
	SPSEXTENSION_ERROR         = 0x100d, //不允许使用SPS扩展
	NALFILTER_ERROR            = 0x100e, //不允许使用NAL滤波
	ASO_ERROR                  = 0x100f, //不允许使用ASO
	I2IDR_ERROR                = 0x1010, //I帧必须为IDR帧

	ES_DROP_FRAME              = 0x1011,//丢帧错误

};
typedef struct  
{
	unsigned char*buf;
	int bitOffset;
	int len;

}MPack;
typedef enum {
	NALU_TYPE_SLICE    = 1,
	NALU_TYPE_DPA      = 2,
	NALU_TYPE_DPB      = 3,
	NALU_TYPE_DPC      = 4,
	NALU_TYPE_IDR      = 5,
	NALU_TYPE_SEI      = 6,
	NALU_TYPE_SPS      = 7,
	NALU_TYPE_PPS      = 8,
	NALU_TYPE_AUD      = 9,
	NALU_TYPE_EOSEQ    = 10,
	NALU_TYPE_EOSTREAM = 11,
	NALU_TYPE_FILL     = 12,
	NALU_TYPE_SPS_EXTENSION=13,
} NaluType;

typedef enum {
	NALU_PRIORITY_HIGHEST     = 3,
	NALU_PRIORITY_HIGH        = 2,
	NALU_PRIORITY_LOW         = 1,
	NALU_PRIORITY_DISPOSABLE  = 0
} NalRefIdc;
typedef struct
{
	int       startcodeprefix_len;   //!< 4 for parameter sets and first slice in picture, 3 for everything else (suggested)
	unsigned  len;                   //!< Length of the NAL unit (Excluding the start code, which does not belong to the NALU)
	unsigned  max_size;              //!< NAL Unit Buffer size
	int       forbidden_bit;         //!< should be always FALSE
	NaluType  nal_unit_type;         //!< NALU_TYPE_xxxx
	NalRefIdc nal_reference_idc;     //!< NALU_PRIORITY_xxxx  
	unsigned char       *buf;                   //!< contains the first unsigned char   followed by the EBSP
	//uint16    lost_packets;          //!< true, if packet loss is detected

} NALU_t;

#define MAXnum_slice_groups_minus1  8

typedef struct
{
	int   Valid;                  // indicates the parameter set is valid
	unsigned int pic_parameter_set_id;                             // ue(v)
	unsigned int seq_parameter_set_id;                             // ue(v)
	int   entropy_coding_mode_flag;                            // u(1)
	int   transform_8x8_mode_flag;                             // u(1)

	int   pic_scaling_matrix_present_flag;                     // u(1)
	int       pic_scaling_list_present_flag[12];                   // u(1)
	int       ScalingList4x4[6][16];                               // se(v)
	int       ScalingList8x8[6][64];                               // se(v)
	int   UseDefaultScalingMatrix4x4Flag[6];
	int   UseDefaultScalingMatrix8x8Flag[6];

	// if( pic_order_cnt_type < 2 )  in the sequence parameter set
	int      bottom_field_pic_order_in_frame_present_flag;                           // u(1)
	unsigned int num_slice_groups_minus1;                          // ue(v)
	unsigned int slice_group_map_type;                        // ue(v)
	// if( slice_group_map_type = = 0 )
	unsigned int run_length_minus1[MAXnum_slice_groups_minus1]; // ue(v)
	// else if( slice_group_map_type = = 2 )
	unsigned int top_left[MAXnum_slice_groups_minus1];         // ue(v)
	unsigned int bottom_right[MAXnum_slice_groups_minus1];     // ue(v)
	// else if( slice_group_map_type = = 3 || 4 || 5
	int   slice_group_change_direction_flag;            // u(1)
	unsigned int slice_group_change_rate_minus1;               // ue(v)
	// else if( slice_group_map_type = = 6 )
	unsigned int pic_size_in_map_units_minus1;             // ue(v)
	unsigned char        *slice_group_id;                              // complete MBAmap u(v)

	int num_ref_idx_l0_active_minus1;                     // ue(v)
	int num_ref_idx_l1_active_minus1;                     // ue(v)
	int   weighted_pred_flag;                               // u(1)
	unsigned int  weighted_bipred_idc;                              // u(2)
	int       pic_init_qp_minus26;                              // se(v)
	int       pic_init_qs_minus26;                              // se(v)
	int       chroma_qp_index_offset;                           // se(v)

	int       cb_qp_index_offset;                               // se(v)
	int       cr_qp_index_offset;                               // se(v)
	int       second_chroma_qp_index_offset;                    // se(v)

	int   deblocking_filter_control_present_flag;           // u(1)
	int   constrained_intra_pred_flag;                      // u(1)
	int   redundant_pic_cnt_present_flag;                   // u(1)
	int   vui_pic_parameters_flag;                          // u(1)
} pic_parameter_set_rbsp_t;

#define MAXIMUMVALUEOFcpb_cnt   32
typedef struct
{
	unsigned int cpb_cnt_minus1;                                   // ue(v)
	unsigned int bit_rate_scale;                                   // u(4)
	unsigned int cpb_size_scale;                                   // u(4)
	unsigned int bit_rate_value_minus1 [MAXIMUMVALUEOFcpb_cnt];    // ue(v)
	unsigned int cpb_size_value_minus1 [MAXIMUMVALUEOFcpb_cnt];    // ue(v)
	unsigned int cbr_flag              [MAXIMUMVALUEOFcpb_cnt];    // u(1)
	unsigned int initial_cpb_removal_delay_length_minus1;          // u(5)
	unsigned int cpb_removal_delay_length_minus1;                  // u(5)
	unsigned int dpb_output_delay_length_minus1;                   // u(5)
	unsigned int time_offset_length;                               // u(5)
} hrd_parameters_t;

typedef struct
{
	int      aspect_ratio_info_present_flag;                   // u(1)
	unsigned int aspect_ratio_idc;                                 // u(8)
	unsigned short sar_width;                                      // u(16)
	unsigned short sar_height;                                     // u(16)
	int      overscan_info_present_flag;                       // u(1)
	int      overscan_appropriate_flag;                        // u(1)
	int      video_signal_type_present_flag;                   // u(1)
	unsigned int video_format;                                     // u(3)
	int      video_full_range_flag;                            // u(1)
	int      colour_description_present_flag;                  // u(1)
	unsigned int colour_primaries;                                 // u(8)
	unsigned int transfer_characteristics;                         // u(8)
	unsigned int matrix_coefficients;                              // u(8)
	int      chroma_location_info_present_flag;                // u(1)
	unsigned int  chroma_sample_loc_type_top_field;                // ue(v)
	unsigned int  chroma_sample_loc_type_bottom_field;             // ue(v)
	int      timing_info_present_flag;                         // u(1)
	unsigned int num_units_in_tick;                                // u(32)
	unsigned int time_scale;                                       // u(32)
	int      fixed_frame_rate_flag;                            // u(1)
	int      nal_hrd_parameters_present_flag;                  // u(1)
	hrd_parameters_t nal_hrd_parameters;                           // hrd_paramters_t
	int      vcl_hrd_parameters_present_flag;                  // u(1)
	hrd_parameters_t vcl_hrd_parameters;                           // hrd_paramters_t
	// if ((nal_hrd_parameters_present_flag || (vcl_hrd_parameters_present_flag))
	int      low_delay_hrd_flag;                               // u(1)
	int      pic_struct_present_flag;                          // u(1)
	int      bitstream_restriction_flag;                       // u(1)
	int      motion_vectors_over_pic_boundaries_flag;          // u(1)
	unsigned int max_bytes_per_pic_denom;                          // ue(v)
	unsigned int max_bits_per_mb_denom;                            // ue(v)
	unsigned int log2_max_mv_length_vertical;                      // ue(v)
	unsigned int log2_max_mv_length_horizontal;                    // ue(v)
	unsigned int num_reorder_frames;                               // ue(v)
	unsigned int max_dec_frame_buffering;                          // ue(v)
} vui_seq_parameters_t;


#define MAXnum_ref_frames_in_pic_order_cnt_cycle  256
typedef struct
{
	int   Valid;                  // indicates the parameter set is valid

	unsigned int profile_idc;                                       // u(8)
	int   constrained_set0_flag;                                // u(1)
	int   constrained_set1_flag;                                // u(1)
	int   constrained_set2_flag;                                // u(1)
	int   constrained_set3_flag;                                // u(1)

	unsigned  int level_idc;                                        // u(8)
	unsigned  int seq_parameter_set_id;                             // ue(v)
	unsigned  int chroma_format_idc;                                // ue(v)

	int   seq_scaling_matrix_present_flag;                   // u(1)
	int       seq_scaling_list_present_flag[12];                 // u(1)
	int       ScalingList4x4[6][16];                             // se(v)
	int       ScalingList8x8[6][64];                             // se(v)
	int   UseDefaultScalingMatrix4x4Flag[6];
	int   UseDefaultScalingMatrix8x8Flag[6];

	unsigned int bit_depth_luma_minus8;                            // ue(v)
	unsigned int bit_depth_chroma_minus8;                          // ue(v)
	unsigned int log2_max_frame_num_minus4;                        // ue(v)
	unsigned int pic_order_cnt_type;
	// if( pic_order_cnt_type == 0 )
	unsigned int log2_max_pic_order_cnt_lsb_minus4;                 // ue(v)
	// else if( pic_order_cnt_type == 1 )
	int delta_pic_order_always_zero_flag;               // u(1)
	int     offset_for_non_ref_pic;                         // se(v)
	int     offset_for_top_to_bottom_field;                 // se(v)
	unsigned int num_ref_frames_in_pic_order_cnt_cycle;          // ue(v)
	// for( i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ )
	int   offset_for_ref_frame[MAXnum_ref_frames_in_pic_order_cnt_cycle];   // se(v)
	unsigned int num_ref_frames;                                   // ue(v)
	int   gaps_in_frame_num_value_allowed_flag;             // u(1)
	unsigned int pic_width_in_mbs_minus1;                          // ue(v)
	unsigned int pic_height_in_map_units_minus1;                   // ue(v)
	int   frame_mbs_only_flag;                              // u(1)
	// if( !frame_mbs_only_flag )
	int   mb_adaptive_frame_field_flag;                   // u(1)
	int   direct_8x8_inference_flag;                        // u(1)
	int   frame_cropping_flag;                              // u(1)
	unsigned int frame_crop_left_offset;                // ue(v)
	unsigned int frame_crop_right_offset;               // ue(v)
	unsigned int frame_crop_top_offset;                 // ue(v)
	unsigned int frame_crop_bottom_offset;              // ue(v)
	int   vui_parameters_present_flag;                      // u(1)
	vui_seq_parameters_t vui_seq_parameters;                  // vui_seq_parameters_t
	unsigned  separate_colour_plane_flag;                       // u(1)

	int lossless_qpprime_flag;
} seq_parameter_set_rbsp_t;

#define ZEROBYTES_SHORTSTARTCODE 2 
#define MAX_CODED_FRAME_SIZE 8000000         //!< bytes for one frame


typedef struct
{
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

	int iInterval;
	int minI;

	int seqInterval;
	int minSeq;

	int gopInterval;
	int minGop;

	int pesOnePic;

	int vbv_buffer_size;
	int profile_and_level_indication;

}MGlobalVars;



#endif