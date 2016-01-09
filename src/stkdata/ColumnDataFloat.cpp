#include <windows.h>
#include "stkdata.h"

// Constructor
ColumnDataFloat::ColumnDataFloat(TCHAR* ColumnName, float Value)
{
	lstrcpyn(m_ColumnName, ColumnName, COLUMN_NAME_SIZE);
	m_ColumnType = COLUMN_TYPE_FLOAT;
	m_ComparisonOperator = COMP_EQUAL;
	m_Value = Value;
}

ColumnDataFloat::ColumnDataFloat(TCHAR* ColumnName, float Value, int CompOpe)
{
	*this = ColumnDataFloat(ColumnName, Value);
	m_ComparisonOperator = CompOpe;
}

// Destructor
ColumnDataFloat::~ColumnDataFloat()
{
}

float ColumnDataFloat::GetValue()
{
	return m_Value;
}
