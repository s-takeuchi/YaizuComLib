#include "StkWebAppTest3.h"

StkObject* StkWebAppTest3::Execute(StkObject* ReqObj, int Method, wchar_t UrlPath[StkWebAppExec::URL_PATH_LENGTH], int* ResultCode, wchar_t* HttpHeader)
{
	*ResultCode = 200;
	return ReqObj->Clone();
}
