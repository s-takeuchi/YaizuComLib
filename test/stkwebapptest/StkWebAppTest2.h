#pragma once
#include "..\..\src\stkwebapp\StkWebAppExec.h"

class StkWebAppTest2 : StkWebAppExec
{
	StkObject* Execute(StkObject*, int, TCHAR[StkWebAppExec::URL_PATH_LENGTH], int*, TCHAR[3]);
};
