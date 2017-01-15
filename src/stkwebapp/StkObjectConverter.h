#pragma once
#include "..\commonfunc\StkObject.h"

class StkObjectConverter
{
private:
	class Impl;
	Impl* pImpl;

public:
	StkObjectConverter(int*, int, TCHAR*, int);
	virtual ~StkObjectConverter();
};
