#include "../stkpl/StkPl.h"
#include "StkStringParser.h"

int StkStringParser::ParseInto1Param(const wchar_t* OriginStr, const wchar_t* Format, wchar_t Target, wchar_t* OutStr1)
{
	return ParseInto4Params(OriginStr, Format, Target, OutStr1, -1, NULL, -1, NULL, -1, NULL, -1);
}

int StkStringParser::ParseInto1Param(const wchar_t* OriginStr, const wchar_t* Format, wchar_t Target, wchar_t* OutStr1, int OutStr1Len)
{
	return ParseInto4Params(OriginStr, Format, Target, OutStr1, OutStr1Len, NULL, -1, NULL, -1, NULL, -1);
}

int StkStringParser::ParseInto2Params(const wchar_t* OriginStr, const wchar_t* Format, wchar_t Target, wchar_t* OutStr1, wchar_t* OutStr2)
{
	return ParseInto4Params(OriginStr, Format, Target, OutStr1, -1, OutStr2, -1, NULL, -1, NULL, -1);
}

int StkStringParser::ParseInto2Params(const wchar_t* OriginStr, const wchar_t* Format, wchar_t Target, wchar_t* OutStr1, int OutStr1Len, wchar_t* OutStr2, int OutStr2Len)
{
	return ParseInto4Params(OriginStr, Format, Target, OutStr1, OutStr1Len, OutStr2, OutStr2Len, NULL, -1, NULL, -1);
}

int StkStringParser::ParseInto3Params(const wchar_t* OriginStr, const wchar_t* Format, wchar_t Target, wchar_t* OutStr1, wchar_t* OutStr2, wchar_t* OutStr3)
{
	return ParseInto4Params(OriginStr, Format, Target, OutStr1, -1, OutStr2, -1, OutStr3, -1, NULL, -1);
}

int StkStringParser::ParseInto3Params(const wchar_t* OriginStr, const wchar_t* Format, wchar_t Target, wchar_t* OutStr1, int OutStr1Len, wchar_t* OutStr2, int OutStr2Len, wchar_t* OutStr3, int OutStr3Len)
{
	return ParseInto4Params(OriginStr, Format, Target, OutStr1, OutStr1Len, OutStr2, OutStr2Len, OutStr3, OutStr3Len, NULL, -1);
}

int StkStringParser::ParseInto4Params(const wchar_t* OriginStr, const wchar_t* Format, wchar_t Target, wchar_t* OutStr1, wchar_t* OutStr2, wchar_t* OutStr3, wchar_t* OutStr4)
{
	return ParseInto4Params(OriginStr, Format, Target, OutStr1, -1, OutStr2, -1, OutStr3, -1, OutStr4, -1);
}

// OutStr1   [out] : 1st Parameter to be acquired
// OutStr1Len [in] : Size of OutStr1. If -1 is presented, it is regarded as infinite length.
// OutStr2   [out] : 2nd Parameter to be acquired
// OutStr2Len [in] : Size of OutStr2. If -1 is presented, it is regarded as infinite length.
// OutStr3   [out] : 3rd Parameter to be acquired
// OutStr3Len [in] : Size of OutStr3. If -1 is presented, it is regarded as infinite length.
// OutStr4   [out] : 4th Parameter to be acquired
// OutStr4Len [in] : Size of OutStr4. If -1 is presented, it is regarded as infinite length.
// Return: Result code (0: Number of parameters to be set differs from the number of targets,
//                      1: Number of parameters to be set is same as the number of targets,
//                     -1: Invalid OriginStrWk or FormatWk was presented)
int StkStringParser::ParseInto4Params(const wchar_t* OriginStr, const wchar_t* Format, wchar_t Target, wchar_t* OutStr1, int OutStr1Len, wchar_t* OutStr2, int OutStr2Len, wchar_t* OutStr3, int OutStr3Len, wchar_t* OutStr4, int OutStr4Len)
{
	if (OutStr1 != NULL) {
		OutStr1[0] = L'\0';
	}
	if (OutStr2 != NULL) {
		OutStr2[0] = L'\0';
	}
	if (OutStr3 != NULL) {
		OutStr3[0] = L'\0';
	}
	if (OutStr4 != NULL) {
		OutStr4[0] = L'\0';
	}

	// Check the presented input parameters
	if (OriginStr == NULL || *OriginStr == '\0' ||
		Format == NULL || *Format == '\0') {
		return -1;
	}
	int FormatLen = (int)StkPlWcsLen(Format);
	int OriginStrLen = (int)StkPlWcsLen(OriginStr);
	if (FormatLen <= 0 || FormatLen > OriginStrLen) {
		return -1;
	}

	// Get the number of target existence in the presented string
	int NumberOfTargets = 0;
	for (int Loop = 0; Format[Loop] != '\0'; Loop++) {
		if (Format[Loop] == Target) {
			NumberOfTargets++;
		}
	}
	if (NumberOfTargets == 0) {
		if (StkPlWcsCmp(OriginStr, Format) == 0) {
			return 1;
		} else {
			return 0;
		}
	}

	// Make work variables and configure them
	// Note that 0xFF20 of UTF-16 is internally used as delimiter code.
	wchar_t* OriginStrWk = new wchar_t[OriginStrLen + 3];
	wchar_t* FormatWk = new wchar_t[FormatLen + 3];
	FormatWk[0] = 0xff20;
	OriginStrWk[0] = 0xff20;
	StkPlWcsCpy(&OriginStrWk[1], OriginStrLen + 3 - 1, OriginStr);
	StkPlWcsCpy(&FormatWk[1], FormatLen + 3 - 1, Format);
	FormatWk[FormatLen + 1] = 0xff20;
	OriginStrWk[OriginStrLen + 1] = 0xff20;
	FormatWk[FormatLen + 2] = L'\0';
	OriginStrWk[OriginStrLen + 2] = L'\0';

	// Configure SearchPtr and SearchEndPtr
	wchar_t* SearchPtr[5] = {NULL, NULL, NULL, NULL, NULL};
	wchar_t* SearchEndPtr[5] = {NULL, NULL, NULL, NULL, NULL};
	int SearchPtrIndex = 0;
	SearchPtr[SearchPtrIndex] = FormatWk;
	SearchPtrIndex++;
	int Loop;
	for (Loop = 0; Loop < (int)StkPlWcsLen(FormatWk); Loop++) {
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
	wchar_t* SearchString[5];
	for (Loop = 0; Loop < 5; Loop++) {
		SearchString[Loop] = new wchar_t[SearchEndPtr[Loop] - SearchPtr[Loop] + 2];
		if (SearchPtr[Loop] != NULL && SearchEndPtr[Loop] != NULL) {
			StkPlWcsNCpy(SearchString[Loop], SearchEndPtr[Loop] - SearchPtr[Loop] + 2, SearchPtr[Loop], SearchEndPtr[Loop] - SearchPtr[Loop]);
			SearchString[Loop][SearchEndPtr[Loop] - SearchPtr[Loop]] = L'\0';
		} else {
			StkPlWcsCpy(SearchString[Loop], SearchEndPtr[Loop] - SearchPtr[Loop] + 2, L"");
		}
	}

	// Make OutStrs
	const wchar_t* OutputBegin[4];
	const wchar_t* OutputEnd[4];
	const wchar_t* CurrStr;
	int NumberOfParamSet = 0;
	for (Loop = 0; Loop < 4; Loop++) {
		if (Loop == 0) {
			CurrStr = OriginStrWk;
		} else {
			CurrStr = OutputEnd[Loop - 1];
		}
		if (CurrStr != NULL) {
			OutputBegin[Loop] = StkPlWcsStr(CurrStr, SearchString[Loop]);
			OutputEnd[Loop] = StkPlWcsStr(CurrStr + StkPlWcsLen(SearchString[Loop]), SearchString[Loop + 1]);
			if (OutputBegin[Loop] != NULL && OutputBegin[Loop][0] == L'\0') {
				OutputBegin[Loop] = NULL;
			}
			if (OutputEnd[Loop] != NULL && OutputEnd[Loop][0] == L'\0') {
				OutputEnd[Loop] = NULL;
			}
			if (OutputBegin[Loop] != NULL) {
				OutputBegin[Loop] = StkPlWcsStr(CurrStr, SearchString[Loop]) + StkPlWcsLen(SearchString[Loop]);
			}
		} else {
			OutputBegin[Loop] = NULL;
			OutputEnd[Loop] = NULL;
		}
		if (Loop == 0 && OutStr1 != NULL && OutputBegin[0] != NULL && OutputEnd[0] != NULL) {
			int TmpLen = (int)(OutputEnd[0] - OutputBegin[0]) + 1;
			if (OutStr1Len != -1 && OutStr1Len < TmpLen) {
				TmpLen = OutStr1Len;
			}
			StkPlWcsNCpy(OutStr1, OutStr1Len, OutputBegin[0], TmpLen - 1);
			OutStr1[TmpLen - 1] = L'\0';
			NumberOfParamSet++;
		}
		if (Loop == 1 && OutStr2 != NULL && OutputBegin[1] != NULL && OutputEnd[1] != NULL) {
			int TmpLen = (int)(OutputEnd[1] - OutputBegin[1]) + 1;
			if (OutStr2Len != -1 && OutStr2Len < TmpLen) {
				TmpLen = OutStr2Len;
			}
			StkPlWcsNCpy(OutStr2, OutStr2Len, OutputBegin[1], TmpLen - 1);
			OutStr2[TmpLen - 1] = L'\0';
			NumberOfParamSet++;
		}
		if (Loop == 2 && OutStr3 != NULL && OutputBegin[2] != NULL && OutputEnd[2] != NULL) {
			int TmpLen = (int)(OutputEnd[2] - OutputBegin[2]) + 1;
			if (OutStr3Len != -1 && OutStr3Len < TmpLen) {
				TmpLen = OutStr3Len;
			}
			StkPlWcsNCpy(OutStr3, OutStr3Len, OutputBegin[2], TmpLen - 1);
			OutStr3[TmpLen - 1] = L'\0';
			NumberOfParamSet++;
		}
		if (Loop == 3 && OutStr4 != NULL && OutputBegin[3] != NULL && OutputEnd[3] != NULL) {
			int TmpLen = (int)(OutputEnd[3] - OutputBegin[3]) + 1;
			if (OutStr4Len != -1 && OutStr4Len < TmpLen) {
				TmpLen = OutStr4Len;
			}
			StkPlWcsNCpy(OutStr4, OutStr4Len, OutputBegin[3], TmpLen - 1);
			OutStr4[TmpLen - 1] = L'\0';
			NumberOfParamSet++;
		}
	}

	// Release the allocated memory
	for (Loop = 0; Loop < 5; Loop++) {
		delete SearchString[Loop];
	}
	delete OriginStrWk;
	delete FormatWk;

	if (NumberOfTargets == NumberOfParamSet) {
		return 1;
	}
	return 0;
}
