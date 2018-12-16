#pragma once
#include <windows.h>
#include "stkdata.h"

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
	LPVOID m_TableAddr[MAX_TABLE_NUMBER];
	SIZE_T m_TableSize[MAX_TABLE_NUMBER];
	int    m_TableLock[MAX_TABLE_NUMBER];
	int    m_TableVer[MAX_TABLE_NUMBER];

	SIZE_T m_RecordSize[MAX_TABLE_NUMBER];
	int    m_RecordCount[MAX_TABLE_NUMBER];

	wchar_t  m_ColumnName[MAX_TABLE_NUMBER][MAX_COLUMN_NUMBER][COLUMN_NAME_SIZE];
	int    m_ColumnOffset[MAX_TABLE_NUMBER][MAX_COLUMN_NUMBER];
	int    m_ColumnType[MAX_TABLE_NUMBER][MAX_COLUMN_NUMBER];
	int    m_ColumnSize[MAX_TABLE_NUMBER][MAX_COLUMN_NUMBER];
	int    m_ColumnCount[MAX_TABLE_NUMBER];

	// Offset of target column for SortCompare() function
	static __declspec(thread) int m_CompareColOffset;
	// Type for target column for SortCompare() function
	static __declspec(thread) int m_CompareColType;
	// Size for target column for SortCompare() function
	static __declspec(thread) int m_CompareColSize;

	// Critical Section
	CRITICAL_SECTION Cs4Lock;
	CRITICAL_SECTION Cs4AutoSave;

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
	int SearchTable(wchar_t*);
	// Get column index
	int GetColumnIndex(wchar_t*, wchar_t*);
	// Check and change characters
	void CheckAndChangeChar(char* Start, char* End);

	// Sort records
	int SortRecord(wchar_t*, wchar_t*, bool);
	// Compare function for quick sort
	static int AzSortCompare(const void*, const void*);
	static int ZaSortCompare(const void*, const void*);

	// Allocate data area of the specified table ID
	int AllocDataArea(SIZE_T, int);
	// Release data area of the specified table ID
	int FreeDataArea(int);
	// Commit the reserved area
	int CommitDataArea(int);
	// Decommit the reserved area
	int DecommitDataArea(int);
	// Commit the reserved area
	int CommitDataArea(wchar_t*);
	// Decommit the reserved area
	int DecommitDataArea(wchar_t*);

	// Common record operation
	RecordData* CommonRecordOperation(int, RecordData*, RecordData*, int*);
	// Increment table version
	void IncrementTableVersion(int);

	int AutoSaveInterval;
	ULONG AutoSaveThreadId;
	wchar_t AutoSaveFilePath[256];
	DWORD AutoSaveThreadProc();
	static DWORD WINAPI AutoSaveThreadProcDummy(LPVOID);

public:
	// Get this instance
	static DataAcController* GetInstance();

	// Create table into main area
	int CreateTable(TableDef*);
	// Delete table from main area
	int DeleteTable(wchar_t*);
	// Rename table name
	int RenameTable(wchar_t*, wchar_t*);

	// Lock table
	int LockTable(wchar_t*, int);
	int LockAllTable(int);
	// Unlock table
	int UnlockTable(wchar_t*);
	int UnlockAllTable();

	// Insert Record
	int InsertRecord(RecordData*);
	// Delete all records of the specified table
	int DeleteRecord(wchar_t*);
	// Delete records which are matched by the specified condition
	int DeleteRecord(RecordData*);
	// Update Record
	int UpdateRecord(RecordData*, RecordData*);
	// Get all records of the specified table
	RecordData* GetRecord(wchar_t*);
	// Get records which are matched by the specified condition
	RecordData* GetRecord(RecordData*);

	// Sort records
	int AzSortRecord(wchar_t*, wchar_t*);
	int ZaSortRecord(wchar_t*, wchar_t*);

	// Get column count
	int GetColumnCount(wchar_t*);
	// Get column size
	int GetColumnSize(wchar_t*, wchar_t*);
	//Get column name
	int GetColumnName(wchar_t*, wchar_t[MAX_COLUMN_NUMBER][COLUMN_NAME_SIZE]);
	// Get column type
	int GetColumnType(wchar_t*, wchar_t*);
	// Get table count
	int GetTableCount();
	//Get table name
	int GetTableName(wchar_t [MAX_TABLE_NUMBER][TABLE_NAME_SIZE]);
	// Get table size
	int GetTableSize(wchar_t*);
	//Get max number of records
	int GetMaxNumOfRecords(wchar_t*);
	//Get number of records
	int GetNumOfRecords(wchar_t*);
	// Get table version
	int GetTableVersion(wchar_t*);

	// Save data
	int SaveData(wchar_t*);
	// Load data
	int LoadData(wchar_t*);
	// Auto save data
	int AutoSave(wchar_t*, int, bool);
};
