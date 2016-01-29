#include <windows.h>
#pragma once

class StkObject
{
private:
	class Impl;
	Impl* pImpl;

public:
	static const int STKOBJECT_UNKNOWN = 0;
	static const int STKOBJECT_INT = 1;
	static const int STKOBJECT_FLOAT = 2;
	static const int STKOBJECT_STRING = 3;
	static const int STKOBJECT_ELEMENT = 10;

	StkObject(TCHAR*); // for element
	StkObject(TCHAR*, int); // for int attribute
	StkObject(TCHAR*, float); // for float attribute
	StkObject(TCHAR*, TCHAR*); // for string attribute
	virtual ~StkObject();

	StkObject* Clone();
	int GetArrayLength();

	TCHAR* GetName();
	int GetType();
	int GetIntValue();
	float GetFloatValue();
	TCHAR* GetStringValue();

	StkObject* GetFirstChildElement();
	StkObject* GetLastChildElement();
	StkObject* GetFirstAttribute();
	StkObject* GetLastAttribute();
	StkObject* GetNext();

	void AppendChildElement(StkObject*);
	void AppendAttribute(StkObject*);
	void SetNext(StkObject*); // DO NOT USE THIS API. USE Append* API INSTEAD.

	void ToXml(int);
};
