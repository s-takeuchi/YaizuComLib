#include <stdio.h>
#include <string.h>
#include <tchar.h>
#include "..\..\src\stksocket\stksocket.h"
#include "StkSocketIPv6.h"

int StkSocketIPv6::ThreadStartCount = 0;
int StkSocketIPv6::ThreadEndCount = 0;
int StkSocketIPv6::ErrFlag = FALSE;

DWORD WINAPI StkSocketIPv6::TestThreadForRecv(LPVOID Param)
{
	ThreadStartCount++;
	BYTE Buf[1000000];
	while (TRUE) {
		if (StkSocket_Accept(121) == 0) {
			int Len = 0;
			Len = StkSocket_Receive(121, 121, Buf, 1000000, STKSOCKET_RECV_FINISHCOND_PEERCLOSURE, 30, NULL, 0, FALSE);
			Buf[Len] = '\0';
			if (strcmp((char*)Buf, "Hello, world from #1\r\n") != 0) {
				ErrFlag = TRUE;
			}
			Len = StkSocket_Receive(121, 121, Buf, 1000000, STKSOCKET_RECV_FINISHCOND_PEERCLOSURE, 0, NULL, 0, FALSE);
			Buf[Len] = '\0';
			if (strstr((char*)Buf, "Hello, world from #1\r\nHello, world from #1\r\nHello, world from #1\r\n") == 0) {
				ErrFlag = TRUE;
			}
			StkSocket_CloseAccept(121, 121, FALSE);
			break;
		}
	}
	ThreadEndCount++;
	return 0;
}

DWORD WINAPI StkSocketIPv6::TestThreadForSend(LPVOID Param)
{
	ThreadStartCount++;
	char Buf[1024];
	strcpy_s(Buf, 1024, "Hello, world from #1\r\n");
	Sleep(500);
	StkSocket_Connect(201);
	for (int Loop = 0; Loop < 50; Loop++) {
		StkSocket_Send(201, 201, (BYTE*)Buf, strlen(Buf));
		Sleep(50);
	}
	StkSocket_Disconnect(201, 201, FALSE);

	ThreadEndCount++;
	return 0;
}

void StkSocketIPv6::TestIPv6()
{
	StkSocket_ClearLog();

	printf("[StkSocketIPv6] : Run sender and receiver ... ");
	StkSocket_AddInfo(121, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("::1"), 2202);
	StkSocket_Open(121);
	StkSocket_AddInfo(201, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, _T("::1"), 2202);
	StkSocket_Open(201);

	ThreadStartCount = 0;
	ThreadEndCount = 0;
	ErrFlag = FALSE;

	DWORD TmpId;
	CreateThread(NULL, 0, &TestThreadForRecv, NULL, 0, &TmpId);
	CreateThread(NULL, 0, &TestThreadForSend, NULL, 0, &TmpId);

	int Loop;
	for (Loop = 0; Loop < 30; Loop++) {
		if (ThreadStartCount == 2 && StkSocket_GetStatus(121) == STKSOCKET_STATUS_ACCEPT) {
			break;
		}
		Sleep(100);
	}
	if (Loop == 30) {
		printf("NG\r\n");
		exit(-1);
	} else {
		printf("OK\r\n");
	}

	printf("[StkSocketIPv6] : Wait for stopping sender and receiver ... ");
	for (Loop = 0; Loop < 30; Loop++) {
		if (ThreadEndCount == 2 && StkSocket_GetStatus(121) == STKSOCKET_STATUS_OPEN) {
			break;
		}
		Sleep(250);
	}
	if (Loop == 30) {
		printf("NG\r\n");
		exit(-1);
	}
	printf("OK\r\n");

	StkSocket_DeleteInfo(121);
	StkSocket_DeleteInfo(201);

	printf("[StkSocketIPv6] : Check that appropriate data is reecived ... ");
	if (ErrFlag) {
		printf("NG\r\n");
		exit(-1);
	} else {
		printf("OK\r\n");
	}

	StkSocket_ClearLog();
}

DWORD WINAPI StkSocketIPv6::TestThreadForRecvUdp(LPVOID Param)
{
	BYTE Data[10000];
	while (TRUE) {
		int Len1 = StkSocket_ReceiveUdp(121, 121, Data, 10000);
		int Len2 = StkSocket_SendUdp(121, 121, Data, 10000);
		if (Len1 == 10000 && Len2 == 10000) {
			ThreadEndCount++;
			break;
		}
	}
	return 0;
}

DWORD WINAPI StkSocketIPv6::TestThreadForSendUdp(LPVOID Param)
{
	BYTE Data[10000];
	BYTE DataRes[10000];
	for (int Loop = 0; Loop < 10000; Loop++) {
		Data[Loop] = 0xcc;
	}
	int Len1 = StkSocket_SendUdp(201, 201, Data, 10000);
	Sleep(100);
	int Len2 = StkSocket_ReceiveUdp(201, 201, DataRes, 10000);
	if (Len1 == 10000 && Len2 == 10000 && DataRes[0] == 0xcc && DataRes[100] == 0xcc && DataRes[1000] == 0xcc) {
		ThreadEndCount++;
	}
	return 0;
}

void StkSocketIPv6::TestIPv6Udp()
{
	StkSocket_ClearLog();

	printf("[StkSocketIPv6] : Send and receive data using UDP ... ");
	StkSocket_AddInfo(121, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("::1"), 2202);
	StkSocket_Open(121);
	StkSocket_AddInfo(201, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_SENDER, _T("::1"), 2202);
	StkSocket_Connect(201);

	if (StkSocket_GetNumOfStkInfos() != 2 || StkSocket_GetStatus(121) != STKSOCKET_STATUS_OPEN || StkSocket_GetStatus(201) != STKSOCKET_STATUS_OPEN) {
		printf("NG\r\n");
		exit(-1);
	}

	ThreadStartCount = 0;
	ThreadEndCount = 0;
	DWORD TmpId;
	CreateThread(NULL, 0, &TestThreadForRecvUdp, NULL, 0, &TmpId);
	Sleep(500);
	CreateThread(NULL, 0, &TestThreadForSendUdp, NULL, 0, &TmpId);

	int Loop;
	for (Loop = 0; Loop < 30; Loop++) {
		if (ThreadEndCount == 2) {
			break;
		}
		Sleep(100);
	}
	if (Loop == 30) {
		printf("NG\r\n");
		exit(-1);
	}
	printf("OK\r\n");

	StkSocket_Disconnect(201, 201, FALSE);
	StkSocket_Close(121, FALSE);

	StkSocket_DeleteInfo(121);
	StkSocket_DeleteInfo(201);

	StkSocket_ClearLog();
}
