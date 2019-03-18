#include "../../src/stkpl/StkPl.h"
#include "../../src/stkdata/stkdata.h"
#include "../../src/stkdata/stkdataapi.h"

/*
TableVersion
・"商品"テーブル, "顧客"テーブルを生成する前のGetTableVersion(L"商品")またはGetTableVersion(L"顧客")呼び出しで-1が返る
・CreateTableで"商品"テーブルを最大レコード=500として生成することができる
・CreateTableで"顧客"テーブルを最大レコード=5000として生成することができる
・"商品"テーブル, "顧客"テーブルを生成後のGetTableVersion(L"商品")またはGetTableVersion(L"顧客")呼び出しで0が返る
・InsertRecordでテーブル"顧客"に1レコード追加後，GetTableVersion("顧客")で1が返却される。GetTableVersion("商品")は0のまま
・InsertRecordでテーブル"顧客"に存在しないカラムを指定してもレコードは追加されず，GetTableVersion("顧客")で1が返却される。GetTableVersion("商品")は0のまま
・UpdateRecordでテーブル"顧客"の1レコード（2カラム分）を更新後，GetTableVersion("顧客")で2が返却される。GetTableVersion("商品")は0のまま
・GetRecordでテーブル"顧客"の1レコードを取得後，GetTableVersion("顧客")で2が返却される。GetTableVersion("商品")は0のまま
・DeleteRecordでテーブル"顧客"の1レコードを削除後，GetTableVersion("顧客")で3が返却される。GetTableVersion("商品")は0のまま
・InsertRecordで5レコード分（内2レコードは不正）追加後，GetTableVersion("顧客")で3が返却され，GetTableVersion("商品")で3が返却される
・UpdateRecord"商品"(Search=複数カラム指定:ただし不正, Update=複数カラム指定:適正)後，GetTableVersion("顧客")で3が返却され，GetTableVersion("商品")で3が返却される
・UpdateRecord"商品"(Search=複数カラム指定:正しい3レコード指定で検索, Update=複数カラム指定:ただし不正)後，GetTableVersion("顧客")で3が返却され，GetTableVersion("商品")で3が返却される
・UpdateRecord"商品"で3レコード更新(Search=複数カラム指定, Update=複数カラム指定)後，GetTableVersion("顧客")で3が返却され，GetTableVersion("商品")で6が返却される
・SaveData後，GetTableVersion("顧客")で3が返却され，GetTableVersion("商品")で6が返却される
・LoadData後，GetTableVersion("顧客")で0が返却され，GetTableVersion("商品")で0が返却される
・DeleteTable(L"商品")でテーブルを削除後，GetTableVersion(L"商品")で-1が返り，GetTableVersion(L"顧客")で-1以外が返る
・さらにDeleteTable(L"顧客")でテーブルを削除後，GetTableVersion(L"商品")，GetTableVersion(L"顧客")ともに-1が返る
*/
int TableVersion()
{
	// "商品"テーブル, "顧客"テーブルを生成する前のGetTableVersion(L"商品")またはGetTableVersion(L"顧客")呼び出しで-1が返る
	{
		StkPlPrintf("GetTableVersion(L\"customer\") and GetTableVersion(L\"merchandise\") return -1 before merchandise and customer tables are created.");
		if (GetTableVersion(L"商品") != -1 || GetTableVersion(L"顧客") != -1) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
	}
	StkPlPrintf("...[OK]\r\n");

	// CreateTableで"商品"テーブルを最大レコード=500として生成することができる
	{
		StkPlPrintf("merchandise table (max records = 500) can be created by CreateTable.");
		ColumnDefInt ColDefId(L"商品番号");
		ColumnDefWStr ColDefName(L"商品名", 10);
		ColumnDefInt ColDefPrice(L"値段");
		ColumnDefInt ColDefCount(L"在庫数");
		TableDef TabDefMarc(L"商品", 500);
		TabDefMarc.AddColumnDef(&ColDefId);
		TabDefMarc.AddColumnDef(&ColDefName);
		TabDefMarc.AddColumnDef(&ColDefPrice);
		TabDefMarc.AddColumnDef(&ColDefCount);
		if (CreateTable(&TabDefMarc) != 0) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
	}
	StkPlPrintf("...[OK]\r\n");

	// CreateTableで"顧客"テーブルを最大レコード=5000として生成することができる
	{
		StkPlPrintf("customer table (max records = 5000) can be created by CreateTable.");
		ColumnDefWStr ColDefName(L"氏名", 10);
		ColumnDefWStr ColDefAddr(L"住所", 20);
		ColumnDefStr ColDefTel(L"電話番号", 20);
		TableDef TabDefCustomer(L"顧客", 5000);
		TabDefCustomer.AddColumnDef(&ColDefName);
		TabDefCustomer.AddColumnDef(&ColDefAddr);
		TabDefCustomer.AddColumnDef(&ColDefTel);
		if (CreateTable(&TabDefCustomer) != 0) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
	}
	StkPlPrintf("...[OK]\r\n");

	// "商品"テーブル, "顧客"テーブルを生成後のGetTableVersion(L"商品")またはGetTableVersion(L"顧客")呼び出しで0が返る
	{
		StkPlPrintf("GetTableVersion(L\"merchandise\") and GetTableVersion(L\"customer\") return 0 after merchandise table and customer table are created.");
		if (GetTableVersion(L"商品") != 0 || GetTableVersion(L"顧客") != 0) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
	}
	StkPlPrintf("...[OK]\r\n");

	// InsertRecordでテーブル"顧客"に1レコード追加後，GetTableVersion("顧客")で1が返却される。GetTableVersion("商品")は0のまま
	{
		StkPlPrintf("GetTableVersion(\"customer\") returns 1 after one record is added to table 'customer' by InsertRecord. GetTableVersion(\"merchandise\") returns 0.");
		ColumnData *ColDat[10];
		ColDat[0] = new ColumnDataWStr(L"氏名", L"竹内伸也");
		ColDat[1] = new ColumnDataWStr(L"住所", L"静岡県焼津市");
		ColDat[2] = new ColumnDataStr(L"電話番号", "111-222-3333");
		RecordData *RecDat = new RecordData(L"顧客", ColDat, 3);
		LockTable(L"顧客", LOCK_EXCLUSIVE);
		InsertRecord(RecDat);
		UnlockTable(L"顧客");
		delete RecDat;
		if (GetTableVersion(L"商品") != 0 || GetTableVersion(L"顧客") != 1) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
	}
	StkPlPrintf("...[OK]\r\n");

	// InsertRecordでテーブル"顧客"に存在しないカラムを指定してもレコードは追加されず，GetTableVersion("顧客")で1が返却される。GetTableVersion("商品")は0のまま
	{
		StkPlPrintf("GetTableVersion(\"customer\") returns 1 if non existing column is specified by InsertRecord('customer'). GetTableVersion(\"merchandise\") returns 0.");
		ColumnData *ColDat[10];
		ColDat[0] = new ColumnDataWStr(L"氏", L"竹内伸也2");
		ColDat[1] = new ColumnDataWStr(L"住", L"静岡県焼津市2");
		ColDat[2] = new ColumnDataStr(L"電話番号", "000-222-3333");
		RecordData *RecDat = new RecordData(L"顧客", ColDat, 3);
		LockTable(L"顧客", LOCK_EXCLUSIVE);
		InsertRecord(RecDat);
		UnlockTable(L"顧客");
		delete RecDat;
		if (GetNumOfRecords(L"顧客") != 1) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
		if (GetTableVersion(L"商品") != 0 || GetTableVersion(L"顧客") != 1) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
	}
	StkPlPrintf("...[OK]\r\n");

	// UpdateRecordでテーブル"顧客"の1レコード（2カラム分）を更新後，GetTableVersion("顧客")で2が返却される。GetTableVersion("商品")は0のまま
	{
		StkPlPrintf("GetTableVersion(\"customer\") returns 2 after 1 record (2 columns) in customer table is updated using UpdateRecord. GetTableVersion(\"merchandise\") shows 0.");
		ColumnData* ColTgt[5];
		ColumnData* ColUpd[5];
		ColTgt[0] = new ColumnDataWStr(L"氏名", L"竹内伸也");
		ColTgt[1] = new ColumnDataWStr(L"住所", L"静岡県焼津市");
		ColUpd[0] = new ColumnDataWStr(L"氏名", L"常見朱美");
		ColUpd[1] = new ColumnDataWStr(L"住所", L"埼玉県熊谷市");
		RecordData* RecTgt = new RecordData(L"顧客", ColTgt, 2);
		RecordData* RecUpd = new RecordData(L"顧客", ColUpd, 2);
		LockTable(L"顧客", LOCK_EXCLUSIVE);
		UpdateRecord(RecTgt, RecUpd);
		UnlockTable(L"顧客");
		delete RecTgt;
		delete RecUpd;
		int a = GetTableVersion(L"顧客");
		if (GetTableVersion(L"商品") != 0 || GetTableVersion(L"顧客") != 2) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
		StkPlPrintf("...[OK]\r\n");
	}

	// GetRecordでテーブル"顧客"の1レコードを取得後，GetTableVersion("顧客")で2が返却される。GetTableVersion("商品")は0のまま
	{
		StkPlPrintf("GetTableVersion(\"customer\") returns 2 after GetRecord(customer) is executed and 1 record is acquired. GetTableVersion(\"merchandise\") shows 0.");
		ColumnData *ColDat[10];
		ColDat[0] = new ColumnDataWStr(L"氏名", L"常見朱美");
		RecordData* RecDat = new RecordData(L"顧客", ColDat, 1);
		RecordData* RecGet;
		LockTable(L"顧客", LOCK_SHARE);
		RecGet = GetRecord(RecDat);
		UnlockTable(L"顧客");
		ColumnDataWStr* GetCol = (ColumnDataWStr*)RecGet->GetColumn(1);
		if (StkPlWcsCmp(GetCol->GetValue(), L"埼玉県熊谷市") != 0) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
		delete RecDat;
		delete RecGet;
		if (GetTableVersion(L"商品") != 0 || GetTableVersion(L"顧客") != 2) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
	}
	StkPlPrintf("...[OK]\r\n");

	// DeleteRecordでテーブル"顧客"の1レコードを削除後，GetTableVersion("顧客")で3が返却される。GetTableVersion("商品")は0のまま
	{
		StkPlPrintf("GetTableVersion(\"customer\") returns 3 after 1 record is deleted from customer table using DeleteRecord. GetTableVersion(\"merchandise\") returns 0.");
		ColumnData *ColDat[10];
		ColDat[0] = new ColumnDataWStr(L"氏名", L"常見朱美");
		RecordData* RecDat = new RecordData(L"顧客", ColDat, 1);
		LockTable(L"顧客", LOCK_EXCLUSIVE);
		DeleteRecord(RecDat);
		UnlockTable(L"顧客");
		delete RecDat;
		if (GetNumOfRecords(L"顧客") != 0) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
		if (GetTableVersion(L"商品") != 0 || GetTableVersion(L"顧客") != 3) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
		StkPlPrintf("...[OK]\r\n");
	}

	// InsertRecordで5レコード分（内2レコードは不正）追加後，GetTableVersion("顧客")で3が返却され，GetTableVersion("商品")で3が返却される
	{
		StkPlPrintf("GetTableVersion(\"customer\") returns 3 after 5 records (including 2 invalid records) are added by InsertRecord. GetTableVersion(\"merchandise\") returns 0");
		ColumnData* ColDat[100][4];
		RecordData* RecDat[100];
		ColDat[0][0] = new ColumnDataInt(L"商品番号", 1);
		ColDat[0][1] = new ColumnDataWStr(L"商品名", L"あああ");
		ColDat[0][2] = new ColumnDataInt(L"値段", 10000);
		ColDat[0][3] = new ColumnDataInt(L"在庫数", 10);
		RecDat[0] = new RecordData(L"商品", ColDat[0], 4);

		ColDat[1][0] = new ColumnDataInt(L"商番", 2);
		ColDat[1][1] = new ColumnDataWStr(L"商品", L"いいい");
		ColDat[1][2] = new ColumnDataInt(L"値", 20000);
		ColDat[1][3] = new ColumnDataInt(L"在庫", 20);
		RecDat[1] = new RecordData(L"商品", ColDat[1], 4);

		ColDat[2][0] = new ColumnDataInt(L"商品番号", 3);
		ColDat[2][1] = new ColumnDataWStr(L"商品名", L"ううう");
		ColDat[2][2] = new ColumnDataInt(L"値段", 30000);
		ColDat[2][3] = new ColumnDataInt(L"在庫数", 30);
		RecDat[2] = new RecordData(L"商品", ColDat[2], 4);

		ColDat[3][0] = new ColumnDataInt(L"商品番号", 4);
		ColDat[3][1] = new ColumnDataWStr(L"商品名", L"えええ");
		ColDat[3][2] = new ColumnDataInt(L"値段", 40000);
		ColDat[3][3] = new ColumnDataInt(L"在庫数", 40);
		RecDat[3] = new RecordData(L"商品", ColDat[3], 4);

		ColDat[4][0] = new ColumnDataInt(L"商品番号", 5);
		ColDat[4][1] = new ColumnDataWStr(L"商品名", L"おおお");
		ColDat[4][2] = new ColumnDataInt(L"値", 50000);
		ColDat[4][3] = new ColumnDataInt(L"在庫数", 50);
		RecDat[4] = new RecordData(L"商品", ColDat[4], 5);

		LockTable(L"商品", LOCK_EXCLUSIVE);
		for (int i = 0; i < 4; i++) {
			InsertRecord(RecDat[i]);
			delete RecDat[i];
		}
		UnlockTable(L"商品");

		if (GetTableVersion(L"商品") != 3 || GetTableVersion(L"顧客") != 3) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
		StkPlPrintf("...[OK]\r\n");
	}

	// UpdateRecord"商品"(Search=複数カラム指定:ただし不正, Update=複数カラム指定:適正)後，GetTableVersion("顧客")で3が返却され，GetTableVersion("商品")で3が返却される
	{
		StkPlPrintf("GetTableVersion(\"customer\") returns 3 after UpdateRecord execution of 'merchandise' (search=multi column specification(invalid), update=multi column(proper)). GetTableVersion(merchandise) returns 3.");
		ColumnData* ColTgt[100][4];
		RecordData* RecTgt[100];
		ColumnData* ColUpd[4];
		RecordData* RecUpd;

		ColTgt[0][0] = new ColumnDataInt(L"商品番号", 1);
		ColTgt[0][1] = new ColumnDataInt(L"値", 10000);
		RecTgt[0] = new RecordData(L"商品", ColTgt[0], 2);

		ColTgt[1][0] = new ColumnDataWStr(L"商品名", L"うはうはうは");
		RecTgt[1] = new RecordData(L"商品", ColTgt[1], 1);

		ColTgt[2][0] = new ColumnDataInt(L"在庫数", -40);
		ColTgt[2][1] = new ColumnDataInt(L"商品番号", 4);
		ColTgt[2][2] = new ColumnDataWStr(L"商品名", L"えええ");
		RecTgt[2] = new RecordData(L"商品", ColTgt[2], 3);

		ColUpd[0] = new ColumnDataInt(L"商品番号", 100);
		ColUpd[1] = new ColumnDataWStr(L"商品名", L"竹内伸也");
		ColUpd[2] = new ColumnDataInt(L"値段", 1000000);
		RecUpd = new RecordData(L"商品", ColUpd, 3);

		LockTable(L"商品", LOCK_EXCLUSIVE);
		for (int i = 0; i < 3; i++) {
			UpdateRecord(RecTgt[i], RecUpd);
			delete RecTgt[i];
		}
		UnlockTable(L"商品");
		delete RecUpd;

		if (GetTableVersion(L"商品") != 3 || GetTableVersion(L"顧客") != 3) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
		StkPlPrintf("...[OK]\r\n");
	}

	// UpdateRecord"商品"(Search=複数カラム指定:正しい3レコード指定で検索, Update=複数カラム指定:ただし不正)後，GetTableVersion("顧客")で3が返却され，GetTableVersion("商品")で3が返却される
	{
		StkPlPrintf("Both GetTableVersion(\"customer\") and GetTableVersion(\"merchandise\") return 3 after UpdateRecord(merchandise) (Search=multi column with proper 3 records, Update=multi invalid column).");
		ColumnData* ColTgt[100][4];
		RecordData* RecTgt[100];
		ColumnData* ColUpd[4];
		RecordData* RecUpd;

		ColTgt[0][0] = new ColumnDataInt(L"商品番号", 1);
		ColTgt[0][1] = new ColumnDataInt(L"値段", 10000);
		RecTgt[0] = new RecordData(L"商品", ColTgt[0], 2);

		ColTgt[1][0] = new ColumnDataWStr(L"商品名", L"ううう");
		RecTgt[1] = new RecordData(L"商品", ColTgt[1], 1);

		ColTgt[2][0] = new ColumnDataInt(L"在庫数", 40);
		ColTgt[2][1] = new ColumnDataInt(L"商品番号", 4);
		ColTgt[2][2] = new ColumnDataWStr(L"商品名", L"えええ");
		RecTgt[2] = new RecordData(L"商品", ColTgt[2], 3);

		ColUpd[0] = new ColumnDataInt(L"xxx", 200);
		ColUpd[1] = new ColumnDataWStr(L"yyy", L"XXXXX");
		ColUpd[2] = new ColumnDataInt(L"zzz", 2000000);
		RecUpd = new RecordData(L"商品", ColUpd, 3);

		LockTable(L"商品", LOCK_EXCLUSIVE);
		for (int i = 0; i < 3; i++) {
			UpdateRecord(RecTgt[i], RecUpd);
			delete RecTgt[i];
		}
		UnlockTable(L"商品");
		delete RecUpd;
		int a = GetTableVersion(L"商品");

		if (GetTableVersion(L"商品") != 3 || GetTableVersion(L"顧客") != 3) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
		StkPlPrintf("...[OK]\r\n");
	}

	// UpdateRecord"商品"で3レコード更新(Search=複数カラム指定, Update=複数カラム指定)後，GetTableVersion("顧客")で3が返却され，GetTableVersion("商品")で6が返却される
	{
		StkPlPrintf("GetTableVersion(\"customer\") returns 3 and GetTableVersion(\"merchandise\") returns 6 after UpdateRecord(merchandise) for 3 records update (Search=multi column, Update=multi column).");
		ColumnData* ColTgt[100][4];
		RecordData* RecTgt[100];
		ColumnData* ColUpd[4];
		RecordData* RecUpd;

		ColTgt[0][0] = new ColumnDataInt(L"商品番号", 1);
		ColTgt[0][1] = new ColumnDataInt(L"値段", 10000);
		RecTgt[0] = new RecordData(L"商品", ColTgt[0], 2);

		ColTgt[1][0] = new ColumnDataWStr(L"商品名", L"ううう");
		RecTgt[1] = new RecordData(L"商品", ColTgt[1], 1);

		ColTgt[2][0] = new ColumnDataInt(L"在庫数", 40);
		ColTgt[2][1] = new ColumnDataInt(L"商品番号", 4);
		ColTgt[2][2] = new ColumnDataWStr(L"商品名", L"えええ");
		RecTgt[2] = new RecordData(L"商品", ColTgt[2], 3);

		ColUpd[0] = new ColumnDataInt(L"商品番号", 100);
		ColUpd[1] = new ColumnDataWStr(L"商品名", L"竹内伸也");
		ColUpd[2] = new ColumnDataInt(L"値段", 1000000);
		RecUpd = new RecordData(L"商品", ColUpd, 3);

		LockTable(L"商品", LOCK_EXCLUSIVE);
		for (int i = 0; i < 3; i++) {
			UpdateRecord(RecTgt[i], RecUpd);
			delete RecTgt[i];
		}
		UnlockTable(L"商品");
		delete RecUpd;

		if (GetTableVersion(L"商品") != 6 || GetTableVersion(L"顧客") != 3) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
		StkPlPrintf("...[OK]\r\n");
	}

	// SaveData後，GetTableVersion("顧客")で3が返却され，GetTableVersion("商品")で6が返却される
	// LoadData後，GetTableVersion("顧客")で0が返却され，GetTableVersion("商品")で0が返却される
	{
		StkPlPrintf("GetTableVersion(\"customer\") returns 3 and GetTableVersion(\"merchandise\") returns 6 after SaveData.");
		LockAllTable(LOCK_SHARE);
		SaveData(L"TableVersion.std");
		UnlockAllTable();
		if (GetTableVersion(L"商品") != 6 || GetTableVersion(L"顧客") != 3) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
		StkPlPrintf("...[OK]\r\n");
		StkPlPrintf("Both GetTableVersion(\"customer\") and GetTableVersion(\"merchandise\") return 0 after LoadData.");
		LockAllTable(LOCK_EXCLUSIVE);
		LoadData(L"TableVersion.std");
		UnlockAllTable();
		if (GetTableVersion(L"商品") != 0 || GetTableVersion(L"顧客") != 0) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
		StkPlPrintf("...[OK]\r\n");
	}

	// 商品名="竹内伸也"を指定してDeleteRecord後，GetTableVersion("顧客")で0が返却され，GetTableVersion("商品")で3が返却される
	{
		StkPlPrintf("GetTableVersion(\"customer\") returns 0 and GetTableVersion(\"merchandise\") returns 3 after DeleteRecord execution with merchandise specification.");
		ColumnData* ColDat[5];
		RecordData* RecDat;
		ColDat[0] = new ColumnDataWStr(L"商品名", L"竹内伸也");
		RecDat = new RecordData(L"商品", ColDat, 1);
		LockTable(L"商品", LOCK_EXCLUSIVE);
		DeleteRecord(RecDat);
		UnlockTable(L"商品");
		delete RecDat;
		if (GetTableVersion(L"商品") != 3 || GetTableVersion(L"顧客") != 0) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
		StkPlPrintf("...[OK]\r\n");
	}

	// DeleteTable(L"商品")でテーブルを削除後，GetTableVersion(L"商品")で-1が返り，GetTableVersion(L"顧客")で-1以外が返る
	{
		StkPlPrintf("GetTableVersion(L\"merchandise\") returns -1 and GetTableVersion(L\"customer\") returns other than -1 after table removal by DeleteTable(L\"merchandise\").");
		if (DeleteTable(L"商品") != 0) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
		if (GetTableVersion(L"商品") != -1 || GetTableVersion(L"顧客") == -1) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
		StkPlPrintf("...[OK]\r\n");
	}

	// さらにDeleteTable(L"顧客")でテーブルを削除後，GetTableVersion(L"商品")，GetTableVersion(L"顧客")ともに-1が返る
	{
		StkPlPrintf("Both GetTableVersion(L\"merchandise\") and GetTableVersion(\"customer\") return -1 after table removal by DeleteTable(L\"customer\")");
		if (DeleteTable(L"顧客") != 0) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
		if (GetTableVersion(L"商品") != -1 || GetTableVersion(L"顧客") != -1) {
			StkPlPrintf("...[NG]\r\n");
			return -1;
		}
		StkPlPrintf("...[OK]\r\n");
	}

	return 0;
}
