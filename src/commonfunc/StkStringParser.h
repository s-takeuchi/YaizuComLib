#pragma once

class StkStringParser
{
public:
	static int ParseInto1Param(wchar_t*, wchar_t*, wchar_t, wchar_t*);
	static int ParseInto1Param(wchar_t*, wchar_t*, wchar_t, wchar_t*, int);
	static int ParseInto2Params(wchar_t*, wchar_t*, wchar_t, wchar_t*, wchar_t*);
	static int ParseInto2Params(wchar_t*, wchar_t*, wchar_t, wchar_t*, int, wchar_t*, int);
	static int ParseInto3Params(wchar_t*, wchar_t*, wchar_t, wchar_t*, wchar_t*, wchar_t*);
	static int ParseInto3Params(wchar_t*, wchar_t*, wchar_t, wchar_t*, int, wchar_t*, int, wchar_t*, int);
	static int ParseInto4Params(wchar_t*, wchar_t*, wchar_t, wchar_t*, wchar_t*, wchar_t*, wchar_t*);
	static int ParseInto4Params(wchar_t*, wchar_t*, wchar_t, wchar_t*, int, wchar_t*, int, wchar_t*, int, wchar_t*, int);
};
