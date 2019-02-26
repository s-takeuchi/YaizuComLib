#include "../../src/StkPl.h"
#include "..\..\src\stkdata\stkdata.h"
#include "..\..\src\stkdata\stkdataapi.h"


int BasicBinaryTest01()
{
	// 10バイトのバイナリ型カラムを含むテーブル"Bin-Test"をCreateTableで生成できる
	StkPlPrintf("A table 'Bin-Test' including 10 bytes binary column can be created by 'CreateTable'.");
	ColumnDefInt ColDefId(L"ID");
	ColumnDefBin ColDefImg1(L"Img1", 10);
	ColumnDefBin ColDefImg2(L"Img2", 10);
	TableDef TabDefTest(L"Bin-Test", 100);

	TabDefTest.AddColumnDef(&ColDefId);
	TabDefTest.AddColumnDef(&ColDefImg1);
	TabDefTest.AddColumnDef(&ColDefImg2);
	CreateTable(&TabDefTest);
	StkPlPrintf("...[OK]\r\n");

	// テーブル"Bin-Test"にInsertRecordで1レコードを追加
	{
		StkPlPrintf("Insert one record to 'Bin-Test' table.");
		ColumnData* ColDat[10];
		RecordData* RecDat;
		unsigned char one_img1[10] = {0x00, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0xFF, 0xFF};
		unsigned char one_img2[10] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x00, 0x66, 0x00, 0x77, 0x00};
		ColDat[0] = new ColumnDataInt(L"ID", 10);
		ColDat[1] = new ColumnDataBin(L"Img1", one_img1, 10);
		ColDat[2] = new ColumnDataBin(L"Img2", one_img2, 10);
		RecDat = new RecordData(L"Bin-Test", ColDat, 3);
		LockTable(L"Bin-Test", 2);
		InsertRecord(RecDat);
		UnlockTable(L"Bin-Test");
		delete RecDat;
		StkPlPrintf("...[OK]\r\n");
	}

	// テーブル"Bin-Test"にInsertRecordで連結されたレコード３個を追加
	{
		StkPlPrintf("Insert 3 records connected each other to table 'Bin-Test'.");
		ColumnData* ColDat1[10];
		ColumnData* ColDat2[10];
		ColumnData* ColDat3[10];
		RecordData* RecDat1;
		RecordData* RecDat2;
		RecordData* RecDat3;

		unsigned char one_img11[10] = {0xAB, 0xCD, 0xEF, 0x00, 0xFF, 0x10, 0x20, 0x30, 0x40, 0x50};
		unsigned char one_img12[10] = {0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};
		ColDat1[0] = new ColumnDataInt(L"ID", 777);
		ColDat1[1] = new ColumnDataBin(L"Img1", one_img11, 10);
		ColDat1[2] = new ColumnDataBin(L"Img2", one_img12, 10);
		RecDat1 = new RecordData(L"Bin-Test", ColDat1, 3);

		unsigned char one_img21[10] = {0x00, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0xFF, 0xFF};
		unsigned char one_img22[10] = {0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};
		ColDat2[0] = new ColumnDataInt(L"ID", -500);
		ColDat2[1] = new ColumnDataBin(L"Img1", one_img21, 10);
		ColDat2[2] = new ColumnDataBin(L"Img2", one_img22, 10);
		RecDat2 = new RecordData(L"Bin-Test", ColDat2, 3);
		RecDat1->SetNextRecord(RecDat2);

		unsigned char one_img31[10] = {0xAB, 0xCD, 0xEF, 0x00, 0xFF, 0x10, 0x20, 0x30, 0x40, 0x50};
		unsigned char one_img32[10] = {0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};
		ColDat3[0] = new ColumnDataInt(L"ID", 10);
		ColDat3[1] = new ColumnDataBin(L"Img1", one_img31, 10);
		ColDat3[2] = new ColumnDataBin(L"Img2", one_img32, 10);
		RecDat3 = new RecordData(L"Bin-Test", ColDat3, 3);
		RecDat2->SetNextRecord(RecDat3);

		LockTable(L"Bin-Test", 2);
		InsertRecord(RecDat1);
		UnlockTable(L"Bin-Test");
		delete RecDat1;
		StkPlPrintf("...[OK]\r\n");
	}

	// テーブル"Bin-Test"にInsertRecordで1レコードを追加
	{
		StkPlPrintf("Insert one record to 'Bin-Test' table using 'InsertRecord'.");
		ColumnData* ColDat[10];
		RecordData* RecDat;
		unsigned char one_img1[10] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x00, 0x00, 0xFF};
		unsigned char one_img2[10] = {0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x00, 0x00, 0x00, 0xFF};
		ColDat[0] = new ColumnDataInt(L"ID", 12345);
		ColDat[1] = new ColumnDataBin(L"Img1", one_img1, 10);
		ColDat[2] = new ColumnDataBin(L"Img2", one_img2, 10);
		RecDat = new RecordData(L"Bin-Test", ColDat, 3);
		LockTable(L"Bin-Test", 2);
		InsertRecord(RecDat);
		UnlockTable(L"Bin-Test");
		delete RecDat;
		StkPlPrintf("...[OK]\r\n");
	}

	return 0;
}

// テーブル"Bin-Test"内の全てのレコードをGetRecrd(L"Bin-Test")で取得し，取得した連結レコードが適切であることを確認する
int BasicBinaryTest02()
{
	StkPlPrintf("Acquire all records in 'Bin-Test' table using GetRecrd(L\"Bin-Test\") and then, confirm that acquired records are appropriate.");
	RecordData* RecDat;
	LockTable(L"Bin-Test", 1);
	RecDat = GetRecord(L"Bin-Test");
	UnlockTable(L"Bin-Test");
	RecordData* TopRecDat = RecDat;

	int Cnt = 0;
	int Err = 0;
	ColumnDataBin* ColBin1;
	ColumnDataBin* ColBin2;
	do {
		ColBin1 = (ColumnDataBin*)RecDat->GetColumn(1);
		ColBin2 = (ColumnDataBin*)RecDat->GetColumn(2);
		if (Cnt == 0) {
			unsigned char one_img1[10] = {0x00, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0xFF, 0xFF};
			unsigned char one_img2[10] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x00, 0x66, 0x00, 0x77, 0x00};
			if (StkPlMemCmp(ColBin1->GetValue(), one_img1, 10) != 0 || StkPlMemCmp(ColBin2->GetValue(), one_img2, 10) != 0) {
				Err = 1;
				break;
			}
		}
		if (Cnt == 2) {
			unsigned char one_img1[10] = {0x00, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0xFF, 0xFF};
			unsigned char one_img2[10] = {0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};
			if (StkPlMemCmp(ColBin1->GetValue(), one_img1, 10) != 0 || StkPlMemCmp(ColBin2->GetValue(), one_img2, 10) != 0) {
				Err = 1;
				break;
			}
		}
		if (Cnt == 4) {
			unsigned char one_img1[10] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x00, 0x00, 0xFF};
			unsigned char one_img2[10] = {0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x00, 0x00, 0x00, 0xFF};
			if (StkPlMemCmp(ColBin1->GetValue(), one_img1, 10) != 0 || StkPlMemCmp(ColBin2->GetValue(), one_img2, 10) != 0) {
				Err = 1;
				break;
			}
		}
		Cnt++;
		RecDat = RecDat->GetNextRecord();
	} while (RecDat);

	delete TopRecDat;

	if (Err == 1 || Cnt != 5) {
		StkPlPrintf("...[NG]\r\n");
		return -1;
	}
	StkPlPrintf("...[OK]\r\n");
	return 0;
}

// テーブル"Bin-Test"内のレコードをGetRecrd(Img1:バイナリデータ条件指定)で取得し，取得した連結レコードが適切であることを確認する
int BasicBinaryTest04()
{
	StkPlPrintf("Acquire records in 'Bin-Test' using GetRecrd(Img1:Binary data criteria) and then, confirm that acquired records are appropriate.");
	ColumnData* ColDat[10];
	RecordData* RecDat;
	RecordData* GetRecDat;
	unsigned char one_img[10] = {0x00, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0xFF, 0xFF};
	ColDat[0] = new ColumnDataBin(L"Img1", one_img, 10);
	RecDat = new RecordData(L"Bin-Test", ColDat, 1);
	LockTable(L"Bin-Test", 1);
	GetRecDat = GetRecord(RecDat);
	UnlockTable(L"Bin-Test");

	RecordData* RecDat1 = GetRecDat;
	unsigned char one_img1[10] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x00, 0x66, 0x00, 0x77, 0x00};
	if (StkPlMemCmp(((ColumnDataBin*)(RecDat1->GetColumn(2)))->GetValue(), one_img1, 10) != 0) {
		StkPlPrintf("...[NG]#1\r\n");
		return -1;
	}
	RecordData* RecDat2 = GetRecDat->GetNextRecord();
	unsigned char one_img2[10] = {0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};
	if (StkPlMemCmp(((ColumnDataBin*)(RecDat2->GetColumn(2)))->GetValue(), one_img2, 10) != 0) {
		StkPlPrintf("...[NG]#2\r\n");
		return -1;
	}
	RecordData* RecDat3 = RecDat2->GetNextRecord();
	if (RecDat3 != NULL) {
		StkPlPrintf("...[NG]#3\r\n");
		return -1;
	}

	delete RecDat;
	delete GetRecDat;
	StkPlPrintf("...[OK]\r\n");
	return 0;
}

// テーブル"Bin-Test"内のレコードをGetRecrd(整数データ条件指定)で取得し，取得した連結レコードが適切であることを確認する
int BasicBinaryTest05()
{
	StkPlPrintf("Acquire records in 'Bin-Test' using GetRecrd(integer data criteria) and then, confirm that acquired records are appropriate.");
	ColumnData* ColDat[10];
	RecordData* RecDat;
	RecordData* GetRecDat;
	ColDat[0] = new ColumnDataInt(L"ID", 10);
	RecDat = new RecordData(L"Bin-Test", ColDat, 1);
	LockTable(L"Bin-Test", 1);
	GetRecDat = GetRecord(RecDat);
	UnlockTable(L"Bin-Test");

	RecordData* RecDat1 = GetRecDat;
	unsigned char one_img1[10] = {0x00, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0xFF, 0xFF};
	if (StkPlMemCmp(((ColumnDataBin*)(RecDat1->GetColumn(1)))->GetValue(), one_img1, 10) != 0) {
		StkPlPrintf("...[NG]#1\r\n");
		return -1;
	}
	RecordData* RecDat2 = GetRecDat->GetNextRecord();
	unsigned char one_img2[10] = {0xAB, 0xCD, 0xEF, 0x00, 0xFF, 0x10, 0x20, 0x30, 0x40, 0x50};
	if (StkPlMemCmp(((ColumnDataBin*)(RecDat2->GetColumn(1)))->GetValue(), one_img2, 10) != 0) {
		StkPlPrintf("...[NG]#2\r\n");
		return -1;
	}
	RecordData* RecDat3 = RecDat2->GetNextRecord();
	if (RecDat3 != NULL) {
		StkPlPrintf("...[NG]#3\r\n");
		return -1;
	}

	delete RecDat;
	delete GetRecDat;
	StkPlPrintf("...[OK]\r\n");
	return 0;
}

// テーブル"Bin-Test"内のレコードをGetRecrd(Img2:バイナリデータ条件指定)で取得し，取得した連結レコードが適切であることを確認する
int BasicBinaryTest06()
{
	StkPlPrintf("Acquire records in 'Bin-Test' using GetRecrd(Img2:Binary data criteria) and then, confirm that acquired records are appropriate.");
	ColumnData* ColDat[10];
	RecordData* RecDat;
	RecordData* GetRecDat;
	unsigned char one_img[10] = {0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};
	ColDat[0] = new ColumnDataBin(L"Img2", one_img, 10);
	RecDat = new RecordData(L"Bin-Test", ColDat, 1);
	LockTable(L"Bin-Test", 1);
	GetRecDat = GetRecord(RecDat);
	UnlockTable(L"Bin-Test");

	RecordData* RecDat1 = GetRecDat;
	unsigned char one_img1[10] = {0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};
	if (StkPlMemCmp(((ColumnDataBin*)(RecDat1->GetColumn(2)))->GetValue(), one_img1, 10) != 0) {
		StkPlPrintf("...[NG]#1\r\n");
		return -1;
	}
	RecordData* RecDat2 = GetRecDat->GetNextRecord();
	unsigned char one_img2[10] = {0x00, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0xFF, 0xFF};
	if (StkPlMemCmp(((ColumnDataBin*)(RecDat2->GetColumn(1)))->GetValue(), one_img2, 10) != 0) {
		StkPlPrintf("...[NG]#2\r\n");
		return -1;
	}
	RecordData* RecDat3 = RecDat2->GetNextRecord();
	RecordData* RecDat4 = RecDat3->GetNextRecord();
	if (RecDat4 != NULL) {
		StkPlPrintf("...[NG]#3\r\n");
		return -1;
	}

	delete RecDat;
	delete GetRecDat;
	StkPlPrintf("...[OK]\r\n");
	return 0;
}

// テーブル"Bin-Test"内のレコードをGetRecrd(Img1, Img2:バイナリデータ条件指定)で取得し，取得した連結レコードが適切であることを確認する
int BasicBinaryTest07()
{
	StkPlPrintf("Acquire records in 'Bin-Test' using GetRecrd(Img1, Img2:Binary data criteria) and then, confirm that acquired records are appropriate.");
	ColumnData* ColDat[10];
	RecordData* RecDat;
	RecordData* GetRecDat;
	unsigned char one_img1[10] = {0x00, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0xFF, 0xFF};
	unsigned char one_img2[10] = {0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0xFF};
	ColDat[0] = new ColumnDataBin(L"Img1", one_img1, 10);
	ColDat[1] = new ColumnDataBin(L"Img2", one_img2, 10);
	RecDat = new RecordData(L"Bin-Test", ColDat, 2);
	LockTable(L"Bin-Test", 1);
	GetRecDat = GetRecord(RecDat);
	UnlockTable(L"Bin-Test");

	RecordData* RecDat1 = GetRecDat;
	if (((ColumnDataInt*)(RecDat1->GetColumn(0)))->GetValue() != -500) {
		StkPlPrintf("...[NG]#4\r\n");
		return -1;
	}
	RecordData* RecDat2 = GetRecDat->GetNextRecord();
	if (RecDat2 != NULL) {
		StkPlPrintf("...[NG]#4\r\n");
		return -1;
	}

	delete RecDat;
	delete GetRecDat;
	StkPlPrintf("...[OK]\r\n");
	return 0;
}

// テーブル"Bin-Test"内のレコードをGetRecrd(Img1, Img2:複数の連結されたレコードによるバイナリデータ条件指定)で取得し，取得した連結レコードが適切であることを確認する
int BasicBinaryTest08()
{
	StkPlPrintf("Acquire records in 'Bin-Test' using GetRecrd(Img1, Img2:Binary data criteria by multi connected records) and then, confirm that acquired records are appropriate.");
	ColumnData* ColDat[10];
	RecordData* RecDat1;
	RecordData* RecDat2;
	RecordData* GetRecDat;

	unsigned char one_img1[10] = {0x00, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0xFF, 0xFF};
	ColDat[0] = new ColumnDataBin(L"Img1", one_img1, 10);
	RecDat1 = new RecordData(L"Bin-Test", ColDat, 1);

	unsigned char one_img2[10] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x00, 0x00, 0xFF};
	ColDat[0] = new ColumnDataBin(L"Img1", one_img2, 10);
	RecDat2 = new RecordData(L"Bin-Test", ColDat, 1);
	RecDat1->SetNextRecord(RecDat2);

	LockTable(L"Bin-Test", 1);
	GetRecDat = GetRecord(RecDat1);
	UnlockTable(L"Bin-Test");

	RecordData* RecoDat1 = GetRecDat;
	RecordData* RecoDat2 = RecoDat1->GetNextRecord();
	RecordData* RecoDat3 = RecoDat2->GetNextRecord();
	RecordData* RecoDat4 = RecoDat3->GetNextRecord();
	if (RecoDat4 != NULL) {
		StkPlPrintf("...[NG]#4\r\n");
		return -1;
	}

	delete RecDat1;
	delete GetRecDat;
	StkPlPrintf("...[OK]\r\n");
	return 0;
}

// 2つの連結したレコード(バイナリデータカラム有り)を指定してバイナリデータをUpdateRecordで更新する
int BasicBinaryTest09()
{
	StkPlPrintf("Update binary data using UpdateRecord by two linked records (including binary column) specification.");
	ColumnData* ColDat[10];
	RecordData* RecDat1;
	RecordData* RecDat2;
	RecordData* RecDat3;
	RecordData* GetRecDat;

	unsigned char one_img1[10] = {0x00, 0xFF, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0xFF, 0xFF};
	ColDat[0] = new ColumnDataBin(L"Img1", one_img1, 10);
	RecDat1 = new RecordData(L"Bin-Test", ColDat, 1);

	unsigned char one_img2[10] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x00, 0x00, 0xFF};
	ColDat[0] = new ColumnDataBin(L"Img1", one_img2, 10);
	RecDat2 = new RecordData(L"Bin-Test", ColDat, 1);
	RecDat1->SetNextRecord(RecDat2);

	unsigned char one_img3[10] = {0x39, 0x39, 0x39, 0x39, 0x39, 0x19, 0x19, 0x19, 0x19, 0x19};
	ColDat[0] = new ColumnDataBin(L"Img2", one_img3, 10);
	RecDat3 = new RecordData(L"Bin-Test", ColDat, 1);

	LockTable(L"Bin-Test", 2);
	UpdateRecord(RecDat1, RecDat3);
	UnlockTable(L"Bin-Test");

	LockTable(L"Bin-Test", 1);
	GetRecDat = GetRecord(RecDat3);
	UnlockTable(L"Bin-Test");

	RecordData* Top = GetRecDat;
	for (int i = 0; i < 3; i++) {
		ColumnDataBin* img1 = (ColumnDataBin*)GetRecDat->GetColumn(2);
		if (StkPlMemCmp(img1->GetValue(), one_img3, 10) != 0) {
			StkPlPrintf("...[NG]#1\r\n");
			return -1;
		}
		ColumnDataInt* Id = (ColumnDataInt*)GetRecDat->GetColumn(0);
		if (i == 0 && Id->GetValue() != 10) {
			StkPlPrintf("...[NG]#2\r\n");
			return -1;
		}
		if (i == 1 && Id->GetValue() != -500) {
			StkPlPrintf("...[NG]#3\r\n");
			return -1;
		}
		if (i == 2 && Id->GetValue() != 12345) {
			StkPlPrintf("...[NG]#4\r\n");
			return -1;
		}
		GetRecDat = GetRecDat->GetNextRecord();
	}
	if (GetRecDat != NULL) {
		StkPlPrintf("...[NG]\r\n");
		return -1;
	}
	StkPlPrintf("...[OK]\r\n");

	// バイナリ型カラム"Img2"を指定してレコードを降順ソート→適正にソートされている
	{
		StkPlPrintf("Sort records with binary column 'Img2' specification and then, check the records are appropriately sorted.");
		LockTable(L"Bin-Test", LOCK_EXCLUSIVE);
		if (ZaSortRecord(L"Bin-Test", L"Img2") != 0) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
		RecordData* Top = GetRecord(L"Bin-Test");
		UnlockTable(L"Bin-Test");
		RecordData* Cul = Top;
		ColumnDataInt* Col = (ColumnDataInt*)Cul->GetColumn(0);
		int Val = Col->GetValue();
		if (Val != 12345) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
		delete Top;
		StkPlPrintf("...[OK]\r\n");
	}

	// Img1がソートで最小と評価されるレコードを追加後，バイナリ型カラム"Img1"を指定してレコードを昇順ソートする→適正にソートされている
	{
		StkPlPrintf("Sort records with binary column 'Img1' specification after addition of record that Img1 is evaluated as minimum and then, check the records are appropriately sorted.");
		ColumnData* ColDat[10];
		RecordData* RecDat;
		unsigned char one_img1[10] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
		unsigned char one_img2[10] = {0xcc, 0x2a, 0x3b, 0xcc, 0x5d, 0x6e, 0xcc, 0x00, 0x00, 0x00};
		ColDat[0] = new ColumnDataInt(L"ID", 9292);
		ColDat[1] = new ColumnDataBin(L"Img1", one_img1, 10);
		ColDat[2] = new ColumnDataBin(L"Img2", one_img2, 10);
		RecDat = new RecordData(L"Bin-Test", ColDat, 3);
		LockTable(L"Bin-Test", 2);
		InsertRecord(RecDat);
		UnlockTable(L"Bin-Test");
		delete RecDat;

		LockTable(L"Bin-Test", LOCK_EXCLUSIVE);
		if (AzSortRecord(L"Bin-Test", L"Img1") != 0) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
		RecordData* Top = GetRecord(L"Bin-Test");
		UnlockTable(L"Bin-Test");
		RecordData* Cul = Top;
		ColumnDataInt* Col = (ColumnDataInt*)Cul->GetColumn(0);
		int Val = Col->GetValue();
		if (Val != 9292) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
		delete Top;
		StkPlPrintf("...[OK]\r\n");
	}

	// バイナリデータを検索条件にしてレコードを削除する
	StkPlPrintf("Delete records by the specification of binary data criteria.");
	LockTable(L"Bin-Test", 2);
	DeleteRecord(RecDat3);
	UnlockTable(L"Bin-Test");

	delete RecDat1;
	delete RecDat3;
	delete Top;

	if (DeleteTable(L"Bin-Test") != 0) {
		StkPlPrintf("...[NG]\r\n");
		return -1;
	}
	StkPlPrintf("...[OK]\r\n");
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
			StkPlPrintf("CreateTable 1MB(binary column) * 32 * 10 = 320MB table \"LargeBinTable\" and then, delete it with DeleteTable.");
		} else {
			StkPlPrintf("CreateTable 2MB(binary column) * 32 * 10 = 640MB table \"LargeBinTable\" and then, delete it with DeleteTable.");
		}

		LargeBinTable = new TableDef(L"LargeBinTable", 10);
		for (int j = 0; j < 32; j++) {
			wchar_t ColName[16];
			StkPlSwPrintf(ColName, 16, L"BinBin%d", j);
			ColDef[j] = new ColumnDefBin(ColName, i);
			LargeBinTable->AddColumnDef(ColDef[j]);
		}
		int Ret = CreateTable(LargeBinTable);
		if (Ret != 0) {
			wchar_t MsgBuf[256];
			StkPlSwPrintf(MsgBuf, 256, L"Failed to create table.  %d", i);
			return -1;
		}
		for (int j = 0; j < 32; j++) {
			delete ColDef[j];
		}
		delete LargeBinTable;
		if (DeleteTable(L"LargeBinTable") != 0) {
			wchar_t MsgBuf[256];
			StkPlSwPrintf(MsgBuf, 256, L"Failed to delete table.  %d", i);
			return -1;
		}
		StkPlPrintf("...[OK]\r\n");
	}

	return 0;
}

// 6MB * 64 = 384MBのテーブル"LargeBinTable2"をCreateTableで作成できる
// 6MBのバイナリデータを含むレコードを64回InsertRecordで追加する。
int LargeBinaryTableTest02()
{
	StkPlPrintf("CreateTable : 6MB * 64 = 384MB table \"LargeBinTable2\" can be created.");
	TableDef* LargeBinTable;
	ColumnDef* ColDef[32];
	LargeBinTable = new TableDef(L"LargeBinTable2", 64);
	ColDef[0] = new ColumnDefInt(L"ID");
	ColDef[1] = new ColumnDefBin(L"Data", 6000000);
	LargeBinTable->AddColumnDef(ColDef[0]);
	LargeBinTable->AddColumnDef(ColDef[1]);
	int Ret = CreateTable(LargeBinTable);
	if (Ret != 0) {
		StkPlPrintf("...[NG]\r\n");
		return -1;
	}
	StkPlPrintf("...[OK]\r\n");

	StkPlPrintf("InsertRecord : insert in 64 times for records include 6MB binary data.");
	unsigned char *Dat = new unsigned char[6000000];
	for (int i = 0; i < 6000000; i++) {
		Dat[i] = 127;
	}
	ColumnData* ColDat[3];
	RecordData* RecDat;
	LockTable(L"LargeBinTable2", 2);
	for (int i = 0; i < 64; i++) {
		ColDat[0] = new ColumnDataInt(L"ID", i);
		ColDat[1] = new ColumnDataBin(L"Data", Dat, 6000000);
		RecDat = new RecordData(L"LargeBinTable2", ColDat, 2);
		InsertRecord(RecDat);
		delete RecDat;
	}
	UnlockTable(L"LargeBinTable2");
	if (DeleteTable(L"LargeBinTable2") != 0) {
		StkPlPrintf("...[NG]\r\n");
		return -1;
	}
	StkPlPrintf("...[OK]\r\n");

	return 0;
}

/*
BinaryTest
・10バイトのバイナリ型カラムを含むテーブル"Bin-Test"をCreateTableで生成できる
・テーブル"Bin-Test"にInsertRecordで1レコードを追加
・テーブル"Bin-Test"にInsertRecordで連結されたレコード３個を追加
・テーブル"Bin-Test"にInsertRecordで1レコードを追加
・テーブル"Bin-Test"内の全てのレコードをGetRecrd(L"Bin-Test")で取得し，取得した連結レコードが適切であることを確認する
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
