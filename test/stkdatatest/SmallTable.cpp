#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "..\..\src\stkdata\stkdata.h"

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
・DeleteRecord(_T("SmallTable"))後，GetNumOfRecords(_T("SmallTable"))が0を返却する
・DeleteTable(_T("SmallTable"))が0を返却する
*/
int SmallTable()
{
	// CreateTableでSmallTableテーブル(ID:Int) Max=1 を生成することができる
	printf("CreateTableでSmallTableテーブル(ID:Int) Max=1 を生成することができる");
	ColumnDefInt ColDefId(_T("ID"));
	TableDef TabDefPerson(_T("SmallTable"), 1);
	TabDefPerson.AddColumnDef(&ColDefId);
	if (CreateTable(&TabDefPerson) != 0) {
		printf("...[NG]\r\n");
		return -1;
	}
	if (GetMaxNumOfRecords(_T("SmallTable")) != 1) {
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
			ColDat[0] = new ColumnDataInt(_T("ID"), i);
			CurRecDat = new RecordData(_T("SmallTable"), ColDat, 1);
			if (i == 0) {
				TopRecDat = CurRecDat;
			}
			if (i >= 1) {
				PrvRecDat->SetNextRecord(CurRecDat);
			}
			PrvRecDat = CurRecDat;
		}
		LockTable(_T("SmallTable"), 2);
		InsertRecord(TopRecDat);
		UnlockTable(_T("SmallTable"));
		ClearRecordData(TopRecDat);
		if (GetMaxNumOfRecords(_T("SmallTable")) != 1) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}

	// BigTable("ID"カラム = 1)を検索条件に指定しGetRecord（レコード指定）を実行するとNULLが返る
	{
		printf("BigTable(\"ID\"カラム = 1)を検索条件に指定しGetRecord（レコード指定）を実行するとNULLが返る");
		ColumnData *ColDatTake[1];
		ColDatTake[0] = new ColumnDataInt(_T("ID"), 1);
		RecordData RecDatTake(_T("BigTable"), ColDatTake, 1);
		RecordData *RecDatGet;
		LockTable(_T("SmallTable"), 1);
		RecDatGet = GetRecord(&RecDatTake);
		UnlockTable(_T("SmallTable"));
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
		ColDatTake[0] = new ColumnDataInt(_T("ID"), 0);
		RecordData RecDatTake(_T("SmallTable"), ColDatTake, 1);
		RecordData *RecDatGet;
		LockTable(_T("SmallTable"), 1);
		RecDatGet = GetRecord(&RecDatTake);
		UnlockTable(_T("SmallTable"));
		if (RecDatGet == NULL) {
			printf("...[NG]\r\n");
			return -1;
		}
		ClearRecordData(RecDatGet);
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
		if (GetNumOfRecords(_T("SmallTable")) != 1) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}

	// GetMaxNumOfRecords("SmallTable")で取得したレコード件数は1件となる
	{
		printf("GetMaxNumOfRecords(\"SmallTable\")で取得したレコード件数は1件となる");
		if (GetMaxNumOfRecords(_T("SmallTable")) != 1) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}

	// GetNumOfRecords("BigTable")で取得したレコード件数は-1件となる
	{
		printf("GetNumOfRecords(\"BigTable\")で取得したレコード件数は-1件となる");
		if (GetNumOfRecords(_T("BigTable")) != -1) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}

	// GetMaxNumOfRecords("BigTable")で取得したレコード件数は-1件となる
	{
		printf("GetMaxNumOfRecords(\"BigTable\")で取得したレコード件数は-1件となる");
		if (GetMaxNumOfRecords(_T("BigTable")) != -1) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}

	// DeleteRecord(_T("SmallTable"))後，GetNumOfRecords(_T("SmallTable"))が0を返却する
	{
		printf("DeleteRecord(_T(\"SmallTable\"))後，GetNumOfRecords(_T(\"SmallTable\"))が0を返却する");
		LockTable(_T("SmallTable"), LOCK_EXCLUSIVE);
		DeleteRecord(_T("SmallTable"));
		UnlockTable(_T("SmallTable"));
		if (GetNumOfRecords(_T("SmallTable")) != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}

	// DeleteTable(_T("SmallTable"))が0を返却する
	{
		printf("DeleteTable(_T(\"SmallTable\"))が0を返却する");
		if (DeleteTable(_T("SmallTable")) != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}
	return 0;
}
