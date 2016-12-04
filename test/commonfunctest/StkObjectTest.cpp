#include <windows.h>
#include <string>
#include <tchar.h>
#include <stdio.h>
#include <Psapi.h>

#include "..\..\src\commonfunc\StkObject.h"

void Abort(BOOL Cond)
{
	if (!Cond) {
		printf("NG\r\n");
		exit(0);
	} else {
		printf("OK\r\n");
	}
}

void AbnormalCaseForJson(TCHAR* JsonTxt, int ErrorCode)
{
	int Offset = 0;
	StkObject* RetObj;

	if (JsonTxt == NULL) {
		wprintf(_T("Abnormal case (JSON): NULL is presented..."));
	} else {
		wprintf(_T("Abnormal case (JSON): \"%s\" is presented..."), JsonTxt);
	}
	RetObj = StkObject::CreateObjectFromJson(JsonTxt, &Offset);
	if (RetObj != NULL || Offset != ErrorCode) {
		printf("NG\r\n");
		exit(0);
	}
	delete RetObj;
	printf("OK\r\n");
}

void AbnormalCaseForXml(TCHAR* XmlTxt, int ErrorCode)
{
	int Offset = 0;
	StkObject* RetObj;

	if (XmlTxt == NULL) {
		wprintf(_T("Abnormal case (XML): NULL is presented..."));
	} else {
		wprintf(_T("Abnormal case (XML): \"%s\" is presented..."), XmlTxt);
	}
	RetObj = StkObject::CreateObjectFromXml(XmlTxt, &Offset);
	if (RetObj != NULL || Offset != ErrorCode) {
		printf("NG\r\n");
		exit(0);
	}
	delete RetObj;
	printf("OK\r\n");
}

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

StkObject* MakeTestData1(int Mode = 0)
{
	StkObject* Elem1 = new StkObject(_T("SoftwareCompany"));

	for (int Loop = 0; Loop < 20; Loop++) {
		StkObject* Elem6 = new StkObject(_T("Product"));
		Elem6->AppendAttribute(new StkObject(_T("Name"), _T("qwer")));
		Elem6->AppendAttribute(new StkObject(_T("Price"), 10000));
		Elem1->AppendChildElement(Elem6);
	}

	StkObject* Elem2 = new StkObject(_T("Product"));
	if (Mode == 1) {
		Elem2->AppendAttribute(new StkObject(_T("Price"), _T("11000")));
		Elem2->AppendAttribute(new StkObject(_T("Name"), _T("Xxxxx paint")));
	} else {
		Elem2->AppendAttribute(new StkObject(_T("Name"), _T("Xxxxx paint")));
		Elem2->AppendAttribute(new StkObject(_T("Price"), _T("11000")));
	}

	StkObject* Elem5 = new StkObject(_T("License"));
	Elem5->AppendAttribute(new StkObject(_T("Cap"), 322.44f));
	Elem2->AppendChildElement(Elem5);

	StkObject* Elem3 = new StkObject(_T("Product"));
	if (Mode == 1) {
		Elem3->AppendAttribute(new StkObject(_T("Price"), _T("39000")));
		Elem3->AppendAttribute(new StkObject(_T("Name"), _T("Aaaaa 3D")));
	} else {
		Elem3->AppendAttribute(new StkObject(_T("Name"), _T("Aaaaa 3D")));
		if (Mode == 2) {
			Elem3->AppendAttribute(new StkObject(_T("Price"), _T(" 39000")));
		} else {
			Elem3->AppendAttribute(new StkObject(_T("Price"), _T("39000")));
		}
	}

	StkObject* Elem4 = new StkObject(_T("ELEM_INT"), 123);
	StkObject* Elem7 = new StkObject(_T("ELEM_FLOAT"), 456.7f);
	StkObject* Elem8 = new StkObject(_T("ELEM_STRING"), _T(" ABC FFF DDD EEE "));

	Elem1->AppendAttribute(new StkObject(_T("CompanyName"), _T("TakeuchiSoft")));
	Elem1->AppendAttribute(new StkObject(_T("Employee"), _T("110")));
	Elem1->AppendAttribute(new StkObject(_T("Address"), _T("Shizuoka Pref. Yaizu city")));
	Elem1->AppendChildElement(Elem2);
	Elem1->AppendChildElement(Elem3);
	Elem1->AppendChildElement(Elem4);
	Elem1->AppendChildElement(Elem7);
	Elem1->AppendChildElement(Elem8);

	return Elem1;
}

StkObject* MakeTestData2(int Mode = 0)
{
	StkObject* Elem1 = new StkObject(_T("個人の財産"));

	if (Mode == 1) {
		StkObject* Elem1_3 = new StkObject(_T("現金"));
		Elem1_3->AppendChildElement(new StkObject(_T("C銀行"), 7000000));
		Elem1_3->AppendChildElement(new StkObject(_T("A銀行"), 5000000));
		Elem1_3->AppendChildElement(new StkObject(_T("B銀行"), 10000000));
		Elem1->AppendChildElement(Elem1_3);

		StkObject* Elem1_2 = new StkObject(_T("時計"));
		Elem1_2->AppendChildElement(new StkObject(_T("ロレックス"), _T("金")));
		Elem1_2->AppendChildElement(new StkObject(_T("オメガ"), _T("黒")));
		Elem1_2->AppendChildElement(new StkObject(_T("グランドセイコー"), _T("白")));
		Elem1->AppendChildElement(Elem1_2);

		StkObject* Elem1_1 = new StkObject(_T("乗用車"));
		StkObject* Elem1_1_2 = new StkObject(_T("ブルーバード"));
		Elem1_1_2->AppendAttribute(new StkObject(_T("メーカー"), _T("日産")));
		Elem1_1_2->AppendAttribute(new StkObject(_T("色"), _T("白")));
		Elem1_1->AppendChildElement(Elem1_1_2);
		StkObject* Elem1_1_1 = new StkObject(_T("カローラ"));
		Elem1_1_1->AppendAttribute(new StkObject(_T("メーカー"), _T("トヨタ")));
		Elem1_1_1->AppendAttribute(new StkObject(_T("色"), _T("赤")));
		Elem1_1->AppendChildElement(Elem1_1_1);
		Elem1->AppendChildElement(Elem1_1);
	} else {
		StkObject* Elem1_1 = new StkObject(_T("乗用車"));
		StkObject* Elem1_1_1 = new StkObject(_T("カローラ"));
		Elem1_1_1->AppendAttribute(new StkObject(_T("色"), _T("赤")));
		Elem1_1_1->AppendAttribute(new StkObject(_T("メーカー"), _T("トヨタ")));
		Elem1_1->AppendChildElement(Elem1_1_1);
		StkObject* Elem1_1_2 = new StkObject(_T("ブルーバード"));
		Elem1_1_2->AppendAttribute(new StkObject(_T("色"), _T("白")));
		Elem1_1_2->AppendAttribute(new StkObject(_T("メーカー"), _T("日産")));
		Elem1_1->AppendChildElement(Elem1_1_2);
		Elem1->AppendChildElement(Elem1_1);

		StkObject* Elem1_2 = new StkObject(_T("時計"));
		Elem1_2->AppendChildElement(new StkObject(_T("グランドセイコー"), _T("白")));
		Elem1_2->AppendChildElement(new StkObject(_T("オメガ"), _T("黒")));
		Elem1_2->AppendChildElement(new StkObject(_T("ロレックス"), _T("金")));
		Elem1->AppendChildElement(Elem1_2);

		StkObject* Elem1_3 = new StkObject(_T("現金"));
		Elem1_3->AppendChildElement(new StkObject(_T("A銀行"), 5000000));
		Elem1_3->AppendChildElement(new StkObject(_T("B銀行"), 10000000));
		Elem1_3->AppendChildElement(new StkObject(_T("C銀行"), 7000000));
		Elem1->AppendChildElement(Elem1_3);
	}

	StkObject* Elem1_4 = new StkObject(_T("証券"));
	StkObject* Elem1_4_1 = new StkObject(_T("株"));
	Elem1_4_1->AppendChildElement(new StkObject(_T("A社"), 2000000));
	Elem1_4_1->AppendChildElement(new StkObject(_T("B社"), 500000));
	Elem1_4->AppendChildElement(Elem1_4_1);
	StkObject* Elem1_4_2 = new StkObject(_T("国債"));
	if (Mode == 1) {
		Elem1_4_2->AppendChildElement(new StkObject(_T("債権"), 1000000));
		Elem1_4_2->AppendChildElement(new StkObject(_T("債権"), 2000000));
		Elem1_4_2->AppendChildElement(new StkObject(_T("債権"), 1500000));
	} else {
		Elem1_4_2->AppendChildElement(new StkObject(_T("債権"), 2000000));
		Elem1_4_2->AppendChildElement(new StkObject(_T("債権"), 1500000));
		if (Mode == 2) {
			Elem1_4_2->AppendChildElement(new StkObject(_T("債権"), 1000001));
		} else {
			Elem1_4_2->AppendChildElement(new StkObject(_T("債権"), 1000000));
		}
	}
	Elem1_4->AppendChildElement(Elem1_4_2);
	StkObject* Elem1_4_3 = new StkObject(_T("金券"));
	Elem1_4_3->AppendChildElement(new StkObject(_T("VISA"), 200000));
	Elem1_4_3->AppendChildElement(new StkObject(_T("JCB"), 100000));
	Elem1_4->AppendChildElement(Elem1_4_3);
	Elem1->AppendChildElement(Elem1_4);

	return Elem1;
}

StkObject* MakeTestData3(TCHAR Name[64], int Width, int Height, int CurrentLevel = 0)
{
	StkObject* Obj = new StkObject(Name);
	if (CurrentLevel > Height) {
		Obj->AppendChildElement(new StkObject(_T("Aaa"), 12345));
		Obj->AppendChildElement(new StkObject(_T("Bbb"), 67890));
		Obj->AppendChildElement(new StkObject(_T("Ccc"), 123.45f));
		Obj->AppendChildElement(new StkObject(_T("Ddd"), 678.90f));
	} else {
		for (int Loop = 0; Loop < Width; Loop++) {
			TCHAR Buf[25];
			wsprintf(Buf, _T("Elem_%d:%d"), CurrentLevel, Loop);
			Obj->AppendChildElement(MakeTestData3(Buf, Width, Height, CurrentLevel + 1));
		}
	}
	return Obj;
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
	wprintf(_T("%s#Acquired element is validated..."), Name);
	StkObject* Elem3 = Elem1->GetFirstChildElement();
	for (int Loop = 0; Loop < 21; Loop++) {
		if (Loop < 20) {
			if (lstrcmp(Elem3->GetName(), _T("Product")) != 0 || lstrcmp(Elem3->GetFirstAttribute()->GetStringValue(), _T("qwer")) != 0) {
				printf("NG\r\n");
				exit(0);
			}
		}
		Elem3 = Elem3->GetNext();
	}
	printf("OK\r\n");
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

void GeneralTestCase2(StkObject* Elem1, TCHAR* Name)
{
	wprintf(_T("%s#Proper count can be acquired."), Name);
	if (Elem1->GetChildElementCount() != Elem1->GetFirstChildElement()->GetArrayLength()) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");

	wprintf(_T("%s#Each name is correct..."), Name);
	if (lstrcmp(Elem1->GetName(), _T("個人の財産")) != 0 ||
		lstrcmp(Elem1->GetFirstChildElement()->GetName(), _T("乗用車")) != 0 ||
		lstrcmp(Elem1->GetFirstChildElement()->GetNext()->GetName(), _T("時計")) != 0 ||
		lstrcmp(Elem1->GetFirstChildElement()->GetNext()->GetNext()->GetName(), _T("現金")) != 0 ||
		lstrcmp(Elem1->GetFirstChildElement()->GetNext()->GetNext()->GetNext()->GetName(), _T("証券")) != 0) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");

	wprintf(_T("%s#Attribute of car is correct..."), Name);
	if (lstrcmp(Elem1->GetFirstChildElement()->GetFirstChildElement()->GetFirstAttribute()->GetStringValue(), _T("赤")) != 0 ||
		lstrcmp(Elem1->GetFirstChildElement()->GetFirstChildElement()->GetFirstAttribute()->GetNext()->GetStringValue(), _T("トヨタ")) != 0 ||
		lstrcmp(Elem1->GetFirstChildElement()->GetFirstChildElement()->GetNext()->GetFirstAttribute()->GetStringValue(), _T("白")) != 0 ||
		lstrcmp(Elem1->GetFirstChildElement()->GetFirstChildElement()->GetNext()->GetFirstAttribute()->GetNext()->GetStringValue(), _T("日産")) != 0) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");

	wprintf(_T("%s#Fetch integer value..."), Name);
	if (Elem1->GetFirstChildElement()->GetNext()->GetNext()->GetFirstChildElement()->GetIntValue() != 5000000 ||
		Elem1->GetFirstChildElement()->GetNext()->GetNext()->GetFirstChildElement()->GetNext()->GetIntValue() != 10000000 ||
		Elem1->GetFirstChildElement()->GetNext()->GetNext()->GetFirstChildElement()->GetNext()->GetNext()->GetIntValue() != 7000000) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");

	wprintf(_T("%s#Fetch tail element..."), Name);
	if (Elem1->GetFirstChildElement()->GetNext()->GetNext()->GetNext()->GetFirstChildElement()->GetNext()->GetNext()->GetFirstChildElement()->GetIntValue() != 200000 ||
		lstrcmp(Elem1->GetFirstChildElement()->GetNext()->GetNext()->GetNext()->GetFirstChildElement()->GetNext()->GetNext()->GetFirstChildElement()->GetName(), _T("VISA")) != 0) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");
}

void GeneralTestCase3()
{
	wprintf(_T("GeneralCheck3#Test of Equals 1st ..."));
	StkObject* Origin1 = MakeTestData1();
	StkObject* Normal1 = MakeTestData1(0);
	StkObject* Order1 = MakeTestData1(1);
	StkObject* Diff1 = MakeTestData1(2);
	BOOL Ch1 = Origin1->Equals(Normal1);
	BOOL Ch2 = Origin1->Equals(Order1);
	BOOL Ch3 = Origin1->Equals(Diff1);
	if (!Ch1 || !Ch2 || Ch3) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");
	delete Origin1;
	delete Normal1;
	delete Order1;
	delete Diff1;

	wprintf(_T("GeneralCheck3#Test of Equals 2nd ..."));
	StkObject* Origin2 = MakeTestData2();
	StkObject* Normal2 = MakeTestData2(0);
	StkObject* Order2 = MakeTestData2(1);
	StkObject* Diff2 = MakeTestData2(2);
	BOOL Ci1 = Origin2->Equals(Normal2);
	BOOL Ci2 = Origin2->Equals(Order2);
	BOOL Ci3 = Origin2->Equals(Diff2);
	if (!Ci1 || !Ci2 || Ci3) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");
	delete Origin2;
	delete Normal2;
	delete Order2;
	delete Diff2;

	wprintf(_T("GeneralCheck3#Test of Equals 3rd integer match ..."));
	StkObject* Obj1a = new StkObject(_T("Shinya Takeuchi"), 123);
	StkObject* Obj1b = new StkObject(_T("Shinya Takeuchi"), 123);
	if (Obj1a->Equals(Obj1b) == FALSE) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");
	delete Obj1a;
	delete Obj1b;

	wprintf(_T("GeneralCheck3#Test of Equals 3rd integer unmatch ..."));
	StkObject* Obj2a = new StkObject(_T("Shinya Takeuchi"), 123);
	StkObject* Obj2b = new StkObject(_T("Shinya Takeuchi"), 321);
	if (Obj2a->Equals(Obj2b) == TRUE) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");
	delete Obj2a;
	delete Obj2b;

	wprintf(_T("GeneralCheck3#Test of Equals 3rd float match ..."));
	StkObject* Obj3a = new StkObject(_T("Shinya Takeuchi"), 123.45f);
	StkObject* Obj3b = new StkObject(_T("Shinya Takeuchi"), 123.45f);
	if (Obj3a->Equals(Obj3b) == FALSE) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");
	delete Obj3a;
	delete Obj3b;

	wprintf(_T("GeneralCheck3#Test of Equals 3rd float unmatch ..."));
	StkObject* Obj4a = new StkObject(_T("Shinya Takeuchi"), 123.45f);
	StkObject* Obj4b = new StkObject(_T("Shinya Takeuchi"), 123.46f);
	if (Obj4a->Equals(Obj4b) == TRUE) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");
	delete Obj4a;
	delete Obj4b;

	wprintf(_T("GeneralCheck3#Test of Equals 3rd string match ..."));
	StkObject* Obj5a = new StkObject(_T("Shinya Takeuchi"), _T("Hello, World!"));
	StkObject* Obj5b = new StkObject(_T("Shinya Takeuchi"), _T("Hello, World!"));
	if (Obj5a->Equals(Obj5b) == FALSE) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");
	delete Obj5a;
	delete Obj5b;

	wprintf(_T("GeneralCheck3#Test of Equals 3rd string unmatch ..."));
	StkObject* Obj6a = new StkObject(_T("Shinya Takeuchi"), _T("Hello, World!"));
	StkObject* Obj6b = new StkObject(_T("Shinya Takeuchi"), _T("Hello; World!"));
	if (Obj6a->Equals(Obj6b) == TRUE) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");
	delete Obj6a;
	delete Obj6b;
}

void GeneralTestCase4()
{
	{
		wprintf(_T("GeneralCheck4#Test of Contains  (first elem match case) ..."));
		int ErrorCode;
		StkObject* Origin = StkObject::CreateObjectFromXml(_T("<Hello><First>Shinya</First><Last>Takeuchi</Last></Hello>"), &ErrorCode);
		StkObject* Target = StkObject::CreateObjectFromXml(_T("<First>Shinya</First>"), &ErrorCode);
		StkObject* Ret = Origin->Contains(Target);
		Abort(Ret != NULL &&
			  lstrcmp(Ret->GetStringValue(), _T("Shinya")) == 0 &&
			  lstrcmp(Ret->GetName(), _T("First")) == 0 &&
			  lstrcmp(Ret->GetNext()->GetStringValue(), _T("Takeuchi")) == 0 &&
			  lstrcmp(Ret->GetNext()->GetName(), _T("Last")) == 0);
		delete Origin;
		delete Target;
	}
	{
		wprintf(_T("GeneralCheck4#Test of Contains  (last elem match case) ..."));
		int ErrorCode;
		StkObject* Origin = StkObject::CreateObjectFromXml(_T("<Hello><First>Shinya</First><Middle>Tsunemi</Middle><Last>Takeuchi</Last></Hello>"), &ErrorCode);
		StkObject* Target = StkObject::CreateObjectFromXml(_T("<Last>Takeuchi</Last>"), &ErrorCode);
		StkObject* Ret = Origin->Contains(Target);
		Abort(Ret != NULL &&
			  lstrcmp(Ret->GetStringValue(), _T("Takeuchi")) == 0 &&
			  lstrcmp(Ret->GetName(), _T("Last")) == 0);
		delete Origin;
		delete Target;
	}
	{
		wprintf(_T("GeneralCheck4#Test of Contains  (simple int/float/string element) ..."));
		StkObject* OriginInt = new StkObject(_T("ObjInt"), 345);
		StkObject* TargetInt = new StkObject(_T("ObjInt"), 345);
		StkObject* OriginFloat = new StkObject(_T("ObjFloat"), 345.6f);
		StkObject* TargetFloat = new StkObject(_T("ObjFloat"), 345.6f);
		StkObject* OriginStr = new StkObject(_T("ObjFloat"), _T("Hello, World!"));
		StkObject* TargetStr = new StkObject(_T("ObjFloat"), _T("Hello, World!"));
		StkObject* RetInt = OriginInt->Contains(TargetInt);
		StkObject* RetFloat = OriginFloat->Contains(TargetFloat);
		StkObject* RetStr = OriginStr->Contains(TargetStr);
		Abort(RetInt == OriginInt && RetFloat == OriginFloat && RetStr == OriginStr);
		delete OriginInt;
		delete TargetInt;
		delete OriginFloat;
		delete TargetFloat;
		delete OriginStr;
		delete TargetStr;
	}
	{
		wprintf(_T("GeneralCheck4#Test of Contains  (simple NG case) ..."));
		int ErrorCode;
		StkObject* Origin = StkObject::CreateObjectFromXml(_T("<Hello><First>Shinya</First><Middle>Tsunemi</Middle><Last>Takeuchi</Last></Hello>"), &ErrorCode);
		StkObject* Target = StkObject::CreateObjectFromXml(_T("<Last>Suzuki</Last>"), &ErrorCode);
		StkObject* Ret = Origin->Contains(Target);
		Abort(Ret == NULL);
		delete Origin;
		delete Target;
	}
	{
		wprintf(_T("GeneralCheck4#Test of Contains  (Common data 1) ..."));
		StkObject* Origin = MakeTestData1();
		StkObject* Target = MakeTestData1();
		StkObject* Ret = Origin->Contains(Target);
		Abort(Ret != NULL);
		delete Origin;
		delete Target;
	}
	{
		wprintf(_T("GeneralCheck4#Test of Contains  (Common data 1 : advanced) ..."));
		StkObject* Origin = MakeTestData1();

		StkObject* Elem1 = new StkObject(_T("SoftwareCompany"));
		StkObject* Elem8 = new StkObject(_T("ELEM_STRING"), _T(" ABC FFF DDD EEE "));
		StkObject* Elem7 = new StkObject(_T("ELEM_FLOAT"), 456.7f);
		StkObject* Elem4 = new StkObject(_T("ELEM_INT"), 123);
		Elem1->AppendAttribute(new StkObject(_T("Employee"), _T("110")));
		Elem1->AppendChildElement(Elem8);
		Elem1->AppendChildElement(Elem7);
		Elem1->AppendChildElement(Elem4);
		StkObject* Ret = Origin->Contains(Elem1);
		Abort(Ret == Origin);
		delete Origin;
		delete Elem1;
	}
	{
		wprintf(_T("GeneralCheck4#Test of Contains  (Common data 2) ..."));
		StkObject* Origin = MakeTestData2();
		StkObject* Target = MakeTestData2();
		StkObject* Ret = Origin->Contains(Target);
		Abort(Ret != NULL);
		delete Origin;
		delete Target;
	}
	{
		wprintf(_T("GeneralCheck4#Test of Contains  (Common data 2 : advanced) ..."));
		StkObject* Origin = MakeTestData2();
		StkObject* Elem1_4 = new StkObject(_T("証券"));
		StkObject* Elem1_4_1 = new StkObject(_T("株"));
		Elem1_4_1->AppendChildElement(new StkObject(_T("B社"), 500000));
		Elem1_4_1->AppendChildElement(new StkObject(_T("A社"), 2000000));
		Elem1_4->AppendChildElement(Elem1_4_1);
		StkObject* Ret = Origin->Contains(Elem1_4);
		Abort(Ret != NULL && lstrcmp(Ret->GetName(), _T("証券")) == 0 && Ret->GetFirstChildElement()->GetFirstChildElement()->GetIntValue() == 2000000);
		delete Origin;
		delete Elem1_4;
	}
	{
		wprintf(_T("GeneralCheck4#Test of Contains  (All type of element) ..."));
		StkObject* Origin = MakeTestData1();
		StkObject* Target1 = new StkObject(_T("ELEM_INT"), 123);
		StkObject* Target2 = new StkObject(_T("ELEM_INT"), 321);
		StkObject* Target3 = new StkObject(_T("ELEM_FLOAT"), 456.7f);
		StkObject* Target4 = new StkObject(_T("ELEM_FLOAT"), 765.4f);
		StkObject* Target5 = new StkObject(_T("ELEM_STRING"), _T(" ABC FFF DDD EEE "));
		StkObject* Target6 = new StkObject(_T("ELEM_STRING"), _T(" FFF DDD EEE ABC "));
		StkObject* Ret1 = Origin->Contains(Target1);
		StkObject* Ret2 = Origin->Contains(Target2);
		StkObject* Ret3 = Origin->Contains(Target3);
		StkObject* Ret4 = Origin->Contains(Target4);
		StkObject* Ret5 = Origin->Contains(Target5);
		StkObject* Ret6 = Origin->Contains(Target6);
		Abort(Ret1 != NULL && Ret2 == NULL && Ret3 != NULL && Ret4 == NULL && Ret5 != NULL && Ret6 == NULL &&
			  lstrcmp(Ret1->GetName(), _T("ELEM_INT")) == 0 &&
			  Ret1->GetIntValue() == 123 &&
			  lstrcmp(Ret3->GetName(), _T("ELEM_FLOAT")) == 0 &&
			  Ret3->GetFloatValue() == 456.7f &&
			  lstrcmp(Ret5->GetName(), _T("ELEM_STRING")) == 0 &&
			  lstrcmp(Ret5->GetStringValue(), _T(" ABC FFF DDD EEE ")) == 0);
		delete Origin;
		delete Target1;
		delete Target2;
		delete Target3;
		delete Target4;
		delete Target5;
		delete Target6;
	}
	{
		wprintf(_T("GeneralCheck4#Test of Contains  (Hierarchy : normal) ..."));
		int ErrorCode;
		StkObject* Origin = StkObject::CreateObjectFromXml(_T("<Aaa><Bbb><Ccc Name=\"Spring\"><Ddd>春</Ddd></Ccc><Ccc Name=\"Summer\"><Ddd>夏</Ddd></Ccc></Bbb><Bbb><Ccc Name=\"Fall\"><Ddd>秋</Ddd></Ccc><Ccc Name=\"Winter\"><Ddd>冬</Ddd></Ccc></Bbb></Aaa>"), &ErrorCode);
		StkObject* Target1 = StkObject::CreateObjectFromXml(_T("<Aaa><Bbb><Ccc Name=\"Summer\"><Ddd>夏</Ddd></Ccc></Bbb></Aaa>"), &ErrorCode);
		StkObject* Target2 = StkObject::CreateObjectFromXml(_T("<Aaa><Bbb><Ccc Name=\"Winter\"></Ccc></Bbb></Aaa>"), &ErrorCode);
		StkObject* Target3 = StkObject::CreateObjectFromXml(_T("<Aaa></Aaa>"), &ErrorCode);
		StkObject* Target4 = StkObject::CreateObjectFromXml(_T("<Bbb><Ccc Name=\"Fall\"><Ddd>秋</Ddd></Ccc></Bbb>"), &ErrorCode);
		StkObject* Target5 = StkObject::CreateObjectFromXml(_T("<Ccc Name=\"Spring\"><Ddd>春</Ddd></Ccc>"), &ErrorCode);
		StkObject* Target6 = StkObject::CreateObjectFromXml(_T("<Ddd>冬</Ddd>"), &ErrorCode);
		StkObject* Target7 = StkObject::CreateObjectFromXml(_T("<Aaa><Bbb><Ccc Name=\"Spring\"><Ddd>春</Ddd></Ccc></Bbb><Bbb><Ccc Name=\"Winter\"><Ddd>冬</Ddd></Ccc></Bbb></Aaa>"), &ErrorCode);
		StkObject* Target8 = StkObject::CreateObjectFromXml(_T("<Ccc Name=\"Fall\"/>"), &ErrorCode);
		StkObject* Ret1 = Origin->Contains(Target1);
		StkObject* Ret2 = Origin->Contains(Target2);
		StkObject* Ret3 = Origin->Contains(Target3);
		StkObject* Ret4 = Origin->Contains(Target4);
		StkObject* Ret5 = Origin->Contains(Target5);
		StkObject* Ret6 = Origin->Contains(Target6);
		StkObject* Ret7 = Origin->Contains(Target7);
		StkObject* Ret8 = Origin->Contains(Target8);
		Abort(Ret1 != NULL && Ret2 != NULL && Ret3 != NULL && Ret4 != NULL && Ret5 != NULL && Ret6 != NULL && Ret7 != NULL && Ret8 != NULL &&
			  lstrcmp(Ret4->GetName(), _T("Bbb")) == 0 &&
			  lstrcmp(Ret5->GetName(), _T("Ccc")) == 0 &&
			  lstrcmp(Ret6->GetName(), _T("Ddd")) == 0);
		delete Origin;
		delete Target1;
		delete Target2;
		delete Target3;
		delete Target4;
		delete Target5;
		delete Target6;
		delete Target7;
		delete Target8;
	}
	{
		wprintf(_T("GeneralCheck4#Test of Contains  (Hierarchy : abnormal) ..."));
		int ErrorCode;
		StkObject* Origin = StkObject::CreateObjectFromXml(_T("<Aaa><Bbb><Ccc Name=\"Spring\"><Ddd>春</Ddd></Ccc><Ccc Name=\"Summer\"><Ddd>夏</Ddd></Ccc></Bbb><Bbb><Ccc Name=\"Fall\"><Ddd>秋</Ddd></Ccc><Ccc Name=\"Winter\"><Ddd>冬</Ddd></Ccc></Bbb></Aaa>"), &ErrorCode);
		StkObject* Target1 = StkObject::CreateObjectFromXml(_T("<Aaa><Bbb><Ccc Name=\"Summer\"><Ddd>冬</Ddd></Ccc></Bbb></Aaa>"), &ErrorCode);
		StkObject* Target2 = StkObject::CreateObjectFromXml(_T("<Aaa><Bbb><Ddd>夏</Ddd></Bbb></Aaa>"), &ErrorCode);
		StkObject* Target3 = StkObject::CreateObjectFromXml(_T("<Aaa><Ccc Name=\"Winter\"><Ddd>冬</Ddd></Ccc></Aaa>"), &ErrorCode);
		StkObject* Target4 = StkObject::CreateObjectFromXml(_T("<Aaa><Ddd>夏</Ddd></Aaa>"), &ErrorCode);
		StkObject* Target5 = StkObject::CreateObjectFromXml(_T("<Bbb><Ddd>夏</Ddd></Bbb>"), &ErrorCode);
		StkObject* Target6 = StkObject::CreateObjectFromXml(_T("<Aaa>冬</Aaa>"), &ErrorCode);
		StkObject* Target7 = StkObject::CreateObjectFromXml(_T("<Aaa><Bbb><Ccc Name=\"Spring\"><Ddd>春</Ddd></Ccc><Ccc Name=\"Winter\"><Ddd>冬</Ddd></Ccc></Bbb></Aaa>"), &ErrorCode);
		StkObject* Target8 = StkObject::CreateObjectFromXml(_T("<Aaa><Ccc Name=\"Fall\"/></Aaa>"), &ErrorCode);
		StkObject* Ret1 = Origin->Contains(Target1);
		StkObject* Ret2 = Origin->Contains(Target2);
		StkObject* Ret3 = Origin->Contains(Target3);
		StkObject* Ret4 = Origin->Contains(Target4);
		StkObject* Ret5 = Origin->Contains(Target5);
		StkObject* Ret6 = Origin->Contains(Target6);
		StkObject* Ret7 = Origin->Contains(Target7);
		StkObject* Ret8 = Origin->Contains(Target8);
		Abort(Ret1 == NULL && Ret2 == NULL && Ret3 == NULL && Ret4 == NULL && Ret5 == NULL && Ret6 == NULL && Ret7 == NULL && Ret8 == NULL);
		delete Origin;
		delete Target1;
		delete Target2;
		delete Target3;
		delete Target4;
		delete Target5;
		delete Target6;
		delete Target7;
		delete Target8;
	}
	{
		wprintf(_T("GeneralCheck4#Test of Contains  (Hierarchy : sequential check) ..."));
		int ErrorCode;
		StkObject* Origin = StkObject::CreateObjectFromXml(_T("<Aaa><Bbb Type=\"文字\"><Ccc Name=\"季節\"><Ddd>春</Ddd><Ddd>夏</Ddd><Ddd>秋</Ddd><Ddd>冬</Ddd></Ccc><Ccc Name=\"方角\"><Ddd>東</Ddd><Ddd>西</Ddd><Ddd>南</Ddd><Ddd>北</Ddd></Ccc></Bbb><Bbb Type=\"氏名\"><Ccc Name=\"家族\"><Ddd>竹内伸也</Ddd><Ddd>常見朱美</Ddd><Ddd>木村優子</Ddd></Ccc><Ccc Name=\"会社メンバー\"><Ddd>山田太郎</Ddd><Ddd>鈴木一郎</Ddd></Ccc></Bbb></Aaa>"), &ErrorCode);
		StkObject* Target1 = StkObject::CreateObjectFromXml(_T("<Bbb Type=\"文字\"/>"), &ErrorCode);
		StkObject* Target2 = StkObject::CreateObjectFromXml(_T("<Ccc Name=\"方角\"/>"), &ErrorCode);
		StkObject* Target3 = StkObject::CreateObjectFromXml(_T("<Ccc Name=\"季節\"/>"), &ErrorCode);
		StkObject* Target4 = StkObject::CreateObjectFromXml(_T("<Ddd>南</Ddd>"), &ErrorCode);
		StkObject* Target5 = StkObject::CreateObjectFromXml(_T("<Ddd>冬</Ddd>"), &ErrorCode);
		StkObject* Ret124 = Origin->Contains(Target1)->Contains(Target2)->Contains(Target4);
		StkObject* Ret125 = Origin->Contains(Target1)->Contains(Target2)->Contains(Target5);
		StkObject* Ret134 = Origin->Contains(Target1)->Contains(Target3)->Contains(Target4);
		StkObject* Ret135 = Origin->Contains(Target1)->Contains(Target3)->Contains(Target5);
		Abort(lstrcmp(Ret124->GetStringValue(), _T("南")) == 0 && Ret125 == NULL &&
			  Ret134 == NULL && lstrcmp(Ret135->GetStringValue(), _T("冬")) == 0);
		delete Origin;
		delete Target1;
		delete Target2;
		delete Target3;
		delete Target4;
	}
}

void GeneralTestCase5()
{
	{
		wprintf(_T("GeneralCheck5#Test of Fetching the Value of Invalid Type for Int Value ..."));
		StkObject* Obj = new StkObject(_T("IntValue"), 123);
		Abort(Obj->GetIntValue() == 123 && Obj->GetFloatValue() == 0.0f && Obj->GetStringValue() == NULL);

		wprintf(_T("GeneralCheck5#Test of Fetching the Value of Invalid Type for Int Value (Child Element) ..."));
		StkObject* ParentObj = new StkObject(_T("Parent"));
		ParentObj->AppendChildElement(Obj);
		ParentObj->GetFirstChildElement()->SetIntValue(321);
		ParentObj->GetFirstChildElement()->SetFloatValue(999.9f);
		ParentObj->GetFirstChildElement()->SetStringValue(_T("Hello"));
		Abort(ParentObj->GetFirstChildElement()->GetIntValue() == 321 &&
			ParentObj->GetFirstChildElement()->GetFloatValue() == 0.0f &&
			ParentObj->GetFirstChildElement()->GetStringValue() == NULL);
		delete ParentObj;
	}
	{
		wprintf(_T("GeneralCheck5#Test of Fetching the Value of Invalid Type for Float Value ..."));
		StkObject* Obj = new StkObject(_T("IntValue"), 123.4f);
		Abort(Obj->GetIntValue() == 0 && Obj->GetFloatValue() == 123.4f && Obj->GetStringValue() == NULL);

		wprintf(_T("GeneralCheck5#Test of Fetching the Value of Invalid Type for Float Value (Child Element) ..."));
		StkObject* ParentObj = new StkObject(_T("Parent"));
		ParentObj->AppendChildElement(Obj);
		ParentObj->GetFirstChildElement()->SetIntValue(999);
		ParentObj->GetFirstChildElement()->SetFloatValue(432.1f);
		ParentObj->GetFirstChildElement()->SetStringValue(_T("Hello"));
		Abort(ParentObj->GetFirstChildElement()->GetIntValue() == 0 &&
			ParentObj->GetFirstChildElement()->GetFloatValue() == 432.1f &&
			ParentObj->GetFirstChildElement()->GetStringValue() == NULL);
		delete ParentObj;
	}
	{
		wprintf(_T("GeneralCheck5#Test of Fetching the Value of Invalid Type for String Value ..."));
		StkObject* Obj = new StkObject(_T("IntValue"), _T("Test"));
		Abort(Obj->GetIntValue() == 0 && Obj->GetFloatValue() == 0.0f && Obj->GetStringValue() != NULL);

		wprintf(_T("GeneralCheck5#Test of Fetching the Value of Invalid Type for String Value (Child Element) ..."));
		StkObject* ParentObj = new StkObject(_T("Parent"));
		ParentObj->AppendChildElement(Obj);
		ParentObj->GetFirstChildElement()->SetIntValue(999);
		ParentObj->GetFirstChildElement()->SetFloatValue(999.9f);
		ParentObj->GetFirstChildElement()->SetStringValue(_T("Hello"));
		Abort(ParentObj->GetFirstChildElement()->GetIntValue() == 0 &&
			ParentObj->GetFirstChildElement()->GetFloatValue() == 0.0f &&
			ParentObj->GetFirstChildElement()->GetStringValue() != NULL &&
			lstrcmp(ParentObj->GetFirstChildElement()->GetStringValue(), _T("Hello")) == 0);
		delete ParentObj;
	}
}

void GeneralTestCase6()
{
	wprintf(_T("GeneralCheck6#Identify the presented text is XML or JSON ..."));

	StkObject* Elem1 = MakeTestData1();
	std::wstring Xml1;
	std::wstring Json1;
	Elem1->ToXml(&Xml1);
	Elem1->ToJson(&Json1);
	int RetXml1 = StkObject::Analyze((TCHAR*)Xml1.c_str());
	int RetJson1 = StkObject::Analyze((TCHAR*)Json1.c_str());

	StkObject* Elem2 = MakeTestData2();
	std::wstring Xml2;
	std::wstring Json2;
	Elem2->ToXml(&Xml2);
	Elem2->ToJson(&Json2);
	int RetXml2 = StkObject::Analyze((TCHAR*)Xml2.c_str());
	int RetJson2 = StkObject::Analyze((TCHAR*)Json2.c_str());

	StkObject* Elem3 = MakeTestData3(_T("Hello"), 4, 5);
	std::wstring Xml3;
	std::wstring Json3;
	Elem2->ToXml(&Xml3);
	Elem2->ToJson(&Json3);
	int RetXml3 = StkObject::Analyze((TCHAR*)Xml3.c_str());
	int RetJson3 = StkObject::Analyze((TCHAR*)Json3.c_str());

	if (!(RetXml1 == 1 && RetJson1 == 2 && RetXml2 == 1 && RetJson2 == 2 && RetXml3 == 1 && RetJson3 == 2)) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");
}

void JsonEncodingTest1()
{
	StkObject* Obj = new StkObject(_T("EncodeTesting"));
	Obj->AppendChildElement(new StkObject(_T("Element1"), _T("Hello-\\\\\\\\\\-World!!")));
	Obj->AppendChildElement(new StkObject(_T("Element2"), _T("Hello-\"\"\"\"\"-World!!")));
	Obj->AppendChildElement(new StkObject(_T("Element3"), _T("Hello-/////-World!!")));
	Obj->AppendChildElement(new StkObject(_T("Element4"), _T("Hello-\b\b\b\b\b-World!!")));
	Obj->AppendChildElement(new StkObject(_T("Element5"), _T("Hello-\f\f\f\f\f-World!!")));
	Obj->AppendChildElement(new StkObject(_T("Element6"), _T("Hello-\r\r\r\r\r-World!!")));
	Obj->AppendChildElement(new StkObject(_T("Element7"), _T("Hello-\n\n\n\n\n-World!!")));
	Obj->AppendChildElement(new StkObject(_T("Element8"), _T("Hello-\t\t\t\t\t-World!!")));
	std::wstring JsonTxt;
	Obj->ToJson(&JsonTxt);
	wprintf(_T("JSON Encoding#Escape ... "));
	if (JsonTxt.find(_T("\"Element1\" : \"Hello-\\\\\\\\\\\\\\\\\\\\-World!!\"")) == std::wstring::npos ||
		JsonTxt.find(_T("\"Element2\" : \"Hello-\\\"\\\"\\\"\\\"\\\"-World!!\"")) == std::wstring::npos ||
		JsonTxt.find(_T("\"Element3\" : \"Hello-\\/\\/\\/\\/\\/-World!!\"")) == std::wstring::npos ||
		JsonTxt.find(_T("\"Element4\" : \"Hello-\\b\\b\\b\\b\\b-World!!\"")) == std::wstring::npos ||
		JsonTxt.find(_T("\"Element5\" : \"Hello-\\f\\f\\f\\f\\f-World!!\"")) == std::wstring::npos ||
		JsonTxt.find(_T("\"Element6\" : \"Hello-\\r\\r\\r\\r\\r-World!!\"")) == std::wstring::npos ||
		JsonTxt.find(_T("\"Element7\" : \"Hello-\\n\\n\\n\\n\\n-World!!\"")) == std::wstring::npos ||
		JsonTxt.find(_T("\"Element8\" : \"Hello-\\t\\t\\t\\t\\t-World!!\"")) == std::wstring::npos) {
		printf("NG\r\n");
		exit(0);
	} else {
		printf("OK\r\n");
	}
	delete Obj;
}

void XmlEncodingTest2()
{
	StkObject* Xml2 = new StkObject(_T("EncodeTesting"));
	Xml2->AppendAttribute(new StkObject(_T("Lt"), _T("<")));
	Xml2->AppendAttribute(new StkObject(_T("Gt"), _T(">")));
	Xml2->AppendAttribute(new StkObject(_T("Amp"), _T("&")));
	Xml2->AppendAttribute(new StkObject(_T("Quot"), _T("\"")));
	Xml2->AppendAttribute(new StkObject(_T("Apos"), _T("\'")));
	Xml2->AppendChildElement(new StkObject(_T("Element"), _T("<")));
	Xml2->AppendChildElement(new StkObject(_T("Element"), _T(">")));
	Xml2->AppendChildElement(new StkObject(_T("Element"), _T("&")));
	Xml2->AppendChildElement(new StkObject(_T("Element"), _T("\"")));
	Xml2->AppendChildElement(new StkObject(_T("Element"), _T("\'")));

	StkObject* Attr1 = Xml2->GetFirstAttribute();
	std::wstring Attr1Val;
	Attr1->ToXml(&Attr1Val);
	wprintf(_T("XmlEncoding(Single)#Attribute value '<', '>', '&', '\"' and '\'' are escaped ... "));
	if (Attr1Val.find(_T("Lt=\"&lt;\"")) == std::wstring::npos ||
		Attr1Val.find(_T("Gt=\"&gt;\"")) == std::wstring::npos ||
		Attr1Val.find(_T("Amp=\"&amp;\"")) == std::wstring::npos ||
		Attr1Val.find(_T("Quot=\"&quot;\"")) == std::wstring::npos ||
		Attr1Val.find(_T("Apos=\"&apos;\"")) == std::wstring::npos) {
		printf("NG\r\n");
		exit(0);
	} else {
		printf("OK\r\n");
	}

	StkObject* Elem1 = Xml2->GetFirstChildElement();
	std::wstring Elem1Val;
	Elem1->ToXml(&Elem1Val);
	wprintf(_T("XmlEncoding(Single)#Element value '<', '>', '&', '\"' and '\'' are escaped ... "));
	if (Elem1Val.find(_T(">&lt;<")) == std::wstring::npos ||
		Elem1Val.find(_T(">&gt;<")) == std::wstring::npos ||
		Elem1Val.find(_T(">&amp;<")) == std::wstring::npos ||
		Elem1Val.find(_T(">&quot;<")) == std::wstring::npos ||
		Elem1Val.find(_T(">&apos;<")) == std::wstring::npos) {
		printf("NG\r\n");
		exit(0);
	} else {
		printf("OK\r\n");
	}
	delete Xml2;
}

void XmlEncodingTest3()
{
	StkObject* Xml2 = new StkObject(_T("EncodeTesting"));
	Xml2->AppendAttribute(new StkObject(_T("Lt"), _T("<<<<<<<<<<")));
	Xml2->AppendAttribute(new StkObject(_T("Gt"), _T(">>>>>>>>>>")));
	Xml2->AppendAttribute(new StkObject(_T("Amp"), _T("&&&&&&&&&&")));
	Xml2->AppendAttribute(new StkObject(_T("Quot"), _T("\"\"\"\"\"\"\"\"\"\"")));
	Xml2->AppendAttribute(new StkObject(_T("Apos"), _T("\'\'\'\'\'\'\'\'\'\'")));
	Xml2->AppendChildElement(new StkObject(_T("Element"), _T("<<<<<<<<<<")));
	Xml2->AppendChildElement(new StkObject(_T("Element"), _T(">>>>>>>>>>")));
	Xml2->AppendChildElement(new StkObject(_T("Element"), _T("&&&&&&&&&&")));
	Xml2->AppendChildElement(new StkObject(_T("Element"), _T("\"\"\"\"\"\"\"\"\"\"")));
	Xml2->AppendChildElement(new StkObject(_T("Element"), _T("\'\'\'\'\'\'\'\'\'\'")));

	StkObject* Attr1 = Xml2->GetFirstAttribute();
	std::wstring Attr1Val;
	Attr1->ToXml(&Attr1Val);
	wprintf(_T("XmlEncoding(Multi)#Attribute value '<', '>', '&', '\"' and '\'' are escaped ... "));
	if (Attr1Val.find(_T("Lt=\"&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;\"")) == std::wstring::npos ||
		Attr1Val.find(_T("Gt=\"&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;\"")) == std::wstring::npos ||
		Attr1Val.find(_T("Amp=\"&amp;&amp;&amp;&amp;&amp;&amp;&amp;&amp;&amp;&amp;\"")) == std::wstring::npos ||
		Attr1Val.find(_T("Quot=\"&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;\"")) == std::wstring::npos ||
		Attr1Val.find(_T("Apos=\"&apos;&apos;&apos;&apos;&apos;&apos;&apos;&apos;&apos;&apos;\"")) == std::wstring::npos) {
		printf("NG\r\n");
		exit(0);
	} else {
		printf("OK\r\n");
	}

	StkObject* Elem1 = Xml2->GetFirstChildElement();
	std::wstring Elem1Val;
	Elem1->ToXml(&Elem1Val);
	wprintf(_T("XmlEncoding(Multi)#Element value '<', '>', '&', '\"' and '\'' are escaped ... "));
	if (Elem1Val.find(_T(">&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;<")) == std::wstring::npos ||
		Elem1Val.find(_T(">&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;<")) == std::wstring::npos ||
		Elem1Val.find(_T(">&amp;&amp;&amp;&amp;&amp;&amp;&amp;&amp;&amp;&amp;<")) == std::wstring::npos ||
		Elem1Val.find(_T(">&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;<")) == std::wstring::npos ||
		Elem1Val.find(_T(">&apos;&apos;&apos;&apos;&apos;&apos;&apos;&apos;&apos;&apos;<")) == std::wstring::npos) {
		printf("NG\r\n");
		exit(0);
	} else {
		printf("OK\r\n");
	}
	delete Xml2;
}

void XmlJsonEncodingTest1()
{
	StkObject* TopElem = new StkObject(_T("House"));

	StkObject* ChildElem1 = new StkObject(_T("KitchenRoom"));
	ChildElem1->AppendChildElement(new StkObject(_T("Furniture"), _T("Kitchen Table")));
	ChildElem1->AppendChildElement(new StkObject(_T("Furniture"), _T("Gas Cooker")));
	ChildElem1->AppendChildElement(new StkObject(_T("Furniture"), _T("Kitchen Rack")));

	StkObject* ChildElem2 = new StkObject(_T("KidsRoom"));

	StkObject* ChildElem2a = new StkObject(_T("Kid"));
	ChildElem2a->AppendChildElement(new StkObject(_T("Furniture"), _T("Desk")));
	ChildElem2a->AppendChildElement(new StkObject(_T("Furniture"), _T("Chair")));
	ChildElem2a->AppendChildElement(new StkObject(_T("Furniture"), _T("Bed")));
	ChildElem2->AppendChildElement(ChildElem2a);

	StkObject* ChildElem2b = new StkObject(_T("Kid"));
	ChildElem2b->AppendChildElement(new StkObject(_T("Furniture"), _T("Wood Desk")));
	ChildElem2b->AppendChildElement(new StkObject(_T("Furniture"), _T("Wood Chair")));
	ChildElem2b->AppendChildElement(new StkObject(_T("Furniture"), _T("Wood Bed")));
	StkObject* ChildElem2bx = new StkObject(_T("WalkInCloset"));
	ChildElem2bx->AppendChildElement(new StkObject(_T("Hunger"), _T("Wood")));
	ChildElem2bx->AppendChildElement(new StkObject(_T("Rack"), _T("Wood")));
	ChildElem2b->AppendChildElement(ChildElem2bx);
	ChildElem2->AppendChildElement(ChildElem2b);

	StkObject* ChildElem2c = new StkObject(_T("Kid"));
	ChildElem2c->AppendChildElement(new StkObject(_T("Furniture"), _T("Steel Desk")));
	ChildElem2c->AppendChildElement(new StkObject(_T("Furniture"), _T("Steel Chair")));
	ChildElem2c->AppendChildElement(new StkObject(_T("Furniture"), _T("Steel Bed")));
	ChildElem2->AppendChildElement(ChildElem2c);

	TopElem->AppendChildElement(ChildElem1);
	TopElem->AppendChildElement(ChildElem2);

	std::wstring XmlTxt;
	std::wstring JsonTxt;
	TopElem->ToXml(&XmlTxt);
	TopElem->ToJson(&JsonTxt);

	int Offset;
	StkObject* ObjFromXml  = StkObject::CreateObjectFromXml((TCHAR*)XmlTxt.c_str(), &Offset);
	StkObject* ObjFromJson = StkObject::CreateObjectFromJson((TCHAR*)JsonTxt.c_str(), &Offset);
	std::wstring Temp1;
	std::wstring Temp2;
	printf("[Obj--->XML--->Obj--->JSON] and [Obj--->JSON--->Obj--->JSON] bring the same result...");
	ObjFromXml->ToJson(&Temp1);
	ObjFromJson->ToJson(&Temp2);
	if (lstrcmp(Temp1.c_str(), Temp2.c_str()) != 0) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");

	printf("[Obj--->XML--->Obj--->XML] and [Obj--->JSON--->Obj--->XML] bring the same result...");
	ObjFromXml->ToXml(&Temp1);
	ObjFromJson->ToXml(&Temp2);
	if (lstrcmp(Temp1.c_str(), Temp2.c_str()) != 0) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");

	delete TopElem;
}

void XmlJsonEncodingTest2()
{
	std::wstring Temp1;
	std::wstring Temp2;

	StkObject* TopElem = new StkObject(_T("Japan"));
	TopElem->AppendAttribute(new StkObject(_T(""), _T("")));
	TopElem->AppendAttribute(new StkObject(_T("A"), _T("B")));
	StkObject* ChildElem = new StkObject(_T(""));
	ChildElem->AppendChildElement(new StkObject(_T("")));
	ChildElem->AppendChildElement(new StkObject(_T(""), _T("")));
	ChildElem->AppendChildElement(new StkObject(_T("A"), _T("B")));
	TopElem->AppendChildElement(ChildElem);
	TopElem->ToXml(&Temp1);
	TopElem->ToJson(&Temp2);
	printf("XML encoding with no name and no value...");
	if (Temp1.find(_T("<Japan =\"\" A=\"B\">\r\n  <>\r\n    </>\r\n    <></>\r\n    <A>B</A>\r\n  </>\r\n</Japan>")) == std::wstring::npos) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");
	printf("JSON encoding with no name and no value...");
	if (Temp2.find(_T("\"Japan\" : {\r\n  \"@attributes\" : {\r\n    \"\" : \"\", \"A\" : \"B\"\r\n  },\r\n  \"\" : {\r\n    \"\" : {\r\n    },\r\n    \"\" : \"\",\r\n    \"A\" : \"B\"\r\n  }\r\n}")) == std::wstring::npos) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");
	delete TopElem;
}

void XmlDecodingTest1()
{
	int Offset = 0;

	TCHAR* Xml1 = _T("<?xml version=\"1.0\"?><!-- comment --><Aaaa A1=\"a1\" A2=\"a2\" A3=\"a3\"><Bbbb B1=\"b1\" B2=\"b2\" B3=\"b3\"/><Cccc C1=\"c1\" C2=\"c2\" C3=\"c3\"/></Aaaa>");
	TCHAR* Xml2 = _T("<?xml version=\"1.0\"?><!-- comment --><Aaaa A1=\"a1\" A2=\"a2\" A3=\"a3\"><Bbbb B1=\"b1\" B2=\"b2\" B3=\"b3\"></Bbbb><Cccc C1=\"c1\" C2=\"c2\" C3=\"c3\"></Cccc></Aaaa>");
	TCHAR* Xml3 = _T("  <  Aaaa  A1  =  \"a1\"  A2  =  \"a2\"  A3  =  \"a3\"  >  <  Bbbb  B1  =  \"b1\"  B2  =  \"b2\"  B3  =  \"b3\"  /  >  <  Cccc  C1  =  \"c1\"  C2  =  \"c2\"  C3  =  \"c3\"  /  >  <  /  Aaaa  >  ");
	TCHAR* Xml4 = _T("\t<\tAaaa\tA1\t=\t\"a1\"\tA2\t=\t\"a2\"\tA3\t=\t\"a3\"\t>\t<\tBbbb\tB1\t=\t\"b1\"\tB2\t=\t\"b2\"\tB3\t=\t\"b3\"\t/\t>\t<\tCccc\tC1\t=\t\"c1\"\tC2\t=\t\"c2\"\tC3\t=\t\"c3\"\t/\t>\t<\t/\tAaaa\t>\t");
	TCHAR* Xml5 = _T("\r\n<\r\nAaaa\r\nA1\r\n=\r\n\"a1\"\r\nA2\r\n=\r\n\"a2\"\r\nA3\r\n=\r\n\"a3\"\r\n>\r\n<\r\nBbbb\r\nB1\r\n=\r\n\"b1\"\r\nB2\r\n=\r\n\"b2\"\r\nB3\r\n=\r\n\"b3\"\r\n/\r\n>\r\n<\r\nCccc\r\nC1\r\n=\r\n\"c1\"\r\nC2\r\n=\r\n\"c2\"\r\nC3\r\n=\r\n\"c3\"\r\n/\r\n>\r\n<\r\n/\r\nAaaa\r\n>\r\n");

	StkObject* RetObj1 = StkObject::CreateObjectFromXml(Xml1, &Offset);
	StkObject* RetObj2 = StkObject::CreateObjectFromXml(Xml2, &Offset);
	StkObject* RetObj3 = StkObject::CreateObjectFromXml(Xml3, &Offset);
	StkObject* RetObj4 = StkObject::CreateObjectFromXml(Xml4, &Offset);
	StkObject* RetObj5 = StkObject::CreateObjectFromXml(Xml5, &Offset);

	std::wstring Msg1;
	std::wstring Msg2;
	std::wstring Msg3;
	std::wstring Msg4;
	std::wstring Msg5;

	RetObj1->ToXml(&Msg1);
	RetObj2->ToXml(&Msg2);
	RetObj3->ToXml(&Msg3);
	RetObj4->ToXml(&Msg4);
	RetObj5->ToXml(&Msg5);

	printf("Decoding variation test (sampling check)...");
	if (lstrcmp(RetObj1->GetFirstChildElement()->GetNext()->GetFirstAttribute()->GetStringValue(), _T("c1")) != 0 ||
		lstrcmp(RetObj2->GetFirstChildElement()->GetNext()->GetFirstAttribute()->GetStringValue(), _T("c1")) != 0 ||
		lstrcmp(RetObj3->GetFirstChildElement()->GetNext()->GetFirstAttribute()->GetStringValue(), _T("c1")) != 0 ||
		lstrcmp(RetObj4->GetFirstChildElement()->GetNext()->GetFirstAttribute()->GetStringValue(), _T("c1")) != 0 ||
		lstrcmp(RetObj5->GetFirstChildElement()->GetNext()->GetFirstAttribute()->GetStringValue(), _T("c1")) != 0) {
		printf("NG\r\n");
		exit(0);
	} else {
		printf("OK\r\n");
	}

	printf("Decoding variation test (ToXml check)...");
	if (Msg1.compare(Msg2.c_str()) != 0 || Msg1.compare(Msg3.c_str()) != 0 || Msg1.compare(Msg4.c_str()) != 0 || Msg1.compare(Msg5.c_str()) != 0) {
		printf("NG\r\n");
		exit(0);
	} else {
		printf("OK\r\n");
	}

	delete RetObj1;
	delete RetObj2;
	delete RetObj3;
	delete RetObj4;
	delete RetObj5;
}

void XmlDecodingTest2()
{
	printf("<Aaaa>=</Aaaa> ; <Aaaa>/</Aaaa> ; <Aaaa>?</Aaaa> ; <Aaaa>!</Aaaa> ; ---> Element value can be acquired...");
	int Offset = 0;

	TCHAR* Xml1 = _T("<Aaaa>=</Aaaa>");
	TCHAR* Xml2 = _T("<Aaaa>/</Aaaa>");
	TCHAR* Xml3 = _T("<Aaaa>?</Aaaa>");
	TCHAR* Xml4 = _T("<Aaaa>!</Aaaa>");

	StkObject* RetObj1 = StkObject::CreateObjectFromXml(Xml1, &Offset);
	StkObject* RetObj2 = StkObject::CreateObjectFromXml(Xml2, &Offset);
	StkObject* RetObj3 = StkObject::CreateObjectFromXml(Xml3, &Offset);
	StkObject* RetObj4 = StkObject::CreateObjectFromXml(Xml4, &Offset);

	if (lstrcmp(RetObj1->GetStringValue(), _T("=")) != 0) {
		printf("NG\r\n");
		exit(0);
	}
	if (lstrcmp(RetObj2->GetStringValue(), _T("/")) != 0) {
		printf("NG\r\n");
		exit(0);
	}
	if (lstrcmp(RetObj3->GetStringValue(), _T("?")) != 0) {
		printf("NG\r\n");
		exit(0);
	}
	if (lstrcmp(RetObj4->GetStringValue(), _T("!")) != 0) {
		printf("NG\r\n");
		exit(0);
	}

	delete RetObj1;
	delete RetObj2;
	delete RetObj3;
	delete RetObj4;
	printf("OK\r\n");
}

void XmlDecodingTest3(TCHAR* Name)
{
	TCHAR* Xml = new TCHAR[512];
	lstrcpy(Xml, _T("<Aaaa Lt=\"&lt;\" Gt=\"&gt;\" Amp=\"&amp;\" Quot=\"&quot;\" Apos=\"&apos;\"><Bbbb>&lt;</Bbbb><Bbbb>&gt;</Bbbb><Bbbb>&amp;</Bbbb><Bbbb>&quot;</Bbbb><Bbbb>&apos;</Bbbb></Aaaa>"));

	int Offset = 0;
	StkObject* Elem =StkObject::CreateObjectFromXml(Xml, &Offset);
	StkObject* Attr1 = Elem->GetFirstAttribute();
	StkObject* Attr2 = Attr1->GetNext();
	StkObject* Attr3 = Attr2->GetNext();
	StkObject* Attr4 = Attr3->GetNext();
	StkObject* Attr5 = Attr4->GetNext();
	StkObject* Elem1 = Elem->GetFirstChildElement();
	StkObject* Elem2 = Elem1->GetNext();
	StkObject* Elem3 = Elem2->GetNext();
	StkObject* Elem4 = Elem3->GetNext();
	StkObject* Elem5 = Elem4->GetNext();

	wprintf(_T("%s#Attribute value '&lt;', '&gt;', '&amp;', '&quot;' and '&apos;' are decoded appropriately... "), Name);
	if (lstrcmp(Attr1->GetStringValue(), _T("<")) != 0 ||
		lstrcmp(Attr2->GetStringValue(), _T(">")) != 0 ||
		lstrcmp(Attr3->GetStringValue(), _T("&")) != 0 ||
		lstrcmp(Attr4->GetStringValue(), _T("\"")) != 0 ||
		lstrcmp(Attr5->GetStringValue(), _T("\'")) != 0) {
		printf("NG\r\n");
		exit(0);
	} else {
		printf("OK\r\n");
	}

	wprintf(_T("%s#Element value '&lt;', '&gt;', '&amp;', '&quot;' and '&apos;' are decoded appropriately... "), Name);
	if (lstrcmp(Elem1->GetStringValue(), _T("<")) != 0 ||
		lstrcmp(Elem2->GetStringValue(), _T(">")) != 0 ||
		lstrcmp(Elem3->GetStringValue(), _T("&")) != 0 ||
		lstrcmp(Elem4->GetStringValue(), _T("\"")) != 0 ||
		lstrcmp(Elem5->GetStringValue(), _T("\'")) != 0) {
		printf("NG\r\n");
		exit(0);
	} else {
		printf("OK\r\n");
	}

	delete Elem;
	delete Xml;
}

void XmlDecodingTest4(TCHAR* Name)
{
	TCHAR* Xml = new TCHAR[512];
	lstrcpy(Xml, _T("<Aaaa Lt=\"&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;\" Apos=\"&apos;&apos;&apos;&apos;&apos;&apos;&apos;&apos;&apos;&apos;\"><Bbbb>&amp;&amp;&amp;&amp;&amp;&amp;&amp;&amp;&amp;&amp;</Bbbb><Bbbb>&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;</Bbbb></Aaaa>"));

	int Offset = 0;
	StkObject* Elem = StkObject::CreateObjectFromXml(Xml, &Offset);
	StkObject* Attr1 = Elem->GetFirstAttribute();
	StkObject* Attr2 = Attr1->GetNext();
	StkObject* Elem1 = Elem->GetFirstChildElement();
	StkObject* Elem2 = Elem1->GetNext();

	wprintf(_T("%s#Attribute value '&lt;...&lt;' and '&apos;...&apos;' are decoded appropriately... "), Name);
	if (lstrcmp(Attr1->GetStringValue(), _T("<<<<<<<<<<")) != 0 ||
		lstrcmp(Attr2->GetStringValue(), _T("\'\'\'\'\'\'\'\'\'\'")) != 0) {
		printf("NG\r\n");
		exit(0);
	} else {
		printf("OK\r\n");
	}

	wprintf(_T("%s#Element value '&amp;...&amp;' and '&quot;...&quot;' are decoded appropriately... "), Name);
	if (lstrcmp(Elem1->GetStringValue(), _T("&&&&&&&&&&")) != 0 ||
		lstrcmp(Elem2->GetStringValue(), _T("\"\"\"\"\"\"\"\"\"\"")) != 0) {
		printf("NG\r\n");
		exit(0);
	} else {
		printf("OK\r\n");
	}

	delete Elem;
	delete Xml;
}

void XmlDecordingAbnormalTest1()
{
	AbnormalCaseForXml(_T(""), StkObject::ERROR_XML_NO_ELEMENT_FOUND);
	AbnormalCaseForXml(NULL, StkObject::ERROR_XML_NO_ELEMENT_FOUND);
	AbnormalCaseForXml(_T("<>"), StkObject::ERROR_XML_INVALID_ELEMENT_END_FOUND);
	AbnormalCaseForXml(_T("<<"), StkObject::ERROR_XML_INVALID_ELEMENT_START_FOUND);
	AbnormalCaseForXml(_T("<\"ABC\"/>"), StkObject::ERROR_XML_INVALID_QUOT_FOUND);
	AbnormalCaseForXml(_T("xyz"), StkObject::ERROR_XML_CANNOT_HANDLE);
	AbnormalCaseForXml(_T("<Aaa Bbb=Ccc/>"), StkObject::ERROR_XML_CANNOT_HANDLE);
	AbnormalCaseForXml(_T("<Aaa Bbb Ccc=\"Ddd\"/>"), StkObject::ERROR_XML_CANNOT_HANDLE);
	AbnormalCaseForXml(_T("<Aaa Bbb=></Aaa>"), StkObject::ERROR_XML_INVALID_ELEMENT_END_FOUND);
	AbnormalCaseForXml(_T("<Aaa =></Aaa>"), StkObject::ERROR_XML_EQUAL_FOUND_WITHOUT_ATTRIBUTE_NAME);
	AbnormalCaseForXml(_T("<Aaa></Aaa"), StkObject::ERROR_XML_SLASH_FOUND_WITHOUT_ELEMENT_END);
	AbnormalCaseForXml(_T("<Aaa>/Aaa"), StkObject::ERROR_XML_ELEMENT_END_NOT_FOUND);
	AbnormalCaseForXml(_T("</Aaa>"), StkObject::ERROR_XML_SLASH_FOUND_WITHOUT_ELEMENT);
	AbnormalCaseForXml(_T("<Aaaa"), StkObject::ERROR_XML_ELEMENT_END_NOT_FOUND);
	AbnormalCaseForXml(_T("<Aaaa Xxx=\"Xxx"), StkObject::ERROR_XML_ELEMENT_END_NOT_FOUND);
	AbnormalCaseForXml(_T("<?xml?></?xml?>"), StkObject::ERROR_XML_SLASH_FOUND_WITHOUT_ELEMENT);
	AbnormalCaseForXml(_T("<Aaa><Bbb><Cc"), StkObject::ERROR_XML_ELEMENT_END_NOT_FOUND);
	AbnormalCaseForXml(_T("<Aaa>&"), StkObject::ERROR_XML_ELEMENT_END_NOT_FOUND);
	AbnormalCaseForXml(_T("/<Aaa></Aaa>"), StkObject::ERROR_XML_INVALID_SLASH_FOUND);
}

void JsonDecodingTest1()
{
	TCHAR Msg[512];
	int Offset;
	StkObject* RetObj;

	////////////////////////////////////////////////////
	lstrcpy(Msg, _T("\"Aaa\" : { \"Bbb\" : \"This is a test.\", \"Ccc\" : 123, \"Ddd\" : 999.9 }"));
	wprintf(_T("JSON Decoding : %s ..."), Msg);
	RetObj = StkObject::CreateObjectFromJson(Msg, &Offset);
	if (RetObj == NULL || RetObj->GetChildElementCount() != 3 || lstrcmp(RetObj->GetName(), _T("Aaa")) != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetStringValue(), _T("This is a test.")) != 0 ||
		RetObj->GetFirstChildElement()->GetNext()->GetNext()->GetFloatValue() != 999.9f) {
		printf("NG\r\n");
		exit(0);
	}
	delete RetObj;
	printf("OK\r\n");
	////////////////////////////////////////////////////
	lstrcpy(Msg, _T("\"Xxx\" : { \"Aaa\" : { \"Bbb\" : \"This is a test.\", \"Ccc\" : 123, \"Ddd\" : { \"D1\" : 0, \"D2\" : {\"D3a\" : \"test\"}, \"D3\" : 2}, \"Eee\" : 999.9 }}"));
	wprintf(_T("JSON Decoding : %s ..."), Msg);
	RetObj = StkObject::CreateObjectFromJson(Msg, &Offset);
	if (RetObj == NULL ||
		lstrcmp(RetObj->GetFirstChildElement()->GetFirstChildElement()->GetNext()->GetNext()->GetFirstChildElement()->GetName(), _T("D1")) != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetFirstChildElement()->GetNext()->GetNext()->GetFirstChildElement()->GetNext()->GetFirstChildElement()->GetStringValue(), _T("test")) != 0 ||
		RetObj->GetFirstChildElement()->GetFirstChildElement()->GetNext()->GetNext()->GetNext()->GetFloatValue() != 999.9f) {
		printf("NG\r\n");
		exit(0);
	}
	delete RetObj;
	printf("OK\r\n");
	////////////////////////////////////////////////////
	lstrcpy(Msg, _T("\"Yyy\" : {\"Xxx\" : [\"Aaa\", \"Bbb\", \"Ccc\"]}"));
	wprintf(_T("JSON Decoding : %s ..."), Msg);
	RetObj = StkObject::CreateObjectFromJson(Msg, &Offset);
	if (RetObj == NULL ||
		lstrcmp(RetObj->GetFirstChildElement()->GetName(), _T("Xxx")) != 0 || lstrcmp(RetObj->GetFirstChildElement()->GetStringValue(), _T("Aaa")) != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetNext()->GetNext()->GetName(), _T("Xxx")) != 0 || lstrcmp(RetObj->GetFirstChildElement()->GetNext()->GetNext()->GetStringValue(), _T("Ccc")) != 0) {
		printf("NG\r\n");
		exit(0);
	}
	delete RetObj;
	printf("OK\r\n");
	////////////////////////////////////////////////////
	lstrcpy(Msg, _T("\"Yyy\" : {\"Xxx\" : [{\"Aaa\" : 123, \"Bbb\" : 456, \"Ccc\":789},{\"Aaa\" : [333, 222, 111]}]}"));
	wprintf(_T("JSON Decoding : %s ..."), Msg);
	RetObj = StkObject::CreateObjectFromJson(Msg, &Offset);
	if (RetObj == NULL ||
		lstrcmp(RetObj->GetFirstChildElement()->GetFirstChildElement()->GetName(), _T("Aaa")) != 0 || RetObj->GetFirstChildElement()->GetFirstChildElement()->GetNext()->GetIntValue() != 456 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetNext()->GetFirstChildElement()->GetNext()->GetName(), _T("Aaa")) != 0 || RetObj->GetFirstChildElement()->GetNext()->GetFirstChildElement()->GetNext()->GetIntValue() != 222) {
		printf("NG\r\n");
		exit(0);
	}
	delete RetObj;
	printf("OK\r\n");
	////////////////////////////////////////////////////
	lstrcpy(Msg, _T("{ \"Aaa\" : { \"Bbb\" : \"This is a test.\", \"Ccc\" : 123, \"Ddd\" : { \"D1\" : 0, \"D2\" : {\"D3a\" : \"test\"}, \"D3\" : 2}, \"Eee\" : 999.9 }}"));
	wprintf(_T("JSON Decoding : %s ..."), Msg);
	RetObj = StkObject::CreateObjectFromJson(Msg, &Offset);
	if (RetObj == NULL ||
		lstrcmp(RetObj->GetFirstChildElement()->GetFirstChildElement()->GetNext()->GetNext()->GetFirstChildElement()->GetName(), _T("D1")) != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetFirstChildElement()->GetNext()->GetNext()->GetFirstChildElement()->GetNext()->GetFirstChildElement()->GetStringValue(), _T("test")) != 0 ||
		RetObj->GetFirstChildElement()->GetFirstChildElement()->GetNext()->GetNext()->GetNext()->GetFloatValue() != 999.9f) {
		printf("NG\r\n");
		exit(0);
	}
	delete RetObj;
	printf("OK\r\n");
	////////////////////////////////////////////////////
	lstrcpy(Msg, _T("{\"Xxx\" : [{\"Aaa\" : 123, \"Bbb\" : 456, \"Ccc\":789},{\"Aaa\" : [333, 222, 111]}]}"));
	wprintf(_T("JSON Decoding : %s ..."), Msg);
	RetObj = StkObject::CreateObjectFromJson(Msg, &Offset);
	if (RetObj == NULL ||
		lstrcmp(RetObj->GetFirstChildElement()->GetFirstChildElement()->GetName(), _T("Aaa")) != 0 || RetObj->GetFirstChildElement()->GetFirstChildElement()->GetNext()->GetIntValue() != 456 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetNext()->GetFirstChildElement()->GetNext()->GetName(), _T("Aaa")) != 0 || RetObj->GetFirstChildElement()->GetNext()->GetFirstChildElement()->GetNext()->GetIntValue() != 222) {
		printf("NG\r\n");
		exit(0);
	}
	delete RetObj;
	printf("OK\r\n");
}

void JsonDecodingTest2()
{
	TCHAR Msg1[512];
	TCHAR Msg2[512];
	TCHAR Msg3[512];
	TCHAR Msg4[512];
	StkObject* RetObj1;
	StkObject* RetObj2;
	StkObject* RetObj3;
	StkObject* RetObj4;
	std::wstring Str1;
	std::wstring Str2;
	std::wstring Str3;
	std::wstring Str4;
	int Offset;

	////////////////////////////////////////////////////
	lstrcpy(Msg1, _T("\"Aaa\":{\"Bbb\":[{\"Xxx\":123,\"Bbb\":456,\"Ccc\":789},\"test\",0.1]}"));
	lstrcpy(Msg2, _T(    "\"Aaa\"    :    {    \"Bbb\"    :    [    {    \"Xxx\"    :    123    ,    \"Bbb\"    :    456    ,    \"Ccc\"    :    789    }    ,    \"test\"    ,    0.1    ]    }    "));
	lstrcpy(Msg3, _T("\t\t\"Aaa\"\t\t:\t\t{\t\t\"Bbb\"\t\t:\t\t[\t\t{\t\t\"Xxx\"\t\t:\t\t123\t\t,\t\t\"Bbb\"\t\t:\t\t456\t\t,\t\t\"Ccc\"\t\t:\t\t789\t\t}\t\t,\t\t\"test\"\t\t,\t\t0.1\t\t]\t\t}\t\t"));
	lstrcpy(Msg4, _T("\r\n\"Aaa\"\r\n:\r\n{\r\n\"Bbb\"\r\n:\r\n[\r\n{\r\n\"Xxx\"\r\n:\r\n123\r\n,\r\n\"Bbb\"\r\n:\r\n456\r\n,\r\n\"Ccc\"\r\n:\r\n789\r\n}\r\n,\r\n\"test\"\r\n,\r\n0.1\r\n]\r\n}\r\n"));
	RetObj1 = StkObject::CreateObjectFromJson(Msg1, &Offset);
	RetObj2 = StkObject::CreateObjectFromJson(Msg2, &Offset);
	RetObj3 = StkObject::CreateObjectFromJson(Msg3, &Offset);
	RetObj4 = StkObject::CreateObjectFromJson(Msg4, &Offset);
	RetObj1->ToJson(&Str1);
	RetObj2->ToJson(&Str2);
	RetObj3->ToJson(&Str3);
	RetObj4->ToJson(&Str4);
	printf("JSON Decoding (empty charactor)...");
	if (lstrcmp(Str1.c_str(), Str2.c_str()) != 0 || lstrcmp(Str2.c_str(), Str3.c_str()) != 0 || lstrcmp(Str3.c_str(), Str4.c_str()) != 0) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");
	delete RetObj1;
	delete RetObj2;
	delete RetObj3;
	delete RetObj4;
}

void JsonDecordingAbnormalTest1()
{
	AbnormalCaseForJson(_T(""), StkObject::ERROR_JSON_NO_ELEMENT_FOUND);
	AbnormalCaseForJson(NULL, StkObject::ERROR_JSON_NO_ELEMENT_FOUND);
	AbnormalCaseForJson(_T("[ \"Aaa\" : \"Xxx\" ]"), StkObject::ERROR_JSON_INVALID_ARRAY_STRUCTURE);
	AbnormalCaseForJson(_T("\"Aaa\" : 123"), StkObject::ERROR_JSON_NO_ROOT_ELEMENT);
	AbnormalCaseForJson(_T("\"Aaa\" : 999.9"), StkObject::ERROR_JSON_NO_ROOT_ELEMENT);
	AbnormalCaseForJson(_T("\"Aaa\" : \"Xyz\""), StkObject::ERROR_JSON_NO_ROOT_ELEMENT);
	AbnormalCaseForJson(_T("\"Aaa\" : { \"Xyz\" : 123, \"Abc\" : "), StkObject::ERROR_JSON_INVALID_STRUCTURE);
	AbnormalCaseForJson(_T("\"Aaa\" : { \"Xyz\" : 123, \"Abc\" : \""), StkObject::ERROR_JSON_INVALID_STRUCTURE);
	AbnormalCaseForJson(_T("\"Aaa\" : { \"Xyz\" : 123, \"Abc\" \"Xyz\" }"), StkObject::ERROR_JSON_INVALID_QUOT_FOUND);
	AbnormalCaseForJson(_T("\"Aaa\" : { \"Bbb\" : 123, Ccc : 456 }"), StkObject::ERROR_JSON_CANNOT_HANDLE);
	AbnormalCaseForJson(_T("\"Aaa\" : { \"Bbb\" : 123, \"Ccc\" : 456, \"Ddd\" : 789 {"), StkObject::ERROR_JSON_INVALID_STRUCTURE);
	AbnormalCaseForJson(_T("\"Aaa\" : {}"), StkObject::ERROR_JSON_INVALID_STRUCTURE);
	AbnormalCaseForJson(_T(": { \"Aaa\" : \"Xxx\" }"), StkObject::ERROR_JSON_INVALID_COLON_FOUND);
}

void CloneTest(StkObject* Obj)
{
	std::wstring OrgMsg;
	Obj->ToXml(&OrgMsg);

	StkObject* NewObj1 = Obj->Clone();
	StkObject* NewObj2 = NewObj1->Clone();

	std::wstring Msg1;
	std::wstring Msg2;
	NewObj1->ToXml(&Msg1);
	NewObj2->ToXml(&Msg2);

	printf("Clone Test#Validation of two clone's string...");
	if (lstrcmp(Msg1.c_str(), Msg2.c_str()) == 0 && lstrcmp(Msg1.c_str(), OrgMsg.c_str()) == 0 && NewObj1->Equals(NewObj2) && NewObj1->Equals(Obj)) {
		printf("OK\r\n");
	} else {
		printf("NG\r\n");
		exit(0);
	}

	delete NewObj1;
	delete NewObj2;
}

int MemoryLeakChecking1(StkObject* PassedObj)
{
	printf("Checks memory leak (repeat cloning and encoding)...");
	long MaxMem[30];
	for (int CreationLoop = 0; CreationLoop < 30; CreationLoop++) {
		for (int Loop = 0; Loop < 1000; Loop++) {
			StkObject* NewObj = PassedObj->Clone();
			delete NewObj;
		}
		for (int Loop = 0; Loop < 50; Loop++) {
			std::wstring *Msg1 = new std::wstring(_T(""));
			PassedObj->ToXml(Msg1);
			delete Msg1;
			std::wstring *Msg2 = new std::wstring(_T(""));
			PassedObj->ToJson(Msg2);
			delete Msg2;
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
		printf("NG : %d %d %d %d %d %d %d %d %d\r\n", MaxMem[0], MaxMem[3], MaxMem[6], MaxMem[9], MaxMem[12], MaxMem[15], MaxMem[18], MaxMem[21], MaxMem[24]);
		exit(0);
	}
	printf("OK : %d %d %d %d %d %d %d %d %d\r\n", MaxMem[0], MaxMem[3], MaxMem[6], MaxMem[9], MaxMem[12], MaxMem[15], MaxMem[18], MaxMem[21], MaxMem[24]);
	return 0;
}

int MemoryLeakChecking2()
{
	printf("Checks memory leak (repeat encoding and decoding)...");
	long MaxMem[30];
	std::wstring StrVal;
	int Offset = 0;
	StkObject* NewObj1;
	StkObject* NewObj2;

	for (int CreationLoop = 0; CreationLoop < 30; CreationLoop++) {
		for (int Loop = 0; Loop < 250; Loop++) {
			NewObj1 = new StkObject(_T("EncodeTesting"));
			NewObj1->AppendAttribute(new StkObject(_T("XmlLt"), _T("<<<<<<<<<<<<<<<<<<<<")));
			NewObj1->AppendAttribute(new StkObject(_T("XmlGt"), _T(">>>>>>>>>>>>>>>>>>>>")));
			NewObj1->AppendAttribute(new StkObject(_T("XmlApos"), _T("\'\'\'\'\'\'\'\'\'\'\'\'\'\'\'\'\'\'\'\'")));
			NewObj1->AppendChildElement(new StkObject(_T("XmlAmp"), _T("&&&&&&&&&&&&&&&&&&&&")));
			NewObj1->AppendChildElement(new StkObject(_T("XmlQuot"), _T("\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"")));
			StrVal.assign(_T(""));
			NewObj1->ToXml(&StrVal);
			delete NewObj1;

			NewObj1 = new StkObject(_T("EncodeTesting"));
			NewObj1->AppendChildElement(new StkObject(_T("Bbbb"), _T("\"\"\"\"\"\"\"\"\"\"")));
			NewObj1->AppendChildElement(new StkObject(_T("Bbbb"), _T("\\\\\\\\\\\\\\\\\\\\")));
			NewObj1->AppendChildElement(new StkObject(_T("Bbbb"), _T("//////////")));
			NewObj1->AppendChildElement(new StkObject(_T("Bbbb"), _T("\b\b\b\b\b\b\b\b\b\b")));
			NewObj1->AppendChildElement(new StkObject(_T("Bbbb"), _T("\f\f\f\f\f\f\f\f\f\f")));
			NewObj1->AppendChildElement(new StkObject(_T("Bbbb"), _T("\n\n\n\n\n\n\n\n\n\n")));
			NewObj1->AppendChildElement(new StkObject(_T("Bbbb"), _T("\r\r\r\r\r\r\r\r\r\r")));
			NewObj1->AppendChildElement(new StkObject(_T("Bbbb"), _T("\t\t\t\t\t\t\t\t\t\t")));
			StrVal.assign(_T(""));
			NewObj1->ToJson(&StrVal);
			delete NewObj1;

			NewObj2 = StkObject::CreateObjectFromXml(_T("<Aaaa Lt=\"&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;\" Gt=\"&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;\" Apos=\"&apos;&apos;&apos;&apos;&apos;&apos;&apos;&apos;&apos;&apos;\"><Amp>&amp;&amp;&amp;&amp;&amp;&amp;&amp;&amp;&amp;&amp;</Amp><Quot>&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;</Quot></Aaaa>"), &Offset);
			StrVal.assign(_T(""));
			NewObj2->ToXml(&StrVal);
			delete NewObj2;

			NewObj2 = StkObject::CreateObjectFromJson(_T("\"Aaaa\" : { \"Bbbb\" : \"\\\"\\\"\\\"\\\"\\\"\\\"\\\"\\\"\\\"\\\"\", \"Bbbb\" : \"\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\", \"Bbbb\" : \"\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\", \"Bbbb\" : \"\\b\\b\\b\\b\\b\\b\\b\\b\\b\\b\", \"Bbbb\" : \"\\f\\f\\f\\f\\f\\f\\f\\f\\f\\f\", \"Bbbb\" : \"\\n\\n\\n\\n\\n\\n\\n\\n\\n\\n\", \"Bbbb\" : \"\\r\\r\\r\\r\\r\\r\\r\\r\\r\\r\", \"Bbbb\" : \"\\t\\t\\t\\t\\t\\t\\t\\t\\t\\t\" }"), &Offset);
			StrVal = _T("");
			NewObj2->ToJson(&StrVal);
			delete NewObj2;
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
		printf("NG : %d %d %d %d %d %d %d %d %d\r\n", MaxMem[0], MaxMem[3], MaxMem[6], MaxMem[9], MaxMem[12], MaxMem[15], MaxMem[18], MaxMem[21], MaxMem[24]);
		exit(0);
	}
	printf("OK : %d %d %d %d %d %d %d %d %d\r\n", MaxMem[0], MaxMem[3], MaxMem[6], MaxMem[9], MaxMem[12], MaxMem[15], MaxMem[18], MaxMem[21], MaxMem[24]);
	return 0;
}

int MemoryLeakChecking3()
{
	printf("Checks memory leak (repeat abnormal case)...");
	long MaxMem[30];
	int Offset = 0;

	for (int CreationLoop = 0; CreationLoop < 30; CreationLoop++) {
		for (int Loop = 0; Loop < 1000; Loop++) {
			// For XML
			StkObject::CreateObjectFromXml(_T(""), &Offset);
			StkObject::CreateObjectFromXml(NULL, &Offset);
			StkObject::CreateObjectFromXml(_T("<X><Y><Z><></X></Y></Z>"), &Offset);
			StkObject::CreateObjectFromXml(_T("<X><Y><Z><<</X></Y></Z>"), &Offset);
			StkObject::CreateObjectFromXml(_T("<X><Y><Z><\"ABC\"/></X></Y></Z>"), &Offset);
			StkObject::CreateObjectFromXml(_T("xyz"), &Offset);
			StkObject::CreateObjectFromXml(_T("<X><Y><Z><Aaa Bbb=Ccc/></X></Y></Z>"), &Offset);
			StkObject::CreateObjectFromXml(_T("<X><Y><Z><Aaa Bbb Ccc=\"Ddd\"/></X></Y></Z>"), &Offset);
			StkObject::CreateObjectFromXml(_T("<X><Y><Z><Aaa Bbb=></Aaa></X></Y></Z>"), &Offset);
			StkObject::CreateObjectFromXml(_T("<X><Y><Z><Aaa =></Aaa></X></Y></Z>"), &Offset);
			StkObject::CreateObjectFromXml(_T("<Aaa></Aaa"), &Offset);
			StkObject::CreateObjectFromXml(_T("<Aaa>/Aaa"), &Offset);
			StkObject::CreateObjectFromXml(_T("</Aaa>"), &Offset);
			StkObject::CreateObjectFromXml(_T("<X><Y><Z><Aaaa"), &Offset);
			StkObject::CreateObjectFromXml(_T("<X><Y><Z><Aaaa Xxx=\"Xxx"), &Offset);
			StkObject::CreateObjectFromXml(_T("<Aaa><Bbb><Cc"), &Offset);
			StkObject::CreateObjectFromXml(_T("<Aaa>&"), &Offset);
			StkObject::CreateObjectFromXml(_T("/<Aaa></Aaa>"), &Offset);

			// For JSON
			StkObject::CreateObjectFromJson(_T(""), &Offset);
			StkObject::CreateObjectFromJson(NULL, &Offset);
			StkObject::CreateObjectFromJson(_T("[ \"Aaa\" : \"Xxx\" ]"), &Offset);
			StkObject::CreateObjectFromJson(_T("\"Aaa\" : 123"), &Offset);
			StkObject::CreateObjectFromJson(_T("\"Aaa\" : 999.9"), &Offset);
			StkObject::CreateObjectFromJson(_T("\"Aaa\" : \"Xyz\""), &Offset);
			StkObject::CreateObjectFromJson(_T("\"Aaa\" : { \"Xyz\" : 123, \"Abc\" : "), &Offset);
			StkObject::CreateObjectFromJson(_T("\"Aaa\" : { \"Xyz\" : 123, \"Abc\" : \""), &Offset);
			StkObject::CreateObjectFromJson(_T("\"Aaa\" : { \"Xyz\" : 123, \"Abc\" \"Xyz\" }"), &Offset);
			StkObject::CreateObjectFromJson(_T("\"Aaa\" : { \"Bbb\" : 123, Ccc : 456 }"), &Offset);
			StkObject::CreateObjectFromJson(_T("\"Aaa\" : { \"Bbb\" : 123, \"Ccc\" : 456, \"Ddd\" : 789 {"), &Offset);
			StkObject::CreateObjectFromJson(_T("\"Aaa\" : {}"), &Offset);
			StkObject::CreateObjectFromJson(_T(": { \"Aaa\" : \"Xxx\" }"),  &Offset);
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
		printf("NG : %d %d %d %d %d %d %d %d %d\r\n", MaxMem[0], MaxMem[3], MaxMem[6], MaxMem[9], MaxMem[12], MaxMem[15], MaxMem[18], MaxMem[21], MaxMem[24]);
		exit(0);
	}
	printf("OK : %d %d %d %d %d %d %d %d %d\r\n", MaxMem[0], MaxMem[3], MaxMem[6], MaxMem[9], MaxMem[12], MaxMem[15], MaxMem[18], MaxMem[21], MaxMem[24]);
	return 0;
}

int MemoryLeakChecking4()
{
	printf("Checks memory leak (large data)...");
	long MaxMem[30];
	for (int CreationLoop = 0; CreationLoop < 30; CreationLoop++) {
		StkObject* NewObj = MakeTestData3(_T("Hello"), 4, 5);
		delete NewObj;
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
		printf("NG : %d %d %d %d %d %d %d %d %d\r\n", MaxMem[0], MaxMem[3], MaxMem[6], MaxMem[9], MaxMem[12], MaxMem[15], MaxMem[18], MaxMem[21], MaxMem[24]);
		exit(0);
	}
	printf("OK : %d %d %d %d %d %d %d %d %d\r\n", MaxMem[0], MaxMem[3], MaxMem[6], MaxMem[9], MaxMem[12], MaxMem[15], MaxMem[18], MaxMem[21], MaxMem[24]);
	return 0;
}

void StkObjectTest()
{
	printf("StkObjectTest started.\r\n");

	// Encode check
	{
		JsonEncodingTest1();
		XmlEncodingTest2();
		XmlEncodingTest3();
		XmlJsonEncodingTest1();
		XmlJsonEncodingTest2();
	}

	// Decode check
	{
		// XML
		XmlDecodingTest1();
		XmlDecodingTest2();
		XmlDecodingTest3(_T("DecodeCheck1"));
		XmlDecodingTest4(_T("DecodeCheck2"));
		XmlDecordingAbnormalTest1();

		// JSON
		JsonDecodingTest1();
		JsonDecodingTest2();
		JsonDecordingAbnormalTest1();
	}

	// General check
	{
		StkObject* Elem1 = MakeTestData1();
		GeneralTestCase1(Elem1, _T("GeneralCheck1"));
		delete Elem1;

		StkObject* Elem2 = MakeTestData2();
		GeneralTestCase2(Elem2, _T("GeneralCheck2"));
		delete Elem2;

		GeneralTestCase3();
		GeneralTestCase4();
		GeneralTestCase5();
		GeneralTestCase6();
	}

	// Clone check
	{
		StkObject* Elem1 = MakeTestData1();
		CloneTest(Elem1);
		delete Elem1;

		StkObject* Elem2 = MakeTestData2();
		CloneTest(Elem2);
		delete Elem2;
	}

	// Memory leak check
	{

		StkObject* Elem1 = MakeTestData1();
		MemoryLeakChecking1(Elem1);
		delete Elem1;

		StkObject* Elem2 = MakeTestData2();
		MemoryLeakChecking1(Elem2);
		delete Elem2;

		MemoryLeakChecking2();
		MemoryLeakChecking3();
		MemoryLeakChecking4();
	}

	printf("StkObjectTest completed.\r\n\r\n\r\n");

}
