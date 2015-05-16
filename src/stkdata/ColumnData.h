#pragma once
#include <windows.h>
#include "stkdatadef.h"

class ColumnData
{
protected:
	TCHAR m_ColumnName[COLUMN_NAME_SIZE];
	int m_ColumnType;

public:
	ColumnData();
	virtual ~ColumnData();

	// Set column name
	void SetColumnName(TCHAR*);
	// Get column name
	TCHAR* GetColumnName();

	// Get column type
	int GetColumnType();
};
