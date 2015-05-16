#include <windows.h>
#include <tchar.h>
#include "ColumnData.h"

// Constructor
ColumnData::ColumnData()
{
	lstrcpy(m_ColumnName, _T(""));
}

// Destructor
ColumnData::~ColumnData()
{
}

// Set column name
// [in] TCHAR* : Column name
void ColumnData::SetColumnName(TCHAR* ColumnName)
{
	lstrcpyn(m_ColumnName, ColumnName, COLUMN_NAME_SIZE);
}

// Get column name
// [out] TCHAR* : Column name
TCHAR* ColumnData::GetColumnName()
{
	return m_ColumnName;
}

// Get column type
// [out] int : Column type
int ColumnData::GetColumnType()
{
	return m_ColumnType;
}
