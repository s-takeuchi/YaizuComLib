#pragma once
#include <windows.h>
#include <memory.h>

class StkThreadGuiLogging
{
private:
	int MaxLogSize;

	// This instance
	static StkThreadGuiLogging* ThisInstance;

	StkThreadGuiLogging();
	~StkThreadGuiLogging();

	wchar_t *Log;
	int UpdateVersion;

	static CRITICAL_SECTION CriticalSection;

	void JoinToExistedLogStream(wchar_t* Buf);

public:
	// Get this instance
	static StkThreadGuiLogging* GetInstance();
	void ClearLog();
	void AddLog(wchar_t*);
	void AddLogWithThreadInfo(wchar_t*, wchar_t*);
	void GetLog(wchar_t*, int);
	int GetLogUpdateVersion();
	int GetLogMaxSize();
	int GetLogSize();
	void ChangeLogSize(int);
};
