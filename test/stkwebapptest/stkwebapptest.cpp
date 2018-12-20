﻿#include <windows.h>
#include <shlwapi.h>
#include <stdio.h>
#include <string.h>
#include <Psapi.h>
#include "..\..\src\stkthread\stkthread.h"
#include "..\..\src\commonfunc\StkObject.h"
#include "..\..\src\stksocket\stksocket.h"
#include "..\..\src\stkwebapp\StkWebApp.h"
#include "..\..\src\stkwebapp\StkWebAppExec.h"
#include "StkWebAppTest1.h"
#include "StkWebAppTest2.h"
#include "StkWebAppTest3.h"
#include "StkWebAppTest4.h"
#include "StkWebAppTest5.h"


#define THREADNUM 10

bool SendTestDataFailed = false;
int SendTestDataCount = 0;

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

wchar_t* FindNewLine(wchar_t* Dat)
{
	wchar_t* Ret;
	Ret = StrStr(Dat, L"\r\n\r\n");
	if (Ret != 0) {
		return Ret + 4;
	}
	Ret = StrStr(Dat, L"\n\r\n\r");
	if (Ret != 0) {
		return Ret + 4;
	}
	Ret = StrStr(Dat, L"\n\n");
	if (Ret != 0) {
		return Ret + 2;
	}
	return NULL;
}

bool CompObjs(unsigned char* Dat, unsigned char* RecvDat)
{
	bool Ret = true;
	int ErrCode;
	wchar_t SendDatW[1000000];
	_snwprintf_s(SendDatW, 1000000, _TRUNCATE, L"%S", Dat);
	wchar_t RecvDatW[1000000];
	_snwprintf_s(RecvDatW, 1000000, _TRUNCATE, L"%S", RecvDat);
	wchar_t* Skip = FindNewLine(RecvDatW);
	StkObject* SendObj = NULL;
	StkObject* RecvObj = NULL;
	if (StkObject::Analyze(SendDatW) == 1) {
		SendObj = StkObject::CreateObjectFromXml(SendDatW, &ErrCode);
		RecvObj = StkObject::CreateObjectFromXml(Skip, &ErrCode);
	} else {
		SendObj = StkObject::CreateObjectFromJson(SendDatW, &ErrCode);
		RecvObj = StkObject::CreateObjectFromJson(Skip, &ErrCode);
	}
	if (SendObj == NULL || RecvObj == NULL || !SendObj->Equals(RecvObj)) {
		Ret = false;
	}
	if (SendObj != NULL) {
		delete SendObj;
	}
	if (RecvObj != NULL) {
		delete RecvObj;
	}
	return Ret;
}

StkObject* MakeLargeTestData(wchar_t Name[64], int Width, int Height, int CurrentLevel = 0)
{
	StkObject* Obj = new StkObject(Name);
	if (CurrentLevel > Height) {
		Obj->AppendChildElement(new StkObject(L"Aaa", 12345));
		Obj->AppendChildElement(new StkObject(L"Bbb", 67890));
		Obj->AppendChildElement(new StkObject(L"Ccc", 123.45f));
		Obj->AppendChildElement(new StkObject(L"Ddd", 678.90f));
		Obj->AppendChildElement(new StkObject(L"Eee", L"abcdefghijklmnopqrstuvwxyz0123456789"));
	} else {
		for (int Loop = 0; Loop < Width; Loop++) {
			wchar_t Buf[25];
			wsprintf(Buf, L"Elem_%d:%d", CurrentLevel, Loop);
			Obj->AppendChildElement(MakeLargeTestData(Buf, Width, Height, CurrentLevel + 1));
		}
	}
	return Obj;
}

bool SendTestData(int Id, char* Dat)
{
	unsigned char RecvDat[1000000];
	StkSocket_Connect(Id);

	char TmpHeader[256];
	char UrlPath[StkWebAppExec::URL_PATH_LENGTH];
	if (Id % 3 == 0) {
		strcpy_s(UrlPath, StkWebAppExec::URL_PATH_LENGTH, "/aaa/bbb/");
	} else if (Id % 3 == 1) {
		strcpy_s(UrlPath, StkWebAppExec::URL_PATH_LENGTH, "/aaa/ccc/");
	} else {
		strcpy_s(UrlPath, StkWebAppExec::URL_PATH_LENGTH, "/aaa/ddd/");
	}
	sprintf_s(TmpHeader, 256, "POST %s HTTP/1.1\nContent-Length: %d\nAccept-Language: ja-JP;q=0.5,en-US;q=0.3,en-GB;q=0.2\nContent-Type: application/json\n\n", UrlPath, strlen(Dat) + 1);

	if (StkSocket_Send(Id, Id, (unsigned char*)TmpHeader, strlen((char*)TmpHeader)) <= 0) {
		return false;
	}
	if (StkSocket_Send(Id, Id, (unsigned char*)Dat, strlen(Dat) + 1) <= 0) {
		return false;
	}
	int RetR;
	while (true) {
		RetR = StkSocket_Receive(Id, Id, RecvDat, 1000000, STKSOCKET_RECV_FINISHCOND_CONTENTLENGTH, 5000, NULL, -1);
		if (RetR > 0) {
			RecvDat[RetR] = '\0';
			break;
		}
		if (RetR <= 0) {
			printf("... NG (Ret=%d)\r\n", RetR);
			exit(0);
		}
	}
	StkSocket_Disconnect(Id, Id, true);
	if (RetR < 0) {
		return false;
	}
	if (RetR == 0) {
		return true;
	}
	if (RetR > 0 && strstr((char*)RecvDat, "200 OK") == 0) {
		return false;
	} else {
		return CompObjs((unsigned char*)Dat, (unsigned char*)RecvDat);
	}
}

int SendTestData2(int Id, char* Method, char* Url, char* Dat, char* ContType, int* ErrorCode, wchar_t Header[64] = NULL, int ContLen = -1)
{
	char Tmp[256];
	unsigned char RecvDat[8192];
	*ErrorCode = -1;

	int RetC = StkSocket_Connect(Id);
	int RetS = 0;

	if (Method == NULL && Url == NULL) {
		sprintf_s(Tmp, 256, "%s", Dat);
	} else if (ContLen == -1) {
		sprintf_s(Tmp, 256, "%s %s HTTP/1.1\nContent-Length: %d\nContent-Type: %s\n\n%s", Method, Url, strlen(Dat), ContType, Dat);
	} else {
		sprintf_s(Tmp, 256, "%s %s HTTP/1.1\nContent-Length: %d\nContent-Type: %s\n\n%s", Method, Url, ContLen, ContType, Dat);
	}
	if ((RetS = StkSocket_Send(Id, Id, (unsigned char*)Tmp, strlen((char*)Tmp))) <= 0) {
		return -1;
	}
	int RetR;
	for (int Loop = 0; Loop < 10; Loop++) {
		RetR = StkSocket_Receive(Id, Id, RecvDat, 8192, STKSOCKET_RECV_FINISHCOND_CONTENTLENGTH, 5000, NULL, -1);
		if (RetR > 0) {
			RecvDat[RetR] = '\0';
			break;
		}
	}
	StkSocket_Disconnect(Id, Id, true);
	if (RetR <= 0) {
		return -1;
	}

	// Change UTF-8 into UTF-16
	wchar_t* RecvDatW = NULL;
	int WcSize = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (LPCSTR)RecvDat, -1, NULL, NULL);
	if (WcSize > 0) {
		RecvDatW = new wchar_t[WcSize + 1];
		WcSize = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, (LPCSTR)RecvDat, -1, RecvDatW, WcSize);
		RecvDatW[WcSize] = '\0';
	}

	if (Header != NULL) {
		lstrcpyn(Header, RecvDatW, 64);
	}

	// Skip HTTP header
	wchar_t* DatPtr = StrStr(RecvDatW, L"\n\n");
	if (DatPtr == NULL) {
		DatPtr = StrStr(RecvDatW, L"\r\n\r\n");
	}
	if (DatPtr == NULL) {
		DatPtr = StrStr(RecvDatW, L"\n\r\n\r");
	}

	// Make StkObject from HTTP response data
	int XmlJsonType = StkObject::Analyze(DatPtr);
	StkObject* DatObj = NULL;
	if (XmlJsonType == 1) {
		int TmpCode;
		DatObj = StkObject::CreateObjectFromXml(DatPtr, &TmpCode);
	}
	if (XmlJsonType == 2) {
		int TmpCode;
		DatObj = StkObject::CreateObjectFromJson(DatPtr, &TmpCode);
	}
	delete RecvDatW;

	// Acquire error code
	if (XmlJsonType == 1 || XmlJsonType == 2) {
		wchar_t* ErrorCodeStr = NULL;
		StkObject* TmpObj = DatObj->GetFirstChildElement();
		while (TmpObj != NULL) {
			if (lstrcmp(TmpObj->GetName(), L"Code") == 0) {
				if (XmlJsonType == 1) {
					ErrorCodeStr = TmpObj->GetStringValue();
					*ErrorCode = _wtoi(ErrorCodeStr);
				}
				if (XmlJsonType == 2) {
					*ErrorCode = TmpObj->GetIntValue();
				}
				break;
			}
			TmpObj = TmpObj->GetNext();
		}
		delete DatObj;
	}

	// Acquire status code
	char* ValStr = strstr((char*)RecvDat, "HTTP/1.1");
	ValStr += 9;
	int Val;
	sscanf_s(ValStr, "%d", &Val);
	return Val;
}

int ElemStkThreadMainSend(int Id)
{
	bool Ret;
	if (Id % 3 == 0) {
		Ret = SendTestData(Id, "\"aaa\" : {\"Hello\" : { \"FirstName\" : \"Shinya\", \"Middle\" : \"Tsunemi\", \"Last\" : \"Takeuchi\" }, \"Bye\" : \"Bye\"}");
	} else if (Id % 3 == 1) {
		Ret = SendTestData(Id, "\"Yyy\" : {\"Xxx\" : [{\"Aaa\" : 123, \"Bbb\" : 456, \"Ccc\":789},{\"Aaa\" : [333, 222, 111]}]}");
	} else if (Id % 3 == 2) {
		Ret = SendTestData(Id, "\"Xxx\" : { \"Aaa\" : { \"Bbb\" : \"This is a test.\", \"Ccc\" : 123, \"Ddd\" : { \"D1\" : 0, \"D2\" : {\"D3a\" : \"test\"}, \"D3\" : 2}, \"Eee\" : 0.5 }}");
	}
	if (Ret == false) {
		SendTestDataFailed = true;
	}
	SendTestDataCount++;

	return 0;
}

int ElemStkThreadMainSend2(int Id)
{
	int ErrorCode;

	printf("StkWebAppTest2:Invalid request 1 == 400");
	if (SendTestData2(Id, NULL, NULL, "dummy", "", &ErrorCode) != 400 || ErrorCode != 1005) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	printf("StkWebAppTest2:Invalid request 2 == 400");
	if (SendTestData2(Id, NULL, NULL, "Aaaaaaaaaaaaaaaaaaaaaaaaa\r\n\r\n Bbbbbbbbbbbbbb HTTP Cccccccccccccccccccccccccccc", "", &ErrorCode) != 400 || ErrorCode != 1005) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	printf("StkWebAppTest2:GET /aaa/ [{ \"AAA\":123 } with No Content-Type] == 404");
	if (SendTestData2(Id, NULL, NULL, "GET /aaa/ HTTP/1.1\r\n\r\n{ \"AAA\":123 }\n", "", &ErrorCode) != 404 || ErrorCode != 1001) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	printf("StkWebAppTest2:GET /abc/ [{ \"AAA\":123 }] == 404");
	if (SendTestData2(Id, "GET", "/abc/", "{ \"AAA\":123 }\n", "application/json", &ErrorCode) != 404 || ErrorCode != 1001) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	printf("StkWebAppTest2:GET /abc/ [\"aaa\" : {\"bbb\" : \"xxx\"}] == 404");
	if (SendTestData2(Id, "GET", "/abc/", "\"aaa\" : {\"bbb\" : \"xxx\"}\n", "application/json", &ErrorCode) != 404 || ErrorCode != 1001) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	printf("StkWebAppTest2:GET /abc/ [{ \"AAA\":123 } with application/xml] == 400");
	if (SendTestData2(Id, "GET", "/abc/", "{ \"AAA\":123 }\n", "application/xml", &ErrorCode) != 400 || ErrorCode != 1002) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	printf("StkWebAppTest2:GET /abc/ [XYZ] == 400");
	if (SendTestData2(Id, "GET", "/abc/", "XYZ\n", "application/json", &ErrorCode) != 400 || ErrorCode != 1002) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	printf("StkWebAppTest2:GET /abc/ [<Aaa><Bbb/></Aaa>] == 400");
	if (SendTestData2(Id, "GET", "/abc/", "<Aaa><Bbb/></Aaa>\n", "application/xml", &ErrorCode) != 400 || ErrorCode != 1002) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	// Timeout detection start
	{
		wchar_t Header[64];

		int Ret = 0;
		DWORD StartTime = 0;
		DWORD EndTime = 0;

		printf("StkWebAppTest2:Invalid Content-Length (Timeout default 3000 milliseconds)");
		StartTime = GetTickCount();
		if ((Ret = SendTestData2(Id, "POST", "/test/", "{ \"Test\" : \"1\" }\n", "application/json", &ErrorCode, Header, 1024)) != 404 || ErrorCode != 1001) {
			printf("... NG\r\n");
			exit(0);
		}
		EndTime = GetTickCount();
		if ((EndTime - StartTime) < 3000 || (EndTime - StartTime) > 4000) {
			printf("... NG\r\n");
			exit(0);
		}
		printf("... OK\r\n");
	}
	// Timeout detection end

	printf("StkWebAppTest2:POST /service/ [{ \"Operation\" : \"Start\" }] == 400");
	if (SendTestData2(Id, "POST", "/service/", "{ \"Operation\" : \"Start\" }\n", "application/json", &ErrorCode) != 400 || ErrorCode != 1004) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	printf("StkWebAppTest2:POST /service/ [{ \"Stop\" : \"YES\" }] == 400");
	if (SendTestData2(Id, "POST", "/service/", "{ \"Stop\" : \"YES\" }\n", "application/json", &ErrorCode) != 400 || ErrorCode != 1004) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	printf("StkWebAppTest2:POST /service/ [{ \"Operation\" : \"Stop\" }] == 202");
	if (SendTestData2(Id, "POST", "/service/", "{ \"Operation\" : \"Stop\" }\n", "application/json", &ErrorCode) != 202 || ErrorCode != -1) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	Sleep(1000);

	return 0;
}

int ElemStkThreadMainSend3(int Id)
{
	int ErrorCode;
	wchar_t Header[64];

	printf("StkWebAppTest3:GET /aaa/100/ [{ \"AAA\":\"aaa\" }] == 100 Continue");
	if (SendTestData2(Id, "GET", "/aaa/100/", "{ \"AAA\":\"aaa\" }\n", "application/json", &ErrorCode, Header) != 100 || StrStr(Header, L"Continue") == NULL || ErrorCode != -1) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	printf("StkWebAppTest3:GET /aaa/102/ [{ \"AAA\":\"aaa\" }] == 102 Processing");
	if (SendTestData2(Id, "GET", "/aaa/102/", "{ \"AAA\":\"aaa\" }\n", "application/json", &ErrorCode, Header) != 102 || StrStr(Header, L"Processing") == NULL || ErrorCode != -1) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	printf("StkWebAppTest3:GET /aaa/202/ [{ \"AAA\":\"aaa\" }] == 202 Accepted");
	if (SendTestData2(Id, "GET", "/aaa/202/", "{ \"AAA\":\"aaa\" }\n", "application/json", &ErrorCode, Header) != 202 || StrStr(Header, L"Accepted") == NULL || ErrorCode != -1) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	printf("StkWebAppTest3:GET /aaa/204/ [{ \"AAA\":\"aaa\" }] == 204 No Content");
	if (SendTestData2(Id, "GET", "/aaa/204/", "{ \"AAA\":\"aaa\" }\n", "application/json", &ErrorCode, Header) != 204 || StrStr(Header, L"No Content") == NULL || ErrorCode != -1) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	printf("StkWebAppTest3:GET /aaa/304/ [{ \"AAA\":\"aaa\" }] == 304 Not Modified");
	if (SendTestData2(Id, "GET", "/aaa/304/", "{ \"AAA\":\"aaa\" }\n", "application/json", &ErrorCode, Header) != 304 || StrStr(Header, L"Not Modified") == NULL || ErrorCode != -1) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	printf("StkWebAppTest3:GET /aaa/400/ [{ \"AAA\":\"aaa\" }] == 400 Bad Request");
	if (SendTestData2(Id, "GET", "/aaa/400/", "{ \"AAA\":\"aaa\" }\n", "application/json", &ErrorCode, Header) != 400 || StrStr(Header, L"Bad Request") == NULL || ErrorCode != -1) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	printf("StkWebAppTest3:GET /aaa/401/ [{ \"AAA\":\"aaa\" }] == 401 Unauthorized");
	if (SendTestData2(Id, "GET", "/aaa/401/", "{ \"AAA\":\"aaa\" }\n", "application/json", &ErrorCode, Header) != 401 || StrStr(Header, L"Unauthorized") == NULL || ErrorCode != -1) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	printf("StkWebAppTest3:GET /aaa/404/ [{ \"AAA\":\"aaa\" }] == 404 Not Found");
	if (SendTestData2(Id, "GET", "/aaa/404/", "{ \"AAA\":\"aaa\" }\n", "application/json", &ErrorCode, Header) != 404 || StrStr(Header, L"Not Found") == NULL || ErrorCode != -1) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	printf("StkWebAppTest3:GET /aaa/414/ [{ \"AAA\":\"aaa\" }] == 414 URI Too Long");
	if (SendTestData2(Id, "GET", "/aaa/414/", "{ \"AAA\":\"aaa\" }\n", "application/json", &ErrorCode, Header) != 414 || StrStr(Header, L"URI Too Long") == NULL || ErrorCode != -1) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	printf("StkWebAppTest3:GET /aaa/500/ [{ \"AAA\":\"aaa\" }] == 500 Internal Server Error");
	if (SendTestData2(Id, "GET", "/aaa/500/", "{ \"AAA\":\"aaa\" }\n", "application/json", &ErrorCode, Header) != 500 || StrStr(Header, L"Internal Server Error") == NULL || ErrorCode != -1) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	printf("StkWebAppTest3:GET /aaa/510/ [{ \"AAA\":\"aaa\" }] == 510 Not Extended");
	if (SendTestData2(Id, "GET", "/aaa/510/", "{ \"AAA\":\"aaa\" }\n", "application/json", &ErrorCode, Header) != 510 || StrStr(Header, L"Not Extended") == NULL || ErrorCode != -1) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	// Timeout detection start
	{
		wchar_t Header[64];

		int Ret = 0;
		DWORD StartTime = 0;
		DWORD EndTime = 0;

		printf("StkWebAppTest3:Invalid Content-Length (Timeout 5000 milliseconds)");
		StartTime = GetTickCount();
		if ((Ret = SendTestData2(Id, "GET", "/aaa/200/", "{ \"Test\" : \"1\" }\n", "application/json", &ErrorCode, Header, 1024)) != 200 || ErrorCode != -1) {
			printf("... NG\r\n");
			exit(0);
		}
		EndTime = GetTickCount();
		if ((EndTime - StartTime) < 5000 || (EndTime - StartTime) > 6000) {
			printf("... NG\r\n");
			exit(0);
		}
		printf("... OK\r\n");
	}
	// Timeout detection end

	printf("StkWebAppTest3:POST /service/ [{ \"Operation\" : \"Stop\" }] == 202");
	if (SendTestData2(Id, "POST", "/service/", "{ \"Operation\" : \"Stop\" }\n", "application/json", &ErrorCode) != 202 || ErrorCode != -1) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	Sleep(1000);

	return 0;
}

int ElemStkThreadMainSend4(int Id)
{
	int ErrorCode;

	printf("StkWebAppTest4:GET /bigdata/ [{ \"AAA\":123 }] == 500 buffer over flow");
	if (SendTestData2(Id, "GET", "/bigdata/", "{ \"AAA\":123 }\n", "application/json", &ErrorCode) != 500 || ErrorCode != 1006) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	printf("StkWebAppTest4:GET /middledata/ [{ \"AAA\":123 }] == 500 buffer over flow");
	if (SendTestData2(Id, "GET", "/middledata/", "{ \"AAA\":123 }\n", "application/json", &ErrorCode) != 500 || ErrorCode != 1006) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	printf("StkWebAppTest4:GET /smalldata/ [{ \"AAA\":123 }] == 200 sufficient buffer allocated");
	if (SendTestData2(Id, "GET", "/smalldata/", "{ \"AAA\":123 }\n", "application/json", &ErrorCode) != 200) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	printf("StkWebAppTest4:POST /smalldata/ [{ \"AAAAAAAAAABBBBBBBBBB\":\"CCCCCCCCCCDDDDDDDDDD\" }] == 400 read buffer over flow");
	if (SendTestData2(Id, "POST", "/smalldata/", "{ \"AAAAAAAAAABBBBBBBBBB\":\"CCCCCCCCCCDDDDDDDDDD\" }\n", "application/json", &ErrorCode) != 400 || ErrorCode != 1002) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	printf("StkWebAppTest4:POST /service/ [{ \"Operation\" : \"Stop\" }] == 202");
	if (SendTestData2(Id, "POST", "/service/", "{ \"Operation\" : \"Stop\" }\n", "application/json", &ErrorCode) != 202 || ErrorCode != -1) {
		printf("... NG\r\n");
		exit(0);
	}
	printf("... OK\r\n");

	Sleep(1000);

	return 0;
}

int ElemStkThreadMainSend5(int Id)
{
	bool Ret;
	wchar_t TestObjWstr[1000000];
	char TestObjStr[1000000];
	StkObject* TestObj = MakeLargeTestData(L"testObject", 3, 3);
	TestObj->ToJson(TestObjWstr, 1000000);
	sprintf_s(TestObjStr, 1000000, "%S", TestObjWstr);
	int l = strlen(TestObjStr);
	Ret = SendTestData(Id, TestObjStr);
	SendTestDataCount++;
	delete TestObj;
	if (Ret == false) {
		SendTestDataFailed = true;
	}

	return 0;
}

void ReqResTest1(bool LargeFlag)
{
	printf("StkWebAppTest1:one minute performance ");
	LargeFlag == true ? printf("Large ") : printf("Small ");

	int Ids[10] = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
	int SendIds[10] = {31, 32, 33, 34, 35, 36, 37, 38, 39, 40};
	wchar_t Name[MAX_LENGTH_OF_STKTHREAD_NAME];
	wchar_t Desc[MAX_LENGTH_OF_STKTHREAD_DESCRIPTION];

	for (int Loop = 0; Loop < THREADNUM; Loop++) {
		wsprintf(Name, L"Send-%d", Loop);
		wsprintf(Desc, L"Description-%d", Loop);
		if (LargeFlag) {
			AddStkThread(SendIds[Loop], Name, Desc, NULL, NULL, ElemStkThreadMainSend5, NULL, NULL);
		} else {
			AddStkThread(SendIds[Loop], Name, Desc, NULL, NULL, ElemStkThreadMainSend, NULL, NULL);
		}
		StkSocket_AddInfo(SendIds[Loop], SOCK_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"localhost", 8080);
	}

	StkWebApp* Soc = new StkWebApp(Ids, THREADNUM, L"localhost", 8080);

	StkWebAppTest1* Test1Hndl = new StkWebAppTest1();
	int Add1 = Soc->AddReqHandler(StkWebAppExec::STKWEBAPP_METHOD_POST, L"/aaa/bbb/", (StkWebAppExec*)Test1Hndl);
	StkWebAppTest2* Test2Hndl = new StkWebAppTest2();
	int Add2 = Soc->AddReqHandler(StkWebAppExec::STKWEBAPP_METHOD_POST, L"/aaa/ccc/", (StkWebAppExec*)Test2Hndl);
	StkWebAppTest3* Test3Hndl = new StkWebAppTest3();
	int Add3 = Soc->AddReqHandler(StkWebAppExec::STKWEBAPP_METHOD_POST, L"/aaa/ddd/", (StkWebAppExec*)Test3Hndl);

	SendTestDataCount = 0;
	////////// Main logic starts
	StartSpecifiedStkThreads(SendIds, THREADNUM);
	while (GetNumOfRunStkThread() != GetNumOfStkThread()) {
		Sleep(100);
	}
	Sleep(30000);
	int MemChk[6];
	for (int Loop = 0; Loop < 6; Loop++) {
		Sleep(5000);
		MemChk[Loop] = GetUsedMemorySizeOfCurrentProcess();
		printf("%d,", MemChk[Loop]);
	}
	printf("(Repeat=%d)", SendTestDataCount);
	StopSpecifiedStkThreads(SendIds, THREADNUM);
	while (GetNumOfRunStkThread() != THREADNUM) {
		Sleep(100);
	}
	////////// Main logic ends

	int Del1 = Soc->DeleteReqHandler(StkWebAppExec::STKWEBAPP_METHOD_POST, L"/aaa/bbb/");
	int Del2 = Soc->DeleteReqHandler(StkWebAppExec::STKWEBAPP_METHOD_POST, L"/aaa/ccc/");
	int Del3 = Soc->DeleteReqHandler(StkWebAppExec::STKWEBAPP_METHOD_POST, L"/aaa/ddd/");

	delete Soc;

	for (int Loop = 0; Loop < THREADNUM; Loop++) {
		StkSocket_DeleteInfo(SendIds[Loop]);
		DeleteStkThread(SendIds[Loop]);
	}

	if (SendTestDataFailed == true) {
		printf("... NG(1)\r\n");
		exit(0);
	} else if (MemChk[0] < MemChk[1] && MemChk[1] < MemChk[2] && MemChk[2] < MemChk[3] &&
				MemChk[3] < MemChk[4] && MemChk[4] < MemChk[5]) {
		printf("... NG(2)\r\n");
		exit(0);
	} else if (Add1 != 1 || Add2 != 2 || Add3 != 3 || Del1 != 2 || Del2 != 1 || Del3 != 0) {
		printf("... NG(3)\r\n");
		exit(0);
	}else {
		printf("... OK\r\n");
	}
}

void ReqResTest2()
{
	int Ids[1] = {11};
	int SendIds[1] = {31};
	wchar_t Name[MAX_LENGTH_OF_STKTHREAD_NAME];
	wchar_t Desc[MAX_LENGTH_OF_STKTHREAD_DESCRIPTION];

	wsprintf(Name, L"Sender");
	wsprintf(Desc, L"Description");
	AddStkThread(SendIds[0], Name, Desc, NULL, NULL, ElemStkThreadMainSend2, NULL, NULL);
	StkSocket_AddInfo(SendIds[0], SOCK_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"localhost", 8080);

	StkWebApp* Soc = new StkWebApp(Ids, 1, L"localhost", 8080);

	StartSpecifiedStkThreads(SendIds, 1);
	Soc->TheLoop();
	StopSpecifiedStkThreads(SendIds, 1);

	delete Soc;

	StkSocket_DeleteInfo(SendIds[0]);
	DeleteStkThread(SendIds[0]);
}

void ReqResTest3()
{
	int Ids[1] = {11};
	int SendIds[1] = {31};
	wchar_t Name[MAX_LENGTH_OF_STKTHREAD_NAME];
	wchar_t Desc[MAX_LENGTH_OF_STKTHREAD_DESCRIPTION];

	wsprintf(Name, L"Sender");
	wsprintf(Desc, L"Description");
	AddStkThread(SendIds[0], Name, Desc, NULL, NULL, ElemStkThreadMainSend3, NULL, NULL);
	StkSocket_AddInfo(SendIds[0], SOCK_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"localhost", 8080);

	StkWebApp* Soc = new StkWebApp(Ids, 1, L"localhost", 8080);
	Soc->SetTimeoutInterval(5000);
	StkWebAppTest4* Test4Hndl = new StkWebAppTest4();
	int Add1 = Soc->AddReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"/aaa/$/", (StkWebAppExec*)Test4Hndl);

	StartSpecifiedStkThreads(SendIds, 1);
	Soc->TheLoop();
	StopSpecifiedStkThreads(SendIds, 1);

	int Del1 = Soc->DeleteReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"/aaa/$/");
	delete Soc;

	StkSocket_DeleteInfo(SendIds[0]);
	DeleteStkThread(SendIds[0]);
}

void ReqResTest4()
{
	int Ids[1] = {11};
	int SendIds[1] = {31};
	wchar_t Name[MAX_LENGTH_OF_STKTHREAD_NAME];
	wchar_t Desc[MAX_LENGTH_OF_STKTHREAD_DESCRIPTION];

	wsprintf(Name, L"Sender");
	wsprintf(Desc, L"Description");
	AddStkThread(SendIds[0], Name, Desc, NULL, NULL, ElemStkThreadMainSend4, NULL, NULL);
	StkSocket_AddInfo(SendIds[0], SOCK_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"localhost", 8080);

	StkWebApp* Soc = new StkWebApp(Ids, 1, L"localhost", 8080);
	printf("StkWebAppTest4:Test GetSendBufSize, GetRecvBufSize, SetSendBufSize, SetRecvBufSize ... ");
	if (Soc->GetSendBufSize() != 1000000 || Soc->GetRecvBufSize() != 1000000) {
		printf("NG\r\n");
		exit(0);
	}
	Soc->SetSendBufSize(2000);
	Soc->SetRecvBufSize(100);
	if (Soc->GetSendBufSize() != 2000 || Soc->GetRecvBufSize() != 100) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");

	StkWebAppTest5* Test5aHndl = new StkWebAppTest5();
	StkWebAppTest5* Test5bHndl = new StkWebAppTest5();
	StkWebAppTest5* Test5cHndl = new StkWebAppTest5();
	int Add1 = Soc->AddReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"/bigdata/", (StkWebAppExec*)Test5aHndl);
	int Add2 = Soc->AddReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"/middledata/", (StkWebAppExec*)Test5bHndl);
	int Add3 = Soc->AddReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"/smalldata/", (StkWebAppExec*)Test5cHndl);

	StartSpecifiedStkThreads(SendIds, 1);
	Soc->TheLoop();
	StopSpecifiedStkThreads(SendIds, 1);

	int Del1 = Soc->DeleteReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"/bigdata/");
	int Del2 = Soc->DeleteReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"/middledata/");
	int Del3 = Soc->DeleteReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"/smalldata/");
	delete Soc;

	StkSocket_DeleteInfo(SendIds[0]);
	DeleteStkThread(SendIds[0]);
}

void AddDeleteStkWebAppTest()
{
	{
		printf("Search StkWebApp which contains specified ID (No StkWebApp exists) ... ");
		if (StkWebApp::GetStkWebAppByThreadId(1) != NULL) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}

	{
		printf("Search StkWebApp which contains specified ID (Normal case) ... ");
		int TmpIds1[3] = {11, 12, 13};
		int TmpIds2[4] = {21, 22, 23, 24};
		int TmpIds3[1] = {31};
		StkWebApp* TmpApp1 = new StkWebApp(TmpIds1, 3, L"localhost", 8081);
		StkWebApp* TmpApp2 = new StkWebApp(TmpIds2, 4, L"localhost", 8082);
		StkWebApp* TmpApp3 = new StkWebApp(TmpIds3, 1, L"localhost", 8083);
		if (StkWebApp::GetStkWebAppByThreadId(11) == NULL ||
			StkWebApp::GetStkWebAppByThreadId(13) == NULL ||
			StkWebApp::GetStkWebAppByThreadId(21) == NULL ||
			StkWebApp::GetStkWebAppByThreadId(24) == NULL ||
			StkWebApp::GetStkWebAppByThreadId(31) == NULL) {
			printf("NG\r\n");
			exit(0);
		}
		StkWebApp* AcquiredApp2 = StkWebApp::GetStkWebAppByThreadId(23);
		if (AcquiredApp2 == NULL ||
			AcquiredApp2->Contains(22) == false || AcquiredApp2->Contains(11) == true || AcquiredApp2->Contains(25) == true) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");

		printf("Search StkWebApp which contains specified ID (Abnormal case) ... ");
		if (StkWebApp::GetStkWebAppByThreadId(41) != NULL ||
			StkWebApp::GetStkWebAppByThreadId(1) != NULL) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");

		printf("Search StkWebApp which contains specified ID (Search deleted StkWebApp) ... ");
		delete TmpApp2;
		if (StkWebApp::GetStkWebAppByThreadId(11) == NULL ||
			StkWebApp::GetStkWebAppByThreadId(13) == NULL ||
			StkWebApp::GetStkWebAppByThreadId(21) != NULL ||
			StkWebApp::GetStkWebAppByThreadId(24) != NULL ||
			StkWebApp::GetStkWebAppByThreadId(31) == NULL) {
			printf("NG\r\n");
			exit(0);
		}
		delete TmpApp1;
		if (StkWebApp::GetStkWebAppByThreadId(11) != NULL ||
			StkWebApp::GetStkWebAppByThreadId(13) != NULL ||
			StkWebApp::GetStkWebAppByThreadId(21) != NULL ||
			StkWebApp::GetStkWebAppByThreadId(24) != NULL ||
			StkWebApp::GetStkWebAppByThreadId(31) == NULL) {
			printf("NG\r\n");
			exit(0);
		}
		delete TmpApp3;
		if (StkWebApp::GetStkWebAppByThreadId(11) != NULL ||
			StkWebApp::GetStkWebAppByThreadId(13) != NULL ||
			StkWebApp::GetStkWebAppByThreadId(21) != NULL ||
			StkWebApp::GetStkWebAppByThreadId(24) != NULL ||
			StkWebApp::GetStkWebAppByThreadId(31) != NULL) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
}

void AddDeleteReqHandlerTest()
{
	printf("Add / Delete ReqHandler ... ");
	int TmpIds1[3] = {11, 12, 13};
	int TmpIds2[3] = {21, 22, 23};
	StkWebApp* TmpApp1 = new StkWebApp(TmpIds1, 3, L"localhost", 8081);
	StkWebApp* TmpApp2 = new StkWebApp(TmpIds2, 3, L"localhost", 8082);

	StkWebAppTest1* Test1Hndl = new StkWebAppTest1();
	int Add1 = TmpApp1->AddReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"aaa", (StkWebAppExec*)Test1Hndl);

	StkWebAppTest2* Test2Hndl = new StkWebAppTest2();
	int Add2 = TmpApp1->AddReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"aaa", (StkWebAppExec*)Test2Hndl);

	StkWebAppTest3* Test3Hndl = new StkWebAppTest3();
	int Add3 = TmpApp2->AddReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"aaa", (StkWebAppExec*)Test3Hndl);

	int Del1 = TmpApp1->DeleteReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"aaa");
	int Del2 = TmpApp1->DeleteReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"aaa");
	delete Test2Hndl;
	int Del3 = TmpApp2->DeleteReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"aaa");

	delete TmpApp1;
	delete TmpApp2;

	if (Add1 != 1 || Add2 != -1 || Add3 != 1 || Del1 != 0 || Del2 != -1 || Del3 != 0) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");
}

int main(int Argc, char* Argv[])
{
	AddDeleteStkWebAppTest();
	AddDeleteReqHandlerTest();
	ReqResTest4();
	ReqResTest3();
	ReqResTest2();
	ReqResTest1(true);
	ReqResTest1(false);

	return 0;
}
