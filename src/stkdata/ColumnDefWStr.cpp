#include <windows.h>
#include "stkdata.h"

// Constructor
ColumnDefWStr::ColumnDefWStr()
{
	m_MaxLength = 256;
}

// Constructor
ColumnDefWStr::ColumnDefWStr(wchar_t* ColumnName, int MaxLength)
{
	if (lstrcpyn(m_ColumnName, ColumnName, COLUMN_NAME_SIZE) == NULL) {
		lstrcpy(m_ColumnName, L"");
	}
	m_ColumnType = COLUMN_TYPE_WSTR;
	m_MaxLength = MaxLength;
}

// Destructor
ColumnDefWStr::~ColumnDefWStr()
{
}

// Set maximum length of string
// [in] int : maximum length
void ColumnDefWStr::SetMaxLength(int MaxLength)
{
	m_MaxLength = MaxLength;
}

// Get maximum length of string
// [out] int : maximum length
int ColumnDefWStr::GetMaxLength()
{
	return m_MaxLength;
}
