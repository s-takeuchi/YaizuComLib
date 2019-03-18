#include "../stkpl/StkPl.h"
#include "StkProperties.h"

class StkProperties::Impl
{
public:
	char PropertyName[256][256];
	char PropertyValue[256][256];
	int NumOfProperties;
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

// Get properties from the specified file name
// FileName [in] : Target file name which you want to get properties. Do not specify path. The file is searched from Module existing folder.
// Return : Result code 0:Success, -1:Failure
int StkProperties::GetProperties(const wchar_t* FileName)
{
	wchar_t Buf[FILENAME_MAX];
	StkPlGetFullPathFromFileName(FileName, Buf);

	size_t WorkDatLength = StkPlGetFileSize(Buf);
	if (WorkDatLength == 0) {
		return 0;
	}
	if (WorkDatLength == (size_t)-1) {
#ifndef WIN32
		StkPlSwPrintf(Buf, FILENAME_MAX, L"/etc/%S", FileName);
		WorkDatLength = StkPlGetFileSize(Buf);
		if (WorkDatLength == (size_t)-1) {
			return-1;
		}
#else
		return -1;
#endif
	}

	char* WorkDat = new char[WorkDatLength + 1];
	StkPlReadFile(Buf, WorkDat, WorkDatLength);
	WorkDat[WorkDatLength] = '\0';

	// set line
	char PropertyLine[64][256];
	char* WorkDatAddr = WorkDat;
	char* WorkDatEnd = WorkDat + WorkDatLength;
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
		if (StkPlStrLen(Start) < 256) {
			StkPlStrCpy(PropertyLine[Loop], 256, (char*)Start);
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
		if (StkPlStrStr(PropertyLine[PrLoop], "=") == 0) {
			continue;
		}

		int PName = 0;
		for (; ChLoop < (int)StkPlStrLen(PropertyLine[PrLoop]); ChLoop++) {
			if (PropertyLine[PrLoop][ChLoop] != ' ' && PropertyLine[PrLoop][ChLoop] != '=' && PropertyLine[PrLoop][ChLoop] != '\t') {
				pImpl->PropertyName[pImpl->NumOfProperties][PName] = PropertyLine[PrLoop][ChLoop];
				PName++;
			} else {
				break;
			}
		}
		pImpl->PropertyName[pImpl->NumOfProperties][PName] = '\0';

		// If length of property name is zero, proceed to next line.
		if (StkPlStrLen(pImpl->PropertyName[pImpl->NumOfProperties]) == 0) {
			continue;
		}
		// If same property name has already been registered, proceed to next line.
		bool Found = false;
		for (int Lp = 0; Lp < pImpl->NumOfProperties; Lp++) {
			if (StkPlStrCmp(pImpl->PropertyName[Lp], pImpl->PropertyName[pImpl->NumOfProperties]) == 0) {
				Found = true;
				break;
			}
		}
		if (Found == true) {
			continue;
		}

		for (; ChLoop < (int)StkPlStrLen(PropertyLine[PrLoop]); ChLoop++) {
			if (PropertyLine[PrLoop][ChLoop] != ' ' && PropertyLine[PrLoop][ChLoop] != '=' && PropertyLine[PrLoop][ChLoop] != '\t') {
				break;
			}
		}

		int PValue = 0;
		for (; ChLoop < (int)StkPlStrLen(PropertyLine[PrLoop]); ChLoop++) {
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
int StkProperties::GetPropertyInt(const char* PropName, int* Value)
{
	for (int Loop = 0; Loop < pImpl->NumOfProperties; Loop++) {
		if (StkPlStrCmp(PropName, pImpl->PropertyName[Loop]) == 0) {
			*Value = StkPlAtoi(pImpl->PropertyValue[Loop]);
			return 0;
		}
	}
	return -1;
}

// This method acquires the value of the specified property name
// Value [out] : Acquired value (char[256])
// Return : Reault code, 0:Success, -1:Failure
int StkProperties::GetPropertyStr(const char* PropName, char Value[256])
{
	for (int Loop = 0; Loop < pImpl->NumOfProperties; Loop++) {
		if (StkPlStrCmp(PropName, pImpl->PropertyName[Loop]) == 0) {
			StkPlStrCpy(Value, 256, pImpl->PropertyValue[Loop]);
			return 0;
		}
	}
	return -1;
}
