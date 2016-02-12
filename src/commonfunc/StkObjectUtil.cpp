#include <windows.h>
#include <tchar.h>
#include <shlwapi.h>
#include "StkObjectUtil.h"
#include "StkObject.h"

void StkObjectUtil::CleanupObjects(TCHAR* PrevAttrName, StkObject* RetObj)
{
	if (PrevAttrName != NULL) {
		delete PrevAttrName;
	}
	if (RetObj != NULL) {
		delete RetObj;
	}
}

TCHAR* StkObjectUtil::GetName(TCHAR* TgtName, int* Len)
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

TCHAR* StkObjectUtil::GetValue(TCHAR* TgtValue, int* Len)
{
	TCHAR* CurPnt = TgtValue;
	int ValueLength = 0;
	while (*CurPnt != TCHAR('\"') && *CurPnt != TCHAR('<') && *CurPnt != TCHAR('\0')) {
		if (*CurPnt == TCHAR('&')) {
			if (*(CurPnt + 1) != TCHAR('\0') && *(CurPnt + 2) != TCHAR('\0') && *(CurPnt + 3) != TCHAR('\0') && StrStr(CurPnt, _T("&lt;")) == CurPnt) {
				CurPnt += 4;
			}
			if (*(CurPnt + 1) != TCHAR('\0') && *(CurPnt + 2) != TCHAR('\0') && *(CurPnt + 3) != TCHAR('\0') && StrStr(CurPnt, _T("&gt;")) == CurPnt) {
				CurPnt += 4;
			}
			if (*(CurPnt + 1) != TCHAR('\0') && *(CurPnt + 2) != TCHAR('\0') && *(CurPnt + 3) != TCHAR('\0') && *(CurPnt + 4) != TCHAR('\0') && StrStr(CurPnt, _T("&amp;")) == CurPnt) {
				CurPnt += 5;
			}
			if (*(CurPnt + 1) != TCHAR('\0') && *(CurPnt + 2) != TCHAR('\0') && *(CurPnt + 3) != TCHAR('\0') && *(CurPnt + 4) != TCHAR('\0') && *(CurPnt + 5) != TCHAR('\0') && StrStr(CurPnt, _T("&quot;")) == CurPnt) {
				CurPnt += 6;
			}
			if (*(CurPnt + 1) != TCHAR('\0') && *(CurPnt + 2) != TCHAR('\0') && *(CurPnt + 3) != TCHAR('\0') && *(CurPnt + 4) != TCHAR('\0') && *(CurPnt + 5) != TCHAR('\0') && StrStr(CurPnt, _T("&apos;")) == CurPnt) {
				CurPnt += 6;
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
		CleanupObjects(PrevAttrName, RetObj);
		*Offset = ERROR_NO_ELEMENT_FOUND;
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
					CleanupObjects(PrevAttrName, RetObj);
					*Offset = OffsetCld;
					return NULL;
				}
				continue;
			} else {
				CleanupObjects(PrevAttrName, RetObj);
				*Offset = ERROR_INVALID_ELEMENT_START_FOUND;
				return NULL;
			}
		}

		// if '=' is appeared...
		if (Xml[Loop] == TCHAR('=') && PrevStatus != ELEM_DOWN) {
			if (PrevStatus == ATTRNAME_END) {
				PrevStatus = ATTR_EQUAL;
				continue;
			} else {
				CleanupObjects(PrevAttrName, RetObj);
				*Offset = ERROR_EQUAL_FOUND_WITHOUT_ATTRIBUTE_NAME;
				return NULL;
			}
		}

		// if '\"' is appeared...
		if (Xml[Loop] == TCHAR('\"')) {
			if (PrevStatus == ATTR_EQUAL) {
				PrevStatus = ATTRVAL_START;
				continue;
			} else if (PrevStatus == ATTRVAL_START) {
				StkObject* AttrObj = new StkObject(StkObject::STKOBJECT_ATTRIBUTE, PrevAttrName, _T(""));
				RetObj->AppendAttribute(AttrObj);
				delete PrevAttrName;
				PrevAttrName = NULL;
				PrevStatus = ATTRVAL_END;
				continue;
			} else {
				CleanupObjects(PrevAttrName, RetObj);
				*Offset = ERROR_INVALID_QUOT_FOUND;
				return NULL;
			}
		}

		// if '>' is appeared...
		if (Xml[Loop] == TCHAR('>')) {
			if (PrevStatus == ATTRVAL_END || PrevStatus == ELEMNAME_END) {
				PrevStatus = ELEM_DOWN;
				continue;
			} else {
				CleanupObjects(PrevAttrName, RetObj);
				*Offset = ERROR_INVALID_ELEMENT_END_FOUND;
				return NULL;
			}
		}

		// if '?' is appeared...
		if (Xml[Loop] == TCHAR('?') && PrevStatus != ELEM_DOWN) {
			if (PrevStatus == ELEMNAME_START) {
				PrevStatus = ELEM_START;
				for (; Xml[Loop] != TCHAR('\0'); Loop++) {
					if (Xml[Loop] == TCHAR('>')) {
						break;
					}
				}
				if (Xml[Loop] == TCHAR('\0')) {
					CleanupObjects(PrevAttrName, RetObj);
					*Offset = ERROR_ELEMENT_END_NOT_FOUND;
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
				CleanupObjects(PrevAttrName, RetObj);
				*Offset = ERROR_INVALID_SLASH_FOUND;
				return NULL;
			}
			for (int Loop2 = Loop + 1; Xml[Loop2] != TCHAR('\0'); Loop2++) {
				if (Xml[Loop2] == TCHAR('>')) {
					if (RetObj == NULL) {
						CleanupObjects(PrevAttrName, RetObj);
						*Offset = ERROR_SLASH_FOUND_WITHOUT_ELEMENT;
						return NULL;
					}
					*Offset = Loop2 + 1;
					if (PrevAttrName != NULL) {
						delete PrevAttrName;
					}
					return RetObj;
				}
			}
			CleanupObjects(PrevAttrName, RetObj);
			*Offset = ERROR_SLASH_FOUND_WITHOUT_ELEMENT_END;
			return NULL;
		}

		// if other...
		if (PrevStatus == ELEMNAME_START) {
			int StrLen = 0;
			TCHAR* Name = GetName(&Xml[Loop], &StrLen);
			RetObj = new StkObject(Name);
			delete Name;
			Loop = Loop + StrLen - 1;
			PrevStatus = ELEMNAME_END;
			continue;
		}
		if (PrevStatus == ELEMNAME_END || PrevStatus == ATTRVAL_END) {
			int StrLen = 0;
			PrevAttrName = GetName(&Xml[Loop], &StrLen);
			Loop = Loop + StrLen - 1;
			PrevStatus = ATTRNAME_END;
			continue;
		}
		if (PrevStatus == ATTRVAL_START) {
			int StrLen = 0;
			TCHAR* Value = GetValue(&Xml[Loop], &StrLen);
			StkObject* AttrObj = new StkObject(StkObject::STKOBJECT_ATTRIBUTE, PrevAttrName, Value);
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
			TCHAR* Value = GetValue(&Xml[Loop], &StrLen);
			Loop = Loop + StrLen;
			if (RetObj != NULL) {
				RetObj->SetType(StkObject::STKOBJECT_ELEM_STRING);
				RetObj->SetStringValue(Value);
			}
			delete Value;
			continue;
		}
		CleanupObjects(PrevAttrName, RetObj);
		*Offset = ERROR_CANNOT_HANDLE;
		return NULL;
	}

	CleanupObjects(PrevAttrName, RetObj);
	*Offset = ERROR_ELEMENT_END_NOT_FOUND;
	return NULL;
}
