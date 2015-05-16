#pragma once
#include <windows.h>
#include "ColumnData.h"
#include "stkdatadef.h"

class ColumnDataInt : public ColumnData
{
private:
	int m_Value;

public:
	ColumnDataInt(TCHAR*, int);
	virtual ~ColumnDataInt();

	int GetValue();
};
