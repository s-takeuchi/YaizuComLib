#include "stkdata.h"
#include "../StkPl.h"

// Constructor
ColumnDefFloat::ColumnDefFloat()
{
}

// Constructor
ColumnDefFloat::ColumnDefFloat(const wchar_t* ColumnName)
{
	StkPlWcsNCpy(m_ColumnName, COLUMN_NAME_SIZE, ColumnName, COLUMN_NAME_SIZE - 1);
	m_ColumnName[COLUMN_NAME_SIZE - 1] = L'\0';
	m_ColumnType = COLUMN_TYPE_FLOAT;
}

// Destructor
ColumnDefFloat::~ColumnDefFloat()
{
}
