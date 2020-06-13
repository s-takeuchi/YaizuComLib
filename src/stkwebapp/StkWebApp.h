#pragma once

#include "StkWebAppExec.h"

class StkWebApp
{
private:
	class Impl;
	Impl* pImpl;

public:
	StkWebApp(int*, int, const wchar_t*, int, const char* PrivateKey = NULL, const char* Certificate = NULL);
	virtual ~StkWebApp();

	int GetSendBufSize();
	void SetSendBufSize(int);
	int GetRecvBufSize();
	void SetRecvBufSize(int);

	int GetTimeoutInterval();
	void SetTimeoutInterval(int);

	int AddReqHandler(int, const wchar_t[StkWebAppExec::URL_PATH_LENGTH], StkWebAppExec*);
	int DeleteReqHandler(int, const wchar_t[StkWebAppExec::URL_PATH_LENGTH]);

	void TheLoop();

	// Do not call APIs shown below. These are only for internal use.
	static StkWebApp* GetStkWebAppByThreadId(int);
	bool Contains(int);
	int ThreadLoop(int);
};
