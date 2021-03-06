﻿#include "../../src/stkpl/StkPl.h"
#include "../../src/stksocket/stksocket.h"
#include "StkSocketTestGetSockInfo.h"

int StkSocketTestGetSockInfo::TestAddDel()
{
	{
		StkPlPrintf("[GetSockInfo] : Current number of existing sock info is zero ... ");
		if (StkSocket_GetNumOfStkInfos() != 0) {
			StkPlPrintf("NG\n");
			return -1;
		}
		StkPlPrintf("OK\n");
	}
	/////////////////////////
	{
		StkPlPrintf("[GetSockInfo] : No socket information is acquired ... ");
		int TargetId;
		int SocketType;
		int ActionType;
		wchar_t TargetIpAddr[256];
		int TargetPort;
		bool CopiedFlag;
		int Ret = StkSocket_GetInfo(0, &TargetId, &SocketType, &ActionType, TargetIpAddr, &TargetPort, &CopiedFlag);
		if (Ret != -1) {
			StkPlPrintf("NG\n");
			return -1;
		}
		StkPlPrintf("OK\n");
	}
	/////////////////////////
	{
		StkPlPrintf("[GetSockInfo] : Added elements can be acquired ... ");
		StkSocket_AddInfo(21, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2221);
		StkSocket_AddInfo(22, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2222);
		StkSocket_AddInfo(23, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2223);
		StkSocket_AddInfo(24, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2224);
		StkSocket_AddInfo(25, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2225);
		if (StkSocket_GetNumOfStkInfos() != 5) {
			StkPlPrintf("NG[0]\r\n");
			return -1;
		}
		int TargetId;
		int SocketType;
		int ActionType;
		wchar_t TargetIpAddr[256];
		int TargetPort;
		bool CopiedFlag;
		int Ret;

		Ret = StkSocket_GetInfo(21, &SocketType, &ActionType, TargetIpAddr, &TargetPort, &CopiedFlag);
		if (Ret != 0) {
			StkPlPrintf("NG[1]\r\n");
			return -1;
		}
		Ret = StkSocket_GetInfo(25, &SocketType, &ActionType, TargetIpAddr, &TargetPort, &CopiedFlag);
		if (Ret != 0) {
			StkPlPrintf("NG[1]\r\n");
			return -1;
		}
		Ret = StkSocket_GetInfo(20, &SocketType, &ActionType, TargetIpAddr, &TargetPort, &CopiedFlag);
		if (Ret != -1) {
			StkPlPrintf("NG[1]\r\n");
			return -1;
		}
		Ret = StkSocket_GetInfo(26, &SocketType, &ActionType, TargetIpAddr, &TargetPort, &CopiedFlag);
		if (Ret != -1) {
			StkPlPrintf("NG[1]\r\n");
			return -1;
		}

		Ret = StkSocket_GetInfo(0, &TargetId, &SocketType, &ActionType, TargetIpAddr, &TargetPort, &CopiedFlag);
		if (Ret == -1 || TargetId < 21 || TargetId > 25) {
			StkPlPrintf("NG[1]\r\n");
			return -1;
		}
		Ret = StkSocket_GetInfo(2, &TargetId, &SocketType, &ActionType, TargetIpAddr, &TargetPort, &CopiedFlag);
		if (Ret == -1 || TargetId < 21 || TargetId > 25) {
			StkPlPrintf("NG[2]\r\n");
			return -1;
		}
		Ret = StkSocket_GetInfo(4, &TargetId, &SocketType, &ActionType, TargetIpAddr, &TargetPort, &CopiedFlag);
		if (Ret == -1 || TargetId < 21 || TargetId > 25) {
			StkPlPrintf("NG[3]\r\n");
			return -1;
		}
		if (StkSocket_GetInfo(-1, &TargetId, &SocketType, &ActionType, TargetIpAddr, &TargetPort, &CopiedFlag) != -1) {
			StkPlPrintf("NG[4]\r\n");
			return -1;
		}
		if (StkSocket_GetInfo(5, &TargetId, &SocketType, &ActionType, TargetIpAddr, &TargetPort, &CopiedFlag) != -1) {
			StkPlPrintf("NG[5]\r\n");
			return -1;
		}
		StkPlPrintf("OK\n");
	}
	/////////////////////////
	{
		StkPlPrintf("[GetSockInfo] : Deleted elements cannot be acquired ... ");
		StkSocket_DeleteInfo(23);
		StkSocket_DeleteInfo(21);
		StkSocket_DeleteInfo(25);
		if (StkSocket_GetNumOfStkInfos() != 2) {
			StkPlPrintf("NG[0]\r\n");
			return -1;
		}
		int TargetId;
		int SocketType;
		int ActionType;
		wchar_t TargetIpAddr[256];
		int TargetPort;
		bool CopiedFlag;
		int Ret;
		Ret = StkSocket_GetInfo(0, &TargetId, &SocketType, &ActionType, TargetIpAddr, &TargetPort, &CopiedFlag);
		if (Ret == -1 || (TargetId != 22 && TargetId != 24)) {
			StkPlPrintf("NG[1]\r\n");
			return -1;
		}
		Ret = StkSocket_GetInfo(1, &TargetId, &SocketType, &ActionType, TargetIpAddr, &TargetPort, &CopiedFlag);
		if (Ret == -1 || (TargetId != 22 && TargetId != 24)) {
			StkPlPrintf("NG[2]\r\n");
			return -1;
		}
		if (StkSocket_GetInfo(-1, &TargetId, &SocketType, &ActionType, TargetIpAddr, &TargetPort, &CopiedFlag) != -1) {
			StkPlPrintf("NG[3]\r\n");
			return -1;
		}
		if (StkSocket_GetInfo(2, &TargetId, &SocketType, &ActionType, TargetIpAddr, &TargetPort, &CopiedFlag) != -1) {
			StkPlPrintf("NG[4]\r\n");
			return -1;
		}
		StkPlPrintf("OK\n");
	}
	/////////////////////////
	{
		StkPlPrintf("[GetSockInfo] : All elements are deleted. No elements are aquired ... ");
		StkSocket_DeleteInfo(22);
		StkSocket_DeleteInfo(24);
		if (StkSocket_GetNumOfStkInfos() != 0) {
			StkPlPrintf("NG[0]\r\n");
			return -1;
		}
		int TargetId;
		int SocketType;
		int ActionType;
		wchar_t TargetIpAddr[256];
		int TargetPort;
		bool CopiedFlag;
		int Ret;
		Ret = StkSocket_GetInfo(0, &TargetId, &SocketType, &ActionType, TargetIpAddr, &TargetPort, &CopiedFlag);
		if (Ret != -1) {
			StkPlPrintf("NG[1]\r\n");
			return -1;
		}
		StkPlPrintf("OK\n");
	}
	return 0;
}
