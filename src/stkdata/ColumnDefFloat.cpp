#include "stkdata.h"

// Constructor
ColumnDefFloat::ColumnDefFloat()
{
}

// Constructor
ColumnDefFloat::ColumnDefFloat(wchar_t* ColumnName)
{
	if (lstrcpyn(m_ColumnName, ColumnName, COLUMN_NAME_SIZE) == NULL) {
		lstrcpy(m_ColumnName, L"");
	}
	m_ColumnType = COLUMN_TYPE_FLOAT;
}

// Destructor
ColumnDefFloat::~ColumnDefFloat()
{
}
