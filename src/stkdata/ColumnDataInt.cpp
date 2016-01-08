#include <windows.h>
#include "stkdata.h"

// Constructor
ColumnDataInt::ColumnDataInt(TCHAR* ColumnName, int Value)
{
	lstrcpyn(m_ColumnName, ColumnName, COLUMN_NAME_SIZE);
	m_ColumnType = COLUMN_TYPE_INT;
	m_ComparisonOperator = COMP_EQUAL;
	m_Value = Value;
}

ColumnDataInt::ColumnDataInt(TCHAR* ColumnName, int Value, int CompOpe)
{
	ColumnDataInt(ColumnName, Value);
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
