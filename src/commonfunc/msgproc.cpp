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

	TCHAR* StkMsg[MAX_MSG_COUNT][2];
	BYTE* StkMsgSjis[MAX_MSG_COUNT][2];
	BYTE* StkMsgUtf8[MAX_MSG_COUNT][2];

	void Eng(int, TCHAR*);
	void Jpn(int, TCHAR*);

	void InitMsg();
	void AllClear();

	int GetLocale();
};

MessageProc* MessageProc::Impl::Instance = NULL;

int MessageProc::Impl::GetLocale()
{
	if (Mode == MessageProc::LOCALE_MODE_WIN32) {
		TCHAR Buf[256];
		GetLocaleInfo(GetUserDefaultLCID(), LOCALE_SENGLANGUAGE, Buf, 256);
		if (lstrcmp(Buf, _T("Japanese")) == 0) { // "XXX" or "Japanese"
			return MessageProc::Impl::MLANG_JAPANESE;
		}
		return MessageProc::Impl::MLANG_ENGLISH;
	}
	if (Mode == MessageProc::LOCALE_MODE_WEB) {
		char* Locale;
		size_t LocaleSize;
		if (_dupenv_s(&Locale, &LocaleSize, "HTTP_ACCEPT_LANGUAGE") == 0) {
			if (Locale == 0 || LocaleSize ==0) {
				return 0;
			}
			if (strstr(Locale, (char*)"ja") == Locale) {
				free(Locale);
				return MessageProc::Impl::MLANG_JAPANESE;
			}
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

void MessageProc::Impl::Eng(int Id, TCHAR* Msg)
{
	if (StkMsg[Id][ENG] != NULL) {
		delete StkMsg[Id][ENG];
	}
	int WcBufLen = lstrlen(Msg);
	StkMsg[Id][ENG] = new TCHAR[WcBufLen + 1];
	lstrcpyn(StkMsg[Id][ENG], Msg, WcBufLen + 1);

	int MltBufLen;

	if (StkMsgSjis[Id][ENG] != NULL) {
		delete StkMsgSjis[Id][ENG];
	}
	MltBufLen = WideCharToMultiByte(CP_THREAD_ACP, 0, Msg, -1, (LPSTR)NULL, 0, NULL, NULL);
	StkMsgSjis[Id][ENG] = (BYTE*)new CHAR[MltBufLen];
	if (WideCharToMultiByte(CP_THREAD_ACP, 0, Msg, -1, (LPSTR)StkMsgSjis[Id][ENG], MltBufLen, NULL, NULL) != 0) {
		StkMsgSjis[Id][ENG][MltBufLen - 1] = 0;
	}

	if (StkMsgUtf8[Id][ENG] != NULL) {
		delete StkMsgUtf8[Id][ENG];
	}
	MltBufLen = WideCharToMultiByte(CP_UTF8, 0, Msg, -1, (LPSTR)NULL, 0, NULL, NULL);
	StkMsgUtf8[Id][ENG] = (BYTE*)new CHAR[MltBufLen];
	if (WideCharToMultiByte(CP_UTF8, 0, Msg, -1, (LPSTR)StkMsgUtf8[Id][ENG], MltBufLen, NULL, NULL) != 0) {
		StkMsgUtf8[Id][ENG][MltBufLen - 1] = 0;
	}
}

void MessageProc::Impl::Jpn(int Id, TCHAR* Msg)
{
	if (StkMsg[Id][JPN] != NULL) {
		delete StkMsg[Id][JPN];
	}
	int WcBufLen = lstrlen(Msg);
	StkMsg[Id][JPN] = new TCHAR[WcBufLen + 1];
	lstrcpyn(StkMsg[Id][JPN], Msg, WcBufLen + 1);

	int MltBufLen;

	if (StkMsgSjis[Id][JPN] != NULL) {
		delete StkMsgSjis[Id][JPN];
	}
	MltBufLen = WideCharToMultiByte(CP_THREAD_ACP, 0, Msg, -1, (LPSTR)NULL, 0, NULL, NULL);
	StkMsgSjis[Id][JPN] = (BYTE*)new CHAR[MltBufLen];
	if (WideCharToMultiByte(CP_THREAD_ACP, 0, Msg, -1, (LPSTR)StkMsgSjis[Id][JPN], MltBufLen, NULL, NULL) != 0) {
		StkMsgSjis[Id][JPN][MltBufLen - 1] = 0;
	}

	if (StkMsgUtf8[Id][JPN] != NULL) {
		delete StkMsgUtf8[Id][JPN];
	}
	MltBufLen = WideCharToMultiByte(CP_UTF8, 0, Msg, -1, (LPSTR)NULL, 0, NULL, NULL);
	StkMsgUtf8[Id][JPN] = (BYTE*)new CHAR[MltBufLen];
	if (WideCharToMultiByte(CP_UTF8, 0, Msg, -1, (LPSTR)StkMsgUtf8[Id][JPN], MltBufLen, NULL, NULL) != 0) {
		StkMsgUtf8[Id][JPN][MltBufLen - 1] = 0;
	}
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

TCHAR* MessageProc::GetMsg(int Id)
{
	if (Impl::Instance == NULL) {
		Impl::Instance = new MessageProc();
	}
	return Impl::Instance->pImpl->StkMsg[Id][Impl::Instance->pImpl->GetLocale()];
}

TCHAR* MessageProc::GetMsgEng(int Id)
{
	if (Impl::Instance == NULL) {
		Impl::Instance = new MessageProc();
	}
	return Impl::Instance->pImpl->StkMsg[Id][MessageProc::Impl::MLANG_ENGLISH];
}

TCHAR* MessageProc::GetMsgJpn(int Id)
{
	if (Impl::Instance == NULL) {
		Impl::Instance = new MessageProc();
	}
	return Impl::Instance->pImpl->StkMsg[Id][MessageProc::Impl::MLANG_JAPANESE];
}

BYTE* MessageProc::GetMsgSjis(int Id)
{
	if (Impl::Instance == NULL) {
		Impl::Instance = new MessageProc();
	}
	return Impl::Instance->pImpl->StkMsgSjis[Id][Impl::Instance->pImpl->GetLocale()];
}

BYTE* MessageProc::GetMsgSjisEng(int Id)
{
	if (Impl::Instance == NULL) {
		Impl::Instance = new MessageProc();
	}
	return Impl::Instance->pImpl->StkMsgSjis[Id][MessageProc::Impl::MLANG_ENGLISH];
}

BYTE* MessageProc::GetMsgSjisJpn(int Id)
{
	if (Impl::Instance == NULL) {
		Impl::Instance = new MessageProc();
	}
	return Impl::Instance->pImpl->StkMsgSjis[Id][MessageProc::Impl::MLANG_JAPANESE];
}

BYTE* MessageProc::GetMsgUtf8(int Id)
{
	if (Impl::Instance == NULL) {
		Impl::Instance = new MessageProc();
	}
	return Impl::Instance->pImpl->StkMsgUtf8[Id][Impl::Instance->pImpl->GetLocale()];
}

BYTE* MessageProc::GetMsgUtf8Eng(int Id)
{
	if (Impl::Instance == NULL) {
		Impl::Instance = new MessageProc();
	}
	return Impl::Instance->pImpl->StkMsgUtf8[Id][MessageProc::Impl::MLANG_ENGLISH];
}

BYTE* MessageProc::GetMsgUtf8Jpn(int Id)
{
	if (Impl::Instance == NULL) {
		Impl::Instance = new MessageProc();
	}
	return Impl::Instance->pImpl->StkMsgUtf8[Id][MessageProc::Impl::MLANG_JAPANESE];
}

void MessageProc::AddEng(int Id, TCHAR* Msg)
{
	if (Impl::Instance == NULL) {
		Impl::Instance = new MessageProc();
	}
	Impl::Instance->pImpl->Eng(Id, Msg);
}

void MessageProc::AddJpn(int Id, TCHAR* Msg)
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
