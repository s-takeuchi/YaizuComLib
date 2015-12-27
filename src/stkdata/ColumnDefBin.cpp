#include <windows.h>
#include <tchar.h>
#include "stkdata.h"

// Constructor
ColumnDefBin::ColumnDefBin()
{
	m_MaxLength = 10000000;
}

// Constructor
ColumnDefBin::ColumnDefBin(TCHAR* ColumnName, int MaxLength)
{
	if (lstrcpyn(m_ColumnName, ColumnName, COLUMN_NAME_SIZE) == NULL) {
		lstrcpy(m_ColumnName, _T(""));
	}
	m_ColumnType = COLUMN_TYPE_BIN;
	m_MaxLength = MaxLength;
}

// Destructor
ColumnDefBin::~ColumnDefBin()
{
}

// Set maximum length of string
// [in] int : maximum length
void ColumnDefBin::SetMaxLength(int MaxLength)
{
	m_MaxLength = MaxLength;
}

// Get maximum length of string
// [out] int : maximum length
int ColumnDefBin::GetMaxLength()
{
	return m_MaxLength;
}
