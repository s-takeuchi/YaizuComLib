#include "../../src/StkPl.h"
#include "../../src/commonfunc/msgproc.h"

void MsgProcTest()
{
	StkPlPrintf("MsgProcTest started.\n");

	MessageProc::AddJpn(100, L"あいうえお");
	MessageProc::AddEng(100, L"abcde");

	MessageProc::SetLocaleMode(MessageProc::LOCALE_MODE_ENGLISH);
	if (StkPlWcsCmp(MessageProc::GetMsg(100), L"abcde") != 0) {
		StkPlPrintf("English message cannot be acquired in the English locale configuration ... NG case\n");
		StkPlExit(0);
	} else {
		StkPlPrintf("English message can be acquired in the English locale configuration ... OK case\n");
	}
	if (StkPlWcsCmp(MessageProc::GetMsgJpn(100), L"あいうえお") != 0) {
		StkPlPrintf("Japanese message cannot not be acquired using GetMsgJpn in the English locale configuration ... NG case\n");
		StkPlExit(0);
	} else {
		StkPlPrintf("Japanese message can be acquired using GetMsgJpn() in the English locale configuration ... OK case\n");
	}

	MessageProc::SetLocaleMode(MessageProc::LOCALE_MODE_JAPANESE);
	if (StkPlWcsCmp(MessageProc::GetMsg(100), L"あいうえお") != 0) {
		StkPlPrintf("Japanese message cannot not be acquired in the Japanese locale configuration ... NG case\n");
		StkPlExit(0);
	} else {
		StkPlPrintf("Japanese message can be acquired in the Japanese locale configuration ... OK case\n");
	}
	if (StkPlWcsCmp(MessageProc::GetMsgEng(100), L"abcde") != 0) {
		StkPlPrintf("English message cannot not be acquired using GetMsgEng in the Japanese locale configuration ... NG case\n");
		StkPlExit(0);
	} else {
		StkPlPrintf("English message can be acquired using GetMsgEng() in the Japanese locale configuration ... OK case\n");
	}

	MessageProc::SetLocaleMode(MessageProc::LOCALE_MODE_ENGLISH);
	if (MessageProc::GetMsgSjisEng(100) == NULL || MessageProc::GetMsgSjisJpn(100) == NULL ||
		MessageProc::GetMsgUtf8Eng(100) == NULL || MessageProc::GetMsgUtf8Jpn(100) == NULL) {
		StkPlPrintf("Sjis message and Utf8 message cannot be acquired ... NG case\n");
		StkPlExit(0);
	} else {
		StkPlPrintf("Sjis message and Utf8 message can be acquired ... OK case\n");
	}

	if (MessageProc::GetMsg(101) != NULL) {
		StkPlPrintf("GetMsg does not return NULL if undefined message ID is specified ... NG case\n");
		StkPlExit(0);
	} else {
		StkPlPrintf("GetMsg returns NULL if undefined message ID is specified ... OK case\n");
	}

	MessageProc::ClearAllMsg();
	if (MessageProc::GetMsg(100) != NULL) {
		StkPlPrintf("Old message can be acquired even if clear API is called ... NG case\n");
		StkPlExit(0);
	} else {
		StkPlPrintf("After clear API is called no message can be acquired ... OK case\n");
	}

	MessageProc::AddJpn(101, L"あいうえお");
	MessageProc::AddEng(101, L"abcde");
	MessageProc::AddJpn(102, L"かきくけこ");
	MessageProc::AddEng(102, L"fghij");
	MessageProc::DelJpn(101);
	MessageProc::DelEng(102);
	if (MessageProc::GetMsgEng(101) == NULL || MessageProc::GetMsgSjisEng(101) == NULL || MessageProc::GetMsgUtf8Eng(101) == NULL) {
		StkPlPrintf("Existing English message is not returned ... NG case\n");
		StkPlExit(0);
	} else {
		StkPlPrintf("Existing English message is returned ... OK case\n");
	}
	if (MessageProc::GetMsgJpn(102) == NULL || MessageProc::GetMsgSjisJpn(102) == NULL || MessageProc::GetMsgUtf8Jpn(102) == NULL) {
		StkPlPrintf("Existing Japanese message is not returned ... NG case\n");
		StkPlExit(0);
	} else {
		StkPlPrintf("Existing Japanese message is returned ... OK case\n");
	}
	if (MessageProc::GetMsgJpn(101) == NULL || MessageProc::GetMsgSjisJpn(101) == NULL || MessageProc::GetMsgUtf8Jpn(101) == NULL) {
		StkPlPrintf("Deleted Japanese message is not returned ... OK case\n");
	} else {
		StkPlPrintf("Deleted Japanese message is returned ... NG case\n");
		StkPlExit(0);
	}
	if (MessageProc::GetMsgEng(102) == NULL || MessageProc::GetMsgSjisEng(102) == NULL || MessageProc::GetMsgUtf8Eng(102) == NULL) {
		StkPlPrintf("Deleted English message is not returned ... OK case\n");
	} else {
		StkPlPrintf("Deleted English message is returned ... NG case\n");
		StkPlExit(0);
	}

	MessageProc::SetLocaleMode(MessageProc::LOCALE_MODE_WIN32);
	MessageProc::AddJpn(101, L"東西南北,魑魅魍魎,古今東西,老若男女,焼肉定食");
	MessageProc::AddEng(101, L"abcde");
	MessageProc::AddJpn(102, L"あいうえおかきくけこさしすせそたちつてと");
	MessageProc::AddEng(102, L"fghij");
	if (StkPlWcsCmp(MessageProc::GetMsg(101), L"東西南北,魑魅魍魎,古今東西,老若男女,焼肉定食") != 0 && StkPlIsJapaneseLocale() == true) {
		StkPlPrintf("Japanese locale check ... NG case\n");
		StkPlExit(0);
	}
	MessageProc::DelJpn(101);
	MessageProc::DelEng(102);
	StkPlPrintf("Japanese locale check ... OK case\n");

	{
		char16_t* Utf16leStr = (char16_t*)u"𠮷野家𠮷野家𠮷野家";
		char32_t Utf32leStr[32];
		char16_t Utf16leStrRst[32];
		size_t LenUtf32 = MessageProc::StkPlConvUtf16leToUtf32le(Utf32leStr, 32, Utf16leStr);
		size_t LenUtf16 = MessageProc::StkPlConvUtf32leToUtf16le(Utf16leStrRst, 32, Utf32leStr);
		if (LenUtf32 != 9 || LenUtf16 != 12 || StkPlWcsCmp((wchar_t*)Utf16leStr, (wchar_t*)Utf16leStrRst) != 0 || StkPlWcsCmp(L"𠮷野家𠮷野家𠮷野家", (wchar_t*)Utf16leStrRst) != 0) {
			StkPlPrintf("UTF16 -> UTF32 -> UTF16 ... NG case\n");
			StkPlExit(0);
		}
		StkPlPrintf("UTF16 -> UTF32 -> UTF16 ... OK case\n");
	}

	{
		char16_t* Utf16leStr = (char16_t*)u"あいうえおかきくけこ";
		char32_t* Utf32leStr = (char32_t*)U"あいうえおかきくけこ";
		char16_t Utf16le[5] = u"";
		char32_t Utf32le[5] = U"";
		for (int Loop = 0; Loop <= 5; Loop++) {
			size_t LenUtf32 = MessageProc::StkPlConvUtf16leToUtf32le(Utf32le, Loop, Utf16leStr);
			size_t LenUtf16 = MessageProc::StkPlConvUtf32leToUtf16le(Utf16le, Loop, Utf32leStr);
			if (Loop == 0 && (LenUtf16 != 0 || LenUtf32 != 0)) {
				StkPlPrintf("To UTF32 with size specification , To UTF16 with size=0 specification ... NG case\n");
				StkPlExit(0);
			}
			if ((Loop >= 1 && Loop <= 5) && (LenUtf16 != Loop - 1 || LenUtf32 != Loop - 1)) {
				StkPlPrintf("To UTF32 with size specification , To UTF16 with size=0 specification ... NG case\n");
				StkPlExit(0);
			}
		}
		if (StkPlWcsCmp((wchar_t*)Utf16le, L"あいうえ") != 0) {
			StkPlPrintf("To UTF32 with size specification , To UTF16 with size=0 specification ... NG case\n");
			StkPlExit(0);
		}
		StkPlPrintf("To UTF32 with size=0 specification , To UTF16 with size=0 specification ... OK case\n");
	}

	{
		char32_t* Utf32leStr = (char32_t*)U"𠀋𡈽𡌛𡑮𡢽𠮟𡚴𡸴𣗄𣜿";
		char16_t Utf16le[11] = u"";
		size_t LenUtf16 = MessageProc::StkPlConvUtf32leToUtf16le(Utf16le, 11, Utf32leStr);
		if (LenUtf16 != 10 || StkPlWcsCmp((wchar_t*)Utf16le, L"𠀋𡈽𡌛𡑮𡢽") != 0) {
			StkPlPrintf("UTF32 -> UTF16 with lacking buffer ... NG case\n");
			StkPlExit(0);
		}
		StkPlPrintf("UTF32 -> UTF16 with lacking buffer ... OK case\n");
	}
	StkPlPrintf("MsgProcTest completed.\n\n\n");
}
