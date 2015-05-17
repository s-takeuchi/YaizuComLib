#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "..\..\src\com\stkdata\stkdata.h"

int BasicFloatTest01()
{
	printf("32個のFloat型カラムを保持し最大16383個のレコードを格納することができるテーブル\"Float-Test\"を作成する");
	ColumnDefFloat ColDef00(_T("Fcol00"));
	ColumnDefFloat ColDef01(_T("Fcol01"));
	ColumnDefFloat ColDef02(_T("Fcol02"));
	ColumnDefFloat ColDef03(_T("Fcol03"));
	ColumnDefFloat ColDef04(_T("Fcol04"));
	ColumnDefFloat ColDef05(_T("Fcol05"));
	ColumnDefFloat ColDef06(_T("Fcol06"));
	ColumnDefFloat ColDef07(_T("Fcol07"));
	ColumnDefFloat ColDef08(_T("Fcol08"));
	ColumnDefFloat ColDef09(_T("Fcol09"));
	ColumnDefFloat ColDef10(_T("Fcol10"));
	ColumnDefFloat ColDef11(_T("Fcol11"));
	ColumnDefFloat ColDef12(_T("Fcol12"));
	ColumnDefFloat ColDef13(_T("Fcol13"));
	ColumnDefFloat ColDef14(_T("Fcol14"));
	ColumnDefFloat ColDef15(_T("Fcol15"));
	ColumnDefFloat ColDef16(_T("Fcol16"));
	ColumnDefFloat ColDef17(_T("Fcol17"));
	ColumnDefFloat ColDef18(_T("Fcol18"));
	ColumnDefFloat ColDef19(_T("Fcol19"));
	ColumnDefFloat ColDef20(_T("Fcol20"));
	ColumnDefFloat ColDef21(_T("Fcol21"));
	ColumnDefFloat ColDef22(_T("Fcol22"));
	ColumnDefFloat ColDef23(_T("Fcol23"));
	ColumnDefFloat ColDef24(_T("Fcol24"));
	ColumnDefFloat ColDef25(_T("Fcol25"));
	ColumnDefFloat ColDef26(_T("Fcol26"));
	ColumnDefFloat ColDef27(_T("Fcol27"));
	ColumnDefFloat ColDef28(_T("Fcol28"));
	ColumnDefFloat ColDef29(_T("Fcol29"));
	ColumnDefFloat ColDef30(_T("Fcol30"));
	ColumnDefFloat ColDef31(_T("Fcol31"));
	TableDef TabDefTest(_T("Float-Test"), 16383);
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
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");


	printf("テーブル\"Float-Test\"に32個のカラムが存在することを確認する");
	if (GetColumnCount(_T("Float-Test")) != 32) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");


	printf("テーブル\"Float-Test\"の0番目, 1番目, 30番目, 31番目のカラムのサイズが何れも4バイトであることを確認する");
	if (GetColumnSize(_T("Float-Test"), _T("Fcol00")) != 4 ||
		GetColumnSize(_T("Float-Test"), _T("Fcol01")) != 4 ||
		GetColumnSize(_T("Float-Test"), _T("Fcol30")) != 4 ||
		GetColumnSize(_T("Float-Test"), _T("Fcol31")) != 4) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");


	printf("テーブル\"Float-Test\"のFcol10, Fcol20, Fcol30のカラムタイプがFloatであることを確認する");
	if (GetColumnType(_T("Float-Test"), _T("Fcol10")) != COLUMN_TYPE_FLOAT ||
		GetColumnSize(_T("Float-Test"), _T("Fcol20")) != COLUMN_TYPE_FLOAT ||
		GetColumnSize(_T("Float-Test"), _T("Fcol30")) != COLUMN_TYPE_FLOAT) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");

	return 0;
}

int BasicFloatTest02()
{
	printf("\"Float-Test\"テーブルに10個のレコード(with Fcol00～Fcol31カラム)を繰り返し追加する");
	ColumnData *ColDat[32];
	RecordData *RecDat;
	LockTable(_T("Float-Test"), LOCK_EXCLUSIVE);
	for (int j = 0; j < 10; j++) {
		for (int i = 0; i < 32; i++) {
			TCHAR Buf[32];
			wsprintf(Buf, _T("Fcol%02d"), i);
			ColDat[i] = new ColumnDataFloat(Buf, (float)(100.0 + 0.1 * j + i * 0.001));
		}
		RecDat = new RecordData(_T("Float-Test"), ColDat, 32);
		if (InsertRecord(RecDat) != 0) {
			printf("...[NG]\r\n");
			ClearRecordData(RecDat);
			UnlockTable(_T("Float-Test"));
			return -1;
		}
		ClearRecordData(RecDat);
	}
	UnlockTable(_T("Float-Test"));
	printf("...[OK]\r\n");


	printf("\"Float-Test\"テーブルに存在するレコードの数が10個であることを確認する");
	if (GetNumOfRecords(_T("Float-Test")) != 10) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");


	printf("\"Float-Test\"テーブルに追加した10個のレコード(with Fcol00～Fcol31カラム)の情報が正しく取得できることを確認する");
	RecordData *AcquiredRecDat;
	LockTable(_T("Float-Test"), LOCK_SHARE);
	AcquiredRecDat = GetRecord(_T("Float-Test"));
	UnlockTable(_T("Float-Test"));
	for (int j = 0; j < 10; j++) {
		for (int i = 0; i < 32; i++) {
			if (((ColumnDataFloat*)(AcquiredRecDat->GetColumn(i)))->GetValue() != (float)(100.0 + 0.1 * j + i * 0.001)) {
				printf("...[NG]\r\n");
				ClearRecordData(AcquiredRecDat);
				return -1;
			}
		}
		AcquiredRecDat = AcquiredRecDat->GetNextRecord();
	}
	ClearRecordData(AcquiredRecDat);
	printf("...[OK]\r\n");

	return 0;
}

int BasicFloatTest03()
{
	printf("\"Float-Test\"テーブルの10個のレコード(Fcol00およびFcol01)を繰り返し更新する");
	ColumnData *ColDat[1];
	ColumnData *UpdColDat[2];
	RecordData *RecDat;
	RecordData *UpdRecDat;
	LockTable(_T("Float-Test"), LOCK_EXCLUSIVE);
	for (int i = 0; i < 10; i++) {
		ColDat[0] = new ColumnDataFloat(_T("Fcol00"), (float)(100.0 + 0.1 * i));
		RecDat = new RecordData(_T("Float-Test"), ColDat, 1);
		UpdColDat[0] = new ColumnDataFloat(_T("Fcol00"), 0);
		UpdColDat[1] = new ColumnDataFloat(_T("Fcol01"), 999999000000.0f);
		UpdRecDat = new RecordData(_T("Float-Test"), UpdColDat, 2);
		if (UpdateRecord(RecDat, UpdRecDat) != 0) {
			printf("...[NG]\r\n");
			ClearRecordData(RecDat);
			ClearRecordData(UpdRecDat);
			UnlockTable(_T("Float-Test"));
		}
		ClearRecordData(RecDat);
		ClearRecordData(UpdRecDat);
	}
	UnlockTable(_T("Float-Test"));
	printf("...[OK]\r\n");


	RecordData *AcquiredRecDat;
	LockTable(_T("Float-Test"), LOCK_SHARE);
	AcquiredRecDat = GetRecord(_T("Float-Test"));
	UnlockTable(_T("Float-Test"));
	printf("更新した10個のレコード(with Fcol00～Fcol31カラム)の情報が正しく取得できることを確認する");
	for (int j = 0; j < 10; j++) {
		for (int i = 0; i < 32; i++) {
			if (i == 0) {
				if (((ColumnDataFloat*)(AcquiredRecDat->GetColumn(i)))->GetValue() != 0) {
					printf("...[NG]\r\n");
					ClearRecordData(AcquiredRecDat);
					return -1;
				}
			} else if (i == 1) {
			} else if (((ColumnDataFloat*)(AcquiredRecDat->GetColumn(i)))->GetValue() != (float)(100.0 + 0.1 * j + i * 0.001)) {
				printf("...[NG]\r\n");
				ClearRecordData(AcquiredRecDat);
				return -1;
			}
		}
		AcquiredRecDat = AcquiredRecDat->GetNextRecord();
	}
	ClearRecordData(AcquiredRecDat);
	printf("...[OK]\r\n");

	return 0;
}

int BasicFloatTest04()
{
	printf("\"Float-Test\"テーブルにさらに10個のレコード(with Fcol00～Fcol31カラム)を一度に追加する");
	ColumnData *ColDat[32];
	RecordData *RecDat;
	RecordData *HeadRecDat;
	RecordData *LastRecDat;
	for (int j = 0; j < 10; j++) {
		for (int i = 0; i < 32; i++) {
			TCHAR Buf[32];
			wsprintf(Buf, _T("Fcol%02d"), i);
			ColDat[i] = new ColumnDataFloat(Buf, (float)(200.0 + 0.1 * j + i * 0.001));
		}
		RecDat = new RecordData(_T("Float-Test"), ColDat, 32);
		if (j >= 1) {
			LastRecDat->SetNextRecord(RecDat);
		} else {
			HeadRecDat = RecDat;
		}
		LastRecDat = RecDat;
	}
	LockTable(_T("Float-Test"), LOCK_EXCLUSIVE);
	if (InsertRecord(HeadRecDat) != 0) {
		printf("...[NG]\r\n");
		ClearRecordData(HeadRecDat);
		UnlockTable(_T("Float-Test"));
		return -1;
	}
	UnlockTable(_T("Float-Test"));
	ClearRecordData(HeadRecDat);
	printf("...[OK]\r\n");


	printf("\"Float-Test\"テーブルに存在するレコードの数が20個であることを確認する");
	if (GetNumOfRecords(_T("Float-Test")) != 20) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");


	printf("\"Float-Test\"テーブルに存在する20個のレコードのうち条件指定で10個のレコードの情報が適切に取得できることを確認する");
	RecordData *AcquiredRecDat;
	for (int i = 0; i < 10; i++) {
		ColDat[0] = new ColumnDataFloat(_T("Fcol00"), (float)(200.0 + 0.1 * i));
		RecDat = new RecordData(_T("Float-Test"), ColDat, 1);
		if (i >= 1) {
			LastRecDat->SetNextRecord(RecDat);
		} else {
			HeadRecDat = RecDat;
		}
		LastRecDat = RecDat;
	}
	LockTable(_T("Float-Test"), LOCK_SHARE);
	AcquiredRecDat = GetRecord(HeadRecDat);
	UnlockTable(_T("Float-Test"));
	ClearRecordData(HeadRecDat);
	if (AcquiredRecDat == NULL) {
		printf("...[NG]\r\n");
		return -1;
	}
	for (int j = 0; j < 10; j++) {
		for (int i = 0; i < 32; i++) {
			if (((ColumnDataFloat*)(AcquiredRecDat->GetColumn(i)))->GetValue() != (float)(200.0 + 0.1 * j + i * 0.001)) {
				printf("...[NG]\r\n");
				ClearRecordData(AcquiredRecDat);
				return -1;
			}
		}
		AcquiredRecDat = AcquiredRecDat->GetNextRecord();
	}
	ClearRecordData(AcquiredRecDat);
	printf("...[OK]\r\n");

	return 0;
}

int BasicFloatTest05()
{
	printf("\"Float-Test\"テーブルの10個のレコード(Fcol00およびFcol01)を一度に更新する");
	ColumnData *ColDat[1];
	RecordData *RecDat;
	RecordData *LastRecDat;
	RecordData *HeadRecDat;
	ColumnData *UpdColDat[2];
	RecordData *UpdRecDat;
	LockTable(_T("Float-Test"), LOCK_EXCLUSIVE);
	for (int i = 0; i < 10; i++) {
		ColDat[0] = new ColumnDataFloat(_T("Fcol00"), (float)(200.0 + 0.1 * i));
		RecDat = new RecordData(_T("Float-Test"), ColDat, 1);
		if (i >= 1) {
			LastRecDat->SetNextRecord(RecDat);
		} else {
			HeadRecDat = RecDat;
		}
		LastRecDat = RecDat;
	}
	UpdColDat[0] = new ColumnDataFloat(_T("Fcol00"), 0);
	UpdColDat[1] = new ColumnDataFloat(_T("Fcol01"), 999999000000.0f);
	UpdRecDat = new RecordData(_T("Float-Test"), UpdColDat, 2);
	if (UpdateRecord(HeadRecDat, UpdRecDat) != 0) {
		printf("...[NG]\r\n");
		ClearRecordData(HeadRecDat);
		ClearRecordData(UpdRecDat);
		UnlockTable(_T("Float-Test"));
	}
	ClearRecordData(HeadRecDat);
	ClearRecordData(UpdRecDat);
	UnlockTable(_T("Float-Test"));
	printf("...[OK]\r\n");


	RecordData *AcquiredRecDat;
	LockTable(_T("Float-Test"), LOCK_SHARE);
	AcquiredRecDat = GetRecord(_T("Float-Test"));
	UnlockTable(_T("Float-Test"));
	printf("更新した10個のレコードを含むすべてのレコード(20個)の情報(Fcol00)が正しく取得できることを確認する");
	for (int j = 0; j < 20; j++) {
		if (((ColumnDataFloat*)(AcquiredRecDat->GetColumn(0)))->GetValue() != 0) {
			printf("...[NG]\r\n");
			ClearRecordData(AcquiredRecDat);
			return -1;
		}
		AcquiredRecDat = AcquiredRecDat->GetNextRecord();
	}
	ClearRecordData(AcquiredRecDat);
	printf("...[OK]\r\n");


	return 0;
}

int BasicFloatTest06()
{
	printf("テーブル情報を\"FloatColumn.std\"に保存する。");
	LockAllTable(LOCK_SHARE);
	if (SaveData(_T("FloatColumn.std")) != 0) {
		printf("...[NG]\r\n");
		UnlockAllTable();
		return -1;
	}
	UnlockAllTable();
	printf("...[OK]\r\n");


	printf("すべてのレコード（Fcol00の値が0となるレコードを検索）をテーブルから削除する。");
	ColumnData *ColDat[1];
	ColDat[0] = new ColumnDataFloat(_T("Fcol00"), 0);
	RecordData *RecDat = new RecordData(_T("Float-Test"), ColDat, 1);
	LockTable(_T("Float-Test"), LOCK_EXCLUSIVE);
	if (DeleteRecord(RecDat) != 0) {
		printf("...[NG]\r\n");
		UnlockTable(_T("Float-Test"));
		ClearRecordData(RecDat);
		return -1;
	}
	UnlockTable(_T("Float-Test"));
	ClearRecordData(RecDat);
	printf("...[OK]\r\n");


	printf("すべてのレコードが\"Float-Test\"テーブルから削除されていることを確認する");
	if (GetNumOfRecords(_T("Float-Test")) != 0) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");


	printf("テーブル情報を\"FloatColumn.std\"から読み込む。");
	LockAllTable(LOCK_EXCLUSIVE);
	if (LoadData(_T("FloatColumn.std")) != 0) {
		printf("...[NG]\r\n");
		UnlockAllTable();
		return -1;
	}
	UnlockAllTable();
	printf("...[OK]\r\n");


	printf("\"Float-Test\"テーブルに存在するレコードの数が20個であることを確認する");
	if (GetNumOfRecords(_T("Float-Test")) != 20) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");


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
