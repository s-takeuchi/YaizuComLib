#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <shlwapi.h>
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
		printf("WStr(256)×32カラム×16383レコードのテーブルを作成することができる。InsertRecordを16383回繰り返しレコードを追加できる");
		ColumnDefWStr* ColDef[32];
		TableDef LargeTable(_T("焼津沼津辰口町和泉町中田北白楽"), 16383);
		for (int i = 0; i < 32; i++) {
			TCHAR ColName[16];
			wsprintf(ColName, _T("東西南北老若男女焼肉定食愛%d"), i);
			ColDef[i] = new ColumnDefWStr(ColName, 256);
			LargeTable.AddColumnDef(ColDef[i]);
		}
		if (CreateTable(&LargeTable) != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		for (int i = 0; i < 32; i++) {
			delete ColDef[i];
		}

		for (int k = 0; k < 16383; k++) {
			RecordData* RecDat;
			ColumnData* ColDat[32];
			for (int i = 0; i < 32; i++) {
				TCHAR ColName[16];
				TCHAR Val[256] = _T("");
				wsprintf(ColName, _T("東西南北老若男女焼肉定食愛%d"), i);
				wsprintf(Val, _T("%d %d :12345"), k, i);
				for (int j = 0; j < 24; j++) {
					lstrcat(Val, _T("一二三四五六七八九十"));
				}
				ColDat[i] = new ColumnDataWStr(ColName, Val);
			}
			RecDat = new RecordData(_T("焼津沼津辰口町和泉町中田北白楽"), ColDat, 32);
			LockTable(_T("焼津沼津辰口町和泉町中田北白楽"), 2);
			if (InsertRecord(RecDat) != 0) {
				printf("...[NG]\r\n");
				return -1;
			}
			UnlockTable(_T("焼津沼津辰口町和泉町中田北白楽"));
			delete RecDat;
		}
		printf("...[OK]\r\n");
	}

	{
		printf("既存の[焼津沼津辰口町和泉町中田北白楽]テーブルからレコードを取得する。");
		LockTable(_T("焼津沼津辰口町和泉町中田北白楽"), LOCK_SHARE);
		RecordData* RecDat = GetRecord(_T("焼津沼津辰口町和泉町中田北白楽"));
		UnlockTable(_T("焼津沼津辰口町和泉町中田北白楽"));
		RecordData* CurRecDat = RecDat;
		printf("カラム名指定で[東西南北老若男女焼肉定食愛0]or[東西南北老若男女焼肉定食愛31]のカラム情報を取得できるか");
		do {
			ColumnDataWStr* ColDat0 = (ColumnDataWStr*)CurRecDat->GetColumn(_T("東西南北老若男女焼肉定食愛0"));
			if (ColDat0 == NULL) {
				printf("...[NG]\r\n");
				return -1;
			}
			TCHAR* ColDat0Value = ColDat0->GetValue();
			if (ColDat0Value == NULL || lstrlen(ColDat0Value) == 0) {
				printf("...[NG]\r\n");
				return -1;
			}
			ColumnDataWStr* ColDat31 = (ColumnDataWStr*)CurRecDat->GetColumn(_T("東西南北老若男女焼肉定食愛31"));
			if (ColDat31 == NULL) {
				printf("...[NG]\r\n");
				return -1;
			}
			TCHAR* ColDat31Value = ColDat0->GetValue();
			if (ColDat31Value == NULL || lstrlen(ColDat31Value) == 0) {
				printf("...[NG]\r\n");
				return -1;
			}
		} while (CurRecDat = CurRecDat->GetNextRecord());
		printf("...[OK]\r\n");
		delete RecDat;
	}

	{
		printf("既存の[焼津沼津辰口町和泉町中田北白楽]テーブルからレコードを検索(WStr複数条件:CONTAIN指定)する。");
		ColumnData* ColDat[2];
		ColDat[0] = new ColumnDataWStr(_T("東西南北老若男女焼肉定食愛2"), _T("100 2 :"));
		ColDat[0]->SetComparisonOperator(COMP_CONTAIN);
		ColDat[1] = new ColumnDataWStr(_T("東西南北老若男女焼肉定食愛3"), _T("100 3 :"));
		ColDat[1]->SetComparisonOperator(COMP_CONTAIN);
		RecordData* RecDatSch = new RecordData(_T("焼津沼津辰口町和泉町中田北白楽"), ColDat, 2);
		LockTable(_T("焼津沼津辰口町和泉町中田北白楽"), LOCK_SHARE);
		RecordData* RecDatRet = GetRecord(RecDatSch);
		UnlockTable(_T("焼津沼津辰口町和泉町中田北白楽"));
		RecordData* CurRecDat = RecDatRet;
		do {
			ColumnDataWStr* ColDat2 = (ColumnDataWStr*)CurRecDat->GetColumn(_T("東西南北老若男女焼肉定食愛2"));
			ColumnDataWStr* ColDat3 = (ColumnDataWStr*)CurRecDat->GetColumn(_T("東西南北老若男女焼肉定食愛3"));
			if (ColDat2 == NULL || ColDat3 == NULL) {
				printf("...[NG]\r\n");
				return -1;
			}
			TCHAR* ColDat0Value2 = ColDat2->GetValue();
			TCHAR* ColDat0Value3 = ColDat3->GetValue();
			if (StrStr(ColDat0Value2, _T("100 2 :")) == NULL) {
				printf("...[NG]\r\n");
				return -1;
			}
			if (StrStr(ColDat0Value3, _T("100 3 :")) == NULL) {
				printf("...[NG]\r\n");
				return -1;
			}
		} while (CurRecDat = CurRecDat->GetNextRecord());
		delete RecDatSch;
		delete RecDatRet;
		printf("...[OK]\r\n");
	}

	{
		printf("既存の[焼津沼津辰口町和泉町中田北白楽]テーブルからレコードを検索(WStr複数条件(不正EQUAL)指定)する。");
		ColumnData* ColDat[1];
		ColDat[0] = new ColumnDataWStr(_T("東西南北老若男女焼肉定食愛2"), _T("100 2 :"));
		ColDat[0]->SetComparisonOperator(COMP_EQUAL);
		RecordData* RecDatSch = new RecordData(_T("焼津沼津辰口町和泉町中田北白楽"), ColDat, 1);
		LockTable(_T("焼津沼津辰口町和泉町中田北白楽"), LOCK_SHARE);
		RecordData* RecDatRet = GetRecord(RecDatSch);
		UnlockTable(_T("焼津沼津辰口町和泉町中田北白楽"));
		if (RecDatRet != NULL) {
			printf("...[NG]\r\n");
			return -1;
		}
		delete RecDatSch;
		printf("...[OK]\r\n");
	}

	{
		printf("既存の[焼津沼津辰口町和泉町中田北白楽]テーブルからレコードを検索(WStr複数条件:NOT CONTAIN指定)する。");
		ColumnData* ColDat[2];
		ColDat[0] = new ColumnDataWStr(_T("東西南北老若男女焼肉定食愛2"), _T("100 2 :"), COMP_NOT_CONTAIN);
		ColDat[1] = new ColumnDataWStr(_T("東西南北老若男女焼肉定食愛3"), _T("100 3 :"), COMP_NOT_CONTAIN);
		RecordData* RecDatSch = new RecordData(_T("焼津沼津辰口町和泉町中田北白楽"), ColDat, 2);
		LockTable(_T("焼津沼津辰口町和泉町中田北白楽"), LOCK_SHARE);
		RecordData* RecDatRet = GetRecord(RecDatSch);
		UnlockTable(_T("焼津沼津辰口町和泉町中田北白楽"));
		RecordData* CurRecDat = RecDatRet;
		int NumOfDat = 0;
		do {
			ColumnDataWStr* ColDat2 = (ColumnDataWStr*)CurRecDat->GetColumn(_T("東西南北老若男女焼肉定食愛2"));
			ColumnDataWStr* ColDat3 = (ColumnDataWStr*)CurRecDat->GetColumn(_T("東西南北老若男女焼肉定食愛3"));
			if (ColDat2 == NULL || ColDat3 == NULL) {
				printf("...[NG]\r\n");
				return -1;
			}
			TCHAR* ColDat0Value2 = ColDat2->GetValue();
			TCHAR* ColDat0Value3 = ColDat3->GetValue();
			if (StrStr(ColDat0Value2, _T("100 2 :")) != NULL) {
				printf("...[NG]\r\n");
				return -1;
			}
			if (StrStr(ColDat0Value3, _T("100 3 :")) != NULL) {
				printf("...[NG]\r\n");
				return -1;
			}
			NumOfDat++;
		} while (CurRecDat = CurRecDat->GetNextRecord());
		delete RecDatSch;
		delete RecDatRet;
		printf("...%d[OK]\r\n", NumOfDat);
	}

	{
		printf("既存の[焼津沼津辰口町和泉町中田北白楽]テーブルからレコードを検索(WStr複数条件(不正)指定)する。");
		ColumnData* ColDat[2];
		ColDat[0] = new ColumnDataWStr(_T("東西南北老若男女焼肉定食愛2"), _T("100 2 :"), COMP_CONTAIN);
		ColDat[1] = new ColumnDataWStr(_T("東西南北老若男女焼肉定食愛3"), _T("100 3 :"), COMP_NOT_CONTAIN);
		RecordData* RecDatSch = new RecordData(_T("焼津沼津辰口町和泉町中田北白楽"), ColDat, 2);
		LockTable(_T("焼津沼津辰口町和泉町中田北白楽"), LOCK_SHARE);
		RecordData* RecDatRet = GetRecord(RecDatSch);
		UnlockTable(_T("焼津沼津辰口町和泉町中田北白楽"));
		if (RecDatRet != NULL) {
			printf("...[NG]\r\n");
			return -1;
		}
		delete RecDatSch;
		printf("...[OK]\r\n");
	}

	{
		printf("既存の[焼津沼津辰口町和泉町中田北白楽]テーブルから10レコードを削除できる。条件として連結されたレコードを指定する");
		RecordData* RecDat;
		RecordData* TopRecDat;
		RecordData* PrvRecDat;
		ColumnData* ColDat;
		for (int i = 0; i < 10; i ++) {
			TCHAR ColName[16];
			TCHAR Val[256] = _T("");
			wsprintf(ColName, _T("東西南北老若男女焼肉定食愛%d"), 0);
			wsprintf(Val, _T("%d %d :12345"), i, 0);
			for (int j = 0; j < 24; j++) {
				lstrcat(Val, _T("一二三四五六七八九十"));
			}
			ColDat = new ColumnDataWStr(ColName, Val);
			RecDat = new RecordData(_T("焼津沼津辰口町和泉町中田北白楽"), &ColDat, 1);

			if (i == 0) {
				TopRecDat = RecDat;
			}
			if (i >= 1) {
				PrvRecDat->SetNextRecord(RecDat);
			}
			PrvRecDat = RecDat;
		}
		LockTable(_T("焼津沼津辰口町和泉町中田北白楽"), 2);
		DeleteRecord(TopRecDat);
		UnlockTable(_T("焼津沼津辰口町和泉町中田北白楽"));
		delete TopRecDat;
		if (GetNumOfRecords(_T("焼津沼津辰口町和泉町中田北白楽")) != 16373) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}

	{
		printf("存在しないカラム名を指定してZaSortRecordを実行したとき，-1が返却される。");
		LockTable(_T("焼津沼津辰口町和泉町中田北白楽"), LOCK_EXCLUSIVE);
		if (ZaSortRecord(_T("焼津沼津辰口町和泉町中田北白楽"), _T("aaa")) != -1) {
			printf("...[NG]\r\n");
			return -1;
		}
		UnlockTable(_T("焼津沼津辰口町和泉町中田北白楽"));
		printf("...[OK]\r\n");
	}

	printf("大量レコードが含まれるテーブルの削除");
	if (DeleteTable(_T("焼津沼津辰口町和泉町中田北白楽")) != 0) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");

	return 0;
}
