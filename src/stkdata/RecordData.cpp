#include <windows.h>
#include <tchar.h>
#include "stkdata.h"

RecordData::RecordData()
{
	int Loop;
	for (Loop = 0; Loop < MAX_COLUMN_NUMBER; Loop++) {
		m_ColumnData[Loop] = NULL;
	}
	m_CurrentColumnNum = 0;
	lstrcpyn(m_TableName, _T(""), TABLE_NAME_SIZE);
	m_NextRecord = NULL;
}

RecordData::RecordData(TCHAR TableName[TABLE_NAME_SIZE], ColumnData** ColDat, int Cnt)
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
	lstrcpyn(m_TableName, TableName, TABLE_NAME_SIZE);
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

ColumnData* RecordData::GetColumn(TCHAR* ColumnName)
{
	for (int Loop = 0; Loop < m_CurrentColumnNum; Loop++) {
		ColumnData* ColDat = m_ColumnData[Loop];
		if (ColDat != NULL) {
			TCHAR* ColDatName = ColDat->GetColumnName();
			if (ColDatName != NULL) {
				if (lstrcmp(ColDatName, ColumnName) == 0) {
					return ColDat;
				}
			}
		}
	}
	return NULL;
}

void RecordData::SetTableName(TCHAR TableName[TABLE_NAME_SIZE])
{
	lstrcpyn(m_TableName, TableName, TABLE_NAME_SIZE);
}

TCHAR* RecordData::GetTableName()
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
