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
};

void StkObject::Impl::ClearMember()
{
	Name = NULL;
	Type = STKOBJECT_UNKNOWN;
	Value = NULL;
	FirstAttr = NULL;
	LastAttr = NULL;
	FirstElem = NULL;
	LastElem = NULL;
	Next = NULL;
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
	pImpl->Type = STKOBJECT_INT;
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
	pImpl->Type = STKOBJECT_FLOAT;
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
	pImpl->Type = STKOBJECT_STRING;
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
	if (pImpl->Type == STKOBJECT_INT) {
		NewObj = new StkObject(TmpName, GetIntValue());
	} else if (pImpl->Type == STKOBJECT_FLOAT) {
		NewObj = new StkObject(TmpName, GetFloatValue());
	} else if (pImpl->Type == STKOBJECT_STRING) {
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

TCHAR* StkObject::GetName()
{
	return pImpl->Name;
}

int StkObject::GetType()
{
	return pImpl->Type;
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
	if (pImpl->Name == NULL || pImpl->Type == STKOBJECT_UNKNOWN) {
		return;
	}
	if (pImpl->Type != STKOBJECT_ELEMENT) {
		if (pImpl->Type == STKOBJECT_INT) {
			int *Val = (int*)pImpl->Value;
			TCHAR TmpBuf[15];
			_snwprintf_s(TmpBuf, 15, _TRUNCATE, _T("%d"), *Val);
			*Msg = *Msg + _T(" ") + pImpl->Name + _T("=\"") + TmpBuf + _T("\"");
		} else if (pImpl->Type == STKOBJECT_FLOAT) {
			float *Val = (float*)pImpl->Value;
			TCHAR TmpBuf[25];
			_snwprintf_s(TmpBuf, 25, _TRUNCATE, _T("%f"), *Val);
			*Msg = *Msg + _T(" ") + pImpl->Name + _T("=\"") + TmpBuf + _T("\"");
		} else if (pImpl->Type == STKOBJECT_STRING) {
			*Msg = *Msg + _T(" ") + pImpl->Name + _T("=\"") + (TCHAR*)pImpl->Value + _T("\"");
		}
		if (pImpl->Next != NULL) {
			StkObject* TmpObj = pImpl->Next;
			TmpObj->ToXml(Msg, Indent);
		}
	} else {
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
	}
}
