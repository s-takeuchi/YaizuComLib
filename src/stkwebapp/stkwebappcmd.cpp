#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <shlwapi.h>

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
PROCESS_INFORMATION pi_wapp;
PROCESS_INFORMATION pi_nginx;

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
	ZeroMemory(&si_wapp, sizeof(si_wapp));
	si_wapp.cb = sizeof(si_wapp);
	wsprintf(CmdLine, _T("%s\\stkwebapp.exe"), Buf);
	CreateProcess(NULL, CmdLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si_wapp, &pi_wapp);

	STARTUPINFO si_nginx;
	ZeroMemory(&si_nginx, sizeof(si_nginx));
	si_nginx.cb = sizeof(si_nginx);
	wsprintf(CmdLine, _T("\"%s\\nginx.exe\""), Buf); /* instead of  [cmd.exe /c "start \nginx.exe"] */
	CreateProcess(NULL, CmdLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si_nginx, &pi_nginx);
}

void StopProcesses()
{
	EnumWindows(EnumWindowsProc, (LPARAM)&pi_wapp);
	if (WaitForSingleObject(pi_wapp.hProcess, 5000) == WAIT_TIMEOUT) {
		TerminateProcess(pi_wapp.hProcess, 0);
	}
	CloseHandle(pi_wapp.hThread);
	CloseHandle(pi_wapp.hProcess);

	TCHAR Buf[256];
	TCHAR CmdLine[512];
	GetModuleFileName(NULL, Buf, 255);
	LPTSTR Addr = StrStr(Buf, _T("\\stkwebappcmd.exe"));
	lstrcpy(Addr, _T(""));
	TCHAR SystemDir[MAX_PATH];
	GetSystemDirectory(SystemDir, MAX_PATH);

	SetCurrentDirectory(Buf);

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
		return 0;
	}

	///////////////////////////////////
	// Commands for installation     //
	///////////////////////////////////

	if (strcmp(argv[1], "modperm") == 0) {
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

		return 0;
	}
	if (strcmp(argv[1], "modconfig") == 0) {
		HANDLE FileHndl;

		TCHAR NginxConfPath[MAX_PATH];
		swprintf(NginxConfPath, MAX_PATH, _T("%s\\conf\\nginx.conf"), WorkPath);
		FileHndl = CreateFile(NginxConfPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (FileHndl != INVALID_HANDLE_VALUE) {
			char ConfStr1[4096] = "worker_processes 1;\r\nevents {\r\n  worker_connections 1024;\r\n}\r\n";
			char ConfStr2[4096] = "http {\r\n  include mime.types;\r\n  default_type application/octet-stream;\r\n  sendfile on;\r\n  keepalive_timeout 65;\r\n  server {\r\n";
			char ConfStr3[4096] = "    listen 80;\r\n    server_name localhost;\r\n    location / {\r\n      root html;\r\n      index index.html index.htm;\r\n    }\r\n";
			char ConfStr4[4096] = "    location /api/ {\r\n      proxy_pass http://127.0.0.1:8081;\r\n    }\r\n    error_page 500 502 503 504  /50x.html;\r\n    location = /50x.html {\r\n      root html;\r\n    }\r\n  }\r\n}\r\n";
			DWORD NumOfByteWrite;
			WriteFile(FileHndl, ConfStr1, strlen(ConfStr1), &NumOfByteWrite, NULL);
			WriteFile(FileHndl, ConfStr2, strlen(ConfStr2), &NumOfByteWrite, NULL);
			WriteFile(FileHndl, ConfStr3, strlen(ConfStr3), &NumOfByteWrite, NULL);
			WriteFile(FileHndl, ConfStr4, strlen(ConfStr4), &NumOfByteWrite, NULL);
			CloseHandle(FileHndl);
		};

		return 0;
	}
	if (strcmp(argv[1], "fwadd") == 0) {
		return 0;
	}
	if (strcmp(argv[1], "srvadd") == 0) {
		// Add service
		printf("Add service...\r\n");
		ZeroMemory(&si,sizeof(si));
		si.cb=sizeof(si);
		TCHAR CmdLineForService[512];
		wsprintf(CmdLineForService, _T("\"%s\\sc.exe\" create StkWebAppCmd binPath= \"%s\" start= auto displayname= \"StkWebAppCmd\""), SystemDir, ThisCmdPath);
		printf("%S\r\n", CmdLineForService);
		CreateProcess(NULL, CmdLineForService, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		return 0;
	}
	if (strcmp(argv[1], "start") == 0) {
		// Start service
		printf("Start service...\r\n");
		ZeroMemory(&si,sizeof(si));
		si.cb=sizeof(si);
		TCHAR CmdLineForNet[512];
		wsprintf(CmdLineForNet, _T("\"%s\\net.exe\" start StkWebAppCmd"), SystemDir);
		printf("%S\r\n", CmdLineForNet);
		CreateProcess(NULL, CmdLineForNet, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		return 0;
	}

	///////////////////////////////////
	// Commands for uninstallation   //
	///////////////////////////////////

	if (strcmp(argv[1], "stop") == 0) {
		// Stop service
		printf("Stop service...\r\n");
		ZeroMemory(&si,sizeof(si));
		si.cb=sizeof(si);
		TCHAR CmdLineForNet[512];
		wsprintf(CmdLineForNet, _T("\"%s\\net.exe\" stop StkWebAppCmd"), SystemDir);
		printf("%S\r\n", CmdLineForNet);
		CreateProcess(NULL, CmdLineForNet, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		return 0;
	}
	if (strcmp(argv[1], "srvdel") == 0) {
		// Delete service
		printf("Delete service...\r\n");
		ZeroMemory(&si,sizeof(si));
		si.cb=sizeof(si);
		TCHAR CmdLineForService[512];
		wsprintf(CmdLineForService, _T("\"%s\\sc.exe\" delete StkWebAppCmd"), SystemDir);
		printf("%S\r\n", CmdLineForService);
		CreateProcess(NULL, CmdLineForService, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
		return 0;
	}
	if (strcmp(argv[1], "fwdel") == 0) {
		return 0;
	}

}
