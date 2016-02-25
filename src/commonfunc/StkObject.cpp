#include <windows.h>
#include <string>
#include <tchar.h>
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
