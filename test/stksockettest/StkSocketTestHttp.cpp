#include <stdio.h>
#include <tchar.h>
#include "..\..\src\stksocket\stksocket.h"
#include "StkSocketTestHttp.h"

BOOL StkSocketTestHttp::FinishSendTest = FALSE;
BOOL StkSocketTestHttp::FinishRecvTest = FALSE;

DWORD WINAPI StkSocketTestHttp::TestRecvHttpTermination1(LPVOID Param)
{
	BYTE Dat[1024];

	while (TRUE) {
		Sleep(10);
		int Ret = StkSocket_Accept(100);
		if (Ret == -1) {
			continue;
		}
		Ret = StkSocket_Receive(100, 100, Dat, 1024, STKSOCKET_RECV_FINISHCOND_CONTENTLENGTH, 3000, NULL, -1, FALSE);
		if (Ret == -1 || Ret == -2) {
			printf("NG\r\n");
			exit(0);
		}
		if (strstr((char*)Dat, "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789\0") == NULL) {
			printf("NG\r\n");
			exit(0);
		}

		strcpy_s((char*)Dat, 1024, "");
		strcat_s((char*)Dat, 1024, "HTTP/1.1 200 OK\n");
		strcat_s((char*)Dat, 1024, "Content-Length: 51\n");
		strcat_s((char*)Dat, 1024, "Content-Type: text/html\n");
		strcat_s((char*)Dat, 1024, "\n");
		strcat_s((char*)Dat, 1024, "0123456789012345678901234567890123456789012345678z");
		int RetS = StkSocket_Send(100, 100, (BYTE*)Dat, strlen((char*)Dat) + 1);
		if (RetS <= 0) {
			printf("NG\r\n");
			exit(0);
		}
		StkSocket_CloseAccept(100, 100, TRUE);
		break;
	}

	FinishRecvTest = TRUE;
	return 0;
}

DWORD WINAPI StkSocketTestHttp::TestSendHttpTermination1(LPVOID Param)
{
	BYTE Dat[1024];

	strcpy_s((char*)Dat, 1024, "");
	strcat_s((char*)Dat, 1024, "POST / HTTP/1.1\n");
	strcat_s((char*)Dat, 1024, "Content-Type: text/html\n");
	strcat_s((char*)Dat, 1024, "Content-Length: 101\n");
	strcat_s((char*)Dat, 1024, "\n");
	strcat_s((char*)Dat, 1024, "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789");

	Sleep(3000);
	StkSocket_Connect(101);
	int RetS = StkSocket_Send(101, 101, (BYTE*)Dat, strlen((char*)Dat) + 1);
	if (RetS <= 0) {
		printf("NG\r\n");
		exit(0);
	}
	int RetR;
	for (int Loop = 0; Loop < 10; Loop++) {
		RetR = StkSocket_Receive(101, 101, Dat, 1024, STKSOCKET_RECV_FINISHCOND_CONTENTLENGTH, 3000, NULL, -1, FALSE);
		if (RetR > 0) {
			break;
		}
		Sleep(100);
	}
	StkSocket_Disconnect(101, 101, TRUE);
	if (RetR <= 0) {
		printf("NG\r\n");
		exit(0);
	}
	if (strstr((char*)Dat, "0123456789012345678901234567890123456789012345678z\0") == NULL) {
		printf("NG\r\n");
		exit(0);
	}

	FinishSendTest = TRUE;
	return 0;
}

DWORD WINAPI StkSocketTestHttp::TestRecvHttpTermination2(LPVOID Param)
{
	BYTE Dat[1024];
	BYTE TestData[9][256] = {
		"TestTestTest\0",
		"POST / HTTP/1.1\nContent-Type: text/html\n\nTestTestTest",
		"POST / HTTP/1.1\nContent-Type: text/html\nContent-Length:\n\n",
		"Content-Length:\n\n",
		"POST / HTTP/1.1\nContent-Length: 0\nContent-Type: text/html\n\n",
		"\n\nContent-Length: 10\n\naa",
		"POST / HTTP/1.1\nContent-Type: text/html\nContent-Length: 4\n\nTest",
		"POST / HTTP/1.1\nContent-Type: text/html\nContent-Length:TestTestTestTestTest\n\n",
		"\n\nContent-Length: 4\n\naa"
	};

	for (int Loop = 0; Loop < 9; Loop++) {
		while (TRUE) {
			Sleep(10);
			int TcB = GetTickCount();
			int Ret = StkSocket_Accept(100);
			if (Ret == -1) {
				continue;
			}
			Ret = StkSocket_Receive(100, 100, Dat, 1024, STKSOCKET_RECV_FINISHCOND_CONTENTLENGTH, 3000, NULL, -1, FALSE);
			if (Ret == -1 || Ret == -2) {
				printf("NG\r\n");
				exit(0);
			}
			Dat[Ret] = '\0';
			if (strcmp((char*)Dat, (char*)TestData[Loop]) != 0) {
				printf("NG\r\n");
				exit(0);
			}
			int TcE = GetTickCount();
			int Tc = (TcE - TcB) / 1000;
			printf("<%d> ", Tc);
			if ((Loop == 0 && Tc != 3) || (Loop == 1 && Tc != 3) || (Loop == 2 && Tc != 0) ||
				(Loop == 3 && Tc != 0) || (Loop == 4 && Tc != 0) || (Loop == 5 && Tc != 3) ||
				(Loop == 6 && Tc != 0) || (Loop == 7 && Tc != 3) || (Loop == 8 && Tc != 3)) {
				printf("NG\r\n");
				exit(0);
			}

			strcpy_s((char*)Dat, 1024, "");
			strcat_s((char*)Dat, 1024, "HTTP/1.1 200 OK\n");
			strcat_s((char*)Dat, 1024, "Content-Type: text/html\n");
			strcat_s((char*)Dat, 1024, "Content-Length: 51\n");
			strcat_s((char*)Dat, 1024, "\n");
			strcat_s((char*)Dat, 1024, "0123456789012345678901234567890123456789012345678z");
			int RetS = StkSocket_Send(100, 100, (BYTE*)Dat, strlen((char*)Dat) + 1);
			if (RetS <= 0) {
				printf("NG\r\n");
				exit(0);
			}
			StkSocket_CloseAccept(100, 100, TRUE);
			break;
		}
	}

	FinishRecvTest = TRUE;
	return 0;
}

DWORD WINAPI StkSocketTestHttp::TestSendHttpTermination2(LPVOID Param)
{
	BYTE Dat[1024];
	BYTE TestData[9][256] = {
		"TestTestTest\0",
		"POST / HTTP/1.1\nContent-Type: text/html\n\nTestTestTest",
		"POST / HTTP/1.1\nContent-Type: text/html\nContent-Length:\n\nTestTestTest",
		"Content-Length:\n\n",
		"POST / HTTP/1.1\nContent-Length: 0\nContent-Type: text/html\n\n",
		"\n\nContent-Length: 10\n\naa",
		"POST / HTTP/1.1\nContent-Type: text/html\nContent-Length: 4\n\nTestTestTestTestTest",
		"POST / HTTP/1.1\nContent-Type: text/html\nContent-Length:TestTestTestTestTest\n\n",
		"\n\nContent-Length: 4\n\naa"
	};

	for (int Loop = 0; Loop < 9; Loop++) {
		strcpy_s((char*)Dat, 1024, (char*)TestData[Loop]);

		Sleep(3000);
		StkSocket_Connect(101);
		int RetS = StkSocket_Send(101, 101, (BYTE*)Dat, strlen((char*)Dat) + 1);
		if (RetS <= 0) {
			printf("NG\r\n");
			exit(0);
		}
		int RetR;
		for (int Loop = 0; Loop < 10; Loop++) {
			RetR = StkSocket_Receive(101, 101, Dat, 1024, STKSOCKET_RECV_FINISHCOND_CONTENTLENGTH, 3000, NULL, -1, FALSE);
			if (RetR > 0) {
				break;
			}
			Sleep(100);
		}
		StkSocket_Disconnect(101, 101, TRUE);
		if (RetR <= 0) {
			printf("NG\r\n");
			exit(0);
		}
		if (strstr((char*)Dat, "0123456789012345678901234567890123456789012345678z\0") == NULL) {
			printf("NG\r\n");
			exit(0);
		}
	}

	FinishSendTest = TRUE;
	return 0;
}

void StkSocketTestHttp::TestHttpTermination()
{
	DWORD TmpId;

	StkSocket_AddInfo(100, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_RECEIVER, _T("127.0.0.1"), 2100);
	StkSocket_Open(100);
	StkSocket_AddInfo(101, STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, _T("127.0.0.1"), 2100);

	printf("[StkSocketTestHttp]:Normal case ... ");
	FinishRecvTest = FALSE;
	FinishSendTest = FALSE;
	CreateThread(NULL, 0, &TestRecvHttpTermination1, NULL, 0, &TmpId);
	CreateThread(NULL, 0, &TestSendHttpTermination1, NULL, 0, &TmpId);
	while (FinishRecvTest != TRUE || FinishSendTest != TRUE) {
		Sleep(100);
	}
	printf("OK\r\n");

	printf("[StkSocketTestHttp]:Abnormal case (no Content-Length) ... ");
	FinishRecvTest = FALSE;
	FinishSendTest = FALSE;
	CreateThread(NULL, 0, &TestRecvHttpTermination2, NULL, 0, &TmpId);
	CreateThread(NULL, 0, &TestSendHttpTermination2, NULL, 0, &TmpId);
	while (FinishRecvTest != TRUE || FinishSendTest != TRUE) {
		Sleep(100);
	}
	printf("OK\r\n");

	StkSocket_DeleteInfo(101);
	StkSocket_Close(100, TRUE);
	StkSocket_DeleteInfo(100);
	StkSocket_ClearLog();
}
