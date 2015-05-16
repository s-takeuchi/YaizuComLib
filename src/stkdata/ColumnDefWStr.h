#pragma once
#include <windows.h>
#include "ColumnDef.h"
#include "stkdatadef.h"

class ColumnDefWStr : public ColumnDef
{
private:
	int m_MaxLength;

public:
	ColumnDefWStr();
	ColumnDefWStr(TCHAR*, int);
	~ColumnDefWStr();

	// Set maximum length of string
	void SetMaxLength(int MaxLength);
	// Get maximum length of string
	int GetMaxLength();
};
