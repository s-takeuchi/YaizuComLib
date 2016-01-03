#pragma once
#include <windows.h>

// Lock type
#define LOCK_FREE       0
#define LOCK_SHARE      1
#define LOCK_EXCLUSIVE  2


int CreateTable(TableDef*);
int DeleteTable(TCHAR*);

int LockTable(TCHAR*, int);
int LockAllTable(int);
int UnlockTable(TCHAR*);
int UnlockAllTable();

int InsertRecord(RecordData*);
int DeleteRecord(TCHAR*);
int DeleteRecord(RecordData*);
int UpdateRecord(RecordData*, RecordData*);
RecordData* GetRecord(TCHAR*);
RecordData* GetRecord(RecordData*);

int AzSortRecord(TCHAR*, TCHAR*);
int ZaSortRecord(TCHAR*, TCHAR*);

int GetColumnCount(TCHAR*);
int GetColumnSize(TCHAR*, TCHAR*);
int GetColumnName(TCHAR*, TCHAR[MAX_COLUMN_NUMBER][COLUMN_NAME_SIZE]);
int GetColumnType(TCHAR*, TCHAR*);
int GetTableCount();
int GetTableName(TCHAR [MAX_TABLE_NUMBER][TABLE_NAME_SIZE]);
int GetTableSize(TCHAR*);
int GetMaxNumOfRecords(TCHAR*);
int GetNumOfRecords(TCHAR*);
int GetTableVersion(TCHAR*);

int SaveData(TCHAR*);
int LoadData(TCHAR*);
int AutoSave(TCHAR*, int, BOOL);

