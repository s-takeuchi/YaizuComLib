#include <windows.h>
#include "stkdata.h"

// Constructor
ColumnDataInt::ColumnDataInt(wchar_t* ColumnName, int Value)
{
	lstrcpyn(m_ColumnName, ColumnName, COLUMN_NAME_SIZE);
	m_ColumnType = COLUMN_TYPE_INT;
	m_ComparisonOperator = COMP_EQUAL;
	m_Value = Value;
}

ColumnDataInt::ColumnDataInt(wchar_t* ColumnName, int Value, int CompOpe)
{
	*this = ColumnDataInt(ColumnName, Value);
	m_ComparisonOperator = CompOpe;
}

// Destructor
ColumnDataInt::~ColumnDataInt()
{
}

int ColumnDataInt::GetValue()
{
	return m_Value;
}
