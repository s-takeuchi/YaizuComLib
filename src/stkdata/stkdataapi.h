#pragma once

// Lock type
#define LOCK_FREE       0
#define LOCK_SHARE      1
#define LOCK_EXCLUSIVE  2


int CreateTable(TableDef*);
int DeleteTable(const wchar_t*);
int RenameTable(const wchar_t*, const wchar_t*);

int LockTable(const wchar_t*, int);
int LockAllTable(int);
int UnlockTable(const wchar_t*);
int UnlockAllTable();

int InsertRecord(RecordData*);
int DeleteRecord(const wchar_t*);
int DeleteRecord(RecordData*);
int UpdateRecord(RecordData*, RecordData*);
RecordData* GetRecord(const wchar_t*);
RecordData* GetRecord(RecordData*);

int AzSortRecord(const wchar_t*, const wchar_t*);
int ZaSortRecord(const wchar_t*, const wchar_t*);

int GetColumnCount(const wchar_t*);
int GetColumnSize(const wchar_t*, const wchar_t*);
int GetColumnName(const wchar_t*, wchar_t[MAX_COLUMN_NUMBER][COLUMN_NAME_SIZE]);
int GetColumnType(const wchar_t*, const wchar_t*);
int GetTableCount();
int GetTableName(wchar_t[MAX_TABLE_NUMBER][TABLE_NAME_SIZE]);
int GetTableSize(const wchar_t*);
int GetMaxNumOfRecords(const wchar_t*);
int GetNumOfRecords(const wchar_t*);
int GetTableVersion(const wchar_t*);

int SaveData(const wchar_t*);
int LoadData(const wchar_t*);
int AutoSave(const wchar_t*, int, bool);

