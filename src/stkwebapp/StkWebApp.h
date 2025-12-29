#pragma once

#include "../stkpl/StkPl.h"
#include "StkWebAppExec.h"

class StkWebApp
{
private:
	class Impl;
	Impl* pImpl;

public:
	StkWebApp(int*, int, const wchar_t*, int, const char* PrivateKey = NULL, const char* Certificate = NULL);
	virtual ~StkWebApp();

	int GetTimeoutInterval();
	void SetTimeoutInterval(int);

	int AddReqHandler(int, const wchar_t[StkWebAppExec::URL_PATH_LENGTH], StkWebAppExec*);
	int DeleteReqHandler(int, const wchar_t[StkWebAppExec::URL_PATH_LENGTH]);

	void TheLoop();
	bool IsStop();
	int GetStatusOfApiCall(int, long long*, long long*, wchar_t[StkWebAppExec::URL_PATH_LENGTH]);

	// Do not call APIs shown below. These are only for internal use.
	static StkWebApp* GetStkWebAppByThreadId(int);
	bool Contains(int);
	int ThreadLoop(int);
};
