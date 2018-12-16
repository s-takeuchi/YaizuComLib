#include <windows.h>
#include <stdio.h>
#include "..\..\src\stkdata\stkdata.h"
#include "..\..\src\stkdata\stkdataapi.h"

/*
SmallTable
・CreateTableでSmallTableテーブル(ID:Int) Max=1 を生成することができる
・最大レコード登録可能数=1のSmallTableに対して16383個の連結されたレコードをInsertRecordすると1レコードのみ追加される
・BigTable("ID"カラム = 1)を検索条件に指定しGetRecord（レコード指定）を実行するとNULLが返る
・SmallTable("ID"カラム = 0)を検索条件に指定しロック後にGetRecord（レコード指定）を実行するとRecordDataが返る。ただしロック無しで実行した場合NULLが返る
・GetNumOfRecords("SmallTable")で取得したレコード件数は1件となる
・GetMaxNumOfRecords("SmallTable")で取得したレコード件数は1件となる
・GetNumOfRecords("BigTable")で取得したレコード件数は-1件となる
・GetMaxNumOfRecords("BigTable")で取得したレコード件数は-1件となる
・DeleteRecord(L"SmallTable")後，GetNumOfRecords(L"SmallTable")が0を返却する
・DeleteTable(L"SmallTable")が0を返却する
*/
int SmallTable()
{
	// CreateTableでSmallTableテーブル(ID:Int) Max=1 を生成することができる
	printf("CreateTableでSmallTableテーブル(ID:Int) Max=1 を生成することができる");
	ColumnDefInt ColDefId(L"ID");
	TableDef TabDefPerson(L"SmallTable", 1);
	TabDefPerson.AddColumnDef(&ColDefId);
	if (CreateTable(&TabDefPerson) != 0) {
		printf("...[NG]\r\n");
		return -1;
	}
	if (GetMaxNumOfRecords(L"SmallTable") != 1) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");

	// 最大レコード登録可能数=1のSmallTableに対して16383個の連結されたレコードをInsertRecordすると1レコードのみ追加される
	{
		printf("最大レコード登録可能数=1のSmallTableに対して16383個の連結されたレコードをInsertRecordすると1レコードのみ追加される");
		ColumnData *ColDat[4];
		RecordData *TopRecDat;
		RecordData *PrvRecDat;
		RecordData *CurRecDat;
		for (int i = 0; i < 16383; i++) {
			ColDat[0] = new ColumnDataInt(L"ID", i);
			CurRecDat = new RecordData(L"SmallTable", ColDat, 1);
			if (i == 0) {
				TopRecDat = CurRecDat;
			}
			if (i >= 1) {
				PrvRecDat->SetNextRecord(CurRecDat);
			}
			PrvRecDat = CurRecDat;
		}
		LockTable(L"SmallTable", 2);
		InsertRecord(TopRecDat);
		UnlockTable(L"SmallTable");
		delete TopRecDat;
		if (GetMaxNumOfRecords(L"SmallTable") != 1) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}

	// BigTable("ID"カラム = 1)を検索条件に指定しGetRecord（レコード指定）を実行するとNULLが返る
	{
		printf("BigTable(\"ID\"カラム = 1)を検索条件に指定しGetRecord（レコード指定）を実行するとNULLが返る");
		ColumnData *ColDatTake[1];
		ColDatTake[0] = new ColumnDataInt(L"ID", 1);
		RecordData RecDatTake(L"BigTable", ColDatTake, 1);
		RecordData *RecDatGet;
		LockTable(L"SmallTable", 1);
		RecDatGet = GetRecord(&RecDatTake);
		UnlockTable(L"SmallTable");
		if (RecDatGet != NULL) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}

	// SmallTable("ID"カラム = 0)を検索条件に指定しロック後にGetRecord（レコード指定）を実行するとRecordDataが返る。ただしロック無しで実行した場合NULLが返る
	{
		printf("SmallTable(\"ID\"カラム = 0)を検索条件に指定しロック後にGetRecord（レコード指定）を実行するとRecordDataが返る。ただしロック無しで実行した場合NULLが返る");
		ColumnData *ColDatTake[1];
		ColDatTake[0] = new ColumnDataInt(L"ID", 0);
		RecordData RecDatTake(L"SmallTable", ColDatTake, 1);
		RecordData *RecDatGet;
		LockTable(L"SmallTable", 1);
		RecDatGet = GetRecord(&RecDatTake);
		UnlockTable(L"SmallTable");
		if (RecDatGet == NULL) {
			printf("...[NG]\r\n");
			return -1;
		}
		delete RecDatGet;
		RecDatGet = GetRecord(&RecDatTake);
		if (RecDatGet != NULL) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}

	// GetNumOfRecords("SmallTable")で取得したレコード件数は1件となる
	{
		printf("GetNumOfRecords(\"SmallTable\")で取得したレコード件数は1件となる");
		if (GetNumOfRecords(L"SmallTable") != 1) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}

	// GetMaxNumOfRecords("SmallTable")で取得したレコード件数は1件となる
	{
		printf("GetMaxNumOfRecords(\"SmallTable\")で取得したレコード件数は1件となる");
		if (GetMaxNumOfRecords(L"SmallTable") != 1) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}

	// GetNumOfRecords("BigTable")で取得したレコード件数は-1件となる
	{
		printf("GetNumOfRecords(\"BigTable\")で取得したレコード件数は-1件となる");
		if (GetNumOfRecords(L"BigTable") != -1) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}

	// GetMaxNumOfRecords("BigTable")で取得したレコード件数は-1件となる
	{
		printf("GetMaxNumOfRecords(\"BigTable\")で取得したレコード件数は-1件となる");
		if (GetMaxNumOfRecords(L"BigTable") != -1) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}

	// DeleteRecord(L"SmallTable")後，GetNumOfRecords(L"SmallTable")が0を返却する
	{
		printf("DeleteRecord(L\"SmallTable\")後，GetNumOfRecords(L\"SmallTable\")が0を返却する");
		LockTable(L"SmallTable", LOCK_EXCLUSIVE);
		DeleteRecord(L"SmallTable");
		UnlockTable(L"SmallTable");
		if (GetNumOfRecords(L"SmallTable") != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}

	// CreateTableでSmallTableテーブル2(ID:Int) Max=100 を生成することができる
	printf("CreateTableでSmallTableテーブル2(ID:Int) Max=100 を生成することができる");
	ColumnDefInt ColDefId2(L"ID");
	TableDef TabDefPerson2(L"SmallTable2", 100);
	TabDefPerson2.AddColumnDef(&ColDefId2);
	if (CreateTable(&TabDefPerson2) != 0) {
		printf("...[NG]\r\n");
		return -1;
	}
	if (GetMaxNumOfRecords(L"SmallTable2") != 100) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");

	// Rename table name (UnexistTable ---> RenamedTable)
	printf("Rename table name (UnexistTable ---> RenamedTable)  Does the API return -1?");
	if (RenameTable(L"UnexistTable", L"RenamedTable") != -1) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");

	// Rename table name (SmallTable ---> SmallTable2)
	printf("Rename table name (SmallTable ---> SmallTable2)  Does the API return -1?");
	if (RenameTable(L"SmallTable", L"SmallTable2") != -1) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");

	// Rename table name (SmallTable2 ---> SmallTable3)
	printf("Rename table name (SmallTable2 ---> SmallTable3)  Does the API return 0?");
	if (RenameTable(L"SmallTable2", L"SmallTable3") != 0) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");

	// Check that SmallTable2 is renamed as SmallTable3.
	printf("Check that SmallTable2 is renamed as SmallTable3.");
	wchar_t TblNames[MAX_TABLE_NUMBER][TABLE_NAME_SIZE];
	int TblCnt = GetTableName(TblNames);
	if (lstrcmp(TblNames[0], L"SmallTable") != 0 && lstrcmp(TblNames[1], L"SmallTable2") != 0) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");

	// Rename table name (SmallTable3 ---> SmallTable4)
	printf("Rename table name (SmallTable3 ---> SmallTable4)  Does the API return 0?");
	if (RenameTable(L"SmallTable3", L"SmallTable4") != 0) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");

	// Rename table name (SmallTable ---> SmallTable5)
	printf("Rename table name (SmallTable ---> SmallTable5)  Does the API return 0?");
	if (RenameTable(L"SmallTable", L"SmallTable5") != 0) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");

	// Check that SmallTable is renamed as SmallTable5.
	printf("Check that SmallTable is renamed as SmallTable5.");
	TblCnt = GetTableName(TblNames);
	if (lstrcmp(TblNames[0], L"SmallTable5") != 0 && lstrcmp(TblNames[1], L"SmallTable4") != 0) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");

	// DeleteTable(L"SmallTable4")が0を返却する
	{
		printf("DeleteTable(L\"SmallTable4\")が0を返却する");
		if (DeleteTable(L"SmallTable4") != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}

	// DeleteTable(L"SmallTable5")が0を返却する
	{
		printf("DeleteTable(L\"SmallTable5\")が0を返却する");
		if (DeleteTable(L"SmallTable5") != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}
	return 0;
}
