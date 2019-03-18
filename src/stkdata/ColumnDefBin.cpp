#include "stkdata.h"
#include "../stkpl/StkPl.h"

// Constructor
ColumnDefBin::ColumnDefBin()
{
	m_MaxLength = 10000000;
}

// Constructor
ColumnDefBin::ColumnDefBin(const wchar_t* ColumnName, int MaxLength)
{
	StkPlWcsNCpy(m_ColumnName, COLUMN_NAME_SIZE, ColumnName, COLUMN_NAME_SIZE - 1);
	m_ColumnName[COLUMN_NAME_SIZE - 1] = L'\0';
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
