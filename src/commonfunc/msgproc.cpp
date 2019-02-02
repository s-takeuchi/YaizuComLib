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
	StkMsgUtf8[Id][ENG] = CreateUtf8FromWideChar(Msg);
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
	StkMsgUtf8[Id][JPN] = (char*)CreateUtf8FromWideChar(Msg);
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

size_t MessageProc::ConvUtf16ToUtf32(char32_t* Utf32, size_t SizeInWord, const char16_t* Utf16)
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
		if ((*Utf16Ptr & 0b1101100000000000) == 0b1101100000000000) {
			if ((*(Utf16Ptr + 1) & 0b1101110000000000) != 0b1101110000000000) {
				break;
			}
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

size_t MessageProc::ConvUtf32ToUtf16(char16_t* Utf16, size_t SizeInWord, const char32_t* Utf32)
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

size_t MessageProc::ConvUtf8ToUtf32(char32_t* Utf32, size_t SizeInWord, const char* Utf8)
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

size_t MessageProc::ConvUtf32ToUtf8(char* Utf8, size_t SizeInWord, const char32_t* Utf32)
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

size_t MessageProc::ConvUtf8ToUtf16(char16_t* Utf16, size_t SizeInWord, const char* Utf8)
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

size_t MessageProc::ConvUtf16ToUtf8(char* Utf8, size_t SizeInWord, const char16_t* Utf16)
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
		if ((*Utf16Ptr & 0b1101100000000000) == 0b1101100000000000) {
			if ((*(Utf16Ptr + 1) & 0b1101110000000000) != 0b1101110000000000) {
				break;
			}
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

size_t MessageProc::ConvUtf16ToWideChar(wchar_t* Wc, size_t SizeInWord, const char16_t* Utf16)
{
#ifdef WIN32
	StkPlWcsCpy(Wc, SizeInWord, (wchar_t*)Utf16);
	return StkPlWcsLen(Wc);
#else
	return ConvUtf16ToUtf32((char32_t*)Wc, SizeInWord, Utf16);
#endif
}

size_t MessageProc::ConvWideCharToUtf16(char16_t* Utf16, size_t SizeInWord, const wchar_t* Wc)
{
#ifdef WIN32
	StkPlWcsCpy((wchar_t*)Utf16, SizeInWord, Wc);
	return StkPlWcsLen(Wc);
#else
	return ConvUtf32ToUtf16(Utf16, SizeInWord, (char32_t*)Wc);
#endif
}

size_t MessageProc::ConvWideCharToUtf32(char32_t* Utf32, size_t SizeInWord, const wchar_t* Wc)
{
#ifdef WIN32
	return ConvUtf16ToUtf32(Utf32, SizeInWord, (char16_t*)Wc);
#else
	StkPlWcsCpy((wchar_t*)Utf32, SizeInWord, Wc);
	return StkPlWcsLen(Wc);
#endif
}

size_t MessageProc::ConvUtf32ToWideChar(wchar_t* Wc, size_t SizeInWord, const char32_t* Utf32)
{
#ifdef WIN32
	return ConvUtf32ToUtf16((char16_t*)Wc, SizeInWord, Utf32);
#else
	StkPlWcsCpy(Wc, SizeInWord, (wchar_t*)Utf32);
	return StkPlWcsLen(Wc);
#endif
}

size_t MessageProc::ConvUtf8ToWideChar(wchar_t* Wc, size_t SizeInWord, const char* Utf8)
{
#ifdef WIN32
	return ConvUtf8ToUtf16((char16_t*)Wc, SizeInWord, Utf8);
#else
	return ConvUtf8ToUtf32((char32_t*)Wc, SizeInWord, Utf8);
#endif
}

size_t MessageProc::ConvWideCharToUtf8(char* Utf8, size_t SizeInWord, const wchar_t* Wc)
{
#ifdef WIN32
	return ConvUtf16ToUtf8(Utf8, SizeInWord, (char16_t*)Wc);
#else
	return ConvUtf32ToUtf8(Utf8, SizeInWord, (char32_t*)Wc);
#endif
}

char32_t* MessageProc::CreateUtf32FromUtf16(const char16_t* Utf16)
{
	int Len = ConvUtf16ToUtf32(NULL, 0, Utf16) + 1;
	char32_t* Utf32 = new char32_t[Len];
	ConvUtf16ToUtf32(Utf32, Len, Utf16);
	return Utf32;
}

char16_t* MessageProc::CreateUtf16FromUtf32(const char32_t* Utf32)
{
	int Len = ConvUtf32ToUtf16(NULL, 0, Utf32) + 1;
	char16_t* Utf16 = new char16_t[Len];
	ConvUtf32ToUtf16(Utf16, Len, Utf32);
	return Utf16;
}

char32_t* MessageProc::CreateUtf32FromUtf8(const char* Utf8)
{
	int Len = ConvUtf8ToUtf32(NULL, 0, Utf8) + 1;
	char32_t* Utf32 = new char32_t[Len];
	ConvUtf8ToUtf32(Utf32, Len, Utf8);
	return Utf32;
}

char* MessageProc::CreateUtf8FromUtf32(const char32_t* Utf32)
{
	int Len = ConvUtf32ToUtf8(NULL, 0, Utf32) + 1;
	char* Utf8 = new char[Len];
	ConvUtf32ToUtf8(Utf8, Len, Utf32);
	return Utf8;
}

char16_t* MessageProc::CreateUtf16FromUtf8(const char* Utf8)
{
	int Len = ConvUtf8ToUtf16(NULL, 0, Utf8) + 1;
	char16_t* Utf16 = new char16_t[Len];
	ConvUtf8ToUtf16(Utf16, Len, Utf8);
	return Utf16;
}

char* MessageProc::CreateUtf8FromUtf16(const char16_t* Utf16)
{
	int Len = ConvUtf16ToUtf8(NULL, 0, Utf16) + 1;
	char* Utf8 = new char[Len];
	ConvUtf16ToUtf8(Utf8, Len, Utf16);
	return Utf8;
}

wchar_t* MessageProc::CreateWideCharFromUtf16(const char16_t* Utf16)
{
#ifdef WIN32
	int Len = StkPlWcsLen((wchar_t*)Utf16) + 1;
	wchar_t* Wc = new wchar_t[Len];
	StkPlWcsCpy(Wc, Len, (wchar_t*)Utf16);
	return Wc;
#else
	return (wchar_t*)CreateUtf32FromUtf16(Utf16);
#endif
}

char16_t* MessageProc::CreateUtf16FromWideChar(const wchar_t* Wc)
{
#ifdef WIN32
	int Len = StkPlWcsLen((wchar_t*)Wc) + 1;
	char16_t* Utf16 = new char16_t[Len];
	StkPlWcsCpy((wchar_t*)Utf16, Len, Wc);
	return Utf16;
#else
	return CreateUtf16FromUtf32((char32_t*)Wc);
#endif
}

char32_t* MessageProc::CreateUtf32FromWideChar(const wchar_t* Wc)
{
#ifdef WIN32
	return CreateUtf32FromUtf16((char16_t*)Wc);
#else
	int Len = StkPlWcsLen((wchar_t*)Wc) + 1;
	char32_t* Utf32 = new char32_t[Len];
	StkPlWcsCpy((wchar_t*)Utf32, Len, Wc);
	return Utf32;
#endif
}

wchar_t* MessageProc::CreateWideCharFromUtf32(const char32_t* Utf32)
{
#ifdef WIN32
	return (wchar_t*)CreateUtf16FromUtf32(Utf32);
#else
	int Len = StkPlWcsLen((wchar_t*)Utf32) + 1;
	wchar_t* Wc = new wchar_t[Len];
	StkPlWcsCpy(Wc, Len, (wchar_t*)Utf32);
	return Wc;
#endif
}

wchar_t* MessageProc::CreateWideCharFromUtf8(const char* Utf8)
{
#ifdef WIN32
	return (wchar_t*)CreateUtf16FromUtf8(Utf8);
#else
	return (wchar_t*)CreateUtf32FromUtf8(Utf8);
#endif
}

char* MessageProc::CreateUtf8FromWideChar(const wchar_t* Wc)
{
#ifdef WIN32
	return CreateUtf8FromUtf16((char16_t*)Wc);
#else
	return CreateUtf8FromUtf32((char32_t*)Wc);
#endif
}
