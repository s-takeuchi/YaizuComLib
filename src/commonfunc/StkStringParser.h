#pragma once

class StkStringParser
{
public:
	static int ParseInto1Param(const wchar_t*, const wchar_t*, wchar_t, wchar_t*);
	static int ParseInto1Param(const wchar_t*, const wchar_t*, wchar_t, wchar_t*, int);
	static int ParseInto2Params(const wchar_t*, const wchar_t*, wchar_t, wchar_t*, wchar_t*);
	static int ParseInto2Params(const wchar_t*, const wchar_t*, wchar_t, wchar_t*, int, wchar_t*, int);
	static int ParseInto3Params(const wchar_t*, const wchar_t*, wchar_t, wchar_t*, wchar_t*, wchar_t*);
	static int ParseInto3Params(const wchar_t*, const wchar_t*, wchar_t, wchar_t*, int, wchar_t*, int, wchar_t*, int);
	static int ParseInto4Params(const wchar_t*, const wchar_t*, wchar_t, wchar_t*, wchar_t*, wchar_t*, wchar_t*);
	static int ParseInto4Params(const wchar_t*, const wchar_t*, wchar_t, wchar_t*, int, wchar_t*, int, wchar_t*, int, wchar_t*, int);
};
