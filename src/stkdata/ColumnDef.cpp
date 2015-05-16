#include <windows.h>
#include <tchar.h>
#include "ColumnDef.h"

// Constructor
ColumnDef::ColumnDef()
{
	lstrcpy(m_ColumnName, _T(""));
	m_ColumnType = -1;
}

// Destructor
ColumnDef::~ColumnDef()
{
}

// Set column name
// [in] TCHAR* : Column name
void ColumnDef::SetColumnName(TCHAR* ColumnName)
{
	if (lstrcpyn(m_ColumnName, ColumnName, COLUMN_NAME_SIZE) == NULL) {
		lstrcpy(m_ColumnName, _T(""));
	}
}

// Get column name
// [out] TCHAR* : Column name
TCHAR* ColumnDef::GetColumnName()
{
	return m_ColumnName;
}

// Set column type
// [in] int : Column type
void ColumnDef::SetColumnType(int Type)
{
	m_ColumnType = Type;
}

// Get column type
// [out] int : Column type
int ColumnDef::GetColumnType()
{
	return m_ColumnType;
}
