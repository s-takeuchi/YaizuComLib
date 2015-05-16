#pragma once
#include <windows.h>
#include "ColumnDef.h"
#include "stkdatadef.h"

class ColumnDefBin : public ColumnDef
{
private:
	int m_MaxLength;

public:
	ColumnDefBin();
	ColumnDefBin(TCHAR*, int);
	~ColumnDefBin();

	// Set maximum length of string
	void SetMaxLength(int MaxLength);
	// Get maximum length of string
	int GetMaxLength();
};
