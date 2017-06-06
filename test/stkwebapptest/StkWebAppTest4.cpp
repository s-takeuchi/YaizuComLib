#include <windows.h>
#include <tchar.h>
#include "..\..\src\commonfunc\StkStringParser.h"
#include "StkWebAppTest4.h"

StkObject* StkWebAppTest4::Execute(StkObject* ReqObj, int Method, TCHAR UrlPath[128], int* ResultCode, TCHAR Locale[3])
{
	TCHAR Buf[10];
	StkStringParser::ParseInto1Param(UrlPath, _T("/aaa/$/"), _T('$'), Buf, 10);
	*ResultCode = _wtoi(Buf);
	return NULL;
}
