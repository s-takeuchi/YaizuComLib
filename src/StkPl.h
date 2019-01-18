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
int StkPlStrNCmp(const char*, const char*, size_t);
const char* StkPlStrStr(const char*, const char*);
char* StkPlStrCpy(char*, size_t, const char*);
char* StkPlStrNCpy(char*, size_t, const char*, size_t);
char * StkPlStrCat(char*, size_t, const char*);

size_t StkPlWcsLen(const wchar_t*);
int StkPlWcsCmp(const wchar_t*, const wchar_t*);
const wchar_t* StkPlWcsStr(const wchar_t*, const wchar_t*);
wchar_t* StkPlWcsCpy(wchar_t*, size_t, const wchar_t*);
wchar_t* StkPlWcsNCpy(wchar_t*, size_t, const wchar_t*, size_t);
wchar_t* StkPlLStrCpy(wchar_t*, const wchar_t*);
wchar_t* StkPlWcsCat(wchar_t*, size_t, const wchar_t*);

void* StkPlMemCpy(void*, const void*, size_t);

bool StkPlIsJapaneseLocale();
bool StkPlIsJapaneseLocaleFromEnv();
char* StkPlWideCharToUtf8(const wchar_t*);
char* StkPlWideCharToSjis(const wchar_t*);
wchar_t* StkPlUtf8ToWideChar(const char*);
wchar_t* StkPlSjisToWideChar(const char*);

int StkPlPrintf(const char*, ...);
int StkPlWPrintf(const wchar_t*, ...);
int StkPlSPrintf(char*, size_t, const char*, ...);
int StkPlSwPrintf(wchar_t*, size_t, const wchar_t*, ...);

int StkPlSScanf(const char* Str, const char* Format, ...);
int StkPlSwScanf(const wchar_t* Str, const wchar_t* Format, ...);

void StkPlExit(int);
int StkPlRand();
void StkPlSleepMs(int);

int StkPlAtoi(const char*);
long int StkPlWcsToL(const wchar_t*);
float StkPlWcsToF(const wchar_t*);

int StkPlGetFullPathFromFileName(const wchar_t*, wchar_t[FILENAME_MAX]);
size_t StkPlGetFileSize(const wchar_t[FILENAME_MAX]);
int StkPlReadFile(const wchar_t[FILENAME_MAX], char*, size_t);

int StkPlGetUsedMemorySizeOfCurrentProcess();
long long StkPlGetTickCount();
void StkPlGetTimeInRfc822(char[64]);
