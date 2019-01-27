#include "../../src/StkPl.h"
#include "../../src/commonfunc/msgproc.h"

void Test_Conv_Utf16_Utf32(char16_t* MsgPtr, int NumOfByte)
{
	char32_t Utf32[256] = U"";
	char16_t Utf16[256] = u"";
	size_t LenUtf32 = MessageProc::StkPlConvUtf16ToUtf32(Utf32, 256, MsgPtr);
	size_t LenUtf16 = MessageProc::StkPlConvUtf32ToUtf16(Utf16, 256, Utf32);
	if (LenUtf16 != LenUtf32 * NumOfByte || StkPlWcsCmp((wchar_t*)MsgPtr, (wchar_t*)Utf16) != 0) {
		StkPlPrintf("UTF16(%d word) -> UTF32 -> UTF16(%d word) ... NG case\n", NumOfByte, NumOfByte);
		StkPlExit(0);
	}
	StkPlPrintf("UTF16(%d word) -> UTF32 -> UTF16(%d word) ... OK case\n", NumOfByte, NumOfByte);
}

void Test_Conv_Utf8_Utf32(char32_t* MsgPtr, int NumOfByte)
{
	char Utf8[256] = "";
	char32_t Utf32[256] = U"";
	size_t LenUtf8 = MessageProc::StkPlConvUtf32ToUtf8(Utf8, 256, MsgPtr);
	size_t LenUtf32 = MessageProc::StkPlConvUtf8ToUtf32(Utf32, 256, Utf8);
	if (LenUtf8 != LenUtf32 * NumOfByte || StkPlMemCmp(MsgPtr, Utf32, LenUtf32 * sizeof(char32_t)) != 0) {
		StkPlPrintf("UTF32 -> UTF8(%d byte) -> UTF32 ... NG case\n", NumOfByte);
		StkPlExit(0);
	}
	StkPlPrintf("UTF32 -> UTF8(%d byte) -> UTF32 ... OK case\n", NumOfByte);
}

size_t Test_Insufficient_Buffer_Utf16_Utf32(char16_t* MsgPtr, size_t Size)
{
	StkPlPrintf("UTF16 -> UTF32 with insufficient buffer ... ");
	char32_t Utf32[256] = U"";
	char32_t Utf32f[256] = U"";
	size_t LenUtf32 = MessageProc::StkPlConvUtf16ToUtf32(Utf32, Size, MsgPtr);
	size_t LenUtf32f = MessageProc::StkPlConvUtf16ToUtf32(Utf32f, 256, MsgPtr);
	if (LenUtf32 > Size - 1 || LenUtf32 >= LenUtf32f || StkPlMemCmp(Utf32, Utf32f, LenUtf32 * sizeof(char32_t)) != 0) {
		StkPlPrintf("NG case\n");
		StkPlExit(0);
	}
	StkPlPrintf("OK case\n");
	return LenUtf32;
}

size_t Test_Insufficient_Buffer_Utf32_Utf16(char32_t* MsgPtr, size_t Size)
{
	StkPlPrintf("UTF32 -> UTF16 with insufficient buffer ... ");
	char16_t Utf16[256] = u"";
	char16_t Utf16f[256] = u"";
	size_t LenUtf16 = MessageProc::StkPlConvUtf32ToUtf16(Utf16, Size, MsgPtr);
	size_t LenUtf16f = MessageProc::StkPlConvUtf32ToUtf16(Utf16f, 256, MsgPtr);
	if (LenUtf16 > Size - 1 || LenUtf16 >= LenUtf16f || StkPlMemCmp(Utf16, Utf16f, LenUtf16 * sizeof(char16_t)) != 0) {
		StkPlPrintf("NG case\n");
		StkPlExit(0);
	}
	StkPlPrintf("OK case\n");
	return LenUtf16;
}

size_t Test_Insufficient_Buffer_Utf8_Utf32(char* MsgPtr, size_t Size)
{
	StkPlPrintf("UTF8 -> UTF32 with insufficient buffer ... ");
	char32_t Utf32[256] = U"";
	char32_t Utf32f[256] = U"";
	size_t LenUtf32 = MessageProc::StkPlConvUtf8ToUtf32(Utf32, Size, MsgPtr);
	size_t LenUtf32f = MessageProc::StkPlConvUtf8ToUtf32(Utf32f, 256, MsgPtr);
	if (LenUtf32 > Size - 1 || LenUtf32 >= LenUtf32f || StkPlMemCmp(Utf32, Utf32f, LenUtf32 * sizeof(char32_t)) != 0) {
		StkPlPrintf("NG case\n");
		StkPlExit(0);
	}
	StkPlPrintf("OK case\n");
	return LenUtf32;
}

size_t Test_Insufficient_Buffer_Utf32_Utf8(char32_t* MsgPtr, size_t Size)
{
	StkPlPrintf("UTF32 -> UTF8 with insufficient buffer ... ");
	char Utf8[256] = "";
	char Utf8f[256] = "";
	size_t LenUtf8 = MessageProc::StkPlConvUtf32ToUtf8(Utf8, Size, MsgPtr);
	size_t LenUtf8f = MessageProc::StkPlConvUtf32ToUtf8(Utf8f, 256, MsgPtr);
	if (LenUtf8 > Size - 1 || LenUtf8 >= LenUtf8f || StkPlMemCmp(Utf8, Utf8f, LenUtf8) != 0) {
		StkPlPrintf("NG case\n");
		StkPlExit(0);
	}
	StkPlPrintf("OK case\n");
	return LenUtf8;
}

void Test_Invalid_String_Utf16_Utf32(char16_t* MsgPtr, int Count)
{
	StkPlPrintf("Invalid string (UTF16) is presented ... ");
	char32_t Buf[256];
	size_t LenUtf32 = MessageProc::StkPlConvUtf16ToUtf32(Buf, 256, MsgPtr);
	if (LenUtf32 != Count) {
		StkPlPrintf("NG case\n");
		StkPlExit(0);
	}
	StkPlPrintf("OK case\n");
}

void Test_Invalid_String_Utf8_Utf32(char* MsgPtr, int Count)
{
	StkPlPrintf("Invalid string (UTF8) is presented ... ");
	char32_t Buf[256];
	size_t LenUtf32 = MessageProc::StkPlConvUtf8ToUtf32(Buf, 256, MsgPtr);
	if (LenUtf32 != Count) {
		StkPlPrintf("NG case\n");
		StkPlExit(0);
	}
	StkPlPrintf("OK case\n");
}

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

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	{
		Test_Conv_Utf16_Utf32(u"abcdexyz-0123456789あいうえおかきくけこさしすせそたちつてと", 1);
		Test_Conv_Utf16_Utf32(u"𠮷𠮷𠮷𠮷𠀋𡈽𡌛𡑮𡢽𠮟𡚴𡸴𣗄𣜿", 2);
	}

	{
		Test_Conv_Utf8_Utf32(U" !\"#$z{|}~abcdexyz012789", 1);
		Test_Conv_Utf8_Utf32(U"¡¢£¶»¾ÀÁÂÃÄ߶߷߸߹ߺǖǖǛʥʧΠθωҖ", 2);
		Test_Conv_Utf8_Utf32(U"東西南北春夏秋冬魑魅魍魎焼肉定食老若男女", 3);
		Test_Conv_Utf8_Utf32(U"🀀🀁🀂🀃🀄🀅🀆𠀋𡈽𡌛𡑮𡢽", 4);
	}

	{
		Test_Insufficient_Buffer_Utf16_Utf32(u"あいうえおかきくけこ", 5);
		Test_Insufficient_Buffer_Utf32_Utf16(U"𠀋𡈽𡌛𡑮𡢽𠮟𡚴𡸴𣗄𣜿", 5);
		Test_Insufficient_Buffer_Utf32_Utf16(U"あ𠀋𡈽𡌛𡑮𡢽𠮟𡚴𡸴𣗄𣜿", 5);
		Test_Insufficient_Buffer_Utf32_Utf16(U"魑魅魍魎サザンクロス", 5);
		Test_Insufficient_Buffer_Utf8_Utf32("\xe3\x9b\x90\xe3\x9b\x90\xe3\x9b\x90\xe3\x9b\x90\xe3\x9b\x90\xe3\x9b\x90\xe3\x9b\x90", 5);
		Test_Insufficient_Buffer_Utf32_Utf8(U"君が代は千代に八千代にさざれ石の巌となりて苔のむすまで", 5);
	}

	{
		char16_t Input[32] = u"𠀋𡈽𡌛𡑮𡢽𠮟𡚴𡸴𣗄𣜿";
		Input[5] = u'\0';
		Test_Invalid_String_Utf16_Utf32(Input, 2);
	}

	{
		Test_Invalid_String_Utf8_Utf32("\xe3\x9b\x90\xe3\x9b\x90\xe3\x9b\x00\xe3\x9b\x90\xe3\x9b\x90\xe3\x9b\x90\xe3\x9b\x90", 2);
		Test_Invalid_String_Utf8_Utf32("\xe3\x9b\x90\xe3\x9b\x90\xe3\x9b\x90\xe3\x00\x90\xe3\x9b\x90\xe3\x9b\x90\xe3\x9b\x90", 3);
		Test_Invalid_String_Utf8_Utf32("\xe3\x9b\x90\xe3\x9b\x90\xe3\x9b\x90\xe3\x00\x90\xe3\x9b\x90\xe3\x9b\x90\xe3\x9b\x90", 3);
		Test_Invalid_String_Utf8_Utf32("\xf0\x90\x8c\xb0\xf0\x90\x8c\xb0\xf0\x90\x8c\xb0\xf0\x90\x8c\x00\xf0\x90\x8c\xb0", 3);
		Test_Invalid_String_Utf8_Utf32("\xf0\x90\x8c\xb0\xf0\x90\x8c\xb0\xf0\x90\x8c\xb0\xf0\x90\x00\xb0\xf0\x90\x8c\xb0", 3);
		Test_Invalid_String_Utf8_Utf32("\xf0\x90\x8c\xb0\xf0\x90\x8c\xb0\xf0\x90\x8c\xb0\xf0\x00\x8c\xb0\xf0\x90\x8c\xb0", 3);
	}

	StkPlPrintf("MsgProcTest completed.\n\n\n");
}
