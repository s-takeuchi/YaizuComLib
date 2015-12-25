#pragma once

class StkProperties  
{
private:
	class Impl;
	Impl* pImpl;

public:
	StkProperties();
	~StkProperties();

	int GetProperties(TCHAR*);
	int GetPropertyInt(char*, int*);
	int GetPropertyStr(char*, char[256]);
};
