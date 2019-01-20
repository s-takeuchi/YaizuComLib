#include "../StkPl.h"
#include "msgproc.h"

class MessageProc::Impl
{
public:
	static const int ENG = 0;
	static const int JPN = 1;

	static const int MLANG_ENGLISH = 0;
	static const int MLANG_JAPANESE = 1;

	static const int MAX_MSG_COUNT = 10000;

	static MessageProc* Instance;

	int Mode;

	wchar_t* StkMsg[MAX_MSG_COUNT][2];
	char* StkMsgSjis[MAX_MSG_COUNT][2];
	char* StkMsgUtf8[MAX_MSG_COUNT][2];

	void Eng(int, const wchar_t*);
	void Jpn(int, const wchar_t*);

	void InitMsg();
	void AllClear();

	int GetLocale();
};

MessageProc* MessageProc::Impl::Instance = NULL;

int MessageProc::Impl::GetLocale()
{
	if (Mode == MessageProc::LOCALE_MODE_WIN32) {
		if (StkPlIsJapaneseLocale()) {
			return MessageProc::Impl::MLANG_JAPANESE;
		}
		return MessageProc::Impl::MLANG_ENGLISH;
	}
	if (Mode == MessageProc::LOCALE_MODE_WEB) {
		if (StkPlIsJapaneseLocaleFromEnv()) {
			return MessageProc::Impl::MLANG_JAPANESE;
		}
		return MessageProc::Impl::MLANG_ENGLISH;
	}
	if (Mode == MessageProc::LOCALE_MODE_ENGLISH) {
		return MessageProc::Impl::MLANG_ENGLISH;
	}
	if (Mode == MessageProc::LOCALE_MODE_JAPANESE) {
		return MessageProc::Impl::MLANG_JAPANESE;
	}
	return MessageProc::Impl::MLANG_ENGLISH;
}

void MessageProc::Impl::Eng(int Id, const wchar_t* Msg)
{
	if (StkMsg[Id][ENG] != NULL) {
		delete StkMsg[Id][ENG];
	}
	int WcBufLen = StkPlWcsLen(Msg);
	StkMsg[Id][ENG] = new wchar_t[WcBufLen + 1];
	StkPlWcsNCpy(StkMsg[Id][ENG], WcBufLen + 1, Msg, WcBufLen);
	StkMsg[Id][ENG][WcBufLen] = L'\0';

	if (StkMsgSjis[Id][ENG] != NULL) {
		delete StkMsgSjis[Id][ENG];
	}
	StkMsgSjis[Id][ENG] = (char*)StkPlWideCharToSjis(Msg);

	if (StkMsgUtf8[Id][ENG] != NULL) {
		delete StkMsgUtf8[Id][ENG];
	}
	StkMsgUtf8[Id][ENG] = (char*)StkPlWideCharToUtf8(Msg);
}

void MessageProc::Impl::Jpn(int Id, const wchar_t* Msg)
{
	if (StkMsg[Id][JPN] != NULL) {
		delete StkMsg[Id][JPN];
	}
	int WcBufLen = StkPlWcsLen(Msg);
	StkMsg[Id][JPN] = new wchar_t[WcBufLen + 1];
	StkPlWcsNCpy(StkMsg[Id][JPN], WcBufLen + 1, Msg, WcBufLen);
	StkMsg[Id][JPN][WcBufLen] = L'\0';

	if (StkMsgSjis[Id][JPN] != NULL) {
		delete StkMsgSjis[Id][JPN];
	}
	StkMsgSjis[Id][JPN] = (char*)StkPlWideCharToSjis(Msg);

	if (StkMsgUtf8[Id][JPN] != NULL) {
		delete StkMsgUtf8[Id][JPN];
	}
	StkMsgUtf8[Id][JPN] = (char*)StkPlWideCharToUtf8(Msg);
}

void MessageProc::Impl::InitMsg()
{
	for (int Loop = 0; Loop < Impl::MAX_MSG_COUNT; Loop++) {
		StkMsg[Loop][ENG] = NULL;
		StkMsg[Loop][JPN] = NULL;
		StkMsgSjis[Loop][ENG] = NULL;
		StkMsgSjis[Loop][JPN] = NULL;
		StkMsgUtf8[Loop][ENG] = NULL;
		StkMsgUtf8[Loop][JPN] = NULL;
	}
}

// This function clears all of messages without memory release of existing allocated message
void MessageProc::Impl::AllClear()
{
	for (int Loop = 0; Loop < Impl::MAX_MSG_COUNT; Loop++) {
		if (StkMsg[Loop][ENG] != NULL) {
			delete StkMsg[Loop][ENG];
		}
		if (StkMsg[Loop][JPN] != NULL) {
			delete StkMsg[Loop][JPN];
		}
		if (StkMsgSjis[Loop][ENG] != NULL) {
			delete StkMsgSjis[Loop][ENG];
		}
		if (StkMsgSjis[Loop][JPN] != NULL) {
			delete StkMsgSjis[Loop][JPN];
		}
		if (StkMsgUtf8[Loop][ENG] != NULL) {
			delete StkMsgUtf8[Loop][ENG];
		}
		if (StkMsgUtf8[Loop][JPN] != NULL) {
			delete StkMsgUtf8[Loop][JPN];
		}

		StkMsg[Loop][ENG] = NULL;
		StkMsg[Loop][JPN] = NULL;
		StkMsgSjis[Loop][ENG] = NULL;
		StkMsgSjis[Loop][JPN] = NULL;
		StkMsgUtf8[Loop][ENG] = NULL;
		StkMsgUtf8[Loop][JPN] = NULL;
	}
}

MessageProc::MessageProc()
{
	pImpl = new Impl;
	pImpl->Mode = MessageProc::LOCALE_MODE_WIN32;
	pImpl->InitMsg();
}

// Probably there is no chance to be called from any functions.
MessageProc::~MessageProc()
{
}

void MessageProc::SetLocaleMode(int SpecifiedMode)
{
	if (Impl::Instance == NULL) {
		Impl::Instance = new MessageProc();
	}
	Impl::Instance->pImpl->Mode = SpecifiedMode;
}

wchar_t* MessageProc::GetMsg(int Id)
{
	if (Impl::Instance == NULL) {
		Impl::Instance = new MessageProc();
	}
	return Impl::Instance->pImpl->StkMsg[Id][Impl::Instance->pImpl->GetLocale()];
}

wchar_t* MessageProc::GetMsgEng(int Id)
{
	if (Impl::Instance == NULL) {
		Impl::Instance = new MessageProc();
	}
	return Impl::Instance->pImpl->StkMsg[Id][MessageProc::Impl::MLANG_ENGLISH];
}

wchar_t* MessageProc::GetMsgJpn(int Id)
{
	if (Impl::Instance == NULL) {
		Impl::Instance = new MessageProc();
	}
	return Impl::Instance->pImpl->StkMsg[Id][MessageProc::Impl::MLANG_JAPANESE];
}

char* MessageProc::GetMsgSjis(int Id)
{
	if (Impl::Instance == NULL) {
		Impl::Instance = new MessageProc();
	}
	return Impl::Instance->pImpl->StkMsgSjis[Id][Impl::Instance->pImpl->GetLocale()];
}

char* MessageProc::GetMsgSjisEng(int Id)
{
	if (Impl::Instance == NULL) {
		Impl::Instance = new MessageProc();
	}
	return Impl::Instance->pImpl->StkMsgSjis[Id][MessageProc::Impl::MLANG_ENGLISH];
}

char* MessageProc::GetMsgSjisJpn(int Id)
{
	if (Impl::Instance == NULL) {
		Impl::Instance = new MessageProc();
	}
	return Impl::Instance->pImpl->StkMsgSjis[Id][MessageProc::Impl::MLANG_JAPANESE];
}

char* MessageProc::GetMsgUtf8(int Id)
{
	if (Impl::Instance == NULL) {
		Impl::Instance = new MessageProc();
	}
	return Impl::Instance->pImpl->StkMsgUtf8[Id][Impl::Instance->pImpl->GetLocale()];
}

char* MessageProc::GetMsgUtf8Eng(int Id)
{
	if (Impl::Instance == NULL) {
		Impl::Instance = new MessageProc();
	}
	return Impl::Instance->pImpl->StkMsgUtf8[Id][MessageProc::Impl::MLANG_ENGLISH];
}

char* MessageProc::GetMsgUtf8Jpn(int Id)
{
	if (Impl::Instance == NULL) {
		Impl::Instance = new MessageProc();
	}
	return Impl::Instance->pImpl->StkMsgUtf8[Id][MessageProc::Impl::MLANG_JAPANESE];
}

void MessageProc::AddEng(int Id, const wchar_t* Msg)
{
	if (Impl::Instance == NULL) {
		Impl::Instance = new MessageProc();
	}
	Impl::Instance->pImpl->Eng(Id, Msg);
}

void MessageProc::AddJpn(int Id, const wchar_t* Msg)
{
	if (Impl::Instance == NULL) {
		Impl::Instance = new MessageProc();
	}
	Impl::Instance->pImpl->Jpn(Id, Msg);
}

void MessageProc::DelEng(int Id)
{
	if (Impl::Instance == NULL) {
		Impl::Instance = new MessageProc();
	}
	if (Impl::Instance->pImpl->StkMsg[Id][Impl::ENG] != NULL) {
		delete Impl::Instance->pImpl->StkMsg[Id][Impl::ENG];
	}
	if (Impl::Instance->pImpl->StkMsgSjis[Id][Impl::ENG] != NULL) {
		delete Impl::Instance->pImpl->StkMsgSjis[Id][Impl::ENG];
	}
	if (Impl::Instance->pImpl->StkMsgUtf8[Id][Impl::ENG] != NULL) {
		delete Impl::Instance->pImpl->StkMsgUtf8[Id][Impl::ENG];
	}
	Impl::Instance->pImpl->StkMsg[Id][Impl::ENG] = NULL;
	Impl::Instance->pImpl->StkMsgSjis[Id][Impl::ENG] = NULL;
	Impl::Instance->pImpl->StkMsgUtf8[Id][Impl::ENG] = NULL;
}

void MessageProc::DelJpn(int Id)
{
	if (Impl::Instance == NULL) {
		Impl::Instance = new MessageProc();
	}
	if (Impl::Instance->pImpl->StkMsg[Id][Impl::JPN] != NULL) {
		delete Impl::Instance->pImpl->StkMsg[Id][Impl::JPN];
	}
	if (Impl::Instance->pImpl->StkMsgSjis[Id][Impl::JPN] != NULL) {
		delete Impl::Instance->pImpl->StkMsgSjis[Id][Impl::JPN];
	}
	if (Impl::Instance->pImpl->StkMsgUtf8[Id][Impl::JPN] != NULL) {
		delete Impl::Instance->pImpl->StkMsgUtf8[Id][Impl::JPN];
	}
	Impl::Instance->pImpl->StkMsg[Id][Impl::JPN] = NULL;
	Impl::Instance->pImpl->StkMsgSjis[Id][Impl::JPN] = NULL;
	Impl::Instance->pImpl->StkMsgUtf8[Id][Impl::JPN] = NULL;
}

void MessageProc::ClearAllMsg()
{
	if (Impl::Instance == NULL) {
		Impl::Instance = new MessageProc();
	}
	Impl::Instance->pImpl->AllClear();
}

size_t MessageProc::StkPlConvUtf16leToUtf32le(char32_t* Utf32le, size_t SizeInWord, const char16_t* Utf16le)
{
	const char16_t* Utf16lePtr = Utf16le;
	char32_t* Utf32lePtr = Utf32le;
	size_t ActualSize = 0;
	if (SizeInWord == 0) {
		return 0;
	}
	while (*Utf16lePtr != u'\0' && ActualSize < SizeInWord - 1) {
		if ((*Utf16lePtr & 0b1101100000000000) == 0b1101100000000000) {
			if ((*(Utf16lePtr + 1) & 0b1101110000000000) != 0b1101110000000000) {
				break;
			}
			char32_t Val = 0x10000 + (*Utf16lePtr - 0b1101100000000000) * 0x400 + (*(Utf16lePtr + 1) - 0b1101110000000000);
			*Utf32lePtr = Val;
			Utf16lePtr += 2;
		} else {
			*Utf32lePtr = (char32_t)*Utf16lePtr;
			Utf16lePtr++;
		}
		Utf32lePtr++;
		ActualSize++;
	}
	*Utf32lePtr = U'\0';
	return ActualSize;
}

size_t MessageProc::StkPlConvUtf32leToUtf16le(char16_t* Utf16le, size_t SizeInWord, const char32_t* Utf32le)
{
	const char32_t* Utf32lePtr = Utf32le;
	char16_t* Utf16lePtr = Utf16le;
	size_t ActualSize = 0;
	if (SizeInWord == 0) {
		return 0;
	}
	while (*Utf32lePtr != u'\0' && ActualSize < SizeInWord - 1) {
		if (*Utf32lePtr < 0x10000) {
			*Utf16lePtr = (char16_t)*Utf32lePtr;
			Utf16lePtr++;
			ActualSize++;
		} else {
			if (ActualSize + 1 < SizeInWord - 1) {
				char16_t Hi = (char16_t)((*Utf32lePtr - 0x10000) / 0x400 + 0b1101100000000000);
				char16_t Lo = (char16_t)((*Utf32lePtr - 0x10000) % 0x400 + 0b1101110000000000);
				*Utf16lePtr = Hi;
				*(Utf16lePtr + 1) = Lo;
				Utf16lePtr += 2;
				ActualSize += 2;
			} else {
				break;
			}
		}
		Utf32lePtr++;
	}
	*Utf16lePtr = u'\0';
	return ActualSize;
}

size_t MessageProc::StkPlConvUtf8ToUtf32le(char* Utf32le, size_t SizeInWord, const char* Utf8)
{
	return 0;
}
size_t MessageProc::StkPlConvUtf32leToUtf8(char*Utf8, size_t SizeInWord, const char* Utf32le)
{
	return 0;
}

size_t MessageProc::StkPlConvUtf8ToUtf16le(char* Utf16le, size_t SizeInWord, const char* Utf8)
{

	return 0;
}

size_t MessageProc::StkPlConvUtf16leToUtf8(char* Utf8, size_t SizeInWord, const char* Utf16le)
{
	return 0;
}
