#include "h264analysis.h"
#include "vlc.h"
#include "errorHandle.h"
#include <cassert>
int InterpretSPS (Bitstream *s ,seq_parameter_set_rbsp_t *sps,int &UsedBits)
{
	unsigned i;
	unsigned n_ScalingList;
	int reserved_zero;

	assert (s != NULL);
	assert (s->streamBuffer != 0);
	assert (sps != NULL);

	sps->profile_idc                            = read_u_v  (8, "SPS: profile_idc"                           , s, &UsedBits);

	if ((sps->profile_idc!=BASELINE       ) &&
		(sps->profile_idc!=MAIN           ) &&
		(sps->profile_idc!=EXTENDED       ) &&
		(sps->profile_idc!=FREXT_HP       ) &&
		(sps->profile_idc!=FREXT_Hi10P    ) &&
		(sps->profile_idc!=FREXT_Hi422    ) &&
		(sps->profile_idc!=FREXT_Hi444    ) &&
		(sps->profile_idc!=FREXT_CAVLC444 )
		)
	{
		printf("Invalid Profile IDC (%d) encountered. \n", sps->profile_idc);
		return UsedBits;
	}


	sps->constrained_set0_flag                  = read_u_1  (   "SPS: constrained_set0_flag"                 , s, &UsedBits);
	sps->constrained_set1_flag                  = read_u_1  (   "SPS: constrained_set1_flag"                 , s, &UsedBits);
	sps->constrained_set2_flag                  = read_u_1  (   "SPS: constrained_set2_flag"                 , s, &UsedBits);
	sps->constrained_set3_flag                  = read_u_1  (   "SPS: constrained_set3_flag"                 , s, &UsedBits);

	reserved_zero                               = read_u_v  (4, "SPS: reserved_zero_4bits"                   , s, &UsedBits);

	//assert (reserved_zero==0);
	if (reserved_zero != 0)
	{
		printf("Warning, reserved_zero flag not equal to 0. Possibly new constrained_setX flag introduced.\n");
	}

	sps->level_idc                              = read_u_v  (8, "SPS: level_idc"                             , s, &UsedBits);

	sps->seq_parameter_set_id                   = read_ue_v ("SPS: seq_parameter_set_id"                     , s, &UsedBits);

	// Fidelity Range Extensions stuff
	sps->chroma_format_idc = 1;
	sps->bit_depth_luma_minus8   = 0;
	sps->bit_depth_chroma_minus8 = 0;
	sps->lossless_qpprime_flag   = 0;
	sps->separate_colour_plane_flag = 0;

	if((sps->profile_idc==FREXT_HP   ) ||
		(sps->profile_idc==FREXT_Hi10P) ||
		(sps->profile_idc==FREXT_Hi422) ||
		(sps->profile_idc==FREXT_Hi444) ||
		(sps->profile_idc==FREXT_CAVLC444)
		)
	{
		sps->chroma_format_idc                      = read_ue_v ("SPS: chroma_format_idc"                       , s, &UsedBits);

		if(sps->chroma_format_idc == YUV444)
		{
			sps->separate_colour_plane_flag           = read_u_1  ("SPS: separate_colour_plane_flag"              , s, &UsedBits);
		}

		sps->bit_depth_luma_minus8                  = read_ue_v ("SPS: bit_depth_luma_minus8"                   , s, &UsedBits);
		sps->bit_depth_chroma_minus8                = read_ue_v ("SPS: bit_depth_chroma_minus8"                 , s, &UsedBits);
		//checking;
		if((sps->bit_depth_luma_minus8+8 > sizeof(unsigned char  )*8) || (sps->bit_depth_chroma_minus8+8> sizeof(unsigned char  )*8))
			error ("Source picture has higher bit depth than unsigned char   data type. \nPlease recompile with larger data type for imgpel.", 500);

		sps->lossless_qpprime_flag                  = read_u_1  ("SPS: lossless_qpprime_y_zero_flag"            , s, &UsedBits);

		sps->seq_scaling_matrix_present_flag        = read_u_1  (   "SPS: seq_scaling_matrix_present_flag"       , s, &UsedBits);

		if(sps->seq_scaling_matrix_present_flag)
		{
			n_ScalingList = (sps->chroma_format_idc != YUV444) ? 8 : 12;
			for(i=0; i<n_ScalingList; i++)
			{
				sps->seq_scaling_list_present_flag[i]   = read_u_1  (   "SPS: seq_scaling_list_present_flag"         , s, &UsedBits);
				if(sps->seq_scaling_list_present_flag[i])
				{
					if(i<6)
						Scaling_List(sps->ScalingList4x4[i], 16, &sps->UseDefaultScalingMatrix4x4Flag[i], s,UsedBits);
					else
						Scaling_List(sps->ScalingList8x8[i-6], 64, &sps->UseDefaultScalingMatrix8x8Flag[i-6], s,UsedBits);
				}
			}
		}
	}

	sps->log2_max_frame_num_minus4              = read_ue_v ("SPS: log2_max_frame_num_minus4"                , s, &UsedBits);
	sps->pic_order_cnt_type                     = read_ue_v ("SPS: pic_order_cnt_type"                       , s, &UsedBits);

	if (sps->pic_order_cnt_type == 0)
		sps->log2_max_pic_order_cnt_lsb_minus4 = read_ue_v ("SPS: log2_max_pic_order_cnt_lsb_minus4"           , s, &UsedBits);
	else if (sps->pic_order_cnt_type == 1)
	{
		sps->delta_pic_order_always_zero_flag      = read_u_1  ("SPS: delta_pic_order_always_zero_flag"       , s, &UsedBits);
		sps->offset_for_non_ref_pic                = read_se_v ("SPS: offset_for_non_ref_pic"                 , s, &UsedBits);
		sps->offset_for_top_to_bottom_field        = read_se_v ("SPS: offset_for_top_to_bottom_field"         , s, &UsedBits);
		sps->num_ref_frames_in_pic_order_cnt_cycle = read_ue_v ("SPS: num_ref_frames_in_pic_order_cnt_cycle"  , s, &UsedBits);
		for(i=0; i<sps->num_ref_frames_in_pic_order_cnt_cycle; i++)
			sps->offset_for_ref_frame[i]               = read_se_v ("SPS: offset_for_ref_frame[i]"              , s, &UsedBits);
	}
	sps->num_ref_frames                        = read_ue_v ("SPS: num_ref_frames"                         , s, &UsedBits);
	sps->gaps_in_frame_num_value_allowed_flag  = read_u_1  ("SPS: gaps_in_frame_num_value_allowed_flag"   , s, &UsedBits);
	sps->pic_width_in_mbs_minus1               = read_ue_v ("SPS: pic_width_in_mbs_minus1"                , s, &UsedBits);
	sps->pic_height_in_map_units_minus1        = read_ue_v ("SPS: pic_height_in_map_units_minus1"         , s, &UsedBits);
	sps->frame_mbs_only_flag                   = read_u_1  ("SPS: frame_mbs_only_flag"                    , s, &UsedBits);
	if (!sps->frame_mbs_only_flag)
	{
		sps->mb_adaptive_frame_field_flag        = read_u_1  ("SPS: mb_adaptive_frame_field_flag"           , s, &UsedBits);
	}
	sps->direct_8x8_inference_flag             = read_u_1  ("SPS: direct_8x8_inference_flag"              , s, &UsedBits);
	sps->frame_cropping_flag                   = read_u_1  ("SPS: frame_cropping_flag"                    , s, &UsedBits);

	if (sps->frame_cropping_flag)
	{
		sps->frame_crop_left_offset      = read_ue_v ("SPS: frame_crop_left_offset"           , s, &UsedBits);
		sps->frame_crop_right_offset     = read_ue_v ("SPS: frame_crop_right_offset"          , s, &UsedBits);
		sps->frame_crop_top_offset       = read_ue_v ("SPS: frame_crop_top_offset"            , s, &UsedBits);
		sps->frame_crop_bottom_offset    = read_ue_v ("SPS: frame_crop_bottom_offset"         , s, &UsedBits);
	}
	sps->vui_parameters_present_flag           = (int) read_u_1  ("SPS: vui_parameters_present_flag"      , s, &UsedBits);

	ReadVUI(s, sps,UsedBits);

	sps->Valid =1;// TRUE;
	return UsedBits;
}

int InterpretPPS (Bitstream *s, pic_parameter_set_rbsp_t *pps,int&UsedBits,seq_parameter_set_rbsp_t SeqParSet[])
{
	unsigned i;
	unsigned n_ScalingList;
	int chroma_format_idc;
	int NumberBitsPerSliceGroupId;

	assert (s != NULL);
	assert (s->streamBuffer != 0);
	assert (pps != NULL);

	pps->pic_parameter_set_id                  = read_ue_v ("PPS: pic_parameter_set_id"                   , s, &UsedBits);
	pps->seq_parameter_set_id                  = read_ue_v ("PPS: seq_parameter_set_id"                   , s, &UsedBits);
	pps->entropy_coding_mode_flag              = read_u_1  ("PPS: entropy_coding_mode_flag"               , s, &UsedBits);

	//! Note: as per JVT-F078 the following bit is unconditional.  If F078 is not accepted, then
	//! one has to fetch the correct SPS to check whether the bit is present (hopefully there is
	//! no consistency problem :-(
	//! The current encoder code handles this in the same way.  When you change this, don't forget
	//! the encoder!  StW, 12/8/02
	pps->bottom_field_pic_order_in_frame_present_flag                = read_u_1  ("PPS: bottom_field_pic_order_in_frame_present_flag"                 , s, &UsedBits);

	pps->num_slice_groups_minus1               = read_ue_v ("PPS: num_slice_groups_minus1"                , s, &UsedBits);

	// FMO stuff begins here
	if (pps->num_slice_groups_minus1 > 0)
	{
		pps->slice_group_map_type               = read_ue_v ("PPS: slice_group_map_type"                , s, &UsedBits);
		if (pps->slice_group_map_type == 0)
		{
			for (i=0; i<=pps->num_slice_groups_minus1; i++)
				pps->run_length_minus1 [i]                  = read_ue_v ("PPS: run_length_minus1 [i]"              , s, &UsedBits);
		}
		else if (pps->slice_group_map_type == 2)
		{
			for (i=0; i<pps->num_slice_groups_minus1; i++)
			{
				//! JVT-F078: avoid reference of SPS by using ue(v) instead of u(v)
				pps->top_left [i]                          = read_ue_v ("PPS: top_left [i]"                        , s, &UsedBits);
				pps->bottom_right [i]                      = read_ue_v ("PPS: bottom_right [i]"                    , s, &UsedBits);
			}
		}
		else if (pps->slice_group_map_type == 3 ||
			pps->slice_group_map_type == 4 ||
			pps->slice_group_map_type == 5)
		{
			pps->slice_group_change_direction_flag     = read_u_1  ("PPS: slice_group_change_direction_flag"      , s, &UsedBits);
			pps->slice_group_change_rate_minus1        = read_ue_v ("PPS: slice_group_change_rate_minus1"         , s, &UsedBits);
		}
		else if (pps->slice_group_map_type == 6)
		{
			if (pps->num_slice_groups_minus1+1 >4)
				NumberBitsPerSliceGroupId = 3;
			else if (pps->num_slice_groups_minus1+1 > 2)
				NumberBitsPerSliceGroupId = 2;
			else
				NumberBitsPerSliceGroupId = 1;
			pps->pic_size_in_map_units_minus1      = read_ue_v ("PPS: pic_size_in_map_units_minus1"               , s, &UsedBits);
			if ((pps->slice_group_id = (unsigned char  *)calloc (pps->pic_size_in_map_units_minus1+1, 1)) == NULL)
				no_mem_exit ("InterpretPPS: slice_group_id");
			for (i=0; i<=pps->pic_size_in_map_units_minus1; i++)
				pps->slice_group_id[i] = (unsigned char  ) read_u_v (NumberBitsPerSliceGroupId, "slice_group_id[i]", s, &UsedBits);
		}
	}

	// End of FMO stuff

	pps->num_ref_idx_l0_active_minus1          = read_ue_v ("PPS: num_ref_idx_l0_active_minus1"           , s, &UsedBits);
	pps->num_ref_idx_l1_active_minus1          = read_ue_v ("PPS: num_ref_idx_l1_active_minus1"           , s, &UsedBits);
	pps->weighted_pred_flag                    = read_u_1  ("PPS: weighted_pred_flag"                     , s, &UsedBits);
	pps->weighted_bipred_idc                   = read_u_v  ( 2, "PPS: weighted_bipred_idc"                , s, &UsedBits);
	pps->pic_init_qp_minus26                   = read_se_v ("PPS: pic_init_qp_minus26"                    , s, &UsedBits);
	pps->pic_init_qs_minus26                   = read_se_v ("PPS: pic_init_qs_minus26"                    , s, &UsedBits);

	pps->chroma_qp_index_offset                = read_se_v ("PPS: chroma_qp_index_offset"                 , s, &UsedBits);

	pps->deblocking_filter_control_present_flag = read_u_1 ("PPS: deblocking_filter_control_present_flag" , s, &UsedBits);
	pps->constrained_intra_pred_flag           = read_u_1  ("PPS: constrained_intra_pred_flag"            , s, &UsedBits);
	pps->redundant_pic_cnt_present_flag        = read_u_1  ("PPS: redundant_pic_cnt_present_flag"         , s, &UsedBits);

	if(more_rbsp_data(s->streamBuffer, s->frame_bitoffset,s->bitstream_length)) // more_data_in_rbsp()
	{
		//Fidelity Range Extensions Stuff
		pps->transform_8x8_mode_flag           = read_u_1  ("PPS: transform_8x8_mode_flag"                , s, &UsedBits);
		pps->pic_scaling_matrix_present_flag   =  read_u_1  ("PPS: pic_scaling_matrix_present_flag"        , s, &UsedBits);

		if(pps->pic_scaling_matrix_present_flag)
		{
			chroma_format_idc = SeqParSet[pps->seq_parameter_set_id].chroma_format_idc;
			n_ScalingList = 6 + ((chroma_format_idc != YUV444) ? 2 : 6) * pps->transform_8x8_mode_flag;
			for(i=0; i<n_ScalingList; i++)
			{
				pps->pic_scaling_list_present_flag[i]= read_u_1  ("PPS: pic_scaling_list_present_flag"          , s, &UsedBits);

				if(pps->pic_scaling_list_present_flag[i])
				{
					if(i<6)
						Scaling_List(pps->ScalingList4x4[i], 16, &pps->UseDefaultScalingMatrix4x4Flag[i], s,UsedBits);
					else
						Scaling_List(pps->ScalingList8x8[i-6], 64, &pps->UseDefaultScalingMatrix8x8Flag[i-6], s,UsedBits);
				}
			}
		}
		pps->second_chroma_qp_index_offset      = read_se_v ("PPS: second_chroma_qp_index_offset"          , s, &UsedBits);
	}

	pps->Valid =1;// TRUE;
	return UsedBits;
}

void Scaling_List(int *scalingList, int sizeOfScalingList, int *UseDefaultScalingMatrix, Bitstream *s,int &UsedBits)
{
	int j, scanj;
	int delta_scale, lastScale, nextScale;

	lastScale      = 8;
	nextScale      = 8;

	for(j=0; j<sizeOfScalingList; j++)
	{
		scanj = (sizeOfScalingList==16) ? ZZ_SCAN[j]:ZZ_SCAN8[j];

		if(nextScale!=0)
		{
			delta_scale = read_se_v (   "   : delta_sl   "                           , s, &UsedBits);
			nextScale = (lastScale + delta_scale + 256) % 256;
			*UseDefaultScalingMatrix = (int) (scanj==0 && nextScale==0);
		}

		scalingList[scanj] = (nextScale==0) ? lastScale:nextScale;
		lastScale = scalingList[scanj];
	}
}


int FirstPartOfSliceHeader(Bitstream *bitstream, SliceHeader*sliceH,int&UsedBits,
						   seq_parameter_set_rbsp_t* &active_sps,
						   pic_parameter_set_rbsp_t* &active_pps,
						   seq_parameter_set_rbsp_t SeqParSet[],
						   pic_parameter_set_rbsp_t PicParSet[])
{
	int tmp;
	//int pic_parameter_set_id;
	//SliceType slice_type;
	//int first_mb_in_slice;

	assert(sliceH!=NULL);

	//int UsedBits= 0;//bitstream->frame_bitoffset; // was hardcoded to 31 for previous start-code. This is better.

	// Get first_mb_in_slice
	sliceH->first_mb_in_slice=read_ue_v ("SH: first_mb_in_slice", bitstream, &UsedBits);

	tmp = read_ue_v ("SH: slice_type", bitstream, &UsedBits);

	if (tmp > 4) tmp -= 5;

	sliceH->sliceType=tmp;

	sliceH->pic_parameter_set_id=read_ue_v ("SH: pic_parameter_set_id", bitstream, &UsedBits);

	//UseParameterSet(sliceH->pic_parameter_set_id);	

	int PicParsetId = sliceH->pic_parameter_set_id;  
	pic_parameter_set_rbsp_t *pps = &PicParSet[PicParsetId];
	seq_parameter_set_rbsp_t *sps = &SeqParSet[pps->seq_parameter_set_id];

	if (pps->Valid != 1)//TRUE)
		printf ("Trying to use an invalid (uninitialized) Picture Parameter Set with ID %d, expect the unexpected...\n", PicParsetId);

	if (sps->Valid != 1)//TRUE)
		printf ("PicParset %d references an invalid (uninitialized) Sequence Parameter Set with ID %d, expect the unexpected...\n", 
		PicParsetId, (int) pps->seq_parameter_set_id);

	// Set Sequence Parameter Stuff first
	//  printf ("Using Picture Parameter set %d and associated Sequence Parameter Set %d\n", PicParsetId, pps->seq_parameter_set_id);
	if ((int) sps->pic_order_cnt_type < 0 || sps->pic_order_cnt_type > 2)  // != 1
	{
		printf ("invalid sps->pic_order_cnt_type = %d\n", (int) sps->pic_order_cnt_type);
		error ("pic_order_cnt_type != 1", -1000);
	}

	if (sps->pic_order_cnt_type == 1)
	{
		if(sps->num_ref_frames_in_pic_order_cnt_cycle >= MAXnum_ref_frames_in_pic_order_cnt_cycle)
		{
			error("num_ref_frames_in_pic_order_cnt_cycle too large",-1011);
		}
	}
	//p_Vid->dpb_layer_id = currSlice->layer_id;
	//activate_sps(sps);
	if (active_sps != sps)
	{
		active_sps = sps;
	}
	//activate_pps(pps);
	if (active_pps != pps)
	{
		active_pps = pps;
	}

	if( active_sps->separate_colour_plane_flag )
		sliceH->colour_plane_id=read_u_v (2, "SH: colour_plane_id", bitstream, &UsedBits);

	//Rest part of sliceheader

	int val,len;

	sliceH->frame_num=read_u_v (active_sps->log2_max_frame_num_minus4 + 4, "SH: frame_num", bitstream, &UsedBits);

	if (active_sps->frame_mbs_only_flag)
	{
		sliceH->structure = FRAME;
		sliceH->field_pic_flag=0;
		//printf("Structure:FRAME");
	}
	else
	{
		// field_pic_flag   u(1)
		sliceH->field_pic_flag = read_u_1("SH: field_pic_flag", bitstream, &UsedBits);
		if (sliceH->field_pic_flag)
		{
			// bottom_field_flag  u(1)
			sliceH->bottom_field_flag = (unsigned char  ) read_u_1("SH: bottom_field_flag", bitstream, &UsedBits);
			sliceH->structure = sliceH->bottom_field_flag ? BOTTOM_FIELD : TOP_FIELD;
		}
		else
		{
			sliceH->structure = FRAME;
			sliceH->bottom_field_flag = 0;//FALSE;
		}
	}

	sliceH->mb_aff_frame_flag = (active_sps->mb_adaptive_frame_field_flag && (sliceH->field_pic_flag==0));
	//currSlice->mb_aff_frame_flag = p_Vid->mb_aff_frame_flag;

	if (sliceH->structure == FRAME       ) 
		assert (sliceH->field_pic_flag == 0);
	if (sliceH->structure == TOP_FIELD   ) 
		assert (sliceH->field_pic_flag == 1 && (sliceH->bottom_field_flag == 0));//FALSE));
	if (sliceH->structure == BOTTOM_FIELD) 
		assert (sliceH->field_pic_flag == 1 && (sliceH->bottom_field_flag == 1));//TRUE ));

	if (sliceH->idrFlag)
	{
		sliceH->idrPicId = read_ue_v("SH: idr_pic_id", bitstream, &UsedBits);
	}


	if (active_sps->pic_order_cnt_type == 0)
	{
		sliceH->pic_order_cnt_lsb = read_u_v(active_sps->log2_max_pic_order_cnt_lsb_minus4 + 4, "SH: pic_order_cnt_lsb", bitstream, &UsedBits);
		if( active_pps->bottom_field_pic_order_in_frame_present_flag  ==  1 &&!sliceH->field_pic_flag )
			sliceH->delta_pic_order_cnt_bottom = read_se_v("SH: delta_pic_order_cnt_bottom", bitstream, &UsedBits);
		else
			sliceH->delta_pic_order_cnt_bottom = 0;
	}
	if( active_sps->pic_order_cnt_type == 1 )
	{
		if ( !active_sps->delta_pic_order_always_zero_flag )
		{
			sliceH->delta_pic_order_cnt[ 0 ] = read_se_v("SH: delta_pic_order_cnt[0]", bitstream, &UsedBits);
			if( active_pps->bottom_field_pic_order_in_frame_present_flag  ==  1  && !sliceH->field_pic_flag )
				sliceH->delta_pic_order_cnt[ 1 ] = read_se_v("SH: delta_pic_order_cnt[1]", bitstream, &UsedBits);
			else
				sliceH->delta_pic_order_cnt[ 1 ] = 0;  // set to zero if not in stream
		}
		else
		{
			sliceH->delta_pic_order_cnt[ 0 ] = 0;
			sliceH->delta_pic_order_cnt[ 1 ] = 0;
		}
	}

	//! redundant_pic_cnt is missing here
	if (active_pps->redundant_pic_cnt_present_flag)
	{
		sliceH->redundant_pic_cnt = read_ue_v ("SH: redundant_pic_cnt", bitstream, &UsedBits);
	}

	if(sliceH->sliceType == B_SLICE)
	{
		sliceH->direct_spatial_mv_pred_flag = read_u_1 ("SH: direct_spatial_mv_pred_flag", bitstream, &UsedBits);
	}

	// 	currSlice->num_ref_idx_active[LIST_0] = p_Vid->active_pps->num_ref_idx_l0_active_minus1 + 1;
	// 	currSlice->num_ref_idx_active[LIST_1] = p_Vid->active_pps->num_ref_idx_l1_active_minus1 + 1;

	if(sliceH->sliceType==P_SLICE || sliceH->sliceType == SP_SLICE ||sliceH->sliceType==B_SLICE)
	{
		sliceH->num_ref_idx_override_flag = read_u_1 ("SH: num_ref_idx_override_flag", bitstream, &UsedBits);
		if (sliceH->num_ref_idx_override_flag)
		{
			//currSlice->num_ref_idx_active[LIST_0] = 1 + 
			sliceH->num_ref_idx_activel0=1+read_ue_v ("SH: num_ref_idx_l0_active_minus1", bitstream, &UsedBits);

			if(sliceH->sliceType==B_SLICE)
			{
				//currSlice->num_ref_idx_active[LIST_1] = 1 + 
				sliceH->num_ref_idx_activel1=1+read_ue_v ("SH: num_ref_idx_l1_active_minus1", bitstream, &UsedBits);
			}
		}
	}

	ref_pic_list_reordering(bitstream,sliceH,UsedBits);

	// 	currSlice->weighted_pred_flag = (unsigned short) ((currSlice->slice_type == P_SLICE || currSlice->slice_type == SP_SLICE) 
	// 		? p_Vid->active_pps->weighted_pred_flag 
	// 		: (currSlice->slice_type == B_SLICE && p_Vid->active_pps->weighted_bipred_idc == 1));
	// 	currSlice->weighted_bipred_idc = (unsigned short) (currSlice->slice_type == B_SLICE && p_Vid->active_pps->weighted_bipred_idc > 0);

	if ((active_pps->weighted_pred_flag&&(sliceH->sliceType==P_SLICE|| sliceH->sliceType == SP_SLICE))||
		(active_pps->weighted_bipred_idc==1 && (sliceH->sliceType==B_SLICE)))
	{
		pred_weight_table(bitstream,sliceH,UsedBits,active_sps,active_pps);
	}

	if (sliceH->nal_reference_idc)
		dec_ref_pic_marking(bitstream,sliceH,UsedBits);

	if (active_pps->entropy_coding_mode_flag &&sliceH->sliceType!=I_SLICE && sliceH->sliceType!=SI_SLICE)
	{
		sliceH->cabac_init_idc= read_ue_v("SH: cabac_init_idc", bitstream, &UsedBits);
	}


	sliceH->slice_qp_delta = val = read_se_v("SH: slice_qp_delta", bitstream, &UsedBits);
	//currSlice->qp = p_Vid->qp = 26 + p_Vid->active_pps->pic_init_qp_minus26 + val;
	//currSlice->qp = 26 + p_Vid->active_pps->pic_init_qp_minus26 + val;

	// 	if ((currSlice->qp < -p_Vid->bitdepth_luma_qp_scale) || (currSlice->qp > 51))
	// 		error ("slice_qp_delta makes slice_qp_y out of range", 500);

	if(sliceH->sliceType==SP_SLICE || sliceH->sliceType== SI_SLICE)
	{
		if(sliceH->sliceType==SP_SLICE)
		{
			sliceH->sp_for_switch_flag = read_u_1 ("SH: sp_for_switch_flag", bitstream, &UsedBits);
		}
		sliceH->slice_qs_delta = val = read_se_v("SH: slice_qs_delta", bitstream, &UsedBits);
		// 		currSlice->qs = 26 + p_Vid->active_pps->pic_init_qs_minus26 + val;    
		// 		if ((currSlice->qs < 0) || (currSlice->qs > 51))
		// 			error ("slice_qs_delta makes slice_qs_y out of range", 500);
	}


	if (active_pps->deblocking_filter_control_present_flag)
	{
		sliceH->DFDisableIdc = read_ue_v ("SH: disable_deblocking_filter_idc", bitstream, &UsedBits);

		if (sliceH->DFDisableIdc!=1)
		{
			sliceH->DFAlphaC0Offset= (2 * read_se_v("SH: slice_alpha_c0_offset_div2", bitstream, &UsedBits));
			sliceH->DFBetaOffset    = (short) (2 * read_se_v("SH: slice_beta_offset_div2", bitstream, &UsedBits));
		}
	}//444_TEMP_NOTE. the end of change. 08/07/07
	//Ignore the SEs, by default the Loop Filter is Off
	// 		DFDisableIdc =1;
	// 		currSlice->DFAlphaC0Offset = currSlice->DFBetaOffset = 0;
	//}


	if (active_pps->num_slice_groups_minus1>0 && active_pps->slice_group_map_type>=3 &&
		active_pps->slice_group_map_type<=5)
	{
		len = (active_sps->pic_height_in_map_units_minus1+1)*(active_sps->pic_width_in_mbs_minus1+1)/
			(active_pps->slice_group_change_rate_minus1+1);
		if (((active_sps->pic_height_in_map_units_minus1+1)*(active_sps->pic_width_in_mbs_minus1+1))%
			(active_pps->slice_group_change_rate_minus1+1))
			len +=1;

		len = CeilLog2(len+1);

		sliceH->slice_group_change_cycle = read_u_v (len, "SH: slice_group_change_cycle", bitstream, &UsedBits);
	}



	return UsedBits;
}


int ReadVUI(Bitstream *s, seq_parameter_set_rbsp_t *sps,int &UsedBits)
{
	//Bitstream *s = p->bitstream;
	if (sps->vui_parameters_present_flag)
	{
		sps->vui_seq_parameters.aspect_ratio_info_present_flag = read_u_1  ("VUI: aspect_ratio_info_present_flag"   , s, &UsedBits);
		if (sps->vui_seq_parameters.aspect_ratio_info_present_flag)
		{
			sps->vui_seq_parameters.aspect_ratio_idc             = read_u_v  ( 8, "VUI: aspect_ratio_idc"              , s, &UsedBits);
			if (255==sps->vui_seq_parameters.aspect_ratio_idc)
			{
				sps->vui_seq_parameters.sar_width                  = (unsigned short) read_u_v  (16, "VUI: sar_width"                     , s, &UsedBits);
				sps->vui_seq_parameters.sar_height                 = (unsigned short) read_u_v  (16, "VUI: sar_height"                    , s, &UsedBits);
			}
		}

		sps->vui_seq_parameters.overscan_info_present_flag     = read_u_1  ("VUI: overscan_info_present_flag"        , s, &UsedBits);
		if (sps->vui_seq_parameters.overscan_info_present_flag)
		{
			sps->vui_seq_parameters.overscan_appropriate_flag    = read_u_1  ("VUI: overscan_appropriate_flag"         , s, &UsedBits);
		}

		sps->vui_seq_parameters.video_signal_type_present_flag = read_u_1  ("VUI: video_signal_type_present_flag"    , s, &UsedBits);
		if (sps->vui_seq_parameters.video_signal_type_present_flag)
		{
			sps->vui_seq_parameters.video_format                    = read_u_v  ( 3,"VUI: video_format"                      , s, &UsedBits);
			sps->vui_seq_parameters.video_full_range_flag           = read_u_1  (   "VUI: video_full_range_flag"             , s, &UsedBits);
			sps->vui_seq_parameters.colour_description_present_flag = read_u_1  (   "VUI: color_description_present_flag"    , s, &UsedBits);
			if(sps->vui_seq_parameters.colour_description_present_flag)
			{
				sps->vui_seq_parameters.colour_primaries              = read_u_v  ( 8,"VUI: colour_primaries"                  , s, &UsedBits);
				sps->vui_seq_parameters.transfer_characteristics      = read_u_v  ( 8,"VUI: transfer_characteristics"          , s, &UsedBits);
				sps->vui_seq_parameters.matrix_coefficients           = read_u_v  ( 8,"VUI: matrix_coefficients"               , s, &UsedBits);
			}
		}
		sps->vui_seq_parameters.chroma_location_info_present_flag = read_u_1  (   "VUI: chroma_loc_info_present_flag"      , s, &UsedBits);
		if(sps->vui_seq_parameters.chroma_location_info_present_flag)
		{
			sps->vui_seq_parameters.chroma_sample_loc_type_top_field     = read_ue_v  ( "VUI: chroma_sample_loc_type_top_field"    , s, &UsedBits);
			sps->vui_seq_parameters.chroma_sample_loc_type_bottom_field  = read_ue_v  ( "VUI: chroma_sample_loc_type_bottom_field" , s, &UsedBits);
		}
		sps->vui_seq_parameters.timing_info_present_flag          = read_u_1  ("VUI: timing_info_present_flag"           , s, &UsedBits);
		if (sps->vui_seq_parameters.timing_info_present_flag)
		{
			sps->vui_seq_parameters.num_units_in_tick               = read_u_v  (32,"VUI: num_units_in_tick"               , s, &UsedBits);
			sps->vui_seq_parameters.time_scale                      = read_u_v  (32,"VUI: time_scale"                      , s, &UsedBits);
			sps->vui_seq_parameters.fixed_frame_rate_flag           = read_u_1  (   "VUI: fixed_frame_rate_flag"           , s, &UsedBits);
		}
		sps->vui_seq_parameters.nal_hrd_parameters_present_flag   = read_u_1  ("VUI: nal_hrd_parameters_present_flag"    , s, &UsedBits);
		if (sps->vui_seq_parameters.nal_hrd_parameters_present_flag)
		{
			ReadHRDParameters(s, &(sps->vui_seq_parameters.nal_hrd_parameters),UsedBits);
		}
		sps->vui_seq_parameters.vcl_hrd_parameters_present_flag   = read_u_1  ("VUI: vcl_hrd_parameters_present_flag"    , s, &UsedBits);
		if (sps->vui_seq_parameters.vcl_hrd_parameters_present_flag)
		{
			ReadHRDParameters(s, &(sps->vui_seq_parameters.vcl_hrd_parameters),UsedBits);
		}
		if (sps->vui_seq_parameters.nal_hrd_parameters_present_flag || sps->vui_seq_parameters.vcl_hrd_parameters_present_flag)
		{
			sps->vui_seq_parameters.low_delay_hrd_flag             =  read_u_1  ("VUI: low_delay_hrd_flag"                 , s, &UsedBits);
		}
		sps->vui_seq_parameters.pic_struct_present_flag          =  read_u_1  ("VUI: pic_struct_present_flag   "         , s, &UsedBits);
		sps->vui_seq_parameters.bitstream_restriction_flag       =  read_u_1  ("VUI: bitstream_restriction_flag"         , s, &UsedBits);
		if (sps->vui_seq_parameters.bitstream_restriction_flag)
		{
			sps->vui_seq_parameters.motion_vectors_over_pic_boundaries_flag =  read_u_1  ("VUI: motion_vectors_over_pic_boundaries_flag", s, &UsedBits);
			sps->vui_seq_parameters.max_bytes_per_pic_denom                 =  read_ue_v ("VUI: max_bytes_per_pic_denom"                , s, &UsedBits);
			sps->vui_seq_parameters.max_bits_per_mb_denom                   =  read_ue_v ("VUI: max_bits_per_mb_denom"                  , s, &UsedBits);
			sps->vui_seq_parameters.log2_max_mv_length_horizontal           =  read_ue_v ("VUI: log2_max_mv_length_horizontal"          , s, &UsedBits);
			sps->vui_seq_parameters.log2_max_mv_length_vertical             =  read_ue_v ("VUI: log2_max_mv_length_vertical"            , s, &UsedBits);
			sps->vui_seq_parameters.num_reorder_frames                      =  read_ue_v ("VUI: num_reorder_frames"                     , s, &UsedBits);
			sps->vui_seq_parameters.max_dec_frame_buffering                 =  read_ue_v ("VUI: max_dec_frame_buffering"                , s, &UsedBits);
		}
	}

	return 0;
}

int ReadHRDParameters(Bitstream *s , hrd_parameters_t *hrd,int &UsedBits)
{
	unsigned int SchedSelIdx;

	hrd->cpb_cnt_minus1                                      = read_ue_v (   "VUI: cpb_cnt_minus1"                       , s, &UsedBits);
	hrd->bit_rate_scale                                      = read_u_v  ( 4,"VUI: bit_rate_scale"                       , s, &UsedBits);
	hrd->cpb_size_scale                                      = read_u_v  ( 4,"VUI: cpb_size_scale"                       , s, &UsedBits);

	for( SchedSelIdx = 0; SchedSelIdx <= hrd->cpb_cnt_minus1; SchedSelIdx++ )
	{
		hrd->bit_rate_value_minus1[ SchedSelIdx ]             = read_ue_v  ( "VUI: bit_rate_value_minus1"                  , s, &UsedBits);
		hrd->cpb_size_value_minus1[ SchedSelIdx ]             = read_ue_v  ( "VUI: cpb_size_value_minus1"                  , s, &UsedBits);
		hrd->cbr_flag[ SchedSelIdx ]                          = read_u_1   ( "VUI: cbr_flag"                               , s, &UsedBits);
	}

	hrd->initial_cpb_removal_delay_length_minus1            = read_u_v  ( 5,"VUI: initial_cpb_removal_delay_length_minus1" , s, &UsedBits);
	hrd->cpb_removal_delay_length_minus1                    = read_u_v  ( 5,"VUI: cpb_removal_delay_length_minus1"         , s, &UsedBits);
	hrd->dpb_output_delay_length_minus1                     = read_u_v  ( 5,"VUI: dpb_output_delay_length_minus1"          , s, &UsedBits);
	hrd->time_offset_length                                 = read_u_v  ( 5,"VUI: time_offset_length"          , s, &UsedBits);

	return 0;
}





void ref_pic_list_reordering(Bitstream *currStream,SliceHeader *sliceH,int &UsedBits)
{
	// 	VideoParameters *p_Vid = currSlice->p_Vid;
	// 	unsigned char   dP_nr = assignSE2partition[currSlice->dp_mode][SE_HEADER];
	// 	DataPartition *partition = &(currSlice->partArr[dP_nr]);
	// 	Bitstream *currStream = partition->bitstream;
	int i;//, val;
	int ref_pic_list_reordering_flag_l0,ref_pic_list_reordering_flag_l1;
	int reordering_of_pic_nums_idc;
	int abs_diff_pic_num_minus1;
	int long_term_pic_idx;

	//alloc_ref_pic_list_reordering_buffer(currSlice);

	if (sliceH->sliceType!=I_SLICE && sliceH->sliceType!=SI_SLICE)
	{
		//	val = currSlice->ref_pic_list_reordering_flag[LIST_0] = 
		ref_pic_list_reordering_flag_l0=read_u_1 ("SH: ref_pic_list_reordering_flag_l0", currStream, &UsedBits);

		if (ref_pic_list_reordering_flag_l0)
		{
			i=0;
			do
			{
				//val = currSlice->reordering_of_pic_nums_idc[LIST_0][i] = 
				reordering_of_pic_nums_idc=read_ue_v("SH: reordering_of_pic_nums_idc_l0", currStream, &UsedBits);
				if (reordering_of_pic_nums_idc==0 || reordering_of_pic_nums_idc==1)
				{
					abs_diff_pic_num_minus1= read_ue_v("SH: abs_diff_pic_num_minus1_l0", currStream, &UsedBits);
				}
				else
				{
					if (reordering_of_pic_nums_idc==2)
					{
						long_term_pic_idx = read_ue_v("SH: long_term_pic_idx_l0", currStream, &UsedBits);
					}
				}
				i++;
				// assert (i>currSlice->num_ref_idx_active[LIST_0]);
			} while (reordering_of_pic_nums_idc != 3);
		}
	}

	if (sliceH->sliceType==B_SLICE)
	{
		//val = currSlice->ref_pic_list_reordering_flag[LIST_1] = 
		ref_pic_list_reordering_flag_l1=read_u_1 ("SH: ref_pic_list_reordering_flag_l1", currStream, &UsedBits);

		if (ref_pic_list_reordering_flag_l1)
		{
			i=0;
			do
			{
				//val = currSlice->reordering_of_pic_nums_idc[LIST_1][i] = 
				reordering_of_pic_nums_idc=read_ue_v("SH: reordering_of_pic_nums_idc_l1", currStream, &UsedBits);
				if (reordering_of_pic_nums_idc==0 || reordering_of_pic_nums_idc==1)
				{
					//currSlice->abs_diff_pic_num_minus1[LIST_1][i] = 
					abs_diff_pic_num_minus1=read_ue_v("SH: abs_diff_pic_num_minus1_l1", currStream, &UsedBits);
				}
				else
				{
					if (reordering_of_pic_nums_idc==2)
					{
						//currSlice->long_term_pic_idx[LIST_1][i] = 
						long_term_pic_idx=read_ue_v("SH: long_term_pic_idx_l1", currStream, &UsedBits);
					}
				}
				i++;
				// assert (i>currSlice->num_ref_idx_active[LIST_1]);
			} while (reordering_of_pic_nums_idc != 3);
		}
	}


}

void pred_weight_table(Bitstream *currStream,SliceHeader*sliceH,int &UsedBits,
					   seq_parameter_set_rbsp_t*active_sps,
					   pic_parameter_set_rbsp_t*active_pps)
{

	int i,j;


	sliceH->luma_log2_weight_denom = (unsigned short) read_ue_v ("SH: luma_log2_weight_denom", currStream, &UsedBits);
	//currSlice->wp_round_luma = currSlice->luma_log2_weight_denom ? 1<<(currSlice->luma_log2_weight_denom - 1): 0;

	if ( 0 != active_sps->chroma_format_idc)
	{
		sliceH->chroma_log2_weight_denom = (unsigned short) read_ue_v ("SH: chroma_log2_weight_denom", currStream, &UsedBits);
		//currSlice->wp_round_chroma = currSlice->chroma_log2_weight_denom ? 1<<(currSlice->chroma_log2_weight_denom - 1): 0;
	}

	//reset_wp_params(currSlice);

	for (i=0; i<sliceH->num_ref_idx_activel0; i++)
	{
		sliceH->luma_weight_flag_l0 = read_u_1("SH: luma_weight_flag_l0", currStream, &UsedBits);

		if (sliceH->luma_weight_flag_l0)
		{
			sliceH->luma_weight_l0= read_se_v ("SH: luma_weight_l0", currStream, &UsedBits);
			sliceH->luma_offset_l0= read_se_v ("SH: luma_offset_l0", currStream, &UsedBits);
			//currSlice->wp_offset[0][i][0] = currSlice->wp_offset[0][i][0]<<(p_Vid->bitdepth_luma - 8);
		}

		if (active_sps->chroma_format_idc != 0)
		{
			sliceH->chroma_weight_flag_l0 = read_u_1 ("SH: chroma_weight_flag_l0", currStream, &UsedBits);

			for (j=1; j<3; j++)
			{
				if (sliceH->chroma_weight_flag_l0)
				{
					sliceH->chroma_weight_l0= read_se_v("SH: chroma_weight_l0", currStream, &UsedBits);
					sliceH->chroma_offset_l0= read_se_v("SH: chroma_offset_l0", currStream, &UsedBits);
					//	currSlice->wp_offset[0][i][j] = currSlice->wp_offset[0][i][j]<<(p_Vid->bitdepth_chroma-8);
				}
				// 				else
				// 				{
				// 					currSlice->wp_weight[0][i][j] = 1<<currSlice->chroma_log2_weight_denom;
				// 					currSlice->wp_offset[0][i][j] = 0;
				// 				}
			}
		}
	}
	if ((sliceH->sliceType == B_SLICE) && active_pps->weighted_bipred_idc == 1)
	{
		for (i=0; i<sliceH->num_ref_idx_activel1; i++)
		{
			sliceH->luma_weight_flag_l1 = read_u_1("SH: luma_weight_flag_l1", currStream, &UsedBits);

			if (sliceH->luma_weight_flag_l1)
			{
				sliceH->luma_weight_l1= read_se_v ("SH: luma_weight_l1", currStream, &UsedBits);
				sliceH->luma_offset_l1= read_se_v ("SH: luma_offset_l1", currStream, &UsedBits);
				//currSlice->wp_offset[1][i][0] = currSlice->wp_offset[1][i][0]<<(p_Vid->bitdepth_luma-8);
			}

			if (active_sps->chroma_format_idc != 0)
			{
				sliceH->chroma_weight_flag_l1 = read_u_1 ("SH: chroma_weight_flag_l1", currStream, &UsedBits);

				for (j=1; j<3; j++)
				{
					if (sliceH->chroma_weight_flag_l1)
					{
						sliceH->chroma_weight_l1= read_se_v("SH: chroma_weight_l1", currStream, &UsedBits);
						sliceH->chroma_offset_l1= read_se_v("SH: chroma_offset_l1", currStream, &UsedBits);
						//currSlice->wp_offset[1][i][j] = currSlice->wp_offset[1][i][j]<<(p_Vid->bitdepth_chroma-8);
					}

				}
			}
		}
	}
}


void dec_ref_pic_marking(Bitstream *currStream,SliceHeader *sliceH,int &UsedBits)//, Slice *pSlice)
{
	int val;
	int no_output_of_prior_pics_flag;
	int long_term_reference_flag ;
	int adaptive_ref_pic_buffering_flag;
	int memory_management_control_operation;
	int difference_of_pic_nums_minus1;
	int long_term_pic_num;
	int long_term_frame_idx;
	int max_long_term_frame_idx_plus1;



	if (sliceH->idrFlag)
	{
		no_output_of_prior_pics_flag = read_u_1("SH: no_output_of_prior_pics_flag", currStream, &UsedBits);
		//p_Vid->no_output_of_prior_pics_flag = pSlice->no_output_of_prior_pics_flag;
		long_term_reference_flag = read_u_1("SH: long_term_reference_flag", currStream, &UsedBits);
	}
	else
	{
		adaptive_ref_pic_buffering_flag = read_u_1("SH: adaptive_ref_pic_buffering_flag", currStream, &UsedBits);
		if (adaptive_ref_pic_buffering_flag)
		{
			// read Memory Management Control Operation
			do
			{
				// 				tmp_drpm=(DecRefPicMarking_t*)calloc (1,sizeof (DecRefPicMarking_t));
				// 				tmp_drpm->Next=NULL;

				val = memory_management_control_operation = read_ue_v("SH: memory_management_control_operation", currStream, &UsedBits);

				if ((memory_management_control_operation==1)||
					(memory_management_control_operation==3))
				{
					difference_of_pic_nums_minus1 = read_ue_v("SH: difference_of_pic_nums_minus1", currStream, &UsedBits);
				}
				if (memory_management_control_operation==2)
				{
					long_term_pic_num = read_ue_v("SH: long_term_pic_num", currStream, &UsedBits);
				}

				if ((memory_management_control_operation==3)||
					(memory_management_control_operation==6))
				{
					long_term_frame_idx = read_ue_v("SH: long_term_frame_idx", currStream, &UsedBits);
				}
				if (memory_management_control_operation==4)
				{
					max_long_term_frame_idx_plus1 = read_ue_v("SH: max_long_term_pic_idx_plus1", currStream, &UsedBits);
				}

			}
			while (memory_management_control_operation != 0);
		}
	}
}


int GetRBSP(NALU_t*nalu_t)
{
	int ret;

	ret = NALU2RBSP(nalu_t);

	if (ret < 0)		
	{
		error("Invalid startcode emulation prevention found.\n", 602);
	}

	// Got a NALU
	if (nalu_t->forbidden_bit)
	{
		error("Found NALU with forbidden_bit set, bit error?\n", 603);
	}
	return nalu_t->len;

}
int NALU2RBSP(NALU_t *nalu)
{
	assert (nalu != NULL);

	nalu->len = EBSP2RBSP (nalu->buf, nalu->len, 1) ;

	return nalu->len ;
}
int EBSP2RBSP(unsigned char   *streamBuffer, int end_bytepos, int begin_bytepos)
{
	int i, j, count;
	count = 0;

	if(end_bytepos < begin_bytepos)
		return end_bytepos;

	j = begin_bytepos;

	for(i = begin_bytepos; i < end_bytepos; ++i)
	{ //starting from begin_bytepos to avoid header information
		//in NAL unit, 0x000000, 0x000001 or 0x000002 shall not occur at any unsigned char  -aligned position
		if(count == ZEROBYTES_SHORTSTARTCODE && streamBuffer[i] < 0x03) 
			return -1;
		if(count == ZEROBYTES_SHORTSTARTCODE && streamBuffer[i] == 0x03)
		{
			//check the 4th unsigned char   after 0x000003, except when cabac_zero_word is used, in which case the last three bytes of this NAL unit must be 0x000003
			if((i < end_bytepos-1) && (streamBuffer[i+1] > 0x03))
				return -1;
			//if cabac_zero_word is used, the final unsigned char   of this NAL unit(0x03) is discarded, and the last two bytes of RBSP must be 0x0000
			if(i == end_bytepos-1)
				return j;

			++i;
			count = 0;
		}
		streamBuffer[j] = streamBuffer[i];
		if(streamBuffer[i] == 0x00)
			++count;
		else
			count = 0;
		++j;
	}

	return j;
}


int RBSP2SODB(unsigned char   *streamBuffer, int last_byte_pos)
{
	int ctr_bit, bitoffset;

	bitoffset = 0;
	//find trailing 1
	ctr_bit = (streamBuffer[last_byte_pos-1] & (0x01<<bitoffset));   // set up control bit

	while (ctr_bit==0)
	{                 // find trailing 1 bit
		++bitoffset;
		if(bitoffset == 8)
		{
			if(last_byte_pos == 0)
				printf(" Panic: All zero data sequence in RBSP \n");
			assert(last_byte_pos != 0);
			--last_byte_pos;
			bitoffset = 0;
		}
		ctr_bit= streamBuffer[last_byte_pos - 1] & (0x01<<(bitoffset));
	}

	return(last_byte_pos);

}

seq_parameter_set_rbsp_t *AllocSPS ()
{
	seq_parameter_set_rbsp_t *p;

	if ((p=(seq_parameter_set_rbsp_t *)calloc (1, sizeof (seq_parameter_set_rbsp_t))) == NULL)
		no_mem_exit ("AllocSPS: SPS");
	return p;
}

pic_parameter_set_rbsp_t *AllocPPS ()
{
	pic_parameter_set_rbsp_t *p;

	if ((p=(pic_parameter_set_rbsp_t *)calloc (1, sizeof (pic_parameter_set_rbsp_t))) == NULL)
		no_mem_exit ("AllocPPS: PPS");
	p->slice_group_id = NULL;
	return p;
}

void FreePPS (pic_parameter_set_rbsp_t *pps)
{
	assert (pps != NULL);
	if (pps->slice_group_id != NULL) 
		free (pps->slice_group_id);
	free (pps);
}

void FreeSPS (seq_parameter_set_rbsp_t *sps)
{
	assert (sps != NULL);
	free (sps);
}

Bitstream *AllocBitStream()
{
	Bitstream *bitstream=(Bitstream *) calloc(1, sizeof(Bitstream));
	if (bitstream == NULL)
	{
		//snprintf(errortext, ET_SIZE, "AllocPartition: Memory allocation for Bitstream failed");
		error("AllocPartition: Memory allocation for Bitstream failed", 100);
	}
	bitstream->streamBuffer = (unsigned char   *) calloc(MAX_CODED_FRAME_SIZE, sizeof(unsigned char  ));
	if (bitstream->streamBuffer == NULL)
	{
		//snprintf(errortext, ET_SIZE, "AllocPartition: Memory allocation for streamBuffer failed");
		error("AllocPartition: Memory allocation for streamBuffer failed", 100);
	}
	return bitstream;
}
void FreeBitStream(Bitstream*bitstream)
{
	if(bitstream!=NULL&&bitstream->streamBuffer!=NULL)
	{
		free (bitstream->streamBuffer);
		free (bitstream);
	}

}

int sps_is_equal(seq_parameter_set_rbsp_t *sps1, seq_parameter_set_rbsp_t *sps2)
{
	unsigned i;
	int equal = 1;

	if ((!sps1->Valid) || (!sps2->Valid))
		return 0;

	equal &= (sps1->profile_idc == sps2->profile_idc);
	equal &= (sps1->constrained_set0_flag == sps2->constrained_set0_flag);
	equal &= (sps1->constrained_set1_flag == sps2->constrained_set1_flag);
	equal &= (sps1->constrained_set2_flag == sps2->constrained_set2_flag);
	equal &= (sps1->level_idc == sps2->level_idc);
	equal &= (sps1->seq_parameter_set_id == sps2->seq_parameter_set_id);
	equal &= (sps1->log2_max_frame_num_minus4 == sps2->log2_max_frame_num_minus4);
	equal &= (sps1->pic_order_cnt_type == sps2->pic_order_cnt_type);

	if (!equal) return equal;

	if( sps1->pic_order_cnt_type == 0 )
	{
		equal &= (sps1->log2_max_pic_order_cnt_lsb_minus4 == sps2->log2_max_pic_order_cnt_lsb_minus4);
	}

	else if( sps1->pic_order_cnt_type == 1 )
	{
		equal &= (sps1->delta_pic_order_always_zero_flag == sps2->delta_pic_order_always_zero_flag);
		equal &= (sps1->offset_for_non_ref_pic == sps2->offset_for_non_ref_pic);
		equal &= (sps1->offset_for_top_to_bottom_field == sps2->offset_for_top_to_bottom_field);
		equal &= (sps1->num_ref_frames_in_pic_order_cnt_cycle == sps2->num_ref_frames_in_pic_order_cnt_cycle);
		if (!equal) return equal;

		for ( i = 0 ; i< sps1->num_ref_frames_in_pic_order_cnt_cycle ;i ++)
			equal &= (sps1->offset_for_ref_frame[i] == sps2->offset_for_ref_frame[i]);
	}

	equal &= (sps1->num_ref_frames == sps2->num_ref_frames);
	equal &= (sps1->gaps_in_frame_num_value_allowed_flag == sps2->gaps_in_frame_num_value_allowed_flag);
	equal &= (sps1->pic_width_in_mbs_minus1 == sps2->pic_width_in_mbs_minus1);
	equal &= (sps1->pic_height_in_map_units_minus1 == sps2->pic_height_in_map_units_minus1);
	equal &= (sps1->frame_mbs_only_flag == sps2->frame_mbs_only_flag);

	if (!equal) return equal;
	if( !sps1->frame_mbs_only_flag )
		equal &= (sps1->mb_adaptive_frame_field_flag == sps2->mb_adaptive_frame_field_flag);

	equal &= (sps1->direct_8x8_inference_flag == sps2->direct_8x8_inference_flag);
	equal &= (sps1->frame_cropping_flag == sps2->frame_cropping_flag);
	if (!equal) return equal;
	if (sps1->frame_cropping_flag)
	{
		equal &= (sps1->frame_crop_left_offset == sps2->frame_crop_left_offset);
		equal &= (sps1->frame_crop_right_offset == sps2->frame_crop_right_offset);
		equal &= (sps1->frame_crop_top_offset == sps2->frame_crop_top_offset);
		equal &= (sps1->frame_crop_bottom_offset == sps2->frame_crop_bottom_offset);
	}
	equal &= (sps1->vui_parameters_present_flag == sps2->vui_parameters_present_flag);

	return equal;
}

int pps_is_equal(pic_parameter_set_rbsp_t *pps1, pic_parameter_set_rbsp_t *pps2)
{
	unsigned i, j;
	int equal = 1;

	if ((!pps1->Valid) || (!pps2->Valid))
		return 0;

	equal &= (pps1->pic_parameter_set_id == pps2->pic_parameter_set_id);
	equal &= (pps1->seq_parameter_set_id == pps2->seq_parameter_set_id);
	equal &= (pps1->entropy_coding_mode_flag == pps2->entropy_coding_mode_flag);
	equal &= (pps1->bottom_field_pic_order_in_frame_present_flag == pps2->bottom_field_pic_order_in_frame_present_flag);
	equal &= (pps1->num_slice_groups_minus1 == pps2->num_slice_groups_minus1);

	if (!equal) return equal;

	if (pps1->num_slice_groups_minus1>0)
	{
		equal &= (pps1->slice_group_map_type == pps2->slice_group_map_type);
		if (!equal) return equal;
		if (pps1->slice_group_map_type == 0)
		{
			for (i=0; i<=pps1->num_slice_groups_minus1; i++)
				equal &= (pps1->run_length_minus1[i] == pps2->run_length_minus1[i]);
		}
		else if( pps1->slice_group_map_type == 2 )
		{
			for (i=0; i<pps1->num_slice_groups_minus1; i++)
			{
				equal &= (pps1->top_left[i] == pps2->top_left[i]);
				equal &= (pps1->bottom_right[i] == pps2->bottom_right[i]);
			}
		}
		else if( pps1->slice_group_map_type == 3 || pps1->slice_group_map_type==4 || pps1->slice_group_map_type==5 )
		{
			equal &= (pps1->slice_group_change_direction_flag == pps2->slice_group_change_direction_flag);
			equal &= (pps1->slice_group_change_rate_minus1 == pps2->slice_group_change_rate_minus1);
		}
		else if( pps1->slice_group_map_type == 6 )
		{
			equal &= (pps1->pic_size_in_map_units_minus1 == pps2->pic_size_in_map_units_minus1);
			if (!equal) return equal;
			for (i=0; i<=pps1->pic_size_in_map_units_minus1; i++)
				equal &= (pps1->slice_group_id[i] == pps2->slice_group_id[i]);
		}
	}

	equal &= (pps1->num_ref_idx_l0_active_minus1 == pps2->num_ref_idx_l0_active_minus1);
	equal &= (pps1->num_ref_idx_l1_active_minus1 == pps2->num_ref_idx_l1_active_minus1);
	equal &= (pps1->weighted_pred_flag == pps2->weighted_pred_flag);
	equal &= (pps1->weighted_bipred_idc == pps2->weighted_bipred_idc);
	equal &= (pps1->pic_init_qp_minus26 == pps2->pic_init_qp_minus26);
	equal &= (pps1->pic_init_qs_minus26 == pps2->pic_init_qs_minus26);
	equal &= (pps1->chroma_qp_index_offset == pps2->chroma_qp_index_offset);
	equal &= (pps1->deblocking_filter_control_present_flag == pps2->deblocking_filter_control_present_flag);
	equal &= (pps1->constrained_intra_pred_flag == pps2->constrained_intra_pred_flag);
	equal &= (pps1->redundant_pic_cnt_present_flag == pps2->redundant_pic_cnt_present_flag);

	if (!equal) return equal;

	//Fidelity Range Extensions Stuff
	//It is initialized to zero, so should be ok to check all the time.
	equal &= (pps1->transform_8x8_mode_flag == pps2->transform_8x8_mode_flag);
	equal &= (pps1->pic_scaling_matrix_present_flag == pps2->pic_scaling_matrix_present_flag);
	if(pps1->pic_scaling_matrix_present_flag)
	{
		for(i = 0; i < (6 + ((unsigned)pps1->transform_8x8_mode_flag << 1)); i++)
		{
			equal &= (pps1->pic_scaling_list_present_flag[i] == pps2->pic_scaling_list_present_flag[i]);
			if(pps1->pic_scaling_list_present_flag[i])
			{
				if(i < 6)
				{
					for (j = 0; j < 16; j++)
						equal &= (pps1->ScalingList4x4[i][j] == pps2->ScalingList4x4[i][j]);
				}
				else
				{
					for (j = 0; j < 64; j++)
						equal &= (pps1->ScalingList8x8[i-6][j] == pps2->ScalingList8x8[i-6][j]);
				}
			}
		}
	}
	equal &= (pps1->second_chroma_qp_index_offset == pps2->second_chroma_qp_index_offset);

	return equal;
}

unsigned CeilLog2( unsigned uiVal)
{
	unsigned uiTmp = uiVal-1;
	unsigned uiRet = 0;

	while( uiTmp != 0 )
	{
		uiTmp >>= 1;
		uiRet++;
	}
	return uiRet;
}


NALU_t *AllocNALU(int buffersize)
{
	NALU_t *n;

	if ((n = (NALU_t*)calloc (1, sizeof (NALU_t))) == NULL)
		no_mem_exit ("AllocNALU: n");

	n->max_size=buffersize;

	if ((n->buf = (unsigned char  *)calloc (buffersize, sizeof (unsigned char  ))) == NULL)
	{
		free (n);
		no_mem_exit ("AllocNALU: n->buf");
	}

	return n;
}
void FreeNALU(NALU_t *n)
{
	if (n != NULL)
	{
		if (n->buf != NULL)
		{
			free(n->buf);
			n->buf=NULL;
		}
		free (n);
	}
}

int FindStartCode_h264 (unsigned char *Buf, int zeros_in_startcode)
{
	int i;

	for (i = 0; i < zeros_in_startcode; i++)
	{
		if(*(Buf++) != 0)
		{
			return 0;
		}
	}

	if(*Buf != 1)
		return 0;

	return 1;
}
SliceHeader*AllocSH()
{
	SliceHeader*sliceH=(SliceHeader *) calloc(1, sizeof(SliceHeader));
	return sliceH;

}
void FreeSH(SliceHeader*sliceH)
{
	if(sliceH!=NULL)
		free(sliceH);

}