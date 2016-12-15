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
	void AllClose(int*, int);
	StkObject* RecvRequest(int, int*);
	void SendResponse(StkObject*, int, int);
};
