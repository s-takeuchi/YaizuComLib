#include "StkWebAppTest1.h"

StkObject* StkWebAppTest1::Execute(StkObject* ReqObj, int Method, TCHAR UrlPath[128], int* ResultCode, TCHAR Locale[3])
{
	*ResultCode = 200;
	return ReqObj->Clone();
}
