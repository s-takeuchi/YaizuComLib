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
	static const unsigned char STKWEBAPP_METHOD_INVALID   = 0x20;

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
