#include <windows.h>
#include <stdio.h>
#include "..\..\src\stkdata\stkdata.h"
#include "..\..\src\stkdata\stkdataapi.h"


int AutoSaveTest()
{
	printf("AutoSave(ŠÔŠu=30•b, o—Í=AutoSave.std)‚ğŠJn‚·‚é");
	if (AutoSave(L"AutoSave.std", 30, true) != 0) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");


	for (int Loop = 0; Loop < 30; Loop++) {
		if (Loop == 5) {
			printf("AutoSave(30•bŠÔŠu)ŠJnŒã50•bŒã‚ÉPersonƒe[ƒuƒ‹‚ğì¬‚·‚é");
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
			printf("AutoSave(30•bŠÔŠu)ŠJnŒã100•bŒã‚ÉShinya Takeuchi, 35‚ğInsertRecord‚Å’Ç‰Á‚·‚é");
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
			printf("AutoSave(30•bŠÔŠu)ŠJnŒã110•bŒã‚ÉAkemi Tsunemi, 38‚ğInsertRecord‚Å’Ç‰Á‚·‚é");
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
			printf("AutoSave(30•bŠÔŠu)ŠJnŒã150•bŒã‚ÉJoe Satriani, 50‚ğInsertRecord‚Å’Ç‰Á‚·‚é");
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
			printf("AutoSave(30•bŠÔŠu)ŠJnŒã200•bŒã‚ÉAutoSave‚ğ’â~‚·‚é");
			if (AutoSave(L"AutoSave.std", 30, false) != 0) {
				printf("...[NG]\r\n");
				return -1;
			}
			printf("...[OK]\r\n");
		}

		if (Loop == 21) {
			printf("AutoSave’â~ŒãSteve Vail, 45‚ğInsertRecord‚Å’Ç‰Á‚·‚é");
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

	printf("AutoSave‚Å•Û‘¶‚µ‚½ƒf[ƒ^(AutoSave.std)‚ğ“Ç‚İ‚İC3Œ‚ÌƒŒƒR[ƒh‚ª‘¶İ‚·‚é‚±‚Æ‚ğŠm”F‚·‚é");
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
