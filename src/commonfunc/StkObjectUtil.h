#include <windows.h>
#include "StkObject.h"
#pragma once

class StkObjectUtil
{
private:
	TCHAR* StkObjectUtil::GetElementName(TCHAR*, int*);

public:
	StkObject* CreateObjectFromXml(TCHAR*, int*);
};
