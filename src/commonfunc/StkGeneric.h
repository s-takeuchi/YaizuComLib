#pragma once

#include <windows.h>

class StkGeneric  
{
private:
	StkGeneric();

public:
	static StkGeneric* GetInstance();
	virtual ~StkGeneric();

	bool IsDuplicateAppExisting(HWND);
	int GetFullPathFromFileName(wchar_t*, wchar_t[MAX_PATH]);
	int GetFullPathWithoutFileName(wchar_t*, wchar_t[MAX_PATH]);
	int GetFileSize(wchar_t*);

	int JsonEncodeSize(wchar_t*);
	void JsonEncode(wchar_t*, wchar_t*, int);

	int HtmlEncodeSize(wchar_t*);
	void HtmlEncode(wchar_t*, wchar_t*, int);
	void HtmlDecode(wchar_t*, wchar_t*, int);
};
