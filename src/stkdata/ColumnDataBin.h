#pragma once
#include <windows.h>
#include "ColumnData.h"
#include "stkdatadef.h"

class ColumnDataBin : public ColumnData
{
private:
	BYTE* m_Value;

public:
	ColumnDataBin(TCHAR*, BYTE*, int);
	virtual ~ColumnDataBin();

	BYTE* GetValue();
};
