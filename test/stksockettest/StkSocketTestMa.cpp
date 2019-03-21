#include <thread>
#include <chrono>
#include "../../src/stkpl/StkPl.h"
#include "../../src/stksocket/stksocket.h"
#include "StkSocketTestMa.h"

int StkSocketTestMa::ThreadStartCount = 0;
int StkSocketTestMa::ThreadEndCount = 0;

void StkSocketTestMa::TestThreadForAccept2Recv1()
{
	ThreadStartCount++;
	unsigned char Buf[1000000];
	while (true) {
		if (StkSocket_Accept(121) == 0) {
			while (StkSocket_GetStatus(121) == STKSOCKET_STATUS_ACCEPT) {
				StkSocket_Receive(121, 121, Buf, 1000000, STKSOCKET_RECV_FINISHCOND_UNCONDITIONAL, 100, NULL, 0);
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
			StkSocket_CloseAccept(121, 121, false);
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	ThreadEndCount++;
}

void StkSocketTestMa::TestThreadForAccept2Recv2()
{
	ThreadStartCount++;
	unsigned char Buf[1000000];
	while (true) {
		if (StkSocket_Accept(122) == 0) {
			while (StkSocket_GetStatus(122) == STKSOCKET_STATUS_ACCEPT) {
				StkSocket_Receive(122, 122, Buf, 1000000, STKSOCKET_RECV_FINISHCOND_UNCONDITIONAL, 100, NULL, 0);
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
			StkSocket_CloseAccept(122, 122, false);
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	ThreadEndCount++;
}

void StkSocketTestMa::TestThreadForAccept2Recv3()
{
	ThreadStartCount++;
	unsigned char Buf[1000000];
	while (true) {
		if (StkSocket_Accept(123) == 0) {
			while (StkSocket_GetStatus(123) == STKSOCKET_STATUS_ACCEPT) {
				StkSocket_Receive(123, 123, Buf, 1000000, STKSOCKET_RECV_FINISHCOND_UNCONDITIONAL, 100, NULL, 0);
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
			StkSocket_CloseAccept(123, 123, false);
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	ThreadEndCount++;
}

void StkSocketTestMa::TestThreadForAccept2Send1()
{
	ThreadStartCount++;
	char Buf[1024];
	StkPlStrCpy(Buf, 1024, "Hello, world from #1\r\n");
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	StkSocket_Connect(201);
	for (int Loop = 0; Loop < 50; Loop++) {
		StkSocket_Send(201, 201, (unsigned char*)Buf, StkPlStrLen(Buf) + 1);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	StkSocket_Disconnect(201, 201, false);

	ThreadEndCount++;
}

void StkSocketTestMa::TestThreadForAccept2Send2()
{
	ThreadStartCount++;
	char Buf[1024];
	StkPlStrCpy(Buf, 1024, "Hello, world from #2\r\n");
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	StkSocket_Connect(202);
	for (int Loop = 0; Loop < 50; Loop++) {
		StkSocket_Send(202, 202, (unsigned char*)Buf, StkPlStrLen(Buf) + 1);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	StkSocket_Disconnect(202, 202, false);

	ThreadEndCount++;
}

void StkSocketTestMa::TestThreadForAccept2Send3()
{
	ThreadStartCount++;
	char Buf[1024];
	StkPlStrCpy(Buf, 1024, "Hello, world from #3\r\n");
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	StkSocket_Connect(203);
	for (int Loop = 0; Loop < 50; Loop++) {
		StkSocket_Send(203, 203, (unsigned char*)Buf, StkPlStrLen(Buf) + 1);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	StkSocket_Disconnect(203, 203, false);

	ThreadEndCount++;
}

void StkSocketTestMa::TestMultiAccept2()
{
	StkPlPrintf("[StkSocketTestMa::TestMultiAccept2] : Create socket --> Status validation [open] ... ");
	StkSocket_AddInfo(121, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2202);
	StkSocket_Open(121);
	StkSocket_CopyInfo(122, 121);
	StkSocket_CopyInfo(123, 121);
	if (StkSocket_GetStatus(121) != STKSOCKET_STATUS_OPEN ||
		StkSocket_GetStatus(122) != STKSOCKET_STATUS_OPEN ||
		StkSocket_GetStatus(123) != STKSOCKET_STATUS_OPEN) {
			StkPlPrintf("NG\r\n");
			exit(-1);
	}
	StkPlPrintf("OK\r\n");

	StkSocket_AddInfo(201, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2202);
	StkSocket_Open(201);
	StkSocket_AddInfo(202, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2202);
	StkSocket_Open(202);
	StkSocket_AddInfo(203, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2202);
	StkSocket_Open(203);

	ThreadStartCount = 0;
	ThreadEndCount = 0;

	std::thread *Receiver1 = new std::thread(TestThreadForAccept2Recv1);
	std::thread *Receiver2 = new std::thread(TestThreadForAccept2Recv2);
	std::thread *Receiver3 = new std::thread(TestThreadForAccept2Recv3);
	std::thread *Sender1 = new std::thread(TestThreadForAccept2Send1);
	std::thread *Sender2 = new std::thread(TestThreadForAccept2Send2);
	std::thread *Sender3 = new std::thread(TestThreadForAccept2Send3);
	Sender1->join();
	Sender2->join();
	Sender3->join();

	int Loop;
	for (Loop = 0; Loop < 30; Loop++) {
		if (ThreadStartCount == 6 &&
			StkSocket_GetStatus(122) == STKSOCKET_STATUS_ACCEPT &&
			StkSocket_GetStatus(123) == STKSOCKET_STATUS_ACCEPT &&
			StkSocket_GetStatus(121) == STKSOCKET_STATUS_ACCEPT) {
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	StkPlPrintf("[StkSocketTestMa::TestMultiAccept2] : All threads are started [wait=%d msec] ... ", Loop * 100);
	if (Loop == 30) {
		StkPlPrintf("NG\r\n");
		exit(-1);
	}
	StkPlPrintf("OK\r\n");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	StkPlPrintf("[StkSocketTestMa::TestMultiAccept2] : All receiver status are showing STKSOCKET_STATUS_ACCEPT ... ");
	if (StkSocket_GetStatus(122) != STKSOCKET_STATUS_ACCEPT ||
		StkSocket_GetStatus(123) != STKSOCKET_STATUS_ACCEPT ||
		StkSocket_GetStatus(121) != STKSOCKET_STATUS_ACCEPT) {
		StkPlPrintf("NG\r\n");
		exit(-1);
	}
	StkPlPrintf("OK\r\n");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	StkSocket_Close(122, false);
	StkPlPrintf("[StkSocketTestMa::TestMultiAccept2] : Target socket status is showing STKSOCKET_STATUS_CLOSE ... ");
	if (StkSocket_GetStatus(122) != STKSOCKET_STATUS_CLOSE ||
		StkSocket_GetStatus(123) != STKSOCKET_STATUS_ACCEPT ||
		StkSocket_GetStatus(121) != STKSOCKET_STATUS_ACCEPT) {
		StkPlPrintf("NG\r\n");
		exit(-1);
	}
	StkPlPrintf("OK\r\n");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	StkSocket_Close(121, false);
	StkPlPrintf("[StkSocketTestMa::TestMultiAccept2] : All receiver status are showing STKSOCKET_STATUS_CLOSE ... ");
	if (StkSocket_GetStatus(121) != STKSOCKET_STATUS_CLOSE ||
		StkSocket_GetStatus(122) != STKSOCKET_STATUS_CLOSE ||
		StkSocket_GetStatus(123) != STKSOCKET_STATUS_CLOSE) {
		StkPlPrintf("NG\r\n");
		exit(-1);
	}
	StkPlPrintf("OK\r\n");
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	for (Loop = 0; Loop < 150; Loop++) {
		if (ThreadEndCount == 6) {
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	StkPlPrintf("[StkSocketTestMa::TestMultiAccept2] : All threads are ended [wait=%d msec] ... ", Loop * 100);
	if (Loop == 50) {
		StkPlPrintf("NG\r\n");
		exit(-1);
	}
	StkPlPrintf("OK\r\n");

	StkSocket_DeleteInfo(121);
	StkSocket_DeleteInfo(122);
	StkSocket_DeleteInfo(123);
	StkSocket_DeleteInfo(201);
	StkSocket_DeleteInfo(202);
	StkSocket_DeleteInfo(203);

	Receiver1->join();
	Receiver2->join();
	Receiver3->join();
	delete Receiver1;
	delete Receiver2;
	delete Receiver3;
	delete Sender1;
	delete Sender2;
	delete Sender3;
}

void StkSocketTestMa::TestMultiAccept3()
{
	StkPlPrintf("[StkSocketTestMa::TestMultiAccept3] : Create/Copy socket --> Status validation [close] ... ");
	StkSocket_AddInfo(121, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2202);
	StkSocket_CopyInfo(122, 121);
	StkSocket_CopyInfo(123, 121);
	if (StkSocket_GetStatus(121) != STKSOCKET_STATUS_CLOSE ||
		StkSocket_GetStatus(122) != STKSOCKET_STATUS_CLOSE ||
		StkSocket_GetStatus(123) != STKSOCKET_STATUS_CLOSE) {
			StkPlPrintf("NG\r\n");
			exit(-1);
	}
	StkPlPrintf("OK\r\n");

	StkPlPrintf("[StkSocketTestMa::TestMultiAccept3] : Check the copied sockets are also opened after source socket opened ... ");
	StkSocket_Open(121);
	if (StkSocket_GetStatus(121) != STKSOCKET_STATUS_OPEN ||
		StkSocket_GetStatus(122) != STKSOCKET_STATUS_OPEN ||
		StkSocket_GetStatus(123) != STKSOCKET_STATUS_OPEN) {
			StkPlPrintf("NG\r\n");
			exit(-1);
	}
	StkPlPrintf("OK\r\n");

	StkPlPrintf("[StkSocketTestMa::TestMultiAccept3] : Check the copied sockets are also closed after source socket closed ... ");
	StkSocket_Close(121, true);
	if (StkSocket_GetStatus(121) != STKSOCKET_STATUS_CLOSE ||
		StkSocket_GetStatus(122) != STKSOCKET_STATUS_CLOSE ||
		StkSocket_GetStatus(123) != STKSOCKET_STATUS_CLOSE) {
			StkPlPrintf("NG\r\n");
			exit(-1);
	}
	StkPlPrintf("OK\r\n");

	StkPlPrintf("[StkSocketTestMa::TestMultiAccept3] : Check only copied socket is opened. Source socket is closed ... ");
	StkSocket_Open(122);
	if (StkSocket_GetStatus(121) != STKSOCKET_STATUS_CLOSE ||
		StkSocket_GetStatus(122) != STKSOCKET_STATUS_OPEN ||
		StkSocket_GetStatus(123) != STKSOCKET_STATUS_CLOSE) {
			StkPlPrintf("NG\r\n");
			exit(-1);
	}
	StkPlPrintf("OK\r\n");

	StkPlPrintf("[StkSocketTestMa::TestMultiAccept3] : Check all copied socket is opened after source socket is opend ... ");
	StkSocket_Open(121);
	if (StkSocket_GetStatus(121) != STKSOCKET_STATUS_OPEN ||
		StkSocket_GetStatus(122) != STKSOCKET_STATUS_OPEN ||
		StkSocket_GetStatus(123) != STKSOCKET_STATUS_OPEN) {
			StkPlPrintf("NG\r\n");
			exit(-1);
	}
	StkPlPrintf("OK\r\n");

	StkPlPrintf("[StkSocketTestMa::TestMultiAccept3] : Run threads ... ");
	StkSocket_AddInfo(201, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2202);
	StkSocket_Open(201);
	StkSocket_AddInfo(202, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2202);
	StkSocket_Open(202);
	StkSocket_AddInfo(203, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2202);
	StkSocket_Open(203);

	ThreadStartCount = 0;
	ThreadEndCount = 0;

	std::thread *Receiver1 = new std::thread(TestThreadForAccept2Recv1);
	std::thread *Receiver2 = new std::thread(TestThreadForAccept2Recv2);
	std::thread *Receiver3 = new std::thread(TestThreadForAccept2Recv3);
	std::thread *Sender1 = new std::thread(TestThreadForAccept2Send1);
	std::thread *Sender2 = new std::thread(TestThreadForAccept2Send2);
	std::thread *Sender3 = new std::thread(TestThreadForAccept2Send3);
	Sender1->join();
	Sender2->join();
	Sender3->join();

	StkPlPrintf("OK\r\n");

	int Loop;
	for (Loop = 0; Loop < 30; Loop++) {
		if (ThreadStartCount == 6 &&
			StkSocket_GetStatus(122) == STKSOCKET_STATUS_ACCEPT &&
			StkSocket_GetStatus(123) == STKSOCKET_STATUS_ACCEPT &&
			StkSocket_GetStatus(121) == STKSOCKET_STATUS_ACCEPT) {
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	StkPlPrintf("[StkSocketTestMa::TestMultiAccept3] : All threads are started [wait=%d msec] ... ", Loop * 100);
	if (Loop == 30) {
		StkPlPrintf("NG\r\n");
		exit(-1);
	}
	StkPlPrintf("OK\r\n");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	StkPlPrintf("[StkSocketTestMa::TestMultiAccept3] : All receiver status are showing STKSOCKET_STATUS_ACCEPT ... ");
	if (StkSocket_GetStatus(122) != STKSOCKET_STATUS_ACCEPT ||
		StkSocket_GetStatus(123) != STKSOCKET_STATUS_ACCEPT ||
		StkSocket_GetStatus(121) != STKSOCKET_STATUS_ACCEPT) {
		StkPlPrintf("NG\r\n");
		exit(-1);
	}
	StkPlPrintf("OK\r\n");
	std::this_thread::sleep_for(std::chrono::milliseconds(500));

	StkPlPrintf("[StkSocketTestMa::TestMultiAccept3] : Check only copied socket is closed ... ");
	StkSocket_Close(122, true);
	if (StkSocket_GetStatus(121) != STKSOCKET_STATUS_ACCEPT ||
		StkSocket_GetStatus(122) != STKSOCKET_STATUS_CLOSE ||
		StkSocket_GetStatus(123) != STKSOCKET_STATUS_ACCEPT) {
			StkPlPrintf("NG\r\n");
			exit(-1);
	}
	StkPlPrintf("OK\r\n");

	StkPlPrintf("[StkSocketTestMa::TestMultiAccept3] : Check all copied socket is closed after source socket is closed ... ");
	StkSocket_Close(121, true);
	if (StkSocket_GetStatus(121) != STKSOCKET_STATUS_CLOSE ||
		StkSocket_GetStatus(122) != STKSOCKET_STATUS_CLOSE ||
		StkSocket_GetStatus(123) != STKSOCKET_STATUS_CLOSE) {
			StkPlPrintf("NG\r\n");
			exit(-1);
	}
	StkPlPrintf("OK\r\n");
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	for (Loop = 0; Loop < 150; Loop++) {
		if (ThreadEndCount == 6) {
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	StkPlPrintf("[StkSocketTestMa::TestMultiAccept3] : All threads are ended [wait=%d msec] ... ", Loop * 100);
	if (Loop == 50) {
		StkPlPrintf("NG\r\n");
		exit(-1);
	}
	StkPlPrintf("OK\r\n");

	StkSocket_DeleteInfo(121);
	StkSocket_DeleteInfo(122);
	StkSocket_DeleteInfo(123);
	StkSocket_DeleteInfo(201);
	StkSocket_DeleteInfo(202);
	StkSocket_DeleteInfo(203);

	Receiver1->join();
	Receiver2->join();
	Receiver3->join();
	delete Receiver1;
	delete Receiver2;
	delete Receiver3;
	delete Sender1;
	delete Sender2;
	delete Sender3;
}
