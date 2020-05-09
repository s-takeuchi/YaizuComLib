#include <thread>
#include <mutex>

#include "../stkpl/StkPl.h"
#include "DataAcController.h"
#include "stkdata.h"
#include "stkdataapi.h"


//////////////////////////////////////////////////////////////////////////////////////

thread_local int DataAcController::m_CompareColOffset;
thread_local int DataAcController::m_CompareColType;
thread_local int DataAcController::m_CompareColSize;
DataAcController* DataAcController::StkDataInstance;

//////////////////////////////////////////////////////////////////////////////////////

// Constructor
DataAcController::DataAcController()
{
	// Initialization of table information
	int Loop;
	for (Loop = 0; Loop < MAX_TABLE_NUMBER; Loop++) {
		m_TableAddr[Loop] = NULL;
		m_TableSize[Loop] = 0;
		m_TableLock[Loop] = 0;
		m_TableVer[Loop] = 0;
		ClearParameter(Loop);
	}
}

// Destructor
DataAcController::~DataAcController()
{
}

// Get this instance
DataAcController* DataAcController::GetInstance()
{
	static int Init = 1;
	if (Init == 1) {
		StkDataInstance = new DataAcController();
		Init = 0;
	}
	return StkDataInstance;
}

//////////////////////////////////////////////////////////////////////////////////////

// Clear parameters
// [in] int : Table ID
void DataAcController::ClearParameter(int TableId)
{
	int Loop;

	StkPlWcsCpy(m_TableName[TableId], TABLE_NAME_SIZE, L"");
	m_RecordSize[TableId] = 0;
	m_RecordCount[TableId] = 0;
	for (Loop = 0; Loop < MAX_COLUMN_NUMBER; Loop++) {
		StkPlWcsCpy(m_ColumnName[TableId][Loop], COLUMN_NAME_SIZE, L"");
		m_ColumnOffset[TableId][Loop] = -1;
		m_ColumnType[TableId][Loop] = -1;
		m_ColumnSize[TableId][Loop] = -1;
	}
	m_ColumnCount[TableId] = 0;
}

// Get free table ID which was found first.
// [out] int : Table ID (-1: Free table ID can not be allocated.)
int DataAcController::GetFreeTableId()
{
	int Loop;
	for (Loop = 0; Loop < MAX_TABLE_NUMBER; Loop++) {
		if (m_TableAddr[Loop] == NULL &&
			m_TableSize[Loop] == 0 &&
			StkPlWcsCmp(m_TableName[Loop], L"") == 0) {
			return Loop;
		}
	}
	return -1;
}

// Search the table which has the specified table name.
// [in] wchar_t* : Table name
// [out] int : Table Id (-1: Table does not found.)
int DataAcController::SearchTable(const wchar_t* TableName)
{
	int Loop;
	for (Loop = 0; Loop < MAX_TABLE_NUMBER; Loop++) {
		if (StkPlWcsCmp(m_TableName[Loop], TableName) == 0) {
			return Loop;
		}
	}
	return -1;
}

// Get column index
// [in] wchar_t* : Table name
// [in] wchar_t* : Column name
// [out] int : Column index (-1:Not found)
int DataAcController::GetColumnIndex(const wchar_t* TableName, const wchar_t* ColumnName)
{
	if (TableName == NULL || StkPlWcsCmp(TableName, L"") == 0) {
		return -1;
	}
	int TableId = SearchTable(TableName);
	if (TableId == -1) {
		return -1;
	}
	int Loop;
	for (Loop = 0; Loop < m_ColumnCount[TableId]; Loop++) {
		if (StkPlWcsCmp(m_ColumnName[TableId][Loop], ColumnName) == 0) {
			return Loop;
		}
	}
	return -1;
}

// Check and change characters
// [in] char* : Start address of string
// [in] char* : End address of string
void DataAcController::CheckAndChangeChar(char* Start, char* End)
{
	char* LoopChr;
	for (LoopChr = Start; LoopChr <= End; LoopChr++) {
		if (*LoopChr == 0x00 || (*LoopChr >= 0x20 && *LoopChr <= 0x7e)) {
			continue;
		}
		*LoopChr = ' ';
	}
}

//////////////////////////////////////////////////////////////////////////////////////

// Allocate data area of the specified table ID
// [in] SIZE_T : Allocation size
// [in] int : Table ID
// [out] int : Result code (0: Success, -1: Error)
int DataAcController::AllocDataArea(size_t Size, int TableId)
{
	if (TableId < 0 || TableId >= MAX_TABLE_NUMBER) {
		return -1;
	}
	char* Result = new (std::nothrow) char[Size];
	if (Result == NULL) {
		return -1;
	}
	m_TableAddr[TableId] = Result;
	m_TableSize[TableId] = Size;
	m_TableVer[TableId] = 0;
	return 0;
}

// Release data area of the specified table ID
// [in] int : Table ID
// [out] int : Result code (Always zero returns)
int DataAcController::FreeDataArea(int TableId)
{
	if (TableId < 0 || TableId >= MAX_TABLE_NUMBER) {
		return -1;
	}
	delete m_TableAddr[TableId];
	m_TableAddr[TableId] = NULL;
	m_TableSize[TableId] = 0;
	m_TableVer[TableId] = 0;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////

// Create table
// [in] TableDef* : Table information for the creation
// [out] int : Result Code (0: Sucess, -1: Memory allocation error, -2: Parameter error, -4: Maximum number of tables is exceeded)
int DataAcController::CreateTable(TableDef* TabDef)
{
	// Check whether specified TableDef has already been existed.
	wchar_t* TableName = TabDef->GetTableName();
	if (StkPlWcsCmp(TableName, L"") == 0) {
		return -2;
	}
	int TableId = SearchTable(TableName);
	if (TableId != -1) {
		return -2;
	}
	TableId = GetFreeTableId();
	if (TableId == -1) {
		return -4;
	}

	// Clear table lock information
	m_TableLock[TableId] = 0;

	// Get number of column
	int NumOfColumn = TabDef->GetNumOfColumn();
	if (NumOfColumn == 0) {
		return -2;
	}
	// Get number of record
	int NumOfRecord = TabDef->GetMaxRecord();
	if (NumOfRecord <= 0 || NumOfRecord >= MAX_RECORD) {
		return -2;
	}

	// Calculate allocation size
	size_t AllocSize = 0;
	int Loop;
	for (Loop = 0; Loop < NumOfColumn; Loop++) {
		ColumnDef* CurrentColDef = TabDef->GetColumnDef(Loop);
		if (CurrentColDef == NULL) {
			continue;
		}
		if (StkPlWcsCmp(CurrentColDef->GetColumnName(), L"") == 0) {
			ClearParameter(TableId);
			return -2;
		}
		m_ColumnOffset[TableId][Loop] = (int)AllocSize;
		if (CurrentColDef->GetColumnType() == COLUMN_TYPE_INT) {
			AllocSize += 4;
			m_ColumnSize[TableId][Loop] = 4;
			m_ColumnType[TableId][Loop] = COLUMN_TYPE_INT;
		} else if (CurrentColDef->GetColumnType() == COLUMN_TYPE_FLOAT) {
			AllocSize += 4;
			m_ColumnSize[TableId][Loop] = 4;
			m_ColumnType[TableId][Loop] = COLUMN_TYPE_FLOAT;
		} else if (CurrentColDef->GetColumnType() == COLUMN_TYPE_STR) {
			int StrLen = ((ColumnDefStr*)CurrentColDef)->GetMaxLength();
			if (StrLen <= 1 || StrLen > 256) {
				ClearParameter(TableId);
				return -2;
			}
			AllocSize += StrLen;
			m_ColumnSize[TableId][Loop] = StrLen;
			m_ColumnType[TableId][Loop] = COLUMN_TYPE_STR;
		} else if (CurrentColDef->GetColumnType() == COLUMN_TYPE_WSTR) {
			int StrLen = ((ColumnDefWStr*)CurrentColDef)->GetMaxLength();
			if (StrLen <= 1 || StrLen > 256) {
				ClearParameter(TableId);
				return -2;
			}
			AllocSize += (StrLen * sizeof(wchar_t));
			m_ColumnSize[TableId][Loop] = (StrLen * sizeof(wchar_t));
			m_ColumnType[TableId][Loop] = COLUMN_TYPE_WSTR;
		} else if (CurrentColDef->GetColumnType() == COLUMN_TYPE_BIN) {
			int StrLen = ((ColumnDefBin*)CurrentColDef)->GetMaxLength();
			if (StrLen < 1 || StrLen > 10000000) {
				ClearParameter(TableId);
				return -2;
			}
			AllocSize += StrLen;
			m_ColumnSize[TableId][Loop] = StrLen;
			m_ColumnType[TableId][Loop] = COLUMN_TYPE_BIN;
		} else {
			ClearParameter(TableId);
			return -2;
		}
		StkPlWcsNCpy(m_ColumnName[TableId][Loop], COLUMN_NAME_SIZE, CurrentColDef->GetColumnName(), COLUMN_NAME_SIZE - 1);
		m_ColumnName[TableId][Loop][COLUMN_NAME_SIZE - 1] = L'\0';
		m_ColumnCount[TableId]++;
	}
	m_RecordSize[TableId] = AllocSize;
	AllocSize *= NumOfRecord;

	// Allocate memory
	if (AllocDataArea(AllocSize, TableId) == -1) {
		ClearParameter(TableId);
		return -1;
	}
	StkPlWcsNCpy(m_TableName[TableId], TABLE_NAME_SIZE, TableName, TABLE_NAME_SIZE - 1);
	m_TableName[TableId][TABLE_NAME_SIZE - 1] = L'\0';

	StkPlMemSet(m_TableAddr[TableId], 0, m_TableSize[TableId]);

	return 0;
}

// Delete table
// [in] wchar_t* : Table name
// [out] int : Result Code (0: Sucess, -1: Memory unallocation error, -2: Parameter error, -3: Decommit error)
int DataAcController::DeleteTable(const wchar_t* TableName)
{
	if (TableName == NULL || StkPlWcsCmp(TableName, L"") == 0) {
		return -2;
	}
	int TableId = SearchTable(TableName);
	if (TableId == -1) {
		return -2;
	}

	if (FreeDataArea(TableId) != 0) {
		return -1;
	}

	ClearParameter(TableId);

	return 0;
}

// Rename table name
// [in] OldName : Old name
// [in] NewName : New name
// [out] int : Result Code (0: Sucess, -1: Error)
int DataAcController::RenameTable(const wchar_t* OldName, const wchar_t* NewName)
{
	if (StkPlWcsCmp(NewName, L"") == 0) {
		return -1;
	}
	int NewTableId = SearchTable(NewName);
	if (NewTableId != -1) {
		return -1;
	}
	int OldTableId = SearchTable(OldName);
	if (OldTableId == -1) {
		return -1;
	}
	StkPlWcsNCpy(m_TableName[OldTableId], TABLE_NAME_SIZE, NewName, TABLE_NAME_SIZE - 1);
	m_TableName[OldTableId][TABLE_NAME_SIZE - 1] = L'\0';
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////

// Lock table
// [in] wchar_t* : Table name
// [in] int    : Lock type (LOCK_SHARE or LOCK_EXCLUSIVE)
// [out] int : Result Code (0: Sucess, -1: Locking error)
int DataAcController::LockTable(const wchar_t* TableName, int LockType)
{
	if (TableName == NULL || StkPlWcsCmp(TableName, L"") == 0) {
		return -1;
	}
	int TableId = SearchTable(TableName);
	if (TableId == -1) {
		return -1;
	}
	if (LockType != LOCK_SHARE && LockType != LOCK_EXCLUSIVE) {
		return -1;
	}

	Cs4Lock.lock();
	while (true) {
		if (m_TableLock[TableId] >= 0 && LockType == LOCK_SHARE) {
			m_TableLock[TableId]++;
			break;
		}
		if (m_TableLock[TableId] < 0 && LockType == LOCK_SHARE) {
			Cs4Lock.unlock();
			StkPlSleepMs(200);
			Cs4Lock.lock();
			continue;
		}
		if (m_TableLock[TableId] == 0 && LockType == LOCK_EXCLUSIVE) {
			m_TableLock[TableId] = -1;
			break;
		}
		if ((m_TableLock[TableId] > 0 || m_TableLock[TableId] < 0) && LockType == LOCK_EXCLUSIVE) {
			Cs4Lock.unlock();
			StkPlSleepMs(200);
			Cs4Lock.lock();
			continue;
		}
	}
	Cs4Lock.unlock();

	return 0;
}

// Unlock table
// [in] wchar_t* : Table name
// [out] int : Result Code (0: Sucess, -1: Locking error)
int DataAcController::UnlockTable(const wchar_t* TableName)
{
	if (TableName == NULL || StkPlWcsCmp(TableName, L"") == 0) {
		return -1;
	}
	int TableId = SearchTable(TableName);
	if (TableId == -1) {
		return -1;
	}

	Cs4Lock.lock();
	if (m_TableLock[TableId] > 0) {
		m_TableLock[TableId]--;
	}
	if (m_TableLock[TableId] < 0) {
		m_TableLock[TableId] = 0;
	}
	Cs4Lock.unlock();

	return 0;
}

// Unlock all tables
// [in] int    : Lock type (LOCK_SHARE or LOCK_EXCLUSIVE)
// [out] int : Result Code (0: Sucess)
int DataAcController::LockAllTable(int LockType)
{
	int LoopTbl;

	// Check whether the table has already been locked.
	for (LoopTbl = 0; LoopTbl < MAX_TABLE_NUMBER; LoopTbl++) {
		if (StkPlWcsCmp(m_TableName[LoopTbl], L"") == 0) {
			continue;
		}
		LockTable(m_TableName[LoopTbl], LockType);
	}
	return 0;
}

// Lock all tables
// [out] int : Result Code (0: Sucess)
int DataAcController::UnlockAllTable()
{
	int LoopTbl;

	// Check whether the table has already been locked.
	for (LoopTbl = 0; LoopTbl < MAX_TABLE_NUMBER; LoopTbl++) {
		if (StkPlWcsCmp(m_TableName[LoopTbl], L"") == 0) {
			continue;
		}
		UnlockTable(m_TableName[LoopTbl]);
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////

// Insert Record
// [in] RecordData* : Record data
// [out] int : Result code (0: Sucess, -1: Error)
int DataAcController::InsertRecord(RecordData* RecDat)
{
	if (RecDat == NULL) {
		return -1;
	}

	do {
		wchar_t* TableName = RecDat->GetTableName();
		if (TableName == NULL || StkPlWcsCmp(TableName, L"") == 0) {
			return -1;
		}
		int TableId = SearchTable(TableName);
		if (TableId == -1) {
			return -1;
		}
		if (m_TableLock[TableId] != -1) {
			return -1;
		}

		unsigned char* Addr = (unsigned char*)m_TableAddr[TableId];
		Addr = Addr + (m_RecordCount[TableId] * m_RecordSize[TableId]);
		int Loop = 0;
		for (Loop = 0; Loop < m_ColumnCount[TableId]; Loop++) {
			if ((size_t)Addr >= m_TableSize[TableId] + (size_t)m_TableAddr[TableId]) {
				return -1;
			}
			ColumnData* ColDat = RecDat->GetColumn(Loop);
			if (ColDat == NULL) {
				return -1;
			}
			if (StkPlWcsCmp(m_ColumnName[TableId][Loop], ColDat->GetColumnName()) == 0) {
				if (ColDat->GetColumnType() == COLUMN_TYPE_INT) {
					if (m_ColumnType[TableId][Loop] != COLUMN_TYPE_INT) {
						return -1;
					}
					ColumnDataInt* ColDatInt = (ColumnDataInt*)ColDat;
					int32_t Value = (int32_t)ColDatInt->GetValue();
					int32_t* ValueStore = (int32_t*)Addr;
					*ValueStore = Value;
				} else if (ColDat->GetColumnType() == COLUMN_TYPE_FLOAT) {
					if (m_ColumnType[TableId][Loop] != COLUMN_TYPE_FLOAT) {
						return -1;
					}
					ColumnDataFloat* ColDatFloat = (ColumnDataFloat*)ColDat;
					float Value = (float)ColDatFloat->GetValue();
					float* ValueStore = (float*)Addr;
					*ValueStore = Value;
				} else if (ColDat->GetColumnType() == COLUMN_TYPE_STR) {
					if (m_ColumnType[TableId][Loop] != COLUMN_TYPE_STR) {
						return -1;
					}
					ColumnDataStr* ColDatStr = (ColumnDataStr*)ColDat;
					char* Value = (char*)ColDatStr->GetValue();
					char* ValueStore = (char*)Addr;
					StkPlStrNCpy(ValueStore, m_ColumnSize[TableId][Loop], Value, m_ColumnSize[TableId][Loop] - 1);
					ValueStore[m_ColumnSize[TableId][Loop] - 1] = '\0';
					//strncpy_sの代わりにCopyMemoryを使用することはできない。CopyMemoryの場合
					//制限長いっぱいの文字列をコピーしたときに終端に\0が付加されない。
					CheckAndChangeChar(ValueStore, ValueStore + m_ColumnSize[TableId][Loop] - 1);
				} else if (ColDat->GetColumnType() == COLUMN_TYPE_WSTR) {
					if (m_ColumnType[TableId][Loop] != COLUMN_TYPE_WSTR) {
						return -1;
					}
					ColumnDataWStr* ColDatWStr = (ColumnDataWStr*)ColDat;
					wchar_t* Value = (wchar_t*)ColDatWStr->GetValue();
					wchar_t* ValueStore = (wchar_t*)Addr;
					size_t TmpSize = m_ColumnSize[TableId][Loop] / sizeof(wchar_t);
					StkPlWcsNCpy(ValueStore, TmpSize, Value, TmpSize - 1);
					ValueStore[TmpSize - 1] = L'\0';
				} else if (ColDat->GetColumnType() == COLUMN_TYPE_BIN) {
					if (m_ColumnType[TableId][Loop] != COLUMN_TYPE_BIN) {
						return -1;
					}
					ColumnDataBin* ColDatBin = (ColumnDataBin*)ColDat;
					wchar_t* Value = (wchar_t*)ColDatBin->GetValue();
					wchar_t* ValueStore = (wchar_t*)Addr;
					StkPlMemCpy(ValueStore, Value, m_ColumnSize[TableId][Loop]);
				}
				Addr += m_ColumnSize[TableId][Loop];
			} else {
				return -1;
			}
		}
		if (RecDat->GetColumn(Loop) != NULL) {
			return -1;
		}
		m_RecordCount[TableId]++;
		IncrementTableVersion(TableId);
		RecDat = RecDat->GetNextRecord();
	} while (RecDat != NULL);

	return 0;
}

// Get record
// [in] wchar_t* : Table name
// [out] RecordData* : Pointer to the records (NULL:Error)
RecordData* DataAcController::GetRecord(const wchar_t* TableName)
{
	if (TableName == NULL || StkPlWcsCmp(TableName, L"") == 0) {
		return NULL;
	}
	int TableId = SearchTable(TableName);
	if (TableId == -1) {
		return NULL;
	}
	if (m_TableLock[TableId] == 0) {
		return NULL;
	}

	unsigned char* Addr = (unsigned char*)m_TableAddr[TableId];
	ColumnData* ColDat;
	RecordData* RecDat;
	RecordData* PrevRecDat = NULL;
	RecordData* FrstRecDat = NULL;
	int LoopRec;
	int LoopCol;
	for (LoopRec = 0; LoopRec < m_RecordCount[TableId]; LoopRec++) {
		RecDat = new RecordData();
		if (FrstRecDat == NULL) {
			FrstRecDat = RecDat;
		}
		RecDat->SetTableName(TableName);
		if (PrevRecDat != NULL) {
			PrevRecDat->SetNextRecord(RecDat);
		}
		for (LoopCol = 0; LoopCol < m_ColumnCount[TableId]; LoopCol++) {
			wchar_t* ColNam = m_ColumnName[TableId][LoopCol];
			if (m_ColumnType[TableId][LoopCol] == COLUMN_TYPE_INT) {
				int32_t* Value = (int32_t*)Addr;
				ColDat = new ColumnDataInt(ColNam, *Value);
			} else if (m_ColumnType[TableId][LoopCol] == COLUMN_TYPE_FLOAT) {
				float* Value = (float*)Addr;
				ColDat = new ColumnDataFloat(ColNam, *Value);
			} else if (m_ColumnType[TableId][LoopCol] == COLUMN_TYPE_STR) {
				char* Value = (char*)Addr;
				ColDat = new ColumnDataStr(ColNam, Value);
			} else if (m_ColumnType[TableId][LoopCol] == COLUMN_TYPE_WSTR) {
				wchar_t* Value = (wchar_t*)Addr;
				ColDat = new ColumnDataWStr(ColNam, Value);
			} else if (m_ColumnType[TableId][LoopCol] == COLUMN_TYPE_BIN) {
				unsigned char* Value = (unsigned char*)Addr;
				ColDat = new ColumnDataBin(ColNam, Value, m_ColumnSize[TableId][LoopCol]);
			}
			RecDat->AddColumn(ColDat);
			Addr += m_ColumnSize[TableId][LoopCol];
		}
		PrevRecDat = RecDat;
	}
	return FrstRecDat;
}

// Get records which are matched by the specified condition
// [in] RecordData* : Pointer to the record to search
// [out] RecordData* : Pointer to the record for response (NULL:Error or Not found)
RecordData* DataAcController::GetRecord(RecordData* RecDatReq)
{
	int Dummy;
	return CommonRecordOperation(OPE_GET, RecDatReq, NULL, &Dummy);
}

// Delete all records of the specified table
// [in] wchar_t* : Table name
// [out] int : Result code (0: Sucess, -1: Error)
int DataAcController::DeleteRecord(const wchar_t* TableName)
{
	if (TableName == NULL || StkPlWcsCmp(TableName, L"") == 0) {
		return -1;
	}
	int TableId = SearchTable(TableName);
	if (TableId == -1) {
		return -1;
	}
	if (m_TableLock[TableId] != -1) {
		return -1;
	}
	m_RecordCount[TableId] = 0;
	IncrementTableVersion(TableId);
	return 0;
}

// Delete records which are matched by the specified condition
// [in] RecordData* : Pointer to the record to search
int DataAcController::DeleteRecord(RecordData* RecDatReq)
{
	int RetCode;
	CommonRecordOperation(OPE_DELETE, RecDatReq, NULL, &RetCode);
	return RetCode;
}

// Update record which are matched by specified condition
// [in] RecordData* : Pointer to the record to search
// [in] RecordData* : Pointer to the record to update
int DataAcController::UpdateRecord(RecordData* RecDatReq, RecordData* RecDatUdt)
{
	int RetCode;
	CommonRecordOperation(OPE_UPDATE, RecDatReq, RecDatUdt, &RetCode);
	return RetCode;
}

//////////////////////////////////////////////////////////////////////////////////////

// Increment table version
// [in] : int : Table index
void DataAcController::IncrementTableVersion(int Index)
{
	if (m_TableVer[Index] == 2147483647) {
		m_TableVer[Index] = 0;
	} else {
		m_TableVer[Index]++;
	}
}

// Common operation for getting, deleting and updating record.
// [in] int : Operation type. OPE_GET, OPE_DELETE and OPE_UPDATE can be specified.
// [in] RecordData* : Pointer to the RecordData to search.
// [in] RecordData* : Pointer to the RecordData for updating. (NULL: When the operation is not for update operation.)
// [in] RetCode : Result code (0: Sucess, -1: Error)
// [out] RecordData* : Pointer to the RecordData for getting. (NULL:Not a get operation)
RecordData* DataAcController::CommonRecordOperation(int OpType, RecordData* RecDatReq, RecordData* RecDatUdt, int* RetCode)
{
	if (RecDatReq == NULL) {
		*RetCode = -1;
		return NULL;
	}
	if (RecDatUdt == NULL && OpType == OPE_UPDATE) {
		*RetCode = -1;
		return NULL;
	}

	RecordData* RecDatRes = NULL;
	RecordData* PrevRecDatRes = NULL;
	RecordData* FrstRecDatRes = NULL;

	do {
		wchar_t* TableName = RecDatReq->GetTableName();
		if (TableName == NULL || StkPlWcsCmp(TableName, L"") == 0) {
			*RetCode = -1;
			return NULL;
		}
		int TableId = SearchTable(TableName);
		if (TableId == -1) {
			*RetCode = -1;
			return NULL;
		}
		if (OpType == OPE_GET) {
			if (m_TableLock[TableId] == 0) {
				*RetCode = -1;
				return NULL;
			}
		} else {
			if (m_TableLock[TableId] != -1) {
				*RetCode = -1;
				return NULL;
			}
		}

		unsigned char* Addr = (unsigned char*)m_TableAddr[TableId];

		// Search records by the presented condition.
		int RecCnt = m_RecordCount[TableId];
		int ColCnt = RecDatReq->GetColumnCount();
		int LoopRec;
		int LoopCol;
		for (LoopRec = 0; LoopRec < RecCnt; LoopRec++) {

			// check records by the presented condition(Columns).
			int FoundColCnt = 0;
			bool ProceedNextReq = false;
			for (LoopCol = 0; LoopCol < ColCnt; LoopCol++) {
				ColumnData* CurColReq = RecDatReq->GetColumn(LoopCol);
				int ColIndx = GetColumnIndex(TableName, CurColReq->GetColumnName());
				// If unknown column name was presented, proceed to next record of the request.
				if (ColIndx == -1) {
					ProceedNextReq = true;
					break;
				}
				// If column type is not matched, proceed to next record of the request.
				if (CurColReq->GetColumnType() != m_ColumnType[TableId][ColIndx]) {
					ProceedNextReq = true;
					break;
				}
				// Compare value
				if (CurColReq->GetColumnType() == COLUMN_TYPE_INT) {
					int32_t* Value = (int32_t*)(Addr + m_ColumnOffset[TableId][ColIndx]);
					ColumnDataInt* CurColIntReq = (ColumnDataInt*)CurColReq;
					if ((CurColReq->GetComparisonOperator() == COMP_EQUAL && (int)*Value == CurColIntReq->GetValue()) ||
						(CurColReq->GetComparisonOperator() == COMP_NOT_EQUAL && (int)*Value != CurColIntReq->GetValue()) ||
						(CurColReq->GetComparisonOperator() == COMP_GE && (int)*Value >= CurColIntReq->GetValue()) ||
						(CurColReq->GetComparisonOperator() == COMP_GT && (int)*Value > CurColIntReq->GetValue()) ||
						(CurColReq->GetComparisonOperator() == COMP_LE && (int)*Value <= CurColIntReq->GetValue()) ||
						(CurColReq->GetComparisonOperator() == COMP_LT && (int)*Value < CurColIntReq->GetValue())) {
						FoundColCnt++;
					}
				} else if (CurColReq->GetColumnType() == COLUMN_TYPE_FLOAT) {
					float* Value = (float*)(Addr + m_ColumnOffset[TableId][ColIndx]);
					ColumnDataFloat* CurColFloatReq = (ColumnDataFloat*)CurColReq;
					if ((CurColReq->GetComparisonOperator() == COMP_EQUAL && (float)*Value == CurColFloatReq->GetValue()) ||
						(CurColReq->GetComparisonOperator() == COMP_NOT_EQUAL && (float)*Value != CurColFloatReq->GetValue()) ||
						(CurColReq->GetComparisonOperator() == COMP_GE && (float)*Value >= CurColFloatReq->GetValue()) ||
						(CurColReq->GetComparisonOperator() == COMP_GT && (float)*Value > CurColFloatReq->GetValue()) ||
						(CurColReq->GetComparisonOperator() == COMP_LE && (float)*Value <= CurColFloatReq->GetValue()) ||
						(CurColReq->GetComparisonOperator() == COMP_LT && (float)*Value < CurColFloatReq->GetValue())) {
						FoundColCnt++;
					}
				} else if (CurColReq->GetColumnType() == COLUMN_TYPE_STR) {
					char* Value = (char*)(Addr + m_ColumnOffset[TableId][ColIndx]);
					ColumnDataStr* CurColStrReq = (ColumnDataStr*)CurColReq;
					if ((CurColReq->GetComparisonOperator() == COMP_EQUAL && StkPlStrCmp(Value, CurColStrReq->GetValue()) == 0) ||
						(CurColReq->GetComparisonOperator() == COMP_NOT_EQUAL && StkPlStrCmp(Value, CurColStrReq->GetValue()) != 0) ||
						(CurColReq->GetComparisonOperator() == COMP_CONTAIN && StkPlStrStr(Value, CurColStrReq->GetValue()) != 0) ||
						(CurColReq->GetComparisonOperator() == COMP_NOT_CONTAIN && StkPlStrStr(Value, CurColStrReq->GetValue()) == 0)) {
						FoundColCnt++;
					}
				} else if (CurColReq->GetColumnType() == COLUMN_TYPE_WSTR) {
					wchar_t* Value = (wchar_t*)(Addr + m_ColumnOffset[TableId][ColIndx]);
					ColumnDataWStr* CurColWStrReq = (ColumnDataWStr*)CurColReq;
					if ((CurColReq->GetComparisonOperator() == COMP_EQUAL && StkPlWcsCmp(Value, CurColWStrReq->GetValue()) == 0) ||
						(CurColReq->GetComparisonOperator() == COMP_NOT_EQUAL && StkPlWcsCmp(Value, CurColWStrReq->GetValue()) != 0) ||
						(CurColReq->GetComparisonOperator() == COMP_CONTAIN && StkPlWcsStr(Value, CurColWStrReq->GetValue()) != 0) ||
						(CurColReq->GetComparisonOperator() == COMP_NOT_CONTAIN && StkPlWcsStr(Value, CurColWStrReq->GetValue()) == 0)) {
						FoundColCnt++;
					}
				} else if (CurColReq->GetColumnType() == COLUMN_TYPE_BIN) {
					unsigned char* Value = (unsigned char*)(Addr + m_ColumnOffset[TableId][ColIndx]);
					ColumnDataBin* CurColBinReq = (ColumnDataBin*)CurColReq;
					if ((CurColReq->GetComparisonOperator() == COMP_EQUAL && StkPlMemCmp(Value, CurColBinReq->GetValue(), m_ColumnSize[TableId][ColIndx]) == 0) ||
						(CurColReq->GetComparisonOperator() == COMP_NOT_EQUAL && StkPlMemCmp(Value, CurColBinReq->GetValue(), m_ColumnSize[TableId][ColIndx]) != 0)) {
						FoundColCnt++;
					}
				}
			}
			if (ProceedNextReq == true) {
				break;
			}

			// When the record was found, following process is executed.
			if (FoundColCnt == ColCnt) {
				if (OpType == OPE_GET) {
					RecDatRes = new RecordData();
					if (FrstRecDatRes == NULL) {
						FrstRecDatRes = RecDatRes;
					}
					RecDatRes->SetTableName(RecDatReq->GetTableName());
					if (PrevRecDatRes != NULL) {
						PrevRecDatRes->SetNextRecord(RecDatRes);
					}
					ColumnData* NewColDat;
					for (LoopCol = 0; LoopCol < m_ColumnCount[TableId]; LoopCol++) {
						wchar_t* ColNam = m_ColumnName[TableId][LoopCol];
						if (m_ColumnType[TableId][LoopCol] == COLUMN_TYPE_INT) {
							int32_t* Value = (int32_t*)Addr;
							NewColDat = new ColumnDataInt(ColNam, *Value);
						} else if (m_ColumnType[TableId][LoopCol] == COLUMN_TYPE_FLOAT) {
							float* Value = (float*)Addr;
							NewColDat = new ColumnDataFloat(ColNam, *Value);
						} else if (m_ColumnType[TableId][LoopCol] == COLUMN_TYPE_STR) {
							char* Value = (char*)Addr;
							NewColDat = new ColumnDataStr(ColNam, Value);
						} else if (m_ColumnType[TableId][LoopCol] == COLUMN_TYPE_WSTR) {
							wchar_t* Value = (wchar_t*)Addr;
							NewColDat = new ColumnDataWStr(ColNam, Value);
						} else if (m_ColumnType[TableId][LoopCol] == COLUMN_TYPE_BIN) {
							unsigned char* Value = (unsigned char*)Addr;
							NewColDat = new ColumnDataBin(ColNam, Value, m_ColumnSize[TableId][LoopCol]);
						}
						RecDatRes->AddColumn(NewColDat);
						Addr += m_ColumnSize[TableId][LoopCol];
					}
					PrevRecDatRes = RecDatRes;
				} else if (OpType == OPE_DELETE) {
					// 性能問題となるコードの修正 #10055
					size_t Source = (size_t)m_TableAddr[TableId] + (m_RecordCount[TableId] - 1) * m_RecordSize[TableId];
					StkPlMemCpy((void*)Addr, (const void*)Source, m_RecordSize[TableId]);

					/* 性能問題となるコードの修正 #10055
					SIZE_T Length = (SIZE_T)(m_TableSize[TableId] + (SIZE_T)m_TableAddr[TableId] - (SIZE_T)Addr - m_RecordSize[TableId]);
					CopyMemory((void*)Addr, (void*)(Addr + m_RecordSize[TableId]), Length);
					FillMemory((void*)(m_TableSize[TableId] + (SIZE_T)m_TableAddr[TableId] - m_RecordSize[TableId]), m_RecordSize[TableId], 0);
					*/
					m_RecordCount[TableId]--;
					IncrementTableVersion(TableId);
				} else if (OpType == OPE_UPDATE) {
					if (StkPlWcsCmp(TableName, RecDatUdt->GetTableName()) != 0) {
						*RetCode = -1;
						return NULL;
					}
					int LoopUdt;
					int ColCntUdt = RecDatUdt->GetColumnCount();
					bool UdtFlag = false;
					for (LoopUdt = 0; LoopUdt < ColCntUdt; LoopUdt++) {
						ColumnData* ColDatUdt = RecDatUdt->GetColumn(LoopUdt);
						if (ColDatUdt == NULL) {
							continue;
						}
						int CurColIdx = GetColumnIndex(RecDatUdt->GetTableName(), ColDatUdt->GetColumnName());
						if (CurColIdx == -1) {
							continue;
						}

						if (ColDatUdt->GetColumnType() == COLUMN_TYPE_INT) {
							if (m_ColumnType[TableId][CurColIdx] != COLUMN_TYPE_INT) {
								continue;
							}
							ColumnDataInt* ColDatInt = (ColumnDataInt*)ColDatUdt;
							int32_t Value = (int32_t)ColDatInt->GetValue();
							int32_t* ValueStore = (int32_t*)(Addr + m_ColumnOffset[TableId][CurColIdx]);
							*ValueStore = Value;
							UdtFlag = true;
						} else if (ColDatUdt->GetColumnType() == COLUMN_TYPE_FLOAT) { 
							if (m_ColumnType[TableId][CurColIdx] != COLUMN_TYPE_FLOAT) {
								continue;
							}
							ColumnDataFloat* ColDatFloat = (ColumnDataFloat*)ColDatUdt;
							float Value = (float)ColDatFloat->GetValue();
							float* ValueStore = (float*)(Addr + m_ColumnOffset[TableId][CurColIdx]);
							*ValueStore = Value;
							UdtFlag = true;
						} else if (ColDatUdt->GetColumnType() == COLUMN_TYPE_STR) {
							if (m_ColumnType[TableId][CurColIdx] != COLUMN_TYPE_STR) {
								continue;
							}
							ColumnDataStr* ColDatStr = (ColumnDataStr*)ColDatUdt;
							char* Value = (char*)ColDatStr->GetValue();
							char* ValueStore = (char*)(Addr + m_ColumnOffset[TableId][CurColIdx]);
							StkPlStrNCpy(ValueStore, m_ColumnSize[TableId][CurColIdx], Value, m_ColumnSize[TableId][CurColIdx] - 1);
							ValueStore[m_ColumnSize[TableId][CurColIdx] - 1] = '\0';
							//strncpy_sの代わりにCopyMemoryを使用することはできない。CopyMemoryの場合
							//制限長いっぱいの文字列をコピーしたときに終端に\0が付加されない。
							CheckAndChangeChar(ValueStore, ValueStore + m_ColumnSize[TableId][CurColIdx] - 1);
							UdtFlag = true;
						} else if (ColDatUdt->GetColumnType() == COLUMN_TYPE_WSTR) {
							if (m_ColumnType[TableId][CurColIdx] != COLUMN_TYPE_WSTR) {
								continue;
							}
							ColumnDataWStr* ColDatWStr = (ColumnDataWStr*)ColDatUdt;
							wchar_t* Value = (wchar_t*)ColDatWStr->GetValue();
							wchar_t* ValueStore = (wchar_t*)(Addr + m_ColumnOffset[TableId][CurColIdx]);
							size_t TmpSize = m_ColumnSize[TableId][CurColIdx] / sizeof(wchar_t);
							StkPlWcsNCpy(ValueStore, TmpSize, Value, TmpSize - 1);
							ValueStore[TmpSize - 1] = L'\0';
							UdtFlag = true;
						} else if (ColDatUdt->GetColumnType() == COLUMN_TYPE_BIN) {
							if (m_ColumnType[TableId][CurColIdx] != COLUMN_TYPE_BIN) {
								continue;
							}
							ColumnDataBin* ColDatBin = (ColumnDataBin*)ColDatUdt;
							unsigned char* Value = (unsigned char*)ColDatBin->GetValue();
							unsigned char* ValueStore = (unsigned char*)(Addr + m_ColumnOffset[TableId][CurColIdx]);
							StkPlMemCpy(ValueStore, Value, m_ColumnSize[TableId][CurColIdx]);
							UdtFlag = true;
						}
					}
					if (UdtFlag == true) {
						IncrementTableVersion(TableId);
					}
					Addr += m_RecordSize[TableId];
				} else {
					Addr += m_RecordSize[TableId];
				}
			} else {
				Addr += m_RecordSize[TableId];
			}
		}

		RecDatReq = RecDatReq->GetNextRecord();
	} while (RecDatReq != NULL);
	*RetCode = 0;
	return FrstRecDatRes;
}

//////////////////////////////////////////////////////////////////////////////////////

// Sort records
// [in] wchar_t* : Target column name for sorting
// [in] wchar_t* : Table name
// [in] bool : true="A to Z", false="Z to A"
// [out] int : Result code (0: Sucess, -1: Error)
int DataAcController::SortRecord(const wchar_t* TableName, const wchar_t* ColumnName, bool Flag)
{
	if (TableName == NULL || StkPlWcsCmp(TableName, L"") == 0 ||
		ColumnName == NULL || StkPlWcsCmp(ColumnName, L"") == 0) {
		return -1;
	}
	int TableId = SearchTable(TableName);
	int ColumnId = GetColumnIndex(TableName, ColumnName);
	if (TableId == -1 || ColumnId == -1) {
		return -1;
	}
	if (m_TableLock[TableId] != -1) {
		return -1;
	}
	m_CompareColOffset = m_ColumnOffset[TableId][ColumnId];
	m_CompareColType   = m_ColumnType[TableId][ColumnId];
	m_CompareColSize   = m_ColumnSize[TableId][ColumnId];
	if (Flag == true) {
		qsort(m_TableAddr[TableId], m_RecordCount[TableId], m_RecordSize[TableId], AzSortCompare);
	} else {
		qsort(m_TableAddr[TableId], m_RecordCount[TableId], m_RecordSize[TableId], ZaSortCompare);
	}
	return 0;
}

// Sort records (A to Z)
// [in] wchar_t* : Target column name for sorting
// [in] wchar_t* : Table name
// [out] int : Result code (0: Sucess, -1: Error)
int DataAcController::AzSortRecord(const wchar_t* TableName, const wchar_t* ColumnName)
{
	return SortRecord(TableName, ColumnName, true);
}

// Sort records (Z to A)
// [in] wchar_t* : Target column name for sorting
// [in] wchar_t* : Table name
// [out] int : Result code (0: Sucess, -1: Error)
int DataAcController::ZaSortRecord(const wchar_t* TableName, const wchar_t* ColumnName)
{
	return SortRecord(TableName, ColumnName, false);
}

// Compare function for quick sort
// [in] const void* : Pointer to sorting key
// [in] const void* : Pointer to compare target
int DataAcController::AzSortCompare(const void *Arg1, const void *Arg2)
{
	if (m_CompareColType == COLUMN_TYPE_INT) {
		int32_t* Addr1 = (int32_t*)((unsigned char*)Arg1 + m_CompareColOffset);
		int32_t* Addr2 = (int32_t*)((unsigned char*)Arg2 + m_CompareColOffset);
		if (*Addr1 < *Addr2) {
			return -1;
		}
		if (*Addr1 > *Addr2) {
			return 1;
		}
		return 0;
	} else if (m_CompareColType == COLUMN_TYPE_FLOAT) {
		float* Addr1 = (float*)((unsigned char*)Arg1 + m_CompareColOffset);
		float* Addr2 = (float*)((unsigned char*)Arg2 + m_CompareColOffset);
		if (*Addr1 < *Addr2) {
			return -1;
		}
		if (*Addr1 > *Addr2) {
			return 1;
		}
		return 0;
	} else if (m_CompareColType == COLUMN_TYPE_STR) {
		char* Addr1 = (char*)((unsigned char*)Arg1 + m_CompareColOffset);
		char* Addr2 = (char*)((unsigned char*)Arg2 + m_CompareColOffset);
		int Ret = StkPlStrCmp(Addr1, Addr2);
		return Ret;
	} else if (m_CompareColType == COLUMN_TYPE_WSTR) {
		wchar_t* Addr1 = (wchar_t*)((unsigned char*)Arg1 + m_CompareColOffset);
		wchar_t* Addr2 = (wchar_t*)((unsigned char*)Arg2 + m_CompareColOffset);
		int Ret = StkPlWcsCmp(Addr1, Addr2);
		return Ret;
	} else if (m_CompareColType == COLUMN_TYPE_BIN) {
		void* Addr1 = (void*)((unsigned char*)Arg1 + m_CompareColOffset);
		void* Addr2 = (void*)((unsigned char*)Arg2 + m_CompareColOffset);
		int Ret = StkPlMemCmp(Addr1, Addr2, m_CompareColSize);
		return Ret;
	}
	return 0;
}

// Compare function for quick sort
// [in] const void* : Pointer to sorting key
// [in] const void* : Pointer to compare target
int DataAcController::ZaSortCompare(const void *Arg1, const void *Arg2)
{
	if (m_CompareColType == COLUMN_TYPE_INT) {
		int32_t* Addr1 = (int32_t*)((unsigned char*)Arg1 + m_CompareColOffset);
		int32_t* Addr2 = (int32_t*)((unsigned char*)Arg2 + m_CompareColOffset);
		if (*Addr1 < *Addr2) {
			return 1;
		}
		if (*Addr1 > *Addr2) {
			return -1;
		}
		return 0;
	} else if (m_CompareColType == COLUMN_TYPE_FLOAT) {
		float* Addr1 = (float*)((unsigned char*)Arg1 + m_CompareColOffset);
		float* Addr2 = (float*)((unsigned char*)Arg2 + m_CompareColOffset);
		if (*Addr1 < *Addr2) {
			return 1;
		}
		if (*Addr1 > *Addr2) {
			return -1;
		}
		return 0;
	} else if (m_CompareColType == COLUMN_TYPE_STR) {
		char* Addr1 = (char*)((unsigned char*)Arg1 + m_CompareColOffset);
		char* Addr2 = (char*)((unsigned char*)Arg2 + m_CompareColOffset);
		int Ret = StkPlStrCmp(Addr2, Addr1);
		return Ret;
	} else if (m_CompareColType == COLUMN_TYPE_WSTR) {
		wchar_t* Addr1 = (wchar_t*)((unsigned char*)Arg1 + m_CompareColOffset);
		wchar_t* Addr2 = (wchar_t*)((unsigned char*)Arg2 + m_CompareColOffset);
		int Ret = StkPlWcsCmp(Addr2, Addr1);
		return Ret;
	} else if (m_CompareColType == COLUMN_TYPE_BIN) {
		void* Addr1 = (void*)((unsigned char*)Arg1 + m_CompareColOffset);
		void* Addr2 = (void*)((unsigned char*)Arg2 + m_CompareColOffset);
		int Ret = StkPlMemCmp(Addr2, Addr1, m_CompareColSize);
		return Ret;
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////

// Get column count
// [in] wchar_t* : Table name
// [out] int : Column count (-1:Not found)
int DataAcController::GetColumnCount(const wchar_t* TableName)
{
	if (TableName == NULL || StkPlWcsCmp(TableName, L"") == 0) {
		return -1;
	}
	int TableId = SearchTable(TableName);
	if (TableId == -1) {
		return -1;
	}
	return m_ColumnCount[TableId];
}

//Get column name
// [in] wchar_t* : Table name
// [in] wchar_t[MAX_COLUMN_NUMBER][COLUMN_NAME_SIZE] : Column names which are acquired.
// [out] int : Found column name count
int DataAcController::GetColumnName(const wchar_t* TableName, wchar_t ColumnNames[MAX_COLUMN_NUMBER][COLUMN_NAME_SIZE])
{
	if (TableName == NULL || StkPlWcsCmp(TableName, L"") == 0) {
		return -1;
	}
	int TableId = SearchTable(TableName);
	if (TableId == -1) {
		return -1;
	}

	int CurPos = 0;
	for (int Loop = 0; Loop < MAX_COLUMN_NUMBER; Loop++) {
		if (StkPlWcsCmp(m_ColumnName[TableId][Loop], L"") == 0) {
			continue;
		}
		StkPlWcsNCpy(ColumnNames[CurPos], COLUMN_NAME_SIZE, m_ColumnName[TableId][Loop], COLUMN_NAME_SIZE - 1);
		ColumnNames[CurPos][COLUMN_NAME_SIZE - 1] = L'\0';
		CurPos++;
	}
	return m_ColumnCount[TableId];
}

// Get column size
// [in] wchar_t* : Table name
// [in] wchar_t* : Column name
// [out] int : Column size (-1:Not found)
int DataAcController::GetColumnSize(const wchar_t* TableName, const wchar_t* ColumnName)
{
	if (TableName == NULL || StkPlWcsCmp(TableName, L"") == 0) {
		return -1;
	}
	int TableId = SearchTable(TableName);
	if (TableId == -1) {
		return -1;
	}
	int Index = GetColumnIndex(TableName, ColumnName);
	if (Index == -1) {
		return -1;
	}
	return m_ColumnSize[TableId][Index];
}

// Get column type
// [in] wchar_t* : Table name
// [in] wchar_t* : Column name
// [out] int : Column type
int DataAcController::GetColumnType(const wchar_t* TableName, const wchar_t* ColumnName)
{
	if (TableName == NULL || StkPlWcsCmp(TableName, L"") == 0) {
		return -1;
	}
	int TableId = SearchTable(TableName);
	if (TableId == -1) {
		return -1;
	}
	int Index = GetColumnIndex(TableName, ColumnName);
	if (Index == -1) {
		return -1;
	}
	return m_ColumnType[TableId][Index];
}

// Get table count
// [out] int : Table count
int DataAcController::GetTableCount()
{
	int TableCnt = 0;
	int Loop;
	for (Loop = 0; Loop < MAX_TABLE_NUMBER; Loop++) {
		if (m_TableAddr[Loop] != NULL &&
			m_TableSize[Loop] != 0 &&
			StkPlWcsCmp(m_TableName[Loop], L"") != 0) {
			TableCnt++;
		}
	}
	return TableCnt;
}

//Get table name
// [in] wchar_t[MAX_TABLE_NUMBER][TABLE_NAME_SIZE] : Table names which are acquired.
// [out] int : Found table name count
int DataAcController::GetTableName(wchar_t TableNames[MAX_TABLE_NUMBER][TABLE_NAME_SIZE])
{
	int CurPos = 0;
	for (int Loop = 0; Loop < MAX_TABLE_NUMBER; Loop++) {
		if (StkPlWcsCmp(m_TableName[Loop], L"") == 0) {
			continue;
		}
		StkPlWcsNCpy(TableNames[CurPos], TABLE_NAME_SIZE, m_TableName[Loop], TABLE_NAME_SIZE - 1);
		TableNames[CurPos][TABLE_NAME_SIZE - 1] = L'\0';
		CurPos++;
	}
	return CurPos;
}

//Get table size
// [in] wchar_t* : Table name for acquiring the size
// [out] int : Size of the table
int DataAcController::GetTableSize(const wchar_t* TableName)
{
	if (TableName == NULL || StkPlWcsCmp(TableName, L"") == 0) {
		return -1;
	}
	int TableId = SearchTable(TableName);
	if (TableId == -1) {
		return -1;
	}
	return (int)m_TableSize[TableId];
}

// Get table version
// [in] : wchar_t* : TableName
// [out] : int : The version of the specified table
int DataAcController::GetTableVersion(const wchar_t* TableName)
{
	if (TableName == NULL || StkPlWcsCmp(TableName, L"") == 0) {
		return -1;
	}
	int TableId = SearchTable(TableName);
	if (TableId == -1) {
		return -1;
	}
	return m_TableVer[TableId];
}

//Get max number of records
// [in] wchar_t* : Table name
// [out] int : Max number of records
int DataAcController::GetMaxNumOfRecords(const wchar_t* TableName)
{
	int Idx = SearchTable(TableName);
	if (Idx == -1) {
		return -1;
	}
	return (int)(m_TableSize[Idx] / m_RecordSize[Idx]);
}

//Get number of records
// [in] wchar_t* : Table name
// [out] int : number of records
int DataAcController::GetNumOfRecords(const wchar_t* TableName)
{
	int Idx = SearchTable(TableName);
	if (Idx == -1) {
		return -1;
	}
	return m_RecordCount[Idx];
}

//////////////////////////////////////////////////////////////////////////////////////

// Save data
// [out] int : Result code (0: Sucess, -1: Error)
int DataAcController::SaveData(const wchar_t* FilePath)
{
	int LoopTbl;
	int LoopClm;

	// Check whether the table has already been locked.
	for (LoopTbl = 0; LoopTbl < MAX_TABLE_NUMBER; LoopTbl++) {
		if (StkPlWcsCmp(m_TableName[LoopTbl], L"") == 0) {
			continue;
		}
		if (m_TableLock[LoopTbl] == 0) {
			return -1;
		}
	}

	// Check the file path name.
	if (FilePath == NULL) {
		return -1;
	}
	if (StkPlWcsCmp(FilePath, L"") == 0) {
		return -1;
	}

	void* FileHndl = StkPlOpenFileForWrite(FilePath);
	if (FileHndl == NULL) {
		return -1;
	};

	size_t NumOfByteWrite;
	// Write key code
	wchar_t KeyCodeA[16];
	StkPlWcsCpy(KeyCodeA, 16, L"StkData_0101");
	char16_t KeyCode[16] = u"";
	StkPlConvWideCharToUtf16(KeyCode, 16, KeyCodeA);
	if (StkPlWrite(FileHndl, (char*)KeyCode, sizeof(KeyCode), &NumOfByteWrite) == 0) {
		StkPlCloseFile(FileHndl);
		return -1;
	}

	// Write table count
	int32_t TableCount = GetTableCount();
	if (StkPlWrite(FileHndl, (char*)&TableCount, sizeof(TableCount), &NumOfByteWrite) == 0) {
		StkPlCloseFile(FileHndl);
		return -1;
	}
	for (LoopTbl = 0; LoopTbl < MAX_TABLE_NUMBER; LoopTbl++) {
		if (StkPlWcsCmp(m_TableName[LoopTbl], L"") == 0) {
			continue;
		}

		// Write table name
		wchar_t TableNameA[TABLE_NAME_SIZE];
		StkPlWcsNCpy(TableNameA, TABLE_NAME_SIZE, m_TableName[LoopTbl], TABLE_NAME_SIZE - 1);
		TableNameA[TABLE_NAME_SIZE - 1] = L'\0';
		char16_t TableName[TABLE_NAME_SIZE] = u"";
		StkPlConvWideCharToUtf16(TableName, TABLE_NAME_SIZE, TableNameA);
		if (StkPlWrite(FileHndl, (char*)TableName, sizeof(TableName), &NumOfByteWrite) == 0) {
			StkPlCloseFile(FileHndl);
			return -1;
		}

		// Write max record size
		int32_t MaxRecordCount = (int32_t)(m_TableSize[LoopTbl] / m_RecordSize[LoopTbl]);
		if (StkPlWrite(FileHndl, (char*)&MaxRecordCount, sizeof(MaxRecordCount), &NumOfByteWrite) == 0) {
			StkPlCloseFile(FileHndl);
			return -1;
		}

		// Write column count
		int32_t ColumnCount = (int32_t)m_ColumnCount[LoopTbl];
		if (StkPlWrite(FileHndl, (char*)&ColumnCount, sizeof(ColumnCount), &NumOfByteWrite) == 0) {
			StkPlCloseFile(FileHndl);
			return -1;
		}

		for (LoopClm = 0; LoopClm < ColumnCount; LoopClm++) {
			// Write column name
			wchar_t ColumnNameA[COLUMN_NAME_SIZE];
			StkPlWcsNCpy(ColumnNameA, COLUMN_NAME_SIZE, m_ColumnName[LoopTbl][LoopClm], COLUMN_NAME_SIZE - 1);
			ColumnNameA[COLUMN_NAME_SIZE - 1] = L'\0';
			char16_t ColumnName[COLUMN_NAME_SIZE] = u"";
			StkPlConvWideCharToUtf16(ColumnName, COLUMN_NAME_SIZE, ColumnNameA);
			if (StkPlWrite(FileHndl, (char*)ColumnName, sizeof(ColumnName), &NumOfByteWrite) == 0) {
				StkPlCloseFile(FileHndl);
				return -1;
			}

			// Write column type
			int32_t ColumnType = (int32_t)m_ColumnType[LoopTbl][LoopClm];
			if (StkPlWrite(FileHndl, (char*)&ColumnType, sizeof(ColumnType), &NumOfByteWrite) == 0) {
				StkPlCloseFile(FileHndl);
				return -1;
			}
			if (ColumnType == COLUMN_TYPE_STR || ColumnType == COLUMN_TYPE_WSTR || ColumnType == COLUMN_TYPE_BIN) {
				// Write column size
				int32_t ColumnSize = (int32_t)m_ColumnSize[LoopTbl][LoopClm];
				if (ColumnType == COLUMN_TYPE_WSTR) {
					ColumnSize /= (sizeof(wchar_t) / sizeof(char16_t)); // If linux, ColumnSize needs to be divided by 2.
				}
				if (StkPlWrite(FileHndl, (char*)&ColumnSize, sizeof(ColumnSize), &NumOfByteWrite) == 0) {
					StkPlCloseFile(FileHndl);
					return -1;
				}
			}
		}

		// Write table information
		int32_t Header[3];
		Header[0] = (int32_t)0; // This parameter is not used.
		Header[1] = (int32_t)0; // This parameter is not used.
		Header[2] = (int32_t)m_RecordCount[LoopTbl];
		if (StkPlWrite(FileHndl, (char*)Header, sizeof(Header), &NumOfByteWrite) == 0) {
			StkPlCloseFile(FileHndl);
			return -1;
		}

		// Write data
		if (sizeof(wchar_t) == sizeof(char16_t)) {
			// For windows
			if (StkPlWrite(FileHndl, (char*)m_TableAddr[LoopTbl], m_RecordSize[LoopTbl] * m_RecordCount[LoopTbl], &NumOfByteWrite) == 0) {
				StkPlCloseFile(FileHndl);
				return -1;
			}
		} else {
			// For linux
			char* DatPtr = m_TableAddr[LoopTbl];
			for (int LoopRd = 0; LoopRd < m_RecordCount[LoopTbl]; LoopRd++) {
				for (LoopClm = 0; LoopClm < m_ColumnCount[LoopTbl]; LoopClm++) {
					int ColumnType = m_ColumnType[LoopTbl][LoopClm];
					NumOfByteWrite = 0;
					if (ColumnType == COLUMN_TYPE_INT) {
						StkPlWrite(FileHndl, (char*)DatPtr, sizeof(int32_t), &NumOfByteWrite);
						DatPtr += sizeof(int32_t);
					} else if (ColumnType == COLUMN_TYPE_FLOAT) {
						StkPlWrite(FileHndl, (char*)DatPtr, sizeof(float), &NumOfByteWrite);
						DatPtr += sizeof(float);
					} else if (ColumnType == COLUMN_TYPE_STR || ColumnType == COLUMN_TYPE_BIN) {
						int ColumnSize = m_ColumnSize[LoopTbl][LoopClm];
						StkPlWrite(FileHndl, (char*)DatPtr, ColumnSize, &NumOfByteWrite);
						DatPtr += m_ColumnSize[LoopTbl][LoopClm];
					} else if (ColumnType == COLUMN_TYPE_WSTR) {
						int ColumnSizeInWord = m_ColumnSize[LoopTbl][LoopClm] / sizeof(wchar_t);
						wchar_t* TmpWStr = new wchar_t[ColumnSizeInWord];
						char16_t* TmpUtf16 = new char16_t[ColumnSizeInWord];
						StkPlMemCpy(TmpWStr, DatPtr, ColumnSizeInWord * sizeof(wchar_t));
						TmpWStr[ColumnSizeInWord - 1] = L'\0';
						StkPlConvWideCharToUtf16(TmpUtf16, ColumnSizeInWord, TmpWStr);
						StkPlWrite(FileHndl, (char*)TmpUtf16, ColumnSizeInWord * sizeof(char16_t), &NumOfByteWrite);
						delete TmpWStr;
						delete TmpUtf16;
						DatPtr += m_ColumnSize[LoopTbl][LoopClm];
					}
				}
			}
		}
	}
	StkPlCloseFile(FileHndl);

	return 0;
}

// Load data
// [out] int : Result code (0: Sucess, -1: Error)
int DataAcController::LoadData(const wchar_t* FilePath)
{
	int LoopTbl;
	int LoopClm;

	// Check whether the table has already been locked.
	for (LoopTbl = 0; LoopTbl < MAX_TABLE_NUMBER; LoopTbl++) {
		if (StkPlWcsCmp(m_TableName[LoopTbl], L"") == 0) {
			continue;
		}
		if (m_TableLock[LoopTbl] != -1) {
			return -1;
		}
	}

	// Check the file path name.
	if (FilePath == NULL) {
		return -1;
	}
	if (StkPlWcsCmp(FilePath, L"") == 0) {
		return -1;
	}

	//Delete all tables
	wchar_t DelTblName[MAX_TABLE_NUMBER][TABLE_NAME_SIZE];
	int DelTblCnt = GetTableName(DelTblName);
	for (LoopTbl = 0; LoopTbl < DelTblCnt; LoopTbl++) {
		DeleteTable(DelTblName[LoopTbl]);
	}

	void* FileHndl = StkPlOpenFileForRead(FilePath);
	if (FileHndl == NULL) {
		return -1;
	}

	size_t NumOfByteRead;
	// Read key code
	wchar_t KeyCode[16];
	char16_t KeyCodeA[16];
	if (StkPlRead(FileHndl, (char*)KeyCodeA, sizeof(KeyCodeA), &NumOfByteRead) == 0) {
		StkPlCloseFile(FileHndl);
		return -1;
	}
	StkPlConvUtf16ToWideChar(KeyCode, 16, KeyCodeA);

	if (StkPlWcsCmp(KeyCode, L"StkData_0100") != 0 && StkPlWcsCmp(KeyCode, L"StkData_0101") != 0) {
		StkPlCloseFile(FileHndl);
		return -1;
	}
	// Read table count
	int32_t TableCount;
	if (StkPlRead(FileHndl, (char*)&TableCount, sizeof(TableCount), &NumOfByteRead) == 0) {
		StkPlCloseFile(FileHndl);
		return -1;
	}

	for (LoopTbl = 0; LoopTbl < TableCount; LoopTbl++) {
		// Read table name
		wchar_t TableName[TABLE_NAME_SIZE];
		char16_t TableNameA[TABLE_NAME_SIZE];
		if (StkPlRead(FileHndl, (char*)TableNameA, sizeof(TableNameA), &NumOfByteRead) == 0) {
			StkPlCloseFile(FileHndl);
			return -1;
		}
		StkPlConvUtf16ToWideChar(TableName, TABLE_NAME_SIZE, TableNameA);

		// Read max record size
		int32_t MaxRecordCount;
		if (StkPlRead(FileHndl, (char*)&MaxRecordCount, sizeof(MaxRecordCount), &NumOfByteRead) == 0) {
			StkPlCloseFile(FileHndl);
			return -1;
		}

		// Read column count
		int32_t ColumnCount;
		if (StkPlRead(FileHndl, (char*)&ColumnCount, sizeof(ColumnCount), &NumOfByteRead) == 0) {
			StkPlCloseFile(FileHndl);
			return -1;
		}

		// Table definition
		TableDef NewTable(TableName, MaxRecordCount);
		ColumnDef* NewColumn[MAX_COLUMN_NUMBER];
		for (LoopClm = 0; LoopClm < ColumnCount; LoopClm++) {
			// Read column name
			wchar_t ColumnName[COLUMN_NAME_SIZE];
			char16_t ColumnNameA[COLUMN_NAME_SIZE];
			if (StkPlRead(FileHndl, (char*)ColumnNameA, sizeof(ColumnNameA), &NumOfByteRead) == 0) {
				StkPlCloseFile(FileHndl);
				return -1;
			}
			StkPlConvUtf16ToWideChar(ColumnName, COLUMN_NAME_SIZE, ColumnNameA);

			// Read column type
			int32_t ColumnType;
			if (StkPlRead(FileHndl, (char*)&ColumnType, sizeof(ColumnType), &NumOfByteRead) == 0) {
				StkPlCloseFile(FileHndl);
				return -1;
			}
			if (ColumnType == COLUMN_TYPE_STR || ColumnType == COLUMN_TYPE_WSTR || ColumnType == COLUMN_TYPE_BIN) {
				// Read column size
				int32_t ColumnSize;
				if (StkPlRead(FileHndl, (char*)&ColumnSize, sizeof(ColumnSize), &NumOfByteRead) == 0) {
					StkPlCloseFile(FileHndl);
					return -1;
				}
				if (ColumnType == COLUMN_TYPE_WSTR) {
					ColumnSize *= (sizeof(wchar_t) / sizeof(char16_t)); // If linux, ColumnSize needs to be multiplied by 2.
				}

				// Column definition
				if (ColumnType == COLUMN_TYPE_STR) {
					ColumnDefStr* NewColmnStr = new ColumnDefStr(ColumnName, ColumnSize);
					NewTable.AddColumnDef(NewColmnStr);
					NewColumn[LoopClm] = (ColumnDef*)NewColmnStr;
				} else if (ColumnType == COLUMN_TYPE_WSTR) {
					ColumnDefWStr* NewColmnWStr = new ColumnDefWStr(ColumnName, ColumnSize / sizeof(wchar_t));
					NewTable.AddColumnDef(NewColmnWStr);
					NewColumn[LoopClm] = (ColumnDef*)NewColmnWStr;
				} else if (ColumnType == COLUMN_TYPE_BIN) {
					ColumnDefBin* NewColmnBin = new ColumnDefBin(ColumnName, ColumnSize);
					NewTable.AddColumnDef(NewColmnBin);
					NewColumn[LoopClm] = (ColumnDef*)NewColmnBin;
				}
			} else {
				// Column definition
				if (ColumnType == COLUMN_TYPE_INT) {
					ColumnDefInt* NewColmnInt = new ColumnDefInt(ColumnName);
					NewTable.AddColumnDef(NewColmnInt);
					NewColumn[LoopClm] = (ColumnDef*)NewColmnInt;
				} else if (ColumnType == COLUMN_TYPE_FLOAT) {
					ColumnDefFloat* NewColmnFloat = new ColumnDefFloat(ColumnName);
					NewTable.AddColumnDef(NewColmnFloat);
					NewColumn[LoopClm] = (ColumnDef*)NewColmnFloat;
				}
			}
		}
		CreateTable(&NewTable);
		for (LoopClm = 0; LoopClm < ColumnCount; LoopClm++) {
			delete NewColumn[LoopClm];
		}

		// Read table information
		int32_t Header[3];
		if (StkPlRead(FileHndl, (char*)Header, sizeof(Header), &NumOfByteRead) == 0) {
			StkPlCloseFile(FileHndl);
			return -1;
		}
		int TableId = SearchTable(TableName);
		// Header[0] and Header[1] are not used.
		m_RecordCount[TableId] = Header[2];

		// Read data
		if (sizeof(wchar_t) == sizeof(char16_t)) {
			// For windows
			if (StkPlRead(FileHndl, (char*)m_TableAddr[TableId], m_RecordSize[LoopTbl] * m_RecordCount[LoopTbl], &NumOfByteRead) == 0) {
				StkPlCloseFile(FileHndl);
				return -1;
			}
		} else {
			// For linux
			char* DatPtr = m_TableAddr[TableId];
			for (int LoopRd = 0; LoopRd < m_RecordCount[TableId]; LoopRd++) {
				for (LoopClm = 0; LoopClm < ColumnCount; LoopClm++) {
					int ColumnType = m_ColumnType[TableId][LoopClm];
					NumOfByteRead = 0;
					if (ColumnType == COLUMN_TYPE_INT) {
						StkPlRead(FileHndl, (char*)DatPtr, sizeof(int32_t), &NumOfByteRead);
						DatPtr += sizeof(int32_t);
					} else if (ColumnType == COLUMN_TYPE_FLOAT) {
						StkPlRead(FileHndl, (char*)DatPtr, sizeof(float), &NumOfByteRead);
						DatPtr += sizeof(float);
					} else if (ColumnType == COLUMN_TYPE_STR || ColumnType == COLUMN_TYPE_BIN) {
						int ColumnSize = m_ColumnSize[TableId][LoopClm];
						StkPlRead(FileHndl, (char*)DatPtr, ColumnSize, &NumOfByteRead);
						DatPtr += m_ColumnSize[TableId][LoopClm];
					} else if (ColumnType == COLUMN_TYPE_WSTR) {
						int ColumnSizeInWord = m_ColumnSize[TableId][LoopClm] / sizeof(wchar_t);
						char16_t* TmpUtf16 = new char16_t[ColumnSizeInWord];
						StkPlRead(FileHndl, (char*)TmpUtf16, ColumnSizeInWord * sizeof(char16_t), &NumOfByteRead);
						TmpUtf16[ColumnSizeInWord - 1] = u'\0';
						StkPlConvUtf16ToWideChar((wchar_t*)DatPtr, ColumnSizeInWord, TmpUtf16);
						delete TmpUtf16;
						DatPtr += m_ColumnSize[TableId][LoopClm];
					}
				}
			}
		}
	}
	StkPlCloseFile(FileHndl);

	return 0;
}

void DataAcController::AutoSaveThreadProcDummy(void* ObjAddr)
{
	reinterpret_cast<DataAcController*>(ObjAddr)->AutoSaveThreadProc();
}

void DataAcController::AutoSaveThreadProc()
{
	int PrevTblCnt = 0;
	int PrevTblVer[MAX_TABLE_NUMBER];
	bool ChgFlag = false;
	while (true) {
		StkPlSleepMs(AutoSaveInterval);
		// If auto save has been cancelled, break the while process.
		if (AutoSaveInterval == -1) {
			break;
		}

		// If table has been changed from the previous, SaveData() is called.
		wchar_t TblName[MAX_TABLE_NUMBER][TABLE_NAME_SIZE];
		int TblCnt = GetTableName(TblName);
		if (PrevTblCnt == TblCnt) {
			int TblVer[MAX_TABLE_NUMBER];
			for (int Loop = 0; Loop < TblCnt; Loop++) {
				TblVer[Loop] = GetTableVersion(TblName[Loop]);
				if (PrevTblVer[Loop] != TblVer[Loop]) {
					ChgFlag = true;
					PrevTblVer[Loop] = TblVer[Loop];
				}
			}
		} else {
			ChgFlag = true;
			PrevTblCnt = TblCnt;
			for (int Loop = 0; Loop < TblCnt; Loop++) {
				PrevTblVer[Loop] = GetTableVersion(TblName[Loop]);
			}
		}
		if (ChgFlag == true) {
			ChgFlag = false;
			LockAllTable(LOCK_SHARE);
			SaveData((wchar_t*)AutoSaveFilePath);
			UnlockAllTable();
		}
	}
}

int DataAcController::AutoSave(const wchar_t* FilePath, int Sec, bool Flag)
{
	static bool Switch = false;

	Cs4AutoSave.lock();
	if (Flag == Switch) {
		Cs4AutoSave.unlock();
		return -1;
	}
	if (Sec < 30 || Sec > 86400) {
		Cs4AutoSave.unlock();
		return -1;
	}

	if (Flag == true) {
		Switch = true;
		AutoSaveInterval = Sec * 1000;
		StkPlWcsNCpy(AutoSaveFilePath, 256, FilePath, 256 - 1);
		AutoSaveFilePath[256 - 1] = L'\0';
		std::thread MyThread(AutoSaveThreadProcDummy, this);
		MyThread.detach();
	} else {
		Switch = false;
		AutoSaveInterval = -1;
	}
	Cs4AutoSave.unlock();
	return 0;
}
