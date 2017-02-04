#pragma once
#include <windows.h>

class StkStringParser
{
public:
	static int ParseInto1Param(TCHAR*, TCHAR*, TCHAR, TCHAR[512]);
	static int ParseInto2Params(TCHAR*, TCHAR*, TCHAR, TCHAR[512], TCHAR[512]);
	static int ParseInto3Params(TCHAR*, TCHAR*, TCHAR, TCHAR[512], TCHAR[512], TCHAR[512]);
	static int ParseInto4Params(TCHAR*, TCHAR*, TCHAR, TCHAR[512], TCHAR[512], TCHAR[512], TCHAR[512]);
};
