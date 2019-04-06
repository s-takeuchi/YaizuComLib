#pragma once
#include <cstddef>

#ifndef FILENAME_MAX
#ifdef WIN32
#define FILENAME_MAX 260
#else
#define FILENAME_MAX 4096
#endif
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// APIs for char
size_t StkPlStrLen(const char*);
int StkPlStrCmp(const char*, const char*);
int StkPlStrNCmp(const char*, const char*, size_t);
const char* StkPlStrStr(const char*, const char*);
char* StkPlStrCpy(char*, size_t, const char*);
char* StkPlStrNCpy(char*, size_t, const char*, size_t);
char * StkPlStrCat(char*, size_t, const char*);


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// APIs for wchar
size_t StkPlWcsLen(const wchar_t*);
int StkPlWcsCmp(const wchar_t*, const wchar_t*);
const wchar_t* StkPlWcsStr(const wchar_t*, const wchar_t*);
wchar_t* StkPlWcsCpy(wchar_t*, size_t, const wchar_t*);
wchar_t* StkPlWcsNCpy(wchar_t*, size_t, const wchar_t*, size_t);
wchar_t* StkPlLStrCpy(wchar_t*, const wchar_t*);
wchar_t* StkPlWcsCat(wchar_t*, size_t, const wchar_t*);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// APIs for memory
int StkPlMemCmp(const void*, const void*, size_t);
void* StkPlMemCpy(void*, const void*, size_t);
void* StkPlMemSet(void*, int, size_t);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// APIs for locale
bool StkPlIsJapaneseLocale();
bool StkPlIsJapaneseLocaleFromEnv();
char* StkPlWideCharToSjis(const wchar_t*);
wchar_t* StkPlSjisToWideChar(const char*);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// APIs for UTF conversion
size_t StkPlConvUtf16ToUtf32(char32_t*, size_t, const char16_t*);
size_t StkPlConvUtf32ToUtf16(char16_t*, size_t, const char32_t*);
size_t StkPlConvUtf8ToUtf32(char32_t*, size_t, const char*);
size_t StkPlConvUtf32ToUtf8(char*, size_t, const char32_t*);
size_t StkPlConvUtf8ToUtf16(char16_t*, size_t, const char*);
size_t StkPlConvUtf16ToUtf8(char*, size_t, const char16_t*);

size_t StkPlConvUtf16ToWideChar(wchar_t*, size_t, const char16_t*);
size_t StkPlConvWideCharToUtf16(char16_t*, size_t, const wchar_t*);
size_t StkPlConvWideCharToUtf32(char32_t*, size_t, const wchar_t*);
size_t StkPlConvUtf32ToWideChar(wchar_t*, size_t, const char32_t*);
size_t StkPlConvUtf8ToWideChar(wchar_t*, size_t, const char*);
size_t StkPlConvWideCharToUtf8(char*, size_t, const wchar_t*);

char32_t* StkPlCreateUtf32FromUtf16(const char16_t*);
char16_t* StkPlCreateUtf16FromUtf32(const char32_t*);
char32_t* StkPlCreateUtf32FromUtf8(const char*);
char* StkPlCreateUtf8FromUtf32(const char32_t*);
char16_t* StkPlCreateUtf16FromUtf8(const char*);
char* StkPlCreateUtf8FromUtf16(const char16_t*);

wchar_t* StkPlCreateWideCharFromUtf16(const char16_t*);
char16_t* StkPlCreateUtf16FromWideChar(const wchar_t*);
char32_t* StkPlCreateUtf32FromWideChar(const wchar_t*);
wchar_t* StkPlCreateWideCharFromUtf32(const char32_t*);
wchar_t* StkPlCreateWideCharFromUtf8(const char*);
char* StkPlCreateUtf8FromWideChar(const wchar_t*);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// APIs for etc
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

int StkPlGetUsedMemorySizeOfCurrentProcess();

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// APIs for date and time
long long StkPlGetTickCount();
void StkPlGetTimeInRfc2822(char[64], bool);
void StkPlGetWTimeInRfc2822(wchar_t[64], bool);
void StkPlGetTimeInOldFormat(char[64], bool);
void StkPlGetWTimeInOldFormat(wchar_t[64], bool);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// APIs for file access
int StkPlGetFullPathFromFileName(const wchar_t*, wchar_t[FILENAME_MAX]);
int StkPlGetFullPathWithoutFileName(wchar_t*, wchar_t[FILENAME_MAX]);
size_t StkPlGetFileSize(const wchar_t[FILENAME_MAX]);
int StkPlReadFile(const wchar_t[FILENAME_MAX], char*, size_t);
void* StkPlOpenFileForRead(const wchar_t[FILENAME_MAX]);
void* StkPlOpenFileForWrite(const wchar_t[FILENAME_MAX]);
void StkPlCloseFile(void*);
int StkPlRead(void*, char*, size_t);
int StkPlWrite(void*, char*, size_t);
