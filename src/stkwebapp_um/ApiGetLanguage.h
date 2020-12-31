#pragma once
#include "ApiBase.h"

class ApiGetLanguage : public ApiBase
{
protected:
	StkObject* ExecuteImpl(StkObject*, int, wchar_t[StkWebAppExec::URL_PATH_LENGTH], int*, wchar_t[3], wchar_t*);
};
