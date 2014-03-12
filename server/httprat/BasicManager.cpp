#include "BasicManager.h"
#include "Map.h"

CBasicManager::CBasicManager(CClient* pclient)
{

	b_quit=FALSE;
	m_pClient=pclient;
	m_pClient->setManagerCallBack(this);
	m_hEvent = CreateEvent(NULL, true, false, NULL);
	
}

CBasicManager::~CBasicManager()
{
	CloseHandle(m_hEvent);
}

void CBasicManager::OnReceive(LPBYTE lpBuff,DWORD dwSize)
{
	
}
void CBasicManager::Post(LPCWSTR lpurl,char* strPostData)
{
	m_pClient->post(lpurl,strPostData);
}

void CBasicManager::WaitEvent()
{
	WaitForSingleObject(m_hEvent, INFINITE);
}