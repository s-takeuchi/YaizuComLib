#pragma once
#include <windows.h>

class StkStringParser
{
public:
	static int ParseInto1Param(TCHAR*, TCHAR*, TCHAR, TCHAR*);
	static int ParseInto1Param(TCHAR*, TCHAR*, TCHAR, TCHAR*, int);
	static int ParseInto2Params(TCHAR*, TCHAR*, TCHAR, TCHAR*, TCHAR*);
	static int ParseInto2Params(TCHAR*, TCHAR*, TCHAR, TCHAR*, int, TCHAR*, int);
	static int ParseInto3Params(TCHAR*, TCHAR*, TCHAR, TCHAR*, TCHAR*, TCHAR*);
	static int ParseInto3Params(TCHAR*, TCHAR*, TCHAR, TCHAR*, int, TCHAR*, int, TCHAR*, int);
	static int ParseInto4Params(TCHAR*, TCHAR*, TCHAR, TCHAR*, TCHAR*, TCHAR*, TCHAR*);
	static int ParseInto4Params(TCHAR*, TCHAR*, TCHAR, TCHAR*, int, TCHAR*, int, TCHAR*, int, TCHAR*, int);
};
