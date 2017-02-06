#include <windows.h>
#include <tchar.h>
#include <shlwapi.h>
#include "StkStringParser.h"

int StkStringParser::ParseInto1Param(TCHAR* OriginStr, TCHAR* Format, TCHAR Target, TCHAR* OutStr1)
{
	return ParseInto4Params(OriginStr, Format, Target, OutStr1, NULL, NULL, NULL);
}

int StkStringParser::ParseInto2Params(TCHAR* OriginStr, TCHAR* Format, TCHAR Target, TCHAR* OutStr1, TCHAR* OutStr2)
{
	return ParseInto4Params(OriginStr, Format, Target, OutStr1, OutStr2, NULL, NULL);
}

int StkStringParser::ParseInto3Params(TCHAR* OriginStr, TCHAR* Format, TCHAR Target, TCHAR* OutStr1, TCHAR* OutStr2, TCHAR* OutStr3)
{
	return ParseInto4Params(OriginStr, Format, Target, OutStr1, OutStr2, OutStr3, NULL);
}

// OutputStr1 [out] : 1st Parameter acquired (512 bytes as max)
// OutputStr2 [out] : 2nd Parameter acquired (512 bytes as max)
// OutputStr3 [out] : 3rd Parameter acquired (512 bytes as max)
// OutputStr4 [out] : 4th Parameter acquired (512 bytes as max)
// Return: Result code (0: Ended normaly, -1: Invalid OriginStrWk or FormatWk was presented)
int StkStringParser::ParseInto4Params(TCHAR* OriginStr, TCHAR* Format, TCHAR Target, TCHAR* OutStr1, TCHAR* OutStr2, TCHAR* OutStr3, TCHAR* OutStr4)
{
	// Check the presented input parameters
	if (OriginStr == NULL || *OriginStr == '\0' ||
		Format == NULL || *Format == '\0') {
		return -1;
	}
	int FormatLen = lstrlen(Format);
	int OriginStrLen = lstrlen(OriginStr);
	if (FormatLen <= 0 || FormatLen > OriginStrLen) {
		return -1;
	}

	// Make work variables and configure them
	TCHAR* OriginStrWk = new TCHAR[OriginStrLen + 3];
	TCHAR* FormatWk = new TCHAR[FormatLen + 3];
	int Offset = 0;
	if (Format[0] == Target) {
		Offset++;
		FormatWk[0] = (Target != _T('#'))? _T('#') : _T('$');
		OriginStrWk[0] = (Target != _T('#'))? _T('#') : _T('$');
	}
	lstrcpy(&OriginStrWk[Offset], OriginStr);
	lstrcpy(&FormatWk[Offset], Format);
	if (Format[FormatLen - 1] == Target) {
		Offset++;
		FormatWk[FormatLen - 1 + Offset] = (Target != _T('#'))? _T('#') : _T('$');
		OriginStrWk[OriginStrLen - 1 + Offset] = (Target != _T('#'))? _T('#') : _T('$');
		FormatWk[FormatLen + Offset] = _T('\0');
		OriginStrWk[OriginStrLen + Offset] = _T('\0');
	}

	if (OutStr1 != NULL) {
		lstrcpy(OutStr1, _T(""));
	}
	if (OutStr2 != NULL) {
		lstrcpy(OutStr2, _T(""));
	}
	if (OutStr3 != NULL) {
		lstrcpy(OutStr3, _T(""));
	}
	if (OutStr4 != NULL) {
		lstrcpy(OutStr4, _T(""));
	}

	// Configure SearchPtr and SearchEndPtr
	TCHAR* SearchPtr[5] = {NULL, NULL, NULL, NULL, NULL};
	TCHAR* SearchEndPtr[5] = {NULL, NULL, NULL, NULL, NULL};
	int SearchPtrIndex = 0;
	SearchPtr[SearchPtrIndex] = FormatWk;
	SearchPtrIndex++;
	int Loop;
	for (Loop = 0; Loop < lstrlen(FormatWk); Loop++) {
		if (FormatWk[Loop] == Target) {
			SearchPtr[SearchPtrIndex] = &FormatWk[Loop] + 1;
			SearchEndPtr[SearchPtrIndex - 1] = &FormatWk[Loop];
			SearchPtrIndex++;
			if (SearchPtrIndex == 6) {
				break;
			}
		}
	}
	SearchEndPtr[SearchPtrIndex - 1] = &FormatWk[Loop];

	// Make SearchString from SearchPtr and SearchEndPtr
	TCHAR SearchString[5][1024] = {_T(""), _T(""), _T(""), _T(""), _T("")};
	for (Loop = 0; Loop < 5; Loop++) {
		if (SearchPtr[Loop] != NULL && SearchEndPtr[Loop] != NULL) {
			lstrcpyn(SearchString[Loop], SearchPtr[Loop], SearchEndPtr[Loop] - SearchPtr[Loop] + 1);
		}
	}

	// Make OutStrs
	TCHAR* OutputBegin[4];
	TCHAR* OutputEnd[4];
	TCHAR* CurrStr;
	for (Loop = 0; Loop < 4; Loop++) {
		if (Loop == 0) {
			CurrStr = OriginStrWk;
		} else {
			CurrStr = OutputEnd[Loop - 1];
		}
		if (CurrStr != NULL) {
			OutputBegin[Loop] = StrStr(CurrStr, SearchString[Loop]);
			OutputEnd[Loop] = StrStr(CurrStr + 1, SearchString[Loop + 1]);
			if (OutputBegin[Loop] != NULL) {
				OutputBegin[Loop] = StrStr(CurrStr, SearchString[Loop]) + lstrlen(SearchString[Loop]);
			}
		} else {
			OutputBegin[Loop] = NULL;
			OutputEnd[Loop] = NULL;
		}
		if (Loop == 0 && OutStr1 != NULL && OutputEnd[0] != NULL) {
			lstrcpyn(OutStr1, OutputBegin[0], OutputEnd[0] - OutputBegin[0] + 1);
		}
		if (Loop == 1 && OutStr2 != NULL && OutputEnd[1] != NULL) {
			lstrcpyn(OutStr2, OutputBegin[1], OutputEnd[1] - OutputBegin[1] + 1);
		}
		if (Loop == 2 && OutStr3 != NULL && OutputEnd[2] != NULL) {
			lstrcpyn(OutStr3, OutputBegin[2], OutputEnd[2] - OutputBegin[2] + 1);
		}
		if (Loop == 3 && OutStr4 != NULL && OutputEnd[3] != NULL) {
			lstrcpyn(OutStr4, OutputBegin[3], OutputEnd[3] - OutputBegin[3] + 1);
		}
	}
	delete OriginStrWk;
	delete FormatWk;
	return 0;
}
