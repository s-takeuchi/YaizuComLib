#include "StkWebAppTest2.h"

StkObject* StkWebAppTest2::Execute(StkObject* ReqObj, int Method, TCHAR UrlPath[128], int* ResultCode)
{
	return ReqObj->Clone();
}
