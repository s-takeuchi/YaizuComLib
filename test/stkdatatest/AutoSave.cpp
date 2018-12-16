#include <windows.h>
#include <stdio.h>
#include "..\..\src\stkdata\stkdata.h"
#include "..\..\src\stkdata\stkdataapi.h"


int AutoSaveTest()
{
	printf("AutoSave(間隔=30秒, 出力=AutoSave.std)を開始する");
	if (AutoSave(L"AutoSave.std", 30, true) != 0) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");


	for (int Loop = 0; Loop < 30; Loop++) {
		if (Loop == 5) {
			printf("AutoSave(30秒間隔)開始後50秒後にPersonテーブルを作成する");
			ColumnDefWStr ColDefName(L"Name", 50);
			ColumnDefInt ColDefAge(L"Age");
			TableDef TabDefPerson(L"Person", 16383);
			TabDefPerson.AddColumnDef(&ColDefName);
			TabDefPerson.AddColumnDef(&ColDefAge);
			if (CreateTable(&TabDefPerson) != 0) {
				printf("...[NG]\r\n");
				return -1;
			}
			printf("...[OK]\r\n");
		}


		if (Loop == 10) {
			printf("AutoSave(30秒間隔)開始後100秒後にShinya Takeuchi, 35をInsertRecordで追加する");
			ColumnData* ColDat[2];
			ColDat[0] = new ColumnDataWStr(L"Name", L"Shinya Takeuchi");
			ColDat[1] = new ColumnDataInt(L"Age", 35);
			RecordData* RecDat = new RecordData(L"Person", ColDat, 2);
			LockTable(L"Person", LOCK_EXCLUSIVE);
			if (InsertRecord(RecDat) != 0) {
				UnlockTable(L"Person");
				delete RecDat;
				printf("...[NG]\r\n");
				return -1;
			}
			UnlockTable(L"Person");
			delete RecDat;
			printf("...[OK]\r\n");
		}


		if (Loop == 11) {
			printf("AutoSave(30秒間隔)開始後110秒後にAkemi Tsunemi, 38をInsertRecordで追加する");
			ColumnData* ColDat[2];
			ColDat[0] = new ColumnDataWStr(L"Name", L"Akemi Tsunemi");
			ColDat[1] = new ColumnDataInt(L"Age", 38);
			RecordData* RecDat = new RecordData(L"Person", ColDat, 2);
			LockTable(L"Person", LOCK_EXCLUSIVE);
			if (InsertRecord(RecDat) != 0) {
				UnlockTable(L"Person");
				delete RecDat;
				printf("...[NG]\r\n");
				return -1;
			}
			UnlockTable(L"Person");
			delete RecDat;
			printf("...[OK]\r\n");
		}


		if (Loop == 15) {
			printf("AutoSave(30秒間隔)開始後150秒後にJoe Satriani, 50をInsertRecordで追加する");
			ColumnData* ColDat[2];
			ColDat[0] = new ColumnDataWStr(L"Name", L"Joe Satriani");
			ColDat[1] = new ColumnDataInt(L"Age", 50);
			RecordData* RecDat = new RecordData(L"Person", ColDat, 2);
			LockTable(L"Person", LOCK_EXCLUSIVE);
			if (InsertRecord(RecDat) != 0) {
				UnlockTable(L"Person");
				delete RecDat;
				printf("...[NG]\r\n");
				return -1;
			}
			UnlockTable(L"Person");
			delete RecDat;
			printf("...[OK]\r\n");
		}


		if (Loop == 20) {
			printf("AutoSave(30秒間隔)開始後200秒後にAutoSaveを停止する");
			if (AutoSave(L"AutoSave.std", 30, false) != 0) {
				printf("...[NG]\r\n");
				return -1;
			}
			printf("...[OK]\r\n");
		}

		if (Loop == 21) {
			printf("AutoSave停止後Steve Vail, 45をInsertRecordで追加する");
			ColumnData* ColDat[2];
			ColDat[0] = new ColumnDataWStr(L"Name", L"Steve Vai");
			ColDat[1] = new ColumnDataInt(L"Age", 45);
			RecordData* RecDat = new RecordData(L"Person", ColDat, 2);
			LockTable(L"Person", LOCK_EXCLUSIVE);
			if (InsertRecord(RecDat) != 0) {
				UnlockTable(L"Person");
				delete RecDat;
				printf("...[NG]\r\n");
				return -1;
			}
			UnlockTable(L"Person");
			delete RecDat;
			printf("...[OK]\r\n");
		}

		Sleep(10000);
	}

	printf("AutoSaveで保存したデータ(AutoSave.std)を読み込み，3件のレコードが存在することを確認する");
	LockAllTable(LOCK_EXCLUSIVE);
	if (LoadData(L"AutoSave.std") != 0) {
		printf("...[NG]\r\n");
		return -1;
	}
	LockAllTable(LOCK_EXCLUSIVE);
	UnlockAllTable();
	if (GetNumOfRecords(L"Person") != 3) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");

	return 0;
}
