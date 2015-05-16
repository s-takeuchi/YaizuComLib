#include <tchar.h>
#include "ColumnDefInt.h"

// Constructor
ColumnDefInt::ColumnDefInt()
{
}

// Constructor
ColumnDefInt::ColumnDefInt(TCHAR* ColumnName)
{
	if (lstrcpyn(m_ColumnName, ColumnName, COLUMN_NAME_SIZE) == NULL) {
		lstrcpy(m_ColumnName, _T(""));
	}
	m_ColumnType = COLUMN_TYPE_INT;
}

// Destructor
ColumnDefInt::~ColumnDefInt()
{
}
