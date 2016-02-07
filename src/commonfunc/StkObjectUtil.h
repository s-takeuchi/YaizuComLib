#include <windows.h>
#include "StkObject.h"
#pragma once

class StkObjectUtil
{
private:
	TCHAR* GetName(TCHAR*, int*);
	TCHAR* GetValue(TCHAR*, int*);

public:
	StkObject* CreateObjectFromXml(TCHAR*, int*);
};
