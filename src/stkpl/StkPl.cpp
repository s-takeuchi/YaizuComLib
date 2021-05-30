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
#else
#include <unistd.h>
#include <dirent.h>
#include <experimental/filesystem>
#include <sys/timeb.h>
#include <sys/stat.h>
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
	if (strstr(TmpLoc, "ja_JP") != NULL || strstr(TmpLoc, "Japanese_Japan") != NULL) {
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
	setlocale(LC_CTYPE, "");
	char* ConfLc = setlocale(LC_CTYPE, "ja_JP.sjis");
	mbstate_t MbState;
	memset((void*)&MbState, 0, sizeof(MbState));
	int ActualSize = wcsrtombs(NULL, &Txt, 1, &MbState);
	if (ActualSize == -1 || ConfLc == NULL) {
		return NULL;
	}
	char* NewMbs = new char[ActualSize + 1];
	wcsrtombs(NewMbs, &Txt, ActualSize, &MbState);
	NewMbs[ActualSize] = '\0';
	return NewMbs;
#endif
}

wchar_t* StkPlSjisToWideChar(const char* Txt)
{
#ifdef WIN32
	int WcBufLen = MultiByteToWideChar(CP_THREAD_ACP, MB_ERR_INVALID_CHARS, (LPCSTR)Txt, -1, NULL, NULL);
	if (WcBufLen > 0) {
		wchar_t* WcTxt = new wchar_t[WcBufLen + 1];
		WcBufLen = MultiByteToWideChar(CP_THREAD_ACP, MB_ERR_INVALID_CHARS, (LPCSTR)Txt, -1, WcTxt, WcBufLen);
		WcTxt[WcBufLen] = L'\0';
		return WcTxt;
	}
	return NULL;
#else
	setlocale(LC_CTYPE, "");
	char* ConfLc = setlocale(LC_CTYPE, "ja_JP.sjis");
	mbstate_t MbState;
	memset((void*)&MbState, 0, sizeof(MbState));
	int ActualSize = mbsrtowcs(NULL, &Txt, 1, &MbState);
	if (ActualSize == -1 || ConfLc == NULL) {
		return NULL;
	}
	wchar_t* NewWcs = new wchar_t[ActualSize + 1];
	mbsrtowcs(NewWcs, &Txt, ActualSize, &MbState);
	NewWcs[ActualSize] = L'\0';
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
	delete TmpUrlBc;
	delete TmpUrlAc;
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
	delete WkUrlIn;
	delete TmpUrlBc;
	delete TmpUrlAc;
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

	long Size;
	if ((hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, false, dwProcessID)) != NULL) {
		if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
			Size = (long)pmc.WorkingSetSize;
		}
	}
	CloseHandle(hProcess);
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

	long Size;
	if ((hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, false, dwProcessID)) != NULL) {
		if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
			Size = (long)pmc.PagefileUsage;
		}
	}
	CloseHandle(hProcess);
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
#else
	bool Res = std::experimental::filesystem::create_directory(DirName);
#endif
	if (Res) {
		return 0;
	} else {
		return -1;
	}
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
	return std::experimental::filesystem::remove(Path);
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
	char c_full_path[FILENAME_MAX];
	readlink("/proc/self/exe", c_full_path, sizeof(c_full_path) - 1);
	std::experimental::filesystem::path CurPath = c_full_path;
	std::experimental::filesystem::path NewPath = CurPath.parent_path() / FileName;
	wcscpy(FullPath, NewPath.wstring().c_str());
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
	try {
		FileSize = std::experimental::filesystem::file_size(FilePathUtf8);
	} catch (std::experimental::filesystem::filesystem_error ex) {
		delete FilePathUtf8;
		return -1;
	}
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
		return -1;
	}
	char* work_dat = new char[(int)FileSize + 1];
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
		return -1;
	}
	char* work_dat = new char[(int)FileSize + 1];
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
