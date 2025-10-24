#ifdef WIN32
	#include <windows.h>
#endif
#include <sql.h>
#include <sqlext.h>

#include "../../../YaizuComLib/src/stkpl/StkPl.h"
#include "DbAccessor.h"
#include "DbMySqlAccessor.h"
#include "FilteringCondition.h"

class DbAccessor::Impl
{
public:
	wchar_t* ConnectionString;

	SQLHENV  Henv;
	SQLHDBC  Hdbc;
	SQLHSTMT Hstmt;
};

DbMySqlAccessor::DbMySqlAccessor(wchar_t* TmpConnStr) : DbAccessor(TmpConnStr)
{
}

DbMySqlAccessor::~DbMySqlAccessor()
{
}

int DbMySqlAccessor::GetNumOfRecords(wchar_t* TableName, FilteringCondition* FilCond, wchar_t StateMsg[10], wchar_t Msg[1024])
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

int DbMySqlAccessor::GetTables(StkObject* Obj, wchar_t StateMsg[10], wchar_t Msg[1024])
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

int DbMySqlAccessor::GetColumnInfoByTableName(wchar_t* TableName, StkObject* TblObj, wchar_t StateMsg[10], wchar_t Msg[1024])
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
	delete CvtSqlBuf;
	delete [] EcdTableName;
	if (Ret != SQL_SUCCESS) {
		SQLGetDiagRec(SQL_HANDLE_STMT, pImpl->Hstmt, 1, CvtStateMsg, &Native, CvtMsg, 1024, &ActualMsgLen);
		ConvertMessage(StateMsg, Msg, (char16_t*)CvtStateMsg, (char16_t*)CvtMsg);
		return 0;
	}
	SQLWCHAR TmpColumnName[COLUMNNAME_LENGTH];
	SQLWCHAR TmpColumnType[COLUMNTYPE_LENGTH];
	SQLWCHAR TmpIsNull[10];
	SQLBindCol(pImpl->Hstmt, 1, SQL_C_WCHAR, TmpColumnName, COLUMNNAME_LENGTH * sizeof(SQLWCHAR), NULL);
	SQLBindCol(pImpl->Hstmt, 2, SQL_C_WCHAR, TmpColumnType, COLUMNTYPE_LENGTH * sizeof(SQLWCHAR), NULL);
	SQLBindCol(pImpl->Hstmt, 4, SQL_C_WCHAR, TmpIsNull, 10 * sizeof(SQLWCHAR), NULL);

	int Loop = 0;
	for (; Loop < MAXNUM_COLUMNS; Loop++) {
		Ret = SQLFetch(pImpl->Hstmt);
		if (Ret == SQL_NO_DATA_FOUND) break;
		if (Ret != SQL_SUCCESS && Ret != SQL_SUCCESS_WITH_INFO) {
			SQLGetDiagRec(SQL_HANDLE_STMT, pImpl->Hstmt, 1, CvtStateMsg, &Native, CvtMsg, 1024, &ActualMsgLen);
			ConvertMessage(StateMsg, Msg, (char16_t*)CvtStateMsg, (char16_t*)CvtMsg);
			return 0;
		}

		wchar_t* TmpColumnNameCnv = StkPlCreateWideCharFromUtf16((char16_t*)TmpColumnName);
		wchar_t* TmpColumnTypeCnv = StkPlCreateWideCharFromUtf16((char16_t*)TmpColumnType);
		wchar_t* TmpIsNullCnv = StkPlCreateWideCharFromUtf16((char16_t*)TmpIsNull);

		wchar_t ColTypeCnv[COLUMNTYPE_LENGTH];
		ConvertAttrType(TmpColumnTypeCnv, ColTypeCnv);
		StkObject* ClmObj = new StkObject(L"ColumnInfo");
		ClmObj->AppendChildElement(new StkObject(L"title", TmpColumnNameCnv));
		ClmObj->AppendChildElement(new StkObject(L"width", 100));
		ClmObj->AppendChildElement(new StkObject(L"dataType", ColTypeCnv));
		ClmObj->AppendChildElement(new StkObject(L"dataIndx", Loop));
		ClmObj->AppendChildElement(new StkObject(L"coltype", TmpColumnTypeCnv));
		ClmObj->AppendChildElement(new StkObject(L"isnull", TmpIsNullCnv));
		TblObj->AppendChildElement(ClmObj);

		delete TmpColumnNameCnv;
		delete TmpColumnTypeCnv;
		delete TmpIsNullCnv;
	}
	Ret = CloseDatabase(StateMsg, Msg);

	return Loop;
}

int DbMySqlAccessor::GetRecordsByTableName(wchar_t* TableName, FilteringCondition* FilCond, int NumOfCols, StkObject* DatObj, wchar_t* SortTarget, wchar_t* SortOrder, int Limit, int Offset, wchar_t StateMsg[10], wchar_t Msg[1024])
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

int DbMySqlAccessor::ConvertAttrType(wchar_t InAttr[COLUMNTYPE_LENGTH], wchar_t OutAttr[COLUMNTYPE_LENGTH])
{
	if (StkPlWcsStr(InAttr, L"bigint") != NULL ||
		StkPlWcsStr(InAttr, L"int") != NULL ||
		StkPlWcsStr(InAttr, L"mediumint") != NULL ||
		StkPlWcsStr(InAttr, L"smallint") != NULL ||
		StkPlWcsStr(InAttr, L"tinyint") != NULL) {
		StkPlLStrCpy(OutAttr, L"integer");
		return 0;
	}
	if (StkPlWcsStr(InAttr, L"float") != NULL ||
		StkPlWcsStr(InAttr, L"double") != NULL ||
		StkPlWcsStr(InAttr, L"double precision") != NULL) {
		StkPlLStrCpy(OutAttr, L"float");
		return 0;
	}
	StkPlLStrCpy(OutAttr, L"string");
	return 0;
}

void DbMySqlAccessor::SqlEncoding(wchar_t* InSql, wchar_t* OutSql, int Type)
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
