#include "../../../YaizuComLib/src/stkpl/StkPl.h"
#include "FilteringCondition.h"

FilteringCondition::FilteringCondition(wchar_t ColumnNameWk[COLUMNNAME_LENGTH], int OpeTypeWk, wchar_t ColumnValueWk[COLUMNVAL_LENGTH])
{
	StkPlWcsCpy(ColumnName, COLUMNNAME_LENGTH, ColumnNameWk);
	OpeType = OpeTypeWk;
	StkPlWcsCpy(ColumnVal, COLUMNVAL_LENGTH, ColumnValueWk);
	Next = NULL;
}

FilteringCondition::~FilteringCondition()
{
	if (Next) {
		delete Next;
	}
}

FilteringCondition* FilteringCondition::GetNext()
{
	return Next;
}

void FilteringCondition::SetNext(FilteringCondition* FilCond)
{
	Next = FilCond;
}

wchar_t* FilteringCondition::GetColumnName()
{
	return ColumnName;
}

int FilteringCondition::GetOpeType()
{
	return OpeType;
}

wchar_t* FilteringCondition::GetColumnVal()
{
	return ColumnVal;
}
