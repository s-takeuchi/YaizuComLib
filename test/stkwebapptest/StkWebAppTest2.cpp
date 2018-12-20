#include "StkWebAppTest2.h"

StkObject* StkWebAppTest2::Execute(StkObject* ReqObj, int Method, wchar_t UrlPath[StkWebAppExec::URL_PATH_LENGTH], int* ResultCode, wchar_t Locale[3])
{
	*ResultCode = 200;
	return ReqObj->Clone();
}
