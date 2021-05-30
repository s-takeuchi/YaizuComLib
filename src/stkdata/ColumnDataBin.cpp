#include "stkdata.h"
#include "../stkpl/StkPl.h"

// Constructor
ColumnDataBin::ColumnDataBin(const wchar_t* ColumnName, const unsigned char* Value, int Length)
{
	StkPlWcsNCpy(m_ColumnName, COLUMN_NAME_SIZE, ColumnName, COLUMN_NAME_SIZE - 1);
	m_ColumnName[COLUMN_NAME_SIZE - 1] = L'\0';
	m_ColumnType = COLUMN_TYPE_BIN;
	m_ComparisonOperator = COMP_EQUAL;
	m_Value = new unsigned char[Length];
	if (Value == NULL) {
		StkPlMemSet(m_Value, 0, Length);
	} else {
		StkPlMemSet(m_Value, 0, Length);
		StkPlMemCpy(m_Value, Value, Length);
	}
	m_Length = Length;
}

ColumnDataBin::ColumnDataBin(const wchar_t* ColumnName, const unsigned char* Value, int Length, int CompOpe)
{
	*this = ColumnDataBin(ColumnName, Value, Length);
	m_ComparisonOperator = CompOpe;
}

// Destructor
ColumnDataBin::~ColumnDataBin()
{
	m_Length = 0;
	delete m_Value;
}

unsigned char* ColumnDataBin::GetValue()
{
	return m_Value;
}

int ColumnDataBin::GetLength()
{
	return m_Length;
}
