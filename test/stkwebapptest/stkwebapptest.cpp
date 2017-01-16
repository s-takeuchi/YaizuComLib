#include <windows.h>
#include <tchar.h>
#include <shlwapi.h>
#include <stdio.h>
#include <Psapi.h>
#include "..\..\src\stkthread\stkthread.h"
#include "..\..\src\stkwebapp\StkWebApp.h"
#include "..\..\src\commonfunc\StkObject.h"
#include "..\..\src\stksocket\stksocket.h"

BOOL SendTestDataFailed = FALSE;

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
	int RetS = StkSocket_Send(Id, Id, (BYTE*)Dat, strlen(Dat) + 1);
	if (RetS <= 0) {
		printf("(Failed in sending data.)");
		return FALSE;
	}
	int RetR;
	for (int Loop = 0; Loop < 10; Loop++) {
		RetR = StkSocket_Receive(Id, Id, RecvDat, 4096, 100, NULL, -1, FALSE);
		if (RetR > 0) {
			break;
		}
		Sleep(100);
	}
	StkSocket_Disconnect(Id, Id, TRUE);
	if (RetR <= 0) {
		return FALSE;
	}
	BOOL RetC = CompObjs((BYTE*)Dat, (BYTE*)RecvDat);
	if (RetC == FALSE) {
		printf("(Failed in comparing data.)");
	}
	return RetC;
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

	/////
	printf("Thread ID=%d, UsedMem=%d\r\n", Id, GetUsedMemorySizeOfCurrentProcess());
	/////

	return 0;
}

int main(int Argc, char* Argv[])
{
	printf("Test StkWebApp ... ");
	printf("\r\n"); // To be deleted

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

	////////// Main logic starts
	StartSpecifiedStkThreads(SendIds, 3);
	while (GetNumOfRunStkThread() != GetNumOfStkThread()) {
		Sleep(100);
	}
	Sleep(10000);
	StopSpecifiedStkThreads(SendIds, 3);
	while (GetNumOfRunStkThread() != 3) {
		Sleep(100);
	}
	////////// Main logic ends

	delete Soc;

	for (int Loop = 0; Loop < 3; Loop++) {
		DeleteStkThread(SendIds[Loop]);
	}

	if (SendTestDataFailed == TRUE) {
		printf("NG\r\n");
	} else {
		printf("OK\r\n");
	}

	return 0;
}
