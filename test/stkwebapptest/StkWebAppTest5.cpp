#include <windows.h>
#include <tchar.h>
#include <shlwapi.h>
#include "..\..\src\commonfunc\StkStringParser.h"
#include "StkWebAppTest5.h"

StkObject* StkWebAppTest5::Execute(StkObject* ReqObj, int Method, TCHAR UrlPath[StkWebAppExec::URL_PATH_LENGTH], int* ResultCode, TCHAR Locale[3])
{
	StkObject* RetObj = NULL;
	if (StrStr(UrlPath, _T("/bigdata/")) != NULL) {
		RetObj = new StkObject(_T("BigData"));
		for (int Loop = 0; Loop < 150; Loop++) {
			RetObj->AppendChildElement(new StkObject(_T("ChildObj"), _T("Hello, world!!")));
		}
	}
	if (StrStr(UrlPath, _T("/middledata/")) != NULL) {
		RetObj = new StkObject(_T("MiddleData"));
		for (int Loop = 0; Loop < 90; Loop++) {
			RetObj->AppendChildElement(new StkObject(_T("ChildObj"), _T("Hello, world!!")));
		}
	}
	if (StrStr(UrlPath, _T("/smalldata/")) != NULL) {
		RetObj = new StkObject(_T("SmallData"));
		for (int Loop = 0; Loop < 50; Loop++) {
			RetObj->AppendChildElement(new StkObject(_T("ChildObj"), _T("Hello, world!!")));
		}
	}
	return RetObj;
}
