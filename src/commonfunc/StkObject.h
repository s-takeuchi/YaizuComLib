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

	StkObject(TCHAR*);
	StkObject(TCHAR*, int);
	StkObject(TCHAR*, float);
	StkObject(TCHAR*, TCHAR*);
	virtual ~StkObject();

	StkObject* Clone();
	int GetArrayLength();
	int GetChildElementCount();
	int GetAttributeCount();

	TCHAR* GetName();
	int GetType();
	void SetType(int);

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

	void ToXml(std::wstring* Msg, int Indent = 0); // DO NOT SPECIFY 2ND PARAMETER
	void ToJson(std::wstring* Msg, int Indent = 0, BOOL ArrayFlag = FALSE); // DO NOT SPECIFY 2ND AND 3RD PARAMETERS
};
