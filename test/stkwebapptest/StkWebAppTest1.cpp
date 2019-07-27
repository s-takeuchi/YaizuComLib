#include "StkWebAppTest1.h"

StkObject* StkWebAppTest1::Execute(StkObject* ReqObj, int Method, wchar_t UrlPath[StkWebAppExec::URL_PATH_LENGTH], int* ResultCode, wchar_t Locale[3])
{
	*ResultCode = 200;
	return ReqObj->Clone();
}

StkObject* StkWebAppTest1::Execute(StkObject* ReqObj, int Method, wchar_t UrlPath[StkWebAppExec::URL_PATH_LENGTH], int* ResultCode, wchar_t Locale[3], wchar_t* HttpHeader)
{
	*ResultCode = 200;
	return ReqObj->Clone();
}
