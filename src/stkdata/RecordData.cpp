#include "stkdata.h"
#include "../StkPl.h"

RecordData::RecordData()
{
	int Loop;
	for (Loop = 0; Loop < MAX_COLUMN_NUMBER; Loop++) {
		m_ColumnData[Loop] = NULL;
	}
	m_CurrentColumnNum = 0;
	StkPlWcsNCpy(m_TableName, TABLE_NAME_SIZE, L"", TABLE_NAME_SIZE - 1);
	m_TableName[TABLE_NAME_SIZE - 1] = L'\0';
	m_NextRecord = NULL;
}

RecordData::RecordData(const wchar_t TableName[TABLE_NAME_SIZE], ColumnData** ColDat, int Cnt)
{
	int Loop;
	m_CurrentColumnNum = 0;
	if (Cnt > MAX_COLUMN_NUMBER) {
		Cnt = MAX_COLUMN_NUMBER;
	}
	for (Loop = 0; Loop < Cnt; Loop++) {
		if (ColDat[Loop] != NULL) {
			m_ColumnData[Loop] = ColDat[Loop];
		} else {
			m_ColumnData[Loop] = NULL;
		}
		m_CurrentColumnNum++;
	}
	for (; Loop < MAX_COLUMN_NUMBER; Loop++) {
		m_ColumnData[Loop] = NULL;
	}
	StkPlWcsNCpy(m_TableName, TABLE_NAME_SIZE, TableName, TABLE_NAME_SIZE - 1);
	m_TableName[TABLE_NAME_SIZE - 1] = L'\0';
	m_NextRecord = NULL;
}

RecordData::~RecordData()
{
	int Loop;
	int ColCnt;
	RecordData* NextRd = GetNextRecord();
	if (NextRd != NULL) {
		delete NextRd;
	}
	ColCnt = GetColumnCount();
	for (Loop = ColCnt - 1; Loop >= 0 ; Loop--) {
		ColumnData* ColDat = GetColumn(Loop);
		DeleteColumn();
		if (ColDat != NULL) {
			delete ColDat;
		}
	}
}

void RecordData::AddColumn(ColumnData* ColDat)
{
	if (m_CurrentColumnNum < MAX_COLUMN_NUMBER) {
		m_ColumnData[m_CurrentColumnNum] = ColDat;
		m_CurrentColumnNum++;
	}
}

void RecordData::DeleteColumn()
{
	m_ColumnData[m_CurrentColumnNum] = NULL;
	m_CurrentColumnNum--;
}

ColumnData* RecordData::GetColumn(int ColIndex)
{
	if (ColIndex >= m_CurrentColumnNum || ColIndex < 0) {
		return NULL;
	}
	return m_ColumnData[ColIndex];
}

ColumnData* RecordData::GetColumn(const wchar_t* ColumnName)
{
	for (int Loop = 0; Loop < m_CurrentColumnNum; Loop++) {
		ColumnData* ColDat = m_ColumnData[Loop];
		if (ColDat != NULL) {
			wchar_t* ColDatName = ColDat->GetColumnName();
			if (ColDatName != NULL) {
				if (StkPlWcsCmp(ColDatName, ColumnName) == 0) {
					return ColDat;
				}
			}
		}
	}
	return NULL;
}

void RecordData::SetTableName(const wchar_t TableName[TABLE_NAME_SIZE])
{
	StkPlWcsNCpy(m_TableName, TABLE_NAME_SIZE, TableName, TABLE_NAME_SIZE - 1);
	m_TableName[TABLE_NAME_SIZE - 1] = L'\0';
}

wchar_t* RecordData::GetTableName()
{
	return m_TableName;
}

void RecordData::SetNextRecord(RecordData* RecDat)
{
	m_NextRecord = RecDat;
}

RecordData* RecordData::GetNextRecord()
{
	return m_NextRecord;
}

int RecordData::GetColumnCount()
{
	return m_CurrentColumnNum;
}
