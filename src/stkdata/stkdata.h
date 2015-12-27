#pragma once
#include <windows.h>

// Max number
#define MAX_TABLE_NUMBER   16
#define MAX_COLUMN_NUMBER  32
#define MAX_RECORD         16384

// Lock type
#define LOCK_FREE       0
#define LOCK_SHARE      1
#define LOCK_EXCLUSIVE  2

// Column Type
#define COLUMN_TYPE_INT   0
#define COLUMN_TYPE_STR   1
#define COLUMN_TYPE_WSTR  2
#define COLUMN_TYPE_BIN   3
#define COLUMN_TYPE_FLOAT 4

// Size of name
#define TABLE_NAME_SIZE  16
#define COLUMN_NAME_SIZE 16

class ColumnDef
{
protected:
	TCHAR m_ColumnName[16];
	int m_ColumnType;
public:
	ColumnDef();
	virtual ~ColumnDef();
	void SetColumnName(TCHAR*);
	TCHAR* GetColumnName();
	void SetColumnType(int);
	int GetColumnType();
};

class ColumnDefInt : public ColumnDef
{
public:
	ColumnDefInt();
	ColumnDefInt(TCHAR*);
	~ColumnDefInt();
};

class ColumnDefFloat : public ColumnDef
{
public:
	ColumnDefFloat();
	ColumnDefFloat(TCHAR*);
	~ColumnDefFloat();
};

class ColumnDefStr : public ColumnDef
{
private:
	int m_MaxLength;
public:
	ColumnDefStr();
	ColumnDefStr(TCHAR*, int);
	~ColumnDefStr();
	void SetMaxLength(int MaxLength);
	int GetMaxLength();
};

class ColumnDefWStr : public ColumnDef
{
private:
	int m_MaxLength;

public:
	ColumnDefWStr();
	ColumnDefWStr(TCHAR*, int);
	~ColumnDefWStr();
	void SetMaxLength(int MaxLength);
	int GetMaxLength();
};

class ColumnDefBin : public ColumnDef
{
private:
	int m_MaxLength;

public:
	ColumnDefBin();
	ColumnDefBin(TCHAR*, int);
	~ColumnDefBin();

	void SetMaxLength(int MaxLength);
	int GetMaxLength();
};

class TableDef
{
private:
	TCHAR m_TableName[16];
	ColumnDef* m_Column[MAX_COLUMN_NUMBER];
	int m_NumberOfColumn;
	int m_MaxRecord;
public:
	TableDef();
	TableDef(TCHAR*, int);
	virtual ~TableDef();
	void SetTableName(TCHAR*);
	TCHAR* GetTableName();
	void SetMaxRecord(int);
	int GetMaxRecord();
	int AddColumnDef(ColumnDef*);
	int DeleteLastColumnDef();
	ColumnDef* GetColumnDef(int);
	int GetNumOfColumn();
};

class ColumnData
{
protected:
	TCHAR m_ColumnName[16];
	int m_ColumnType;
public:
	ColumnData();
	virtual ~ColumnData();
	void SetColumnName(TCHAR*);
	TCHAR* GetColumnName();
	int GetColumnType();
};

class ColumnDataInt : public ColumnData
{
private:
	int m_Value;
public:
	ColumnDataInt(TCHAR*, int);
	virtual ~ColumnDataInt();

	int GetValue();
};

class ColumnDataFloat : public ColumnData
{
private:
	float m_Value;
public:
	ColumnDataFloat(TCHAR*, float);
	virtual ~ColumnDataFloat();

	float GetValue();
};

class ColumnDataStr : public ColumnData
{
private:
	char m_Value[256];

public:
	ColumnDataStr(TCHAR*, char*);
	virtual ~ColumnDataStr();

	char* GetValue();
};

class ColumnDataWStr : public ColumnData
{
private:
	TCHAR m_Value[256];

public:
	ColumnDataWStr(TCHAR*, TCHAR*);
	virtual ~ColumnDataWStr();

	TCHAR* GetValue();
};

class ColumnDataBin : public ColumnData
{
private:
	BYTE *m_Value;

public:
	ColumnDataBin(TCHAR*, BYTE*, int);
	virtual ~ColumnDataBin();

	BYTE* GetValue();
};

class RecordData
{
private:
	int m_CurrentColumnNum;
	TCHAR m_TableName[16];
	ColumnData* m_ColumnData[MAX_COLUMN_NUMBER];
	RecordData* m_NextRecord;
public:
	RecordData();
	RecordData(TCHAR TableName[16], ColumnData**, int);
	virtual ~RecordData();
	void AddColumn(ColumnData*);
	void DeleteColumn();
	ColumnData* GetColumn(int ColIndex);
	ColumnData* GetColumn(TCHAR*);
	int GetColumnCount();
	void SetTableName(TCHAR TableName[16]);
	TCHAR* GetTableName();
	void SetNextRecord(RecordData*);
	RecordData* GetNextRecord();
};

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
void ClearRecordData(RecordData*);

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

