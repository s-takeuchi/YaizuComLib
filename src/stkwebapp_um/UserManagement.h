#pragma once

#include "../stkwebapp/StkWebApp.h"

class UserManagement
{
public:

	static const int MAXLEN_OF_USERNAME = 32;
	static const int MAXLEN_OF_PASSWORD = 32;
	static const int MAXNUM_OF_USERRECORDS = 64;
	static const int MAXLEN_OF_LOGTIME = 32;
	static const int MAXLEN_OF_LOGMSG = 100;
	static const int MAXNUM_OF_LOGRECORDS = 111;
	static const int MAXLEN_OF_PROPERTY_NAME = 256;
	static const int MAXLEN_OF_PROPERTY_VALUEWSTR = 256;
	static const int MAXNUM_OF_PROPERTY_RECORDS = 1024;


	static const int UM_AUTH_ERROR = 2001;
	static const int UM_ACCESS_RIGHT_ERROR = 2002;
	static const int UM_USER_DOES_NOT_EXIST = 2003;
	static const int UM_CANNOT_MODIFY_YOUR_INFO = 2004;
	static const int UM_REQ_NOT_SUFFICIENT = 2005;
	static const int UM_PARAM_LENGTH_TOO_LONG = 2006;
	static const int UM_INVALID_LEN_OF_USER_NAME = 2007;
	static const int UM_INVALID_LEN_OF_PASSWORD = 2008;
	static const int UM_INVALID_USER_NAME = 2009;
	static const int UM_INVALID_PASSWORD = 2010;
	static const int UM_EXCEEDED_MAX_NUM_OF_USER = 2011;
	static const int UM_SAME_USER_NAME_EXIST = 2012;
	static const int UM_PASSWORD_REQUIRED_FOR_ADD = 2013;
	static const int UM_NO_CLIENTLOCALE = 2014;

	static const int UM_USER_ADD = 2301;
	static const int UM_USER_DELETE = 2302;
	static const int UM_USER_EDIT = 2303;

	static void AddMsg();
	static wchar_t* GetMsg(int);
	static char* GetMsgSjis(int);
	static wchar_t* GetMsgJpn(int);
	static wchar_t* GetMsgEng(int);

	static void RegisterApi(StkWebApp*);
	static void UnregisterApi(StkWebApp*);
};
