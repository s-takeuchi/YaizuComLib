#include <windows.h>
#include <string.h>
#include "stkdata.h"

// Constructor
ColumnDataBin::ColumnDataBin(wchar_t* ColumnName, unsigned char* Value, int Length)
{
	lstrcpyn(m_ColumnName, ColumnName, COLUMN_NAME_SIZE);
	m_ColumnType = COLUMN_TYPE_BIN;
	m_ComparisonOperator = COMP_EQUAL;
	m_Value = new unsigned char[Length];
	if (Value == NULL) {
		FillMemory(m_Value, Length, 0);
	} else {
		FillMemory(m_Value, Length, 0);
		CopyMemory(m_Value, Value, Length);
	}
}

ColumnDataBin::ColumnDataBin(wchar_t* ColumnName, unsigned char* Value, int Length, int CompOpe)
{
	*this = ColumnDataBin(ColumnName, Value, Length);
	m_ComparisonOperator = CompOpe;
}

// Destructor
ColumnDataBin::~ColumnDataBin()
{
	delete m_Value;
}

unsigned char* ColumnDataBin::GetValue()
{
	return m_Value;
}
