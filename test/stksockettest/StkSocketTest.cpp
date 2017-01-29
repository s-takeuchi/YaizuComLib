#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <shlwapi.h>
#include "..\..\src\stksocket\stksocket.h"
#include "StkSocketTestMa.h"
#include "StkSocketTestGetSockInfo.h"
#include "StkSocketIPv6.h"
#include "StkSocketMemoryLeak.h"
#include "StkSocketTestHttp.h"

BOOL FinishFlag = FALSE;
int FindFlagCounter = 0;

/*
void AddExceptionForFilewall()
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&si,sizeof(si));
	si.cb=sizeof(si);
	TCHAR Buf[256];
	TCHAR CmdLine[512];
	GetModuleFileName(NULL, Buf, 255);
	wsprintf(CmdLine, _T("netsh firewall add allowedprogram \"%s\" StkSocketTest ENABLE"), Buf);
	CreateProcess(NULL, CmdLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
}

void DeleteExceptionForFirewall()
{
	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&si,sizeof(si));
	si.cb=sizeof(si);
	TCHAR Buf[256];
	TCHAR CmdLine[512];
	GetModuleFileName(NULL, Buf, 255);
	wsprintf(CmdLine, _T("netsh firewall delete allowedprogram \"%s\""), Buf);
	CreateProcess(NULL, CmdLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
}
*/

int ConnectDisconnectTcpPort()
{
	int Msg;
	int LogId;
	TCHAR ParamStr1[256];
	TCHAR ParamStr2[256];
	int ParamInt1;
	int ParamInt2;

	printf("[ConnectSocket()/DisconnectSocket() for TCP] : Connect invalid IP address and port...");
	StkSocket_AddInfo(22, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, _T("127.0.0.1"), 2222);
	StkSocket_Connect(22);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_CONNERROR && ParamInt2 != 10061) {
		printf("NG\r\n");
		return -1;
	}
	StkSocket_Disconnect(22, 22, TRUE);
	StkSocket_Close(22, TRUE);
	StkSocket_DeleteInfo(22);
	printf("OK [%d]\r\n", ParamInt2);
	return 0;
}

int ConnectDisconnectUdpPort()
{
	int Msg;
	int LogId;
	TCHAR ParamStr1[256];
	TCHAR ParamStr2[256];
	int ParamInt1;
	int ParamInt2;

	printf("[ConnectSocket()/DisconnectSocket() for UDP] : Only open and close socket.  It will be ended successfully...");
	StkSocket_AddInfo(22, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_SENDER, _T("127.0.0.1"), 2222);
	StkSocket_Connect(22);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_SUCCESSCS) {
		printf("NG (LOG_SUCCESSCS is not returned.\r\n");
		return -1;
	}
	StkSocket_Disconnect(22, 22, TRUE);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_UDPSOCKCLOSE) {
		printf("NG (LOG_UDPSOCKCLOSE is not returned.)\r\n");
		return -1;
	}
	StkSocket_DeleteInfo(22);
	printf("OK\r\n", ParamInt2);
	return 0;
}

int OpenClosePort()
{
	int Msg;
	int LogId;
	TCHAR ParamStr1[256];
	TCHAR ParamStr2[256];
	int ParamInt1;
	int ParamInt2;


	printf("[OpenSocket()/CloseSocket()] : Open and close using same port. Check open and close operation are successfully end...");
	StkSocket_AddInfo(0, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("127.0.0.1"), 2001);
	if (StkSocket_GetNumOfStkInfos() != 1) {
		printf("NG (Number of Socket Info is invalid.)\r\n");
		return -1;
	}
	{
		int TargetId;
		int SockType;
		int ActionType;
		TCHAR TargetAddr[256];
		int TargetPort;
		BOOL CopiedFlag;

		StkSocket_GetInfo(0, &TargetId, &SockType, &ActionType, TargetAddr, &TargetPort, &CopiedFlag);
		if (TargetId != 0 ||
			SockType != STKSOCKET_TYPE_STREAM ||
			ActionType != STKSOCKET_ACTIONTYPE_RECEIVER ||
			lstrcmp(TargetAddr, _T("127.0.0.1")) != 0 ||
			TargetPort != 2001 ||
			CopiedFlag != FALSE) {
			printf("NG (Acquired socket information is not matched.)\r\n");
			return -1;
		}
	}
	StkSocket_Open(0);
	StkSocket_Close(0, TRUE);
	StkSocket_DeleteInfo(0);
	if (StkSocket_GetNumOfStkInfos() != 0) {
		printf("NG (Number of Socket Info is invalid. (not zero))\r\n");
		return -1;
	}
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_SOCKCLOSE) {
		printf("NG\r\n");
		return -1;
	}
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_SUCCESSCSBNLS) {
		printf("NG\r\n");
		return -1;
	}
	printf("OK\r\n");


	printf("[OpenSocket()/CloseSocket()] : If OpenSocket is called twice with the same ID, the method processes nothing...");
	StkSocket_AddInfo(1, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("127.0.0.1"), 2001);
	StkSocket_Open(1);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_SUCCESSCSBNLS) {
		printf("NG\r\n");
		return -1;
	}
	StkSocket_Open(1);
	if (StkSocket_GetNumOfLogs() != 0) {
		printf("NG\r\n");
		return -1;
	}
	StkSocket_Close(1, TRUE);
	StkSocket_DeleteInfo(1);
	if (StkSocket_GetNumOfLogs() != 1) {
		printf("NG\r\n");
		return -1;
	}
	StkSocket_ClearLog();
	printf("OK\r\n");


	printf("[OpenSocket()/CloseSocket()] : If CloseSocket is called with nonexisting ID, the method processes nothing...");
	StkSocket_Close(9999, TRUE);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != 0) {
		printf("NG\r\n");
		return -1;
	}
	printf("OK\r\n");


	printf("[OpenSocket()/CloseSocket()] : If the receiver IP address is invalid(11.11.11.11), test program fails the socket opening...");
	StkSocket_AddInfo(0, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("11.11.11.11"), 2000);
	StkSocket_Open(0);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_BINDLISTENERR && ParamInt2 != 10049) {
		printf("NG\r\n");
		return -1;
	}
	StkSocket_Close(0, TRUE);
	StkSocket_DeleteInfo(0);
	printf("OK [%d]\r\n", ParamInt2);


	printf("[OpenSocket()/CloseSocket()] : If OpenSocket is called with invalid host name, An error (LOG_NAMESOLVEERR) occurred...");
	StkSocket_AddInfo(0, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("invalid"), 2001);
	StkSocket_Open(0);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	StkSocket_Close(0, TRUE);
	StkSocket_DeleteInfo(0);
	if (Msg != STKSOCKET_LOG_NAMESOLVEERR && ParamInt2 != 11004) {
		printf("NG\r\n");
		return -1;
	}
	printf("OK [%d]\r\n", ParamInt2);


	return 0;
}

int OpenCloseUdpPort()
{
	int Msg;
	int LogId;
	TCHAR ParamStr1[256];
	TCHAR ParamStr2[256];
	int ParamInt1;
	int ParamInt2;


	printf("[OpenSocket()/CloseSocket() for UDP] : Open and close using same port. Check open and close operation are successfully end...");
	StkSocket_AddInfo(0, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("127.0.0.1"), 2001);
	StkSocket_Open(0);
	StkSocket_Close(0, TRUE);
	StkSocket_DeleteInfo(0);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_UDPSOCKCLOSE) {
		printf("NG\r\n");
		return -1;
	}
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_SUCCESSCSBN) {
		printf("NG\r\n");
		return -1;
	}
	printf("OK\r\n");


	printf("[OpenSocket()/CloseSocket() for UDP] : If OpenSocket is called twice with the same ID, the method processes nothing...");
	StkSocket_AddInfo(1, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("127.0.0.1"), 2001);
	StkSocket_Open(1);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_SUCCESSCSBN) {
		printf("NG\r\n");
		return -1;
	}
	StkSocket_Open(1);
	if (StkSocket_GetNumOfLogs() != 0) {
		printf("NG\r\n");
		return -1;
	}
	StkSocket_Close(1, TRUE);
	StkSocket_DeleteInfo(1);
	if (StkSocket_GetNumOfLogs() != 1) {
		printf("NG\r\n");
		return -1;
	}
	StkSocket_ClearLog();
	printf("OK\r\n");


	printf("[OpenSocket()/CloseSocket() for UDP] : If CloseSocket is called with nonexisting ID, the method processes nothing...");
	StkSocket_Close(9999, TRUE);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != 0) {
		printf("NG\r\n");
		return -1;
	}
	printf("OK\r\n");


	printf("[OpenSocket()/CloseSocket() for UDP] : If the receiver IP address is invalid(11.11.11.11), test program fails the socket opening...");
	StkSocket_AddInfo(0, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("11.11.11.11"), 2000);
	StkSocket_Open(0);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_BINDERR && ParamInt2 != 10049) {
		printf("NG\r\n");
		return -1;
	}
	StkSocket_Close(0, TRUE);
	StkSocket_DeleteInfo(0);
	printf("OK [%d]\r\n", ParamInt2);


	printf("[OpenSocket()/CloseSocket() for UDP] : If OpenSocket is called with invalid host name, An error (LOG_NAMESOLVEERR) occurred...");
	StkSocket_AddInfo(0, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("invalid"), 2001);
	StkSocket_Open(0);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	StkSocket_Close(0, TRUE);
	StkSocket_DeleteInfo(0);
	if (Msg != STKSOCKET_LOG_NAMESOLVEERR && ParamInt2 != 11004) {
		printf("NG\r\n");
		return -1;
	}
	printf("OK [%d]\r\n", ParamInt2);


	return 0;
}

int GenerateLogs()
{
	int Msg;
	int LogId;
	TCHAR ParamStr1[256];
	TCHAR ParamStr2[256];
	int ParamInt1;
	int ParamInt2;

	printf("[Logging] : Check the current number of logs is zero...");
	if (StkSocket_GetNumOfLogs() != 0) {
		printf("NG\r\n");
		return -1;
	}
	printf("OK\r\n");


	printf("[Logging] : Repeat OpenSocket and CloseSocket in 40 times for each. Current number of log messages is 64...");
	for (int Loop = 0; Loop < 40; Loop++) {
		StkSocket_AddInfo(Loop, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("127.0.0.1"), Loop + 3000);
		StkSocket_Open(Loop);
	}
	for (int Loop = 0; Loop < 40; Loop++) {
		StkSocket_Close(Loop, TRUE);
		StkSocket_DeleteInfo(Loop);
	}
	if (StkSocket_GetNumOfLogs() != STKSOCKET_MAX_NUM_OF_LOG) {
		printf("NG %d\r\n", StkSocket_GetNumOfLogs());
		return -1;
	}
	printf("OK\r\n");


	printf("[Logging] : Take log messages in 20 times. Check the log messages taken are LOG_SOCKCLOSE...");
	for (int Loop = 0; Loop < 20; Loop++) {
		StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
		if (Msg != STKSOCKET_LOG_SOCKCLOSE) {
			printf("NG\r\n");
			return -1;
		}
	}
	printf("OK\r\n");


	printf("[Logging] : After taking 20 log messages, number of the left log messages is 44...");
	if (StkSocket_GetNumOfLogs() != 44) {
		printf("NG\r\n");
		return -1;
	}
	printf("OK\r\n");


	printf("[Logging] : Moreover take 20 log messages. Check the log messages taken are LOG_SOCKCLOSE...");
	for (int Loop = 0; Loop < 20; Loop++) {
		StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
		if (Msg != STKSOCKET_LOG_SOCKCLOSE) {
			printf("NG\r\n");
			return -1;
		}
	}
	printf("OK\r\n");


	printf("[Logging] : After taking 20 log messages, number of the left log messages is 24...");
	if (StkSocket_GetNumOfLogs() != 24) {
		printf("NG\r\n");
		return -1;
	}
	printf("OK\r\n");


	printf("[Logging] : Take one more log message. Check the log message is LOG_SUCCESSCSBN...");
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_SUCCESSCSBNLS) {
		printf("NG\r\n");
		return -1;
	}
	printf("OK\r\n");


	printf("[Logging] : After ClearLog() execution, number of logs left is zero...");
	StkSocket_ClearLog();
	if (StkSocket_GetNumOfLogs() != 0) {
		printf("NG\r\n");
		return -1;
	}
	StkSocket_ClearLog();
	if (StkSocket_GetNumOfLogs() != 0) {
		printf("NG\r\n");
		return -1;
	}
	printf("OK\r\n");


	printf("[Logging] : Call TakeLastLog() but empty log is returned...");
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != NULL) {
		printf("NG\r\n");
		return -1;
	}
	printf("OK\r\n");


	printf("[Logging] : Repeat OpenSocket and CloseSocket in 40 times for each. Current number of log messages is 64 but after ClearLog is called left Msg becomes zero...");
	for (int Loop = 0; Loop < 40; Loop++) {
		StkSocket_AddInfo(Loop, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("127.0.0.1"), Loop + 2000);
		StkSocket_Open(Loop);
	}
	for (int Loop = 0; Loop < 40; Loop++) {
		StkSocket_Close(Loop, TRUE);
		StkSocket_DeleteInfo(Loop);
	}
	if (StkSocket_GetNumOfLogs() != STKSOCKET_MAX_NUM_OF_LOG) {
		printf("NG %d\r\n", StkSocket_GetNumOfLogs());
		return -1;
	}
	StkSocket_ClearLog();
	if (StkSocket_GetNumOfLogs() != 0) {
		printf("NG\r\n");
		return -1;
	}
	printf("OK\r\n");


	printf("[Logging] : Call TakeFirstLog() but empty log is returned...");
	StkSocket_TakeFirstLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != NULL) {
		printf("NG\r\n");
		return -1;
	}
	printf("OK\r\n");


	printf("[Logging] : Repeat OpenSocket and CloseSocket in 40 times for each...");
	for (int Loop = 0; Loop < 40; Loop++) {
		StkSocket_AddInfo(Loop, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("127.0.0.1"), Loop + 3000);
		StkSocket_Open(Loop);
	}
	for (int Loop = 0; Loop < 40; Loop++) {
		StkSocket_Close(Loop, TRUE);
		StkSocket_DeleteInfo(Loop);
	}
	if (StkSocket_GetNumOfLogs() != STKSOCKET_MAX_NUM_OF_LOG) {
		printf("NG %d\r\n", StkSocket_GetNumOfLogs());
		return -1;
	}
	printf("OK\r\n");


	printf("[Logging] : Take log using TakeFirstLog method in 24 times. Check the log messages taken are STKSOCKET_LOG_SUCCESSCSBNLS...");
	for (int Loop = 0; Loop < 24; Loop++) {
		StkSocket_TakeFirstLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
		if (Msg != STKSOCKET_LOG_SUCCESSCSBNLS) {
			printf("NG\r\n");
			return -1;
		}
	}
	printf("OK\r\n");


	printf("[Logging] : After taking 24 log messages, number of the left log messages is 40...");
	if (StkSocket_GetNumOfLogs() != 40) {
		printf("NG\r\n");
		return -1;
	}
	printf("OK\r\n");


	printf("[Logging] : Take log using TakeFirstLog method in 40 times. Check the log messages taken are STKSOCKET_LOG_SOCKCLOSE...");
	for (int Loop = 0; Loop < 40; Loop++) {
		StkSocket_TakeFirstLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
		if (Msg != STKSOCKET_LOG_SOCKCLOSE) {
			printf("NG\r\n");
			return -1;
		}
	}
	printf("OK\r\n");


	printf("[Logging] : Call TakeFirstLog() but empty log is returned...");
	StkSocket_TakeFirstLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != NULL) {
		printf("NG\r\n");
		return -1;
	}
	printf("OK\r\n");


	return 0;
}

int TestAddDeleteSocketInfo()
{
	printf("[AddSocketInfo()] : If ActionType is invalid, -1 is returned...");
	int Ret = StkSocket_AddInfo(0, STKSOCKET_TYPE_STREAM, 2, _T("11.11.11.11"), 2000);
	if (Ret != -1) {
		printf("NG\r\n");
		return -1;
	}
	printf("OK\r\n");


	printf("[AddSocketInfo()] : Maximum number of stksockets can be registered...");
	for (int Loop = 0; Loop < STKSOCKET_MAX_SOCKET_NUMBER; Loop++) {
		if (StkSocket_AddInfo(Loop, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("11.11.11.11"), Loop + 2000) != 0) {
			printf("NG\r\n");
			return -1;
		}
	}
	printf("OK\r\n");


	printf("[AddSocketInfo()] : If Maximum number of stksockets is exceeded, -1 is returned...");
	if (StkSocket_AddInfo(STKSOCKET_MAX_SOCKET_NUMBER, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("11.11.11.11"), STKSOCKET_MAX_SOCKET_NUMBER + 2000) != -1) {
		printf("NG\r\n");
		return -1;
	}
	printf("OK\r\n");


	printf("[AddSocketInfo()] : If duplicated stksocket ID is registered, -1 is returned...");
	if (StkSocket_AddInfo(0, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("11.11.11.11"), 2000) != -1) {
		printf("NG\r\n");
		return -1;
	}
	printf("OK\r\n");


	printf("[DeleteSocketInfo()] : If nonexisting ID is specified with DeleteSocketInfo(), -1 is returned...");
	if (StkSocket_DeleteInfo(99999) != -1) {
		printf("NG\r\n");
		return -1;
	}
	printf("OK\r\n");


	printf("[DeleteSocketInfo()] : Delete all of stksocketinfos registered. After that check whether GetNumOfStkInfos returns 0...");
	for (int Loop = 0; Loop < STKSOCKET_MAX_SOCKET_NUMBER; Loop++) {
		if (StkSocket_DeleteInfo(Loop) != 0) {
			printf("NG\r\n");
			return -1;
		}
	}
	if (StkSocket_GetNumOfStkInfos() != 0) {
		printf("NG\r\n");
		return -1;
	}
	printf("OK\r\n");

	return 0;
}

DWORD WINAPI TestThreadProc0(LPVOID Param)
{
	int Msg;
	int LogId;
	TCHAR ParamStr1[256];
	TCHAR ParamStr2[256];
	int ParamInt1;
	int ParamInt2;

	StkSocket_AddInfo(0, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("127.0.0.1"), 2001);
	StkSocket_Open(0);
	BYTE Buffer[10000]; 
	BYTE CondStr[1000];

	printf("[Recv/Send] : Appropriate string has been received by receiver...");
	while (TRUE) {
		if (StkSocket_Accept(0) == 0) {
			int Ret = StkSocket_Receive(0, 0, Buffer, 10000, 100, CondStr, 1000, FALSE);
			StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
			if (Ret > 0) {
				if (lstrcmp((TCHAR*)Buffer, _T("Hello, world!!")) == 0 && Msg == STKSOCKET_LOG_ACPTRECV) {
					printf("OK [%S]\r\n", Buffer);
					StkSocket_Send(0, 0, (BYTE*)_T("Reply Hello, world!!"), 50);
					StkSocket_CloseAccept(0, 0, TRUE);
					break;
				} else {
					printf("NG\r\n");
					exit(-1);
				}
			}
		}
	}
	Sleep(1000);

	printf("[Recv/Send] : Appropriate string has been received by receiver...");
	while (TRUE) {
		if (StkSocket_Accept(0) == 0) {
			int Ret = StkSocket_Receive(0, 0, Buffer, 10000, 100, CondStr, 1000, FALSE);
			StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
			if (Ret > 0) {
				if (lstrcmp((TCHAR*)Buffer, _T("Dummy data!!")) == 0 && Msg == STKSOCKET_LOG_ACPTRECV) {
					printf("OK [%S]\r\n", Buffer);
					StkSocket_Send(0, 0, (BYTE*)_T("Reply Dummy data!!"), 50);
					StkSocket_CloseAccept(0, 0, TRUE);
					break;
				} else {
					printf("NG\r\n");
					exit(-1);
				}
			}
		}
	}
	Sleep(1000);

	StkSocket_Close(0, TRUE);
	StkSocket_DeleteInfo(0);
	FinishFlag = TRUE;

	return 0;
}


DWORD WINAPI TestThreadProc1(LPVOID Param)
{
	int Msg;
	int LogId;
	TCHAR ParamStr1[256];
	TCHAR ParamStr2[256];
	int ParamInt1;
	int ParamInt2;

	Sleep(1000);
	StkSocket_AddInfo(1, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, _T("127.0.0.1"), 2001);
	TCHAR Buf[10000];
	int Ret;

	lstrcpy(Buf, _T("Hello, world!!"));
	StkSocket_Connect(1);
	StkSocket_Send(1, 1, (BYTE*)Buf, (lstrlen(Buf) + 1) * sizeof(TCHAR));
	StkSocket_Send(1, 1, (BYTE*)Buf, (lstrlen(Buf) + 1) * sizeof(TCHAR));
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_CNCTSEND || ParamInt1 != (lstrlen(Buf) + 1) * sizeof(TCHAR)) {
		printf("[Recv/Send] : Send data %S...", Buf);
		printf("NG\r\n");
		exit(-1);
	}
	Sleep(1000);
	printf("[Recv/Send] : Sender received data...");
	Ret = 0;
	do {
		Ret = StkSocket_Receive(1, 1, (BYTE*)Buf, 50, 100, NULL, 0, FALSE);
		if (Ret > 0) {
			StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
			if (lstrcmp((TCHAR*)Buf, _T("Reply Hello, world!!")) != 0 || Msg != STKSOCKET_LOG_CNCTRECV) {
				printf("NG\r\n");
				exit(-1);
			}
			printf("OK [%S]\r\n", Buf);
		}
	} while (Ret <= 0);
	StkSocket_Disconnect(1, 1, TRUE);
	Sleep(1000);

	lstrcpy(Buf, _T("Dummy data!!"));
	StkSocket_Connect(1);
	StkSocket_Send(1, 1, (BYTE*)Buf, (lstrlen(Buf) + 1) * sizeof(TCHAR));
	StkSocket_Send(1, 1, (BYTE*)Buf, (lstrlen(Buf) + 1) * sizeof(TCHAR));
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_CNCTSEND || ParamInt1 != (lstrlen(Buf) + 1) * sizeof(TCHAR)) {
		printf("[Recv/Send] : Send data %S...", Buf);
		printf("NG\r\n");
		exit(-1);
	}
	Sleep(1000);
	printf("[Recv/Send] : Sender received data...");
	Ret = 0;
	do {
		Ret = StkSocket_Receive(1, 1, (BYTE*)Buf, 50, 100, NULL, 0, FALSE);
		if (Ret > 0) {
			StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
			if (lstrcmp((TCHAR*)Buf, _T("Reply Dummy data!!")) != 0 || Msg != STKSOCKET_LOG_CNCTRECV) {
				printf("NG\r\n");
				exit(-1);
			}
			printf("OK [%S]\r\n", Buf);
		}
	} while (Ret <= 0);
	StkSocket_Disconnect(1, 1, TRUE);
	Sleep(1000);

	StkSocket_Close(1, TRUE);
	StkSocket_DeleteInfo(1);
	FinishFlag = TRUE;

	return 0;
}

DWORD WINAPI TestThreadProc2(LPVOID Param)
{
	int Msg;
	int LogId;
	TCHAR ParamStr1[256];
	TCHAR ParamStr2[256];
	int ParamInt1;
	int ParamInt2;

	StkSocket_AddInfo(0, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("127.0.0.1"), 2001);
	StkSocket_Open(0);
	BYTE Buffer[10000]; 
	BYTE CondStr[1000];

	while (TRUE) {
		if (StkSocket_Accept(0) == 0) {
			int Ret = StkSocket_Receive(0, 0, Buffer, 10000, 100, CondStr, 1000, FALSE);
			StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
			if (Ret > 0) {
				printf("[Recv/Send2] : Appropriate string has been received by receiver...");
				if (lstrcmp((TCHAR*)Buffer, _T("Hello, world!!")) == 0 && Msg == STKSOCKET_LOG_ACPTRECV) {
					printf("OK [%S]\r\n", Buffer);
					break;
				} else {
					printf("NG\r\n");
					exit(-1);
				}
			}
		}
	}

	StkSocket_DeleteInfo(0);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg == STKSOCKET_LOG_CLOSEACCLISNSOCK && LogId == 0) {
		printf("[Recv/Send2] : Receiver socket close is called...OK\r\n");
	} else {
		printf("[Recv/Send2] : Receiver socket close is called...NG\r\n");
		exit(-1);
	}
	Sleep(1000);
	FinishFlag = TRUE;
	return 0;
}

DWORD WINAPI TestThreadProc3(LPVOID Param)
{
	int Msg;
	int LogId;
	TCHAR ParamStr1[256];
	TCHAR ParamStr2[256];
	int ParamInt1;
	int ParamInt2;

	Sleep(1000);
	StkSocket_AddInfo(1, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, _T("127.0.0.1"), 2001);
	TCHAR Buf[10000];

	printf("[Recv/Send2] : Sender sent data...");
	lstrcpy(Buf, _T("Hello, world!!"));
	StkSocket_Connect(1);
	StkSocket_Send(1, 1, (BYTE*)Buf, (lstrlen(Buf) + 1) * sizeof(TCHAR));
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_CNCTSEND || ParamInt1 != (lstrlen(Buf) + 1) * sizeof(TCHAR)) {
		printf("NG\r\n");
		exit(-1);
	}
	printf("OK [%S]\r\n", Buf);

	StkSocket_DeleteInfo(1);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg == STKSOCKET_LOG_SOCKCLOSE && LogId == 1) {
		printf("[Recv/Send2] : Sender socket close is called...OK\r\n");
	} else {
		printf("[Recv/Send2] : Sender socket close is called...NG\r\n");
		exit(-1);
	}
	Sleep(1000);
	FinishFlag = TRUE;
	return 0;
}

DWORD WINAPI TestThreadProc4(LPVOID Param)
{
	int Msg;
	int LogId;
	TCHAR ParamStr1[256];
	TCHAR ParamStr2[256];
	int ParamInt1;
	int ParamInt2;

	StkSocket_AddInfo(20, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("127.0.0.1"), 2002);
	StkSocket_AddInfo(0, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("127.0.0.1"), 2001);
	StkSocket_AddInfo(30, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("127.0.0.1"), 2003);
	StkSocket_Open(0);

	if (StkSocket_GetNumOfStkInfos() < 3) {
		printf("[Recv/Send for UDP] : Number of socket info is invalid...NG\r\n");
		exit(-1);
	}
	{
		int TargetId;
		int SockType;
		int ActionType;
		TCHAR TargetAddr[256];
		int TargetPort;
		BOOL CopiedFlag;

		for (int Loop = 0; Loop < StkSocket_GetNumOfStkInfos(); Loop++) {
			StkSocket_GetInfo(0, &TargetId, &SockType, &ActionType, TargetAddr, &TargetPort, &CopiedFlag);
			if (SockType != STKSOCKET_TYPE_DGRAM ||
				lstrcmp(TargetAddr, _T("127.0.0.1")) != 0 ||
				CopiedFlag != FALSE) {
				printf("[Recv/Send for UDP] : NG (Acquired socket information is invalid.)\r\n");
				return -1;
			}
		}
	}

	BYTE Buffer[10000]; 
	while (TRUE) {
		int Ret = StkSocket_ReceiveUdp(0, 0, Buffer, 10000);
		if (Ret > 0) {
			StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
			if (lstrcmp((TCHAR*)Buffer, _T("Hello, world!!")) == 0 && Msg == STKSOCKET_LOG_UDPRECV) {
				printf("[Recv/Send for UDP] : Appropriate string has been received by receiver...OK [%S]\r\n", Buffer);
				break;
			} else {
				printf("[Recv/Send for UDP] : Appropriate string has been received by receiver...NG\r\n");
				exit(-1);
			}
		}
	}

	StkSocket_DeleteInfo(20);
	StkSocket_DeleteInfo(30);

	Sleep(1000);

	TCHAR Buf[10000];
	lstrcpy(Buf, _T("Shinya Takeuchi"));
	int Ret = StkSocket_SendUdp(0, 0, (BYTE*)Buf, (lstrlen(Buf) + 1) * sizeof(TCHAR));
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_UDPSEND || ParamInt1 != (lstrlen(Buf) + 1) * sizeof(TCHAR)) {
		printf("[Recv/Send for UDP] : Receiver replied data...NG\r\n");
		exit(-1);
	}
	printf("[Recv/Send for UDP] : Receiver replied data...OK [%S]\r\n", Buf);

	Sleep(1000);

	StkSocket_Close(0, TRUE);
	StkSocket_DeleteInfo(0);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg == STKSOCKET_LOG_UDPSOCKCLOSE && LogId == 0) {
		printf("[Recv/Send for UDP] : Receiver socket close is called...OK\r\n");
	} else {
		printf("[Recv/Send for UDP] : Receiver socket close is called...NG\r\n");
		exit(-1);
	}
	Sleep(1000);
	FinishFlag = TRUE;

	return 0;
}

DWORD WINAPI TestThreadProc5(LPVOID Param)
{
	int Msg;
	int LogId;
	TCHAR ParamStr1[256];
	TCHAR ParamStr2[256];
	int ParamInt1;
	int ParamInt2;

	Sleep(1000);
	StkSocket_AddInfo(1, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_SENDER, _T("127.0.0.1"), 2001);

	TCHAR Buf[10000];
	lstrcpy(Buf, _T("Hello, world!!"));
	StkSocket_Connect(1);
	StkSocket_SendUdp(1, 1, (BYTE*)Buf, (lstrlen(Buf) + 1) * sizeof(TCHAR));
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_UDPSEND || ParamInt1 != (lstrlen(Buf) + 1) * sizeof(TCHAR)) {
		printf("[Recv/Send for UDP] : Sender sent data...NG\r\n");
		exit(-1);
	}
	printf("[Recv/Send for UDP] : Sender sent data...OK [%S]\r\n", Buf);

	BYTE Buffer[10000]; 
	while (TRUE) {
		int Ret = StkSocket_ReceiveUdp(1, 1, Buffer, 10000);
		if (Ret > 0) {
			StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
			if (lstrcmp((TCHAR*)Buffer, _T("Shinya Takeuchi")) == 0 && Msg == STKSOCKET_LOG_UDPRECV) {
				printf("[Recv/Send for UDP] : Appropriate string has been received by sender...OK [%S]\r\n", Buffer);
				break;
			} else {
				printf("[Recv/Send for UDP] : Appropriate string has been received by sender...NG\r\n");
				exit(-1);
			}
		}
	}

	StkSocket_Disconnect(1, 1, TRUE);
	StkSocket_DeleteInfo(1);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg == STKSOCKET_LOG_UDPSOCKCLOSE && LogId == 1) {
		printf("[Recv/Send for UDP] : Sender socket close is called...OK\r\n");
	} else {
		printf("[Recv/Send for UDP] : Sender socket close is called...NG\r\n");
		exit(-1);
	}
	Sleep(1000);
	FinishFlag = TRUE;
	return 0;
}

DWORD WINAPI TestThreadForAcceptRecv1(LPVOID Param)
{
	BYTE Buf[1000000];
	BOOL S1Flag = FALSE;
	BOOL S2Flag = FALSE;
	BOOL S3Flag = FALSE;
	srand(101);
	while (TRUE) {
		if (StkSocket_Accept(101) == 0) {
			StkSocket_Receive(101, 101, Buf, 1000000, 100, NULL, 0, FALSE);
			if (strstr((char*)Buf, "#1") != 0) S1Flag = TRUE;
			if (strstr((char*)Buf, "#2") != 0) S2Flag = TRUE;
			if (strstr((char*)Buf, "#3") != 0) S3Flag = TRUE;
			StkSocket_CloseAccept(101, 101, TRUE);
			if (S1Flag && S2Flag && S3Flag) break;
			Sleep((rand() % 10) * 100);
		}
	}
	printf("[Multi accepts] : Acceptor #1 is working...OK\r\n");
	FindFlagCounter++;
	return 0;
}

DWORD WINAPI TestThreadForAcceptRecv2(LPVOID Param)
{
	BYTE Buf[1000000];
	BOOL S1Flag = FALSE;
	BOOL S2Flag = FALSE;
	BOOL S3Flag = FALSE;
	srand(102);
	while (TRUE) {
		if (StkSocket_Accept(102) == 0) {
			StkSocket_Receive(102, 102, Buf, 1000000, 100, NULL, 0, FALSE);
			if (strstr((char*)Buf, "#1") != 0) S1Flag = TRUE;
			if (strstr((char*)Buf, "#2") != 0) S2Flag = TRUE;
			if (strstr((char*)Buf, "#3") != 0) S3Flag = TRUE;
			StkSocket_CloseAccept(102, 102, TRUE);
			if (S1Flag && S2Flag && S3Flag) break;
			Sleep((rand() % 10) * 100);
		}
	}
	printf("[Multi accepts] : Acceptor #2 is working...OK\r\n");
	FindFlagCounter++;
	return 0;
}

DWORD WINAPI TestThreadForAcceptRecv3(LPVOID Param)
{
	BYTE Buf[1000000];
	BOOL S1Flag = FALSE;
	BOOL S2Flag = FALSE;
	BOOL S3Flag = FALSE;
	srand(103);
	while (TRUE) {
		if (StkSocket_Accept(103) == 0) {
			StkSocket_Receive(103, 103, Buf, 1000000, 100, NULL, 0, FALSE);
			if (strstr((char*)Buf, "#1") != 0) S1Flag = TRUE;
			if (strstr((char*)Buf, "#2") != 0) S2Flag = TRUE;
			if (strstr((char*)Buf, "#3") != 0) S3Flag = TRUE;
			StkSocket_CloseAccept(103, 103, TRUE);
			if (S1Flag && S2Flag && S3Flag) break;
			Sleep((rand() % 10) * 100);
		}
	}
	printf("[Multi accepts] : Acceptor #3 is working...OK\r\n");
	FindFlagCounter++;
	return 0;
}

DWORD WINAPI TestThreadForAcceptSend1(LPVOID Param)
{
	char Buf[1024];
	BOOL S1Flag = FALSE;
	BOOL S2Flag = FALSE;
	BOOL S3Flag = FALSE;
	strcpy_s(Buf, 1024, "Hello, world from #1\r\n");
	for (int Loop = 0; Loop < 50; Loop++) {
		StkSocket_Connect(201);
		StkSocket_Send(201, 201, (BYTE*)Buf, strlen(Buf) + 1);
		StkSocket_Disconnect(201, 201, FALSE);
		if (FindFlagCounter >= 3) {
			break;
		}
	}

	FindFlagCounter++;
	return 0;
}

DWORD WINAPI TestThreadForAcceptSend2(LPVOID Param)
{
	char Buf[1024];
	strcpy_s(Buf, 1024, "Hello, world from #2\r\n");
	for (int Loop = 0; Loop < 50; Loop++) {
		StkSocket_Connect(202);
		StkSocket_Send(202, 202, (BYTE*)Buf, strlen(Buf) + 1);
		StkSocket_Disconnect(202, 202, FALSE);
		if (FindFlagCounter >= 3) {
			break;
		}
	}

	FindFlagCounter++;
	return 0;
}

DWORD WINAPI TestThreadForAcceptSend3(LPVOID Param)
{
	char Buf[1024];
	strcpy_s(Buf, 1024, "Hello, world from #3\r\n");
	for (int Loop = 0; Loop < 50; Loop++) {
		StkSocket_Connect(203);
		StkSocket_Send(203, 203, (BYTE*)Buf, strlen(Buf) + 1);
		StkSocket_Disconnect(203, 203, FALSE);
		if (FindFlagCounter >= 3) {
			break;
		}
	}

	FindFlagCounter++;
	return 0;
}

void TestThreadForAcceptSendRecv()
{
	StkSocket_AddInfo(101, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("127.0.0.1"), 2001);
	if (StkSocket_CopyInfo(102, 101) != -1) {
		printf("[Multi accepts] : CopySocketInfo successed ... OK\r\n");
	} else {
		printf("[Multi accepts] : Failed to CopySocketInfo execution ... NG\r\n");
		exit(-1);
	}
	if (StkSocket_GetNumOfStkInfos() == 2) {
		printf("[Multi accepts] : The number of StkInfo is two ... OK\r\n");
	} else {
		printf("[Multi accepts] : The number of StkInfo is not two ... NG\r\n");
		exit(-1);
	}
	StkSocket_DeleteInfo(102);
	if (StkSocket_GetNumOfStkInfos() == 1) {
		printf("[Multi accepts] : After deleting StkSock, the number of StkInfo is one ... OK\r\n");
	} else {
		printf("[Multi accepts] : After deleting StkSock, the number of StkInfo is not two ... NG\r\n");
		exit(-1);
	}

	StkSocket_Open(101);
	if (StkSocket_CopyInfo(102, 101) != 0) {
		printf("[Multi accepts] : Invalid return code detected ... NG\r\n");
		exit(-1);
	} else {
		printf("[Multi accepts] : Success to copy StkSocketInfo ... OK\r\n");
	}
	if (StkSocket_CopyInfo(103, 101) != 0) {
		printf("[Multi accepts] : Invalid return code detected ... NG\r\n");
		exit(-1);
	} else {
		printf("[Multi accepts] : Success to copy one more StkSocketInfo ... OK\r\n");
	}
	if (StkSocket_CopyInfo(104, 999) != -1) {
		printf("[Multi accepts] : Invalid return code detected ... NG\r\n");
		exit(-1);
	} else {
		printf("[Multi accepts] : Success to detect invalid code for unexistence base info ... OK\r\n");
	}
	StkSocket_AddInfo(201, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, _T("127.0.0.1"), 2001);
	StkSocket_AddInfo(202, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, _T("127.0.0.1"), 2001);
	StkSocket_AddInfo(203, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, _T("127.0.0.1"), 2001);
	if (StkSocket_GetNumOfStkInfos() == 6) {
		printf("[Multi accepts] : 3 socket information exists ... OK\r\n");
	} else {
		printf("[Multi accepts] : 3 socket information exists ... NG\r\n");
		exit(-1);
	}

	FindFlagCounter = 0;
	DWORD TmpId;
	CreateThread(NULL, 0, &TestThreadForAcceptRecv1, NULL, 0, &TmpId);
	CreateThread(NULL, 0, &TestThreadForAcceptRecv2, NULL, 0, &TmpId);
	CreateThread(NULL, 0, &TestThreadForAcceptRecv3, NULL, 0, &TmpId);
	CreateThread(NULL, 0, &TestThreadForAcceptSend1, NULL, 0, &TmpId);
	CreateThread(NULL, 0, &TestThreadForAcceptSend2, NULL, 0, &TmpId);
	CreateThread(NULL, 0, &TestThreadForAcceptSend3, NULL, 0, &TmpId);
	while (FindFlagCounter != 6) {
		Sleep(1000);
	}
	Sleep(1000);

	printf("[Multi accepts] : Try StkSocket_CopyInfo() after source socket closure with StkSocket_Close() ... ");
	StkSocket_Close(101, TRUE);
	if (StkSocket_CopyInfo(104, 101) != -1) {
		printf("OK\r\n");
	} else {
		printf("NG\r\n");
		exit(-1);
	}
	StkSocket_DeleteInfo(101);
	StkSocket_DeleteInfo(102);
	StkSocket_DeleteInfo(103);
	StkSocket_DeleteInfo(104);
	StkSocket_DeleteInfo(201);
	StkSocket_DeleteInfo(202);
	StkSocket_DeleteInfo(203);
	if (StkSocket_GetNumOfStkInfos() == 0) {
		printf("[Multi accepts] : No socket information exists ... OK\r\n");
	} else {
		printf("[Multi accepts] : No socket information exists ... NG\r\n");
		exit(-1);
	}
}

void TestMultiAccept1()
{
	StkSocket_AddInfo(111, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("127.0.0.1"), 2201);
	StkSocket_AddInfo(121, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("127.0.0.1"), 2202);
	StkSocket_AddInfo(131, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("127.0.0.1"), 2203);
	StkSocket_Open(111);
	StkSocket_Open(121);
	StkSocket_Open(131);
	StkSocket_CopyInfo(122, 121);
	StkSocket_CopyInfo(123, 121);
	StkSocket_CopyInfo(124, 121);
	if (StkSocket_GetStatus(111) != STKSOCKET_STATUS_OPEN ||
		StkSocket_GetStatus(121) != STKSOCKET_STATUS_OPEN ||
		StkSocket_GetStatus(131) != STKSOCKET_STATUS_OPEN ||
		StkSocket_GetStatus(122) != STKSOCKET_STATUS_OPEN ||
		StkSocket_GetStatus(123) != STKSOCKET_STATUS_OPEN ||
		StkSocket_GetStatus(124) != STKSOCKET_STATUS_OPEN) {
			printf("[Multi accepts] : Status validation [open] ... NG\r\n");
			exit(-1);
	}
	printf("[Multi accepts] : Status validation [open] ... OK\r\n");

	StkSocket_Close(121, FALSE);
	if (StkSocket_GetStatus(111) != STKSOCKET_STATUS_OPEN ||
		StkSocket_GetStatus(121) != STKSOCKET_STATUS_CLOSE ||
		StkSocket_GetStatus(131) != STKSOCKET_STATUS_OPEN ||
		StkSocket_GetStatus(122) != STKSOCKET_STATUS_CLOSE ||
		StkSocket_GetStatus(123) != STKSOCKET_STATUS_CLOSE ||
		StkSocket_GetStatus(124) != STKSOCKET_STATUS_CLOSE) {
			printf("[Multi accepts] : Status validation [close] ... NG\r\n");
			exit(-1);
	}

	StkSocket_Close(111, FALSE);
	StkSocket_Close(131, FALSE);
	if (StkSocket_GetStatus(111) != STKSOCKET_STATUS_CLOSE ||
		StkSocket_GetStatus(131) != STKSOCKET_STATUS_CLOSE) {
			printf("[Multi accepts] : Status validation [close+] ... NG\r\n");
			exit(-1);
	}
	printf("[Multi accepts] : Status validation [close] ... OK\r\n");
	StkSocket_DeleteInfo(111);
	StkSocket_DeleteInfo(121);
	StkSocket_DeleteInfo(131);
	StkSocket_DeleteInfo(122);
	StkSocket_DeleteInfo(123);
	StkSocket_DeleteInfo(124);
}

int main(int Argc, char* Argv[])
{
	//AddExceptionForFilewall();

	printf("Test started\r\n");

	if (ConnectDisconnectTcpPort() != 0) {
		return -1;
	}
	if (ConnectDisconnectUdpPort() != 0) {
		return -1;
	}
	if (OpenClosePort() != 0) {
		return -1;
	}
	if (OpenCloseUdpPort() != 0) {
		return -1;
	}
	if (GenerateLogs() != 0) {
		return -1;
	}
	if (TestAddDeleteSocketInfo() != 0) {
		return -1;
	}
	DWORD TmpId;

	FinishFlag = FALSE;
	CreateThread(NULL, 0, &TestThreadProc0, NULL, 0, &TmpId);
	CreateThread(NULL, 0, &TestThreadProc1, NULL, 0, &TmpId);
	while (FinishFlag == FALSE) {
		Sleep(1000);
	}
	Sleep(1000);

	FinishFlag = FALSE;
	CreateThread(NULL, 0, &TestThreadProc2, NULL, 0, &TmpId);
	CreateThread(NULL, 0, &TestThreadProc3, NULL, 0, &TmpId);
	while (FinishFlag == FALSE) {
		Sleep(1000);
	}
	Sleep(1000);

	FinishFlag = FALSE;
	CreateThread(NULL, 0, &TestThreadProc4, NULL, 0, &TmpId);
	CreateThread(NULL, 0, &TestThreadProc5, NULL, 0, &TmpId);
	while (FinishFlag == FALSE) {
		Sleep(1000);
	}
	Sleep(1000);

	TestThreadForAcceptSendRecv();
	TestMultiAccept1();
	StkSocketTestMa objStkSocketTestMa;
	objStkSocketTestMa.TestMultiAccept2();
	objStkSocketTestMa.TestMultiAccept3();

	StkSocketTestGetSockInfo objStkSocketGetSockInfo;
	if (objStkSocketGetSockInfo.TestAddDel() != 0) {
		return -1;
	}

	StkSocketIPv6 objStkSocketIPv6;
	objStkSocketIPv6.TestIPv6();
	objStkSocketIPv6.TestIPv6Udp();

	StkSocketMemoryLeak objStkSocketMemoryLeak;
	objStkSocketMemoryLeak.CheckLeakTcp();
	objStkSocketMemoryLeak.CheckLeakUdp();

	StkSocketTestHttp TestHttpObj;
	TestHttpObj.TestHttpTermination();

	printf("Test completed\r\n");

	//DeleteExceptionForFirewall();
	return 0;
}
