#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "..\..\src\commonfunc\MsgProc.h"

void MsgProcTest()
{
	printf("MsgProcTest started.\n");

	MessageProc::AddJpn(100, _T("‚ ‚¢‚¤‚¦‚¨"));
	MessageProc::AddEng(100, _T("abcde"));

	MessageProc::SetLocaleMode(MessageProc::LOCALE_MODE_ENGLISH);
	if (lstrcmp(MessageProc::GetMsg(100), _T("abcde")) != 0) {
		printf("English message cannot be acquired in the English locale configuration ... NG case\n");
		exit(0);
	} else {
		printf("English message can be acquired in the English locale configuration ... OK case\n");
	}
	if (lstrcmp(MessageProc::GetMsgJpn(100), _T("‚ ‚¢‚¤‚¦‚¨")) != 0) {
		printf("Japanese message cannot not be acquired using GetMsgJpn in the English locale configuration ... NG case\n");
		exit(0);
	} else {
		printf("Japanese message can be acquired using GetMsgJpn() in the English locale configuration ... OK case\n");
	}

	MessageProc::SetLocaleMode(MessageProc::LOCALE_MODE_JAPANESE);
	if (lstrcmp(MessageProc::GetMsg(100), _T("‚ ‚¢‚¤‚¦‚¨")) != 0) {
		printf("Japanese message cannot not be acquired in the Japanese locale configuration ... NG case\n");
		exit(0);
	} else {
		printf("Japanese message can be acquired in the Japanese locale configuration ... OK case\n");
	}
	if (lstrcmp(MessageProc::GetMsgEng(100), _T("abcde")) != 0) {
		printf("English message cannot not be acquired using GetMsgEng in the Japanese locale configuration ... NG case\n");
		exit(0);
	} else {
		printf("English message can be acquired using GetMsgEng() in the Japanese locale configuration ... OK case\n");
	}

	MessageProc::SetLocaleMode(MessageProc::LOCALE_MODE_ENGLISH);
	if (MessageProc::GetMsgSjisEng(100) == NULL || MessageProc::GetMsgSjisJpn(100) == NULL ||
		MessageProc::GetMsgUtf8Eng(100) == NULL || MessageProc::GetMsgUtf8Jpn(100) == NULL) {
		printf("Sjis message and Utf8 message cannot be acquired ... NG case\n");
		exit(0);
	} else {
		printf("Sjis message and Utf8 message can be acquired ... OK case\n");
	}

	if (MessageProc::GetMsg(101) != NULL) {
		printf("GetMsg does not return NULL if undefined message ID is specified ... NG case\n");
		exit(0);
	} else {
		printf("GetMsg returns NULL if undefined message ID is specified ... OK case\n");
	}

	MessageProc::ClearAllMsg();
	if (MessageProc::GetMsg(100) != NULL) {
		printf("Old message can be acquired even if clear API is called ... NG case\n");
		exit(0);
	} else {
		printf("After clear API is called no message can be acquired ... OK case\n");
	}

	MessageProc::AddJpn(101, _T("‚ ‚¢‚¤‚¦‚¨"));
	MessageProc::AddEng(101, _T("abcde"));
	MessageProc::AddJpn(102, _T("‚©‚«‚­‚¯‚±"));
	MessageProc::AddEng(102, _T("fghij"));
	MessageProc::DelJpn(101);
	MessageProc::DelEng(102);
	if (MessageProc::GetMsgEng(101) == NULL || MessageProc::GetMsgSjisEng(101) == NULL || MessageProc::GetMsgUtf8Eng(101) == NULL) {
		printf("Existing English message is not returned ... NG case\n");
		exit(0);
	} else {
		printf("Existing English message is returned ... OK case\n");
	}
	if (MessageProc::GetMsgJpn(102) == NULL || MessageProc::GetMsgSjisJpn(102) == NULL || MessageProc::GetMsgUtf8Jpn(102) == NULL) {
		printf("Existing Japanese message is not returned ... NG case\n");
		exit(0);
	} else {
		printf("Existing Japanese message is returned ... OK case\n");
	}
	if (MessageProc::GetMsgJpn(101) == NULL || MessageProc::GetMsgSjisJpn(101) == NULL || MessageProc::GetMsgUtf8Jpn(101) == NULL) {
		printf("Deleted Japanese message is not returned ... OK case\n");
	} else {
		printf("Deleted Japanese message is returned ... NG case\n");
		exit(0);
	}
	if (MessageProc::GetMsgEng(102) == NULL || MessageProc::GetMsgSjisEng(102) == NULL || MessageProc::GetMsgUtf8Eng(102) == NULL) {
		printf("Deleted English message is not returned ... OK case\n");
	} else {
		printf("Deleted English message is returned ... NG case\n");
		exit(0);
	}


	printf("MsgProcTest completed.\n\n\n");
}
