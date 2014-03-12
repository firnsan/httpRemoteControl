//描述: 从服务器下载文件
//作者: firnsan 2012.12

#if !defined(DOWNLOAD_H_INCLUDED)
#define DOWNLOAD_H_INCLUDED


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BasicManager.h"
//class CBasicManager;
class CDownload : public CBasicManager  
{
public:
	CDownload(CClient *pClient);
	virtual ~CDownload();
	virtual void OnReceive(LPBYTE lpBuff,DWORD dwSize);

};
#endif