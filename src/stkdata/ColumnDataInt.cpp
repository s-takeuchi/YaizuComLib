#include "stkdata.h"
#include "../StkPl.h"

// Constructor
ColumnDataInt::ColumnDataInt(wchar_t* ColumnName, int Value)
{
	StkPlWcsNCpy(m_ColumnName, COLUMN_NAME_SIZE, ColumnName, COLUMN_NAME_SIZE - 1);
	m_ColumnName[COLUMN_NAME_SIZE - 1] = L'\0';
	m_ColumnType = COLUMN_TYPE_INT;
	m_ComparisonOperator = COMP_EQUAL;
	m_Value = Value;
}

ColumnDataInt::ColumnDataInt(wchar_t* ColumnName, int Value, int CompOpe)
{
	*this = ColumnDataInt(ColumnName, Value);
	m_ComparisonOperator = CompOpe;
}

// Destructor
ColumnDataInt::~ColumnDataInt()
{
}

int ColumnDataInt::GetValue()
{
	return m_Value;
}
