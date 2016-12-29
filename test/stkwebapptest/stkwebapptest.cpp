#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "..\..\src\stkthread\stkthread.h"
#include "..\..\src\stkwebapp\StkObjectConverter.h"
#include "..\..\src\commonfunc\StkObject.h"
#include "..\..\src\stksocket\stksocket.h"

StkObjectConverter* Soc;

int ElemStkThreadMain(int Id)
{
	int XmlJsonType;
	StkObject* StkObj = Soc->RecvRequest(Id, &XmlJsonType);
	if (StkObj == NULL) {
	} else {
		printf("Hello %d = StkObject\r\n", Id);
		std::wstring Xml1;
		std::wstring Json1;
		StkObj->ToXml(&Xml1);
		StkObj->ToJson(&Json1);
		wprintf(_T("%s\r\n"), Xml1.c_str());
		wprintf(_T("%s\r\n"), Json1.c_str());
	}
	return 0;
}

int TestData1(int Id)
{
	char *Dat1 = "<Hello><First>Shinya</First><Middle>Tsunemi</Middle><Last>Takeuchi</Last></Hello>";
	StkSocket_Connect(Id);
	int Ret1 = StkSocket_Send(Id, Id, (BYTE*)Dat1, strlen(Dat1) + 1);
	StkSocket_Disconnect(Id, Id, TRUE);
	return 0;
}

int TestData2(int Id)
{
	char *Dat2 = "<Aaa><Bbb><Ccc Name=\"Spring\"><Ddd>Haru</Ddd></Ccc><Ccc Name=\"Summer\"><Ddd>Natsu</Ddd></Ccc></Bbb><Bbb><Ccc Name=\"Fall\"><Ddd>Aki</Ddd></Ccc><Ccc Name=\"Winter\"><Ddd>Fuyu</Ddd></Ccc></Bbb></Aaa>";
	StkSocket_Connect(Id);
	int Ret2 = StkSocket_Send(Id, Id, (BYTE*)Dat2, strlen(Dat2) + 1);
	StkSocket_Disconnect(Id, Id, TRUE);
	return 0;
}

int main(int Argc, char* Argv[])
{
	int Ids[3] = {3, 7, 12};
	TCHAR Name[MAX_LENGTH_OF_STKTHREAD_NAME];
	TCHAR Desc[MAX_LENGTH_OF_STKTHREAD_DESCRIPTION];
	for (int Loop = 0; Loop < 3; Loop++) {
		wsprintf(Name, _T("Name-%d"), Loop);
		wsprintf(Desc, _T("Description-%d"), Loop);
		AddStkThread(Ids[Loop], Name, Desc, NULL, NULL, ElemStkThreadMain, NULL, NULL);
	}

	Soc = new StkObjectConverter(Ids, 3, _T("localhost"), 8080);
	StkSocket_AddInfo(0, SOCK_STREAM, STKSOCKET_ACTIONTYPE_SENDER, _T("localhost"), 8080);



	////////// Main logic starts
	StartAllOfStkThreads();
	while (GetNumOfRunStkThread() != GetNumOfStkThread()) {
		Sleep(100);
	}
	printf("Wait starts\r\n");
	Sleep(1000);
	TestData1(0);
	Sleep(1000);
	TestData2(0);
	Sleep(3000);
	StopAllOfStkThreads();
	while (GetNumOfRunStkThread() != 0) {
		Sleep(100);
	}
	printf("Wait ends\r\n");
	////////// Main logic ends



	Soc->AllClose(Ids, 3);
	delete Soc;

	for (int Loop = 0; Loop < 3; Loop++) {
		DeleteStkThread(Ids[Loop]);
	}
	return 0;
}
