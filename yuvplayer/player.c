
#define	min(a,b)	((a) < (b) ? (a) : (b))
#define	max(a,b)	((a) > (b) ? (a) : (b))


void display(void) ;
void init1(void) ;
//void spinDisplay(void);
void reshape(int,int);
void mouse(int,int,int,int);
//int main(int argc, char** argv);
int read_QCIF_frame(char *filename, int number, int **, int **, int **);
//ssize_t	 Readn(int, void *, size_t);
//ssize_t	 readn(int, void *, size_t);
void init_color_table(void);
int display_launcher(int);
//ssize_t	 writen(int fd, const void *vptr, size_t n);








/****************************************************************///
//***************************************************************///
// author : Eric Setton                                          ///
// date : Summer 2004                                            ///
// group : Stanford University Image, Video and Multimedia Systems///
//                                                               ///  
// filename : disp.c                                             ///
// This file contains the source for a simple color video player ///
//                                                               ///
//***************************************************************///


#include <GL/glu.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <GL/glx.h>
#include <math.h>
#include	<sys/types.h>	/* basic system data types */
#include	<sys/socket.h>	/* basic socket definitions */
#include	<sys/time.h>	/* timeval{} for select() */
#include	<sys/select.h>	/* timeval{} for select() */
#include	<time.h>		/* timespec{} for pselect() */
#include	<netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include	<arpa/inet.h>	/* inet(3) functions */
#include	<errno.h>
#include	<fcntl.h>		/* for nonblocking */
#include	<netdb.h>
#include	<signal.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<sys/stat.h>	/* for S_xxx file mode constants */
#include	<sys/uio.h>		/* for iovec{} and readv/writev */
#include	<unistd.h>
#include	<sys/wait.h>
#include	<sys/un.h>		/* for Unix domain sockets */


static GLubyte image[144][176][3];
static GLubyte image_big[288][352][3];
static int **framey, **frameu, **framev;

static int mode ;
static int k=1;

static struct timeval currentTime ;
static struct timeval displayTime ;
static char filename[128];
static int file_descriptor ; //modif

static struct timeval timerstart ;
static struct timeval timerend ;

static float lookuptableur[257];
static float lookuptablevr[257];
static float lookuptableug[257];
static float lookuptablevg[257];
static float lookuptableub[257];
static float lookuptablevb[257];

static unsigned char buffery[25344];
static unsigned char bufferu[6336]; 
static unsigned char bufferv[6336];
static unsigned char buffery_big[101376];
static unsigned char bufferu_big[25344];
static unsigned char bufferv_big[25344];


static int width;
static int height;

FILE *f;
/****************************************************************///
//***************************************************************///
// author : Eric Setton                                          ///
// date : Summer 2004                                            ///
// group : Stanford University Image, Video and Multimedia Systems//
//                                                               ///  
// function : read_QCIF_frame                                    ///
// description : reads the data from the pipe linking the display///
//               to the decoder                                  ///
//***************************************************************///

int read_QCIF_frame(char *filename, int number, int  **framey, int **frameu, int **framev){
   int i,j,maxfdp1,i0,j0;
   //fd_set rset;
   struct timeval timeout;
   
   timeout.tv_sec = 250;
   timeout.tv_usec= 0;
   
   //FD_ZERO(&rset);
   //FD_SET(file_descriptor,&rset);
   //maxfdp1 = file_descriptor+1;
   //if((i=select(maxfdp1,&rset,NULL,NULL,&timeout))==-1){
   //  printf("error on select");
   //  exit(1);
   //}
   //if(i==0)
   //  exit(0);

   

  /*  if((f=fopen(filename,"r"))==NULL){ */
/*       printf("cannot open file %s\n",*filename); */
/*       exit(1); */
/*       } */
   
   //fseek(f,0,SEEK_SET);
   fseek(f,sizeof(unsigned char)*((int)height*width*1.5)*(k-1),SEEK_SET);
   
   if(mode == 0){
     fread(buffery,sizeof(unsigned char),height*width,f);
     fread(bufferv,sizeof(unsigned char),height*width/4,f);
     fread(bufferu,sizeof(unsigned char),height*width/4,f);
   }  
   else
   {
     fread(buffery_big,sizeof(unsigned char),height*width,f);
     fread(bufferv_big,sizeof(unsigned char),height*width/4,f);
     fread(bufferu_big,sizeof(unsigned char),height*width/4,f);
   }
     //Readn(file_descriptor,buffery,25344);
   //Readn(file_descriptor,bufferu,6336);
   //Readn(file_descriptor,bufferv,6336);
   //Readn(filename,buffery,25344);
   //Readn(file_descriptor,bufferu,6336);
   //Readn(file_descriptor,bufferv,6336);
  
   
   //fclose(f);
   // data formatting
   for(i=0;i<height;i++){
     for(j=0;j<width;j++){
       i0=i/2;
       j0=j/2;
       if(mode ==0)
	 *(*(framey+i)+j)=(int) *(buffery+i*width+j);
       else
	 *(*(framey+i)+j)=(int) *(buffery_big+i*width+j);
       if(i/2==floor(i/2) && j/2==floor(j/2)){
	 if(mode ==0){
	   *(*(frameu+i0)+j0)=(int) *(bufferu+i0*88+j0);
	   *(*(framev+i0)+j0)=(int) *(bufferv+i0*88+j0);
	 }
	 else{
	   *(*(frameu+i0)+j0)=(int) *(bufferu_big+i0*176+j0);
	   *(*(framev+i0)+j0)=(int) *(bufferv_big+i0*176+j0);
	 }
       }
     }
   }

   return 1;
}


/****************************************************************///
//***************************************************************///
// author : Eric Setton                                          ///
// date : Summer 2004                                            ///
// group :  Stanford University Image, Video and Multimedia Systems//
//                                                               ///  
// function : display                                            ///
// description : calls the decoded data reader operates the      ///
//               color conversion and draws the image            ///
//***************************************************************///

void display(){
  
  int i,j,i0,j0,w,h;
  read_QCIF_frame(filename,k,framey,frameu,framev);
  for(i=0;i<height;i++){
    for(j=0;j<width;j++){
      i0= i/2;
      j0=j/2;
      
      //for color display this should be used
      if(mode == 0){
	image[143-i][j][0]=(GLubyte)(framey[i][j]+lookuptableur[framev[i0][j0]]+lookuptablevr[frameu[i0][j0]]-177.9);
	image[143-i][j][1]=(GLubyte)(framey[i][j]+lookuptableug[framev[i0][j0]]+lookuptablevg[frameu[i0][j0]]+134.3);
	image[143-i][j][2]=(GLubyte)(framey[i][j]+lookuptableub[framev[i0][j0]]+lookuptablevb[frameu[i0][j0]]-225.2);
      }
      else{
	image_big[287-i][j][0]=(GLubyte)(framey[i][j]+lookuptableur[framev[i0][j0]]+lookuptablevr[frameu[i0][j0]]-177.9);
	image_big[287-i][j][1]=(GLubyte)(framey[i][j]+lookuptableug[framev[i0][j0]]+lookuptablevg[frameu[i0][j0]]+134.3);
	image_big[287-i][j][2]=(GLubyte)(framey[i][j]+lookuptableub[framev[i0][j0]]+lookuptablevb[frameu[i0][j0]]-225.2);
      }

      //for black and white display this should be used
      //image[143-i][j][0]=(GLubyte)(framey[i][j]); 
      //image[143-i][j][1]=(GLubyte)(frameu[i0][j0]);
      //image[143-i][j][2]=(GLubyte)(framev[i0][j0]);

    }
  }

 
  glClear(GL_COLOR_BUFFER_BIT);
  glRasterPos2i(0,0);
  if(mode ==0)
    glDrawPixels(width,height,GL_RGB,GL_UNSIGNED_BYTE,image);
  else
    glDrawPixels(width,height,GL_RGB,GL_UNSIGNED_BYTE,image_big);
  glFlush();
  
  gettimeofday(&currentTime, NULL);
  while(currentTime.tv_sec*1000000+currentTime.tv_usec<displayTime.tv_sec*1000000+displayTime.tv_usec ){
    //printf("in loop\n");
    gettimeofday(&currentTime, NULL);
  }
  displayTime.tv_sec=currentTime.tv_sec;
  displayTime.tv_usec=currentTime.tv_usec+33333;

  glutSwapBuffers();
}


/****************************************************************///
//***************************************************************///
// author : Eric Setton                                          ///
// date : Summer 2004                                            ///
// group :  Stanford University Image, Video and Multimedia Systems//
//                                                               ///  
// function : startDisplay                                       ///
// description :                                                 /// 
//***************************************************************///

void startDisplay(void){
  k=k+1;
  if(k>400)
    k=1;  
  displayTime.tv_sec=currentTime.tv_sec;
  displayTime.tv_usec=currentTime.tv_usec+33333;  //33333;
  glutPostRedisplay();

}


/****************************************************************///
//***************************************************************///
// author : Eric Setton                                          ///
// date : Summer 2004                                            ///
// group :  Stanford University Image, Video and Multimedia Systems//
//                                                               ///  
// function :  init1                                             ///
// description : clears the window                               /// 
//***************************************************************///
void init(void){
  glClearColor(0.0,0.0,0.0,0.0);
  glShadeModel(GL_FLAT);
}
void init1(void){
  glClearColor(0.0,0.0,0.0,0.0);
  glShadeModel(GL_FLAT);
}


/****************************************************************///
//***************************************************************///
// author : Eric Setton                                          ///
// date : Summer 2004                                            ///
// group :  Stanford University Image, Video and Multimedia Systems//
//                                                               ///  
// function : reshape                                            ///
// description : redraws the window on reshape and does the      /// 
//               operates the appropriate zoom                   ///
//                                                               ///
//***************************************************************///

void reshape(int w,int h){

  glViewport(0,0,(GLsizei) w,(GLsizei) h);
  glPixelZoom((GLfloat) w/width,(GLfloat) h/height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0,(GLdouble) w ,0,(GLdouble) h);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

}

/****************************************************************///
//***************************************************************///
// author : Eric Setton                                          ///
// date : Summer 2004                                            ///
// group :  Stanford University Image, Video and Multimedia Systems//
//                                                               ///  
// function :  mouse                                             ///
// description : mouse control function                          /// 
//              -left button used to start display               ///
//              -middle button used to stop display              ///
//***************************************************************///

void mouse(int button, int state, int x, int y){

  switch (button){
  case GLUT_LEFT_BUTTON:
    if(state == GLUT_DOWN)
      glutIdleFunc(startDisplay);
    break;
  case GLUT_MIDDLE_BUTTON:
    if(state == GLUT_DOWN)
        glutIdleFunc(NULL);
    break;
  default :
    break;
  }
}



int main(int argc, char** argv){
 
  int i,j;
  char response[128];

  printf("is this a qcif file ?\n");
    
  scanf("%s",response);
  mode =1;
  for(i=0;i<128;i++)
    if(response[i]=='y'){
      mode = 0;
      break;
    }

  if(argc >=2)
    strcpy(filename,argv[1]);

  if((f=fopen(filename,"r"))==NULL){
    printf("cannot open file %s\n",*filename);
    exit(1);
  }

  if(mode==0){
    height = 144;
    width = 176;
  }
  else{
    height = 288;
    width = 352;
  }

  init_color_table();

  // memory allocation
  framey=(int **)malloc(sizeof(int *)*height);
  frameu=(int **)malloc(sizeof(int *)*height/2);
  framev=(int **)malloc(sizeof(int *)*height/2);
  for(i=0;i<height;i++){
    framey[i]=(int *)malloc(sizeof(int)*width);
    if(i/2==floor(i/2)){
      frameu[i/2]=(int *)malloc(sizeof(int)*width/2);
      framev[i/2]=(int *)malloc(sizeof(int)*width/2);
    }
  }

  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(width,height);
  glutInitWindowPosition(100,100);
  glutCreateWindow(argv[0]);
  
  gettimeofday(&currentTime, NULL);
  displayTime.tv_sec=currentTime.tv_sec;
  displayTime.tv_usec=currentTime.tv_usec;
  
 

  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMainLoop();
    
  return 0;
}







//************************************************************///
//************************************************************///
// author : Eric Setton                                       ///
// date : Summer 2004           
// group :  Stanford University Image, Video and Multimedia Systems//
//                                                            ///  
// function :  init_color_table                               ///
// description : creates the lookuptables for the color       /// 
//               conversion                                   ///
//************************************************************///

void init_color_table(void){
     lookuptableur[0]=-0;
     lookuptableur[1]=-0.0009;
     lookuptableur[2]=-0.0018;
     lookuptableur[3]=-0.0027;
     lookuptableur[4]=-0.0036;
     lookuptableur[5]=-0.0045;
     lookuptableur[6]=-0.0054;
     lookuptableur[7]=-0.0063;
     lookuptableur[8]=-0.0072;
     lookuptableur[9]=-0.0081;
     lookuptableur[10]=-0.009;
     lookuptableur[11]=-0.0099;
     lookuptableur[12]=-0.0108;
     lookuptableur[13]=-0.0117;
     lookuptableur[14]=-0.0126;
     lookuptableur[15]=-0.0135;
     lookuptableur[16]=-0.0144;
     lookuptableur[17]=-0.0153;
     lookuptableur[18]=-0.0162;
     lookuptableur[19]=-0.0171;
     lookuptableur[20]=-0.018;
     lookuptableur[21]=-0.0189;
     lookuptableur[22]=-0.0198;
     lookuptableur[23]=-0.0207;
     lookuptableur[24]=-0.0216;
     lookuptableur[25]=-0.0225;
     lookuptableur[26]=-0.0234;
     lookuptableur[27]=-0.0243;
     lookuptableur[28]=-0.0252;
     lookuptableur[29]=-0.0261;
     lookuptableur[30]=-0.027;
     lookuptableur[31]=-0.0279;
     lookuptableur[32]=-0.0288;
     lookuptableur[33]=-0.0297;
     lookuptableur[34]=-0.0306;
     lookuptableur[35]=-0.0315;
     lookuptableur[36]=-0.0324;
     lookuptableur[37]=-0.0333;
     lookuptableur[38]=-0.0342;
     lookuptableur[39]=-0.0351;
     lookuptableur[40]=-0.036;
     lookuptableur[41]=-0.0369;
     lookuptableur[42]=-0.0378;
     lookuptableur[43]=-0.0387;
     lookuptableur[44]=-0.0396;
     lookuptableur[45]=-0.0405;
     lookuptableur[46]=-0.0414;
     lookuptableur[47]=-0.0423;
     lookuptableur[48]=-0.0432;
     lookuptableur[49]=-0.0441;
     lookuptableur[50]=-0.045;
     lookuptableur[51]=-0.0459;
     lookuptableur[52]=-0.0468;
     lookuptableur[53]=-0.0477;
     lookuptableur[54]=-0.0486;
     lookuptableur[55]=-0.0495;
     lookuptableur[56]=-0.0504;
     lookuptableur[57]=-0.0513;
     lookuptableur[58]=-0.0522;
     lookuptableur[59]=-0.0531;
     lookuptableur[60]=-0.054;
     lookuptableur[61]=-0.0549;
     lookuptableur[62]=-0.0558;
     lookuptableur[63]=-0.0567;
     lookuptableur[64]=-0.0576;
     lookuptableur[65]=-0.0585;
     lookuptableur[66]=-0.0594;
     lookuptableur[67]=-0.0603;
     lookuptableur[68]=-0.0612;
     lookuptableur[69]=-0.0621;
     lookuptableur[70]=-0.063;
     lookuptableur[71]=-0.0639;
     lookuptableur[72]=-0.0648;
     lookuptableur[73]=-0.0657;
     lookuptableur[74]=-0.0666;
     lookuptableur[75]=-0.0675;
     lookuptableur[76]=-0.0684;
     lookuptableur[77]=-0.0693;
     lookuptableur[78]=-0.0702;
     lookuptableur[79]=-0.0711;
     lookuptableur[80]=-0.072;
     lookuptableur[81]=-0.0729;
     lookuptableur[82]=-0.0738;
     lookuptableur[83]=-0.0747;
     lookuptableur[84]=-0.0756;
     lookuptableur[85]=-0.0765;
     lookuptableur[86]=-0.0774;
     lookuptableur[87]=-0.0783;
     lookuptableur[88]=-0.0792;
     lookuptableur[89]=-0.0801;
     lookuptableur[90]=-0.081;
     lookuptableur[91]=-0.0819;
     lookuptableur[92]=-0.0828;
     lookuptableur[93]=-0.0837;
     lookuptableur[94]=-0.0846;
     lookuptableur[95]=-0.0855;
     lookuptableur[96]=-0.0864;
     lookuptableur[97]=-0.0873;
     lookuptableur[98]=-0.0882;
     lookuptableur[99]=-0.0891;
     lookuptableur[100]=-0.09;
     lookuptableur[101]=-0.0909;
     lookuptableur[102]=-0.0918;
     lookuptableur[103]=-0.0927;
     lookuptableur[104]=-0.0936;
     lookuptableur[105]=-0.0945;
     lookuptableur[106]=-0.0954;
     lookuptableur[107]=-0.0963;
     lookuptableur[108]=-0.0972;
     lookuptableur[109]=-0.0981;
     lookuptableur[110]=-0.099;
     lookuptableur[111]=-0.0999;
     lookuptableur[112]=-0.1008;
     lookuptableur[113]=-0.1017;
     lookuptableur[114]=-0.1026;
     lookuptableur[115]=-0.1035;
     lookuptableur[116]=-0.1044;
     lookuptableur[117]=-0.1053;
     lookuptableur[118]=-0.1062;
     lookuptableur[119]=-0.1071;
     lookuptableur[120]=-0.108;
     lookuptableur[121]=-0.1089;
     lookuptableur[122]=-0.1098;
     lookuptableur[123]=-0.1107;
     lookuptableur[124]=-0.1116;
     lookuptableur[125]=-0.1125;
     lookuptableur[126]=-0.1134;
     lookuptableur[127]=-0.1143;
     lookuptableur[128]=-0.1152;
     lookuptableur[129]=-0.1161;
     lookuptableur[130]=-0.117;
     lookuptableur[131]=-0.1179;
     lookuptableur[132]=-0.1188;
     lookuptableur[133]=-0.1197;
     lookuptableur[134]=-0.1206;
     lookuptableur[135]=-0.1215;
     lookuptableur[136]=-0.1224;
     lookuptableur[137]=-0.1233;
     lookuptableur[138]=-0.1242;
     lookuptableur[139]=-0.1251;
     lookuptableur[140]=-0.126;
     lookuptableur[141]=-0.1269;
     lookuptableur[142]=-0.1278;
     lookuptableur[143]=-0.1287;
     lookuptableur[144]=-0.1296;
     lookuptableur[145]=-0.1305;
     lookuptableur[146]=-0.1314;
     lookuptableur[147]=-0.1323;
     lookuptableur[148]=-0.1332;
     lookuptableur[149]=-0.1341;
     lookuptableur[150]=-0.135;
     lookuptableur[151]=-0.1359;
     lookuptableur[152]=-0.1368;
     lookuptableur[153]=-0.1377;
     lookuptableur[154]=-0.1386;
     lookuptableur[155]=-0.1395;
     lookuptableur[156]=-0.1404;
     lookuptableur[157]=-0.1413;
     lookuptableur[158]=-0.1422;
     lookuptableur[159]=-0.1431;
     lookuptableur[160]=-0.144;
     lookuptableur[161]=-0.1449;
     lookuptableur[162]=-0.1458;
     lookuptableur[163]=-0.1467;
     lookuptableur[164]=-0.1476;
     lookuptableur[165]=-0.1485;
     lookuptableur[166]=-0.1494;
     lookuptableur[167]=-0.1503;
     lookuptableur[168]=-0.1512;
     lookuptableur[169]=-0.1521;
     lookuptableur[170]=-0.153;
     lookuptableur[171]=-0.1539;
     lookuptableur[172]=-0.1548;
     lookuptableur[173]=-0.1557;
     lookuptableur[174]=-0.1566;
     lookuptableur[175]=-0.1575;
     lookuptableur[176]=-0.1584;
     lookuptableur[177]=-0.1593;
     lookuptableur[178]=-0.1602;
     lookuptableur[179]=-0.1611;
     lookuptableur[180]=-0.162;
     lookuptableur[181]=-0.1629;
     lookuptableur[182]=-0.1638;
     lookuptableur[183]=-0.1647;
     lookuptableur[184]=-0.1656;
     lookuptableur[185]=-0.1665;
     lookuptableur[186]=-0.1674;
     lookuptableur[187]=-0.1683;
     lookuptableur[188]=-0.1692;
     lookuptableur[189]=-0.1701;
     lookuptableur[190]=-0.171;
     lookuptableur[191]=-0.1719;
     lookuptableur[192]=-0.1728;
     lookuptableur[193]=-0.1737;
     lookuptableur[194]=-0.1746;
     lookuptableur[195]=-0.1755;
     lookuptableur[196]=-0.1764;
     lookuptableur[197]=-0.1773;
     lookuptableur[198]=-0.1782;
     lookuptableur[199]=-0.1791;
     lookuptableur[200]=-0.18;
     lookuptableur[201]=-0.1809;
     lookuptableur[202]=-0.1818;
     lookuptableur[203]=-0.1827;
     lookuptableur[204]=-0.1836;
     lookuptableur[205]=-0.1845;
     lookuptableur[206]=-0.1854;
     lookuptableur[207]=-0.1863;
     lookuptableur[208]=-0.1872;
     lookuptableur[209]=-0.1881;
     lookuptableur[210]=-0.189;
     lookuptableur[211]=-0.1899;
     lookuptableur[212]=-0.1908;
     lookuptableur[213]=-0.1917;
     lookuptableur[214]=-0.1926;
     lookuptableur[215]=-0.1935;
     lookuptableur[216]=-0.1944;
     lookuptableur[217]=-0.1953;
     lookuptableur[218]=-0.1962;
     lookuptableur[219]=-0.1971;
     lookuptableur[220]=-0.198;
     lookuptableur[221]=-0.1989;
     lookuptableur[222]=-0.1998;
     lookuptableur[223]=-0.2007;
     lookuptableur[224]=-0.2016;
     lookuptableur[225]=-0.2025;
     lookuptableur[226]=-0.2034;
     lookuptableur[227]=-0.2043;
     lookuptableur[228]=-0.2052;
     lookuptableur[229]=-0.2061;
     lookuptableur[230]=-0.207;
     lookuptableur[231]=-0.2079;
     lookuptableur[232]=-0.2088;
     lookuptableur[233]=-0.2097;
     lookuptableur[234]=-0.2106;
     lookuptableur[235]=-0.2115;
     lookuptableur[236]=-0.2124;
     lookuptableur[237]=-0.2133;
     lookuptableur[238]=-0.2142;
     lookuptableur[239]=-0.2151;
     lookuptableur[240]=-0.216;
     lookuptableur[241]=-0.2169;
     lookuptableur[242]=-0.2178;
     lookuptableur[243]=-0.2187;
     lookuptableur[244]=-0.2196;
     lookuptableur[245]=-0.2205;
     lookuptableur[246]=-0.2214;
     lookuptableur[247]=-0.2223;
     lookuptableur[248]=-0.2232;
     lookuptableur[249]=-0.2241;
     lookuptableur[250]=-0.225;
     lookuptableur[251]=-0.2259;
     lookuptableur[252]=-0.2268;
     lookuptableur[253]=-0.2277;
     lookuptableur[254]=-0.2286;
     lookuptableur[255]=-0.2295;
     lookuptableur[256]=-0.2304;

     
     lookuptablevr[0]=0;
     lookuptablevr[1]=1.4017;
     lookuptablevr[2]=2.8034;
     lookuptablevr[3]=4.2051;
     lookuptablevr[4]=5.6068;
     lookuptablevr[5]=7.0085;
     lookuptablevr[6]=8.4102;
     lookuptablevr[7]=9.8119;
     lookuptablevr[8]=11.2136;
     lookuptablevr[9]=12.6153;
     lookuptablevr[10]=14.017;
     lookuptablevr[11]=15.4187;
     lookuptablevr[12]=16.8204;
     lookuptablevr[13]=18.2221;
     lookuptablevr[14]=19.6238;
     lookuptablevr[15]=21.0255;
     lookuptablevr[16]=22.4272;
     lookuptablevr[17]=23.8289;
     lookuptablevr[18]=25.2306;
     lookuptablevr[19]=26.6323;
     lookuptablevr[20]=28.034;
     lookuptablevr[21]=29.4357;
     lookuptablevr[22]=30.8374;
     lookuptablevr[23]=32.2391;
     lookuptablevr[24]=33.6408;
     lookuptablevr[25]=35.0425;
     lookuptablevr[26]=36.4442;
     lookuptablevr[27]=37.8459;
     lookuptablevr[28]=39.2476;
     lookuptablevr[29]=40.6493;
     lookuptablevr[30]=42.051;
     lookuptablevr[31]=43.4527;
     lookuptablevr[32]=44.8544;
     lookuptablevr[33]=46.2561;
     lookuptablevr[34]=47.6578;
     lookuptablevr[35]=49.0595;
     lookuptablevr[36]=50.4612;
     lookuptablevr[37]=51.8629;
     lookuptablevr[38]=53.2646;
     lookuptablevr[39]=54.6663;
     lookuptablevr[40]=56.068;
     lookuptablevr[41]=57.4697;
     lookuptablevr[42]=58.8714;
     lookuptablevr[43]=60.2731;
     lookuptablevr[44]=61.6748;
     lookuptablevr[45]=63.0765;
     lookuptablevr[46]=64.4782;
     lookuptablevr[47]=65.8799;
     lookuptablevr[48]=67.2816;
     lookuptablevr[49]=68.6833;
     lookuptablevr[50]=70.085;
     lookuptablevr[51]=71.4867;
     lookuptablevr[52]=72.8884;
     lookuptablevr[53]=74.2901;
     lookuptablevr[54]=75.6918;
     lookuptablevr[55]=77.0935;
     lookuptablevr[56]=78.4952;
     lookuptablevr[57]=79.8969;
     lookuptablevr[58]=81.2986;
     lookuptablevr[59]=82.7003;
     lookuptablevr[60]=84.102;
     lookuptablevr[61]=85.5037;
     lookuptablevr[62]=86.9054;
     lookuptablevr[63]=88.3071;
     lookuptablevr[64]=89.7088;
     lookuptablevr[65]=91.1105;
lookuptablevr[66]=92.5122;
lookuptablevr[67]=93.9139;
lookuptablevr[68]=95.3156;
lookuptablevr[69]=96.7173;
lookuptablevr[70]=98.119;
lookuptablevr[71]=99.5207;
lookuptablevr[72]=100.9224;
lookuptablevr[73]=102.3241;
lookuptablevr[74]=103.7258;
lookuptablevr[75]=105.1275;
lookuptablevr[76]=106.5292;
lookuptablevr[77]=107.9309;
lookuptablevr[78]=109.3326;
lookuptablevr[79]=110.7343;
lookuptablevr[80]=112.136;
lookuptablevr[81]=113.5377;
lookuptablevr[82]=114.9394;
lookuptablevr[83]=116.3411;
lookuptablevr[84]=117.7428;
lookuptablevr[85]=119.1445;
lookuptablevr[86]=120.5462;
lookuptablevr[87]=121.9479;
lookuptablevr[88]=123.3496;
lookuptablevr[89]=124.7513;
lookuptablevr[90]=126.153;
lookuptablevr[91]=127.5547;
lookuptablevr[92]=128.9564;
lookuptablevr[93]=130.3581;
lookuptablevr[94]=131.7598;
lookuptablevr[95]=133.1615;
lookuptablevr[96]=134.5632;
lookuptablevr[97]=135.9649;
lookuptablevr[98]=137.3666;
lookuptablevr[99]=138.7683;
lookuptablevr[100]=140.17;
lookuptablevr[101]=141.5717;
lookuptablevr[102]=142.9734;
lookuptablevr[103]=144.3751;
lookuptablevr[104]=145.7768;
lookuptablevr[105]=147.1785;
lookuptablevr[106]=148.5802;
lookuptablevr[107]=149.9819;
lookuptablevr[108]=151.3836;
lookuptablevr[109]=152.7853;
lookuptablevr[110]=154.187;
lookuptablevr[111]=155.5887;
lookuptablevr[112]=156.9904;
lookuptablevr[113]=158.3921;
lookuptablevr[114]=159.7938;
lookuptablevr[115]=161.1955;
lookuptablevr[116]=162.5972;
lookuptablevr[117]=163.9989;
lookuptablevr[118]=165.4006;
lookuptablevr[119]=166.8023;
lookuptablevr[120]=168.204;
lookuptablevr[121]=169.6057;
lookuptablevr[122]=171.0074;
lookuptablevr[123]=172.4091;
lookuptablevr[124]=173.8108;
lookuptablevr[125]=175.2125;
lookuptablevr[126]=176.6142;
lookuptablevr[127]=178.0159;
lookuptablevr[128]=179.4176;
lookuptablevr[129]=180.8193;
lookuptablevr[130]=182.221;
lookuptablevr[131]=183.6227;
lookuptablevr[132]=185.0244;
lookuptablevr[133]=186.4261;
lookuptablevr[134]=187.8278;
lookuptablevr[135]=189.2295;
lookuptablevr[136]=190.6312;
lookuptablevr[137]=192.0329;
lookuptablevr[138]=193.4346;
lookuptablevr[139]=194.8363;
lookuptablevr[140]=196.238;
lookuptablevr[141]=197.6397;
lookuptablevr[142]=199.0414;
lookuptablevr[143]=200.4431;
lookuptablevr[144]=201.8448;
lookuptablevr[145]=203.2465;
lookuptablevr[146]=204.6482;
lookuptablevr[147]=206.0499;
lookuptablevr[148]=207.4516;
lookuptablevr[149]=208.8533;
lookuptablevr[150]=210.255;
lookuptablevr[151]=211.6567;
lookuptablevr[152]=213.0584;
lookuptablevr[153]=214.4601;
lookuptablevr[154]=215.8618;
lookuptablevr[155]=217.2635;
lookuptablevr[156]=218.6652;
lookuptablevr[157]=220.0669;
lookuptablevr[158]=221.4686;
lookuptablevr[159]=222.8703;
lookuptablevr[160]=224.272;
lookuptablevr[161]=225.6737;
lookuptablevr[162]=227.0754;
lookuptablevr[163]=228.4771;
lookuptablevr[164]=229.8788;
lookuptablevr[165]=231.2805;
lookuptablevr[166]=232.6822;
lookuptablevr[167]=234.0839;
lookuptablevr[168]=235.4856;
lookuptablevr[169]=236.8873;
lookuptablevr[170]=238.289;
lookuptablevr[171]=239.6907;
lookuptablevr[172]=241.0924;
lookuptablevr[173]=242.4941;
lookuptablevr[174]=243.8958;
lookuptablevr[175]=245.2975;
lookuptablevr[176]=246.6992;
lookuptablevr[177]=248.1009;
lookuptablevr[178]=249.5026;
lookuptablevr[179]=250.9043;
lookuptablevr[180]=252.306;
lookuptablevr[181]=253.7077;
lookuptablevr[182]=255.1094;
lookuptablevr[183]=256.5111;
lookuptablevr[184]=257.9128;
lookuptablevr[185]=259.3145;
lookuptablevr[186]=260.7162;
lookuptablevr[187]=262.1179;
lookuptablevr[188]=263.5196;
lookuptablevr[189]=264.9213;
lookuptablevr[190]=266.323;
lookuptablevr[191]=267.7247;
lookuptablevr[192]=269.1264;
lookuptablevr[193]=270.5281;
lookuptablevr[194]=271.9298;
lookuptablevr[195]=273.3315;
lookuptablevr[196]=274.7332;
lookuptablevr[197]=276.1349;
lookuptablevr[198]=277.5366;
lookuptablevr[199]=278.9383;
lookuptablevr[200]=280.34;
lookuptablevr[201]=281.7417;
lookuptablevr[202]=283.1434;
lookuptablevr[203]=284.5451;
lookuptablevr[204]=285.9468;
lookuptablevr[205]=287.3485;
lookuptablevr[206]=288.7502;
lookuptablevr[207]=290.1519;
lookuptablevr[208]=291.5536;
lookuptablevr[209]=292.9553;
lookuptablevr[210]=294.357;
lookuptablevr[211]=295.7587;
lookuptablevr[212]=297.1604;
lookuptablevr[213]=298.5621;
lookuptablevr[214]=299.9638;
lookuptablevr[215]=301.3655;
lookuptablevr[216]=302.7672;
lookuptablevr[217]=304.1689;
lookuptablevr[218]=305.5706;
lookuptablevr[219]=306.9723;
lookuptablevr[220]=308.374;
lookuptablevr[221]=309.7757;
lookuptablevr[222]=311.1774;
lookuptablevr[223]=312.5791;
lookuptablevr[224]=313.9808;
lookuptablevr[225]=315.3825;
lookuptablevr[226]=316.7842;
lookuptablevr[227]=318.1859;
lookuptablevr[228]=319.5876;
lookuptablevr[229]=320.9893;
lookuptablevr[230]=322.391;
lookuptablevr[231]=323.7927;
lookuptablevr[232]=325.1944;
lookuptablevr[233]=326.5961;
lookuptablevr[234]=327.9978;
lookuptablevr[235]=329.3995;
lookuptablevr[236]=330.8012;
lookuptablevr[237]=332.2029;
lookuptablevr[238]=333.6046;
lookuptablevr[239]=335.0063;
lookuptablevr[240]=336.408;
lookuptablevr[241]=337.8097;
lookuptablevr[242]=339.2114;
lookuptablevr[243]=340.6131;
lookuptablevr[244]=342.0148;
lookuptablevr[245]=343.4165;
lookuptablevr[246]=344.8182;
lookuptablevr[247]=346.2199;
lookuptablevr[248]=347.6216;
lookuptablevr[249]=349.0233;
lookuptablevr[250]=350.425;
lookuptablevr[251]=351.8267;
lookuptablevr[252]=353.2284;
lookuptablevr[253]=354.6301;
lookuptablevr[254]=356.0318;
lookuptablevr[255]=357.4335;
lookuptablevr[256]=358.8352;


lookuptableug[0]=-0;
lookuptableug[1]=-0.3437;
lookuptableug[2]=-0.6874;
lookuptableug[3]=-1.0311;
lookuptableug[4]=-1.3748;
lookuptableug[5]=-1.7185;
lookuptableug[6]=-2.0622;
lookuptableug[7]=-2.4059;
lookuptableug[8]=-2.7496;
lookuptableug[9]=-3.0933;
lookuptableug[10]=-3.437;
lookuptableug[11]=-3.7807;
lookuptableug[12]=-4.1244;
lookuptableug[13]=-4.4681;
lookuptableug[14]=-4.8118;
lookuptableug[15]=-5.1555;
lookuptableug[16]=-5.4992;
lookuptableug[17]=-5.8429;
lookuptableug[18]=-6.1866;
lookuptableug[19]=-6.5303;
lookuptableug[20]=-6.874;
lookuptableug[21]=-7.2177;
lookuptableug[22]=-7.5614;
lookuptableug[23]=-7.9051;
lookuptableug[24]=-8.2488;
lookuptableug[25]=-8.5925;
lookuptableug[26]=-8.9362;
lookuptableug[27]=-9.2799;
lookuptableug[28]=-9.6236;
lookuptableug[29]=-9.9673;
lookuptableug[30]=-10.311;
lookuptableug[31]=-10.6547;
lookuptableug[32]=-10.9984;
lookuptableug[33]=-11.3421;
lookuptableug[34]=-11.6858;
lookuptableug[35]=-12.0295;
lookuptableug[36]=-12.3732;
lookuptableug[37]=-12.7169;
lookuptableug[38]=-13.0606;
lookuptableug[39]=-13.4043;
lookuptableug[40]=-13.748;
lookuptableug[41]=-14.0917;
lookuptableug[42]=-14.4354;
lookuptableug[43]=-14.7791;
lookuptableug[44]=-15.1228;
lookuptableug[45]=-15.4665;
lookuptableug[46]=-15.8102;
lookuptableug[47]=-16.1539;
lookuptableug[48]=-16.4976;
lookuptableug[49]=-16.8413;
lookuptableug[50]=-17.185;
lookuptableug[51]=-17.5287;
lookuptableug[52]=-17.8724;
lookuptableug[53]=-18.2161;
lookuptableug[54]=-18.5598;
lookuptableug[55]=-18.9035;
lookuptableug[56]=-19.2472;
lookuptableug[57]=-19.5909;
lookuptableug[58]=-19.9346;
lookuptableug[59]=-20.2783;
lookuptableug[60]=-20.622;
lookuptableug[61]=-20.9657;
lookuptableug[62]=-21.3094;
lookuptableug[63]=-21.6531;
lookuptableug[64]=-21.9968;
lookuptableug[65]=-22.3405;
lookuptableug[66]=-22.6842;
lookuptableug[67]=-23.0279;
lookuptableug[68]=-23.3716;
lookuptableug[69]=-23.7153;
lookuptableug[70]=-24.059;
lookuptableug[71]=-24.4027;
lookuptableug[72]=-24.7464;
lookuptableug[73]=-25.0901;
lookuptableug[74]=-25.4338;
lookuptableug[75]=-25.7775;
lookuptableug[76]=-26.1212;
lookuptableug[77]=-26.4649;
lookuptableug[78]=-26.8086;
lookuptableug[79]=-27.1523;
lookuptableug[80]=-27.496;
lookuptableug[81]=-27.8397;
lookuptableug[82]=-28.1834;
lookuptableug[83]=-28.5271;
lookuptableug[84]=-28.8708;
lookuptableug[85]=-29.2145;
lookuptableug[86]=-29.5582;
lookuptableug[87]=-29.9019;
lookuptableug[88]=-30.2456;
lookuptableug[89]=-30.5893;
lookuptableug[90]=-30.933;
lookuptableug[91]=-31.2767;
lookuptableug[92]=-31.6204;
lookuptableug[93]=-31.9641;
lookuptableug[94]=-32.3078;
lookuptableug[95]=-32.6515;
lookuptableug[96]=-32.9952;
lookuptableug[97]=-33.3389;
lookuptableug[98]=-33.6826;
lookuptableug[99]=-34.0263;
lookuptableug[100]=-34.37;
lookuptableug[101]=-34.7137;
lookuptableug[102]=-35.0574;
lookuptableug[103]=-35.4011;
lookuptableug[104]=-35.7448;
lookuptableug[105]=-36.0885;
lookuptableug[106]=-36.4322;
lookuptableug[107]=-36.7759;
lookuptableug[108]=-37.1196;
lookuptableug[109]=-37.4633;
lookuptableug[110]=-37.807;
lookuptableug[111]=-38.1507;
lookuptableug[112]=-38.4944;
lookuptableug[113]=-38.8381;
lookuptableug[114]=-39.1818;
lookuptableug[115]=-39.5255;
lookuptableug[116]=-39.8692;
lookuptableug[117]=-40.2129;
lookuptableug[118]=-40.5566;
lookuptableug[119]=-40.9003;
lookuptableug[120]=-41.244;
lookuptableug[121]=-41.5877;
lookuptableug[122]=-41.9314;
lookuptableug[123]=-42.2751;
lookuptableug[124]=-42.6188;
lookuptableug[125]=-42.9625;
lookuptableug[126]=-43.3062;
lookuptableug[127]=-43.6499;
lookuptableug[128]=-43.9936;
lookuptableug[129]=-44.3373;
lookuptableug[130]=-44.681;
lookuptableug[131]=-45.0247;
lookuptableug[132]=-45.3684;
lookuptableug[133]=-45.7121;
lookuptableug[134]=-46.0558;
lookuptableug[135]=-46.3995;
lookuptableug[136]=-46.7432;
lookuptableug[137]=-47.0869;
lookuptableug[138]=-47.4306;
lookuptableug[139]=-47.7743;
lookuptableug[140]=-48.118;
lookuptableug[141]=-48.4617;
lookuptableug[142]=-48.8054;
lookuptableug[143]=-49.1491;
lookuptableug[144]=-49.4928;
lookuptableug[145]=-49.8365;
lookuptableug[146]=-50.1802;
lookuptableug[147]=-50.5239;
lookuptableug[148]=-50.8676;
lookuptableug[149]=-51.2113;
lookuptableug[150]=-51.555;
lookuptableug[151]=-51.8987;
lookuptableug[152]=-52.2424;
lookuptableug[153]=-52.5861;
lookuptableug[154]=-52.9298;
lookuptableug[155]=-53.2735;
lookuptableug[156]=-53.6172;
lookuptableug[157]=-53.9609;
lookuptableug[158]=-54.3046;
lookuptableug[159]=-54.6483;
lookuptableug[160]=-54.992;
lookuptableug[161]=-55.3357;
lookuptableug[162]=-55.6794;
lookuptableug[163]=-56.0231;
lookuptableug[164]=-56.3668;
lookuptableug[165]=-56.7105;
lookuptableug[166]=-57.0542;
lookuptableug[167]=-57.3979;
lookuptableug[168]=-57.7416;
lookuptableug[169]=-58.0853;
lookuptableug[170]=-58.429;
lookuptableug[171]=-58.7727;
lookuptableug[172]=-59.1164;
lookuptableug[173]=-59.4601;
lookuptableug[174]=-59.8038;
lookuptableug[175]=-60.1475;
lookuptableug[176]=-60.4912;
lookuptableug[177]=-60.8349;
lookuptableug[178]=-61.1786;
lookuptableug[179]=-61.5223;
lookuptableug[180]=-61.866;
lookuptableug[181]=-62.2097;
lookuptableug[182]=-62.5534;
lookuptableug[183]=-62.8971;
lookuptableug[184]=-63.2408;
lookuptableug[185]=-63.5845;
lookuptableug[186]=-63.9282;
lookuptableug[187]=-64.2719;
lookuptableug[188]=-64.6156;
lookuptableug[189]=-64.9593;
lookuptableug[190]=-65.303;
lookuptableug[191]=-65.6467;
lookuptableug[192]=-65.9904;
lookuptableug[193]=-66.3341;
lookuptableug[194]=-66.6778;
lookuptableug[195]=-67.0215;
lookuptableug[196]=-67.3652;
lookuptableug[197]=-67.7089;
lookuptableug[198]=-68.0526;
lookuptableug[199]=-68.3963;
lookuptableug[200]=-68.74;
lookuptableug[201]=-69.0837;
lookuptableug[202]=-69.4274;
lookuptableug[203]=-69.7711;
lookuptableug[204]=-70.1148;
lookuptableug[205]=-70.4585;
lookuptableug[206]=-70.8022;
lookuptableug[207]=-71.1459;
lookuptableug[208]=-71.4896;
lookuptableug[209]=-71.8333;
lookuptableug[210]=-72.177;
lookuptableug[211]=-72.5207;
lookuptableug[212]=-72.8644;
lookuptableug[213]=-73.2081;
lookuptableug[214]=-73.5518;
lookuptableug[215]=-73.8955;
lookuptableug[216]=-74.2392;
lookuptableug[217]=-74.5829;
lookuptableug[218]=-74.9266;
lookuptableug[219]=-75.2703;
lookuptableug[220]=-75.614;
lookuptableug[221]=-75.9577;
lookuptableug[222]=-76.3014;
lookuptableug[223]=-76.6451;
lookuptableug[224]=-76.9888;
lookuptableug[225]=-77.3325;
lookuptableug[226]=-77.6762;
lookuptableug[227]=-78.0199;
lookuptableug[228]=-78.3636;
lookuptableug[229]=-78.7073;
lookuptableug[230]=-79.051;
lookuptableug[231]=-79.3947;
lookuptableug[232]=-79.7384;
lookuptableug[233]=-80.0821;
lookuptableug[234]=-80.4258;
lookuptableug[235]=-80.7695;
lookuptableug[236]=-81.1132;
lookuptableug[237]=-81.4569;
lookuptableug[238]=-81.8006;
lookuptableug[239]=-82.1443;
lookuptableug[240]=-82.488;
lookuptableug[241]=-82.8317;
lookuptableug[242]=-83.1754;
lookuptableug[243]=-83.5191;
lookuptableug[244]=-83.8628;
lookuptableug[245]=-84.2065;
lookuptableug[246]=-84.5502;
lookuptableug[247]=-84.8939;
lookuptableug[248]=-85.2376;
lookuptableug[249]=-85.5813;
lookuptableug[250]=-85.925;
lookuptableug[251]=-86.2687;
lookuptableug[252]=-86.6124;
lookuptableug[253]=-86.9561;
lookuptableug[254]=-87.2998;
lookuptableug[255]=-87.6435;
lookuptableug[256]=-87.9872;


lookuptablevg[0]=-0;
lookuptablevg[1]=-0.7142;
lookuptablevg[2]=-1.4284;
lookuptablevg[3]=-2.1426;
lookuptablevg[4]=-2.8568;
lookuptablevg[5]=-3.571;
lookuptablevg[6]=-4.2852;
lookuptablevg[7]=-4.9994;
lookuptablevg[8]=-5.7136;
lookuptablevg[9]=-6.4278;
lookuptablevg[10]=-7.142;
lookuptablevg[11]=-7.8562;
lookuptablevg[12]=-8.5704;
lookuptablevg[13]=-9.2846;
lookuptablevg[14]=-9.9988;
lookuptablevg[15]=-10.713;
lookuptablevg[16]=-11.4272;
lookuptablevg[17]=-12.1414;
lookuptablevg[18]=-12.8556;
lookuptablevg[19]=-13.5698;
lookuptablevg[20]=-14.284;
lookuptablevg[21]=-14.9982;
lookuptablevg[22]=-15.7124;
lookuptablevg[23]=-16.4266;
lookuptablevg[24]=-17.1408;
lookuptablevg[25]=-17.855;
lookuptablevg[26]=-18.5692;
lookuptablevg[27]=-19.2834;
lookuptablevg[28]=-19.9976;
lookuptablevg[29]=-20.7118;
lookuptablevg[30]=-21.426;
lookuptablevg[31]=-22.1402;
lookuptablevg[32]=-22.8544;
lookuptablevg[33]=-23.5686;
lookuptablevg[34]=-24.2828;
lookuptablevg[35]=-24.997;
lookuptablevg[36]=-25.7112;
lookuptablevg[37]=-26.4254;
lookuptablevg[38]=-27.1396;
lookuptablevg[39]=-27.8538;
lookuptablevg[40]=-28.568;
lookuptablevg[41]=-29.2822;
lookuptablevg[42]=-29.9964;
lookuptablevg[43]=-30.7106;
lookuptablevg[44]=-31.4248;
lookuptablevg[45]=-32.139;
lookuptablevg[46]=-32.8532;
lookuptablevg[47]=-33.5674;
lookuptablevg[48]=-34.2816;
lookuptablevg[49]=-34.9958;
lookuptablevg[50]=-35.71;
lookuptablevg[51]=-36.4242;
lookuptablevg[52]=-37.1384;
lookuptablevg[53]=-37.8526;
lookuptablevg[54]=-38.5668;
lookuptablevg[55]=-39.281;
lookuptablevg[56]=-39.9952;
lookuptablevg[57]=-40.7094;
lookuptablevg[58]=-41.4236;
lookuptablevg[59]=-42.1378;
lookuptablevg[60]=-42.852;
lookuptablevg[61]=-43.5662;
lookuptablevg[62]=-44.2804;
lookuptablevg[63]=-44.9946;
lookuptablevg[64]=-45.7088;
lookuptablevg[65]=-46.423;
lookuptablevg[66]=-47.1372;
lookuptablevg[67]=-47.8514;
lookuptablevg[68]=-48.5656;
lookuptablevg[69]=-49.2798;
lookuptablevg[70]=-49.994;
lookuptablevg[71]=-50.7082;
lookuptablevg[72]=-51.4224;
lookuptablevg[73]=-52.1366;
lookuptablevg[74]=-52.8508;
lookuptablevg[75]=-53.565;
lookuptablevg[76]=-54.2792;
lookuptablevg[77]=-54.9934;
lookuptablevg[78]=-55.7076;
lookuptablevg[79]=-56.4218;
lookuptablevg[80]=-57.136;
lookuptablevg[81]=-57.8502;
lookuptablevg[82]=-58.5644;
lookuptablevg[83]=-59.2786;
lookuptablevg[84]=-59.9928;
lookuptablevg[85]=-60.707;
lookuptablevg[86]=-61.4212;
lookuptablevg[87]=-62.1354;
lookuptablevg[88]=-62.8496;
lookuptablevg[89]=-63.5638;
lookuptablevg[90]=-64.278;
lookuptablevg[91]=-64.9922;
lookuptablevg[92]=-65.7064;
lookuptablevg[93]=-66.4206;
lookuptablevg[94]=-67.1348;
lookuptablevg[95]=-67.849;
lookuptablevg[96]=-68.5632;
lookuptablevg[97]=-69.2774;
lookuptablevg[98]=-69.9916;
lookuptablevg[99]=-70.7058;
lookuptablevg[100]=-71.42;
lookuptablevg[101]=-72.1342;
lookuptablevg[102]=-72.8484;
lookuptablevg[103]=-73.5626;
lookuptablevg[104]=-74.2768;
lookuptablevg[105]=-74.991;
lookuptablevg[106]=-75.7052;
lookuptablevg[107]=-76.4194;
lookuptablevg[108]=-77.1336;
lookuptablevg[109]=-77.8478;
lookuptablevg[110]=-78.562;
lookuptablevg[111]=-79.2762;
lookuptablevg[112]=-79.9904;
lookuptablevg[113]=-80.7046;
lookuptablevg[114]=-81.4188;
lookuptablevg[115]=-82.133;
lookuptablevg[116]=-82.8472;
lookuptablevg[117]=-83.5614;
lookuptablevg[118]=-84.2756;
lookuptablevg[119]=-84.9898;
lookuptablevg[120]=-85.704;
lookuptablevg[121]=-86.4182;
lookuptablevg[122]=-87.1324;
lookuptablevg[123]=-87.8466;
lookuptablevg[124]=-88.5608;
lookuptablevg[125]=-89.275;
lookuptablevg[126]=-89.9892;
lookuptablevg[127]=-90.7034;
lookuptablevg[128]=-91.4176;
lookuptablevg[129]=-92.1318;
lookuptablevg[130]=-92.846;
lookuptablevg[131]=-93.5602;
lookuptablevg[132]=-94.2744;
lookuptablevg[133]=-94.9886;
lookuptablevg[134]=-95.7028;
lookuptablevg[135]=-96.417;
lookuptablevg[136]=-97.1312;
lookuptablevg[137]=-97.8454;
lookuptablevg[138]=-98.5596;
lookuptablevg[139]=-99.2738;
lookuptablevg[140]=-99.988;
lookuptablevg[141]=-100.7022;
lookuptablevg[142]=-101.4164;
lookuptablevg[143]=-102.1306;
lookuptablevg[144]=-102.8448;
lookuptablevg[145]=-103.559;
lookuptablevg[146]=-104.2732;
lookuptablevg[147]=-104.9874;
lookuptablevg[148]=-105.7016;
lookuptablevg[149]=-106.4158;
lookuptablevg[150]=-107.13;
lookuptablevg[151]=-107.8442;
lookuptablevg[152]=-108.5584;
lookuptablevg[153]=-109.2726;
lookuptablevg[154]=-109.9868;
lookuptablevg[155]=-110.701;
lookuptablevg[156]=-111.4152;
lookuptablevg[157]=-112.1294;
lookuptablevg[158]=-112.8436;
lookuptablevg[159]=-113.5578;
lookuptablevg[160]=-114.272;
lookuptablevg[161]=-114.9862;
lookuptablevg[162]=-115.7004;
lookuptablevg[163]=-116.4146;
lookuptablevg[164]=-117.1288;
lookuptablevg[165]=-117.843;
lookuptablevg[166]=-118.5572;
lookuptablevg[167]=-119.2714;
lookuptablevg[168]=-119.9856;
lookuptablevg[169]=-120.6998;
lookuptablevg[170]=-121.414;
lookuptablevg[171]=-122.1282;
lookuptablevg[172]=-122.8424;
lookuptablevg[173]=-123.5566;
lookuptablevg[174]=-124.2708;
lookuptablevg[175]=-124.985;
lookuptablevg[176]=-125.6992;
lookuptablevg[177]=-126.4134;
lookuptablevg[178]=-127.1276;
lookuptablevg[179]=-127.8418;
lookuptablevg[180]=-128.556;
lookuptablevg[181]=-129.2702;
lookuptablevg[182]=-129.9844;
lookuptablevg[183]=-130.6986;
lookuptablevg[184]=-131.4128;
lookuptablevg[185]=-132.127;
lookuptablevg[186]=-132.8412;
lookuptablevg[187]=-133.5554;
lookuptablevg[188]=-134.2696;
lookuptablevg[189]=-134.9838;
lookuptablevg[190]=-135.698;
lookuptablevg[191]=-136.4122;
lookuptablevg[192]=-137.1264;
lookuptablevg[193]=-137.8406;
lookuptablevg[194]=-138.5548;
lookuptablevg[195]=-139.269;
lookuptablevg[196]=-139.9832;
lookuptablevg[197]=-140.6974;
lookuptablevg[198]=-141.4116;
lookuptablevg[199]=-142.1258;
lookuptablevg[200]=-142.84;
lookuptablevg[201]=-143.5542;
lookuptablevg[202]=-144.2684;
lookuptablevg[203]=-144.9826;
lookuptablevg[204]=-145.6968;
lookuptablevg[205]=-146.411;
lookuptablevg[206]=-147.1252;
lookuptablevg[207]=-147.8394;
lookuptablevg[208]=-148.5536;
lookuptablevg[209]=-149.2678;
lookuptablevg[210]=-149.982;
lookuptablevg[211]=-150.6962;
lookuptablevg[212]=-151.4104;
lookuptablevg[213]=-152.1246;
lookuptablevg[214]=-152.8388;
lookuptablevg[215]=-153.553;
lookuptablevg[216]=-154.2672;
lookuptablevg[217]=-154.9814;
lookuptablevg[218]=-155.6956;
lookuptablevg[219]=-156.4098;
lookuptablevg[220]=-157.124;
lookuptablevg[221]=-157.8382;
lookuptablevg[222]=-158.5524;
lookuptablevg[223]=-159.2666;
lookuptablevg[224]=-159.9808;
lookuptablevg[225]=-160.695;
lookuptablevg[226]=-161.4092;
lookuptablevg[227]=-162.1234;
lookuptablevg[228]=-162.8376;
lookuptablevg[229]=-163.5518;
lookuptablevg[230]=-164.266;
lookuptablevg[231]=-164.9802;
lookuptablevg[232]=-165.6944;
lookuptablevg[233]=-166.4086;
lookuptablevg[234]=-167.1228;
lookuptablevg[235]=-167.837;
lookuptablevg[236]=-168.5512;
lookuptablevg[237]=-169.2654;
lookuptablevg[238]=-169.9796;
lookuptablevg[239]=-170.6938;
lookuptablevg[240]=-171.408;
lookuptablevg[241]=-172.1222;
lookuptablevg[242]=-172.8364;
lookuptablevg[243]=-173.5506;
lookuptablevg[244]=-174.2648;
lookuptablevg[245]=-174.979;
lookuptablevg[246]=-175.6932;
lookuptablevg[247]=-176.4074;
lookuptablevg[248]=-177.1216;
lookuptablevg[249]=-177.8358;
lookuptablevg[250]=-178.55;
lookuptablevg[251]=-179.2642;
lookuptablevg[252]=-179.9784;
lookuptablevg[253]=-180.6926;
lookuptablevg[254]=-181.4068;
lookuptablevg[255]=-182.121;
lookuptablevg[256]=-182.8352;


lookuptableub[0]=0;
lookuptableub[1]=1.7722;
lookuptableub[2]=3.5444;
lookuptableub[3]=5.3166;
lookuptableub[4]=7.0888;
lookuptableub[5]=8.861;
lookuptableub[6]=10.6332;
lookuptableub[7]=12.4054;
lookuptableub[8]=14.1776;
lookuptableub[9]=15.9498;
lookuptableub[10]=17.722;
lookuptableub[11]=19.4942;
lookuptableub[12]=21.2664;
lookuptableub[13]=23.0386;
lookuptableub[14]=24.8108;
lookuptableub[15]=26.583;
lookuptableub[16]=28.3552;
lookuptableub[17]=30.1274;
lookuptableub[18]=31.8996;
lookuptableub[19]=33.6718;
lookuptableub[20]=35.444;
lookuptableub[21]=37.2162;
lookuptableub[22]=38.9884;
lookuptableub[23]=40.7606;
lookuptableub[24]=42.5328;
lookuptableub[25]=44.305;
lookuptableub[26]=46.0772;
lookuptableub[27]=47.8494;
lookuptableub[28]=49.6216;
lookuptableub[29]=51.3938;
lookuptableub[30]=53.166;
lookuptableub[31]=54.9382;
lookuptableub[32]=56.7104;
lookuptableub[33]=58.4826;
lookuptableub[34]=60.2548;
lookuptableub[35]=62.027;
lookuptableub[36]=63.7992;
lookuptableub[37]=65.5714;
lookuptableub[38]=67.3436;
lookuptableub[39]=69.1158;
lookuptableub[40]=70.888;
lookuptableub[41]=72.6602;
lookuptableub[42]=74.4324;
lookuptableub[43]=76.2046;
lookuptableub[44]=77.9768;
lookuptableub[45]=79.749;
lookuptableub[46]=81.5212;
lookuptableub[47]=83.2934;
lookuptableub[48]=85.0656;
lookuptableub[49]=86.8378;
lookuptableub[50]=88.61;
lookuptableub[51]=90.3822;
lookuptableub[52]=92.1544;
lookuptableub[53]=93.9266;
lookuptableub[54]=95.6988;
lookuptableub[55]=97.471;
lookuptableub[56]=99.2432;
lookuptableub[57]=101.0154;
lookuptableub[58]=102.7876;
lookuptableub[59]=104.5598;
lookuptableub[60]=106.332;
lookuptableub[61]=108.1042;
lookuptableub[62]=109.8764;
lookuptableub[63]=111.6486;
lookuptableub[64]=113.4208;
lookuptableub[65]=115.193;
lookuptableub[66]=116.9652;
lookuptableub[67]=118.7374;
lookuptableub[68]=120.5096;
lookuptableub[69]=122.2818;
lookuptableub[70]=124.054;
lookuptableub[71]=125.8262;
lookuptableub[72]=127.5984;
lookuptableub[73]=129.3706;
lookuptableub[74]=131.1428;
lookuptableub[75]=132.915;
lookuptableub[76]=134.6872;
lookuptableub[77]=136.4594;
lookuptableub[78]=138.2316;
lookuptableub[79]=140.0038;
lookuptableub[80]=141.776;
lookuptableub[81]=143.5482;
lookuptableub[82]=145.3204;
lookuptableub[83]=147.0926;
lookuptableub[84]=148.8648;
lookuptableub[85]=150.637;
lookuptableub[86]=152.4092;
lookuptableub[87]=154.1814;
lookuptableub[88]=155.9536;
lookuptableub[89]=157.7258;
lookuptableub[90]=159.498;
lookuptableub[91]=161.2702;
lookuptableub[92]=163.0424;
lookuptableub[93]=164.8146;
lookuptableub[94]=166.5868;
lookuptableub[95]=168.359;
lookuptableub[96]=170.1312;
lookuptableub[97]=171.9034;
lookuptableub[98]=173.6756;
lookuptableub[99]=175.4478;
lookuptableub[100]=177.22;
lookuptableub[101]=178.9922;
lookuptableub[102]=180.7644;
lookuptableub[103]=182.5366;
lookuptableub[104]=184.3088;
lookuptableub[105]=186.081;
lookuptableub[106]=187.8532;
lookuptableub[107]=189.6254;
lookuptableub[108]=191.3976;
lookuptableub[109]=193.1698;
lookuptableub[110]=194.942;
lookuptableub[111]=196.7142;
lookuptableub[112]=198.4864;
lookuptableub[113]=200.2586;
lookuptableub[114]=202.0308;
lookuptableub[115]=203.803;
lookuptableub[116]=205.5752;
lookuptableub[117]=207.3474;
lookuptableub[118]=209.1196;
lookuptableub[119]=210.8918;
lookuptableub[120]=212.664;
lookuptableub[121]=214.4362;
lookuptableub[122]=216.2084;
lookuptableub[123]=217.9806;
lookuptableub[124]=219.7528;
lookuptableub[125]=221.525;
lookuptableub[126]=223.2972;
lookuptableub[127]=225.0694;
lookuptableub[128]=226.8416;
lookuptableub[129]=228.6138;
lookuptableub[130]=230.386;
lookuptableub[131]=232.1582;
lookuptableub[132]=233.9304;
lookuptableub[133]=235.7026;
lookuptableub[134]=237.4748;
lookuptableub[135]=239.247;
lookuptableub[136]=241.0192;
lookuptableub[137]=242.7914;
lookuptableub[138]=244.5636;
lookuptableub[139]=246.3358;
lookuptableub[140]=248.108;
lookuptableub[141]=249.8802;
lookuptableub[142]=251.6524;
lookuptableub[143]=253.4246;
lookuptableub[144]=255.1968;
lookuptableub[145]=256.969;
lookuptableub[146]=258.7412;
lookuptableub[147]=260.5134;
lookuptableub[148]=262.2856;
lookuptableub[149]=264.0578;
lookuptableub[150]=265.83;
lookuptableub[151]=267.6022;
lookuptableub[152]=269.3744;
lookuptableub[153]=271.1466;
lookuptableub[154]=272.9188;
lookuptableub[155]=274.691;
lookuptableub[156]=276.4632;
lookuptableub[157]=278.2354;
lookuptableub[158]=280.0076;
lookuptableub[159]=281.7798;
lookuptableub[160]=283.552;
lookuptableub[161]=285.3242;
lookuptableub[162]=287.0964;
lookuptableub[163]=288.8686;
lookuptableub[164]=290.6408;
lookuptableub[165]=292.413;
lookuptableub[166]=294.1852;
lookuptableub[167]=295.9574;
lookuptableub[168]=297.7296;
lookuptableub[169]=299.5018;
lookuptableub[170]=301.274;
lookuptableub[171]=303.0462;
lookuptableub[172]=304.8184;
lookuptableub[173]=306.5906;
lookuptableub[174]=308.3628;
lookuptableub[175]=310.135;
lookuptableub[176]=311.9072;
lookuptableub[177]=313.6794;
lookuptableub[178]=315.4516;
lookuptableub[179]=317.2238;
lookuptableub[180]=318.996;
lookuptableub[181]=320.7682;
lookuptableub[182]=322.5404;
lookuptableub[183]=324.3126;
lookuptableub[184]=326.0848;
lookuptableub[185]=327.857;
lookuptableub[186]=329.6292;
lookuptableub[187]=331.4014;
lookuptableub[188]=333.1736;
lookuptableub[189]=334.9458;
lookuptableub[190]=336.718;
lookuptableub[191]=338.4902;
lookuptableub[192]=340.2624;
lookuptableub[193]=342.0346;
lookuptableub[194]=343.8068;
lookuptableub[195]=345.579;
lookuptableub[196]=347.3512;
lookuptableub[197]=349.1234;
lookuptableub[198]=350.8956;
lookuptableub[199]=352.6678;
lookuptableub[200]=354.44;
lookuptableub[201]=356.2122;
lookuptableub[202]=357.9844;
lookuptableub[203]=359.7566;
lookuptableub[204]=361.5288;
lookuptableub[205]=363.301;
lookuptableub[206]=365.0732;
lookuptableub[207]=366.8454;
lookuptableub[208]=368.6176;
lookuptableub[209]=370.3898;
lookuptableub[210]=372.162;
lookuptableub[211]=373.9342;
lookuptableub[212]=375.7064;
lookuptableub[213]=377.4786;
lookuptableub[214]=379.2508;
lookuptableub[215]=381.023;
lookuptableub[216]=382.7952;
lookuptableub[217]=384.5674;
lookuptableub[218]=386.3396;
lookuptableub[219]=388.1118;
lookuptableub[220]=389.884;
lookuptableub[221]=391.6562;
lookuptableub[222]=393.4284;
lookuptableub[223]=395.2006;
lookuptableub[224]=396.9728;
lookuptableub[225]=398.745;
lookuptableub[226]=400.5172;
lookuptableub[227]=402.2894;
lookuptableub[228]=404.0616;
lookuptableub[229]=405.8338;
lookuptableub[230]=407.606;
lookuptableub[231]=409.3782;
lookuptableub[232]=411.1504;
lookuptableub[233]=412.9226;
lookuptableub[234]=414.6948;
lookuptableub[235]=416.467;
lookuptableub[236]=418.2392;
lookuptableub[237]=420.0114;
lookuptableub[238]=421.7836;
lookuptableub[239]=423.5558;
lookuptableub[240]=425.328;
lookuptableub[241]=427.1002;
lookuptableub[242]=428.8724;
lookuptableub[243]=430.6446;
lookuptableub[244]=432.4168;
lookuptableub[245]=434.189;
lookuptableub[246]=435.9612;
lookuptableub[247]=437.7334;
lookuptableub[248]=439.5056;
lookuptableub[249]=441.2778;
lookuptableub[250]=443.05;
lookuptableub[251]=444.8222;
lookuptableub[252]=446.5944;
lookuptableub[253]=448.3666;
lookuptableub[254]=450.1388;
lookuptableub[255]=451.911;
lookuptableub[256]=453.6832;



lookuptablevb[0]=0;
lookuptablevb[1]=0.001;
lookuptablevb[2]=0.002;
lookuptablevb[3]=0.003;
lookuptablevb[4]=0.004;
lookuptablevb[5]=0.005;
lookuptablevb[6]=0.006;
lookuptablevb[7]=0.007;
lookuptablevb[8]=0.008;
lookuptablevb[9]=0.009;
lookuptablevb[10]=0.01;
lookuptablevb[11]=0.011;
lookuptablevb[12]=0.012;
lookuptablevb[13]=0.013;
lookuptablevb[14]=0.014;
lookuptablevb[15]=0.015;
lookuptablevb[16]=0.016;
lookuptablevb[17]=0.017;
lookuptablevb[18]=0.018;
lookuptablevb[19]=0.019;
lookuptablevb[20]=0.02;
lookuptablevb[21]=0.021;
lookuptablevb[22]=0.022;
lookuptablevb[23]=0.023;
lookuptablevb[24]=0.024;
lookuptablevb[25]=0.025;
lookuptablevb[26]=0.026;
lookuptablevb[27]=0.027;
lookuptablevb[28]=0.028;
lookuptablevb[29]=0.029;
lookuptablevb[30]=0.03;
lookuptablevb[31]=0.031;
lookuptablevb[32]=0.032;
lookuptablevb[33]=0.033;
lookuptablevb[34]=0.034;
lookuptablevb[35]=0.035;
lookuptablevb[36]=0.036;
lookuptablevb[37]=0.037;
lookuptablevb[38]=0.038;
lookuptablevb[39]=0.039;
lookuptablevb[40]=0.04;
lookuptablevb[41]=0.041;
lookuptablevb[42]=0.042;
lookuptablevb[43]=0.043;
lookuptablevb[44]=0.044;
lookuptablevb[45]=0.045;
lookuptablevb[46]=0.046;
lookuptablevb[47]=0.047;
lookuptablevb[48]=0.048;
lookuptablevb[49]=0.049;
lookuptablevb[50]=0.05;
lookuptablevb[51]=0.051;
lookuptablevb[52]=0.052;
lookuptablevb[53]=0.053;
lookuptablevb[54]=0.054;
lookuptablevb[55]=0.055;
lookuptablevb[56]=0.056;
lookuptablevb[57]=0.057;
lookuptablevb[58]=0.058;
lookuptablevb[59]=0.059;
lookuptablevb[60]=0.06;
lookuptablevb[61]=0.061;
lookuptablevb[62]=0.062;
lookuptablevb[63]=0.063;
lookuptablevb[64]=0.064;
lookuptablevb[65]=0.065;
lookuptablevb[66]=0.066;
lookuptablevb[67]=0.067;
lookuptablevb[68]=0.068;
lookuptablevb[69]=0.069;
lookuptablevb[70]=0.07;
lookuptablevb[71]=0.071;
lookuptablevb[72]=0.072;
lookuptablevb[73]=0.073;
lookuptablevb[74]=0.074;
lookuptablevb[75]=0.075;
lookuptablevb[76]=0.076;
lookuptablevb[77]=0.077;
lookuptablevb[78]=0.078;
lookuptablevb[79]=0.079;
lookuptablevb[80]=0.08;
lookuptablevb[81]=0.081;
lookuptablevb[82]=0.082;
lookuptablevb[83]=0.083;
lookuptablevb[84]=0.084;
lookuptablevb[85]=0.085;
lookuptablevb[86]=0.086;
lookuptablevb[87]=0.087;
lookuptablevb[88]=0.088;
lookuptablevb[89]=0.089;
lookuptablevb[90]=0.09;
lookuptablevb[91]=0.091;
lookuptablevb[92]=0.092;
lookuptablevb[93]=0.093;
lookuptablevb[94]=0.094;
lookuptablevb[95]=0.095;
lookuptablevb[96]=0.096;
lookuptablevb[97]=0.097;
lookuptablevb[98]=0.098;
lookuptablevb[99]=0.099;
lookuptablevb[100]=0.1;
lookuptablevb[101]=0.101;
lookuptablevb[102]=0.102;
lookuptablevb[103]=0.103;
lookuptablevb[104]=0.104;
lookuptablevb[105]=0.105;
lookuptablevb[106]=0.106;
lookuptablevb[107]=0.107;
lookuptablevb[108]=0.108;
lookuptablevb[109]=0.109;
lookuptablevb[110]=0.11;
lookuptablevb[111]=0.111;
lookuptablevb[112]=0.112;
lookuptablevb[113]=0.113;
lookuptablevb[114]=0.114;
lookuptablevb[115]=0.115;
lookuptablevb[116]=0.116;
lookuptablevb[117]=0.117;
lookuptablevb[118]=0.118;
lookuptablevb[119]=0.119;
lookuptablevb[120]=0.12;
lookuptablevb[121]=0.121;
lookuptablevb[122]=0.122;
lookuptablevb[123]=0.123;
lookuptablevb[124]=0.124;
lookuptablevb[125]=0.125;
lookuptablevb[126]=0.126;
lookuptablevb[127]=0.127;
lookuptablevb[128]=0.128;
lookuptablevb[129]=0.129;
lookuptablevb[130]=0.13;
lookuptablevb[131]=0.131;
lookuptablevb[132]=0.132;
lookuptablevb[133]=0.133;
lookuptablevb[134]=0.134;
lookuptablevb[135]=0.135;
lookuptablevb[136]=0.136;
lookuptablevb[137]=0.137;
lookuptablevb[138]=0.138;
lookuptablevb[139]=0.139;
lookuptablevb[140]=0.14;
lookuptablevb[141]=0.141;
lookuptablevb[142]=0.142;
lookuptablevb[143]=0.143;
lookuptablevb[144]=0.144;
lookuptablevb[145]=0.145;
lookuptablevb[146]=0.146;
lookuptablevb[147]=0.147;
lookuptablevb[148]=0.148;
lookuptablevb[149]=0.149;
lookuptablevb[150]=0.15;
lookuptablevb[151]=0.151;
lookuptablevb[152]=0.152;
lookuptablevb[153]=0.153;
lookuptablevb[154]=0.154;
lookuptablevb[155]=0.155;
lookuptablevb[156]=0.156;
lookuptablevb[157]=0.157;
lookuptablevb[158]=0.158;
lookuptablevb[159]=0.159;
lookuptablevb[160]=0.16;
lookuptablevb[161]=0.161;
lookuptablevb[162]=0.162;
lookuptablevb[163]=0.163;
lookuptablevb[164]=0.164;
lookuptablevb[165]=0.165;
lookuptablevb[166]=0.166;
lookuptablevb[167]=0.167;
lookuptablevb[168]=0.168;
lookuptablevb[169]=0.169;
lookuptablevb[170]=0.17;
lookuptablevb[171]=0.171;
lookuptablevb[172]=0.172;
lookuptablevb[173]=0.173;
lookuptablevb[174]=0.174;
lookuptablevb[175]=0.175;
lookuptablevb[176]=0.176;
lookuptablevb[177]=0.177;
lookuptablevb[178]=0.178;
lookuptablevb[179]=0.179;
lookuptablevb[180]=0.18;
lookuptablevb[181]=0.181;
lookuptablevb[182]=0.182;
lookuptablevb[183]=0.183;
lookuptablevb[184]=0.184;
lookuptablevb[185]=0.185;
lookuptablevb[186]=0.186;
lookuptablevb[187]=0.187;
lookuptablevb[188]=0.188;
lookuptablevb[189]=0.189;
lookuptablevb[190]=0.19;
lookuptablevb[191]=0.191;
lookuptablevb[192]=0.192;
lookuptablevb[193]=0.193;
lookuptablevb[194]=0.194;
lookuptablevb[195]=0.195;
lookuptablevb[196]=0.196;
lookuptablevb[197]=0.197;
lookuptablevb[198]=0.198;
lookuptablevb[199]=0.199;
lookuptablevb[200]=0.2;
lookuptablevb[201]=0.201;
lookuptablevb[202]=0.202;
lookuptablevb[203]=0.203;
lookuptablevb[204]=0.204;
lookuptablevb[205]=0.205;
lookuptablevb[206]=0.206;
lookuptablevb[207]=0.207;
lookuptablevb[208]=0.208;
lookuptablevb[209]=0.209;
lookuptablevb[210]=0.21;
lookuptablevb[211]=0.211;
lookuptablevb[212]=0.212;
lookuptablevb[213]=0.213;
lookuptablevb[214]=0.214;
lookuptablevb[215]=0.215;
lookuptablevb[216]=0.216;
lookuptablevb[217]=0.217;
lookuptablevb[218]=0.218;
lookuptablevb[219]=0.219;
lookuptablevb[220]=0.22;
lookuptablevb[221]=0.221;
lookuptablevb[222]=0.222;
lookuptablevb[223]=0.223;
lookuptablevb[224]=0.224;
lookuptablevb[225]=0.225;
lookuptablevb[226]=0.226;
lookuptablevb[227]=0.227;
lookuptablevb[228]=0.228;
lookuptablevb[229]=0.229;
lookuptablevb[230]=0.23;
lookuptablevb[231]=0.231;
lookuptablevb[232]=0.232;
lookuptablevb[233]=0.233;
lookuptablevb[234]=0.234;
lookuptablevb[235]=0.235;
lookuptablevb[236]=0.236;
lookuptablevb[237]=0.237;
lookuptablevb[238]=0.238;
lookuptablevb[239]=0.239;
lookuptablevb[240]=0.24;
lookuptablevb[241]=0.241;
lookuptablevb[242]=0.242;
lookuptablevb[243]=0.243;
lookuptablevb[244]=0.244;
lookuptablevb[245]=0.245;
lookuptablevb[246]=0.246;
lookuptablevb[247]=0.247;
lookuptablevb[248]=0.248;
lookuptablevb[249]=0.249;
lookuptablevb[250]=0.25;
lookuptablevb[251]=0.251;
lookuptablevb[252]=0.252;
lookuptablevb[253]=0.253;
lookuptablevb[254]=0.254;
lookuptablevb[255]=0.255;
lookuptablevb[256]=0.256;
}
