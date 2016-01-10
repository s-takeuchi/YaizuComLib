#pragma once
#include <windows.h>
#include <tchar.h>

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
	static TCHAR* GetMsg(int);
	static TCHAR* GetMsgEng(int);
	static TCHAR* GetMsgJpn(int);
	static BYTE* GetMsgSjis(int);
	static BYTE* GetMsgSjisEng(int);
	static BYTE* GetMsgSjisJpn(int);
	static BYTE* GetMsgUtf8(int);
	static BYTE* GetMsgUtf8Eng(int);
	static BYTE* GetMsgUtf8Jpn(int);

	static void AddEng(int, TCHAR*);
	static void AddJpn(int, TCHAR*);

	static void ClearAllMsg();
};
