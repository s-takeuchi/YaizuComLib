#include "StkThreadGuiLogging.h"

StkThreadGuiLogging* StkThreadGuiLogging::ThisInstance;
CRITICAL_SECTION StkThreadGuiLogging::CriticalSection;

// Get this instance
StkThreadGuiLogging* StkThreadGuiLogging::GetInstance()
{
	static int Init = 1;
	if (Init == 1) {
		ThisInstance = new StkThreadGuiLogging();
		Init = 0;
	}
	return ThisInstance;
}

StkThreadGuiLogging::StkThreadGuiLogging()
{
	MaxLogSize = 10000;
	Log = new wchar_t[MaxLogSize];
	Log[0] = '\0';
	InitializeCriticalSection(&StkThreadGuiLogging::CriticalSection);
	ClearLog();
}

StkThreadGuiLogging::~StkThreadGuiLogging()
{
	delete Log;
}

void StkThreadGuiLogging::ClearLog()
{
	EnterCriticalSection(&CriticalSection);
	lstrcpy(Log, L"");
	UpdateVersion = 0;
	LeaveCriticalSection(&CriticalSection);
}

void StkThreadGuiLogging::JoinToExistedLogStream(wchar_t* Buf)
{
	lstrcat(Buf, Log);
	lstrcpyn(Log, Buf, MaxLogSize);
	if (lstrlen(Buf) >= MaxLogSize) {
		Log[MaxLogSize - 25] = '\0';
		lstrcat(Log, L"...[End of log stream]");
	}
}

void StkThreadGuiLogging::AddLog(wchar_t* Message)
{
	EnterCriticalSection(&CriticalSection);
	wchar_t *TmpBuf = new wchar_t[MaxLogSize + lstrlen(Message) + 1]; // 追加するメッセージの最大長を512文字と想定
	lstrcpy(TmpBuf, Message);
	JoinToExistedLogStream(TmpBuf);
	delete TmpBuf;
	UpdateVersion++;
	LeaveCriticalSection(&CriticalSection);
}

void StkThreadGuiLogging::AddLogWithThreadInfo(wchar_t* Name, wchar_t* Message)
{
	EnterCriticalSection(&CriticalSection);
	SYSTEMTIME SysTm;
	GetLocalTime(&SysTm);
	wchar_t *TmpBuf = new wchar_t[MaxLogSize + lstrlen(Message) + 100]; // 追加するメッセージの最大長を512文字と想定
	wsprintf(TmpBuf, L"%02d:%02d:%02d [%s]  %s", SysTm.wHour, SysTm.wMinute, SysTm.wSecond, Name, Message);
	JoinToExistedLogStream(TmpBuf);
	delete TmpBuf;
	UpdateVersion++;
	LeaveCriticalSection(&CriticalSection);
}

void StkThreadGuiLogging::GetLog(wchar_t* Out, int Length)
{
	lstrcpyn(Out, Log, Length);

	return;
}

int StkThreadGuiLogging::GetLogMaxSize()
{
	return MaxLogSize;
}

int StkThreadGuiLogging::GetLogSize()
{
	return lstrlen(Log);
}

int StkThreadGuiLogging::GetLogUpdateVersion()
{
	return UpdateVersion;
}

void StkThreadGuiLogging::ChangeLogSize(int Size)
{
	if (Size < 100) {
		Size = 100;
	}
	MaxLogSize = Size;
	delete Log;
	Log = new wchar_t[MaxLogSize];
	Log[0] = '\0';
}
