#include <thread>
#include <chrono>

#include "../../src/StkPl.h"
#include "../../src/stksocket/stksocket.h"
#include "StkSocketTestHttp.h"

void StkSocketTestHttp::TestRecvHttpTermination1()
{
	unsigned char Dat[1024];

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		int Ret = StkSocket_Accept(100);
		if (Ret == -1) {
			continue;
		}
		Ret = StkSocket_Receive(100, 100, Dat, 1024, STKSOCKET_RECV_FINISHCOND_CONTENTLENGTH, 3000, NULL, -1);
		if (Ret == -1 || Ret == -2) {
			StkPlPrintf("NG\r\n");
			StkPlExit(0);
		}
		if (StkPlStrStr((char*)Dat, "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789\0") == NULL) {
			StkPlPrintf("NG\r\n");
			StkPlExit(0);
		}

		StkPlStrCpy((char*)Dat, 1024, "");
		StkPlStrCat((char*)Dat, 1024, "HTTP/1.1 200 OK\n");
		StkPlStrCat((char*)Dat, 1024, "Content-Length: 51\n");
		StkPlStrCat((char*)Dat, 1024, "Content-Type: text/html\n");
		StkPlStrCat((char*)Dat, 1024, "\n");
		StkPlStrCat((char*)Dat, 1024, "0123456789012345678901234567890123456789012345678z");
		int RetS = StkSocket_Send(100, 100, (unsigned char*)Dat, StkPlStrLen((char*)Dat) + 1);
		if (RetS <= 0) {
			StkPlPrintf("NG\r\n");
			StkPlExit(0);
		}
		StkSocket_CloseAccept(100, 100, true);
		break;
	}
}

void StkSocketTestHttp::TestSendHttpTermination1()
{
	unsigned char Dat[1024];

	StkPlStrCpy((char*)Dat, 1024, "");
	StkPlStrCat((char*)Dat, 1024, "POST / HTTP/1.1\n");
	StkPlStrCat((char*)Dat, 1024, "Content-Type: text/html\n");
	StkPlStrCat((char*)Dat, 1024, "Content-Length: 101\n");
	StkPlStrCat((char*)Dat, 1024, "\n");
	StkPlStrCat((char*)Dat, 1024, "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789");

	std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	StkSocket_Connect(101);
	int RetS = StkSocket_Send(101, 101, (unsigned char*)Dat, StkPlStrLen((char*)Dat) + 1);
	if (RetS <= 0) {
		StkPlPrintf("NG\r\n");
		StkPlExit(0);
	}
	int RetR;
	for (int Loop = 0; Loop < 10; Loop++) {
		RetR = StkSocket_Receive(101, 101, Dat, 1024, STKSOCKET_RECV_FINISHCOND_CONTENTLENGTH, 3000, NULL, -1);
		if (RetR > 0) {
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	StkSocket_Disconnect(101, 101, true);
	if (RetR <= 0) {
		StkPlPrintf("NG\r\n");
		StkPlExit(0);
	}
	if (StkPlStrStr((char*)Dat, "0123456789012345678901234567890123456789012345678z\0") == NULL) {
		StkPlPrintf("NG\r\n");
		StkPlExit(0);
	}
}

void StkSocketTestHttp::TestRecvHttpTermination2()
{
	unsigned char Dat[1024];
	unsigned char TestData[13][256] = {
		"TestTestTest\0",
		"POST / HTTP/1.1\nTransfer-Encoding: chunked\nContent-Type: text/html\n\nTestTestTest",
		"POST / HTTP/1.1\nContent-Type: text/html\nContent-Length:\n\n",
		"Content-Length:\n\n",
		"POST / HTTP/1.1\nContent-Length: 0\nContent-Type: text/html\n\n",
		"Content-Length: 10\n\naa",
		"POST / HTTP/1.1\nContent-Type: text/html\nContent-Length: 4\n\nTest",
		"POST / HTTP/1.1\nContent-Type: text/html\nContent-Length:TestTestTestTestTest\n\n",
		"Content-Length: 4\n\naa",
		"\n\r\n\r",
		"POST / HTTP/1.1\r\nTransfer-Encoding: chunked\r\nContent-Type: text/html\r\n\r\nTestTestTestHello!",
		"POST / HTTP/1.1\r\nTransfer-Encoding: chunked\r\nContent-Type: text/html\r\n\r\nTestTestTestHello!",
		"POST /aaa/bbb HTTP/1.1\r\nTransfer-Encoding: chunked\nContent-Type: text/html\r\n\r\nhello, world!"
	};

	for (int Loop = 0; Loop < 13; Loop++) {
		while (true) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			long long TcB = StkPlGetTickCount();
			int Ret = StkSocket_Accept(100);
			if (Ret == -1) {
				continue;
			}
			Ret = StkSocket_Receive(100, 100, Dat, 1024, STKSOCKET_RECV_FINISHCOND_CONTENTLENGTH, 3000, NULL, -1);
			if (Ret == -1 || Ret == -2) {
				StkPlPrintf("NG (1)\r\n");
				StkPlExit(0);
			}
			Dat[Ret] = '\0';
			if (StkPlStrCmp((char*)Dat, (char*)TestData[Loop]) != 0) {
				StkPlPrintf("NG (2)\r\n");
				StkPlExit(0);
			}
			long long TcE = StkPlGetTickCount();
			int Tc = int(TcE - TcB) / 1000;
			StkPlPrintf("<%d> ", Tc);
			if ((Loop == 0 && Tc != 3) || (Loop == 1 && Tc != 3) || (Loop == 2 && Tc != 0) ||
				(Loop == 3 && Tc != 0) || (Loop == 4 && Tc != 0) || (Loop == 5 && Tc != 3) ||
				(Loop == 6 && Tc != 0) || (Loop == 7 && Tc != 3) || (Loop == 8 && Tc != 3) ||
				(Loop == 9 && Tc != 0) || (Loop == 10 && Tc != 3) || (Loop == 11 && Tc != 0) ||
				(Loop == 12 && Tc != 0)) {
				StkPlPrintf("NG (3)\r\n");
				StkPlExit(0);
			}
			if (Loop == 12) {
				StkSocket_CloseAccept(100, 100, true);
				break;
			}

			StkPlStrCpy((char*)Dat, 1024, "");
			StkPlStrCat((char*)Dat, 1024, "HTTP/1.1 200 OK\n");
			StkPlStrCat((char*)Dat, 1024, "Content-Type: text/html\n");
			StkPlStrCat((char*)Dat, 1024, "Content-Length: 51\n");
			StkPlStrCat((char*)Dat, 1024, "\n");
			StkPlStrCat((char*)Dat, 1024, "0123456789012345678901234567890123456789012345678z");
			int RetS = StkSocket_Send(100, 100, (unsigned char*)Dat, StkPlStrLen((char*)Dat) + 1);
			if (RetS <= 0) {
				StkPlPrintf("NG (4)\r\n");
				StkPlExit(0);
			}
			StkSocket_CloseAccept(100, 100, true);
			break;
		}
	}
}

void StkSocketTestHttp::TestSendHttpTermination2()
{
	unsigned char Dat[1024];
	unsigned char TestData[13][256] = {
		"TestTestTest\0",
		"POST / HTTP/1.1\nTransfer-Encoding: chunked\nContent-Type: text/html\n\nTestTestTest",
		"POST / HTTP/1.1\nContent-Type: text/html\nContent-Length:\n\nTestTestTest",
		"Content-Length:\n\n",
		"POST / HTTP/1.1\nContent-Length: 0\nContent-Type: text/html\n\n",
		"Content-Length: 10\n\naa",
		"POST / HTTP/1.1\nContent-Type: text/html\nContent-Length: 4\n\nTestTestTestTestTest",
		"POST / HTTP/1.1\nContent-Type: text/html\nContent-Length:TestTestTestTestTest\n\n",
		"Content-Length: 4\n\naa",
		"\n\r\n\raaaaa",
		"POST / HTTP/1.1\r\nTransfer-Encoding: chunked\r\nContent-Type: text/html\r\n\r\n0c\r\nTestTestTest\r\n0006\r\nHello!\r\n",
		"POST / HTTP/1.1\r\nTransfer-Encoding: chunked\r\nContent-Type: text/html\r\n\r\n0c\r\nTestTestTest\r\n0006\r\nHello!\r\n00\r\n\r\n",
		"POST /aaa/bbb HTTP/1.1\r\nTransfer-Encoding: chunked\nContent-Type: text/html\r\n\r\n100\r\nhello, world!"
	};

	for (int Loop = 0; Loop < 13; Loop++) {
		StkPlStrCpy((char*)Dat, 1024, (char*)TestData[Loop]);

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		while (StkSocket_Connect(101) == -1) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		int RetS = StkSocket_Send(101, 101, (unsigned char*)Dat, StkPlStrLen((char*)Dat) + 1);
		if (RetS <= 0) {
			StkPlPrintf("NG\r\n");
			StkPlExit(0);
		}
		if (Loop == 12) {
			StkSocket_Disconnect(101, 101, true);
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			break;
		}
		int RetR;
		for (int Loop = 0; Loop < 10; Loop++) {
			RetR = StkSocket_Receive(101, 101, Dat, 1024, STKSOCKET_RECV_FINISHCOND_CONTENTLENGTH, 3000, NULL, -1);
			if (RetR > 0) {
				break;
			}
			
		}
		StkSocket_Disconnect(101, 101, true);
		if (RetR <= 0) {
			StkPlPrintf("NG\r\n");
			StkPlExit(0);
		}
		if (StkPlStrStr((char*)Dat, "0123456789012345678901234567890123456789012345678z\0") == NULL) {
			StkPlPrintf("NG\r\n");
			StkPlExit(0);
		}
	}
}

void StkSocketTestHttp::TestHttpTermination()
{
	StkSocket_AddInfo(100, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, L"127.0.0.1", 2100);
	StkSocket_Open(100);
	StkSocket_AddInfo(101, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"127.0.0.1", 2100);

	StkPlPrintf("[StkSocketTestHttp]:Normal case ... ");
	{
		std::thread *Receiver = new std::thread(TestRecvHttpTermination1);
		std::thread *Sender = new std::thread(TestSendHttpTermination1);
		Receiver->join();
		Sender->join();
		delete Receiver;
		delete Sender;
	}
	StkPlPrintf("OK\r\n");

	StkPlPrintf("[StkSocketTestHttp]:Abnormal case (no Content-Length) ... ");
	{
		std::thread *Receiver = new std::thread(TestRecvHttpTermination2);
		std::thread *Sender = new std::thread(TestSendHttpTermination2);
		Receiver->join();
		Sender->join();
		delete Receiver;
		delete Sender;
	}
	StkPlPrintf("OK\r\n");

	StkSocket_DeleteInfo(101);
	StkSocket_Close(100, true);
	StkSocket_DeleteInfo(100);
	StkSocket_ClearLog();
}
