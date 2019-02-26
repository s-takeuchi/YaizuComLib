#include "../../src/StkPl.h"
#include "../../src/stkdata/stkdata.h"
#include "../../src/stkdata/stkdataapi.h"

int BasicFloatTest01()
{
	StkPlPrintf("Create table \"Float-Test\" which contains 32 float columns and 16383 records.");
	ColumnDefFloat ColDef00(L"Fcol00");
	ColumnDefFloat ColDef01(L"Fcol01");
	ColumnDefFloat ColDef02(L"Fcol02");
	ColumnDefFloat ColDef03(L"Fcol03");
	ColumnDefFloat ColDef04(L"Fcol04");
	ColumnDefFloat ColDef05(L"Fcol05");
	ColumnDefFloat ColDef06(L"Fcol06");
	ColumnDefFloat ColDef07(L"Fcol07");
	ColumnDefFloat ColDef08(L"Fcol08");
	ColumnDefFloat ColDef09(L"Fcol09");
	ColumnDefFloat ColDef10(L"Fcol10");
	ColumnDefFloat ColDef11(L"Fcol11");
	ColumnDefFloat ColDef12(L"Fcol12");
	ColumnDefFloat ColDef13(L"Fcol13");
	ColumnDefFloat ColDef14(L"Fcol14");
	ColumnDefFloat ColDef15(L"Fcol15");
	ColumnDefFloat ColDef16(L"Fcol16");
	ColumnDefFloat ColDef17(L"Fcol17");
	ColumnDefFloat ColDef18(L"Fcol18");
	ColumnDefFloat ColDef19(L"Fcol19");
	ColumnDefFloat ColDef20(L"Fcol20");
	ColumnDefFloat ColDef21(L"Fcol21");
	ColumnDefFloat ColDef22(L"Fcol22");
	ColumnDefFloat ColDef23(L"Fcol23");
	ColumnDefFloat ColDef24(L"Fcol24");
	ColumnDefFloat ColDef25(L"Fcol25");
	ColumnDefFloat ColDef26(L"Fcol26");
	ColumnDefFloat ColDef27(L"Fcol27");
	ColumnDefFloat ColDef28(L"Fcol28");
	ColumnDefFloat ColDef29(L"Fcol29");
	ColumnDefFloat ColDef30(L"Fcol30");
	ColumnDefFloat ColDef31(L"Fcol31");
	TableDef TabDefTest(L"Float-Test", 16383);
	TabDefTest.AddColumnDef(&ColDef00);
	TabDefTest.AddColumnDef(&ColDef01);
	TabDefTest.AddColumnDef(&ColDef02);
	TabDefTest.AddColumnDef(&ColDef03);
	TabDefTest.AddColumnDef(&ColDef04);
	TabDefTest.AddColumnDef(&ColDef05);
	TabDefTest.AddColumnDef(&ColDef06);
	TabDefTest.AddColumnDef(&ColDef07);
	TabDefTest.AddColumnDef(&ColDef08);
	TabDefTest.AddColumnDef(&ColDef09);
	TabDefTest.AddColumnDef(&ColDef10);
	TabDefTest.AddColumnDef(&ColDef11);
	TabDefTest.AddColumnDef(&ColDef12);
	TabDefTest.AddColumnDef(&ColDef13);
	TabDefTest.AddColumnDef(&ColDef14);
	TabDefTest.AddColumnDef(&ColDef15);
	TabDefTest.AddColumnDef(&ColDef16);
	TabDefTest.AddColumnDef(&ColDef17);
	TabDefTest.AddColumnDef(&ColDef18);
	TabDefTest.AddColumnDef(&ColDef19);
	TabDefTest.AddColumnDef(&ColDef20);
	TabDefTest.AddColumnDef(&ColDef21);
	TabDefTest.AddColumnDef(&ColDef22);
	TabDefTest.AddColumnDef(&ColDef23);
	TabDefTest.AddColumnDef(&ColDef24);
	TabDefTest.AddColumnDef(&ColDef25);
	TabDefTest.AddColumnDef(&ColDef26);
	TabDefTest.AddColumnDef(&ColDef27);
	TabDefTest.AddColumnDef(&ColDef28);
	TabDefTest.AddColumnDef(&ColDef29);
	TabDefTest.AddColumnDef(&ColDef30);
	TabDefTest.AddColumnDef(&ColDef31);
	if (CreateTable(&TabDefTest) != 0) {
		StkPlPrintf("...[NG]\r\n");
		return -1;
	}
	StkPlPrintf("...[OK]\r\n");


	StkPlPrintf("Check the table \"Float-Test\" has 32 columns.");
	if (GetColumnCount(L"Float-Test") != 32) {
		StkPlPrintf("...[NG]\r\n");
		return -1;
	}
	StkPlPrintf("...[OK]\r\n");


	StkPlPrintf("Check the size of any 0th, 1st, 30th and 31st column of \"Float-Test\" table shows 4 bytes.");
	if (GetColumnSize(L"Float-Test", L"Fcol00") != 4 ||
		GetColumnSize(L"Float-Test", L"Fcol01") != 4 ||
		GetColumnSize(L"Float-Test", L"Fcol30") != 4 ||
		GetColumnSize(L"Float-Test", L"Fcol31") != 4) {
		StkPlPrintf("...[NG]\r\n");
		return -1;
	}
	StkPlPrintf("...[OK]\r\n");


	StkPlPrintf("Check the type of Fcol10, Fcol20 and Fcol30 column of \"Float-Test\" table is float.");
	if (GetColumnType(L"Float-Test", L"Fcol10") != COLUMN_TYPE_FLOAT ||
		GetColumnSize(L"Float-Test", L"Fcol20") != COLUMN_TYPE_FLOAT ||
		GetColumnSize(L"Float-Test", L"Fcol30") != COLUMN_TYPE_FLOAT) {
		StkPlPrintf("...[NG]\r\n");
		return -1;
	}
	StkPlPrintf("...[OK]\r\n");

	return 0;
}

int BasicFloatTest02()
{
	StkPlPrintf("Add 10 records (with Fcol00 - Fcol31 columns to \"Float-Test\" table repeatedly.");
	ColumnData *ColDat[32];
	RecordData *RecDat;
	LockTable(L"Float-Test", LOCK_EXCLUSIVE);
	for (int j = 0; j < 10; j++) {
		for (int i = 0; i < 32; i++) {
			wchar_t Buf[32];
			StkPlSwPrintf(Buf, 32, L"Fcol%02d", i);
			ColDat[i] = new ColumnDataFloat(Buf, (float)(100.0 + 0.1 * j + i * 0.001));
		}
		RecDat = new RecordData(L"Float-Test", ColDat, 32);
		if (InsertRecord(RecDat) != 0) {
			StkPlPrintf("...[NG]\r\n");
			delete RecDat;
			UnlockTable(L"Float-Test");
			return -1;
		}
		delete RecDat;
	}
	UnlockTable(L"Float-Test");
	StkPlPrintf("...[OK]\r\n");


	StkPlPrintf("Check the number of records which are existed in \"Float-Test\" table is 10.");
	if (GetNumOfRecords(L"Float-Test") != 10) {
		StkPlPrintf("...[NG]\r\n");
		return -1;
	}
	StkPlPrintf("...[OK]\r\n");


	StkPlPrintf("Check that 10 records (with Fcol00-Fcol31 column) in \"Float-Test\" table added can be acquired appropriately.");
	RecordData *AcquiredRecDat;
	LockTable(L"Float-Test", LOCK_SHARE);
	AcquiredRecDat = GetRecord(L"Float-Test");
	UnlockTable(L"Float-Test");
	for (int j = 0; j < 10; j++) {
		for (int i = 0; i < 32; i++) {
			if (((ColumnDataFloat*)(AcquiredRecDat->GetColumn(i)))->GetValue() != (float)(100.0 + 0.1 * j + i * 0.001)) {
				StkPlPrintf("...[NG]\r\n");
				delete AcquiredRecDat;
				return -1;
			}
		}
		AcquiredRecDat = AcquiredRecDat->GetNextRecord();
	}
	delete AcquiredRecDat;
	StkPlPrintf("...[OK]\r\n");

	return 0;
}

int BasicFloatTest03()
{
	StkPlPrintf("Update 10 records (Fcol00 and Fcol01) in \"Float-Test\" table repeatedly.");
	ColumnData *ColDat[1];
	ColumnData *UpdColDat[2];
	RecordData *RecDat;
	RecordData *UpdRecDat;
	LockTable(L"Float-Test", LOCK_EXCLUSIVE);
	for (int i = 0; i < 10; i++) {
		ColDat[0] = new ColumnDataFloat(L"Fcol00", (float)(100.0 + 0.1 * i));
		RecDat = new RecordData(L"Float-Test", ColDat, 1);
		UpdColDat[0] = new ColumnDataFloat(L"Fcol00", 0);
		UpdColDat[1] = new ColumnDataFloat(L"Fcol01", 999999000000.0f);
		UpdRecDat = new RecordData(L"Float-Test", UpdColDat, 2);
		if (UpdateRecord(RecDat, UpdRecDat) != 0) {
			StkPlPrintf("...[NG]\r\n");
			delete RecDat;
			delete UpdRecDat;
			UnlockTable(L"Float-Test");
		}
		delete RecDat;
		delete UpdRecDat;
	}
	UnlockTable(L"Float-Test");
	StkPlPrintf("...[OK]\r\n");


	RecordData *AcquiredRecDat;
	LockTable(L"Float-Test", LOCK_SHARE);
	AcquiredRecDat = GetRecord(L"Float-Test");
	UnlockTable(L"Float-Test");
	StkPlPrintf("Check that 10 records (with Fcol00 - Fcol31 column) which are updated can be acquired appropriately.");
	for (int j = 0; j < 10; j++) {
		for (int i = 0; i < 32; i++) {
			if (i == 0) {
				if (((ColumnDataFloat*)(AcquiredRecDat->GetColumn(i)))->GetValue() != 0) {
					StkPlPrintf("...[NG]\r\n");
					delete AcquiredRecDat;
					return -1;
				}
			} else if (i == 1) {
			} else if (((ColumnDataFloat*)(AcquiredRecDat->GetColumn(i)))->GetValue() != (float)(100.0 + 0.1 * j + i * 0.001)) {
				StkPlPrintf("...[NG]\r\n");
				delete AcquiredRecDat;
				return -1;
			}
		}
		AcquiredRecDat = AcquiredRecDat->GetNextRecord();
	}
	delete AcquiredRecDat;
	StkPlPrintf("...[OK]\r\n");

	return 0;
}

int BasicFloatTest04()
{
	StkPlPrintf("Add 10 records (with Fcol00 - Fcol31 columns) to  \"Float-Test\" table simaltainiously.");
	ColumnData *ColDat[32];
	RecordData *RecDat;
	RecordData *HeadRecDat;
	RecordData *LastRecDat;
	for (int j = 0; j < 10; j++) {
		for (int i = 0; i < 32; i++) {
			wchar_t Buf[32];
			StkPlSwPrintf(Buf, 32, L"Fcol%02d", i);
			ColDat[i] = new ColumnDataFloat(Buf, (float)(200.0 + 0.1 * j + i * 0.001));
		}
		RecDat = new RecordData(L"Float-Test", ColDat, 32);
		if (j >= 1) {
			LastRecDat->SetNextRecord(RecDat);
		} else {
			HeadRecDat = RecDat;
		}
		LastRecDat = RecDat;
	}
	LockTable(L"Float-Test", LOCK_EXCLUSIVE);
	if (InsertRecord(HeadRecDat) != 0) {
		StkPlPrintf("...[NG]\r\n");
		delete HeadRecDat;
		UnlockTable(L"Float-Test");
		return -1;
	}
	UnlockTable(L"Float-Test");
	delete HeadRecDat;
	StkPlPrintf("...[OK]\r\n");

	{
		StkPlPrintf("Search \"Float-table\" table (with Less than specification)");
		ColDat[0] = new ColumnDataFloat(L"Fcol05", 200.5);
		ColDat[0]->SetComparisonOperator(COMP_LT);
		RecDat = new RecordData(L"Float-Test", ColDat, 1);
		LockTable(L"Float-Test", LOCK_SHARE);
		HeadRecDat = GetRecord(RecDat);
		UnlockTable(L"Float-Test");
		RecordData* CurRecDat;
		if (HeadRecDat == NULL) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
		for (CurRecDat = HeadRecDat; CurRecDat != NULL; CurRecDat = CurRecDat->GetNextRecord()) {
			ColumnDataFloat* TmpCol = (ColumnDataFloat*)CurRecDat->GetColumn(L"Fcol05");
			float TmpFl = TmpCol->GetValue();
			if (TmpFl >= 200.5) {
				StkPlPrintf("...[NG]\r\n");
				return -1;
			}
		}
		delete HeadRecDat;
		delete RecDat;
		StkPlPrintf("...[OK]\r\n");
	}

	{
		StkPlPrintf("Search \"Float-table\" table (with Greater than or Equal to specification)");
		ColDat[0] = new ColumnDataFloat(L"Fcol05", 200.5, COMP_GE);
		RecDat = new RecordData(L"Float-Test", ColDat, 1);
		LockTable(L"Float-Test", LOCK_SHARE);
		HeadRecDat = GetRecord(RecDat);
		UnlockTable(L"Float-Test");
		RecordData* CurRecDat;
		if (HeadRecDat == NULL) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
		for (CurRecDat = HeadRecDat; CurRecDat != NULL; CurRecDat = CurRecDat->GetNextRecord()) {
			ColumnDataFloat* TmpCol = (ColumnDataFloat*)CurRecDat->GetColumn(L"Fcol05");
			float TmpFl = TmpCol->GetValue();
			if (TmpFl < 200.5) {
				StkPlPrintf("...[NG]\r\n");
				return -1;
			}
		}
		delete HeadRecDat;
		delete RecDat;
		StkPlPrintf("...[OK]\r\n");
	}

	StkPlPrintf("Check the number of existed records which are in \"Float-Test\" table.");
	if (GetNumOfRecords(L"Float-Test") != 20) {
		StkPlPrintf("...[NG]\r\n");
		return -1;
	}
	StkPlPrintf("...[OK]\r\n");


	StkPlPrintf("Check that 10 records information can be acquired appropriately from 20 records in \"Float-Test\" table with criteria specification.");
	RecordData *AcquiredRecDat;
	for (int i = 0; i < 10; i++) {
		ColDat[0] = new ColumnDataFloat(L"Fcol00", (float)(200.0 + 0.1 * i));
		RecDat = new RecordData(L"Float-Test", ColDat, 1);
		if (i >= 1) {
			LastRecDat->SetNextRecord(RecDat);
		} else {
			HeadRecDat = RecDat;
		}
		LastRecDat = RecDat;
	}
	LockTable(L"Float-Test", LOCK_SHARE);
	AcquiredRecDat = GetRecord(HeadRecDat);
	UnlockTable(L"Float-Test");
	delete HeadRecDat;
	if (AcquiredRecDat == NULL) {
		StkPlPrintf("...[NG]\r\n");
		return -1;
	}
	for (int j = 0; j < 10; j++) {
		for (int i = 0; i < 32; i++) {
			if (((ColumnDataFloat*)(AcquiredRecDat->GetColumn(i)))->GetValue() != (float)(200.0 + 0.1 * j + i * 0.001)) {
				StkPlPrintf("...[NG]\r\n");
				delete AcquiredRecDat;
				return -1;
			}
		}
		AcquiredRecDat = AcquiredRecDat->GetNextRecord();
	}
	delete AcquiredRecDat;
	StkPlPrintf("...[OK]\r\n");

	return 0;
}

int BasicFloatTest05()
{
	StkPlPrintf("Update 10 records (Fcol00 and Fcol01) in \"Float-Test\" table simultaneously.");
	ColumnData *ColDat[1];
	RecordData *RecDat;
	RecordData *LastRecDat;
	RecordData *HeadRecDat;
	ColumnData *UpdColDat[2];
	RecordData *UpdRecDat;
	LockTable(L"Float-Test", LOCK_EXCLUSIVE);
	for (int i = 0; i < 10; i++) {
		ColDat[0] = new ColumnDataFloat(L"Fcol00", (float)(200.0 + 0.1 * i));
		RecDat = new RecordData(L"Float-Test", ColDat, 1);
		if (i >= 1) {
			LastRecDat->SetNextRecord(RecDat);
		} else {
			HeadRecDat = RecDat;
		}
		LastRecDat = RecDat;
	}
	UpdColDat[0] = new ColumnDataFloat(L"Fcol00", 0);
	UpdColDat[1] = new ColumnDataFloat(L"Fcol01", 999999000000.0f);
	UpdRecDat = new RecordData(L"Float-Test", UpdColDat, 2);
	if (UpdateRecord(HeadRecDat, UpdRecDat) != 0) {
		StkPlPrintf("...[NG]\r\n");
		delete HeadRecDat;
		delete UpdRecDat;
		UnlockTable(L"Float-Test");
	}
	delete HeadRecDat;
	delete UpdRecDat;
	UnlockTable(L"Float-Test");
	StkPlPrintf("...[OK]\r\n");


	RecordData *AcquiredRecDat;
	LockTable(L"Float-Test", LOCK_SHARE);
	AcquiredRecDat = GetRecord(L"Float-Test");
	UnlockTable(L"Float-Test");
	StkPlPrintf("Check that all of records (Fcol00) including updated 10 records can be acquired appropriately.");
	for (int j = 0; j < 20; j++) {
		if (((ColumnDataFloat*)(AcquiredRecDat->GetColumn(0)))->GetValue() != 0) {
			StkPlPrintf("...[NG]\r\n");
			delete AcquiredRecDat;
			return -1;
		}
		AcquiredRecDat = AcquiredRecDat->GetNextRecord();
	}
	delete AcquiredRecDat;
	StkPlPrintf("...[OK]\r\n");


	return 0;
}

int BasicFloatTest06()
{
	StkPlPrintf("Preserve table information into \"FloatColumn.std\".");
	LockAllTable(LOCK_SHARE);
	if (SaveData(L"FloatColumn.std") != 0) {
		StkPlPrintf("...[NG]\r\n");
		UnlockAllTable();
		return -1;
	}
	UnlockAllTable();
	StkPlPrintf("...[OK]\r\n");


	StkPlPrintf("Delete all records (search the records whose value of Fcol00 is zero) from table.");
	ColumnData *ColDat[1];
	ColDat[0] = new ColumnDataFloat(L"Fcol00", 0);
	RecordData *RecDat = new RecordData(L"Float-Test", ColDat, 1);
	LockTable(L"Float-Test", LOCK_EXCLUSIVE);
	if (DeleteRecord(RecDat) != 0) {
		StkPlPrintf("...[NG]\r\n");
		UnlockTable(L"Float-Test");
		delete RecDat;
		return -1;
	}
	UnlockTable(L"Float-Test");
	delete RecDat;
	StkPlPrintf("...[OK]\r\n");


	StkPlPrintf("Check that all of records can be deleted from \"Float-Test\" table.");
	if (GetNumOfRecords(L"Float-Test") != 0) {
		StkPlPrintf("...[NG]\r\n");
		return -1;
	}
	StkPlPrintf("...[OK]\r\n");


	StkPlPrintf("Load table information from \"FloatColumn.std\".");
	LockAllTable(LOCK_EXCLUSIVE);
	if (LoadData(L"FloatColumn.std") != 0) {
		StkPlPrintf("...[NG]\r\n");
		UnlockAllTable();
		return -1;
	}
	UnlockAllTable();
	StkPlPrintf("...[OK]\r\n");


	StkPlPrintf("Check the number of records which are in \"Float-Test\" table is 20.");
	if (GetNumOfRecords(L"Float-Test") != 20) {
		StkPlPrintf("...[NG]\r\n");
		return -1;
	}
	StkPlPrintf("...[OK]\r\n");


	return 0;
}

int FloatTest()
{
	if (BasicFloatTest01() == -1) {
		return -1;
	}
	if (BasicFloatTest02() == -1) {
		return -1;
	}
	if (BasicFloatTest03() == -1) {
		return -1;
	}
	if (BasicFloatTest04() == -1) {
		return -1;
	}
	if (BasicFloatTest05() == -1) {
		return -1;
	}
	if (BasicFloatTest06() == -1) {
		return -1;
	}
	return 0;
}
