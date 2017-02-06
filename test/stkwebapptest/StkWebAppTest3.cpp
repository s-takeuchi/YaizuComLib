#include "StkWebAppTest3.h"

StkObject* StkWebAppTest3::Execute(StkObject* ReqObj, int Method, TCHAR UrlPath[128], int* ResultCode)
{
	return ReqObj->Clone();
}
