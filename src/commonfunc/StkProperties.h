#pragma once

class StkProperties  
{
private:
	class Impl;
	Impl* pImpl;

public:
	StkProperties();
	virtual ~StkProperties();

	int GetProperties(const wchar_t*);
	int GetPropertyInt(const char*, int*);
	int GetPropertyStr(const char*, char[256]);
};
