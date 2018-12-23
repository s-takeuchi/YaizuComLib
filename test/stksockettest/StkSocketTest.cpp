#include <winsock2.h>
#include <windows.h>
#include <cstdio>
#include <cstring>
#include <shlwapi.h>
#include "..\..\src\stksocket\stksocket.h"
#include "StkSocketTestMa.h"
#include "StkSocketTestGetSockInfo.h"
#include "StkSocketIPv6.h"
#include "StkSocketMemoryLeak.h"
#include "StkSocketTestHttp.h"

bool StartFlag = false;
bool PeerCloseOkFlag = false;
bool FinishFlag = false;
int FindFlagCounter = 0;


int ConnectDisconnectTcpPort()
{
	int Msg;
	int LogId;
	wchar_t ParamStr1[256];
	wchar_t ParamStr2[256];
	int ParamInt1;
	int ParamInt2;

	printf("[ConnectSocket()/DisconnectSocket() for TCP] : Connect invalid IP address and port...");
	StkSocket_AddInfo(22, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2222);
	StkSocket_Connect(22);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_CONNERROR && ParamInt2 != 10061) {
		printf("NG\r\n");
		return -1;
	}
	StkSocket_Disconnect(22, 22, true);
	StkSocket_Close(22, true);
	StkSocket_DeleteInfo(22);
	printf("OK [%d]\r\n", ParamInt2);
	return 0;
}

int ConnectDisconnectUdpPort()
{
	int Msg;
	int LogId;
	wchar_t ParamStr1[256];
	wchar_t ParamStr2[256];
	int ParamInt1;
	int ParamInt2;

	printf("[ConnectSocket()/DisconnectSocket() for UDP] : Only open and close socket.  It will be ended successfully...");
	StkSocket_AddInfo(22, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2222);
	StkSocket_Connect(22);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_SUCCESSCS) {
		printf("NG (LOG_SUCCESSCS is not returned.\r\n");
		return -1;
	}
	StkSocket_Disconnect(22, 22, true);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_UDPSOCKCLOSE) {
		printf("NG (LOG_UDPSOCKCLOSE is not returned.)\r\n");
		return -1;
	}
	StkSocket_DeleteInfo(22);
	printf("OK\r\n");
	return 0;
}

int OpenClosePort()
{
	int Msg;
	int LogId;
	wchar_t ParamStr1[256];
	wchar_t ParamStr2[256];
	int ParamInt1;
	int ParamInt2;


	printf("[OpenSocket()/CloseSocket()] : Open and close using same port. Check open and close operation are successfully end...");
	StkSocket_AddInfo(0, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2001);
	if (StkSocket_GetNumOfStkInfos() != 1) {
		printf("NG (Number of Socket Info is invalid.)\r\n");
		return -1;
	}
	{
		int TargetId;
		int SockType;
		int ActionType;
		wchar_t TargetAddr[256];
		int TargetPort;
		bool CopiedFlag;

		StkSocket_GetInfo(0, &TargetId, &SockType, &ActionType, TargetAddr, &TargetPort, &CopiedFlag);
		if (TargetId != 0 ||
			SockType != STKSOCKET_TYPE_STREAM ||
			ActionType != STKSOCKET_ACTIONTYPE_RECEIVER ||
			lstrcmp(TargetAddr, L"127.0.0.1") != 0 ||
			TargetPort != 2001 ||
			CopiedFlag != false) {
			printf("NG (Acquired socket information is not matched.)\r\n");
			return -1;
		}
	}
	StkSocket_Open(0);
	StkSocket_Close(0, true);
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
	StkSocket_AddInfo(1, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2001);
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
	StkSocket_Close(1, true);
	StkSocket_DeleteInfo(1);
	if (StkSocket_GetNumOfLogs() != 1) {
		printf("NG\r\n");
		return -1;
	}
	StkSocket_ClearLog();
	printf("OK\r\n");


	printf("[OpenSocket()/CloseSocket()] : If CloseSocket is called with nonexisting ID, the method processes nothing...");
	StkSocket_Close(9999, true);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != 0) {
		printf("NG\r\n");
		return -1;
	}
	printf("OK\r\n");


	printf("[OpenSocket()/CloseSocket()] : If the receiver IP address is invalid(11.11.11.11), test program fails the socket opening...");
	StkSocket_AddInfo(0, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"11.11.11.11", 2000);
	StkSocket_Open(0);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_BINDLISTENERR && ParamInt2 != 10049) {
		printf("NG\r\n");
		return -1;
	}
	StkSocket_Close(0, true);
	StkSocket_DeleteInfo(0);
	printf("OK [%d]\r\n", ParamInt2);


	printf("[OpenSocket()/CloseSocket()] : If OpenSocket is called with invalid host name, An error (LOG_NAMESOLVEERR) occurred...");
	StkSocket_AddInfo(0, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"invalid", 2001);
	StkSocket_Open(0);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	StkSocket_Close(0, true);
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
	wchar_t ParamStr1[256];
	wchar_t ParamStr2[256];
	int ParamInt1;
	int ParamInt2;


	printf("[OpenSocket()/CloseSocket() for UDP] : Open and close using same port. Check open and close operation are successfully end...");
	StkSocket_AddInfo(0, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2001);
	StkSocket_Open(0);
	StkSocket_Close(0, true);
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
	StkSocket_AddInfo(1, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2001);
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
	StkSocket_Close(1, true);
	StkSocket_DeleteInfo(1);
	if (StkSocket_GetNumOfLogs() != 1) {
		printf("NG\r\n");
		return -1;
	}
	StkSocket_ClearLog();
	printf("OK\r\n");


	printf("[OpenSocket()/CloseSocket() for UDP] : If CloseSocket is called with nonexisting ID, the method processes nothing...");
	StkSocket_Close(9999, true);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != 0) {
		printf("NG\r\n");
		return -1;
	}
	printf("OK\r\n");


	printf("[OpenSocket()/CloseSocket() for UDP] : If the receiver IP address is invalid(11.11.11.11), test program fails the socket opening...");
	StkSocket_AddInfo(0, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"11.11.11.11", 2000);
	StkSocket_Open(0);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_BINDERR && ParamInt2 != 10049) {
		printf("NG\r\n");
		return -1;
	}
	StkSocket_Close(0, true);
	StkSocket_DeleteInfo(0);
	printf("OK [%d]\r\n", ParamInt2);


	printf("[OpenSocket()/CloseSocket() for UDP] : If OpenSocket is called with invalid host name, An error (LOG_NAMESOLVEERR) occurred...");
	StkSocket_AddInfo(0, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"invalid", 2001);
	StkSocket_Open(0);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	StkSocket_Close(0, true);
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
	wchar_t ParamStr1[256];
	wchar_t ParamStr2[256];
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
		StkSocket_AddInfo(Loop, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", Loop + 3000);
		StkSocket_Open(Loop);
	}
	for (int Loop = 0; Loop < 40; Loop++) {
		StkSocket_Close(Loop, true);
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
		StkSocket_AddInfo(Loop, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", Loop + 2000);
		StkSocket_Open(Loop);
	}
	for (int Loop = 0; Loop < 40; Loop++) {
		StkSocket_Close(Loop, true);
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
		StkSocket_AddInfo(Loop, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", Loop + 3000);
		StkSocket_Open(Loop);
	}
	for (int Loop = 0; Loop < 40; Loop++) {
		StkSocket_Close(Loop, true);
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
	int Ret = StkSocket_AddInfo(0, STKSOCKET_TYPE_STREAM, 2, L"11.11.11.11", 2000);
	if (Ret != -1) {
		printf("NG\r\n");
		return -1;
	}
	printf("OK\r\n");


	printf("[AddSocketInfo()] : Maximum number of stksockets can be registered...");
	for (int Loop = 0; Loop < STKSOCKET_MAX_SOCKET_NUMBER; Loop++) {
		if (StkSocket_AddInfo(Loop, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"11.11.11.11", Loop + 2000) != 0) {
			printf("NG\r\n");
			return -1;
		}
	}
	printf("OK\r\n");


	printf("[AddSocketInfo()] : If Maximum number of stksockets is exceeded, -1 is returned...");
	if (StkSocket_AddInfo(STKSOCKET_MAX_SOCKET_NUMBER, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"11.11.11.11", STKSOCKET_MAX_SOCKET_NUMBER + 2000) != -1) {
		printf("NG\r\n");
		return -1;
	}
	printf("OK\r\n");


	printf("[AddSocketInfo()] : If duplicated stksocket ID is registered, -1 is returned...");
	if (StkSocket_AddInfo(0, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"11.11.11.11", 2000) != -1) {
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

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//

DWORD WINAPI TestThreadProc0(LPVOID Param)
{
	int Msg;
	int LogId;
	wchar_t ParamStr1[256];
	wchar_t ParamStr2[256];
	int ParamInt1;
	int ParamInt2;

	StkSocket_AddInfo(0, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2001);
	StkSocket_Open(0);
	unsigned char Buffer[10000]; 
	unsigned char CondStr[1000];

	printf("[Recv/Send] : Appropriate string has been received by receiver...");
	while (true) {
		if (StkSocket_Accept(0) == 0) {
			int Ret = StkSocket_Receive(0, 0, Buffer, 10000, STKSOCKET_RECV_FINISHCOND_UNCONDITIONAL, 100, CondStr, 1000);
			StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
			if (Ret > 0) {
				if (lstrcmp((wchar_t*)Buffer, L"Hello, world!!") == 0 && Msg == STKSOCKET_LOG_ACPTRECV) {
					printf("OK [%S]\r\n", (wchar_t*)Buffer);
					StkSocket_Send(0, 0, (unsigned char*)L"Reply Hello, world!!", 50);
					StkSocket_CloseAccept(0, 0, true);
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
	while (true) {
		if (StkSocket_Accept(0) == 0) {
			int Ret = StkSocket_Receive(0, 0, Buffer, 10000, STKSOCKET_RECV_FINISHCOND_UNCONDITIONAL, 100, CondStr, 1000);
			StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
			if (Ret > 0) {
				if (lstrcmp((wchar_t*)Buffer, L"Dummy data!!") == 0 && Msg == STKSOCKET_LOG_ACPTRECV) {
					printf("OK [%S]\r\n", (wchar_t*)Buffer);
					StkSocket_Send(0, 0, (unsigned char*)L"Reply Dummy data!!", 50);
					StkSocket_CloseAccept(0, 0, true);
					break;
				} else {
					printf("NG\r\n");
					exit(-1);
				}
			}
		}
	}
	Sleep(1000);

	StkSocket_Close(0, true);
	StkSocket_DeleteInfo(0);
	FinishFlag = true;

	return 0;
}


DWORD WINAPI TestThreadProc1(LPVOID Param)
{
	int Msg;
	int LogId;
	wchar_t ParamStr1[256];
	wchar_t ParamStr2[256];
	int ParamInt1;
	int ParamInt2;

	Sleep(1000);
	StkSocket_AddInfo(1, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2001);
	wchar_t Buf[10000];
	int Ret;

	lstrcpy(Buf, L"Hello, world!!");
	StkSocket_Connect(1);
	StkSocket_Send(1, 1, (unsigned char*)Buf, (lstrlen(Buf) + 1) * sizeof(wchar_t));
	StkSocket_Send(1, 1, (unsigned char*)Buf, (lstrlen(Buf) + 1) * sizeof(wchar_t));
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_CNCTSEND || ParamInt1 != (lstrlen(Buf) + 1) * sizeof(wchar_t)) {
		printf("[Recv/Send] : Send data %S...", Buf);
		printf("NG\r\n");
		exit(-1);
	}
	Sleep(1000);
	printf("[Recv/Send] : Sender received data...");
	Ret = 0;
	do {
		Ret = StkSocket_Receive(1, 1, (unsigned char*)Buf, 50, STKSOCKET_RECV_FINISHCOND_UNCONDITIONAL, 100, NULL, 0);
		if (Ret > 0) {
			StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
			if (lstrcmp((wchar_t*)Buf, L"Reply Hello, world!!") != 0 || Msg != STKSOCKET_LOG_CNCTRECV) {
				printf("NG\r\n");
				exit(-1);
			}
			printf("OK [%S]\r\n", Buf);
		}
	} while (Ret <= 0);
	StkSocket_Disconnect(1, 1, true);
	Sleep(1000);

	lstrcpy(Buf, L"Dummy data!!");
	StkSocket_Connect(1);
	StkSocket_Send(1, 1, (unsigned char*)Buf, (lstrlen(Buf) + 1) * sizeof(wchar_t));
	StkSocket_Send(1, 1, (unsigned char*)Buf, (lstrlen(Buf) + 1) * sizeof(wchar_t));
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_CNCTSEND || ParamInt1 != (lstrlen(Buf) + 1) * sizeof(wchar_t)) {
		printf("[Recv/Send] : Send data %S...", Buf);
		printf("NG\r\n");
		exit(-1);
	}
	Sleep(1000);
	printf("[Recv/Send] : Sender received data...");
	Ret = 0;
	do {
		Ret = StkSocket_Receive(1, 1, (unsigned char*)Buf, 50, STKSOCKET_RECV_FINISHCOND_UNCONDITIONAL, 100, NULL, 0);
		if (Ret > 0) {
			StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
			if (lstrcmp((wchar_t*)Buf, L"Reply Dummy data!!") != 0 || Msg != STKSOCKET_LOG_CNCTRECV) {
				printf("NG\r\n");
				exit(-1);
			}
			printf("OK [%S]\r\n", Buf);
		}
	} while (Ret <= 0);
	StkSocket_Disconnect(1, 1, true);
	Sleep(1000);

	StkSocket_Close(1, true);
	StkSocket_DeleteInfo(1);
	FinishFlag = true;

	return 0;
}

DWORD WINAPI TestThreadProc2(LPVOID Param)
{
	int Msg;
	int LogId;
	wchar_t ParamStr1[256];
	wchar_t ParamStr2[256];
	int ParamInt1;
	int ParamInt2;

	StkSocket_AddInfo(0, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2001);
	StkSocket_Open(0);
	unsigned char Buffer[10000]; 
	unsigned char CondStr[1000];

	while (true) {
		if (StkSocket_Accept(0) == 0) {
			int Ret = StkSocket_Receive(0, 0, Buffer, 10000, STKSOCKET_RECV_FINISHCOND_UNCONDITIONAL, 100, CondStr, 1000);
			StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
			if (Ret > 0) {
				printf("[Recv/Send2] : Appropriate string has been received by receiver...");
				if (lstrcmp((wchar_t*)Buffer, L"Hello, world!!") == 0 && Msg == STKSOCKET_LOG_ACPTRECV) {
					printf("OK [%S]\r\n", (wchar_t*)Buffer);
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
	FinishFlag = true;
	return 0;
}

DWORD WINAPI TestThreadProc3(LPVOID Param)
{
	int Msg;
	int LogId;
	wchar_t ParamStr1[256];
	wchar_t ParamStr2[256];
	int ParamInt1;
	int ParamInt2;

	Sleep(1000);
	StkSocket_AddInfo(1, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2001);
	wchar_t Buf[10000];

	printf("[Recv/Send2] : Sender sent data...");
	lstrcpy(Buf, L"Hello, world!!");
	StkSocket_Connect(1);
	StkSocket_Send(1, 1, (unsigned char*)Buf, (lstrlen(Buf) + 1) * sizeof(wchar_t));
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_CNCTSEND || ParamInt1 != (lstrlen(Buf) + 1) * sizeof(wchar_t)) {
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
	FinishFlag = true;
	return 0;
}

DWORD WINAPI TestThreadProc4(LPVOID Param)
{
	int Msg;
	int LogId;
	wchar_t ParamStr1[256];
	wchar_t ParamStr2[256];
	int ParamInt1;
	int ParamInt2;

	StkSocket_AddInfo(20, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2002);
	StkSocket_AddInfo(0, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2001);
	StkSocket_AddInfo(30, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2003);
	StkSocket_Open(0);

	if (StkSocket_GetNumOfStkInfos() < 3) {
		printf("[Recv/Send for UDP] : Number of socket info is invalid...NG\r\n");
		exit(-1);
	}
	{
		int TargetId;
		int SockType;
		int ActionType;
		wchar_t TargetAddr[256];
		int TargetPort;
		bool CopiedFlag;

		for (int Loop = 0; Loop < StkSocket_GetNumOfStkInfos(); Loop++) {
			StkSocket_GetInfo(0, &TargetId, &SockType, &ActionType, TargetAddr, &TargetPort, &CopiedFlag);
			if (SockType != STKSOCKET_TYPE_DGRAM ||
				lstrcmp(TargetAddr, L"127.0.0.1") != 0 ||
				CopiedFlag != false) {
				printf("[Recv/Send for UDP] : NG (Acquired socket information is invalid.)\r\n");
				return -1;
			}
		}
	}

	unsigned char Buffer[10000]; 
	while (true) {
		int Ret = StkSocket_ReceiveUdp(0, 0, Buffer, 10000);
		if (Ret > 0) {
			StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
			if (StrStr((wchar_t*)Buffer, L"Hello, world!!") != 0 && (Msg == STKSOCKET_LOG_UDPRECV || Msg == STKSOCKET_LOG_UDPSEND)) { // There is a possibility that STKSOCKET_LOG_ACPTSEND is taken depends on the timing.
				printf("[Recv/Send for UDP] : Appropriate string has been received by receiver...OK [%S]\r\n", (wchar_t*)Buffer);
				break;
			} else {
				printf("[Recv/Send for UDP] : Appropriate string has been received by receiver...NG [%S:%d]\r\n", (wchar_t*)Buffer, Msg);
				exit(-1);
			}
		}
	}

	StkSocket_DeleteInfo(20);
	StkSocket_DeleteInfo(30);

	Sleep(1000);

	wchar_t Buf[10000];
	for (int LoopSnd = 0; LoopSnd < 50; LoopSnd++) {
		_snwprintf_s(Buf, 10000, L"Shinya Takeuchi %d", LoopSnd);
		int Ret = StkSocket_SendUdp(0, 0, (unsigned char*)Buf, (lstrlen(Buf) + 1) * sizeof(wchar_t));
		Sleep(10);
	}
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_UDPSEND || ParamInt1 != (lstrlen(Buf) + 1) * sizeof(wchar_t)) {
		printf("[Recv/Send for UDP] : Receiver replied data...NG\r\n");
		exit(-1);
	}
	printf("[Recv/Send for UDP] : Receiver replied data...OK [%S]\r\n", Buf);

	Sleep(1000);

	StkSocket_Close(0, true);
	StkSocket_DeleteInfo(0);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg == STKSOCKET_LOG_UDPSOCKCLOSE && LogId == 0) {
		printf("[Recv/Send for UDP] : Receiver socket close is called...OK\r\n");
	} else {
		printf("[Recv/Send for UDP] : Receiver socket close is called...NG\r\n");
		exit(-1);
	}
	Sleep(1000);
	FinishFlag = true;

	return 0;
}

DWORD WINAPI TestThreadProc5(LPVOID Param)
{
	int Msg;
	int LogId;
	wchar_t ParamStr1[256];
	wchar_t ParamStr2[256];
	int ParamInt1;
	int ParamInt2;

	Sleep(1000);
	StkSocket_AddInfo(1, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2001);

	wchar_t Buf[10000];
	StkSocket_Connect(1);
	for (int LoopSnd = 0; LoopSnd < 50; LoopSnd++) {
		_snwprintf_s(Buf, 10000, L"Hello, world!! %d", LoopSnd);
		StkSocket_SendUdp(1, 1, (unsigned char*)Buf, (lstrlen(Buf) + 1) * sizeof(wchar_t));
		Sleep(10);
	}
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_UDPSEND || ParamInt1 != (lstrlen(Buf) + 1) * sizeof(wchar_t)) {
		printf("[Recv/Send for UDP] : Sender sent data...NG\r\n");
		exit(-1);
	}
	printf("[Recv/Send for UDP] : Sender sent data...OK [%S]\r\n", Buf);

	unsigned char Buffer[10000]; 
	while (true) {
		int Ret = StkSocket_ReceiveUdp(1, 1, Buffer, 10000);
		if (Ret > 0) {
			StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
			if (StrStr((wchar_t*)Buffer, L"Shinya Takeuchi") != 0 && (Msg == STKSOCKET_LOG_UDPRECV || Msg == STKSOCKET_LOG_UDPSEND)) { // There is a possibility that STKSOCKET_LOG_ACPTSEND is taken depends on the timing.
				printf("[Recv/Send for UDP] : Appropriate string has been received by sender...OK [%S]\r\n", (wchar_t*)Buffer);
				break;
			} else {
				printf("[Recv/Send for UDP] : Appropriate string has been received by sender...NG [%S:%d]\r\n", (wchar_t*)Buffer, Msg);
				exit(-1);
			}
		}
	}

	StkSocket_Disconnect(1, 1, true);
	StkSocket_DeleteInfo(1);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg == STKSOCKET_LOG_UDPSOCKCLOSE && LogId == 1) {
		printf("[Recv/Send for UDP] : Sender socket close is called...OK\r\n");
	} else {
		printf("[Recv/Send for UDP] : Sender socket close is called...NG\r\n");
		exit(-1);
	}
	Sleep(1000);
	FinishFlag = true;
	return 0;
}

DWORD WINAPI TestThreadProc6(LPVOID Param)
{
	printf("[Recv/Send] : Keep waiting for closure of sender ...");
	StkSocket_AddInfo(0, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2001);
	StkSocket_Open(0);
	unsigned char Buffer[10000];
	unsigned char CondStr[1000];
	while (true) {
		int Ret = StkSocket_Receive(0, 0, Buffer, 10000, STKSOCKET_RECV_FINISHCOND_UNCONDITIONAL, 100, CondStr, 1000);
		if (Ret > 0) {
			printf("NG\r\n");
			exit(-1);
		}
		if (FinishFlag == true) {
			break;
		}
	}
	StkSocket_Close(0, false);
	StkSocket_DeleteInfo(0);
	printf("OK\r\n");
	return 0;
}

DWORD WINAPI TestThreadProc7(LPVOID Param)
{
	Sleep(3000);
	FinishFlag = true;
	return 0;
}

DWORD WINAPI TestThreadProc8(LPVOID Param)
{
	printf("[Recv/Send] : Receiver method out detected with timeout=0...");
	StkSocket_AddInfo(0, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2001);
	StkSocket_Open(0);
	unsigned char Buffer[10000];
	unsigned char CondStr[1000];

	while (true) {
		if (StkSocket_Accept(0) == 0) {
			int Ret = StkSocket_Receive(0, 0, Buffer, 10000, STKSOCKET_RECV_FINISHCOND_TIMEOUT, 0, CondStr, 1000);
			if (Ret > 0) {
				break;
			}
		}
	}
	StkSocket_Close(0, true);
	StkSocket_DeleteInfo(0);
	printf("OK\r\n");
	FinishFlag = true;
	return 0;
}

DWORD WINAPI TestThreadProc9(LPVOID Param)
{
	Sleep(1000);
	StkSocket_AddInfo(1, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2001);
	wchar_t Buf[10000];

	lstrcpy(Buf, L"Hello, world!!");
	StkSocket_Connect(1);
	StkSocket_Send(1, 1, (unsigned char*)Buf, (lstrlen(Buf) + 1) * sizeof(wchar_t));
	Sleep(3000);
	StkSocket_ForceStop(0);
	StkSocket_Disconnect(1, 1, true);
	StkSocket_DeleteInfo(1);
	return 0;
}

DWORD WINAPI TestThreadProc10(LPVOID Param)
{
	unsigned char Buffer[65536] = "";
	unsigned char TestStr[65536] = "";
	int *Command = (int*)Param;
	int RecvType = 0;
	int Size = 0;

	if (*Command == 0) {
		RecvType = 256;
		strcpy_s((char*)TestStr, 65536, "012345678901234567890123456789");
		Size = 30;
	}
	if (*Command >= 1 && *Command <= 4) {
		RecvType = *Command * -1;
		strcpy_s((char*)TestStr, 65536, "01234567890123456789012345678901234567890123456789");
		Size = 50;
	}
	if (*Command == 5) {
		RecvType = -3;
		strcpy_s((char*)TestStr, 65536, "POST / HTTP/1.1\r\nTransfer-Encoding: chunked\r\nContent-Type: text/html\r\n\r\nTestTestTestHello!!!0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef");
		Size = 156;
	}

	StkSocket_AddInfo(10, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2002);
	StkSocket_Open(10);
	StartFlag = true;
	
	printf("[Recv/Send] : Receiver's buffer overflow occurrence (Command=%d) ...", RecvType);
	while (true) {
		if (StkSocket_Accept(10) == 0) {
			memset(Buffer, '\0', 512);
			int Ret = StkSocket_Receive(10, 10, Buffer, Size, RecvType, 0, NULL, 0);
			if (Ret != Size || strncmp((char*)Buffer, (char*)TestStr, Size) != 0) {
				printf("NG (return=%d, expectation=%d)\r\n", Ret, Size);
				exit(-1);
			}
			break;
		}
	}
	printf("OK\r\n");
	PeerCloseOkFlag = true;

	StkSocket_Close(10, true);
	StkSocket_DeleteInfo(10);
	FinishFlag = true;
	return 0;
}

DWORD WINAPI TestThreadProc11(LPVOID Param)
{
	char Buf[65536];
	int *Command = (int*)Param;
	if (*Command >= 0 && *Command <= 4) {
		strcpy_s(Buf, 65536, "012345678901234567890123456789012345678901234567890123456789");
	}
	if (*Command == 5) {
		strcpy_s(Buf, 65536, "POST / HTTP/1.1\r\nTransfer-Encoding: chunked\r\nContent-Type: text/html\r\n\r\n0c\r\nTestTestTest\r\n0008\r\nHello!!!\r\n00000020\r\n0123456789abcdef0123456789abcdef\r\n00000020\r\n0123456789abcdef0123456789abcdef\r\n00\r\n\r\n");
	}

	StkSocket_AddInfo(1, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2002);

	while (StartFlag == false) {
		Sleep(100);
	}
	StkSocket_Connect(1);
	StkSocket_Send(1, 1, (unsigned char*)Buf, strlen(Buf));
	while (PeerCloseOkFlag == false) {
		Sleep(100);
	}
	StkSocket_Disconnect(1, 1, true);

	StkSocket_DeleteInfo(1);
	return 0;
}

//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD WINAPI TestThreadForAcceptRecv1(LPVOID Param)
{
	unsigned char Buf[1000000];
	bool S1Flag = false;
	bool S2Flag = false;
	bool S3Flag = false;
	srand(101);
	while (true) {
		if (StkSocket_Accept(101) == 0) {
			StkSocket_Receive(101, 101, Buf, 1000000, STKSOCKET_RECV_FINISHCOND_UNCONDITIONAL, 100, NULL, 0);
			if (strstr((char*)Buf, "#1") != 0) S1Flag = true;
			if (strstr((char*)Buf, "#2") != 0) S2Flag = true;
			if (strstr((char*)Buf, "#3") != 0) S3Flag = true;
			StkSocket_CloseAccept(101, 101, true);
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
	unsigned char Buf[1000000];
	bool S1Flag = false;
	bool S2Flag = false;
	bool S3Flag = false;
	srand(102);
	while (true) {
		if (StkSocket_Accept(102) == 0) {
			StkSocket_Receive(102, 102, Buf, 1000000, STKSOCKET_RECV_FINISHCOND_UNCONDITIONAL, 100, NULL, 0);
			if (strstr((char*)Buf, "#1") != 0) S1Flag = true;
			if (strstr((char*)Buf, "#2") != 0) S2Flag = true;
			if (strstr((char*)Buf, "#3") != 0) S3Flag = true;
			StkSocket_CloseAccept(102, 102, true);
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
	unsigned char Buf[1000000];
	bool S1Flag = false;
	bool S2Flag = false;
	bool S3Flag = false;
	srand(103);
	while (true) {
		if (StkSocket_Accept(103) == 0) {
			StkSocket_Receive(103, 103, Buf, 1000000, STKSOCKET_RECV_FINISHCOND_UNCONDITIONAL, 100, NULL, 0);
			if (strstr((char*)Buf, "#1") != 0) S1Flag = true;
			if (strstr((char*)Buf, "#2") != 0) S2Flag = true;
			if (strstr((char*)Buf, "#3") != 0) S3Flag = true;
			StkSocket_CloseAccept(103, 103, true);
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
	bool S1Flag = false;
	bool S2Flag = false;
	bool S3Flag = false;
	strcpy_s(Buf, 1024, "Hello, world from #1\r\n");
	for (int Loop = 0; Loop < 50; Loop++) {
		StkSocket_Connect(201);
		StkSocket_Send(201, 201, (unsigned char*)Buf, strlen(Buf) + 1);
		StkSocket_Disconnect(201, 201, false);
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
		StkSocket_Send(202, 202, (unsigned char*)Buf, strlen(Buf) + 1);
		StkSocket_Disconnect(202, 202, false);
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
		StkSocket_Send(203, 203, (unsigned char*)Buf, strlen(Buf) + 1);
		StkSocket_Disconnect(203, 203, false);
		if (FindFlagCounter >= 3) {
			break;
		}
	}

	FindFlagCounter++;
	return 0;
}

void TestThreadForAcceptSendRecv()
{
	StkSocket_AddInfo(101, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2001);
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
	StkSocket_AddInfo(201, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2001);
	StkSocket_AddInfo(202, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2001);
	StkSocket_AddInfo(203, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2001);
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
	StkSocket_Close(101, true);
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
	StkSocket_AddInfo(111, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2201);
	StkSocket_AddInfo(121, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2202);
	StkSocket_AddInfo(131, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2203);
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

	StkSocket_Close(121, false);
	if (StkSocket_GetStatus(111) != STKSOCKET_STATUS_OPEN ||
		StkSocket_GetStatus(121) != STKSOCKET_STATUS_CLOSE ||
		StkSocket_GetStatus(131) != STKSOCKET_STATUS_OPEN ||
		StkSocket_GetStatus(122) != STKSOCKET_STATUS_CLOSE ||
		StkSocket_GetStatus(123) != STKSOCKET_STATUS_CLOSE ||
		StkSocket_GetStatus(124) != STKSOCKET_STATUS_CLOSE) {
			printf("[Multi accepts] : Status validation [close] ... NG\r\n");
			exit(-1);
	}

	StkSocket_Close(111, false);
	StkSocket_Close(131, false);
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
	DWORD TmpId;

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

	StkSocketTestHttp TestHttpObj;
	TestHttpObj.TestHttpTermination();

	FinishFlag = false;
	CreateThread(NULL, 0, &TestThreadProc0, NULL, 0, &TmpId);
	CreateThread(NULL, 0, &TestThreadProc1, NULL, 0, &TmpId);
	while (FinishFlag == false) {
		Sleep(1000);
	}
	Sleep(1000);

	FinishFlag = false;
	CreateThread(NULL, 0, &TestThreadProc2, NULL, 0, &TmpId);
	CreateThread(NULL, 0, &TestThreadProc3, NULL, 0, &TmpId);
	while (FinishFlag == false) {
		Sleep(1000);
	}
	Sleep(1000);

	FinishFlag = false;
	CreateThread(NULL, 0, &TestThreadProc4, NULL, 0, &TmpId);
	CreateThread(NULL, 0, &TestThreadProc5, NULL, 0, &TmpId);
	while (FinishFlag == false) {
		Sleep(1000);
	}
	Sleep(1000);

	FinishFlag = false;
	CreateThread(NULL, 0, &TestThreadProc6, NULL, 0, &TmpId);
	CreateThread(NULL, 0, &TestThreadProc7, NULL, 0, &TmpId);
	while (FinishFlag == false) {
		Sleep(1000);
	}
	Sleep(1000);

	FinishFlag = false;
	CreateThread(NULL, 0, &TestThreadProc8, NULL, 0, &TmpId);
	CreateThread(NULL, 0, &TestThreadProc9, NULL, 0, &TmpId);
	while (FinishFlag == false) {
		Sleep(1000);
	}
	Sleep(1000);

	for (int Loop = 0; Loop <= 5; Loop++) {
		StartFlag = false;
		PeerCloseOkFlag = false;
		FinishFlag = false;
		int Command = Loop;
		CreateThread(NULL, 0, &TestThreadProc10, &Command, 0, &TmpId);
		CreateThread(NULL, 0, &TestThreadProc11, &Command, 0, &TmpId);
		while (FinishFlag == false) {
			Sleep(100);
		}
	}

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

	printf("Test completed\r\n");

	//DeleteExceptionForFirewall();
	return 0;
}
