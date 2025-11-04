#ifdef WIN32
    #include <windows.h>
#endif
#include <sql.h>
#include <sqlext.h>

#include "DbAccessor.h"
#include "../../../YaizuComLib/src/commonfunc/StkObject.h"
#include "../../../YaizuComLib/src/stkpl/StkPl.h"

class DbAccessor::Impl
{
public:
	wchar_t* ConnectionString;

	SQLHENV  Henv;
	SQLHDBC  Hdbc;
	SQLHSTMT Hstmt;
};

DbAccessor::DbAccessor(wchar_t* TmpConnStr)
{
	pImpl = new Impl;
	pImpl->Henv = 0;
	pImpl->Hdbc = 0;
	pImpl->Hstmt = 0;
	size_t Len = StkPlWcsLen(TmpConnStr);
	pImpl->ConnectionString = new wchar_t[Len + 1];
	StkPlWcsCpy(pImpl->ConnectionString, Len + 1, TmpConnStr);
}

DbAccessor::~DbAccessor()
{
	delete[] pImpl->ConnectionString;
	delete pImpl;
}

void DbAccessor::ConvertMessage(wchar_t StateMsg[10], wchar_t Msg[1024], const char16_t SqlStateMsg[10], const char16_t SqlMsg[1024])
{
	StkPlConvUtf16ToWideChar(StateMsg, 10, SqlStateMsg);
	StkPlConvUtf16ToWideChar(Msg, 1024, SqlMsg);
}

// Return 0: Success, -1:Error
int DbAccessor::Test(wchar_t StateMsg[10], wchar_t Msg[1024])
{
	int Ret = 0;
	Ret = OpenDatabase(StateMsg, Msg);
	if (Ret != 0) {
		return Ret;
	}
	Ret = CloseDatabase(StateMsg, Msg);
	if (Ret != 0) {
		return Ret;
	}
	return 0;
}

// Return 0: Success, -1:Error
int DbAccessor::GetTablesCommon(const wchar_t* Query, StkObject* Obj, wchar_t StateMsg[10], wchar_t Msg[1024])
{
	SQLWCHAR CvtStateMsg[10];
	SQLWCHAR CvtMsg[1024];
	SQLINTEGER Native; // This will not be refered from anywhere
	SQLSMALLINT ActualMsgLen; // This will not be refered from anywhere
	SQLRETURN Ret = 0;

	// SQLExecDirect
	SQLWCHAR* CvtQuery = (SQLWCHAR*)StkPlCreateUtf16FromWideChar(Query);
	Ret = SQLExecDirect(pImpl->Hstmt, CvtQuery, SQL_NTS);
	delete [] CvtQuery;

	if (Ret != SQL_SUCCESS) {
		SQLGetDiagRecW(SQL_HANDLE_STMT, pImpl->Hstmt, 1, CvtStateMsg, &Native, CvtMsg, 1024, &ActualMsgLen);
		ConvertMessage(StateMsg, Msg, (char16_t*)CvtStateMsg, (char16_t*)CvtMsg);
		return -1;
	}
	SQLWCHAR TableName[TABLENAME_LENGTH];
	SQLBindCol(pImpl->Hstmt, 1, SQL_C_WCHAR, TableName, TABLENAME_LENGTH * sizeof(SQLWCHAR), NULL);

	bool InitFlag = true;
	int Loop = 0;
	for (;;) {
		Ret = SQLFetch(pImpl->Hstmt);
		if (Ret == SQL_NO_DATA_FOUND) break;
		if (Ret != SQL_SUCCESS && Ret != SQL_SUCCESS_WITH_INFO) {
			SQLGetDiagRecW(SQL_HANDLE_STMT, pImpl->Hstmt, 1, CvtStateMsg, &Native, CvtMsg, 1024, &ActualMsgLen);
			ConvertMessage(StateMsg, Msg, (char16_t*)CvtStateMsg, (char16_t*)CvtMsg);
			return -1;
		}
		StkObject* TblInfObj = new StkObject(L"TableInfo");
		wchar_t* CvtTableName = StkPlCreateWideCharFromUtf16((char16_t*)TableName);
		TblInfObj->AppendChildElement(new StkObject(L"Name", (wchar_t*)CvtTableName));
		delete [] CvtTableName;
		Obj->AppendChildElement(TblInfObj);
		Loop++;
		if (Loop >= MAXNUM_TABLES) {
			break;
		}
	}

	return 0;
}

int DbAccessor::GetNumOfRecordsCommon(wchar_t* TableName,
	bool FilterSwitch,
	wchar_t ColumnNameCnv[5][COLUMNNAME_LENGTH * 4 + 2],
	int OpeType[5],
	wchar_t Value[5][COLUMNVAL_LENGTH * 4 + 2],
	wchar_t StateMsg[10],
	wchar_t Msg[1024])
{
	SQLWCHAR CvtStateMsg[10];
	SQLWCHAR CvtMsg[1024];
	SQLINTEGER Native; // This will not be refered from anywhere
	SQLSMALLINT ActualMsgLen; // This will not be refered from anywhere
	SQLRETURN Ret = 0;

	Ret = OpenDatabase(StateMsg, Msg);
	if (Ret != 0) {
		return Ret;
	}

	wchar_t SqlBuf[1024];
	StkPlSwPrintf(SqlBuf, 1024, L"select count(*) from %ls", TableName);
	bool FirstCond = true;
	if (FilterSwitch) {
		for (int Loop = 1; Loop <= 5; Loop++) {
			if (StkPlWcsCmp(ColumnNameCnv[Loop - 1], L"\"*\"") == 0 || StkPlWcsCmp(ColumnNameCnv[Loop - 1], L"`*`") == 0 || OpeType[Loop - 1] == 0) {
				continue;
			}
			if (FirstCond == true) {
				StkPlWcsCat(SqlBuf, 1024, L" where ");
				FirstCond = false;
			} else {
				StkPlWcsCat(SqlBuf, 1024, L" and ");
			}
			StkPlWcsCat(SqlBuf, 1024, ColumnNameCnv[Loop - 1]);
			switch (OpeType[Loop - 1]) {
			case 1:  StkPlWcsCat(SqlBuf, 1024, L" = "); break;
			case 2:  StkPlWcsCat(SqlBuf, 1024, L" <> "); break;
			case 3:  StkPlWcsCat(SqlBuf, 1024, L" <= "); break;
			case 4:  StkPlWcsCat(SqlBuf, 1024, L" < "); break;
			case 5:  StkPlWcsCat(SqlBuf, 1024, L" >= "); break;
			case 6:  StkPlWcsCat(SqlBuf, 1024, L" > "); break;
			case 10: StkPlWcsCat(SqlBuf, 1024, L" like "); break;
			case 11: StkPlWcsCat(SqlBuf, 1024, L" not like "); break;
			case 20: StkPlWcsCat(SqlBuf, 1024, L" is null "); break;
			case 21: StkPlWcsCat(SqlBuf, 1024, L" is not null "); break;
			}
			if (OpeType[Loop - 1] != 20 && OpeType[Loop - 1] != 21) {
				StkPlWcsCat(SqlBuf, 1024, L"'");
				if (OpeType[Loop - 1] == 10 || OpeType[Loop - 1] == 11) {
					StkPlWcsCat(SqlBuf, 1024, L"%");
				}
				StkPlWcsCat(SqlBuf, 1024, Value[Loop - 1]);
				if (OpeType[Loop - 1] == 10 || OpeType[Loop - 1] == 11) {
					StkPlWcsCat(SqlBuf, 1024, L"%");
				}
				StkPlWcsCat(SqlBuf, 1024, L"'");
			}
		}
	}
	StkPlWcsCat(SqlBuf, 1024, L";");
	char16_t* CvtSqlBuf = StkPlCreateUtf16FromWideChar(SqlBuf);
	Ret = SQLExecDirect(pImpl->Hstmt, (SQLWCHAR*)CvtSqlBuf, SQL_NTS);
	delete [] CvtSqlBuf;
	if (Ret != SQL_SUCCESS) {
		SQLGetDiagRecW(SQL_HANDLE_STMT, pImpl->Hstmt, 1, CvtStateMsg, &Native, CvtMsg, 1024, &ActualMsgLen);
		ConvertMessage(StateMsg, Msg, (char16_t*)CvtStateMsg, (char16_t*)CvtMsg);
		return 0;
	}
	DWORD TmpNumOfRec = 0;
	SQLBindCol(pImpl->Hstmt, 1, SQL_C_SLONG, &TmpNumOfRec, 0, NULL);
	Ret = SQLFetch(pImpl->Hstmt);

	Ret = CloseDatabase(StateMsg, Msg);
	if (Ret != 0) {
		return Ret;
	}
	return (int)TmpNumOfRec;
}

int DbAccessor::GetRecordsByTableNameCommon(const wchar_t* TableName,
	int NumOfCols, StkObject* Obj, bool FilterSwitch,
	wchar_t ColumnNameCnv[5][COLUMNNAME_LENGTH * 4 + 2], int OpeType[5], const wchar_t Value[5][COLUMNVAL_LENGTH * 4 + 2],
	const wchar_t SortTarget[COLUMNNAME_LENGTH * 4 + 2],
	const wchar_t SortOrder[5],
	int Limit, int Offset,
	wchar_t StateMsg[10], wchar_t Msg[1024])
{
	SQLWCHAR CvtStateMsg[10];
	SQLWCHAR CvtMsg[1024];
	SQLINTEGER Native; // This will not be refered from anywhere
	SQLSMALLINT ActualMsgLen; // This will not be refered from anywhere
	SQLRETURN Ret = 0;

	wchar_t SqlBuf[1024];
	StkPlLStrCpy(SqlBuf, L"");

	StkPlSwPrintf(SqlBuf, 1024, L"select * from %ls", TableName);
	bool FirstCond = true;
	if (FilterSwitch) {
		for (int Loop = 1; Loop <= 5; Loop++) {
			if (StkPlWcsCmp(ColumnNameCnv[Loop - 1], L"\"*\"") == 0 || StkPlWcsCmp(ColumnNameCnv[Loop - 1], L"`*`") == 0 || OpeType[Loop - 1] == 0) {
				continue;
			}
			if (FirstCond == true) {
				StkPlWcsCat(SqlBuf, 1024, L" where ");
				FirstCond = false;
			} else {
				StkPlWcsCat(SqlBuf, 1024, L" and ");
			}
			StkPlWcsCat(SqlBuf, 1024, ColumnNameCnv[Loop - 1]);
			switch (OpeType[Loop - 1]) {
			case 1:  StkPlWcsCat(SqlBuf, 1024, L" = "); break;
			case 2:  StkPlWcsCat(SqlBuf, 1024, L" <> "); break;
			case 3:  StkPlWcsCat(SqlBuf, 1024, L" <= "); break;
			case 4:  StkPlWcsCat(SqlBuf, 1024, L" < "); break;
			case 5:  StkPlWcsCat(SqlBuf, 1024, L" >= "); break;
			case 6:  StkPlWcsCat(SqlBuf, 1024, L" > "); break;
			case 10: StkPlWcsCat(SqlBuf, 1024, L" like "); break;
			case 11: StkPlWcsCat(SqlBuf, 1024, L" not like "); break;
			case 20: StkPlWcsCat(SqlBuf, 1024, L" is null "); break;
			case 21: StkPlWcsCat(SqlBuf, 1024, L" is not null "); break;
			}
			if (OpeType[Loop - 1] != 20 && OpeType[Loop - 1] != 21) {
				StkPlWcsCat(SqlBuf, 1024, L"'");
				if (OpeType[Loop - 1] == 10 || OpeType[Loop - 1] == 11) {
					StkPlWcsCat(SqlBuf, 1024, L"%");
				}
				StkPlWcsCat(SqlBuf, 1024, Value[Loop - 1]);
				if (OpeType[Loop - 1] == 10 || OpeType[Loop - 1] == 11) {
					StkPlWcsCat(SqlBuf, 1024, L"%");
				}
				StkPlWcsCat(SqlBuf, 1024, L"'");
			}
		}
	}
	if (SortTarget != NULL && *SortTarget != L'\0') {
		wchar_t SqlSortBuf[128];
		StkPlLStrCpy(SqlSortBuf, L"");
		StkPlSwPrintf(SqlSortBuf, 128, L" order by %ls %ls", SortTarget, SortOrder);
		StkPlWcsCat(SqlBuf, 1024, SqlSortBuf);
	}
	if (Limit != -1 && Offset != -1) {
		wchar_t SqlLimitBuf[128];
		StkPlLStrCpy(SqlLimitBuf, L"");
		StkPlSwPrintf(SqlLimitBuf, 128, L" limit %d offset %d", Limit, Offset);
		StkPlWcsCat(SqlBuf, 1024, SqlLimitBuf);
	}
	StkPlWcsCat(SqlBuf, 1024, L";");
	char16_t* CvtSqlBuf = StkPlCreateUtf16FromWideChar(SqlBuf);
	Ret = SQLExecDirect(pImpl->Hstmt, (SQLWCHAR*)CvtSqlBuf, SQL_NTS);
	delete [] CvtSqlBuf;
	if (Ret != SQL_SUCCESS) {
		SQLGetDiagRecW(SQL_HANDLE_STMT, pImpl->Hstmt, 1, CvtStateMsg, &Native, CvtMsg, 1024, &ActualMsgLen);
		ConvertMessage(StateMsg, Msg, (char16_t*)CvtStateMsg, (char16_t*)CvtMsg);
		return 0;
	}

	SQLWCHAR TmpRecord[MAXNUM_COLUMNS][COLUMNVAL_LENGTH + 10]; // Why +10 ... MariaDB-Fetch returns data exceeding buffer size
	SQLLEN ValLen[MAXNUM_COLUMNS];
	for (int LoopCol = 0; LoopCol < NumOfCols; LoopCol++) {
		SQLBindCol(pImpl->Hstmt, LoopCol + 1, SQL_C_WCHAR, TmpRecord[LoopCol], (COLUMNVAL_LENGTH - 1) * sizeof(SQLWCHAR), &ValLen[LoopCol]);
	}
	int LoopRec = 0;
	for (;;) {
		Ret = SQLFetch(pImpl->Hstmt);
		if (Ret == SQL_NO_DATA_FOUND) break;
		if (Ret != SQL_SUCCESS && Ret != SQL_SUCCESS_WITH_INFO) {
			SQLGetDiagRecW(SQL_HANDLE_STMT, pImpl->Hstmt, 1, CvtStateMsg, &Native, CvtMsg, 1024, &ActualMsgLen);
			ConvertMessage(StateMsg, Msg, (char16_t*)CvtStateMsg, (char16_t*)CvtMsg);
			return 0;
		}
		StkObject* RecObj = new StkObject(L"Record");
		for (int LoopCol = 0; LoopCol < NumOfCols; LoopCol++) {
			wchar_t IndStr[5];
			StkPlSwPrintf(IndStr, 5, L"%d", LoopCol);

			if (ValLen[LoopCol] == -1) {
				RecObj->AppendChildElement(new StkObject(IndStr, L""));
			} else {
				TmpRecord[LoopCol][COLUMNVAL_LENGTH - 1] = '\0';
				wchar_t* TmpRecordCnv = StkPlCreateWideCharFromUtf16((char16_t*)TmpRecord[LoopCol]);
				RecObj->AppendChildElement(new StkObject(IndStr, TmpRecordCnv));
				delete [] TmpRecordCnv;
			}
		}
		Obj->AppendChildElement(RecObj);
		LoopRec++;
	}
	return LoopRec;
}

// TableInfo [in] : Object represents { "zzz" : { "ColumnInfo" : [ { "Name" : "xxx", "Type" : "yyy" }, ... ] } }
// StateMsg [out] : State code
// Msg [out] : Error message
// Return : 0=Success, -1=Error
int DbAccessor::AddTableCommon(StkObject* TableInfo, wchar_t StateMsg[10], wchar_t Msg[1024])
{
	wchar_t SqlBuf[1024] = L"";
	wchar_t TableName[TABLENAME_LENGTH] = L"";
	if (TableInfo) {
		StkPlWcsCpy(TableName, TABLENAME_LENGTH, TableInfo->GetName());
		StkPlSwPrintf(SqlBuf, 1024, L"CREATE TABLE %ls (", TableName);
		StkObject* ColumnInfo = TableInfo->GetFirstChildElement();
		while (ColumnInfo && StkPlWcsCmp(ColumnInfo->GetName(), L"ColumnInfo") == 0) {
			StkObject* Attr = ColumnInfo->GetFirstChildElement();
			wchar_t ColumnName[COLUMNNAME_LENGTH] = L"";
			wchar_t ColumnType[COLUMNTYPE_LENGTH] = L"";
			while (Attr) {
				if (StkPlWcsCmp(Attr->GetName(), L"Name") == 0) {
					StkPlWcsCpy(ColumnName, COLUMNNAME_LENGTH, Attr->GetStringValue());
				} else if (StkPlWcsCmp(Attr->GetName(), L"Type") == 0) {
					StkPlWcsCpy(ColumnType, COLUMNTYPE_LENGTH, Attr->GetStringValue());
				}
				Attr = Attr->GetNext();
			}
			wchar_t SqlBufTmp[COLUMNNAME_LENGTH + COLUMNTYPE_LENGTH + 2] = L"";
			StkPlSwPrintf(SqlBufTmp, COLUMNNAME_LENGTH + COLUMNTYPE_LENGTH + 2, L"%ls %ls", ColumnName, ColumnType);
			StkPlWcsCat(SqlBuf, 1024, SqlBufTmp);
			ColumnInfo = ColumnInfo->GetNext();
			if (ColumnInfo) {
				StkPlWcsCat(SqlBuf, 1024, L",");
			} else {
				StkPlWcsCat(SqlBuf, 1024, L");");
			}
		}
	}
	return 0;
}

// Return 0: Success, -1:Error
int DbAccessor::OpenDatabase(wchar_t StateMsg[10], wchar_t Msg[1024])
{
	SQLWCHAR CvtConnectStr[256];
	StkPlConvWideCharToUtf16((char16_t*)CvtConnectStr, 256, pImpl->ConnectionString);

	SQLWCHAR CvtStateMsg[10];
	SQLWCHAR CvtMsg[1024];
	SQLINTEGER Native; // This will not be refered from anywhere
	SQLSMALLINT ActualMsgLen; // This will not be refered from anywhere
	StkPlLStrCpy(Msg, L"");
	StkPlLStrCpy(StateMsg, L"");

	// Alloc environment handle
	if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &(pImpl->Henv)) == SQL_ERROR) {
		if (pImpl->Henv != SQL_NULL_HENV) {
			SQLGetDiagRecW(SQL_HANDLE_ENV, pImpl->Henv, 1, CvtStateMsg, &Native, CvtMsg, 1024, &ActualMsgLen);
		}
		ConvertMessage(StateMsg, Msg, (char16_t*)CvtStateMsg, (char16_t*)CvtMsg);
		return -1;
	}
	SQLSetEnvAttr(pImpl->Henv, SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), 0);

	// Alloc DB connection handle
	if (SQLAllocHandle(SQL_HANDLE_DBC, pImpl->Henv, &(pImpl->Hdbc)) == SQL_ERROR) {
		SQLGetDiagRecW(SQL_HANDLE_ENV, pImpl->Henv, 1, CvtStateMsg, &Native, CvtMsg, 1024, &ActualMsgLen);
		ConvertMessage(StateMsg, Msg, (char16_t*)CvtStateMsg, (char16_t*)CvtMsg);
		return -1;
	}

	// SQLDriverConnect
	SQLWCHAR ConnOut[255]; // This will not be refered from anywhere
	SQLSMALLINT ConnOutLen; // This will not be refered from anywhere
	SQLRETURN Ret = SQLDriverConnectW(pImpl->Hdbc, NULL, CvtConnectStr, SQL_NTS, ConnOut, 255, &ConnOutLen, SQL_DRIVER_COMPLETE);
	if (Ret == SQL_ERROR || Ret == SQL_SUCCESS_WITH_INFO) {
		SQLGetDiagRecW(SQL_HANDLE_DBC, pImpl->Hdbc, 1, CvtStateMsg, &Native, CvtMsg, 1024, &ActualMsgLen);
		ConvertMessage(StateMsg, Msg, (char16_t*)CvtStateMsg, (char16_t*)CvtMsg);
		return -1;
	}

	// Alloc statement handle 
	if (SQLAllocHandle(SQL_HANDLE_STMT, pImpl->Hdbc, &(pImpl->Hstmt)) == SQL_ERROR) {
		SQLGetDiagRecW(SQL_HANDLE_DBC, pImpl->Hdbc, 1, CvtStateMsg, &Native, CvtMsg, 1024, &ActualMsgLen);
		ConvertMessage(StateMsg, Msg, (char16_t*)CvtStateMsg, (char16_t*)CvtMsg);
		return -1;
	}

	return 0;
}

// Return 0: Success, -1:Error
int DbAccessor::CloseDatabase(wchar_t StateMsg[10], wchar_t Msg[1024])
{
	SQLWCHAR CvtStateMsg[10];
	SQLWCHAR CvtMsg[1024];
	SQLINTEGER Native; // This will not be refered from anywhere
	SQLSMALLINT ActualMsgLen; // This will not be refered from anywhere
	StkPlLStrCpy((wchar_t*)Msg, L"");
	StkPlLStrCpy((wchar_t*)StateMsg, L"");

	// Free statement handle
	if (SQLFreeHandle(SQL_HANDLE_STMT, pImpl->Hstmt) == SQL_ERROR) {
		SQLGetDiagRecW(SQL_HANDLE_DBC, pImpl->Hdbc, 1, CvtStateMsg, &Native, CvtMsg, 1024, &ActualMsgLen);
		ConvertMessage(StateMsg, Msg, (char16_t*)CvtStateMsg, (char16_t*)CvtMsg);
		return -1;
	}
	pImpl->Hstmt = 0;

	// SQLDisconnect
	SQLRETURN Ret = SQLDisconnect(pImpl->Hdbc);
	if (Ret == SQL_ERROR || Ret == SQL_SUCCESS_WITH_INFO) {
		SQLGetDiagRecW(SQL_HANDLE_DBC, pImpl->Hdbc, 1, CvtStateMsg, &Native, CvtMsg, 1024, &ActualMsgLen);
		ConvertMessage(StateMsg, Msg, (char16_t*)CvtStateMsg, (char16_t*)CvtMsg);
		return -1;
	}

	// Free DB connection handle
	if (SQLFreeHandle(SQL_HANDLE_DBC, pImpl->Hdbc) == SQL_ERROR) {
		SQLGetDiagRecW(SQL_HANDLE_ENV, pImpl->Henv, 1, CvtStateMsg, &Native, CvtMsg, 1024, &ActualMsgLen);
		ConvertMessage(StateMsg, Msg, (char16_t*)CvtStateMsg, (char16_t*)CvtMsg);
		return -1;
	}
	pImpl->Hdbc = 0;

	// Free environment handle
	if (SQLFreeHandle(SQL_HANDLE_ENV, pImpl->Henv) == SQL_ERROR) {
		SQLGetDiagRecW(SQL_HANDLE_ENV, pImpl->Henv, 1, CvtStateMsg, &Native, CvtMsg, 1024, &ActualMsgLen);
		ConvertMessage(StateMsg, Msg, (char16_t*)CvtStateMsg, (char16_t*)CvtMsg);
		return -1;
	}
	pImpl->Henv = 0;

	return 0;
}
