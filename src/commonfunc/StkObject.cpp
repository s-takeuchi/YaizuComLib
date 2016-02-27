#include <windows.h>
#include <string>
#include <tchar.h>
#include <shlwapi.h>
#include "StkObject.h"

class StkObject::Impl
{
public:
	TCHAR* Name;
	int Type;
	void* Value;

	StkObject* FirstAttr;
	StkObject* LastAttr;
	StkObject* FirstElem;
	StkObject* LastElem;
	StkObject* Next;

public:
	void ClearMember();
	int XmlEncodeSize(TCHAR*);
	void XmlEncode(TCHAR*, TCHAR*, int);
	int JsonEncodeSize(TCHAR*);
	void JsonEncode(TCHAR*, TCHAR*, int);

	BOOL IsArrayExpression(StkObject*);

	static TCHAR* GetJsonString(TCHAR*, int*);
	static BOOL GetJsonNumber(TCHAR*, int*, int*, float*);
	static TCHAR* GetName(TCHAR*, int*);
	static TCHAR* GetValue(TCHAR*, int*);
	static void CleanupObjectsForXml(TCHAR*, StkObject*);
	static void CleanupObjectsForJson(TCHAR*, StkObject*);
};

void StkObject::Impl::ClearMember()
{
	Name = NULL;
	Value = NULL;
	FirstAttr = NULL;
	LastAttr = NULL;
	FirstElem = NULL;
	LastElem = NULL;
	Next = NULL;
}

// Returns word size of escaped message.
// InMsg [in] : Original string
// Return : Word size of escaped message.
int StkObject::Impl::XmlEncodeSize(TCHAR* InMsg)
{
	int Size = 0;
	int InMsgLen = lstrlen(InMsg);
	for (int Loop = 0; Loop < InMsgLen; Loop++) {
		if (InMsg[Loop] == '<') Size += 4;
		else if (InMsg[Loop] == '>') Size += 4;
		else if (InMsg[Loop] == '&') Size += 5;
		else if (InMsg[Loop] == '\"') Size += 6;
		else if (InMsg[Loop] == '\'') Size += 6;
		else Size ++;
	}
	return Size;
}

// Returns word size of escaped message.
// InMsg [in] : Original string
// Return : Word size of escaped message.
int StkObject::Impl::JsonEncodeSize(TCHAR* InMsg)
{
	int Size = 0;
	int InMsgLen = lstrlen(InMsg);
	for (int Loop = 0; Loop < InMsgLen; Loop++) {
		if (InMsg[Loop] == '\"' || InMsg[Loop] == '\\' || InMsg[Loop] == '/' ||
			InMsg[Loop] == '\b' || InMsg[Loop] == '\f' || InMsg[Loop] == '\n' ||
			InMsg[Loop] == '\r' || InMsg[Loop] == '\t') {
				Size += 2;
		} else {
			Size ++;
		}
	}
	return Size;
}

// Convert the string to escaped message.
// InMsg [in] : Zero terminate string
// OutMsg [out] : Converted string.
// SizeOfOutMsg [in] : Length of OutData
void StkObject::Impl::XmlEncode(TCHAR* InMsg, TCHAR* OutMsg, int SizeOfOutMsg)
{
	int OutMsgIndex = 0;
	int InMsgLen = lstrlen(InMsg);
	for (int Loop = 0; Loop < InMsgLen; Loop++) {
		if (InMsg[Loop] == '<') {
			if (OutMsgIndex + 4 >= SizeOfOutMsg)
				break;
			lstrcpy(&OutMsg[OutMsgIndex], _T("&lt;"));
			OutMsgIndex += 4;
			continue;
		}
		if (InMsg[Loop] == '>') {
			if (OutMsgIndex + 4 >= SizeOfOutMsg)
				break;
			lstrcpy(&OutMsg[OutMsgIndex], _T("&gt;"));
			OutMsgIndex += 4;
			continue;
		}
		if (InMsg[Loop] == '&') {
			if (OutMsgIndex + 5 >= SizeOfOutMsg)
				break;
			lstrcpy(&OutMsg[OutMsgIndex], _T("&amp;"));
			OutMsgIndex += 5;
			continue;
		}
		if (InMsg[Loop] == '\"') {
			if (OutMsgIndex + 6 >= SizeOfOutMsg)
				break;
			lstrcpy(&OutMsg[OutMsgIndex], _T("&quot;"));
			OutMsgIndex += 6;
			continue;
		}
		if (InMsg[Loop] == '\'') {
			if (OutMsgIndex + 6 >= SizeOfOutMsg)
				break;
			lstrcpy(&OutMsg[OutMsgIndex], _T("&apos;"));
			OutMsgIndex += 6;
			continue;
		}
		if (OutMsgIndex + 1 >= SizeOfOutMsg)
			break;
		OutMsg[OutMsgIndex] = InMsg[Loop];
		OutMsgIndex++;
	}
	OutMsg[OutMsgIndex] = '\0';
}

// Convert the string to escaped message.
// InData [in] : Zero terminate string
// OutData [out] : Converted string.
// OutDataLen [in] : Length of OutData
void StkObject::Impl::JsonEncode(TCHAR* InData, TCHAR* OutData, int OutDataLen)
{
	TCHAR* CurInPtr = InData;
	TCHAR* CurOutPtr = OutData;
	for (;;) {
		if (*CurInPtr == '\0') {
			*CurOutPtr = '\0';
			return;
		} else if (*CurInPtr == '\"' || *CurInPtr == '\\' || *CurInPtr == '/' ||
			*CurInPtr == '\b' || *CurInPtr == '\f' || *CurInPtr == '\n' ||
			*CurInPtr == '\r' || *CurInPtr == '\t') {
			*CurOutPtr = '\\';
			CurOutPtr++;
			switch (*CurInPtr) {
			case '\"': *CurOutPtr = '\"'; break;
			case '\\': *CurOutPtr = '\\'; break;
			case '/': *CurOutPtr = '/';  break;
			case '\b': *CurOutPtr = 'b';  break;
			case '\f': *CurOutPtr = 'f';  break;
			case '\n': *CurOutPtr = 'n';  break;
			case '\r': *CurOutPtr = 'r';  break;
			case '\t': *CurOutPtr = 't';  break;
			}
		} else {
			*CurOutPtr = *CurInPtr;
		}
		CurOutPtr++;
		CurInPtr++;
		if (CurOutPtr - OutData == OutDataLen) {
			*CurOutPtr = '\0';
			return;
		}
	}
}

BOOL StkObject::Impl::IsArrayExpression(StkObject* Obj)
{
	if (!(Obj = Obj->GetFirstChildElement())) {
		return FALSE;
	}
	if (!Obj->GetNext()) {
		return FALSE;
	}
	TCHAR* BsName = Obj->GetName();
	while (Obj = Obj->GetNext()) {
		TCHAR* CurName = Obj->GetName();
		if (lstrcmp(BsName, CurName) != 0) {
			return FALSE;
		}
	}
	return TRUE;
}


void StkObject::Impl::CleanupObjectsForXml(TCHAR* PrevAttrName, StkObject* RetObj)
{
	if (PrevAttrName != NULL) {
		delete PrevAttrName;
	}
	if (RetObj != NULL) {
		delete RetObj;
	}
}

void StkObject::Impl::CleanupObjectsForJson(TCHAR* PrevName, StkObject* RetObj)
{
	if (PrevName != NULL) {
		delete PrevName;
	}
	if (RetObj != NULL) {
		delete RetObj;
	}
}

BOOL StkObject::Impl::GetJsonNumber(TCHAR* OrgStr, int* Len, int* ValInt, float* ValFloat)
{
	TCHAR* CurPnt = OrgStr;
	int ValueLength = 0;
	BOOL FloatFlag = FALSE;

	while (*CurPnt != TCHAR('\0')) {
		if (*CurPnt >= TCHAR('0') && *CurPnt <= TCHAR('9')) {
			ValueLength++;
			CurPnt++;
		} else if (*CurPnt == TCHAR('.')) {
			ValueLength++;
			CurPnt++;
			FloatFlag = TRUE;
		} else {
			break;
		}
	}
	TCHAR* RtnValue = new TCHAR[ValueLength + 1];
	*Len = ValueLength;
	int Loop = 0;
	for (; Loop < ValueLength; Loop++) {
		RtnValue[Loop] = OrgStr[Loop];
	}
	RtnValue[Loop] = TCHAR('\0');

	if (FloatFlag == TRUE) {
		*ValFloat = (float)_wtof(RtnValue);
	} else {
		*ValInt = _wtoi(RtnValue);
	}
	delete RtnValue;
	return FloatFlag;
}

TCHAR* StkObject::Impl::GetJsonString(TCHAR* OrgStr, int* Len)
{
	int OutLength = 0;
	TCHAR* CurPnt = OrgStr;
	while (*CurPnt != TCHAR('\"') && *CurPnt != _T('\0')) {
		if (*CurPnt == TCHAR('\\')) {
			if (*(CurPnt + 1) == TCHAR('\"') || *(CurPnt + 1) == TCHAR('\\') || *(CurPnt + 1) == TCHAR('/') || *(CurPnt + 1) == TCHAR('b') ||
				*(CurPnt + 1) == TCHAR('f') || *(CurPnt + 1) == TCHAR('n') || *(CurPnt + 1) == TCHAR('r') || *(CurPnt + 1) == TCHAR('t')) {
				OutLength++;
				CurPnt += 2;
			} else {
				OutLength++;
				CurPnt++;
			}
		} else {
			OutLength++;
			CurPnt++;
		}
	}
	if (*CurPnt == TCHAR('\"')) {
		*Len = CurPnt - OrgStr;
	} else if (*CurPnt == TCHAR('\0')) {
		*Len = CurPnt - OrgStr - 1;
	}

	TCHAR* RtnValue = new TCHAR[OutLength + 1];
	int RtnLoop = 0;
	for (TCHAR* Loop = OrgStr; Loop < CurPnt; Loop++) {
		if (StrStr(Loop, _T("\\\"")) == Loop) {
			RtnValue[RtnLoop] = '\"';
			RtnLoop++;
			Loop++;
			continue;
		}
		if (StrStr(Loop, _T("\\\\")) == Loop) {
			RtnValue[RtnLoop] = '\\';
			RtnLoop++;
			Loop++;
			continue;
		}
		if (StrStr(Loop, _T("\\/")) == Loop) {
			RtnValue[RtnLoop] = '/';
			RtnLoop++;
			Loop++;
			continue;
		}
		if (StrStr(Loop, _T("\\b")) == Loop) {
			RtnValue[RtnLoop] = '\b';
			RtnLoop++;
			Loop++;
			continue;
		}
		if (StrStr(Loop, _T("\\f")) == Loop) {
			RtnValue[RtnLoop] = '\f';
			RtnLoop++;
			Loop++;
			continue;
		}
		if (StrStr(Loop, _T("\\n")) == Loop) {
			RtnValue[RtnLoop] = '\n';
			RtnLoop++;
			Loop++;
			continue;
		}
		if (StrStr(Loop, _T("\\r")) == Loop) {
			RtnValue[RtnLoop] = '\r';
			RtnLoop++;
			Loop++;
			continue;
		}
		if (StrStr(Loop, _T("\\t")) == Loop) {
			RtnValue[RtnLoop] = '\t';
			RtnLoop++;
			Loop++;
			continue;
		}
		RtnValue[RtnLoop] = *Loop;
		RtnLoop++;
	}
	RtnValue[OutLength] = TCHAR('\0');
	return RtnValue;
}

TCHAR* StkObject::Impl::GetName(TCHAR* TgtName, int* Len)
{
	TCHAR* CurPnt = TgtName;
	int NameLength = 0;
	while (*CurPnt != TCHAR(' ') && *CurPnt != TCHAR('\t') && *CurPnt != TCHAR('\r') && *CurPnt != TCHAR('\n') && *CurPnt != TCHAR('>') && *CurPnt != TCHAR('/') && *CurPnt != TCHAR('=') && *CurPnt != TCHAR('\0')) {
		NameLength++;
		CurPnt++;
	}
	*Len = NameLength;
	TCHAR* RtnName = new TCHAR[NameLength + 1];
	for (int Loop = 0; Loop < NameLength; Loop++) {
		RtnName[Loop] = TgtName[Loop];
	}
	RtnName[NameLength] = TCHAR('\0');
	return RtnName;
}

TCHAR* StkObject::Impl::GetValue(TCHAR* TgtValue, int* Len)
{
	TCHAR* CurPnt = TgtValue;
	int ValueLength = 0;
	while (*CurPnt != TCHAR('\"') && *CurPnt != TCHAR('<') && *CurPnt != TCHAR('\0')) {
		if (*CurPnt == TCHAR('&')) {
			if (*(CurPnt + 1) != TCHAR('\0') && *(CurPnt + 2) != TCHAR('\0') && *(CurPnt + 3) != TCHAR('\0') && StrStr(CurPnt, _T("&lt;")) == CurPnt) {
				CurPnt += 4;
			} else if (*(CurPnt + 1) != TCHAR('\0') && *(CurPnt + 2) != TCHAR('\0') && *(CurPnt + 3) != TCHAR('\0') && StrStr(CurPnt, _T("&gt;")) == CurPnt) {
				CurPnt += 4;
			} else if (*(CurPnt + 1) != TCHAR('\0') && *(CurPnt + 2) != TCHAR('\0') && *(CurPnt + 3) != TCHAR('\0') && *(CurPnt + 4) != TCHAR('\0') && StrStr(CurPnt, _T("&amp;")) == CurPnt) {
				CurPnt += 5;
			} else if (*(CurPnt + 1) != TCHAR('\0') && *(CurPnt + 2) != TCHAR('\0') && *(CurPnt + 3) != TCHAR('\0') && *(CurPnt + 4) != TCHAR('\0') && *(CurPnt + 5) != TCHAR('\0') && StrStr(CurPnt, _T("&quot;")) == CurPnt) {
				CurPnt += 6;
			} else if (*(CurPnt + 1) != TCHAR('\0') && *(CurPnt + 2) != TCHAR('\0') && *(CurPnt + 3) != TCHAR('\0') && *(CurPnt + 4) != TCHAR('\0') && *(CurPnt + 5) != TCHAR('\0') && StrStr(CurPnt, _T("&apos;")) == CurPnt) {
				CurPnt += 6;
			} else {
				CurPnt++;
				continue;
			}
		} else {
			CurPnt++;
		}
		ValueLength++;
	}
	if (*CurPnt == TCHAR('\"')) {
		*Len = CurPnt - TgtValue;
	} else if (*CurPnt == TCHAR('<') || *CurPnt == TCHAR('\0')) {
		*Len = CurPnt - TgtValue - 1;
	}
	TCHAR* RtnValue = new TCHAR[ValueLength + 1];
	int RtnLoop = 0;
	for (TCHAR* Loop = TgtValue; Loop < CurPnt; Loop++) {
		if (StrStr(Loop, _T("&lt;")) == Loop) {
			RtnValue[RtnLoop] = '<';
			RtnLoop++;
			Loop += 3;
			continue;
		}
		if (StrStr(Loop, _T("&gt;")) == Loop) {
			RtnValue[RtnLoop] = '>';
			RtnLoop++;
			Loop += 3;
			continue;
		}
		if (StrStr(Loop, _T("&amp;")) == Loop) {
			RtnValue[RtnLoop] = '&';
			RtnLoop++;
			Loop += 4;
			continue;
		}
		if (StrStr(Loop, _T("&quot;")) == Loop) {
			RtnValue[RtnLoop] = '\"';
			RtnLoop++;
			Loop += 5;
			continue;
		}
		if (StrStr(Loop, _T("&apos;")) == Loop) {
			RtnValue[RtnLoop] = '\'';
			RtnLoop++;
			Loop += 5;
			continue;
		}
		RtnValue[RtnLoop] = *Loop;
		RtnLoop++;
	}
	RtnValue[ValueLength] = TCHAR('\0');
	return RtnValue;
}






StkObject::StkObject(TCHAR* TmpName)
{
	pImpl = new Impl;
	pImpl->ClearMember();
	pImpl->Type = STKOBJECT_ELEMENT;
	if (TmpName != NULL) {
		int Len = lstrlen(TmpName) + 1;
		pImpl->Name = new TCHAR[Len];
		lstrcpy(pImpl->Name, TmpName);
	}
}

StkObject::StkObject(TCHAR* TmpName, int TmpValue)
{
	pImpl = new Impl;
	pImpl->ClearMember();
	pImpl->Type = STKOBJECT_UNKW_INT;
	if (TmpName != NULL) {
		int Len = lstrlen(TmpName) + 1;
		pImpl->Name = new TCHAR[Len];
		lstrcpy(pImpl->Name, TmpName);
	}
	pImpl->Value = new int(TmpValue);
}

StkObject::StkObject(TCHAR* TmpName, float TmpValue)
{
	pImpl = new Impl;
	pImpl->ClearMember();
	pImpl->Type = STKOBJECT_UNKW_FLOAT;
	if (TmpName != NULL) {
		int Len = lstrlen(TmpName) + 1;
		pImpl->Name = new TCHAR[Len];
		lstrcpy(pImpl->Name, TmpName);
	}
	pImpl->Value = new float(TmpValue);
}

StkObject::StkObject(TCHAR* TmpName, TCHAR* TmpValue)
{
	pImpl = new Impl;
	pImpl->ClearMember();
	pImpl->Type = STKOBJECT_UNKW_STRING;
	if (TmpName != NULL) {
		int Len = lstrlen(TmpName) + 1;
		pImpl->Name = new TCHAR[Len];
		lstrcpy(pImpl->Name, TmpName);
	}
	if (TmpValue != NULL) {
		int Len = lstrlen(TmpValue) + 1;
		pImpl->Value = new TCHAR[Len];
		lstrcpy((TCHAR*)pImpl->Value, TmpValue);
	}
}

StkObject::~StkObject()
{
	if (pImpl->Next != NULL) {
		delete pImpl->Next;
	}
	if (pImpl->FirstAttr != NULL) {
		delete pImpl->FirstAttr;
	}
	if (pImpl->FirstElem != NULL) {
		delete pImpl->FirstElem;
	}
	if (pImpl->Name != NULL) {
		delete pImpl->Name;
	}
	if (pImpl->Value != NULL) {
		delete pImpl->Value;
	}
	delete pImpl;
}

StkObject* StkObject::Clone()
{
	StkObject* NewObj;
	TCHAR* TmpName = GetName();
	if (pImpl->Type == STKOBJECT_ATTR_INT) {
		NewObj = new StkObject(TmpName, GetIntValue());
	} else if (pImpl->Type == STKOBJECT_ATTR_FLOAT) {
		NewObj = new StkObject(TmpName, GetFloatValue());
	} else if (pImpl->Type == STKOBJECT_ATTR_STRING) {
		NewObj = new StkObject(TmpName, GetStringValue());
	} else if (pImpl->Type == STKOBJECT_ELEM_INT) {
		NewObj = new StkObject(TmpName, GetIntValue());
	} else if (pImpl->Type == STKOBJECT_ELEM_FLOAT) {
		NewObj = new StkObject(TmpName, GetFloatValue());
	} else if (pImpl->Type == STKOBJECT_ELEM_STRING) {
		NewObj = new StkObject(TmpName, GetStringValue());
	} else if (pImpl->Type == STKOBJECT_ELEMENT) {
		NewObj = new StkObject(TmpName);
		StkObject* TmpAttr = GetFirstAttribute();
		if (TmpAttr != NULL) {
			NewObj->AppendAttribute(TmpAttr->Clone());
			StkObject* CurAttr = TmpAttr->GetNext();
			while (CurAttr != NULL) {
				NewObj->AppendAttribute(CurAttr->Clone());
				CurAttr = CurAttr->GetNext();
			}
		}
		StkObject* TmpElem = GetFirstChildElement();
		if (TmpElem != NULL) {
			NewObj->AppendChildElement(TmpElem->Clone());
			StkObject* CurElem = TmpElem->GetNext();
			while (CurElem != NULL) {
				NewObj->AppendChildElement(CurElem->Clone());
				CurElem = CurElem->GetNext();
			}
		}
	}
	return NewObj;
}

int StkObject::GetArrayLength()
{
	StkObject* NextObj = pImpl->Next;
	if (NextObj != NULL) {
		return 1 + NextObj->GetArrayLength();
	}
	return 1;
}

int StkObject::GetChildElementCount()
{
	StkObject* CldObj = pImpl->FirstElem;
	if (CldObj != NULL) {
		return CldObj->GetArrayLength();
	}
	return 0;
}

int StkObject::GetAttributeCount()
{
	StkObject* AtrObj = pImpl->FirstAttr;
	if (AtrObj != NULL) {
		return AtrObj->GetArrayLength();
	}
	return 0;
}

void StkObject::SetIntValue(int TmpValue)
{
	if (pImpl->Value != NULL) {
		delete pImpl->Value;
	}
	pImpl->Value = new int(TmpValue);
}

void StkObject::SetFloatValue(float TmpValue)
{
	if (pImpl->Value != NULL) {
		delete pImpl->Value;
	}
	pImpl->Value = new float(TmpValue);
}

void StkObject::SetStringValue(TCHAR* TmpValue)
{
	if (pImpl->Value != NULL) {
		delete pImpl->Value;
	}
	int Len = lstrlen(TmpValue) + 1;
	pImpl->Value = new TCHAR[Len];
	lstrcpy((TCHAR*)pImpl->Value, TmpValue);
}

TCHAR* StkObject::GetName()
{
	return pImpl->Name;
}

int StkObject::GetType()
{
	return pImpl->Type;
}

void StkObject::SetType(int TmpType)
{
	pImpl->Type = TmpType;
}

int StkObject::GetIntValue()
{
	int *Val = (int*)pImpl->Value;
	return *Val;
}

float StkObject::GetFloatValue()
{
	float *Val = (float*)pImpl->Value;
	return *Val;
}

TCHAR* StkObject::GetStringValue()
{
	return (TCHAR*)pImpl->Value;
}

StkObject* StkObject::GetFirstChildElement()
{
	return pImpl->FirstElem;
}

StkObject* StkObject::GetLastChildElement()
{
	return pImpl->LastElem;
}

StkObject* StkObject::GetFirstAttribute()
{
	return pImpl->FirstAttr;
}

StkObject* StkObject::GetLastAttribute()
{
	return pImpl->LastAttr;
}

StkObject* StkObject::GetNext()
{
	return pImpl->Next;
}

void StkObject::AppendChildElement(StkObject* TmpObj)
{
	int TmpType = TmpObj->GetType();
	if (TmpType == STKOBJECT_UNKW_INT) {
		TmpObj->SetType(STKOBJECT_ELEM_INT);
	} else if (TmpType == STKOBJECT_UNKW_FLOAT) {
		TmpObj->SetType(STKOBJECT_ELEM_FLOAT);
	} else if (TmpType == STKOBJECT_UNKW_STRING) {
		TmpObj->SetType(STKOBJECT_ELEM_STRING);
	}
	if (pImpl->FirstElem == NULL) {
		pImpl->FirstElem = TmpObj;
		pImpl->LastElem = TmpObj;
	} else {
		StkObject* CurElem = pImpl->LastElem;
		CurElem->SetNext(TmpObj);
		pImpl->LastElem = TmpObj;
	}
}

void StkObject::AppendAttribute(StkObject* TmpObj)
{
	int TmpType = TmpObj->GetType();
	if (TmpType == STKOBJECT_UNKW_INT) {
		TmpObj->SetType(STKOBJECT_ATTR_INT);
	} else if (TmpType == STKOBJECT_UNKW_FLOAT) {
		TmpObj->SetType(STKOBJECT_ATTR_FLOAT);
	} else if (TmpType == STKOBJECT_UNKW_STRING) {
		TmpObj->SetType(STKOBJECT_ATTR_STRING);
	}
	if (pImpl->FirstAttr == NULL) {
		pImpl->FirstAttr = TmpObj;
		pImpl->LastAttr = TmpObj;
	} else {
		StkObject* CurAttr = pImpl->LastAttr;
		CurAttr->SetNext(TmpObj);
		pImpl->LastAttr = TmpObj;
	}
}

void StkObject::SetNext(StkObject* TmpObj)
{
	if (pImpl->Next == NULL) {
		pImpl->Next = TmpObj;
	}
}

void StkObject::ToXml(std::wstring* Msg, int Indent)
{
	if (pImpl->Name == NULL) {
		return;
	}
	if (pImpl->Type == STKOBJECT_ATTR_INT || pImpl->Type == STKOBJECT_ATTR_FLOAT || pImpl->Type == STKOBJECT_ATTR_STRING) {
		if (pImpl->Type == STKOBJECT_ATTR_INT) {
			int *Val = (int*)pImpl->Value;
			TCHAR TmpBuf[15];
			_snwprintf_s(TmpBuf, 15, _TRUNCATE, _T("%d"), *Val);
			*Msg = *Msg + _T(" ") + pImpl->Name + _T("=\"") + TmpBuf + _T("\"");
		} else if (pImpl->Type == STKOBJECT_ATTR_FLOAT) {
			float *Val = (float*)pImpl->Value;
			TCHAR TmpBuf[25];
			_snwprintf_s(TmpBuf, 25, _TRUNCATE, _T("%f"), *Val);
			*Msg = *Msg + _T(" ") + pImpl->Name + _T("=\"") + TmpBuf + _T("\"");
		} else if (pImpl->Type == STKOBJECT_ATTR_STRING) {
			int StrLen = pImpl->XmlEncodeSize((TCHAR*)pImpl->Value);
			TCHAR* TmpStr = new TCHAR[StrLen + 1];
			pImpl->XmlEncode((TCHAR*)pImpl->Value, TmpStr, StrLen + 1);
			*Msg = *Msg + _T(" ") + pImpl->Name + _T("=\"") + TmpStr + _T("\"");
			delete TmpStr;
		}
		if (pImpl->Next != NULL) {
			StkObject* TmpObj = pImpl->Next;
			TmpObj->ToXml(Msg, Indent);
		}
	} else if (pImpl->Type == STKOBJECT_ELEMENT) {
		for (int Loop = 0; Loop < Indent; Loop++) {
			*Msg += _T(" ");
		}
		*Msg = *Msg + _T("<") + pImpl->Name;
		if (pImpl->FirstAttr != NULL) {
			StkObject* TmpObj = pImpl->FirstAttr;
			TmpObj->ToXml(Msg, Indent);
		}
		if (pImpl->FirstElem != NULL) {
			*Msg += _T(">\r\n");
			StkObject* TmpObj = pImpl->FirstElem;
			TmpObj->ToXml(Msg, Indent + 2);
			for (int Loop = 0; Loop < Indent; Loop++) {
				*Msg += _T(" ");
			}
			*Msg = *Msg + _T("</") + pImpl->Name + _T(">\r\n");
		} else {
			*Msg += _T("/>\r\n");
		}
		if (pImpl->Next != NULL) {
			StkObject* TmpObj = pImpl->Next;
			TmpObj->ToXml(Msg, Indent);
		}
	} else if (pImpl->Type == STKOBJECT_ELEM_INT || pImpl->Type == STKOBJECT_ELEM_FLOAT || pImpl->Type == STKOBJECT_ELEM_STRING) {
		for (int Loop = 0; Loop < Indent; Loop++) {
			*Msg += _T(" ");
		}
		if (pImpl->Type == STKOBJECT_ELEM_INT) {
			int *Val = (int*)pImpl->Value;
			TCHAR TmpBuf[15];
			_snwprintf_s(TmpBuf, 15, _TRUNCATE, _T("%d"), *Val);
			*Msg = *Msg + _T("<") + pImpl->Name + _T(">") + TmpBuf + _T("</") + pImpl->Name + _T(">\r\n");
		} else if (pImpl->Type == STKOBJECT_ELEM_FLOAT) {
			float *Val = (float*)pImpl->Value;
			TCHAR TmpBuf[25];
			_snwprintf_s(TmpBuf, 25, _TRUNCATE, _T("%f"), *Val);
			*Msg = *Msg + _T("<") + pImpl->Name + _T(">") + TmpBuf + _T("</") + pImpl->Name + _T(">\r\n");
		} else if (pImpl->Type == STKOBJECT_ELEM_STRING) {
			int StrLen = pImpl->XmlEncodeSize((TCHAR*)pImpl->Value);
			TCHAR* TmpStr = new TCHAR[StrLen + 1];
			pImpl->XmlEncode((TCHAR*)pImpl->Value, TmpStr, StrLen + 1);
			*Msg = *Msg + _T("<") + pImpl->Name + _T(">") + TmpStr + _T("</") + pImpl->Name + _T(">\r\n");
			delete TmpStr;
		}
		if (pImpl->Next != NULL) {
			StkObject* TmpObj = pImpl->Next;
			TmpObj->ToXml(Msg, Indent);
		}
	}
}

void StkObject::ToJson(std::wstring* Msg, int Indent, BOOL ArrayFlag)
{
	if (pImpl->Name == NULL) {
		return;
	}
	if (pImpl->Type == STKOBJECT_ATTR_INT || pImpl->Type == STKOBJECT_ATTR_FLOAT || pImpl->Type == STKOBJECT_ATTR_STRING) {
		if (pImpl->Type == STKOBJECT_ATTR_INT) {
			int *Val = (int*)pImpl->Value;
			TCHAR TmpBuf[15];
			_snwprintf_s(TmpBuf, 15, _TRUNCATE, _T("%d"), *Val);
			*Msg = *Msg + _T("\"") + pImpl->Name + _T("\" : ") + TmpBuf;
		} else if (pImpl->Type == STKOBJECT_ATTR_FLOAT) {
			float *Val = (float*)pImpl->Value;
			TCHAR TmpBuf[25];
			_snwprintf_s(TmpBuf, 25, _TRUNCATE, _T("%f"), *Val);
			*Msg = *Msg + _T("\"") + pImpl->Name + _T("\" : ") + TmpBuf;
		} else if (pImpl->Type == STKOBJECT_ATTR_STRING) {
			int StrLen = pImpl->JsonEncodeSize((TCHAR*)pImpl->Value);
			TCHAR* TmpStr = new TCHAR[StrLen + 1];
			pImpl->JsonEncode((TCHAR*)pImpl->Value, TmpStr, StrLen + 1);
			*Msg = *Msg + _T("\"") + pImpl->Name + _T("\" : \"") + TmpStr + _T("\"");
			delete TmpStr;
		}
		if (pImpl->Next != NULL) {
			*Msg = *Msg + _T(", ");
			StkObject* TmpObj = pImpl->Next;
			TmpObj->ToJson(Msg, Indent);
		} else {
			*Msg = *Msg + _T("\r\n");
		}
		return;
	} else if (pImpl->Type == STKOBJECT_ELEM_INT || pImpl->Type == STKOBJECT_ELEM_FLOAT || pImpl->Type == STKOBJECT_ELEM_STRING) {
		for (int Loop = 0; Loop < Indent; Loop++) {
			*Msg += _T(" ");
		}
		if (pImpl->Type == STKOBJECT_ELEM_INT) {
			int *Val = (int*)pImpl->Value;
			TCHAR TmpBuf[15];
			_snwprintf_s(TmpBuf, 15, _TRUNCATE, _T("%d"), *Val);
			if (ArrayFlag == FALSE) {
				*Msg = *Msg + _T("\"") + pImpl->Name + _T("\" : ") + TmpBuf;
			} else {
				*Msg = *Msg + TmpBuf;
			}
		} else if (pImpl->Type == STKOBJECT_ELEM_FLOAT) {
			float *Val = (float*)pImpl->Value;
			TCHAR TmpBuf[25];
			_snwprintf_s(TmpBuf, 25, _TRUNCATE, _T("%f"), *Val);
			if (ArrayFlag == FALSE) {
				*Msg = *Msg + _T("\"") + pImpl->Name + _T("\" : ") + TmpBuf;
			} else {
				*Msg = *Msg + TmpBuf;
			}
		} else if (pImpl->Type == STKOBJECT_ELEM_STRING) {
			int StrLen = pImpl->JsonEncodeSize((TCHAR*)pImpl->Value);
			TCHAR* TmpStr = new TCHAR[StrLen + 1];
			pImpl->JsonEncode((TCHAR*)pImpl->Value, TmpStr, StrLen + 1);
			if (ArrayFlag == FALSE) {
				*Msg = *Msg + _T("\"") + pImpl->Name + _T("\" : \"") + TmpStr + _T("\"");
			} else {
				*Msg = *Msg + _T("\"") + TmpStr + _T("\"");
			}
			delete TmpStr;
		}
		return;
	}

	if (pImpl->Type == STKOBJECT_ELEMENT) {
		// For this element
		for (int Loop = 0; Loop < Indent; Loop++) {
			*Msg += _T(" ");
		}
		if (ArrayFlag == FALSE) {
			if (pImpl->Type == STKOBJECT_ELEMENT || pImpl->FirstAttr != NULL) {
				*Msg = *Msg + _T("\"") + pImpl->Name + _T("\" : {\r\n");
			}
		} else {
			if (pImpl->Type == STKOBJECT_ELEMENT || pImpl->FirstAttr != NULL) {
				*Msg = *Msg + _T("{\r\n");
			}
		}

		// For attribute
		if (pImpl->FirstAttr != NULL) {
			for (int Loop = 0; Loop < Indent + 2; Loop++) {
				*Msg += _T(" ");
			}
			*Msg = *Msg + _T("\"@attributes\" : {\r\n");
			for (int Loop = 0; Loop < Indent + 4; Loop++) {
				*Msg += _T(" ");
			}
			StkObject* TmpObj = pImpl->FirstAttr;
			TmpObj->ToJson(Msg, Indent);
			for (int Loop = 0; Loop < Indent + 2; Loop++) {
				*Msg += _T(" ");
			}
			if (pImpl->FirstElem == NULL) {
				*Msg = *Msg + _T("}\r\n");
			} else {
				*Msg = *Msg + _T("},\r\n");
			}
		}

		if (pImpl->IsArrayExpression(this)) {
			for (int Loop = 0; Loop < Indent + 2; Loop++) {
				*Msg += _T(" ");
			}
			StkObject* TmpObj = GetFirstChildElement();
			*Msg = *Msg + _T("\"") + TmpObj->GetName() + _T("\" : [\r\n");
			while (TmpObj) {
				TmpObj->ToJson(Msg, Indent + 4, TRUE);
				if (TmpObj->GetNext()) {
					*Msg += _T(",\r\n");
				} else {
					*Msg += _T("\r\n");
				}
				TmpObj = TmpObj->GetNext();
			}
			for (int Loop = 0; Loop < Indent + 2; Loop++) {
				*Msg += _T(" ");
			}
			*Msg = *Msg + _T("]\r\n");
		} else {
			StkObject* TmpObj = GetFirstChildElement();
			while (TmpObj) {
				TmpObj->ToJson(Msg, Indent + 2);
				if (TmpObj->GetNext()) {
					*Msg += _T(",\r\n");
				} else {
					*Msg += _T("\r\n");
				}
				TmpObj = TmpObj->GetNext();
			}
		}

		for (int Loop = 0; Loop < Indent; Loop++) {
			*Msg += _T(" ");
		}
		*Msg = *Msg + _T("}");
		return;
	}
}

//   " Aaa" : { " Bbb" : " Xxx" , " Ccc" : 123 , " Ddd" : [ 345, 678 ] }
//  | |    | | | |    | | |    | | |    | |   | |        | |          | |
//  | |    | | | |    | | |    | | |    | |   | |        | |          | ELEMOBJ_END
//  | |    | | | |    | | |    | | |    | |   | |        | |          ARRAY_END
//  | |    | | | |    | | |    | | |    | |   | |        | ARRAY_START
//  | |    | | | |    | | |    | | |    | |   | |        ELEMOBJ_START
//  | |    | | | |    | | |    | | |    | |   | ELEM_START
//  | |    | | | |    | | |    | | |    | |   ELEMOBJ_END
//  | |    | | | |    | | |    | | |    | ELEMOBJ_START
//  | |    | | | |    | | |    | | |    ELEMNAME_END
//  | |    | | | |    | | |    | | ELEMNAME_START
//  | |    | | | |    | | |    | ELEM_START
//  | |    | | | |    | | |    ELEMOBJ_END
//  | |    | | | |    | | STRVAL_START
//  | |    | | | |    | ELEMOBJ_START
//  | |    | | | |    ELEMNAME_END
//  | |    | | | ELEMNAME_START
//  | |    | | ELEM_START
//  | |    | ELEMOBJ_START
//  | |    ELEMNAME_END
//  | ELEMNAME_START
//  ELEM_START
//
StkObject* StkObject::CreateObjectFromJson(TCHAR* Json, int* Offset, StkObject* Parent)
{
	static const int ELEM_START = 1;
	static const int ELEMNAME_START = 2;
	static const int ELEMNAME_END = 3;
	static const int ELEMOBJ_START = 4;
	static const int ELEMOBJ_END = 5;
	static const int STRVAL_START = 6;
	static const int ARRAY_START = 7;
	static const int ARRAY_END = 8;

	StkObject* RetObj = Parent;
	TCHAR* PrevName = NULL;
	int PrevStatus = ELEM_START;
	BOOL ArrayFlag = FALSE;

	if (Json == NULL || lstrcmp(Json, _T("")) == 0) {
		Impl::CleanupObjectsForJson(PrevName, RetObj);
		*Offset = ERROR_JSON_NO_ELEMENT_FOUND;
		return NULL;
	}

	int Loop = 0;
	for (; Json[Loop] != TCHAR('\0'); Loop++) {

		// If ArrayFlag is TRUE, mode needs to be changed.
		if (ArrayFlag == TRUE && (PrevStatus == ARRAY_START || PrevStatus == ELEM_START)) {
			PrevStatus = ELEMOBJ_START;
		}

		// if blank is appeared...
		if ((Json[Loop] == TCHAR(' ') || Json[Loop] == TCHAR('\t') || Json[Loop] == TCHAR('\r') || Json[Loop] == TCHAR('\n')) && PrevStatus != ELEMNAME_START && PrevStatus != STRVAL_START) {
			continue;
		}

		// if \" is appeared...
		if (Json[Loop] == TCHAR('\"')) {
			if (PrevStatus == ELEM_START || PrevStatus == ARRAY_START) {
				PrevStatus = ELEMNAME_START;
				continue;
			} else if (PrevStatus == ELEMOBJ_START) {
				PrevStatus = STRVAL_START;
				continue;
			} else {
				Impl::CleanupObjectsForJson(PrevName, RetObj);
				*Offset = ERROR_JSON_INVALID_QUOT_FOUND;
				return NULL;
			}
		}

		// if : is appeared...
		if (Json[Loop] == TCHAR(':')) {
			if (PrevStatus == ELEMNAME_END) {
				PrevStatus = ELEMOBJ_START;
				continue;
			} else {
				Impl::CleanupObjectsForJson(PrevName, RetObj);
				*Offset = ERROR_JSON_INVALID_COLON_FOUND;
				return NULL;
			}
		}

		// if { is appeared...
		if (Json[Loop] == TCHAR('{')) {
			if (PrevStatus == ELEMOBJ_START) {
				int OffsetCld = 0;
				StkObject* ChildElem;
				if (RetObj == NULL) {
					RetObj = new StkObject(PrevName);
					ChildElem = CreateObjectFromJson(&Json[Loop + 1], &OffsetCld, RetObj);
				} else {
					StkObject* TmpObj = new StkObject(PrevName);
					RetObj->AppendChildElement(TmpObj);
					ChildElem = CreateObjectFromJson(&Json[Loop + 1], &OffsetCld, TmpObj);
				}
				Loop += OffsetCld;
				PrevStatus = ELEMOBJ_END;
				if (ChildElem != NULL) {
					// Nothing to do
				} else {
					RetObj = NULL;
					Impl::CleanupObjectsForJson(PrevName, RetObj);
					*Offset = OffsetCld;
					return NULL;
				}
				if (ArrayFlag == FALSE) {
					delete PrevName;
					PrevName = NULL;
				}
				continue;
			} else {
				Impl::CleanupObjectsForJson(PrevName, RetObj);
				*Offset = ERROR_JSON_INVALID_STRUCTURE;
				return NULL;
			}
		}

		// if } is appeared...
		if (Json[Loop] == TCHAR('}')) {
			if (PrevStatus == ELEMOBJ_END || PrevStatus == ARRAY_END) {
				*Offset = Loop + 1;
				return RetObj;
			} else {
				Impl::CleanupObjectsForJson(PrevName, RetObj);
				*Offset = ERROR_JSON_INVALID_STRUCTURE;
				return NULL;
			}
		}

		// if [ is appeared...
		if (Json[Loop] == TCHAR('[')) {
			if (PrevStatus == ELEMOBJ_START) {
				PrevStatus = ARRAY_START;
				ArrayFlag = TRUE;
				continue;
			} else {
				Impl::CleanupObjectsForJson(PrevName, RetObj);
				*Offset = ERROR_JSON_INVALID_ARRAY_STRUCTURE;
				return NULL;
			}
		}

		// if ] is appeared...
		if (Json[Loop] == TCHAR(']')) {
			if (PrevStatus == ELEMOBJ_END) {
				PrevStatus = ARRAY_END;
				ArrayFlag = FALSE;
				continue;
			} else {
				Impl::CleanupObjectsForJson(PrevName, RetObj);
				*Offset = ERROR_JSON_INVALID_ARRAY_STRUCTURE;
				return NULL;
			}
		}

		// if , is appeared...
		if (Json[Loop] == TCHAR(',')) {
			if (PrevStatus == ELEMOBJ_END) {
				PrevStatus = ELEM_START;
				continue;
			} else {
				Impl::CleanupObjectsForJson(PrevName, RetObj);
				*Offset = ERROR_JSON_INVALID_COMMA;
				return NULL;
			}
		}

		// if decimal number is appeared...
		if (Json[Loop] >= TCHAR('0') && Json[Loop] <= TCHAR('9')) {
			int ValInt = 0;
			float ValFloat = 0;
			if (PrevStatus == ELEMOBJ_START) {
				int StrLen = 0;
				BOOL IsFloat = Impl::GetJsonNumber(&Json[Loop], &StrLen, &ValInt, &ValFloat);
				StkObject* ChildObj;
				if (RetObj == NULL) {
					Impl::CleanupObjectsForJson(PrevName, RetObj);
					*Offset = ERROR_JSON_NO_ROOT_ELEMENT;
					return NULL;
				}
				if (IsFloat) {
					ChildObj = new StkObject(PrevName, ValFloat);
				} else {
					ChildObj = new StkObject(PrevName, ValInt);
				}
				RetObj->AppendChildElement(ChildObj);
				if (ArrayFlag == FALSE) {
					delete PrevName;
					PrevName = NULL;
				}
				Loop = Loop + StrLen - 1;
				PrevStatus = ELEMOBJ_END;
				continue;
			} else {
			}
		}

		// if other...
		if (PrevStatus == ELEMNAME_START) {
			int StrLen = 0;
			PrevName = Impl::GetJsonString(&Json[Loop], &StrLen);
			Loop = Loop + StrLen;
			PrevStatus = ELEMNAME_END;
			continue;
		}
		if (PrevStatus == STRVAL_START) {
			int StrLen = 0;
			TCHAR* Value = Impl::GetJsonString(&Json[Loop], &StrLen);
			if (RetObj == NULL) {
				delete Value;
				Impl::CleanupObjectsForJson(PrevName, RetObj);
				*Offset = ERROR_JSON_NO_ROOT_ELEMENT;
				return NULL;
			}
			RetObj->AppendChildElement(new StkObject(PrevName, Value));
			delete Value;
			if (ArrayFlag == FALSE) {
				delete PrevName;
				PrevName = NULL;
			}
			Loop = Loop + StrLen;
			PrevStatus = ELEMOBJ_END;
			continue;
		}
		Impl::CleanupObjectsForJson(PrevName, RetObj);
		*Offset = ERROR_JSON_CANNOT_HANDLE;
		return NULL;
	}

	if (PrevStatus != ELEMOBJ_END) {
		Impl::CleanupObjectsForJson(PrevName, RetObj);
		*Offset = ERROR_JSON_INVALID_STRUCTURE;
		return NULL;
	}

	if (RetObj == NULL) {
		*Offset = ERROR_JSON_NO_ELEMENT_FOUND;
		return NULL;
	}

	*Offset = Loop;
	return RetObj;
}

//   < Aaa Xxx = " Xxxxxxxxx"  Yyy = " Yyyyyyyyy" >  < / Aaa>
//  | |   |   | | |          |    | | |          | |  | |
//  | |   |   | | |          |    | | |          | |  | ELEM_END
//  | |   |   | | |          |    | | |          | |  ELEMNAME_START
//  | |   |   | | |          |    | | |          | ELEM_DOWN
//  | |   |   | | |          |    | | |          ATTRVAL_END
//  | |   |   | | |          |    | | ATTRVAL_START
//  | |   |   | | |          |    | ATTR_EQUAL
//  | |   |   | | |          |    ATTRNAME_END
//  | |   |   | | |          ATTRVAL_END
//  | |   |   | | ATTRVAL_START
//  | |   |   | ATTR_EQUAL
//  | |   |   ATTRNAME_END
//  | |   ELEMNAME_END
//  | ELEMNAME_START
//  ELEM_START
//
StkObject* StkObject::CreateObjectFromXml(TCHAR* Xml, int* Offset)
{
	static const int ELEM_START = 1;
	static const int ELEMNAME_START = 2;
	static const int ELEMNAME_END = 3;
	static const int ATTRNAME_END = 4;
	static const int ATTR_EQUAL = 5;
	static const int ATTRVAL_START = 6;
	static const int ATTRVAL_END = 7;
	static const int ELEM_DOWN = 8;
	static const int ELEM_END = 9;

	StkObject* RetObj = NULL;
	TCHAR* PrevAttrName = NULL;
	int PrevStatus = ELEM_START;

	if (Xml == NULL || lstrcmp(Xml, _T("")) == 0) {
		Impl::CleanupObjectsForXml(PrevAttrName, RetObj);
		*Offset = ERROR_XML_NO_ELEMENT_FOUND;
		return NULL;
	}

	for (int Loop = 0; Xml[Loop] != TCHAR('\0'); Loop++) {

		// if blank is appeared...
		if ((Xml[Loop] == TCHAR(' ') || Xml[Loop] == TCHAR('\t') || Xml[Loop] == TCHAR('\r') || Xml[Loop] == TCHAR('\n')) && PrevStatus != ATTRVAL_START) {
			continue;
		}

		// if '<' is appeared...
		if (Xml[Loop] == TCHAR('<')) {
			if (PrevStatus == ELEM_START) {
				PrevStatus = ELEMNAME_START;
				continue;
			} else if (PrevStatus == ELEM_DOWN) {
				for (int Loop2 = Loop + 1; Xml[Loop2] != CHAR('\0'); Loop2++) {
					if (Xml[Loop2] == TCHAR(' ') || Xml[Loop2] == TCHAR('\t') || Xml[Loop2] == TCHAR('\r') || Xml[Loop2] == TCHAR('\n')) {
						continue;
					} else if (Xml[Loop2] == TCHAR('/')) {
						PrevStatus = ELEMNAME_START;
						break;
					} else {
						break;
					}
				}
				if (PrevStatus == ELEMNAME_START) {
					continue;
				}
				int OffsetCld = 0;
				StkObject* ChildElem = CreateObjectFromXml(&Xml[Loop], &OffsetCld);
				Loop += OffsetCld - 1;
				if (ChildElem != NULL) {
					RetObj->AppendChildElement(ChildElem);
				} else {
					Impl::CleanupObjectsForXml(PrevAttrName, RetObj);
					*Offset = OffsetCld;
					return NULL;
				}
				continue;
			} else {
				Impl::CleanupObjectsForXml(PrevAttrName, RetObj);
				*Offset = ERROR_XML_INVALID_ELEMENT_START_FOUND;
				return NULL;
			}
		}

		// if '=' is appeared...
		if (Xml[Loop] == TCHAR('=') && PrevStatus != ELEM_DOWN) {
			if (PrevStatus == ATTRNAME_END) {
				PrevStatus = ATTR_EQUAL;
				continue;
			} else {
				Impl::CleanupObjectsForXml(PrevAttrName, RetObj);
				*Offset = ERROR_XML_EQUAL_FOUND_WITHOUT_ATTRIBUTE_NAME;
				return NULL;
			}
		}

		// if '\"' is appeared...
		if (Xml[Loop] == TCHAR('\"')) {
			if (PrevStatus == ATTR_EQUAL) {
				PrevStatus = ATTRVAL_START;
				continue;
			} else if (PrevStatus == ATTRVAL_START) {
				StkObject* AttrObj = new StkObject(PrevAttrName, _T(""));
				RetObj->AppendAttribute(AttrObj);
				delete PrevAttrName;
				PrevAttrName = NULL;
				PrevStatus = ATTRVAL_END;
				continue;
			} else {
				Impl::CleanupObjectsForXml(PrevAttrName, RetObj);
				*Offset = ERROR_XML_INVALID_QUOT_FOUND;
				return NULL;
			}
		}

		// if '>' is appeared...
		if (Xml[Loop] == TCHAR('>')) {
			if (PrevStatus == ATTRVAL_END || PrevStatus == ELEMNAME_END) {
				PrevStatus = ELEM_DOWN;
				continue;
			} else {
				Impl::CleanupObjectsForXml(PrevAttrName, RetObj);
				*Offset = ERROR_XML_INVALID_ELEMENT_END_FOUND;
				return NULL;
			}
		}

		// if '?' is appeared...
		if ((Xml[Loop] == TCHAR('?') || Xml[Loop] == TCHAR('!')) && PrevStatus != ELEM_DOWN) {
			if (PrevStatus == ELEMNAME_START) {
				PrevStatus = ELEM_START;
				for (; Xml[Loop] != TCHAR('\0'); Loop++) {
					if (Xml[Loop] == TCHAR('>')) {
						break;
					}
				}
				if (Xml[Loop] == TCHAR('\0')) {
					Impl::CleanupObjectsForXml(PrevAttrName, RetObj);
					*Offset = ERROR_XML_ELEMENT_END_NOT_FOUND;
					return NULL;
				}
				continue;
			}
		}

		// if '/' is appeared...
		if (Xml[Loop] == TCHAR('/') && PrevStatus != ELEM_DOWN) {
			if (PrevStatus == ELEMNAME_START || PrevStatus == ELEMNAME_END || PrevStatus == ATTRVAL_END) {
				PrevStatus = ELEM_END;
			} else {
				Impl::CleanupObjectsForXml(PrevAttrName, RetObj);
				*Offset = ERROR_XML_INVALID_SLASH_FOUND;
				return NULL;
			}
			for (int Loop2 = Loop + 1; Xml[Loop2] != TCHAR('\0'); Loop2++) {
				if (Xml[Loop2] == TCHAR('>')) {
					if (RetObj == NULL) {
						Impl::CleanupObjectsForXml(PrevAttrName, RetObj);
						*Offset = ERROR_XML_SLASH_FOUND_WITHOUT_ELEMENT;
						return NULL;
					}
					*Offset = Loop2 + 1;
					if (PrevAttrName != NULL) {
						delete PrevAttrName;
					}
					return RetObj;
				}
			}
			Impl::CleanupObjectsForXml(PrevAttrName, RetObj);
			*Offset = ERROR_XML_SLASH_FOUND_WITHOUT_ELEMENT_END;
			return NULL;
		}

		// if other...
		if (PrevStatus == ELEMNAME_START) {
			int StrLen = 0;
			TCHAR* Name = Impl::GetName(&Xml[Loop], &StrLen);
			RetObj = new StkObject(Name);
			delete Name;
			Loop = Loop + StrLen - 1;
			PrevStatus = ELEMNAME_END;
			continue;
		}
		if (PrevStatus == ELEMNAME_END || PrevStatus == ATTRVAL_END) {
			int StrLen = 0;
			PrevAttrName = Impl::GetName(&Xml[Loop], &StrLen);
			Loop = Loop + StrLen - 1;
			PrevStatus = ATTRNAME_END;
			continue;
		}
		if (PrevStatus == ATTRVAL_START) {
			int StrLen = 0;
			TCHAR* Value = Impl::GetValue(&Xml[Loop], &StrLen);
			StkObject* AttrObj = new StkObject(PrevAttrName, Value);
			RetObj->AppendAttribute(AttrObj);
			delete PrevAttrName;
			PrevAttrName = NULL;
			delete Value;
			Loop = Loop + StrLen;
			PrevStatus = ATTRVAL_END;
			continue;
		}
		if (PrevStatus == ELEM_DOWN) {
			int StrLen = 0;
			TCHAR* Value = Impl::GetValue(&Xml[Loop], &StrLen);
			Loop = Loop + StrLen;
			if (RetObj != NULL) {
				RetObj->SetType(StkObject::STKOBJECT_ELEM_STRING);
				RetObj->SetStringValue(Value);
			}
			delete Value;
			continue;
		}
		Impl::CleanupObjectsForXml(PrevAttrName, RetObj);
		*Offset = ERROR_XML_CANNOT_HANDLE;
		return NULL;
	}

	Impl::CleanupObjectsForXml(PrevAttrName, RetObj);
	*Offset = ERROR_XML_ELEMENT_END_NOT_FOUND;
	return NULL;
}
