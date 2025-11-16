#ifdef WIN32
	#include <windows.h>
#endif
#include <sql.h>
#include <sqlext.h>

#include "../../../YaizuComLib/src/stkpl/StkPl.h"
#include "DbAccessor.h"
#include "DbMariaDbAccessor.h"
#include "FilteringCondition.h"

class DbAccessor::Impl
{
public:
	wchar_t* ConnectionString;

	SQLHENV  Henv;
	SQLHDBC  Hdbc;
	SQLHSTMT Hstmt;
};

DbMariaDbAccessor::DbMariaDbAccessor(wchar_t* TmpConnStr) : DbAccessor(TmpConnStr)
{
}

DbMariaDbAccessor::~DbMariaDbAccessor()
{
}

int DbMariaDbAccessor::GetNumOfRecords(wchar_t* TableName, FilteringCondition* FilCond, wchar_t StateMsg[10], wchar_t Msg[1024])
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

int DbMariaDbAccessor::GetTables(StkObject* Obj, wchar_t StateMsg[10], wchar_t Msg[1024])
{
	SQLRETURN Ret = 0;

	Ret = OpenDatabase(StateMsg, Msg);
	if (Ret != 0) {
		return Ret;
	}
	Ret = GetTablesCommon(L"show tables;", Obj, StateMsg, Msg);
	Ret = CloseDatabase(StateMsg, Msg);

	return Ret;
}

int DbMariaDbAccessor::AddTable(StkObject* Obj, wchar_t StateMsg[10], wchar_t Msg[1024])
{
	return AddTableCommon(Obj, StateMsg, Msg);
}

int DbMariaDbAccessor::GetColumnInfoByTableName(wchar_t* TableName, StkObject* TblObj, wchar_t StateMsg[10], wchar_t Msg[1024])
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
	SqlEncoding(TableName, EcdTableName, TYPE_KEY);

	wchar_t SqlBuf[1024];
	StkPlSwPrintf(SqlBuf, 1024, L"show full columns from %ls;", (wchar_t*)EcdTableName);
	char16_t* CvtSqlBuf = StkPlCreateUtf16FromWideChar(SqlBuf);
	Ret = SQLExecDirect(pImpl->Hstmt, (SQLWCHAR*)CvtSqlBuf, SQL_NTS);
	delete [] CvtSqlBuf;
	delete [] EcdTableName;
	if (Ret != SQL_SUCCESS) {
		SQLGetDiagRec(SQL_HANDLE_STMT, pImpl->Hstmt, 1, CvtStateMsg, &Native, CvtMsg, 1024, &ActualMsgLen);
		ConvertMessage(StateMsg, Msg, (char16_t*)CvtStateMsg, (char16_t*)CvtMsg);
		return 0;
	}
	SQLWCHAR TmpColumnNameTmp[COLUMNNAME_LENGTH * 2]; // For adaptation to the bug of MariaDB ODBC connector
	SQLWCHAR TmpColumnTypeTmp[COLUMNTYPE_LENGTH * 2]; // For adaptation to the bug of MariaDB ODBC connector
	SQLWCHAR TmpIsNullTmp[10];
	SQLLEN ColumneNameLen = 0;
	SQLLEN ColumneTypeLen = 0;
	SQLLEN IsNullLen = 0;
	SQLBindCol(pImpl->Hstmt, 1, SQL_C_WCHAR, TmpColumnNameTmp, COLUMNNAME_LENGTH * sizeof(SQLWCHAR), &ColumneNameLen);
	SQLBindCol(pImpl->Hstmt, 2, SQL_C_WCHAR, TmpColumnTypeTmp, COLUMNTYPE_LENGTH * sizeof(SQLWCHAR), &ColumneTypeLen);
	SQLBindCol(pImpl->Hstmt, 4, SQL_C_WCHAR, TmpIsNullTmp, 10 * sizeof(SQLWCHAR), &IsNullLen);

	int Loop = 0;
	for (; Loop < MAXNUM_COLUMNS; Loop++) {
		Ret = SQLFetch(pImpl->Hstmt);
		if (Ret == SQL_NO_DATA_FOUND) break;
		if (Ret != SQL_SUCCESS && Ret != SQL_SUCCESS_WITH_INFO) {
			SQLGetDiagRec(SQL_HANDLE_STMT, pImpl->Hstmt, 1, CvtStateMsg, &Native, CvtMsg, 1024, &ActualMsgLen);
			ConvertMessage(StateMsg, Msg, (char16_t*)CvtStateMsg, (char16_t*)CvtMsg);
			return 0;
		}

		wchar_t* TmpColumnNameTmpCnv = StkPlCreateWideCharFromUtf16((char16_t*)TmpColumnNameTmp);
		wchar_t* TmpColumnTypeTmpCnv = StkPlCreateWideCharFromUtf16((char16_t*)TmpColumnTypeTmp);
		wchar_t* TmpIsNullCnv = StkPlCreateWideCharFromUtf16((char16_t*)TmpIsNullTmp);

		wchar_t TmpColumnName[COLUMNNAME_LENGTH];
		wchar_t TmpColumnType[COLUMNTYPE_LENGTH];
		wchar_t TmpIsNull[10];
		StkPlWcsCpy(TmpColumnName, (size_t)COLUMNNAME_LENGTH, TmpColumnNameTmpCnv);	// For adaptation to the bug of MariaDB ODBC connector
		StkPlWcsCpy(TmpColumnType, (size_t)COLUMNTYPE_LENGTH, TmpColumnTypeTmpCnv);	// For adaptation to the bug of MariaDB ODBC connector
		StkPlWcsCpy(TmpIsNull, 10, TmpIsNullCnv);											// For adaptation to the bug of MariaDB ODBC connector

		wchar_t ColTypeCnv[COLUMNTYPE_LENGTH];
		ConvertAttrType(TmpColumnType, ColTypeCnv);
		StkObject* ClmObj = new StkObject(L"ColumnInfo");
		ClmObj->AppendChildElement(new StkObject(L"title", (wchar_t*)TmpColumnName));
		ClmObj->AppendChildElement(new StkObject(L"width", 100));
		ClmObj->AppendChildElement(new StkObject(L"dataType", (wchar_t*)ColTypeCnv));
		ClmObj->AppendChildElement(new StkObject(L"dataIndx", Loop));
		ClmObj->AppendChildElement(new StkObject(L"coltype", (wchar_t*)TmpColumnType));
		ClmObj->AppendChildElement(new StkObject(L"isnull", (wchar_t*)TmpIsNull));
		TblObj->AppendChildElement(ClmObj);

		delete [] TmpColumnNameTmpCnv;
		delete [] TmpColumnTypeTmpCnv;
		delete [] TmpIsNullCnv;
	}
	Ret = CloseDatabase(StateMsg, Msg);

	return Loop;
}

int DbMariaDbAccessor::GetRecordsByTableName(wchar_t* TableName, FilteringCondition* FilCond, int NumOfCols, StkObject* DatObj, wchar_t* SortTarget, wchar_t* SortOrder, int Limit, int Offset, wchar_t StateMsg[10], wchar_t Msg[1024])
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

int DbMariaDbAccessor::ConvertAttrType(wchar_t InAttr[COLUMNTYPE_LENGTH], wchar_t OutAttr[COLUMNTYPE_LENGTH])
{
	if (StkPlWcsStr((wchar_t*)InAttr, L"bigint") != NULL ||
		StkPlWcsStr((wchar_t*)InAttr, L"int") != NULL ||
		StkPlWcsStr((wchar_t*)InAttr, L"mediumint") != NULL ||
		StkPlWcsStr((wchar_t*)InAttr, L"smallint") != NULL ||
		StkPlWcsStr((wchar_t*)InAttr, L"tinyint") != NULL) {
		StkPlLStrCpy((wchar_t*)OutAttr, L"integer");
		return 0;
	}
	if (StkPlWcsStr((wchar_t*)InAttr, L"float") != NULL ||
		StkPlWcsStr((wchar_t*)InAttr, L"double") != NULL ||
		StkPlWcsStr((wchar_t*)InAttr, L"double precision") != NULL) {
		StkPlLStrCpy((wchar_t*)OutAttr, L"float");
		return 0;
	}
	StkPlLStrCpy((wchar_t*)OutAttr, L"string");
	return 0;
}

void DbMariaDbAccessor::SqlEncoding(wchar_t* InSql, wchar_t* OutSql, int Type)
{
	size_t LenOfInSql = StkPlWcsLen(InSql);
	int OutSqlIndex = 0;
	if (Type == TYPE_KEY) {
		OutSql[0] = L'`';
		OutSqlIndex++;
	}
	for (int Loop = 0; Loop < LenOfInSql; Loop++) {
		if (InSql[Loop] == L'`' && Type == TYPE_KEY) {
			StkPlLStrCpy(&OutSql[OutSqlIndex], L"``");
			OutSqlIndex += 2;
			continue;
		}
		if (InSql[Loop] == L'\'' && (Type == TYPE_VALUE || Type == TYPE_LIKE_VALUE)) {
			StkPlLStrCpy(&OutSql[OutSqlIndex], L"\'\'");
			OutSqlIndex += 2;
			continue;
		}
		if (InSql[Loop] == L'\\' && Type == TYPE_VALUE) {
			StkPlLStrCpy(&OutSql[OutSqlIndex], L"\\\\");
			OutSqlIndex += 2;
			continue;
		}
		if (InSql[Loop] == L'\\' && Type == TYPE_LIKE_VALUE) {
			StkPlLStrCpy(&OutSql[OutSqlIndex], L"\\\\\\\\");
			OutSqlIndex += 4;
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
		OutSql[OutSqlIndex] = L'`';
		OutSqlIndex++;
	}
	OutSql[OutSqlIndex] = L'\0';
}
