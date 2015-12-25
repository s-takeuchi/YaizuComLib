#pragma once

#include <windows.h>

class StkGeneric  
{
private:
	StkGeneric();

public:
	static const int UNKNOWN       = 0;
	static const int WINDOWS_2000  = 50;
	static const int WINDOWS_XP    = 51;
	static const int WINDOWS_VISTA = 60;

	static const TCHAR* S_UNKNOWN;
	static const TCHAR* S_WINDOWS_2000;
	static const TCHAR* S_WINDOWS_XP;
	static const TCHAR* S_WINDOWS_VISTA;

	static StkGeneric* GetInstance();
	virtual ~StkGeneric();

	BOOL IsDuplicateAppExisting(HWND);
	int GetOSVersion(TCHAR*);
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
