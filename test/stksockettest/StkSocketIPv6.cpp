#include <thread>
#include "../../src/stkpl/StkPl.h"
#include "../../src/stksocket/stksocket.h"
#include "StkSocketIPv6.h"

int StkSocketIPv6::ThreadStartCount = 0;
int StkSocketIPv6::ThreadEndCount = 0;
int StkSocketIPv6::ErrFlag = false;

void StkSocketIPv6::TestThreadForRecv()
{
	ThreadStartCount++;
	unsigned char Buf[1000000];
	while (true) {
		if (StkSocket_Accept(121) == 0) {
			int Len = 0;

			Len = StkSocket_Receive(121, 121, Buf, 1000000, STKSOCKET_RECV_FINISHCOND_PEERCLOSURE, 30, NULL, 0);
			Buf[Len] = '\0';
			if (StkPlStrCmp((char*)Buf, "Hello, world from #1\r\n") != 0) {
				ErrFlag = true;
			}

			Len = StkSocket_Receive(121, 121, Buf, 1000000, STKSOCKET_RECV_FINISHCOND_STRING, 0, (unsigned char*)"world", 5);
			Buf[Len] = '\0';
			if (StkPlStrCmp((char*)Buf, "Hello, world") != 0) {
				ErrFlag = true;
			}

			Len = StkSocket_Receive(121, 121, Buf, 1000000, 10, 0, NULL, -1);
			Buf[Len] = '\0';
			if (StkPlStrCmp((char*)Buf, " from #1\r\n") != 0) {
				ErrFlag = true;
			}

			Len = StkSocket_Receive(121, 121, Buf, 12, 30, 0, NULL, -1);
			Buf[Len] = '\0';
			if (StkPlStrCmp((char*)Buf, "Hello, world") != 0) {
				ErrFlag = true;
			}

			Len = StkSocket_Receive(121, 121, Buf, 1000000, STKSOCKET_RECV_FINISHCOND_PEERCLOSURE, 0, NULL, 0);
			Buf[Len] = '\0';
			if (StkPlStrStr((char*)Buf, "Hello, world from #1\r\nHello, world from #1\r\nHello, world from #1\r\n") == 0) {
				ErrFlag = true;
			}

			StkSocket_CloseAccept(121, 121, false);
			break;
		}
	}
	ThreadEndCount++;
}

void StkSocketIPv6::TestThreadForSend()
{
	ThreadStartCount++;
	char Buf[1024];
	StkPlStrCpy(Buf, 1024, "Hello, world from #1\r\n");
	StkPlSleepMs(500);
	StkSocket_Connect(201);
	for (int Loop = 0; Loop < 50; Loop++) {
		StkSocket_Send(201, 201, (unsigned char*)Buf, StkPlStrLen(Buf));
		StkPlSleepMs(50);
	}
	StkSocket_Disconnect(201, 201, false);

	ThreadEndCount++;
}

void StkSocketIPv6::TestIPv6()
{
	StkSocket_ClearLog();

	StkPlPrintf("[StkSocketIPv6] : Run sender and receiver ... ");
	StkSocket_AddInfo(121, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"::1", 2202);
	StkSocket_Open(121);
	StkSocket_AddInfo(201, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"::1", 2202);
	StkSocket_Open(201);

	ThreadStartCount = 0;
	ThreadEndCount = 0;
	ErrFlag = false;

	std::thread *Receiver = new std::thread(TestThreadForRecv);
	std::thread *Sender = new std::thread(TestThreadForSend);

	int Loop;
	for (Loop = 0; Loop < 30; Loop++) {
		if (ThreadStartCount == 2 && StkSocket_GetStatus(121) == STKSOCKET_STATUS_ACCEPT) {
			break;
		}
		StkPlSleepMs(100);
	}
	if (Loop == 30) {
		StkPlPrintf("NG\r\n");
		exit(-1);
	} else {
		StkPlPrintf("OK\r\n");
	}

	StkPlPrintf("[StkSocketIPv6] : Wait for stopping sender and receiver ... ");
	for (Loop = 0; Loop < 30; Loop++) {
		if (ThreadEndCount == 2 && StkSocket_GetStatus(121) == STKSOCKET_STATUS_OPEN) {
			break;
		}
		StkPlSleepMs(250);
	}
	if (Loop == 30) {
		StkPlPrintf("NG\r\n");
		exit(-1);
	}
	StkPlPrintf("OK\r\n");

	StkSocket_DeleteInfo(121);
	StkSocket_DeleteInfo(201);

	StkPlPrintf("[StkSocketIPv6] : Check that appropriate data is reecived ... ");
	if (ErrFlag) {
		StkPlPrintf("NG\r\n");
		exit(-1);
	} else {
		StkPlPrintf("OK\r\n");
	}

	Receiver->join();
	Sender->join();
	delete Receiver;
	delete Sender;

	StkSocket_ClearLog();
}

void StkSocketIPv6::TestThreadForRecvUdp()
{
	unsigned char Data[10000];
	while (true) {
		int Len1 = StkSocket_ReceiveUdp(121, 121, Data, 10000);
		int Len2 = StkSocket_SendUdp(121, 121, Data, 10000);
		if (Len1 == 10000 && Len2 == 10000) {
			ThreadEndCount++;
			break;
		}
	}
}

void StkSocketIPv6::TestThreadForSendUdp()
{
	unsigned char Data[10000];
	unsigned char DataRes[10000];
	for (int Loop = 0; Loop < 10000; Loop++) {
		Data[Loop] = 0xcc;
	}
	int Len1 = StkSocket_SendUdp(201, 201, Data, 10000);
	StkPlSleepMs(100);
	int Len2 = StkSocket_ReceiveUdp(201, 201, DataRes, 10000);
	if (Len1 == 10000 && Len2 == 10000 && DataRes[0] == 0xcc && DataRes[100] == 0xcc && DataRes[1000] == 0xcc) {
		ThreadEndCount++;
	}
}

void StkSocketIPv6::TestIPv6Udp()
{
	StkSocket_ClearLog();

	StkPlPrintf("[StkSocketIPv6] : Send and receive data using UDP ... ");
	StkSocket_AddInfo(121, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"::1", 2202);
	StkSocket_Open(121);
	StkSocket_AddInfo(201, STKSOCKET_TYPE_DGRAM, STKSOCKET_ACTIONTYPE_SENDER, L"::1", 2202);
	StkSocket_Connect(201);

	if (StkSocket_GetNumOfStkInfos() != 2 || StkSocket_GetStatus(121) != STKSOCKET_STATUS_OPEN || StkSocket_GetStatus(201) != STKSOCKET_STATUS_OPEN) {
		StkPlPrintf("NG\r\n");
		exit(-1);
	}

	ThreadStartCount = 0;
	ThreadEndCount = 0;

	std::thread *Receiver = new std::thread(TestThreadForRecvUdp);
	StkPlSleepMs(500);
	std::thread *Sender = new std::thread(TestThreadForSendUdp);

	int Loop;
	for (Loop = 0; Loop < 30; Loop++) {
		if (ThreadEndCount == 2) {
			break;
		}
		StkPlSleepMs(100);
	}
	if (Loop == 30) {
		StkPlPrintf("NG\r\n");
		exit(-1);
	}
	StkPlPrintf("OK\r\n");

	StkSocket_Disconnect(201, 201, false);
	StkSocket_Close(121, false);

	StkSocket_DeleteInfo(121);
	StkSocket_DeleteInfo(201);

	Receiver->join();
	Sender->join();
	delete Receiver;
	delete Sender;

	StkSocket_ClearLog();
}
