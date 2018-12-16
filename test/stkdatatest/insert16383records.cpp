﻿#include <windows.h>
#include <stdio.h>
#include <shlwapi.h>
#include "..\..\src\stkdata\stkdata.h"
#include "..\..\src\stkdata\stkdataapi.h"


int Insert16383Records()
{
	printf("CreateTableでPersonテーブル(Name:WStr(10), ID;Integer, Address:WStr(15), Key:Str(10), Height:Float) Max=16383 を生成することができる");
	ColumnDefWStr ColDefName(L"Name", 10);
	ColumnDefInt ColDefAge(L"ID");
	ColumnDefWStr ColDefAddr(L"Address", 15);
	ColumnDefStr ColDefKey(L"Key", 10);
	ColumnDefFloat ColDefHeight(L"Height");
	TableDef TabDefPerson(L"Person", 16383);
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
	wchar_t Lname[10][5] = {	L"鈴木", L"木村", L"山田", L"田中", L"佐々木",
							L"秋山", L"高橋", L"長谷川", L"渡辺", L"伊藤"
	};
	wchar_t Fname[10][5] = {	L"和人", L"明子", L"達也", L"今日子", L"智",
							L"由美", L"健一", L"亜美", L"琢磨", L"由紀"
	};
	wchar_t Pref[7][15] = {L"東京都", L"神奈川県", L"茨城県", L"埼玉県", L"千葉県", L"栃木県", L"山梨"};
	ColumnData *ColDat1[5];
	RecordData *CurRecDat;
	for (int i = 0; i < 16383; i++) {
		wchar_t Name[10];
		wsprintf(Name, L"%s %s", Lname[rand() % 10], Fname[rand() % 10]);
		if (i % 5000 == 0) {
			ColDat1[0] = new ColumnDataWStr(L"Name", L"竹内 伸也");
		} else {
			ColDat1[0] = new ColumnDataWStr(L"Name", Name);
		}
		ColDat1[1] = new ColumnDataInt(L"ID", i);
		wchar_t Address[15];
		wsprintf(Address, L"%s%d", Pref[rand() % 7], rand() % 10000);
		ColDat1[2] = new ColumnDataWStr(L"Address", Address);
		ColDat1[3] = new ColumnDataStr(L"Key", "No Key");
		ColDat1[4] = new ColumnDataFloat(L"Height", (float)(150.0f + (rand() % 500) / 10.0f));
		CurRecDat = new RecordData(L"Person", ColDat1, 5);
		LockTable(L"Person", 2);
		if (InsertRecord(CurRecDat) != 0) {
			UnlockTable(L"Person");
			printf("...[NG]\r\n");
			delete CurRecDat;
			return -1;
		}
		UnlockTable(L"Person");
		delete CurRecDat;
	}
	printf("...[OK]\r\n");


	{
		printf("ロック無しでソート(AzSortRecord(L\"Person\", L\"ID\")))を行うと-1が返る");
		if (AzSortRecord(L"Person", L"ID") != -1) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");


		printf("ロック無しでソート(ZaSortRecord(L\"Person\", L\"ID\")))を行うと-1が返る");
		if (ZaSortRecord(L"Person", L"ID") != -1) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");


		printf("共有ロック後ソート(AzSortRecord(L\"Person\", L\"ID\"))を行うと-1が返る");
		LockTable(L"Person", LOCK_SHARE);
		if (AzSortRecord(L"Person", L"ID") != -1) {
			UnlockTable(L"Person");
			printf("...[NG]\r\n");
			return -1;
		}
		UnlockTable(L"Person");
		printf("...[OK]\r\n");


		printf("共有ロック後ソート(ZaSortRecord(L\"Person\", L\"ID\"))を行うと-1が返る");
		LockTable(L"Person", LOCK_SHARE);
		if (ZaSortRecord(L"Person", L"ID") != -1) {
			UnlockTable(L"Person");
			printf("...[NG]\r\n");
			return -1;
		}
		UnlockTable(L"Person");
		printf("...[OK]\r\n");


		printf("排他ロック後ソート(AzSortRecord(L\"Person\", L\"ID\"))を行うと0が返る");
		LockTable(L"Person", LOCK_EXCLUSIVE);
		if (AzSortRecord(L"Person", L"ID") != 0) {
			UnlockTable(L"Person");
			printf("...[NG]\r\n");
			return -1;
		}
		UnlockTable(L"Person");
		printf("...[OK]\r\n");


		printf("排他ロック後ソート(ZaSortRecord(\"Person\", L\"ID\"))を行うと0が返る");
		LockTable(L"Person", LOCK_EXCLUSIVE);
		if (ZaSortRecord(L"Person", L"ID") != 0) {
			UnlockTable(L"Person");
			printf("...[NG]\r\n");
			return -1;
		}
		UnlockTable(L"Person");
		printf("...[OK]\r\n");
	}


	{
		printf("追加した16383個のレコードに対しGetRecord(テーブル名指定)で連結した16383個のレコードを取得することができる");
		int Count = 0;
		RecordData *RecDatGet;
		LockTable(L"Person", 1);
		RecDatGet = GetRecord(L"Person");
		UnlockTable(L"Person");
		while (RecDatGet) {
			Count++;
			RecDatGet = RecDatGet->GetNextRecord();
		}
		if (Count != 16383) {
			printf("...[NG]\r\n");
			return -1;
		}
		delete RecDatGet;
		printf("...[OK]\r\n");
	}


	{
		printf("追加した16383個のレコード中に存在するName=\"木村\"または\"田中\"を含み，1000 < ID < 2000  のレコードを検索することができる");
		bool Err = false;
		int FndCnt = 0;
		RecordData *RecDatGet;
		ColumnData *ColDatTake[6];
		RecordData* RecDatKim;
		RecordData* RecDatTan;

		ColDatTake[0] = new ColumnDataWStr(L"Name", L"木村", COMP_CONTAIN);
		ColDatTake[1] = new ColumnDataInt(L"ID", 1000, COMP_GT);
		ColDatTake[2] = new ColumnDataInt(L"ID", 2000, COMP_LT);
		RecDatKim = new RecordData(L"Person", &ColDatTake[0], 3);

		ColDatTake[3] = new ColumnDataWStr(L"Name", L"田中", COMP_CONTAIN);
		ColDatTake[4] = new ColumnDataInt(L"ID", 1000, COMP_GT);
		ColDatTake[5] = new ColumnDataInt(L"ID", 2000, COMP_LT);
		RecDatTan = new RecordData(L"Person", &ColDatTake[3], 3);
		RecDatKim->SetNextRecord(RecDatTan);

		LockTable(L"Person", 1);
		RecDatGet = GetRecord(RecDatKim);
		UnlockTable(L"Person");

		if (RecDatGet == NULL) {
			printf("...[NG]\r\n");
			return -1;
		}
		int NumOfFnd = 0;
		for (RecordData* CurRecDat = RecDatGet; CurRecDat != NULL; CurRecDat = CurRecDat->GetNextRecord()) {
			ColumnDataWStr* ColName = (ColumnDataWStr*)CurRecDat->GetColumn(L"Name");
			if (StrStr(ColName->GetValue(), L"木村") == NULL && StrStr(ColName->GetValue(), L"田中") == NULL ) {
				printf("...[NG]\r\n");
				return -1;
			}
			ColumnDataInt* ColId = (ColumnDataInt*)CurRecDat->GetColumn(L"ID");
			if (ColId->GetValue() <= 1000 && ColId->GetValue() >= 2000) {
				printf("...[NG]\r\n");
				return -1;
			}
			NumOfFnd++;
		}
		delete RecDatKim;
		delete RecDatGet;
		printf("...%d records found [OK]\r\n", NumOfFnd);
	}


	{
		printf("追加した16383個のレコード中に存在するName=\"竹内 伸也\"を含む4つのレコードをGetRecordで検索&取得することができる");
		bool Err = false;
		int FndCnt = 0;
		RecordData *RecDatGet;
		ColumnData *ColDatTake[1];
		ColDatTake[0] = new ColumnDataWStr(L"Name", L"竹内 伸也");
		RecordData RecDatTake(L"Person", ColDatTake, 1);
		LockTable(L"Person", 1);
		RecDatGet = GetRecord(&RecDatTake);
		UnlockTable(L"Person");
		while (RecDatGet) {
			ColumnDataWStr *ColDatName = (ColumnDataWStr*)(RecDatGet->GetColumn(0));
			wchar_t *Name = ColDatName->GetValue();
			if (lstrcmp(Name, L"竹内 伸也") != 0) {
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
		delete RecDatGet;
		printf("...[OK]\r\n");
	}

	{
		printf("16383個のレコード中に存在しないレコード(Name = \"ジョージ ブッシュ\")を検索条件に指定してGetRecord実行後レコードが取得できない");
		RecordData *RecDatGet;
		ColumnData *ColDat[1];
		ColDat[0] = new ColumnDataWStr(L"Name", L"ジョージ ブッシュ");
		RecordData RecDat(L"Person", ColDat, 1);
		LockTable(L"Person", 1);
		RecDatGet = GetRecord(&RecDat);
		UnlockTable(L"Person");
		if (RecDatGet != NULL) {
			printf("...[NG]\r\n");
			return -1;
		}
		delete RecDatGet;
		printf("...[OK]\r\n");
	}

	{
		printf("GetNumOfRecords(\"Person\")で取得したレコード件数は16383件となる");
		if (GetNumOfRecords(L"Person") != 16383) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}

	{
		printf("GetMaxNumOfRecords(\"Person\")で取得したレコード件数は16383件となる");
		if (GetMaxNumOfRecords(L"Person") != 16383) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}

	{
		printf("テーブル情報を\"Insert16383records.std\"に保存する。");
		LockAllTable(LOCK_SHARE);
		if (SaveData(L"Insert16383records.std") != 0) {
			printf("...[NG]\r\n");
			UnlockAllTable();
			return -1;
		}
		UnlockAllTable();
		printf("...[OK]\r\n");
	}

	{
		printf("DeleteRecord(Person-Name=\"竹内 伸也\")をロック無しで実行した場合，-1が返却され，GetNumOfRecords(L\"Person\")の値は16383のままとなる");
		ColumnData* Col[1];
		Col[0] = new ColumnDataWStr(L"Name", L"竹内 伸也");
		RecordData* Rec = new RecordData(L"Person", Col, 1);
		if (DeleteRecord(Rec) != -1) {
			printf("...[NG]\r\n");
			return -1;
		}
		if (GetNumOfRecords(L"Person") != 16383) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");


		printf("DeleteRecord(Person-Name=\"竹内 伸也\")を共有ロックで実行した場合，-1が返却され，GetNumOfRecords(L\"Person\")の値は16383のままとなる");
		LockTable(L"Person", LOCK_SHARE);
		if (DeleteRecord(Rec) != -1) {
			printf("...[NG]\r\n");
			return -1;
		}
		UnlockTable(L"Person");
		if (GetNumOfRecords(L"Person") != 16383) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");


		printf("DeleteRecord(Person-Name=\"竹内 伸也\")を排他ロックで実行した場合，0が返却され，GetNumOfRecords(L\"Person\")の値は16379となる");
		LockTable(L"Person", LOCK_EXCLUSIVE);
		if (DeleteRecord(Rec) != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		UnlockTable(L"Person");
		if (GetNumOfRecords(L"Person") != 16379) {
			printf("...[NG]\r\n");
			return -1;
		}
		delete Rec;
		printf("...[OK]\r\n");
	}

	{
		printf("DeleteRecord(L\"Person\"をロック無しで実行した場合，-1が返却され，GetNumOfRecords(L\"Person\")の値は16379のままとなる");
		if (DeleteRecord(L"Person") != -1) {
			printf("...[NG]\r\n");
			return -1;
		}
		if (GetNumOfRecords(L"Person") != 16379) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");


		printf("DeleteRecord(L\"Person\"を共有ロックで実行した場合，-1が返却され，GetNumOfRecords(L\"Person\")の値は16379のままとなる");
		LockTable(L"Person", LOCK_SHARE);
		if (DeleteRecord(L"Person") != -1) {
			printf("...[NG]\r\n");
			return -1;
		}
		UnlockTable(L"Person");
		if (GetNumOfRecords(L"Person") != 16379) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}


	{
		printf("16383個のレコード中に存在するName=\"由紀\"または\"由美\"を含み，100 < ID < 500  のレコードのKeyカラムを\"Key exist\"に変更する。");
		RecordData *RecDatGet;
		ColumnData *ColDat[7];
		RecordData* RecDatSch1;
		RecordData* RecDatSch2;
		RecordData* RecDatUpd;

		ColDat[0] = new ColumnDataWStr(L"Name", L"由紀", COMP_CONTAIN);
		ColDat[1] = new ColumnDataInt(L"ID", 100, COMP_GT);
		ColDat[2] = new ColumnDataInt(L"ID", 500, COMP_LT);
		RecDatSch1 = new RecordData(L"Person", &ColDat[0], 3);

		ColDat[3] = new ColumnDataWStr(L"Name", L"由美", COMP_CONTAIN);
		ColDat[4] = new ColumnDataInt(L"ID", 100, COMP_GT);
		ColDat[5] = new ColumnDataInt(L"ID", 500, COMP_LT);
		RecDatSch2 = new RecordData(L"Person", &ColDat[3], 3);
		RecDatSch1->SetNextRecord(RecDatSch2);

		ColDat[6] = new ColumnDataStr(L"Key", "Key exist");
		RecDatUpd = new RecordData(L"Person", &ColDat[6], 1);

		LockTable(L"Person", LOCK_EXCLUSIVE);
		UpdateRecord(RecDatSch1, RecDatUpd);
		UnlockTable(L"Person");


		LockTable(L"Person", LOCK_SHARE);
		RecDatGet = GetRecord(L"Person");
		UnlockTable(L"Person");

		if (RecDatGet == NULL) {
			printf("...[NG]\r\n");
			return -1;
		}
		int NumOfKey = 0;
		int NumOfNoKey = 0;
		for (RecordData* CurRecDat = RecDatGet; CurRecDat != NULL; CurRecDat = CurRecDat->GetNextRecord()) {
			ColumnDataWStr* ColName = (ColumnDataWStr*)CurRecDat->GetColumn(L"Name");
			ColumnDataInt* ColId = (ColumnDataInt*)CurRecDat->GetColumn(L"ID");
			ColumnDataStr* ColKey = (ColumnDataStr*)CurRecDat->GetColumn(L"Key");

			if ((StrStr(ColName->GetValue(), L"由紀") != NULL || StrStr(ColName->GetValue(), L"由美") != NULL) && ColId->GetValue() > 100 && ColId->GetValue() < 500) {
				if (strcmp(ColKey->GetValue(), "Key exist") != 0) {
					printf("...[NG]\r\n");
					return -1;
				} else {
					NumOfKey++;
				}
			} else {
				if (strcmp(ColKey->GetValue(), "No Key") != 0) {
					printf("...[NG]\r\n");
					return -1;
				} else {
					NumOfNoKey++;
				}
			}
		}
		printf("(Key=%d, NoKey=%d)...[OK]\r\n", NumOfKey, NumOfNoKey);


		printf("16383個のレコード中に存在するName=\"由紀\"または\"由美\"を含み，100 < ID < 500  のレコードを削除する。");
		if (GetNumOfRecords(L"Person") != 16379) {
			printf("...[NG]\r\n");
			return -1;
		}
		LockTable(L"Person", LOCK_EXCLUSIVE);
		DeleteRecord(RecDatSch1);
		UnlockTable(L"Person");
		int Rest = GetNumOfRecords(L"Person");
		if (Rest != (16379 - NumOfKey)) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("(Deleted count = %d, Rest = %d)...[OK]\r\n", NumOfKey, Rest);

		delete RecDatSch1;
		delete RecDatUpd;
		delete RecDatGet;
	}


	{
		printf("DeleteRecord(L\"Person\"を排他ロックで実行した場合，0が返却され，GetNumOfRecords(L\"Person\"))の値は0となる");
		LockTable(L"Person", LOCK_EXCLUSIVE);
		if (DeleteRecord(L"Person") != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		UnlockTable(L"Person");
		if (GetNumOfRecords(L"Person") != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");


		printf("再度DeleteRecord(L\"Person\"を排他ロックで実行した場合，もはや対象テーブルが存在しないため-1が返却される");
		LockTable(L"Person", LOCK_EXCLUSIVE);
		if (DeleteRecord(L"Person") != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		UnlockTable(L"Person");
		printf("...[OK]\r\n");
	}


	{
		printf("DeleteTable(\"Person\")でテーブルを削除することができる");
		if (DeleteTable(L"Person") != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}

	return 0;
}
