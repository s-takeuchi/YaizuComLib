#pragma once
#include <windows.h>
#include "ColumnData.h"
#include "stkdatadef.h"

class ColumnDataWStr : public ColumnData
{
private:
	TCHAR m_Value[256];

public:
	ColumnDataWStr(TCHAR*, TCHAR*);
	virtual ~ColumnDataWStr();

	TCHAR* GetValue();
};
