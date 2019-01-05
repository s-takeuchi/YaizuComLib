#include <cwchar>
#include <cstring>
#include <clocale>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>

#include "StkPl.h"

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

size_t StkPlStrLen(const char* Str)
{
	return strlen(Str);
}

int StkPlStrCmp(const char* Str1, const char* Str2)
{
	return strcmp(Str1, Str2);
}

int StkPlStrNCmp(const char* Str1, const char* Str2, size_t Num)
{
	return strncmp(Str1, Str2, Num);
}

const char* StkPlStrStr(const char* Str1, const char* Str2)
{
	return strstr(Str1, Str2);
}

size_t StkPlWcsLen(const wchar_t* Wcs)
{
	return wcslen(Wcs);
}

int StkPlWcsCmp(const wchar_t* Wcs1, const wchar_t* Wcs2)
{
	return wcscmp(Wcs1, Wcs2);
}

const wchar_t* StkPlWcsStr(const wchar_t* Wcs1, const wchar_t* Wcs2)
{
	return wcsstr(Wcs1, Wcs2);
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

int StkPlPrintf(const char* Format, ...)
{
	va_list va;
	va_start(va, Format);
	int Ret = vprintf(Format, va);
	va_end(va);
	return Ret;
}

int StkPlWPrintf(const wchar_t* Format, ...)
{
	va_list va;
	va_start(va, Format);
	int Ret = vwprintf(Format, va);
	va_end(va);
	return Ret;
}

int StkPlSPrintf(char* Str, size_t Len, const char* Format, ...)
{
	va_list va;
	va_start(va, Format);
	int Ret = vsnprintf(Str, Len, Format, va);
	va_end(va);
	return Ret;
}

int StkPlSwPrintf(wchar_t* Str, size_t Len, const wchar_t* Format, ...)
{
	va_list va;
	va_start(va, Format);
	int Ret = vswprintf(Str, Len, Format, va);
	va_end(va);
	return Ret;
}

int StkPlSScanf(const char* Str, const char* Format, ...)
{
	va_list va;
	va_start(va, Format);
	int Ret = vsscanf(Str, Format, va);
	va_end(va);
	return Ret;
}

int StkPlRand()
{
	return rand();
}

int StkPlAtoi(const char* Str)
{
	return atoi(Str);
}

long int StkPlWcsToL(const wchar_t* Str)
{
	return wcstol(Str, NULL, 10);
}

float StkPlWcsToF(const wchar_t* Str)
{
	return wcstof(Str, NULL);
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

#ifdef WIN32

#include <windows.h>
#include <Psapi.h>
#include <filesystem>

char* StkPlStrCpy(char* Destination, size_t NumberOfElements, const char* Source)
{
	strcpy_s(Destination, NumberOfElements, Source);
	return Destination;
}

char* StkPlStrNCpy(char* Destination, size_t NumberOfElements, const char* Source, size_t Num)
{
	strncpy_s(Destination, NumberOfElements, Source, Num);
	return Destination;
}

wchar_t* StkPlWcsCpy(wchar_t* Destination, size_t NumberOfElements, const wchar_t* Source)
{
	wcsncpy_s(Destination, NumberOfElements, Source, _TRUNCATE);
	return Destination;
}

wchar_t* StkPlWcsNCpy(wchar_t* Destination, size_t NumberOfElements, const wchar_t* Source, size_t Num)
{
	wcsncpy_s(Destination, NumberOfElements, Source, Num);
	return Destination;
}

wchar_t* StkPlLStrCpy(wchar_t* Destination, const wchar_t* Source)
{
	return lstrcpy(Destination, Source);
}

wchar_t* StkPlWcsCat(wchar_t* Destination, size_t NumberOfElements, const wchar_t* Source)
{
	wcscat_s(Destination, NumberOfElements, Source);
	return Destination;
}

bool StkPlIsJapaneseLocaleFromEnv()
{
	char* Locale;
	unsigned int LocaleSize;
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

// Get full path from the specified file name.
// FileName [in] : File name which you want to get absolute path for. Do not specify path. Specify only file name. The file needs to be placed in the same folder of executing module.
// FullPath [out] : Acquired full path for the specified file.
// Return : Always zero
int StkPlGetFullPathFromFileName(const wchar_t* FileName, wchar_t FullPath[FILENAME_MAX])
{
	GetModuleFileName(NULL, FullPath, FILENAME_MAX - 1);
	std::experimental::filesystem::path CurPath = FullPath;
	std::filesystem::path NewPath = CurPath.parent_path() / FileName;
	wcscpy_s(FullPath, FILENAME_MAX, NewPath.c_str());
	return 0;
}

size_t StkPlGetFileSize(const wchar_t FilePath[FILENAME_MAX])
{
	uintmax_t FileSize = 0;
	try {
		FileSize = std::filesystem::file_size(FilePath);
	} catch (std::filesystem::filesystem_error ex) {
		return -1;
	}
	if (FileSize >= 1000000) {
		return -1;
	}
	if (FileSize == 0) {
		return 0;
	}
	return (size_t)FileSize;
}

int StkPlReadFile(const wchar_t FilePath[FILENAME_MAX], char* Buffer, size_t FileSize)
{
	HANDLE ReadFileHndl = CreateFile(FilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (ReadFileHndl == INVALID_HANDLE_VALUE) {
		return -1;
	}

	DWORD TmpSize = 0;
	if (ReadFile(ReadFileHndl, (LPVOID)Buffer, FileSize, &TmpSize, NULL) == 0) {
		CloseHandle(ReadFileHndl);
		return -1;
	}

	CloseHandle(ReadFileHndl);
	return TmpSize;
}

int StkPlGetUsedMemorySizeOfCurrentProcess()
{
	DWORD dwProcessID = GetCurrentProcessId();
	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc = { 0 };

	long Size;
	if ((hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, false, dwProcessID)) != NULL) {
		if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
			Size = pmc.WorkingSetSize;
		}
	}
	CloseHandle(hProcess);
	return Size;
}

long long StkPlGetTickCount()
{
	return GetTickCount();
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

#else

#include <unistd.h>
#include <time.h>
#include <experimental/filesystem>

char* StkPlStrCpy(char* Destination, size_t NumberOfElements, const char* Source)
{
	return strcpy(Destination, Source);
}

char* StkPlStrNCpy(char* Destination, size_t NumberOfElements, const char* Source, size_t Num)
{
	return strncpy(Destination, Source, Num);
}

wchar_t* StkPlWcsCpy(wchar_t* Destination, size_t NumberOfElements, const wchar_t* Source)
{
	return wcscpy(Destination, Source);
}

wchar_t* StkPlWcsNCpy(wchar_t* Destination, size_t NumberOfElements, const wchar_t* Source, size_t Num)
{
	return wcsncpy(Destination, Source, Num);
}

wchar_t* StkPlLStrCpy(wchar_t* Destination, const wchar_t* Source)
{
	return wcscpy(Destination, Source);
}

wchar_t* StkPlWcsCat(wchar_t* Destination, size_t NumberOfElements, const wchar_t* Source)
{
	return wcscat(Destination, Source);
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
	char* ConfLc = setlocale(LC_CTYPE, "ja_JP.UTF-8");
	mbstate_t MbState;
	memset((void*)&MbState, 0, sizeof(MbState));
	int ActualSize = wcsrtombs(NULL, &Msg, 1, &MbState);
	if (ActualSize == -1 || ConfLc == NULL) {
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
	char* ConfLc = setlocale(LC_CTYPE, "ja_JP.sjis");
	mbstate_t MbState;
	memset((void*)&MbState, 0, sizeof(MbState));
	int ActualSize = wcsrtombs(NULL, &Msg, 1, &MbState);
	if (ActualSize == -1 || ConfLc == NULL) {
		char* NewMbs = new char[1];
		NewMbs[0] = L'\0';
		return NewMbs;
	}
	char* NewMbs = new char[ActualSize + 1];
	wcsrtombs(NewMbs, &Msg, ActualSize, &MbState);
	NewMbs[ActualSize] = '\0';
	return NewMbs;
}

// Get full path from the specified file name.
// FileName [in] : File name which you want to get absolute path for. Do not specify path. Specify only file name. The file needs to be placed in the same folder of executing module.
// FullPath [out] : Acquired full path for the specified file.
// Return : Always zero
int StkPlGetFullPathFromFileName(const wchar_t* FileName, wchar_t FullPath[FILENAME_MAX])
{
	char c_full_path[FILENAME_MAX];
	readlink("/proc/self/exe", c_full_path, sizeof(c_full_path) - 1);
	std::experimental::filesystem::path CurPath = c_full_path;
	std::experimental::filesystem::path NewPath = CurPath.parent_path() / FileName;
	wcscpy(FullPath, NewPath.wstring().c_str());
	return 0;
}

size_t StkPlGetFileSize(const wchar_t FilePath[FILENAME_MAX])
{
	uintmax_t FileSize = 0;
	try {
		FileSize = std::experimental::filesystem::file_size(FilePath);
	} catch (std::experimental::filesystem::filesystem_error ex) {
		return -1;
	}
	if (FileSize >= 1000000) {
		return -1;
	}
	if (FileSize == 0) {
		return 0;
	}
	return (size_t)FileSize;
}

int StkPlReadFile(const wchar_t FilePath[FILENAME_MAX], char* Buffer, size_t FileSize)
{
	char* FileNameUtf8 = StkPlWideCharToUtf8(FilePath);
	FILE *fp = fopen(FileNameUtf8, "r");
	if (fp == NULL) {
		return -1;
	}
	char* work_dat = new char[(int)FileSize + 1];
	int actual_filesize = fread(Buffer, sizeof(char), (int)FileSize, fp);
	fclose(fp);
	delete FileNameUtf8;
	return actual_filesize;
}

int StkPlGetUsedMemorySizeOfCurrentProcess()
{
	FILE *fp;
	char ProcInfo[64];
	char Buffer[4096];
	sprintf(ProcInfo, "/proc/%d/status", getpid());
	if ((fp = fopen(ProcInfo, "r")) == NULL) {
		return -1;
	}
	int ActualFileSize = fread(Buffer, sizeof(char), 4096, fp);
	fclose(fp);
	char* Ptr = strstr(Buffer, "VmSize:");
	if (Ptr == NULL) {
		return -1;
	}
	char DummyStr[32] = "";
	int VmSize = 0;
	sscanf(Ptr, "%s %d", DummyStr, &VmSize);
	return VmSize;
}

long long StkPlGetTickCount()
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (long long)(ts.tv_nsec / 1000000) + ((long long)ts.tv_sec * 1000ull);
}

#endif
