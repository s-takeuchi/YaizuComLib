#include "stkdata.h"
#include "../StkPl.h"

// Constructor
ColumnData::ColumnData()
{
	StkPlWcsCpy(m_ColumnName, COLUMN_NAME_SIZE, L"");
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
	StkPlWcsNCpy(m_ColumnName, COLUMN_NAME_SIZE, ColumnName, COLUMN_NAME_SIZE - 1);
	m_ColumnName[COLUMN_NAME_SIZE - 1] = L'\0';
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
