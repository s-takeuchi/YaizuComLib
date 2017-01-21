#include <windows.h>
#include <tchar.h>
#include <shlwapi.h>
#include <stdio.h>
#include <Psapi.h>
#include "..\..\src\stkthread\stkthread.h"
#include "..\..\src\commonfunc\StkObject.h"
#include "..\..\src\stksocket\stksocket.h"
#include "..\..\src\stkwebapp\StkWebApp.h"
#include "StkWebAppTest1.h"
#include "StkWebAppTest2.h"
#include "StkWebAppTest3.h"

BOOL SendTestDataFailed = FALSE;
int SendTestDataCount = 0;

int GetUsedMemorySizeOfCurrentProcess()
{
	DWORD dwProcessID = GetCurrentProcessId();
	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc = { 0 };

	long Size;
	if ((hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessID)) != NULL) {
		if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
			Size = pmc.WorkingSetSize;
		}
	}
	CloseHandle(hProcess);
	return Size;
}

TCHAR* FindNewLine(TCHAR* Dat)
{
	TCHAR* Ret;
	Ret = StrStr(Dat, _T("\r\n\r\n"));
	if (Ret != 0) {
		return Ret + 4;
	}
	Ret = StrStr(Dat, _T("\n\r\n\r"));
	if (Ret != 0) {
		return Ret + 4;
	}
	Ret = StrStr(Dat, _T("\n\n"));
	if (Ret != 0) {
		return Ret + 2;
	}
	return NULL;
}

BOOL CompObjs(BYTE* Dat, BYTE* RecvDat)
{
	BOOL Ret = TRUE;
	int ErrCode;
	TCHAR SendDatW[4096];
	wsprintf(SendDatW, _T("%S"), Dat);
	TCHAR RecvDatW[4096];
	wsprintf(RecvDatW, _T("%S"), RecvDat);
	TCHAR* Skip = FindNewLine(RecvDatW);
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
		Ret = FALSE;
	}
	if (SendObj != NULL) {
		delete SendObj;
	}
	if (RecvObj != NULL) {
		delete RecvObj;
	}
	return Ret;
}

BOOL SendTestData(int Id, char* Dat)
{
	BYTE RecvDat[4096];
	StkSocket_Connect(Id);

	char TmpHeader[256];
	sprintf_s(TmpHeader, 256, "POST / HTTP/1.1\nContent-Length: %d\nContent-Type: Content-Type: application/xml\n\n", strlen(Dat) + 1);

	if (StkSocket_Send(Id, Id, (BYTE*)TmpHeader, strlen((char*)TmpHeader)) <= 0) {
		return FALSE;
	}
	if (StkSocket_Send(Id, Id, (BYTE*)Dat, strlen(Dat) + 1) <= 0) {
		return FALSE;
	}
	int RetR;
	for (int Loop = 0; Loop < 10; Loop++) {
		RetR = StkSocket_Receive(Id, Id, RecvDat, 4096, 9999998, NULL, -1, FALSE);
		if (RetR > 0) {
			break;
		}
		Sleep(100);
	}
	StkSocket_Disconnect(Id, Id, TRUE);
	if (RetR <= 0) {
		return FALSE;
	}
	return CompObjs((BYTE*)Dat, (BYTE*)RecvDat);
}

int ElemStkThreadMainSend(int Id)
{
	BOOL Ret;
	if (Id % 3 == 0) {
		Ret = SendTestData(Id, "<Hello><First>Shinya</First><Middle>Tsunemi</Middle><Last>Takeuchi</Last></Hello>");
	} else if (Id % 3 == 1) {
		Ret = SendTestData(Id, "<Aaa><Bbb><Ccc Name=\"Spring\"><Ddd>Haru</Ddd></Ccc><Ccc Name=\"Summer\"><Ddd>Natsu</Ddd></Ccc></Bbb><Bbb><Ccc Name=\"Fall\"><Ddd>Aki</Ddd></Ccc><Ccc Name=\"Winter\"><Ddd>Fuyu</Ddd></Ccc></Bbb></Aaa>");
	} else if (Id % 3 == 2) {
		Ret = SendTestData(Id, "\"Xxx\" : { \"Aaa\" : { \"Bbb\" : \"This is a test.\", \"Ccc\" : 123, \"Ddd\" : { \"D1\" : 0, \"D2\" : {\"D3a\" : \"test\"}, \"D3\" : 2}, \"Eee\" : 0.5 }}");
	}
	if (Ret == FALSE) {
		SendTestDataFailed = TRUE;
	}
	SendTestDataCount++;

	return 0;
}

void ReqResTest1()
{
	printf("StkWebAppTest1 ");

	int Ids[3] = {11, 12, 13};
	int SendIds[3] = {31, 32, 33};
	TCHAR Name[MAX_LENGTH_OF_STKTHREAD_NAME];
	TCHAR Desc[MAX_LENGTH_OF_STKTHREAD_DESCRIPTION];

	for (int Loop = 0; Loop < 3; Loop++) {
		wsprintf(Name, _T("Send-%d"), Loop);
		wsprintf(Desc, _T("Description-%d"), Loop);
		AddStkThread(SendIds[Loop], Name, Desc, NULL, NULL, ElemStkThreadMainSend, NULL, NULL);
		StkSocket_AddInfo(SendIds[Loop], SOCK_STREAM, STKSOCKET_ACTIONTYPE_SENDER, _T("localhost"), 8080);
	}

	StkWebApp* Soc = new StkWebApp(Ids, 3, _T("localhost"), 8080);

	int ErrorCode;
	StkObject* Test1Req = StkObject::CreateObjectFromXml(_T("<Ddd>Natsu</Ddd>"), &ErrorCode);
	StkWebAppTest1* Test1Hndl = new StkWebAppTest1();
	int Add1 = Soc->AddReqHandler(Test1Req, (StkWebAppExec*)Test1Hndl);
	StkObject* Test2Req = StkObject::CreateObjectFromXml(_T("<First>Shinya</First>"), &ErrorCode);
	StkWebAppTest2* Test2Hndl = new StkWebAppTest2();
	int Add2 = Soc->AddReqHandler(Test2Req, (StkWebAppExec*)Test2Hndl);
	StkObject* Test3Req = new StkObject(_T("D3a"), _T("test"));
	StkWebAppTest3* Test3Hndl = new StkWebAppTest3();
	int Add3 = Soc->AddReqHandler(Test3Req, (StkWebAppExec*)Test3Hndl);

	SendTestDataCount = 0;
	////////// Main logic starts
	StartSpecifiedStkThreads(SendIds, 3);
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
	StopSpecifiedStkThreads(SendIds, 3);
	while (GetNumOfRunStkThread() != 3) {
		Sleep(100);
	}
	////////// Main logic ends

	int Del1 = Soc->DeleteReqHandler(Test1Req);
	int Del2 = Soc->DeleteReqHandler(Test2Req);
	int Del3 = Soc->DeleteReqHandler(Test3Req);

	delete Soc;

	for (int Loop = 0; Loop < 3; Loop++) {
		DeleteStkThread(SendIds[Loop]);
	}

	if (SendTestDataFailed == TRUE) {
		printf("... NG\r\n");
		exit(0);
	} else if (MemChk[0] < MemChk[1] && MemChk[1] < MemChk[2] && MemChk[2] < MemChk[3] &&
				MemChk[3] < MemChk[4] && MemChk[4] < MemChk[5]) {
		printf("... NG\r\n");
		exit(0);
	} else if (Add1 != 1 || Add2 != 2 || Add3 != 3 || Del1 != 2 || Del2 != 1 || Del3 != 0) {
		printf("... NG\r\n");
		exit(0);
	}else {
		printf("... OK\r\n");
	}
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
		StkWebApp* TmpApp1 = new StkWebApp(TmpIds1, 3, _T("localhost"), 8081);
		StkWebApp* TmpApp2 = new StkWebApp(TmpIds2, 4, _T("localhost"), 8082);
		StkWebApp* TmpApp3 = new StkWebApp(TmpIds3, 1, _T("localhost"), 8083);
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
			AcquiredApp2->Contains(22) == FALSE || AcquiredApp2->Contains(11) == TRUE || AcquiredApp2->Contains(25) == TRUE) {
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
	StkWebApp* TmpApp1 = new StkWebApp(TmpIds1, 3, _T("localhost"), 8081);
	StkWebApp* TmpApp2 = new StkWebApp(TmpIds2, 3, _T("localhost"), 8082);

	StkObject* Test1Req = new StkObject(_T("Test"), _T("Test"));
	StkWebAppTest1* Test1Hndl = new StkWebAppTest1();
	int Add1 = TmpApp1->AddReqHandler(Test1Req, (StkWebAppExec*)Test1Hndl);

	StkObject* Test2Req = new StkObject(_T("Test"), _T("Test"));
	StkWebAppTest2* Test2Hndl = new StkWebAppTest2();
	int Add2 = TmpApp1->AddReqHandler(Test2Req, (StkWebAppExec*)Test2Hndl);

	StkObject* Test3Req = new StkObject(_T("Test"), _T("Test"));
	StkWebAppTest3* Test3Hndl = new StkWebAppTest3();
	int Add3 = TmpApp2->AddReqHandler(Test3Req, (StkWebAppExec*)Test3Hndl);

	int Del1 = TmpApp1->DeleteReqHandler(Test1Req);
	int Del2 = TmpApp1->DeleteReqHandler(Test2Req);
	delete Test2Req;
	delete Test2Hndl;
	int Del3 = TmpApp2->DeleteReqHandler(Test3Req);

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
	ReqResTest1();

	return 0;
}
