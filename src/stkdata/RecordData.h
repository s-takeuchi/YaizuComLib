#pragma once
#include <windows.h>
#include "ColumnData.h"
#include "stkdatadef.h"

class RecordData
{
private:
	int m_CurrentColumnNum;
	TCHAR m_TableName[TABLE_NAME_SIZE];
	ColumnData* m_ColumnData[MAX_COLUMN_NUMBER];
	RecordData* m_NextRecord;

public:
	RecordData();
	RecordData(TCHAR TableName[TABLE_NAME_SIZE], ColumnData**, int);
	virtual ~RecordData();

	void AddColumn(ColumnData*);
	void DeleteColumn();
	ColumnData* GetColumn(int ColIndex);
	ColumnData* GetColumn(TCHAR*);
	int GetColumnCount();
	void SetTableName(TCHAR TableName[TABLE_NAME_SIZE]);
	TCHAR* GetTableName();
	void SetNextRecord(RecordData*);
	RecordData* GetNextRecord();
};
