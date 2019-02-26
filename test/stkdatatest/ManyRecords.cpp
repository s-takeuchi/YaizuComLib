#include "../../src/StkPl.h"
#include "..\..\src\stkdata\stkdata.h"
#include "..\..\src\stkdata\stkdataapi.h"

/*
ManyRecords
・WStr(256)×32カラム×16383レコードのテーブルを作成することができる。InsertRecordを16383回繰り返しレコードを追加できる
・既存の[焼津沼津辰口町和泉町中田北白楽]テーブルから10レコードを削除できる。条件として連結されたレコードを指定する
・存在しないカラム名を指定してZaSortRecordを実行したとき，-1が返却される。
*/
int ManyRecords()
{
	{
		StkPlPrintf("Table can be created defined as WStr(256) x 32 columns×16383 records. Records can be inserted 16383 times using InsertRecord.");
		ColumnDefWStr* ColDef[32];
		TableDef LargeTable(L"焼津沼津辰口町和泉町中田北白楽", 16383);
		for (int i = 0; i < 32; i++) {
			wchar_t ColName[16];
			StkPlSwPrintf(ColName, 16, L"東西南北老若男女焼肉定食愛%d", i);
			ColDef[i] = new ColumnDefWStr(ColName, 256);
			LargeTable.AddColumnDef(ColDef[i]);
		}
		if (CreateTable(&LargeTable) != 0) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
		for (int i = 0; i < 32; i++) {
			delete ColDef[i];
		}

		for (int k = 0; k < 16383; k++) {
			RecordData* RecDat;
			ColumnData* ColDat[32];
			for (int i = 0; i < 32; i++) {
				wchar_t ColName[16];
				wchar_t Val[256] = L"";
				StkPlSwPrintf(ColName, 16, L"東西南北老若男女焼肉定食愛%d", i);
				StkPlSwPrintf(Val, 256, L"%d %d :12345", k, i);
				for (int j = 0; j < 24; j++) {
					StkPlWcsCat(Val, 256, L"一二三四五六七八九十");
				}
				ColDat[i] = new ColumnDataWStr(ColName, Val);
			}
			RecDat = new RecordData(L"焼津沼津辰口町和泉町中田北白楽", ColDat, 32);
			LockTable(L"焼津沼津辰口町和泉町中田北白楽", 2);
			if (InsertRecord(RecDat) != 0) {
				StkPlPrintf("...[NG]\r\n");
				return -1;
			}
			UnlockTable(L"焼津沼津辰口町和泉町中田北白楽");
			delete RecDat;
		}
		StkPlPrintf("...[OK]\r\n");
	}

	{
		StkPlPrintf("Records can be acquired from exising table.");
		LockTable(L"焼津沼津辰口町和泉町中田北白楽", LOCK_SHARE);
		RecordData* RecDat = GetRecord(L"焼津沼津辰口町和泉町中田北白楽");
		UnlockTable(L"焼津沼津辰口町和泉町中田北白楽");
		RecordData* CurRecDat = RecDat;
		StkPlPrintf("Column information can be acquired with column name specification.");
		do {
			ColumnDataWStr* ColDat0 = (ColumnDataWStr*)CurRecDat->GetColumn(L"東西南北老若男女焼肉定食愛0");
			if (ColDat0 == NULL) {
				StkPlPrintf("...[NG]\r\n");
				return -1;
			}
			wchar_t* ColDat0Value = ColDat0->GetValue();
			if (ColDat0Value == NULL || StkPlWcsLen(ColDat0Value) == 0) {
				StkPlPrintf("...[NG]\r\n");
				return -1;
			}
			ColumnDataWStr* ColDat31 = (ColumnDataWStr*)CurRecDat->GetColumn(L"東西南北老若男女焼肉定食愛31");
			if (ColDat31 == NULL) {
				StkPlPrintf("...[NG]\r\n");
				return -1;
			}
			wchar_t* ColDat31Value = ColDat0->GetValue();
			if (ColDat31Value == NULL || StkPlWcsLen(ColDat31Value) == 0) {
				StkPlPrintf("...[NG]\r\n");
				return -1;
			}
		} while (CurRecDat = CurRecDat->GetNextRecord());
		StkPlPrintf("...[OK]\r\n");
		delete RecDat;
	}

	{
		StkPlPrintf("Search records from existing table.  Search criteria=WStr:multi+CONTAIN");
		ColumnData* ColDat[2];
		ColDat[0] = new ColumnDataWStr(L"東西南北老若男女焼肉定食愛2", L"100 2 :");
		ColDat[0]->SetComparisonOperator(COMP_CONTAIN);
		ColDat[1] = new ColumnDataWStr(L"東西南北老若男女焼肉定食愛3", L"100 3 :");
		ColDat[1]->SetComparisonOperator(COMP_CONTAIN);
		RecordData* RecDatSch = new RecordData(L"焼津沼津辰口町和泉町中田北白楽", ColDat, 2);
		LockTable(L"焼津沼津辰口町和泉町中田北白楽", LOCK_SHARE);
		RecordData* RecDatRet = GetRecord(RecDatSch);
		UnlockTable(L"焼津沼津辰口町和泉町中田北白楽");
		RecordData* CurRecDat = RecDatRet;
		do {
			ColumnDataWStr* ColDat2 = (ColumnDataWStr*)CurRecDat->GetColumn(L"東西南北老若男女焼肉定食愛2");
			ColumnDataWStr* ColDat3 = (ColumnDataWStr*)CurRecDat->GetColumn(L"東西南北老若男女焼肉定食愛3");
			if (ColDat2 == NULL || ColDat3 == NULL) {
				StkPlPrintf("...[NG]\r\n");
				return -1;
			}
			wchar_t* ColDat0Value2 = ColDat2->GetValue();
			wchar_t* ColDat0Value3 = ColDat3->GetValue();
			if (StkPlWcsStr(ColDat0Value2, L"100 2 :") == NULL) {
				StkPlPrintf("...[NG]\r\n");
				return -1;
			}
			if (StkPlWcsStr(ColDat0Value3, L"100 3 :") == NULL) {
				StkPlPrintf("...[NG]\r\n");
				return -1;
			}
		} while (CurRecDat = CurRecDat->GetNextRecord());
		delete RecDatSch;
		delete RecDatRet;
		StkPlPrintf("...[OK]\r\n");
	}

	{
		StkPlPrintf("Search records from existing table.  Search criteria=WStr:multi+EQUAL(invalid)");
		ColumnData* ColDat[1];
		ColDat[0] = new ColumnDataWStr(L"東西南北老若男女焼肉定食愛2", L"100 2 :");
		ColDat[0]->SetComparisonOperator(COMP_EQUAL);
		RecordData* RecDatSch = new RecordData(L"焼津沼津辰口町和泉町中田北白楽", ColDat, 1);
		LockTable(L"焼津沼津辰口町和泉町中田北白楽", LOCK_SHARE);
		RecordData* RecDatRet = GetRecord(RecDatSch);
		UnlockTable(L"焼津沼津辰口町和泉町中田北白楽");
		if (RecDatRet != NULL) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
		delete RecDatSch;
		StkPlPrintf("...[OK]\r\n");
	}

	{
		StkPlPrintf("Search records from existing table.  Search criteria=WStr:multi+NOT CONTAIN");
		ColumnData* ColDat[2];
		ColDat[0] = new ColumnDataWStr(L"東西南北老若男女焼肉定食愛2", L"100 2 :", COMP_NOT_CONTAIN);
		ColDat[1] = new ColumnDataWStr(L"東西南北老若男女焼肉定食愛3", L"100 3 :", COMP_NOT_CONTAIN);
		RecordData* RecDatSch = new RecordData(L"焼津沼津辰口町和泉町中田北白楽", ColDat, 2);
		LockTable(L"焼津沼津辰口町和泉町中田北白楽", LOCK_SHARE);
		RecordData* RecDatRet = GetRecord(RecDatSch);
		UnlockTable(L"焼津沼津辰口町和泉町中田北白楽");
		RecordData* CurRecDat = RecDatRet;
		int NumOfDat = 0;
		do {
			ColumnDataWStr* ColDat2 = (ColumnDataWStr*)CurRecDat->GetColumn(L"東西南北老若男女焼肉定食愛2");
			ColumnDataWStr* ColDat3 = (ColumnDataWStr*)CurRecDat->GetColumn(L"東西南北老若男女焼肉定食愛3");
			if (ColDat2 == NULL || ColDat3 == NULL) {
				StkPlPrintf("...[NG]\r\n");
				return -1;
			}
			wchar_t* ColDat0Value2 = ColDat2->GetValue();
			wchar_t* ColDat0Value3 = ColDat3->GetValue();
			if (StkPlWcsStr(ColDat0Value2, L"100 2 :") != NULL) {
				StkPlPrintf("...[NG]\r\n");
				return -1;
			}
			if (StkPlWcsStr(ColDat0Value3, L"100 3 :") != NULL) {
				StkPlPrintf("...[NG]\r\n");
				return -1;
			}
			NumOfDat++;
		} while (CurRecDat = CurRecDat->GetNextRecord());
		delete RecDatSch;
		delete RecDatRet;
		StkPlPrintf("...%d[OK]\r\n", NumOfDat);
	}

	{
		StkPlPrintf("Search records from existing table.  Search criteria=WStr:multi(invalid)");
		ColumnData* ColDat[2];
		ColDat[0] = new ColumnDataWStr(L"東西南北老若男女焼肉定食愛2", L"100 2 :", COMP_CONTAIN);
		ColDat[1] = new ColumnDataWStr(L"東西南北老若男女焼肉定食愛3", L"100 3 :", COMP_NOT_CONTAIN);
		RecordData* RecDatSch = new RecordData(L"焼津沼津辰口町和泉町中田北白楽", ColDat, 2);
		LockTable(L"焼津沼津辰口町和泉町中田北白楽", LOCK_SHARE);
		RecordData* RecDatRet = GetRecord(RecDatSch);
		UnlockTable(L"焼津沼津辰口町和泉町中田北白楽");
		if (RecDatRet != NULL) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
		delete RecDatSch;
		StkPlPrintf("...[OK]\r\n");
	}

	{
		StkPlPrintf("10 records can be acquired from specified table. Connected records are specified.");
		RecordData* RecDat;
		RecordData* TopRecDat;
		RecordData* PrvRecDat;
		ColumnData* ColDat;
		for (int i = 0; i < 10; i ++) {
			wchar_t ColName[16];
			wchar_t Val[256] = L"";
			StkPlSwPrintf(ColName, 16, L"東西南北老若男女焼肉定食愛%d", 0);
			StkPlSwPrintf(Val, 256, L"%d %d :12345", i, 0);
			for (int j = 0; j < 24; j++) {
				StkPlWcsCat(Val, 256, L"一二三四五六七八九十");
			}
			ColDat = new ColumnDataWStr(ColName, Val);
			RecDat = new RecordData(L"焼津沼津辰口町和泉町中田北白楽", &ColDat, 1);

			if (i == 0) {
				TopRecDat = RecDat;
			}
			if (i >= 1) {
				PrvRecDat->SetNextRecord(RecDat);
			}
			PrvRecDat = RecDat;
		}
		LockTable(L"焼津沼津辰口町和泉町中田北白楽", 2);
		DeleteRecord(TopRecDat);
		UnlockTable(L"焼津沼津辰口町和泉町中田北白楽");
		delete TopRecDat;
		if (GetNumOfRecords(L"焼津沼津辰口町和泉町中田北白楽") != 16373) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
		StkPlPrintf("...[OK]\r\n");
	}

	{
		StkPlPrintf("-1 is returned if non existing column name is specified to ZaSortRecord.");
		LockTable(L"焼津沼津辰口町和泉町中田北白楽", LOCK_EXCLUSIVE);
		if (ZaSortRecord(L"焼津沼津辰口町和泉町中田北白楽", L"aaa") != -1) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
		UnlockTable(L"焼津沼津辰口町和泉町中田北白楽");
		StkPlPrintf("...[OK]\r\n");
	}

	StkPlPrintf("Delete a table which contains large number of records.");
	if (DeleteTable(L"焼津沼津辰口町和泉町中田北白楽") != 0) {
		StkPlPrintf("...[NG]\r\n");
		return -1;
	}
	StkPlPrintf("...[OK]\r\n");

	return 0;
}
