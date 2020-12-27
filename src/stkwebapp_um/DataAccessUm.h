#pragma once
#include "../stkpl/StkPl.h"
#include "stkwebapp_um.h"

class DataAccessUm
{
private:
	// This instance
	static DataAccessUm* ThisInstance;
	DataAccessUm();
	~DataAccessUm();

public:
	// Get this instance
	static DataAccessUm* GetInstance();

	int AddLogMsg(wchar_t[MAXLEN_OF_LOGMSG], wchar_t[MAXLEN_OF_LOGMSG]);
	int GetMaxLogId();
	int GetNumOfLogs();
	int GetLogs(wchar_t[MAXNUM_OF_LOGRECORDS][MAXLEN_OF_LOGTIME], wchar_t[MAXNUM_OF_LOGRECORDS][MAXLEN_OF_LOGMSG], wchar_t[MAXNUM_OF_LOGRECORDS][MAXLEN_OF_LOGMSG]);
	int DeleteOldLogs();

	bool GetTargetUserByName(wchar_t[MAXLEN_OF_USERNAME], int*, wchar_t[MAXLEN_OF_PASSWORD], int*);
	bool GetTargetUserById(int, wchar_t[MAXLEN_OF_USERNAME], wchar_t[MAXLEN_OF_PASSWORD], int*);
	int GetTargetUsers(int[MAXNUM_OF_USERRECORDS], wchar_t[MAXNUM_OF_USERRECORDS][MAXLEN_OF_USERNAME], wchar_t[MAXNUM_OF_USERRECORDS][MAXLEN_OF_PASSWORD], int[MAXNUM_OF_USERRECORDS]);
	int GetNumberOfUsers();
	bool AddUser(wchar_t[MAXLEN_OF_USERNAME], int, wchar_t[MAXLEN_OF_PASSWORD]);
	bool UpdateUser(int, wchar_t[MAXLEN_OF_USERNAME], int, wchar_t[MAXLEN_OF_PASSWORD]);
	bool DeleteUser(int);

	int CreateUserTable();
};
