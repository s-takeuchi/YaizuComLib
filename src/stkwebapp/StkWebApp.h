#pragma once

#include <windows.h>
#include "..\commonfunc\StkObject.h"
#include "StkWebAppExec.h"

class StkWebApp
{
private:
	class Impl;
	Impl* pImpl;

public:
	static const int STKWEBAPP_METHOD_UNDEFINED = -1;
	static const int STKWEBAPP_METHOD_GET = 0;
	static const int STKWEBAPP_METHOD_HEAD = 1;
	static const int STKWEBAPP_METHOD_POST = 2;
	static const int STKWEBAPP_METHOD_PUT = 3;
	static const int STKWEBAPP_METHOD_DELETE = 4;

public:
	StkWebApp(int*, int, TCHAR*, int);
	virtual ~StkWebApp();

	static StkWebApp* GetStkWebAppByThreadId(int);

	BOOL Contains(int);

	int ThreadLoop(int);

	int AddReqHandler(StkObject*, StkWebAppExec*);
	int AddReqHandler(int, TCHAR[128], StkWebAppExec*);
	int DeleteReqHandler(StkObject*);
	int DeleteReqHandler(int, TCHAR[128]);
};
