/**
 * 最简单的基于FFmpeg的视频解码器（纯净版）
 * Simplest FFmpeg Decoder Pure
 *
 * 雷霄骅 Lei Xiaohua
 * leixiaohua1020@126.com
 * 中国传媒大学/数字电视技术
 * Communication University of China / Digital TV Technology
 * http://blog.csdn.net/leixiaohua1020
 *
 *
 * 本程序实现了视频码流(支持HEVC，H.264，MPEG2等)解码为YUV数据。
 * 它仅仅使用了libavcodec（而没有使用libavformat）。
 * 是最简单的FFmpeg视频解码方面的教程。
 * 通过学习本例子可以了解FFmpeg的解码流程。
 * This software is a simplest decoder based on FFmpeg.
 * It decode bitstreams to YUV pixel data.
 * It just use libavcodec (do not contains libavformat).
 * Suitable for beginner of FFmpeg.
 */

#include <stdio.h>

#define __STDC_CONSTANT_MACROS

#ifdef _WIN32
//Windows
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
};
#else
//Linux...
#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#ifdef __cplusplus
};
#endif
#endif


//test different codec
#define TEST_H264  0
#define TEST_HEVC  0

int nearest_frame_window[100]={0};
int win_end=0;
int i_total_bits=0;
int i_frame_done=0;

static int get_frame_win_bits(int win_sz) {
  int tmp_bits = 0;
  int j = win_end;
  for(int i = 0; i < win_sz; i++) {
    j--;
    if(j < 0)
      j += win_sz;
    tmp_bits += nearest_frame_window[j];
  }
  return tmp_bits;
}
FILE* pf;

static void calc_bitrate(int bits,int i_fps){
    //calculate bitrate of the nearest 1 second
    nearest_frame_window[win_end % i_fps] = bits;
    win_end = (win_end + 1) % i_fps;
    i_frame_done++;
    int fwin_bits = get_frame_win_bits(i_fps);
    double curr_bitrate_1sec;
    if(i_frame_done<i_fps)
        curr_bitrate_1sec = fwin_bits * 0.001/i_frame_done*i_fps;
    else
        curr_bitrate_1sec = fwin_bits * 0.001;

    i_total_bits+=bits;
    double curr_bitrate=
        i_total_bits*0.001/i_frame_done*i_fps;

    fprintf(pf,"%d: bitrate of 1 second=%.2f gobal bitrate=%.2f\n",
        i_frame_done,curr_bitrate_1sec,curr_bitrate);

}

int main(int argc, char* argv[])
{
	AVCodec *pCodec;
    AVCodecContext *pCodecCtx= NULL;
	AVCodecParserContext *pCodecParserCtx=NULL;

    int frame_count;
    FILE *fp_in;
	FILE *fp_out;
    AVFrame	*pFrame,*pFrameYUV;
	uint8_t *out_buffer;
	const int in_buffer_size=4096;
	uint8_t in_buffer[in_buffer_size + FF_INPUT_BUFFER_PADDING_SIZE]={0};
	uint8_t *cur_ptr;
	int cur_size;

    AVPacket packet;
	int ret, got_picture;
	
	int y_size;

#if TEST_HEVC
	enum AVCodecID codec_id=AV_CODEC_ID_HEVC;
	char filepath_in[]="bigbuckbunny_480x272.hevc";
#elif TEST_H264
	AVCodecID codec_id=AV_CODEC_ID_H264;
//char filepath_in[]="bigbuckbunny_480x272.h264";
    char filepath_in[]="d:/test/pom.flv";
#else
	AVCodecID codec_id=AV_CODEC_ID_MPEG2VIDEO;
	char filepath_in[]="bigbuckbunny_480x272.m2v";
#endif

	char filepath_out[]="bigbuckbunny_480x272.yuv";
	int first_time=1;

    pf=fopen("a.txt","w");
	struct SwsContext *img_convert_ctx;

	//av_log_set_level(AV_LOG_DEBUG);
	
	avcodec_register_all();

    pCodec = avcodec_find_decoder(codec_id);
    if (!pCodec) {
        printf("Codec not found\n");
        return -1;
    }
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (!pCodecCtx){
        printf("Could not allocate video codec context\n");
        return -1;
    }

	pCodecParserCtx=av_parser_init(codec_id);
	if (!pCodecParserCtx){
		printf("Could not allocate video parser context\n");
		return -1;
	}

    //if(pCodec->capabilities&CODEC_CAP_TRUNCATED)
    //    pCodecCtx->flags|= CODEC_FLAG_TRUNCATED; /* we do not send complete frames */
    
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        printf("Could not open codec\n");
        return -1;
    }
	//Input File
    fp_in = fopen(filepath_in, "rb");
    if (!fp_in) {
        printf("Could not open input stream\n");
        return -1;
    }
	//Output File
	fp_out = fopen(filepath_out, "wb");
	if (!fp_out) {
		printf("Could not open output YUV file\n");
		return -1;
	}

    pFrame = av_frame_alloc();
	av_init_packet(&packet);

    int i_fps=25;
    int i_frame_done=0;
    int i_total_bits=0;


	while (1) {
        cur_size = fread(in_buffer, 1, in_buffer_size, fp_in);
        if (cur_size == 0)
            break;
        cur_ptr=in_buffer;

        while (cur_size>0){

			int len = av_parser_parse2(
				pCodecParserCtx, pCodecCtx,
				&packet.data, &packet.size,
				cur_ptr , cur_size ,
				AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);

			cur_ptr += len;
			cur_size -= len;

			if(packet.size==0)
				continue;

			//Some Info from AVCodecParserContext
			printf("Packet Size:%6d\t",packet.size);
			switch(pCodecParserCtx->pict_type){
				case AV_PICTURE_TYPE_I: printf("Type: I\t");break;
				case AV_PICTURE_TYPE_P: printf("Type: P\t");break;
				case AV_PICTURE_TYPE_B: printf("Type: B\t");break;
				default: printf("Type: Other\t");break;
			}
			printf("Output Number:%4d\t",pCodecParserCtx->output_picture_number);
			printf("Offset:%lld\n",pCodecParserCtx->cur_offset);

			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &packet);
			if (ret < 0) {
				printf("Decode Error.\n");
				return ret;
			}
			if (got_picture) {
				if(first_time){
					printf("\nCodec Full Name:%s\n",pCodecCtx->codec->long_name);
					printf("width:%d\nheight:%d\n\n",pCodecCtx->width,pCodecCtx->height);
					//SwsContext
					img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, 
						pCodecCtx->width, pCodecCtx->height, PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL); 
					
					pFrameYUV=av_frame_alloc();
					out_buffer=(uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
					avpicture_fill((AVPicture *)pFrameYUV, out_buffer, PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
					
					y_size=pCodecCtx->width*pCodecCtx->height;

					first_time=0;
				}

				printf("Succeed to decode 1 frame!\n");
				sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, 
					pFrameYUV->data, pFrameYUV->linesize);

				fwrite(pFrameYUV->data[0],1,y_size,fp_out);     //Y 
				fwrite(pFrameYUV->data[1],1,y_size/4,fp_out);   //U
				fwrite(pFrameYUV->data[2],1,y_size/4,fp_out);   //V
			}

            calc_bitrate(packet.size,i_fps);


		}
        if(i_frame_done>100)
            break;

    }

	//Flush Decoder
    packet.data = NULL;
    packet.size = 0;
	while(1){
		ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &packet);
		if (ret < 0) {
			printf("Decode Error.\n");
			return ret;
		}
		if (!got_picture)
			break;
		if (got_picture) {
			printf("Flush Decoder: Succeed to decode 1 frame!\n");
			sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, 
				pFrameYUV->data, pFrameYUV->linesize);

			fwrite(pFrameYUV->data[0],1,y_size,fp_out);     //Y
			fwrite(pFrameYUV->data[1],1,y_size/4,fp_out);   //U
			fwrite(pFrameYUV->data[2],1,y_size/4,fp_out);   //V
		}
        calc_bitrate(packet.size,i_fps);
	}

    fclose(fp_in);
	fclose(fp_out);
    fclose(pf);
    
	sws_freeContext(img_convert_ctx);
	av_parser_close(pCodecParserCtx);

	av_frame_free(&pFrameYUV);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	av_free(pCodecCtx);

	return 0;
}

