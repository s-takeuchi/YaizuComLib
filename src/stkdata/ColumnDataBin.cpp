#include <windows.h>
#include <string.h>
#include "stkdata.h"

// Constructor
ColumnDataBin::ColumnDataBin(TCHAR* ColumnName, BYTE* Value, int Length)
{
	lstrcpyn(m_ColumnName, ColumnName, COLUMN_NAME_SIZE);
	m_ColumnType = COLUMN_TYPE_BIN;
	m_Value = new BYTE[Length];
	if (Value == NULL) {
		FillMemory(m_Value, Length, 0);
	} else {
		FillMemory(m_Value, Length, 0);
		CopyMemory(m_Value, Value, Length);
	}
}

// Destructor
ColumnDataBin::~ColumnDataBin()
{
	delete m_Value;
}

BYTE* ColumnDataBin::GetValue()
{
	return m_Value;
}
