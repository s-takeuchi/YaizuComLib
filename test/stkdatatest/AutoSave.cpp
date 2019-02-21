#include <windows.h>
#include <stdio.h>
#include "..\..\src\stkdata\stkdata.h"
#include "..\..\src\stkdata\stkdataapi.h"


int AutoSaveTest()
{
	printf("AutoSave start (interval=30sec, target=AutoSave.std)");
	if (AutoSave(L"AutoSave.std", 30, true) != 0) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");


	for (int Loop = 0; Loop < 30; Loop++) {
		if (Loop == 5) {
			printf("AutoSave(interval=30sec) Create 'Person' table after 50 sec later of start.");
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
			printf("AutoSave(interval=30sec) Insert record of [Shinya Takeuchi, 35] after 100 sec later of start.");
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
			printf("AutoSave(interval=30sec) Insert record of [Akemi Tsunemi, 38] after 110 sec later of start.");
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
			printf("AutoSave(interval=30sec) Insert record of [Joe Satriani, 50] after 150 sec later of start.");
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
			printf("AutoSave(interval=30sec) Stop 'AutoSave' after 200 sec later of start.");
			if (AutoSave(L"AutoSave.std", 30, false) != 0) {
				printf("...[NG]\r\n");
				return -1;
			}
			printf("...[OK]\r\n");
		}

		if (Loop == 21) {
			printf("Insert record of [Steve Vail, 45] after stop of AutoSave.");
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

	printf("Load the preserved data (AutoSave.std) using 'AutoSave' and then, check 3 records exist.");
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
