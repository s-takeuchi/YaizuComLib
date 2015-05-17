#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "..\..\src\com\stkdata\stkdata.h"


int Insert16383Records()
{
	printf("CreateTableでPersonテーブル(Name:WStr(10), ID;Integer, Address:WStr(15), Key:Str(10), Height:Float) Max=16383 を生成することができる");
	ColumnDefWStr ColDefName(_T("Name"), 10);
	ColumnDefInt ColDefAge(_T("ID"));
	ColumnDefWStr ColDefAddr(_T("Address"), 15);
	ColumnDefStr ColDefKey(_T("Key"), 10);
	ColumnDefFloat ColDefHeight(_T("Height"));
	TableDef TabDefPerson(_T("Person"), 16383);
	TabDefPerson.AddColumnDef(&ColDefName);
	TabDefPerson.AddColumnDef(&ColDefAge);
	TabDefPerson.AddColumnDef(&ColDefAddr);
	TabDefPerson.AddColumnDef(&ColDefKey);
	TabDefPerson.AddColumnDef(&ColDefHeight);
	if (CreateTable(&TabDefPerson) != 0) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");


	printf("Personテーブルに対してInsertRecordを16383回繰り返し，16383個のレコードを追加することができる");
	TCHAR Lname[10][5] = {	_T("鈴木"), _T("木村"), _T("山田"), _T("田中"), _T("佐々木"),
							_T("秋山"), _T("高橋"), _T("長谷川"), _T("渡辺"), _T("伊藤")
	};
	TCHAR Fname[10][5] = {	_T("和人"), _T("明子"), _T("達也"), _T("今日子"), _T("智"),
							_T("由美"), _T("健一"), _T("亜美"), _T("琢磨"), _T("由紀")
	};
	TCHAR Pref[7][15] = {_T("東京都"), _T("神奈川県"), _T("茨城県"), _T("埼玉県"), _T("千葉県"), _T("栃木県"), _T("山梨")};
	ColumnData *ColDat1[5];
	RecordData *CurRecDat;
	for (int i = 0; i < 16383; i++) {
		TCHAR Name[10];
		wsprintf(Name, _T("%s %s"), Lname[rand() % 10], Fname[rand() % 10]);
		if (i % 5000 == 0) {
			ColDat1[0] = new ColumnDataWStr(_T("Name"), _T("竹内 伸也"));
		} else {
			ColDat1[0] = new ColumnDataWStr(_T("Name"), Name);
		}
		ColDat1[1] = new ColumnDataInt(_T("ID"), i);
		TCHAR Address[15];
		wsprintf(Address, _T("%s%d"), Pref[rand() % 7], rand() % 10000);
		ColDat1[2] = new ColumnDataWStr(_T("Address"), Address);
		ColDat1[3] = new ColumnDataStr(_T("Key"), "No Key");
		ColDat1[4] = new ColumnDataFloat(_T("Height"), (float)(150.0f + (rand() % 500) / 10.0f));
		CurRecDat = new RecordData(_T("Person"), ColDat1, 5);
		LockTable(_T("Person"), 2);
		if (InsertRecord(CurRecDat) != 0) {
			UnlockTable(_T("Person"));
			printf("...[NG]\r\n");
			ClearRecordData(CurRecDat);
			return -1;
		}
		UnlockTable(_T("Person"));
		delete ColDat1[0];
		delete ColDat1[1];
		delete ColDat1[2];
		delete ColDat1[3];
		delete ColDat1[4];
		delete CurRecDat;
	}
	printf("...[OK]\r\n");


	{
		printf("ロック無しでソート(AzSortRecord(_T(\"Person\"), _T(\"ID\")))を行うと-1が返る");
		if (AzSortRecord(_T("Person"), _T("ID")) != -1) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");


		printf("ロック無しでソート(ZaSortRecord(_T(\"Person\"), _T(\"ID\")))を行うと-1が返る");
		if (ZaSortRecord(_T("Person"), _T("ID")) != -1) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");


		printf("共有ロック後ソート(AzSortRecord(_T(\"Person\"), _T(\"ID\")))を行うと-1が返る");
		LockTable(_T("Person"), LOCK_SHARE);
		if (AzSortRecord(_T("Person"), _T("ID")) != -1) {
			UnlockTable(_T("Person"));
			printf("...[NG]\r\n");
			return -1;
		}
		UnlockTable(_T("Person"));
		printf("...[OK]\r\n");


		printf("共有ロック後ソート(ZaSortRecord(_T(\"Person\"), _T(\"ID\")))を行うと-1が返る");
		LockTable(_T("Person"), LOCK_SHARE);
		if (ZaSortRecord(_T("Person"), _T("ID")) != -1) {
			UnlockTable(_T("Person"));
			printf("...[NG]\r\n");
			return -1;
		}
		UnlockTable(_T("Person"));
		printf("...[OK]\r\n");


		printf("排他ロック後ソート(AzSortRecord(_T(\"Person\"), _T(\"ID\")))を行うと0が返る");
		LockTable(_T("Person"), LOCK_EXCLUSIVE);
		if (AzSortRecord(_T("Person"), _T("ID")) != 0) {
			UnlockTable(_T("Person"));
			printf("...[NG]\r\n");
			return -1;
		}
		UnlockTable(_T("Person"));
		printf("...[OK]\r\n");


		printf("排他ロック後ソート(ZaSortRecord(_T(\"Person\"), _T(\"ID\")))を行うと0が返る");
		LockTable(_T("Person"), LOCK_EXCLUSIVE);
		if (ZaSortRecord(_T("Person"), _T("ID")) != 0) {
			UnlockTable(_T("Person"));
			printf("...[NG]\r\n");
			return -1;
		}
		UnlockTable(_T("Person"));
		printf("...[OK]\r\n");
	}


	{
		printf("追加した16383個のレコードに対しGetRecord(テーブル名指定)で連結した16383個のレコードを取得することができる");
		int Count = 0;
		RecordData *RecDatGet;
		LockTable(_T("Person"), 1);
		RecDatGet = GetRecord(_T("Person"));
		UnlockTable(_T("Person"));
		while (RecDatGet) {
			Count++;
			RecDatGet = RecDatGet->GetNextRecord();
		}
		if (Count != 16383) {
			printf("...[NG]\r\n");
			return -1;
		}
		ClearRecordData(RecDatGet);
		printf("...[OK]\r\n");
	}


	{
		printf("追加した16383個のレコード中に存在するName=\"竹内 伸也\"を含む4つのレコードをGetRecordで検索&取得することができる");
		BOOL Err = FALSE;
		int FndCnt = 0;
		RecordData *RecDatGet;
		ColumnData *ColDatTake[1];
		ColDatTake[0] = new ColumnDataWStr(_T("Name"), _T("竹内 伸也"));
		RecordData RecDatTake(_T("Person"), ColDatTake, 1);
		LockTable(_T("Person"), 1);
		RecDatGet = GetRecord(&RecDatTake);
		UnlockTable(_T("Person"));
		while (RecDatGet) {
			ColumnDataWStr *ColDatName = (ColumnDataWStr*)(RecDatGet->GetColumn(0));
			TCHAR *Name = ColDatName->GetValue();
			if (lstrcmp(Name, _T("竹内 伸也")) != 0) {
				printf("...[NG]\r\n");
				return -1;
			}
			FndCnt++;
			RecDatGet = RecDatGet->GetNextRecord();
		}
		if (FndCnt != 4) {
			printf("...[NG]\r\n");
			return -1;
		}
		ClearRecordData(RecDatGet);
		printf("...[OK]\r\n");
	}

	{
		printf("16383個のレコード中に存在しないレコード(Name = \"ジョージ ブッシュ\")を検索条件に指定してGetRecord実行後レコードが取得できない");
		RecordData *RecDatGet;
		ColumnData *ColDat[1];
		ColDat[0] = new ColumnDataWStr(_T("Name"), _T("ジョージ ブッシュ"));
		RecordData RecDat(_T("Person"), ColDat, 1);
		LockTable(_T("Person"), 1);
		RecDatGet = GetRecord(&RecDat);
		UnlockTable(_T("Person"));
		if (RecDatGet != NULL) {
			printf("...[NG]\r\n");
			return -1;
		}
		ClearRecordData(RecDatGet);
		printf("...[OK]\r\n");
	}

	{
		printf("GetNumOfRecords(\"Person\")で取得したレコード件数は16383件となる");
		if (GetNumOfRecords(_T("Person")) != 16383) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}

	{
		printf("GetMaxNumOfRecords(\"Person\")で取得したレコード件数は16383件となる");
		if (GetMaxNumOfRecords(_T("Person")) != 16383) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}

	{
		printf("テーブル情報を\"Insert16383records.std\"に保存する。");
		LockAllTable(LOCK_SHARE);
		if (SaveData(_T("Insert16383records.std")) != 0) {
			printf("...[NG]\r\n");
			UnlockAllTable();
			return -1;
		}
		UnlockAllTable();
		printf("...[OK]\r\n");
	}

	{
		printf("DeleteRecord(Person-Name=\"竹内 伸也\")をロック無しで実行した場合，-1が返却され，GetNumOfRecords(_T(\"Person\"))の値は16383のままとなる");
		ColumnData* Col[1];
		Col[0] = new ColumnDataWStr(_T("Name"), _T("竹内 伸也"));
		RecordData* Rec = new RecordData(_T("Person"), Col, 1);
		if (DeleteRecord(Rec) != -1) {
			printf("...[NG]\r\n");
			return -1;
		}
		if (GetNumOfRecords(_T("Person")) != 16383) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");


		printf("DeleteRecord(Person-Name=\"竹内 伸也\")を共有ロックで実行した場合，-1が返却され，GetNumOfRecords(_T(\"Person\"))の値は16383のままとなる");
		LockTable(_T("Person"), LOCK_SHARE);
		if (DeleteRecord(Rec) != -1) {
			printf("...[NG]\r\n");
			return -1;
		}
		UnlockTable(_T("Person"));
		if (GetNumOfRecords(_T("Person")) != 16383) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");


		printf("DeleteRecord(Person-Name=\"竹内 伸也\")を排他ロックで実行した場合，0が返却され，GetNumOfRecords(_T(\"Person\"))の値は16379となる");
		LockTable(_T("Person"), LOCK_EXCLUSIVE);
		if (DeleteRecord(Rec) != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		UnlockTable(_T("Person"));
		if (GetNumOfRecords(_T("Person")) != 16379) {
			printf("...[NG]\r\n");
			return -1;
		}
		ClearRecordData(Rec);
		printf("...[OK]\r\n");
	}

	{
		printf("DeleteRecord(_T(\"Person\")をロック無しで実行した場合，-1が返却され，GetNumOfRecords(_T(\"Person\"))の値は16379のままとなる");
		if (DeleteRecord(_T("Person")) != -1) {
			printf("...[NG]\r\n");
			return -1;
		}
		if (GetNumOfRecords(_T("Person")) != 16379) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");


		printf("DeleteRecord(_T(\"Person\")を共有ロックで実行した場合，-1が返却され，GetNumOfRecords(_T(\"Person\"))の値は16379のままとなる");
		LockTable(_T("Person"), LOCK_SHARE);
		if (DeleteRecord(_T("Person")) != -1) {
			printf("...[NG]\r\n");
			return -1;
		}
		UnlockTable(_T("Person"));
		if (GetNumOfRecords(_T("Person")) != 16379) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");


		printf("DeleteRecord(_T(\"Person\")を排他ロックで実行した場合，0が返却され，GetNumOfRecords(_T(\"Person\"))の値は0となる");
		LockTable(_T("Person"), LOCK_EXCLUSIVE);
		if (DeleteRecord(_T("Person")) != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		UnlockTable(_T("Person"));
		if (GetNumOfRecords(_T("Person")) != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");


		printf("再度DeleteRecord(_T(\"Person\")を排他ロックで実行した場合，もはや対象テーブルが存在しないため-1が返却される");
		LockTable(_T("Person"), LOCK_EXCLUSIVE);
		if (DeleteRecord(_T("Person")) != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		UnlockTable(_T("Person"));
		printf("...[OK]\r\n");
	}


	{
		printf("DeleteTable(_T(\"Person\"))でテーブルを削除することができる");
		if (DeleteTable(_T("Person")) != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}

	return 0;
}
