#include <cstdio>
#include <cstring>
#include <cmath>
#include <string>
#include "stdint.h"
using namespace std;

#ifdef WIN32
#define fseek _fseeki64
#define ftell _ftelli64
#else
#define fseek fseeko
#define ftell ftello
#endif

#define YUV420 0
#define YUV422 1
#define YUV444 2
#define YUV400 3

typedef unsigned char pixel;

char	*optarg;		// global argument pointer
int		optind = 0; 	// global argv index

int getopt(int argc, char *argv[], char *optstring)
{
    static char *next = NULL;
    if (optind == 0)
        next = NULL;

    optarg = NULL;

    if (next == NULL || *next == '\0')
    {
        if (optind == 0)
            optind++;

        if (optind >= argc || argv[optind][0] != '-' || argv[optind][1] == '\0')
        {
            optarg = NULL;
            if (optind < argc)
                optarg = argv[optind];
            return EOF;
        }

        if (strcmp(argv[optind], "--") == 0)
        {
            optind++;
            optarg = NULL;
            if (optind < argc)
                optarg = argv[optind];
            return EOF;
        }

        next = argv[optind];
        next++;		// skip past -
        optind++;
    }

    char c = *next++;
    char *cp = strchr(optstring, c);

    if (cp == NULL || c == ':')
        return '?';

    cp++;
    if (*cp == ':')
    {
        if (*next != '\0')
        {
            optarg = next;
            next = NULL;
        }
        else if (optind < argc)
        {
            optarg = argv[optind];
            optind++;
        }
        else
        {
            return '?';
        }
    }

    return c;
}

int64_t filesize( FILE *fp )
{
    int64_t save_pos;
    int64_t size_of_file;

    /* Save the current position. */
    save_pos = ftell( fp );

    /* Jump to the end of the file. */
    fseek( fp, 0, SEEK_END );

    /* Get the end position. */
    size_of_file = ftell( fp );

    /* Jump back to the original position. */
    fseek( fp, save_pos, SEEK_SET );

    return( size_of_file );
}
int TIandSI(const string& strPath,int width,int height,//int pixfmt,
            int start,int frame_number,bool b_ti,bool b_si)
{
    if(!b_ti&&!b_si)
        return -1;

    FILE* fYUV;
    string out_filename;

    int nHeight, nWidth;
    int nFrameNum;
    int nYSize;	
    int nUVSize;
    int nFrameSize;
    int end;

    pixel *pFrame;
    pixel **pFrame2D;
    pixel *pNextFrame;
    pixel **pNextFrame2D;
    pixel *NewYBuffer;

    nHeight = height;
    nWidth = width;
    nYSize = nHeight*nWidth;
    //switch (pixfmt) {
    //  case YUV400: nUVSize = 0; break;
    //  case YUV422: nUVSize = nYSize; break;
    //  case YUV444: nUVSize = nYSize*2; break;
    //  case YUV420: nUVSize = nYSize/2; break;
    //}
    nFrameSize = nYSize*3/2;

    pFrame = new pixel [nYSize];
    pFrame2D = new pixel*[nHeight];
    for(int i = 0; i < nHeight; i++)
        pFrame2D[i] = pFrame + i*nWidth;

    pNextFrame = new pixel [nYSize];
    pNextFrame2D = new pixel*[nHeight];
    for(int i = 0; i < nHeight; i++)
        pNextFrame2D[i] = pNextFrame + i*nWidth;

    NewYBuffer = new unsigned char [nYSize];

    fYUV=fopen(strPath.c_str(),"r");
    if(fYUV==NULL)
    {
        printf("cant open file\n");
        return -1;
    }
    if(frame_number==-1){
        end= filesize(fYUV)/nFrameSize;
    }
    else
        end=start+frame_number;

    if(b_si){
        FILE* fout_si;
        double *NewSIBuffer= new double [nYSize];
        out_filename=strPath+".si";
        fout_si=fopen(out_filename.c_str(),"w");
        fprintf(stdout,"Calculating SI:\n");
        for(int k = start;k < end; k++) {
            fseek(fYUV,k*nFrameSize,SEEK_SET);
            fread(pFrame,sizeof(pixel),nYSize,fYUV);

            for(int j = 0; j < nHeight; j++){
                for(int i = 0; i < nWidth; i++) {
                    double XSobel, YSobel, SobelValue;
                    if((j == 0) && (i == 0)) {
                        //(0, 0)
                        XSobel = -3*pFrame2D[0][0] + 3*pFrame2D[0][i+1]				
                        -pFrame2D[j+1][0] + pFrame2D[j+1][i+1];

                        YSobel = 3*pFrame2D[0][0] + pFrame2D[0][i+1]
                        -3*pFrame2D[j+1][0] - pFrame2D[j+1][i+1];
                    }
                    else if((j == 0) && (i == (nWidth - 1))) {
                        //(nWidth - 1, 0)
                        XSobel = -3*pFrame2D[0][i-1] + 3*pFrame2D[0][i]
                        -pFrame2D[j+1][i-1] + pFrame2D[j+1][i];

                        YSobel = pFrame2D[0][i-1] + 3*pFrame2D[0][i]
                        -pFrame2D[j+1][i-1] - 3*pFrame2D[j+1][i];
                    }
                    else if((j == (nHeight - 1)) && (i == 0)) {
                        //(0, nHeight - 1)
                        XSobel = -pFrame2D[j-1][0] + pFrame2D[j-1][i+1]
                        -3*pFrame2D[j][0] + 3*pFrame2D[j][i+1];				

                        YSobel = 3*pFrame2D[j-1][0] + pFrame2D[j-1][i+1]
                        -3*pFrame2D[j][0] - pFrame2D[j][i+1];
                    }
                    else if((j == (nHeight - 1)) && (i == (nWidth - 1))) {
                        //(nWidth - 1, nHeight - 1)
                        XSobel = -pFrame2D[j-1][i-1] + pFrame2D[j-1][i]
                        -3*pFrame2D[j][i-1] + 3*pFrame2D[j][i];				

                        YSobel = pFrame2D[j-1][i-1] + 3*pFrame2D[j-1][i]
                        -pFrame2D[j][i-1] - 3*pFrame2D[j][i];
                    }
                    else if(j == 0) {
                        XSobel = -3*pFrame2D[0][i-1] + 3*pFrame2D[0][i+1]
                        -pFrame2D[j+1][i-1] + pFrame2D[j+1][i+1];

                        YSobel = pFrame2D[0][i-1] + 2*pFrame2D[0][i] + pFrame2D[0][i+1]
                        -pFrame2D[j+1][i-1] - 2*pFrame2D[j+1][i] - pFrame2D[j+1][i+1];
                    }
                    else if(j == (nHeight - 1)) {
                        XSobel = -pFrame2D[j-1][i-1] + pFrame2D[j-1][i+1]
                        -3*pFrame2D[j][i-1] + 3*pFrame2D[j][i+1];

                        YSobel = pFrame2D[j-1][i-1] + 2*pFrame2D[j-1][i] + pFrame2D[j-1][i+1]
                        -pFrame2D[j][i-1] - 2*pFrame2D[j][i] - pFrame2D[j][i+1];
                    }
                    else if(i == 0) {
                        XSobel = -pFrame2D[j-1][0] + pFrame2D[j-1][i+1]
                        -2*pFrame2D[j][0] + 2*pFrame2D[j][i+1]				
                        -pFrame2D[j+1][0] + pFrame2D[j+1][i+1];

                        YSobel = 3*pFrame2D[j-1][0] + pFrame2D[j-1][i+1]
                        -3*pFrame2D[j+1][0] - pFrame2D[j+1][i+1];
                    }
                    else if(i == (nWidth - 1)) {
                        XSobel = -pFrame2D[j-1][i-1] + pFrame2D[j-1][i]
                        -2*pFrame2D[j][i-1] + 2*pFrame2D[j][i]				
                        -pFrame2D[j+1][i-1] + pFrame2D[j+1][i];

                        YSobel = pFrame2D[j-1][i-1] + 3*pFrame2D[j-1][i]
                        -pFrame2D[j+1][i-1] - 3*pFrame2D[j+1][i];
                    }
                    else {
                        XSobel = -pFrame2D[j-1][i-1] + pFrame2D[j-1][i+1]
                        -2*pFrame2D[j][i-1] + 2*pFrame2D[j][i+1]				
                        -pFrame2D[j+1][i-1] + pFrame2D[j+1][i+1];

                        YSobel = pFrame2D[j-1][i-1] + 2*pFrame2D[j-1][i] + pFrame2D[j-1][i+1]
                        -pFrame2D[j+1][i-1] - 2*pFrame2D[j+1][i] - pFrame2D[j+1][i+1];
                    }

                    SobelValue = sqrt(pow(XSobel, 2) + pow(YSobel, 2));

                    //NewYBuffer[j*nWidth + i] = (unsigned char)SobelValue;
                    NewSIBuffer[j*nWidth + i] = SobelValue;
                }
            }

            double flSI_A=0, flSI_S=0;
            for(int m = 1; m < (nHeight - 1); m++){
                for(int n = 1; n < (nWidth - 1); n++) {
                    flSI_A = flSI_A + NewSIBuffer[m*nWidth + n];
                }
            }
            flSI_A = flSI_A/((nWidth - 2)*(nHeight - 2));

            for(int m = 1; m < (nHeight - 1); m++){
                for(int n = 1; n < (nWidth - 1); n++)
                {
                    flSI_S = flSI_S + pow((NewSIBuffer[m*nWidth+n]-flSI_A), 2);
                }
            }
            flSI_S = sqrt(flSI_S/((nWidth - 2)*(nHeight - 2) - 1));

            fprintf(fout_si,"%.4f ",flSI_S);
            fflush(fout_si);
            fprintf(stdout,".");
            if((k-start+1)%50==0)
                fprintf(stdout,"\n");
            fflush(stdout);
        }

        fclose(fout_si);
        delete []NewSIBuffer;
    }

    if(b_ti){
        FILE* fout_ti;
        out_filename=strPath+".ti";
        fout_ti=fopen(out_filename.c_str(),"w");
        double *NewTIBuffer= new double [nYSize];

        fprintf(stdout,"Calculating TI:\n");
        for(int k = start+1;k < end; k++)
        {
            //get current frame
            fseek(fYUV,k*nFrameSize,SEEK_SET);
            fread(pFrame,sizeof(pixel),nYSize,fYUV);

            //get the previous frame
            fseek(fYUV,(k-1)*nFrameSize,SEEK_SET);
            fread(pNextFrame,sizeof(pixel),nYSize,fYUV);

            double flTI_A=0, flTI_S=0;
            for(int j = 0; j < nHeight; j++){
                for(int i = 0; i < nWidth; i++)
                {
                    NewTIBuffer[j*nWidth + i] = pNextFrame2D[j][i] - pFrame2D[j][i];
                    NewYBuffer[j*nWidth + i] = abs(pNextFrame2D[j][i] - pFrame2D[j][i]);

                    flTI_A = flTI_A + NewTIBuffer[j*nWidth + i];
                }
            }

            flTI_A = flTI_A/nYSize;

            for(int j = 0; j < nHeight; j++){
                for(int i = 0; i < nWidth; i++)
                {
                    flTI_S = flTI_S + pow((NewTIBuffer[j*nWidth + i]-flTI_A), 2);
                }
            }

            flTI_S = sqrt(flTI_S/(nYSize - 1));
            fprintf(fout_ti,"%.4f ",flTI_S);
            fflush(fout_ti);
            fprintf(stdout,".");
            if((k-start+1)%50==0)
                fprintf(stdout,"\n");
            fflush(stdout);
        }
        fclose(fout_ti);
        delete []NewTIBuffer;

    }

    fclose(fYUV);

    delete []pFrame;
    delete []pFrame2D;
    delete []pNextFrame;
    delete []pNextFrame2D;
    //delete []NewYBuffer;

    return 1;
}

//usage
void tisi_usage(){
    printf("---------------------help------------------------\n");
    printf("TISI:calculate the TI and SI values for YUV\n");
    printf("options:\n");
    printf("-i <string> the input file name\n");
    printf("-w <int> width\n");
    printf("-h <int> height\n");
    printf("-s <int> start frame number\n");
    printf("-f <int> frame number\n");
    //printf("-f <string> format:0:420,1:422,2:444,3:400\n");
    printf("-H help\n");
    printf("-------------------------------------------------\n");
}

int main(int argc, char* argv[])
{
    int ret;
    string filename="";
    int width=-1,height=-1,pixfmt=0;
    int start=0,frame_number=-1;


    int opt;
    if(argc==1){
        printf("parameters are not enough!\n");
        tisi_usage();
        return -1;
    }
    while ((opt =getopt(argc, argv,"i:w:h:s:f:H")) != -1)
    {
        switch (opt)
        {
        case 'H':
            tisi_usage();
            return -1;
        case 'i':
            filename=optarg;
            break;
        case 'w':
            width=atoi(optarg);
            break;
        case 'h':
            height=atoi(optarg);
            break;
        case 's':
            start=atoi(optarg);
            break;
        case 'f':
            frame_number=atoi(optarg);
            break;
        default:
            printf("unkown: %c\n", opt);
            tisi_usage();
            return -1;
        }
    }
    if(filename==""){
        fprintf(stderr,"please input the filename\n");
        return -1;
    }
    if(width==-1||height==-1){
        fprintf(stderr,"please input width and height\n");
        return -1;
    }
    if(start<0) {
        fprintf(stderr,"please input the right start frame number\n");
        return -1;
    }
    ret=TIandSI(filename,width,height,start,frame_number,true,true);
    if(!ret)
        return -1;

    return 0;
}
