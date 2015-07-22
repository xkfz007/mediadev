#ifndef _VLC_H_
#define _VLC_H_
#include "h264typedefs.h"
extern int read_se_v (char *tracestring, Bitstream *bitstream, int *used_bits);
extern int read_ue_v (char *tracestring, Bitstream *bitstream, int *used_bits);
extern int read_u_1 (char *tracestring, Bitstream *bitstream, int *used_bits);
extern int read_u_v (int LenInBits, char *tracestring, Bitstream *bitstream, int *used_bits);
//extern int read_i_v (int LenInBits, char *tracestring, Bitstream *bitstream, int *used_bits);

extern void linfo_ue(int len, int info, int *value1, int *dummy);
extern void linfo_se(int len, int info, int *value1, int *dummy);

extern int  readSyntaxElement_VLC (SyntaxElement *sym, Bitstream *currStream);

extern int  GetVLCSymbol (unsigned char   buffer[],int totbitoffset,int *info, int bytecount);

extern int readSyntaxElement_FLC (SyntaxElement *sym, Bitstream *currStream);

extern int GetBits_h264  (unsigned char   buffer[],int totbitoffset,int *info, int bitcount, int numbits);
/*extern int ShowBits (unsigned char   buffer[],int totbitoffset,int bitcount, int numbits);*/
extern int more_rbsp_data (unsigned char   buffer[],int totbitoffset,int bytecount);
#endif