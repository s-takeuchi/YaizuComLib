#include <stdio.h>
#include <tchar.h>
#include "..\..\src\stksocket\stksocket.h"
#include "StkSocketTestGetSockInfo.h"

int StkSocketTestGetSockInfo::TestAddDel()
{
	{
		printf("[GetSockInfo] : Current number of existing sock info is zero ... ");
		if (StkSocket_GetNumOfStkInfos() != 0) {
			printf("NG\r\n");
			return -1;
		}
		printf("OK\r\n");
	}
	/////////////////////////
	{
		printf("[GetSockInfo] : No socket information is acquired ... ");
		int TargetId;
		int SocketType;
		int ActionType;
		TCHAR TargetIpAddr[256];
		int TargetPort;
		BOOL CopiedFlag;
		int Ret = StkSocket_GetInfo(0, &TargetId, &SocketType, &ActionType, TargetIpAddr, &TargetPort, &CopiedFlag);
		if (Ret != -1) {
			printf("NG\r\n");
			return -1;
		}
		printf("OK\r\n");
	}
	/////////////////////////
	{
		printf("[GetSockInfo] : Added elements can be acquired ... ");
		StkSocket_AddInfo(21, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("127.0.0.1"), 2221);
		StkSocket_AddInfo(22, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("127.0.0.1"), 2222);
		StkSocket_AddInfo(23, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("127.0.0.1"), 2223);
		StkSocket_AddInfo(24, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("127.0.0.1"), 2224);
		StkSocket_AddInfo(25, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("127.0.0.1"), 2225);
		if (StkSocket_GetNumOfStkInfos() != 5) {
			printf("NG[0]\r\n");
			return -1;
		}
		int TargetId;
		int SocketType;
		int ActionType;
		TCHAR TargetIpAddr[256];
		int TargetPort;
		BOOL CopiedFlag;
		int Ret;
		Ret = StkSocket_GetInfo(0, &TargetId, &SocketType, &ActionType, TargetIpAddr, &TargetPort, &CopiedFlag);
		if (Ret == -1 || TargetId < 21 || TargetId > 25) {
			printf("NG[1]\r\n");
			return -1;
		}
		Ret = StkSocket_GetInfo(2, &TargetId, &SocketType, &ActionType, TargetIpAddr, &TargetPort, &CopiedFlag);
		if (Ret == -1 || TargetId < 21 || TargetId > 25) {
			printf("NG[2]\r\n");
			return -1;
		}
		Ret = StkSocket_GetInfo(4, &TargetId, &SocketType, &ActionType, TargetIpAddr, &TargetPort, &CopiedFlag);
		if (Ret == -1 || TargetId < 21 || TargetId > 25) {
			printf("NG[3]\r\n");
			return -1;
		}
		if (StkSocket_GetInfo(-1, &TargetId, &SocketType, &ActionType, TargetIpAddr, &TargetPort, &CopiedFlag) != -1) {
			printf("NG[4]\r\n");
			return -1;
		}
		if (StkSocket_GetInfo(5, &TargetId, &SocketType, &ActionType, TargetIpAddr, &TargetPort, &CopiedFlag) != -1) {
			printf("NG[5]\r\n");
			return -1;
		}
		printf("OK\r\n");
	}
	/////////////////////////
	{
		printf("[GetSockInfo] : Deleted elements cannot be acquired ... ");
		StkSocket_DeleteInfo(23);
		StkSocket_DeleteInfo(21);
		StkSocket_DeleteInfo(25);
		if (StkSocket_GetNumOfStkInfos() != 2) {
			printf("NG[0]\r\n");
			return -1;
		}
		int TargetId;
		int SocketType;
		int ActionType;
		TCHAR TargetIpAddr[256];
		int TargetPort;
		BOOL CopiedFlag;
		int Ret;
		Ret = StkSocket_GetInfo(0, &TargetId, &SocketType, &ActionType, TargetIpAddr, &TargetPort, &CopiedFlag);
		if (Ret == -1 || (TargetId != 22 && TargetId != 24)) {
			printf("NG[1]\r\n");
			return -1;
		}
		Ret = StkSocket_GetInfo(1, &TargetId, &SocketType, &ActionType, TargetIpAddr, &TargetPort, &CopiedFlag);
		if (Ret == -1 || (TargetId != 22 && TargetId != 24)) {
			printf("NG[2]\r\n");
			return -1;
		}
		if (StkSocket_GetInfo(-1, &TargetId, &SocketType, &ActionType, TargetIpAddr, &TargetPort, &CopiedFlag) != -1) {
			printf("NG[3]\r\n");
			return -1;
		}
		if (StkSocket_GetInfo(2, &TargetId, &SocketType, &ActionType, TargetIpAddr, &TargetPort, &CopiedFlag) != -1) {
			printf("NG[4]\r\n");
			return -1;
		}
		printf("OK\r\n");
	}
	/////////////////////////
	{
		printf("[GetSockInfo] : All elements are deleted. No elements are aquired ... ");
		StkSocket_DeleteInfo(22);
		StkSocket_DeleteInfo(24);
		if (StkSocket_GetNumOfStkInfos() != 0) {
			printf("NG[0]\r\n");
			return -1;
		}
		int TargetId;
		int SocketType;
		int ActionType;
		TCHAR TargetIpAddr[256];
		int TargetPort;
		BOOL CopiedFlag;
		int Ret;
		Ret = StkSocket_GetInfo(0, &TargetId, &SocketType, &ActionType, TargetIpAddr, &TargetPort, &CopiedFlag);
		if (Ret != -1) {
			printf("NG[1]\r\n");
			return -1;
		}
		printf("OK\r\n");
	}
	return 0;
}
