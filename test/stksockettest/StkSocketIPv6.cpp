#include <stdio.h>
#include <tchar.h>
#include "..\..\src\stksocket\stksocket.h"
#include "StkSocketIPv6.h"

int StkSocketIPv6::ThreadStartCount = 0;
int StkSocketIPv6::ThreadEndCount = 0;

DWORD WINAPI StkSocketIPv6::TestThreadForRecv(LPVOID Param)
{
	ThreadStartCount++;
	BYTE Buf[1000000];
	while (TRUE) {
		if (StkSocket_Accept(121) == 0) {
			int Len = StkSocket_Receive(121, 121, Buf, 1000000, 9999999, NULL, 0, FALSE);
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
		StkSocket_Send(201, 201, (BYTE*)Buf, strlen(Buf) + 1);
		Sleep(50);
	}
	StkSocket_Disconnect(201, 201, FALSE);

	ThreadEndCount++;
	return 0;
}

void StkSocketIPv6::TestIPv6()
{
	printf("[StkSocketIPv6::TestIPv6] : Run sender and receiver ... ");
	StkSocket_AddInfo(121, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("::1"), 2202);
	StkSocket_Open(121);
	StkSocket_AddInfo(201, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, _T("::1"), 2202);
	StkSocket_Open(201);

	ThreadStartCount = 0;
	ThreadEndCount = 0;
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

	printf("[StkSocketIPv6::TestIPv6] : Wait for stopping sender and receiver ... ");
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
}
