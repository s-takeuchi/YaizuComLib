#pragma once
#include "ColumnDef.h"
#include "stkdatadef.h"

class ColumnDefFloat : public ColumnDef
{
public:
	ColumnDefFloat();
	ColumnDefFloat(TCHAR*);
	~ColumnDefFloat();
};
