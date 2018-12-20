#pragma once
#include "..\..\src\stkwebapp\StkWebAppExec.h"

class StkWebAppTest4 : StkWebAppExec
{
	StkObject* Execute(StkObject*, int, wchar_t[StkWebAppExec::URL_PATH_LENGTH], int*, wchar_t[3]);
};
