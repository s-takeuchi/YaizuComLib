#pragma once
#include "..\commonfunc\stkobject.h"

class StkWebAppExec
{
public:
	virtual StkObject* Execute(StkObject*, int, TCHAR[128], int*) = 0;
};
