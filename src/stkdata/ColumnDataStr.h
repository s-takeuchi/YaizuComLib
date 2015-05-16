#pragma once
#include <windows.h>
#include "ColumnData.h"
#include "stkdatadef.h"

class ColumnDataStr : public ColumnData
{
private:
	char m_Value[256];

public:
	ColumnDataStr(TCHAR*, char*);
	virtual ~ColumnDataStr();

	char* GetValue();
};
