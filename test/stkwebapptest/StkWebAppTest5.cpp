#include <windows.h>
#include <shlwapi.h>
#include "..\..\src\commonfunc\StkStringParser.h"
#include "StkWebAppTest5.h"

StkObject* StkWebAppTest5::Execute(StkObject* ReqObj, int Method, wchar_t UrlPath[StkWebAppExec::URL_PATH_LENGTH], int* ResultCode, wchar_t Locale[3])
{
	StkObject* RetObj = NULL;
	if (StrStr(UrlPath, L"/bigdata/") != NULL) {
		RetObj = new StkObject(L"BigData");
		for (int Loop = 0; Loop < 150; Loop++) {
			RetObj->AppendChildElement(new StkObject(L"ChildObj", L"Hello, world!!"));
		}
	}
	if (StrStr(UrlPath, L"/middledata/") != NULL) {
		RetObj = new StkObject(L"MiddleData");
		for (int Loop = 0; Loop < 90; Loop++) {
			RetObj->AppendChildElement(new StkObject(L"ChildObj", L"Hello, world!!"));
		}
	}
	if (StrStr(UrlPath, L"/smalldata/") != NULL) {
		RetObj = new StkObject(L"SmallData");
		for (int Loop = 0; Loop < 50; Loop++) {
			RetObj->AppendChildElement(new StkObject(L"ChildObj", L"Hello, world!!"));
		}
	}
	return RetObj;
}
