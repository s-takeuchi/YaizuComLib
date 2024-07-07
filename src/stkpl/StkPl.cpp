#include <cwchar>
#include <cstring>
#include <clocale>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <thread>
#include <chrono>
#include <ctime>

#ifdef WIN32
#include <windows.h>
#include <Psapi.h>
#include <filesystem>
#include <sys/types.h>
#include <sys/timeb.h>
#include <tlhelp32.h>
#else
#include <unistd.h>
#include <dirent.h>
#include <experimental/filesystem>
#include <sys/timeb.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

#include "StkPl.h"


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// APIs for char

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

char* StkPlStrCpy(char* Destination, size_t NumberOfElements, const char* Source)
{
#ifdef WIN32
	strcpy_s(Destination, NumberOfElements, Source);
	return Destination;
#else
	return strcpy(Destination, Source);
#endif
}

char* StkPlStrNCpy(char* Destination, size_t NumberOfElements, const char* Source, size_t Num)
{
#ifdef WIN32
	strncpy_s(Destination, NumberOfElements, Source, Num);
	return Destination;
#else
	return strncpy(Destination, Source, Num);
#endif
}

char * StkPlStrCat(char* Destination, size_t NumberOfElements, const char* Source)
{
#ifdef WIN32
	strcat_s(Destination, NumberOfElements, Source);
	return Destination;
#else
	return strcat(Destination, Source);
#endif
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// APIs for wchar

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

wchar_t* StkPlWcsCpy(wchar_t* Destination, size_t NumberOfElements, const wchar_t* Source)
{
#ifdef WIN32
	wcsncpy_s(Destination, NumberOfElements, Source, _TRUNCATE);
	return Destination;
#else
	return wcscpy(Destination, Source);
#endif
}

wchar_t* StkPlWcsNCpy(wchar_t* Destination, size_t NumberOfElements, const wchar_t* Source, size_t Num)
{
#ifdef WIN32
	wcsncpy_s(Destination, NumberOfElements, Source, Num);
	return Destination;
#else
	return wcsncpy(Destination, Source, Num);
#endif
}

wchar_t* StkPlLStrCpy(wchar_t* Destination, const wchar_t* Source)
{
#ifdef WIN32
	return lstrcpy(Destination, Source);
#else
	return wcscpy(Destination, Source);
#endif
}

wchar_t* StkPlWcsCat(wchar_t* Destination, size_t NumberOfElements, const wchar_t* Source)
{
#ifdef WIN32
	wcscat_s(Destination, NumberOfElements, Source);
	return Destination;
#else
	return wcscat(Destination, Source);
#endif
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// APIs for memory

int StkPlMemCmp(const void* Ptr1, const void* Ptr2, size_t Num)
{
	return memcmp(Ptr1, Ptr2, Num);
}

void* StkPlMemCpy(void* Destination, const void* Source, size_t NumberOfElements)
{
	return memcpy(Destination, Source, NumberOfElements);
}

void* StkPlMemSet(void* Ptr, int Value, size_t Size)
{
	return memset(Ptr, Value, Size);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// APIs for locale

bool StkPlIsJapaneseLocale()
{
	setlocale(LC_ALL, "");
	char* TmpLoc = setlocale(LC_CTYPE, NULL);
	if (TmpLoc != NULL && (strstr(TmpLoc, "ja_JP") != NULL || strstr(TmpLoc, "Japanese_Japan") != NULL)) {
		return true;
	}
	return false;
}

bool StkPlIsJapaneseLocaleFromEnv()
{
#ifdef WIN32
	char* Locale;
	size_t LocaleSize;
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
#else
	char* Locale = getenv("HTTP_ACCEPT_LANGUAGE");
	if (Locale == 0) {
		return false;
	}
	if (strstr(Locale, (char*)"ja") == Locale) {
		return true;
	}
	return false;
#endif
}

char* StkPlWideCharToSjis(const wchar_t* Txt)
{
#ifdef WIN32
	int MltBufLen = WideCharToMultiByte(CP_THREAD_ACP, 0, Txt, -1, (LPSTR)NULL, 0, NULL, NULL);
	if (MltBufLen > 0) {
		char* MltBuf = (char*)new char[MltBufLen + 1];
		if (WideCharToMultiByte(CP_THREAD_ACP, 0, Txt, -1, MltBuf, MltBufLen, NULL, NULL) != 0) {
			MltBuf[MltBufLen] = '\0';
		}
		return MltBuf;
	}
	return NULL;
#else
	char OrgLc[64] = "";
	char* TmpOrgLc = setlocale(LC_CTYPE, "");
	strncpy(OrgLc, TmpOrgLc, 64);
	char* ConfLc = setlocale(LC_CTYPE, "ja_JP.sjis");
	mbstate_t MbState;
	memset((void*)&MbState, 0, sizeof(MbState));
	int ActualSize = wcsrtombs(NULL, &Txt, 1, &MbState);
	if (ActualSize == -1 || ConfLc == NULL) {
		setlocale(LC_CTYPE, OrgLc);
		return NULL;
	}
	char* NewMbs = new char[ActualSize + 1];
	wcsrtombs(NewMbs, &Txt, ActualSize, &MbState);
	NewMbs[ActualSize] = '\0';
	setlocale(LC_CTYPE, OrgLc);
	return NewMbs;
#endif
}

wchar_t* StkPlSjisToWideChar(const char* Txt)
{
#ifdef WIN32
	int WcBufLen = MultiByteToWideChar(CP_THREAD_ACP, MB_USEGLYPHCHARS, (LPCSTR)Txt, -1, NULL, NULL);
	if (WcBufLen > 0) {
		wchar_t* WcTxt = new wchar_t[WcBufLen + 1];
		WcBufLen = MultiByteToWideChar(CP_THREAD_ACP, MB_USEGLYPHCHARS, (LPCSTR)Txt, -1, WcTxt, WcBufLen);
		WcTxt[WcBufLen] = L'\0';
		return WcTxt;
	}
	return NULL;
#else
	char OrgLc[64] = "";
	char* TmpOrgLc = setlocale(LC_CTYPE, "");
	strncpy(OrgLc, TmpOrgLc, 64);
	char* ConfLc = setlocale(LC_CTYPE, "ja_JP.sjis");
	mbstate_t MbState;
	memset((void*)&MbState, 0, sizeof(MbState));
	int ActualSize = mbsrtowcs(NULL, &Txt, 1, &MbState);
	if (ActualSize == -1 || ConfLc == NULL) {
		setlocale(LC_CTYPE, OrgLc);
		return NULL;
	}
	wchar_t* NewWcs = new wchar_t[ActualSize + 1];
	mbsrtowcs(NewWcs, &Txt, ActualSize, &MbState);
	NewWcs[ActualSize] = L'\0';
	setlocale(LC_CTYPE, OrgLc);
	return NewWcs;
#endif
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// APIs for URL encoding / decoding

void DecodeURL(wchar_t* UrlIn, size_t UrlInSize, wchar_t* UrlOut, size_t UrlOutSize)
{
	*UrlOut = '\0';
	if (UrlInSize <= 0 || UrlOutSize <= 0) {
		return;
	}
	char* TmpUrlBc = new char[UrlInSize];
	char* TmpUrlAc = new char[UrlInSize];
	StkPlConvWideCharToUtf8(TmpUrlBc, UrlInSize, UrlIn);
	int TmpUrlBcLen = (int)StkPlStrLen(TmpUrlBc);
	int AcIndex = 0;
	for (int BcIndex = 0; BcIndex < TmpUrlBcLen; BcIndex++) {
		if (TmpUrlBc[BcIndex] == '%' && BcIndex + 2 < TmpUrlBcLen) {
			char Val = 0;
			if (TmpUrlBc[BcIndex + 1] >= '0' && TmpUrlBc[BcIndex + 1] <= '9') {
				Val += (TmpUrlBc[BcIndex + 1] - '0') * 16;
			}
			if (TmpUrlBc[BcIndex + 1] >= 'a' && TmpUrlBc[BcIndex + 1] <= 'f') {
				Val += (TmpUrlBc[BcIndex + 1] - 'a' + 10) * 16;
			}
			if (TmpUrlBc[BcIndex + 1] >= 'A' && TmpUrlBc[BcIndex + 1] <= 'F') {
				Val += (TmpUrlBc[BcIndex + 1] - 'A' + 10) * 16;
			}
			if (TmpUrlBc[BcIndex + 2] >= '0' && TmpUrlBc[BcIndex + 2] <= '9') {
				Val += (TmpUrlBc[BcIndex + 2] - '0');
			}
			if (TmpUrlBc[BcIndex + 2] >= 'a' && TmpUrlBc[BcIndex + 2] <= 'f') {
				Val += (TmpUrlBc[BcIndex + 2] - 'a' + 10);
			}
			if (TmpUrlBc[BcIndex + 2] >= 'A' && TmpUrlBc[BcIndex + 2] <= 'F') {
				Val += (TmpUrlBc[BcIndex + 2] - 'A' + 10);
			}
			TmpUrlAc[AcIndex] = Val;
			BcIndex += 2;
		} else if (TmpUrlBc[BcIndex] == '+') { // space can be converted to '+'.
			TmpUrlAc[AcIndex] = ' ';
		} else {
			TmpUrlAc[AcIndex] = TmpUrlBc[BcIndex];
		}
		AcIndex++;
	}
	TmpUrlAc[AcIndex] = '\0';
	StkPlConvUtf8ToWideChar(UrlOut, UrlOutSize, TmpUrlAc);
	delete [] TmpUrlBc;
	delete [] TmpUrlAc;
}

void EncodeURL(wchar_t* UrlIn, size_t UrlInSize, wchar_t* UrlOut, size_t UrlOutSize)
{
	*UrlOut = '\0';
	if (UrlInSize <= 0 || UrlOutSize <= 0) {
		return;
	}
	wchar_t* WkUrlIn = new wchar_t[UrlInSize];
	StkPlWcsNCpy(WkUrlIn, UrlInSize, UrlIn, UrlInSize - 1);
	char Map[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
	char* TmpUrlBc = new char[UrlInSize * 4];
	char* TmpUrlAc = new char[UrlOutSize * 3];
	StkPlConvWideCharToUtf8(TmpUrlBc, UrlInSize * 4, WkUrlIn);
	int TmpUrlBcLen = (int)StkPlStrLen(TmpUrlBc);
	int AcIndex = 0;
	for (int BcIndex = 0; BcIndex < TmpUrlBcLen; BcIndex++) {
		if ((TmpUrlBc[BcIndex] >= '0' && TmpUrlBc[BcIndex] <= '9') ||
			(TmpUrlBc[BcIndex] >= 'a' && TmpUrlBc[BcIndex] <= 'z') ||
			(TmpUrlBc[BcIndex] >= 'A' && TmpUrlBc[BcIndex] <= 'Z') ||
			TmpUrlBc[BcIndex] == '-' ||
			TmpUrlBc[BcIndex] == '_') {
			TmpUrlAc[AcIndex] = TmpUrlBc[BcIndex];
			if (AcIndex + 1 >= UrlOutSize) {
				break;
			}
			AcIndex++;
		} else {
			if (AcIndex + 3 >= UrlOutSize) {
				break;
			}
			TmpUrlAc[AcIndex] = '%';
			TmpUrlAc[AcIndex + 1] = Map[(unsigned char)TmpUrlBc[BcIndex] / 16];
			TmpUrlAc[AcIndex + 2] = Map[(unsigned char)TmpUrlBc[BcIndex] % 16];
			AcIndex += 3;
		}
	}
	TmpUrlAc[AcIndex] = '\0';
	StkPlConvUtf8ToWideChar(UrlOut, UrlOutSize, TmpUrlAc);
	delete [] WkUrlIn;
	delete [] TmpUrlBc;
	delete [] TmpUrlAc;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// APIs for UTF conversion

size_t StkPlConvUtf16ToUtf32(char32_t* Utf32, size_t SizeInWord, const char16_t* Utf16)
{
	const char16_t* Utf16Ptr = Utf16;
	char32_t* Utf32Ptr = Utf32;
	size_t ActualSize = 0;
	bool ConversionFlag = true;
	if (SizeInWord == 0) {
		ConversionFlag = false;
		SizeInWord = (size_t)-1;
	}
	while (*Utf16Ptr != u'\0' && ActualSize < SizeInWord - 1) {
		if ((*Utf16Ptr >> 10) == 0b110110 && (*(Utf16Ptr + 1) >> 10) == 0b110111) {
			if (ConversionFlag) {
				*Utf32Ptr = 0x10000 + (*Utf16Ptr - 0b1101100000000000) * 0x400 + (*(Utf16Ptr + 1) - 0b1101110000000000);
			}
			Utf16Ptr += 2;
		} else {
			if (ConversionFlag) {
				*Utf32Ptr = (char32_t)*Utf16Ptr;
			}
			Utf16Ptr++;
		}
		Utf32Ptr++;
		ActualSize++;
	}
	if (ConversionFlag) {
		*Utf32Ptr = U'\0';
	}
	return ActualSize;
}

size_t StkPlConvUtf32ToUtf16(char16_t* Utf16, size_t SizeInWord, const char32_t* Utf32)
{
	const char32_t* Utf32Ptr = Utf32;
	char16_t* Utf16Ptr = Utf16;
	size_t ActualSize = 0;
	bool ConversionFlag = true;
	if (SizeInWord == 0) {
		ConversionFlag = false;
		SizeInWord = (size_t)-1;
	}
	while (*Utf32Ptr != u'\0' && ActualSize < SizeInWord - 1) {
		if (*Utf32Ptr < 0x10000) {
			if (ConversionFlag) {
				*Utf16Ptr = (char16_t)*Utf32Ptr;
			}
			Utf16Ptr++;
			ActualSize++;
		} else {
			if (ActualSize + 1 < SizeInWord - 1) {
				if (ConversionFlag) {
					*Utf16Ptr = (char16_t)((*Utf32Ptr - 0x10000) / 0x400 + 0b1101100000000000);
					*(Utf16Ptr + 1) = (char16_t)((*Utf32Ptr - 0x10000) % 0x400 + 0b1101110000000000);
				}
				Utf16Ptr += 2;
				ActualSize += 2;
			} else {
				break;
			}
		}
		Utf32Ptr++;
	}
	if (ConversionFlag) {
		*Utf16Ptr = u'\0';
	}
	return ActualSize;
}

size_t StkPlConvUtf8ToUtf32(char32_t* Utf32, size_t SizeInWord, const char* Utf8)
{
	const unsigned char* Utf8Ptr = (unsigned char*)Utf8;
	char32_t* Utf32Ptr = Utf32;
	size_t ActualSize = 0;
	bool ConversionFlag = true;
	if (SizeInWord == 0) {
		ConversionFlag = false;
		SizeInWord = (size_t)-1;
	}
	while (*Utf8Ptr != u'\0' && ActualSize < SizeInWord - 1) {
		if (*Utf8Ptr < 0x80) {
			if (ConversionFlag) {
				*Utf32Ptr = (char32_t)*Utf8Ptr;
			}
			Utf8Ptr++;
		} else if ((*Utf8Ptr & 0b11100000) == 0b11000000) {
			// Following value is evaluated from left to right. There is no case the element after zero-terminating character is accessed.
			if ((*(Utf8Ptr + 1) & 0b10000000) != 0b10000000) {
				break;
			}
			if (ConversionFlag) {
				*Utf32Ptr = (*Utf8Ptr - 0b11000000) * 0x40 + (*(Utf8Ptr + 1) - 0b10000000);
			}
			Utf8Ptr += 2;
		} else if ((*Utf8Ptr & 0b11110000) == 0b11100000) {
			// Following value is evaluated from left to right. There is no case the element after zero-terminating character is accessed.
			if ((*(Utf8Ptr + 1) & 0b10000000) != 0b10000000 || (*(Utf8Ptr + 2) & 0b10000000) != 0b10000000) {
				break;
			}
			if (ConversionFlag) {
				*Utf32Ptr = (*Utf8Ptr - 0b11100000) * 0x1000 + (*(Utf8Ptr + 1) - 0b10000000) * 0x40 + (*(Utf8Ptr + 2) - 0b10000000);
			}
			Utf8Ptr += 3;
		} else if ((*Utf8Ptr & 0b11111000) == 0b11110000) {
			// Following value is evaluated from left to right. There is no case the element after zero-terminating character is accessed.
			if ((*(Utf8Ptr + 1) & 0b10000000) != 0b10000000 || (*(Utf8Ptr + 2) & 0b10000000) != 0b10000000 || (*(Utf8Ptr + 3) & 0b10000000) != 0b10000000) {
				break;
			}
			if (ConversionFlag) {
				*Utf32Ptr = (*Utf8Ptr - 0b11110000) * 0x40000 + (*(Utf8Ptr + 1) - 0b10000000) * 0x1000 + (*(Utf8Ptr + 2) - 0b10000000) * 0x40 + (*(Utf8Ptr + 3) - 0b10000000);
			}
			Utf8Ptr += 4;
		} else {
			break;
		}
		Utf32Ptr++;
		ActualSize++;
	}
	if (ConversionFlag) {
		*Utf32Ptr = U'\0';
	}
	return ActualSize;
}

size_t StkPlConvUtf32ToUtf8(char* Utf8, size_t SizeInWord, const char32_t* Utf32)
{
	const char32_t* Utf32Ptr = (const char32_t*)Utf32;
	unsigned char* Utf8Ptr = (unsigned char*)Utf8;
	size_t ActualSize = 0;
	bool ConversionFlag = true;
	if (SizeInWord == 0) {
		ConversionFlag = false;
		SizeInWord = (size_t)-1;
	}
	while (*Utf32Ptr != u'\0' && ActualSize < SizeInWord - 1) {
		if (*Utf32Ptr < 0x0080) {
			if (ConversionFlag) {
				*Utf8Ptr = (unsigned char)*Utf32Ptr;
			}
			Utf8Ptr++;
			ActualSize++;
		} else if (*Utf32Ptr < 0x0800) {
			if (ActualSize + 1 >= SizeInWord - 1) {
				break;
			}
			if (ConversionFlag) {
				*Utf8Ptr = (unsigned char)(*Utf32Ptr / 0x40) + 0b11000000;
				*(Utf8Ptr + 1) = (unsigned char)(*Utf32Ptr & 0b00111111) + 0b10000000;
			}
			Utf8Ptr += 2;
			ActualSize += 2;
		} else if (*Utf32Ptr < 0x10000) {
			if (ActualSize + 2 >= SizeInWord - 1) {
				break;
			}
			if (ConversionFlag) {
				*Utf8Ptr = (unsigned char)(*Utf32Ptr / 0x1000) + 0b11100000;
				*(Utf8Ptr + 1) = (unsigned char)((*Utf32Ptr / 0x40) & 0b00111111) + 0b10000000;
				*(Utf8Ptr + 2) = (unsigned char)(*Utf32Ptr & 0b00111111) + 0b10000000;
			}
			Utf8Ptr += 3;
			ActualSize += 3;
		} else if (*Utf32Ptr < 0x200000) {
			if (ActualSize + 3 >= SizeInWord - 1) {
				break;
			}
			if (ConversionFlag) {
				*Utf8Ptr = (unsigned char)(*Utf32Ptr / 0x40000) + 0b11110000;
				*(Utf8Ptr + 1) = (unsigned char)((*Utf32Ptr / 0x1000) & 0b00111111) + 0b10000000;
				*(Utf8Ptr + 2) = (unsigned char)((*Utf32Ptr / 0x40) & 0b00111111) + 0b10000000;
				*(Utf8Ptr + 3) = (unsigned char)(*Utf32Ptr & 0b00111111) + 0b10000000;
			}
			Utf8Ptr += 4;
			ActualSize += 4;
		} else {
			break;
		}
		Utf32Ptr++;
	}
	if (ConversionFlag) {
		*Utf8Ptr = U'\0';
	}
	return ActualSize;
}

size_t StkPlConvUtf8ToUtf16(char16_t* Utf16, size_t SizeInWord, const char* Utf8)
{
	const unsigned char* Utf8Ptr = (unsigned char*)Utf8;
	char16_t* Utf16Ptr = Utf16;
	size_t ActualSize = 0;
	bool ConversionFlag = true;
	if (SizeInWord == 0) {
		ConversionFlag = false;
		SizeInWord = (size_t)-1;
	}
	while (*Utf8Ptr != u'\0' && ActualSize < SizeInWord - 1) {
		if (*Utf8Ptr < 0x80) {
			if (ConversionFlag) {
				*Utf16Ptr = (char32_t)*Utf8Ptr;
			}
			Utf16Ptr++;
			Utf8Ptr++;
			ActualSize++;
		} else if ((*Utf8Ptr & 0b11100000) == 0b11000000) {
			// Following value is evaluated from left to right. There is no case the element after zero-terminating character is accessed.
			if ((*(Utf8Ptr + 1) & 0b10000000) != 0b10000000) {
				break;
			}
			if (ConversionFlag) {
				*Utf16Ptr = (*Utf8Ptr - 0b11000000) * 0x40 + (*(Utf8Ptr + 1) - 0b10000000);
			}
			Utf16Ptr++;
			ActualSize++;
			Utf8Ptr += 2;
		} else if ((*Utf8Ptr & 0b11110000) == 0b11100000) {
			// Following value is evaluated from left to right. There is no case the element after zero-terminating character is accessed.
			if ((*(Utf8Ptr + 1) & 0b10000000) != 0b10000000 || (*(Utf8Ptr + 2) & 0b10000000) != 0b10000000) {
				break;
			}
			if (ConversionFlag) {
				*Utf16Ptr = (*Utf8Ptr - 0b11100000) * 0x1000 + (*(Utf8Ptr + 1) - 0b10000000) * 0x40 + (*(Utf8Ptr + 2) - 0b10000000);
			}
			Utf16Ptr++;
			ActualSize++;
			Utf8Ptr += 3;
		} else if ((*Utf8Ptr & 0b11111000) == 0b11110000) {
			// Following value is evaluated from left to right. There is no case the element after zero-terminating character is accessed.
			if ((*(Utf8Ptr + 1) & 0b10000000) != 0b10000000 || (*(Utf8Ptr + 2) & 0b10000000) != 0b10000000 || (*(Utf8Ptr + 3) & 0b10000000) != 0b10000000) {
				break;
			}
			if (ActualSize + 1 >= SizeInWord - 1) {
				break;
			}
			char32_t Tmp = (*Utf8Ptr - 0b11110000) * 0x40000 + (*(Utf8Ptr + 1) - 0b10000000) * 0x1000 + (*(Utf8Ptr + 2) - 0b10000000) * 0x40 + (*(Utf8Ptr + 3) - 0b10000000);
			if (ConversionFlag) {
				*Utf16Ptr = (char16_t)((Tmp - 0x10000) / 0x400 + 0b1101100000000000);
				*(Utf16Ptr + 1) = (char16_t)((Tmp - 0x10000) % 0x400 + 0b1101110000000000);
			}
			Utf16Ptr += 2;
			ActualSize += 2;
			Utf8Ptr += 4;
		} else {
			break;
		}
	}
	if (ConversionFlag) {
		*Utf16Ptr = U'\0';
	}
	return ActualSize;
}

size_t StkPlConvUtf16ToUtf8(char* Utf8, size_t SizeInWord, const char16_t* Utf16)
{
	const char16_t* Utf16Ptr = Utf16;
	char* Utf8Ptr = Utf8;
	size_t ActualSize = 0;
	bool ConversionFlag = true;
	if (SizeInWord == 0) {
		ConversionFlag = false;
		SizeInWord = (size_t)-1;
	}
	while (*Utf16Ptr != u'\0' && ActualSize < SizeInWord - 1) {
		if ((*Utf16Ptr >> 10) == 0b110110 && (*(Utf16Ptr + 1) >> 10) == 0b110111) {
			char32_t Utf32 = 0x10000 + (*Utf16Ptr - 0b1101100000000000) * 0x400 + (*(Utf16Ptr + 1) - 0b1101110000000000);
			if (Utf32 < 0x200000) {
				if (ActualSize + 3 >= SizeInWord - 1) {
					break;
				}
				if (ConversionFlag) {
					*Utf8Ptr = (unsigned char)(Utf32 / 0x40000) + 0b11110000;
					*(Utf8Ptr + 1) = (unsigned char)((Utf32 / 0x1000) & 0b00111111) + 0b10000000;
					*(Utf8Ptr + 2) = (unsigned char)((Utf32 / 0x40) & 0b00111111) + 0b10000000;
					*(Utf8Ptr + 3) = (unsigned char)(Utf32 & 0b00111111) + 0b10000000;
				}
				Utf8Ptr += 4;
				ActualSize += 4;
			} else {
				break;
			}
			Utf16Ptr += 2;
		} else {
			char32_t Utf32 = (char32_t)*Utf16Ptr;
			if (Utf32 < 0x0080) {
				if (ConversionFlag) {
					*Utf8Ptr = (unsigned char)Utf32;
				}
				Utf8Ptr++;
				ActualSize++;
			} else if (Utf32 < 0x0800) {
				if (ActualSize + 1 >= SizeInWord - 1) {
					break;
				}
				if (ConversionFlag) {
					*Utf8Ptr = (unsigned char)(Utf32 / 0x40) + 0b11000000;
					*(Utf8Ptr + 1) = (unsigned char)(Utf32 & 0b00111111) + 0b10000000;
				}
				Utf8Ptr += 2;
				ActualSize += 2;
			} else if (Utf32 < 0x10000) {
				if (ActualSize + 2 >= SizeInWord - 1) {
					break;
				}
				if (ConversionFlag) {
					*Utf8Ptr = (unsigned char)(Utf32 / 0x1000) + 0b11100000;
					*(Utf8Ptr + 1) = (unsigned char)((Utf32 / 0x40) & 0b00111111) + 0b10000000;
					*(Utf8Ptr + 2) = (unsigned char)(Utf32 & 0b00111111) + 0b10000000;
				}
				Utf8Ptr += 3;
				ActualSize += 3;
			} else {
				break;
			}
			Utf16Ptr++;
		}
	}
	if (ConversionFlag) {
		*Utf8Ptr = U'\0';
	}
	return ActualSize;
}

size_t StkPlConvUtf16ToWideChar(wchar_t* Wc, size_t SizeInWord, const char16_t* Utf16)
{
#ifdef WIN32
	StkPlWcsCpy(Wc, SizeInWord, (wchar_t*)Utf16);
	return StkPlWcsLen(Wc);
#else
	return StkPlConvUtf16ToUtf32((char32_t*)Wc, SizeInWord, Utf16);
#endif
}

size_t StkPlConvWideCharToUtf16(char16_t* Utf16, size_t SizeInWord, const wchar_t* Wc)
{
#ifdef WIN32
	StkPlWcsCpy((wchar_t*)Utf16, SizeInWord, Wc);
	return StkPlWcsLen(Wc);
#else
	return StkPlConvUtf32ToUtf16(Utf16, SizeInWord, (char32_t*)Wc);
#endif
}

size_t StkPlConvWideCharToUtf32(char32_t* Utf32, size_t SizeInWord, const wchar_t* Wc)
{
#ifdef WIN32
	return StkPlConvUtf16ToUtf32(Utf32, SizeInWord, (char16_t*)Wc);
#else
	StkPlWcsCpy((wchar_t*)Utf32, SizeInWord, Wc);
	return StkPlWcsLen(Wc);
#endif
}

size_t StkPlConvUtf32ToWideChar(wchar_t* Wc, size_t SizeInWord, const char32_t* Utf32)
{
#ifdef WIN32
	return StkPlConvUtf32ToUtf16((char16_t*)Wc, SizeInWord, Utf32);
#else
	StkPlWcsCpy(Wc, SizeInWord, (wchar_t*)Utf32);
	return StkPlWcsLen(Wc);
#endif
}

size_t StkPlConvUtf8ToWideChar(wchar_t* Wc, size_t SizeInWord, const char* Utf8)
{
#ifdef WIN32
	return StkPlConvUtf8ToUtf16((char16_t*)Wc, SizeInWord, Utf8);
#else
	return StkPlConvUtf8ToUtf32((char32_t*)Wc, SizeInWord, Utf8);
#endif
}

size_t StkPlConvWideCharToUtf8(char* Utf8, size_t SizeInWord, const wchar_t* Wc)
{
#ifdef WIN32
	return StkPlConvUtf16ToUtf8(Utf8, SizeInWord, (char16_t*)Wc);
#else
	return StkPlConvUtf32ToUtf8(Utf8, SizeInWord, (char32_t*)Wc);
#endif
}

char32_t* StkPlCreateUtf32FromUtf16(const char16_t* Utf16)
{
	int Len = (int)StkPlConvUtf16ToUtf32(NULL, 0, Utf16) + 1;
	char32_t* Utf32 = new char32_t[Len];
	StkPlConvUtf16ToUtf32(Utf32, Len, Utf16);
	return Utf32;
}

char16_t* StkPlCreateUtf16FromUtf32(const char32_t* Utf32)
{
	int Len = (int)StkPlConvUtf32ToUtf16(NULL, 0, Utf32) + 1;
	char16_t* Utf16 = new char16_t[Len];
	StkPlConvUtf32ToUtf16(Utf16, Len, Utf32);
	return Utf16;
}

char32_t* StkPlCreateUtf32FromUtf8(const char* Utf8)
{
	int Len = (int)StkPlConvUtf8ToUtf32(NULL, 0, Utf8) + 1;
	char32_t* Utf32 = new char32_t[Len];
	StkPlConvUtf8ToUtf32(Utf32, Len, Utf8);
	return Utf32;
}

char* StkPlCreateUtf8FromUtf32(const char32_t* Utf32)
{
	int Len = (int)StkPlConvUtf32ToUtf8(NULL, 0, Utf32) + 1;
	char* Utf8 = new char[Len];
	StkPlConvUtf32ToUtf8(Utf8, Len, Utf32);
	return Utf8;
}

char16_t* StkPlCreateUtf16FromUtf8(const char* Utf8)
{
	int Len = (int)StkPlConvUtf8ToUtf16(NULL, 0, Utf8) + 1;
	char16_t* Utf16 = new char16_t[Len];
	StkPlConvUtf8ToUtf16(Utf16, Len, Utf8);
	return Utf16;
}

char* StkPlCreateUtf8FromUtf16(const char16_t* Utf16)
{
	int Len = (int)StkPlConvUtf16ToUtf8(NULL, 0, Utf16) + 1;
	char* Utf8 = new char[Len];
	StkPlConvUtf16ToUtf8(Utf8, Len, Utf16);
	return Utf8;
}

wchar_t* StkPlCreateWideCharFromUtf16(const char16_t* Utf16)
{
#ifdef WIN32
	int Len = (int)StkPlWcsLen((wchar_t*)Utf16) + 1;
	wchar_t* Wc = new wchar_t[Len];
	StkPlWcsCpy(Wc, Len, (wchar_t*)Utf16);
	return Wc;
#else
	return (wchar_t*)StkPlCreateUtf32FromUtf16(Utf16);
#endif
}

char16_t* StkPlCreateUtf16FromWideChar(const wchar_t* Wc)
{
#ifdef WIN32
	int Len = (int)StkPlWcsLen((wchar_t*)Wc) + 1;
	char16_t* Utf16 = new char16_t[Len];
	StkPlWcsCpy((wchar_t*)Utf16, Len, Wc);
	return Utf16;
#else
	return StkPlCreateUtf16FromUtf32((char32_t*)Wc);
#endif
}

char32_t* StkPlCreateUtf32FromWideChar(const wchar_t* Wc)
{
#ifdef WIN32
	return StkPlCreateUtf32FromUtf16((char16_t*)Wc);
#else
	int Len = StkPlWcsLen((wchar_t*)Wc) + 1;
	char32_t* Utf32 = new char32_t[Len];
	StkPlWcsCpy((wchar_t*)Utf32, Len, Wc);
	return Utf32;
#endif
}

wchar_t* StkPlCreateWideCharFromUtf32(const char32_t* Utf32)
{
#ifdef WIN32
	return (wchar_t*)StkPlCreateUtf16FromUtf32(Utf32);
#else
	int Len = StkPlWcsLen((wchar_t*)Utf32) + 1;
	wchar_t* Wc = new wchar_t[Len];
	StkPlWcsCpy(Wc, Len, (wchar_t*)Utf32);
	return Wc;
#endif
}

wchar_t* StkPlCreateWideCharFromUtf8(const char* Utf8)
{
#ifdef WIN32
	return (wchar_t*)StkPlCreateUtf16FromUtf8(Utf8);
#else
	return (wchar_t*)StkPlCreateUtf32FromUtf8(Utf8);
#endif
}

char* StkPlCreateUtf8FromWideChar(const wchar_t* Wc)
{
#ifdef WIN32
	return StkPlCreateUtf8FromUtf16((char16_t*)Wc);
#else
	return StkPlCreateUtf8FromUtf32((char32_t*)Wc);
#endif
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// APIs for etc

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

int StkPlSwScanf(const wchar_t* Str, const wchar_t* Format, ...)
{
	va_list va;
	va_start(va, Format);
	int Ret = vswscanf(Str, Format, va);
	va_end(va);
	return Ret;
}

int StkPlSystem(const char* Command)
{
	return system(Command);
}

void StkPlExit(int Status)
{
	exit(Status);
}

int StkPlRand()
{
	static bool InitFlag = true;
	if (InitFlag) {
		InitFlag = false;
		srand((unsigned int)time(NULL));
	}
	return rand();
}

void StkPlSleepMs(int MilliSec)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(MilliSec));
}

#ifdef WIN32

// This API is called from only StkPlExec
void TerminateChildProcess(int TargetProcessId, int ExitCode)
{
	PROCESSENTRY32 ProEnt;
	ZeroMemory(&ProEnt, sizeof(ProEnt));
	HANDLE ProcSnapHndl = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (ProcSnapHndl != INVALID_HANDLE_VALUE) {
		ProEnt.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(ProcSnapHndl, &ProEnt)) {
			do {
				if (ProEnt.th32ParentProcessID == TargetProcessId) {
					TerminateChildProcess(ProEnt.th32ProcessID, ExitCode);
					HANDLE ProcHndl = OpenProcess(PROCESS_TERMINATE, FALSE, ProEnt.th32ProcessID);
					TerminateProcess(ProcHndl, ExitCode);
					WaitForSingleObject(ProcHndl, INFINITE);
					CloseHandle(ProcHndl);
				}
			} while (Process32Next(ProcSnapHndl, &ProEnt));
		}
		CloseHandle(ProcSnapHndl);
	}
}
#endif

// Execute command as child process
// CmdLine [in] : command line
// TimeoutInMs [in] : Timeout interval in millisec
// Result [out] : (0:Normal, -1:Abnormal)
// Return : (-1:internal error, -2:timeout, otherwise:exist code of the command)
int StkPlExec(const wchar_t* CmdLine, int TimeoutInMs, int* Result)
{
	*Result = 0;
#ifdef WIN32
	wchar_t CmdLineBuf[FILENAME_MAX];
	StkPlWcsCpy(CmdLineBuf, FILENAME_MAX, CmdLine);
	PROCESS_INFORMATION pi_cmd;
	STARTUPINFO si_cmd;
	ZeroMemory(&si_cmd, sizeof(si_cmd));
	si_cmd.cb = sizeof(si_cmd);
	int Ret = CreateProcess(NULL, CmdLineBuf, NULL, NULL, false, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si_cmd, &pi_cmd);
	if (Ret == 0) {
		CloseHandle(pi_cmd.hProcess);
		*Result = -1;
		return -1;
	}
	int RetSig = WaitForSingleObject(pi_cmd.hProcess, TimeoutInMs);
	if (RetSig == WAIT_TIMEOUT) {
		TerminateChildProcess(pi_cmd.dwProcessId, -2);
		TerminateProcess(pi_cmd.hProcess, -2);
		*Result = -1;
	}

	DWORD ExitCode = 0;
	WaitForSingleObject(pi_cmd.hProcess, INFINITE);
	StkPlSleepMs(1000);
	GetExitCodeProcess(pi_cmd.hProcess, &ExitCode);
	CloseHandle(pi_cmd.hThread);
	CloseHandle(pi_cmd.hProcess);
	return ExitCode;
#else
	int ExitCode = 0;
	pid_t RetFork = fork();
	if (RetFork < 0) {
		return -1;
	} else if (RetFork == 0) {
		// child process
		setpgid(0, 0);
		char* CmdLineU8 = StkPlCreateUtf8FromWideChar(CmdLine);

		char* Argv[100];
		int Loop = 0;
		int CurrentPos = 0;
		for (; Loop < 100; Loop++) {
			bool OptFound = false;
			bool ExitFlag = false;
			bool DqFlag = false;
			while (true) {
				if (CmdLineU8[CurrentPos] == ' ') {
					CurrentPos++;
				} else if (CmdLineU8[CurrentPos] == '\"' && CmdLineU8[CurrentPos + 1] != '\0') {
					Argv[Loop] = &CmdLineU8[CurrentPos + 1];
					OptFound = true;
					DqFlag = true;
					CurrentPos += 2;
					break;
				} else if (CmdLineU8[CurrentPos] == '\"' && CmdLineU8[CurrentPos + 1] == '\0') {
					break;
				} else if (CmdLineU8[CurrentPos] == '\0') {
					ExitFlag = true;
					break;
				} else {
					Argv[Loop] = &CmdLineU8[CurrentPos];
					OptFound = true;
					CurrentPos++;
					break;
				}
			}
			while (OptFound) {
				if (DqFlag == false && CmdLineU8[CurrentPos] == ' ') {
					CmdLineU8[CurrentPos] = '\0';
					CurrentPos++;
					break;
				} else if (DqFlag == true && CmdLineU8[CurrentPos] == '\"') {
					CmdLineU8[CurrentPos] = '\0';
					CurrentPos++;
					break;
				} else if (CmdLineU8[CurrentPos] == '\0') {
					ExitFlag = true;
					break;
				} else {
					CurrentPos++;
				}
			}
			if (ExitFlag) {
				break;
			}
		}
		Argv[Loop] = NULL;

		execv(Argv[0], Argv);
		delete CmdLineU8;
		StkPlExit(0);
	} else {
		// parent process
		int SumOfSleep = 0;
		pid_t RetWait = 0;
		do {
			int Status = 0;
			RetWait = waitpid(RetFork, &Status, WNOHANG);
			if (RetWait == -1) {
				// Abnormal end
				*Result = -1;
				ExitCode = -1;
				break;
			} else if (RetWait == 0) {
				// Child process is still running.
				StkPlSleepMs(100);
				SumOfSleep += 100;
				if (SumOfSleep >= TimeoutInMs) {
					// Kill child process
					kill(-RetFork, SIGTERM);
					StkPlSleepMs(2000);
					kill(-RetFork, SIGKILL);
					pid_t RetChild = waitpid(RetFork, &Status, WUNTRACED);
					*Result = -1;
					ExitCode = -2;
					break;
				}
			} else {
				// Child process ended.
				if (WIFEXITED(Status)) {
					ExitCode = WEXITSTATUS(Status);
					break;
				} else {
					ExitCode = -1;
				}
			}
			StkPlSleepMs(10);
		} while (RetWait == 0);
		return ExitCode;
	}
	return -1;
#endif
}

void StkPlLockCs(StkPlCriticalSection* Cs)
{
	Cs->lock();
}

void StkPlUnlockCs(StkPlCriticalSection* Cs)
{
	Cs->unlock();
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

int StkPlGetUsedMemorySizeOfCurrentProcess()
{
#ifdef WIN32
	DWORD dwProcessID = GetCurrentProcessId();
	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc = { 0 };

	long Size = 0;
	if ((hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, false, dwProcessID)) != NULL) {
		if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
			Size = (long)pmc.WorkingSetSize;
		}
		CloseHandle(hProcess);
	}
	Size /= 1024;
	return Size;
#else
	FILE *fp;
	char ProcInfo[64];
	char Buffer[4096];
	sprintf(ProcInfo, "/proc/%d/status", getpid());
	if ((fp = fopen(ProcInfo, "r")) == NULL) {
		return -1;
	}
	int ActualFileSize = fread(Buffer, sizeof(char), 4096, fp);
	fclose(fp);
	char* Ptr = strstr(Buffer, "VmRSS:");
	if (Ptr == NULL) {
		return -1;
	}
	char DummyStr[32] = "";
	int VmSize = 0;
	sscanf(Ptr, "%s %d", DummyStr, &VmSize);
	return VmSize;
#endif
}

int StkPlGetUsedVmSizeOfCurrentProcess()
{
#ifdef WIN32
	DWORD dwProcessID = GetCurrentProcessId();
	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc = { 0 };

	long Size = 0;
	if ((hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, false, dwProcessID)) != NULL) {
		if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
			Size = (long)pmc.PagefileUsage;
		}
		CloseHandle(hProcess);
	}
	Size /= 1024;
	return Size;
#else
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
#endif
}

void StkPlGetHostName(wchar_t* HostName, size_t Size)
{
#ifdef WIN32
	DWORD SizeTmp = (DWORD)Size;
	GetComputerName(HostName, &SizeTmp);
#else
	char HostNameTmp[1024];
	gethostname(HostNameTmp, 1024);
	StkPlConvUtf8ToWideChar(HostName, Size, HostNameTmp);
#endif
}

bool StkPlCheckHostName(const wchar_t* HostName)
{
	bool ValidFlag = true;
	const wchar_t* HostNamePtr = HostName;
	while (*HostNamePtr) {
		if (*HostNamePtr >= L'a' && *HostNamePtr <= L'z') {
		} else if (*HostNamePtr >= L'A' && *HostNamePtr <= L'Z') {
		} else if (*HostNamePtr >= L'0' && *HostNamePtr <= L'9') {
		} else if (*HostNamePtr == L'-') {
		} else if (*HostNamePtr == L'.') {
		} else if (*HostNamePtr == L':') { // For IPv6
		} else {
			ValidFlag = false;
		}
		HostNamePtr++;
	}
	return ValidFlag;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// APIs for date and time


long long StkPlGetTickCount()
{
#ifdef WIN32
	return GetTickCount();
#else
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (long long)(ts.tv_nsec / 1000000) + ((long long)ts.tv_sec * 1000ull);
#endif
}

void StkPlGetTimeInRfc2822(char Date[64], bool IsLocalTime)
{
#ifdef WIN32
	struct tm TmTime;
	__int64 Ltime;
	_time64(&Ltime);
	if (IsLocalTime) {
		_localtime64_s(&TmTime, &Ltime);
	} else {
		_gmtime64_s(&TmTime, &Ltime);
	}
	int DiffHour = 0;
	int DiffMinute = 0;
	bool IsPlus = false;
	if (IsLocalTime) {
		struct _timeb TimeB;
		_ftime64_s(&TimeB);
		IsPlus = TimeB.timezone > 0 ? true : false;
		DiffHour = abs(TimeB.timezone) / 60;
		DiffMinute = abs(TimeB.timezone) % 60;
	}
	char Diff[16] = "";
	if (!IsPlus) {
		sprintf_s(Diff, 16, "+%02d%02d", DiffHour, DiffMinute);
	} else {
		sprintf_s(Diff, 16, "-%02d%02d", DiffHour, DiffMinute);
	}
	
	char MonStr[12][4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	char WdayStr[7][4] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
	sprintf_s(Date, 64, "%s, %02d %s %d %02d:%02d:%02d %s",
		WdayStr[TmTime.tm_wday],
		TmTime.tm_mday,
		MonStr[TmTime.tm_mon],
		TmTime.tm_year + 1900,
		TmTime.tm_hour,
		TmTime.tm_min,
		TmTime.tm_sec,
		Diff);
#else
	struct tm* TmTime;
	time_t Ltime;
	time(&Ltime);
	if (IsLocalTime) {
		TmTime = localtime(&Ltime);
	} else {
		TmTime = gmtime(&Ltime);
	}
	int DiffHour = 0;
	int DiffMinute = 0;
	bool IsPlus = false;
	if (IsLocalTime) {
		IsPlus = timezone > 0 ? true : false;
		DiffHour = abs(timezone) / 60 / 60;
		DiffMinute = abs(timezone) / 60 % 60;
	}
	char Diff[16] = "";
	if (!IsPlus) {
		sprintf(Diff, "+%02d%02d", DiffHour, DiffMinute);
	} else {
		sprintf(Diff, "-%02d%02d", DiffHour, DiffMinute);
	}

	char MonStr[12][4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	char WdayStr[7][4] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
	sprintf(Date, "%s, %02d %s %d %02d:%02d:%02d %s",
		WdayStr[TmTime->tm_wday],
		TmTime->tm_mday,
		MonStr[TmTime->tm_mon],
		TmTime->tm_year + 1900,
		TmTime->tm_hour,
		TmTime->tm_min,
		TmTime->tm_sec,
		Diff);
#endif
}

void StkPlGetWTimeInRfc2822(wchar_t Date[64], bool IsLocalTime)
{
	char DateTmp[64];
	StkPlGetTimeInRfc2822(DateTmp, IsLocalTime);
	StkPlConvUtf8ToWideChar(Date, 64, DateTmp);
}

void StkPlGetTimeInIso8601(char Date[64], bool IsLocalTime)
{
#ifdef WIN32
	struct tm TmTime;
	__int64 Ltime;
	_time64(&Ltime);
	if (IsLocalTime) {
		_localtime64_s(&TmTime, &Ltime);
	} else {
		_gmtime64_s(&TmTime, &Ltime);
	}
	int DiffHour = 0;
	int DiffMinute = 0;
	bool IsPlus = false;
	if (IsLocalTime) {
		struct _timeb TimeB;
		_ftime64_s(&TimeB);
		IsPlus = TimeB.timezone > 0 ? true : false;
		DiffHour = abs(TimeB.timezone) / 60;
		DiffMinute = abs(TimeB.timezone) % 60;
	}
	char Diff[16] = "";
	if (!IsPlus) {
		sprintf_s(Diff, 16, "+%02d:%02d", DiffHour, DiffMinute);
	} else {
		sprintf_s(Diff, 16, "-%02d:%02d", DiffHour, DiffMinute);
	}

	sprintf_s(Date, 64, "%d-%02d-%02dT%02d:%02d:%02d%s", TmTime.tm_year + 1900, TmTime.tm_mon + 1, TmTime.tm_mday, TmTime.tm_hour, TmTime.tm_min, TmTime.tm_sec, Diff);
#else
	tzset();
	struct tm* TmTime;
	time_t Ltime;
	time(&Ltime);
	if (IsLocalTime) {
		TmTime = localtime(&Ltime);
	} else {
		TmTime = gmtime(&Ltime);
	}
	int DiffHour = 0;
	int DiffMinute = 0;
	bool IsPlus = false;
	if (IsLocalTime) {
		IsPlus = timezone > 0 ? true : false;
		DiffHour = abs(timezone) / 60 / 60;
		DiffMinute = abs(timezone) / 60 % 60;
	}
	char Diff[16] = "";
	if (!IsPlus) {
		sprintf(Diff, "+%02d:%02d", DiffHour, DiffMinute);
	} else {
		sprintf(Diff, "-%02d:%02d", DiffHour, DiffMinute);
	}

	sprintf(Date, "%d-%02d-%02dT%02d:%02d:%02d%s", TmTime->tm_year + 1900, TmTime->tm_mon + 1, TmTime->tm_mday, TmTime->tm_hour, TmTime->tm_min, TmTime->tm_sec, Diff);
#endif
}

void StkPlGetWTimeInIso8601(wchar_t Date[64], bool IsLocalTime)
{
	char DateTmp[64];
	StkPlGetTimeInIso8601(DateTmp, IsLocalTime);
	StkPlConvUtf8ToWideChar(Date, 64, DateTmp);
}

void StkPlGetTimeInOldFormat(char Date[64], bool IsLocalTime)
{
#ifdef WIN32
	struct tm TmTime;
	__int64 Ltime;
	_time64(&Ltime);
	if (IsLocalTime) {
		_localtime64_s(&TmTime, &Ltime);
	} else {
		_gmtime64_s(&TmTime, &Ltime);
	}
	sprintf_s(Date, 64, "%d-%02d-%02d %02d:%02d:%02d", TmTime.tm_year + 1900, TmTime.tm_mon + 1, TmTime.tm_mday, TmTime.tm_hour, TmTime.tm_min, TmTime.tm_sec);
#else
	struct tm* TmTime;
	time_t Ltime;
	time(&Ltime);
	if (IsLocalTime) {
		TmTime = localtime(&Ltime);
	} else {
		TmTime = gmtime(&Ltime);
	}
	sprintf(Date, "%d-%02d-%02d %02d:%02d:%02d", TmTime->tm_year + 1900, TmTime->tm_mon + 1, TmTime->tm_mday, TmTime->tm_hour, TmTime->tm_min, TmTime->tm_sec);
#endif
}

void StkPlGetWTimeInOldFormat(wchar_t Date[64], bool IsLocalTime)
{
	char DateTmp[64];
	StkPlGetTimeInOldFormat(DateTmp, IsLocalTime);
	StkPlConvUtf8ToWideChar(Date, 64, DateTmp);
}

long long StkPlGetUnixTimeFromRfc2822(const char StrRfc2822[64])
{
	// "%s, %02d %s %d %02d:%02d:%02d %s", wday, mday, mon, year, hour, min, sec, diff
	if (StrRfc2822 == NULL) {
		return -1;
	}
	char workStr[64] = "";
	StkPlStrCpy(workStr, 64, StrRfc2822);
	char* elem[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
	char deli[8][5] = {", ", " ", " ", " ", ":", ":", " ", " "};

	int Ptr = 0;
	for (int Index = 0; Index < 8; Index++) {
		elem[Index] = &workStr[Ptr];
		for (; Ptr < 63 && workStr[Ptr] != '\0'; Ptr++) {
			if (StkPlStrNCmp(&workStr[Ptr], deli[Index], strlen(deli[Index])) == 0) {
				break;
			}
		}
		if (Ptr == 63 || workStr[Ptr] == '\0') {
			if (Index == 7) {
				break;
			}
			return -1;
		} else {
			workStr[Ptr] = '\0';
			Ptr += (int)strlen(deli[Index]);
			continue;
		}
	}
	struct tm Origin;
	if (elem[0] != NULL) {
		const char *TmpWday[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
		int IntWday = -1;
		for (int Loop = 0; Loop < 7; Loop++) {
			if (strcmp(TmpWday[Loop], elem[0]) == 0) {
				IntWday = Loop;
			}
		}
		if (IntWday == -1) {
			return -1;
		}
		Origin.tm_wday = IntWday;
	}
	if (elem[1] != NULL) {
		Origin.tm_mday = atoi(elem[1]);
	}
	if (elem[2] != NULL) {
		const char* TmpMon[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
		int IntMon = -1;
		for (int Loop = 0; Loop < 12; Loop++) {
			if (strcmp(TmpMon[Loop], elem[2]) == 0) {
				IntMon = Loop;
			}
		}
		if (IntMon == -1) {
			return -1;
		}
		Origin.tm_mon = IntMon;
	}
	if (elem[3] != NULL) {
		Origin.tm_year = atoi(elem[3]) - 1900;
	}
	if (elem[4] != NULL) {
		Origin.tm_hour = atoi(elem[4]);
	}
	if (elem[5] != NULL) {
		Origin.tm_min = atoi(elem[5]);
	}
	if (elem[6] != NULL) {
		Origin.tm_sec = atoi(elem[6]);
	}
	Origin.tm_isdst = -1;
	Origin.tm_yday = 0;
	return mktime(&Origin);
}

long long StkPlGetUnixTimeFromOldFormat(const char StrOldFormat[64])
{
	// "%d-%02d-%02d %02d:%02d:%02d", year, mon, mday, hour, min, sec
	if (StrOldFormat == NULL) {
		return -1;
	}
	char workStr[64] = "";
	StkPlStrCpy(workStr, 64, StrOldFormat);
	char* elem[6] = { NULL, NULL, NULL, NULL, NULL, NULL };
	char deli[6][5] = { "-", "-", " ", ":", ":", " "};

	int Ptr = 0;
	for (int Index = 0; Index < 6; Index++) {
		elem[Index] = &workStr[Ptr];
		for (; Ptr < 63 && workStr[Ptr] != '\0'; Ptr++) {
			if (StkPlStrNCmp(&workStr[Ptr], deli[Index], strlen(deli[Index])) == 0) {
				break;
			}
		}
		if (Ptr == 63 || workStr[Ptr] == '\0') {
			if (Index == 5) {
				break;
			}
			return -1;
		} else {
			workStr[Ptr] = '\0';
			Ptr += (int)strlen(deli[Index]);
			continue;
		}
	}
	struct tm Origin;

	if (elem[0] != NULL) {
		Origin.tm_year = atoi(elem[0]) - 1900;
	}
	if (elem[1] != NULL) {
		Origin.tm_mon = atoi(elem[1]) - 1;
	}
	if (elem[2] != NULL) {
		Origin.tm_mday = atoi(elem[2]);
	}
	if (elem[3] != NULL) {
		Origin.tm_hour = atoi(elem[3]);
	}
	if (elem[4] != NULL) {
		Origin.tm_min = atoi(elem[4]);
	}
	if (elem[5] != NULL) {
		Origin.tm_sec = atoi(elem[5]);
	}
	Origin.tm_isdst = -1;
	Origin.tm_yday = 0;
	Origin.tm_wday = 0;
	return mktime(&Origin);
}

long long StkPlGetUnixTimeFromIso8601(const char StrIso8601[64])
{
	// %d-%02d-%02dT%02d:%02d:%02d%s, year, mon, mday, hour, min, sec, diff
	if (StrIso8601 == NULL) {
		return -1;
	}
	char workStr[64] = "";
	StkPlStrCpy(workStr, 64, StrIso8601);
	char* elem[7] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };
	char deli[7][5] = { "-", "-", "T", ":", ":", "@", " "};

	int Ptr = 0;
	for (int Index = 0; Index < 7; Index++) {
		elem[Index] = &workStr[Ptr];
		for (; Ptr < 63 && workStr[Ptr] != '\0'; Ptr++) {
			if (deli[Index][0] == '@' && (workStr[Ptr] == '-' || workStr[Ptr] == '+')) {
				break;
			}
			if (StkPlStrNCmp(&workStr[Ptr], deli[Index], strlen(deli[Index])) == 0) {
				break;
			}
		}
		if (Ptr == 63 || workStr[Ptr] == '\0') {
			if (Index == 6) {
				break;
			}
			return -1;
		} else {
			workStr[Ptr] = '\0';
			Ptr += (int)strlen(deli[Index]);
			continue;
		}
	}
	struct tm Origin;

	if (elem[0] != NULL) {
		Origin.tm_year = atoi(elem[0]) - 1900;
	}
	if (elem[1] != NULL) {
		Origin.tm_mon = atoi(elem[1]) - 1;
	}
	if (elem[2] != NULL) {
		Origin.tm_mday = atoi(elem[2]);
	}
	if (elem[3] != NULL) {
		Origin.tm_hour = atoi(elem[3]);
	}
	if (elem[4] != NULL) {
		Origin.tm_min = atoi(elem[4]);
	}
	if (elem[5] != NULL) {
		Origin.tm_sec = atoi(elem[5]);
	}
	Origin.tm_isdst = -1;
	Origin.tm_yday = 0;
	Origin.tm_wday = 0;
	return mktime(&Origin);
}

long long StkPlGetUnixTimeFromRfc2822W(const wchar_t StrRfc2822[64])
{
	char NewDateTime[64];
	StkPlConvWideCharToUtf8(NewDateTime, 64, StrRfc2822);
	return StkPlGetUnixTimeFromRfc2822(NewDateTime);
}

long long StkPlGetUnixTimeFromOldFormatW(const wchar_t StrOldFormat[64])
{
	char NewDateTime[64];
	StkPlConvWideCharToUtf8(NewDateTime, 64, StrOldFormat);
	return StkPlGetUnixTimeFromOldFormat(NewDateTime);
}

long long StkPlGetUnixTimeFromIso8601W(const wchar_t StrIso8601[64])
{
	char NewDateTime[64];
	StkPlConvWideCharToUtf8(NewDateTime, 64, StrIso8601);
	return StkPlGetUnixTimeFromIso8601(NewDateTime);
}

long long StkPlGetTime()
{
#ifdef WIN32
	__int64 Ltime;
	_time64(&Ltime);
	return (long long)Ltime;
#else
	time_t Ltime;
	time(&Ltime);
	return (long long)Ltime;
#endif
}

void StkPlGetTime(int* Year, int* Mon, int* Day, int* Hour, int* Min, int* Sec, bool IsLocalTime)
{
#ifdef WIN32
	struct tm TmTime;
	__int64 Ltime;
	_time64(&Ltime);
	if (IsLocalTime) {
		_localtime64_s(&TmTime, &Ltime);
	} else {
		_gmtime64_s(&TmTime, &Ltime);
	}
	*Year = TmTime.tm_year + 1900;
	*Mon = TmTime.tm_mon + 1;
	*Day = TmTime.tm_mday;
	*Hour = TmTime.tm_hour;
	*Min = TmTime.tm_min;
	*Sec = TmTime.tm_sec;
#else
	struct tm* TmTime;
	time_t Ltime;
	time(&Ltime);
	if (IsLocalTime) {
		TmTime = localtime(&Ltime);
	} else {
		TmTime = gmtime(&Ltime);
	}
	*Year = TmTime->tm_year + 1900;
	*Mon = TmTime->tm_mon + 1;
	*Day = TmTime->tm_mday;
	*Hour = TmTime->tm_hour;
	*Min = TmTime->tm_min;
	*Sec = TmTime->tm_sec;
#endif
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// APIs for file access

// Change current directory
// PathToDir [in] : Path to directory where current directory is changed.
// Return : 0: Success, -1 Failure
int ChangeCurrentDirectory(const wchar_t* PathToDir)
{
	int Ret = 0;
#ifdef WIN32
	Ret = SetCurrentDirectory(PathToDir);
	if (Ret == 0) {
		Ret = -1;
	} else {
		Ret = 0;
	}
#else
	char* PathToDirUtf8 = StkPlCreateUtf8FromWideChar(PathToDir);
	Ret = chdir(PathToDirUtf8);
	delete PathToDirUtf8;
#endif
	return Ret;
}

int StkPlCreateDirectory(const wchar_t* DirName)
{
#ifdef WIN32
	bool Res = std::filesystem::create_directory(DirName);
	if (Res) {
		return 0;
	} else {
		return -1;
	}
#else
	char* PathToCreateDirUtf8 = StkPlCreateUtf8FromWideChar(DirName);
	int Res = mkdir(PathToCreateDirUtf8, 0777);
	delete PathToCreateDirUtf8;
	if (Res == 0) {
		return 0;
	} else {
		return -1;
	}
#endif
}

int StkPlAddSeparator(wchar_t* Path, size_t PathSize)
{
	size_t TmpLen = StkPlWcsLen(Path);
#ifdef WIN32
	if (Path[TmpLen - 1] != L'\\') {
		StkPlWcsCat(Path, PathSize, L"\\");
	}
#else
	if (Path[TmpLen - 1] != L'/') {
		StkPlWcsCat(Path, PathSize, L"/");
	}
#endif
	return 0;
}

bool StkPlDeleteFile(wchar_t* Path)
{
#ifdef WIN32
	return std::filesystem::remove(Path);
#else
	char* removeTargetUtf8 = StkPlCreateUtf8FromWideChar(Path);
	int Ret = remove(removeTargetUtf8);
	delete removeTargetUtf8;
	if (Ret == 0) {
		return false;
	} else {
		return true;
	}
#endif
}

// Get full path from the specified file name.
// FileName [in] : File name which you want to get absolute path for. Do not specify path. Specify only file name. The file needs to be placed in the same folder of executing module.
// FullPath [out] : Acquired full path for the specified file.
// Return : Always zero
int StkPlGetFullPathFromFileName(const wchar_t* FileName, wchar_t FullPath[FILENAME_MAX])
{
#ifdef WIN32
	GetModuleFileName(NULL, FullPath, FILENAME_MAX - 1);
	std::filesystem::path CurPath = FullPath;
	std::filesystem::path NewPath = CurPath.parent_path() / FileName;
	wcscpy_s(FullPath, FILENAME_MAX, NewPath.c_str());
	return 0;
#else
	char c_full_path[FILENAME_MAX * 6]; // Assuming string in UTF-8 is returned.
	readlink("/proc/self/exe", c_full_path, sizeof(c_full_path) - 1);
	char* FileNameInUtf8 = StkPlCreateUtf8FromWideChar(FileName);
	for (char* Ptr = c_full_path + strlen(c_full_path) - 2; Ptr > c_full_path; Ptr--) {
		if (*Ptr == '/') {
			*Ptr = '\0';
			break;
		}
	}
	strncat(c_full_path, "/", sizeof(c_full_path) - 1);
	strncat(c_full_path, FileNameInUtf8, sizeof(c_full_path) - 1);
	delete FileNameInUtf8;
	StkPlConvUtf8ToWideChar(FullPath, FILENAME_MAX, c_full_path);
	return 0;
#endif
}

// Get full path without file name for the specified full path file name.
// PathWithFileName [in] : Full path which contains file name.
// Output [out] : Acquired full path without file name.
// Return : 0:Success, -1:Failure
int StkPlGetFullPathWithoutFileName(wchar_t* PathWithFileName, wchar_t Output[FILENAME_MAX])
{
	if (PathWithFileName == NULL || Output == NULL) {
		return -1;
	}
	if (wcscmp(PathWithFileName, L"") == 0) {
		return -1;
	}
#ifdef WIN32
	wcsncpy_s(Output, FILENAME_MAX, PathWithFileName, _TRUNCATE);
#else
	wcscpy(Output, PathWithFileName);
#endif
	wchar_t* Addr = NULL;
	for (Addr = Output + wcslen(Output); Addr > Output; Addr--) {
		if (*Addr == L'\\' || *Addr == L'/') {
			break;
		}
	}
	if (Addr == Output) {
		return -1;
	}
	*Addr = L'\0';
	return 0;
}

size_t StkPlGetFileSize(const wchar_t FilePath[FILENAME_MAX])
{
#ifdef WIN32
	uintmax_t FileSize = 0;
	try {
		FileSize = std::filesystem::file_size(FilePath);
	} catch (std::filesystem::filesystem_error ex) {
		return -1;
	}
	if (FileSize == 0) {
		return 0;
	}
	return (size_t)FileSize;
#else
	uintmax_t FileSize = 0;
	char* FilePathUtf8 = StkPlCreateUtf8FromWideChar(FilePath);
	struct stat TmpSt;
	if (stat(FilePathUtf8, &TmpSt) != 0) {
		delete FilePathUtf8;
		return -1;
	}
	FileSize = TmpSt.st_size;
	delete FilePathUtf8;
	if (FileSize == 0) {
		return 0;
	}
	return (size_t)FileSize;
#endif
}

int StkPlReadFile(const wchar_t FilePath[FILENAME_MAX], char* Buffer, size_t FileSize)
{
#ifdef WIN32
	HANDLE ReadFileHndl = CreateFile(FilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (ReadFileHndl == INVALID_HANDLE_VALUE) {
		return -1;
	}

	DWORD TmpSize = 0;
	if (ReadFile(ReadFileHndl, (LPVOID)Buffer, (DWORD)FileSize, &TmpSize, NULL) == 0) {
		CloseHandle(ReadFileHndl);
		return -1;
	}

	CloseHandle(ReadFileHndl);
	return (int)TmpSize;
#else
	char* FileNameUtf8 = StkPlCreateUtf8FromWideChar(FilePath);
	FILE *fp = fopen(FileNameUtf8, "r");
	if (fp == NULL) {
		if (FileNameUtf8 != NULL) {
			delete FileNameUtf8;
		}
		return -1;
	}
	int actual_filesize = fread(Buffer, sizeof(char), (int)FileSize, fp);
	fclose(fp);
	delete FileNameUtf8;
	return actual_filesize;
#endif
}

int StkPlWriteFile(const wchar_t FilePath[FILENAME_MAX], char* Buffer, size_t FileSize)
{
#ifdef WIN32
	HANDLE WriteFileHndl = CreateFile(FilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (WriteFileHndl == INVALID_HANDLE_VALUE) {
		return -1;
	}

	DWORD TmpSize = 0;
	if (WriteFile(WriteFileHndl, (LPVOID)Buffer, (DWORD)FileSize, &TmpSize, NULL) == 0) {
		CloseHandle(WriteFileHndl);
		return -1;
	}

	CloseHandle(WriteFileHndl);
	return TmpSize;
#else
	char* FileNameUtf8 = StkPlCreateUtf8FromWideChar(FilePath);
	FILE *fp = fopen(FileNameUtf8, "w");
	if (fp == NULL) {
		if (FileNameUtf8 != NULL) {
			delete FileNameUtf8;
		}
		return -1;
	}
	int actual_filesize = fwrite(Buffer, sizeof(char), (int)FileSize, fp);
	fclose(fp);
	delete FileNameUtf8;
	return actual_filesize;
#endif
}

void* StkPlOpenFileForRead(const wchar_t FilePath[FILENAME_MAX])
{
#ifdef WIN32
	HANDLE FileHndl = CreateFile(FilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (FileHndl == INVALID_HANDLE_VALUE) {
		return NULL;
	};
	return (void*)FileHndl;
#else
	char* FileNameUtf8 = StkPlCreateUtf8FromWideChar(FilePath);
	FILE *fp = fopen(FileNameUtf8, "r");
	if (fp == NULL) {
		return NULL;
	}
	delete FileNameUtf8;
	return fp;
#endif
}

void* StkPlOpenFileForWrite(const wchar_t FilePath[FILENAME_MAX], bool AddFlag)
{
#ifdef WIN32
	DWORD Type = CREATE_ALWAYS;
	if (AddFlag) {
		Type = OPEN_ALWAYS;
	}
	HANDLE FileHndl = CreateFile(FilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, Type, FILE_ATTRIBUTE_NORMAL, NULL);
	if (FileHndl == INVALID_HANDLE_VALUE) {
		return NULL;
	};
	return (void*)FileHndl;
#else
	char Type[8] = "w";
	if (AddFlag) {
		strcpy(Type, "a");
	}
	char* FileNameUtf8 = StkPlCreateUtf8FromWideChar(FilePath);
	FILE *fp = fopen(FileNameUtf8, Type);
	if (fp == NULL) {
		return NULL;
	}
	delete FileNameUtf8;
	return fp;
#endif
}

void StkPlCloseFile(void* FileHndl)
{
#ifdef WIN32
	CloseHandle((HANDLE)FileHndl);
#else
	fclose((FILE*)FileHndl);
#endif
}

int StkPlRead(void* FileHndl, char* Ptr, size_t Size, size_t* ActSize)
{
#ifdef WIN32
	DWORD TmpSize = 0;
	if (ReadFile(FileHndl, (LPVOID)Ptr, (DWORD)Size, &TmpSize, NULL) == 0) {
		return 0;
	}
	*ActSize = TmpSize;
	return 1;
#else
	size_t TmpSize = fread(Ptr, sizeof(char), (int)Size, (FILE*)FileHndl);
	*ActSize = TmpSize;
	return 1;
#endif
}

int StkPlWrite(void* FileHndl, char* Ptr, size_t Size, size_t* ActSize)
{
#ifdef WIN32
	DWORD TmpSize = 0;
	if (WriteFile(FileHndl, (LPVOID)Ptr, (DWORD)Size, &TmpSize, NULL) == 0) {
		return 0;
	}
	*ActSize = TmpSize;
	return 1;
#else
	size_t TmpSize = fwrite(Ptr, sizeof(char), (int)Size, (FILE*)FileHndl);
	if (TmpSize < Size) {
		return 0;
	}
	*ActSize = TmpSize;
	return 1;
#endif
}

void StkPlFlush(void* FileHndl)
{
#ifdef WIN32
	FlushFileBuffers(FileHndl);
#else
	fflush((FILE*)FileHndl);
#endif
}

void StkPlSeekFromBegin(void* FileHndl, size_t Offset)
{
#ifdef WIN32
	SetFilePointer(FileHndl, (LONG)Offset, 0, FILE_BEGIN);
#else
	fseek((FILE*)FileHndl, Offset, SEEK_SET);
#endif
}

void StkPlSeekFromEnd(void* FileHndl, size_t Offset)
{
#ifdef WIN32
	SetFilePointer(FileHndl, (LONG)Offset, 0, FILE_END);
#else
	fseek((FILE*)FileHndl, Offset, SEEK_END);
#endif
}

FileInfoList* StkPlCreateFileInfoList(const wchar_t TargetDir[FILENAME_MAX])
{
#ifdef WIN32
	wchar_t TmpTargetDir[FILENAME_MAX];
	lstrcpy(TmpTargetDir, TargetDir);
	lstrcat(TmpTargetDir, L"\\*");
	WIN32_FIND_DATAW Fd;
	HANDLE FileNameHndl = FindFirstFile(TmpTargetDir, &Fd);
	if (FileNameHndl == INVALID_HANDLE_VALUE) {
		return NULL;
	}
	FileInfoList* TopFileName = new FileInfoList;
	FileInfoList* CurFileName = TopFileName;
	for (;;) {
		// File name
		lstrcpy(CurFileName->FileName, Fd.cFileName);

		// Directory flag
		if (Fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			CurFileName->IsDir = true;
		} else {
			CurFileName->IsDir = false;
		}

		// File size
		wchar_t FullPathName[FILENAME_MAX] = L"";
		lstrcpy(FullPathName, TargetDir);
		StkPlAddSeparator(FullPathName, FILENAME_MAX);
		lstrcat(FullPathName, CurFileName->FileName);
		CurFileName->Size = StkPlGetFileSize(FullPathName);

		// Update time
		LARGE_INTEGER Date, Adjust;
		Date.HighPart = Fd.ftLastWriteTime.dwHighDateTime;
		Date.LowPart = Fd.ftLastWriteTime.dwLowDateTime;
		Adjust.QuadPart = 11644473600000 * 10000;
		Date.QuadPart -= Adjust.QuadPart;
		CurFileName->UpdateTime = Date.QuadPart / 10000000;

		// Next element
		bool Ret = FindNextFile(FileNameHndl, &Fd);
		if (Ret) {
			FileInfoList* NewFileName = new FileInfoList;
			CurFileName->Next = NewFileName;
			CurFileName = NewFileName;
		} else {
			CurFileName->Next = NULL;
			break;
		}
	}
	FindClose(FileNameHndl);
	return TopFileName;
#else
	char* TmpPath = StkPlCreateUtf8FromWideChar(TargetDir);
	DIR* DirPtr = opendir(TmpPath);
	delete TmpPath;
	if (DirPtr == NULL) {
		return NULL;
	}
	dirent* Entry = readdir(DirPtr);
	FileInfoList* TopFileName = new FileInfoList;
	FileInfoList* CurFileName = TopFileName;
	while (Entry) {
		// File name
		wchar_t* TmpFileName = StkPlCreateWideCharFromUtf8(Entry->d_name);
		wcscpy(CurFileName->FileName, TmpFileName);
		delete TmpFileName;

		// Directory flag
		if (Entry->d_type == DT_DIR) {
			CurFileName->IsDir = true;
		} else {
			CurFileName->IsDir = false;
		}

		// File size
		wchar_t FullPathName[FILENAME_MAX] = L"";
		wcscpy(FullPathName, TargetDir);
		StkPlAddSeparator(FullPathName, FILENAME_MAX);
		wcscat(FullPathName, CurFileName->FileName);
		CurFileName->Size = StkPlGetFileSize(FullPathName);

		// Update time
		char* FullPathNameUtf8 = StkPlCreateUtf8FromWideChar(FullPathName);
		struct stat TmpSt;
		stat(FullPathNameUtf8, &TmpSt);
		delete FullPathNameUtf8;
		time_t TmpT = TmpSt.st_mtime;
		CurFileName->UpdateTime = (long long)TmpT;

		// Next element
		Entry = readdir(DirPtr);
		if (Entry != NULL) {
			FileInfoList* NewFileName = new FileInfoList;
			CurFileName->Next = NewFileName;
			CurFileName = NewFileName;
		} else {
			CurFileName->Next = NULL;
			break;
		}
	}
	closedir(DirPtr);
	return TopFileName;
#endif
}

void StkPlDeleteFileInfoList(FileInfoList* CurFileName)
{
	do {
		FileInfoList* NextFileName = CurFileName->Next;
		delete CurFileName;
		CurFileName = NextFileName;
	} while (CurFileName);
}
