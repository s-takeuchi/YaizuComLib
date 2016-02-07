#include <windows.h>
#include <tchar.h>
#include "StkObjectUtil.h"
#include "StkObject.h"

TCHAR* StkObjectUtil::GetName(TCHAR* TgtName, int* Len)
{
	TCHAR* CurPnt = TgtName;
	int NameLength = 0;
	while (*CurPnt != TCHAR(' ') && *CurPnt != TCHAR('\t') && *CurPnt != TCHAR('\r') && *CurPnt != TCHAR('\n') && *CurPnt != TCHAR('>') && *CurPnt != TCHAR('/') && *CurPnt != TCHAR('=')) {
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
	while (*CurPnt != TCHAR('\"') && *CurPnt != TCHAR('<')) {
		ValueLength++;
		CurPnt++;
	}
	*Len = ValueLength;
	TCHAR* RtnValue = new TCHAR[ValueLength + 1];
	for (int Loop = 0; Loop < ValueLength; Loop++) {
		RtnValue[Loop] = TgtValue[Loop];
	}
	RtnValue[ValueLength] = TCHAR('\0');
	return RtnValue;
}

//   < Aaa Xxx = " Xxxxxxxxx"  Yyy = " Yyyyyyyyy" >   < / Aaa>
//  | |   |   | | |          |    | | |          |  |  | |
//  | |   |   | | |          |    | | |          |  |  | ELEM_END
//  | |   |   | | |          |    | | |          |  |  ELEMNAME_START
//  | |   |   | | |          |    | | |          |  ELEM_DOWN
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
					if (Xml[Loop2] == TCHAR(' ') || Xml[Loop2] == TCHAR('\t')) {
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
				}
				continue;
			} else {
				*Offset = 0;
				return RetObj;
			}
		}

		// if '=' is appeared...
		if (Xml[Loop] == TCHAR('=')) {
			if (PrevStatus == ATTRNAME_END) {
				PrevStatus = ATTR_EQUAL;
				continue;
			} else {
				*Offset = 0;
				return RetObj;
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
				PrevStatus = ATTRVAL_END;
				continue;
			} else {
				*Offset = 0;
				return RetObj;
			}
		}

		// if '>' is appeared...
		if (Xml[Loop] == TCHAR('>')) {
			if (PrevStatus == ATTRVAL_END || PrevStatus == ELEMNAME_END) {
				PrevStatus = ELEM_DOWN;
				continue;
			} else {
				*Offset = 0;
				return RetObj;
			}
		}

		// if '/' is appeared...
		if (Xml[Loop] == TCHAR('/')) {
			if (PrevStatus == ELEMNAME_START || PrevStatus == ELEMNAME_END || PrevStatus == ATTRVAL_END) {
				PrevStatus = ELEM_END;
			}
			for (int Loop2 = Loop + 1; Xml[Loop2] != TCHAR('\0'); Loop2++) {
				if (Xml[Loop2] == TCHAR('>')) {
					*Offset = Loop2 + 1;
					return RetObj;
				}
			}
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
	}

	return NULL;
}
