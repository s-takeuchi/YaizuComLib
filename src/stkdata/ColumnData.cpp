#include <windows.h>
#include "stkdata.h"

// Constructor
ColumnData::ColumnData()
{
	lstrcpy(m_ColumnName, L"");
	m_ComparisonOperator = COMP_EQUAL;
}

// Destructor
ColumnData::~ColumnData()
{
}

// Set column name
// [in] wchar_t* : Column name
void ColumnData::SetColumnName(wchar_t* ColumnName)
{
	lstrcpyn(m_ColumnName, ColumnName, COLUMN_NAME_SIZE);
}

// Get column name
// [out] wchar_t* : Column name
wchar_t* ColumnData::GetColumnName()
{
	return m_ColumnName;
}

// Get column type
// [out] int : Column type
int ColumnData::GetColumnType()
{
	return m_ColumnType;
}

// Set comparison operator
// [in] CompOpe : Comparison operator
void ColumnData::SetComparisonOperator(int CompOpe)
{
	m_ComparisonOperator = CompOpe;
}

// Get comparison operator
// [out] int : Comparison operator
int ColumnData::GetComparisonOperator()
{
	return m_ComparisonOperator;
}
