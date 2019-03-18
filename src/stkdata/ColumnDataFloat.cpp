#include "stkdata.h"
#include "../stkpl/StkPl.h"

// Constructor
ColumnDataFloat::ColumnDataFloat(const wchar_t* ColumnName, float Value)
{
	StkPlWcsNCpy(m_ColumnName, COLUMN_NAME_SIZE, ColumnName, COLUMN_NAME_SIZE - 1);
	m_ColumnName[COLUMN_NAME_SIZE - 1] = L'\0';
	m_ColumnType = COLUMN_TYPE_FLOAT;
	m_ComparisonOperator = COMP_EQUAL;
	m_Value = Value;
}

ColumnDataFloat::ColumnDataFloat(const wchar_t* ColumnName, float Value, int CompOpe)
{
	*this = ColumnDataFloat(ColumnName, Value);
	m_ComparisonOperator = CompOpe;
}

// Destructor
ColumnDataFloat::~ColumnDataFloat()
{
}

float ColumnDataFloat::GetValue()
{
	return m_Value;
}
