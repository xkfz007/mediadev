#ifndef _MPEG2ANALYSIS_H_
#define _MPEG2ANALYSIS_H_
#include "es_defines.h"
unsigned int mPackStartCode(MPack*mpk);
void sequence_header(MPack*mpk);
unsigned int GetBits(int numbits,char*tracestring,MPack*mpk);
unsigned int GetStartCodeBits(MPack*mpk);
void user_data(MPack*mpk);


MPack* AllocMpack(int maxSize);
void FreeMpack(MPack*mpk);
int FindStartCode (unsigned char *Buf, int zeros_in_startcode);


#endif