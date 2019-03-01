#include "stkdata.h"
#include "../StkPl.h"

// Constructor
ColumnDefStr::ColumnDefStr()
{
	m_MaxLength = 256;
}

// Constructor
ColumnDefStr::ColumnDefStr(const wchar_t* ColumnName, int MaxLength)
{
	StkPlWcsNCpy(m_ColumnName, COLUMN_NAME_SIZE, ColumnName, COLUMN_NAME_SIZE - 1);
	m_ColumnName[COLUMN_NAME_SIZE - 1] = L'\0';
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
