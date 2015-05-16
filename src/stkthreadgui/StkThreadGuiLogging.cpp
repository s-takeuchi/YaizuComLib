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
	Log = new TCHAR[MaxLogSize];
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
	lstrcpy(Log, _T(""));
	UpdateVersion = 0;
	LeaveCriticalSection(&CriticalSection);
}

void StkThreadGuiLogging::JoinToExistedLogStream(TCHAR* Buf)
{
	lstrcat(Buf, Log);
	lstrcpyn(Log, Buf, MaxLogSize);
	if (lstrlen(Buf) >= MaxLogSize) {
		Log[MaxLogSize - 25] = '\0';
		lstrcat(Log, _T("...[End of log stream]"));
	}
}

void StkThreadGuiLogging::AddLog(TCHAR* Message)
{
	EnterCriticalSection(&CriticalSection);
	TCHAR *TmpBuf = new TCHAR[MaxLogSize + lstrlen(Message) + 1]; // 追加するメッセージの最大長を512文字と想定
	lstrcpy(TmpBuf, Message);
	JoinToExistedLogStream(TmpBuf);
	delete TmpBuf;
	UpdateVersion++;
	LeaveCriticalSection(&CriticalSection);
}

void StkThreadGuiLogging::AddLogWithThreadInfo(TCHAR* Name, TCHAR* Message)
{
	EnterCriticalSection(&CriticalSection);
	SYSTEMTIME SysTm;
	GetLocalTime(&SysTm);
	TCHAR *TmpBuf = new TCHAR[MaxLogSize + lstrlen(Message) + 100]; // 追加するメッセージの最大長を512文字と想定
	wsprintf(TmpBuf, _T("%02d:%02d:%02d [%s]  %s"), SysTm.wHour, SysTm.wMinute, SysTm.wSecond, Name, Message);
	JoinToExistedLogStream(TmpBuf);
	delete TmpBuf;
	UpdateVersion++;
	LeaveCriticalSection(&CriticalSection);
}

void StkThreadGuiLogging::GetLog(TCHAR* Out, int Length)
{
	if (Length > MaxLogSize) {
		Out[0] = '\0';
		return;
	}
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
	Log = new TCHAR[MaxLogSize];
	Log[0] = '\0';
}
