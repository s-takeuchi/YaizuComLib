#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <shlwapi.h>
#include "..\commonfunc\StkProperties.h"
#include "..\commonfunc\StkStringParser.h"
#include "..\stksocket\stksocket.h"

#define SERVICE_NAME (TEXT("StkWebAppCmd"))

DWORD WINAPI HandlerEx(DWORD, DWORD, PVOID, PVOID);
VOID WINAPI ServiceMain(DWORD dwArgc, PTSTR* pszArgv);
void StartProcesses();
void StopProcesses();
BOOL CALLBACK EnumWindowsProc(HWND, LPARAM);

SERVICE_TABLE_ENTRY ServiceTable[] = {
	{ SERVICE_NAME, ServiceMain },
	{ NULL, NULL }
};

BOOL g_bRun = TRUE;
BOOL g_bService = TRUE;
SERVICE_STATUS_HANDLE g_hServiceStatus = NULL;

/////////////////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    PROCESS_INFORMATION* pi = (PROCESS_INFORMATION*)lParam;

    DWORD lpdwProcessId = 0;
    GetWindowThreadProcessId(hWnd, &lpdwProcessId);
    if (pi->dwProcessId == lpdwProcessId) {
        PostMessage(hWnd, WM_CLOSE, 0, 0);
        return FALSE;
    }
    return TRUE;
}

void StartProcesses()
{
	TCHAR Buf[256];
	TCHAR CmdLine[512];
	GetModuleFileName(NULL, Buf, 255);
	LPTSTR Addr = StrStr(Buf, _T("\\stkwebappcmd.exe"));
	lstrcpy(Addr, _T(""));
	TCHAR SystemDir[MAX_PATH];
	GetSystemDirectory(SystemDir, MAX_PATH);

	SetCurrentDirectory(Buf);

	STARTUPINFO si_wapp;
	PROCESS_INFORMATION pi_wapp;
	ZeroMemory(&si_wapp, sizeof(si_wapp));
	si_wapp.cb = sizeof(si_wapp);
	wsprintf(CmdLine, _T("%s\\stkwebapp.exe"), Buf);
	CreateProcess(NULL, CmdLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si_wapp, &pi_wapp);

	STARTUPINFO si_nginx;
	PROCESS_INFORMATION pi_nginx;
	ZeroMemory(&si_nginx, sizeof(si_nginx));
	si_nginx.cb = sizeof(si_nginx);
	wsprintf(CmdLine, _T("\"%s\\nginx.exe\""), Buf); /* instead of  [cmd.exe /c "start \nginx.exe"] */
	CreateProcess(NULL, CmdLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si_nginx, &pi_nginx);
}

void StopProcesses()
{
	/***** Set current directory *****/
	TCHAR Buf[256];
	TCHAR CmdLine[512];
	GetModuleFileName(NULL, Buf, 255);
	LPTSTR Addr = StrStr(Buf, _T("\\stkwebappcmd.exe"));
	lstrcpy(Addr, _T(""));
	SetCurrentDirectory(Buf);

	/***** Stop StkWebApp *****/
	char IpAddrTmp[256] = "127.0.0.1";
	TCHAR IpAddr[256] = _T("127.0.0.1");
	int Port = 8081;
	StkProperties *Prop = new StkProperties();
	if (Prop->GetProperties(_T("stkwebapp.conf")) == 0) {
		Prop->GetPropertyStr("servicehost", IpAddrTmp);
		wsprintf(IpAddr, _T("%S"), IpAddrTmp);
		Prop->GetPropertyInt("serviceport", &Port);
	}
	StkSocket_AddInfo(1, SOCK_STREAM, STKSOCKET_ACTIONTYPE_SENDER, IpAddr, Port);
	if (StkSocket_Connect(1) == 0) {
		char SendDat[1024];
		char Dat[256] = "<Stop/>";
		BYTE RecvDat[1024];
		sprintf_s(SendDat, 1024, "POST /service/ HTTP/1.1\nContent-Length: %d\nContent-Type: application/xml\n\n%s", strlen(Dat), Dat);
		StkSocket_Send(1, 1, (BYTE*)SendDat, strlen((char*)SendDat));
		int RetR;
		for (int Loop = 0; Loop < 10; Loop++) {
			RetR = StkSocket_Receive(1, 1, RecvDat, 1024, 205000, NULL, -1, FALSE);
			if (RetR > 0) {
				break;
			}
		}
		StkSocket_Disconnect(1, 1, TRUE);
	}
	StkSocket_DeleteInfo(1);

	/***** Stop nginx *****/
	PROCESS_INFORMATION pi_nginxstop;
	STARTUPINFO si_nginx;
	ZeroMemory(&si_nginx, sizeof(si_nginx));
	si_nginx.cb = sizeof(si_nginx);
	wsprintf(CmdLine, _T("\"%s\\nginx.exe\" -s stop"), Buf);
	CreateProcess(NULL, CmdLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si_nginx, &pi_nginxstop);
}

DWORD WINAPI HandlerEx (DWORD dwControl, DWORD dwEventType, LPVOID lpEventData, LPVOID lpContext)
{
	// Initialize Variables for Service Control
	SERVICE_STATUS ss;
	ss.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	ss.dwWin32ExitCode = NO_ERROR;
	ss.dwServiceSpecificExitCode = 0;
	ss.dwCheckPoint = 1;
	ss.dwWaitHint = 1000;
	ss.dwControlsAccepted = 0;

	switch(dwControl) {
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:

		StopProcesses();

		// Set STOP_PENDING status.
		ss.dwCurrentState = SERVICE_STOP_PENDING;
		if (!SetServiceStatus(g_hServiceStatus, &ss)) {
			break;
		}

		// SERVICE SPECIFIC STOPPING CODE HERE.
		// ...
		// ...
		g_bService = FALSE;
		Sleep(2 * 1000);

		// Set STOPPED status.
		ss.dwCurrentState = SERVICE_STOPPED;
		ss.dwCheckPoint = 0;
		ss.dwWaitHint = 0;
		if (!SetServiceStatus(g_hServiceStatus, &ss)) {
			break;
		}
		break;

	case SERVICE_CONTROL_PAUSE:

		// Set PAUSE_PENDING status.
		ss.dwCurrentState = SERVICE_PAUSE_PENDING;
		if (!SetServiceStatus(g_hServiceStatus, &ss)) {
			break;
		}

		// APPLICATION SPECIFIC PAUSE_PENDING CODE HERE.
		// ...
		// ...
		g_bRun = FALSE;
		Sleep(2 * 1000);

		// Set PAUSE_PENDING status.
		ss.dwCurrentState = SERVICE_PAUSED;
		ss.dwCheckPoint = 0;
		ss.dwWaitHint = 0;
		ss.dwControlsAccepted =
			SERVICE_ACCEPT_PAUSE_CONTINUE |
			SERVICE_ACCEPT_SHUTDOWN |
			SERVICE_ACCEPT_STOP;
		if (!SetServiceStatus(g_hServiceStatus, &ss)) {
			break;
		}
		break;

	case SERVICE_CONTROL_CONTINUE:

		// Set PAUSE_PENDING status.
		ss.dwCurrentState = SERVICE_START_PENDING;
		if (!SetServiceStatus (g_hServiceStatus, &ss)) {
			break;
		}

		// APPLICATION SPECIFIC START_PENDING CODE HERE.
		// ...
		// ...
		g_bRun = TRUE;
		Sleep(2 * 1000);

		// Set RUNNING status.
		ss.dwCurrentState = SERVICE_RUNNING;
		ss.dwCheckPoint = 0;
		ss.dwWaitHint = 0;
		ss.dwControlsAccepted =
			SERVICE_ACCEPT_PAUSE_CONTINUE |
			SERVICE_ACCEPT_SHUTDOWN |
			SERVICE_ACCEPT_STOP;
		if (!SetServiceStatus(g_hServiceStatus, &ss)) {
			break;
		}
		break;

	default:
		return ERROR_CALL_NOT_IMPLEMENTED;
	}

	return NO_ERROR;
}

VOID WINAPI ServiceMain(DWORD dwArgc, PTSTR* pszArgv)
{
	// Initialize Variables for Service Control
	SERVICE_STATUS ss;
	ss.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	ss.dwWin32ExitCode = NO_ERROR;
	ss.dwServiceSpecificExitCode = 0;
	ss.dwCheckPoint = 1;
	ss.dwWaitHint = 1000;
	ss.dwControlsAccepted = 0;
	ss.dwCurrentState = SERVICE_START_PENDING;

	// Register Service Control Handler
	g_hServiceStatus = RegisterServiceCtrlHandlerEx(SERVICE_NAME, HandlerEx, NULL);
	if(0 == g_hServiceStatus) {
		return;
	}

	// Entering Starting Service.
	if (!SetServiceStatus(g_hServiceStatus, &ss)) {
		return;
	}

	// APPLICATION SPECIFIC INITIALIZATION CODE
	// ...
	// ...

	// Finish Initializing.
	ss.dwCurrentState = SERVICE_RUNNING;
	ss.dwCheckPoint = 0;
	ss.dwWaitHint = 0;
	ss.dwControlsAccepted = 
		SERVICE_ACCEPT_PAUSE_CONTINUE |
		SERVICE_ACCEPT_SHUTDOWN |
		SERVICE_ACCEPT_STOP;
	if (!SetServiceStatus(g_hServiceStatus, &ss)) {
		return;
	}

	StartProcesses();

	while(g_bService) {
		if(!g_bRun) {
			Sleep(1000);
			continue;
		}
		Sleep(1);
	}
}

int main(int argc, char* argv[])
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	TCHAR ThisCmdPath[256];
	GetModuleFileName(NULL, ThisCmdPath, 255);

	TCHAR WorkPath[256];
	lstrcpy(WorkPath, ThisCmdPath);
	LPTSTR Addr = StrStr(WorkPath, _T("\\stkwebappcmd.exe"));
	lstrcpy(Addr, _T(""));

	TCHAR SystemDir[MAX_PATH];
	GetSystemDirectory(SystemDir, MAX_PATH);

	/////////////////////////////////////////////////////////////
	// If no command is specified, service info is configured. //
	/////////////////////////////////////////////////////////////

	if (argc == 1) {
		BOOL bRet = StartServiceCtrlDispatcher(ServiceTable);
		printf("StkWebAppCmd (Service command for StkWebApp)\r\n");
		printf("Sepcify 'help' for the command usage\r\n");
		return 0;
	}

	///////////////////////////////////
	// Abnormal case                 //
	///////////////////////////////////

	if (strcmp(argv[1], "help") != 0 &&
		strcmp(argv[1], "modperm") != 0 &&
		strcmp(argv[1], "modconfig") != 0 &&
		strcmp(argv[1], "fwadd") != 0 &&
		strcmp(argv[1], "fwdel") != 0 &&
		strcmp(argv[1], "srvadd") != 0 &&
		strcmp(argv[1], "srvdel") != 0 &&
		strcmp(argv[1], "start") != 0 &&
		strcmp(argv[1], "stop") != 0 &&
		strcmp(argv[1], "inst") != 0 &&
		strcmp(argv[1], "uninst") != 0) {
		printf("Invalid command is specified. [%s]\r\n", argv[1]);
		return 0;
	}
	if (argc >= 3) {
		for (int Loop = 2; Loop < argc; Loop++) {
			TCHAR TargetParam[64] = _T("");
			wsprintf(TargetParam, _T("%S"), argv[Loop]);
			TCHAR TmpFetchedParam[64] = _T("");
			if (StkStringParser::ParseInto1Param(TargetParam, _T("ProductName=$"), _T('$'), TmpFetchedParam) != 1 &&
				StkStringParser::ParseInto1Param(TargetParam, _T("SrvHost=$"), _T('$'), TmpFetchedParam) != 1 &&
				StkStringParser::ParseInto1Param(TargetParam, _T("SrvPort=$"), _T('$'), TmpFetchedParam) != 1 &&
				StkStringParser::ParseInto1Param(TargetParam, _T("WebHost=$"), _T('$'), TmpFetchedParam) != 1 &&
				StkStringParser::ParseInto1Param(TargetParam, _T("WebPort=$"), _T('$'), TmpFetchedParam) != 1) {
				printf("Invalid option is specified. [%S]\r\n", TargetParam);
				return 0;
			}
		}
	}

	///////////////////////////////////
	// Help                          //
	///////////////////////////////////

	if (strcmp(argv[1], "help") == 0) {
		printf("StkWebAppCmd (Service command for StkWebApp)\r\n");
		printf("Command usage : %s <command> <option 1> <option 2> ...\r\n", argv[0]);
		printf("\r\nCommands\r\n");
		printf("  help      : Display help.\r\n");
		printf("  modperm   : Modify permission of configuration and data files.\r\n");
		printf("  modconfig : Modify configuration files. ProductName, SrvHost, SrvPort, WebHost and WebPort options are required.\r\n");
		printf("  fwadd     : Add exception to the firewall. ProductName option is required.\r\n");
		printf("  fwdel     : Delete exception from the firewall. ProductName option is required.\r\n");
		printf("  srvadd    : Add service to the system. ProductName option is required.\r\n");
		printf("  srvdel    : Delete service from the system. ProductName option is required.\r\n");
		printf("  start     : Start the service. ProductName option is required.\r\n");
		printf("  stop      : Stop the service. ProductName option is required.\r\n");
		printf("  inst      : Execute modperm, modconfig, fwadd, srvadd and start sequencially. \r\n");
		printf("  uninst    : Execute fwdel, srvdel and stop sequencially.\r\n");
		printf("\r\nOptions\r\n");
		printf("  ProductName : Product name\r\n");
		printf("  SrvHost : Host name or IP address of REST API service\r\n");
		printf("  SrvPort : Port number of REST API service\r\n");
		printf("  WebHost : Host name or IP address of WEB service\r\n");
		printf("  WebPort : Port number of WEB service\r\n");
		return 0;
	}

	///////////////////////////////////
	// Fetch the parameters          //
	///////////////////////////////////

	TCHAR ProductName[16] = _T("DummyName");
	TCHAR SrvHost[64] = _T("127.0.0.1");
	TCHAR SrvPort[8] = _T("8081");
	TCHAR WebHost[64] = _T("localhost");
	TCHAR WebPort[8] = _T("80");

	if (argc >= 3) {
		for (int Loop = 2; Loop < argc; Loop++) {
			TCHAR TargetParam[64] = _T("");
			wsprintf(TargetParam, _T("%S"), argv[Loop]);
			TCHAR TmpFetchedParam[64] = _T("");
			if (StkStringParser::ParseInto1Param(TargetParam, _T("ProductName=$"), _T('$'), TmpFetchedParam) == 1) {
				lstrcpy(ProductName, TmpFetchedParam);
			}
			if (StkStringParser::ParseInto1Param(TargetParam, _T("SrvHost=$"), _T('$'), TmpFetchedParam) == 1) {
				lstrcpy(SrvHost, TmpFetchedParam);
			}
			if (StkStringParser::ParseInto1Param(TargetParam, _T("SrvPort=$"), _T('$'), TmpFetchedParam) == 1) {
				lstrcpy(SrvPort, TmpFetchedParam);
			}
			if (StkStringParser::ParseInto1Param(TargetParam, _T("WebHost=$"), _T('$'), TmpFetchedParam) == 1) {
				lstrcpy(WebHost, TmpFetchedParam);
			}
			if (StkStringParser::ParseInto1Param(TargetParam, _T("WebPort=$"), _T('$'), TmpFetchedParam) == 1) {
				lstrcpy(WebPort, TmpFetchedParam);
			}
		}
		printf("ProductName=%S\r\n", ProductName);
		printf("SrvHost=%S\r\n", SrvHost);
		printf("SrvPort=%S\r\n", SrvPort);
		printf("WebHost=%S\r\n", WebHost);
		printf("WebPort=%S\r\n", WebPort);
	}

	///////////////////////////////////
	// Commands for installation     //
	///////////////////////////////////

	BOOL InstFlag = FALSE;
	if (strcmp(argv[1], "inst") == 0) {
		InstFlag = TRUE;
	}
	if (strcmp(argv[1], "modperm") == 0 || InstFlag == TRUE) {
		// Modify permissions
		printf("Modify permissions...\r\n");
		TCHAR CmdLineForIcacls[512];

		// Grant modify permission to nginx.conf
		ZeroMemory(&si,sizeof(si));
		si.cb=sizeof(si);
		wsprintf(CmdLineForIcacls, _T("\"%s\\icacls.exe\" \"%s\\conf\\nginx.conf\" /grant Users:M"), SystemDir, WorkPath);
		printf("%S\r\n", CmdLineForIcacls);
		CreateProcess(NULL, CmdLineForIcacls, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);

		// Grant modify permission to stkwebapp.conf
		ZeroMemory(&si,sizeof(si));
		si.cb=sizeof(si);
		wsprintf(CmdLineForIcacls, _T("\"%s\\icacls.exe\" \"%s\\stkwebapp.conf\" /grant Users:M"), SystemDir, WorkPath);
		printf("%S\r\n", CmdLineForIcacls);
		CreateProcess(NULL, CmdLineForIcacls, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);

		// Grant modify permission to stkwebapp.dat
		ZeroMemory(&si,sizeof(si));
		si.cb=sizeof(si);
		wsprintf(CmdLineForIcacls, _T("\"%s\\icacls.exe\" \"%s\\stkwebapp.dat\" /grant Users:M"), SystemDir, WorkPath);
		printf("%S\r\n", CmdLineForIcacls);
		CreateProcess(NULL, CmdLineForIcacls, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
	}
	if (strcmp(argv[1], "modconfig") == 0 || InstFlag == TRUE) {
		// Modify config files
		printf("Modify config files...\r\n");
		HANDLE FileHndl;

		TCHAR NginxConfPath[MAX_PATH];
		swprintf(NginxConfPath, MAX_PATH, _T("%s\\conf\\nginx.conf"), WorkPath);
		FileHndl = CreateFile(NginxConfPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (FileHndl != INVALID_HANDLE_VALUE) {
			char ConfStr1[4096] = "worker_processes 1;\r\nevents {\r\n  worker_connections 1024;\r\n}\r\n";
			char ConfStr2[4096] = "http {\r\n  include mime.types;\r\n  default_type application/octet-stream;\r\n  sendfile on;\r\n  keepalive_timeout 65;\r\n  server {\r\n";
			char ConfStr3[4096];
			sprintf_s(ConfStr3, 4096, "    listen %S;\r\n    server_name %S;\r\n    location / {\r\n      root html;\r\n      index index.html index.htm;\r\n    }\r\n", WebPort, WebHost);
			char ConfStr4[4096];
			sprintf_s(ConfStr4, 4096, "    location /api/ {\r\n      proxy_pass http://%S:%S;\r\n    }\r\n    error_page 500 502 503 504  /50x.html;\r\n    location = /50x.html {\r\n      root html;\r\n    }\r\n  }\r\n}\r\n", SrvHost, SrvPort);
			DWORD NumOfByteWrite;
			WriteFile(FileHndl, ConfStr1, strlen(ConfStr1), &NumOfByteWrite, NULL);
			WriteFile(FileHndl, ConfStr2, strlen(ConfStr2), &NumOfByteWrite, NULL);
			WriteFile(FileHndl, ConfStr3, strlen(ConfStr3), &NumOfByteWrite, NULL);
			WriteFile(FileHndl, ConfStr4, strlen(ConfStr4), &NumOfByteWrite, NULL);
			CloseHandle(FileHndl);
		};

		TCHAR StkWebAppConfPath[MAX_PATH];
		swprintf(StkWebAppConfPath, MAX_PATH, _T("%s\\stkwebapp.conf"), WorkPath);
		FileHndl = CreateFile(StkWebAppConfPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (FileHndl != INVALID_HANDLE_VALUE) {
			char ConfStr1[4096];
			sprintf_s(ConfStr1, 4096, "servicehost=%S\r\nserviceport=%S\r\n", SrvHost, SrvPort);
			DWORD NumOfByteWrite;
			WriteFile(FileHndl, ConfStr1, strlen(ConfStr1), &NumOfByteWrite, NULL);
			CloseHandle(FileHndl);
		};
	}
	if (strcmp(argv[1], "fwadd") == 0 || InstFlag == TRUE) {
		// Add firewall exception
		printf("Add firewall exception...\r\n");
		ZeroMemory(&si,sizeof(si));
		si.cb=sizeof(si);
		TCHAR CmdLine[512];
		wsprintf(CmdLine, _T("\"%s\\netsh.exe\" advfirewall firewall add rule name=\"%s\" dir=in program=\"%s\\nginx.exe\" action=allow"), SystemDir, ProductName, WorkPath);
		printf("%S\r\n", CmdLine);
		CreateProcess(NULL, CmdLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
	}
	if (strcmp(argv[1], "srvadd") == 0 || InstFlag == TRUE) {
		// Add service
		printf("Add service...\r\n");
		ZeroMemory(&si,sizeof(si));
		si.cb=sizeof(si);
		TCHAR CmdLineForService[512];
		wsprintf(CmdLineForService, _T("\"%s\\sc.exe\" create %s binPath= \"%s\" start= auto displayname= \"%s\""), SystemDir, ProductName, ThisCmdPath, ProductName);
		printf("%S\r\n", CmdLineForService);
		CreateProcess(NULL, CmdLineForService, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
	}
	if (strcmp(argv[1], "start") == 0 || InstFlag == TRUE) {
		// Start service
		printf("Start service...\r\n");
		ZeroMemory(&si,sizeof(si));
		si.cb=sizeof(si);
		TCHAR CmdLineForNet[512];
		wsprintf(CmdLineForNet, _T("\"%s\\net.exe\" start %s"), SystemDir, ProductName);
		printf("%S\r\n", CmdLineForNet);
		CreateProcess(NULL, CmdLineForNet, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
	}

	///////////////////////////////////
	// Commands for uninstallation   //
	///////////////////////////////////

	BOOL UninstFlag = FALSE;
	if (strcmp(argv[1], "uninst") == 0) {
		UninstFlag = TRUE;
	}
	if (strcmp(argv[1], "stop") == 0 || UninstFlag == TRUE) {
		// Stop service
		printf("Stop service...\r\n");
		ZeroMemory(&si,sizeof(si));
		si.cb=sizeof(si);
		TCHAR CmdLineForNet[512];
		wsprintf(CmdLineForNet, _T("\"%s\\net.exe\" stop %s"), SystemDir, ProductName);
		printf("%S\r\n", CmdLineForNet);
		CreateProcess(NULL, CmdLineForNet, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
	}
	if (strcmp(argv[1], "srvdel") == 0 || UninstFlag == TRUE) {
		// Delete service
		printf("Delete service...\r\n");
		ZeroMemory(&si,sizeof(si));
		si.cb=sizeof(si);
		TCHAR CmdLineForService[512];
		wsprintf(CmdLineForService, _T("\"%s\\sc.exe\" delete %s"), SystemDir, ProductName);
		printf("%S\r\n", CmdLineForService);
		CreateProcess(NULL, CmdLineForService, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
	}
	if (strcmp(argv[1], "fwdel") == 0 || UninstFlag == TRUE) {
		// Delete firewall exception
		printf("Delete firewall exception...\r\n");
		ZeroMemory(&si,sizeof(si));
		si.cb=sizeof(si);
		TCHAR CmdLine[512];
		wsprintf(CmdLine, _T("\"%s\\netsh.exe\" advfirewall firewall delete rule name=\"%s\""), SystemDir, ProductName);
		printf("%S\r\n", CmdLine);
		CreateProcess(NULL, CmdLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
	}
}
