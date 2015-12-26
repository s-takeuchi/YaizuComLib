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

	printf("MsgProcTest completed.\n\n\n");
}
