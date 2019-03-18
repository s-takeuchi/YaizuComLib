#include "stkdata.h"
#include "../stkpl/StkPl.h"

// Constructor
ColumnDataWStr::ColumnDataWStr(const wchar_t* ColumnName, const wchar_t* Value)
{
	StkPlWcsNCpy(m_ColumnName, COLUMN_NAME_SIZE, ColumnName, COLUMN_NAME_SIZE - 1);
	m_ColumnName[COLUMN_NAME_SIZE - 1] = L'\0';
	m_ColumnType = COLUMN_TYPE_WSTR;
	m_ComparisonOperator = COMP_EQUAL;
	if (Value == NULL) {
		StkPlWcsNCpy(m_Value, 256, L"", 256 - 1);
		m_Value[256 - 1] = L'\0';
	} else {
		StkPlWcsNCpy(m_Value, 256, Value, 256 - 1);
		m_Value[256 - 1] = L'\0';
	}
}

ColumnDataWStr::ColumnDataWStr(const wchar_t* ColumnName, const wchar_t* Value, int CompOpe)
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
