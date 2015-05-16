#include <windows.h>
#include "ColumnData.h"
#include "ColumnDataFloat.h"

// Constructor
ColumnDataFloat::ColumnDataFloat(TCHAR* ColumnName, float Value)
{
	lstrcpyn(m_ColumnName, ColumnName, COLUMN_NAME_SIZE);
	m_ColumnType = COLUMN_TYPE_FLOAT;
	m_Value = Value;
}

// Destructor
ColumnDataFloat::~ColumnDataFloat()
{
}

float ColumnDataFloat::GetValue()
{
	return m_Value;
}
