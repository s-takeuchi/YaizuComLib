#pragma once
#include "../stkwebapp/StkWebAppExec.h"

class ApiBase : public StkWebAppExec
{
protected:
	void AddCodeAndMsg(StkObject*, int, wchar_t*, wchar_t*);
	void PrintRequest(unsigned int, int, wchar_t[StkWebAppExec::URL_PATH_LENGTH]);
	void PrintResponse(unsigned int, int);
	void DebugObject(StkObject*);
	virtual StkObject* ExecuteImpl(StkObject*, int, wchar_t[StkWebAppExec::URL_PATH_LENGTH], int*, wchar_t[3], wchar_t*) = 0;
	bool CheckCredentials(wchar_t*, wchar_t*);
	bool IsAdminUser(wchar_t*);

public:
	virtual StkObject* Execute(StkObject*, int, wchar_t[StkWebAppExec::URL_PATH_LENGTH], int*, wchar_t*);
};
