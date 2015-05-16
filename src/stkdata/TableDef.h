#pragma once
#include <windows.h>
#include "ColumnDef.h"
#include "stkdatadef.h"

class TableDef
{
private:
	TCHAR m_TableName[TABLE_NAME_SIZE];
	ColumnDef* m_Column[MAX_COLUMN_NUMBER];
	int m_NumberOfColumn;
	int m_MaxRecord;

public:
	TableDef();
	TableDef(TCHAR*, int);
	virtual ~TableDef();

	// Set table name
	void SetTableName(TCHAR*);
	// Get table name
	TCHAR* GetTableName();

	// Set maximum record number
	void SetMaxRecord(int);
	// Get maximum record number
	int GetMaxRecord();

	// Add column
	int AddColumnDef(ColumnDef*);
	// Delete column
	int DeleteLastColumnDef();
	// Get column
	ColumnDef* GetColumnDef(int);
	// Get number of column in the table
	int GetNumOfColumn();
};
