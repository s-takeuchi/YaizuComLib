#include "StkWebAppTest2.h"

StkObject* StkWebAppTest2::Execute(StkObject* ReqObj, int Method, TCHAR UrlPath[StkWebAppExec::URL_PATH_LENGTH], int* ResultCode, TCHAR Locale[3])
{
	*ResultCode = 200;
	return ReqObj->Clone();
}
