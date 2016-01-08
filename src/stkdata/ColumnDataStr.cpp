#include <windows.h>
#include <string.h>
#include "stkdata.h"

// Constructor
ColumnDataStr::ColumnDataStr(TCHAR* ColumnName, char* Value)
{
	lstrcpyn(m_ColumnName, ColumnName, COLUMN_NAME_SIZE);
	m_ColumnType = COLUMN_TYPE_STR;
	m_ComparisonOperator = COMP_EQUAL;
	if (Value == NULL) {
		strcpy_s(m_Value, 256, "");
	} else {
		strcpy_s(m_Value, 256, Value);
	}
}

ColumnDataStr::ColumnDataStr(TCHAR* ColumnName, char* Value, int CompOpe)
{
	ColumnDataStr(ColumnName, Value);
	m_ComparisonOperator = CompOpe;
}

// Destructor
ColumnDataStr::~ColumnDataStr()
{
}

char* ColumnDataStr::GetValue()
{
	return m_Value;
}
