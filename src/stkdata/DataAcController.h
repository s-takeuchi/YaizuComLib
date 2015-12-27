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
	TCHAR  m_TableName[MAX_TABLE_NUMBER][TABLE_NAME_SIZE];
	LPVOID m_TableAddr[MAX_TABLE_NUMBER];
	SIZE_T m_TableSize[MAX_TABLE_NUMBER];
	int    m_TableLock[MAX_TABLE_NUMBER];
	int    m_TableVer[MAX_TABLE_NUMBER];

	SIZE_T m_RecordSize[MAX_TABLE_NUMBER];
	int    m_RecordCount[MAX_TABLE_NUMBER];

	TCHAR  m_ColumnName[MAX_TABLE_NUMBER][MAX_COLUMN_NUMBER][COLUMN_NAME_SIZE];
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
	int SearchTable(TCHAR*);
	// Get column index
	int GetColumnIndex(TCHAR*, TCHAR*);
	// Check and change characters
	void CheckAndChangeChar(char* Start, char* End);

	// Sort records
	int SortRecord(TCHAR*, TCHAR*, BOOL);
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
	int CommitDataArea(TCHAR*);
	// Decommit the reserved area
	int DecommitDataArea(TCHAR*);

	// Common record operation
	RecordData* CommonRecordOperation(int, RecordData*, RecordData*, int*);
	// Increment table version
	void IncrementTableVersion(int);

	int AutoSaveInterval;
	ULONG AutoSaveThreadId;
	TCHAR AutoSaveFilePath[256];
	DWORD AutoSaveThreadProc();
	static DWORD WINAPI AutoSaveThreadProcDummy(LPVOID);

public:
	// Get this instance
	static DataAcController* GetInstance();

	// Create table into main area
	int CreateTable(TableDef*);
	// Delete table from main area
	int DeleteTable(TCHAR*);

	// Lock table
	int LockTable(TCHAR*, int);
	int LockAllTable(int);
	// Unlock table
	int UnlockTable(TCHAR*);
	int UnlockAllTable();

	// Insert Record
	int InsertRecord(RecordData*);
	// Delete all records of the specified table
	int DeleteRecord(TCHAR*);
	// Delete records which are matched by the specified condition
	int DeleteRecord(RecordData*);
	// Update Record
	int UpdateRecord(RecordData*, RecordData*);
	// Get all records of the specified table
	RecordData* GetRecord(TCHAR*);
	// Get records which are matched by the specified condition
	RecordData* GetRecord(RecordData*);
	// Clear specified response data
	void ClearRecordData(RecordData*);

	// Sort records
	int AzSortRecord(TCHAR*, TCHAR*);
	int ZaSortRecord(TCHAR*, TCHAR*);

	// Get column count
	int GetColumnCount(TCHAR*);
	// Get column size
	int GetColumnSize(TCHAR*, TCHAR*);
	//Get column name
	int GetColumnName(TCHAR*, TCHAR[MAX_COLUMN_NUMBER][COLUMN_NAME_SIZE]);
	// Get column type
	int GetColumnType(TCHAR*, TCHAR*);
	// Get table count
	int GetTableCount();
	//Get table name
	int GetTableName(TCHAR [MAX_TABLE_NUMBER][TABLE_NAME_SIZE]);
	// Get table size
	int GetTableSize(TCHAR*);
	//Get max number of records
	int GetMaxNumOfRecords(TCHAR*);
	//Get number of records
	int GetNumOfRecords(TCHAR*);
	// Get table version
	int GetTableVersion(TCHAR*);

	// Save data
	int SaveData(TCHAR*);
	// Load data
	int LoadData(TCHAR*);
	// Auto save data
	int AutoSave(TCHAR*, int, BOOL);
};
