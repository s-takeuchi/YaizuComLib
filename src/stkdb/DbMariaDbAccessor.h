﻿#pragma once
#ifdef WIN32
	#include <windows.h>
#endif
#include "DbAccessor.h"
#include "FilteringCondition.h"

class DbMariaDbAccessor : public DbAccessor
{
private:
	static const int TYPE_KEY = 0;
	static const int TYPE_VALUE = 1;
	static const int TYPE_LIKE_VALUE = 2;

protected:
	int ConvertAttrType(wchar_t[COLUMNTYPE_LENGTH], wchar_t[COLUMNTYPE_LENGTH]);
	void SqlEncoding(wchar_t*, wchar_t*, int);

public:
	DbMariaDbAccessor(wchar_t*);
	~DbMariaDbAccessor();

	int GetNumOfRecords(wchar_t*, FilteringCondition*, wchar_t[10], wchar_t[1024]);
	int GetTables(StkObject*, wchar_t[10], wchar_t[1024]);
	int GetColumnInfoByTableName(wchar_t*,	StkObject*, wchar_t[10], wchar_t[1024]);
	int GetRecordsByTableName(wchar_t*, FilteringCondition*, int, StkObject*, wchar_t*, wchar_t*, int, int, wchar_t[10], wchar_t[1024]);
};
