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

	static const int MLANG_ENGLISH = 0;
	static const int MLANG_JAPANESE = 1;

	static void SetLocaleMode(int);
	static TCHAR* GetMsg(int);
	static TCHAR* GetMsgEng(int);
	static TCHAR* GetMsgJpn(int);
	static BYTE* GetMsgSjis(int);
	static BYTE* GetMsgSjisEng(int);
	static BYTE* GetMsgSjisJpn(int);
	static void StkErr(int, HWND);
	static void StkErr(int, TCHAR*, HWND);
	static void StkInf(int, HWND);
	static void StkInf(int, TCHAR*, HWND);
	static int StkYesNo(int, HWND);
	static int StkYesNo(int, TCHAR*, HWND);

	static void AddEng(int, TCHAR*);
	static void AddJpn(int, TCHAR*);

};
