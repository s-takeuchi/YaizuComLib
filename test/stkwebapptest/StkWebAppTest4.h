#pragma once
#include "..\..\src\stkwebapp\StkWebAppExec.h"

class StkWebAppTest4 : StkWebAppExec
{
	StkObject* Execute(StkObject*, int, TCHAR[StkWebAppExec::URL_PATH_LENGTH], int*, TCHAR[3]);
};
