#pragma once
#include "../stkpl/StkPl.h"
#include "UserManagement.h"

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

	int AddLogMsg(wchar_t[UserManagement::MAXLEN_OF_LOGMSG], wchar_t[UserManagement::MAXLEN_OF_LOGMSG], int UserId = -1);
	int GetNumOfLogs();
	int GetLogs(wchar_t[UserManagement::MAXNUM_OF_LOGRECORDS][UserManagement::MAXLEN_OF_LOGTIME], int[UserManagement::MAXNUM_OF_LOGRECORDS], wchar_t[UserManagement::MAXNUM_OF_LOGRECORDS][UserManagement::MAXLEN_OF_LOGMSG], wchar_t[UserManagement::MAXNUM_OF_LOGRECORDS][UserManagement::MAXLEN_OF_LOGMSG], int);
	int DeleteOldLogs();

	bool GetTargetUserByName(wchar_t[UserManagement::MAXLEN_OF_USERNAME], int*, wchar_t[UserManagement::MAXLEN_OF_PASSWORD], int*);
	bool GetTargetUserById(int, wchar_t[UserManagement::MAXLEN_OF_USERNAME], wchar_t[UserManagement::MAXLEN_OF_PASSWORD], int*);
	int GetTargetUsers(int[UserManagement::MAXNUM_OF_USERRECORDS], wchar_t[UserManagement::MAXNUM_OF_USERRECORDS][UserManagement::MAXLEN_OF_USERNAME], wchar_t[UserManagement::MAXNUM_OF_USERRECORDS][UserManagement::MAXLEN_OF_PASSWORD], int[UserManagement::MAXNUM_OF_USERRECORDS]);
	int GetNumberOfUsers();
	bool AddUser(wchar_t[UserManagement::MAXLEN_OF_USERNAME], int, wchar_t[UserManagement::MAXLEN_OF_PASSWORD]);
	bool UpdateUser(int, wchar_t[UserManagement::MAXLEN_OF_USERNAME], int, wchar_t[UserManagement::MAXLEN_OF_PASSWORD]);
	bool DeleteUser(int);

	void SetPropertyValueInt(const wchar_t*, const int);
	void SetPropertyValueWStr(const wchar_t*, const wchar_t[UserManagement::MAXLEN_OF_PROPERTY_VALUEWSTR]);
	int GetPropertyValueInt(const wchar_t*);
	void GetPropertyValueWStr(const wchar_t*, wchar_t[UserManagement::MAXLEN_OF_PROPERTY_VALUEWSTR]);
	int GetAllPropertyData(wchar_t[UserManagement::MAXNUM_OF_PROPERTY_RECORDS][UserManagement::MAXLEN_OF_PROPERTY_NAME],
							int[UserManagement::MAXNUM_OF_PROPERTY_RECORDS],
							wchar_t[UserManagement::MAXNUM_OF_PROPERTY_RECORDS][UserManagement::MAXLEN_OF_PROPERTY_VALUEWSTR]);
	void DeleteProperty(const wchar_t*);

	int CreateUserTable();
};
