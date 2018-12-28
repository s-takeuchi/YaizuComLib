#pragma once
#include <cstddef>

size_t StkPlWcsLen(const wchar_t*);
int StkPlWcsCmp(const wchar_t*, const wchar_t*);
wchar_t* StkPlWcsStr(wchar_t*, const wchar_t*);
char* StkPlStrStr(char*, const char*);
wchar_t* StkPlWcsCpy(wchar_t*, size_t, const wchar_t*);
wchar_t* StkPlWcsNCpy(wchar_t*, size_t, const wchar_t*, size_t);
wchar_t* StkPlWcsCat(wchar_t*, size_t, const wchar_t*);
bool StkPlIsJapaneseLocale();
bool StkPlIsJapaneseLocaleFromEnv();
char* StkPlWideCharToUtf8(const wchar_t*);
char* StkPlWideCharToSjis(const wchar_t*);
void StkPlExit(int);
int StkPlPrintf(const char*);
int StkPlRand();
