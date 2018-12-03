#include <windows.h>
#include <stdio.h>
#include "..\..\src\stkdata\stkdata.h"
#include "..\..\src\stkdata\stkdataapi.h"


int Insert16383RecordsAtOneTime()
{
	printf("CreateTableで自動車テーブル(通し番号:Int, 車名:WStr(50), 型番:Str(100), 製造会社:WStr(20)) Max=16383 を生成することができる");
	ColumnDefInt ColDefId(L"通し番号");
	ColumnDefWStr ColDefName(L"車名", 50);
	ColumnDefStr ColDefType(L"型番", 100);
	ColumnDefWStr ColDefManuf(L"製造会社", 20);
	TableDef TabDefPerson(L"自動車", 16383);
	TabDefPerson.AddColumnDef(&ColDefId);
	TabDefPerson.AddColumnDef(&ColDefName);
	TabDefPerson.AddColumnDef(&ColDefType);
	TabDefPerson.AddColumnDef(&ColDefManuf);
	if (CreateTable(&TabDefPerson) != 0) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");


	printf("16383個の結合されたレコードを自動車テーブルに対してInsertRecord(呼び出し1回)を実行し，16383個のレコードを追加することができる");
	ColumnData *ColDat[4];
	RecordData *TopRecDat;
	RecordData *PrvRecDat;
	RecordData *CurRecDat;
	for (int i = 0; i < 16383; i++) {
		ColDat[0] = new ColumnDataInt(L"通し番号", i);
		ColDat[1] = new ColumnDataWStr(L"車名", L"スーパーチャージャー付2400cc 2シーターオープンカー ドーロスター VS");
		ColDat[2] = new ColumnDataStr(L"型番", "ABCDEFGHIJABCDEFGHIJABCDEFGHIJ01234567890123456789012345678901234567890123456789");
		ColDat[3] = new ColumnDataWStr(L"製造会社", L"Mazzzzzzzda自動車工業");
		CurRecDat = new RecordData(L"自動車", ColDat, 4);
		if (i == 0) {
			TopRecDat = CurRecDat;
		}
		if (i >= 1) {
			PrvRecDat->SetNextRecord(CurRecDat);
		}
		PrvRecDat = CurRecDat;
	}
	LockTable(L"自動車", 2);
	if (InsertRecord(TopRecDat) != 0) {
		delete TopRecDat;
		printf("...[NG]\r\n");
		return -1;
	}
	UnlockTable(L"自動車");
	delete TopRecDat;
	printf("...[OK]\r\n");


	printf("GetTableVersion(L\"自動車\")が16383を返すことを確認する");
	if (GetTableVersion(L"自動車") != 16383) {
		printf("...[NG]\r\n");
		return -1;
	}
	printf("...[OK]\r\n");


	printf("追加した16383個のレコードに対しGetRecord(レコード指定)で連結した16383個のレコードを取得することができる");
	int Count = 0;
	ColumnData *ColDatTake[1];
	ColDatTake[0] = new ColumnDataStr(L"型番", "ABCDEFGHIJABCDEFGHIJABCDEFGHIJ01234567890123456789012345678901234567890123456789");
	RecordData RecDatTake(L"自動車", ColDatTake, 1);
	RecordData *RecDatGet;
	LockTable(L"自動車", 1);
	RecDatGet = GetRecord(&RecDatTake);
	UnlockTable(L"自動車");
	while (RecDatGet) {
		if (lstrcmp(((ColumnDataWStr*)(RecDatGet->GetColumn(1)))->GetValue(), L"スーパーチャージャー付2400cc 2シーターオープンカー ドーロスター VS") != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		Count++;
		RecDatGet = RecDatGet->GetNextRecord();
	}
	if (Count != 16383) {
		printf("...[NG]\r\n");
		return -1;
	}
	delete RecDatGet;
	printf("...[OK]\r\n");


	{
		printf("GetColumnCount(L\"自動車\")およびGetColumnName(L\"自動車\")の結果は4となる。GetColumnNameで取得した1番目のカラム名は\"通し番号\",4番目のカラム名は\"製造会社\"となる");
		wchar_t ColNames[MAX_COLUMN_NUMBER][16];
		if (GetColumnCount(L"自動車") != 4) {
			printf("...[NG]\r\n");
			return -1;
		}
		if (GetColumnName(L"自動車", ColNames) != 4) {
			printf("...[NG]\r\n");
			return -1;
		}
		if (lstrcmp(ColNames[0], L"通し番号") != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		if (lstrcmp(ColNames[3], L"製造会社") != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}


	{
		printf("GetColumnCount(L\"自転車\")およびGetColumnName(L\"自転車\")の結果は-1となる");
		wchar_t ColNames[MAX_COLUMN_NUMBER][16];
		if (GetColumnCount(L"自転車") != -1) {
			printf("...[NG]\r\n");
			return -1;
		}
		if (GetColumnName(L"自転車", ColNames) != -1) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}

	{
		printf("既存の自動車テーブルの[通し番号=奇数]となるレコード(8191個)について車名=\"日本国内閣総理大臣専用クーペ\"に変更する(8191個の連結されたRecordDataをUpdateRecordに渡す)");
		ColumnData *ColDat[4];
		RecordData *TopRecDat;
		RecordData *PrvRecDat;
		RecordData *CurRecDat;
		ColDat[0] = new ColumnDataInt(L"通し番号", 100);
		ColDat[1] = new ColumnDataWStr(L"車名", L"日本国内閣総理大臣専用クーペ");
		ColDat[2] = new ColumnDataStr(L"型番", "So-Ri");
		ColDat[3] = new ColumnDataWStr(L"製造会社", L"内閣府");
		CurRecDat = new RecordData(L"自動車", ColDat, 4);
		for (int i = 1; i < 16383; i += 2) {
			ColumnData* ColDatSch[1];
			ColDatSch[0] = new ColumnDataInt(L"通し番号", i);
			RecordData* RecDatSch = new RecordData(L"自動車", ColDatSch, 1);
			if (i == 1) {
				TopRecDat = RecDatSch;
			}
			if (i >= 2) {
				PrvRecDat->SetNextRecord(RecDatSch);
			}
			PrvRecDat = RecDatSch;
		}
		LockTable(L"自動車", 2);
		if (UpdateRecord(TopRecDat, CurRecDat) != 0) {
			UnlockTable(L"自動車");
			delete TopRecDat;
			delete CurRecDat;
			printf("...[NG]\r\n");
			return -1;
		}
		UnlockTable(L"自動車");
		delete TopRecDat;
		delete CurRecDat;
		printf("...[OK]\r\n");


		printf("GetRecord(L\"自動車\")で取得したレコードは，”通し番号奇数が”日本国・・・”，偶数が”スーパーチャージャー付・・・”となっている");
		int Count = 0;
		RecordData *RecDatGet;
		LockTable(L"自動車", 1);
		RecDatGet = GetRecord(L"自動車");
		UnlockTable(L"自動車");
		while (RecDatGet) {
			wchar_t *ColName = ((ColumnDataWStr*)(RecDatGet->GetColumn(1)))->GetValue();
			if (Count % 2 == 0) {
				if (lstrcmp(ColName, L"スーパーチャージャー付2400cc 2シーターオープンカー ドーロスター VS") != 0) {
					printf("...[NG]\r\n");
					return -1;
				}
			} else {
				if (lstrcmp(ColName, L"日本国内閣総理大臣専用クーペ") != 0) {
					printf("...[NG]\r\n");
					return -1;
				}
			}
			Count++;
			RecDatGet = RecDatGet->GetNextRecord();
		}
		printf("...[OK]\r\n");


		printf("GetRecord(L\"自動車\")で16383個のレコードを取得でき且つGetNumOfRecords()結果が16383となる");
		if (Count != 16383 || GetNumOfRecords(L"自動車") != 16383) {
			printf("...[NG]\r\n");
			delete RecDatGet;
			return -1;
		}
		delete RecDatGet;
		printf("...[OK]\r\n");
	}

	{
		printf("\"自動車\"テーブルから\"製造会社\" = \"内閣府X\"となるレコードの削除を行う。検索前件不一致のためDeleteRecord後のGetNumOfRecordsは16383を返す");
		ColumnData *ColDat[1];
		ColDat[0] = new ColumnDataWStr(L"製造会社", L"内閣府X");
		RecordData *RecDat = new RecordData(L"自動車", ColDat, 1);
		LockTable(L"自動車", LOCK_EXCLUSIVE);
		if (DeleteRecord(RecDat) != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		UnlockTable(L"自動車");
		delete RecDat;
		if (GetNumOfRecords(L"自動車") != 16383) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}


	{
		printf("\"自動車\"テーブルから\"製造会社\" = \"内閣府\"となるレコードの削除を行う。DeleteRecord後のGetNumOfRecordsは8192を返す");
		ColumnData *ColDat[1];
		ColDat[0] = new ColumnDataWStr(L"製造会社", L"内閣府");
		RecordData *RecDat = new RecordData(L"自動車", ColDat, 1);
		LockTable(L"自動車", LOCK_EXCLUSIVE);
		if (DeleteRecord(RecDat) != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		UnlockTable(L"自動車");
		delete RecDat;
		if (GetNumOfRecords(L"自動車") != 8192) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}

	{
		printf("GetMaxNumOfRecords(L\"自動車\")の結果は16383となる");
		if (GetMaxNumOfRecords(L"自動車") != 16383) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}


	{
		printf("レコードが8192件存在する状態でDeleteTable(L\"自動車\")を実行後，GetNumOfRecords(L\"自動車\")およびGetMaxNumOfRecords(L\"自動車\")の返り値は-1となる。");
		if (DeleteTable(L"自動車") != 0) {
			printf("...[NG]\r\n");
			return -1;
		}
		if (GetMaxNumOfRecords(L"自動車") != -1) {
			printf("...[NG]\r\n");
			return -1;
		}
		if (GetNumOfRecords(L"自動車") != -1) {
			printf("...[NG]\r\n");
			return -1;
		}
		printf("...[OK]\r\n");
	}


	return 0;
}
