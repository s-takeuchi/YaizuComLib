#include <thread>
#include "../../src/StkPl.h"
#include "../../src/stksocket/stksocket.h"
#include "StkSocketMemoryLeak.h"

void StkSocketMemoryLeak::TestThreadForRecv()
{
	unsigned char Buf[1000000];
	while (true) {
		if (StkSocket_Accept(121) == 0) {
			int Len = StkSocket_Receive(121, 121, Buf, 1000000, STKSOCKET_RECV_FINISHCOND_PEERCLOSURE, 0, NULL, 0);
			StkSocket_CloseAccept(121, 121, false);
			break;
		}
	}
}

void StkSocketMemoryLeak::TestThreadForSend()
{
	char Buf[1000000];
	StkSocket_Connect(201);
	StkSocket_Send(201, 201, (unsigned char*)Buf, 1000000);
	StkSocket_Disconnect(201, 201, false);
}

void StkSocketMemoryLeak::CheckLeakTcp()
{
	long MaxMem[60];
	StkSocket_ClearLog();

	StkPlPrintf("[StkSocketMemoryLeak] : Check memory leak for TCP ... ");

	for (int Loop = 0; Loop < 60; Loop++) {
		StkSocket_AddInfo(121, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"::1", 2202);
		StkSocket_Open(121);
		StkSocket_AddInfo(201, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"::1", 2202);
		StkSocket_Open(201);

		std::thread *Receiver = new std::thread(TestThreadForRecv);
		StkPlSleepMs(200);
		std::thread *Sender = new std::thread(TestThreadForSend);
		StkPlSleepMs(200);

		StkSocket_DeleteInfo(121);
		StkSocket_DeleteInfo(201);

		Receiver->join();
		Sender->join();
		delete Receiver;
		delete Sender;

		MaxMem[Loop] = StkPlGetUsedMemorySizeOfCurrentProcess();
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
		StkPlPrintf("[NG] : %d %d %d %d %d %d %d %d %d\r\n", MaxMem[30], MaxMem[33], MaxMem[36], MaxMem[39], MaxMem[42], MaxMem[45], MaxMem[48], MaxMem[51], MaxMem[54]);
		exit(-1);
	}
	StkPlPrintf("[OK] : %d %d %d %d %d %d %d %d %d\r\n", MaxMem[30], MaxMem[33], MaxMem[36], MaxMem[39], MaxMem[42], MaxMem[45], MaxMem[48], MaxMem[51], MaxMem[54]);
	StkSocket_ClearLog();
}

void StkSocketMemoryLeak::TestThreadForRecvUdp()
{
	unsigned char Data[10000];
	while (true) {
		int Len1 = StkSocket_ReceiveUdp(121, 121, Data, 10000);
		int Len2 = StkSocket_SendUdp(121, 121, Data, 10000);
		if (Len1 == 10000 && Len2 == 10000) {
			//StkPlPrintf("*");
		}
	}
}

void StkSocketMemoryLeak::TestThreadForSendUdp()
{
	unsigned char Data[10000];
	unsigned char DataRes[10000];
	while (true) {
		for (int Loop = 0; Loop < 10000; Loop++) {
			Data[Loop] = 0xcc;
		}
		int Len1 = StkSocket_SendUdp(201, 201, Data, 10000);
		StkPlSleepMs(10);
		int Len2 = StkSocket_ReceiveUdp(201, 201, DataRes, 10000);
		if (Len1 == 10000 && Len2 == 10000 && DataRes[0] == 0xcc && DataRes[100] == 0xcc && DataRes[1000] == 0xcc) {
			//StkPlPrintf("+");
		}
	}
}

void StkSocketMemoryLeak::CheckLeakUdp()
{
	StkSocket_ClearLog();

	StkPlPrintf("[StkSocketMemoryLeak] : Check memory leak for UDP ... ");
	StkSocket_AddInfo(121, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"::1", 2202);
	StkSocket_Open(121);
	StkSocket_AddInfo(201, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_SENDER, L"::1", 2202);
	StkSocket_Connect(201);

	if (StkSocket_GetNumOfStkInfos() != 2 || StkSocket_GetStatus(121) != STKSOCKET_STATUS_OPEN || StkSocket_GetStatus(201) != STKSOCKET_STATUS_OPEN) {
		StkPlPrintf("NG\r\n");
		exit(-1);
	}

	std::thread *Receiver = new std::thread(TestThreadForRecvUdp);
	StkPlSleepMs(500);
	std::thread *Sender = new std::thread(TestThreadForSendUdp);

	long MaxMem[30];
	StkPlSleepMs(3000);
	for (int Loop = 0; Loop < 30; Loop++) {
		MaxMem[Loop] = StkPlGetUsedMemorySizeOfCurrentProcess();
		StkPlSleepMs(800);
	}

	StkSocket_Disconnect(201, 201, false);
	StkSocket_Close(121, false);

	StkSocket_DeleteInfo(121);
	StkSocket_DeleteInfo(201);

	Receiver->detach();
	Sender->detach();

	if (MaxMem[0] < MaxMem[3] &&
		MaxMem[3] < MaxMem[6] &&
		MaxMem[6] < MaxMem[9] &&
		MaxMem[9] < MaxMem[12] &&
		MaxMem[12] < MaxMem[15] &&
		MaxMem[15] < MaxMem[18] &&
		MaxMem[18] < MaxMem[21] &&
		MaxMem[21] < MaxMem[24] &&
		MaxMem[24] < MaxMem[27]) {
		StkPlPrintf("[NG] : %d %d %d %d %d %d %d %d %d\r\n", MaxMem[0], MaxMem[3], MaxMem[6], MaxMem[9], MaxMem[12], MaxMem[15], MaxMem[18], MaxMem[21], MaxMem[24]);
		exit(-1);
	}
	StkPlPrintf("[OK] : %d %d %d %d %d %d %d %d %d\r\n", MaxMem[0], MaxMem[3], MaxMem[6], MaxMem[9], MaxMem[12], MaxMem[15], MaxMem[18], MaxMem[21], MaxMem[24]);

	StkSocket_ClearLog();
}
