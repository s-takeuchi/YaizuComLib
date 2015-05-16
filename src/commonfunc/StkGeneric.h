#pragma once

#include <windows.h>

class StkGeneric  
{
private:
	char PropertyName[256][256];
	char PropertyValue[256][256];
	int NumOfProperties;
	
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
	int GetFileSize(TCHAR*);
	int GetProperties(TCHAR*);
	int GetPropertyInt(char*, int*);
	int GetPropertyStr(char*, char[256]);
	void GetLocalTimeStr(char[32]);
	void GetLocalTimeWStr(TCHAR[32]);

	int JsonEncodeSize(TCHAR*);
	void JsonEncode(TCHAR*, TCHAR*, int);

	int HtmlEncodeSize(TCHAR*);
	void HtmlEncode(TCHAR*, TCHAR*, int);
	void HtmlDecode(TCHAR*, TCHAR*, int);
};
