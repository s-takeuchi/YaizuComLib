#include <tchar.h>
#include "stkdata.h"

// Constructor
ColumnDefFloat::ColumnDefFloat()
{
}

// Constructor
ColumnDefFloat::ColumnDefFloat(TCHAR* ColumnName)
{
	if (lstrcpyn(m_ColumnName, ColumnName, COLUMN_NAME_SIZE) == NULL) {
		lstrcpy(m_ColumnName, _T(""));
	}
	m_ColumnType = COLUMN_TYPE_FLOAT;
}

// Destructor
ColumnDefFloat::~ColumnDefFloat()
{
}
