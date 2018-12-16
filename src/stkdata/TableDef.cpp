#include <windows.h>
#include "stkdata.h"

// Constructor
TableDef::TableDef()
{
	lstrcpy(m_TableName, L"");
	for (int Loop = 0; Loop < MAX_COLUMN_NUMBER; Loop++) {
		m_Column[Loop] = NULL;
	}
	m_NumberOfColumn = 0;
	m_MaxRecord = 0;
}

// Constructor
TableDef::TableDef(wchar_t* TableName, int MaxRecord)
{
	for (int Loop = 0; Loop < MAX_COLUMN_NUMBER; Loop++) {
		m_Column[Loop] = NULL;
	}
	m_NumberOfColumn = 0;
	m_MaxRecord = MaxRecord;
	if (lstrcpyn(m_TableName, TableName, TABLE_NAME_SIZE) == NULL) {
		lstrcpy(m_TableName, L"");
	}
}

// Destructor
TableDef::~TableDef()
{
}

// Set table name
// [in] wchar_t* : Table Name
void TableDef::SetTableName(wchar_t* TableName)
{
	if (lstrcpyn(m_TableName, TableName, TABLE_NAME_SIZE) == NULL) {
		lstrcpy(m_TableName, L"");
	}
}

// Get table name
// [out] wchar_t* : Table name
wchar_t* TableDef::GetTableName()
{
	return m_TableName;
}

// Set maximum record number
// [in] int : maximum record number
void TableDef::SetMaxRecord(int MaxRecord)
{
	m_MaxRecord = MaxRecord;
}

// Get maximum record number
// [out] int : maximum record number
int TableDef::GetMaxRecord()
{
	return m_MaxRecord;
}

// Add column
// [in] ColumnDef: Column information
// [out] int : Result code (0: Success, -1: Error)
int TableDef::AddColumnDef(ColumnDef* ColDef)
{
	if (m_NumberOfColumn >= MAX_COLUMN_NUMBER) {
		return -1;
	}
	m_Column[m_NumberOfColumn] = ColDef;
	m_NumberOfColumn++;
	return 0;
}

// Delete column
// [out] int : Result code (0: Success, -1: Error)
int TableDef::DeleteLastColumnDef()
{
	if (m_NumberOfColumn <= 0) {
		return -1;
	}
	m_Column[m_NumberOfColumn - 1] = NULL;
	m_NumberOfColumn--;
	return 0;
}

// Get column
// [in] int : Column information number of the array
// [out] ColumnDef* : Pointer to ColumnDef which found (NULL: Not found)
ColumnDef* TableDef::GetColumnDef(int Num)
{
	if (Num < 0 || Num >= m_NumberOfColumn) {
		return NULL;
	}
	return m_Column[Num];
}

// Get number of column in the table
// [out] int : Number of column in the table
int TableDef::GetNumOfColumn()
{
	return m_NumberOfColumn;
}
