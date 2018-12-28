#pragma once

#define NULL 0

#ifdef WIN32
#define FILENAME_MAX 260
#else
#define FILENAME_MAX 4096
#endif

int StkPlStrLen(const char*);
int StkPlStrCmp(const char*, const char*);
char* StkPlStrStr(char*, const char*);
char* StkPlStrCpy(char*, int, const char*);

int StkPlWcsLen(const wchar_t*);
int StkPlWcsCmp(const wchar_t*, const wchar_t*);
wchar_t* StkPlWcsStr(wchar_t*, const wchar_t*);
wchar_t* StkPlWcsCpy(wchar_t*, int, const wchar_t*);
wchar_t* StkPlWcsNCpy(wchar_t*, int, const wchar_t*, int);
wchar_t* StkPlWcsCat(wchar_t*, int, const wchar_t*);

bool StkPlIsJapaneseLocale();
bool StkPlIsJapaneseLocaleFromEnv();
char* StkPlWideCharToUtf8(const wchar_t*);
char* StkPlWideCharToSjis(const wchar_t*);

void StkPlExit(int);
int StkPlPrintf(const char*);
int StkPlRand();
int StkPlAtoi(const char*);

int GetFullPathFromFileName(wchar_t*, wchar_t[FILENAME_MAX]);
int GetFileSize(wchar_t[FILENAME_MAX]);
int ReadFile(wchar_t[FILENAME_MAX], char*, int);
