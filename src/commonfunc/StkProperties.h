#pragma once

class StkProperties  
{
private:
	class Impl;
	Impl* pImpl;

public:
	StkProperties();
	virtual ~StkProperties();

	int GetProperties(wchar_t*);
	int GetPropertyInt(char*, int*);
	int GetPropertyStr(char*, char[256]);
};
