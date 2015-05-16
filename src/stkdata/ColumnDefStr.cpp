#include <windows.h>
#include <tchar.h>
#include "ColumnDefStr.h"

// Constructor
ColumnDefStr::ColumnDefStr()
{
	m_MaxLength = 256;
}

// Constructor
ColumnDefStr::ColumnDefStr(TCHAR* ColumnName, int MaxLength)
{
	if (lstrcpyn(m_ColumnName, ColumnName, COLUMN_NAME_SIZE) == NULL) {
		lstrcpy(m_ColumnName, _T(""));
	}
	m_ColumnType = COLUMN_TYPE_STR;
	m_MaxLength = MaxLength;
}

// Destructor
ColumnDefStr::~ColumnDefStr()
{
}

// Set maximum length of string
// [in] int : maximum length
void ColumnDefStr::SetMaxLength(int MaxLength)
{
	m_MaxLength = MaxLength;
}

// Get maximum length of string
// [out] int : maximum length
int ColumnDefStr::GetMaxLength()
{
	return m_MaxLength;
}
