#pragma once

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

	static const int LOG_TYPE_FATAL = 0;
	static const int LOG_TYPE_ERROR = 1;
	static const int LOG_TYPE_WARN = 2;
	static const int LOG_TYPE_INFO = 3;

	static void SetLocaleMode(int);

	static wchar_t* GetMsg(int);
	static wchar_t* GetMsgEng(int);
	static wchar_t* GetMsgJpn(int);
	static char* GetMsgSjis(int);
	static char* GetMsgSjisEng(int);
	static char* GetMsgSjisJpn(int);
	static char* GetMsgUtf8(int);
	static char* GetMsgUtf8Eng(int);
	static char* GetMsgUtf8Jpn(int);

	static void AddEng(int, const wchar_t*);
	static void AddJpn(int, const wchar_t*);
	static void DelEng(int);
	static void DelJpn(int);

	static void ClearAllMsg();

	static int StartLogging(const wchar_t*);
	static void AddLog(const char*, int);
	static int StopLogging();
};
