#include <windows.h>
#include <stdio.h>
#include <shlwapi.h>
#include "StkGeneric.h"

StkGeneric::StkGeneric()
{
}

StkGeneric::~StkGeneric()
{

}

StkGeneric* StkGeneric::GetInstance()
{
	static int init = 1;
	static StkGeneric *f;
	if (init == 1) {
		f = new StkGeneric();
		init = 0;
	}
	return f;
}

bool StkGeneric::IsDuplicateAppExisting(HWND hwnd)
{
	wchar_t or[64];
	wchar_t ws[64];

	GetWindowText(hwnd, or, 64);
	HWND chwnd = GetTopWindow(NULL);
	int i = 0;
	do {
		GetWindowText(chwnd, ws, 64);
		if (lstrcmp(ws, or) == 0) {
			i++;
		}
		if (i >= 2) {
			return true;
		}
		chwnd = GetNextWindow(chwnd, GW_HWNDNEXT);
	} while (chwnd != NULL);
	return false;
}

// Get full path from the specified file name.
// FileName [in] : File name which you want to get absolute path for. Do not specify path. Specify only file name. The file needs to be placed in the same folder of executing module.
// FullPath [out] : Acquired full path for the specified file.
// Return : 0:Success, -1:Failure
int StkGeneric::GetFullPathFromFileName(wchar_t* FileName, wchar_t FullPath[MAX_PATH])
{
	GetModuleFileName(NULL, FullPath, MAX_PATH - 1);
	LPTSTR Addr = NULL;
	for (Addr = FullPath + lstrlen(FullPath); Addr >= FullPath; Addr--) {
		if (*Addr == '\\') {
			break;
		}
	}
	if (Addr == FullPath) {
		return -1;
	}
	Addr++;
	lstrcpy(Addr, FileName);
	return 0;
}

// Get full path without file name for the specified full path file name.
// FileName [in] : Full path which contains file name.
// FullPath [out] : Acquired full path for the specified file.
// Return : 0:Success, -1:Failure
int StkGeneric::GetFullPathWithoutFileName(wchar_t* FileName, wchar_t FullPath[MAX_PATH]) {
	if (FileName == NULL || FullPath == NULL) {
		return -1;
	}
	if (lstrcmp(FileName, L"") == 0) {
		return -1;
	}
	lstrcpy(FullPath, FileName);
	LPTSTR Addr = NULL;
	for (Addr = FullPath + lstrlen(FullPath); Addr > FullPath; Addr--) {
		if (*Addr == '\\') {
			break;
		}
	}
	if (Addr == FullPath) {
		return -1;
	}
	*Addr = '\0\0';
	return 0;
}


// Get file size of the specified file name
// FileName [in] : File name which you want to get the file size. Do not specify path. The file is searched from Module existing folder.
// Return : Size of the specified file. If an error occurred, -1 is returned. (The value is casted into int.)
int StkGeneric::GetFileSize(wchar_t* FileName)
{
	wchar_t Buf[MAX_PATH];
	GetFullPathFromFileName(FileName, Buf);

	HANDLE ReadFileHndl = CreateFile(Buf, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (ReadFileHndl == INVALID_HANDLE_VALUE) {
		return -1;
	}
	LARGE_INTEGER ExistingFileSize;
	if (GetFileSizeEx(ReadFileHndl, &ExistingFileSize) == 0) {
		CloseHandle(ReadFileHndl);
		return -1;
	}
	CloseHandle(ReadFileHndl);
	return (int)ExistingFileSize.QuadPart;
}

// Returns word size of escaped JSON data.
// InMsg [in] : Original string
// Return : Word size of escaped JSON data.
int StkGeneric::JsonEncodeSize(wchar_t* InMsg)
{
	int Size = 0;
	int InMsgLen = lstrlen(InMsg);
	for (int Loop = 0; Loop < InMsgLen; Loop++) {
		if (InMsg[Loop] == '\"' || InMsg[Loop] == '\\' || InMsg[Loop] == '/' ||
			InMsg[Loop] == '\b' || InMsg[Loop] == '\f' || InMsg[Loop] == '\n' ||
			InMsg[Loop] == '\r' || InMsg[Loop] == '\t') {
				Size += 2;
		} else {
			Size ++;
		}
	}
	return Size;
}

// Convert the string to escaped JSON data
// InData [in] : Zero terminate string
// OutData [out] : Converted string is registered.
// OutDataLen [in] : Length of OutData
void StkGeneric::JsonEncode(wchar_t* InData, wchar_t* OutData, int OutDataLen)
{
	wchar_t* CurInPtr = InData;
	wchar_t* CurOutPtr = OutData;
	for (;;) {
		if (*CurInPtr == '\0') {
			*CurOutPtr = '\0';
			return;
		} else if (*CurInPtr == '\"' || *CurInPtr == '\\' || *CurInPtr == '/' ||
			*CurInPtr == '\b' || *CurInPtr == '\f' || *CurInPtr == '\n' ||
			*CurInPtr == '\r' || *CurInPtr == '\t') {
			*CurOutPtr = '\\';
			CurOutPtr++;
			switch (*CurInPtr) {
			case '\"': *CurOutPtr = '\"'; break;
			case '\\': *CurOutPtr = '\\'; break;
			case '/': *CurOutPtr = '/';  break;
			case '\b': *CurOutPtr = 'b';  break;
			case '\f': *CurOutPtr = 'f';  break;
			case '\n': *CurOutPtr = 'n';  break;
			case '\r': *CurOutPtr = 'r';  break;
			case '\t': *CurOutPtr = 't';  break;
			}
		} else {
			*CurOutPtr = *CurInPtr;
		}
		CurOutPtr++;
		CurInPtr++;
		if (CurOutPtr - OutData >= OutDataLen - 2) {
			*OutData = '\0';
			return;
		}
	}
}

// Returns word size of escaped HTML text.
// InMsg [in] : Original string
// Return : Word size of escaped HTML text.
int StkGeneric::HtmlEncodeSize(wchar_t* InMsg)
{
	int Size = 0;
	int InMsgLen = lstrlen(InMsg);
	for (int Loop = 0; Loop < InMsgLen; Loop++) {
		if (InMsg[Loop] == '<') Size += 4;
		else if (InMsg[Loop] == '>') Size += 4;
		else if (InMsg[Loop] == '&') Size += 5;
		else if (InMsg[Loop] == '\"') Size += 6;
		else Size ++;
	}
	return Size;
}

// Convert the string to escaped HTML text.
// InMsg [in] : Zero terminate string
// OutMsg [out] : Converted string is registered.
// SizeOfOutMsg [in] : Length of OutData
void StkGeneric::HtmlEncode(wchar_t* InMsg, wchar_t* OutMsg, int SizeOfOutMsg)
{
	int OutMsgIndex = 0;
	int InMsgLen = lstrlen(InMsg);
	for (int Loop = 0; Loop < InMsgLen; Loop++) {
		if (InMsg[Loop] == '<') {
			if (OutMsgIndex + 4 >= SizeOfOutMsg)
				break;
			lstrcpy(&OutMsg[OutMsgIndex], L"&lt;");
			OutMsgIndex += 4;
			continue;
		}
		if (InMsg[Loop] == '>') {
			if (OutMsgIndex + 4 >= SizeOfOutMsg)
				break;
			lstrcpy(&OutMsg[OutMsgIndex], L"&gt;");
			OutMsgIndex += 4;
			continue;
		}
		if (InMsg[Loop] == '&') {
			if (OutMsgIndex + 5 >= SizeOfOutMsg)
				break;
			lstrcpy(&OutMsg[OutMsgIndex], L"&amp;");
			OutMsgIndex += 5;
			continue;
		}
		if (InMsg[Loop] == '\"') {
			if (OutMsgIndex + 6 >= SizeOfOutMsg)
				break;
			lstrcpy(&OutMsg[OutMsgIndex], L"&quot;");
			OutMsgIndex += 6;
			continue;
		}
		if (OutMsgIndex + 1 >= SizeOfOutMsg)
			break;
		OutMsg[OutMsgIndex] = InMsg[Loop];
		OutMsgIndex++;
	}
	OutMsg[OutMsgIndex] = '\0';
}

// Convert the escaped HTML text to string.
// InMsg [in] : Zero terminate string
// OutMsg [out] : Converted string is registered.
// SizeOfOutMsg [in] : Length of OutData
void StkGeneric::HtmlDecode(wchar_t* InMsg, wchar_t* OutMsg, int SizeOfOutMsg)
{
	int OutMsgIndex = 0;
	int InMsgLen = lstrlen(InMsg);
	for (int Loop = 0; Loop < InMsgLen; Loop++) {
		if (OutMsgIndex + 1 >= SizeOfOutMsg)
			break;
		if (StrStr(&InMsg[Loop], L"&lt;") == &InMsg[Loop]) {
			OutMsg[OutMsgIndex] = '<';
			OutMsgIndex++;
			Loop += 3;
			continue;
		}
		if (StrStr(&InMsg[Loop], L"&gt;") == &InMsg[Loop]) {
			OutMsg[OutMsgIndex] = '>';
			OutMsgIndex++;
			Loop += 3;
			continue;
		}
		if (StrStr(&InMsg[Loop], L"&amp;") == &InMsg[Loop]) {
			OutMsg[OutMsgIndex] = '&';
			OutMsgIndex++;
			Loop += 4;
			continue;
		}
		if (StrStr(&InMsg[Loop], L"&quot;") == &InMsg[Loop]) {
			OutMsg[OutMsgIndex] = '\"';
			OutMsgIndex++;
			Loop += 5;
			continue;
		}
		OutMsg[OutMsgIndex] = InMsg[Loop];
		OutMsgIndex++;
	}
	OutMsg[OutMsgIndex] = '\0';
}
