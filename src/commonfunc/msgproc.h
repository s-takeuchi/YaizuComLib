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

	static size_t ConvUtf16ToUtf32(char32_t*, size_t, const char16_t*);
	static size_t ConvUtf32ToUtf16(char16_t*, size_t, const char32_t*);
	static size_t ConvUtf8ToUtf32(char32_t*, size_t, const char*);
	static size_t ConvUtf32ToUtf8(char*, size_t, const char32_t*);
	static size_t ConvUtf8ToUtf16(char16_t*, size_t, const char*);
	static size_t ConvUtf16ToUtf8(char*, size_t, const char16_t*);

	static char32_t* CreateUtf32FromUtf16(const char16_t*);
	static char16_t* CreateUtf16FromUtf32(const char32_t*);
	static char32_t* CreateUtf32FromUtf8(const char*);
	static char* CreateUtf8FromUtf32(const char32_t*);
	static char16_t* CreateUtf16FromUtf8(const char*);
	static char* CreateUtf8FromUtf16(const char16_t*);
};
