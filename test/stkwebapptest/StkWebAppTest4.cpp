﻿#include <windows.h>
#include "..\..\src\commonfunc\StkStringParser.h"
#include "StkWebAppTest4.h"

StkObject* StkWebAppTest4::Execute(StkObject* ReqObj, int Method, wchar_t UrlPath[StkWebAppExec::URL_PATH_LENGTH], int* ResultCode, wchar_t Locale[3])
{
	wchar_t Buf[10];
	StkStringParser::ParseInto1Param(UrlPath, L"/aaa/$/", L'$', Buf, 10);
	*ResultCode = _wtoi(Buf);
	return NULL;
}
