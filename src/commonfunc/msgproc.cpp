#include <thread>
#include <mutex>

#include "../stkpl/StkPl.h"
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

	void* LogFD;
	bool StopLogging;
	bool LoggingStopped;
	char LoggingBuf[65536];
	std::mutex Cs4LogBuf;
	size_t LogSizeA;
	size_t LogSizeB;
	wchar_t FileName[FILENAME_MAX];

	void Eng(int, const wchar_t*);
	void Jpn(int, const wchar_t*);

	void InitMsg();
	void AllClear();

	int GetLocale();

	static void LoggingProc();
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
	int WcBufLen = (int)StkPlWcsLen(Msg);
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
	StkMsgUtf8[Id][ENG] = StkPlCreateUtf8FromWideChar(Msg);
}

void MessageProc::Impl::Jpn(int Id, const wchar_t* Msg)
{
	if (StkMsg[Id][JPN] != NULL) {
		delete StkMsg[Id][JPN];
	}
	int WcBufLen = (int)StkPlWcsLen(Msg);
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
	StkMsgUtf8[Id][JPN] = (char*)StkPlCreateUtf8FromWideChar(Msg);
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

void MessageProc::Impl::LoggingProc()
{
	char TmpLogBuf[65536];
	size_t WrittenSize = 0;
	size_t LogSize = StkPlGetFileSize(Impl::Instance->pImpl->FileName);
	size_t TotalAddSize = 0;

	while (true) {
		StkPlSleepMs(500);
		if (Impl::Instance->pImpl->StopLogging && Impl::Instance->pImpl->LoggingBuf[0] == '\0') {
			break;
		}
		if (Impl::Instance->pImpl->LoggingBuf[0] == '\0') {
			continue;
		}
		Impl::Instance->pImpl->Cs4LogBuf.lock();
		StkPlStrCpy(TmpLogBuf, 65536, Impl::Instance->pImpl->LoggingBuf);
		Impl::Instance->pImpl->LoggingBuf[0] = '\0';
		Impl::Instance->pImpl->Cs4LogBuf.unlock();
		size_t AddSize = StkPlStrLen(TmpLogBuf);
		TotalAddSize += AddSize;
		StkPlWrite(Impl::Instance->pImpl->LogFD, TmpLogBuf, AddSize, &WrittenSize);
		StkPlFlush(Impl::Instance->pImpl->LogFD);

		// Cut a part of log file off
		if (LogSize + TotalAddSize > Impl::Instance->pImpl->LogSizeB) {
			char Buf[2000000] = "";
			size_t Diff = LogSize + TotalAddSize - Impl::Instance->pImpl->LogSizeA;
			size_t ActSizeR = 0;
			size_t ActSizeW = 0;
			StkPlCloseFile(Impl::Instance->pImpl->LogFD);

			Impl::Instance->pImpl->LogFD = StkPlOpenFileForRead(Impl::Instance->pImpl->FileName);
			StkPlSeekFromBegin(Impl::Instance->pImpl->LogFD, Diff);
			StkPlRead(Impl::Instance->pImpl->LogFD, Buf, 2000000, &ActSizeR);
			StkPlCloseFile(Impl::Instance->pImpl->LogFD);

			Impl::Instance->pImpl->LogFD = StkPlOpenFileForWrite(Impl::Instance->pImpl->FileName, false);
			StkPlSeekFromBegin(Impl::Instance->pImpl->LogFD, 0);
			StkPlWrite(Impl::Instance->pImpl->LogFD, Buf, ActSizeR, &ActSizeW);
			char TmpBuf[128] = "";
			char TmpTimeStr[64] = "";
			size_t TmpSize = 0;
			StkPlGetTimeInIso8601(TmpTimeStr, true);
			StkPlSPrintf(TmpBuf, 128, "%s [%08x] I/ %s [%d --> %d]\r\n", TmpTimeStr, std::this_thread::get_id(), "Log rotation (old log was deleted)", LogSize + TotalAddSize, ActSizeW);
			StkPlWrite(Impl::Instance->pImpl->LogFD, TmpBuf, StkPlStrLen(TmpBuf), &TmpSize);
			StkPlCloseFile(Impl::Instance->pImpl->LogFD);

			Impl::Instance->pImpl->LogFD = StkPlOpenFileForWrite(Impl::Instance->pImpl->FileName, true);
			StkPlSeekFromEnd(Impl::Instance->pImpl->LogFD, 0);
			LogSize = StkPlGetFileSize(Impl::Instance->pImpl->FileName);
			TotalAddSize = 0;
		}
	}
	Impl::Instance->pImpl->LoggingStopped = true;
}

MessageProc::MessageProc()
{
	pImpl = new Impl;
	pImpl->Mode = MessageProc::LOCALE_MODE_WIN32;
	pImpl->InitMsg();
	pImpl->StopLogging = false;
	pImpl->LoggingStopped = false;
	pImpl->LogFD = NULL;
	pImpl->LoggingBuf[0] = '\0';
	pImpl->LogSizeA = 1000000; // delete logs up to this size when the log size exceeds LogSizeB
	pImpl->LogSizeB = 2000000; // limitation of log size
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

// Enable logging feature. This API executes procedures shown below.
// (1) Open file descriptor for logging
// (2) Launch a thread for logging
// FilePath [in] : Path to log file
// Return : Result code (0: Success, -1: Failure)
int MessageProc::StartLogging(const wchar_t* FilePath)
{
	StkPlWcsCpy(Impl::Instance->pImpl->FileName, FILENAME_MAX, FilePath);
	
	// Open log file
	Impl::Instance->pImpl->LogFD = StkPlOpenFileForWrite(FilePath, true);
	if (Impl::Instance->pImpl->LogFD == NULL) {
		return -1;
	}
	StkPlSeekFromEnd(Impl::Instance->pImpl->LogFD, 0);
	// Start logging thread
	Impl::Instance->pImpl->StopLogging = false;
	Impl::Instance->pImpl->LoggingStopped = false;
	std::thread MyThread(Impl::Instance->pImpl->LoggingProc);
	MyThread.detach();

	return 0;
}

// Logging message
void MessageProc::AddLog(const char* Msg, int Type)
{
	char TypeStr[4][8] = { "F/", "E/", "W/", "I/" };
	char TmpTimeStr[64] = "";
	char TmpBuf[65536] = "";
	StkPlGetTimeInIso8601(TmpTimeStr, true);
	StkPlSPrintf(TmpBuf, 65536, "%s [%08x] %s %s\r\n", TmpTimeStr, std::this_thread::get_id(), TypeStr[Type], Msg);
	while (true) {
		Impl::Instance->pImpl->Cs4LogBuf.lock();
		if (StkPlStrLen(Impl::Instance->pImpl->LoggingBuf) + StkPlStrLen(TmpBuf) + 1 < 65536) {
			StkPlStrCat(Impl::Instance->pImpl->LoggingBuf, 65536, TmpBuf);
			Impl::Instance->pImpl->Cs4LogBuf.unlock();
			break;
		} else {
			Impl::Instance->pImpl->Cs4LogBuf.unlock();
			StkPlSleepMs(10);
		}
	}
}

// Disable logging feature. This API executes procedures shown below.
// (1) Stop a thread for logging
// (2) Close file descriptor for logging
// Return : Always zero returns
int MessageProc::StopLogging()
{
	// Stop logging thread
	Impl::Instance->pImpl->StopLogging = true;
	while (!Impl::Instance->pImpl->LoggingStopped) {
		StkPlSleepMs(10);
	}
	//Close log file
	if (Impl::Instance->pImpl->LogFD != NULL) {
		StkPlCloseFile(Impl::Instance->pImpl->LogFD);
		Impl::Instance->pImpl->LogFD = NULL;
	}

	return 0;
}
