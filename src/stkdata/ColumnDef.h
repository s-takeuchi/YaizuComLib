#pragma once
#include <windows.h>
#include "stkdatadef.h"

class ColumnDef
{
protected:
	TCHAR m_ColumnName[COLUMN_NAME_SIZE];
	int m_ColumnType;

public:
	ColumnDef();
	virtual ~ColumnDef();

	// Set column name
	void SetColumnName(TCHAR*);
	// Get column name
	TCHAR* GetColumnName();

	// Set column type
	void SetColumnType(int);
	// Get column type
	int GetColumnType();
};
