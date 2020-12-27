#pragma once
#include "../stkwebapp/StkWebAppExec.h"
#include "ApiBase.h"

class ApiGetUser : ApiBase
{
protected:
	StkObject* ExecuteImpl(StkObject*, int, wchar_t[StkWebAppExec::URL_PATH_LENGTH], int*, wchar_t[3], wchar_t*);
};
