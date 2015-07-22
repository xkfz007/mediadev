#ifndef _MPEG2TYPEDEFS_H_
#define _MPEG2TYPEDEFS_H_
#ifndef _TYPEDEFS_H_
#define _TYPEDEFS_H_
#include <string>
using std::string;


#ifdef MPEG2_DECODER_CPP_DLL_EXPORTS 
#define MPEG2_API __declspec(dllexport)
#else
#define MPEG2_API __declspec(dllimport)
#endif

//#define MAX_CODED_FRAME_SIZE 80000

#define PICTURE_START_CODE      0x100
#define SLICE_START_CODE_MIN    0x101
#define SLICE_START_CODE_MAX    0x1AF
#define USER_DATA_START_CODE    0x1B2
#define SEQUENCE_HEADER_CODE    0x1B3
#define SEQUENCE_ERROR_CODE     0x1B4
#define EXTENSION_START_CODE    0x1B5
#define SEQUENCE_END_CODE       0x1B7
#define GROUP_START_CODE        0x1B8
#define SYSTEM_START_CODE_MIN   0x1B9
#define SYSTEM_START_CODE_MAX   0x1FF

#define ISO_END_CODE            0x1B9
#define PACK_START_CODE         0x1BA
#define SYSTEM_START_CODE       0x1BB

#define VIDEO_ELEMENTARY_STREAM 0x1e0

#define SEQUENCE_EXTENSION_ID                    1
#define SEQUENCE_DISPLAY_EXTENSION_ID            2
#define QUANT_MATRIX_EXTENSION_ID                3
#define COPYRIGHT_EXTENSION_ID                   4
#define SEQUENCE_SCALABLE_EXTENSION_ID           5
#define PICTURE_DISPLAY_EXTENSION_ID             7
#define PICTURE_CODING_EXTENSION_ID              8
#define PICTURE_SPATIAL_SCALABLE_EXTENSION_ID    9
#define PICTURE_TEMPORAL_SCALABLE_EXTENSION_ID  10

/* picture coding type */
#define I_TYPE 1
#define P_TYPE 2
#define B_TYPE 3
#define D_TYPE 4

/* picture structure */
#define TOP_FIELD     1
#define BOTTOM_FIELD  2
#define FRAME_PICTURE 3

/* chroma_format */
#define CHROMA420 1
#define CHROMA422 2
#define CHROMA444 3

#define RESERVED    -1 
#define PROFILE_422   (128+5)
#define MAIN_LEVEL    8



#endif
#endif