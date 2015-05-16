#pragma once
#include "ColumnDef.h"
#include "stkdatadef.h"

class ColumnDefInt : public ColumnDef
{
public:
	ColumnDefInt();
	ColumnDefInt(TCHAR*);
	~ColumnDefInt();
};
