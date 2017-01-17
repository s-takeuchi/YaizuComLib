#pragma once
#include "..\commonfunc\stkobject.h"

class StkWebAppExec
{
public:
	virtual StkObject* Execute(StkObject*) = 0;
};
