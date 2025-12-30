#pragma once
#ifdef WIN32
	#include <windows.h>
#endif
#include "DbAccessor.h"
#include "FilteringCondition.h"

class DbPostgreSqlAccessor : public DbAccessor
{
protected:
	int ConvertAttrType(wchar_t[COLUMNTYPE_LENGTH], wchar_t[COLUMNTYPE_LENGTH]);
	void SqlEncoding(const wchar_t*, wchar_t*, int);

public:
	DbPostgreSqlAccessor(wchar_t*);
	~DbPostgreSqlAccessor();

	int GetNumOfRecords(wchar_t*, FilteringCondition*, wchar_t[10], wchar_t[1024]);
	int GetTables(StkObject*, wchar_t[10], wchar_t[1024]);
	int GetColumnInfoByTableName(wchar_t*,	StkObject*, wchar_t[10], wchar_t[1024]);
	int GetRecordsByTableName(wchar_t*, FilteringCondition*, int, StkObject*, wchar_t*, wchar_t*, int, int, wchar_t[10], wchar_t[1024]);
	int CreateTable(StkObject*, wchar_t[10], wchar_t[1024]);
	int DropTable(const wchar_t*, wchar_t[10], wchar_t[1024]);
	int InsertRecord(StkObject*, wchar_t[10], wchar_t[1024]);
};
