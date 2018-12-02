#include "stkdata.h"

// Constructor
ColumnDefInt::ColumnDefInt()
{
}

// Constructor
ColumnDefInt::ColumnDefInt(wchar_t* ColumnName)
{
	if (lstrcpyn(m_ColumnName, ColumnName, COLUMN_NAME_SIZE) == NULL) {
		lstrcpy(m_ColumnName, L"");
	}
	m_ColumnType = COLUMN_TYPE_INT;
}

// Destructor
ColumnDefInt::~ColumnDefInt()
{
}
