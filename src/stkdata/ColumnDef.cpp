#include "stkdata.h"
#include "../StkPl.h"

// Constructor
ColumnDef::ColumnDef()
{
	StkPlWcsCpy(m_ColumnName, COLUMN_NAME_SIZE, L"");
	m_ColumnType = -1;
}

// Destructor
ColumnDef::~ColumnDef()
{
}

// Set column name
// [in] wchar_t* : Column name
void ColumnDef::SetColumnName(const wchar_t* ColumnName)
{
	StkPlWcsNCpy(m_ColumnName, COLUMN_NAME_SIZE, ColumnName, COLUMN_NAME_SIZE - 1);
	m_ColumnName[COLUMN_NAME_SIZE - 1] = L'\0';
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
