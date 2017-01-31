#pragma once
#include <windows.h>

class StkStringParser
{
public:
	static int ParseInto4Params(TCHAR*, TCHAR*, TCHAR, TCHAR[512], TCHAR[512], TCHAR[512], TCHAR[512]);
};
