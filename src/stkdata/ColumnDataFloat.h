#pragma once
#include <windows.h>
#include "ColumnData.h"
#include "stkdatadef.h"

class ColumnDataFloat : public ColumnData
{
private:
	float m_Value;

public:
	ColumnDataFloat(TCHAR*, float);
	virtual ~ColumnDataFloat();

	float GetValue();
};
