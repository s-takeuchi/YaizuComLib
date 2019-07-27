#pragma once
#include "../../src/stkwebapp/StkWebAppExec.h"

class StkWebAppTest5 : StkWebAppExec
{
	StkObject* Execute(StkObject*, int, wchar_t[StkWebAppExec::URL_PATH_LENGTH], int*, wchar_t[3]);
	StkObject* Execute(StkObject*, int, wchar_t[StkWebAppExec::URL_PATH_LENGTH], int*, wchar_t[3], wchar_t*);
};
