#include "stkdata.h"
#include "../StkPl.h"

// Constructor
ColumnDefWStr::ColumnDefWStr()
{
	m_MaxLength = 256;
}

// Constructor
ColumnDefWStr::ColumnDefWStr(const wchar_t* ColumnName, int MaxLength)
{
	StkPlWcsNCpy(m_ColumnName, COLUMN_NAME_SIZE, ColumnName, COLUMN_NAME_SIZE - 1);
	m_ColumnName[COLUMN_NAME_SIZE - 1] = L'\0';
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
