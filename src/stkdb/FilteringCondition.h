#pragma once

class FilteringCondition
{
public:
	static const int COLUMNNAME_LENGTH = 65;
	static const int COLUMNVAL_LENGTH = 64;

private:
	wchar_t ColumnName[COLUMNNAME_LENGTH];
	int OpeType;
	wchar_t ColumnVal[COLUMNVAL_LENGTH];
	FilteringCondition* Next;

public:
	FilteringCondition(wchar_t[COLUMNNAME_LENGTH], int, wchar_t[COLUMNVAL_LENGTH]);
	~FilteringCondition();
	FilteringCondition* GetNext();
	void SetNext(FilteringCondition*);
	wchar_t* GetColumnName();
	int GetOpeType();
	wchar_t* GetColumnVal();
};
