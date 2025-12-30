#pragma once
#ifdef WIN32
    #include <windows.h>
#endif
#include "FilteringCondition.h"
#include "../../../YaizuComLib/src/commonfunc/StkObject.h"

class DbAccessor
{
protected:
	class Impl;
	Impl* pImpl;

protected:
	static const int MAX_PARAM_LENGTH = 28800;
	static const int TABLENAME_LENGTH = 65;
	static const int COLUMNNAME_LENGTH = 65;
	static const int COLUMNTYPE_LENGTH = 64;
	static const int COLUMNVAL_LENGTH = 64;
	static const int MAXNUM_TABLES = 300;
	static const int MAXNUM_COLUMNS = 50;

	static const int TYPE_KEY = 0;
	static const int TYPE_VALUE = 1;
	static const int TYPE_LIKE_VALUE = 2;

protected:
	int OpenDatabase(wchar_t[10], wchar_t[1024]);
	int CloseDatabase(wchar_t[10], wchar_t[1024]);

	int GetTablesCommon(const wchar_t*, StkObject*, wchar_t[10], wchar_t[1024]);
	int GetRecordsByTableNameCommon(const wchar_t*, int, StkObject* RecObj, bool,
		wchar_t[5][COLUMNNAME_LENGTH * 4 + 2], int[5], const wchar_t[5][COLUMNVAL_LENGTH * 4 + 2],
		const wchar_t[COLUMNNAME_LENGTH * 4 + 2], const wchar_t[5],
		int, int, wchar_t[10], wchar_t[1024]);
	int GetNumOfRecordsCommon(wchar_t*, bool, wchar_t[5][COLUMNNAME_LENGTH * 4 + 2],
		int[5], wchar_t[5][COLUMNVAL_LENGTH * 4 + 2], wchar_t[10], wchar_t[1024]);
	int CreateTableCommon(StkObject*, wchar_t[10], wchar_t[1024]);
	int DropTableCommon(wchar_t*, wchar_t[10], wchar_t[1024]);
	int InsertRecordCommon(StkObject*, wchar_t[10], wchar_t[1024]);

	void ConvertMessage(wchar_t[10], wchar_t[1024], const char16_t[10], const char16_t[1024]);

protected:
	virtual int ConvertAttrType(wchar_t[COLUMNTYPE_LENGTH], wchar_t[COLUMNTYPE_LENGTH]) = 0;
	virtual void SqlEncoding(wchar_t*, wchar_t*, int) = 0;

public:
	DbAccessor(wchar_t*);
	virtual ~DbAccessor();
	static int Init();
	static int Uninit();

	virtual int Test(wchar_t[10], wchar_t[1024]);
	virtual int GetNumOfRecords(wchar_t*, FilteringCondition*, wchar_t[10], wchar_t[1024]) = 0;
	virtual int GetTables(StkObject*, wchar_t[10], wchar_t[1024]) = 0;
	virtual int GetColumnInfoByTableName(wchar_t*,	StkObject*, wchar_t[10], wchar_t[1024]) = 0;
	virtual int GetRecordsByTableName(wchar_t*, FilteringCondition*, int, StkObject*, wchar_t*, wchar_t*, int, int, wchar_t[10], wchar_t[1024]) = 0;
	virtual int CreateTable(StkObject*, wchar_t[10], wchar_t[1024]) = 0;
	virtual int DropTable(wchar_t*, wchar_t[10], wchar_t[1024]) = 0;
	virtual int InsertRecord(StkObject*, wchar_t[10], wchar_t[1024]) = 0;
};
