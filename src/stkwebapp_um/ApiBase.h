#pragma once
#include "../stkwebapp/StkWebAppExec.h"
#include "UserManagement.h"

class ApiBase : public StkWebAppExec
{
protected:
	void AddCodeAndMsg(StkObject*, int, const wchar_t*, const wchar_t*);
	void PrintRequest(unsigned int, int, wchar_t[StkWebAppExec::URL_PATH_LENGTH]);
	void PrintResponse(unsigned int, int);
	void DebugObject(StkObject*);
	virtual StkObject* ExecuteImpl(StkObject*, int, wchar_t[StkWebAppExec::URL_PATH_LENGTH], int*, wchar_t[3], wchar_t*) = 0;
	bool CheckCredentials(wchar_t*, wchar_t*, int*);
	bool IsAdminUser(wchar_t*);

public:
	static const int MAXLEN_OF_USERNAME = UserManagement::MAXLEN_OF_USERNAME;
	static const int MAXLEN_OF_PASSWORD = UserManagement::MAXLEN_OF_PASSWORD;
	static const int MAXNUM_OF_USERRECORDS = UserManagement::MAXNUM_OF_USERRECORDS;
	static const int MAXLEN_OF_LOGTIME = UserManagement::MAXLEN_OF_LOGTIME;
	static const int MAXLEN_OF_LOGMSG = UserManagement::MAXLEN_OF_LOGMSG;
	static const int MAXNUM_OF_LOGRECORDS = UserManagement::MAXNUM_OF_LOGRECORDS;

	virtual StkObject* Execute(StkObject*, int, wchar_t[StkWebAppExec::URL_PATH_LENGTH], int*, wchar_t*);
};
