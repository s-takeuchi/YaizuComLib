#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <shlwapi.h>
#include "StkGeneric.h"

const TCHAR* StkGeneric::S_UNKNOWN       = _T("UNKNOWN");
const TCHAR* StkGeneric::S_WINDOWS_2000  = _T("WINDOWS 2000");
const TCHAR* StkGeneric::S_WINDOWS_XP    = _T("WINDOWS XP");
const TCHAR* StkGeneric::S_WINDOWS_VISTA = _T("WINDOWS VISTA");

StkGeneric::StkGeneric()
{
	NumOfProperties = 0;
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

BOOL StkGeneric::IsDuplicateAppExisting(HWND hwnd)
{
	TCHAR or[64];
	TCHAR ws[64];

	GetWindowText(hwnd, or, 64);
	HWND chwnd = GetTopWindow(NULL);
	int i = 0;
	do {
		GetWindowText(chwnd, ws, 64);
		if (lstrcmp(ws, or) == 0) {
			i++;
		}
		if (i >= 2) {
			return TRUE;
		}
		chwnd = GetNextWindow(chwnd, GW_HWNDNEXT);
	} while (chwnd != NULL);
	return FALSE;
}

int StkGeneric::GetOSVersion(TCHAR* OsVer)
{
	OSVERSIONINFO OsVerInfo;
	OsVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&OsVerInfo);
	int MaVer = OsVerInfo.dwMajorVersion;
	int MiVer = OsVerInfo.dwMinorVersion;
	if (MaVer == 5) {
		if (MiVer == 0) {
			lstrcpy(OsVer, S_WINDOWS_2000);
			return WINDOWS_2000;
		}
		if (MiVer == 1) {
			lstrcpy(OsVer, S_WINDOWS_XP);
			return WINDOWS_XP;
		}
	}
	if (MaVer == 6) {
		if (MiVer == 0) {
			lstrcpy(OsVer, S_WINDOWS_VISTA);
			return WINDOWS_VISTA;
		}
	}
	lstrcpy(OsVer, S_UNKNOWN);
	return UNKNOWN;
}

// Get full path from the specified file name.
// FileName [in] : File name which you want to get absolute path for. Do not specify path. Specify only file name. The file needs to be placed in the same folder of executing module.
// FullPath [out] : Acquired full path for the specified file.
// Return : 0:Success, -1:Failure
int StkGeneric::GetFullPathFromFileName(TCHAR* FileName, TCHAR FullPath[MAX_PATH])
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
int StkGeneric::GetFullPathWithoutFileName(TCHAR* FileName, TCHAR FullPath[MAX_PATH]) {
	lstrcpy(FullPath, FileName);
	LPTSTR Addr = NULL;
	for (Addr = FullPath + lstrlen(FullPath); Addr >= FullPath; Addr--) {
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
int StkGeneric::GetFileSize(TCHAR* FileName)
{
	TCHAR Buf[MAX_PATH];
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

// Get properties from the specified file name
// FileName [in] : Target file name which you want to get properties. Do not specify path. The file is searched from Module existing folder.
// Return : Result code 0:Success, -1:Failure
int StkGeneric::GetProperties(TCHAR* FileName)
{
	TCHAR Buf[MAX_PATH];
	GetFullPathFromFileName(FileName, Buf);

	HANDLE ReadFileHndl = CreateFile(Buf, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (ReadFileHndl == INVALID_HANDLE_VALUE) {
		return -1;
	}

	LARGE_INTEGER ExistingFileSize;
	GetFileSizeEx(ReadFileHndl, &ExistingFileSize);
	if (ExistingFileSize.QuadPart >= 1000000) {
		CloseHandle(ReadFileHndl);
		return -1;
	}
	if (ExistingFileSize.QuadPart == 0) {
		CloseHandle(ReadFileHndl);
		return 0;
	}
	int WorkDatLength = (int)ExistingFileSize.QuadPart;
	BYTE* WorkDat = new BYTE[WorkDatLength + 1];
	DWORD TmpSize = 0;

	if (ReadFile(ReadFileHndl, (LPVOID)WorkDat, WorkDatLength, &TmpSize, NULL) == 0) {
		CloseHandle(ReadFileHndl);
		delete WorkDat;
		return -1;
	}
	WorkDat[WorkDatLength] = '\0';

	CloseHandle(ReadFileHndl);

	// set line
	char PropertyLine[64][256];
	BYTE* WorkDatAddr = WorkDat;
	BYTE* WorkDatEnd = WorkDat + WorkDatLength;
	int Loop;
	for (Loop = 0; Loop < 64; Loop++) {
		for (; WorkDatAddr < WorkDatEnd; WorkDatAddr++) {
			if ((*WorkDatAddr != '\n') && (*WorkDatAddr != '\r') && (*WorkDatAddr != '\t') && (*WorkDatAddr != ' ')) {
				break;
			}
		}
		char* Start = (char*)WorkDatAddr;

		for (; WorkDatAddr < WorkDatEnd; WorkDatAddr++) {
			if (*WorkDatAddr == '\n' || *WorkDatAddr == '\r') {
				break;
			}
		}
		*WorkDatAddr = '\0';

		if (WorkDatAddr >= WorkDatEnd) {
			break;
		}
		if (strlen(Start) < 256) {
			strcpy_s(PropertyLine[Loop], (char*)Start);
		} else {
			Loop--;
		}
		WorkDatAddr++;
	}
	Loop++;
	delete WorkDat;

	// Acquire property name and value
	for (int PrLoop = 0; PrLoop < Loop; PrLoop++) {
		int ChLoop = 0;
		if (strstr(PropertyLine[PrLoop], "=") == 0) {
			continue;
		}

		int PName = 0;
		for (; ChLoop < (int)strlen(PropertyLine[PrLoop]); ChLoop++) {
			if (PropertyLine[PrLoop][ChLoop] != ' ' && PropertyLine[PrLoop][ChLoop] != '=' && PropertyLine[PrLoop][ChLoop] != '\t') {
				PropertyName[NumOfProperties][PName] = PropertyLine[PrLoop][ChLoop];
				PName++;
			} else {
				break;
			}
		}
		PropertyName[NumOfProperties][PName] = '\0';

		// If length of property name is zero, proceed to next line.
		if (strlen(PropertyName[NumOfProperties]) == 0) {
			continue;
		}
		// If same property name has already been registered, proceed to next line.
		BOOL Found = FALSE;
		for (int Lp = 0; Lp < NumOfProperties; Lp++) {
			if (strcmp(PropertyName[Lp], PropertyName[NumOfProperties]) == 0) {
				Found = TRUE;
				break;
			}
		}
		if (Found == TRUE) {
			continue;
		}

		for (; ChLoop < (int)strlen(PropertyLine[PrLoop]); ChLoop++) {
			if (PropertyLine[PrLoop][ChLoop] != ' ' && PropertyLine[PrLoop][ChLoop] != '=' && PropertyLine[PrLoop][ChLoop] != '\t') {
				break;
			}
		}

		int PValue = 0;
		for (; ChLoop < (int)strlen(PropertyLine[PrLoop]); ChLoop++) {
			PropertyValue[NumOfProperties][PValue] = PropertyLine[PrLoop][ChLoop];
			PValue++;
		}
		PropertyValue[NumOfProperties][PValue] = '\0';

		NumOfProperties++;
	}

	return 0;
}

// This method acquires the value of the specified property name
// Value [out] : Acquired value (int)
// Return : Reault code, 0:Success, -1:Failure
int StkGeneric::GetPropertyInt(char* PropName, int* Value)
{
	for (int Loop = 0; Loop < NumOfProperties; Loop++) {
		if (strcmp(PropName, PropertyName[Loop]) == 0) {
			*Value = atoi(PropertyValue[Loop]);
			return 0;
		}
	}
	return -1;
}

// This method acquires the value of the specified property name
// Value [out] : Acquired value (char[256])
// Return : Reault code, 0:Success, -1:Failure
int StkGeneric::GetPropertyStr(char* PropName, char Value[256])
{
	for (int Loop = 0; Loop < NumOfProperties; Loop++) {
		if (strcmp(PropName, PropertyName[Loop]) == 0) {
			strcpy_s(Value, 256, PropertyValue[Loop]);
			return 0;
		}
	}
	return -1;
}

// Acquire local time as a string.
// LocalTimeStr [out] : Acquired time of local sysytem
void StkGeneric::GetLocalTimeStr(char LocalTimeStr[32])
{
	SYSTEMTIME Systime;
	GetLocalTime(&Systime);
	char *Mon[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	sprintf_s(LocalTimeStr, 32, "%s %d %d %02d:%02d:%02d", Mon[Systime.wMonth - 1], Systime.wDay, Systime.wYear, Systime.wHour, Systime.wMinute, Systime.wSecond);
}

// Acquire local time as a string.
// LocalTimeStr [out] : Acquired time of local sysytem
void StkGeneric::GetLocalTimeWStr(TCHAR LocalTimeStr[32])
{
	SYSTEMTIME Systime;
	GetLocalTime(&Systime);
	TCHAR *Mon[] = {_T("Jan"), _T("Feb"), _T("Mar"), _T("Apr"), _T("May"), _T("Jun"), _T("Jul"), _T("Aug"), _T("Sep"), _T("Oct"), _T("Nov"), _T("Dec")};
	wsprintf(LocalTimeStr, _T("%s %d %d %02d:%02d:%02d"), Mon[Systime.wMonth - 1], Systime.wDay, Systime.wYear, Systime.wHour, Systime.wMinute, Systime.wSecond);
}

// Returns word size of escaped JSON data.
// InMsg [in] : Original string
// Return : Word size of escaped JSON data.
int StkGeneric::JsonEncodeSize(TCHAR* InMsg)
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
void StkGeneric::JsonEncode(TCHAR* InData, TCHAR* OutData, int OutDataLen)
{
	TCHAR* CurInPtr = InData;
	TCHAR* CurOutPtr = OutData;
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
int StkGeneric::HtmlEncodeSize(TCHAR* InMsg)
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
void StkGeneric::HtmlEncode(TCHAR* InMsg, TCHAR* OutMsg, int SizeOfOutMsg)
{
	int OutMsgIndex = 0;
	int InMsgLen = lstrlen(InMsg);
	for (int Loop = 0; Loop < InMsgLen; Loop++) {
		if (InMsg[Loop] == '<') {
			if (OutMsgIndex + 4 >= SizeOfOutMsg)
				break;
			lstrcpy(&OutMsg[OutMsgIndex], _T("&lt;"));
			OutMsgIndex += 4;
			continue;
		}
		if (InMsg[Loop] == '>') {
			if (OutMsgIndex + 4 >= SizeOfOutMsg)
				break;
			lstrcpy(&OutMsg[OutMsgIndex], _T("&gt;"));
			OutMsgIndex += 4;
			continue;
		}
		if (InMsg[Loop] == '&') {
			if (OutMsgIndex + 5 >= SizeOfOutMsg)
				break;
			lstrcpy(&OutMsg[OutMsgIndex], _T("&amp;"));
			OutMsgIndex += 5;
			continue;
		}
		if (InMsg[Loop] == '\"') {
			if (OutMsgIndex + 6 >= SizeOfOutMsg)
				break;
			lstrcpy(&OutMsg[OutMsgIndex], _T("&quot;"));
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
void StkGeneric::HtmlDecode(TCHAR* InMsg, TCHAR* OutMsg, int SizeOfOutMsg)
{
	int OutMsgIndex = 0;
	int InMsgLen = lstrlen(InMsg);
	for (int Loop = 0; Loop < InMsgLen; Loop++) {
		if (OutMsgIndex + 1 >= SizeOfOutMsg)
			break;
		if (StrStr(&InMsg[Loop], _T("&lt;")) == &InMsg[Loop]) {
			OutMsg[OutMsgIndex] = '<';
			OutMsgIndex++;
			Loop += 3;
			continue;
		}
		if (StrStr(&InMsg[Loop], _T("&gt;")) == &InMsg[Loop]) {
			OutMsg[OutMsgIndex] = '>';
			OutMsgIndex++;
			Loop += 3;
			continue;
		}
		if (StrStr(&InMsg[Loop], _T("&amp;")) == &InMsg[Loop]) {
			OutMsg[OutMsgIndex] = '&';
			OutMsgIndex++;
			Loop += 4;
			continue;
		}
		if (StrStr(&InMsg[Loop], _T("&quot;")) == &InMsg[Loop]) {
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
