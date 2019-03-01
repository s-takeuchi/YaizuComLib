#include "stkdata.h"
#include "../StkPl.h"

// Constructor
ColumnDataStr::ColumnDataStr(const wchar_t* ColumnName, const char* Value)
{
	StkPlWcsNCpy(m_ColumnName, COLUMN_NAME_SIZE, ColumnName, COLUMN_NAME_SIZE - 1);
	m_ColumnName[COLUMN_NAME_SIZE - 1] = L'\0';
	m_ColumnType = COLUMN_TYPE_STR;
	m_ComparisonOperator = COMP_EQUAL;
	if (Value == NULL) {
		StkPlStrCpy(m_Value, 256, "");
	} else {
		StkPlStrCpy(m_Value, 256, Value);
	}
}

ColumnDataStr::ColumnDataStr(const wchar_t* ColumnName, const char* Value, int CompOpe)
{
	*this = ColumnDataStr(ColumnName, Value);
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
