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
		for (int Loop = 0; Loop < 100; Loop++) {
			RetObj->AppendChildElement(new StkObject(_T("ChildObj"), _T("Hello, world!!")));
		}
	}
	return RetObj;
}
