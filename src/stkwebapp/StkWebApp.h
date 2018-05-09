#pragma once

#include <windows.h>
#include "StkWebAppExec.h"

class StkWebApp
{
private:
	class Impl;
	Impl* pImpl;

public:
	StkWebApp(int*, int, TCHAR*, int);
	virtual ~StkWebApp();

	int GetSendBufSize();
	void SetSendBufSize(int);
	int GetRecvBufSize();
	void SetRecvBufSize(int);

	int GetTimeoutInterval();
	void SetTimeoutInterval(int);

	int AddReqHandler(int, TCHAR[StkWebAppExec::URL_PATH_LENGTH], StkWebAppExec*);
	int DeleteReqHandler(int, TCHAR[StkWebAppExec::URL_PATH_LENGTH]);

	void TheLoop();

	// Do not call APIs shown below. These are only for internal use.
	static StkWebApp* GetStkWebAppByThreadId(int);
	BOOL Contains(int);
	int ThreadLoop(int);
};
