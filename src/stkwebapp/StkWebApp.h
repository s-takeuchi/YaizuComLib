#pragma once

#include <windows.h>
#include "StkWebAppExec.h"

class StkWebApp
{
private:
	class Impl;
	Impl* pImpl;

public:
	static const unsigned char STKWEBAPP_METHOD_UNDEFINED = 0x00;
	static const unsigned char STKWEBAPP_METHOD_GET       = 0x01;
	static const unsigned char STKWEBAPP_METHOD_HEAD      = 0x02;
	static const unsigned char STKWEBAPP_METHOD_POST      = 0x04;
	static const unsigned char STKWEBAPP_METHOD_PUT       = 0x08;
	static const unsigned char STKWEBAPP_METHOD_DELETE    = 0x10;

public:
	StkWebApp(int*, int, TCHAR*, int);
	virtual ~StkWebApp();

	static StkWebApp* GetStkWebAppByThreadId(int);

	BOOL Contains(int);

	int ThreadLoop(int);

	int AddReqHandler(int, TCHAR[128], StkWebAppExec*);
	int DeleteReqHandler(int, TCHAR[128]);
};
