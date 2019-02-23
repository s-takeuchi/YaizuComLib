﻿#include <windows.h>
#include <stdio.h>
#include "..\..\src\stkdata\stkdata.h"
#include "..\..\src\stkdata\stkdataapi.h"


int Insert16383RecordsAtOneTime()
{
	printf("Automobile table (index:Int, name:WStr(50), type:Str(100), manufactular:WStr(20) is generated.");
	ColumnDefInt ColDefId(L"通し番号");
	ColumnDefWStr ColDefName(L"車名", 50);
	ColumnDefStr ColDefType(L"型番", 100);
	ColumnDefWStr ColDefManuf(L"製造会社", 20);
	TableDef TabDefPerson(L"自動車", 16383);
	TabDefPerson.AddColumnDef(&ColDefId);
	TabDefPerson.AddColumnDef(&ColDefName);
	TabDefPerson.AddColumnDef(&ColDefType);
	TabDefPerson.AddColumnDef(&ColDefManuf);
	if (CreateTable(&TabDefPerson) != 0) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");


	printf("16383 records can be added by one time invocation of InsertRecord for automobile table using 16383 connected records.");
	ColumnData *ColDat[4];
	RecordData *TopRecDat;
	RecordData *PrvRecDat;
	RecordData *CurRecDat;
	for (int i = 0; i < 16383; i++) {
		ColDat[0] = new ColumnDataInt(L"通し番号", i);
		ColDat[1] = new ColumnDataWStr(L"車名", L"スーパーチャージャー付2400cc 2シーターオープンカー ドーロスター VS");
		ColDat[2] = new ColumnDataStr(L"型番", "ABCDEFGHIJABCDEFGHIJABCDEFGHIJ01234567890123456789012345678901234567890123456789");
		ColDat[3] = new ColumnDataWStr(L"製造会社", L"Mazzzzzzzda自動車工業");
		CurRecDat = new RecordData(L"自動車", ColDat, 4);
		if (i == 0) {
			TopRecDat = CurRecDat;
		}
		if (i >= 1) {
			PrvRecDat->SetNextRecord(CurRecDat);
		}
		PrvRecDat = CurRecDat;
	}
	LockTable(L"自動車", 2);
	if (InsertRecord(TopRecDat) != 0) {
		delete TopRecDat;
		printf("...[NG]\r\n");
		return -1;
	}
	UnlockTable(L"自動車");
	delete TopRecDat;
	printf("...[OK]\r\n");


	printf("GetTableVersion(particular table name) returns 16383.");
	if (GetTableVersion(L"自動車") != 16383) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");


	printf("Connected 16383 records can be acquired from 16383 records which are added using GetRecord(record specification).");
	int Count = 0;
	ColumnData *ColDatTake[1];
	ColDatTake[0] = new ColumnDataStr(L"型番", "ABCDEFGHIJABCDEFGHIJABCDEFGHIJ01234567890123456789012345678901234567890123456789");
	RecordData RecDatTake(L"自動車", ColDatTake, 1);
	RecordData *RecDatGet;
	LockTable(L"自動車", 1);
	RecDatGet = GetRecord(&RecDatTake);
	UnlockTable(L"自動車");
	while (RecDatGet) {
		if (lstrcmp(((ColumnDataWStr*)(RecDatGet->GetColumn(1)))->GetValue(), L"スーパーチャージャー付2400cc 2シーターオープンカー ドーロスター VS") != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		Count++;
		RecDatGet = RecDatGet->GetNextRecord();
	}
	if (Count != 16383) {
		printf("...[NG]\r\n");
		return -1;
	}
	delete RecDatGet;
	printf("...[OK]\r\n");


	{
		printf("Same value '4' is returned from both GetColumnCount(table name) and GetColumnName(table name). 1st acquired column by GetColumnName shows index. 4th acquired column shows manufactular.");
		wchar_t ColNames[MAX_COLUMN_NUMBER][16];
		if (GetColumnCount(L"自動車") != 4) {
			printf("...[NG]\r\n");
			return -1;
		}
		if (GetColumnName(L"自動車", ColNames) != 4) {
			printf("...[NG]\r\n");
			return -1;
		}
		if (lstrcmp(ColNames[0], L"通し番号") != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		if (lstrcmp(ColNames[3], L"製造会社") != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}


	{
		printf("Both GetColumnCount(L\"Automobile\") and GetColumnName(L\"Automobile\") return -1.");
		wchar_t ColNames[MAX_COLUMN_NUMBER][16];
		if (GetColumnCount(L"自転車") != -1) {
			printf("...[NG]\r\n");
			return -1;
		}
		if (GetColumnName(L"自転車", ColNames) != -1) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}

	{
		printf("Change values of records whose ids are odd number. Connected records are passed to UpdateRecord.");
		ColumnData *ColDat[4];
		RecordData *TopRecDat;
		RecordData *PrvRecDat;
		RecordData *CurRecDat;
		ColDat[0] = new ColumnDataInt(L"通し番号", 100);
		ColDat[1] = new ColumnDataWStr(L"車名", L"日本国内閣総理大臣専用クーペ");
		ColDat[2] = new ColumnDataStr(L"型番", "So-Ri");
		ColDat[3] = new ColumnDataWStr(L"製造会社", L"内閣府");
		CurRecDat = new RecordData(L"自動車", ColDat, 4);
		for (int i = 1; i < 16383; i += 2) {
			ColumnData* ColDatSch[1];
			ColDatSch[0] = new ColumnDataInt(L"通し番号", i);
			RecordData* RecDatSch = new RecordData(L"自動車", ColDatSch, 1);
			if (i == 1) {
				TopRecDat = RecDatSch;
			}
			if (i >= 2) {
				PrvRecDat->SetNextRecord(RecDatSch);
			}
			PrvRecDat = RecDatSch;
		}
		LockTable(L"自動車", 2);
		if (UpdateRecord(TopRecDat, CurRecDat) != 0) {
			UnlockTable(L"自動車");
			delete TopRecDat;
			delete CurRecDat;
			printf("...[NG]\r\n");
			return -1;
		}
		UnlockTable(L"自動車");
		delete TopRecDat;
		delete CurRecDat;
		printf("...[OK]\r\n");


		printf("Records which are acquired by GetRecord(L\"Automobile\") contain two record types which records have even index number and odd index number.");
		int Count = 0;
		RecordData *RecDatGet;
		LockTable(L"自動車", 1);
		RecDatGet = GetRecord(L"自動車");
		UnlockTable(L"自動車");
		while (RecDatGet) {
			wchar_t *ColName = ((ColumnDataWStr*)(RecDatGet->GetColumn(1)))->GetValue();
			if (Count % 2 == 0) {
				if (lstrcmp(ColName, L"スーパーチャージャー付2400cc 2シーターオープンカー ドーロスター VS") != 0) {
					printf("...[NG]\r\n");
					return -1;
				}
			} else {
				if (lstrcmp(ColName, L"日本国内閣総理大臣専用クーペ") != 0) {
					printf("...[NG]\r\n");
					return -1;
				}
			}
			Count++;
			RecDatGet = RecDatGet->GetNextRecord();
		}
		printf("...[OK]\r\n");


		printf("GetRecord(L\"Automibile\") returns 16383 records and GetNumOfRecords() also returns 16383.");
		if (Count != 16383 || GetNumOfRecords(L"自動車") != 16383) {
			printf("...[NG]\r\n");
			delete RecDatGet;
			return -1;
		}
		delete RecDatGet;
		printf("...[OK]\r\n");
	}

	{
		printf("Delete records that manufacturer is government. The search criteria is invalid hence GetNumOfRecords returns 16383 after DeleteRecordexecution.");
		ColumnData *ColDat[1];
		ColDat[0] = new ColumnDataWStr(L"製造会社", L"内閣府X");
		RecordData *RecDat = new RecordData(L"自動車", ColDat, 1);
		LockTable(L"自動車", LOCK_EXCLUSIVE);
		if (DeleteRecord(RecDat) != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		UnlockTable(L"自動車");
		delete RecDat;
		if (GetNumOfRecords(L"自動車") != 16383) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}


	{
		printf("Delete records which contain \"Automobile\" records and \"manufacturer\". GetNumOfRecords shows 8192 after DeleteRecord execution.");
		ColumnData *ColDat[1];
		ColDat[0] = new ColumnDataWStr(L"製造会社", L"内閣府");
		RecordData *RecDat = new RecordData(L"自動車", ColDat, 1);
		LockTable(L"自動車", LOCK_EXCLUSIVE);
		if (DeleteRecord(RecDat) != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		UnlockTable(L"自動車");
		delete RecDat;
		if (GetNumOfRecords(L"自動車") != 8192) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}

	{
		printf("GetMaxNumOfRecords(L\"Automobile\") returns 16383.");
		if (GetMaxNumOfRecords(L"自動車") != 16383) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}


	{
		printf("Both GetNumOfRecords(\"Automobile\") and GetMaxNumOfRecords(\"Automobile\") returns -1 after execution of DeleteTable(\"Automobile\") that the table contains 8192 records.");
		if (DeleteTable(L"自動車") != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		if (GetMaxNumOfRecords(L"自動車") != -1) {
			printf("...[NG]\r\n");
			return -1;
		}
		if (GetNumOfRecords(L"自動車") != -1) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}


	return 0;
}
