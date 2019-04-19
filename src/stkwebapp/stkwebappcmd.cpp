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
bool CALLBACK EnumWindowsProc(HWND, LPARAM);

SERVICE_TABLE_ENTRY ServiceTable[] = {
	{ SERVICE_NAME, ServiceMain },
	{ NULL, NULL }
};

bool g_bRun = true;
bool g_bService = true;
SERVICE_STATUS_HANDLE g_hServiceStatus = NULL;

/////////////////////////////////////////////////////////////////////////////////////////

// Get file size of the specified file name
// FilePaht [in] : Path of file which you want to get the size.
// Return : Size of the specified file. If an error occurred, -1 is returned. (The value is casted into int.)
int GetFileSize(wchar_t* FilePath)
{
	HANDLE ReadFileHndl = CreateFile(FilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (ReadFileHndl == INVALID_HANDLE_VALUE) {
		return -1;
	}
	LARGE_INTEGER ExistingFileSize;
	if (GetFileSizeEx(ReadFileHndl, &ExistingFileSize) == 0) {
		CloseHandle(ReadFileHndl);
		return -1;
	}
	CloseHandle(ReadFileHndl);
	return (int)ExistingFileSize.QuadPart;
}

bool CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    PROCESS_INFORMATION* pi = (PROCESS_INFORMATION*)lParam;

    DWORD lpdwProcessId = 0;
    GetWindowThreadProcessId(hWnd, &lpdwProcessId);
    if (pi->dwProcessId == lpdwProcessId) {
        PostMessage(hWnd, WM_CLOSE, 0, 0);
        return false;
    }
    return true;
}

void StartProcesses()
{
	wchar_t Buf[256];
	wchar_t CmdLine[512];
	GetModuleFileName(NULL, Buf, 255);
	LPTSTR Addr = StrStr(Buf, L"\\stkwebappcmd.exe");
	lstrcpy(Addr, L"");
	wchar_t SystemDir[MAX_PATH];
	GetSystemDirectory(SystemDir, MAX_PATH);

	SetCurrentDirectory(Buf);

	char TmpSrvProgram[FILENAME_MAX] = "xxxx";
	wchar_t SrvProgram[FILENAME_MAX] = L"xxxx";
	StkProperties *Prop = new StkProperties();
	if (Prop->GetProperties(L"stkwebappcmd.conf") == 0) {
		Prop->GetPropertyStr("serviceprogram", TmpSrvProgram);
		wsprintf(SrvProgram, L"%S", TmpSrvProgram);
	}
	delete Prop;

	STARTUPINFO si_wapp;
	PROCESS_INFORMATION pi_wapp;
	ZeroMemory(&si_wapp, sizeof(si_wapp));
	si_wapp.cb = sizeof(si_wapp);
	wsprintf(CmdLine, L"%s\\%s.exe", Buf, SrvProgram);
	CreateProcess(NULL, CmdLine, NULL, NULL, false, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si_wapp, &pi_wapp);

	STARTUPINFO si_nginx;
	PROCESS_INFORMATION pi_nginx;
	ZeroMemory(&si_nginx, sizeof(si_nginx));
	si_nginx.cb = sizeof(si_nginx);
	wsprintf(CmdLine, L"\"%s\\nginx.exe\"", Buf); /* instead of  [cmd.exe /c "start \nginx.exe"] */
	CreateProcess(NULL, CmdLine, NULL, NULL, false, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si_nginx, &pi_nginx);
}

void StopProcesses()
{
	/***** Set current directory *****/
	wchar_t Buf[256];
	wchar_t CmdLine[512];
	GetModuleFileName(NULL, Buf, 255);
	LPTSTR Addr = StrStr(Buf, L"\\stkwebappcmd.exe");
	lstrcpy(Addr, L"");
	SetCurrentDirectory(Buf);

	/***** Stop StkWebApp *****/
	char TmpSrvProgramConf[FILENAME_MAX] = "";
	wchar_t SrvProgramConf[FILENAME_MAX] = L"";
	StkProperties *PropSrv = new StkProperties();
	if (PropSrv->GetProperties(L"stkwebappcmd.conf") == 0) {
		PropSrv->GetPropertyStr("serviceprogram", TmpSrvProgramConf);
		wsprintf(SrvProgramConf, L"%S.conf", TmpSrvProgramConf);
	}
	delete PropSrv;

	char IpAddrTmp[256] = "127.0.0.1";
	wchar_t IpAddr[256] = L"127.0.0.1";
	int Port = 8081;
	StkProperties *Prop = new StkProperties();
	if (Prop->GetProperties(SrvProgramConf) == 0) {
		Prop->GetPropertyStr("servicehost", IpAddrTmp);
		wsprintf(IpAddr, L"%S", IpAddrTmp);
		Prop->GetPropertyInt("serviceport", &Port);
	}
	delete Prop;

	StkSocket_AddInfo(1, SOCK_STREAM, STKSOCKET_ACTIONTYPE_SENDER, IpAddr, Port);
	if (StkSocket_Connect(1) == 0) {
		char SendDat[1024];
		char Dat[256] = "{ \"Operation\" : \"Stop\" }";
		unsigned char RecvDat[1024];
		sprintf_s(SendDat, 1024, "POST /service/ HTTP/1.1\nContent-Length: %d\nContent-Type: application/json\n\n%s", strlen(Dat), Dat);
		StkSocket_Send(1, 1, (unsigned char*)SendDat, strlen((char*)SendDat));
		int RetR;
		for (int Loop = 0; Loop < 10; Loop++) {
			RetR = StkSocket_Receive(1, 1, RecvDat, 1024, STKSOCKET_RECV_FINISHCOND_CONTENTLENGTH, 5000, NULL, -1);
			if (RetR > 0) {
				break;
			}
		}
		StkSocket_Disconnect(1, 1, true);
	}
	StkSocket_DeleteInfo(1);

	/***** Stop nginx *****/
	PROCESS_INFORMATION pi_nginxstop;
	STARTUPINFO si_nginx;
	ZeroMemory(&si_nginx, sizeof(si_nginx));
	si_nginx.cb = sizeof(si_nginx);
	wsprintf(CmdLine, L"\"%s\\nginx.exe\" -s stop", Buf);
	CreateProcess(NULL, CmdLine, NULL, NULL, false, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si_nginx, &pi_nginxstop);
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
		g_bService = false;
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
		g_bRun = false;
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
		g_bRun = true;
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

	wchar_t ThisCmdPath[256];
	GetModuleFileName(NULL, ThisCmdPath, 255);

	wchar_t WorkPath[256];
	lstrcpy(WorkPath, ThisCmdPath);
	LPTSTR Addr = StrStr(WorkPath, L"\\stkwebappcmd.exe");
	lstrcpy(Addr, L"");

	wchar_t SystemDir[MAX_PATH];
	GetSystemDirectory(SystemDir, MAX_PATH);

	/////////////////////////////////////////////////////////////
	// If no command is specified, service info is configured. //
	/////////////////////////////////////////////////////////////

	if (argc == 1) {
		bool bRet = StartServiceCtrlDispatcher(ServiceTable);
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
			wchar_t TargetParam[64] = L"";
			wsprintf(TargetParam, L"%S", argv[Loop]);
			wchar_t TmpFetchedParam[64] = L"";
			if (StkStringParser::ParseInto1Param(TargetParam, L"ProductName=$", L'$', TmpFetchedParam) != 1 &&
				StkStringParser::ParseInto1Param(TargetParam, L"SrvHost=$", L'$', TmpFetchedParam) != 1 &&
				StkStringParser::ParseInto1Param(TargetParam, L"SrvPort=$", L'$', TmpFetchedParam) != 1 &&
				StkStringParser::ParseInto1Param(TargetParam, L"SrvProgram=$", L'$', TmpFetchedParam) != 1 &&
				StkStringParser::ParseInto1Param(TargetParam, L"WebHost=$", L'$', TmpFetchedParam) != 1 &&
				StkStringParser::ParseInto1Param(TargetParam, L"WebPort=$", L'$', TmpFetchedParam) != 1) {
				printf("Invalid option is specified. [%S]\r\n", TargetParam);
				return 0;
			}
		}
	}
	{
		wchar_t TargetFile[9][MAX_PATH];

		wsprintf(TargetFile[0], L"%s\\icacls.exe", SystemDir);
		wsprintf(TargetFile[1], L"%s\\netsh.exe", SystemDir);
		wsprintf(TargetFile[2], L"%s\\sc.exe", SystemDir);
		wsprintf(TargetFile[3], L"%s\\net.exe", SystemDir);
		wsprintf(TargetFile[4], L"%s\\conf\\nginx.conf", WorkPath);
		wsprintf(TargetFile[5], L"%s\\stkwebappcmd.conf", WorkPath);
		wsprintf(TargetFile[6], L"%s\\nginx.exe", WorkPath);

		for (int Loop = 0; Loop < 7; Loop++) {
			if (GetFileSize(TargetFile[Loop]) == -1) {
				printf("File/Folder structure is invalid. [%S]\r\n", TargetFile[Loop]);
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
		printf("  modperm   : Modify permission of configuration and data files. SrvProgram option is required.\r\n");
		printf("  modconfig : Modify configuration files. ProductName, SrvHost, SrvPort, SrvProgram, WebHost and WebPort options are required.\r\n");
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
		printf("  SrvProgram : Program name (without extension) which stkwebappcmd launches\r\n");
		printf("  WebHost : Host name or IP address of WEB service\r\n");
		printf("  WebPort : Port number of WEB service\r\n");
		return 0;
	}

	///////////////////////////////////
	// Fetch the parameters          //
	///////////////////////////////////

	wchar_t ProductName[16] = L"DummyName";
	wchar_t SrvHost[64] = L"127.0.0.1";
	wchar_t SrvPort[8] = L"8081";
	wchar_t SrvProgram[FILENAME_MAX] = L"xxxx";
	wchar_t WebHost[64] = L"localhost";
	wchar_t WebPort[8] = L"80";

	if (argc >= 3) {
		for (int Loop = 2; Loop < argc; Loop++) {
			wchar_t TargetParam[64] = L"";
			wsprintf(TargetParam, L"%S", argv[Loop]);
			wchar_t TmpFetchedParam[64] = L"";
			if (StkStringParser::ParseInto1Param(TargetParam, L"ProductName=$", L'$', TmpFetchedParam) == 1) {
				lstrcpy(ProductName, TmpFetchedParam);
			}
			if (StkStringParser::ParseInto1Param(TargetParam, L"SrvHost=$", L'$', TmpFetchedParam) == 1) {
				lstrcpy(SrvHost, TmpFetchedParam);
			}
			if (StkStringParser::ParseInto1Param(TargetParam, L"SrvPort=$", L'$', TmpFetchedParam) == 1) {
				lstrcpy(SrvPort, TmpFetchedParam);
			}
			if (StkStringParser::ParseInto1Param(TargetParam, L"SrvProgram=$", L'$', TmpFetchedParam) == 1) {
				lstrcpy(SrvProgram, TmpFetchedParam);
			}
			if (StkStringParser::ParseInto1Param(TargetParam, L"WebHost=$", L'$', TmpFetchedParam) == 1) {
				lstrcpy(WebHost, TmpFetchedParam);
			}
			if (StkStringParser::ParseInto1Param(TargetParam, L"WebPort=$", L'$', TmpFetchedParam) == 1) {
				lstrcpy(WebPort, TmpFetchedParam);
			}
		}
	}

	///////////////////////////////////
	// Commands for installation     //
	///////////////////////////////////

	bool InstFlag = false;
	if (strcmp(argv[1], "inst") == 0) {
		wchar_t Buf[512];
		wsprintf(Buf, L"%s\\stkwebappcmd.conf", WorkPath);
		if (GetFileSize(Buf) > 0) {
			printf("Program has already been installed.\r\n");
			return 0;
		} else {
			InstFlag = true;
		}
	}
	if (strcmp(argv[1], "modperm") == 0 || InstFlag == true) {
		// Modify permissions
		printf("Modify permissions...\r\n");
		wchar_t CmdLineForIcacls[512];

		// Grant modify permission to nginx.conf
		ZeroMemory(&si,sizeof(si));
		si.cb=sizeof(si);
		wsprintf(CmdLineForIcacls, L"\"%s\\icacls.exe\" \"%s\\conf\\nginx.conf\" /grant Users:M", SystemDir, WorkPath);
		printf("%S\r\n", CmdLineForIcacls);
		CreateProcess(NULL, CmdLineForIcacls, NULL, NULL, false, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);

		// Grant modify permission to stkwebappcmd.conf
		ZeroMemory(&si,sizeof(si));
		si.cb=sizeof(si);
		wsprintf(CmdLineForIcacls, L"\"%s\\icacls.exe\" \"%s\\stkwebappcmd.conf\" /grant Users:M", SystemDir, WorkPath);
		printf("%S\r\n", CmdLineForIcacls);
		CreateProcess(NULL, CmdLineForIcacls, NULL, NULL, false, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);

		// Grant modify permission to SrvProgram.dat
		ZeroMemory(&si,sizeof(si));
		si.cb=sizeof(si);
		wsprintf(CmdLineForIcacls, L"\"%s\\icacls.exe\" \"%s\\%s.dat\" /grant Users:M", SystemDir, WorkPath, SrvProgram);
		printf("%S\r\n", CmdLineForIcacls);
		CreateProcess(NULL, CmdLineForIcacls, NULL, NULL, false, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);

		// Grant modify permission to SrvProgram.conf
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		wsprintf(CmdLineForIcacls, L"\"%s\\icacls.exe\" \"%s\\%s.conf\" /grant Users:M", SystemDir, WorkPath, SrvProgram);
		printf("%S\r\n", CmdLineForIcacls);
		CreateProcess(NULL, CmdLineForIcacls, NULL, NULL, false, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
	}
	if (strcmp(argv[1], "modconfig") == 0 || InstFlag == true) {
		// Modify config files
		printf("Modify config files...\r\n");
		HANDLE FileHndl;

		wchar_t NginxConfPath[MAX_PATH];
		swprintf(NginxConfPath, MAX_PATH, L"%s\\conf\\nginx.conf", WorkPath);
		FileHndl = CreateFile(NginxConfPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (FileHndl != INVALID_HANDLE_VALUE) {
			char ConfStr1[4096] = "worker_processes 1;\r\nevents {\r\n  worker_connections 1024;\r\n}\r\n";
			char ConfStr2[4096] = "http {\r\n  include mime.types;\r\n  default_type application/octet-stream;\r\n  sendfile on;\r\n  keepalive_timeout 65;\r\n  server {\r\n";
			char ConfStr3[4096];
			sprintf_s(ConfStr3, 4096, "    listen %S;\r\n    listen [::]:%S;\r\n    server_name %S;\r\n    location / {\r\n      root html;\r\n      index index.html index.htm;\r\n    }\r\n", WebPort, WebPort, WebHost);
			char ConfStr4[4096];
			sprintf_s(ConfStr4, 4096, "    location /api/ {\r\n      proxy_pass http://%S:%S;\r\n    }\r\n    error_page 500 502 503 504  /50x.html;\r\n    location = /50x.html {\r\n      root html;\r\n    }\r\n  }\r\n}\r\n", SrvHost, SrvPort);
			DWORD NumOfByteWrite;
			WriteFile(FileHndl, ConfStr1, strlen(ConfStr1), &NumOfByteWrite, NULL);
			WriteFile(FileHndl, ConfStr2, strlen(ConfStr2), &NumOfByteWrite, NULL);
			WriteFile(FileHndl, ConfStr3, strlen(ConfStr3), &NumOfByteWrite, NULL);
			WriteFile(FileHndl, ConfStr4, strlen(ConfStr4), &NumOfByteWrite, NULL);
			CloseHandle(FileHndl);
		};

		wchar_t StkWebAppConfPath[MAX_PATH];
		swprintf(StkWebAppConfPath, MAX_PATH, L"%s\\stkwebappcmd.conf", WorkPath);
		FileHndl = CreateFile(StkWebAppConfPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (FileHndl != INVALID_HANDLE_VALUE) {
			char ConfStr1[4096];
			sprintf_s(ConfStr1, 4096, "serviceprogram=%S\r\n", SrvProgram);
			DWORD NumOfByteWrite;
			WriteFile(FileHndl, ConfStr1, strlen(ConfStr1), &NumOfByteWrite, NULL);
			CloseHandle(FileHndl);
		};

		wchar_t SrvProgramConfPath[MAX_PATH];
		swprintf(SrvProgramConfPath, MAX_PATH, L"%s\\%s.conf", WorkPath, SrvProgram);
		FileHndl = CreateFile(SrvProgramConfPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (FileHndl != INVALID_HANDLE_VALUE) {
			char ConfStr1[4096];
			sprintf_s(ConfStr1, 4096, "servicehost=%S\r\nserviceport=%S\r\n", SrvHost, SrvPort);
			DWORD NumOfByteWrite;
			WriteFile(FileHndl, ConfStr1, strlen(ConfStr1), &NumOfByteWrite, NULL);
			CloseHandle(FileHndl);
		};
	}
	if (strcmp(argv[1], "fwadd") == 0 || InstFlag == true) {
		// Add firewall exception
		printf("Add firewall exception...\r\n");
		ZeroMemory(&si,sizeof(si));
		si.cb=sizeof(si);
		wchar_t CmdLine[512];
		wsprintf(CmdLine, L"\"%s\\netsh.exe\" advfirewall firewall add rule name=\"%s\" dir=in program=\"%s\\nginx.exe\" action=allow", SystemDir, ProductName, WorkPath);
		printf("%S\r\n", CmdLine);
		CreateProcess(NULL, CmdLine, NULL, NULL, false, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
	}
	if (strcmp(argv[1], "srvadd") == 0 || InstFlag == true) {
		// Add service
		printf("Add service...\r\n");
		ZeroMemory(&si,sizeof(si));
		si.cb=sizeof(si);
		wchar_t CmdLineForService[512];
		wsprintf(CmdLineForService, L"\"%s\\sc.exe\" create %s binPath= \"%s\" start= auto displayname= \"%s\"", SystemDir, ProductName, ThisCmdPath, ProductName);
		printf("%S\r\n", CmdLineForService);
		CreateProcess(NULL, CmdLineForService, NULL, NULL, false, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
	}
	if (strcmp(argv[1], "start") == 0 || InstFlag == true) {
		// Start service
		printf("Start service...\r\n");
		ZeroMemory(&si,sizeof(si));
		si.cb=sizeof(si);
		wchar_t CmdLineForNet[512];
		wsprintf(CmdLineForNet, L"\"%s\\net.exe\" start %s", SystemDir, ProductName);
		printf("%S\r\n", CmdLineForNet);
		CreateProcess(NULL, CmdLineForNet, NULL, NULL, false, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
	}

	///////////////////////////////////
	// Commands for uninstallation   //
	///////////////////////////////////

	bool UninstFlag = false;
	if (strcmp(argv[1], "uninst") == 0) {
		UninstFlag = true;
	}
	if (strcmp(argv[1], "stop") == 0 || UninstFlag == true) {
		// Stop service
		printf("Stop service...\r\n");
		ZeroMemory(&si,sizeof(si));
		si.cb=sizeof(si);
		wchar_t CmdLineForNet[512];
		wsprintf(CmdLineForNet, L"\"%s\\net.exe\" stop %s", SystemDir, ProductName);
		printf("%S\r\n", CmdLineForNet);
		CreateProcess(NULL, CmdLineForNet, NULL, NULL, false, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
	}
	if (strcmp(argv[1], "srvdel") == 0 || UninstFlag == true) {
		// Delete service
		printf("Delete service...\r\n");
		ZeroMemory(&si,sizeof(si));
		si.cb=sizeof(si);
		wchar_t CmdLineForService[512];
		wsprintf(CmdLineForService, L"\"%s\\sc.exe\" delete %s", SystemDir, ProductName);
		printf("%S\r\n", CmdLineForService);
		CreateProcess(NULL, CmdLineForService, NULL, NULL, false, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
	}
	if (strcmp(argv[1], "fwdel") == 0 || UninstFlag == true) {
		// Delete firewall exception
		printf("Delete firewall exception...\r\n");
		ZeroMemory(&si,sizeof(si));
		si.cb=sizeof(si);
		wchar_t CmdLine[512];
		wsprintf(CmdLine, L"\"%s\\netsh.exe\" advfirewall firewall delete rule name=\"%s\"", SystemDir, ProductName);
		printf("%S\r\n", CmdLine);
		CreateProcess(NULL, CmdLine, NULL, NULL, false, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
	}

	return 0;
}
