#pragma once
#include <windows.h>
#include "ColumnDef.h"
#include "stkdatadef.h"

class ColumnDefStr : public ColumnDef
{
private:
	int m_MaxLength;

public:
	ColumnDefStr();
	ColumnDefStr(TCHAR*, int);
	~ColumnDefStr();

	// Set maximum length of string
	void SetMaxLength(int MaxLength);
	// Get maximum length of string
	int GetMaxLength();
};
