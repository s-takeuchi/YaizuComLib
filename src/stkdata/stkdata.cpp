﻿#include "DataAcController.h"
#include "stkdata.h"
#include "stkdataapi.h"


int CreateTable(TableDef* TabDef)
{
	return DataAcController::GetInstance()->CreateTable(TabDef);
}

int DeleteTable(const wchar_t* TableName)
{
	return DataAcController::GetInstance()->DeleteTable(TableName);
}

int RenameTable(const wchar_t* OldName, const wchar_t* NewName)
{
	return DataAcController::GetInstance()->RenameTable(OldName, NewName);
}

int LockTable(const wchar_t* TableName, int Type)
{
	return DataAcController::GetInstance()->LockTable(TableName, Type);
}

int LockAllTable(int Type)
{
	return DataAcController::GetInstance()->LockAllTable(Type);
}

int UnlockTable(const wchar_t* TableName)
{
	return DataAcController::GetInstance()->UnlockTable(TableName);
}

int UnlockAllTable()
{
	return DataAcController::GetInstance()->UnlockAllTable();
}

int InsertRecord(RecordData* RecDat)
{
	return DataAcController::GetInstance()->InsertRecord(RecDat);
}

int DeleteRecord(const wchar_t* TableName)
{
	return DataAcController::GetInstance()->DeleteRecord(TableName);
}

int DeleteRecord(RecordData* RecDat)
{
	return DataAcController::GetInstance()->DeleteRecord(RecDat);
}

int UpdateRecord(RecordData* RecDat, RecordData* UdRecDat)
{
	return DataAcController::GetInstance()->UpdateRecord(RecDat, UdRecDat);
}

RecordData* GetRecord(const wchar_t* TableName)
{
	return DataAcController::GetInstance()->GetRecord(TableName);
}

RecordData* GetRecord(RecordData* RecDat)
{
	return DataAcController::GetInstance()->GetRecord(RecDat);
}

int AzSortRecord(const wchar_t* TableName, const wchar_t* ColumnName)
{
	return DataAcController::GetInstance()->AzSortRecord(TableName, ColumnName);
}

int ZaSortRecord(const wchar_t* TableName, const wchar_t* ColumnName)
{
	return DataAcController::GetInstance()->ZaSortRecord(TableName, ColumnName);
}

int SaveData(const wchar_t* FilePath)
{
	return DataAcController::GetInstance()->SaveData(FilePath);
}
int LoadData(const wchar_t* FilePath)
{
	return DataAcController::GetInstance()->LoadData(FilePath);
}

int AutoSave(const wchar_t* FilePath, int Sec, bool Flag)
{
	return DataAcController::GetInstance()->AutoSave(FilePath, Sec, Flag);
}

int GetMaxNumOfRecords(const wchar_t* TableName)
{
	return DataAcController::GetInstance()->GetMaxNumOfRecords(TableName);
}

int GetNumOfRecords(const wchar_t* TableName)
{
	return DataAcController::GetInstance()->GetNumOfRecords(TableName);
}

int GetColumnCount(const wchar_t* TableName)
{
	return DataAcController::GetInstance()->GetColumnCount(TableName);
}

int GetColumnSize(const wchar_t* TableName, const wchar_t* ColumnName)
{
	return DataAcController::GetInstance()->GetColumnSize(TableName, ColumnName);
}

int GetColumnName(const wchar_t* TableName, wchar_t ColumnNames[MAX_COLUMN_NUMBER][COLUMN_NAME_SIZE])
{
	return DataAcController::GetInstance()->GetColumnName(TableName, ColumnNames);
}

int GetColumnType(const wchar_t* TableName, const wchar_t* ColumnName)
{
	return DataAcController::GetInstance()->GetColumnType(TableName, ColumnName);
}

int GetTableCount()
{
	return DataAcController::GetInstance()->GetTableCount();
}

int GetTableName(wchar_t TableNames[MAX_TABLE_NUMBER][TABLE_NAME_SIZE])
{
	return DataAcController::GetInstance()->GetTableName(TableNames);
}

int GetTableSize(const wchar_t* TableName)
{
	return DataAcController::GetInstance()->GetTableSize(TableName);
}

int GetTableVersion(const wchar_t* TableName)
{
	return DataAcController::GetInstance()->GetTableVersion(TableName);
}
