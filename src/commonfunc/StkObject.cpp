#include "../stkpl/StkPl.h"
#include "StkObject.h"

class StkObject::Impl
{
public:
	wchar_t* Name;
	int Type;
	void* Value;

	StkObject* FirstAttr;
	StkObject* LastAttr;
	StkObject* FirstElem;
	StkObject* LastElem;
	StkObject* Next;

public:
	void ClearMember();
	int XmlEncodeSize(wchar_t*);
	void XmlEncode(wchar_t*, wchar_t*, int);
	int JsonEncodeSize(wchar_t*);
	void JsonEncode(wchar_t*, wchar_t*, int);

	bool IsArrayExpression(StkObject*);
	int AddString(wchar_t*, const wchar_t*, const wchar_t*);

	static wchar_t* GetJsonString(const wchar_t*, int*);
	static bool GetJsonNumber(const wchar_t*, int*, int*, float*);
	static wchar_t* GetName(const wchar_t*, int*);
	static wchar_t* GetValue(const wchar_t*, int*);
	static void CleanupObjectsForXml(wchar_t*, StkObject*);
	static void CleanupObjectsForJson(wchar_t*, StkObject*);

	StkObject* ContainsInArray(StkObject*, StkObject*);
	bool Equals(StkObject*, StkObject*);
	StkObject* Contains(StkObject*, StkObject*, bool);
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
int StkObject::Impl::XmlEncodeSize(wchar_t* InMsg)
{
	int Size = 0;
	int InMsgLen = StkPlWcsLen(InMsg);
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
int StkObject::Impl::JsonEncodeSize(wchar_t* InMsg)
{
	int Size = 0;
	int InMsgLen = StkPlWcsLen(InMsg);
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
void StkObject::Impl::XmlEncode(wchar_t* InMsg, wchar_t* OutMsg, int SizeOfOutMsg)
{
	int OutMsgIndex = 0;
	int InMsgLen = StkPlWcsLen(InMsg);
	for (int Loop = 0; Loop < InMsgLen; Loop++) {
		if (InMsg[Loop] == '<') {
			if (OutMsgIndex + 4 >= SizeOfOutMsg)
				break;
			StkPlLStrCpy(&OutMsg[OutMsgIndex], L"&lt;");
			OutMsgIndex += 4;
			continue;
		}
		if (InMsg[Loop] == '>') {
			if (OutMsgIndex + 4 >= SizeOfOutMsg)
				break;
			StkPlLStrCpy(&OutMsg[OutMsgIndex], L"&gt;");
			OutMsgIndex += 4;
			continue;
		}
		if (InMsg[Loop] == '&') {
			if (OutMsgIndex + 5 >= SizeOfOutMsg)
				break;
			StkPlLStrCpy(&OutMsg[OutMsgIndex], L"&amp;");
			OutMsgIndex += 5;
			continue;
		}
		if (InMsg[Loop] == '\"') {
			if (OutMsgIndex + 6 >= SizeOfOutMsg)
				break;
			StkPlLStrCpy(&OutMsg[OutMsgIndex], L"&quot;");
			OutMsgIndex += 6;
			continue;
		}
		if (InMsg[Loop] == '\'') {
			if (OutMsgIndex + 6 >= SizeOfOutMsg)
				break;
			StkPlLStrCpy(&OutMsg[OutMsgIndex], L"&apos;");
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
void StkObject::Impl::JsonEncode(wchar_t* InData, wchar_t* OutData, int OutDataLen)
{
	wchar_t* CurInPtr = InData;
	wchar_t* CurOutPtr = OutData;
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

bool StkObject::Impl::IsArrayExpression(StkObject* Obj)
{
	if (!(Obj = Obj->GetFirstChildElement())) {
		return false;
	}
	if (!Obj->GetNext()) {
		return false;
	}
	wchar_t* BsName = Obj->GetName();
	while (Obj = Obj->GetNext()) {
		wchar_t* CurName = Obj->GetName();
		if (StkPlWcsCmp(BsName, CurName) != 0) {
			return false;
		}
	}
	return true;
}

// Add string to the presented string
// Msg [in/out] : Pointer where Tgt is appended.
// EndPoint [in] : End point of Msg
// Tgt [in] : String to be appended.
// Return : (0:If enoght area is not prepared, Otherwise:Length of string appended)
int StkObject::Impl::AddString(wchar_t* Msg, const wchar_t* EndPoint, const wchar_t* Tgt)
{
	int Loop = 0;
	for (; Msg + Loop < EndPoint - 1 && Tgt[Loop] != '\0'; Loop++) {
		Msg[Loop] = Tgt[Loop];
	}
	Msg[Loop] = L'\0';
	return Loop;
}

void StkObject::Impl::CleanupObjectsForXml(wchar_t* PrevAttrName, StkObject* RetObj)
{
	if (PrevAttrName != NULL) {
		delete PrevAttrName;
	}
	if (RetObj != NULL) {
		delete RetObj;
	}
}

void StkObject::Impl::CleanupObjectsForJson(wchar_t* PrevName, StkObject* RetObj)
{
	if (PrevName != NULL) {
		delete PrevName;
	}
	if (RetObj != NULL) {
		delete RetObj;
	}
}

bool StkObject::Impl::GetJsonNumber(const wchar_t* OrgStr, int* Len, int* ValInt, float* ValFloat)
{
	const wchar_t* CurPnt = OrgStr;
	int ValueLength = 0;
	bool FloatFlag = false;

	while (*CurPnt != wchar_t('\0')) {
		if (*CurPnt >= wchar_t('0') && *CurPnt <= wchar_t('9')) {
			ValueLength++;
			CurPnt++;
		} else if (*CurPnt == wchar_t('.')) {
			ValueLength++;
			CurPnt++;
			FloatFlag = true;
		} else if (*CurPnt == wchar_t('-')) {
			ValueLength++;
			CurPnt++;
		} else {
			break;
		}
	}
	wchar_t* RtnValue = new wchar_t[ValueLength + 1];
	*Len = ValueLength;
	int Loop = 0;
	for (; Loop < ValueLength; Loop++) {
		RtnValue[Loop] = OrgStr[Loop];
	}
	RtnValue[Loop] = wchar_t('\0');

	if (FloatFlag == true) {
		*ValFloat = (float)StkPlWcsToF(RtnValue);
	} else {
		*ValInt = (int)StkPlWcsToL(RtnValue);
	}
	delete RtnValue;
	return FloatFlag;
}

wchar_t* StkObject::Impl::GetJsonString(const wchar_t* OrgStr, int* Len)
{
	int OutLength = 0;
	const wchar_t* CurPnt = OrgStr;
	while (*CurPnt != wchar_t('\"') && *CurPnt != L'\0') {
		if (*CurPnt == wchar_t('\\')) {
			if (*(CurPnt + 1) == wchar_t('\"') || *(CurPnt + 1) == wchar_t('\\') || *(CurPnt + 1) == wchar_t('/') || *(CurPnt + 1) == wchar_t('b') ||
				*(CurPnt + 1) == wchar_t('f') || *(CurPnt + 1) == wchar_t('n') || *(CurPnt + 1) == wchar_t('r') || *(CurPnt + 1) == wchar_t('t')) {
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
	if (*CurPnt == wchar_t('\"')) {
		*Len = CurPnt - OrgStr;
	} else if (*CurPnt == wchar_t('\0')) {
		*Len = CurPnt - OrgStr - 1;
	}

	wchar_t* RtnValue = new wchar_t[OutLength + 1];
	int RtnLoop = 0;
	for (const wchar_t* Loop = OrgStr; Loop < CurPnt; Loop++) {
		if (*Loop == L'\\' && *(Loop + 1) == L'\"') {
			RtnValue[RtnLoop] = '\"';
			RtnLoop++;
			Loop++;
			continue;
		}
		if (*Loop == L'\\' && *(Loop + 1) == L'\\') {
			RtnValue[RtnLoop] = '\\';
			RtnLoop++;
			Loop++;
			continue;
		}
		if (*Loop == L'\\' && *(Loop + 1) == L'/') {
			RtnValue[RtnLoop] = '/';
			RtnLoop++;
			Loop++;
			continue;
		}
		if (*Loop == L'\\' && *(Loop + 1) == L'b') {
			RtnValue[RtnLoop] = '\b';
			RtnLoop++;
			Loop++;
			continue;
		}
		if (*Loop == L'\\' && *(Loop + 1) == L'f') {
			RtnValue[RtnLoop] = '\f';
			RtnLoop++;
			Loop++;
			continue;
		}
		if (*Loop == L'\\' && *(Loop + 1) == L'n') {
			RtnValue[RtnLoop] = '\n';
			RtnLoop++;
			Loop++;
			continue;
		}
		if (*Loop == L'\\' && *(Loop + 1) == L'r') {
			RtnValue[RtnLoop] = '\r';
			RtnLoop++;
			Loop++;
			continue;
		}
		if (*Loop == L'\\' && *(Loop + 1) == L't') {
			RtnValue[RtnLoop] = '\t';
			RtnLoop++;
			Loop++;
			continue;
		}
		RtnValue[RtnLoop] = *Loop;
		RtnLoop++;
	}
	RtnValue[OutLength] = wchar_t('\0');
	return RtnValue;
}

wchar_t* StkObject::Impl::GetName(const wchar_t* TgtName, int* Len)
{
	const wchar_t* CurPnt = TgtName;
	int NameLength = 0;
	while (*CurPnt != wchar_t(' ') && *CurPnt != wchar_t('\t') && *CurPnt != wchar_t('\r') && *CurPnt != wchar_t('\n') && *CurPnt != wchar_t('>') && *CurPnt != wchar_t('/') && *CurPnt != wchar_t('=') && *CurPnt != wchar_t('\0')) {
		NameLength++;
		CurPnt++;
	}
	*Len = NameLength;
	wchar_t* RtnName = new wchar_t[NameLength + 1];
	for (int Loop = 0; Loop < NameLength; Loop++) {
		RtnName[Loop] = TgtName[Loop];
	}
	RtnName[NameLength] = wchar_t('\0');
	return RtnName;
}

wchar_t* StkObject::Impl::GetValue(const wchar_t* TgtValue, int* Len)
{
	const wchar_t* CurPnt = TgtValue;
	int ValueLength = 0;
	while (*CurPnt != wchar_t('\"') && *CurPnt != wchar_t('<') && *CurPnt != wchar_t('\0')) {
		if (*CurPnt == wchar_t('&')) {
			if (*(CurPnt + 1) != wchar_t('\0') && *(CurPnt + 2) != wchar_t('\0') && *(CurPnt + 3) != wchar_t('\0') && StkPlWcsStr(CurPnt, L"&lt;") == CurPnt) {
				CurPnt += 4;
			} else if (*(CurPnt + 1) != wchar_t('\0') && *(CurPnt + 2) != wchar_t('\0') && *(CurPnt + 3) != wchar_t('\0') && StkPlWcsStr(CurPnt, L"&gt;") == CurPnt) {
				CurPnt += 4;
			} else if (*(CurPnt + 1) != wchar_t('\0') && *(CurPnt + 2) != wchar_t('\0') && *(CurPnt + 3) != wchar_t('\0') && *(CurPnt + 4) != wchar_t('\0') && StkPlWcsStr(CurPnt, L"&amp;") == CurPnt) {
				CurPnt += 5;
			} else if (*(CurPnt + 1) != wchar_t('\0') && *(CurPnt + 2) != wchar_t('\0') && *(CurPnt + 3) != wchar_t('\0') && *(CurPnt + 4) != wchar_t('\0') && *(CurPnt + 5) != wchar_t('\0') && StkPlWcsStr(CurPnt, L"&quot;") == CurPnt) {
				CurPnt += 6;
			} else if (*(CurPnt + 1) != wchar_t('\0') && *(CurPnt + 2) != wchar_t('\0') && *(CurPnt + 3) != wchar_t('\0') && *(CurPnt + 4) != wchar_t('\0') && *(CurPnt + 5) != wchar_t('\0') && StkPlWcsStr(CurPnt, L"&apos;") == CurPnt) {
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
	if (*CurPnt == wchar_t('\"')) {
		*Len = CurPnt - TgtValue;
	} else if (*CurPnt == wchar_t('<') || *CurPnt == wchar_t('\0')) {
		*Len = CurPnt - TgtValue - 1;
	}
	wchar_t* RtnValue = new wchar_t[ValueLength + 1];
	int RtnLoop = 0;
	for (const wchar_t* Loop = TgtValue; Loop < CurPnt; Loop++) {
		if (StkPlWcsStr(Loop, L"&lt;") == Loop) {
			RtnValue[RtnLoop] = '<';
			RtnLoop++;
			Loop += 3;
			continue;
		}
		if (StkPlWcsStr(Loop, L"&gt;") == Loop) {
			RtnValue[RtnLoop] = '>';
			RtnLoop++;
			Loop += 3;
			continue;
		}
		if (StkPlWcsStr(Loop, L"&amp;") == Loop) {
			RtnValue[RtnLoop] = '&';
			RtnLoop++;
			Loop += 4;
			continue;
		}
		if (StkPlWcsStr(Loop, L"&quot;") == Loop) {
			RtnValue[RtnLoop] = '\"';
			RtnLoop++;
			Loop += 5;
			continue;
		}
		if (StkPlWcsStr(Loop, L"&apos;") == Loop) {
			RtnValue[RtnLoop] = '\'';
			RtnLoop++;
			Loop += 5;
			continue;
		}
		RtnValue[RtnLoop] = *Loop;
		RtnLoop++;
	}
	RtnValue[ValueLength] = wchar_t('\0');
	return RtnValue;
}

StkObject* StkObject::Impl::ContainsInArray(StkObject* Obj1, StkObject* Obj2)
{
	if (Obj1 == NULL || Obj2 == NULL) {
		return NULL;
	}
	do {
		if (StkPlWcsCmp(Obj1->GetName(), Obj2->GetName()) != 0) {
			continue;
		}
		int CurType = Obj1->GetType();
		if (CurType != Obj2->GetType() && (CurType == StkObject::STKOBJECT_ELEMENT || CurType == StkObject::STKOBJECT_ATTRIBUTE || CurType == StkObject::STKOBJECT_UNKNOWN)) {
			continue;
		}
		if (CurType % 10 != Obj1->GetType() % 10) {
			continue;
		}
		if (CurType == StkObject::STKOBJECT_ATTR_INT || CurType == StkObject::STKOBJECT_ELEM_INT || CurType == StkObject::STKOBJECT_UNKW_INT) {
			if (Obj1->GetIntValue() == Obj2->GetIntValue()) {
				return Obj2;
			}
		} else if (CurType == StkObject::STKOBJECT_ATTR_FLOAT || CurType == StkObject::STKOBJECT_ELEM_FLOAT || CurType == StkObject::STKOBJECT_UNKW_FLOAT) {
			if (Obj1->GetFloatValue() == Obj2->GetFloatValue()) {
				return Obj2;
			}
		} else if (CurType == StkObject::STKOBJECT_ATTR_STRING || CurType == StkObject::STKOBJECT_ELEM_STRING || CurType == StkObject::STKOBJECT_UNKW_STRING) {
			if (StkPlWcsCmp(Obj1->GetStringValue(), Obj2->GetStringValue()) == 0) {
				return Obj2;
			}
		} else if (CurType == StkObject::STKOBJECT_ELEMENT) {
			return Obj2;
		} else {
			continue;
		}
	} while (Obj2 = Obj2->GetNext());
	return NULL;
}

bool StkObject::Impl::Equals(StkObject* Obj1, StkObject* Obj2)
{
	if (Obj1 == NULL || Obj2 == NULL) {
		return false;
	}
	if (StkPlWcsCmp(Obj1->GetName(), Obj2->GetName()) != 0) {
		return false;
	}
	int CurType = Obj1->GetType();
	if (CurType != Obj2->GetType()) {
		return false;
	}
	if (Obj1->GetType() == StkObject::STKOBJECT_ELEMENT) {
		if (Obj1->GetAttributeCount() != Obj2->GetAttributeCount()) {
			return false;
		}
		if (Obj1->GetChildElementCount() != Obj2->GetChildElementCount()) {
			return false;
		}

		//For attribute
		StkObject* AttrObj1 = Obj1->GetFirstAttribute();
		StkObject* AttrObj2 = Obj2->GetFirstAttribute();
		if (AttrObj1 != NULL && AttrObj2 != NULL) {
			while (AttrObj1) {
				if (ContainsInArray(AttrObj1, AttrObj2) == NULL) {
					return false;
				}
				AttrObj1 = AttrObj1->GetNext();
			}
		} else if (AttrObj1 != NULL || AttrObj2 != NULL) {
			return false;
		}

		//For Element
		StkObject* ElemObj1 = Obj1->GetFirstChildElement();
		StkObject* ElemObj2 = Obj2->GetFirstChildElement();
		if (ElemObj1 != NULL && ElemObj2 != NULL) {
			while (ElemObj1) {
				if (ElemObj1->GetType() == StkObject::STKOBJECT_ELEMENT) {
					StkObject* AcquiredObj = ElemObj2;
					bool Ret = false;
					do {
						AcquiredObj = ContainsInArray(ElemObj1, AcquiredObj);
						if (AcquiredObj == NULL) {
							return false;
						}
						Ret = Equals(ElemObj1, AcquiredObj);
						AcquiredObj = AcquiredObj->GetNext();
					} while (!Ret);
				} else {
					StkObject* AcquiredObj = ContainsInArray(ElemObj1, ElemObj2);
					if (AcquiredObj == NULL) {
						return false;
					}
				}
				ElemObj1 = ElemObj1->GetNext();
			}
		} else if (ElemObj1 != NULL || ElemObj2 != NULL) {
			return false;
		}
	} else if (CurType == StkObject::STKOBJECT_ATTR_INT || CurType == StkObject::STKOBJECT_ELEM_INT || CurType == StkObject::STKOBJECT_UNKW_INT) {
		if (Obj1->GetIntValue() != Obj2->GetIntValue()) {
			return false;
		}
	} else if (CurType == StkObject::STKOBJECT_ATTR_FLOAT || CurType == StkObject::STKOBJECT_ELEM_FLOAT || CurType == StkObject::STKOBJECT_UNKW_FLOAT) {
		if (Obj1->GetFloatValue() != Obj2->GetFloatValue()) {
			return false;
		}
	} else if (CurType == StkObject::STKOBJECT_ATTR_STRING || CurType == StkObject::STKOBJECT_ELEM_STRING || CurType == StkObject::STKOBJECT_UNKW_STRING) {
		if (StkPlWcsCmp(Obj1->GetStringValue(), Obj2->GetStringValue()) != 0) {
			return false;
		}
	}
	return true;
}

StkObject* StkObject::Impl::Contains(StkObject* Obj1, StkObject* Obj2, bool ParentMatched)
{
	if (Obj1 == NULL || Obj2 == NULL) {
		return NULL;
	}
	if (Obj1->GetType() == StkObject::STKOBJECT_ELEMENT) {
		bool Matched = true;

		if (StkPlWcsCmp(Obj1->GetName(), Obj2->GetName()) != 0) {
			Matched = false;
		}
		if (Obj1->GetType() != Obj2->GetType()) {
			Matched = false;
		}
		if (Obj1->GetAttributeCount() < Obj2->GetAttributeCount()) {
			Matched = false;
		}
		if (Obj1->GetChildElementCount() < Obj2->GetChildElementCount()) {
			Matched = false;
		}
		if (ParentMatched && !Matched) {
			return NULL;
		}

		//For attribute
		if (Matched) {
			StkObject* AttrObj1 = Obj1->GetFirstAttribute();
			StkObject* AttrObj2 = Obj2->GetFirstAttribute();
			if (AttrObj1 != NULL && AttrObj2 != NULL) {
				bool FndFlag = true;
				while (AttrObj2) {
					if (ContainsInArray(AttrObj2, AttrObj1) == NULL) {
						FndFlag = false;
						break;
					}
					AttrObj2 = AttrObj2->GetNext();
				}
				if (FndFlag == false) {
					Matched = false;
				}
			} else if (AttrObj1 == NULL && AttrObj2 != NULL) {
				Matched = false;
			}
		}
		if (ParentMatched && !Matched) {
			return NULL;
		}

		//For Element
		if (Matched) {
			StkObject* ElemObj1 = Obj1->GetFirstChildElement();
			StkObject* ElemObj2 = Obj2->GetFirstChildElement();
			if (ElemObj1 != NULL && ElemObj2 != NULL) {
				bool FndFlag = true;
				while (ElemObj2) {
					if (ContainsInArray(ElemObj2, ElemObj1) == NULL) {
						FndFlag = false;
						break;
					}
					if (ElemObj2->GetType() == StkObject::STKOBJECT_ELEMENT) {
						bool FndFlag2 = false;
						for (StkObject* LoopObj = ElemObj1; LoopObj != NULL; LoopObj = LoopObj->GetNext()) {
							if (LoopObj->GetType() == StkObject::STKOBJECT_ELEMENT && Contains(LoopObj, ElemObj2, true)) {
								FndFlag2 = true;
								break;
							}
						}
						if (!FndFlag2) {
							FndFlag = false;
							break;
						}
					}
					ElemObj2 = ElemObj2->GetNext();
				}
				if (FndFlag == false) {
					Matched = false;
				}
			} else if (ElemObj1 == NULL && ElemObj2 != NULL) {
				Matched = false;
			}
		}
		if (ParentMatched && !Matched) {
			return NULL;
		}
		if (Matched) {
			return Obj1;
		}

		// Not Matched case
		for (StkObject* LoopObj = ContainsInArray(Obj2, Obj1->GetFirstChildElement()); LoopObj != NULL; LoopObj = ContainsInArray(Obj2, LoopObj)) {
			if (LoopObj->GetType() != StkObject::STKOBJECT_ELEMENT) {
				return LoopObj;
			} else {
				break;
			}
		}
		bool FndFlag = false;
		StkObject* CldElm;
		for (StkObject* LoopObj = Obj1->GetFirstChildElement(); LoopObj != NULL; LoopObj = LoopObj->GetNext()) {
			if (LoopObj->GetType() == StkObject::STKOBJECT_ELEMENT && (CldElm = Contains(LoopObj, Obj2, false)) != NULL) {
				FndFlag = true;
				break;
			}
		}
		if (FndFlag) {
			return CldElm;
		}
		return NULL;
	} else {
		if (StkPlWcsCmp(Obj1->GetName(), Obj2->GetName()) != 0) {
			return NULL;
		}
		if (Obj1->GetType() != Obj2->GetType()) {
			return NULL;
		}
		if (Obj1->GetType() == StkObject::STKOBJECT_ATTR_INT || Obj1->GetType() == StkObject::STKOBJECT_ELEM_INT || Obj1->GetType() == StkObject::STKOBJECT_UNKW_INT) {
			if (Obj1->GetIntValue() == Obj2->GetIntValue()) {
				return Obj1;
			}
		} else if (Obj1->GetType() == StkObject::STKOBJECT_ATTR_FLOAT || Obj1->GetType() == StkObject::STKOBJECT_ELEM_FLOAT || Obj1->GetType() == StkObject::STKOBJECT_UNKW_FLOAT) {
			if (Obj1->GetFloatValue() == Obj2->GetFloatValue()) {
				return Obj1;
			}
		} else if (Obj1->GetType() == StkObject::STKOBJECT_ATTR_STRING || Obj1->GetType() == StkObject::STKOBJECT_ELEM_STRING || Obj1->GetType() == StkObject::STKOBJECT_UNKW_STRING) {
			if (StkPlWcsCmp(Obj1->GetStringValue(), Obj2->GetStringValue()) == 0) {
				return Obj1;
			}
		}
	}
	return NULL;
}









StkObject::StkObject(const wchar_t* TmpName)
{
	pImpl = new Impl;
	pImpl->ClearMember();
	pImpl->Type = STKOBJECT_ELEMENT;
	if (TmpName != NULL) {
		int Len = StkPlWcsLen(TmpName) + 1;
		pImpl->Name = new wchar_t[Len];
		StkPlLStrCpy(pImpl->Name, TmpName);
	}
}

StkObject::StkObject(const wchar_t* TmpName, int TmpValue)
{
	pImpl = new Impl;
	pImpl->ClearMember();
	pImpl->Type = STKOBJECT_UNKW_INT;
	if (TmpName != NULL) {
		int Len = StkPlWcsLen(TmpName) + 1;
		pImpl->Name = new wchar_t[Len];
		StkPlLStrCpy(pImpl->Name, TmpName);
	}
	pImpl->Value = new int(TmpValue);
}

StkObject::StkObject(const wchar_t* TmpName, float TmpValue)
{
	pImpl = new Impl;
	pImpl->ClearMember();
	pImpl->Type = STKOBJECT_UNKW_FLOAT;
	if (TmpName != NULL) {
		int Len = StkPlWcsLen(TmpName) + 1;
		pImpl->Name = new wchar_t[Len];
		StkPlLStrCpy(pImpl->Name, TmpName);
	}
	pImpl->Value = new float(TmpValue);
}

StkObject::StkObject(const wchar_t* TmpName, const wchar_t* TmpValue)
{
	pImpl = new Impl;
	pImpl->ClearMember();
	pImpl->Type = STKOBJECT_UNKW_STRING;
	if (TmpName != NULL) {
		int Len = StkPlWcsLen(TmpName) + 1;
		pImpl->Name = new wchar_t[Len];
		StkPlLStrCpy(pImpl->Name, TmpName);
	}
	if (TmpValue != NULL) {
		int Len = StkPlWcsLen(TmpValue) + 1;
		pImpl->Value = new wchar_t[Len];
		StkPlLStrCpy((wchar_t*)pImpl->Value, TmpValue);
	}
}

StkObject::~StkObject()
{
	if (pImpl->Next != NULL) {
		delete pImpl->Next;
		pImpl->Next = NULL;
	}
	if (pImpl->FirstAttr != NULL) {
		delete pImpl->FirstAttr;
		pImpl->FirstAttr = NULL;
	}
	if (pImpl->FirstElem != NULL) {
		delete pImpl->FirstElem;
		pImpl->FirstElem = NULL;
	}
	if (pImpl->Name != NULL) {
		int Len = StkPlWcsLen((wchar_t*)pImpl->Name) + 1;
		delete [] pImpl->Name;
		pImpl->Name = NULL;
	}
	if (pImpl->Value != NULL) {
		if (pImpl->Type == StkObject::STKOBJECT_ATTR_INT || pImpl->Type == StkObject::STKOBJECT_ELEM_INT || pImpl->Type == StkObject::STKOBJECT_UNKW_INT) {
			delete (int*)pImpl->Value;
		} else if (pImpl->Type == StkObject::STKOBJECT_ATTR_FLOAT || pImpl->Type == StkObject::STKOBJECT_ELEM_FLOAT || pImpl->Type == StkObject::STKOBJECT_UNKW_FLOAT) {
			delete (float*)pImpl->Value;
		} else if (pImpl->Type == StkObject::STKOBJECT_ATTR_STRING || pImpl->Type == StkObject::STKOBJECT_ELEM_STRING || pImpl->Type == StkObject::STKOBJECT_UNKW_STRING) {
			int Len = StkPlWcsLen((wchar_t*)pImpl->Value) + 1;
			delete [] (wchar_t*)pImpl->Value;
		} else {
			// There is no case that process enters this line.
		}
		pImpl->Value = NULL;
	}
	delete pImpl;
}

StkObject* StkObject::Clone()
{
	StkObject* NewObj;
	wchar_t* TmpName = GetName();
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

bool StkObject::Equals(StkObject* Obj)
{
	return pImpl->Equals(this, Obj);
}

StkObject* StkObject::Contains(StkObject* Obj)
{
	return pImpl->Contains(this, Obj, false);
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
	if (pImpl->Type != STKOBJECT_UNKW_INT && pImpl->Type != STKOBJECT_ATTR_INT && pImpl->Type != STKOBJECT_ELEM_INT) {
		return;
	}
	if (pImpl->Value != NULL) {
		delete (int*)pImpl->Value;
	}
	pImpl->Value = new int(TmpValue);
}

void StkObject::SetFloatValue(float TmpValue)
{
	if (pImpl->Type != STKOBJECT_UNKW_FLOAT && pImpl->Type != STKOBJECT_ATTR_FLOAT && pImpl->Type != STKOBJECT_ELEM_FLOAT) {
		return;
	}
	if (pImpl->Value != NULL) {
		delete (float*)pImpl->Value;
	}
	pImpl->Value = new float(TmpValue);
}

void StkObject::SetStringValue(const wchar_t* TmpValue)
{
	if (pImpl->Type != STKOBJECT_UNKW_STRING && pImpl->Type != STKOBJECT_ATTR_STRING && pImpl->Type != STKOBJECT_ELEM_STRING) {
		return;
	}
	if (pImpl->Value != NULL) {
		delete [] (wchar_t*)pImpl->Value;
	}
	int Len = StkPlWcsLen(TmpValue) + 1;
	pImpl->Value = new wchar_t[Len];
	StkPlLStrCpy((wchar_t*)pImpl->Value, TmpValue);
}

wchar_t* StkObject::GetName()
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
	if (pImpl->Type != STKOBJECT_UNKW_INT && pImpl->Type != STKOBJECT_ATTR_INT && pImpl->Type != STKOBJECT_ELEM_INT) {
		return 0;
	}
	int *Val = (int*)pImpl->Value;
	return *Val;
}

float StkObject::GetFloatValue()
{
	if (pImpl->Type != STKOBJECT_UNKW_FLOAT && pImpl->Type != STKOBJECT_ATTR_FLOAT && pImpl->Type != STKOBJECT_ELEM_FLOAT) {
		return 0.0f;
	}
	float *Val = (float*)pImpl->Value;
	return *Val;
}

wchar_t* StkObject::GetStringValue()
{
	if (pImpl->Type != STKOBJECT_UNKW_STRING && pImpl->Type != STKOBJECT_ATTR_STRING && pImpl->Type != STKOBJECT_ELEM_STRING) {
		return NULL;
	}
	return (wchar_t*)pImpl->Value;
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
	} else if (TmpType == STKOBJECT_ELEMENT) {
		// nothing to do
	} else {
		// Abnormal case
		return;
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
	} else {
		// Abnormal case
		return;
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

void StkObject::ToXml(wchar_t* Msg, int MsgLength, int Indent)
{
	if (pImpl->Name == NULL) {
		return;
	}
	int Length = StkPlWcsLen(Msg);
	if (pImpl->Type == STKOBJECT_ATTR_INT || pImpl->Type == STKOBJECT_ATTR_FLOAT || pImpl->Type == STKOBJECT_ATTR_STRING) {
		if (pImpl->Type == STKOBJECT_ATTR_INT) {
			int *Val = (int*)pImpl->Value;
			StkPlSwPrintf(&Msg[Length], MsgLength - Length, L" %ls=\"%d\"", pImpl->Name, *Val);
		} else if (pImpl->Type == STKOBJECT_ATTR_FLOAT) {
			float *Val = (float*)pImpl->Value;
			StkPlSwPrintf(&Msg[Length], MsgLength - Length, L" %ls=\"%f\"", pImpl->Name, *Val);
		} else if (pImpl->Type == STKOBJECT_ATTR_STRING) {
			int StrLen = pImpl->XmlEncodeSize((wchar_t*)pImpl->Value);
			wchar_t* TmpStr = new wchar_t[StrLen + 1];
			pImpl->XmlEncode((wchar_t*)pImpl->Value, TmpStr, StrLen + 1);
			StkPlSwPrintf(&Msg[Length], MsgLength - Length, L" %ls=\"%ls\"", pImpl->Name, TmpStr);
			delete TmpStr;
		}
		if (pImpl->Next != NULL) {
			StkObject* TmpObj = pImpl->Next;
			TmpObj->ToXml(Msg, MsgLength, Indent);
		}
	} else if (pImpl->Type == STKOBJECT_ELEMENT) {
		for (int Loop = 0; Loop < Indent; Loop++) {
			StkPlSwPrintf(&Msg[Length], MsgLength - Length, L" ");
			Length++;
			if (Length >= MsgLength) {
				Length = MsgLength - 1;
			}
		}
		StkPlSwPrintf(&Msg[Length], MsgLength - Length, L"<%ls", pImpl->Name);
		Length += (StkPlWcsLen(pImpl->Name) + 1);
		if (Length >= MsgLength) {
			Length = MsgLength - 1;
		}
		if (pImpl->FirstAttr != NULL) {
			StkObject* TmpObj = pImpl->FirstAttr;
			TmpObj->ToXml(Msg, MsgLength, Indent);
			Length = StkPlWcsLen(Msg);
		}
		if (pImpl->FirstElem != NULL) {
			StkPlSwPrintf(&Msg[Length], MsgLength - Length, L">\r\n");
			Length += 3;
			if (Length >= MsgLength) {
				Length = MsgLength - 1;
			}
			StkObject* TmpObj = pImpl->FirstElem;
			TmpObj->ToXml(Msg, MsgLength, Indent + 2);
			Length = StkPlWcsLen(Msg);
			for (int Loop = 0; Loop < Indent; Loop++) {
				StkPlSwPrintf(&Msg[Length], MsgLength - Length, L" ");
				Length++;
				if (Length >= MsgLength) {
					Length = MsgLength - 1;
				}
			}
			StkPlSwPrintf(&Msg[Length], MsgLength - Length, L"</%ls>\r\n", pImpl->Name);
			Length += (StkPlWcsLen(pImpl->Name) + 5);
			if (Length >= MsgLength) {
				Length = MsgLength - 1;
			}
		} else {
			StkPlSwPrintf(&Msg[Length], MsgLength - Length, L"/>\r\n");
			Length += 4;
			if (Length >= MsgLength) {
				Length = MsgLength - 1;
			}
		}
		if (pImpl->Next != NULL) {
			StkObject* TmpObj = pImpl->Next;
			TmpObj->ToXml(Msg, MsgLength, Indent);
			Length = StkPlWcsLen(Msg);
		}
	} else if (pImpl->Type == STKOBJECT_ELEM_INT || pImpl->Type == STKOBJECT_ELEM_FLOAT || pImpl->Type == STKOBJECT_ELEM_STRING) {
		for (int Loop = 0; Loop < Indent; Loop++) {
			StkPlSwPrintf(&Msg[Length], MsgLength - Length, L" ");
			Length++;
			if (Length >= MsgLength) {
				Length = MsgLength - 1;
			}
		}
		if (pImpl->Type == STKOBJECT_ELEM_INT) {
			int *Val = (int*)pImpl->Value;
			StkPlSwPrintf(&Msg[Length], MsgLength - Length, L"<%ls>%d</%ls>\r\n", pImpl->Name, *Val, pImpl->Name);
		} else if (pImpl->Type == STKOBJECT_ELEM_FLOAT) {
			float *Val = (float*)pImpl->Value;
			StkPlSwPrintf(&Msg[Length], MsgLength - Length, L"<%ls>%f</%ls>\r\n", pImpl->Name, *Val, pImpl->Name);
		} else if (pImpl->Type == STKOBJECT_ELEM_STRING) {
			int StrLen = pImpl->XmlEncodeSize((wchar_t*)pImpl->Value);
			wchar_t* TmpStr = new wchar_t[StrLen + 1];
			pImpl->XmlEncode((wchar_t*)pImpl->Value, TmpStr, StrLen + 1);
			StkPlSwPrintf(&Msg[Length], MsgLength - Length, L"<%ls>%ls</%ls>\r\n", pImpl->Name, TmpStr, pImpl->Name);
			delete TmpStr;
		}
		if (pImpl->Next != NULL) {
			StkObject* TmpObj = pImpl->Next;
			TmpObj->ToXml(Msg, MsgLength, Indent);
		}
	}
	return;
}

int StkObject::ToJson(wchar_t* Msg, int MsgLength, int Indent, bool ArrayFlag)
{
	const wchar_t* EndPoint = Msg + MsgLength;
	if (pImpl->Name == NULL) {
		return 0;
	}
	int Length = 0;
	int TmpLength = 0;
	if (pImpl->Type == STKOBJECT_ATTR_INT || pImpl->Type == STKOBJECT_ATTR_FLOAT || pImpl->Type == STKOBJECT_ATTR_STRING) {
		if (pImpl->Type == STKOBJECT_ATTR_INT) {
			int *Val = (int*)pImpl->Value;
			TmpLength = StkPlSwPrintf(&Msg[Length], MsgLength - Length, L"\"%ls\" : %d", pImpl->Name, *Val);
			Length = (TmpLength == -1 ? MsgLength - 1 : Length + TmpLength);
		} else if (pImpl->Type == STKOBJECT_ATTR_FLOAT) {
			float *Val = (float*)pImpl->Value;
			TmpLength = StkPlSwPrintf(&Msg[Length], MsgLength - Length, L"\"%ls\" : %f", pImpl->Name, *Val);
			Length = (TmpLength == -1 ? MsgLength - 1 : Length + TmpLength);
		} else if (pImpl->Type == STKOBJECT_ATTR_STRING) {
			int StrLen = pImpl->JsonEncodeSize((wchar_t*)pImpl->Value);
			wchar_t* TmpStr = new wchar_t[StrLen + 1];
			pImpl->JsonEncode((wchar_t*)pImpl->Value, TmpStr, StrLen + 1);
			Length += pImpl->AddString(&Msg[Length], EndPoint, L"\"");
			Length += pImpl->AddString(&Msg[Length], EndPoint, pImpl->Name);
			Length += pImpl->AddString(&Msg[Length], EndPoint, L"\" : \"");
			Length += pImpl->AddString(&Msg[Length], EndPoint, TmpStr);
			Length += pImpl->AddString(&Msg[Length], EndPoint, L"\"");
			delete TmpStr;
		}
		if (pImpl->Next != NULL) {
			Length += pImpl->AddString(&Msg[Length], EndPoint, L", ");
			StkObject* TmpObj = pImpl->Next;
			Length += TmpObj->ToJson(&Msg[Length], MsgLength - Length, Indent);
		} else {
			Length += pImpl->AddString(&Msg[Length], EndPoint, L"\r\n");
		}
		return Length;
	} else if (pImpl->Type == STKOBJECT_ELEM_INT || pImpl->Type == STKOBJECT_ELEM_FLOAT || pImpl->Type == STKOBJECT_ELEM_STRING) {
		for (int Loop = 0; Loop < Indent; Loop++) {
			Length += pImpl->AddString(&Msg[Length], EndPoint, L" ");
		}
		if (pImpl->Type == STKOBJECT_ELEM_INT) {
			int *Val = (int*)pImpl->Value;
			if (ArrayFlag == false) {
				TmpLength = StkPlSwPrintf(&Msg[Length], MsgLength - Length, L"\"%ls\" : %d", pImpl->Name, *Val);
				Length = (TmpLength == -1 ? MsgLength - 1 : Length + TmpLength);
			} else {
				TmpLength = StkPlSwPrintf(&Msg[Length], MsgLength - Length, L"%d", *Val);
				Length = (TmpLength == -1 ? MsgLength - 1 : Length + TmpLength);
			}
		} else if (pImpl->Type == STKOBJECT_ELEM_FLOAT) {
			float *Val = (float*)pImpl->Value;
			if (ArrayFlag == false) {
				TmpLength = StkPlSwPrintf(&Msg[Length], MsgLength - Length, L"\"%ls\" : %f", pImpl->Name, *Val);
				Length = (TmpLength == -1 ? MsgLength - 1 : Length + TmpLength);
			} else {
				TmpLength = StkPlSwPrintf(&Msg[Length], MsgLength - Length, L"%f", *Val);
				Length = (TmpLength == -1 ? MsgLength - 1 : Length + TmpLength);
			}
		} else if (pImpl->Type == STKOBJECT_ELEM_STRING) {
			int StrLen = pImpl->JsonEncodeSize((wchar_t*)pImpl->Value);
			wchar_t* TmpStr = new wchar_t[StrLen + 1];
			pImpl->JsonEncode((wchar_t*)pImpl->Value, TmpStr, StrLen + 1);
			if (ArrayFlag == false) {
				Length += pImpl->AddString(&Msg[Length], EndPoint, L"\"");
				Length += pImpl->AddString(&Msg[Length], EndPoint, pImpl->Name);
				Length += pImpl->AddString(&Msg[Length], EndPoint, L"\" : \"");
				Length += pImpl->AddString(&Msg[Length], EndPoint, TmpStr);
				Length += pImpl->AddString(&Msg[Length], EndPoint, L"\"");
			} else {
				Length += pImpl->AddString(&Msg[Length], EndPoint, L"\"");
				Length += pImpl->AddString(&Msg[Length], EndPoint, TmpStr);
				Length += pImpl->AddString(&Msg[Length], EndPoint, L"\"");
			}
			delete TmpStr;
		}
		return Length;
	}

	if (pImpl->Type == STKOBJECT_ELEMENT) {
		// For this element
		for (int Loop = 0; Loop < Indent; Loop++) {
			Length += pImpl->AddString(&Msg[Length], EndPoint, L" ");
		}
		if (ArrayFlag == false) {
			if (pImpl->Type == STKOBJECT_ELEMENT || pImpl->FirstAttr != NULL) {
				if (*pImpl->Name == L'\0') {
					Length += pImpl->AddString(&Msg[Length], EndPoint, L"{\r\n");
				} else {
					Length += pImpl->AddString(&Msg[Length], EndPoint, L"\"");
					Length += pImpl->AddString(&Msg[Length], EndPoint, pImpl->Name);
					Length += pImpl->AddString(&Msg[Length], EndPoint, L"\" : {\r\n");
				}
			}
		} else {
			if (pImpl->Type == STKOBJECT_ELEMENT || pImpl->FirstAttr != NULL) {
				Length += pImpl->AddString(&Msg[Length], EndPoint, L"{\r\n");
			}
		}

		// For attribute
		if (pImpl->FirstAttr != NULL) {
			for (int Loop = 0; Loop < Indent + 2; Loop++) {
				Length += pImpl->AddString(&Msg[Length], EndPoint, L" ");
			}
			Length += pImpl->AddString(&Msg[Length], EndPoint, L"\"@attributes\" : {\r\n");
			for (int Loop = 0; Loop < Indent + 4; Loop++) {
				Length += pImpl->AddString(&Msg[Length], EndPoint, L" ");
			}
			StkObject* TmpObj = pImpl->FirstAttr;
			Length += TmpObj->ToJson(&Msg[Length], MsgLength - Length, Indent);
			for (int Loop = 0; Loop < Indent + 2; Loop++) {
				Length += pImpl->AddString(&Msg[Length], EndPoint, L" ");
			}
			if (pImpl->FirstElem == NULL) {
				Length += pImpl->AddString(&Msg[Length], EndPoint, L"}\r\n");
			} else {
				Length += pImpl->AddString(&Msg[Length], EndPoint, L"},\r\n");
			}
		}

		StkObject* TmpObj = GetFirstChildElement();
		bool ArrayMode = false;
		while (TmpObj) {
			wchar_t* CurName = TmpObj->GetName();
			StkObject* NexObj = TmpObj->GetNext();

			// ArrayMode In
			if (NexObj) {
				wchar_t* NextName = NexObj->GetName();
				if (StkPlWcsCmp(CurName, NextName) == 0 && ArrayMode == false) {
					ArrayMode = true;
					for (int Loop = 0; Loop < Indent + 2; Loop++) {
						Length += pImpl->AddString(&Msg[Length], EndPoint, L" ");
					}
					Length += pImpl->AddString(&Msg[Length], EndPoint, L"\"");
					Length += pImpl->AddString(&Msg[Length], EndPoint, CurName);
					Length += pImpl->AddString(&Msg[Length], EndPoint, L"\" : [\r\n");
				}
			}

			Length += TmpObj->ToJson(&Msg[Length], MsgLength - Length, Indent + (ArrayMode? 4 : 2), ArrayMode);
			if ((NexObj && !ArrayMode) || NexObj && ArrayMode && StkPlWcsCmp(CurName, NexObj->GetName()) == 0) {
				Length += pImpl->AddString(&Msg[Length], EndPoint, L",\r\n");
			} else {
				Length += pImpl->AddString(&Msg[Length], EndPoint, L"\r\n");
			}

			// ArrayMode Out
			if ((!NexObj && ArrayMode == true) || (NexObj && StkPlWcsCmp(CurName, NexObj->GetName()) != 0 && ArrayMode == true)) {
				ArrayMode = false;
				for (int Loop = 0; Loop < Indent + 2; Loop++) {
					Length += pImpl->AddString(&Msg[Length], EndPoint, L" ");
				}
				if (NexObj) {
					Length += pImpl->AddString(&Msg[Length], EndPoint, L"],\r\n");
				} else {
					Length += pImpl->AddString(&Msg[Length], EndPoint, L"]\r\n");
				}
			}

			TmpObj = NexObj;
		}

		for (int Loop = 0; Loop < Indent; Loop++) {
			Length += pImpl->AddString(&Msg[Length], EndPoint, L" ");
		}
		Length += pImpl->AddString(&Msg[Length], EndPoint, L"}");
		return Length;
	}
	return Length;
}

// Check the presented string whether which is composed of XML or JSON.
// Txt [in] : Checking target (null-terminated wchar_t string)
// Result : Result of the checking (-1: Invalid, 0: Empty string of NULL, 1: XML, 2: JSON)
int StkObject::Analyze(const wchar_t* Txt)
{
	static const int ELEM_UNKNOWN = 0;
	static const int ELEM_XML_START = 10;
	static const int ELEM_XML_END = 11;
	static const int ELEM_JSON_START = 20;
	static const int ELEM_JSON_END = 21;

	if (Txt == NULL || Txt[0] == wchar_t('\0')) {
		return 0;
	}
	int Status = ELEM_UNKNOWN;
	int Loop = 0;
	for (; Txt[Loop] != wchar_t('\0'); Loop++) {
		// Skip brank character
		if (Txt[Loop] == wchar_t(' ') || Txt[Loop] == wchar_t('\t') || Txt[Loop] == wchar_t('\r') || Txt[Loop] == wchar_t('\n')) {
			continue;
		}
		if (Txt[Loop] == wchar_t('<') && Status == ELEM_UNKNOWN) {
			Status = ELEM_XML_START;
			continue;
		}
		if ((Txt[Loop] == wchar_t('{') || Txt[Loop] == wchar_t('\"')) && Status == ELEM_UNKNOWN) {
			Status = ELEM_JSON_START;
			continue;
		}
		if (Status == ELEM_UNKNOWN) {
			return -1;
		}
	}
	if (Status == ELEM_UNKNOWN) {
		return 0;
	}
	for (Loop--; Loop > 0; Loop--) {
		// Skip brank character
		if (Txt[Loop] == wchar_t(' ') || Txt[Loop] == wchar_t('\t') || Txt[Loop] == wchar_t('\r') || Txt[Loop] == wchar_t('\n')) {
			continue;
		}
		if (Txt[Loop] == wchar_t('>') && Status == ELEM_XML_START) {
			Status = ELEM_XML_END;
			int ErrorCode = 0;
			StkObject* TmpObj = CreateObjectFromXml(Txt, &ErrorCode);
			if (TmpObj == NULL) {
				return -1;
			}
			delete TmpObj;
			return 1;
		}
		if (Txt[Loop] == wchar_t('}') && Status == ELEM_JSON_START) {
			Status = ELEM_JSON_END;
			int ErrorCode = 0;
			StkObject* TmpObj = CreateObjectFromJson(Txt, &ErrorCode);
			if (TmpObj == NULL) {
				return -1;
			}
			delete TmpObj;
			return 2;
		}
		return -1;
	}
	return -1;
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
StkObject* StkObject::CreateObjectFromJson(const wchar_t* Json, int* Offset, StkObject* Parent)
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
	wchar_t* PrevName = NULL;
	int PrevStatus = ELEM_START;
	bool ArrayFlag = false;

	if (Json == NULL || StkPlWcsCmp(Json, L"") == 0) {
		Impl::CleanupObjectsForJson(PrevName, RetObj);
		*Offset = ERROR_JSON_NO_ELEMENT_FOUND;
		return NULL;
	}

	int Loop = 0;

	if (Parent == NULL) {
		for (; Json[Loop] != wchar_t('\0'); Loop++) {
			// if blank is appeared...
			if (Json[Loop] == wchar_t(' ') || Json[Loop] == wchar_t('\t') || Json[Loop] == wchar_t('\r') || Json[Loop] == wchar_t('\n')) {
				continue;
			} else if (Json[Loop] == wchar_t('{')) {
				PrevName = new wchar_t[1];
				StkPlLStrCpy(PrevName, L"");
				PrevStatus = ELEMOBJ_START;
				break;
			} else {
				Loop = 0;
				break;
			}
		}
	}

	for (; Json[Loop] != wchar_t('\0'); Loop++) {

		// If ArrayFlag is true, mode needs to be changed.
		if (ArrayFlag == true && (PrevStatus == ARRAY_START || PrevStatus == ELEM_START)) {
			PrevStatus = ELEMOBJ_START;
		}

		// if blank is appeared...
		if ((Json[Loop] == wchar_t(' ') || Json[Loop] == wchar_t('\t') || Json[Loop] == wchar_t('\r') || Json[Loop] == wchar_t('\n')) && PrevStatus != ELEMNAME_START && PrevStatus != STRVAL_START) {
			continue;
		}

		// if \" is appeared...
		if (Json[Loop] == wchar_t('\"')) {
			if (PrevStatus == ELEM_START || PrevStatus == ARRAY_START) {
				PrevStatus = ELEMNAME_START;
				continue;
			} else if (PrevStatus == ELEMOBJ_START) {
				PrevStatus = STRVAL_START;
				continue;
			} else if (PrevStatus == STRVAL_START) {
			} else {
				Impl::CleanupObjectsForJson(PrevName, RetObj);
				*Offset = ERROR_JSON_INVALID_QUOT_FOUND;
				return NULL;
			}
		}

		// if : is appeared...
		if (Json[Loop] == wchar_t(':')) {
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
		if (Json[Loop] == wchar_t('{')) {
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
				if (ArrayFlag == false) {
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
		if (Json[Loop] == wchar_t('}')) {
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
		if (Json[Loop] == wchar_t('[')) {
			if (PrevStatus == ELEMOBJ_START) {
				PrevStatus = ARRAY_START;
				ArrayFlag = true;
				continue;
			} else {
				Impl::CleanupObjectsForJson(PrevName, RetObj);
				*Offset = ERROR_JSON_INVALID_ARRAY_STRUCTURE;
				return NULL;
			}
		}

		// if ] is appeared...
		if (Json[Loop] == wchar_t(']')) {
			if (PrevStatus == ELEMOBJ_END) {
				PrevStatus = ARRAY_END;
				ArrayFlag = false;
				delete PrevName;
				PrevName = NULL;
				continue;
			} else {
				Impl::CleanupObjectsForJson(PrevName, RetObj);
				*Offset = ERROR_JSON_INVALID_ARRAY_STRUCTURE;
				return NULL;
			}
		}

		// if , is appeared...
		if (Json[Loop] == wchar_t(',')) {
			if (PrevStatus == ELEMOBJ_END || PrevStatus == ARRAY_END) {
				PrevStatus = ELEM_START;
				continue;
			} else {
				Impl::CleanupObjectsForJson(PrevName, RetObj);
				*Offset = ERROR_JSON_INVALID_COMMA;
				return NULL;
			}
		}

		// if decimal number is appeared...
		if ((Json[Loop] >= wchar_t('0') && Json[Loop] <= wchar_t('9')) || Json[Loop] == wchar_t('-')) {
			int ValInt = 0;
			float ValFloat = 0;
			if (PrevStatus == ELEMOBJ_START) {
				int StrLen = 0;
				bool IsFloat = Impl::GetJsonNumber(&Json[Loop], &StrLen, &ValInt, &ValFloat);
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
				if (ArrayFlag == false) {
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
			wchar_t* Value = Impl::GetJsonString(&Json[Loop], &StrLen);
			if (RetObj == NULL) {
				delete Value;
				Impl::CleanupObjectsForJson(PrevName, RetObj);
				*Offset = ERROR_JSON_NO_ROOT_ELEMENT;
				return NULL;
			}
			RetObj->AppendChildElement(new StkObject(PrevName, Value));
			delete Value;
			if (ArrayFlag == false) {
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
StkObject* StkObject::CreateObjectFromXml(const wchar_t* Xml, int* Offset)
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
	wchar_t* PrevAttrName = NULL;
	int PrevStatus = ELEM_START;

	if (Xml == NULL || StkPlWcsCmp(Xml, L"") == 0) {
		Impl::CleanupObjectsForXml(PrevAttrName, RetObj);
		*Offset = ERROR_XML_NO_ELEMENT_FOUND;
		return NULL;
	}

	for (int Loop = 0; Xml[Loop] != wchar_t('\0'); Loop++) {

		// if blank is appeared...
		if ((Xml[Loop] == wchar_t(' ') || Xml[Loop] == wchar_t('\t') || Xml[Loop] == wchar_t('\r') || Xml[Loop] == wchar_t('\n')) && PrevStatus != ATTRVAL_START) {
			continue;
		}

		// if '<' is appeared...
		if (Xml[Loop] == wchar_t('<')) {
			if (PrevStatus == ELEM_START) {
				PrevStatus = ELEMNAME_START;
				continue;
			} else if (PrevStatus == ELEM_DOWN) {
				for (int Loop2 = Loop + 1; Xml[Loop2] != char('\0'); Loop2++) {
					if (Xml[Loop2] == wchar_t(' ') || Xml[Loop2] == wchar_t('\t') || Xml[Loop2] == wchar_t('\r') || Xml[Loop2] == wchar_t('\n')) {
						continue;
					} else if (Xml[Loop2] == wchar_t('/')) {
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
		if (Xml[Loop] == wchar_t('=') && PrevStatus != ELEM_DOWN) {
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
		if (Xml[Loop] == wchar_t('\"')) {
			if (PrevStatus == ATTR_EQUAL) {
				PrevStatus = ATTRVAL_START;
				continue;
			} else if (PrevStatus == ATTRVAL_START) {
				StkObject* AttrObj = new StkObject(PrevAttrName, L"");
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
		if (Xml[Loop] == wchar_t('>')) {
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
		if ((Xml[Loop] == wchar_t('?') || Xml[Loop] == wchar_t('!')) && PrevStatus != ELEM_DOWN) {
			if (PrevStatus == ELEMNAME_START) {
				PrevStatus = ELEM_START;
				for (; Xml[Loop] != wchar_t('\0'); Loop++) {
					if (Xml[Loop] == wchar_t('>')) {
						break;
					}
				}
				if (Xml[Loop] == wchar_t('\0')) {
					Impl::CleanupObjectsForXml(PrevAttrName, RetObj);
					*Offset = ERROR_XML_ELEMENT_END_NOT_FOUND;
					return NULL;
				}
				continue;
			}
		}

		// if '/' is appeared...
		if (Xml[Loop] == wchar_t('/') && PrevStatus != ELEM_DOWN) {
			if (PrevStatus == ELEMNAME_START || PrevStatus == ELEMNAME_END || PrevStatus == ATTRVAL_END) {
				PrevStatus = ELEM_END;
			} else {
				Impl::CleanupObjectsForXml(PrevAttrName, RetObj);
				*Offset = ERROR_XML_INVALID_SLASH_FOUND;
				return NULL;
			}
			for (int Loop2 = Loop + 1; Xml[Loop2] != wchar_t('\0'); Loop2++) {
				if (Xml[Loop2] == wchar_t('>')) {
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
			wchar_t* Name = Impl::GetName(&Xml[Loop], &StrLen);
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
			wchar_t* Value = Impl::GetValue(&Xml[Loop], &StrLen);
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
			wchar_t* Value = Impl::GetValue(&Xml[Loop], &StrLen);
			Loop = Loop + StrLen;
			if (RetObj != NULL) {
				RetObj->SetType(StkObject::STKOBJECT_UNKW_STRING);
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
