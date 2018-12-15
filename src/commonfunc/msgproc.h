#pragma once
#include <windows.h>

class MessageProc
{
private:
	MessageProc();
	virtual ~MessageProc();

	class Impl;
	Impl* pImpl;

public:
	static const int LOCALE_MODE_WIN32 = 0;
	static const int LOCALE_MODE_WEB = 1;
	static const int LOCALE_MODE_ENGLISH = 10;
	static const int LOCALE_MODE_JAPANESE = 11;

	static void SetLocaleMode(int);

	static wchar_t* GetMsg(int);
	static wchar_t* GetMsgEng(int);
	static wchar_t* GetMsgJpn(int);
	static unsigned char* GetMsgSjis(int);
	static unsigned char* GetMsgSjisEng(int);
	static unsigned char* GetMsgSjisJpn(int);
	static unsigned char* GetMsgUtf8(int);
	static unsigned char* GetMsgUtf8Eng(int);
	static unsigned char* GetMsgUtf8Jpn(int);

	static void AddEng(int, wchar_t*);
	static void AddJpn(int, wchar_t*);
	static void DelEng(int);
	static void DelJpn(int);

	static void ClearAllMsg();
};
