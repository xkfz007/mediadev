#include "vlc.h"
#include <cstdio>

int read_u_v (int LenInBits, char*tracestring, Bitstream *bitstream, int *used_bits)
{
	SyntaxElement symbol;
	symbol.inf = 0;

	//assert (bitstream->streamBuffer != NULL);
//	symbol.type = SE_HEADER;
	symbol.mapping = linfo_ue;   // Mapping rule
	symbol.len = LenInBits;
	//SYMTRACESTRING(tracestring);
	readSyntaxElement_FLC (&symbol, bitstream);
	*used_bits+=symbol.len;
#if TRACE
	printf("%s=%d\n",tracestring,symbol.inf);
#endif

	return symbol.inf;
}
int read_u_1 (char *tracestring, Bitstream *bitstream, int *used_bits)
{
	return (int) read_u_v (1, tracestring, bitstream, used_bits);
}

int read_ue_v (char *tracestring, Bitstream *bitstream, int *used_bits)
{
	SyntaxElement symbol;

	//assert (bitstream->streamBuffer != NULL);
//	symbol.type = SE_HEADER;
	symbol.mapping = linfo_ue;   // Mapping rule
	//	SYMTRACESTRING(tracestring);
	readSyntaxElement_VLC (&symbol, bitstream);
	*used_bits+=symbol.len;
#if TRACE
	printf("%s=%d\n",tracestring,symbol.value1);
#endif

	return symbol.value1;
}

int read_se_v (char *tracestring, Bitstream *bitstream, int *used_bits)
{
	SyntaxElement symbol;

	//assert (bitstream->streamBuffer != NULL);
//	symbol.type = SE_HEADER;
	symbol.mapping = linfo_se;   // Mapping rule: signed integer

	readSyntaxElement_VLC (&symbol, bitstream);
	*used_bits+=symbol.len;

#if TRACE
	printf("%s=%d\n",tracestring,symbol.value1);
#endif

	return symbol.value1;
}

void linfo_ue(int len, int info, int *value1, int *dummy)
{
	//assert ((len >> 1) < 32);
	*value1 = (int) (((unsigned int) 1 << (len >> 1)) + (unsigned int) (info) - 1);
}


void linfo_se(int len,  int info, int *value1, int *dummy)
{
	//assert ((len >> 1) < 32);
	unsigned int n = ((unsigned int) 1 << (len >> 1)) + (unsigned int) info - 1;
	*value1 = (n + 1) >> 1;
	if((n & 0x01) == 0)                           // lsb is signed bit
		*value1 = -*value1;
}

int readSyntaxElement_FLC(SyntaxElement *sym, Bitstream *currStream)
{
	int BitstreamLengthInBits  = (currStream->bitstream_length << 3) + 7;

	if ((GetBits_h264(currStream->streamBuffer, currStream->frame_bitoffset, &(sym->inf), BitstreamLengthInBits, sym->len)) < 0)
		return -1;

	sym->value1 = sym->inf;
	currStream->frame_bitoffset += sym->len; // move bitstream pointer

	return 1;
}

int readSyntaxElement_VLC(SyntaxElement *sym, Bitstream *currStream)
{

	sym->len =  GetVLCSymbol (currStream->streamBuffer, currStream->frame_bitoffset, &(sym->inf), currStream->bitstream_length);
	if (sym->len == -1)
		return -1;

	currStream->frame_bitoffset += sym->len;
	sym->mapping(sym->len, sym->inf, &(sym->value1), &(sym->value2));


	return 1;
}

int GetVLCSymbol (unsigned char   buffer[],int totbitoffset,int *info, int bytecount)
{
	long byteoffset = (totbitoffset >> 3);         // unsigned char   from start of buffer
	int  bitoffset  = (7 - (totbitoffset & 0x07)); // bit from start of unsigned char  
	int  bitcounter = 1;
	int  len        = 0;
	unsigned char   *cur_byte  = &(buffer[byteoffset]);
	int  ctr_bit    = ((*cur_byte) >> (bitoffset)) & 0x01;  // control bit for current bit posision

	while (ctr_bit == 0)
	{                 // find leading 1 bit
		len++;
		bitcounter++;
		bitoffset--;
		bitoffset &= 0x07;
		cur_byte  += (bitoffset == 7);
		byteoffset+= (bitoffset == 7);      
		ctr_bit    = ((*cur_byte) >> (bitoffset)) & 0x01;
	}

	if (byteoffset + ((len + 7) >> 3) > bytecount)
		return -1;
	else
	{
		// make infoword
		int inf = 0;                          // shortest possible code is 1, then info is always 0    

		while (len--)
		{
			bitoffset --;    
			bitoffset &= 0x07;
			cur_byte  += (bitoffset == 7);
			bitcounter++;
			inf <<= 1;    
			inf |= ((*cur_byte) >> (bitoffset)) & 0x01;
		}

		*info = inf;
		return bitcounter;           // return absolute offset in bit from start of frame
	}
}


int GetBits_h264 (unsigned char   buffer[],int totbitoffset,int *info, int bitcount,
			 int numbits)
{
	if ((totbitoffset + numbits ) > bitcount) 
	{
		return -1;
	}
	else
	{
		int bitoffset  = 7 - (totbitoffset & 0x07); // bit from start of unsigned char  
		int byteoffset = (totbitoffset >> 3); // unsigned char   from start of buffer
		int bitcounter = numbits;
		unsigned char   *curbyte  = &(buffer[byteoffset]);
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
		*info = inf;

		return bitcounter;           // return absolute offset in bit from start of frame
	}
}


int more_rbsp_data (unsigned char   buffer[],int totbitoffset,int bytecount)
{
	long byteoffset = (totbitoffset >> 3);      // unsigned char   from start of buffer
	// there is more until we're in the last unsigned char  
	if (byteoffset < (bytecount - 1)) 
		return 1;//TRUE;
	else
	{
		int bitoffset   = (7 - (totbitoffset & 0x07));      // bit from start of unsigned char  
		unsigned char   *cur_byte  = &(buffer[byteoffset]);
		// read one bit
		int ctr_bit     = ctr_bit = ((*cur_byte)>> (bitoffset--)) & 0x01;      // control bit for current bit posision

		//assert (byteoffset<bytecount);       

		// a stop bit has to be one
		if (ctr_bit==0) 
			return 1;//TRUE;  
		else
		{
			int cnt = 0;

			while (bitoffset>=0 && !cnt)
			{
				cnt |= ((*cur_byte)>> (bitoffset--)) & 0x01;   // set up control bit
			}

			return (cnt);
		}
	}
}

