#include <windows.h>
#include <string.h>
#include "ColumnData.h"
#include "ColumnDataStr.h"

// Constructor
ColumnDataStr::ColumnDataStr(TCHAR* ColumnName, char* Value)
{
	lstrcpyn(m_ColumnName, ColumnName, COLUMN_NAME_SIZE);
	m_ColumnType = COLUMN_TYPE_STR;
	if (Value == NULL) {
		strcpy_s(m_Value, 256, "");
	} else {
		strcpy_s(m_Value, 256, Value);
	}
}

// Destructor
ColumnDataStr::~ColumnDataStr()
{
}

char* ColumnDataStr::GetValue()
{
	return m_Value;
}
