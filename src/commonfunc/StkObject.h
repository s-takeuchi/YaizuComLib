#include <windows.h>
#include <string>
#pragma once

class StkObject
{
private:
	class Impl;
	Impl* pImpl;

public:
	static const int STKOBJECT_ATTRIBUTE = 0;
	static const int STKOBJECT_ATTR_INT = 1;
	static const int STKOBJECT_ATTR_FLOAT = 2;
	static const int STKOBJECT_ATTR_STRING = 3;
	static const int STKOBJECT_ELEMENT = 10;
	static const int STKOBJECT_ELEM_INT = 11;
	static const int STKOBJECT_ELEM_FLOAT = 12;
	static const int STKOBJECT_ELEM_STRING = 13;

	StkObject(TCHAR*);
	StkObject(int, TCHAR*, int);
	StkObject(int, TCHAR*, float);
	StkObject(int, TCHAR*, TCHAR*);
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

	void ToXml(std::wstring* Msg, int Indent = 0);
};
