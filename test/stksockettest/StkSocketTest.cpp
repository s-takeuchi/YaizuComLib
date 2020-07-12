#include <thread>
#include <mutex>
#include <chrono>
#include <cstring>

#include "../../src/stkpl/StkPl.h"
#include "../../src/stksocket/stksocket.h"
#include "StkSocketTestHttp.h"
#include "StkSocketTestMa.h"
#include "StkSocketIPv6.h"
#include "StkSocketTestGetSockInfo.h"
#include "StkSocketMemoryLeak.h"

bool StartFlag = false;
bool PeerCloseOkFlag = false;
bool FinishFlag = false;
bool SslFlag = false;
int FindFlagCounter = 0;
std::mutex Cs4Log;


int ConnectDisconnectTcpPort()
{
	int Msg;
	int LogId;
	wchar_t ParamStr1[256];
	wchar_t ParamStr2[256];
	int ParamInt1;
	int ParamInt2;

	StkPlPrintf("[ConnectSocket()/DisconnectSocket() for TCP] : Connect invalid IP address and port...");
	StkSocket_AddInfo(22, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2222);
	StkSocket_Connect(22);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_CONNERROR && ParamInt2 != 10061) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkSocket_Disconnect(22, 22, true);
	StkSocket_Close(22, true);
	StkSocket_DeleteInfo(22);
	StkPlPrintf("OK [%d]\n", ParamInt2);
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

	StkPlPrintf("[ConnectSocket()/DisconnectSocket() for UDP] : Only open and close socket.  It will be ended successfully...");
	StkSocket_AddInfo(22, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2222);
	StkSocket_Connect(22);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_SUCCESSCS) {
		StkPlPrintf("NG (LOG_SUCCESSCS is not returned.\n");
		return -1;
	}
	StkSocket_Disconnect(22, 22, true);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_UDPSOCKCLOSE) {
		StkPlPrintf("NG (LOG_UDPSOCKCLOSE is not returned.)\n");
		return -1;
	}
	StkSocket_DeleteInfo(22);
	StkPlPrintf("OK\n");
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


	StkPlPrintf("[OpenSocket()/CloseSocket()] : Open and close using same port. Check open and close operation are successfully end...");
	StkSocket_AddInfo(0, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2001);
	if (StkSocket_GetNumOfStkInfos() != 1) {
		StkPlPrintf("NG (Number of Socket Info is invalid.)\n");
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
			StkPlWcsCmp(TargetAddr, L"127.0.0.1") != 0 ||
			TargetPort != 2001 ||
			CopiedFlag != false) {
			StkPlPrintf("NG (Acquired socket information is not matched.)\n");
			return -1;
		}
	}
	StkSocket_Open(0);
	StkSocket_Close(0, true);
	StkSocket_DeleteInfo(0);
	if (StkSocket_GetNumOfStkInfos() != 0) {
		StkPlPrintf("NG (Number of Socket Info is invalid. (not zero))\n");
		return -1;
	}
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_SOCKCLOSE) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_SUCCESSCSBNLS) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkPlPrintf("OK\n");


	StkPlPrintf("[OpenSocket()/CloseSocket()] : If OpenSocket is called twice with the same ID, the method processes nothing...");
	StkSocket_AddInfo(1, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2001);
	StkSocket_Open(1);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_SUCCESSCSBNLS) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkSocket_Open(1);
	if (StkSocket_GetNumOfLogs() != 0) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkSocket_Close(1, true);
	StkSocket_DeleteInfo(1);
	if (StkSocket_GetNumOfLogs() != 1) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkSocket_ClearLog();
	StkPlPrintf("OK\n");


	StkPlPrintf("[OpenSocket()/CloseSocket()] : If CloseSocket is called with nonexisting ID, the method processes nothing...");
	StkSocket_Close(9999, true);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != 0) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkPlPrintf("OK\n");


	StkPlPrintf("[OpenSocket()/CloseSocket()] : If the receiver IP address is invalid(11.11.11.11), test program fails the socket opening...");
	StkSocket_AddInfo(0, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"11.11.11.11", 2000);
	StkSocket_Open(0);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_BINDLISTENERR && ParamInt2 != 10049) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkSocket_Close(0, true);
	StkSocket_DeleteInfo(0);
	StkPlPrintf("OK [%d]\n", ParamInt2);


	StkPlPrintf("[OpenSocket()/CloseSocket()] : If OpenSocket is called with invalid host name, An error (LOG_NAMESOLVEERR) occurred...");
	StkSocket_AddInfo(0, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"invalid", 2001);
	StkSocket_Open(0);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	StkSocket_Close(0, true);
	StkSocket_DeleteInfo(0);
	if (Msg != STKSOCKET_LOG_NAMESOLVEERR && ParamInt2 != 11004) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkPlPrintf("OK [%d]\n", ParamInt2);


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


	StkPlPrintf("[OpenSocket()/CloseSocket() for UDP] : Open and close using same port. Check open and close operation are successfully end...");
	StkSocket_AddInfo(0, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2001);
	StkSocket_Open(0);
	StkSocket_Close(0, true);
	StkSocket_DeleteInfo(0);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_UDPSOCKCLOSE) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_SUCCESSCSBN) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkPlPrintf("OK\n");


	StkPlPrintf("[OpenSocket()/CloseSocket() for UDP] : If OpenSocket is called twice with the same ID, the method processes nothing...");
	StkSocket_AddInfo(1, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2001);
	StkSocket_Open(1);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_SUCCESSCSBN) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkSocket_Open(1);
	if (StkSocket_GetNumOfLogs() != 0) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkSocket_Close(1, true);
	StkSocket_DeleteInfo(1);
	if (StkSocket_GetNumOfLogs() != 1) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkSocket_ClearLog();
	StkPlPrintf("OK\n");


	StkPlPrintf("[OpenSocket()/CloseSocket() for UDP] : If CloseSocket is called with nonexisting ID, the method processes nothing...");
	StkSocket_Close(9999, true);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != 0) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkPlPrintf("OK\n");


	StkPlPrintf("[OpenSocket()/CloseSocket() for UDP] : If the receiver IP address is invalid(11.11.11.11), test program fails the socket opening...");
	StkSocket_AddInfo(0, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"11.11.11.11", 2000);
	StkSocket_Open(0);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_BINDERR && ParamInt2 != 10049) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkSocket_Close(0, true);
	StkSocket_DeleteInfo(0);
	StkPlPrintf("OK [%d]\n", ParamInt2);


	StkPlPrintf("[OpenSocket()/CloseSocket() for UDP] : If OpenSocket is called with invalid host name, An error (LOG_NAMESOLVEERR) occurred...");
	StkSocket_AddInfo(0, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"invalid", 2001);
	StkSocket_Open(0);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	StkSocket_Close(0, true);
	StkSocket_DeleteInfo(0);
	if (Msg != STKSOCKET_LOG_NAMESOLVEERR && ParamInt2 != 11004) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkPlPrintf("OK [%d]\n", ParamInt2);


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

	StkPlPrintf("[Logging] : Check the current number of logs is zero...");
	if (StkSocket_GetNumOfLogs() != 0) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkPlPrintf("OK\n");


	StkPlPrintf("[Logging] : Repeat OpenSocket and CloseSocket in 40 times for each. Current number of log messages is 64...");
	for (int Loop = 0; Loop < 40; Loop++) {
		StkSocket_AddInfo(Loop, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", Loop + 2300);
		StkSocket_Open(Loop);
	}
	for (int Loop = 0; Loop < 40; Loop++) {
		StkSocket_Close(Loop, true);
		StkSocket_DeleteInfo(Loop);
	}
	if (StkSocket_GetNumOfLogs() != STKSOCKET_MAX_NUM_OF_LOG) {
		StkPlPrintf("NG %d\n", StkSocket_GetNumOfLogs());
		return -1;
	}
	StkPlPrintf("OK\n");


	StkPlPrintf("[Logging] : Take log messages in 20 times. Check the log messages taken are LOG_SOCKCLOSE...");
	for (int Loop = 0; Loop < 20; Loop++) {
		StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
		if (Msg != STKSOCKET_LOG_SOCKCLOSE) {
			StkPlPrintf("NG\n");
			return -1;
		}
	}
	StkPlPrintf("OK\n");


	StkPlPrintf("[Logging] : After taking 20 log messages, number of the left log messages is 44...");
	if (StkSocket_GetNumOfLogs() != 44) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkPlPrintf("OK\n");


	StkPlPrintf("[Logging] : Moreover take 20 log messages. Check the log messages taken are LOG_SOCKCLOSE...");
	for (int Loop = 0; Loop < 20; Loop++) {
		StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
		if (Msg != STKSOCKET_LOG_SOCKCLOSE) {
			StkPlPrintf("NG\n");
			return -1;
		}
	}
	StkPlPrintf("OK\n");


	StkPlPrintf("[Logging] : After taking 20 log messages, number of the left log messages is 24...");
	if (StkSocket_GetNumOfLogs() != 24) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkPlPrintf("OK\n");


	StkPlPrintf("[Logging] : Take one more log message. Check the log message is LOG_SUCCESSCSBN...");
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_SUCCESSCSBNLS) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkPlPrintf("OK\n");


	StkPlPrintf("[Logging] : After ClearLog() execution, number of logs left is zero...");
	StkSocket_ClearLog();
	if (StkSocket_GetNumOfLogs() != 0) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkSocket_ClearLog();
	if (StkSocket_GetNumOfLogs() != 0) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkPlPrintf("OK\n");


	StkPlPrintf("[Logging] : Call TakeLastLog() but empty log is returned...");
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != 0) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkPlPrintf("OK\n");


	StkPlPrintf("[Logging] : Repeat OpenSocket and CloseSocket in 40 times for each. Current number of log messages is 64 but after ClearLog is called left Msg becomes zero...");
	for (int Loop = 0; Loop < 40; Loop++) {
		StkSocket_AddInfo(Loop, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", Loop + 2000);
		StkSocket_Open(Loop);
	}
	for (int Loop = 0; Loop < 40; Loop++) {
		StkSocket_Close(Loop, true);
		StkSocket_DeleteInfo(Loop);
	}
	if (StkSocket_GetNumOfLogs() != STKSOCKET_MAX_NUM_OF_LOG) {
		StkPlPrintf("NG %d\n", StkSocket_GetNumOfLogs());
		return -1;
	}
	StkSocket_ClearLog();
	if (StkSocket_GetNumOfLogs() != 0) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkPlPrintf("OK\n");


	StkPlPrintf("[Logging] : Call TakeFirstLog() but empty log is returned...");
	StkSocket_TakeFirstLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != 0) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkPlPrintf("OK\n");


	StkPlPrintf("[Logging] : Repeat OpenSocket and CloseSocket in 40 times for each...");
	for (int Loop = 0; Loop < 40; Loop++) {
		StkSocket_AddInfo(Loop, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", Loop + 2300);
		StkSocket_Open(Loop);
	}
	for (int Loop = 0; Loop < 40; Loop++) {
		StkSocket_Close(Loop, true);
		StkSocket_DeleteInfo(Loop);
	}
	if (StkSocket_GetNumOfLogs() != STKSOCKET_MAX_NUM_OF_LOG) {
		StkPlPrintf("NG %d\n", StkSocket_GetNumOfLogs());
		return -1;
	}
	StkPlPrintf("OK\n");


	StkPlPrintf("[Logging] : Take log using TakeFirstLog method in 24 times. Check the log messages taken are STKSOCKET_LOG_SUCCESSCSBNLS...");
	for (int Loop = 0; Loop < 24; Loop++) {
		StkSocket_TakeFirstLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
		if (Msg != STKSOCKET_LOG_SUCCESSCSBNLS) {
			StkPlPrintf("NG\n");
			return -1;
		}
	}
	StkPlPrintf("OK\n");


	StkPlPrintf("[Logging] : After taking 24 log messages, number of the left log messages is 40...");
	if (StkSocket_GetNumOfLogs() != 40) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkPlPrintf("OK\n");


	StkPlPrintf("[Logging] : Take log using TakeFirstLog method in 40 times. Check the log messages taken are STKSOCKET_LOG_SOCKCLOSE...");
	for (int Loop = 0; Loop < 40; Loop++) {
		StkSocket_TakeFirstLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
		if (Msg != STKSOCKET_LOG_SOCKCLOSE) {
			StkPlPrintf("NG\n");
			return -1;
		}
	}
	StkPlPrintf("OK\n");


	StkPlPrintf("[Logging] : Call TakeFirstLog() but empty log is returned...");
	StkSocket_TakeFirstLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != 0) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkPlPrintf("OK\n");


	return 0;
}

int TestAddDeleteSocketInfo()
{
	StkPlPrintf("[AddSocketInfo()] : If ActionType is invalid, -1 is returned...");
	int Ret = StkSocket_AddInfo(0, STKSOCKET_TYPE_STREAM, 2, L"11.11.11.11", 2000);
	if (Ret != -1) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkPlPrintf("OK\n");


	StkPlPrintf("[AddSocketInfo()] : Maximum number of stksockets can be registered...");
	for (int Loop = 0; Loop < STKSOCKET_MAX_SOCKET_NUMBER; Loop++) {
		if (StkSocket_AddInfo(Loop, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"11.11.11.11", Loop + 2000) != 0) {
			StkPlPrintf("NG\n");
			return -1;
		}
	}
	StkPlPrintf("OK\n");


	StkPlPrintf("[AddSocketInfo()] : If Maximum number of stksockets is exceeded, -1 is returned...");
	if (StkSocket_AddInfo(STKSOCKET_MAX_SOCKET_NUMBER, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"11.11.11.11", STKSOCKET_MAX_SOCKET_NUMBER + 2000) != -1) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkPlPrintf("OK\n");


	StkPlPrintf("[AddSocketInfo()] : If duplicated stksocket ID is registered, -1 is returned...");
	if (StkSocket_AddInfo(0, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"11.11.11.11", 2000) != -1) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkPlPrintf("OK\n");


	StkPlPrintf("[DeleteSocketInfo()] : If nonexisting ID is specified with DeleteSocketInfo(), -1 is returned...");
	if (StkSocket_DeleteInfo(99999) != -1) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkPlPrintf("OK\n");


	StkPlPrintf("[DeleteSocketInfo()] : Delete all of stksocketinfos registered. After that check whether GetNumOfStkInfos returns 0...");
	for (int Loop = 0; Loop < STKSOCKET_MAX_SOCKET_NUMBER; Loop++) {
		if (StkSocket_DeleteInfo(Loop) != 0) {
			StkPlPrintf("NG\n");
			return -1;
		}
	}
	if (StkSocket_GetNumOfStkInfos() != 0) {
		StkPlPrintf("NG\n");
		return -1;
	}
	StkPlPrintf("OK\n");

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
//

void TestThreadProc0(bool SslMode)
{
	int Msg;
	int LogId;
	wchar_t ParamStr1[256];
	wchar_t ParamStr2[256];
	int ParamInt1;
	int ParamInt2;

	StkSocket_AddInfo(0, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2001);
	StkSocket_AddInfo(1, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2001);
	if (SslMode) {
		StkSocket_SecureForRecv(0, "./server.key", "./server.crt");
	}
	StkSocket_Open(0);
	StartFlag = true;

	unsigned char Buffer[10000];
	unsigned char CondStr[1000];

	while (true) {
		Cs4Log.lock();
		int AccRet = StkSocket_Accept(0);
		if (AccRet != 0) {
			Cs4Log.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		} else {
			Cs4Log.unlock();
			break;
		}
	}
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		Cs4Log.lock();
		int Ret = StkSocket_Receive(0, 0, Buffer, 10000, STKSOCKET_RECV_FINISHCOND_UNCONDITIONAL, 100, CondStr, 1000);
		StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
		Cs4Log.unlock();
		if (Ret > 0) {
			StkPlPrintf("[Recv/Send%s] : Appropriate string has been received by receiver...", SslMode ? "(SSL/TSL)" : "");
			if (StkPlWcsCmp((wchar_t*)Buffer, L"Hello, world!!") == 0 && Msg == STKSOCKET_LOG_ACPTRECV) {
				StkPlPrintf("OK [%ls]\n", (wchar_t*)Buffer);
				const wchar_t* TmpDat = L"Reply Hello, world!!";
				StkSocket_Send(0, 0, (const unsigned char*)TmpDat, (int)(StkPlWcsLen(TmpDat) + 1) * sizeof(wchar_t));
				StkSocket_CloseAccept(0, 0, true);
				break;
			} else {
				StkPlPrintf("NG\n");
				exit(-1);
			}
		}
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	while (true) {
		Cs4Log.lock();
		int AccRet = StkSocket_Accept(0);
		if (AccRet != 0) {
			Cs4Log.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			continue;
		} else {
			Cs4Log.unlock();
			break;
		}
	}
	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		Cs4Log.lock();
		int Ret = StkSocket_Receive(0, 0, Buffer, 10000, STKSOCKET_RECV_FINISHCOND_UNCONDITIONAL, 100, CondStr, 1000);
		StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
		Cs4Log.unlock();
		if (Ret > 0) {
			StkPlPrintf("[Recv/Send%s] : Appropriate string has been received by receiver...", SslMode ? "(SSL/TSL)" : "");
			if (StkPlWcsCmp((wchar_t*)Buffer, L"Dummy data!!") == 0 && Msg == STKSOCKET_LOG_ACPTRECV) {
				StkPlPrintf("OK [%ls]\n", (wchar_t*)Buffer);
				const wchar_t* TmpDat = L"Reply Dummy data!!";
				StkSocket_Send(0, 0, (const unsigned char*)TmpDat, (int)(StkPlWcsLen(TmpDat) + 1) * sizeof(wchar_t));
				StkSocket_CloseAccept(0, 0, true);
				break;
			} else {
				StkPlPrintf("NG\n");
				exit(-1);
			}
		}
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	StkSocket_Close(0, true);
}

void TestThreadProc1(bool SslMode)
{
	int Msg;
	int LogId;
	wchar_t ParamStr1[256];
	wchar_t ParamStr2[256];
	int ParamInt1;
	int ParamInt2;

	while (!StartFlag) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	if (SslMode) {
		StkSocket_SecureForSend(1, "./ca.crt", NULL);
	}
	wchar_t Buf[10000];
	int Ret;

	StkPlLStrCpy(Buf, L"Hello, world!!");
	while (StkSocket_Connect(1) == -1) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	Cs4Log.lock();
	StkSocket_Send(1, 1, (const unsigned char*)Buf, (int)(StkPlWcsLen(Buf) + 1) * sizeof(wchar_t));
	StkSocket_Send(1, 1, (const unsigned char*)Buf, (int)(StkPlWcsLen(Buf) + 1) * sizeof(wchar_t));
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	Cs4Log.unlock();
	if (Msg != STKSOCKET_LOG_CNCTSEND || ParamInt1 != (StkPlWcsLen(Buf) + 1) * sizeof(wchar_t)) {
		StkPlPrintf("[Recv/Send%s] : Send data ...", SslMode ? "(SSL/TSL)" : "");
		StkPlPrintf("NG [Buf=%ls, ID=%d, Msg=%d]\n", Buf, LogId, Msg);
		exit(-1);
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	Ret = 0;
	do {
		Ret = StkSocket_Receive(1, 1, (unsigned char*)Buf, 10000, STKSOCKET_RECV_FINISHCOND_PEERCLOSURE, 100, NULL, 0);
		if (Ret > 0) {
			StkPlPrintf("[Recv/Send%s] : Sender received data...", SslMode ? "(SSL/TSL)" : "");
			StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
			if (StkPlWcsCmp((wchar_t*)Buf, L"Reply Hello, world!!") != 0 || Msg != STKSOCKET_LOG_CNCTRECV) {
				StkPlPrintf("NG [Buf=%ls, ID=%d, Msg=%d]\n", Buf, LogId, Msg);
				exit(-1);
			}
			StkPlPrintf("OK [%ls]\n", Buf);
		}
	} while (Ret <= 0);
	StkSocket_Disconnect(1, 1, true);
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	StkPlLStrCpy(Buf, L"Dummy data!!");
	while (StkSocket_Connect(1) == -1) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	Cs4Log.lock();
	StkSocket_Send(1, 1, (const unsigned char*)Buf, (int)(StkPlWcsLen(Buf) + 1) * sizeof(wchar_t));
	StkSocket_Send(1, 1, (const unsigned char*)Buf, (int)(StkPlWcsLen(Buf) + 1) * sizeof(wchar_t));
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	Cs4Log.unlock();
	if (Msg != STKSOCKET_LOG_CNCTSEND || ParamInt1 != (StkPlWcsLen(Buf) + 1) * sizeof(wchar_t)) {
		StkPlPrintf("[Recv/Send%s] : Send data %ls...", SslMode ? "(SSL/TSL)" : "", Buf);
		StkPlPrintf("NG\n");
		exit(-1);
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	Ret = 0;
	do {
		Ret = StkSocket_Receive(1, 1, (unsigned char*)Buf, 10000, STKSOCKET_RECV_FINISHCOND_PEERCLOSURE, 100, NULL, 0);
		if (Ret > 0) {
			StkPlPrintf("[Recv/Send%s] : Sender received data...", SslMode ? "(SSL/TSL)" : "");
			StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
			if (StkPlWcsCmp((wchar_t*)Buf, L"Reply Dummy data!!") != 0 || Msg != STKSOCKET_LOG_CNCTRECV) {
				StkPlPrintf("NG [Buf=%ls, ID=%d, Msg=%d]\n", Buf, LogId, Msg);
				exit(-1);
			}
			StkPlPrintf("OK [%ls]\n", Buf);
		}
	} while (Ret <= 0);
	StkSocket_Disconnect(1, 1, true);
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	StkSocket_Close(1, true);

	while (StkSocket_GetStatus(0) != STKSOCKET_STATUS_CLOSE && StkSocket_GetStatus(1) != STKSOCKET_STATUS_CLOSE) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	StkSocket_DeleteInfo(0);
	StkSocket_DeleteInfo(1);
}

void TestThreadProc2(bool SslMode)
{
	int Msg;
	int LogId;
	wchar_t ParamStr1[256];
	wchar_t ParamStr2[256];
	int ParamInt1;
	int ParamInt2;

	StkSocket_AddInfo(0, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2020);
	StkSocket_AddInfo(1, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2020);

	if (SslMode) {
		StkSocket_SecureForRecv(0, "./server.key", "./server.crt");
	}
	if (StkSocket_Open(0) == -1) {
		StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
		StkPlPrintf("[Recv/Send2%s] : Appropriate string has been received by receiver... NG [Msg=%d, ParamInt1=%d, ParamInt2=%d]\n", SslMode ? "(SSL/TSL)" : "", Msg, ParamInt1, ParamInt2);
		exit(-1);
	}
	StartFlag = true;

	unsigned char Buffer[10000];
	unsigned char CondStr[1000];

	while (true) {
		if (StkSocket_Accept(0) == 0) {
			Cs4Log.lock();
			int Ret = StkSocket_Receive(0, 0, Buffer, 10000, STKSOCKET_RECV_FINISHCOND_UNCONDITIONAL, 100, CondStr, 1000);
			StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
			Cs4Log.unlock();
			if (Ret > 0) {
				StkPlPrintf("[Recv/Send2%s] : Appropriate string has been received by receiver...", SslMode ? "(SSL/TSL)" : "");
				if (StkPlWcsCmp((wchar_t*)Buffer, L"Hello, world!!") == 0 && Msg == STKSOCKET_LOG_ACPTRECV) {
					StkPlPrintf("OK [%ls]\n", (wchar_t*)Buffer);
					break;
				} else {
					StkPlPrintf("NG\n");
					exit(-1);
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
	StkSocket_Close(0, false);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg == STKSOCKET_LOG_CLOSEACCLISNSOCK && LogId == 0) {
		StkPlPrintf("[Recv/Send2%s] : Receiver socket force close is called...OK\n", SslMode ? "(SSL/TSL)" : "");
	} else {
		StkPlPrintf("[Recv/Send2%s] : Receiver socket force close is called...NG(Msg=%d, LogId=%d)\n", SslMode ? "(SSL/TSL)" : "", Msg, LogId);
		exit(-1);
	}
	FinishFlag = true;
}

void TestThreadProc3(bool SslMode)
{
	int Msg;
	int LogId;
	wchar_t ParamStr1[256];
	wchar_t ParamStr2[256];
	int ParamInt1;
	int ParamInt2;

	while (!StartFlag) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	if (SslMode) {
		StkSocket_SecureForSend(1, "./ca.crt", NULL);
	}
	wchar_t Buf[10000];

	StkPlPrintf("[Recv/Send2%s] : Sender sent data...", SslMode ? "(SSL/TSL)" : "");
	StkPlLStrCpy(Buf, L"Hello, world!!");
	while (StkSocket_Connect(1) == -1) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		StkPlPrintf("{Re} ");
	}
	Cs4Log.lock();
	StkSocket_Send(1, 1, (const unsigned char*)Buf, (int)(StkPlWcsLen(Buf) + 1) * sizeof(wchar_t));
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	Cs4Log.unlock();
	if (Msg != STKSOCKET_LOG_CNCTSEND || ParamInt1 != (StkPlWcsLen(Buf) + 1) * sizeof(wchar_t)) {
		StkPlPrintf("NG [Msg=%d, ParamInt1=%d, ParamInt2=%d]\n", Msg, ParamInt1, ParamInt2);
		exit(-1);
	}
	StkPlPrintf("OK [%ls]\n", Buf);

	while (!FinishFlag) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	StkSocket_Close(1, false);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg == STKSOCKET_LOG_SOCKCLOSE && LogId == 1) {
		StkPlPrintf("[Recv/Send2%s] : Sender socket force close is called...OK\n", SslMode ? "(SSL/TSL)" : "");
	} else {
		StkPlPrintf("[Recv/Send2%s] : Sender socket force close is called...NG(Msg=%d, LogId=%d)\n", SslMode ? "(SSL/TSL)" : "", Msg, LogId);
		exit(-1);
	}
	StkSocket_DeleteInfo(0);
	StkSocket_DeleteInfo(1);
}

void TestThreadProc4()
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
	StkSocket_AddInfo(1, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2001);

	StkSocket_Open(0);
	StartFlag = true;

	if (StkSocket_GetNumOfStkInfos() < 3) {
		StkPlPrintf("[Recv/Send for UDP] : Number of socket info is invalid...NG\n");
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
				StkPlWcsCmp(TargetAddr, L"127.0.0.1") != 0 ||
				CopiedFlag != false) {
				StkPlPrintf("[Recv/Send for UDP] : NG (Acquired socket information is invalid.)\n");
				exit(-1);
			}
		}
	}

	unsigned char Buffer[10000]; 
	while (true) {
		int Ret = StkSocket_ReceiveUdp(0, 0, Buffer, 10000);
		if (Ret > 0) {
			StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
			if (StkPlWcsStr((wchar_t*)Buffer, L"Hello, world!!") != 0 && (Msg == STKSOCKET_LOG_UDPRECV || Msg == STKSOCKET_LOG_UDPSEND)) { // There is a possibility that STKSOCKET_LOG_ACPTSEND is taken depends on the timing.
				StkPlPrintf("[Recv/Send for UDP] : Appropriate string has been received by receiver...OK [%ls]\n", (wchar_t*)Buffer);
				break;
			} else {
				StkPlPrintf("[Recv/Send for UDP] : Appropriate string has been received by receiver...NG [%ls:%d]\n", (wchar_t*)Buffer, Msg);
				exit(-1);
			}
		}
	}

	StkSocket_DeleteInfo(20);
	StkSocket_DeleteInfo(30);

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	wchar_t Buf[10000];
	for (int LoopSnd = 0; LoopSnd < 50; LoopSnd++) {
		StkPlSwPrintf(Buf, 10000, L"Shinya Takeuchi %d", LoopSnd);
		int Ret = StkSocket_SendUdp(0, 0, (unsigned char*)Buf, (int)(StkPlWcsLen(Buf) + 1) * sizeof(wchar_t));
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_UDPSEND || ParamInt1 != (StkPlWcsLen(Buf) + 1) * sizeof(wchar_t)) {
		StkPlPrintf("[Recv/Send for UDP] : Receiver replied data...NG\n");
		exit(-1);
	}
	StkPlPrintf("[Recv/Send for UDP] : Receiver replied data...OK [%ls]\n", Buf);

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	StkSocket_Close(0, true);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg == STKSOCKET_LOG_UDPSOCKCLOSE && LogId == 0) {
		StkPlPrintf("[Recv/Send for UDP] : Receiver socket close is called...OK\n");
	} else {
		StkPlPrintf("[Recv/Send for UDP] : Receiver socket close is called...NG\n");
		exit(-1);
	}

	while (!FinishFlag) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	StkSocket_DeleteInfo(0);
	StkSocket_DeleteInfo(1);
}

void TestThreadProc5()
{
	int Msg;
	int LogId;
	wchar_t ParamStr1[256];
	wchar_t ParamStr2[256];
	int ParamInt1;
	int ParamInt2;

	while (!StartFlag) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	wchar_t Buf[10000];
	StkSocket_Connect(1);
	for (int LoopSnd = 0; LoopSnd < 50; LoopSnd++) {
		StkPlSwPrintf(Buf, 10000, L"Hello, world!! %d", LoopSnd);
		StkSocket_SendUdp(1, 1, (unsigned char*)Buf, (int)(StkPlWcsLen(Buf) + 1) * sizeof(wchar_t));
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg != STKSOCKET_LOG_UDPSEND || ParamInt1 != (StkPlWcsLen(Buf) + 1) * sizeof(wchar_t)) {
		StkPlPrintf("[Recv/Send for UDP] : Sender sent data...NG\n");
		exit(-1);
	}
	StkPlPrintf("[Recv/Send for UDP] : Sender sent data...OK [%ls]\n", Buf);

	unsigned char Buffer[10000]; 
	while (true) {
		int Ret = StkSocket_ReceiveUdp(1, 1, Buffer, 10000);
		if (Ret > 0) {
			StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
			if (StkPlWcsStr((wchar_t*)Buffer, L"Shinya Takeuchi") != 0 && (Msg == STKSOCKET_LOG_UDPRECV || Msg == STKSOCKET_LOG_UDPSEND)) { // There is a possibility that STKSOCKET_LOG_ACPTSEND is taken depends on the timing.
				StkPlPrintf("[Recv/Send for UDP] : Appropriate string has been received by sender...OK [%ls]\n", (wchar_t*)Buffer);
				break;
			} else {
				StkPlPrintf("[Recv/Send for UDP] : Appropriate string has been received by sender...NG [%ls:%d]\n", (wchar_t*)Buffer, Msg);
				exit(-1);
			}
		}
	}

	StkSocket_Disconnect(1, 1, true);
	StkSocket_TakeLastLog(&Msg, &LogId, ParamStr1, ParamStr2, &ParamInt1, &ParamInt2);
	if (Msg == STKSOCKET_LOG_UDPSOCKCLOSE && LogId == 1) {
		StkPlPrintf("[Recv/Send for UDP] : Sender socket close is called...OK\n");
	} else {
		StkPlPrintf("[Recv/Send for UDP] : Sender socket close is called...NG\n");
		exit(-1);
	}
	FinishFlag = true;
}

void TestThreadProc6(bool SslMode)
{
	StkPlPrintf("[Recv/Send%s] : Keep waiting for sender's finish ...", SslMode ? "(SSL/TSL)" : "");
	FinishFlag = false;
	StkSocket_AddInfo(0, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2001);
	if (SslMode) {
		StkSocket_SecureForRecv(0, "./server.key", "./server.crt");
	}
	StkSocket_Open(0);
	unsigned char Buffer[10000];
	unsigned char CondStr[1000];
	while (true) {
		int Ret = StkSocket_Receive(0, 0, Buffer, 10000, STKSOCKET_RECV_FINISHCOND_UNCONDITIONAL, 100, CondStr, 1000);
		if (Ret > 0) {
			StkPlPrintf("NG\n");
			exit(-1);
		}
		if (FinishFlag == true) {
			break;
		}
	}
	StkSocket_Close(0, false);
	StkSocket_DeleteInfo(0);
	StkPlPrintf("OK\n");
}

void TestThreadProc7(bool SslMode)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	FinishFlag = true;
}

void TestThreadProc8(bool SslMode)
{
	StkPlPrintf("[Recv/Send%s] : Receiver method out detected with timeout=0...", SslMode ? "(SSL/TSL)" : "");
	StkSocket_AddInfo(0, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2001);
	StkSocket_AddInfo(1, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2001);
	if (SslMode) {
		StkSocket_SecureForRecv(0, "./server.key", "./server.crt");
	}
	StkSocket_Open(0);
	StartFlag = true;

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
	FinishFlag = true;
	StkPlPrintf("OK\n");
	return;
}

void TestThreadProc9(bool SslMode)
{
	while (!StartFlag) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	if (SslMode) {
		StkSocket_SecureForSend(1, "./ca.crt", NULL);
	}
	wchar_t Buf[10000];

	StkPlLStrCpy(Buf, L"Hello, world!!");
	while (StkSocket_Connect(1) == -1) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		StkPlPrintf("{Re} ");
	}
	StkSocket_Send(1, 1, (const unsigned char*)Buf, (int)(StkPlWcsLen(Buf) + 1) * sizeof(wchar_t));
	std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	StkSocket_ForceStop(0);
	StkSocket_Disconnect(1, 1, true);
	while (!FinishFlag) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	StkSocket_DeleteInfo(0);
	StkSocket_DeleteInfo(1);
	return;
}

void TestThreadProc10(int Command)
{
	unsigned char Buffer[65536] = "";
	unsigned char TestStr[65536] = "";
	int RecvType = 0;
	int Size = 0;
	int CheckSize = 0;
	int Option2 = 0;

	if (Command == 0) {
		RecvType = 256;
		StkPlStrCpy((char*)TestStr, 65536, "012345678901234567890123456789");
		Size = 30;
		CheckSize = 30;
		Option2 = 0;
	}
	if (Command >= 1 && Command <= 4) {
		RecvType = Command * -1;
		StkPlStrCpy((char*)TestStr, 65536, "01234567890123456789012345678901234567890123456789");
		Size = 50;
		CheckSize = 50;
		Option2 = 0;
	}
	if (Command == 5) {
		RecvType = -3;
		StkPlStrCpy((char*)TestStr, 65536, "POST / HTTP/1.1\r\nTransfer-Encoding: chunked\r\nContent-Type: text/html\r\n\r\nTestTestTestHello!!!0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef");
		Size = 1024;
		CheckSize = 156;
		Option2 = 0;
	}
	if (Command == 6) {
		RecvType = -3;
		StkPlStrCpy((char*)TestStr, 65536, "POST / HTTP/1.1\r\nTransfer-Encoding: chunked\r\nContent-Type: text/html\r\n\r\nTestTestTest");
		Size = 1024;
		CheckSize = 84;
		Option2 = 0b00000001;
	}
	if (Command == 7) {
		RecvType = -3;
		StkPlStrCpy((char*)TestStr, 65536, "POST / HTTP/1.1\r\nTransfer-Encoding: chunked\r\nContent-Type: text/html\r\n\r\n0c\r\nTestTestTest\r\n");
		Size = 1024;
		CheckSize = 90;
		Option2 = 0b00000011;
	}
	if (Command == 8) {
		RecvType = -3;
		StkPlStrCpy((char*)TestStr, 65536, "POST / HTTP/1.1\r\nTransfer-Encoding: chunked\r\nContent-Type: text/html\r\n\r\n0c\r\nTestTestTest\r\n0008\r\nHello!!!\r\n00000020\r\n0123456789abcdef0123456789abcdef\r\n00000020\r\n0123456789abcdef0123456789abcdef\r\n00\r\n\r\n");
		Size = 1024;
		CheckSize = 200;
		Option2 = 0b00000010;
	}

	StkSocket_AddInfo(1, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2002);
	StkSocket_AddInfo(10, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2002);
	if (SslFlag) {
		StkSocket_SecureForSend(1, "./ca.crt", NULL);
		StkSocket_SecureForRecv(10, "./server.key", "./server.crt");
	}
	StkSocket_Open(10);
	
	if (Command <= 4) {
		StkPlPrintf("[Recv/Send%s] : Receiver's buffer overflow occurrence (Command=%d) ...", SslFlag ? "(SSL/TLS)" : "", RecvType);
	} else {
		StkPlPrintf("[Recv/Send%s] : Transfer-Encoding: chunked (Option2=%d) ...", SslFlag ? "(SSL/TLS)" : "", Option2);
	}
	StartFlag = true;
	while (true) {
		if (StkSocket_Accept(10) == 0) {
			memset(Buffer, '\0', 512);
			int Ret = StkSocket_Receive(10, 10, Buffer, Size, RecvType, 0, NULL, Option2);
			if (Ret != CheckSize || StkPlStrNCmp((char*)Buffer, (char*)TestStr, CheckSize) != 0) {
				int TmpLog;
				int TmpLogId;
				wchar_t TmpLogParamStr1[256];
				wchar_t TmpLogParamStr2[256];
				int TmpLogParamInt1;
				int TmpLogParamInt2;
				StkSocket_TakeLastLog(&TmpLog, &TmpLogId, TmpLogParamStr1, TmpLogParamStr2, &TmpLogParamInt1, &TmpLogParamInt2);
				StkPlPrintf("NG (return=%d, expectation=%d) [%d, %d, %ls, %ls, %d, %d]\n", Ret, Size, TmpLog, TmpLogId, TmpLogParamStr1, TmpLogParamStr2, TmpLogParamInt1, TmpLogParamInt2);
				exit(-1);
			}
			break;
		}
	}
	PeerCloseOkFlag = true;

	StkSocket_Close(10, true);
	while (StkSocket_GetStatus(10) != STKSOCKET_STATUS_CLOSE && StkSocket_GetStatus(1) != STKSOCKET_STATUS_CLOSE) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	while (!FinishFlag) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	StkSocket_DeleteInfo(1);
	StkSocket_DeleteInfo(10);
	StkPlPrintf("OK\n");
	return;
}

void TestThreadProc11(int Command)
{
	char Buf[65536];
	if (Command >= 0 && Command <= 4) {
		StkPlStrCpy(Buf, 65536, "012345678901234567890123456789012345678901234567890123456789");
	}
	if (Command >= 5) {
		StkPlStrCpy(Buf, 65536, "POST / HTTP/1.1\r\nTransfer-Encoding: chunked\r\nContent-Type: text/html\r\n\r\n0c\r\nTestTestTest\r\n0008\r\nHello!!!\r\n00000020\r\n0123456789abcdef0123456789abcdef\r\n00000020\r\n0123456789abcdef0123456789abcdef\r\n00\r\n\r\n");
	}

	while (StartFlag == false) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	while (StkSocket_Connect(1) == -1) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		StkPlPrintf("*");
	}
	StkSocket_Send(1, 1, (const unsigned char*)Buf, (int)StkPlStrLen(Buf));
	while (PeerCloseOkFlag == false) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	StkSocket_Disconnect(1, 1, true);
	FinishFlag = true;

	return;
}

void TestThreadProc12()
{
	StkPlPrintf("[Recv/Send] : SSL connection ...");
	StkSocket_AddInfo(0, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2001);
	StkSocket_AddInfo(1, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2001);
	StkSocket_SecureForRecv(0, "./server.key", "./server.crt");
	StkSocket_Open(0);
	StartFlag = true;

	unsigned char Buffer[10000];
	unsigned char CondStr[1000];

	while (true) {
		if (StkSocket_Accept(0) == 0) {
			int Ret = StkSocket_Receive(0, 0, Buffer, 10000, STKSOCKET_RECV_FINISHCOND_PEERCLOSURE, 0, CondStr, 1000);
			if (Ret > 0) {
				break;
			}
		}
	}
	StkSocket_Close(0, true);
	FinishFlag = true;
	StkPlPrintf("OK\n");
	return;
}

void TestThreadProc13()
{
	while (StartFlag == false) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	StkSocket_SecureForSend(1, "./ca.crt", NULL);
	wchar_t Buf[10000];

	StkPlLStrCpy(Buf, L"Hello, world!!");
	while (StkSocket_Connect(1) == -1) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		StkPlPrintf("{Re} ");
	}
	StkSocket_Send(1, 1, (const unsigned char*)Buf, (int)(StkPlWcsLen(Buf) + 1) * sizeof(wchar_t));
	StkSocket_Disconnect(1, 1, true);

	while (!FinishFlag) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	StkSocket_DeleteInfo(0);
	StkSocket_DeleteInfo(1);
	return;
}

//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

void TestThreadForAcceptRecv1()
{
	unsigned char Buf[1000000];
	bool S1Flag = false;
	bool S2Flag = false;
	bool S3Flag = false;
	srand(101);
	while (true) {
		if (StkSocket_Accept(101) == 0) {
			StkSocket_Receive(101, 101, Buf, 1000000, STKSOCKET_RECV_FINISHCOND_UNCONDITIONAL, 100, NULL, 0);
			if (StkPlStrStr((char*)Buf, "#1") != 0) S1Flag = true;
			if (StkPlStrStr((char*)Buf, "#2") != 0) S2Flag = true;
			if (StkPlStrStr((char*)Buf, "#3") != 0) S3Flag = true;
			StkSocket_CloseAccept(101, 101, true);
			if (S1Flag && S2Flag && S3Flag) break;
			std::this_thread::sleep_for(std::chrono::milliseconds((rand() % 10) * 100));
		}
	}
	StkPlPrintf("[Multi accepts] : Acceptor #1 is working...OK\n");
	FindFlagCounter++;
	return;
}

void TestThreadForAcceptRecv2()
{
	unsigned char Buf[1000000];
	bool S1Flag = false;
	bool S2Flag = false;
	bool S3Flag = false;
	srand(102);
	while (true) {
		if (StkSocket_Accept(102) == 0) {
			StkSocket_Receive(102, 102, Buf, 1000000, STKSOCKET_RECV_FINISHCOND_UNCONDITIONAL, 100, NULL, 0);
			if (StkPlStrStr((char*)Buf, "#1") != 0) S1Flag = true;
			if (StkPlStrStr((char*)Buf, "#2") != 0) S2Flag = true;
			if (StkPlStrStr((char*)Buf, "#3") != 0) S3Flag = true;
			StkSocket_CloseAccept(102, 102, true);
			if (S1Flag && S2Flag && S3Flag) break;
			std::this_thread::sleep_for(std::chrono::milliseconds((rand() % 10) * 100));
		}
	}
	StkPlPrintf("[Multi accepts] : Acceptor #2 is working...OK\n");
	FindFlagCounter++;
	return;
}

void TestThreadForAcceptRecv3()
{
	unsigned char Buf[1000000];
	bool S1Flag = false;
	bool S2Flag = false;
	bool S3Flag = false;
	srand(103);
	while (true) {
		if (StkSocket_Accept(103) == 0) {
			StkSocket_Receive(103, 103, Buf, 1000000, STKSOCKET_RECV_FINISHCOND_UNCONDITIONAL, 100, NULL, 0);
			if (StkPlStrStr((char*)Buf, "#1") != 0) S1Flag = true;
			if (StkPlStrStr((char*)Buf, "#2") != 0) S2Flag = true;
			if (StkPlStrStr((char*)Buf, "#3") != 0) S3Flag = true;
			StkSocket_CloseAccept(103, 103, true);
			if (S1Flag && S2Flag && S3Flag) break;
			std::this_thread::sleep_for(std::chrono::milliseconds((rand() % 10) * 100));
		}
	}
	StkPlPrintf("[Multi accepts] : Acceptor #3 is working...OK\n");
	FindFlagCounter++;
	return;
}

void TestThreadForAcceptSend1()
{
	char Buf[1024];
	bool S1Flag = false;
	bool S2Flag = false;
	bool S3Flag = false;
	StkPlStrCpy(Buf, 1024, "Hello, world from #1\r\n");
	for (int Loop = 0; Loop < 50; Loop++) {
		while (StkSocket_Connect(201) == -1 && FindFlagCounter < 3) {
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
		StkSocket_Send(201, 201, (const unsigned char*)Buf, (int)StkPlStrLen(Buf) + 1);
		StkSocket_Disconnect(201, 201, false);
		if (FindFlagCounter >= 3) {
			break;
		}
	}
	return;
}

void TestThreadForAcceptSend2()
{
	char Buf[1024];
	StkPlStrCpy(Buf, 1024, "Hello, world from #2\r\n");
	for (int Loop = 0; Loop < 50; Loop++) {
		while (StkSocket_Connect(202) == -1 && FindFlagCounter < 3) {
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
		StkSocket_Send(202, 202, (const unsigned char*)Buf, (int)StkPlStrLen(Buf) + 1);
		StkSocket_Disconnect(202, 202, false);
		if (FindFlagCounter >= 3) {
			break;
		}
	}
	return;
}

void TestThreadForAcceptSend3()
{
	char Buf[1024];
	StkPlStrCpy(Buf, 1024, "Hello, world from #3\r\n");
	for (int Loop = 0; Loop < 50; Loop++) {
		while (StkSocket_Connect(203) == -1 && FindFlagCounter < 3) {
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
		StkSocket_Send(203, 203, (const unsigned char*)Buf, (int)StkPlStrLen(Buf) + 1);
		StkSocket_Disconnect(203, 203, false);
		if (FindFlagCounter >= 3) {
			break;
		}
	}
	return;
}

void TestThreadForAcceptSendRecv()
{
	StkSocket_AddInfo(101, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2001);
	if (StkSocket_CopyInfo(102, 101) != -1) {
		StkPlPrintf("[Multi accepts] : CopySocketInfo successed ... OK\n");
	} else {
		StkPlPrintf("[Multi accepts] : Failed to CopySocketInfo execution ... NG\n");
		exit(-1);
	}
	if (StkSocket_GetNumOfStkInfos() == 2) {
		StkPlPrintf("[Multi accepts] : The number of StkInfo is two ... OK\n");
	} else {
		StkPlPrintf("[Multi accepts] : The number of StkInfo is not two ... NG\n");
		exit(-1);
	}
	StkSocket_DeleteInfo(102);
	if (StkSocket_GetNumOfStkInfos() == 1) {
		StkPlPrintf("[Multi accepts] : After deleting StkSock, the number of StkInfo is one ... OK\n");
	} else {
		StkPlPrintf("[Multi accepts] : After deleting StkSock, the number of StkInfo is not two ... NG\n");
		exit(-1);
	}

	StkSocket_Open(101);
	if (StkSocket_CopyInfo(102, 101) != 0) {
		StkPlPrintf("[Multi accepts] : Invalid return code detected ... NG\n");
		exit(-1);
	} else {
		StkPlPrintf("[Multi accepts] : Success to copy StkSocketInfo ... OK\n");
	}
	if (StkSocket_CopyInfo(103, 101) != 0) {
		StkPlPrintf("[Multi accepts] : Invalid return code detected ... NG\n");
		exit(-1);
	} else {
		StkPlPrintf("[Multi accepts] : Success to copy one more StkSocketInfo ... OK\n");
	}
	if (StkSocket_CopyInfo(104, 999) != -1) {
		StkPlPrintf("[Multi accepts] : Invalid return code detected ... NG\n");
		exit(-1);
	} else {
		StkPlPrintf("[Multi accepts] : Success to detect invalid code for unexistence base info ... OK\n");
	}
	StkSocket_AddInfo(201, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2001);
	StkSocket_AddInfo(202, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2001);
	StkSocket_AddInfo(203, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2001);
	if (StkSocket_GetNumOfStkInfos() == 6) {
		StkPlPrintf("[Multi accepts] : 3 socket information exists ... OK\n");
	} else {
		StkPlPrintf("[Multi accepts] : 3 socket information exists ... NG\n");
		exit(-1);
	}

	FindFlagCounter = 0;

	std::thread *Receiver1 = new std::thread(TestThreadForAcceptRecv1);
	std::thread *Receiver2 = new std::thread(TestThreadForAcceptRecv2);
	std::thread *Receiver3 = new std::thread(TestThreadForAcceptRecv3);
	std::thread *Sender1 = new std::thread(TestThreadForAcceptSend1);
	std::thread *Sender2 = new std::thread(TestThreadForAcceptSend2);
	std::thread *Sender3 = new std::thread(TestThreadForAcceptSend3);

	Receiver1->join();
	Receiver2->join();
	Receiver3->join();
	Sender1->join();
	Sender2->join();
	Sender3->join();

	StkPlPrintf("[Multi accepts] : Try StkSocket_CopyInfo() after source socket closure with StkSocket_Close() ... ");
	StkSocket_Close(101, true);
	if (StkSocket_CopyInfo(104, 101) != -1) {
		StkPlPrintf("OK\n");
	} else {
		StkPlPrintf("NG\n");
		exit(-1);
	}
	StkSocket_DeleteInfo(101);
	StkSocket_DeleteInfo(102);
	StkSocket_DeleteInfo(103);
	StkSocket_DeleteInfo(104);
	StkSocket_DeleteInfo(201);
	StkSocket_DeleteInfo(202);
	StkSocket_DeleteInfo(203);
	delete Receiver1;
	delete Receiver2;
	delete Receiver3;
	delete Sender1;
	delete Sender2;
	delete Sender3;

	if (StkSocket_GetNumOfStkInfos() == 0) {
		StkPlPrintf("[Multi accepts] : No socket information exists ... OK\n");
	} else {
		StkPlPrintf("[Multi accepts] : No socket information exists ... NG\n");
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
			StkPlPrintf("[Multi accepts] : Status validation [open] ... NG\n");
			exit(-1);
	}
	StkPlPrintf("[Multi accepts] : Status validation [open] ... OK\n");

	StkSocket_Close(121, false);
	if (StkSocket_GetStatus(111) != STKSOCKET_STATUS_OPEN ||
		StkSocket_GetStatus(121) != STKSOCKET_STATUS_CLOSE ||
		StkSocket_GetStatus(131) != STKSOCKET_STATUS_OPEN ||
		StkSocket_GetStatus(122) != STKSOCKET_STATUS_CLOSE ||
		StkSocket_GetStatus(123) != STKSOCKET_STATUS_CLOSE ||
		StkSocket_GetStatus(124) != STKSOCKET_STATUS_CLOSE) {
			StkPlPrintf("[Multi accepts] : Status validation [close] ... NG\n");
			exit(-1);
	}

	StkSocket_Close(111, false);
	StkSocket_Close(131, false);
	if (StkSocket_GetStatus(111) != STKSOCKET_STATUS_CLOSE ||
		StkSocket_GetStatus(131) != STKSOCKET_STATUS_CLOSE) {
			StkPlPrintf("[Multi accepts] : Status validation [close+] ... NG\n");
			exit(-1);
	}
	StkPlPrintf("[Multi accepts] : Status validation [close] ... OK\n");
	StkSocket_DeleteInfo(111);
	StkSocket_DeleteInfo(121);
	StkSocket_DeleteInfo(131);
	StkSocket_DeleteInfo(122);
	StkSocket_DeleteInfo(123);
	StkSocket_DeleteInfo(124);
}

int main(int Argc, char* Argv[])
{
	StkSocket_InitSecureSetting();

	StkPlPrintf("Test started\n");

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

	for (int Loop = 0; Loop < 2; Loop++) {
		{
			StartFlag = false;
			FinishFlag = false;
			std::thread *Receiver = new std::thread(TestThreadProc0, (bool)Loop);
			std::thread *Sender = new std::thread(TestThreadProc1, (bool)Loop);
			Receiver->join();
			Sender->join();
			delete Receiver;
			delete Sender;
		}
		{
			StartFlag = false;
			FinishFlag = false;
			std::thread *Receiver = new std::thread(TestThreadProc2, (bool)Loop);
			std::thread *Sender = new std::thread(TestThreadProc3, (bool)Loop);
			Receiver->join();
			Sender->join();
			delete Receiver;
			delete Sender;
		}
		{
			StartFlag = false;
			FinishFlag = false;
			std::thread *Receiver = new std::thread(TestThreadProc6, (bool)Loop);
			std::thread *Sender = new std::thread(TestThreadProc7, (bool)Loop);
			Receiver->join();
			Sender->join();
			delete Receiver;
			delete Sender;
		}
		{
			StartFlag = false;
			FinishFlag = false;
			std::thread *Receiver = new std::thread(TestThreadProc8, (bool)Loop);
			std::thread *Sender = new std::thread(TestThreadProc9, (bool)Loop);
			Receiver->join();
			Sender->join();
			delete Receiver;
			delete Sender;
		}
	}
	{
		StartFlag = false;
		FinishFlag = false;
		std::thread *Receiver = new std::thread(TestThreadProc4);
		std::thread *Sender = new std::thread(TestThreadProc5);
		Receiver->join();
		Sender->join();
		delete Receiver;
		delete Sender;
	}

	for (int LoopSsl = 0; LoopSsl < 2; LoopSsl++) {
		if (LoopSsl == 0) {
			SslFlag = false;
		} else {
			SslFlag = true;
		}
		for (int Loop = 0; Loop <= 8; Loop++) {
			StartFlag = false;
			FinishFlag = false;
			PeerCloseOkFlag = false;
			std::thread *Receiver = new std::thread(TestThreadProc10, Loop);
			std::thread *Sender = new std::thread(TestThreadProc11, Loop);
			Receiver->join();
			Sender->join();
			delete Receiver;
			delete Sender;
		}
	}

	{
		StartFlag = false;
		FinishFlag = false;
		std::thread *Receiver = new std::thread(TestThreadProc12);
		std::thread *Sender = new std::thread(TestThreadProc13);
		Receiver->join();
		Sender->join();
		delete Receiver;
		delete Sender;
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

	StkPlPrintf("Test completed\n");

	return 0;
}
