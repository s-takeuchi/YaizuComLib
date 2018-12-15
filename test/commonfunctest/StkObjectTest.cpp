#include <windows.h>
#include <string>
#include <stdio.h>
#include <Psapi.h>
#include <shlwapi.h>

#include "..\..\src\commonfunc\StkObject.h"

void Abort(bool Cond)
{
	if (!Cond) {
		printf("NG\r\n");
		exit(0);
	} else {
		printf("OK\r\n");
	}
}

void AbnormalCaseForJson(wchar_t* JsonTxt, int ErrorCode)
{
	int Offset = 0;
	StkObject* RetObj;

	if (JsonTxt == NULL) {
		wprintf(L"Abnormal case (JSON): NULL is presented...");
	} else {
		wprintf(L"Abnormal case (JSON): \"%s\" is presented...", JsonTxt);
	}
	RetObj = StkObject::CreateObjectFromJson(JsonTxt, &Offset);
	if (RetObj != NULL || Offset != ErrorCode) {
		printf("NG\r\n");
		exit(0);
	}
	delete RetObj;
	printf("OK\r\n");
}

void AbnormalCaseForXml(wchar_t* XmlTxt, int ErrorCode)
{
	int Offset = 0;
	StkObject* RetObj;

	if (XmlTxt == NULL) {
		wprintf(L"Abnormal case (XML): NULL is presented...");
	} else {
		wprintf(L"Abnormal case (XML): \"%s\" is presented...", XmlTxt);
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
	if ((hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, false, dwProcessID)) != NULL) {
		if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
			Size = pmc.WorkingSetSize;
		}
	}
	CloseHandle(hProcess);
	return Size;
}

StkObject* MakeTestData1(int Mode = 0)
{
	StkObject* Elem1 = new StkObject(L"SoftwareCompany");

	for (int Loop = 0; Loop < 20; Loop++) {
		StkObject* Elem6 = new StkObject(L"Product");
		Elem6->AppendAttribute(new StkObject(L"Name", L"qwer"));
		Elem6->AppendAttribute(new StkObject(L"Price", 10000));
		Elem1->AppendChildElement(Elem6);
	}

	StkObject* Elem2 = new StkObject(L"Product");
	if (Mode == 1) {
		Elem2->AppendAttribute(new StkObject(L"Price", L"11000"));
		Elem2->AppendAttribute(new StkObject(L"Name", L"Xxxxx paint"));
	} else {
		Elem2->AppendAttribute(new StkObject(L"Name", L"Xxxxx paint"));
		Elem2->AppendAttribute(new StkObject(L"Price", L"11000"));
	}

	StkObject* Elem5 = new StkObject(L"License");
	Elem5->AppendAttribute(new StkObject(L"Cap", 322.44f));
	Elem2->AppendChildElement(Elem5);

	StkObject* Elem3 = new StkObject(L"Product");
	if (Mode == 1) {
		Elem3->AppendAttribute(new StkObject(L"Price", L"39000"));
		Elem3->AppendAttribute(new StkObject(L"Name", L"Aaaaa 3D"));
	} else {
		Elem3->AppendAttribute(new StkObject(L"Name", L"Aaaaa 3D"));
		if (Mode == 2) {
			Elem3->AppendAttribute(new StkObject(L"Price", L" 39000"));
		} else {
			Elem3->AppendAttribute(new StkObject(L"Price", L"39000"));
		}
	}

	StkObject* Elem4 = new StkObject(L"ELEM_INT", 123);
	StkObject* Elem7 = new StkObject(L"ELEM_FLOAT", 456.7f);
	StkObject* Elem8 = new StkObject(L"ELEM_STRING", L" ABC FFF DDD EEE ");

	Elem1->AppendAttribute(new StkObject(L"CompanyName", L"TakeuchiSoft"));
	Elem1->AppendAttribute(new StkObject(L"Employee", L"110"));
	Elem1->AppendAttribute(new StkObject(L"Address", L"Shizuoka Pref. Yaizu city"));
	Elem1->AppendChildElement(Elem2);
	Elem1->AppendChildElement(Elem3);
	Elem1->AppendChildElement(Elem4);
	Elem1->AppendChildElement(Elem7);
	Elem1->AppendChildElement(Elem8);

	return Elem1;
}

StkObject* MakeTestData2(int Mode = 0)
{
	StkObject* Elem1 = new StkObject(L"個人の財産");

	if (Mode == 1) {
		StkObject* Elem1_3 = new StkObject(L"現金");
		Elem1_3->AppendChildElement(new StkObject(L"C銀行", 7000000));
		Elem1_3->AppendChildElement(new StkObject(L"A銀行", 5000000));
		Elem1_3->AppendChildElement(new StkObject(L"B銀行", 10000000));
		Elem1->AppendChildElement(Elem1_3);

		StkObject* Elem1_2 = new StkObject(L"時計");
		Elem1_2->AppendChildElement(new StkObject(L"ロレックス", L"金"));
		Elem1_2->AppendChildElement(new StkObject(L"オメガ", L"黒"));
		Elem1_2->AppendChildElement(new StkObject(L"グランドセイコー", L"白"));
		Elem1->AppendChildElement(Elem1_2);

		StkObject* Elem1_1 = new StkObject(L"乗用車");
		StkObject* Elem1_1_2 = new StkObject(L"ブルーバード");
		Elem1_1_2->AppendAttribute(new StkObject(L"メーカー", L"日産"));
		Elem1_1_2->AppendAttribute(new StkObject(L"色", L"白"));
		Elem1_1->AppendChildElement(Elem1_1_2);
		StkObject* Elem1_1_1 = new StkObject(L"カローラ");
		Elem1_1_1->AppendAttribute(new StkObject(L"メーカー", L"トヨタ"));
		Elem1_1_1->AppendAttribute(new StkObject(L"色", L"赤"));
		Elem1_1->AppendChildElement(Elem1_1_1);
		Elem1->AppendChildElement(Elem1_1);
	} else {
		StkObject* Elem1_1 = new StkObject(L"乗用車");
		StkObject* Elem1_1_1 = new StkObject(L"カローラ");
		Elem1_1_1->AppendAttribute(new StkObject(L"色", L"赤"));
		Elem1_1_1->AppendAttribute(new StkObject(L"メーカー", L"トヨタ"));
		Elem1_1->AppendChildElement(Elem1_1_1);
		StkObject* Elem1_1_2 = new StkObject(L"ブルーバード");
		Elem1_1_2->AppendAttribute(new StkObject(L"色", L"白"));
		Elem1_1_2->AppendAttribute(new StkObject(L"メーカー", L"日産"));
		Elem1_1->AppendChildElement(Elem1_1_2);
		Elem1->AppendChildElement(Elem1_1);

		StkObject* Elem1_2 = new StkObject(L"時計");
		Elem1_2->AppendChildElement(new StkObject(L"グランドセイコー", L"白"));
		Elem1_2->AppendChildElement(new StkObject(L"オメガ", L"黒"));
		Elem1_2->AppendChildElement(new StkObject(L"ロレックス", L"金"));
		Elem1->AppendChildElement(Elem1_2);

		StkObject* Elem1_3 = new StkObject(L"現金");
		Elem1_3->AppendChildElement(new StkObject(L"A銀行", 5000000));
		Elem1_3->AppendChildElement(new StkObject(L"B銀行", 10000000));
		Elem1_3->AppendChildElement(new StkObject(L"C銀行", 7000000));
		Elem1->AppendChildElement(Elem1_3);
	}

	StkObject* Elem1_4 = new StkObject(L"証券");
	StkObject* Elem1_4_1 = new StkObject(L"株");
	Elem1_4_1->AppendChildElement(new StkObject(L"A社", 2000000));
	Elem1_4_1->AppendChildElement(new StkObject(L"B社", 500000));
	Elem1_4->AppendChildElement(Elem1_4_1);
	StkObject* Elem1_4_2 = new StkObject(L"国債");
	if (Mode == 1) {
		Elem1_4_2->AppendChildElement(new StkObject(L"債権", 1000000));
		Elem1_4_2->AppendChildElement(new StkObject(L"債権", 2000000));
		Elem1_4_2->AppendChildElement(new StkObject(L"債権", 1500000));
	} else {
		Elem1_4_2->AppendChildElement(new StkObject(L"債権", 2000000));
		Elem1_4_2->AppendChildElement(new StkObject(L"債権", 1500000));
		if (Mode == 2) {
			Elem1_4_2->AppendChildElement(new StkObject(L"債権", 1000001));
		} else {
			Elem1_4_2->AppendChildElement(new StkObject(L"債権", 1000000));
		}
	}
	Elem1_4->AppendChildElement(Elem1_4_2);
	StkObject* Elem1_4_3 = new StkObject(L"金券");
	Elem1_4_3->AppendChildElement(new StkObject(L"VISA", 200000));
	Elem1_4_3->AppendChildElement(new StkObject(L"JCB", 100000));
	Elem1_4->AppendChildElement(Elem1_4_3);
	Elem1->AppendChildElement(Elem1_4);

	return Elem1;
}

StkObject* MakeTestData3(wchar_t Name[64], int Width, int Height, int CurrentLevel = 0)
{
	StkObject* Obj = new StkObject(Name);
	if (CurrentLevel > Height) {
		Obj->AppendChildElement(new StkObject(L"Aaa", 12345));
		Obj->AppendChildElement(new StkObject(L"Bbb", 67890));
		Obj->AppendChildElement(new StkObject(L"Ccc", 123.45f));
		Obj->AppendChildElement(new StkObject(L"Ddd", 678.90f));
	} else {
		for (int Loop = 0; Loop < Width; Loop++) {
			wchar_t Buf[25];
			wsprintf(Buf, L"Elem_%d:%d", CurrentLevel, Loop);
			Obj->AppendChildElement(MakeTestData3(Buf, Width, Height, CurrentLevel + 1));
		}
	}
	return Obj;
}

StkObject* MakeTestData4(wchar_t Name[64], int Width, int Height, int CurrentLevel = 0)
{
	StkObject* Obj = new StkObject(Name);
	if (CurrentLevel > Height) {
		Obj->AppendChildElement(new StkObject(L"Aaa", L"東西南北"));
		Obj->AppendChildElement(new StkObject(L"Bbb", L"春夏秋冬"));
		Obj->AppendChildElement(new StkObject(L"Ccc", L"魑魅魍魎"));
		Obj->AppendChildElement(new StkObject(L"Ddd", L"焼肉定食"));
	} else {
		for (int Loop = 0; Loop < Width; Loop++) {
			wchar_t Buf[25];
			wsprintf(Buf, L"ElemRecursive", CurrentLevel, Loop);
			Obj->AppendChildElement(MakeTestData4(Buf, Width, Height, CurrentLevel + 1));
		}
	}
	return Obj;
}

void GeneralTestCase1(StkObject* Elem1, wchar_t* Name)
{
	int Len1 = Elem1->GetFirstChildElement()->GetArrayLength();
	wprintf(L"%s#Len of top level element's child = %d...", Name, Len1);
	if (Len1 == 25) {
		printf("OK\r\n");
	} else {
		printf("NG\r\n");
		exit(0);
	}
	wprintf(L"%s#Acquired element is validated...", Name);
	StkObject* Elem3 = Elem1->GetFirstChildElement();
	for (int Loop = 0; Loop < 21; Loop++) {
		if (Loop < 20) {
			if (lstrcmp(Elem3->GetName(), L"Product") != 0 || lstrcmp(Elem3->GetFirstAttribute()->GetStringValue(), L"qwer") != 0) {
				printf("NG\r\n");
				exit(0);
			}
		}
		Elem3 = Elem3->GetNext();
	}
	printf("OK\r\n");
	int Len1_1 = Elem3->GetArrayLength();
	wprintf(L"%s#Len of top level element's 2nd child = %d...", Name, Len1_1);
	if (Len1_1 == 4) {
		printf("OK\r\n");
	} else {
		printf("NG\r\n");
		exit(0);
	}
	int Len2 = Elem1->GetArrayLength();
	wprintf(L"%s#Len of top level element = %d...", Name, Len2);
	if (Len2 == 1) {
		printf("OK\r\n");
	} else {
		printf("NG\r\n");
		exit(0);
	}
	int ElemCount = Elem1->GetChildElementCount();
	wprintf(L"%s#ElemCount = %d...", Name, ElemCount);
	if (ElemCount == Len1) {
		printf("OK\r\n");
	} else {
		printf("NG\r\n");
		exit(0);
	}
	int AttrCount = Elem1->GetAttributeCount();
	wprintf(L"%s#AttrCount = %d...", Name, AttrCount);
	if (AttrCount == 3) {
		printf("OK\r\n");
	} else {
		printf("NG\r\n");
		exit(0);
	}
	StkObject* Elem4 = Elem3->GetNext();
	int Elem4ElemCnt = Elem4->GetChildElementCount();
	int Elem4AttrCnt = Elem4->GetAttributeCount();
	wprintf(L"%s#SingleElement Count...", Name);
	if (Elem4ElemCnt == 0 && Elem4AttrCnt == 0) {
		printf("OK\r\n");
	} else {
		printf("NG\r\n");
		exit(0);
	}
}

void GeneralTestCase2(StkObject* Elem1, wchar_t* Name)
{
	wprintf(L"%s#Proper count can be acquired.", Name);
	if (Elem1->GetChildElementCount() != Elem1->GetFirstChildElement()->GetArrayLength()) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");

	wprintf(L"%s#Each name is correct...", Name);
	if (lstrcmp(Elem1->GetName(), L"個人の財産") != 0 ||
		lstrcmp(Elem1->GetFirstChildElement()->GetName(), L"乗用車") != 0 ||
		lstrcmp(Elem1->GetFirstChildElement()->GetNext()->GetName(), L"時計") != 0 ||
		lstrcmp(Elem1->GetFirstChildElement()->GetNext()->GetNext()->GetName(), L"現金") != 0 ||
		lstrcmp(Elem1->GetFirstChildElement()->GetNext()->GetNext()->GetNext()->GetName(), L"証券") != 0) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");

	wprintf(L"%s#Attribute of car is correct...", Name);
	if (lstrcmp(Elem1->GetFirstChildElement()->GetFirstChildElement()->GetFirstAttribute()->GetStringValue(), L"赤") != 0 ||
		lstrcmp(Elem1->GetFirstChildElement()->GetFirstChildElement()->GetFirstAttribute()->GetNext()->GetStringValue(), L"トヨタ") != 0 ||
		lstrcmp(Elem1->GetFirstChildElement()->GetFirstChildElement()->GetNext()->GetFirstAttribute()->GetStringValue(), L"白") != 0 ||
		lstrcmp(Elem1->GetFirstChildElement()->GetFirstChildElement()->GetNext()->GetFirstAttribute()->GetNext()->GetStringValue(), L"日産") != 0) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");

	wprintf(L"%s#Fetch integer value...", Name);
	if (Elem1->GetFirstChildElement()->GetNext()->GetNext()->GetFirstChildElement()->GetIntValue() != 5000000 ||
		Elem1->GetFirstChildElement()->GetNext()->GetNext()->GetFirstChildElement()->GetNext()->GetIntValue() != 10000000 ||
		Elem1->GetFirstChildElement()->GetNext()->GetNext()->GetFirstChildElement()->GetNext()->GetNext()->GetIntValue() != 7000000) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");

	wprintf(L"%s#Fetch tail element...", Name);
	if (Elem1->GetFirstChildElement()->GetNext()->GetNext()->GetNext()->GetFirstChildElement()->GetNext()->GetNext()->GetFirstChildElement()->GetIntValue() != 200000 ||
		lstrcmp(Elem1->GetFirstChildElement()->GetNext()->GetNext()->GetNext()->GetFirstChildElement()->GetNext()->GetNext()->GetFirstChildElement()->GetName(), L"VISA") != 0) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");
}

void GeneralTestCase3()
{
	wprintf(L"GeneralCheck3#Test of Equals 1st ...");
	StkObject* Origin1 = MakeTestData1();
	StkObject* Normal1 = MakeTestData1(0);
	StkObject* Order1 = MakeTestData1(1);
	StkObject* Diff1 = MakeTestData1(2);
	bool Ch1 = Origin1->Equals(Normal1);
	bool Ch2 = Origin1->Equals(Order1);
	bool Ch3 = Origin1->Equals(Diff1);
	if (!Ch1 || !Ch2 || Ch3) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");
	delete Origin1;
	delete Normal1;
	delete Order1;
	delete Diff1;

	wprintf(L"GeneralCheck3#Test of Equals 2nd ...");
	StkObject* Origin2 = MakeTestData2();
	StkObject* Normal2 = MakeTestData2(0);
	StkObject* Order2 = MakeTestData2(1);
	StkObject* Diff2 = MakeTestData2(2);
	bool Ci1 = Origin2->Equals(Normal2);
	bool Ci2 = Origin2->Equals(Order2);
	bool Ci3 = Origin2->Equals(Diff2);
	if (!Ci1 || !Ci2 || Ci3) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");
	delete Origin2;
	delete Normal2;
	delete Order2;
	delete Diff2;

	wprintf(L"GeneralCheck3#Test of Equals 3rd integer match ...");
	StkObject* Obj1a = new StkObject(L"Shinya Takeuchi", 123);
	StkObject* Obj1b = new StkObject(L"Shinya Takeuchi", 123);
	if (Obj1a->Equals(Obj1b) == false) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");
	delete Obj1a;
	delete Obj1b;

	wprintf(L"GeneralCheck3#Test of Equals 3rd integer unmatch ...");
	StkObject* Obj2a = new StkObject(L"Shinya Takeuchi", 123);
	StkObject* Obj2b = new StkObject(L"Shinya Takeuchi", 321);
	if (Obj2a->Equals(Obj2b) == true) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");
	delete Obj2a;
	delete Obj2b;

	wprintf(L"GeneralCheck3#Test of Equals 3rd float match ...");
	StkObject* Obj3a = new StkObject(L"Shinya Takeuchi", 123.45f);
	StkObject* Obj3b = new StkObject(L"Shinya Takeuchi", 123.45f);
	if (Obj3a->Equals(Obj3b) == false) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");
	delete Obj3a;
	delete Obj3b;

	wprintf(L"GeneralCheck3#Test of Equals 3rd float unmatch ...");
	StkObject* Obj4a = new StkObject(L"Shinya Takeuchi", 123.45f);
	StkObject* Obj4b = new StkObject(L"Shinya Takeuchi", 123.46f);
	if (Obj4a->Equals(Obj4b) == true) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");
	delete Obj4a;
	delete Obj4b;

	wprintf(L"GeneralCheck3#Test of Equals 3rd string match ...");
	StkObject* Obj5a = new StkObject(L"Shinya Takeuchi", L"Hello, World!");
	StkObject* Obj5b = new StkObject(L"Shinya Takeuchi", L"Hello, World!");
	if (Obj5a->Equals(Obj5b) == false) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");
	delete Obj5a;
	delete Obj5b;

	wprintf(L"GeneralCheck3#Test of Equals 3rd string unmatch ...");
	StkObject* Obj6a = new StkObject(L"Shinya Takeuchi", L"Hello, World!");
	StkObject* Obj6b = new StkObject(L"Shinya Takeuchi", L"Hello; World!");
	if (Obj6a->Equals(Obj6b) == true) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");
	delete Obj6a;
	delete Obj6b;

	wprintf(L"GeneralCheck3#Test of Equals 4th Array of recursive object ...");
	StkObject* Elem4 = MakeTestData4(L"RecursiveArray", 2, 3);
	wchar_t JsonStr[1000000] = L"";
	wchar_t XmlStr[1000000] = L"";
	Elem4->ToJson(JsonStr, 1000000);
	Elem4->ToXml(XmlStr, 1000000);
	delete Elem4;
	int ErrorCode = 0;
	StkObject* NewObjJson = StkObject::CreateObjectFromJson(JsonStr, &ErrorCode);
	StkObject* NewObjXml = StkObject::CreateObjectFromXml(XmlStr, &ErrorCode);
	if (!NewObjXml->Equals(NewObjJson) || !NewObjJson->Equals(NewObjXml)) {
		delete NewObjXml;
		delete NewObjJson;
		printf("NG\r\n");
		exit(0);
	}
	delete NewObjXml;
	delete NewObjJson;
	printf("OK\r\n");
}

void GeneralTestCase4()
{
	{
		wprintf(L"GeneralCheck4#Test of Contains  (first elem match case) ...");
		int ErrorCode;
		StkObject* Origin = StkObject::CreateObjectFromXml(L"<Hello><First>Shinya</First><Last>Takeuchi</Last></Hello>", &ErrorCode);
		StkObject* Target = StkObject::CreateObjectFromXml(L"<First>Shinya</First>", &ErrorCode);
		StkObject* Ret = Origin->Contains(Target);
		Abort(Ret != NULL &&
			  lstrcmp(Ret->GetStringValue(), L"Shinya") == 0 &&
			  lstrcmp(Ret->GetName(), L"First") == 0 &&
			  lstrcmp(Ret->GetNext()->GetStringValue(), L"Takeuchi") == 0 &&
			  lstrcmp(Ret->GetNext()->GetName(), L"Last") == 0);
		delete Origin;
		delete Target;
	}
	{
		wprintf(L"GeneralCheck4#Test of Contains  (last elem match case) ...");
		int ErrorCode;
		StkObject* Origin = StkObject::CreateObjectFromXml(L"<Hello><First>Shinya</First><Middle>Tsunemi</Middle><Last>Takeuchi</Last></Hello>", &ErrorCode);
		StkObject* Target = StkObject::CreateObjectFromXml(L"<Last>Takeuchi</Last>", &ErrorCode);
		StkObject* Ret = Origin->Contains(Target);
		Abort(Ret != NULL &&
			  lstrcmp(Ret->GetStringValue(), L"Takeuchi") == 0 &&
			  lstrcmp(Ret->GetName(), L"Last") == 0);
		delete Origin;
		delete Target;
	}
	{
		wprintf(L"GeneralCheck4#Test of Contains  (simple int/float/string element) ...");
		StkObject* OriginInt = new StkObject(L"ObjInt", 345);
		StkObject* TargetInt = new StkObject(L"ObjInt", 345);
		StkObject* OriginFloat = new StkObject(L"ObjFloat", 345.6f);
		StkObject* TargetFloat = new StkObject(L"ObjFloat", 345.6f);
		StkObject* OriginStr = new StkObject(L"ObjFloat", L"Hello, World!");
		StkObject* TargetStr = new StkObject(L"ObjFloat", L"Hello, World!");
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
		wprintf(L"GeneralCheck4#Test of Contains  (simple NG case) ...");
		int ErrorCode;
		StkObject* Origin = StkObject::CreateObjectFromXml(L"<Hello><First>Shinya</First><Middle>Tsunemi</Middle><Last>Takeuchi</Last></Hello>", &ErrorCode);
		StkObject* Target = StkObject::CreateObjectFromXml(L"<Last>Suzuki</Last>", &ErrorCode);
		StkObject* Ret = Origin->Contains(Target);
		Abort(Ret == NULL);
		delete Origin;
		delete Target;
	}
	{
		wprintf(L"GeneralCheck4#Test of Contains  (Common data 1) ...");
		StkObject* Origin = MakeTestData1();
		StkObject* Target = MakeTestData1();
		StkObject* Ret = Origin->Contains(Target);
		Abort(Ret != NULL);
		delete Origin;
		delete Target;
	}
	{
		wprintf(L"GeneralCheck4#Test of Contains  (Common data 1 : advanced) ...");
		StkObject* Origin = MakeTestData1();

		StkObject* Elem1 = new StkObject(L"SoftwareCompany");
		StkObject* Elem8 = new StkObject(L"ELEM_STRING", L" ABC FFF DDD EEE ");
		StkObject* Elem7 = new StkObject(L"ELEM_FLOAT", 456.7f);
		StkObject* Elem4 = new StkObject(L"ELEM_INT", 123);
		Elem1->AppendAttribute(new StkObject(L"Employee", L"110"));
		Elem1->AppendChildElement(Elem8);
		Elem1->AppendChildElement(Elem7);
		Elem1->AppendChildElement(Elem4);
		StkObject* Ret = Origin->Contains(Elem1);
		Abort(Ret == Origin);
		delete Origin;
		delete Elem1;
	}
	{
		wprintf(L"GeneralCheck4#Test of Contains  (Common data 2) ...");
		StkObject* Origin = MakeTestData2();
		StkObject* Target = MakeTestData2();
		StkObject* Ret = Origin->Contains(Target);
		Abort(Ret != NULL);
		delete Origin;
		delete Target;
	}
	{
		wprintf(L"GeneralCheck4#Test of Contains  (Common data 2 : advanced) ...");
		StkObject* Origin = MakeTestData2();
		StkObject* Elem1_4 = new StkObject(L"証券");
		StkObject* Elem1_4_1 = new StkObject(L"株");
		Elem1_4_1->AppendChildElement(new StkObject(L"B社", 500000));
		Elem1_4_1->AppendChildElement(new StkObject(L"A社", 2000000));
		Elem1_4->AppendChildElement(Elem1_4_1);
		StkObject* Ret = Origin->Contains(Elem1_4);
		Abort(Ret != NULL && lstrcmp(Ret->GetName(), L"証券") == 0 && Ret->GetFirstChildElement()->GetFirstChildElement()->GetIntValue() == 2000000);
		delete Origin;
		delete Elem1_4;
	}
	{
		wprintf(L"GeneralCheck4#Test of Contains  (All type of element) ...");
		StkObject* Origin = MakeTestData1();
		StkObject* Target1 = new StkObject(L"ELEM_INT", 123);
		StkObject* Target2 = new StkObject(L"ELEM_INT", 321);
		StkObject* Target3 = new StkObject(L"ELEM_FLOAT", 456.7f);
		StkObject* Target4 = new StkObject(L"ELEM_FLOAT", 765.4f);
		StkObject* Target5 = new StkObject(L"ELEM_STRING", L" ABC FFF DDD EEE ");
		StkObject* Target6 = new StkObject(L"ELEM_STRING", L" FFF DDD EEE ABC ");
		StkObject* Ret1 = Origin->Contains(Target1);
		StkObject* Ret2 = Origin->Contains(Target2);
		StkObject* Ret3 = Origin->Contains(Target3);
		StkObject* Ret4 = Origin->Contains(Target4);
		StkObject* Ret5 = Origin->Contains(Target5);
		StkObject* Ret6 = Origin->Contains(Target6);
		Abort(Ret1 != NULL && Ret2 == NULL && Ret3 != NULL && Ret4 == NULL && Ret5 != NULL && Ret6 == NULL &&
			  lstrcmp(Ret1->GetName(), L"ELEM_INT") == 0 &&
			  Ret1->GetIntValue() == 123 &&
			  lstrcmp(Ret3->GetName(), L"ELEM_FLOAT") == 0 &&
			  Ret3->GetFloatValue() == 456.7f &&
			  lstrcmp(Ret5->GetName(), L"ELEM_STRING") == 0 &&
			  lstrcmp(Ret5->GetStringValue(), L" ABC FFF DDD EEE ") == 0);
		delete Origin;
		delete Target1;
		delete Target2;
		delete Target3;
		delete Target4;
		delete Target5;
		delete Target6;
	}
	{
		wprintf(L"GeneralCheck4#Test of Contains  (Hierarchy : normal) ...");
		int ErrorCode;
		StkObject* Origin = StkObject::CreateObjectFromXml(L"<Aaa><Bbb><Ccc Name=\"Spring\"><Ddd>春</Ddd></Ccc><Ccc Name=\"Summer\"><Ddd>夏</Ddd></Ccc></Bbb><Bbb><Ccc Name=\"Fall\"><Ddd>秋</Ddd></Ccc><Ccc Name=\"Winter\"><Ddd>冬</Ddd></Ccc></Bbb></Aaa>", &ErrorCode);
		StkObject* Target1 = StkObject::CreateObjectFromXml(L"<Aaa><Bbb><Ccc Name=\"Summer\"><Ddd>夏</Ddd></Ccc></Bbb></Aaa>", &ErrorCode);
		StkObject* Target2 = StkObject::CreateObjectFromXml(L"<Aaa><Bbb><Ccc Name=\"Winter\"></Ccc></Bbb></Aaa>", &ErrorCode);
		StkObject* Target3 = StkObject::CreateObjectFromXml(L"<Aaa></Aaa>", &ErrorCode);
		StkObject* Target4 = StkObject::CreateObjectFromXml(L"<Bbb><Ccc Name=\"Fall\"><Ddd>秋</Ddd></Ccc></Bbb>", &ErrorCode);
		StkObject* Target5 = StkObject::CreateObjectFromXml(L"<Ccc Name=\"Spring\"><Ddd>春</Ddd></Ccc>", &ErrorCode);
		StkObject* Target6 = StkObject::CreateObjectFromXml(L"<Ddd>冬</Ddd>", &ErrorCode);
		StkObject* Target7 = StkObject::CreateObjectFromXml(L"<Aaa><Bbb><Ccc Name=\"Spring\"><Ddd>春</Ddd></Ccc></Bbb><Bbb><Ccc Name=\"Winter\"><Ddd>冬</Ddd></Ccc></Bbb></Aaa>", &ErrorCode);
		StkObject* Target8 = StkObject::CreateObjectFromXml(L"<Ccc Name=\"Fall\"/>", &ErrorCode);
		StkObject* Ret1 = Origin->Contains(Target1);
		StkObject* Ret2 = Origin->Contains(Target2);
		StkObject* Ret3 = Origin->Contains(Target3);
		StkObject* Ret4 = Origin->Contains(Target4);
		StkObject* Ret5 = Origin->Contains(Target5);
		StkObject* Ret6 = Origin->Contains(Target6);
		StkObject* Ret7 = Origin->Contains(Target7);
		StkObject* Ret8 = Origin->Contains(Target8);
		Abort(Ret1 != NULL && Ret2 != NULL && Ret3 != NULL && Ret4 != NULL && Ret5 != NULL && Ret6 != NULL && Ret7 != NULL && Ret8 != NULL &&
			  lstrcmp(Ret4->GetName(), L"Bbb") == 0 &&
			  lstrcmp(Ret5->GetName(), L"Ccc") == 0 &&
			  lstrcmp(Ret6->GetName(), L"Ddd") == 0);
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
		wprintf(L"GeneralCheck4#Test of Contains  (Hierarchy : abnormal) ...");
		int ErrorCode;
		StkObject* Origin = StkObject::CreateObjectFromXml(L"<Aaa><Bbb><Ccc Name=\"Spring\"><Ddd>春</Ddd></Ccc><Ccc Name=\"Summer\"><Ddd>夏</Ddd></Ccc></Bbb><Bbb><Ccc Name=\"Fall\"><Ddd>秋</Ddd></Ccc><Ccc Name=\"Winter\"><Ddd>冬</Ddd></Ccc></Bbb></Aaa>", &ErrorCode);
		StkObject* Target1 = StkObject::CreateObjectFromXml(L"<Aaa><Bbb><Ccc Name=\"Summer\"><Ddd>冬</Ddd></Ccc></Bbb></Aaa>", &ErrorCode);
		StkObject* Target2 = StkObject::CreateObjectFromXml(L"<Aaa><Bbb><Ddd>夏</Ddd></Bbb></Aaa>", &ErrorCode);
		StkObject* Target3 = StkObject::CreateObjectFromXml(L"<Aaa><Ccc Name=\"Winter\"><Ddd>冬</Ddd></Ccc></Aaa>", &ErrorCode);
		StkObject* Target4 = StkObject::CreateObjectFromXml(L"<Aaa><Ddd>夏</Ddd></Aaa>", &ErrorCode);
		StkObject* Target5 = StkObject::CreateObjectFromXml(L"<Bbb><Ddd>夏</Ddd></Bbb>", &ErrorCode);
		StkObject* Target6 = StkObject::CreateObjectFromXml(L"<Aaa>冬</Aaa>", &ErrorCode);
		StkObject* Target7 = StkObject::CreateObjectFromXml(L"<Aaa><Bbb><Ccc Name=\"Spring\"><Ddd>春</Ddd></Ccc><Ccc Name=\"Winter\"><Ddd>冬</Ddd></Ccc></Bbb></Aaa>", &ErrorCode);
		StkObject* Target8 = StkObject::CreateObjectFromXml(L"<Aaa><Ccc Name=\"Fall\"/></Aaa>", &ErrorCode);
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
		wprintf(L"GeneralCheck4#Test of Contains  (Hierarchy : sequential check) ...");
		int ErrorCode;
		StkObject* Origin = StkObject::CreateObjectFromXml(L"<Aaa><Bbb Type=\"文字\"><Ccc Name=\"季節\"><Ddd>春</Ddd><Ddd>夏</Ddd><Ddd>秋</Ddd><Ddd>冬</Ddd></Ccc><Ccc Name=\"方角\"><Ddd>東</Ddd><Ddd>西</Ddd><Ddd>南</Ddd><Ddd>北</Ddd></Ccc></Bbb><Bbb Type=\"氏名\"><Ccc Name=\"家族\"><Ddd>竹内伸也</Ddd><Ddd>常見朱美</Ddd><Ddd>木村優子</Ddd></Ccc><Ccc Name=\"会社メンバー\"><Ddd>山田太郎</Ddd><Ddd>鈴木一郎</Ddd></Ccc></Bbb></Aaa>", &ErrorCode);
		StkObject* Target1 = StkObject::CreateObjectFromXml(L"<Bbb Type=\"文字\"/>", &ErrorCode);
		StkObject* Target2 = StkObject::CreateObjectFromXml(L"<Ccc Name=\"方角\"/>", &ErrorCode);
		StkObject* Target3 = StkObject::CreateObjectFromXml(L"<Ccc Name=\"季節\"/>", &ErrorCode);
		StkObject* Target4 = StkObject::CreateObjectFromXml(L"<Ddd>南</Ddd>", &ErrorCode);
		StkObject* Target5 = StkObject::CreateObjectFromXml(L"<Ddd>冬</Ddd>", &ErrorCode);
		StkObject* Ret124 = Origin->Contains(Target1)->Contains(Target2)->Contains(Target4);
		StkObject* Ret125 = Origin->Contains(Target1)->Contains(Target2)->Contains(Target5);
		StkObject* Ret134 = Origin->Contains(Target1)->Contains(Target3)->Contains(Target4);
		StkObject* Ret135 = Origin->Contains(Target1)->Contains(Target3)->Contains(Target5);
		Abort(lstrcmp(Ret124->GetStringValue(), L"南") == 0 && Ret125 == NULL &&
			  Ret134 == NULL && lstrcmp(Ret135->GetStringValue(), L"冬") == 0);
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
		wprintf(L"GeneralCheck5#Test of Fetching the Value of Invalid Type for Int Value ...");
		StkObject* Obj = new StkObject(L"IntValue", 123);
		Abort(Obj->GetIntValue() == 123 && Obj->GetFloatValue() == 0.0f && Obj->GetStringValue() == NULL);

		wprintf(L"GeneralCheck5#Test of Fetching the Value of Invalid Type for Int Value (Child Element) ...");
		StkObject* ParentObj = new StkObject(L"Parent");
		ParentObj->AppendChildElement(Obj);
		ParentObj->GetFirstChildElement()->SetIntValue(321);
		ParentObj->GetFirstChildElement()->SetFloatValue(999.9f);
		ParentObj->GetFirstChildElement()->SetStringValue(L"Hello");
		Abort(ParentObj->GetFirstChildElement()->GetIntValue() == 321 &&
			ParentObj->GetFirstChildElement()->GetFloatValue() == 0.0f &&
			ParentObj->GetFirstChildElement()->GetStringValue() == NULL);
		delete ParentObj;
	}
	{
		wprintf(L"GeneralCheck5#Test of Fetching the Value of Invalid Type for Float Value ...");
		StkObject* Obj = new StkObject(L"IntValue", 123.4f);
		Abort(Obj->GetIntValue() == 0 && Obj->GetFloatValue() == 123.4f && Obj->GetStringValue() == NULL);

		wprintf(L"GeneralCheck5#Test of Fetching the Value of Invalid Type for Float Value (Child Element) ...");
		StkObject* ParentObj = new StkObject(L"Parent");
		ParentObj->AppendChildElement(Obj);
		ParentObj->GetFirstChildElement()->SetIntValue(999);
		ParentObj->GetFirstChildElement()->SetFloatValue(432.1f);
		ParentObj->GetFirstChildElement()->SetStringValue(L"Hello");
		Abort(ParentObj->GetFirstChildElement()->GetIntValue() == 0 &&
			ParentObj->GetFirstChildElement()->GetFloatValue() == 432.1f &&
			ParentObj->GetFirstChildElement()->GetStringValue() == NULL);
		delete ParentObj;
	}
	{
		wprintf(L"GeneralCheck5#Test of Fetching the Value of Invalid Type for String Value ...");
		StkObject* Obj = new StkObject(L"IntValue", L"Test");
		Abort(Obj->GetIntValue() == 0 && Obj->GetFloatValue() == 0.0f && Obj->GetStringValue() != NULL);

		wprintf(L"GeneralCheck5#Test of Fetching the Value of Invalid Type for String Value (Child Element) ...");
		StkObject* ParentObj = new StkObject(L"Parent");
		ParentObj->AppendChildElement(Obj);
		ParentObj->GetFirstChildElement()->SetIntValue(999);
		ParentObj->GetFirstChildElement()->SetFloatValue(999.9f);
		ParentObj->GetFirstChildElement()->SetStringValue(L"Hello");
		Abort(ParentObj->GetFirstChildElement()->GetIntValue() == 0 &&
			ParentObj->GetFirstChildElement()->GetFloatValue() == 0.0f &&
			ParentObj->GetFirstChildElement()->GetStringValue() != NULL &&
			lstrcmp(ParentObj->GetFirstChildElement()->GetStringValue(), L"Hello") == 0);
		delete ParentObj;
	}
}

void GeneralTestCase6()
{
	{
		wprintf(L"GeneralCheck6#Identify the presented text is XML or JSON (Normal-1) ...");

		StkObject* Elem1 = MakeTestData1();
		wchar_t Xml1[8192] = L"";
		wchar_t Json1[8192] = L"";
		Elem1->ToXml(Xml1, 8192);
		Elem1->ToJson(Json1, 8192);
		int RetXml1 = StkObject::Analyze(Xml1);
		int RetJson1 = StkObject::Analyze(Json1);

		StkObject* Elem2 = MakeTestData2();
		wchar_t Xml2[8192] = L"";
		wchar_t Json2[8192] = L"";
		Elem2->ToXml(Xml2, 8192);
		Elem2->ToJson(Json2, 8192);
		int RetXml2 = StkObject::Analyze(Xml2);
		int RetJson2 = StkObject::Analyze(Json2);

		StkObject* Elem3 = MakeTestData3(L"Hello", 2, 3);
		wchar_t Xml3[8192] = L"";
		wchar_t Json3[8192] = L"";
		Elem3->ToXml(Xml3, 8192);
		Elem3->ToJson(Json3, 8192);
		int RetXml3 = StkObject::Analyze(Xml3);
		int RetJson3 = StkObject::Analyze(Json3);

		if (!(RetXml1 == 1 && RetJson1 == 2 && RetXml2 == 1 && RetJson2 == 2 && RetXml3 == 1 && RetJson3 == 2)) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}

	/////////////////////////////////////////////////////////////////
	{
		wprintf(L"GeneralCheck6#Identify the presented text is XML or JSON (Normal-2) ...");
		int RetXml1 = StkObject::Analyze(L"<aaa/>");
		int RetJson1 = StkObject::Analyze(L"{\"aaa\":1}");
		int RetXml2 = StkObject::Analyze(L"  \t\r\n  <aaa/>  \t\r\n");
		int RetJson2 = StkObject::Analyze(L"  \t\r\n  {\"aaa\":1}  \t\r\n");
		int RetXml3 = StkObject::Analyze(L"  \t\r\n  <aaa>bbb</aaa>  \t\r\n");
		int RetJson3 = StkObject::Analyze(L"  \t\r\n  \"aaa\" : {\"bbb\":1}  \t\r\n");
		if (!(RetXml1 == 1 && RetJson1 == 2 && RetXml2 == 1 && RetJson2 == 2 && RetXml3 == 1 && RetJson3 == 2)) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}

	/////////////////////////////////////////////////////////////////
	{
		wprintf(L"GeneralCheck6#Identify the presented text is XML or JSON (Abnormal) ...");
		int RetXml1 = StkObject::Analyze(L"<");
		int RetJson1 = StkObject::Analyze(L"{");
		int RetXml2 = StkObject::Analyze(L">");
		int RetJson2 = StkObject::Analyze(L"}");
		int RetXml3 = StkObject::Analyze(L"<>");
		int RetJson3 = StkObject::Analyze(L"{}");
		int RetXml4 = StkObject::Analyze(L"  <aaa>  ");
		int RetJson4 = StkObject::Analyze(L"  {\"aaa\"}  ");
		int RetEmpty1 = StkObject::Analyze(L"");
		int RetEmpty2 = StkObject::Analyze(NULL);
		int RetEmpty3 = StkObject::Analyze(L" \t \r\n \t \r\n ");
		if (RetXml1 != -1 || RetJson1 != -1 || RetXml2 != -1 || RetJson2 != -1 || RetXml3 != -1 || RetJson3 != -1 || RetXml4 != -1 || RetJson4 != -1 || RetEmpty1 != 0 || RetEmpty2 != 0 || RetEmpty3 != 0) {
			printf("NG\r\n");
			exit(0);
		}
		printf("OK\r\n");
	}
}

void JsonEncodingTest1()
{
	StkObject* Obj = new StkObject(L"EncodeTesting");
	Obj->AppendChildElement(new StkObject(L"Element1", L"Hello-\\\\\\\\\\-World!!"));
	Obj->AppendChildElement(new StkObject(L"Element2", L"Hello-\"\"\"\"\"-World!!"));
	Obj->AppendChildElement(new StkObject(L"Element3", L"Hello-/////-World!!"));
	Obj->AppendChildElement(new StkObject(L"Element4", L"Hello-\b\b\b\b\b-World!!"));
	Obj->AppendChildElement(new StkObject(L"Element5", L"Hello-\f\f\f\f\f-World!!"));
	Obj->AppendChildElement(new StkObject(L"Element6", L"Hello-\r\r\r\r\r-World!!"));
	Obj->AppendChildElement(new StkObject(L"Element7", L"Hello-\n\n\n\n\n-World!!"));
	Obj->AppendChildElement(new StkObject(L"Element8", L"Hello-\t\t\t\t\t-World!!"));
	wchar_t JsonTxt[2048] = L"";
	Obj->ToJson(JsonTxt, 2048);
	wprintf(L"JSON Encoding#Escape ... ");
	if (StrStr(JsonTxt, L"\"Element1\" : \"Hello-\\\\\\\\\\\\\\\\\\\\-World!!\"") == 0 ||
		StrStr(JsonTxt, L"\"Element2\" : \"Hello-\\\"\\\"\\\"\\\"\\\"-World!!\"") == 0 ||
		StrStr(JsonTxt, L"\"Element3\" : \"Hello-\\/\\/\\/\\/\\/-World!!\"") == 0 ||
		StrStr(JsonTxt, L"\"Element4\" : \"Hello-\\b\\b\\b\\b\\b-World!!\"") == 0 ||
		StrStr(JsonTxt, L"\"Element5\" : \"Hello-\\f\\f\\f\\f\\f-World!!\"") == 0 ||
		StrStr(JsonTxt, L"\"Element6\" : \"Hello-\\r\\r\\r\\r\\r-World!!\"") == 0 ||
		StrStr(JsonTxt, L"\"Element7\" : \"Hello-\\n\\n\\n\\n\\n-World!!\"") == 0 ||
		StrStr(JsonTxt, L"\"Element8\" : \"Hello-\\t\\t\\t\\t\\t-World!!\"") == 0) {
		printf("NG\r\n");
		exit(0);
	} else {
		printf("OK\r\n");
	}
	delete Obj;
}

void JsonEncodingTest2()
{
	wprintf(L"JSON Encoding#Triming ... ");

	StkObject* MkDat1 = MakeTestData1();
	wchar_t JsonTxt1[128];
	int Ret = 0;

	lstrcpy(JsonTxt1, L"");
	Ret = MkDat1->ToJson(JsonTxt1, 32);
	if (lstrcmp(JsonTxt1, L"\"SoftwareCompany\" : {\r\n  \"@attr") != 0) {
		printf("NG\r\n");
		exit(0);
	}
	lstrcpy(JsonTxt1, L"");
	Ret = MkDat1->ToJson(JsonTxt1, 33);
	if (lstrcmp(JsonTxt1, L"\"SoftwareCompany\" : {\r\n  \"@attri") != 0) {
		printf("NG\r\n");
		exit(0);
	}
	lstrcpy(JsonTxt1, L"");
	Ret = MkDat1->ToJson(JsonTxt1, 34);
	if (lstrcmp(JsonTxt1, L"\"SoftwareCompany\" : {\r\n  \"@attrib") != 0) {
		printf("NG\r\n");
		exit(0);
	}

	StkObject* MkDat2 = MakeTestData2();
	wchar_t JsonTxt2[128];
	lstrcpy(JsonTxt2, L"");
	Ret = MkDat2->ToJson(JsonTxt2, 32);
	if (lstrcmp(JsonTxt2, L"\"個人の財産\" : {\r\n  \"乗用車\" : {\r\n    \"") != 0) {
		printf("NG\r\n");
		exit(0);
	}
	lstrcpy(JsonTxt2, L"");
	Ret = MkDat2->ToJson(JsonTxt2, 33);
	if (lstrcmp(JsonTxt2, L"\"個人の財産\" : {\r\n  \"乗用車\" : {\r\n    \"カ") != 0) {
		printf("NG\r\n");
		exit(0);
	}
	lstrcpy(JsonTxt2, L"");
	Ret = MkDat2->ToJson(JsonTxt2, 34);
	if (lstrcmp(JsonTxt2, L"\"個人の財産\" : {\r\n  \"乗用車\" : {\r\n    \"カロ") != 0) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");

}

void XmlEncodingTest2()
{
	StkObject* Xml2 = new StkObject(L"EncodeTesting");
	Xml2->AppendAttribute(new StkObject(L"Lt", L"<"));
	Xml2->AppendAttribute(new StkObject(L"Gt", L">"));
	Xml2->AppendAttribute(new StkObject(L"Amp", L"&"));
	Xml2->AppendAttribute(new StkObject(L"Quot", L"\""));
	Xml2->AppendAttribute(new StkObject(L"Apos", L"\'"));
	Xml2->AppendChildElement(new StkObject(L"Element", L"<"));
	Xml2->AppendChildElement(new StkObject(L"Element", L">"));
	Xml2->AppendChildElement(new StkObject(L"Element", L"&"));
	Xml2->AppendChildElement(new StkObject(L"Element", L"\""));
	Xml2->AppendChildElement(new StkObject(L"Element", L"\'"));

	StkObject* Attr1 = Xml2->GetFirstAttribute();
	wchar_t Attr1Val[2048] = L"";
	Attr1->ToXml(Attr1Val, 2048);
	wprintf(L"XmlEncoding(Single)#Attribute value '<', '>', '&', '\"' and '\'' are escaped ... ");
	if (StrStr(Attr1Val, L"Lt=\"&lt;\"") == 0 ||
		StrStr(Attr1Val, L"Gt=\"&gt;\"") == 0 ||
		StrStr(Attr1Val, L"Amp=\"&amp;\"") == 0 ||
		StrStr(Attr1Val, L"Quot=\"&quot;\"") == 0 ||
		StrStr(Attr1Val, L"Apos=\"&apos;\"") == 0) {
		printf("NG\r\n");
		exit(0);
	} else {
		printf("OK\r\n");
	}

	StkObject* Elem1 = Xml2->GetFirstChildElement();
	wchar_t Elem1Val[2048] = L"";;
	Elem1->ToXml(Elem1Val, 2048);
	wprintf(L"XmlEncoding(Single)#Element value '<', '>', '&', '\"' and '\'' are escaped ... ");
	if (StrStr(Elem1Val, L">&lt;<") == 0 ||
		StrStr(Elem1Val, L">&gt;<") == 0 ||
		StrStr(Elem1Val, L">&amp;<") == 0 ||
		StrStr(Elem1Val, L">&quot;<") == 0 ||
		StrStr(Elem1Val, L">&apos;<") == 0) {
		printf("NG\r\n");
		exit(0);
	} else {
		printf("OK\r\n");
	}
	delete Xml2;
}

void XmlEncodingTest3()
{
	StkObject* Xml2 = new StkObject(L"EncodeTesting");
	Xml2->AppendAttribute(new StkObject(L"Lt", L"<<<<<<<<<<"));
	Xml2->AppendAttribute(new StkObject(L"Gt", L">>>>>>>>>>"));
	Xml2->AppendAttribute(new StkObject(L"Amp", L"&&&&&&&&&&"));
	Xml2->AppendAttribute(new StkObject(L"Quot", L"\"\"\"\"\"\"\"\"\"\""));
	Xml2->AppendAttribute(new StkObject(L"Apos", L"\'\'\'\'\'\'\'\'\'\'"));
	Xml2->AppendChildElement(new StkObject(L"Element", L"<<<<<<<<<<"));
	Xml2->AppendChildElement(new StkObject(L"Element", L">>>>>>>>>>"));
	Xml2->AppendChildElement(new StkObject(L"Element", L"&&&&&&&&&&"));
	Xml2->AppendChildElement(new StkObject(L"Element", L"\"\"\"\"\"\"\"\"\"\""));
	Xml2->AppendChildElement(new StkObject(L"Element", L"\'\'\'\'\'\'\'\'\'\'"));

	StkObject* Attr1 = Xml2->GetFirstAttribute();
	wchar_t Attr1Val[4096] = L"";
	Attr1->ToXml(Attr1Val, 4096);
	wprintf(L"XmlEncoding(Multi)#Attribute value '<', '>', '&', '\"' and '\'' are escaped ... ");
	if (StrStr(Attr1Val, L"Lt=\"&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;\"") == 0 ||
		StrStr(Attr1Val, L"Gt=\"&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;\"") == 0 ||
		StrStr(Attr1Val, L"Amp=\"&amp;&amp;&amp;&amp;&amp;&amp;&amp;&amp;&amp;&amp;\"") == 0 ||
		StrStr(Attr1Val, L"Quot=\"&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;\"") == 0 ||
		StrStr(Attr1Val, L"Apos=\"&apos;&apos;&apos;&apos;&apos;&apos;&apos;&apos;&apos;&apos;\"") == 0) {
		printf("NG\r\n");
		exit(0);
	} else {
		printf("OK\r\n");
	}

	StkObject* Elem1 = Xml2->GetFirstChildElement();
	wchar_t Elem1Val[4096] = L"";
	Elem1->ToXml(Elem1Val, 4096);
	wprintf(L"XmlEncoding(Multi)#Element value '<', '>', '&', '\"' and '\'' are escaped ... ");
	if (StrStr(Elem1Val, L">&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;<") == 0 ||
		StrStr(Elem1Val, L">&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;<") == 0 ||
		StrStr(Elem1Val, L">&amp;&amp;&amp;&amp;&amp;&amp;&amp;&amp;&amp;&amp;<") == 0 ||
		StrStr(Elem1Val, L">&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;<") == 0 ||
		StrStr(Elem1Val, L">&apos;&apos;&apos;&apos;&apos;&apos;&apos;&apos;&apos;&apos;<") == 0) {
		printf("NG\r\n");
		exit(0);
	} else {
		printf("OK\r\n");
	}
	delete Xml2;
}

void XmlJsonDecodingTest1()
{
	wprintf(L"XmlJsonDecodingTest1 ... ");
	int Offset;

	StkObject* RetObjJson = StkObject::CreateObjectFromJson(L"\"Abc\" : {\"Xyz\" : \"123\"}", &Offset);
	StkObject* RetObjXml = StkObject::CreateObjectFromXml(L"<Abc><Xyz>123</Xyz></Abc>", &Offset);
	if (RetObjJson->Equals(RetObjXml)) {
		printf("OK\r\n");
	} else {
		printf("NG\r\n");
		exit(0);
	}
	delete RetObjJson;
	delete RetObjXml;
}

void XmlJsonEncodingTest1()
{
	StkObject* TopElem = new StkObject(L"House");

	StkObject* ChildElem1 = new StkObject(L"KitchenRoom");
	ChildElem1->AppendChildElement(new StkObject(L"Furniture", L"Kitchen Table"));
	ChildElem1->AppendChildElement(new StkObject(L"Furniture", L"Gas Cooker"));
	ChildElem1->AppendChildElement(new StkObject(L"Furniture", L"Kitchen Rack"));

	StkObject* ChildElem2 = new StkObject(L"KidsRoom");

	StkObject* ChildElem2a = new StkObject(L"Kid");
	ChildElem2a->AppendChildElement(new StkObject(L"Furniture", L"Desk"));
	ChildElem2a->AppendChildElement(new StkObject(L"Furniture", L"Chair"));
	ChildElem2a->AppendChildElement(new StkObject(L"Furniture", L"Bed"));
	ChildElem2->AppendChildElement(ChildElem2a);

	StkObject* ChildElem2b = new StkObject(L"Kid");
	ChildElem2b->AppendChildElement(new StkObject(L"Furniture", L"Wood Desk"));
	ChildElem2b->AppendChildElement(new StkObject(L"Furniture", L"Wood Chair"));
	ChildElem2b->AppendChildElement(new StkObject(L"Furniture", L"Wood Bed"));
	StkObject* ChildElem2bx = new StkObject(L"WalkInCloset");
	ChildElem2bx->AppendChildElement(new StkObject(L"Hunger", L"Wood"));
	ChildElem2bx->AppendChildElement(new StkObject(L"Rack", L"Wood"));
	ChildElem2b->AppendChildElement(ChildElem2bx);
	ChildElem2->AppendChildElement(ChildElem2b);

	StkObject* ChildElem2c = new StkObject(L"Kid");
	ChildElem2c->AppendChildElement(new StkObject(L"Furniture", L"Steel Desk"));
	ChildElem2c->AppendChildElement(new StkObject(L"Furniture", L"Steel Chair"));
	ChildElem2c->AppendChildElement(new StkObject(L"Furniture", L"Steel Bed"));
	ChildElem2->AppendChildElement(ChildElem2c);

	StkObject* ChildElem3 = new StkObject(L"Workroom");

	StkObject* ChildElem3a = new StkObject(L"Rack");
	ChildElem3a->AppendChildElement(new StkObject(L"Big-Rack", L"Steel Blue"));
	ChildElem3a->AppendChildElement(new StkObject(L"Big-Rack", L"Steel Red"));
	ChildElem3a->AppendChildElement(new StkObject(L"Big-Rack", L"Steel Green"));
	ChildElem3a->AppendChildElement(new StkObject(L"Small-Rack", L"Steel Blue"));
	ChildElem3a->AppendChildElement(new StkObject(L"Small-Rack", L"Steel Green"));
	ChildElem3a->AppendChildElement(new StkObject(L"Big-Rack", L"Wood Blue"));
	ChildElem3a->AppendChildElement(new StkObject(L"Big-Rack", L"Wood Red"));
	ChildElem3a->AppendChildElement(new StkObject(L"Big-Rack", L"Wood Green"));
	ChildElem3a->AppendChildElement(new StkObject(L"Small-Rack", L"Wood Blue"));
	ChildElem3a->AppendChildElement(new StkObject(L"Small-Rack", L"Wood Green"));
	ChildElem3->AppendChildElement(ChildElem3a);

	StkObject* ChildElem3b = new StkObject(L"Rack");
	ChildElem3b->AppendChildElement(new StkObject(L"Tiny-Rack", L"Steel Blue"));
	ChildElem3b->AppendChildElement(new StkObject(L"Tiny-Rack", L"Steel Red"));
	ChildElem3->AppendChildElement(ChildElem3b);

	StkObject* ChildElem3c = new StkObject(L"Table");
	ChildElem3c->AppendChildElement(new StkObject(L"WorkTable", L"Steel"));
	ChildElem3c->AppendChildElement(new StkObject(L"WorkTable", L"Wood"));
	ChildElem3->AppendChildElement(ChildElem3c);

	StkObject* ChildElem3d = new StkObject(L"Rack");
	ChildElem3d->AppendChildElement(new StkObject(L"Tiny-Rack", L"Steel Blue"));
	ChildElem3d->AppendChildElement(new StkObject(L"Tiny-Rack", L"Steel Red"));
	ChildElem3->AppendChildElement(ChildElem3d);


	TopElem->AppendChildElement(ChildElem1);
	TopElem->AppendChildElement(ChildElem2);
	TopElem->AppendChildElement(ChildElem3);

	wchar_t XmlTxt[8192] = L"";
	wchar_t JsonTxt[8192] = L"";
	TopElem->ToXml(XmlTxt, 8192);
	TopElem->ToJson(JsonTxt, 8192);

	int Offset;
	StkObject* ObjFromXml  = StkObject::CreateObjectFromXml(XmlTxt, &Offset);
	StkObject* ObjFromJson = StkObject::CreateObjectFromJson(JsonTxt, &Offset);
	wchar_t Temp1[8192] = L"";
	wchar_t Temp2[8192] = L"";
	printf("[Obj--->XML--->Obj--->JSON] and [Obj--->JSON--->Obj--->JSON] bring the same result...");
	ObjFromXml->ToJson(Temp1, 8192);
	ObjFromJson->ToJson(Temp2, 8192);
	if (lstrcmp(Temp1, Temp2) != 0) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");

	printf("[Obj--->XML--->Obj--->XML] and [Obj--->JSON--->Obj--->XML] bring the same result...");
	lstrcpy(Temp1, L"");
	lstrcpy(Temp2, L"");
	ObjFromXml->ToXml(Temp1, 8192);
	ObjFromJson->ToXml(Temp2, 8192);
	if (lstrcmp(Temp1, Temp2) != 0) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");

	delete TopElem;
}

void XmlJsonEncodingTest2()
{
	wchar_t Temp1[2048] = L"";
	wchar_t Temp2[2048] = L"";

	StkObject* TopElem = new StkObject(L"Japan");
	TopElem->AppendAttribute(new StkObject(L"", L""));
	TopElem->AppendAttribute(new StkObject(L"A", L"B"));
	StkObject* ChildElem = new StkObject(L"");
	ChildElem->AppendChildElement(new StkObject(L""));
	ChildElem->AppendChildElement(new StkObject(L"", L""));
	ChildElem->AppendChildElement(new StkObject(L"A", L"B"));
	TopElem->AppendChildElement(ChildElem);
	TopElem->ToXml(Temp1, 2048);
	TopElem->ToJson(Temp2, 2048);
	printf("XML encoding with no name and no value...");
	if (StrStr(Temp1, L"<Japan =\"\" A=\"B\">\r\n  <>\r\n    </>\r\n    <></>\r\n    <A>B</A>\r\n  </>\r\n</Japan>") == 0) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");
	printf("JSON encoding with no name and no value...");
	if (StrStr(Temp2, L"\"Japan\" : {\r\n  \"@attributes\" : {\r\n    \"\" : \"\", \"A\" : \"B\"\r\n  },\r\n  {\r\n    \"\" : [\r\n      {\r\n      },\r\n      \"\"\r\n    ],\r\n    \"A\" : \"B\"\r\n  }\r\n}") == 0) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");
	delete TopElem;
}

void XmlJsonEncodingTest3()
{
	wchar_t Buf[4096];
	wchar_t Tmp[4096];

	StkObject* Dat = MakeTestData1();

	printf("Encoded XML data is stored into unsufficient buffer...");
	lstrcpy(Tmp, L"");
	Dat->ToXml(Tmp, 4096);
	for (int Len = 4096; Len > 1; Len--) {
		lstrcpy(Buf, L"");
		Dat->ToXml(Buf, Len);
		if (StrStr(Tmp, Buf) == 0) {
			printf("NG\r\n");
			exit(0);
		}
	}
	printf("OK\r\n");

	printf("Encoded JSON data is stored into unsufficient buffer...");
	lstrcpy(Tmp, L"");
	Dat->ToJson(Tmp, 4096);
	for (int Len = 4096; Len > 1; Len--) {
		lstrcpy(Buf, L"");
		Dat->ToJson(Buf, Len);
		if (StrStr(Tmp, Buf) == 0) {
			printf("NG\r\n");
			exit(0);
		}
	}
	printf("OK\r\n");
}

void XmlDecodingTest1()
{
	int Offset = 0;

	wchar_t* Xml1 = L"<?xml version=\"1.0\"?><!-- comment --><Aaaa A1=\"a1\" A2=\"a2\" A3=\"a3\"><Bbbb B1=\"b1\" B2=\"b2\" B3=\"b3\"/><Cccc C1=\"c1\" C2=\"c2\" C3=\"c3\"/></Aaaa>";
	wchar_t* Xml2 = L"<?xml version=\"1.0\"?><!-- comment --><Aaaa A1=\"a1\" A2=\"a2\" A3=\"a3\"><Bbbb B1=\"b1\" B2=\"b2\" B3=\"b3\"></Bbbb><Cccc C1=\"c1\" C2=\"c2\" C3=\"c3\"></Cccc></Aaaa>";
	wchar_t* Xml3 = L"  <  Aaaa  A1  =  \"a1\"  A2  =  \"a2\"  A3  =  \"a3\"  >  <  Bbbb  B1  =  \"b1\"  B2  =  \"b2\"  B3  =  \"b3\"  /  >  <  Cccc  C1  =  \"c1\"  C2  =  \"c2\"  C3  =  \"c3\"  /  >  <  /  Aaaa  >  ";
	wchar_t* Xml4 = L"\t<\tAaaa\tA1\t=\t\"a1\"\tA2\t=\t\"a2\"\tA3\t=\t\"a3\"\t>\t<\tBbbb\tB1\t=\t\"b1\"\tB2\t=\t\"b2\"\tB3\t=\t\"b3\"\t/\t>\t<\tCccc\tC1\t=\t\"c1\"\tC2\t=\t\"c2\"\tC3\t=\t\"c3\"\t/\t>\t<\t/\tAaaa\t>\t";
	wchar_t* Xml5 = L"\r\n<\r\nAaaa\r\nA1\r\n=\r\n\"a1\"\r\nA2\r\n=\r\n\"a2\"\r\nA3\r\n=\r\n\"a3\"\r\n>\r\n<\r\nBbbb\r\nB1\r\n=\r\n\"b1\"\r\nB2\r\n=\r\n\"b2\"\r\nB3\r\n=\r\n\"b3\"\r\n/\r\n>\r\n<\r\nCccc\r\nC1\r\n=\r\n\"c1\"\r\nC2\r\n=\r\n\"c2\"\r\nC3\r\n=\r\n\"c3\"\r\n/\r\n>\r\n<\r\n/\r\nAaaa\r\n>\r\n";

	StkObject* RetObj1 = StkObject::CreateObjectFromXml(Xml1, &Offset);
	StkObject* RetObj2 = StkObject::CreateObjectFromXml(Xml2, &Offset);
	StkObject* RetObj3 = StkObject::CreateObjectFromXml(Xml3, &Offset);
	StkObject* RetObj4 = StkObject::CreateObjectFromXml(Xml4, &Offset);
	StkObject* RetObj5 = StkObject::CreateObjectFromXml(Xml5, &Offset);

	wchar_t Msg1[1024] = L"";
	wchar_t Msg2[1024] = L"";
	wchar_t Msg3[1024] = L"";
	wchar_t Msg4[1024] = L"";
	wchar_t Msg5[1024] = L"";

	RetObj1->ToXml(Msg1, 1024);
	RetObj2->ToXml(Msg2, 1024);
	RetObj3->ToXml(Msg3, 1024);
	RetObj4->ToXml(Msg4, 1024);
	RetObj5->ToXml(Msg5, 1024);

	printf("Decoding variation test (sampling check)...");
	if (lstrcmp(RetObj1->GetFirstChildElement()->GetNext()->GetFirstAttribute()->GetStringValue(), L"c1") != 0 ||
		lstrcmp(RetObj2->GetFirstChildElement()->GetNext()->GetFirstAttribute()->GetStringValue(), L"c1") != 0 ||
		lstrcmp(RetObj3->GetFirstChildElement()->GetNext()->GetFirstAttribute()->GetStringValue(), L"c1") != 0 ||
		lstrcmp(RetObj4->GetFirstChildElement()->GetNext()->GetFirstAttribute()->GetStringValue(), L"c1") != 0 ||
		lstrcmp(RetObj5->GetFirstChildElement()->GetNext()->GetFirstAttribute()->GetStringValue(), L"c1") != 0) {
		printf("NG\r\n");
		exit(0);
	} else {
		printf("OK\r\n");
	}

	printf("Decoding variation test (ToXml check)...");
	if (lstrcmp(Msg1, Msg2) != 0 || lstrcmp(Msg1, Msg3) != 0 || lstrcmp(Msg1, Msg4) != 0 || lstrcmp(Msg1, Msg5) != 0) {
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

	wchar_t* Xml1 = L"<Aaaa>=</Aaaa>";
	wchar_t* Xml2 = L"<Aaaa>/</Aaaa>";
	wchar_t* Xml3 = L"<Aaaa>?</Aaaa>";
	wchar_t* Xml4 = L"<Aaaa>!</Aaaa>";

	StkObject* RetObj1 = StkObject::CreateObjectFromXml(Xml1, &Offset);
	StkObject* RetObj2 = StkObject::CreateObjectFromXml(Xml2, &Offset);
	StkObject* RetObj3 = StkObject::CreateObjectFromXml(Xml3, &Offset);
	StkObject* RetObj4 = StkObject::CreateObjectFromXml(Xml4, &Offset);

	if (lstrcmp(RetObj1->GetStringValue(), L"=") != 0) {
		printf("NG\r\n");
		exit(0);
	}
	if (lstrcmp(RetObj2->GetStringValue(), L"/") != 0) {
		printf("NG\r\n");
		exit(0);
	}
	if (lstrcmp(RetObj3->GetStringValue(), L"?") != 0) {
		printf("NG\r\n");
		exit(0);
	}
	if (lstrcmp(RetObj4->GetStringValue(), L"!") != 0) {
		printf("NG\r\n");
		exit(0);
	}

	delete RetObj1;
	delete RetObj2;
	delete RetObj3;
	delete RetObj4;
	printf("OK\r\n");
}

void XmlDecodingTest3(wchar_t* Name)
{
	wchar_t* Xml = new wchar_t[512];
	lstrcpy(Xml, L"<Aaaa Lt=\"&lt;\" Gt=\"&gt;\" Amp=\"&amp;\" Quot=\"&quot;\" Apos=\"&apos;\"><Bbbb>&lt;</Bbbb><Bbbb>&gt;</Bbbb><Bbbb>&amp;</Bbbb><Bbbb>&quot;</Bbbb><Bbbb>&apos;</Bbbb></Aaaa>");

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

	wprintf(L"%s#Attribute value '&lt;', '&gt;', '&amp;', '&quot;' and '&apos;' are decoded appropriately... ", Name);
	if (lstrcmp(Attr1->GetStringValue(), L"<") != 0 ||
		lstrcmp(Attr2->GetStringValue(), L">") != 0 ||
		lstrcmp(Attr3->GetStringValue(), L"&") != 0 ||
		lstrcmp(Attr4->GetStringValue(), L"\"") != 0 ||
		lstrcmp(Attr5->GetStringValue(), L"\'") != 0) {
		printf("NG\r\n");
		exit(0);
	} else {
		printf("OK\r\n");
	}

	wprintf(L"%s#Element value '&lt;', '&gt;', '&amp;', '&quot;' and '&apos;' are decoded appropriately... ", Name);
	if (lstrcmp(Elem1->GetStringValue(), L"<") != 0 ||
		lstrcmp(Elem2->GetStringValue(), L">") != 0 ||
		lstrcmp(Elem3->GetStringValue(), L"&") != 0 ||
		lstrcmp(Elem4->GetStringValue(), L"\"") != 0 ||
		lstrcmp(Elem5->GetStringValue(), L"\'") != 0) {
		printf("NG\r\n");
		exit(0);
	} else {
		printf("OK\r\n");
	}

	delete Elem;
	delete Xml;
}

void XmlDecodingTest4(wchar_t* Name)
{
	wchar_t* Xml = new wchar_t[512];
	lstrcpy(Xml, L"<Aaaa Lt=\"&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;\" Apos=\"&apos;&apos;&apos;&apos;&apos;&apos;&apos;&apos;&apos;&apos;\"><Bbbb>&amp;&amp;&amp;&amp;&amp;&amp;&amp;&amp;&amp;&amp;</Bbbb><Bbbb>&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;</Bbbb></Aaaa>");

	int Offset = 0;
	StkObject* Elem = StkObject::CreateObjectFromXml(Xml, &Offset);
	StkObject* Attr1 = Elem->GetFirstAttribute();
	StkObject* Attr2 = Attr1->GetNext();
	StkObject* Elem1 = Elem->GetFirstChildElement();
	StkObject* Elem2 = Elem1->GetNext();

	wprintf(L"%s#Attribute value '&lt;...&lt;' and '&apos;...&apos;' are decoded appropriately... ", Name);
	if (lstrcmp(Attr1->GetStringValue(), L"<<<<<<<<<<") != 0 ||
		lstrcmp(Attr2->GetStringValue(), L"\'\'\'\'\'\'\'\'\'\'") != 0) {
		printf("NG\r\n");
		exit(0);
	} else {
		printf("OK\r\n");
	}

	wprintf(L"%s#Element value '&amp;...&amp;' and '&quot;...&quot;' are decoded appropriately... ", Name);
	if (lstrcmp(Elem1->GetStringValue(), L"&&&&&&&&&&") != 0 ||
		lstrcmp(Elem2->GetStringValue(), L"\"\"\"\"\"\"\"\"\"\"") != 0) {
		printf("NG\r\n");
		exit(0);
	} else {
		printf("OK\r\n");
	}

	delete Elem;
	delete Xml;
}

void XmlDecodingAbnormalTest1()
{
	AbnormalCaseForXml(L"", StkObject::ERROR_XML_NO_ELEMENT_FOUND);
	AbnormalCaseForXml(NULL, StkObject::ERROR_XML_NO_ELEMENT_FOUND);
	AbnormalCaseForXml(L"<>", StkObject::ERROR_XML_INVALID_ELEMENT_END_FOUND);
	AbnormalCaseForXml(L"<<", StkObject::ERROR_XML_INVALID_ELEMENT_START_FOUND);
	AbnormalCaseForXml(L"<\"ABC\"/>", StkObject::ERROR_XML_INVALID_QUOT_FOUND);
	AbnormalCaseForXml(L"xyz", StkObject::ERROR_XML_CANNOT_HANDLE);
	AbnormalCaseForXml(L"<Aaa Bbb=Ccc/>", StkObject::ERROR_XML_CANNOT_HANDLE);
	AbnormalCaseForXml(L"<Aaa Bbb Ccc=\"Ddd\"/>", StkObject::ERROR_XML_CANNOT_HANDLE);
	AbnormalCaseForXml(L"<Aaa Bbb=></Aaa>", StkObject::ERROR_XML_INVALID_ELEMENT_END_FOUND);
	AbnormalCaseForXml(L"<Aaa =></Aaa>", StkObject::ERROR_XML_EQUAL_FOUND_WITHOUT_ATTRIBUTE_NAME);
	AbnormalCaseForXml(L"<Aaa></Aaa", StkObject::ERROR_XML_SLASH_FOUND_WITHOUT_ELEMENT_END);
	AbnormalCaseForXml(L"<Aaa>/Aaa", StkObject::ERROR_XML_ELEMENT_END_NOT_FOUND);
	AbnormalCaseForXml(L"</Aaa>", StkObject::ERROR_XML_SLASH_FOUND_WITHOUT_ELEMENT);
	AbnormalCaseForXml(L"<Aaaa", StkObject::ERROR_XML_ELEMENT_END_NOT_FOUND);
	AbnormalCaseForXml(L"<Aaaa Xxx=\"Xxx", StkObject::ERROR_XML_ELEMENT_END_NOT_FOUND);
	AbnormalCaseForXml(L"<?xml?></?xml?>", StkObject::ERROR_XML_SLASH_FOUND_WITHOUT_ELEMENT);
	AbnormalCaseForXml(L"<Aaa><Bbb><Cc", StkObject::ERROR_XML_ELEMENT_END_NOT_FOUND);
	AbnormalCaseForXml(L"<Aaa>&", StkObject::ERROR_XML_ELEMENT_END_NOT_FOUND);
	AbnormalCaseForXml(L"/<Aaa></Aaa>", StkObject::ERROR_XML_INVALID_SLASH_FOUND);
}

void JsonDecodingTest1()
{
	wchar_t Msg[512];
	int Offset;
	StkObject* RetObj;

	////////////////////////////////////////////////////
	lstrcpy(Msg, L"\"Aaa\" : { \"Bbb\" : \"This is a test.\", \"Ccc\" : 123, \"Ddd\" : 999.9 }");
	wprintf(L"JSON Decoding : %s ...", Msg);
	RetObj = StkObject::CreateObjectFromJson(Msg, &Offset);
	if (RetObj == NULL || RetObj->GetChildElementCount() != 3 || lstrcmp(RetObj->GetName(), L"Aaa") != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetStringValue(), L"This is a test.") != 0 ||
		RetObj->GetFirstChildElement()->GetNext()->GetNext()->GetFloatValue() != 999.9f) {
		printf("NG\r\n");
		exit(0);
	}
	delete RetObj;
	printf("OK\r\n");
	////////////////////////////////////////////////////
	lstrcpy(Msg, L"\"Xxx\" : { \"Aaa\" : { \"Bbb\" : \"This is a test.\", \"Ccc\" : 123, \"Ddd\" : { \"D1\" : 0, \"D2\" : {\"D3a\" : \"test\"}, \"D3\" : 2}, \"Eee\" : 999.9 }}");
	wprintf(L"JSON Decoding : %s ...", Msg);
	RetObj = StkObject::CreateObjectFromJson(Msg, &Offset);
	if (RetObj == NULL ||
		lstrcmp(RetObj->GetFirstChildElement()->GetFirstChildElement()->GetNext()->GetNext()->GetFirstChildElement()->GetName(), L"D1") != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetFirstChildElement()->GetNext()->GetNext()->GetFirstChildElement()->GetNext()->GetFirstChildElement()->GetStringValue(), L"test") != 0 ||
		RetObj->GetFirstChildElement()->GetFirstChildElement()->GetNext()->GetNext()->GetNext()->GetFloatValue() != 999.9f) {
		printf("NG\r\n");
		exit(0);
	}
	delete RetObj;
	printf("OK\r\n");
	////////////////////////////////////////////////////
	lstrcpy(Msg, L"\"Yyy\" : {\"Xxx\" : [\"Aaa\", \"Bbb\", \"Ccc\"]}");
	wprintf(L"JSON Decoding : %s ...", Msg);
	RetObj = StkObject::CreateObjectFromJson(Msg, &Offset);
	if (RetObj == NULL ||
		lstrcmp(RetObj->GetFirstChildElement()->GetName(), L"Xxx") != 0 || lstrcmp(RetObj->GetFirstChildElement()->GetStringValue(), L"Aaa") != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetNext()->GetNext()->GetName(), L"Xxx") != 0 || lstrcmp(RetObj->GetFirstChildElement()->GetNext()->GetNext()->GetStringValue(), L"Ccc") != 0) {
		printf("NG\r\n");
		exit(0);
	}
	delete RetObj;
	printf("OK\r\n");
	////////////////////////////////////////////////////
	lstrcpy(Msg, L"\"Yyy\" : {\"Zzz\" : \"\", \"Xxx\" : [\"\", \"\", \"\"]}");
	wprintf(L"JSON Decoding : %s ...", Msg);
	RetObj = StkObject::CreateObjectFromJson(Msg, &Offset);
	if (RetObj == NULL ||
		lstrcmp(RetObj->GetFirstChildElement()->GetName(), L"Zzz") != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetStringValue(), L"") != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetNext()->GetName(), L"Xxx") != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetNext()->GetStringValue(), L"") != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetNext()->GetNext()->GetName(), L"Xxx") != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetNext()->GetNext()->GetStringValue(), L"") != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetNext()->GetNext()->GetNext()->GetName(), L"Xxx") != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetNext()->GetNext()->GetNext()->GetStringValue(), L"") != 0) {
		printf("NG\r\n");
		exit(0);
	}
	delete RetObj;
	printf("OK\r\n");
	////////////////////////////////////////////////////
	lstrcpy(Msg, L"{\"Aaa\" : \"\", \"Bbb\" : \"222\", \"Ccc\" : \"\", \"Ddd\" : \"444\"}");
	wprintf(L"JSON Decoding : %s ...", Msg);
	RetObj = StkObject::CreateObjectFromJson(Msg, &Offset);
	if (RetObj == NULL ||
		lstrcmp(RetObj->GetFirstChildElement()->GetName(), L"Aaa") != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetStringValue(), L"") != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetNext()->GetName(), L"Bbb") != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetNext()->GetStringValue(), L"222") != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetNext()->GetNext()->GetName(), L"Ccc") != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetNext()->GetNext()->GetStringValue(), L"") != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetNext()->GetNext()->GetNext()->GetName(), L"Ddd") != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetNext()->GetNext()->GetNext()->GetStringValue(), L"444") != 0) {
		printf("NG\r\n");
		exit(0);
	}
	delete RetObj;
	printf("OK\r\n");
	////////////////////////////////////////////////////
	lstrcpy(Msg, L"{\"Aaa\" : \"111\", \"Bbb\" : \"\", \"Ccc\" : \"333\", \"Ddd\" : \"\"}");
	wprintf(L"JSON Decoding : %s ...", Msg);
	RetObj = StkObject::CreateObjectFromJson(Msg, &Offset);
	if (RetObj == NULL ||
		lstrcmp(RetObj->GetFirstChildElement()->GetName(), L"Aaa") != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetStringValue(), L"111") != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetNext()->GetName(), L"Bbb") != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetNext()->GetStringValue(), L"") != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetNext()->GetNext()->GetName(), L"Ccc") != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetNext()->GetNext()->GetStringValue(), L"333") != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetNext()->GetNext()->GetNext()->GetName(), L"Ddd") != 0 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetNext()->GetNext()->GetNext()->GetStringValue(), L"") != 0) {
		printf("NG\r\n");
		exit(0);
	}
	delete RetObj;
	printf("OK\r\n");
	////////////////////////////////////////////////////
	lstrcpy(Msg, L"\"Yyy\" : {\"Xxx\" : [{\"Aaa\" : 123, \"Bbb\" : 456, \"Ccc\":789},{\"Aaa\" : [333, 222, 111]}]}");
	wprintf(L"JSON Decoding : %s ...", Msg);
	RetObj = StkObject::CreateObjectFromJson(Msg, &Offset);
	if (RetObj == NULL ||
		lstrcmp(RetObj->GetFirstChildElement()->GetFirstChildElement()->GetName(), L"Aaa") != 0 || RetObj->GetFirstChildElement()->GetFirstChildElement()->GetNext()->GetIntValue() != 456 ||
		lstrcmp(RetObj->GetFirstChildElement()->GetNext()->GetFirstChildElement()->GetNext()->GetName(), L"Aaa") != 0 || RetObj->GetFirstChildElement()->GetNext()->GetFirstChildElement()->GetNext()->GetIntValue() != 222) {
		printf("NG\r\n");
		exit(0);
	}
	delete RetObj;
	printf("OK\r\n");
	////////////////////////////////////////////////////
	{
		lstrcpy(Msg, L"{ \"Aaa\" : { \"Bbb\" : \"This is a test.\", \"Ccc\" : 123, \"Ddd\" : { \"D1\" : 0, \"D2\" : {\"D3a\" : \"test\"}, \"D3\" : 2}, \"Eee\" : 999.9 }}");
		wprintf(L"JSON Decoding : %s ...", Msg);
		RetObj = StkObject::CreateObjectFromJson(Msg, &Offset);
		if (RetObj == NULL ||
			lstrcmp(RetObj->GetFirstChildElement()->GetFirstChildElement()->GetNext()->GetNext()->GetFirstChildElement()->GetName(), L"D1") != 0 ||
			lstrcmp(RetObj->GetFirstChildElement()->GetFirstChildElement()->GetNext()->GetNext()->GetFirstChildElement()->GetNext()->GetFirstChildElement()->GetStringValue(), L"test") != 0 ||
			RetObj->GetFirstChildElement()->GetFirstChildElement()->GetNext()->GetNext()->GetNext()->GetFloatValue() != 999.9f) {
			printf("NG\r\n");
			exit(0);
		}
		wchar_t TmpBuf[4096] = L"";
		RetObj->ToJson(TmpBuf, 4096);
		StkObject* TmpObj = StkObject::CreateObjectFromJson(TmpBuf, &Offset);
		if (TmpObj->Equals(RetObj) == false) {
			printf("NG\r\n");
			exit(0);
		}
		delete TmpObj;
		delete RetObj;
		printf("OK\r\n");
	}
	////////////////////////////////////////////////////
	{
		lstrcpy(Msg, L"{\"Xxx\" : [{\"Aaa\" : 123, \"Bbb\" : 456, \"Ccc\":789},{\"Aaa\" : [333, 222, 111]}]}");
		wprintf(L"JSON Decoding : %s ...", Msg);
		RetObj = StkObject::CreateObjectFromJson(Msg, &Offset);
		if (RetObj == NULL ||
			lstrcmp(RetObj->GetFirstChildElement()->GetFirstChildElement()->GetName(), L"Aaa") != 0 || RetObj->GetFirstChildElement()->GetFirstChildElement()->GetNext()->GetIntValue() != 456 ||
			lstrcmp(RetObj->GetFirstChildElement()->GetNext()->GetFirstChildElement()->GetNext()->GetName(), L"Aaa") != 0 || RetObj->GetFirstChildElement()->GetNext()->GetFirstChildElement()->GetNext()->GetIntValue() != 222) {
			printf("NG\r\n");
			exit(0);
		}
		wchar_t TmpBuf[4096] = L"";
		RetObj->ToJson(TmpBuf, 4096);
		StkObject* TmpObj = StkObject::CreateObjectFromJson(TmpBuf, &Offset);
		if (TmpObj->Equals(RetObj) == false) {
			printf("NG\r\n");
			exit(0);
		}
		delete TmpObj;
		delete RetObj;
		printf("OK\r\n");
	}
}

void JsonDecodingTest2()
{
	wchar_t Msg1[512];
	wchar_t Msg2[512];
	wchar_t Msg3[512];
	wchar_t Msg4[512];
	StkObject* RetObj1;
	StkObject* RetObj2;
	StkObject* RetObj3;
	StkObject* RetObj4;
	wchar_t Str1[1024] = L"";
	wchar_t Str2[1024] = L"";
	wchar_t Str3[1024] = L"";
	wchar_t Str4[1024] = L"";
	int Offset;

	////////////////////////////////////////////////////
	lstrcpy(Msg1, L"\"Aaa\":{\"Bbb\":[{\"Xxx\":123,\"Bbb\":456,\"Ccc\":789},\"test\",0.1]}");
	lstrcpy(Msg2, L"\"Aaa\"    :    {    \"Bbb\"    :    [    {    \"Xxx\"    :    123    ,    \"Bbb\"    :    456    ,    \"Ccc\"    :    789    }    ,    \"test\"    ,    0.1    ]    }    ");
	lstrcpy(Msg3, L"\t\t\"Aaa\"\t\t:\t\t{\t\t\"Bbb\"\t\t:\t\t[\t\t{\t\t\"Xxx\"\t\t:\t\t123\t\t,\t\t\"Bbb\"\t\t:\t\t456\t\t,\t\t\"Ccc\"\t\t:\t\t789\t\t}\t\t,\t\t\"test\"\t\t,\t\t0.1\t\t]\t\t}\t\t");
	lstrcpy(Msg4, L"\r\n\"Aaa\"\r\n:\r\n{\r\n\"Bbb\"\r\n:\r\n[\r\n{\r\n\"Xxx\"\r\n:\r\n123\r\n,\r\n\"Bbb\"\r\n:\r\n456\r\n,\r\n\"Ccc\"\r\n:\r\n789\r\n}\r\n,\r\n\"test\"\r\n,\r\n0.1\r\n]\r\n}\r\n");
	RetObj1 = StkObject::CreateObjectFromJson(Msg1, &Offset);
	RetObj2 = StkObject::CreateObjectFromJson(Msg2, &Offset);
	RetObj3 = StkObject::CreateObjectFromJson(Msg3, &Offset);
	RetObj4 = StkObject::CreateObjectFromJson(Msg4, &Offset);
	RetObj1->ToJson(Str1, 1024);
	RetObj2->ToJson(Str2, 1024);
	RetObj3->ToJson(Str3, 1024);
	RetObj4->ToJson(Str4, 1024);
	printf("JSON Decoding (empty charactor)...");
	if (lstrcmp(Str1, Str2) != 0 || lstrcmp(Str2, Str3) != 0 || lstrcmp(Str3, Str4) != 0) {
		printf("NG\r\n");
		exit(0);
	}
	printf("OK\r\n");
	delete RetObj1;
	delete RetObj2;
	delete RetObj3;
	delete RetObj4;
}

void JsonDecodingAbnormalTest1()
{
	AbnormalCaseForJson(L"", StkObject::ERROR_JSON_NO_ELEMENT_FOUND);
	AbnormalCaseForJson(NULL, StkObject::ERROR_JSON_NO_ELEMENT_FOUND);
	AbnormalCaseForJson(L"[ \"Aaa\" : \"Xxx\" ]", StkObject::ERROR_JSON_INVALID_ARRAY_STRUCTURE);
	AbnormalCaseForJson(L"\"Aaa\" : 123", StkObject::ERROR_JSON_NO_ROOT_ELEMENT);
	AbnormalCaseForJson(L"\"Aaa\" : 999.9", StkObject::ERROR_JSON_NO_ROOT_ELEMENT);
	AbnormalCaseForJson(L"\"Aaa\" : \"Xyz\"", StkObject::ERROR_JSON_NO_ROOT_ELEMENT);
	AbnormalCaseForJson(L"\"Aaa\" : { \"Xyz\" : 123, \"Abc\" : ", StkObject::ERROR_JSON_INVALID_STRUCTURE);
	AbnormalCaseForJson(L"\"Aaa\" : { \"Xyz\" : 123, \"Abc\" : \"", StkObject::ERROR_JSON_INVALID_STRUCTURE);
	AbnormalCaseForJson(L"\"Aaa\" : { \"Xyz\" : 123, \"Abc\" \"Xyz\" }", StkObject::ERROR_JSON_INVALID_QUOT_FOUND);
	AbnormalCaseForJson(L"\"Aaa\" : { \"Bbb\" : 123, Ccc : 456 }", StkObject::ERROR_JSON_CANNOT_HANDLE);
	AbnormalCaseForJson(L"\"Aaa\" : { \"Bbb\" : 123, \"Ccc\" : 456, \"Ddd\" : 789 {", StkObject::ERROR_JSON_INVALID_STRUCTURE);
	AbnormalCaseForJson(L"\"Aaa\" : {}", StkObject::ERROR_JSON_INVALID_STRUCTURE);
	AbnormalCaseForJson(L": { \"Aaa\" : \"Xxx\" }", StkObject::ERROR_JSON_INVALID_COLON_FOUND);
}

void CloneTest(StkObject* Obj)
{
	wchar_t OrgMsg[8192] = L"";
	Obj->ToXml(OrgMsg, 8192);

	StkObject* NewObj1 = Obj->Clone();
	StkObject* NewObj2 = NewObj1->Clone();

	wchar_t Msg1[8192] = L"";
	wchar_t Msg2[8192] = L"";
	NewObj1->ToXml(Msg1, 8192);
	NewObj2->ToXml(Msg2, 8192);

	printf("Clone Test#Validation of two clone's string...");
	if (lstrcmp(Msg1, Msg2) == 0 && lstrcmp(Msg1, OrgMsg) == 0 && NewObj1->Equals(NewObj2) && NewObj1->Equals(Obj)) {
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
			wchar_t Msg1[8192] = L"";
			PassedObj->ToXml(Msg1, 8192);
			wchar_t Msg2[8192] = L"";
			PassedObj->ToJson(Msg2, 8192);
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
	int Offset = 0;
	StkObject* NewObj1;
	StkObject* NewObj2;

	for (int CreationLoop = 0; CreationLoop < 30; CreationLoop++) {
		for (int Loop = 0; Loop < 250; Loop++) {
			wchar_t StrVal1[4096] = L"";
			NewObj1 = new StkObject(L"EncodeTesting");
			NewObj1->AppendAttribute(new StkObject(L"XmlLt", L"<<<<<<<<<<<<<<<<<<<<"));
			NewObj1->AppendAttribute(new StkObject(L"XmlGt", L">>>>>>>>>>>>>>>>>>>>"));
			NewObj1->AppendAttribute(new StkObject(L"XmlApos", L"\'\'\'\'\'\'\'\'\'\'\'\'\'\'\'\'\'\'\'\'"));
			NewObj1->AppendChildElement(new StkObject(L"XmlAmp", L"&&&&&&&&&&&&&&&&&&&&"));
			NewObj1->AppendChildElement(new StkObject(L"XmlQuot", L"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\""));
			NewObj1->ToXml(StrVal1, 4096);
			delete NewObj1;

			wchar_t StrVal2[4096] = L"";
			NewObj1 = new StkObject(L"EncodeTesting");
			NewObj1->AppendChildElement(new StkObject(L"Bbbb", L"\"\"\"\"\"\"\"\"\"\""));
			NewObj1->AppendChildElement(new StkObject(L"Bbbb", L"\\\\\\\\\\\\\\\\\\\\"));
			NewObj1->AppendChildElement(new StkObject(L"Bbbb", L"//////////"));
			NewObj1->AppendChildElement(new StkObject(L"Bbbb", L"\b\b\b\b\b\b\b\b\b\b"));
			NewObj1->AppendChildElement(new StkObject(L"Bbbb", L"\f\f\f\f\f\f\f\f\f\f"));
			NewObj1->AppendChildElement(new StkObject(L"Bbbb", L"\n\n\n\n\n\n\n\n\n\n"));
			NewObj1->AppendChildElement(new StkObject(L"Bbbb", L"\r\r\r\r\r\r\r\r\r\r"));
			NewObj1->AppendChildElement(new StkObject(L"Bbbb", L"\t\t\t\t\t\t\t\t\t\t"));
			NewObj1->ToJson(StrVal2, 4096);
			delete NewObj1;

			wchar_t StrVal3[4096] = L"";
			NewObj2 = StkObject::CreateObjectFromXml(L"<Aaaa Lt=\"&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;&lt;\" Gt=\"&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;&gt;\" Apos=\"&apos;&apos;&apos;&apos;&apos;&apos;&apos;&apos;&apos;&apos;\"><Amp>&amp;&amp;&amp;&amp;&amp;&amp;&amp;&amp;&amp;&amp;</Amp><Quot>&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;&quot;</Quot></Aaaa>", &Offset);
			NewObj2->ToXml(StrVal3, 4096);
			delete NewObj2;

			wchar_t StrVal4[4096];
			lstrcpy(StrVal4, L"");
			NewObj2 = StkObject::CreateObjectFromJson(L"\"Aaaa\" : { \"Bbbb\" : \"\\\"\\\"\\\"\\\"\\\"\\\"\\\"\\\"\\\"\\\"\", \"Bbbb\" : \"\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\", \"Bbbb\" : \"\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\", \"Bbbb\" : \"\\b\\b\\b\\b\\b\\b\\b\\b\\b\\b\", \"Bbbb\" : \"\\f\\f\\f\\f\\f\\f\\f\\f\\f\\f\", \"Bbbb\" : \"\\n\\n\\n\\n\\n\\n\\n\\n\\n\\n\", \"Bbbb\" : \"\\r\\r\\r\\r\\r\\r\\r\\r\\r\\r\", \"Bbbb\" : \"\\t\\t\\t\\t\\t\\t\\t\\t\\t\\t\" }", &Offset);
			NewObj2->ToJson(StrVal4, 4096);
			delete NewObj2;
			lstrcpy(StrVal4, L"");
			NewObj2 = StkObject::CreateObjectFromJson(L"\"aaa\" : {\"Hello\" : { \"FirstName\" : \"Shinya\", \"Middle\" : \"Tsunemi\", \"Last\" : \"Takeuchi\" }, \"Bye\" : \"Bye\"}", &Offset);
			NewObj2->ToJson(StrVal4, 4096);
			delete NewObj2;
			lstrcpy(StrVal4, L"");
			NewObj2 = StkObject::CreateObjectFromJson(L"\"Yyy\" : {\"Xxx\" : [{\"Aaa\" : 123, \"Bbb\" : 456, \"Ccc\":789},{\"Aaa\" : [333, 222, 111]}]}", &Offset);
			NewObj2->ToJson(StrVal4, 4096);
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
			StkObject::CreateObjectFromXml(L"", &Offset);
			StkObject::CreateObjectFromXml(NULL, &Offset);
			StkObject::CreateObjectFromXml(L"<X><Y><Z><></X></Y></Z>", &Offset);
			StkObject::CreateObjectFromXml(L"<X><Y><Z><<</X></Y></Z>", &Offset);
			StkObject::CreateObjectFromXml(L"<X><Y><Z><\"ABC\"/></X></Y></Z>", &Offset);
			StkObject::CreateObjectFromXml(L"xyz", &Offset);
			StkObject::CreateObjectFromXml(L"<X><Y><Z><Aaa Bbb=Ccc/></X></Y></Z>", &Offset);
			StkObject::CreateObjectFromXml(L"<X><Y><Z><Aaa Bbb Ccc=\"Ddd\"/></X></Y></Z>", &Offset);
			StkObject::CreateObjectFromXml(L"<X><Y><Z><Aaa Bbb=></Aaa></X></Y></Z>", &Offset);
			StkObject::CreateObjectFromXml(L"<X><Y><Z><Aaa =></Aaa></X></Y></Z>", &Offset);
			StkObject::CreateObjectFromXml(L"<Aaa></Aaa", &Offset);
			StkObject::CreateObjectFromXml(L"<Aaa>/Aaa", &Offset);
			StkObject::CreateObjectFromXml(L"</Aaa>", &Offset);
			StkObject::CreateObjectFromXml(L"<X><Y><Z><Aaaa", &Offset);
			StkObject::CreateObjectFromXml(L"<X><Y><Z><Aaaa Xxx=\"Xxx", &Offset);
			StkObject::CreateObjectFromXml(L"<Aaa><Bbb><Cc", &Offset);
			StkObject::CreateObjectFromXml(L"<Aaa>&", &Offset);
			StkObject::CreateObjectFromXml(L"/<Aaa></Aaa>", &Offset);

			// For JSON
			StkObject::CreateObjectFromJson(L"", &Offset);
			StkObject::CreateObjectFromJson(NULL, &Offset);
			StkObject::CreateObjectFromJson(L"[ \"Aaa\" : \"Xxx\" ]", &Offset);
			StkObject::CreateObjectFromJson(L"\"Aaa\" : 123", &Offset);
			StkObject::CreateObjectFromJson(L"\"Aaa\" : 999.9", &Offset);
			StkObject::CreateObjectFromJson(L"\"Aaa\" : \"Xyz\"", &Offset);
			StkObject::CreateObjectFromJson(L"\"Aaa\" : { \"Xyz\" : 123, \"Abc\" : ", &Offset);
			StkObject::CreateObjectFromJson(L"\"Aaa\" : { \"Xyz\" : 123, \"Abc\" : \"", &Offset);
			StkObject::CreateObjectFromJson(L"\"Aaa\" : { \"Xyz\" : 123, \"Abc\" \"Xyz\" }", &Offset);
			StkObject::CreateObjectFromJson(L"\"Aaa\" : { \"Bbb\" : 123, Ccc : 456 }", &Offset);
			StkObject::CreateObjectFromJson(L"\"Aaa\" : { \"Bbb\" : 123, \"Ccc\" : 456, \"Ddd\" : 789 {", &Offset);
			StkObject::CreateObjectFromJson(L"\"Aaa\" : {}", &Offset);
			StkObject::CreateObjectFromJson(L": { \"Aaa\" : \"Xxx\" }",  &Offset);
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
	for (int CreationLoop = 0; CreationLoop < 10; CreationLoop++) {
		StkObject* NewObj = MakeTestData3(L"Hello", 4, 5);
		delete NewObj;
	}
	for (int CreationLoop = 0; CreationLoop < 30; CreationLoop++) {
		StkObject* NewObj = MakeTestData3(L"Hello", 4, 5);
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

void PerformanceTest()
{
	printf("Performance test ... ");

	wchar_t JsonTxt[16000000];
	DWORD PrevMiliSec = 0;
	DWORD MiliSec = 0;

	for (int Loop = 2; Loop < 10; Loop++) {
		lstrcpy(JsonTxt, L"");
		StkObject* ObjWid = MakeTestData3(L"あいうえおかきくけこさしすせそたちつてとなにぬねのはひふへほ", 2, Loop);
		PrevMiliSec = GetTickCount();
		ObjWid->ToJson(JsonTxt, 16000000);
		MiliSec = GetTickCount() - PrevMiliSec;
		printf("JsonTxtLength[width=%d] = %d(%f Sec),   ", Loop, lstrlen(JsonTxt), (float)MiliSec / 1000.0f);
		delete ObjWid;
	}

	printf("\r\n");
	exit(0);
}

void StkObjectTest()
{
	printf("StkObjectTest started.\r\n");

	//PerformanceTest();

	// Encode check
	{
		JsonEncodingTest1();
		JsonEncodingTest2();
		XmlEncodingTest2();
		XmlEncodingTest3();
		XmlJsonEncodingTest1();
		XmlJsonEncodingTest2();
		XmlJsonEncodingTest3();
	}

	// Decode check
	{
		// XML
		XmlDecodingTest1();
		XmlDecodingTest2();
		XmlDecodingTest3(L"DecodeCheck1");
		XmlDecodingTest4(L"DecodeCheck2");
		XmlDecodingAbnormalTest1();

		// JSON
		JsonDecodingTest1();
		JsonDecodingTest2();
		JsonDecodingAbnormalTest1();

		// XML and JSON
		XmlJsonDecodingTest1();
	}

	// General check
	{
		StkObject* Elem1 = MakeTestData1();
		GeneralTestCase1(Elem1, L"GeneralCheck1");
		delete Elem1;

		StkObject* Elem2 = MakeTestData2();
		GeneralTestCase2(Elem2, L"GeneralCheck2");
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
