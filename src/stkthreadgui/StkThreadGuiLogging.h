#pragma once
#include <windows.h>
#include <tchar.h>
#include <memory.h>

class StkThreadGuiLogging
{
private:
	int MaxLogSize;

	// This instance
	static StkThreadGuiLogging* ThisInstance;

	StkThreadGuiLogging();
	~StkThreadGuiLogging();

	TCHAR *Log;
	int UpdateVersion;

	static CRITICAL_SECTION CriticalSection;

	void JoinToExistedLogStream(TCHAR* Buf);

public:
	// Get this instance
	static StkThreadGuiLogging* GetInstance();
	void ClearLog();
	void AddLog(TCHAR*);
	void AddLogWithThreadInfo(TCHAR*, TCHAR*);
	void GetLog(TCHAR*, int);
	int GetLogUpdateVersion();
	int GetLogMaxSize();
	int GetLogSize();
	void ChangeLogSize(int);
};
