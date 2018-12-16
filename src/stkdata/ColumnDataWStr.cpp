#include <windows.h>
#include <string.h>
#include "stkdata.h"

// Constructor
ColumnDataWStr::ColumnDataWStr(wchar_t* ColumnName, wchar_t* Value)
{
	lstrcpyn(m_ColumnName, ColumnName, COLUMN_NAME_SIZE);
	m_ColumnType = COLUMN_TYPE_WSTR;
	m_ComparisonOperator = COMP_EQUAL;
	if (Value == NULL) {
		lstrcpyn(m_Value, L"", 256);
	} else {
		lstrcpyn(m_Value, Value, 256);
	}
}

ColumnDataWStr::ColumnDataWStr(wchar_t* ColumnName, wchar_t* Value, int CompOpe)
{
	*this = ColumnDataWStr(ColumnName, Value);
	m_ComparisonOperator = CompOpe;
}

// Destructor
ColumnDataWStr::~ColumnDataWStr()
{
}

wchar_t* ColumnDataWStr::GetValue()
{
	return m_Value;
}
