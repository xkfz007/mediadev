
#include<windows.h>
#include<math.h>
#include"stdio.h"
int maintest();
#pragma comment(lib,"ws2_32.lib")

#define SERVER_PORT 1111	
#define WM_SOCKET (WM_USER+1)
#define MEDIA_WIDTH		352
#define MEDIA_HEIGHT	288
#define MAX_BUF			MEDIA_WIDTH*MEDIA_HEIGHT*2		//文件大小
#define RGB_BUF			MAX_BUF*2
#define SCREEN_X		600
#define	SCREEN_Y		450
#define SLEEP_TIME		40
#define	FLAGE			300
#define max(a,b)        (((a) > (b)) ? (a) : (b))
#define ID_RED                  1
#define ID_RED1                  2
#define ID_RED2                 3
bool play();
FILE *fp;
BYTE *test;
static HWND		hwnd;
static HWND		sLoad;
static HWND		sPlay;					//button hwnd
static HWND		sCancel;
static HWND		chilren_hwnd;

HINSTANCE	g_hInstance = NULL;
HANDLE		g_hFile = NULL;
BOOL   g_stop = false;

bool ToPlayMedia();						
LRESULT CALLBACK WndProc ( HWND hwnd,  
					   UINT message, 
					   WPARAM wParam, 
					   LPARAM lParam );

int WINAPI WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    PSTR      szCmdLine,
                    int       iCmdShow )
{
	
	MSG msg;
	static TCHAR szAppName[] = TEXT("empty_window");
	g_hInstance = hInstance;
	WNDCLASS wndclass;

	wndclass.style			=	CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc	=	WndProc;
	wndclass.cbClsExtra		=	0;
	wndclass.cbWndExtra		=	0;
	wndclass.hInstance		=	hInstance;
	wndclass.hIcon			=	LoadIcon( NULL,IDI_APPLICATION );
	wndclass.hCursor		=	LoadCursor( NULL,IDC_ARROW );
	wndclass.hbrBackground	=	( HBRUSH )GetStockObject( WHITE_BRUSH );
	wndclass.lpszMenuName	=	NULL;
	wndclass.lpszClassName	= szAppName;

	if( !RegisterClass( &wndclass ) ){
		MessageBox( NULL,TEXT("register error"),szAppName,MB_ICONERROR );
		return false;
	}


	hwnd = CreateWindow( szAppName,						// window class name       
						 TEXT ("yuyv"),	// window caption       
						 WS_OVERLAPPED     | \
                         WS_CAPTION        | \
                         WS_SYSMENU        | \
                         WS_THICKFRAME,			// window style       
						 250,								// initial x position       
						 200,								// initial y position        
						 SCREEN_X,					// initial x size      
						 SCREEN_Y,					// initial y size      
						 NULL,							 // parent window handle        
						 NULL,							// window menu handle
						 hInstance,						// program instance handle
						 NULL ) ;						// creation parameters

	ShowWindow( hwnd,SW_SHOW );
	UpdateWindow( hwnd );

	while( GetMessage( &msg,NULL,0,0 ) ){
			TranslateMessage ( &msg );
			DispatchMessage ( &msg );
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc ( HWND hwnd, 
						   UINT message, 
						   WPARAM wParam, 
						   LPARAM lParam )
{
	HDC hdc;
	PAINTSTRUCT ps ;
    HINSTANCE		hInstance;
	static HWND		hwndReceiveEdit, hwndSendEdit;
	
	WSADATA			WSAData;
	static struct	sockaddr_in server,from;
	static SOCKET	local_server, local_listen;
	int				nError;
	WORD			wEvent;
	

	switch (message)   
    {
		case	WM_CREATE:
			
			 sLoad = CreateWindow( "BUTTON",		//create button window
									   TEXT("load"),
							   		   WS_VISIBLE   |
									   WS_CHILD     |
									   WS_BORDER    | 
									   ES_LEFT,
									   30,		
									   200,	
									   80,
									   20,
									   hwnd,                  
									   (HMENU) ID_RED,   
									   ( HINSTANCE )GetWindowLong( hwnd,GWL_HINSTANCE ),     
										NULL); 
			 sCancel = CreateWindow(  "BUTTON",		//create button window
									   TEXT("play"),
							   		   WS_VISIBLE   |
									   WS_CHILD     |
									   WS_BORDER    | 
									   ES_LEFT,
									   30,		
									   250,	
									   80,
									   20,
									   hwnd,                  
									   (HMENU) ID_RED1,   
									   ( HINSTANCE )GetWindowLong( hwnd,GWL_HINSTANCE ),     
										NULL); 
		
				 sPlay = CreateWindow(  "BUTTON",		//create button window
									   TEXT("cancle"),
							   		   WS_VISIBLE   |
									   WS_CHILD     |
									   WS_BORDER    | 
									   ES_LEFT,
									   30,		
									   300,	
									   80,
									   20,
									   hwnd,                  
									   (HMENU) ID_RED2,   
									   ( HINSTANCE )GetWindowLong( hwnd,GWL_HINSTANCE ),     
										NULL); 
			hInstance = (HINSTANCE) GetWindowLong( hwnd, GWL_HINSTANCE );

	

		if( nError = WSAStartup( MAKEWORD(2,0), &WSAData ) )
		{
			MessageBox( hwnd, "WinSock Startup Error!", "Server", MB_ICONERROR | MB_OK );
			return TRUE;
		}

		local_server = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
		if( local_server == INVALID_SOCKET )
		{
			MessageBox( hwnd, "Socket Creation Error!", "Server", MB_ICONERROR | MB_OK );
			
			WSACleanup();
			return TRUE;
		}

		if( SOCKET_ERROR == WSAAsyncSelect( local_server, hwnd, 
			WM_SOCKET, FD_ACCEPT | FD_READ ) )
		{
			MessageBox( hwnd, "WSAAsyncSelect Error!", "Server", MB_ICONERROR | MB_OK );
			
			closesocket( local_server );
			WSACleanup();
			return TRUE;
		}

		server.sin_family = AF_INET;
		server.sin_port = htons( SERVER_PORT );
		if( SOCKET_ERROR == bind( local_server, (struct sockaddr *)&server, sizeof(server) ) )
		{
			MessageBox( hwnd, "Bind Error!", "Server", MB_ICONERROR | MB_OK );

			closesocket( local_server );
			WSACleanup();
			return TRUE;
		}

		listen( local_server, 5 );
	        return 0; 

		case	WM_PAINT:
			hdc = BeginPaint (hwnd, &ps) ;
	EndPaint (hwnd, &ps) ;
			return 0;


	case WM_SOCKET:
		wEvent = WSAGETSELECTEVENT(lParam);

	  switch( wEvent )
		{
		case FD_ACCEPT:
			if( wEvent )
			{
			play();
			 break;
			}
		}
		return 0;
		case	WM_COMMAND:
	switch (wParam)
        
		{
        case 	ID_RED :
                 ToPlayMedia();
				 break;
        case    ID_RED1 :
			g_stop = false;
             play();
			 break;
		case    ID_RED2 :
		/*	if( g_hFile )
				{
					CloseHandle(g_hFile);
					g_hFile = NULL;
				}*/
			g_stop = true;
			break;
       
		}
			return 0;
	case	WM_DESTROY:
	
            PostQuitMessage ( 0 ) ;
        
            return 0 ;    
    }
        
	return DefWindowProc ( hwnd, message, wParam, lParam ) ;

}

//-------------------------------------------------------------------------------
unsigned char RESET_RGB( int i )								//RGB调整
{
	if( i < 0 )
		return  0;
	if( i > 255 )
		return  255;
	return (unsigned char)i;
}

//--------------------------------------------------------------------------------------------------------

void ConvertDataFromYUYVToRGB32( unsigned char dData[], 
								 unsigned char sData[], 
								 int width, int height )   //YUV TO RGB
{
 	DWORD   dPos = 0, sPos = 0;
	for(int y=0;y<height;y++){
		dPos = y * width * 4;
		for( int x = 0; x < width; x += 2 ) { 
			dData[dPos++] = RESET_RGB( (int)(sData[sPos]/*Y0*/   + (sData[sPos+1]/*U*/-128)*1.772) );//B0				  
			dData[dPos++] = RESET_RGB( (int)(sData[sPos]/*Y0*/   - (sData[sPos+1]/*U*/-128)*0.34414 - (sData[sPos+3]/*V*/- 128)*0.71414) ); //G0
			dData[dPos++] = RESET_RGB( (int)(sData[sPos]/*Y0*/   + (sData[sPos+3]/*V*/-128)*1.402) ); //R0 
			dData[dPos++] = '0';
			dData[dPos++] = RESET_RGB( (int)(sData[sPos+2]/*Y1*/ + (sData[sPos+1]/*U*/-128)*1.772 ) );//B1				
			dData[dPos++] = RESET_RGB( (int)(sData[sPos+2]/*Y1*/ - (sData[sPos+1]/*U*/-128)*0.34414 - (sData[sPos+3]/*V*/- 128)*0.71414) ); //G1
			dData[dPos++] = RESET_RGB( (int)(sData[sPos+2]/*Y1*/ + (sData[sPos+3]/*V*/-128)*1.402) ); //R1   
			dData[dPos++] = '0';
			sPos += 4;
		}
	}
}



DWORD WINAPI ThreadYUVToRGB(LPVOID lpThreadParameter)		
{
	unsigned char *buf;
	unsigned char *tmp_buf;
	

	FILE* fp = fopen("foreman.yuyv","r");				//打开文件
	if( !fp ){	
		printf( "file open failed!\n" );	
		exit(0);
	}

	DWORD read = 0;
	int index = 0;
	int com=0;
	buf = (unsigned char *) malloc ( sizeof( unsigned char ) *MAX_BUF );
	tmp_buf = (unsigned char *) malloc ( sizeof( unsigned char ) *MAX_BUF*2 );

	while(1 & index<FLAGE){
		while( g_stop ){
			Sleep(100);
		}
		com = index%100;
		index ++;
		

		ReadFile(g_hFile,buf,MAX_BUF,&read,NULL);
		if(read!=MAX_BUF){
			break;
			return 0;
		}
		ConvertDataFromYUYVToRGB32(tmp_buf,buf,MEDIA_WIDTH,MEDIA_HEIGHT);

   
        HDC hdc;
	    HBITMAP hbitmap;
	    hdc = GetDC (hwnd) ;
		HDC memHdc = CreateCompatibleDC( hdc );
		hbitmap=CreateBitmap( 352,288,1,32, tmp_buf);
		SelectObject( memHdc, hbitmap );
		BITMAP bitmap;
		GetObject( hbitmap, sizeof( BITMAP ), &bitmap );
		
		BitBlt( hdc, 150,60, MEDIA_WIDTH,MEDIA_HEIGHT, memHdc, 0,0, SRCCOPY );
        ReleaseDC (hwnd,  memHdc) ;
	
		ValidateRect( hwnd, NULL );		//刷新
		Sleep(SLEEP_TIME);
	}


}
bool ToPlayMedia(){							//创建线程播放
	
	char    file[128] = {0};				
	char	file_title[24] = {0};
	
	char filter[64] = {0};
	strcpy( filter, "yuyv files (.yuyv)" );
	strcpy( filter+strlen("yuyv files (.yuyv)")+1, "*.yuyv" );// 

	OPENFILENAME  file_info;	
	file_info.lStructSize       = sizeof(OPENFILENAME);
	file_info.hwndOwner         = NULL;     //m_hWnd;
	file_info.hInstance         = g_hInstance;
	file_info.lpstrFilter       = filter;    
	file_info.lpstrCustomFilter = NULL;     
	file_info.nMaxCustFilter    = 0;
	file_info.nFilterIndex      = 0;
	file_info.lpstrFile         = file;     //point to file
	file_info.nMaxFile          = sizeof(file);
	file_info.lpstrFileTitle    = file_title;
	file_info.nMaxFileTitle     = sizeof(file_title);
	file_info.lpstrInitialDir   = NULL;
	file_info.lpstrTitle        = "Select File";	
	file_info.nFileOffset       = 0;
	file_info.nFileExtension    = 0;
	file_info.lpstrDefExt       = NULL;
	file_info.lCustData         = NULL;    //
	file_info.lpfnHook 		  = NULL;
	file_info.lpTemplateName    = NULL;
	file_info.Flags             =  OFN_EXPLORER | OFN_PATHMUSTEXIST;
	
	if( GetOpenFileName( &file_info ) == 0 )
		return false;

	g_hFile =CreateFile(file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);	//open file

	if( g_hFile == INVALID_HANDLE_VALUE ){
		MessageBox(NULL, "Open file failed !", NULL, MB_OK);
		return false;
	}
}
bool play()
{
   
   
	if(CreateThread(NULL, 0,ThreadYUVToRGB, NULL, 0, NULL)==0){
		MessageBox(NULL,"Open file error",NULL,MB_OK);
		CloseHandle(g_hFile);
		g_hFile = NULL;
		return false;
	}
}

/*int maintest() 
{ 
	WORD wVersionRequested; 
	WSADATA wsaData; 
	int err; 
    
	wVersionRequested = MAKEWORD( 1, 1 ); 
    
	err = WSAStartup( wVersionRequested, &wsaData ); 
	if ( err != 0 ) { 
		return 1; 
	} 
    
	if ( LOBYTE( wsaData.wVersion ) != 1 || 
		HIBYTE( wsaData.wVersion ) != 1 )    
	{                                                                
		WSACleanup( ); 
		return 1;    
	} 
	
	SOCKET socket_srv = socket(AF_INET,SOCK_STREAM,0); 
	struct sockaddr_in        soc_addrs,soc_addrc ; 
	int    soc_lens = sizeof(sockaddr); 
	int    soc_lenc = sizeof(sockaddr); 
	char buffer[100]; 
	soc_addrs.sin_addr.S_un.S_addr = inet_addr("192.168.10.51"); 
	soc_addrs.sin_family = AF_INET; 
	soc_addrs.sin_port = htons(6000); 
	bind(socket_srv,(SOCKADDR *) &soc_addrs,soc_lenc); 
	listen(socket_srv,10); 
	while ( 1 ) 
	{ 
		SOCKET socket_client = accept(socket_srv,(SOCKADDR*)&soc_addrc,&soc_lenc); 
		recv(socket_client,buffer,100,0); 
		if(buffer=="1") 
			{
				if( g_hFile )
				{
					CloseHandle(g_hFile);
					g_hFile = NULL;
				}
				break;
			}
		closesocket( socket_client); 
	} 
	
	WSACleanup(); 
	return 0; 
}*/