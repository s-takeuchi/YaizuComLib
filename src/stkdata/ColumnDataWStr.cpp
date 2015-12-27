#include <windows.h>
#include <tchar.h>
#include <string.h>
#include "stkdata.h"

// Constructor
ColumnDataWStr::ColumnDataWStr(TCHAR* ColumnName, TCHAR* Value)
{
	lstrcpyn(m_ColumnName, ColumnName, COLUMN_NAME_SIZE);
	m_ColumnType = COLUMN_TYPE_WSTR;
	if (Value == NULL) {
		lstrcpyn(m_Value, _T(""), 256);
	} else {
		lstrcpyn(m_Value, Value, 256);
	}
}

// Destructor
ColumnDataWStr::~ColumnDataWStr()
{
}

TCHAR* ColumnDataWStr::GetValue()
{
	return m_Value;
}
