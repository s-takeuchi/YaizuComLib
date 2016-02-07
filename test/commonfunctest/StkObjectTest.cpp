#include <windows.h>
#include <string>
#include <tchar.h>
#include <stdio.h>
#include <Psapi.h>

#include "..\..\src\commonfunc\StkObject.h"
#include "..\..\src\commonfunc\StkObjectUtil.h"

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

StkObject* MakeSampleXml1()
{
	StkObject* Elem1 = new StkObject(_T("SoftwareCompany"));

	for (int Loop = 0; Loop < 20; Loop++) {
		StkObject* Elem6 = new StkObject(_T("Product"));
		Elem6->AppendAttribute(new StkObject(StkObject::STKOBJECT_ATTRIBUTE, _T("Name"), _T("qwer")));
		Elem6->AppendAttribute(new StkObject(StkObject::STKOBJECT_ATTRIBUTE, _T("Price"), 10000));
		Elem1->AppendChildElement(Elem6);
	}

	StkObject* Elem2 = new StkObject(_T("Product"));
	Elem2->AppendAttribute(new StkObject(StkObject::STKOBJECT_ATTRIBUTE, _T("Name"), _T("Xxxxx paint")));
	Elem2->AppendAttribute(new StkObject(StkObject::STKOBJECT_ATTRIBUTE, _T("Price"), _T("11000")));

	StkObject* Elem5 = new StkObject(_T("License"));
	Elem5->AppendAttribute(new StkObject(StkObject::STKOBJECT_ATTRIBUTE, _T("Cap"), 322.44f));
	Elem2->AppendChildElement(Elem5);

	StkObject* Elem3 = new StkObject(_T("Product"));
	Elem3->AppendAttribute(new StkObject(StkObject::STKOBJECT_ATTRIBUTE, _T("Name"), _T("Aaaaa 3D")));
	Elem3->AppendAttribute(new StkObject(StkObject::STKOBJECT_ATTRIBUTE, _T("Price"), _T("39000")));

	StkObject* Elem4 = new StkObject(StkObject::STKOBJECT_ELEMENT, _T("ELEM_INT"), 123);
	StkObject* Elem7 = new StkObject(StkObject::STKOBJECT_ELEMENT, _T("ELEM_FLOAT"), 456.7f);
	StkObject* Elem8 = new StkObject(StkObject::STKOBJECT_ELEMENT, _T("ELEM_STRING"), _T(" ABC FFF DDD EEE "));

	Elem1->AppendAttribute(new StkObject(StkObject::STKOBJECT_ATTRIBUTE, _T("CompanyName"), _T("TakeuchiSoft")));
	Elem1->AppendAttribute(new StkObject(StkObject::STKOBJECT_ATTRIBUTE, _T("Employee"), _T("110")));
	Elem1->AppendAttribute(new StkObject(StkObject::STKOBJECT_ATTRIBUTE, _T("Address"), _T("Shizuoka Pref. Yaizu city")));
	Elem1->AppendChildElement(Elem2);
	Elem1->AppendChildElement(Elem3);
	Elem1->AppendChildElement(Elem4);
	Elem1->AppendChildElement(Elem7);
	Elem1->AppendChildElement(Elem8);

	return Elem1;
}

void GeneralTestCase1(StkObject* Elem1, TCHAR* Name)
{
	int Len1 = Elem1->GetFirstChildElement()->GetArrayLength();
	wprintf(_T("%s#Len of top level element's child = %d..."), Name, Len1);
	if (Len1 == 25) {
		printf("OK\r\n");
	} else {
		printf("NG\r\n");
		exit(0);
	}
	StkObject* Elem3 = Elem1->GetFirstChildElement();
	for (int Loop = 0; Loop < 21; Loop++) {
		Elem3 = Elem3->GetNext();
	}
	int Len1_1 = Elem3->GetArrayLength();
	wprintf(_T("%s#Len of top level element's 2nd child = %d..."), Name, Len1_1);
	if (Len1_1 == 4) {
		printf("OK\r\n");
	} else {
		printf("NG\r\n");
		exit(0);
	}
	int Len2 = Elem1->GetArrayLength();
	wprintf(_T("%s#Len of top level element = %d..."), Name, Len2);
	if (Len2 == 1) {
		printf("OK\r\n");
	} else {
		printf("NG\r\n");
		exit(0);
	}
	int ElemCount = Elem1->GetChildElementCount();
	wprintf(_T("%s#ElemCount = %d..."), Name, ElemCount);
	if (ElemCount == Len1) {
		printf("OK\r\n");
	} else {
		printf("NG\r\n");
		exit(0);
	}
	int AttrCount = Elem1->GetAttributeCount();
	wprintf(_T("%s#AttrCount = %d..."), Name, AttrCount);
	if (AttrCount == 3) {
		printf("OK\r\n");
	} else {
		printf("NG\r\n");
		exit(0);
	}
	StkObject* Elem4 = Elem3->GetNext();
	int Elem4ElemCnt = Elem4->GetChildElementCount();
	int Elem4AttrCnt = Elem4->GetAttributeCount();
	wprintf(_T("%s#SingleElement Count..."), Name, AttrCount);
	if (Elem4ElemCnt == 0 && Elem4AttrCnt == 0) {
		printf("OK\r\n");
	} else {
		printf("NG\r\n");
		exit(0);
	}
}

void CloneTest(StkObject* Obj)
{
	std::wstring OrgMsg;
	Obj->ToXml(&OrgMsg);

	StkObject* NewObj1 = Obj->Clone();
	StkObject* NewObj2 = Obj->Clone();

	std::wstring Msg1;
	std::wstring Msg2;
	NewObj1->ToXml(&Msg1);
	NewObj2->ToXml(&Msg2);

	printf("Clone Test#Validation of two clone's string...");
	if (lstrcmp(Msg1.c_str(), Msg2.c_str()) == 0 && lstrcmp(Msg1.c_str(), OrgMsg.c_str()) == 0) {
		printf("OK\r\n");
	} else {
		printf("NG\r\n");
		exit(0);
	}

	GeneralTestCase1(NewObj1, _T("Clone-1"));
	GeneralTestCase1(NewObj2, _T("Clone-2"));

	delete NewObj1;
	delete NewObj2;
}

void StkObjectUtilTest()
{
	int Offset = 0;
	StkObjectUtil Sou;

	TCHAR* Xml1 = _T("<Aaaa/>");
	TCHAR* Xml2 = _T("<Aaaa Xxxx=\"\"></Aaaa>");
	TCHAR* Xml3 = _T("<Aaaa> <Bbbb/> </Aaaa>");
	TCHAR* Xml4 = _T("<Aaaa> <Bbbb></Bbbb> </Aaaa>");
	TCHAR* Xml5 = _T("<Aaaa> <Bbbb>S.Takeuchi</Bbbb><Bbbb> A.Tsunemi </Bbbb><Bbbb></Bbbb> </Aaaa>");
	TCHAR* Xml6 = _T(" <	 Aaaa	  >		  <		  /		  Aaaa		 >	 ");
	TCHAR* Xml7 = _T("<Aaaa Xxx=\"1234\" Yyy=\"987\"><Bbbb><Cccc Zzz=\"000\"/><Cccc/></Bbbb><Bbbb><Cccc Zzz=\"000\"></Cccc></Bbbb><Bbbb><Cccc></Cccc></Bbbb> </Aaaa>");

	StkObject* RetObj1 = Sou.CreateObjectFromXml(Xml1, &Offset);
	StkObject* RetObj2 = Sou.CreateObjectFromXml(Xml2, &Offset);
	StkObject* RetObj3 = Sou.CreateObjectFromXml(Xml3, &Offset);
	StkObject* RetObj4 = Sou.CreateObjectFromXml(Xml4, &Offset);
	StkObject* RetObj5 = Sou.CreateObjectFromXml(Xml5, &Offset);
	StkObject* RetObj6 = Sou.CreateObjectFromXml(Xml6, &Offset);
	StkObject* RetObj7 = Sou.CreateObjectFromXml(Xml7, &Offset);

	std::wstring Msg1;
	std::wstring Msg2;
	std::wstring Msg3;
	std::wstring Msg4;
	std::wstring Msg5;
	std::wstring Msg6;
	std::wstring Msg7;

	RetObj1->ToXml(&Msg1);
	RetObj2->ToXml(&Msg2);
	RetObj3->ToXml(&Msg3);
	RetObj4->ToXml(&Msg4);
	RetObj5->ToXml(&Msg5);
	RetObj6->ToXml(&Msg6);
	RetObj7->ToXml(&Msg7);

	delete RetObj1;
	delete RetObj2;
	delete RetObj3;
	delete RetObj4;
	delete RetObj5;
	delete RetObj6;
	delete RetObj7;
}

void StkObjectTest()
{
	printf("StkObjectTest started.\r\n");

	StkObjectUtilTest();

	StkObject* Elem1 = MakeSampleXml1();
	GeneralTestCase1(Elem1, _T("Sample1"));
	CloneTest(Elem1);
	MemoryLeakChecking(Elem1);
	delete Elem1;

	printf("StkObjectTest completed.\r\n\r\n\r\n");

}
