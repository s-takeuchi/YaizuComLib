#include <windows.h>
#include <stdio.h>
#include <Psapi.h>
#include "..\..\src\stksocket\stksocket.h"
#include "StkSocketMemoryLeak.h"

int GetUsedMemorySizeOfCurrentProcess()
{
	DWORD dwProcessID = GetCurrentProcessId();
	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc = { 0 };

	long Size;
	if ((hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, false, dwProcessID)) != NULL) {
		if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
			Size = pmc.WorkingSetSize;
		}
	}
	CloseHandle(hProcess);
	return Size;
}

DWORD WINAPI StkSocketMemoryLeak::TestThreadForRecv(LPVOID Param)
{
	unsigned char Buf[1000000];
	while (true) {
		if (StkSocket_Accept(121) == 0) {
			int Len = StkSocket_Receive(121, 121, Buf, 1000000, STKSOCKET_RECV_FINISHCOND_PEERCLOSURE, 0, NULL, 0);
			StkSocket_CloseAccept(121, 121, false);
			break;
		}
	}
	return 0;
}

DWORD WINAPI StkSocketMemoryLeak::TestThreadForSend(LPVOID Param)
{
	char Buf[1000000];
	StkSocket_Connect(201);
	StkSocket_Send(201, 201, (unsigned char*)Buf, 1000000);
	StkSocket_Disconnect(201, 201, false);
	return 0;
}

void StkSocketMemoryLeak::CheckLeakTcp()
{
	long MaxMem[60];
	StkSocket_ClearLog();

	printf("[StkSocketMemoryLeak] : Check memory leak for TCP ... ");

	for (int Loop = 0; Loop < 60; Loop++) {
		StkSocket_AddInfo(121, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"::1", 2202);
		StkSocket_Open(121);
		StkSocket_AddInfo(201, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"::1", 2202);
		StkSocket_Open(201);

		DWORD TmpId;
		CreateThread(NULL, 0, &TestThreadForRecv, NULL, 0, &TmpId);
		Sleep(200);
		CreateThread(NULL, 0, &TestThreadForSend, NULL, 0, &TmpId);
		Sleep(200);

		StkSocket_DeleteInfo(121);
		StkSocket_DeleteInfo(201);

		MaxMem[Loop] = GetUsedMemorySizeOfCurrentProcess();
	}
	if (MaxMem[30] < MaxMem[33] &&
		MaxMem[33] < MaxMem[36] &&
		MaxMem[36] < MaxMem[39] &&
		MaxMem[39] < MaxMem[42] &&
		MaxMem[42] < MaxMem[45] &&
		MaxMem[45] < MaxMem[48] &&
		MaxMem[48] < MaxMem[51] &&
		MaxMem[51] < MaxMem[54] &&
		MaxMem[54] < MaxMem[57]) {
		printf("[NG] : %d %d %d %d %d %d %d %d %d\r\n", MaxMem[30], MaxMem[33], MaxMem[36], MaxMem[39], MaxMem[42], MaxMem[45], MaxMem[48], MaxMem[51], MaxMem[54]);
		exit(-1);
	}
	printf("[OK] : %d %d %d %d %d %d %d %d %d\r\n", MaxMem[30], MaxMem[33], MaxMem[36], MaxMem[39], MaxMem[42], MaxMem[45], MaxMem[48], MaxMem[51], MaxMem[54]);
	StkSocket_ClearLog();
}

DWORD WINAPI StkSocketMemoryLeak::TestThreadForRecvUdp(LPVOID Param)
{
	unsigned char Data[10000];
	while (true) {
		int Len1 = StkSocket_ReceiveUdp(121, 121, Data, 10000);
		int Len2 = StkSocket_SendUdp(121, 121, Data, 10000);
		if (Len1 == 10000 && Len2 == 10000) {
			//printf("*");
		}
	}
	return 0;
}

DWORD WINAPI StkSocketMemoryLeak::TestThreadForSendUdp(LPVOID Param)
{
	unsigned char Data[10000];
	unsigned char DataRes[10000];
	while (true) {
		for (int Loop = 0; Loop < 10000; Loop++) {
			Data[Loop] = 0xcc;
		}
		int Len1 = StkSocket_SendUdp(201, 201, Data, 10000);
		Sleep(10);
		int Len2 = StkSocket_ReceiveUdp(201, 201, DataRes, 10000);
		if (Len1 == 10000 && Len2 == 10000 && DataRes[0] == 0xcc && DataRes[100] == 0xcc && DataRes[1000] == 0xcc) {
			//printf("+");
		}
	}
	return 0;
}

void StkSocketMemoryLeak::CheckLeakUdp()
{
	StkSocket_ClearLog();

	printf("[StkSocketMemoryLeak] : Check memory leak for UDP ... ");
	StkSocket_AddInfo(121, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"::1", 2202);
	StkSocket_Open(121);
	StkSocket_AddInfo(201, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_SENDER, L"::1", 2202);
	StkSocket_Connect(201);

	if (StkSocket_GetNumOfStkInfos() != 2 || StkSocket_GetStatus(121) != STKSOCKET_STATUS_OPEN || StkSocket_GetStatus(201) != STKSOCKET_STATUS_OPEN) {
		printf("NG\r\n");
		exit(-1);
	}

	DWORD TmpId;
	CreateThread(NULL, 0, &TestThreadForRecvUdp, NULL, 0, &TmpId);
	Sleep(500);
	CreateThread(NULL, 0, &TestThreadForSendUdp, NULL, 0, &TmpId);

	long MaxMem[30];
	Sleep(3000);
	for (int Loop = 0; Loop < 30; Loop++) {
		MaxMem[Loop] = GetUsedMemorySizeOfCurrentProcess();
		Sleep(800);
	}

	StkSocket_Disconnect(201, 201, false);
	StkSocket_Close(121, false);

	StkSocket_DeleteInfo(121);
	StkSocket_DeleteInfo(201);

	if (MaxMem[0] < MaxMem[3] &&
		MaxMem[3] < MaxMem[6] &&
		MaxMem[6] < MaxMem[9] &&
		MaxMem[9] < MaxMem[12] &&
		MaxMem[12] < MaxMem[15] &&
		MaxMem[15] < MaxMem[18] &&
		MaxMem[18] < MaxMem[21] &&
		MaxMem[21] < MaxMem[24] &&
		MaxMem[24] < MaxMem[27]) {
		printf("[NG] : %d %d %d %d %d %d %d %d %d\r\n", MaxMem[0], MaxMem[3], MaxMem[6], MaxMem[9], MaxMem[12], MaxMem[15], MaxMem[18], MaxMem[21], MaxMem[24]);
		exit(-1);
	}
	printf("[OK] : %d %d %d %d %d %d %d %d %d\r\n", MaxMem[0], MaxMem[3], MaxMem[6], MaxMem[9], MaxMem[12], MaxMem[15], MaxMem[18], MaxMem[21], MaxMem[24]);

	StkSocket_ClearLog();
}
