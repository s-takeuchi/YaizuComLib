#include <windows.h>
#include "StkObject.h"
#pragma once

class StkObjectUtil
{
private:
	class Impl;
	Impl* pImpl;

public:
	static const int ERROR_XML_ELEMENT_END_NOT_FOUND = -100;
	static const int ERROR_XML_EQUAL_FOUND_WITHOUT_ATTRIBUTE_NAME = -101;
	static const int ERROR_XML_INVALID_ELEMENT_START_FOUND = -102;
	static const int ERROR_XML_INVALID_QUOT_FOUND = -103;
	static const int ERROR_XML_INVALID_ELEMENT_END_FOUND = -104;
	static const int ERROR_XML_SLASH_FOUND_WITHOUT_ELEMENT_END = -105;
	static const int ERROR_XML_CANNOT_HANDLE = -106;
	static const int ERROR_XML_NO_ELEMENT_FOUND = -107;
	static const int ERROR_XML_INVALID_SLASH_FOUND = -108;
	static const int ERROR_XML_SLASH_FOUND_WITHOUT_ELEMENT = -109;

	static const int ERROR_JSON_NO_ELEMENT_FOUND = -200;
	static const int ERROR_JSON_INVALID_QUOT_FOUND = -201;
	static const int ERROR_JSON_INVALID_COLON_FOUND = -202;

	StkObjectUtil();
	virtual ~StkObjectUtil();

	StkObject* CreateObjectFromXml(TCHAR*, int*);
	StkObject* CreateObjectFromJson(TCHAR*, int*);
};
