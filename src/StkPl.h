#pragma once
#include <cstddef>

#ifndef FILENAME_MAX
#ifdef WIN32
#define FILENAME_MAX 260
#else
#define FILENAME_MAX 4096
#endif
#endif

size_t StkPlStrLen(const char*);
int StkPlStrCmp(const char*, const char*);
const char* StkPlStrStr(const char*, const char*);
char* StkPlStrCpy(char*, size_t, const char*);
char* StkPlStrNCpy(char*, size_t, const char*, size_t);


size_t StkPlWcsLen(const wchar_t*);
int StkPlWcsCmp(const wchar_t*, const wchar_t*);
const wchar_t* StkPlWcsStr(const wchar_t*, const wchar_t*);
wchar_t* StkPlWcsCpy(wchar_t*, size_t, const wchar_t*);
wchar_t* StkPlWcsNCpy(wchar_t*, size_t, const wchar_t*, size_t);
wchar_t* StkPlLStrCpy(wchar_t*, const wchar_t*);
wchar_t* StkPlWcsCat(wchar_t*, size_t, const wchar_t*);

bool StkPlIsJapaneseLocale();
bool StkPlIsJapaneseLocaleFromEnv();
char* StkPlWideCharToUtf8(const wchar_t*);
char* StkPlWideCharToSjis(const wchar_t*);

int StkPlPrintf(const char*, ...);
int StkPlWPrintf(const wchar_t*, ...);
int StkPlSPrintf(char*, size_t, const char*, ...);
int StkPlSwPrintf(wchar_t*, size_t, const wchar_t*, ...);

int StkPlSScanf(const char* Str, const char* Format, ...);

void StkPlExit(int);
int StkPlRand();
int StkPlAtoi(const char*);
long int StkPlWcsToL(const wchar_t*);
float StkPlWcsToF(const wchar_t*);

int StkPlGetFullPathFromFileName(const wchar_t*, wchar_t[FILENAME_MAX]);
size_t StkPlGetFileSize(const wchar_t[FILENAME_MAX]);
int StkPlReadFile(const wchar_t[FILENAME_MAX], char*, size_t);

int StkPlGetUsedMemorySizeOfCurrentProcess();
long long StkPlGetTickCount();
