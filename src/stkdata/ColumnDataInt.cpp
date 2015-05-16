#include <windows.h>
#include "ColumnData.h"
#include "ColumnDataInt.h"

// Constructor
ColumnDataInt::ColumnDataInt(TCHAR* ColumnName, int Value)
{
	lstrcpyn(m_ColumnName, ColumnName, COLUMN_NAME_SIZE);
	m_ColumnType = COLUMN_TYPE_INT;
	m_Value = Value;
}

// Destructor
ColumnDataInt::~ColumnDataInt()
{
}

int ColumnDataInt::GetValue()
{
	return m_Value;
}
