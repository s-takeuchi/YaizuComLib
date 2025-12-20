#ifdef WIN32
	#include <windows.h>
#endif
#include <sql.h>
#include <sqlext.h>

#include "../../../YaizuComLib/src/stkpl/StkPl.h"
#include "DbAccessor.h"
#include "DbPostgreSqlAccessor.h"
#include "FilteringCondition.h"

class DbAccessor::Impl
{
public:
	wchar_t* ConnectionString;

	SQLHENV  Henv;
	SQLHDBC  Hdbc;
	SQLHSTMT Hstmt;
};

DbPostgreSqlAccessor::DbPostgreSqlAccessor(wchar_t* TmpConnStr) : DbAccessor(TmpConnStr)
{
}

DbPostgreSqlAccessor::~DbPostgreSqlAccessor()
{
}

int DbPostgreSqlAccessor::GetNumOfRecords(wchar_t* TableName, FilteringCondition* FilCond, wchar_t StateMsg[10], wchar_t Msg[1024])
{
	size_t LenOfTableName = StkPlWcsLen((wchar_t*)TableName);
	wchar_t* EcdTableName = new wchar_t[LenOfTableName * 4 + 2];
	SqlEncoding(TableName, EcdTableName, TYPE_KEY);

	wchar_t ColumnNameCnv[5][COLUMNNAME_LENGTH * 4 + 2];
	int OpeType[5];
	wchar_t ValueCnv[5][COLUMNVAL_LENGTH * 4 + 2];
	bool FilterSwitch = false;
	if (FilCond != NULL) {
		FilterSwitch = true;
	}
	FilteringCondition* CurFilCond = FilCond;
	int Loop = 0;
	while (CurFilCond) {
		SqlEncoding(CurFilCond->GetColumnName(), ColumnNameCnv[Loop], TYPE_KEY);
		OpeType[Loop] = CurFilCond->GetOpeType();
		if (CurFilCond->GetOpeType() == 10 || CurFilCond->GetOpeType() == 11) {
			SqlEncoding(CurFilCond->GetColumnVal(), ValueCnv[Loop], TYPE_LIKE_VALUE);
		} else {
			SqlEncoding(CurFilCond->GetColumnVal(), ValueCnv[Loop], TYPE_VALUE);
		}
		CurFilCond = CurFilCond->GetNext();
		Loop++;
	}

	int Ret = GetNumOfRecordsCommon(EcdTableName, FilterSwitch, ColumnNameCnv, OpeType, ValueCnv, StateMsg, Msg);
	delete[] EcdTableName;
	return Ret;
}

int DbPostgreSqlAccessor::GetTables(StkObject* Obj, wchar_t StateMsg[10], wchar_t Msg[1024])
{
	SQLRETURN Ret = 0;

	Ret = OpenDatabase(StateMsg, Msg);
	if (Ret != 0) {
		return Ret;
	}
	Ret = GetTablesCommon(L"select relname as TABLE_NAME from pg_stat_user_tables;", Obj, StateMsg, Msg);
	Ret = CloseDatabase(StateMsg, Msg);

	return Ret;
}

int DbPostgreSqlAccessor::CreateTable(StkObject* Obj, wchar_t StateMsg[10], wchar_t Msg[1024])
{
	return CreateTableCommon(Obj, StateMsg, Msg);
}

int DbPostgreSqlAccessor::DropTable(wchar_t* TableName, wchar_t StateMsg[10], wchar_t Msg[1024])
{
	return DropTableCommon(TableName, StateMsg, Msg);
}

int DbPostgreSqlAccessor::InsertRecord(StkObject* Record, wchar_t StateMsg[10], wchar_t Msg[1024])
{
	return InsertRecordCommon(Record, StateMsg, Msg);
}

int DbPostgreSqlAccessor::GetColumnInfoByTableName(wchar_t* TableName, StkObject* TblObj, wchar_t StateMsg[10], wchar_t Msg[1024])
{
	SQLWCHAR CvtStateMsg[10];
	SQLWCHAR CvtMsg[1024];

	SQLINTEGER Native; // This will not be refered from anywhere
	SQLSMALLINT ActualMsgLen; // This will not be refered from anywhere
	SQLRETURN Ret = 0;

	Ret = OpenDatabase(StateMsg, Msg);
	if (Ret != 0) {
		return 0;
	}

	size_t LenOfTableName = StkPlWcsLen(TableName);
	wchar_t* EcdTableName = new wchar_t[LenOfTableName * 4 + 2];
	SqlEncoding(TableName, EcdTableName, TYPE_VALUE);

	wchar_t SqlBuf[1024];
	StkPlSwPrintf(SqlBuf, 1024, L"SELECT * FROM information_schema.columns WHERE table_schema='public' and table_name='%ls';", EcdTableName);
	char16_t* CvtSqlBuf = StkPlCreateUtf16FromWideChar(SqlBuf);
	Ret = SQLExecDirect(pImpl->Hstmt, (SQLWCHAR*)CvtSqlBuf, SQL_NTS);
	delete [] CvtSqlBuf;
	delete [] EcdTableName;
	if (Ret != SQL_SUCCESS) {
		SQLGetDiagRec(SQL_HANDLE_STMT, pImpl->Hstmt, 1, CvtStateMsg, &Native, CvtMsg, 1024, &ActualMsgLen);
		ConvertMessage(StateMsg, Msg, (char16_t*)CvtStateMsg, (char16_t*)CvtMsg);
		wchar_t DummyStateMsg[10];
		wchar_t DummyMsg[1024];
		CloseDatabase(DummyStateMsg, DummyMsg);
		return 0;
	}
	SQLWCHAR TmpColumnName[COLUMNNAME_LENGTH] = { 0 };
	SQLWCHAR ColumnType[COLUMNTYPE_LENGTH] = { 0 };
	SQLWCHAR TmpIsNull[10] = { 0 };
	int TmpColumnMaxLen = -1;
	SQLLEN ColumneNameLen = -1, ColumneTypeLen = -1, ColumneMaxLen = -1, IsNullLen = -1;
	SQLBindCol(pImpl->Hstmt, 4, SQL_C_WCHAR, TmpColumnName, COLUMNNAME_LENGTH * sizeof(SQLWCHAR), &ColumneNameLen);
	SQLBindCol(pImpl->Hstmt, 7, SQL_C_WCHAR, TmpIsNull, 10 * sizeof(SQLWCHAR), &IsNullLen);
	SQLBindCol(pImpl->Hstmt, 8, SQL_C_WCHAR, ColumnType, COLUMNTYPE_LENGTH * sizeof(SQLWCHAR), &ColumneTypeLen);
	SQLBindCol(pImpl->Hstmt, 9, SQL_C_SLONG, &TmpColumnMaxLen, 0, &ColumneMaxLen);

	int Loop = 0;
	for (; Loop < MAXNUM_COLUMNS; Loop++) {
		Ret = SQLFetch(pImpl->Hstmt);
		if (Ret == SQL_NO_DATA_FOUND) break;
		if (Ret != SQL_SUCCESS && Ret != SQL_SUCCESS_WITH_INFO && ColumneMaxLen != SQL_NULL_DATA) {
			SQLGetDiagRec(SQL_HANDLE_STMT, pImpl->Hstmt, 1, CvtStateMsg, &Native, CvtMsg, 1024, &ActualMsgLen);
			ConvertMessage(StateMsg, Msg, (char16_t*)CvtStateMsg, (char16_t*)CvtMsg);
			wchar_t DummyStateMsg[10];
			wchar_t DummyMsg[1024];
			CloseDatabase(DummyStateMsg, DummyMsg);
			return 0;
		}

		wchar_t* TmpColumnNameCnv = StkPlCreateWideCharFromUtf16((char16_t*)TmpColumnName);
		wchar_t* TmpIsNullCnv = StkPlCreateWideCharFromUtf16((char16_t*)TmpIsNull);
		wchar_t* ColumnTypeCnv = StkPlCreateWideCharFromUtf16((char16_t*)ColumnType);

		wchar_t TmpColumnType[COLUMNTYPE_LENGTH];
		if (ColumneMaxLen != SQL_NULL_DATA) {
			StkPlSwPrintf(TmpColumnType, COLUMNTYPE_LENGTH, L"%ls(%d)", ColumnTypeCnv, TmpColumnMaxLen);
		} else {
			StkPlLStrCpy(TmpColumnType, ColumnTypeCnv);
		}
		wchar_t ColTypeCnv[COLUMNTYPE_LENGTH];
		ConvertAttrType(TmpColumnType, ColTypeCnv);
		StkObject* ClmObj = new StkObject(L"ColumnInfo");
		ClmObj->AppendChildElement(new StkObject(L"title", TmpColumnNameCnv));
		ClmObj->AppendChildElement(new StkObject(L"width", 100));
		ClmObj->AppendChildElement(new StkObject(L"dataType", ColTypeCnv));
		ClmObj->AppendChildElement(new StkObject(L"dataIndx", Loop));
		ClmObj->AppendChildElement(new StkObject(L"coltype", TmpColumnType));
		ClmObj->AppendChildElement(new StkObject(L"isnull", TmpIsNullCnv));
		TblObj->AppendChildElement(ClmObj);

		delete [] TmpColumnNameCnv;
		delete [] TmpIsNullCnv;
		delete [] ColumnTypeCnv;
	}
	Ret = CloseDatabase(StateMsg, Msg);

	return Loop;
}

int DbPostgreSqlAccessor::GetRecordsByTableName(wchar_t* TableName, FilteringCondition* FilCond, int NumOfCols, StkObject* DatObj, wchar_t* SortTarget, wchar_t* SortOrder, int Limit, int Offset, wchar_t StateMsg[10], wchar_t Msg[1024])
{
	SQLRETURN Ret = 0;

	size_t LenOfTableName = StkPlWcsLen(TableName);
	wchar_t* EcdTableName = new wchar_t[LenOfTableName * 4 + 2];
	SqlEncoding(TableName, EcdTableName, TYPE_KEY);

	wchar_t* EcdSortTarget = NULL;
	if (SortTarget != NULL && *SortTarget != L'\0') {
		size_t LenOfSortTarget = StkPlWcsLen(SortTarget);
		EcdSortTarget = new  wchar_t[LenOfSortTarget * 4 + 2];
		SqlEncoding(SortTarget, EcdSortTarget, TYPE_KEY);
	}

	Ret = OpenDatabase(StateMsg, Msg);

	wchar_t ColumnNameCnv[5][COLUMNNAME_LENGTH * 4 + 2];
	int OpeType[5];
	wchar_t ValueCnv[5][COLUMNVAL_LENGTH * 4 + 2];
	bool FilterSwitch = false;
	if (FilCond != NULL) {
		FilterSwitch = true;
	}
	FilteringCondition* CurFilCond = FilCond;
	int Loop = 0;
	while (CurFilCond) {
		SqlEncoding(CurFilCond->GetColumnName(), ColumnNameCnv[Loop], TYPE_KEY);
		OpeType[Loop] = CurFilCond->GetOpeType();
		if (CurFilCond->GetOpeType() == 10 || CurFilCond->GetOpeType() == 11) {
			SqlEncoding(CurFilCond->GetColumnVal(), ValueCnv[Loop], TYPE_LIKE_VALUE);
		} else {
			SqlEncoding(CurFilCond->GetColumnVal(), ValueCnv[Loop], TYPE_VALUE);
		}
		CurFilCond = CurFilCond->GetNext();
		Loop++;
	}

	int NumOfRecs = GetRecordsByTableNameCommon(EcdTableName, NumOfCols, DatObj, FilterSwitch, ColumnNameCnv, OpeType, ValueCnv, EcdSortTarget, SortOrder, Limit, Offset, StateMsg, Msg);

	delete[] EcdTableName;
	if (EcdSortTarget != NULL) {
		delete[] EcdSortTarget;
	}
	Ret = CloseDatabase(StateMsg, Msg);
	return NumOfRecs;
}

int DbPostgreSqlAccessor::ConvertAttrType(wchar_t InAttr[COLUMNTYPE_LENGTH], wchar_t OutAttr[COLUMNTYPE_LENGTH])
{
	if (StkPlWcsStr((wchar_t*)InAttr, L"bigint") != NULL ||
		StkPlWcsStr((wchar_t*)InAttr, L"integer") != NULL ||
		StkPlWcsStr((wchar_t*)InAttr, L"smallint") != NULL ||
		StkPlWcsStr((wchar_t*)InAttr, L"bigserial") != NULL ||
		StkPlWcsStr((wchar_t*)InAttr, L"serial") != NULL ||
		StkPlWcsStr((wchar_t*)InAttr, L"smallserial") != NULL) {
		StkPlLStrCpy((wchar_t*)OutAttr, L"integer");
		return 0;
	}
	if (StkPlWcsStr((wchar_t*)InAttr, L"real") != NULL ||
		StkPlWcsStr((wchar_t*)InAttr, L"double precision") != NULL) {
		StkPlLStrCpy((wchar_t*)OutAttr, L"float");
		return 0;
	}
	StkPlLStrCpy((wchar_t*)OutAttr, L"string");
	return 0;
}

void DbPostgreSqlAccessor::SqlEncoding(wchar_t* InSql, wchar_t* OutSql, int Type)
{
	size_t LenOfInSql = StkPlWcsLen(InSql);
	int OutSqlIndex = 0;
	if (Type == TYPE_KEY) {
		OutSql[0] = L'\"';
		OutSqlIndex++;
	}
	for (int Loop = 0; Loop < LenOfInSql; Loop++) {
		if (InSql[Loop] == L'\"' && Type == TYPE_KEY) {
			StkPlLStrCpy(&OutSql[OutSqlIndex], L"\"\"");
			OutSqlIndex += 2;
			continue;
		}
		if (InSql[Loop] == L'\'' && (Type == TYPE_VALUE || Type == TYPE_LIKE_VALUE)) {
			StkPlLStrCpy(&OutSql[OutSqlIndex], L"\'\'");
			OutSqlIndex += 2;
			continue;
		}
		if (InSql[Loop] == L'\\' && Type == TYPE_LIKE_VALUE) {
			StkPlLStrCpy(&OutSql[OutSqlIndex], L"\\\\");
			OutSqlIndex += 2;
			continue;
		}
		if (InSql[Loop] == L'%' && Type == TYPE_LIKE_VALUE) {
			StkPlLStrCpy(&OutSql[OutSqlIndex], L"\\%");
			OutSqlIndex += 2;
			continue;
		}
		if (InSql[Loop] == L'_' && Type == TYPE_LIKE_VALUE) {
			StkPlLStrCpy(&OutSql[OutSqlIndex], L"\\_");
			OutSqlIndex += 2;
			continue;
		}
		OutSql[OutSqlIndex] = InSql[Loop];
		OutSqlIndex++;
	}
	if (Type == TYPE_KEY) {
		OutSql[OutSqlIndex] = L'\"';
		OutSqlIndex++;
	}
	OutSql[OutSqlIndex] = L'\0';
}
