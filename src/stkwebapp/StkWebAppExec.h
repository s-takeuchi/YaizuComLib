#pragma once
#include "..\commonfunc\stkobject.h"

class StkWebAppExec
{
public:
	virtual int Execute(StkObject*, StkObject*) = 0;
};
