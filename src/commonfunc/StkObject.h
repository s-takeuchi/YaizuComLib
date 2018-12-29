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

	StkObject(wchar_t*);
	StkObject(wchar_t*, int);
	StkObject(wchar_t*, float);
	StkObject(wchar_t*, wchar_t*);
	virtual ~StkObject();

	StkObject* Clone();
	bool Equals(StkObject*);
	StkObject* Contains(StkObject*);
	int GetArrayLength();
	int GetChildElementCount();
	int GetAttributeCount();

	wchar_t* GetName();
	int GetType();
	void SetType(int); // DO NOT USE THIS API.

	int GetIntValue();
	float GetFloatValue();
	wchar_t* GetStringValue();
	void SetIntValue(int);
	void SetFloatValue(float);
	void SetStringValue(wchar_t*);

	StkObject* GetFirstChildElement();
	StkObject* GetLastChildElement();
	StkObject* GetFirstAttribute();
	StkObject* GetLastAttribute();
	StkObject* GetNext();

	void AppendChildElement(StkObject*);
	void AppendAttribute(StkObject*);
	void SetNext(StkObject*); // DO NOT USE THIS API. USE Append* API INSTEAD.

	void ToXml(wchar_t*, int, int Indent = 0); // DO NOT SPECIFY 3RD PARAMETER.
	int ToJson(wchar_t*, int, int Indent = 0, bool ArrayFlag = false); // DO NOT SPECIFY 3RD AND 4TH PARAMETERS.

	static int Analyze(wchar_t*);
	static StkObject* CreateObjectFromXml(wchar_t*, int*);
	static StkObject* CreateObjectFromJson(wchar_t*, int*, StkObject* Obj = 0); // DO NOT SPECIFY 3RD PARAMETER.
};
