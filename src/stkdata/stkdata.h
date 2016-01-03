#pragma once
#include <windows.h>

// Max number
#define MAX_TABLE_NUMBER   16
#define MAX_COLUMN_NUMBER  32
#define MAX_RECORD         16384

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
	TCHAR m_ColumnName[COLUMN_NAME_SIZE];
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
	TCHAR m_TableName[TABLE_NAME_SIZE];
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
	TCHAR m_ColumnName[COLUMN_NAME_SIZE];
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
	TCHAR m_TableName[TABLE_NAME_SIZE];
	ColumnData* m_ColumnData[MAX_COLUMN_NUMBER];
	RecordData* m_NextRecord;
public:
	RecordData();
	RecordData(TCHAR TableName[TABLE_NAME_SIZE], ColumnData**, int);
	virtual ~RecordData();
	void AddColumn(ColumnData*);
	void DeleteColumn();
	ColumnData* GetColumn(int ColIndex);
	ColumnData* GetColumn(TCHAR*);
	int GetColumnCount();
	void SetTableName(TCHAR TableName[TABLE_NAME_SIZE]);
	TCHAR* GetTableName();
	void SetNextRecord(RecordData*);
	RecordData* GetNextRecord();
};
