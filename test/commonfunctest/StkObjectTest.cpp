#include <windows.h>
#include <string>
#include <tchar.h>
#include <stdio.h>
#include <Psapi.h>

#include "..\..\src\commonfunc\StkObject.h"

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

int MemoryLeakChecking(StkObject* PassedObj)
{
	printf("Checks memory leak...");
	long MaxMem[30];
	for (int CreationLoop = 0; CreationLoop < 30; CreationLoop++) {
		for (int Loop = 0; Loop < 1000; Loop++) {
			StkObject* NewObj = PassedObj->Clone();
			delete NewObj;
		}
		MaxMem[CreationLoop] = GetUsedMemorySizeOfCurrentProcess();
	}
	if (MaxMem[0] < MaxMem[3] &&
		MaxMem[3] < MaxMem[6] &&
		MaxMem[6] < MaxMem[9] &&
		MaxMem[9] < MaxMem[12] &&
		MaxMem[12] < MaxMem[15] &&
		MaxMem[15] < MaxMem[18] &&
		MaxMem[18] < MaxMem[21] &&
		MaxMem[21] < MaxMem[24] &&
		MaxMem[24] < MaxMem[27]) {
		printf("NG\r\n");
		return -1;
	}
	printf("OK\r\n");
	return 0;
}

StkObject* GeneralTestCase1()
{
	StkObject* Elem1 = new StkObject(_T("SoftwareCompany"));

	StkObject* Elem2 = new StkObject(_T("Product"));
	Elem2->AppendAttribute(new StkObject(_T("Name"), _T("Xxxxx paint")));
	Elem2->AppendAttribute(new StkObject(_T("Price"), _T("11000")));

	StkObject* Elem3 = new StkObject(_T("Product"));
	Elem3->AppendAttribute(new StkObject(_T("Name"), _T("Aaaaa 3D")));
	Elem3->AppendAttribute(new StkObject(_T("Price"), _T("39000")));

	StkObject* Elem4 = new StkObject(_T("Product"));

	StkObject* Elem5 = new StkObject(_T("License"));
	Elem5->AppendAttribute(new StkObject(_T("Cap"), 322.44f));
	Elem2->AppendChildElement(Elem5);

	for (int Loop = 0; Loop < 20; Loop++) {
		StkObject* Elem6 = new StkObject(_T("Product"));
		Elem6->AppendAttribute(new StkObject(_T("Name"), _T("qwer")));
		Elem6->AppendAttribute(new StkObject(_T("Price"), 10000));
		Elem1->AppendChildElement(Elem6);
	}

	Elem1->AppendAttribute(new StkObject(_T("CompanyName"), _T("TakeuchiSoft")));
	Elem1->AppendAttribute(new StkObject(_T("Employee"), _T("110")));
	Elem1->AppendAttribute(new StkObject(_T("Address"), _T("Shizuoka Pref. Yaizu city")));
	Elem1->AppendChildElement(Elem2);
	Elem1->AppendChildElement(Elem3);
	Elem1->AppendChildElement(Elem4);

	int Len1 = Elem1->GetFirstChildElement()->GetArrayLength();
	printf("GeneralTestCase1#Len of top level element's child = %d...", Len1);
	if (Len1 == 23) {
		printf("OK\r\n");
	} else {
		printf("NG\r\n");
		exit(0);
	}
	int Len1_1 = Elem3->GetArrayLength();
	printf("GeneralTestCase1#Len of top level element's 2nd child = %d...", Len1_1);
	if (Len1_1 == 2) {
		printf("OK\r\n");
	} else {
		printf("NG\r\n");
		exit(0);
	}
	int Len2 = Elem1->GetArrayLength();
	printf("GeneralTestCase1#Len of top level element = %d...", Len2);
	if (Len2 == 1) {
		printf("OK\r\n");
	} else {
		printf("NG\r\n");
		exit(0);
	}
	int ElemCount = Elem1->GetChildElementCount();
	printf("GeneralTestCase1#ElemCount = %d...", ElemCount);
	if (ElemCount == Len1) {
		printf("OK\r\n");
	} else {
		printf("NG\r\n");
		exit(0);
	}
	int AttrCount = Elem1->GetAttributeCount();
	printf("GeneralTestCase1#AttrCount = %d...", AttrCount);
	if (AttrCount == 3) {
		printf("OK\r\n");
	} else {
		printf("NG\r\n");
		exit(0);
	}
	int Elem4ElemCnt = Elem4->GetChildElementCount();
	int Elem4AttrCnt = Elem4->GetAttributeCount();
	printf("GeneralTestCase1#SingleElement Count...", AttrCount);
	if (Elem4ElemCnt == 0 && Elem4AttrCnt == 0) {
		printf("OK\r\n");
	} else {
		printf("NG\r\n");
		exit(0);
	}

	return Elem1;
}

void CloneTest(StkObject* Obj)
{
	StkObject* NewObj1 = Obj->Clone();
	StkObject* NewObj2 = Obj->Clone();

	std::wstring Msg1;
	std::wstring Msg2;
	NewObj1->ToXml(&Msg1);
	NewObj2->ToXml(&Msg2);

	std::wstring OrgMsg;
	Obj->ToXml(&OrgMsg);

	printf("Clone Test#Validation of two clone's string...");
	if (lstrcmp(Msg1.c_str(), Msg2.c_str()) == 0 && lstrcmp(Msg1.c_str(), OrgMsg.c_str()) == 0) {
		printf("OK\r\n");
	} else {
		printf("NG\r\n");
		exit(0);
	}

	delete NewObj1;
	delete NewObj2;
}

void StkObjectTest()
{
	printf("StkObjectTest started.\r\n");

	StkObject* Elem1 = GeneralTestCase1();
	MemoryLeakChecking(Elem1);
	CloneTest(Elem1);
	delete Elem1;

	printf("StkObjectTest completed.\r\n\r\n\r\n");

}
