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

	int AddReqHandler(int, TCHAR[128], StkWebAppExec*);
	int DeleteReqHandler(int, TCHAR[128]);

	void TheLoop();

	// Do not call APIs shown below. These are only for internal use.
	static StkWebApp* GetStkWebAppByThreadId(int);
	BOOL Contains(int);
	int ThreadLoop(int);
};
