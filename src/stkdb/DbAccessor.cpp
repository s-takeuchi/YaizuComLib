#ifdef WIN32
    #include <windows.h>
#endif
#include <sql.h>
#include <sqlext.h>

#include "DbAccessor.h"
#include "../../../YaizuComLib/src/commonfunc/StkObject.h"
#include "../../../YaizuComLib/src/stkpl/StkPl.h"

static SQLHENV Henv = 0;

class DbAccessor::Impl
{
public:
	wchar_t* ConnectionString;

	SQLHDBC  Hdbc;
	SQLHSTMT Hstmt;
};

DbAccessor::DbAccessor(wchar_t* TmpConnStr)
{
	pImpl = new Impl;
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

int DbAccessor::Init()
{
	// Alloc environment handle
	if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &(Henv)) == SQL_ERROR) {
		return -1;
	}
	SQLSetEnvAttr(Henv, SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), 0);
	return 0;
}

int DbAccessor::Uninit()
{
	// Free environment handle
	if (SQLFreeHandle(SQL_HANDLE_ENV, Henv) == SQL_ERROR) {
		return -1;
	}
	Henv = 0;
	return 0;
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
	SQLWCHAR TableName[TABLENAME_LENGTH] = { 0 };
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
		wchar_t DummyStateMsg[10];
		wchar_t DummyMsg[1024];
		CloseDatabase(DummyStateMsg, DummyMsg);
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

// TableInfo [in] : Object represents  {"zzz" : { "ColumnInfo" : [ { "Name" : "xxx", "Type" : "yyy" }, ... ]  }}
// StateMsg [out] : State code
// Msg [out] : Error message
// Return : 0=Success, -1=Error
int DbAccessor::CreateTableCommon(StkObject* TableInfo, wchar_t StateMsg[10], wchar_t Msg[1024])
{
	wchar_t SqlBuf[2048] = L"";
	wchar_t TableName[TABLENAME_LENGTH] = L"";
	if (TableInfo) {
		TableInfo = TableInfo->GetFirstChildElement();
	}
	if (TableInfo) {
		StkPlWcsCpy(TableName, TABLENAME_LENGTH, TableInfo->GetName());
		size_t LenOfTableName = StkPlWcsLen(TableName);
		wchar_t* EcdTableName = new wchar_t[LenOfTableName * 4 + 2];
		SqlEncoding(TableName, EcdTableName, TYPE_KEY);

		StkPlSwPrintf(SqlBuf, 2048, L"CREATE TABLE %ls (", EcdTableName);
		delete [] EcdTableName;
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
			size_t LenOfColumnName = StkPlWcsLen(ColumnName);
			wchar_t* EcdColumnName = new wchar_t[LenOfColumnName * 4 + 2];
			SqlEncoding(ColumnName, EcdColumnName, TYPE_KEY);

			wchar_t SqlBufTmp[COLUMNNAME_LENGTH + COLUMNTYPE_LENGTH + 2] = L"";
			StkPlSwPrintf(SqlBufTmp, COLUMNNAME_LENGTH + COLUMNTYPE_LENGTH + 2, L"%ls %ls", EcdColumnName, ColumnType);
			delete[] EcdColumnName;
			StkPlWcsCat(SqlBuf, 2048, SqlBufTmp);
			ColumnInfo = ColumnInfo->GetNext();
			if (ColumnInfo) {
				StkPlWcsCat(SqlBuf, 2048, L",");
			} else {
				StkPlWcsCat(SqlBuf, 2048, L");");
			}
		}

		OpenDatabase(StateMsg, Msg);
		SQLWCHAR CvtStateMsg[10];
		SQLWCHAR CvtMsg[1024];
		SQLINTEGER Native; // This will not be refered from anywhere
		SQLSMALLINT ActualMsgLen; // This will not be refered from anywhere
		char16_t* CvtSqlBuf = StkPlCreateUtf16FromWideChar(SqlBuf);
		SQLRETURN Ret = SQLExecDirect(pImpl->Hstmt, (SQLWCHAR*)CvtSqlBuf, SQL_NTS);
		delete[] CvtSqlBuf;
		if (Ret != SQL_SUCCESS) {
			SQLGetDiagRecW(SQL_HANDLE_STMT, pImpl->Hstmt, 1, CvtStateMsg, &Native, CvtMsg, 1024, &ActualMsgLen);
			ConvertMessage(StateMsg, Msg, (char16_t*)CvtStateMsg, (char16_t*)CvtMsg);
			wchar_t DummyStateMsg[10];
			wchar_t DummyMsg[1024];
			CloseDatabase(DummyStateMsg, DummyMsg);
			return -1;
		}
		CloseDatabase(StateMsg, Msg);
	}
	return 0;
}

int DbAccessor::DropTableCommon(const wchar_t* TableName, wchar_t StateMsg[10], wchar_t Msg[1024])
{
	wchar_t SqlBuf[1024] = L"";

	size_t LenOfTableName = StkPlWcsLen(TableName);
	wchar_t* EcdTableName = new wchar_t[LenOfTableName * 4 + 2];
	SqlEncoding(TableName, EcdTableName, TYPE_KEY);

	StkPlSwPrintf(SqlBuf, 1024, L"DROP TABLE %ls;", EcdTableName);
	delete[] EcdTableName;

	OpenDatabase(StateMsg, Msg);
	SQLWCHAR CvtStateMsg[10];
	SQLWCHAR CvtMsg[1024];
	SQLINTEGER Native; // This will not be refered from anywhere
	SQLSMALLINT ActualMsgLen; // This will not be refered from anywhere
	char16_t* CvtSqlBuf = StkPlCreateUtf16FromWideChar(SqlBuf);
	SQLRETURN Ret = SQLExecDirect(pImpl->Hstmt, (SQLWCHAR*)CvtSqlBuf, SQL_NTS);
	delete[] CvtSqlBuf;
	if (Ret != SQL_SUCCESS) {
		SQLGetDiagRecW(SQL_HANDLE_STMT, pImpl->Hstmt, 1, CvtStateMsg, &Native, CvtMsg, 1024, &ActualMsgLen);
		ConvertMessage(StateMsg, Msg, (char16_t*)CvtStateMsg, (char16_t*)CvtMsg);
		wchar_t DummyStateMsg[10];
		wchar_t DummyMsg[1024];
		CloseDatabase(DummyStateMsg, DummyMsg);
		return -1;
	}
	CloseDatabase(StateMsg, Msg);
	return 0;
}

// TableInfo [in] : Object represents  {"zzz" : [{ "RecordInfo" : [ "Value1", "Value2", ... ] }, ...]} 
// StateMsg [out] : State code
// Msg [out] : Error message
// Return : 0=Success, -1=Error
int DbAccessor::InsertRecordCommon(StkObject* Record, wchar_t StateMsg[10], wchar_t Msg[1024])
{
	static const int BUF_SIZE = 8192;
	wchar_t SqlBuf[BUF_SIZE] = L"";
	wchar_t TableName[TABLENAME_LENGTH] = L"";
	wchar_t Value[64] = L"";
	if (Record) {
		Record = Record->GetFirstChildElement();
	}

	StkPlWcsCpy(TableName, TABLENAME_LENGTH, Record->GetName());
	size_t LenOfTableName = StkPlWcsLen(TableName);
	wchar_t* EcdTableName = new wchar_t[LenOfTableName * 4 + 2];
	SqlEncoding(TableName, EcdTableName, TYPE_KEY);
	StkPlWcsCat(SqlBuf, BUF_SIZE, L"INSERT INTO ");
	StkPlWcsCat(SqlBuf, BUF_SIZE, EcdTableName);
	StkPlWcsCat(SqlBuf, BUF_SIZE, L" VALUES \n");
	delete[] EcdTableName;

	while (Record) {
		StkObject* ColumnData = Record->GetFirstChildElement();
		StkPlWcsCat(SqlBuf, BUF_SIZE, L"(");
		while (ColumnData && StkPlWcsCmp(ColumnData->GetName(), L"RecordInfo") == 0) {
			if (ColumnData->GetType() == StkObject::STKOBJECT_ELEM_STRING) {
				size_t LenOfValue = StkPlWcsLen(ColumnData->GetStringValue());
				wchar_t* EcdValue = new wchar_t[LenOfValue * 4 + 2];
				StkPlWcsCpy(Value, 64, ColumnData->GetStringValue());
				SqlEncoding(Value, EcdValue, TYPE_VALUE);
				StkPlWcsCat(SqlBuf, BUF_SIZE, L"'");
				StkPlWcsCat(SqlBuf, BUF_SIZE, EcdValue);
				StkPlWcsCat(SqlBuf, BUF_SIZE, L"'");
				delete[] EcdValue;
				if (ColumnData->GetNext()) {
					StkPlWcsCat(SqlBuf, BUF_SIZE, L",");
				} else {
					StkPlWcsCat(SqlBuf, BUF_SIZE, L")");
				}
			} else if (ColumnData->GetType() == StkObject::STKOBJECT_ELEM_INT) {
				int ValueInt = ColumnData->GetIntValue();
				if (ColumnData->GetNext()) {
					StkPlSwPrintf(Value, 64, L"%d,", ValueInt);
				} else {
					StkPlSwPrintf(Value, 64, L"%d)", ValueInt);
				}
				StkPlWcsCat(SqlBuf, BUF_SIZE, Value);
			} else if (ColumnData->GetType() == StkObject::STKOBJECT_ELEM_FLOAT) {
				float ValueFloat = ColumnData->GetFloatValue();
				if (ColumnData->GetNext()) {
					StkPlSwPrintf(Value, 64, L"%f,", ValueFloat);
				} else {
					StkPlSwPrintf(Value, 64, L"%f)", ValueFloat);
				}
				StkPlWcsCat(SqlBuf, BUF_SIZE, Value);
			}
			ColumnData = ColumnData->GetNext();
		}
		Record = Record->GetNext();
		if (Record) {
			StkPlWcsCat(SqlBuf, BUF_SIZE, L",\n");
		} else {
			StkPlWcsCat(SqlBuf, BUF_SIZE, L";");
		}
	}

	OpenDatabase(StateMsg, Msg);
	SQLWCHAR CvtStateMsg[10];
	SQLWCHAR CvtMsg[1024];
	SQLINTEGER Native; // This will not be refered from anywhere
	SQLSMALLINT ActualMsgLen; // This will not be refered from anywhere
	char16_t* CvtSqlBuf = StkPlCreateUtf16FromWideChar(SqlBuf);
	SQLRETURN Ret = SQLExecDirect(pImpl->Hstmt, (SQLWCHAR*)CvtSqlBuf, SQL_NTS);
	delete[] CvtSqlBuf;
	if (Ret != SQL_SUCCESS) {
		SQLGetDiagRecW(SQL_HANDLE_STMT, pImpl->Hstmt, 1, CvtStateMsg, &Native, CvtMsg, 1024, &ActualMsgLen);
		ConvertMessage(StateMsg, Msg, (char16_t*)CvtStateMsg, (char16_t*)CvtMsg);
		wchar_t DummyStateMsg[10];
		wchar_t DummyMsg[1024];
		CloseDatabase(DummyStateMsg, DummyMsg);
		return -1;
	}
	CloseDatabase(StateMsg, Msg);
	return 0;
}

int DbAccessor::GetCurrentDatabaseCommon(wchar_t DatabaseName[DBNAME_LENGTH], wchar_t StateMsg[10], wchar_t Msg[1024])
{
	SQLRETURN Ret = OpenDatabase(StateMsg, Msg);
	if (Ret != 0) {
		return Ret;
	}
	wchar_t* SqlBuf = GetCurrentDatabase();

	SQLWCHAR CvtStateMsg[10];
	SQLWCHAR CvtMsg[1024];
	SQLINTEGER Native; // This will not be refered from anywhere
	SQLSMALLINT ActualMsgLen; // This will not be refered from anywhere
	char16_t* CvtSqlBuf = StkPlCreateUtf16FromWideChar(SqlBuf);
	Ret = SQLExecDirect(pImpl->Hstmt, (SQLWCHAR*)CvtSqlBuf, SQL_NTS);
	delete[] CvtSqlBuf;
	delete[] SqlBuf;
	if (Ret != SQL_SUCCESS) {
		SQLGetDiagRecW(SQL_HANDLE_STMT, pImpl->Hstmt, 1, CvtStateMsg, &Native, CvtMsg, 1024, &ActualMsgLen);
		ConvertMessage(StateMsg, Msg, (char16_t*)CvtStateMsg, (char16_t*)CvtMsg);
		wchar_t DummyStateMsg[10];
		wchar_t DummyMsg[1024];
		CloseDatabase(DummyStateMsg, DummyMsg);
		return -1;
	}
	SQLWCHAR DbName[DBNAME_LENGTH] = { 0 };
	SQLBindCol(pImpl->Hstmt, 1, SQL_C_WCHAR, DbName, DBNAME_LENGTH * sizeof(SQLWCHAR), NULL);
	if (SQLFetch(pImpl->Hstmt) != SQL_SUCCESS) {
		SQLGetDiagRecW(SQL_HANDLE_STMT, pImpl->Hstmt, 1, CvtStateMsg, &Native, CvtMsg, 1024, &ActualMsgLen);
		ConvertMessage(StateMsg, Msg, (char16_t*)CvtStateMsg, (char16_t*)CvtMsg);
		wchar_t DummyStateMsg[10];
		wchar_t DummyMsg[1024];
		CloseDatabase(DummyStateMsg, DummyMsg);
		return -1;
	}
	Ret = CloseDatabase(StateMsg, Msg);
	StkPlConvUtf16ToWideChar(DatabaseName, DBNAME_LENGTH, (char16_t*)DbName);
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

	// Alloc DB connection handle
	if (SQLAllocHandle(SQL_HANDLE_DBC, Henv, &(pImpl->Hdbc)) == SQL_ERROR) {
		SQLGetDiagRecW(SQL_HANDLE_ENV, Henv, 1, CvtStateMsg, &Native, CvtMsg, 1024, &ActualMsgLen);
		ConvertMessage(StateMsg, Msg, (char16_t*)CvtStateMsg, (char16_t*)CvtMsg);
		return -1;
	}
	SQLSetConnectAttr(pImpl->Hdbc, SQL_ATTR_CONNECTION_TIMEOUT, (SQLPOINTER)10, 0);
	SQLSetConnectAttr(pImpl->Hdbc, SQL_ATTR_LOGIN_TIMEOUT, (SQLPOINTER)10, 0);

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
	SQLSetStmtAttr(pImpl->Hstmt, SQL_ATTR_QUERY_TIMEOUT, (SQLPOINTER)20, 0);

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
		SQLGetDiagRecW(SQL_HANDLE_ENV, Henv, 1, CvtStateMsg, &Native, CvtMsg, 1024, &ActualMsgLen);
		ConvertMessage(StateMsg, Msg, (char16_t*)CvtStateMsg, (char16_t*)CvtMsg);
		return -1;
	}
	pImpl->Hdbc = 0;

	return 0;
}
