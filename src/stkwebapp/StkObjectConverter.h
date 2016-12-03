#pragma once
#include "..\commonfunc\StkObject.h"

class StkObjectConverter
{
public:
	StkObjectConverter(int*, int, TCHAR*, int);
	~StkObjectConverter();
	void AllClose(int*, int);
	StkObject* RecvRequest(int);
	void SendResponse(StkObject*, int);
};
