#include <windows.h>
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
		printf("[NG]\r\n");
		return -1;
	}
	printf("[OK]\r\n");
	return 0;
}

void StkObjectTest()
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
	Elem1->ToXml(2);

	MemoryLeakChecking(Elem1);

	StkObject* NewObj1 = Elem1->Clone();
	StkObject* NewObj2 = Elem1->Clone();
	delete Elem1;

	NewObj1->ToXml(2);
	NewObj2->ToXml(2);

	delete NewObj1;
	delete NewObj2;


}
