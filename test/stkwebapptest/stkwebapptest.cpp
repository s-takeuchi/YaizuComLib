#include "../../src/stkpl/StkPl.h"
#include "../../src/stkthread/stkthread.h"
#include "../../src/commonfunc/StkObject.h"
#include "../../src/commonfunc/msgproc.h"
#include "../../src/stksocket/stksocket.h"
#include "../../src/stkwebapp/StkWebApp.h"
#include "../../src/stkwebapp/StkWebAppSend.h"
#include "../../src/stkwebapp/StkWebAppExec.h"
#include "StkWebAppTest1.h"
#include "StkWebAppTest2.h"
#include "StkWebAppTest3.h"
#include "StkWebAppTest4.h"
#include "StkWebAppTest5.h"

#define THREADNUM 10

int SendTestDataFailed = 0;
int SendTestDataCount = 0;

const wchar_t* FindNewLine(wchar_t* Dat)
{
	const wchar_t* Ret;
	Ret = StkPlWcsStr(Dat, L"\r\n\r\n");
	if (Ret != 0) {
		return Ret + 4;
	}
	Ret = StkPlWcsStr(Dat, L"\n\r\n\r");
	if (Ret != 0) {
		return Ret + 4;
	}
	Ret = StkPlWcsStr(Dat, L"\n\n");
	if (Ret != 0) {
		return Ret + 2;
	}
	return NULL;
}

bool CompObjs(unsigned char* Dat, unsigned char* RecvDat)
{
	bool Ret = true;
	int ErrCode;
	wchar_t *SendDatW = StkPlCreateWideCharFromUtf8((char*)Dat);
	wchar_t *RecvDatW = StkPlCreateWideCharFromUtf8((char*)RecvDat);
	const wchar_t* Skip = FindNewLine(RecvDatW);
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
	delete SendDatW;
	delete RecvDatW;
	return Ret;
}

StkObject* MakeLargeTestData(const wchar_t Name[64], int Width, int Height, int CurrentLevel = 0)
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
			StkPlSwPrintf(Buf, 25, L"Elem_%d:%d", CurrentLevel, Loop);
			Obj->AppendChildElement(MakeLargeTestData(Buf, Width, Height, CurrentLevel + 1));
		}
	}
	return Obj;
}

int SendTestData(int Id, const char* Dat)
{
	unsigned char RecvDat[1000000];
	for (int Loop = 0; Loop < 50 && StkSocket_Connect(Id) != 0; Loop++) {
		StkPlSleepMs(100);
	}

	char TmpHeader[256];
	char UrlPath[StkWebAppExec::URL_PATH_LENGTH];
	if (Id % 3 == 0) {
		StkPlStrCpy(UrlPath, StkWebAppExec::URL_PATH_LENGTH, "/aaa/bbb/");
	} else if (Id % 3 == 1) {
		StkPlStrCpy(UrlPath, StkWebAppExec::URL_PATH_LENGTH, "/aaa/ccc/");
	} else {
		StkPlStrCpy(UrlPath, StkWebAppExec::URL_PATH_LENGTH, "/aaa/ddd/");
	}
	StkPlSPrintf(TmpHeader, 256, "POST %s HTTP/1.1\nContent-Length: %d\nAccept-Language: ja-JP;q=0.5,en-US;q=0.3,en-GB;q=0.2\nContent-Type: application/json\n\n", UrlPath, StkPlStrLen(Dat) + 1);

	if (StkSocket_Send(Id, Id, (unsigned char*)TmpHeader, (int)StkPlStrLen((char*)TmpHeader)) <= 0) {
		return -1;
	}
	if (StkSocket_Send(Id, Id, (unsigned char*)Dat, (int)StkPlStrLen(Dat) + 1) <= 0) {
		return -2;
	}
	int RetR;
	while (true) {
		RetR = StkSocket_Receive(Id, Id, RecvDat, 1000000, STKSOCKET_RECV_FINISHCOND_CONTENTLENGTH, 20000, NULL, -1);
		if (RetR > 0) {
			RecvDat[RetR] = '\0';
			break;
		}
		if (RetR <= 0) {
			StkPlPrintf("... NG (Ret=%d)\n", RetR);
			StkPlExit(-1);
		}
	}
	StkSocket_Disconnect(Id, Id, true);
	if (RetR < 0) {
		return -3;
	}
	if (RetR == 0) {
		return 1;
	}
	if (RetR > 0 && StkPlStrStr((char*)RecvDat, "200 OK") == 0) {
		return -4;
	} else {
		if (CompObjs((unsigned char*)Dat, (unsigned char*)RecvDat)) {
			return 2;
		} else {
			return -5;
		}
	}
}

int SendTestData2(int Id, const char* Method, const char* Url, const char* Dat, const char* ContType, int* ErrorCode, wchar_t Header[1024] = NULL, int ContLen = -1)
{
	char Tmp[4096];
	unsigned char RecvDat[8192];
	*ErrorCode = -1;

	int RetC = StkSocket_Connect(Id);
	int RetS = 0;

	if (Method == NULL && Url == NULL) {
		StkPlSPrintf(Tmp, 4096, "%s", Dat);
	} else if (ContLen == -1) {
		StkPlSPrintf(Tmp, 256, "%s %s HTTP/1.1\nContent-Length: %d\nContent-Type: %s\n\n%s", Method, Url, StkPlStrLen(Dat), ContType, Dat);
	} else {
		StkPlSPrintf(Tmp, 256, "%s %s HTTP/1.1\nContent-Length: %d\nContent-Type: %s\n\n%s", Method, Url, ContLen, ContType, Dat);
	}
	if ((RetS = StkSocket_Send(Id, Id, (unsigned char*)Tmp, (int)StkPlStrLen((char*)Tmp))) <= 0) {
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
	wchar_t* RecvDatW = StkPlCreateWideCharFromUtf8((char*)RecvDat);

	if (Header != NULL) {
		StkPlWcsNCpy(Header, 1024, RecvDatW, 1024 - 1);
		Header[1023] = L'\0';
	}

	// Skip HTTP header
	const wchar_t* DatPtr = StkPlWcsStr(RecvDatW, L"\n\n");
	if (DatPtr == NULL) {
		DatPtr = StkPlWcsStr(RecvDatW, L"\r\n\r\n");
	}
	if (DatPtr == NULL) {
		DatPtr = StkPlWcsStr(RecvDatW, L"\n\r\n\r");
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
			if (StkPlWcsCmp(TmpObj->GetName(), L"Code") == 0) {
				if (XmlJsonType == 1) {
					ErrorCodeStr = TmpObj->GetStringValue();
					*ErrorCode = StkPlWcsToL(ErrorCodeStr);
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
	const char* ValStr = StkPlStrStr((char*)RecvDat, "HTTP/1.1");
	ValStr += 9;
	int Val;
	StkPlSScanf(ValStr, "%d", &Val);
	return Val;
}

int ElemStkThreadMainSend(int Id)
{
	if (Id % 3 == 0) {
		SendTestDataFailed = SendTestData(Id, "\"aaa\" : {\"Hello\" : { \"FirstName\" : \"Shinya\", \"Middle\" : \"Tsunemi\", \"Last\" : \"Takeuchi\" }, \"Bye\" : \"Bye\"}");
	} else if (Id % 3 == 1) {
		SendTestDataFailed = SendTestData(Id, "\"Yyy\" : {\"Xxx\" : [{\"Aaa\" : 123, \"Bbb\" : 456, \"Ccc\":789},{\"Aaa\" : [333, 222, 111]}]}");
	} else if (Id % 3 == 2) {
		SendTestDataFailed = SendTestData(Id, "\"Xxx\" : { \"Aaa\" : { \"Bbb\" : \"This is a test.\", \"Ccc\" : 123, \"Ddd\" : { \"D1\" : 0, \"D2\" : {\"D3a\" : \"test\"}, \"D3\" : 2}, \"Eee\" : 0.5 }}");
	}
	if (SendTestDataFailed < 0) {
		StkPlPrintf("... NG(code=%d)\n", SendTestDataFailed);
		StkPlExit(-1);
	}
	SendTestDataCount++;

	return 0;
}

int ElemStkThreadMainSend2(int Id)
{
	int ErrorCode;

	StkPlPrintf("StkWebAppTest2:Invalid request 1 == 400");
	if (SendTestData2(Id, NULL, NULL, "dummy", "", &ErrorCode) != 400 || ErrorCode != 1005) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	StkPlPrintf("StkWebAppTest2:Invalid request 2 == 400");
	if (SendTestData2(Id, NULL, NULL, "Aaaaaaaaaaaaaaaaaaaaaaaaa\r\n\r\n Bbbbbbbbbbbbbb HTTP Cccccccccccccccccccccccccccc", "", &ErrorCode) != 400 || ErrorCode != 1005) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	StkPlPrintf("StkWebAppTest2:Invalid request 3 (invalid method) == 400");
	if (SendTestData2(Id, NULL, NULL, "ZZZ /aaa/ HTTP/1.1\r\n\r\naaa", "", &ErrorCode) != 400 || ErrorCode != 1005) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	StkPlPrintf("StkWebAppTest2:Invalid request 4 (large method) == 400");
	char TmpBuf[4096] = "";
	for (int Loop = 0; Loop < 3000; Loop++) {
		TmpBuf[Loop] = 'a';
	}
	TmpBuf[3000] = '\r';
	TmpBuf[3001] = '\n';
	TmpBuf[3002] = '\r';
	TmpBuf[3003] = '\n';
	if (SendTestData2(Id, NULL, NULL, TmpBuf, "", &ErrorCode) != 400 || ErrorCode != 1003) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	StkPlPrintf("StkWebAppTest2:GET /aaa/ [{ \"AAA\":123 } with No Content-Type] == 404");
	if (SendTestData2(Id, NULL, NULL, "GET /aaa/ HTTP/1.1\r\n\r\n{ \"AAA\":123 }\n", "", &ErrorCode) != 404 || ErrorCode != 1001) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	StkPlPrintf("StkWebAppTest2:GET /abc/ [{ \"AAA\":123 }] == 404");
	if (SendTestData2(Id, "GET", "/abc/", "{ \"AAA\":123 }\n", "application/json", &ErrorCode) != 404 || ErrorCode != 1001) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	StkPlPrintf("StkWebAppTest2:GET /abc/ [\"aaa\" : {\"bbb\" : \"xxx\"}] == 404");
	if (SendTestData2(Id, "GET", "/abc/", "\"aaa\" : {\"bbb\" : \"xxx\"}\n", "application/json", &ErrorCode) != 404 || ErrorCode != 1001) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	StkPlPrintf("StkWebAppTest2:GET /abc/ [{ \"AAA\":123 } with application/xml] == 400");
	if (SendTestData2(Id, "GET", "/abc/", "{ \"AAA\":123 }\n", "application/xml", &ErrorCode) != 400 || ErrorCode != 1002) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	StkPlPrintf("StkWebAppTest2:GET /abc/ [XYZ] == 400");
	if (SendTestData2(Id, "GET", "/abc/", "XYZ\n", "application/json", &ErrorCode) != 400 || ErrorCode != 1002) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	StkPlPrintf("StkWebAppTest2:GET /abc/ [<Aaa><Bbb/></Aaa>] == 400");
	if (SendTestData2(Id, "GET", "/abc/", "<Aaa><Bbb/></Aaa>\n", "application/xml", &ErrorCode) != 400 || ErrorCode != 1002) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	// Timeout detection start
	{
		wchar_t Header[1024];

		int Ret = 0;
		long long StartTime = 0;
		long long EndTime = 0;

		StkPlPrintf("StkWebAppTest2:Invalid Content-Length (Timeout default 3000 milliseconds)");
		StartTime = StkPlGetTickCount();
		if ((Ret = SendTestData2(Id, "POST", "/test/", "{ \"Test\" : \"1\" }\n", "application/json", &ErrorCode, Header, 1024)) != 404 || ErrorCode != 1001) {
			StkPlPrintf("... NG\n");
			StkPlExit(-1);
		}
		EndTime = StkPlGetTickCount();
		if ((EndTime - StartTime) < 3000 || (EndTime - StartTime) > 4000) {
			StkPlPrintf("... NG\n");
			StkPlExit(-1);
		}
		StkPlPrintf("... OK\n");
	}
	// Timeout detection end

	StkPlPrintf("StkWebAppTest2:POST /service/ [{ \"Operation\" : \"Start\" }] == 400");
	if (SendTestData2(Id, "POST", "/service/", "{ \"Operation\" : \"Start\" }\n", "application/json", &ErrorCode) != 400 || ErrorCode != 1004) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	StkPlPrintf("StkWebAppTest2:POST /service/ [{ \"Stop\" : \"YES\" }] == 400");
	if (SendTestData2(Id, "POST", "/service/", "{ \"Stop\" : \"YES\" }\n", "application/json", &ErrorCode) != 400 || ErrorCode != 1004) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	StkPlPrintf("StkWebAppTest2:POST /service/ [{ \"Operation\" : \"Stop\" }] == 202");
	if (SendTestData2(Id, "POST", "/service/", "{ \"Operation\" : \"Stop\" }\n", "application/json", &ErrorCode) != 202 || ErrorCode != -1) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	StkPlSleepMs(1000);

	return 0;
}

int ElemStkThreadMainSend3(int Id)
{
	int ErrorCode;
	wchar_t Header[1024];

	StkPlPrintf("StkWebAppTest3:GET /aaa/100/ [{ \"AAA\":\"aaa\" }] == 100 Continue");
	if (SendTestData2(Id, "GET", "/aaa/100/", "{ \"AAA\":\"aaa\" }\n", "application/json", &ErrorCode, Header) != 100 || StkPlWcsStr(Header, L"Continue") == NULL || ErrorCode != -1) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	StkPlPrintf("StkWebAppTest3:GET /aaa/102/ [{ \"AAA\":\"aaa\" }] == 102 Processing");
	if (SendTestData2(Id, "GET", "/aaa/102/", "{ \"AAA\":\"aaa\" }\n", "application/json", &ErrorCode, Header) != 102 || StkPlWcsStr(Header, L"Processing") == NULL || ErrorCode != -1) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	StkPlPrintf("StkWebAppTest3:GET /aaa/202/ [{ \"AAA\":\"aaa\" }] == 202 Accepted");
	if (SendTestData2(Id, "GET", "/aaa/202/", "{ \"AAA\":\"aaa\" }\n", "application/json", &ErrorCode, Header) != 202 || StkPlWcsStr(Header, L"Accepted") == NULL || ErrorCode != -1) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	StkPlPrintf("StkWebAppTest3:GET /aaa/204/ [{ \"AAA\":\"aaa\" }] == 204 No Content");
	if (SendTestData2(Id, "GET", "/aaa/204/", "{ \"AAA\":\"aaa\" }\n", "application/json", &ErrorCode, Header) != 204 || StkPlWcsStr(Header, L"No Content") == NULL || ErrorCode != -1) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	StkPlPrintf("StkWebAppTest3: Check whether proper date is set in HTTP header or not.");
	const wchar_t* TmpHeader = StkPlWcsStr(Header, L"Date:");
	if (TmpHeader == 0) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	wchar_t TmpDate[32];
	wchar_t TmpWDay[32];
	int TmpDay;
	StkPlSwScanf(TmpHeader, L"%s %s %d", TmpDate, TmpWDay, &TmpDay);
	if (TmpDay < 1 || TmpDay > 31) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	StkPlPrintf("StkWebAppTest3:GET /aaa/304/ [{ \"AAA\":\"aaa\" }] == 304 Not Modified");
	if (SendTestData2(Id, "GET", "/aaa/304/", "{ \"AAA\":\"aaa\" }\n", "application/json", &ErrorCode, Header) != 304 || StkPlWcsStr(Header, L"Not Modified") == NULL || ErrorCode != -1) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	StkPlPrintf("StkWebAppTest3:GET /aaa/400/ [{ \"AAA\":\"aaa\" }] == 400 Bad Request");
	if (SendTestData2(Id, "GET", "/aaa/400/", "{ \"AAA\":\"aaa\" }\n", "application/json", &ErrorCode, Header) != 400 || StkPlWcsStr(Header, L"Bad Request") == NULL || ErrorCode != -1) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	StkPlPrintf("StkWebAppTest3:GET /aaa/401/ [{ \"AAA\":\"aaa\" }] == 401 Unauthorized");
	if (SendTestData2(Id, "GET", "/aaa/401/", "{ \"AAA\":\"aaa\" }\n", "application/json", &ErrorCode, Header) != 401 || StkPlWcsStr(Header, L"Unauthorized") == NULL || ErrorCode != -1) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	StkPlPrintf("StkWebAppTest3:GET /aaa/404/ [{ \"AAA\":\"aaa\" }] == 404 Not Found");
	if (SendTestData2(Id, "GET", "/aaa/404/", "{ \"AAA\":\"aaa\" }\n", "application/json", &ErrorCode, Header) != 404 || StkPlWcsStr(Header, L"Not Found") == NULL || ErrorCode != -1) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	StkPlPrintf("StkWebAppTest3:GET /aaa/414/ [{ \"AAA\":\"aaa\" }] == 414 URI Too Long");
	if (SendTestData2(Id, "GET", "/aaa/414/", "{ \"AAA\":\"aaa\" }\n", "application/json", &ErrorCode, Header) != 414 || StkPlWcsStr(Header, L"URI Too Long") == NULL || ErrorCode != -1) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	StkPlPrintf("StkWebAppTest3:GET /aaa/500/ [{ \"AAA\":\"aaa\" }] == 500 Internal Server Error");
	if (SendTestData2(Id, "GET", "/aaa/500/", "{ \"AAA\":\"aaa\" }\n", "application/json", &ErrorCode, Header) != 500 || StkPlWcsStr(Header, L"Internal Server Error") == NULL || ErrorCode != 1006) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	StkPlPrintf("StkWebAppTest3:GET /aaa/510/ [{ \"AAA\":\"aaa\" }] == 510 Not Extended");
	if (SendTestData2(Id, "GET", "/aaa/510/", "{ \"AAA\":\"aaa\" }\n", "application/json", &ErrorCode, Header) != 510 || StkPlWcsStr(Header, L"Not Extended") == NULL || ErrorCode != -1) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	// Timeout detection start
	{
		wchar_t Header[1024];

		int Ret = 0;
		long long StartTime = 0;
		long long EndTime = 0;

		StkPlPrintf("StkWebAppTest3:Invalid Content-Length (Timeout 5000 milliseconds)");
		StartTime = StkPlGetTickCount();
		if ((Ret = SendTestData2(Id, "GET", "/aaa/200/", "{ \"Test\" : \"1\" }\n", "application/json", &ErrorCode, Header, 1024)) != 200 || ErrorCode != -1) {
			StkPlPrintf("... NG\n");
			StkPlExit(-1);
		}
		EndTime = StkPlGetTickCount();
		if ((EndTime - StartTime) < 5000 || (EndTime - StartTime) > 6000) {
			StkPlPrintf("... NG\n");
			StkPlExit(-1);
		}
		StkPlPrintf("... OK\n");
	}
	// Timeout detection end

	StkPlPrintf("StkWebAppTest3:POST /service/ [{ \"Operation\" : \"Stop\" }] == 202");
	if (SendTestData2(Id, "POST", "/service/", "{ \"Operation\" : \"Stop\" }\n", "application/json", &ErrorCode) != 202 || ErrorCode != -1) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	StkPlSleepMs(1000);

	return 0;
}

int ElemStkThreadMainSend4(int Id)
{
	int ErrorCode;

	StkPlPrintf("StkWebAppTest4:GET /smalldata/ [{ \"AAA\":123 }] == 200 sufficient buffer allocated");
	if (SendTestData2(Id, "GET", "/smalldata/", "{ \"AAA\":123 }\n", "application/json", &ErrorCode) != 200) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	StkPlPrintf("StkWebAppTest4:POST /middledata/  Deep nest");
	if (SendTestData2(Id, "POST", "/middledata/", "{ \"aaa\":\"bbb\", \"ccc\":{\"ddd\":{\"eee\":{\"fff\":{\"ggg\":\"hhh\"}}}} }\n", "application/json", &ErrorCode) != 200) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	StkPlPrintf("StkWebAppTest4:POST /service/ [{ \"Operation\" : \"Stop\" }] == 202");
	if (SendTestData2(Id, "POST", "/service/", "{ \"Operation\" : \"Stop\" }\n", "application/json", &ErrorCode) != 202 || ErrorCode != -1) {
		StkPlPrintf("... NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("... OK\n");

	StkPlSleepMs(1000);

	return 0;
}

int ElemStkThreadMainSend5(int Id)
{
	wchar_t TestObjWstr[1000000];
	char TestObjStr[1000000];
	StkObject* TestObj = MakeLargeTestData(L"testObject", 3, 3);
	TestObj->ToJson(TestObjWstr, 1000000);
	StkPlSPrintf(TestObjStr, 1000000, "%ls", TestObjWstr);
	int l = (int)StkPlStrLen(TestObjStr);
	SendTestDataFailed = SendTestData(Id, TestObjStr);
	if (SendTestDataFailed < 0) {
		StkPlPrintf("... NG(code=%d)\n", SendTestDataFailed);
		StkPlExit(-1);
	}
	SendTestDataCount++;
	delete TestObj;

	return 0;
}

void ReqResTest1(bool LargeFlag)
{
	StkPlPrintf("StkWebAppTest1:one minute performance ");
	LargeFlag == true ? StkPlPrintf("Large ") : StkPlPrintf("Small ");

	bool SslMode = false;
	if (StkPlGetFileSize(L"./ca.crt") != size_t(-1) && StkPlGetFileSize(L"./server.crt") != size_t(-1) && StkPlGetFileSize(L"./server.key") != size_t(-1)) {
		StkPlPrintf("(SSL/TLS) ");
		SslMode = true;
	}

	int Ids[10] = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
	int SendIds[10] = {31, 32, 33, 34, 35, 36, 37, 38, 39, 40};
	wchar_t Name[MAX_LENGTH_OF_STKTHREAD_NAME];
	wchar_t Desc[MAX_LENGTH_OF_STKTHREAD_DESCRIPTION];

	for (int Loop = 0; Loop < THREADNUM; Loop++) {
		StkPlSwPrintf(Name, MAX_LENGTH_OF_STKTHREAD_NAME, L"Send-%d", Loop);
		StkPlSwPrintf(Desc, MAX_LENGTH_OF_STKTHREAD_DESCRIPTION, L"Description-%d", Loop);
		if (LargeFlag) {
			AddStkThread(SendIds[Loop], Name, Desc, NULL, NULL, ElemStkThreadMainSend5, NULL, NULL);
		} else {
			AddStkThread(SendIds[Loop], Name, Desc, NULL, NULL, ElemStkThreadMainSend, NULL, NULL);
		}
		StkSocket_AddInfo(SendIds[Loop], STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"localhost", 2080);
		if (SslMode) {
			StkSocket_SecureForSend(SendIds[Loop], "./ca.crt", NULL);
		}
	}

	StkWebApp* Soc = NULL;
	if (SslMode) {
		Soc = new StkWebApp(Ids, THREADNUM, L"localhost", 2080, "./server.key", "./server.crt");
	} else {
		Soc = new StkWebApp(Ids, THREADNUM, L"localhost", 2080);
	}

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
		StkPlSleepMs(100);
	}
	StkPlSleepMs(30000);
	int MemChk[6];
	for (int Loop = 0; Loop < 6; Loop++) {
		StkPlSleepMs(5000);
		MemChk[Loop] = StkPlGetUsedMemorySizeOfCurrentProcess();
		StkPlPrintf("%d,", MemChk[Loop]);
	}
	StkPlPrintf("(Repeat=%d)", SendTestDataCount);
	StopSpecifiedStkThreads(SendIds, THREADNUM);
	int WaitingStopCnt = 0;
	while (GetNumOfRunStkThread() != THREADNUM) {
		if (WaitingStopCnt == 50) {
			StkPlPrintf("... NG(Time up for waiting threads stop)\n");
			StkPlExit(-1);
		}
		StkPlSleepMs(400);
		WaitingStopCnt++;
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

	if (MemChk[0] < MemChk[1] && MemChk[1] < MemChk[2] && MemChk[2] < MemChk[3] &&
		MemChk[3] < MemChk[4] && MemChk[4] < MemChk[5]) {
		StkPlPrintf("... NG(2)\n");
		StkPlExit(-1);
	} else if (Add1 != 1 || Add2 != 2 || Add3 != 3 || Del1 != 2 || Del2 != 1 || Del3 != 0) {
		StkPlPrintf("... NG(3)\n");
		StkPlExit(-1);
	}else {
		StkPlPrintf("... OK\n");
	}
}

void ReqResTest2()
{
	int Ids[1] = {11};
	int SendIds[1] = {31};
	wchar_t Name[MAX_LENGTH_OF_STKTHREAD_NAME];
	wchar_t Desc[MAX_LENGTH_OF_STKTHREAD_DESCRIPTION];

	StkPlSwPrintf(Name, MAX_LENGTH_OF_STKTHREAD_NAME, L"Sender");
	StkPlSwPrintf(Desc, MAX_LENGTH_OF_STKTHREAD_DESCRIPTION, L"Description");
	AddStkThread(SendIds[0], Name, Desc, NULL, NULL, ElemStkThreadMainSend2, NULL, NULL);
	StkSocket_AddInfo(SendIds[0], STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"localhost", 2080);

	StkWebApp* Soc = new StkWebApp(Ids, 1, L"localhost", 2080);

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

	StkPlSwPrintf(Name, MAX_LENGTH_OF_STKTHREAD_NAME, L"Sender");
	StkPlSwPrintf(Desc, MAX_LENGTH_OF_STKTHREAD_DESCRIPTION, L"Description");
	AddStkThread(SendIds[0], Name, Desc, NULL, NULL, ElemStkThreadMainSend3, NULL, NULL);
	StkSocket_AddInfo(SendIds[0], STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"localhost", 2080);

	StkWebApp* Soc = new StkWebApp(Ids, 1, L"localhost", 2080);
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

	StkPlSwPrintf(Name, MAX_LENGTH_OF_STKTHREAD_NAME, L"Sender");
	StkPlSwPrintf(Desc, MAX_LENGTH_OF_STKTHREAD_DESCRIPTION, L"Description");
	AddStkThread(SendIds[0], Name, Desc, NULL, NULL, ElemStkThreadMainSend4, NULL, NULL);
	StkSocket_AddInfo(SendIds[0], STKSOCKET_TYPE_STREAM, STKSOCKET_ACTIONTYPE_SENDER, L"localhost", 2080);

	StkWebApp* Soc = new StkWebApp(Ids, 1, L"localhost", 2080);

	StkWebAppTest5* Test5aHndl = new StkWebAppTest5();
	StkWebAppTest5* Test5bHndl = new StkWebAppTest5();
	StkWebAppTest5* Test5cHndl = new StkWebAppTest5();
	int Add1 = Soc->AddReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"/bigdata/", (StkWebAppExec*)Test5aHndl);
	int Add2 = Soc->AddReqHandler(StkWebAppExec::STKWEBAPP_METHOD_POST, L"/middledata/", (StkWebAppExec*)Test5bHndl);
	int Add3 = Soc->AddReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"/smalldata/", (StkWebAppExec*)Test5cHndl);

	StartSpecifiedStkThreads(SendIds, 1);
	Soc->TheLoop();
	StopSpecifiedStkThreads(SendIds, 1);

	int Del1 = Soc->DeleteReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"/bigdata/");
	int Del2 = Soc->DeleteReqHandler(StkWebAppExec::STKWEBAPP_METHOD_POST, L"/middledata/");
	int Del3 = Soc->DeleteReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"/smalldata/");
	delete Soc;

	StkSocket_DeleteInfo(SendIds[0]);
	DeleteStkThread(SendIds[0]);
}

void AddDeleteStkWebAppTest()
{
	{
		StkPlPrintf("Search StkWebApp which contains specified ID (No StkWebApp exists) ... ");
		if (StkWebApp::GetStkWebAppByThreadId(1) != NULL) {
			StkPlPrintf("NG\n");
			StkPlExit(-1);
		}
		StkPlPrintf("OK\n");
	}

	{
		StkPlPrintf("Search StkWebApp which contains specified ID (Normal case) ... ");
		int TmpIds1[3] = {11, 12, 13};
		int TmpIds2[4] = {21, 22, 23, 24};
		int TmpIds3[1] = {31};
		StkWebApp* TmpApp1 = new StkWebApp(TmpIds1, 3, L"localhost", 2081);
		StkWebApp* TmpApp2 = new StkWebApp(TmpIds2, 4, L"localhost", 2082);
		StkWebApp* TmpApp3 = new StkWebApp(TmpIds3, 1, L"localhost", 2083);
		if (StkWebApp::GetStkWebAppByThreadId(11) == NULL ||
			StkWebApp::GetStkWebAppByThreadId(13) == NULL ||
			StkWebApp::GetStkWebAppByThreadId(21) == NULL ||
			StkWebApp::GetStkWebAppByThreadId(24) == NULL ||
			StkWebApp::GetStkWebAppByThreadId(31) == NULL) {
			StkPlPrintf("NG\n");
			StkPlExit(-1);
		}
		StkWebApp* AcquiredApp2 = StkWebApp::GetStkWebAppByThreadId(23);
		if (AcquiredApp2 == NULL ||
			AcquiredApp2->Contains(22) == false || AcquiredApp2->Contains(11) == true || AcquiredApp2->Contains(25) == true) {
			StkPlPrintf("NG\n");
			StkPlExit(-1);
		}
		StkPlPrintf("OK\n");

		StkPlPrintf("Search StkWebApp which contains specified ID (Abnormal case) ... ");
		if (StkWebApp::GetStkWebAppByThreadId(41) != NULL ||
			StkWebApp::GetStkWebAppByThreadId(1) != NULL) {
			StkPlPrintf("NG\n");
			StkPlExit(-1);
		}
		StkPlPrintf("OK\n");

		StkPlPrintf("Search StkWebApp which contains specified ID (Search deleted StkWebApp) ... ");
		delete TmpApp2;
		if (StkWebApp::GetStkWebAppByThreadId(11) == NULL ||
			StkWebApp::GetStkWebAppByThreadId(13) == NULL ||
			StkWebApp::GetStkWebAppByThreadId(21) != NULL ||
			StkWebApp::GetStkWebAppByThreadId(24) != NULL ||
			StkWebApp::GetStkWebAppByThreadId(31) == NULL) {
			StkPlPrintf("NG\n");
			StkPlExit(-1);
		}
		delete TmpApp1;
		if (StkWebApp::GetStkWebAppByThreadId(11) != NULL ||
			StkWebApp::GetStkWebAppByThreadId(13) != NULL ||
			StkWebApp::GetStkWebAppByThreadId(21) != NULL ||
			StkWebApp::GetStkWebAppByThreadId(24) != NULL ||
			StkWebApp::GetStkWebAppByThreadId(31) == NULL) {
			StkPlPrintf("NG\n");
			StkPlExit(-1);
		}
		delete TmpApp3;
		if (StkWebApp::GetStkWebAppByThreadId(11) != NULL ||
			StkWebApp::GetStkWebAppByThreadId(13) != NULL ||
			StkWebApp::GetStkWebAppByThreadId(21) != NULL ||
			StkWebApp::GetStkWebAppByThreadId(24) != NULL ||
			StkWebApp::GetStkWebAppByThreadId(31) != NULL) {
			StkPlPrintf("NG\n");
			StkPlExit(-1);
		}
		StkPlPrintf("OK\n");
	}
}

void AddDeleteReqHandlerTest()
{
	StkPlPrintf("Add / Delete ReqHandler ... ");
	int TmpIds1[3] = {11, 12, 13};
	int TmpIds2[3] = {21, 22, 23};
	StkWebApp* TmpApp1 = new StkWebApp(TmpIds1, 3, L"localhost", 2081);
	StkWebApp* TmpApp2 = new StkWebApp(TmpIds2, 3, L"localhost", 2082);

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
		StkPlPrintf("NG\n");
		StkPlExit(-1);
	}
	StkPlPrintf("OK\n");
}

int StkWebAppSendTest1_T(int Id)
{
	int ResultCode = 0;
	StkWebAppSend* Sender = new StkWebAppSend(20, L"localhost", 2081);
	StkWebAppSend* Sender2 = new StkWebAppSend(22, L"localhostx", 2081);
	StkWebAppSend* Sender3 = new StkWebAppSend(23, L"localhost", 2082);
	{
		StkPlPrintf("StkWebAppSend: Appropriate object is returned. (Normal case) ... ");
		StkObject* ReqObj = new StkObject(L"");
		ReqObj->AppendChildElement(new StkObject(L"Shizuoka", 100));
		ReqObj->AppendChildElement(new StkObject(L"Ishikawa", 200));
		ReqObj->AppendChildElement(new StkObject(L"Kanagawa", 200));
		StkObject* ResObj = Sender->SendRequestRecvResponse(StkWebAppSend::STKWEBAPP_METHOD_GET, "aaa", ReqObj, &ResultCode);
		if (ResultCode != 200 || ReqObj->Equals(ResObj) == false) {
			StkPlPrintf("NG\n");
			StkPlExit(-1);
		}
		delete ReqObj;
		delete ResObj;
		StkPlPrintf("OK\n");
	}
	{
		StkPlPrintf("StkWebAppSend: Appropriate object is returned. (Normal case) ... ");
		StkObject* ReqObj = new StkObject(L"data");
		ReqObj->AppendChildElement(new StkObject(L"xxx", L"XXX"));
		ReqObj->AppendChildElement(new StkObject(L"yyy", L"YYY"));
		ReqObj->AppendChildElement(new StkObject(L"zzz", L"ZZZ"));
		StkObject* ResObj = Sender->SendRequestRecvResponse(StkWebAppSend::STKWEBAPP_METHOD_GET, "bbb", ReqObj, &ResultCode);
		if (ResultCode != 200 || ReqObj->Equals(ResObj) == false) {
			StkPlPrintf("NG\n");
			StkPlExit(-1);
		}
		delete ReqObj;
		delete ResObj;
		StkPlPrintf("OK\n");
	}
	{
		StkPlPrintf("StkWebAppSend: No object is passed as request. (Normal case) ... ");
		StkObject* ResObj = Sender->SendRequestRecvResponse(StkWebAppSend::STKWEBAPP_METHOD_GET, "/bigdata/", NULL, &ResultCode);
		if (ResultCode != 200) {
			StkPlPrintf("NG\n");
			StkPlExit(-1);
		}
		delete ResObj;
		StkPlPrintf("OK\n");
	}
	{
		StkPlPrintf("StkWebAppSend: Undefined API is called. (Abnormal case) ... ");
		StkObject* ResObj = Sender->SendRequestRecvResponse(StkWebAppSend::STKWEBAPP_METHOD_GET, "xxx", NULL, &ResultCode);
		if (ResultCode != 404 || ResObj == NULL) {
			StkPlPrintf("NG\n");
			StkPlExit(-1);
		}
		StkObject* CodeObj = ResObj->GetFirstChildElement();
		if (CodeObj == NULL || CodeObj->GetIntValue() != 1001) {
			StkPlPrintf("NG\n");
			StkPlExit(-1);
		}
		delete ResObj;
		StkPlPrintf("OK\n");
	}
	{
		StkPlPrintf("StkWebAppSend: Incorrect host name is specified. (Abnormal case) ... ");
		StkObject* ResObj = Sender2->SendRequestRecvResponse(StkWebAppSend::STKWEBAPP_METHOD_GET, "/bigdata/", NULL, &ResultCode);
		if (ResultCode != -1 || ResObj != NULL) {
			StkPlPrintf("NG\n");
			StkPlExit(-1);
		}
		delete ResObj;
		StkPlPrintf("OK\n");
	}
	{
		StkPlPrintf("StkWebAppSend: Incorrect port number is specified. (Abnormal case) ... ");
		StkObject* ResObj = Sender3->SendRequestRecvResponse(StkWebAppSend::STKWEBAPP_METHOD_GET, "/bigdata/", NULL, &ResultCode);
		if (ResultCode != -1 || ResObj != NULL) {
			StkPlPrintf("NG\n");
			StkPlExit(-1);
		}
		delete ResObj;
		StkPlPrintf("OK\n");
	}
	{
		StkPlPrintf("StkWebAppSend: No memory leak is occurred. (Normal case) ... ");
		int MemChk[6];
		for (int Loop = 0; Loop < 16; Loop++) {
			for (int Loop2 = 0; Loop2 < 200; Loop2++) {
				StkObject* ResObj = Sender->SendRequestRecvResponse(StkWebAppSend::STKWEBAPP_METHOD_GET, "/bigdata/", NULL, &ResultCode);
				delete ResObj;
			}
		}
		for (int Loop = 0; Loop < 6; Loop++) {
			for (int Loop2 = 0; Loop2 < 200; Loop2++) {
				StkObject* ResObj = Sender->SendRequestRecvResponse(StkWebAppSend::STKWEBAPP_METHOD_GET, "/bigdata/", NULL, &ResultCode);
				if (ResultCode != 200 || ResObj == NULL) {
					StkPlPrintf("NG\n");
					StkPlExit(-1);
				}
				delete ResObj;
			}
			MemChk[Loop] = StkPlGetUsedMemorySizeOfCurrentProcess();
			StkPlPrintf("%d,", MemChk[Loop]);
		}
		if (MemChk[0] < MemChk[1] && MemChk[1] < MemChk[2] && MemChk[2] < MemChk[3] &&
			MemChk[3] < MemChk[4] && MemChk[4] < MemChk[5]) {
			StkPlPrintf(" NG\n");
			StkPlExit(-1);
		}
		StkPlPrintf(" OK\n");
	}
	{
		StkObject* ReqObj = new StkObject(L"");
		ReqObj->AppendChildElement(new StkObject(L"Operation", L"Stop"));
		StkObject* ResObj = Sender->SendRequestRecvResponse(StkWebAppSend::STKWEBAPP_METHOD_POST, "/service/", ReqObj, &ResultCode);
		delete ReqObj;
		delete ResObj;
		StkPlSleepMs(500);
	}
	delete Sender;
	delete Sender2;
	delete Sender3;
	return 0;
}

void StkWebAppSendTest1()
{
	int TmpIds1[3] = { 11, 12, 13 };
	int TmpIds_T[1] = { 14 };

	StkWebApp* TmpApp1 = new StkWebApp(TmpIds1, 3, L"localhost", 2081);

	StkWebAppTest1* Test1Hndl = new StkWebAppTest1();
	int Add1 = TmpApp1->AddReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"aaa", (StkWebAppExec*)Test1Hndl);
	StkWebAppTest2* Test2Hndl = new StkWebAppTest2();
	int Add2 = TmpApp1->AddReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"bbb", (StkWebAppExec*)Test2Hndl);
	StkWebAppTest5* Test5Hndl = new StkWebAppTest5();
	int Add5 = TmpApp1->AddReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"/bigdata/", (StkWebAppExec*)Test5Hndl);

	AddStkThread(TmpIds_T[0], L"StkWebAppSendTest1_T", L"", NULL, NULL, StkWebAppSendTest1_T, NULL, NULL);
	StartSpecifiedStkThreads(TmpIds_T, 1);
	TmpApp1->TheLoop();
	long long StartTm = 0;
	long long EndTm = 0;
	wchar_t ApiStr[3][StkWebAppExec::URL_PATH_LENGTH] = { L"", L"",  L"" };
	TmpApp1->GetStatusOfApiCall(11, &StartTm, &EndTm, ApiStr[0]);
	TmpApp1->GetStatusOfApiCall(12, &StartTm, &EndTm, ApiStr[1]);
	TmpApp1->GetStatusOfApiCall(13, &StartTm, &EndTm, ApiStr[2]);
	if (StkPlWcsCmp(ApiStr[0], L"/bigdata/") != 0 || StkPlWcsCmp(ApiStr[1], L"/bigdata/") != 0 || StkPlWcsCmp(ApiStr[2], L"/bigdata/") != 0) {
		StkPlPrintf(" NG(Inappropriate thread behavior)\n");
		StkPlExit(-1);
	}

	StopSpecifiedStkThreads(TmpIds_T, 1);
	DeleteStkThread(TmpIds_T[0]);

	int Del1 = TmpApp1->DeleteReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"aaa");
	int Del2 = TmpApp1->DeleteReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"bbb");
	int Del5 = TmpApp1->DeleteReqHandler(StkWebAppExec::STKWEBAPP_METHOD_GET, L"/bigdata/");

	delete TmpApp1;
}

int main(int Argc, char* Argv[])
{
	StkPlPrintf("Test started\n");

	StkSocket_InitSecureSetting();

	StkWebAppSendTest1();
	AddDeleteStkWebAppTest();
	AddDeleteReqHandlerTest();
	ReqResTest4();
	ReqResTest3();
	ReqResTest2();
	ReqResTest1(true);
	ReqResTest1(false);

	StkPlPrintf("Test completed\n");

	return 0;
}
