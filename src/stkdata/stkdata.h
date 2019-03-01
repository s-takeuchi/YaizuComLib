#pragma once

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

// Comparison operator
#define COMP_EQUAL           0 // Equal to
#define COMP_NOT_EQUAL       1 // Not equal to
#define COMP_GE              2 // Greater than or equal to
#define COMP_GT              3 // Greater than
#define COMP_LE              4 // Less than or qeual to
#define COMP_LT              5 // Less than
#define COMP_CONTAIN        11 // Contain
#define COMP_NOT_CONTAIN    12 // Not contain

//
// Table definition

class ColumnDef
{
protected:
	wchar_t m_ColumnName[COLUMN_NAME_SIZE];
	int m_ColumnType;
public:
	ColumnDef();
	virtual ~ColumnDef();
	void SetColumnName(const wchar_t*);
	wchar_t* GetColumnName();
	void SetColumnType(int);
	int GetColumnType();
};

class ColumnDefInt : public ColumnDef
{
public:
	ColumnDefInt();
	ColumnDefInt(const wchar_t*);
	~ColumnDefInt();
};

class ColumnDefFloat : public ColumnDef
{
public:
	ColumnDefFloat();
	ColumnDefFloat(const wchar_t*);
	~ColumnDefFloat();
};

class ColumnDefStr : public ColumnDef
{
private:
	int m_MaxLength;
public:
	ColumnDefStr();
	ColumnDefStr(const wchar_t*, int);
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
	ColumnDefWStr(const wchar_t*, int);
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
	ColumnDefBin(const wchar_t*, int);
	~ColumnDefBin();

	void SetMaxLength(int MaxLength);
	int GetMaxLength();
};

class TableDef
{
private:
	wchar_t m_TableName[TABLE_NAME_SIZE];
	ColumnDef* m_Column[MAX_COLUMN_NUMBER];
	int m_NumberOfColumn;
	int m_MaxRecord;
public:
	TableDef();
	TableDef(const wchar_t*, int);
	virtual ~TableDef();
	void SetTableName(const wchar_t*);
	wchar_t* GetTableName();
	void SetMaxRecord(int);
	int GetMaxRecord();
	int AddColumnDef(ColumnDef*);
	int DeleteLastColumnDef();
	ColumnDef* GetColumnDef(int);
	int GetNumOfColumn();
};

//
// Data access

class ColumnData
{
protected:
	wchar_t m_ColumnName[COLUMN_NAME_SIZE];
	int m_ColumnType;
	int m_ComparisonOperator;
public:
	ColumnData();
	virtual ~ColumnData();
	void SetColumnName(const wchar_t*);
	wchar_t* GetColumnName();
	int GetColumnType();
	void SetComparisonOperator(int);
	int GetComparisonOperator();
};

class ColumnDataInt : public ColumnData
{
private:
	int m_Value;
public:
	ColumnDataInt(const wchar_t*, int);
	ColumnDataInt(const wchar_t*, int, int);
	virtual ~ColumnDataInt();

	int GetValue();
};

class ColumnDataFloat : public ColumnData
{
private:
	float m_Value;
public:
	ColumnDataFloat(const wchar_t*, float);
	ColumnDataFloat(const wchar_t*, float, int);
	virtual ~ColumnDataFloat();

	float GetValue();
};

class ColumnDataStr : public ColumnData
{
private:
	char m_Value[256];

public:
	ColumnDataStr(const wchar_t*, const char*);
	ColumnDataStr(const wchar_t*, const char*, int);
	virtual ~ColumnDataStr();

	char* GetValue();
};

class ColumnDataWStr : public ColumnData
{
private:
	wchar_t m_Value[256];

public:
	ColumnDataWStr(const wchar_t*, const wchar_t*);
	ColumnDataWStr(const wchar_t*, const wchar_t*, int);
	virtual ~ColumnDataWStr();

	wchar_t* GetValue();
};

class ColumnDataBin : public ColumnData
{
private:
	unsigned char *m_Value;

public:
	ColumnDataBin(const wchar_t*, const unsigned char*, int);
	ColumnDataBin(const wchar_t*, const unsigned char*, int, int);
	virtual ~ColumnDataBin();

	unsigned char* GetValue();
};

class RecordData
{
private:
	int m_CurrentColumnNum;
	wchar_t m_TableName[TABLE_NAME_SIZE];
	ColumnData* m_ColumnData[MAX_COLUMN_NUMBER];
	RecordData* m_NextRecord;
public:
	RecordData();
	RecordData(const wchar_t TableName[TABLE_NAME_SIZE], ColumnData**, int);
	virtual ~RecordData();
	void AddColumn(ColumnData*);
	void DeleteColumn();
	ColumnData* GetColumn(int ColIndex);
	ColumnData* GetColumn(const wchar_t*);
	int GetColumnCount();
	void SetTableName(const wchar_t TableName[TABLE_NAME_SIZE]);
	wchar_t* GetTableName();
	void SetNextRecord(RecordData*);
	RecordData* GetNextRecord();
};
