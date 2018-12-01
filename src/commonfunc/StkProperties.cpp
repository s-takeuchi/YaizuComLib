#include <windows.h>
#include "StkProperties.h"

class StkProperties::Impl
{
public:
	char PropertyName[256][256];
	char PropertyValue[256][256];
	int NumOfProperties;

	int GetFullPathFromFileName(wchar_t*, wchar_t[MAX_PATH]);
};

StkProperties::StkProperties()
{
	pImpl = new Impl;
	pImpl->NumOfProperties = 0;
}

StkProperties::~StkProperties()
{
	delete pImpl;
}

// Get full path from the specified file name.
// FileName [in] : File name which you want to get absolute path for. Do not specify path. Specify only file name. The file needs to be placed in the same folder of executing module.
// FullPath [out] : Acquired full path for the specified file.
// Return : 0:Success, -1:Failure
int StkProperties::Impl::GetFullPathFromFileName(wchar_t* FileName, wchar_t FullPath[MAX_PATH])
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

// Get properties from the specified file name
// FileName [in] : Target file name which you want to get properties. Do not specify path. The file is searched from Module existing folder.
// Return : Result code 0:Success, -1:Failure
int StkProperties::GetProperties(wchar_t* FileName)
{
	wchar_t Buf[MAX_PATH];
	pImpl->GetFullPathFromFileName(FileName, Buf);

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
				pImpl->PropertyName[pImpl->NumOfProperties][PName] = PropertyLine[PrLoop][ChLoop];
				PName++;
			} else {
				break;
			}
		}
		pImpl->PropertyName[pImpl->NumOfProperties][PName] = '\0';

		// If length of property name is zero, proceed to next line.
		if (strlen(pImpl->PropertyName[pImpl->NumOfProperties]) == 0) {
			continue;
		}
		// If same property name has already been registered, proceed to next line.
		bool Found = false;
		for (int Lp = 0; Lp < pImpl->NumOfProperties; Lp++) {
			if (strcmp(pImpl->PropertyName[Lp], pImpl->PropertyName[pImpl->NumOfProperties]) == 0) {
				Found = true;
				break;
			}
		}
		if (Found == true) {
			continue;
		}

		for (; ChLoop < (int)strlen(PropertyLine[PrLoop]); ChLoop++) {
			if (PropertyLine[PrLoop][ChLoop] != ' ' && PropertyLine[PrLoop][ChLoop] != '=' && PropertyLine[PrLoop][ChLoop] != '\t') {
				break;
			}
		}

		int PValue = 0;
		for (; ChLoop < (int)strlen(PropertyLine[PrLoop]); ChLoop++) {
			pImpl->PropertyValue[pImpl->NumOfProperties][PValue] = PropertyLine[PrLoop][ChLoop];
			PValue++;
		}
		pImpl->PropertyValue[pImpl->NumOfProperties][PValue] = '\0';

		pImpl->NumOfProperties++;
	}

	return 0;
}

// This method acquires the value of the specified property name
// Value [out] : Acquired value (int)
// Return : Reault code, 0:Success, -1:Failure
int StkProperties::GetPropertyInt(char* PropName, int* Value)
{
	for (int Loop = 0; Loop < pImpl->NumOfProperties; Loop++) {
		if (strcmp(PropName, pImpl->PropertyName[Loop]) == 0) {
			*Value = atoi(pImpl->PropertyValue[Loop]);
			return 0;
		}
	}
	return -1;
}

// This method acquires the value of the specified property name
// Value [out] : Acquired value (char[256])
// Return : Reault code, 0:Success, -1:Failure
int StkProperties::GetPropertyStr(char* PropName, char Value[256])
{
	for (int Loop = 0; Loop < pImpl->NumOfProperties; Loop++) {
		if (strcmp(PropName, pImpl->PropertyName[Loop]) == 0) {
			strcpy_s(Value, 256, pImpl->PropertyValue[Loop]);
			return 0;
		}
	}
	return -1;
}
