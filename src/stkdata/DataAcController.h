﻿#pragma once
#include <mutex>
#include "stkdata.h"
#include "../stkpl/StkPl.h"

#define OPE_GET    1
#define OPE_DELETE 2
#define OPE_UPDATE 3

class DataAcController
{
public:
	// This instance
	static DataAcController* StkDataInstance;

private:
	wchar_t  m_TableName[MAX_TABLE_NUMBER][TABLE_NAME_SIZE];
	char* m_TableAddr[MAX_TABLE_NUMBER];
	size_t m_TableSize[MAX_TABLE_NUMBER];
	int    m_TableLock[MAX_TABLE_NUMBER];
	int    m_TableVer[MAX_TABLE_NUMBER];

	size_t m_RecordSize[MAX_TABLE_NUMBER];
	int    m_RecordCount[MAX_TABLE_NUMBER];

	wchar_t  m_ColumnName[MAX_TABLE_NUMBER][MAX_COLUMN_NUMBER][COLUMN_NAME_SIZE];
	int    m_ColumnOffset[MAX_TABLE_NUMBER][MAX_COLUMN_NUMBER];
	int    m_ColumnType[MAX_TABLE_NUMBER][MAX_COLUMN_NUMBER];
	int    m_ColumnSize[MAX_TABLE_NUMBER][MAX_COLUMN_NUMBER];
	int    m_ColumnCount[MAX_TABLE_NUMBER];

	// Offset of target column for SortCompare() function
	static thread_local int m_CompareColOffset;
	// Type for target column for SortCompare() function
	static thread_local int m_CompareColType;
	// Size for target column for SortCompare() function
	static thread_local int m_CompareColSize;

	// Critical Section
	std::mutex Cs4Lock;
	std::mutex Cs4AutoSave;

private:
	// Constructor
	DataAcController();
	// Destructor
	~DataAcController();

	// Clear parameters
	void ClearParameter(int);
	// Get free table ID which was found first.
	int GetFreeTableId();
	// Search the table which has the specified table name.
	int SearchTable(const wchar_t*);
	// Get column index
	int GetColumnIndex(const wchar_t*, const wchar_t*);
	// Check and change characters
	void CheckAndChangeChar(char* Start, char* End);

	// Sort records
	int SortRecord(const wchar_t*, const wchar_t*, bool);
	// Compare function for quick sort
	static int AzSortCompare(const void*, const void*);
	static int ZaSortCompare(const void*, const void*);

	// Allocate data area of the specified table ID
	int AllocDataArea(size_t, int);
	// Release data area of the specified table ID
	int FreeDataArea(int);

	// Common record operation
	RecordData* CommonRecordOperation(int, RecordData*, RecordData*, int*);
	// Increment table version
	void IncrementTableVersion(int);

	int AutoSaveInterval;
	wchar_t AutoSaveFilePath[256];
	void AutoSaveThreadProc();
	static void AutoSaveThreadProcDummy(void*);

public:
	// Get this instance
	static DataAcController* GetInstance();

	// Create table into main area
	int CreateTable(TableDef*);
	// Delete table from main area
	int DeleteTable(const wchar_t*);
	// Rename table name
	int RenameTable(const wchar_t*, const wchar_t*);

	// Lock table
	int LockTable(const wchar_t*, int);
	int LockAllTable(int);
	// Unlock table
	int UnlockTable(const wchar_t*);
	int UnlockAllTable();

	// Insert Record
	int InsertRecord(RecordData*);
	// Delete all records of the specified table
	int DeleteRecord(const wchar_t*);
	// Delete records which are matched by the specified condition
	int DeleteRecord(RecordData*);
	// Update Record
	int UpdateRecord(RecordData*, RecordData*);
	// Get all records of the specified table
	RecordData* GetRecord(const wchar_t*);
	// Get records which are matched by the specified condition
	RecordData* GetRecord(RecordData*);

	// Sort records
	int AzSortRecord(const wchar_t*, const wchar_t*);
	int ZaSortRecord(const wchar_t*, const wchar_t*);

	// Get column count
	int GetColumnCount(const wchar_t*);
	// Get column size
	int GetColumnSize(const wchar_t*, const wchar_t*);
	//Get column name
	int GetColumnName(const wchar_t*, wchar_t[MAX_COLUMN_NUMBER][COLUMN_NAME_SIZE]);
	// Get column type
	int GetColumnType(const wchar_t*, const wchar_t*);
	// Get table count
	int GetTableCount();
	//Get table name
	int GetTableName(wchar_t [MAX_TABLE_NUMBER][TABLE_NAME_SIZE]);
	// Get table size
	int GetTableSize(const wchar_t*);
	//Get max number of records
	int GetMaxNumOfRecords(const wchar_t*);
	//Get number of records
	int GetNumOfRecords(const wchar_t*);
	// Get table version
	int GetTableVersion(const wchar_t*);

	// Save data
	int SaveData(const wchar_t*);
	// Load data
	int LoadData(const wchar_t*);
	// Auto save data
	int AutoSave(const wchar_t*, int, bool);
};
