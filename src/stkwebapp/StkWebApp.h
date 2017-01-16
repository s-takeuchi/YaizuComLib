#pragma once
#include "..\commonfunc\StkObject.h"
#include "StkWebAppExec.h"

class StkWebApp
{
private:
	class Impl;
	Impl* pImpl;

public:
	StkWebApp(int*, int, TCHAR*, int);
	virtual ~StkWebApp();

	static StkWebApp* GetStkWebAppByThreadId(int);

	BOOL Contains(int);

	int ThreadLoop(int);

	int AddReqHandler(StkObject*, StkWebAppExec*);
	int DeleteReqHandler(StkObject*);
};
