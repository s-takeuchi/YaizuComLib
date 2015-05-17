#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "..\..\src\stkdata\stkdata.h"


int BasicBinaryTest01()
{
	// 10バイトのバイナリ型カラムを含むテーブル"Bin-Test"をCreateTableで生成できる
	printf("10バイトのバイナリ型カラムを含むテーブル\"Bin-Test\"をCreateTableで生成できる");
	ColumnDefInt ColDefId(_T("ID"));
	ColumnDefBin ColDefImg1(_T("Img1"), 10);
	ColumnDefBin ColDefImg2(_T("Img2"), 10);
	TableDef TabDefTest(_T("Bin-Test"), 100);

	TabDefTest.AddColumnDef(&ColDefId);
	TabDefTest.AddColumnDef(&ColDefImg1);
	TabDefTest.AddColumnDef(&ColDefImg2);
	CreateTable(&TabDefTest);
	printf("...[OK]\r\n");

	// テーブル"Bin-Test"にInsertRecordで1レコードを追加
	{
		printf("テーブル\"Bin-Test\"にInsertRecordで1レコードを追加");
		ColumnData* ColDat[10];
		RecordData* RecDat;
		BYTE one_img1[10] = {0x00, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0xFF, 0xFF};
		BYTE one_img2[10] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x00, 0x66, 0x00, 0x77, 0x00};
		ColDat[0] = new ColumnDataInt(_T("ID"), 10);
		ColDat[1] = new ColumnDataBin(_T("Img1"), one_img1, 10);
		ColDat[2] = new ColumnDataBin(_T("Img2"), one_img2, 10);
		RecDat = new RecordData(_T("Bin-Test"), ColDat, 3);
		LockTable(_T("Bin-Test"), 2);
		InsertRecord(RecDat);
		UnlockTable(_T("Bin-Test"));
		ClearRecordData(RecDat);
		printf("...[OK]\r\n");
	}

	// テーブル"Bin-Test"にInsertRecordで連結されたレコード３個を追加
	{
		printf("テーブル\"Bin-Test\"にInsertRecordで連結されたレコード３個を追加");
		ColumnData* ColDat1[10];
		ColumnData* ColDat2[10];
		ColumnData* ColDat3[10];
		RecordData* RecDat1;
		RecordData* RecDat2;
		RecordData* RecDat3;

		BYTE one_img11[10] = {0xAB, 0xCD, 0xEF, 0x00, 0xFF, 0x10, 0x20, 0x30, 0x40, 0x50};
		BYTE one_img12[10] = {0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};
		ColDat1[0] = new ColumnDataInt(_T("ID"), 777);
		ColDat1[1] = new ColumnDataBin(_T("Img1"), one_img11, 10);
		ColDat1[2] = new ColumnDataBin(_T("Img2"), one_img12, 10);
		RecDat1 = new RecordData(_T("Bin-Test"), ColDat1, 3);

		BYTE one_img21[10] = {0x00, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0xFF, 0xFF};
		BYTE one_img22[10] = {0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};
		ColDat2[0] = new ColumnDataInt(_T("ID"), -500);
		ColDat2[1] = new ColumnDataBin(_T("Img1"), one_img21, 10);
		ColDat2[2] = new ColumnDataBin(_T("Img2"), one_img22, 10);
		RecDat2 = new RecordData(_T("Bin-Test"), ColDat2, 3);
		RecDat1->SetNextRecord(RecDat2);

		BYTE one_img31[10] = {0xAB, 0xCD, 0xEF, 0x00, 0xFF, 0x10, 0x20, 0x30, 0x40, 0x50};
		BYTE one_img32[10] = {0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};
		ColDat3[0] = new ColumnDataInt(_T("ID"), 10);
		ColDat3[1] = new ColumnDataBin(_T("Img1"), one_img31, 10);
		ColDat3[2] = new ColumnDataBin(_T("Img2"), one_img32, 10);
		RecDat3 = new RecordData(_T("Bin-Test"), ColDat3, 3);
		RecDat2->SetNextRecord(RecDat3);

		LockTable(_T("Bin-Test"), 2);
		InsertRecord(RecDat1);
		UnlockTable(_T("Bin-Test"));
		ClearRecordData(RecDat1);
		printf("...[OK]\r\n");
	}

	// テーブル"Bin-Test"にInsertRecordで1レコードを追加
	{
		printf("テーブル\"Bin-Test\"にInsertRecordで1レコードを追加");
		ColumnData* ColDat[10];
		RecordData* RecDat;
		BYTE one_img1[10] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x00, 0x00, 0xFF};
		BYTE one_img2[10] = {0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x00, 0x00, 0x00, 0xFF};
		ColDat[0] = new ColumnDataInt(_T("ID"), 12345);
		ColDat[1] = new ColumnDataBin(_T("Img1"), one_img1, 10);
		ColDat[2] = new ColumnDataBin(_T("Img2"), one_img2, 10);
		RecDat = new RecordData(_T("Bin-Test"), ColDat, 3);
		LockTable(_T("Bin-Test"), 2);
		InsertRecord(RecDat);
		UnlockTable(_T("Bin-Test"));
		ClearRecordData(RecDat);
		printf("...[OK]\r\n");
	}

	return 0;
}

// テーブル"Bin-Test"内の全てのレコードをGetRecrd(_T("Bin-Test"))で取得し，取得した連結レコードが適切であることを確認する
int BasicBinaryTest02()
{
	printf("テーブル\"Bin-Test\"内の全てのレコードをGetRecrd(_T(\"Bin-Test\"))で取得し，取得した連結レコードが適切であることを確認する");
	RecordData* RecDat;
	LockTable(_T("Bin-Test"), 1);
	RecDat = GetRecord(_T("Bin-Test"));
	UnlockTable(_T("Bin-Test"));
	RecordData* TopRecDat = RecDat;

	int Cnt = 0;
	int Err = 0;
	ColumnDataBin* ColBin1;
	ColumnDataBin* ColBin2;
	do {
		ColBin1 = (ColumnDataBin*)RecDat->GetColumn(1);
		ColBin2 = (ColumnDataBin*)RecDat->GetColumn(2);
		if (Cnt == 0) {
			BYTE one_img1[10] = {0x00, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0xFF, 0xFF};
			BYTE one_img2[10] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x00, 0x66, 0x00, 0x77, 0x00};
			if (memcmp(ColBin1->GetValue(), one_img1, 10) != 0 || memcmp(ColBin2->GetValue(), one_img2, 10) != 0) {
				Err = 1;
				break;
			}
		}
		if (Cnt == 2) {
			BYTE one_img1[10] = {0x00, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0xFF, 0xFF};
			BYTE one_img2[10] = {0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};
			if (memcmp(ColBin1->GetValue(), one_img1, 10) != 0 || memcmp(ColBin2->GetValue(), one_img2, 10) != 0) {
				Err = 1;
				break;
			}
		}
		if (Cnt == 4) {
			BYTE one_img1[10] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x00, 0x00, 0xFF};
			BYTE one_img2[10] = {0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x00, 0x00, 0x00, 0xFF};
			if (memcmp(ColBin1->GetValue(), one_img1, 10) != 0 || memcmp(ColBin2->GetValue(), one_img2, 10) != 0) {
				Err = 1;
				break;
			}
		}
		Cnt++;
		RecDat = RecDat->GetNextRecord();
	} while (RecDat);

	ClearRecordData(TopRecDat);

	if (Err == 1 || Cnt != 5) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");
	return 0;
}

// テーブル"Bin-Test"内のレコードをGetRecrd(Img1:バイナリデータ条件指定)で取得し，取得した連結レコードが適切であることを確認する
int BasicBinaryTest04()
{
	printf("テーブル\"Bin-Test\"内のレコードをGetRecrd(Img1:バイナリデータ条件指定)で取得し，取得した連結レコードが適切であることを確認する");
	ColumnData* ColDat[10];
	RecordData* RecDat;
	RecordData* GetRecDat;
	BYTE one_img[10] = {0x00, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0xFF, 0xFF};
	ColDat[0] = new ColumnDataBin(_T("Img1"), one_img, 10);
	RecDat = new RecordData(_T("Bin-Test"), ColDat, 1);
	LockTable(_T("Bin-Test"), 1);
	GetRecDat = GetRecord(RecDat);
	UnlockTable(_T("Bin-Test"));

	RecordData* RecDat1 = GetRecDat;
	BYTE one_img1[10] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x00, 0x66, 0x00, 0x77, 0x00};
	if (memcmp(((ColumnDataBin*)(RecDat1->GetColumn(2)))->GetValue(), one_img1, 10) != 0) {
		printf("...[NG]#1\r\n");
		return -1;
	}
	RecordData* RecDat2 = GetRecDat->GetNextRecord();
	BYTE one_img2[10] = {0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};
	if (memcmp(((ColumnDataBin*)(RecDat2->GetColumn(2)))->GetValue(), one_img2, 10) != 0) {
		printf("...[NG]#2\r\n");
		return -1;
	}
	RecordData* RecDat3 = RecDat2->GetNextRecord();
	if (RecDat3 != NULL) {
		printf("...[NG]#3\r\n");
		return -1;
	}

	ClearRecordData(RecDat);
	ClearRecordData(GetRecDat);
	printf("...[OK]\r\n");
	return 0;
}

// テーブル"Bin-Test"内のレコードをGetRecrd(整数データ条件指定)で取得し，取得した連結レコードが適切であることを確認する
int BasicBinaryTest05()
{
	printf("テーブル\"Bin-Test\"内のレコードをGetRecrd(整数データ条件指定)で取得し，取得した連結レコードが適切であることを確認する");
	ColumnData* ColDat[10];
	RecordData* RecDat;
	RecordData* GetRecDat;
	ColDat[0] = new ColumnDataInt(_T("ID"), 10);
	RecDat = new RecordData(_T("Bin-Test"), ColDat, 1);
	LockTable(_T("Bin-Test"), 1);
	GetRecDat = GetRecord(RecDat);
	UnlockTable(_T("Bin-Test"));

	RecordData* RecDat1 = GetRecDat;
	BYTE one_img1[10] = {0x00, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0xFF, 0xFF};
	if (memcmp(((ColumnDataBin*)(RecDat1->GetColumn(1)))->GetValue(), one_img1, 10) != 0) {
		printf("...[NG]#1\r\n");
		return -1;
	}
	RecordData* RecDat2 = GetRecDat->GetNextRecord();
	BYTE one_img2[10] = {0xAB, 0xCD, 0xEF, 0x00, 0xFF, 0x10, 0x20, 0x30, 0x40, 0x50};
	if (memcmp(((ColumnDataBin*)(RecDat2->GetColumn(1)))->GetValue(), one_img2, 10) != 0) {
		printf("...[NG]#2\r\n");
		return -1;
	}
	RecordData* RecDat3 = RecDat2->GetNextRecord();
	if (RecDat3 != NULL) {
		printf("...[NG]#3\r\n");
		return -1;
	}

	ClearRecordData(RecDat);
	ClearRecordData(GetRecDat);
	printf("...[OK]\r\n");
	return 0;
}

// テーブル"Bin-Test"内のレコードをGetRecrd(Img2:バイナリデータ条件指定)で取得し，取得した連結レコードが適切であることを確認する
int BasicBinaryTest06()
{
	printf("テーブル\"Bin-Test\"内のレコードをGetRecrd(Img2:バイナリデータ条件指定)で取得し，取得した連結レコードが適切であることを確認する");
	ColumnData* ColDat[10];
	RecordData* RecDat;
	RecordData* GetRecDat;
	BYTE one_img[10] = {0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};
	ColDat[0] = new ColumnDataBin(_T("Img2"), one_img, 10);
	RecDat = new RecordData(_T("Bin-Test"), ColDat, 1);
	LockTable(_T("Bin-Test"), 1);
	GetRecDat = GetRecord(RecDat);
	UnlockTable(_T("Bin-Test"));

	RecordData* RecDat1 = GetRecDat;
	BYTE one_img1[10] = {0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};
	if (memcmp(((ColumnDataBin*)(RecDat1->GetColumn(2)))->GetValue(), one_img1, 10) != 0) {
		printf("...[NG]#1\r\n");
		return -1;
	}
	RecordData* RecDat2 = GetRecDat->GetNextRecord();
	BYTE one_img2[10] = {0x00, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0xFF, 0xFF};
	if (memcmp(((ColumnDataBin*)(RecDat2->GetColumn(1)))->GetValue(), one_img2, 10) != 0) {
		printf("...[NG]#2\r\n");
		return -1;
	}
	RecordData* RecDat3 = RecDat2->GetNextRecord();
	RecordData* RecDat4 = RecDat3->GetNextRecord();
	if (RecDat4 != NULL) {
		printf("...[NG]#3\r\n");
		return -1;
	}

	ClearRecordData(RecDat);
	ClearRecordData(GetRecDat);
	printf("...[OK]\r\n");
	return 0;
}

// テーブル"Bin-Test"内のレコードをGetRecrd(Img1, Img2:バイナリデータ条件指定)で取得し，取得した連結レコードが適切であることを確認する
int BasicBinaryTest07()
{
	printf("テーブル\"Bin-Test\"内のレコードをGetRecrd(Img1, Img2:バイナリデータ条件指定)で取得し，取得した連結レコードが適切であることを確認する");
	ColumnData* ColDat[10];
	RecordData* RecDat;
	RecordData* GetRecDat;
	BYTE one_img1[10] = {0x00, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0xFF, 0xFF};
	BYTE one_img2[10] = {0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};
	ColDat[0] = new ColumnDataBin(_T("Img1"), one_img1, 10);
	ColDat[1] = new ColumnDataBin(_T("Img2"), one_img2, 10);
	RecDat = new RecordData(_T("Bin-Test"), ColDat, 2);
	LockTable(_T("Bin-Test"), 1);
	GetRecDat = GetRecord(RecDat);
	UnlockTable(_T("Bin-Test"));

	RecordData* RecDat1 = GetRecDat;
	if (((ColumnDataInt*)(RecDat1->GetColumn(0)))->GetValue() != -500) {
		printf("...[NG]#4\r\n");
		return -1;
	}
	RecordData* RecDat2 = GetRecDat->GetNextRecord();
	if (RecDat2 != NULL) {
		printf("...[NG]#4\r\n");
		return -1;
	}

	ClearRecordData(RecDat);
	ClearRecordData(GetRecDat);
	printf("...[OK]\r\n");
	return 0;
}

// テーブル"Bin-Test"内のレコードをGetRecrd(Img1, Img2:複数の連結されたレコードによるバイナリデータ条件指定)で取得し，取得した連結レコードが適切であることを確認する
int BasicBinaryTest08()
{
	printf("テーブル\"Bin-Test\"内のレコードをGetRecrd(Img1, Img2:複数の連結されたレコードによるバイナリデータ条件指定)で取得し，取得した連結レコードが適切であることを確認する");
	ColumnData* ColDat[10];
	RecordData* RecDat1;
	RecordData* RecDat2;
	RecordData* GetRecDat;

	BYTE one_img1[10] = {0x00, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0xFF, 0xFF};
	ColDat[0] = new ColumnDataBin(_T("Img1"), one_img1, 10);
	RecDat1 = new RecordData(_T("Bin-Test"), ColDat, 1);

	BYTE one_img2[10] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x00, 0x00, 0xFF};
	ColDat[0] = new ColumnDataBin(_T("Img1"), one_img2, 10);
	RecDat2 = new RecordData(_T("Bin-Test"), ColDat, 1);
	RecDat1->SetNextRecord(RecDat2);

	LockTable(_T("Bin-Test"), 1);
	GetRecDat = GetRecord(RecDat1);
	UnlockTable(_T("Bin-Test"));

	RecordData* RecoDat1 = GetRecDat;
	RecordData* RecoDat2 = RecoDat1->GetNextRecord();
	RecordData* RecoDat3 = RecoDat2->GetNextRecord();
	RecordData* RecoDat4 = RecoDat3->GetNextRecord();
	if (RecoDat4 != NULL) {
		printf("...[NG]#4\r\n");
		return -1;
	}

	ClearRecordData(RecDat1);
	ClearRecordData(GetRecDat);
	printf("...[OK]\r\n");
	return 0;
}

// 2つの連結したレコード(バイナリデータカラム有り)を指定してバイナリデータをUpdateRecordで更新する
int BasicBinaryTest09()
{
	printf("2つの連結したレコード(バイナリデータカラム有り)を指定してバイナリデータをUpdateRecordで更新する");
	ColumnData* ColDat[10];
	RecordData* RecDat1;
	RecordData* RecDat2;
	RecordData* RecDat3;
	RecordData* GetRecDat;

	BYTE one_img1[10] = {0x00, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0xFF, 0xFF};
	ColDat[0] = new ColumnDataBin(_T("Img1"), one_img1, 10);
	RecDat1 = new RecordData(_T("Bin-Test"), ColDat, 1);

	BYTE one_img2[10] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x00, 0x00, 0xFF};
	ColDat[0] = new ColumnDataBin(_T("Img1"), one_img2, 10);
	RecDat2 = new RecordData(_T("Bin-Test"), ColDat, 1);
	RecDat1->SetNextRecord(RecDat2);

	BYTE one_img3[10] = {0x39, 0x39, 0x39, 0x39, 0x39, 0x19, 0x19, 0x19, 0x19, 0x19};
	ColDat[0] = new ColumnDataBin(_T("Img2"), one_img3, 10);
	RecDat3 = new RecordData(_T("Bin-Test"), ColDat, 1);

	LockTable(_T("Bin-Test"), 2);
	UpdateRecord(RecDat1, RecDat3);
	UnlockTable(_T("Bin-Test"));

	LockTable(_T("Bin-Test"), 1);
	GetRecDat = GetRecord(RecDat3);
	UnlockTable(_T("Bin-Test"));

	RecordData* Top = GetRecDat;
	for (int i = 0; i < 3; i++) {
		ColumnDataBin* img1 = (ColumnDataBin*)GetRecDat->GetColumn(2);
		if (memcmp(img1->GetValue(), one_img3, 10) != 0) {
			printf("...[NG]#1\r\n");
			return -1;
		}
		ColumnDataInt* Id = (ColumnDataInt*)GetRecDat->GetColumn(0);
		if (i == 0 && Id->GetValue() != 10) {
			printf("...[NG]#2\r\n");
			return -1;
		}
		if (i == 1 && Id->GetValue() != -500) {
			printf("...[NG]#3\r\n");
			return -1;
		}
		if (i == 2 && Id->GetValue() != 12345) {
			printf("...[NG]#4\r\n");
			return -1;
		}
		GetRecDat = GetRecDat->GetNextRecord();
	}
	if (GetRecDat != NULL) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");

	// バイナリ型カラム"Img2"を指定してレコードを降順ソート→適正にソートされている
	{
		printf("バイナリ型カラム\"Img2\"を指定してレコードを降順ソート→適正にソートされている");
		LockTable(_T("Bin-Test"), LOCK_EXCLUSIVE);
		if (ZaSortRecord(_T("Bin-Test"), _T("Img2")) != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		RecordData* Top = GetRecord(_T("Bin-Test"));
		UnlockTable(_T("Bin-Test"));
		RecordData* Cul = Top;
		ColumnDataInt* Col = (ColumnDataInt*)Cul->GetColumn(0);
		int Val = Col->GetValue();
		if (Val != 12345) {
			printf("...[NG]\r\n");
			return -1;
		}
		ClearRecordData(Top);
		printf("...[OK]\r\n");
	}

	// Img1がソートで最小と評価されるレコードを追加後，バイナリ型カラム"Img1"を指定してレコードを昇順ソートする→適正にソートされている
	{
		printf("Img1がソートで最小と評価されるレコードを追加後，バイナリ型カラム\"Img1\"を指定してレコードを昇順ソートする→適正にソートされている");
		ColumnData* ColDat[10];
		RecordData* RecDat;
		BYTE one_img1[10] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
		BYTE one_img2[10] = {0xcc, 0x2a, 0x3b, 0xcc, 0x5d, 0x6e, 0xcc, 0x00, 0x00, 0x00};
		ColDat[0] = new ColumnDataInt(_T("ID"), 9292);
		ColDat[1] = new ColumnDataBin(_T("Img1"), one_img1, 10);
		ColDat[2] = new ColumnDataBin(_T("Img2"), one_img2, 10);
		RecDat = new RecordData(_T("Bin-Test"), ColDat, 3);
		LockTable(_T("Bin-Test"), 2);
		InsertRecord(RecDat);
		UnlockTable(_T("Bin-Test"));
		ClearRecordData(RecDat);

		LockTable(_T("Bin-Test"), LOCK_EXCLUSIVE);
		if (AzSortRecord(_T("Bin-Test"), _T("Img1")) != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		RecordData* Top = GetRecord(_T("Bin-Test"));
		UnlockTable(_T("Bin-Test"));
		RecordData* Cul = Top;
		ColumnDataInt* Col = (ColumnDataInt*)Cul->GetColumn(0);
		int Val = Col->GetValue();
		if (Val != 9292) {
			printf("...[NG]\r\n");
			return -1;
		}
		ClearRecordData(Top);
		printf("...[OK]\r\n");
	}

	// バイナリデータを検索条件にしてレコードを削除する
	printf("バイナリデータを検索条件にしてレコードを削除する");
	LockTable(_T("Bin-Test"), 2);
	DeleteRecord(RecDat3);
	UnlockTable(_T("Bin-Test"));

	ClearRecordData(RecDat1);
	ClearRecordData(RecDat3);
	ClearRecordData(Top);

	if (DeleteTable(_T("Bin-Test")) != 0) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");
	return 0;
}

// 1MB(バイナリ型カラム) * 32 * 10 = 320MBのテーブル"LargeBinTable"をCreateTableで作成し，その後DeleteTableで削除する
// 2MB(バイナリ型カラム) * 32 * 10 = 640MBのテーブル"LargeBinTable"をCreateTableで作成し，その後DeleteTableで削除する
int LargeBinaryTableTest01()
{
	TableDef* LargeBinTable;
	ColumnDefBin* ColDef[32];
	for (int i = 1000000; i < 2000001; i += 1000000) {
		if (i == 1000000) {
			printf("1MB(バイナリ型カラム) * 32 * 10 = 320MBのテーブル\"LargeBinTable\"をCreateTableで作成し，その後DeleteTableで削除する");
		} else {
			printf("2MB(バイナリ型カラム) * 32 * 10 = 640MBのテーブル\"LargeBinTable\"をCreateTableで作成し，その後DeleteTableで削除する");
		}

		LargeBinTable = new TableDef(_T("LargeBinTable"), 10);
		for (int j = 0; j < 32; j++) {
			TCHAR ColName[16];
			wsprintf(ColName, _T("BinBin%d"), j);
			ColDef[j] = new ColumnDefBin(ColName, i);
			LargeBinTable->AddColumnDef(ColDef[j]);
		}
		int Ret = CreateTable(LargeBinTable);
		if (Ret != 0) {
			TCHAR MsgBuf[256];
			wsprintf(MsgBuf, _T("テーブル作成に失敗しました  %d"), i);
			return -1;
		}
		for (int j = 0; j < 32; j++) {
			delete ColDef[j];
		}
		delete LargeBinTable;
		if (DeleteTable(_T("LargeBinTable")) != 0) {
			TCHAR MsgBuf[256];
			wsprintf(MsgBuf, _T("テーブル削除に失敗しました  %d"), i);
			return -1;
		}
		printf("...[OK]\r\n");
	}

	return 0;
}

// 6MB * 64 = 384MBのテーブル"LargeBinTable2"をCreateTableで作成できる
// 6MBのバイナリデータを含むレコードを64回InsertRecordで追加する。
int LargeBinaryTableTest02()
{
	printf("6MB * 64 = 384MBのテーブル\"LargeBinTable2\"をCreateTableで作成できる");
	TableDef* LargeBinTable;
	ColumnDef* ColDef[32];
	LargeBinTable = new TableDef(_T("LargeBinTable2"), 64);
	ColDef[0] = new ColumnDefInt(_T("ID"));
	ColDef[1] = new ColumnDefBin(_T("Data"), 6000000);
	LargeBinTable->AddColumnDef(ColDef[0]);
	LargeBinTable->AddColumnDef(ColDef[1]);
	int Ret = CreateTable(LargeBinTable);
	if (Ret != 0) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");

	printf("6MBのバイナリデータを含むレコードを64回InsertRecordで追加する");
	BYTE *Dat = new BYTE[6000000];
	for (int i = 0; i < 6000000; i++) {
		Dat[i] = 127;
	}
	ColumnData* ColDat[3];
	RecordData* RecDat;
	LockTable(_T("LargeBinTable2"), 2);
	for (int i = 0; i < 64; i++) {
		ColDat[0] = new ColumnDataInt(_T("ID"), i);
		ColDat[1] = new ColumnDataBin(_T("Data"), Dat, 6000000);
		RecDat = new RecordData(_T("LargeBinTable2"), ColDat, 2);
		InsertRecord(RecDat);
		ClearRecordData(RecDat);
	}
	UnlockTable(_T("LargeBinTable2"));
	if (DeleteTable(_T("LargeBinTable2")) != 0) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");

	return 0;
}

/*
BinaryTest
・10バイトのバイナリ型カラムを含むテーブル"Bin-Test"をCreateTableで生成できる
・テーブル"Bin-Test"にInsertRecordで1レコードを追加
・テーブル"Bin-Test"にInsertRecordで連結されたレコード３個を追加
・テーブル"Bin-Test"にInsertRecordで1レコードを追加
・テーブル"Bin-Test"内の全てのレコードをGetRecrd(_T("Bin-Test"))で取得し，取得した連結レコードが適切であることを確認する
・テーブル"Bin-Test"内のレコードをGetRecrd(Img1:バイナリデータ条件指定)で取得し，取得した連結レコードが適切であることを確認する
・テーブル"Bin-Test"内のレコードをGetRecrd(整数データ条件指定)で取得し，取得した連結レコードが適切であることを確認する
・テーブル"Bin-Test"内のレコードをGetRecrd(Img2:バイナリデータ条件指定)で取得し，取得した連結レコードが適切であることを確認する
・テーブル"Bin-Test"内のレコードをGetRecrd(Img1, Img2:バイナリデータ条件指定)で取得し，取得した連結レコードが適切であることを確認する
・テーブル"Bin-Test"内のレコードをGetRecrd(Img1, Img2:複数の連結されたレコードによるバイナリデータ条件指定)で取得し，取得した連結レコードが適切であることを確認する
・2つの連結したレコード(バイナリデータカラム有り)を指定してバイナリデータをUpdateRecordで更新する
・バイナリ型カラム"Img2"を指定してレコードを降順ソート→適正にソートされている
・Img1がソートで最小と評価されるレコードを追加後，バイナリ型カラム"Img1"を指定してレコードを昇順ソートする→適正にソートされている
・バイナリデータを検索条件にしてDeleteRecordでレコードを削除する
・1MB(バイナリ型カラム) * 32 * 10 = 320MBのテーブル"LargeBinTable"をCreateTableで作成し，その後DeleteTableで削除する
・2MB(バイナリ型カラム) * 32 * 10 = 640MBのテーブル"LargeBinTable"をCreateTableで作成し，その後DeleteTableで削除する
・6MB * 64 = 384MBのテーブル"LargeBinTable2"をCreateTableで作成できる
・6MBのバイナリデータを含むレコードを64回InsertRecordで追加する。
*/
int BinaryTest()
{
	if (BasicBinaryTest01() != 0) {
		return -1;
	}
	if (BasicBinaryTest02() != 0) {
		return -1;
	}
	if (BasicBinaryTest04() != 0) {
		return -1;
	}
	if (BasicBinaryTest05() != 0) {
		return -1;
	}
	if (BasicBinaryTest06() != 0) {
		return -1;
	}
	if (BasicBinaryTest07() != 0) {
		return -1;
	}
	if (BasicBinaryTest08() != 0) {
		return -1;
	}
	if (BasicBinaryTest09() != 0) {
		return -1;
	}
	if (LargeBinaryTableTest01() != 0) {
		return -1;
	}
	if (LargeBinaryTableTest02() != 0) {
		return -1;
	}
	return 0;
}
