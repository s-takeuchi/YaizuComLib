#pragma once

#include <windows.h>

class StkGeneric  
{
private:
	StkGeneric();

public:
	static StkGeneric* GetInstance();
	virtual ~StkGeneric();

	BOOL IsDuplicateAppExisting(HWND);
	int GetFullPathFromFileName(TCHAR*, TCHAR[MAX_PATH]);
	int GetFullPathWithoutFileName(TCHAR*, TCHAR[MAX_PATH]);
	int GetFileSize(TCHAR*);

	void GetLocalTimeStr(char[32]);
	void GetLocalTimeWStr(TCHAR[32]);

	int JsonEncodeSize(TCHAR*);
	void JsonEncode(TCHAR*, TCHAR*, int);

	int HtmlEncodeSize(TCHAR*);
	void HtmlEncode(TCHAR*, TCHAR*, int);
	void HtmlDecode(TCHAR*, TCHAR*, int);
};
