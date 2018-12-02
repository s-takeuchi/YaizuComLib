#pragma once
#include <windows.h>

// Lock type
#define LOCK_FREE       0
#define LOCK_SHARE      1
#define LOCK_EXCLUSIVE  2


int CreateTable(TableDef*);
int DeleteTable(wchar_t*);
int RenameTable(wchar_t*, wchar_t*);

int LockTable(wchar_t*, int);
int LockAllTable(int);
int UnlockTable(wchar_t*);
int UnlockAllTable();

int InsertRecord(RecordData*);
int DeleteRecord(wchar_t*);
int DeleteRecord(RecordData*);
int UpdateRecord(RecordData*, RecordData*);
RecordData* GetRecord(wchar_t*);
RecordData* GetRecord(RecordData*);

int AzSortRecord(wchar_t*, wchar_t*);
int ZaSortRecord(wchar_t*, wchar_t*);

int GetColumnCount(wchar_t*);
int GetColumnSize(wchar_t*, wchar_t*);
int GetColumnName(wchar_t*, wchar_t[MAX_COLUMN_NUMBER][COLUMN_NAME_SIZE]);
int GetColumnType(wchar_t*, wchar_t*);
int GetTableCount();
int GetTableName(wchar_t [MAX_TABLE_NUMBER][TABLE_NAME_SIZE]);
int GetTableSize(wchar_t*);
int GetMaxNumOfRecords(wchar_t*);
int GetNumOfRecords(wchar_t*);
int GetTableVersion(wchar_t*);

int SaveData(wchar_t*);
int LoadData(wchar_t*);
int AutoSave(wchar_t*, int, bool);

