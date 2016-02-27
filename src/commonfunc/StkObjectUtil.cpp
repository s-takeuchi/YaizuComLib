#include <windows.h>
#include <tchar.h>
#include <shlwapi.h>
#include "StkObjectUtil.h"
#include "StkObject.h"

class StkObjectUtil::Impl
{
public:
	TCHAR* GetJsonString(TCHAR*, int*);
	BOOL GetJsonNumber(TCHAR*, int*, int*, float*);
	TCHAR* GetName(TCHAR*, int*);
	TCHAR* GetValue(TCHAR*, int*);
	void CleanupObjectsForXml(TCHAR*, StkObject*);
	void CleanupObjectsForJson(TCHAR*, StkObject*);
};

void StkObjectUtil::Impl::CleanupObjectsForXml(TCHAR* PrevAttrName, StkObject* RetObj)
{
	if (PrevAttrName != NULL) {
		delete PrevAttrName;
	}
	if (RetObj != NULL) {
		delete RetObj;
	}
}

void StkObjectUtil::Impl::CleanupObjectsForJson(TCHAR* PrevName, StkObject* RetObj)
{
	if (PrevName != NULL) {
		delete PrevName;
	}
	if (RetObj != NULL) {
		delete RetObj;
	}
}

BOOL StkObjectUtil::Impl::GetJsonNumber(TCHAR* OrgStr, int* Len, int* ValInt, float* ValFloat)
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

TCHAR* StkObjectUtil::Impl::GetJsonString(TCHAR* OrgStr, int* Len)
{
	int OutLength = 0;
	TCHAR* CurPnt = OrgStr;
	while (*CurPnt != TCHAR('\"') && CurPnt != _T('\0')) {
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

TCHAR* StkObjectUtil::Impl::GetName(TCHAR* TgtName, int* Len)
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

TCHAR* StkObjectUtil::Impl::GetValue(TCHAR* TgtValue, int* Len)
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

StkObjectUtil::StkObjectUtil()
{
	pImpl = new Impl;
}

StkObjectUtil::~StkObjectUtil()
{
	delete pImpl;
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
StkObject* StkObjectUtil::CreateObjectFromJson(TCHAR* Json, int* Offset, StkObject* Parent)
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
		pImpl->CleanupObjectsForJson(PrevName, RetObj);
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
				pImpl->CleanupObjectsForJson(PrevName, RetObj);
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
				pImpl->CleanupObjectsForJson(PrevName, RetObj);
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
					pImpl->CleanupObjectsForJson(PrevName, RetObj);
					*Offset = OffsetCld;
					return NULL;
				}
				if (ArrayFlag == FALSE) {
					delete PrevName;
					PrevName = NULL;
				}
				continue;
			} else {
				pImpl->CleanupObjectsForJson(PrevName, RetObj);
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
				pImpl->CleanupObjectsForJson(PrevName, RetObj);
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
				pImpl->CleanupObjectsForJson(PrevName, RetObj);
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
				pImpl->CleanupObjectsForJson(PrevName, RetObj);
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
				pImpl->CleanupObjectsForJson(PrevName, RetObj);
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
				BOOL IsFloat = pImpl->GetJsonNumber(&Json[Loop], &StrLen, &ValInt, &ValFloat);
				StkObject* ChildObj;
				if (IsFloat) {
					ChildObj = new StkObject(PrevName, ValFloat);
				} else {
					ChildObj = new StkObject(PrevName, ValInt);
				}
				if (RetObj == NULL) {
					pImpl->CleanupObjectsForJson(PrevName, RetObj);
					*Offset = ERROR_JSON_NO_ROOT_ELEMENT;
					return NULL;
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
			PrevName = pImpl->GetJsonString(&Json[Loop], &StrLen);
			Loop = Loop + StrLen;
			PrevStatus = ELEMNAME_END;
			continue;
		}
		if (PrevStatus == STRVAL_START) {
			int StrLen = 0;
			TCHAR* Value = pImpl->GetJsonString(&Json[Loop], &StrLen);
			if (RetObj == NULL) {
				pImpl->CleanupObjectsForJson(PrevName, RetObj);
				*Offset = ERROR_JSON_NO_ROOT_ELEMENT;
				return NULL;
			}
			RetObj->AppendChildElement(new StkObject(PrevName, Value));
			if (ArrayFlag == FALSE) {
				delete PrevName;
				PrevName = NULL;
			}
			delete Value;
			Loop = Loop + StrLen;
			PrevStatus = ELEMOBJ_END;
			continue;
		}
		pImpl->CleanupObjectsForJson(PrevName, RetObj);
		*Offset = ERROR_JSON_CANNOT_HANDLE;
		return NULL;
	}

	if (PrevStatus != ELEMOBJ_END) {
		pImpl->CleanupObjectsForJson(PrevName, RetObj);
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
StkObject* StkObjectUtil::CreateObjectFromXml(TCHAR* Xml, int* Offset)
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
		pImpl->CleanupObjectsForXml(PrevAttrName, RetObj);
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
					pImpl->CleanupObjectsForXml(PrevAttrName, RetObj);
					*Offset = OffsetCld;
					return NULL;
				}
				continue;
			} else {
				pImpl->CleanupObjectsForXml(PrevAttrName, RetObj);
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
				pImpl->CleanupObjectsForXml(PrevAttrName, RetObj);
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
				pImpl->CleanupObjectsForXml(PrevAttrName, RetObj);
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
				pImpl->CleanupObjectsForXml(PrevAttrName, RetObj);
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
					pImpl->CleanupObjectsForXml(PrevAttrName, RetObj);
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
				pImpl->CleanupObjectsForXml(PrevAttrName, RetObj);
				*Offset = ERROR_XML_INVALID_SLASH_FOUND;
				return NULL;
			}
			for (int Loop2 = Loop + 1; Xml[Loop2] != TCHAR('\0'); Loop2++) {
				if (Xml[Loop2] == TCHAR('>')) {
					if (RetObj == NULL) {
						pImpl->CleanupObjectsForXml(PrevAttrName, RetObj);
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
			pImpl->CleanupObjectsForXml(PrevAttrName, RetObj);
			*Offset = ERROR_XML_SLASH_FOUND_WITHOUT_ELEMENT_END;
			return NULL;
		}

		// if other...
		if (PrevStatus == ELEMNAME_START) {
			int StrLen = 0;
			TCHAR* Name = pImpl->GetName(&Xml[Loop], &StrLen);
			RetObj = new StkObject(Name);
			delete Name;
			Loop = Loop + StrLen - 1;
			PrevStatus = ELEMNAME_END;
			continue;
		}
		if (PrevStatus == ELEMNAME_END || PrevStatus == ATTRVAL_END) {
			int StrLen = 0;
			PrevAttrName = pImpl->GetName(&Xml[Loop], &StrLen);
			Loop = Loop + StrLen - 1;
			PrevStatus = ATTRNAME_END;
			continue;
		}
		if (PrevStatus == ATTRVAL_START) {
			int StrLen = 0;
			TCHAR* Value = pImpl->GetValue(&Xml[Loop], &StrLen);
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
			TCHAR* Value = pImpl->GetValue(&Xml[Loop], &StrLen);
			Loop = Loop + StrLen;
			if (RetObj != NULL) {
				RetObj->SetType(StkObject::STKOBJECT_ELEM_STRING);
				RetObj->SetStringValue(Value);
			}
			delete Value;
			continue;
		}
		pImpl->CleanupObjectsForXml(PrevAttrName, RetObj);
		*Offset = ERROR_XML_CANNOT_HANDLE;
		return NULL;
	}

	pImpl->CleanupObjectsForXml(PrevAttrName, RetObj);
	*Offset = ERROR_XML_ELEMENT_END_NOT_FOUND;
	return NULL;
}
