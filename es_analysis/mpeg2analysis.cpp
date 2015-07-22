#include "mpeg2analysis.h"

unsigned int mPackStartCode(MPack*mpk)
{
	unsigned int code=0;
	int i=1;
	unsigned char*p=mpk->buf;
	while(i<=4)
	{
		code|=*p<<(32-i*8);
		p++;
		//	mpk->pbfr++;
		i++;
	}
	//mpk->bitOffset+=32;

	return code;
}

unsigned int GetBits(int numbits,char*tracestring,MPack*mpk)
{
	unsigned char*buffer=mpk->buf;
	int totbitoffset=mpk->bitOffset;
	int bitcount=(mpk->len<<3)+7;

	mpk->bitOffset+=numbits;

	if ((totbitoffset + numbits ) > bitcount) 
	{
		return -1;
	}
	else
	{
		int bitoffset  = 7 - (totbitoffset & 0x07); // bit from start of unsigned char  
		int byteoffset = (totbitoffset >> 3); // unsigned char   from start of buffer
		int bitcounter = numbits;
		unsigned char *curbyte  = &(buffer[byteoffset]);
		int inf = 0;

		while (numbits--)
		{
			inf <<=1;    
			inf |= ((*curbyte)>> (bitoffset--)) & 0x01;    
			if (bitoffset == -1 ) 
			{ //Move onto next unsigned char   to get all of numbits
				curbyte++;
				bitoffset = 7;
			}
			// Above conditional could also be avoided using the following:
			// curbyte   -= (bitoffset >> 3);
			// bitoffset &= 0x07;
		}

#if TRACE
		printf("%s=%d\n",tracestring,inf);
#endif

		return inf;           // return absolute offset in bit from start of frame
	}
}

unsigned int GetStartCodeBits(MPack*mpk)
{
	int code=GetBits(32,"STARTCODE",mpk);
	return code;
}

/* decode sequence display extension */

void user_data(MPack*mpk)
{
	int startcode;
	startcode                   = GetStartCodeBits(mpk);

	//getStartCode();
}
int FindStartCode (unsigned char *Buf, int zeros_in_startcode)
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

MPack* AllocMpack(int maxSize)
{
	MPack*mpk=(MPack*)malloc(sizeof(MPack));
	if(mpk==NULL)
	{
		printf("Alloc PES ERRor!!!\n");
		exit(-1);
	}
	mpk->buf=(unsigned char*)malloc(sizeof(unsigned char)*maxSize);
	if(mpk->buf==NULL)
	{
		printf("Alloc PES->buf ERRor!!!\n");
		exit(-1);
	}
	//mpk->pbfr=mpk->buf;
	return mpk;
}
void FreeMpack(MPack*mpk)
{
	if(mpk!=NULL&&mpk->buf!=NULL)
	{
		free(mpk->buf);
		free(mpk);
	}
}
