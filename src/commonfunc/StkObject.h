#include <windows.h>
#include <string>
#pragma once

class StkObject
{
private:
	class Impl;
	Impl* pImpl;

public:
	static const int STKOBJECT_UNKNOWN = 0;
	static const int STKOBJECT_UNKW_INT = 1;
	static const int STKOBJECT_UNKW_FLOAT = 2;
	static const int STKOBJECT_UNKW_STRING = 3;
	static const int STKOBJECT_ATTRIBUTE = 10;
	static const int STKOBJECT_ATTR_INT = 11;
	static const int STKOBJECT_ATTR_FLOAT = 12;
	static const int STKOBJECT_ATTR_STRING = 13;
	static const int STKOBJECT_ELEMENT = 20;
	static const int STKOBJECT_ELEM_INT = 21;
	static const int STKOBJECT_ELEM_FLOAT = 22;
	static const int STKOBJECT_ELEM_STRING = 23;

	// XMS decoding error code
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

	// JSON decoding error code
	static const int ERROR_JSON_NO_ELEMENT_FOUND = -200;
	static const int ERROR_JSON_INVALID_QUOT_FOUND = -201;
	static const int ERROR_JSON_INVALID_COLON_FOUND = -202;
	static const int ERROR_JSON_INVALID_STRUCTURE = -203;
	static const int ERROR_JSON_INVALID_COMMA = -204;
	static const int ERROR_JSON_CANNOT_HANDLE = -205;
	static const int ERROR_JSON_INVALID_ARRAY_STRUCTURE = -206;
	static const int ERROR_JSON_NO_ROOT_ELEMENT = -207;

	StkObject(TCHAR*);
	StkObject(TCHAR*, int);
	StkObject(TCHAR*, float);
	StkObject(TCHAR*, TCHAR*);
	virtual ~StkObject();

	StkObject* Clone();
	BOOL Equals(StkObject*);
	StkObject* Contains(StkObject*);
	int GetArrayLength();
	int GetChildElementCount();
	int GetAttributeCount();

	TCHAR* GetName();
	int GetType();
	void SetType(int); // DO NOT USE THIS API.

	int GetIntValue();
	float GetFloatValue();
	TCHAR* GetStringValue();
	void SetIntValue(int);
	void SetFloatValue(float);
	void SetStringValue(TCHAR*);

	StkObject* GetFirstChildElement();
	StkObject* GetLastChildElement();
	StkObject* GetFirstAttribute();
	StkObject* GetLastAttribute();
	StkObject* GetNext();

	void AppendChildElement(StkObject*);
	void AppendAttribute(StkObject*);
	void SetNext(StkObject*); // DO NOT USE THIS API. USE Append* API INSTEAD.

	void ToXml(std::wstring*, int Indent = 0); // DO NOT SPECIFY 2ND PARAMETER.
	void ToXml(TCHAR*, int, int Indent = 0); // DO NOT SPECIFY 3RD PARAMETER.
	void ToJson(std::wstring*, int Indent = 0, BOOL ArrayFlag = FALSE); // DO NOT SPECIFY 2ND AND 3RD PARAMETERS.

	static int Analyze(TCHAR*);
	static StkObject* CreateObjectFromXml(TCHAR*, int*);
	static StkObject* CreateObjectFromJson(TCHAR*, int*, StkObject* Obj = NULL); // DO NOT SPECIFY 3RD PARAMETER.
};
