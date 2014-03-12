//描述: 通过http协议和服务器进行通信
//作者: firnsan 2012.12

#if !defined(CLIENT_H_INCLUDED)
#define CLIENT_H_INCLUDED

#include <Windows.h>
#include<tchar.h>
#include <atlstr.h>
#include "BasicManager.h"
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// CrackedUrl 
class CrackedUrl 
{     
	int m_scheme;     
	CStringW m_host;     
	int m_port;     
	CStringW m_path; public:

	CrackedUrl(LPCWSTR url)    
	{         
		URL_COMPONENTS uc = { 0};        
		uc.dwStructSize = sizeof(uc);         
		const DWORD BUF_LEN = 256;         
		WCHAR host[BUF_LEN];         
		uc.lpszHostName = host;         
		uc.dwHostNameLength = BUF_LEN;         
		WCHAR path[BUF_LEN];        
		uc.lpszUrlPath = path;        
		uc.dwUrlPathLength = BUF_LEN;         
		WCHAR extra[BUF_LEN];         
		uc.lpszExtraInfo = extra;         
		uc.dwExtraInfoLength = BUF_LEN;        
		if (!WinHttpCrackUrl(url, 0, ICU_ESCAPE, &uc))
		{             
			printf("Error:WinHttpCrackUrl failed!/n");        
		}  
		m_scheme = uc.nScheme;        
		m_host = host;        
		m_port = uc.nPort;         
		m_path = path;    
	}  

	int GetScheme() const    
	{ 
		return m_scheme;     
	}  

	LPCWSTR GetHostName() const     
	{
		return m_host;     
	}  

	int GetPort() const     
	{
		return m_port;     
	} 

	LPCWSTR GetPath() const     
	{         
		return m_path;     
	}      
	/*
	static CStringA UrlEncode(const char* p)     
	{         
			if (p == 0)
			{             return CStringA();         }         
			CStringA buf;          
			for (;;) 
			{             
				int ch = (BYTE) (*(p++));             
				if (ch == '/0') 
				{                
					break;            }           
				if (isalnum(ch) || ch == '_' || ch == '-' || ch == '.') 
				{                buf += (char)ch;            }           
				else if (ch == ' ')
				{                buf += '+';            }           
				else 
				{                
					char c[16];               
					wsprintfA(c, "%%%02X", ch);               
					buf += c;            }   
			}        
			return buf;    
	}*/
	
};




class CClient
{
	friend class CBasicManager;
	CBasicManager* m_pManager;

public:	
	CClient();
	~CClient();
	
	int post(LPCWSTR lpurl,char* strPostData);
	int post(LPCWSTR lpurl,char* strPostData,DWORD dwSize);
	void setManagerCallBack(CBasicManager *pManager);
	unsigned char char_to_hex(unsigned char x);
	int is_alpha_number_char( unsigned char c );
	DWORD urlencode( unsigned char * src, int  src_len, unsigned char * dest, int  dest_len );
};


#endif