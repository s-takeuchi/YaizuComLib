#include <windows.h>
#include "StkObject.h"
#pragma once

class StkObjectUtil
{
private:
	TCHAR* GetName(TCHAR*, int*);
	TCHAR* GetValue(TCHAR*, int*);
	void CleanupObjects(TCHAR*, StkObject*);

public:
	static const int ERROR_ELEMENT_END_NOT_FOUND = -100;
	static const int ERROR_EQUAL_FOUND_WITHOUT_ATTRIBUTE_NAME = -101;
	static const int ERROR_INVALID_ELEMENT_START_FOUND = -102;
	static const int ERROR_INVALID_QUOT_FOUND = -103;
	static const int ERROR_INVALID_ELEMENT_END_FOUND = -104;
	static const int ERROR_SLASH_FOUND_WITHOUT_ELEMENT_END = -105;
	static const int ERROR_CANNOT_HANDLE = -106;
	static const int ERROR_NO_ELEMENT_FOUND = -107;

	StkObject* CreateObjectFromXml(TCHAR*, int*);
};
