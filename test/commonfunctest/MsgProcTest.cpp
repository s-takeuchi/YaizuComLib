#include "../../src/stkpl/StkPl.h"
#include "../../src/commonfunc/msgproc.h"
#include "../../src/commonfunc/StkStringParser.h"

void MsgProcTest1()
{
	MessageProc::AddJpn(100, L"あいうえお");
	MessageProc::AddEng(100, L"abcde");

	MessageProc::SetLocaleMode(MessageProc::LOCALE_MODE_ENGLISH);
	if (StkPlWcsCmp(MessageProc::GetMsg(100), L"abcde") != 0) {
		StkPlPrintf("English message cannot be acquired in the English locale configuration ... NG case\n");
		StkPlExit(-1);
	} else {
		StkPlPrintf("English message can be acquired in the English locale configuration ... OK case\n");
	}
	if (StkPlWcsCmp(MessageProc::GetMsgJpn(100), L"あいうえお") != 0) {
		StkPlPrintf("Japanese message cannot not be acquired using GetMsgJpn in the English locale configuration ... NG case\n");
		StkPlExit(-1);
	} else {
		StkPlPrintf("Japanese message can be acquired using GetMsgJpn() in the English locale configuration ... OK case\n");
	}

	MessageProc::SetLocaleMode(MessageProc::LOCALE_MODE_JAPANESE);
	if (StkPlWcsCmp(MessageProc::GetMsg(100), L"あいうえお") != 0) {
		StkPlPrintf("Japanese message cannot not be acquired in the Japanese locale configuration ... NG case\n");
		StkPlExit(-1);
	} else {
		StkPlPrintf("Japanese message can be acquired in the Japanese locale configuration ... OK case\n");
	}
	if (StkPlWcsCmp(MessageProc::GetMsgEng(100), L"abcde") != 0) {
		StkPlPrintf("English message cannot not be acquired using GetMsgEng in the Japanese locale configuration ... NG case\n");
		StkPlExit(-1);
	} else {
		StkPlPrintf("English message can be acquired using GetMsgEng() in the Japanese locale configuration ... OK case\n");
	}

	MessageProc::SetLocaleMode(MessageProc::LOCALE_MODE_ENGLISH);
	if (
#ifdef WIN32
		MessageProc::GetMsgSjisEng(100) == NULL || MessageProc::GetMsgSjisJpn(100) == NULL ||
#endif
		MessageProc::GetMsgUtf8Eng(100) == NULL || MessageProc::GetMsgUtf8Jpn(100) == NULL) {
		StkPlPrintf("Sjis message (Windows only) and Utf8 message cannot be acquired ... NG case\n");
		StkPlExit(-1);
	} else {
		StkPlPrintf("Sjis message (Windows only) and Utf8 message can be acquired ... OK case\n");
	}

	if (MessageProc::GetMsg(101) != NULL) {
		StkPlPrintf("GetMsg does not return NULL if undefined message ID is specified ... NG case\n");
		StkPlExit(-1);
	} else {
		StkPlPrintf("GetMsg returns NULL if undefined message ID is specified ... OK case\n");
	}

	MessageProc::ClearAllMsg();
	if (MessageProc::GetMsg(100) != NULL) {
		StkPlPrintf("Old message can be acquired even if clear API is called ... NG case\n");
		StkPlExit(-1);
	} else {
		StkPlPrintf("After clear API is called no message can be acquired ... OK case\n");
	}

	MessageProc::AddJpn(101, L"あいうえお");
	MessageProc::AddEng(101, L"abcde");
	MessageProc::AddJpn(102, L"かきくけこ");
	MessageProc::AddEng(102, L"fghij");
	MessageProc::DelJpn(101);
	MessageProc::DelEng(102);
	if (MessageProc::GetMsgEng(101) == NULL ||
#ifdef WIN32
		MessageProc::GetMsgSjisEng(101) == NULL ||
#endif
		MessageProc::GetMsgUtf8Eng(101) == NULL) {
		StkPlPrintf("Existing English message is not returned ... NG case\n");
		StkPlExit(-1);
	} else {
		StkPlPrintf("Existing English message is returned ... OK case\n");
	}
	if (MessageProc::GetMsgJpn(102) == NULL ||
#ifdef WIN32
		MessageProc::GetMsgSjisJpn(102) == NULL ||
#endif
		MessageProc::GetMsgUtf8Jpn(102) == NULL) {
		StkPlPrintf("Existing Japanese message is not returned ... NG case\n");
		StkPlExit(-1);
	} else {
		StkPlPrintf("Existing Japanese message is returned ... OK case\n");
	}
	if (MessageProc::GetMsgJpn(101) == NULL ||
#ifdef WIN32
		MessageProc::GetMsgSjisJpn(101) == NULL ||
#endif
		MessageProc::GetMsgUtf8Jpn(101) == NULL) {
		StkPlPrintf("Deleted Japanese message is not returned ... OK case\n");
	} else {
		StkPlPrintf("Deleted Japanese message is returned ... NG case\n");
		StkPlExit(-1);
	}
	if (MessageProc::GetMsgEng(102) == NULL ||
#ifdef WIN32
		MessageProc::GetMsgSjisEng(102) == NULL ||
#endif
		MessageProc::GetMsgUtf8Eng(102) == NULL) {
		StkPlPrintf("Deleted English message is not returned ... OK case\n");
	} else {
		StkPlPrintf("Deleted English message is returned ... NG case\n");
		StkPlExit(-1);
	}

	MessageProc::SetLocaleMode(MessageProc::LOCALE_MODE_WIN32);
	MessageProc::AddJpn(101, L"東西南北,魑魅魍魎,古今東西,老若男女,焼肉定食");
	MessageProc::AddEng(101, L"abcde");
	MessageProc::AddJpn(102, L"あいうえおかきくけこさしすせそたちつてと");
	MessageProc::AddEng(102, L"fghij");
	if (StkPlWcsCmp(MessageProc::GetMsg(101), L"東西南北,魑魅魍魎,古今東西,老若男女,焼肉定食") != 0 && StkPlIsJapaneseLocale() == true) {
		StkPlPrintf("Japanese locale check ... NG case\n");
		StkPlExit(-1);
	}
	MessageProc::DelJpn(101);
	MessageProc::DelEng(102);
	StkPlPrintf("Japanese locale check ... OK case\n");
	
	// Logging test begin
	MessageProc::StartLogging(L"log.txt");
	MessageProc::AddLog("hello, world!!", MessageProc::LOG_TYPE_INFO);
	StkPlSleepMs(500);
	MessageProc::AddLog("test!test!test!", MessageProc::LOG_TYPE_FATAL);
	MessageProc::AddLog(L"test#test#test#", MessageProc::LOG_TYPE_ERROR);
	MessageProc::AddLog(L"あいうえおabc👉𠮷", MessageProc::LOG_TYPE_FATAL);
	StkPlSleepMs(500);
	MessageProc::AddLog("done!", MessageProc::LOG_TYPE_WARN);
	char Dummy[10] = "abcdefghi";
	MessageProc::AddLogBin("BinaryData:", Dummy, 10, MessageProc::LOG_TYPE_INFO);
	MessageProc::StopLogging();
	size_t LogSize = 0;
	if ((LogSize = StkPlGetFileSize(L"log.txt")) < 100) {
		StkPlPrintf("Logging ... NG case\n");
		StkPlExit(-1);
	}
	char FileBuf[2048] = "";
	StkPlReadFile(L"log.txt", FileBuf, 2048);
	wchar_t* WcFileBuf = StkPlCreateWideCharFromUtf8(FileBuf);
	if (StkPlWcsStr(WcFileBuf, L"W/ done!") == NULL ||
		StkPlWcsStr(WcFileBuf, L"F/ test!test!test!") == NULL ||
		StkPlWcsStr(WcFileBuf, L"E/ test#test#test#") == NULL ||
		StkPlWcsStr(WcFileBuf, L"F/ あいうえおabc👉𠮷") == NULL ||
		StkPlWcsStr(WcFileBuf, L"I/ hello, world!!") == NULL ||
		StkPlWcsStr(WcFileBuf, L"I/ BinaryData: 61626364656667686900") == NULL) {
		StkPlPrintf("Logging ... NG case\n");
		StkPlExit(-1);
	}
	delete[] WcFileBuf;
	StkPlPrintf("Logging ... OK case(size=%d)\n", LogSize);
	// Logging test end
}

void MsgProcTest2()
{
	MessageProc::StartLogging(L"log2.txt");
	char TmpLog[256] = "";
	for (int Loop = 0; Loop < 14000; Loop++) {
		StkPlSPrintf(TmpLog, 256, "[%d] This is wrapping test!!  This is wrapping test!!  This is wrapping test!!  This is wrapping test!!", Loop);
		MessageProc::AddLog(TmpLog, MessageProc::LOG_TYPE_INFO);
	}
	MessageProc::StopLogging();
	size_t FileSize = StkPlGetFileSize(L"log2.txt");
	if (FileSize > 2000000 || FileSize < 1000000) {
		StkPlPrintf("LoggingTest-2 ... NG case(1)\n");
		StkPlExit(-1);
	}
	char LogDat[2000000] = "";
	StkPlReadFile(L"log2.txt", LogDat, 2000000);
	wchar_t* LogDatW = StkPlCreateWideCharFromUtf8(LogDat);
	wchar_t* LogDatPtr = LogDatW;
	wchar_t AcqDat[64];
	int MinIndex = -1;
	for (int Loop = 0; Loop < 5000; Loop++) {
		LogDatPtr = (wchar_t*)StkPlWcsStr(LogDatPtr, L"This is wrapping test!!\r\n") + 1;
		StkStringParser::ParseInto3Params(LogDatPtr, L"$I/ [$] This is$", L'$', NULL, AcqDat, NULL);
		int Index = StkPlWcsToL(AcqDat);
		if (MinIndex >= Index) {
			delete[] LogDatW;
			StkPlPrintf("LoggingTest-2 ... NG case(2)\n");
			StkPlExit(-1);
		}
		MinIndex = Index;
	}
	delete LogDatW;
	StkPlPrintf("LoggingTest-2 ... OK case\n");
}
