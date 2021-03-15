#include "DataAccessUm.h"
#include "../stkpl/StkPl.h"
#include "UserManagement.h"
#include "../stkdata/stkdata.h"
#include "../stkdata/stkdataapi.h"

DataAccessUm* DataAccessUm::ThisInstance;

DataAccessUm::DataAccessUm()
{
}

DataAccessUm::~DataAccessUm()
{
}

// Get this instance
DataAccessUm* DataAccessUm::GetInstance()
{
	static int Init = 1;
	if (Init == 1) {
		ThisInstance = new DataAccessUm();
		Init = 0;
	}
	return ThisInstance;
}

// Add log message
// LogMsgEn [in] : Message in English which you want to insert
// LogMsgJa [in] : Message in Japanese which you want to insert
// Return : always zero returned.
int DataAccessUm::AddLogMsg(wchar_t LogMsgEn[UserManagement::MAXLEN_OF_LOGMSG], wchar_t LogMsgJa[UserManagement::MAXLEN_OF_LOGMSG], int UserId)
{
	// Delete old logs
	static int DelFlag = 0;
	if (DelFlag == 0) {
		DeleteOldLogs();
		DelFlag++;
	} else if (DelFlag == 10) {
		DeleteOldLogs();
		DelFlag = 0;
	} else {
		DelFlag++;
	}

	LockTable(L"Log", LOCK_EXCLUSIVE);

	// Get max log ID
	int MaxLogId = GetPropertyValueInt(L"MaxLogId");
	MaxLogId++;

	// Create unix epoch time
	wchar_t LocalTimeBuf[64] = L"";
	StkPlSwPrintf(LocalTimeBuf, 64, L"%016x", StkPlGetTime());

	// New record information
	ColumnData *ColDatLog[5];
	ColDatLog[0] = new ColumnDataInt(L"Id", MaxLogId);
	ColDatLog[1] = new ColumnDataWStr(L"Time", LocalTimeBuf);
	ColDatLog[2] = new ColumnDataInt(L"UserId", UserId);
	ColDatLog[3] = new ColumnDataWStr(L"MessageEn", LogMsgEn);
	ColDatLog[4] = new ColumnDataWStr(L"MessageJa", LogMsgJa);
	RecordData* RecDatLog = new RecordData(L"Log", ColDatLog, 5);
	// Add record
	int Ret = InsertRecord(RecDatLog);
	delete RecDatLog;

	// Set max user ID
	SetPropertyValueInt(L"MaxLogId", MaxLogId);

	UnlockTable(L"Log");

	return 0;
}

// Get number of logs.
// This method gets number of logs
// Return : Number of logs
int DataAccessUm::GetNumOfLogs()
{
	LockTable(L"Log", LOCK_SHARE);
	RecordData* RecDatLog = GetRecord(L"Log");
	UnlockTable(L"Log");

	RecordData* CurrRecDat = RecDatLog;
	int NumOfLogs = 0;
	while (CurrRecDat != NULL) {
		NumOfLogs++;
		CurrRecDat = CurrRecDat->GetNextRecord();
	}
	delete RecDatLog;
	return NumOfLogs;
}

// Get log information
// LogMsgTime [out] : Acquired logging time
// LogMsgEn [out] : Acquired log message in English
// LogMsgJa [out] : Acquired log message in Japan
// Return : Number of acquired log messages
int DataAccessUm::GetLogs(wchar_t LogMsgTime[UserManagement::MAXNUM_OF_LOGRECORDS][UserManagement::MAXLEN_OF_LOGTIME],
	int LogMsgUserId[UserManagement::MAXNUM_OF_LOGRECORDS],
	wchar_t LogMsgEn[UserManagement::MAXNUM_OF_LOGRECORDS][UserManagement::MAXLEN_OF_LOGMSG],
	wchar_t LogMsgJa[UserManagement::MAXNUM_OF_LOGRECORDS][UserManagement::MAXLEN_OF_LOGMSG])
{
	LockTable(L"Log", LOCK_EXCLUSIVE);
	AzSortRecord(L"Log", L"Id");
	RecordData* RecDatLog = GetRecord(L"Log");
	UnlockTable(L"Log");

	int NumOfRec = 0;
	RecordData* CurrRecDat = RecDatLog;
	while (CurrRecDat != NULL) {
		ColumnDataWStr* ColDatTime = (ColumnDataWStr*)CurrRecDat->GetColumn(1);
		ColumnDataInt* ColDatUserId = (ColumnDataInt*)CurrRecDat->GetColumn(2);
		ColumnDataWStr* ColDatMsgEn = (ColumnDataWStr*)CurrRecDat->GetColumn(3);
		ColumnDataWStr* ColDatMsgJa = (ColumnDataWStr*)CurrRecDat->GetColumn(4);
		if (ColDatTime != NULL && ColDatTime->GetValue() != NULL) {
			StkPlWcsCpy(LogMsgTime[NumOfRec], UserManagement::MAXLEN_OF_LOGTIME, ColDatTime->GetValue());
		} else {
			StkPlWcsCpy(LogMsgTime[NumOfRec], UserManagement::MAXLEN_OF_LOGTIME, L"");
		}
		if (ColDatUserId != NULL) {
			LogMsgUserId[NumOfRec] = ColDatUserId->GetValue();
		} else {
			LogMsgUserId[NumOfRec] = 0;
		}
		if (ColDatMsgEn != NULL && ColDatMsgEn->GetValue() != NULL) {
			StkPlWcsCpy(LogMsgEn[NumOfRec], UserManagement::MAXLEN_OF_LOGMSG, ColDatMsgEn->GetValue());
		} else {
			StkPlWcsCpy(LogMsgEn[NumOfRec], UserManagement::MAXLEN_OF_LOGMSG, L"");
		}
		if (ColDatMsgJa != NULL && ColDatMsgJa->GetValue() != NULL) {
			StkPlWcsCpy(LogMsgJa[NumOfRec], UserManagement::MAXLEN_OF_LOGMSG, ColDatMsgJa->GetValue());
		} else {
			StkPlWcsCpy(LogMsgJa[NumOfRec], UserManagement::MAXLEN_OF_LOGMSG, L"");
		}
		NumOfRec++;
		CurrRecDat = CurrRecDat->GetNextRecord();
	}
	delete RecDatLog;
	return NumOfRec;
}

// This function deletes old logs.
// Return : Always zero returned
int DataAccessUm::DeleteOldLogs()
{
	int NumOfLogs = GetNumOfLogs();
	if (NumOfLogs >= 100) {
		LockTable(L"Log", LOCK_EXCLUSIVE);
		AzSortRecord(L"Log", L"Id");
		RecordData* RecDatLog = GetRecord(L"Log");
		UnlockTable(L"Log");
		int ExceededNumOfLogs = NumOfLogs - 99;
		RecordData* CurrRecDat = RecDatLog;
		for (int Loop = 0; Loop < ExceededNumOfLogs; Loop++) {
			ColumnDataInt* ColDatId = (ColumnDataInt*)CurrRecDat->GetColumn(0);
			int ValueId = ColDatId->GetValue();

			ColumnData* DelColDat[1];
			DelColDat[0] = new ColumnDataInt(L"Id", ValueId);
			RecordData* DelRecDat = new RecordData(L"Log", DelColDat, 1);
			LockTable(L"Log", LOCK_EXCLUSIVE);
			DeleteRecord(DelRecDat);
			UnlockTable(L"Log");
			delete DelRecDat;

			CurrRecDat = CurrRecDat->GetNextRecord();
		}
		delete RecDatLog;
	}

	return 0;
}

bool DataAccessUm::GetTargetUserByName(wchar_t Name[UserManagement::MAXLEN_OF_USERNAME], int* Id, wchar_t Password[UserManagement::MAXLEN_OF_PASSWORD], int* Role)
{
	ColumnData* ColDat[1];
	ColDat[0] = new ColumnDataWStr(L"Name", Name);
	RecordData* SearchUser = new RecordData(L"User", ColDat, 1);
	LockTable(L"User", LOCK_SHARE);
	RecordData* RecDatUser = GetRecord(SearchUser);
	UnlockTable(L"User");
	delete SearchUser;
	if (RecDatUser != NULL) {
		ColumnDataInt* ColDatId = (ColumnDataInt*)RecDatUser->GetColumn(0);
		ColumnDataWStr* ColDatPw = (ColumnDataWStr*)RecDatUser->GetColumn(2);
		ColumnDataInt* ColDatRole = (ColumnDataInt*)RecDatUser->GetColumn(3);
		if (ColDatId != NULL && ColDatPw != NULL && ColDatRole != NULL) {
			*Id = ColDatId->GetValue();
			StkPlWcsCpy(Password, UserManagement::MAXLEN_OF_PASSWORD, ColDatPw->GetValue());
			*Role = ColDatRole->GetValue();
		}
	} else {
		return false;
	}
	delete RecDatUser;
	return true;
}

bool DataAccessUm::GetTargetUserById(int Id, wchar_t Name[UserManagement::MAXLEN_OF_USERNAME], wchar_t Password[UserManagement::MAXLEN_OF_PASSWORD], int* Role)
{
	ColumnData* ColDat[1];
	ColDat[0] = new ColumnDataInt(L"Id", Id);
	RecordData* SearchUser = new RecordData(L"User", ColDat, 1);
	LockTable(L"User", LOCK_SHARE);
	RecordData* RecDatUser = GetRecord(SearchUser);
	UnlockTable(L"User");
	delete SearchUser;
	if (RecDatUser != NULL) {
		ColumnDataWStr* ColDatName = (ColumnDataWStr*)RecDatUser->GetColumn(1);
		ColumnDataWStr* ColDatPw = (ColumnDataWStr*)RecDatUser->GetColumn(2);
		ColumnDataInt* ColDatRole = (ColumnDataInt*)RecDatUser->GetColumn(3);
		if (ColDatName != NULL && ColDatPw != NULL && ColDatRole != NULL) {
			StkPlWcsCpy(Name, UserManagement::MAXLEN_OF_USERNAME, ColDatName->GetValue());
			StkPlWcsCpy(Password, UserManagement::MAXLEN_OF_PASSWORD, ColDatPw->GetValue());
			*Role = ColDatRole->GetValue();
		}
	} else {
		return false;
	}
	delete RecDatUser;
	return true;
}

int DataAccessUm::GetTargetUsers(int Id[UserManagement::MAXNUM_OF_USERRECORDS],
	wchar_t Name[UserManagement::MAXNUM_OF_USERRECORDS][UserManagement::MAXLEN_OF_USERNAME],
	wchar_t Password[UserManagement::MAXNUM_OF_USERRECORDS][UserManagement::MAXLEN_OF_PASSWORD],
	int Role[UserManagement::MAXNUM_OF_USERRECORDS])
{
	LockTable(L"User", LOCK_SHARE);
	RecordData* RecDatUser = GetRecord(L"User");
	UnlockTable(L"User");
	RecordData* CurRecDatUser = RecDatUser;
	if (!CurRecDatUser) {
		return 0;
	}
	int Loop = 0;
	for (; CurRecDatUser; Loop++) {
		ColumnDataInt* ColDatId = (ColumnDataInt*)CurRecDatUser->GetColumn(0);
		ColumnDataWStr* ColDatName = (ColumnDataWStr*)CurRecDatUser->GetColumn(1);
		ColumnDataWStr* ColDatPw = (ColumnDataWStr*)CurRecDatUser->GetColumn(2);
		ColumnDataInt* ColDatRole = (ColumnDataInt*)CurRecDatUser->GetColumn(3);
		if (ColDatId != NULL && ColDatName != NULL && ColDatPw != NULL && ColDatRole != NULL) {
			Id[Loop] = ColDatId->GetValue();
			StkPlWcsCpy(Name[Loop], UserManagement::MAXLEN_OF_USERNAME, ColDatName->GetValue());
			StkPlWcsCpy(Password[Loop], UserManagement::MAXLEN_OF_PASSWORD, ColDatPw->GetValue());
			Role[Loop] = ColDatRole->GetValue();
		}
		CurRecDatUser = CurRecDatUser->GetNextRecord();
	}
	delete RecDatUser;
	return Loop;
}

int DataAccessUm::GetNumberOfUsers()
{
	return GetNumOfRecords(L"User");
}

bool DataAccessUm::AddUser(wchar_t Name[UserManagement::MAXLEN_OF_USERNAME], int Role, wchar_t Password[UserManagement::MAXLEN_OF_PASSWORD])
{
	LockTable(L"User", LOCK_EXCLUSIVE);

	// Get max user ID
	int MaxUserId = GetPropertyValueInt(L"MaxUserId");
	MaxUserId++;
	
	// Add new user
	ColumnData* ColDatInsertUser[4];
	ColDatInsertUser[0] = new ColumnDataInt(L"Id", MaxUserId);
	ColDatInsertUser[1] = new ColumnDataWStr(L"Name", Name);
	ColDatInsertUser[2] = new ColumnDataWStr(L"Password", Password);
	ColDatInsertUser[3] = new ColumnDataInt(L"Role", Role);
	RecordData* RecDatInsertUser = new RecordData(L"User", ColDatInsertUser, 4);
	int Ret = InsertRecord(RecDatInsertUser);
	delete RecDatInsertUser;

	// Set max user ID
	SetPropertyValueInt(L"MaxUserId", MaxUserId);

	UnlockTable(L"User");
	return true;
}

bool DataAccessUm::UpdateUser(int Id, wchar_t Name[UserManagement::MAXLEN_OF_USERNAME], int Role, wchar_t Password[UserManagement::MAXLEN_OF_PASSWORD])
{
	int ColLen = 0;
	ColumnData* ColDatUpdUser[4];
	ColDatUpdUser[0] = new ColumnDataInt(L"Id", Id);
	if (StkPlWcsCmp(Name, L"") != 0) {
		ColLen++;
		ColDatUpdUser[ColLen] = new ColumnDataWStr(L"Name", Name);
	}
	if (Role != -1) {
		ColLen++;
		ColDatUpdUser[ColLen] = new ColumnDataInt(L"Role", Role);
	}
	if (Password != NULL && Password[0] != L'\0') {
		ColLen++;
		ColDatUpdUser[ColLen] = new ColumnDataWStr(L"Password", Password);
	}
	RecordData* RecDatUpdUser = new RecordData(L"User", ColDatUpdUser, ColLen + 1);

	ColumnData* ColDatSearchUser[1];
	ColDatSearchUser[0] = new ColumnDataInt(L"Id", Id);
	RecordData* RecDatSearchUser = new RecordData(L"User", ColDatSearchUser, 1);

	LockTable(L"User", LOCK_EXCLUSIVE);
	int Ret = UpdateRecord(RecDatSearchUser, RecDatUpdUser);
	UnlockTable(L"User");
	delete RecDatUpdUser;
	delete RecDatSearchUser;
	return true;
}

bool DataAccessUm::DeleteUser(int Id)
{
	ColumnData* ColDatSearchUser[1];
	ColDatSearchUser[0] = new ColumnDataInt(L"Id", Id);
	RecordData* RecDatSearchUser = new RecordData(L"User", ColDatSearchUser, 1);

	LockTable(L"User", LOCK_EXCLUSIVE);
	int Ret = DeleteRecord(RecDatSearchUser);
	UnlockTable(L"User");
	delete RecDatSearchUser;
	return true;
}

void DataAccessUm::SetPropertyValueInt(const wchar_t* Name, const int Value)
{
	LockTable(L"Property", LOCK_EXCLUSIVE);
	ColumnData* ColDatSearch[1];
	ColDatSearch[0] = new ColumnDataWStr(L"Name", Name);
	RecordData* RecDatSearch = new RecordData(L"Property", ColDatSearch, 1);
	RecordData* RecDatFound = GetRecord(RecDatSearch);

	if (RecDatFound != NULL) {
		// Update
		ColumnData* ColDatUpd[1];
		ColDatUpd[0] = new ColumnDataInt(L"ValueInt", Value);
		RecordData* RecDatUpd = new RecordData(L"Property", ColDatUpd, 1);
		UpdateRecord(RecDatSearch, RecDatUpd);
		delete RecDatUpd;
	} else {
		// Insert
		ColumnData* ColDatInsert[3];
		ColDatInsert[0] = new ColumnDataWStr(L"Name", Name);
		ColDatInsert[1] = new ColumnDataInt(L"ValueInt", Value);
		ColDatInsert[2] = new ColumnDataWStr(L"ValueWStr", L"");
		RecordData* RecDatInsert = new RecordData(L"Property", ColDatInsert, 3);
		InsertRecord(RecDatInsert);
		delete RecDatInsert;
	}
	UnlockTable(L"Property");
	delete RecDatSearch;
	delete RecDatFound;
}

void DataAccessUm::SetPropertyValueWStr(const wchar_t* Name, const wchar_t Value[UserManagement::MAXLEN_OF_PROPERTY_VALUEWSTR])
{
	LockTable(L"Property", LOCK_EXCLUSIVE);
	ColumnData* ColDatSearch[1];
	ColDatSearch[0] = new ColumnDataWStr(L"Name", Name);
	RecordData* RecDatSearch = new RecordData(L"Property", ColDatSearch, 1);
	RecordData* RecDatFound = GetRecord(RecDatSearch);

	if (RecDatFound != NULL) {
		// Update
		ColumnData* ColDatUpd[1];
		ColDatUpd[0] = new ColumnDataWStr(L"ValueWStr", Value);
		RecordData* RecDatUpd = new RecordData(L"Property", ColDatUpd, 1);
		UpdateRecord(RecDatSearch, RecDatUpd);
		delete RecDatUpd;
	} else {
		// Insert
		ColumnData* ColDatInsert[3];
		ColDatInsert[0] = new ColumnDataWStr(L"Name", ((ColumnDataWStr*)RecDatFound->GetColumn(0))->GetValue());
		ColDatInsert[1] = new ColumnDataInt(L"ValueInt", 0);
		ColDatInsert[2] = new ColumnDataWStr(L"ValueWStr", Value);
		RecordData* RecDatInsert = new RecordData(L"Property", ColDatInsert, 3);
		InsertRecord(RecDatInsert);
		delete RecDatInsert;
	}
	UnlockTable(L"Property");
	delete RecDatSearch;
	delete RecDatFound;
}

int DataAccessUm::GetPropertyValueInt(const wchar_t* Name)
{
	int Ret = -1;
	LockTable(L"Property", LOCK_SHARE);
	ColumnData* ColDatSearch[1];
	ColDatSearch[0] = new ColumnDataWStr(L"Name", Name);
	RecordData* RecDatSearch = new RecordData(L"Property", ColDatSearch, 1);
	RecordData* RecDatFound = GetRecord(RecDatSearch);
	if (RecDatFound) {
		Ret = ((ColumnDataInt*)RecDatFound->GetColumn(1))->GetValue();
	}
	UnlockTable(L"Property");
	delete RecDatSearch;
	delete RecDatFound;
	return Ret;
}

void DataAccessUm::GetPropertyValueWStr(const wchar_t* Name, wchar_t Value[UserManagement::MAXLEN_OF_PROPERTY_VALUEWSTR])
{
	LockTable(L"Property", LOCK_SHARE);
	ColumnData* ColDatSearch[1];
	ColDatSearch[0] = new ColumnDataWStr(L"Name", Name);
	RecordData* RecDatSearch = new RecordData(L"Property", ColDatSearch, 1);
	RecordData* RecDatFound = GetRecord(RecDatSearch);
	if (RecDatFound) {
		StkPlWcsCpy(Value, UserManagement::MAXLEN_OF_PROPERTY_VALUEWSTR, ((ColumnDataWStr*)RecDatFound->GetColumn(2))->GetValue());
	}
	UnlockTable(L"Property");
	delete RecDatSearch;
	delete RecDatFound;
}

// Create tables for CmdFreak
// Return : [0:Success, -1:Failed]
int DataAccessUm::CreateUserTable()
{
	// User table
	ColumnDefInt ColDefUserId(L"Id");
	ColumnDefWStr ColDefUserName(L"Name", UserManagement::MAXLEN_OF_USERNAME);
	ColumnDefWStr ColDefUserPassword(L"Password", UserManagement::MAXLEN_OF_PASSWORD);
	ColumnDefInt ColDefUserRole(L"Role");
	TableDef TabDefUser(L"User", UserManagement::MAXNUM_OF_USERRECORDS);
	TabDefUser.AddColumnDef(&ColDefUserId);
	TabDefUser.AddColumnDef(&ColDefUserName);
	TabDefUser.AddColumnDef(&ColDefUserPassword);
	TabDefUser.AddColumnDef(&ColDefUserRole);
	if (CreateTable(&TabDefUser) != 0) {
		UnlockAllTable();
		return -1;
	}

	// Log table
	ColumnDefInt ColDefLogId(L"Id");
	ColumnDefWStr ColDefLogTime(L"Time", UserManagement::MAXLEN_OF_LOGTIME);
	ColumnDefInt ColDefLogUserId(L"UserId");
	ColumnDefWStr ColDefLogMsgEn(L"MessageEn", UserManagement::MAXLEN_OF_LOGMSG);
	ColumnDefWStr ColDefLogMsgJa(L"MessageJa", UserManagement::MAXLEN_OF_LOGMSG);
	TableDef TabDefLog(L"Log", UserManagement::MAXNUM_OF_LOGRECORDS);
	TabDefLog.AddColumnDef(&ColDefLogId);
	TabDefLog.AddColumnDef(&ColDefLogTime);
	TabDefLog.AddColumnDef(&ColDefLogUserId);
	TabDefLog.AddColumnDef(&ColDefLogMsgEn);
	TabDefLog.AddColumnDef(&ColDefLogMsgJa);
	if (CreateTable(&TabDefLog) != 0) {
		UnlockAllTable();
		return -1;
	}

	// Property table
	ColumnDefWStr ColDefPropertyName(L"Name", UserManagement::MAXLEN_OF_PROPERTY_NAME);
	ColumnDefInt ColDefPropertyValueInt(L"ValueInt");
	ColumnDefWStr ColDefPropertyValueWStr(L"ValueWStr", UserManagement::MAXLEN_OF_PROPERTY_VALUEWSTR);
	TableDef TabDefProperty(L"Property", UserManagement::MAXNUM_OF_PROPERTY_RECORDS);
	TabDefProperty.AddColumnDef(&ColDefPropertyName);
	TabDefProperty.AddColumnDef(&ColDefPropertyValueInt);
	TabDefProperty.AddColumnDef(&ColDefPropertyValueWStr);
	if (CreateTable(&TabDefProperty) != 0) {
		UnlockAllTable();
		return -1;
	}

	// Add records for User
	{
		ColumnData *ColDatUser[4];
		ColDatUser[0] = new ColumnDataInt(L"Id", 0);
		ColDatUser[1] = new ColumnDataWStr(L"Name", L"admin");
		ColDatUser[2] = new ColumnDataWStr(L"Password", L"manager");
		ColDatUser[3] = new ColumnDataInt(L"Role", 0);
		RecordData* RecUser = new RecordData(L"User", ColDatUser, 4);
		// Add record
		LockTable(L"User", LOCK_EXCLUSIVE);
		int Ret = InsertRecord(RecUser);
		UnlockTable(L"User");
		delete RecUser;
	}
	// For property table
	{
		SetPropertyValueInt(L"MaxUserId", 0);
		SetPropertyValueInt(L"MaxLogId", 0);
	}

	return 0;
}
