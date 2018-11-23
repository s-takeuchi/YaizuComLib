#include <cwchar>
#include <cstdio>
#include <filesystem>

#ifdef WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include "StkProperties.h"

class StkProperties::Impl
{
public:
	char PropertyName[256][256];
	char PropertyValue[256][256];
	int NumOfProperties;

	int GetFullPathFromFileName(const wchar_t*, wchar_t[FILENAME_MAX]);
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
// file_name [in] : File name which you want to get absolute path for. Do not specify path. Specify only file name. The file needs to be placed in the same folder of executing module.
// full_path [out] : Acquired full path for the specified file.
// Return : 0:Success, -1:Failure
int StkProperties::Impl::GetFullPathFromFileName(const wchar_t* file_name, wchar_t full_path[FILENAME_MAX])
{
#ifdef WIN32
	GetModuleFileName(NULL, full_path, FILENAME_MAX - 1);
	std::filesystem::path cur_path = full_path;
#else
	char c_full_path[FILENAME_MAX];
	readlink("/proc/self/exe", c_full_path, sizeof(c_full_path) - 1);
	std::filesystem::path cur_path = c_full_path;
#endif
	std::filesystem::path new_path = cur_path.parent_path() / file_name;
	wcscpy(full_path, new_path.c_str());

	return 0;
}

// Get properties from the specified file name
// FileName [in] : Target file name which you want to get properties. Do not specify path. The file is searched from Module existing folder.
// Return : Result code 0:Success, -1:Failure
int StkProperties::GetProperties(const wchar_t* FileName)
{
	wchar_t Buf[FILENAME_MAX];
	std::filesystem::path presented_path = FileName;
	if (presented_path.is_absolute()) {
		wcscpy(Buf, FileName);
	} else {
		pImpl->GetFullPathFromFileName(FileName, Buf);
	}

	uintmax_t filesize = 0;
	try {
		filesize = std::filesystem::file_size(Buf);
	} catch (std::filesystem::filesystem_error ex) {
		return -1;
	}
	if (filesize >= 1000000) {
		return -1;
	}
	if (filesize == 0) {
		return 0;
	}

	char c_filename[FILENAME_MAX];

#ifdef WIN32
	setlocale(LC_CTYPE, "Japanese_Japan.932");
#else
	setlocale(LC_CTYPE, "ja_JP.utf8");
#endif

	wcstombs(c_filename, Buf, FILENAME_MAX);

	FILE *fp = fopen(c_filename, "r");
	if (fp == NULL) {
		return -1;
	}
	char* work_dat = new char[(int)filesize + 1];
	size_t actual_filesize = fread(work_dat, sizeof(char), (size_t)filesize, fp);
	work_dat[actual_filesize] = '\0';
	work_dat[filesize] = '\0';
	fclose(fp);

	// set line
	char PropertyLine[64][256];
	char* WorkDatAddr = work_dat;
	char* WorkDatEnd = work_dat + filesize;
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
	delete work_dat;

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
