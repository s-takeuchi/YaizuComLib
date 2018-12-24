#include <cwchar>
#include <cstring>
#include <clocale>
#include <cstdlib>
#include <cstdio>

#include "StkPl.h"

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

size_t StkPlWcsLen(const wchar_t* Wcs)
{
	return wcslen(Wcs);
}

int StkPlWcsCmp(const wchar_t* Wcs1, const wchar_t* Wcs2)
{
	return wcscmp(Wcs1, Wcs2);
}

const char* StkPlStrStr(const char* Str1, const char* Str2)
{
	return strstr(Str1, Str2);
}

bool StkPlIsJapaneseLocale()
{
	setlocale(LC_ALL, "");
	char* TmpLoc = setlocale(LC_CTYPE, NULL);
	if (strstr(TmpLoc, "ja_JP") != NULL || strstr(TmpLoc, "Japanese_Japan") != NULL) {
		return true;
	}
	return false;
}

void StkPlExit(int Status)
{
	exit(Status);
}

int StkPlPrintf(const char* Format)
{
	return printf(Format);
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

#ifdef WIN32

#include <windows.h>

wchar_t* StkPlWcsNCpy(wchar_t* Destination, size_t NumberOfElements, const wchar_t* Source, size_t Num)
{
	wcsncpy_s(Destination, NumberOfElements, Source, Num);
	return Destination;
}

bool StkPlIsJapaneseLocaleFromEnv()
{
	char* Locale;
	size_t LocaleSize;
	if (_dupenv_s(&Locale, &LocaleSize, "HTTP_ACCEPT_LANGUAGE") == 0) {
		if (Locale == 0 || LocaleSize == 0) {
			return false;
		}
		if (strstr(Locale, (char*)"ja") == Locale) {
			free(Locale);
			return true;
		}
	}
	return false;
}

char* StkPlWideCharToUtf8(const wchar_t* Msg)
{
	int MltBufLen = WideCharToMultiByte(CP_UTF8, 0, Msg, -1, (LPSTR)NULL, 0, NULL, NULL);
	char* MltBuf = (char*)new char[MltBufLen + 1];
	if (WideCharToMultiByte(CP_UTF8, 0, Msg, -1, MltBuf, MltBufLen, NULL, NULL) != 0) {
		MltBuf[MltBufLen] = '\0';
	}
	return MltBuf;
}

char* StkPlWideCharToSjis(const wchar_t* Msg)
{
	int MltBufLen = WideCharToMultiByte(CP_THREAD_ACP, 0, Msg, -1, (LPSTR)NULL, 0, NULL, NULL);
	char* MltBuf = (char*)new char[MltBufLen + 1];
	if (WideCharToMultiByte(CP_THREAD_ACP, 0, Msg, -1, MltBuf, MltBufLen, NULL, NULL) != 0) {
		MltBuf[MltBufLen] = '\0';
	}
	return MltBuf;
}

#endif

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

#ifndef WIN32

wchar_t* StkPlWcsNCpy(wchar_t* Destination, size_t NumberOfElements, const wchar_t* Source, size_t Num)
{
	return wcsncpy(Destination, Source, Num);
}

bool StkPlIsJapaneseLocaleFromEnv()
{
	char* Locale = getenv("HTTP_ACCEPT_LANGUAGE");
	if (Locale == 0) {
		return false;
	}
	if (strstr(Locale, (char*)"ja") == Locale) {
		return true;
	}
	return false;
}

char* StkPlWideCharToUtf8(const wchar_t* Msg)
{
	setlocale(LC_CTYPE, "");
	setlocale(LC_CTYPE, "ja_JP.UTF-8");
	mbstate_t MbState;
	memset((void*)&MbState, 0, sizeof(MbState));
	size_t ActualSize = wcsrtombs(NULL, &Msg, 1, &MbState);
	if (ActualSize == (size_t)-1) {
		char* NewMbs = new char[1];
		NewMbs[0] = L'\0';
		return NewMbs;
	}
	char* NewMbs = new char[ActualSize + 1];
	wcsrtombs(NewMbs, &Msg, ActualSize, &MbState);
	NewMbs[ActualSize] = '\0';
	return NewMbs;
}

char* StkPlWideCharToSjis(const wchar_t* Msg)
{
	setlocale(LC_CTYPE, "");
	setlocale(LC_CTYPE, "ja_JP.sjis");
	mbstate_t MbState;
	memset((void*)&MbState, 0, sizeof(MbState));
	size_t ActualSize = wcsrtombs(NULL, &Msg, 1, &MbState);
	if (ActualSize == (size_t)-1) {
		char* NewMbs = new char[1];
		NewMbs[0] = L'\0';
		return NewMbs;
	}
	char* NewMbs = new char[ActualSize + 1];
	wcsrtombs(NewMbs, &Msg, ActualSize, &MbState);
	NewMbs[ActualSize] = '\0';
	return NewMbs;
}

#endif
