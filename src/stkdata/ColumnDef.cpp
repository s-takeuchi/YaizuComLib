#include <windows.h>
#include "stkdata.h"

// Constructor
ColumnDef::ColumnDef()
{
	lstrcpy(m_ColumnName, L"");
	m_ColumnType = -1;
}

// Destructor
ColumnDef::~ColumnDef()
{
}

// Set column name
// [in] wchar_t* : Column name
void ColumnDef::SetColumnName(wchar_t* ColumnName)
{
	if (lstrcpyn(m_ColumnName, ColumnName, COLUMN_NAME_SIZE) == NULL) {
		lstrcpy(m_ColumnName, L"");
	}
}

// Get column name
// [out] wchar_t* : Column name
wchar_t* ColumnDef::GetColumnName()
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
