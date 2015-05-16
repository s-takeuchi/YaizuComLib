#include <windows.h>
#include <commctrl.h>
#include <tchar.h>
#include <string.h>
#include <stdio.h>
#include <shlwapi.h>
#include "..\stkdata\stkdata.h"
#include "resource.h"

///// Memory leak proc begin
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//#define new  ::new(_NORMAL_BLOCK, __FILE__, __LINE__)
///// Memory leak proc end

HINSTANCE InstHndl;
HWND ListWndHndl;

// Icon handle
HICON IconHndlrS;
HICON IconHndlrB;

// Selected table information
int SelectedTable = 0;
TCHAR SelectedTableName[16];

// Window title
TCHAR WindowTitle[64];

// Selected filtering conditions
int SelectedComboItemIdx[5];
TCHAR SelectedComboItemClm[5][16];
TCHAR SelectedComboItemWStr[5][256];

// Selected update record
RecordData* UpdtRecDat = NULL;

// Flag for displaying file access menu item (Save As, Open)
BOOL FileAccessFlag = FALSE;

TCHAR TooltipDataNew[] = _T("New");
TCHAR TooltipDataOpen[] = _T("Open");
TCHAR TooltipDataSave[] = _T("Save");
TCHAR TooltipOperationInsert[] = _T("Insert");
TCHAR TooltipOperationDelete[] = _T("Delete");
TCHAR TooltipOperationUpdate[] = _T("Update");

// Functions
void RefreshTableMenu(HWND, TCHAR*);
void CreateTable();
void DeleteTable();
RecordData* GetCheckedRecord(HWND);
void DeleteRecord(HWND);
void ChangeData();
void ViewData(BOOL);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK InsertDlg(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK FilterDlg(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK UpdateDlg(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK CreateTableDlg(HWND, UINT, WPARAM, LPARAM);
LRESULT SpecifyColumn(HWND, UINT, WPARAM, LPARAM, BOOL);
void ChangeWindowTitle(HWND);
void ShowStkDataWindow(TCHAR*, HWND);
int GetOpenFileName(HWND, TCHAR*);
int GetSaveFileName(HWND, TCHAR*);
void EnableFileAccessFlag();
void DisableFileAccessFlag();
int SwitchSortRecord(TCHAR*, TCHAR*);

// DLL entry point
BOOL WINAPI DllMain(HANDLE hinstDLL, DWORD dwReason, LPVOID lpvReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH) {
		InstHndl = (HINSTANCE)hinstDLL;
	}
	if (dwReason == DLL_PROCESS_DETACH) {
	}
	return TRUE;
}

// Display open file dialog and acquire the file name which is specified on the dialog.
// [in] TCHAR* : Acquired file name
// [out] int : Result code (0: Sucess, -1: Error)
int GetOpenFileName(HWND WinHndl, TCHAR* FileName)
{
	TCHAR File[32768] = _T("");
	TCHAR Title[256] = _T("");
	OPENFILENAME Ofn;
	ZeroMemory(&Ofn, sizeof(OPENFILENAME));
	Ofn.lStructSize = sizeof(OPENFILENAME);
	Ofn.hwndOwner = WinHndl;
	Ofn.lpstrFile = File;
	Ofn.nMaxFile = 300;
	Ofn.lpstrFileTitle =Title;
	Ofn.nMaxFileTitle = 300;
	Ofn.lpstrInitialDir = _T(".");
	Ofn.lpstrFilter = _T("Data file(*.std)\0*.std\0Data file(*.dat)\0*.dat\0\0");
	Ofn.lpstrTitle = _T("Select data file [Open]");
	Ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	int Ret = ::GetOpenFileName(&Ofn);
	lstrcpy(FileName, Ofn.lpstrFile);
	return Ret;
}

// Display save file dialog and acquire the file name which is specified on the dialog.
// [in] TCHAR* : Acquired file name
// [out] int : Result code (0: Sucess, -1: Error)
int GetSaveFileName(HWND WinHndl, TCHAR* FileName)
{
	TCHAR File[32768] = _T("");
	TCHAR Title[256] = _T("");
	OPENFILENAME Ofn;
	ZeroMemory(&Ofn, sizeof(OPENFILENAME));
	Ofn.lStructSize = sizeof(OPENFILENAME);
	Ofn.hwndOwner = WinHndl;
	Ofn.lpstrFile = File;
	Ofn.nMaxFile = 300;
	Ofn.lpstrFileTitle =Title;
	Ofn.nMaxFileTitle = 300;
	Ofn.lpstrDefExt = _T("std");
	Ofn.lpstrInitialDir = _T(".");
	Ofn.lpstrFilter = _T("Data file(*.std)\0*.std\0Data file(*.dat)\0*.dat\0\0");
	Ofn.lpstrTitle = _T("Select data file [Save As]");
	int Ret = ::GetSaveFileName(&Ofn);
	lstrcpy(FileName, Ofn.lpstrFile);
	return Ret;
}

void ShowStkDataWindow(TCHAR* AppName, HWND pWnd)
{
	HWND hWnd;

	// Registration of instance
	{
		WNDCLASSEX wcex;
		wcex.cbSize        = sizeof(WNDCLASSEX); 
		wcex.style         = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc   = (WNDPROC)WndProc;
		wcex.cbClsExtra    = 0;
		wcex.cbWndExtra    = 0;
		wcex.hInstance     = InstHndl;
		wcex.hIcon         = LoadIcon(InstHndl, MAKEINTRESOURCE(IDI_STKDATA));
		wcex.hIconSm       = LoadIcon(InstHndl, MAKEINTRESOURCE(IDI_STKDATA));
		wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName  = (LPCTSTR)IDR_MENU;
		wcex.lpszClassName = AppName;
		RegisterClassEx(&wcex);
	}

	// Create window and associate icon to the window
	hWnd = CreateWindow(AppName, AppName,
		WS_OVERLAPPED | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_SIZEBOX,
		CW_USEDEFAULT, 0, 600, 350, pWnd, NULL, InstHndl, NULL);
	if(!hWnd) {
		return;
	}

	// Initialize Icon handles which are used from child dialog box and this window.
	IconHndlrS = (HICON)LoadImage(InstHndl, (LPCTSTR)IDI_STKDATA, IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	IconHndlrB = (HICON)LoadImage(InstHndl, (LPCTSTR)IDI_STKDATA, IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
	if (IconHndlrS != NULL && IconHndlrB != NULL) {
		SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)IconHndlrS);
		SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)IconHndlrB);
	}

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	// Message process
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void EnableFileAccessFlag()
{
	FileAccessFlag = TRUE;
}

void DisableFileAccessFlag()
{
	FileAccessFlag = FALSE;
}

void DeleteRecord(HWND WndHndl)
{
	RecordData* RecDat = GetCheckedRecord(WndHndl);
	if (RecDat != NULL) {
		if (MessageBox(WndHndl, _T("Are you sure you want to delete the selected record(s) ?"), _T("Notice!"), MB_OKCANCEL) == IDCANCEL) {
			return;
		}
	} else {
		MessageBox(WndHndl, _T("No record has been selected."), _T("Notice!"), MB_OK);
		return;
	}
	LockTable(SelectedTableName, LOCK_EXCLUSIVE);
	DeleteRecord(RecDat);
	UnlockTable(SelectedTableName);
	ClearRecordData(RecDat);
	ViewData(FALSE);
}

RecordData* GetCheckedRecord(HWND WndHndl)
{
	int RecCnt = ListView_GetItemCount(ListWndHndl);
	int LoopRec;
	int LoopCol;

	RecordData* FrstRecDat = NULL;
	RecordData* PrevRecDat = NULL;
	TCHAR ColName[32][16];
	int ColCnt = GetColumnName(SelectedTableName, ColName);
	for (LoopRec = 0; LoopRec < RecCnt; LoopRec++) {
		BOOL ChkSt = ListView_GetCheckState(ListWndHndl, LoopRec);
		if (ChkSt == FALSE) {
			continue;
		}
		ColumnData* ColDat[32];
		int FndCnt = 0;
		for (LoopCol = 0; LoopCol < ColCnt; LoopCol++) {
			TCHAR Buf[256];
			ListView_GetItemText(ListWndHndl, LoopRec, LoopCol + 1, Buf, 256);
			if (GetColumnType(SelectedTableName, ColName[LoopCol]) == COLUMN_TYPE_INT) {
				int Dat = StrToInt(Buf);
				ColDat[FndCnt] = new ColumnDataInt(ColName[LoopCol], Dat);
			} else if (GetColumnType(SelectedTableName, ColName[LoopCol]) == COLUMN_TYPE_FLOAT) {
				float Dat;
				swscanf_s(Buf, _T("%g"), &Dat);
				ColDat[FndCnt] = new ColumnDataFloat(ColName[LoopCol], Dat);
			} else if (GetColumnType(SelectedTableName, ColName[LoopCol]) == COLUMN_TYPE_STR) {
				char LocBuf[256];
				sprintf_s(LocBuf, 256, "%S", Buf);
				ColDat[FndCnt] = new ColumnDataStr(ColName[LoopCol], LocBuf);
			} else if (GetColumnType(SelectedTableName, ColName[LoopCol]) == COLUMN_TYPE_WSTR) {
				ColDat[FndCnt] = new ColumnDataWStr(ColName[LoopCol], Buf);
			} else {
				// 選択したレコード内のバイナリ型カラムの値は無視される
				continue;
			}
			FndCnt++;
		}
		RecordData* RecDat = new RecordData(SelectedTableName, ColDat, FndCnt);
		if (PrevRecDat != NULL) {
			PrevRecDat->SetNextRecord(RecDat);
		} else {
			FrstRecDat = RecDat;
		}
		PrevRecDat = RecDat;
	}
	return FrstRecDat;
}

LRESULT CALLBACK InsertDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int WmId    = LOWORD(wParam); 
	int WmEvent = HIWORD(wParam);
	static TCHAR StrBuf[32][256];

	// Initialize dialog
	if (message == WM_INITDIALOG) {
		SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)IconHndlrS);
		SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)IconHndlrB);

		int Loop;

		TCHAR ColName[32][16];
		int ColCnt =  GetColumnName(SelectedTableName, ColName);
		SetDlgItemText(hDlg, IDC_ED_TABLE, SelectedTableName);
		for (Loop = 0; Loop < ColCnt; Loop++) {
			SetDlgItemText(hDlg, IDC_ED_ATTR1 + Loop, ColName[Loop]);
			if (GetColumnType(SelectedTableName, ColName[Loop]) == COLUMN_TYPE_BIN) {
				// バイナリ型カラムの場合，値入力不可とする。
				SendDlgItemMessage(hDlg, IDC_EDIT1 + Loop, EM_SETREADONLY, TRUE, 0);
				SetDlgItemText(hDlg, IDC_EDIT1 + Loop, _T("<<binary data>>"));
				lstrcpy(StrBuf[Loop], _T("bin"));
			} else {
				lstrcpy(StrBuf[Loop], _T(""));
			}
		}
		for (; Loop < 32; Loop++) {
			SendDlgItemMessage(hDlg, IDC_EDIT1 + Loop, EM_SETREADONLY, TRUE, 0);
			lstrcpy(StrBuf[Loop], _T(""));
		}
		return TRUE;
	}

	// Edit box was updated.
	if (message == WM_COMMAND && WmEvent == EN_UPDATE) {
		if (WmId >= IDC_EDIT1 && WmId <= IDC_EDIT32) {
			GetDlgItemText(hDlg, WmId, StrBuf[WmId - IDC_EDIT1], 256);
		}
		return TRUE;
	}

	// Insert button was clicked.
	if (message == WM_COMMAND && WmId == IDOK) {
		int Loop;
		TCHAR ColumnName[16];
		ColumnData* ColDat[32];
		int ColCnt = 0;
		for (Loop = 0; Loop < 32; Loop++) {
			if (StrBuf[Loop] == NULL || lstrcmp(StrBuf[Loop], _T("")) == 0) {
				continue;
			}
			GetDlgItemText(hDlg, Loop + IDC_ED_ATTR1, ColumnName, 16);
			int Type = GetColumnType(SelectedTableName, ColumnName);
			if (Type == COLUMN_TYPE_INT) {
				int LocInt = StrToInt(StrBuf[Loop]);
				ColDat[ColCnt] = new ColumnDataInt(ColumnName, LocInt);
			} else if (Type == COLUMN_TYPE_FLOAT) {
				float LocFloat;
				swscanf_s(StrBuf[Loop], _T("%g"), &LocFloat);
				ColDat[ColCnt] = new ColumnDataFloat(ColumnName, LocFloat);
			} else if (Type == COLUMN_TYPE_STR) {
				char LocBuf[256];
				sprintf_s(LocBuf, 256, "%S", StrBuf[Loop]);
				ColDat[ColCnt] = new ColumnDataStr(ColumnName, LocBuf);
			} else if (Type == COLUMN_TYPE_WSTR) {
				ColDat[ColCnt] = new ColumnDataWStr(ColumnName, StrBuf[Loop]);
			} else if (Type == COLUMN_TYPE_BIN) {
				ColDat[ColCnt] = new ColumnDataBin(ColumnName, NULL, 0);
			}
			ColCnt++;
		}
		RecordData* RecDat = new RecordData(SelectedTableName, ColDat, ColCnt);
		LockTable(SelectedTableName, LOCK_EXCLUSIVE);
		InsertRecord(RecDat);
		UnlockTable(SelectedTableName);
		ClearRecordData(RecDat);
		EndDialog(hDlg, 0);
		ViewData(FALSE);
		return TRUE;
	}

	// Cancel button was clicked.
	if (message == WM_COMMAND && WmId == IDCANCEL) {
		EndDialog(hDlg, 0);
		return TRUE;
	}

	return FALSE;
}

LRESULT CALLBACK CreateTableDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int WmId    = LOWORD(wParam); 
	int WmEvent = HIWORD(wParam);

	// Initialize dialog
	if (message == WM_INITDIALOG) {
		SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)IconHndlrS);
		SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)IconHndlrB);

		for (int LoopCmb = IDC_COMBO1; LoopCmb <= IDC_COMBO32; LoopCmb++) {
			SendDlgItemMessage(hDlg, LoopCmb, CB_INSERTSTRING, 0, (LPARAM)_T("Integer"));
			SendDlgItemMessage(hDlg, LoopCmb, CB_INSERTSTRING, 1, (LPARAM)_T("String"));
			SendDlgItemMessage(hDlg, LoopCmb, CB_INSERTSTRING, 2, (LPARAM)_T("WString"));
			SendDlgItemMessage(hDlg, LoopCmb, CB_INSERTSTRING, 3, (LPARAM)_T("Binary"));
			SendDlgItemMessage(hDlg, LoopCmb, CB_INSERTSTRING, 4, (LPARAM)_T("Float"));
			SendDlgItemMessage(hDlg, LoopCmb, CB_SETCURSEL, 0, 0);
			SendDlgItemMessage(hDlg, IDC_EDIT1 + (LoopCmb - IDC_COMBO1), EM_SETREADONLY, TRUE, 0);
			SetDlgItemText(hDlg, IDC_EDIT1 + (LoopCmb - IDC_COMBO1), _T("32"));
		}

	}

	// Combo box was changed.
	if (message == WM_COMMAND && WmEvent == CBN_SELCHANGE) {
		if (WmId >= IDC_COMBO1 && WmId <= IDC_COMBO32) {
			int ComboItemIdx = (int)SendDlgItemMessage(hDlg, WmId, CB_GETCURSEL, 0, 0);
			if (ComboItemIdx != 0 && ComboItemIdx != 4) {
				SendDlgItemMessage(hDlg, IDC_EDIT1 + (WmId - IDC_COMBO1), EM_SETREADONLY, FALSE, 0);
			} else {
				SendDlgItemMessage(hDlg, IDC_EDIT1 + (WmId - IDC_COMBO1), EM_SETREADONLY, TRUE, 0);
			}
		}
		return TRUE;
	}

	// "Add Table" button was clicked.
	if (message == WM_COMMAND && WmId == IDOK) {

		// Validation-1 : Check table name
		TCHAR TableName[32];
		GetDlgItemText(hDlg, IDC_ED_TABLE, TableName, 32);
		if (lstrcmp(TableName, _T("")) == 0) {
			MessageBox(hDlg, _T("No table name is presented."), _T("Notice!"), MB_OK);
			return TRUE;
		}

		// Validation-2 : Check size
		TCHAR TableSizeBuf[32];
		int TableSize = 0;
		GetDlgItemText(hDlg, IDC_ED_SIZE, TableSizeBuf, 32);
		TableSize = StrToInt(TableSizeBuf);
		if (TableSize <= 0 || TableSize >= MAX_RECORD) {
			MessageBox(hDlg, _T("The max number of records is invalid."), _T("Notice!"), MB_OK);
			return TRUE;
		}

		// Validation-3 : Check presented column name
		BOOL NoAttrFlag = TRUE;
		for (int LoopAtr = IDC_ED_ATTR1; LoopAtr <= IDC_ED_ATTR32; LoopAtr++) {
			TCHAR Buf[16];
			GetDlgItemText(hDlg, LoopAtr, Buf, 16);
			Buf[15] = NULL;
			if (lstrcmp(Buf, _T("")) != 0) {
				NoAttrFlag = FALSE;
			}
		}
		if (NoAttrFlag == TRUE) {
			MessageBox(hDlg, _T("No column information is presented."), _T("Notice!"), MB_OK);
			return TRUE;
		}

		// Validation-4 : Check length if "String" / "WString" / "Binary" is selected.
		for (int LoopCmb = IDC_COMBO1; LoopCmb <= IDC_COMBO32; LoopCmb++) {
			int ComboItemIdx = (int)SendDlgItemMessage(hDlg, LoopCmb, CB_GETCURSEL, 0, 0);
			if (ComboItemIdx != 0) {
				TCHAR Buf[32];
				GetDlgItemText(hDlg, IDC_EDIT1 + (LoopCmb - IDC_COMBO1), Buf, 32);
				int Val = StrToInt(Buf);
				if ((ComboItemIdx == 1 || ComboItemIdx == 2) && (Val < 2 || Val > 256)) {
					TCHAR ErrMsg[256];
					wsprintf(ErrMsg, _T("The presented value is invalid.(Length of #%d)\r\nSpecify a length value in from 2 to 256."), LoopCmb - IDC_COMBO1 + 1);
					MessageBox(hDlg, ErrMsg, _T("Notice!"), MB_OK);
					return TRUE;
				} else if (ComboItemIdx == 3 && (Val < 1 || Val > 10000000)) {
					TCHAR ErrMsg[256];
					wsprintf(ErrMsg, _T("The presented value is invalid.(Length of #%d)\r\nSpecify a length value in from 1 to 10000000."), LoopCmb - IDC_COMBO1 + 1);
					MessageBox(hDlg, ErrMsg, _T("Notice!"), MB_OK);
					return TRUE;
				}
			}
		}

		// Create table
		TableDef* Tbl = new TableDef(TableName, TableSize);
		ColumnDef* Clm;
		for (int Loop = 0; Loop <= 32; Loop++) {
			TCHAR Buf[16];
			GetDlgItemText(hDlg, Loop + IDC_ED_ATTR1, Buf, 16);
			Buf[15] = NULL;
			if (lstrcmp(Buf, _T("")) != 0) {
				int SelCur = (int)SendDlgItemMessage(hDlg, Loop + IDC_COMBO1, CB_GETCURSEL, 0, 0);
				if (SelCur == 0) { // Integerが選択された
					Clm = new ColumnDefInt(Buf);
				} else if (SelCur == 1) { // Stringが選択された
					TCHAR BufStrSize[32];
					GetDlgItemText(hDlg, IDC_EDIT1 + Loop, BufStrSize, 32);
					int Val = StrToInt(BufStrSize);
					Clm = new ColumnDefStr(Buf, Val);
				} else if (SelCur == 2) { // WStringが選択された
					TCHAR BufStrSize[32];
					GetDlgItemText(hDlg, IDC_EDIT1 + Loop, BufStrSize, 32);
					int Val = StrToInt(BufStrSize);
					Clm = new ColumnDefWStr(Buf, Val);
				} else if (SelCur == 3) { // Binaryが選択された
					TCHAR BufBinSize[32];
					GetDlgItemText(hDlg, IDC_EDIT1 + Loop, BufBinSize, 32);
					int Val = StrToInt(BufBinSize);
					Clm = new ColumnDefBin(Buf, Val);
				} else if (SelCur == 4) { // Floatが選択された
					Clm = new ColumnDefFloat(Buf);
				}
				Tbl->AddColumnDef(Clm);
			}
		}
		if (CreateTable(Tbl) != 0) {
			MessageBox(hDlg, _T("Failed to create table."), _T("Notice!"), MB_OK);
			return TRUE;
		}

		// Delete temporary object
		for (int Loop = 0; Loop < Tbl->GetNumOfColumn(); Loop++) {
			delete Tbl->GetColumnDef(Loop);
		}
		delete Tbl;

		EndDialog(hDlg, 0);
		return TRUE;

	}

	// Cancel button was clicked.
	if (message == WM_COMMAND && WmId == IDCANCEL) {
		EndDialog(hDlg, 0);
		return TRUE;
	}

	return FALSE;
}

// Common dialog process which is called by filtering or update dialog.
// [in] HWND : Handle of filtering or update dialog
// [in] UINT : Message
// [in] WPARAM : WParam
// [in] LPARAM : LParam
// [in] BOOL : Update flag. If update dialog call this function, this parameter must be specified as TRUE.
// [out] LRESULT : Result code
LRESULT SpecifyColumn(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam, BOOL UpdtFlag)
{
	int WmId    = LOWORD(wParam); 
	int WmEvent = HIWORD(wParam);

	// Initialize dialog
	if (message == WM_INITDIALOG) {
		SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)IconHndlrS);
		SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)IconHndlrB);

		int LoopCmb;
		int LoopCol;
		static BOOL Init = TRUE;

		if (Init == TRUE) {
			if (UpdtFlag == FALSE) {
				for (LoopCmb = 0; LoopCmb < 5; LoopCmb++) {
					SelectedComboItemIdx[LoopCmb] = 0;
					lstrcpyn(SelectedComboItemClm[LoopCmb], _T(""), 16);
					lstrcpyn(SelectedComboItemWStr[LoopCmb], _T(""), 256);
				}
			}
			Init = FALSE;
		}
		TCHAR ColName[32][16];
		int ColCnt = GetColumnName(SelectedTableName, ColName);
		for (LoopCmb = IDC_F_COMBO1; LoopCmb <= IDC_F_COMBO5; LoopCmb++) {
			SendDlgItemMessage(hDlg, LoopCmb, CB_ADDSTRING, 0, (LPARAM)_T("n/s"));
			for (LoopCol = 0; LoopCol < ColCnt; LoopCol++) {
				SendDlgItemMessage(hDlg, LoopCmb, CB_ADDSTRING, 0, (LPARAM)ColName[LoopCol]);
			}
			if (UpdtFlag == FALSE) {
				SendDlgItemMessage(hDlg, LoopCmb, CB_SETCURSEL, SelectedComboItemIdx[LoopCmb - IDC_F_COMBO1], 0);
				if (SelectedComboItemIdx[LoopCmb - IDC_F_COMBO1] == 0) {
					SendDlgItemMessage(hDlg, IDC_F_EDIT1 + (LoopCmb - IDC_F_COMBO1), EM_SETREADONLY, TRUE, 0);
				} else {
					SetDlgItemText(hDlg, IDC_F_EDIT1 + (LoopCmb - IDC_F_COMBO1), SelectedComboItemWStr[LoopCmb - IDC_F_COMBO1]);
				}
			} else {
				SendDlgItemMessage(hDlg, LoopCmb, CB_SETCURSEL, 0, 0);
				SendDlgItemMessage(hDlg, IDC_F_EDIT1 + (LoopCmb - IDC_F_COMBO1), EM_SETREADONLY, TRUE, 0);
			}
		}
		TCHAR Title[1024];
		HWND BtnHndl = GetDlgItem(hDlg, IDOK);
		HWND ClrHndl = GetDlgItem(hDlg, ID_BUTTON_F_CLEAR);
		if (UpdtFlag == FALSE) {
			wsprintf(Title, _T("Specify the filtering criteria, then click [OK].\r\nTable Name : %s"), SelectedTableName);
			SetWindowText(hDlg, _T("Filtering"));
			SetWindowText(BtnHndl, _T("Filtering"));
			EnableWindow(ClrHndl, TRUE);
		} else {
			wsprintf(Title, _T("Specify the update column, then click [OK].\r\nTable Name : %s"), SelectedTableName);
			SetWindowText(hDlg, _T("Update"));
			SetWindowText(BtnHndl, _T("Update"));
			EnableWindow(ClrHndl, FALSE);
		}
		SetDlgItemText(hDlg, IDC_F_TITLE, Title);
		return TRUE;
	}

	// Combo box was changed.
	if (message == WM_COMMAND && WmEvent == CBN_SELCHANGE) {
		if (WmId >= IDC_F_COMBO1 && WmId <= IDC_F_COMBO5) {
			int ComboItemIdx = (int)SendDlgItemMessage(hDlg, WmId, CB_GETCURSEL, 0, 0);
			TCHAR ColName[16];
			SendDlgItemMessage(hDlg, WmId, CB_GETLBTEXT, ComboItemIdx, (LPARAM)ColName);
			if (ComboItemIdx != 0 && GetColumnType(SelectedTableName, ColName) != COLUMN_TYPE_BIN) {
				SendDlgItemMessage(hDlg, IDC_F_EDIT1 + (WmId - IDC_F_COMBO1), EM_SETREADONLY, FALSE, 0);
			} else {
				SendDlgItemMessage(hDlg, IDC_F_EDIT1 + (WmId - IDC_F_COMBO1), EM_SETREADONLY, TRUE, 0);
			}
			if (GetColumnType(SelectedTableName, ColName) == COLUMN_TYPE_BIN) {
				SetDlgItemText(hDlg, IDC_F_EDIT1 + (WmId - IDC_F_COMBO1), _T("<<binary data>>"));
			} else {
				SetDlgItemText(hDlg, IDC_F_EDIT1 + (WmId - IDC_F_COMBO1), _T(""));
			}
		}
		return TRUE;
	}

	// Clear button was clicked.
	if (message == WM_COMMAND && WmId == ID_BUTTON_F_CLEAR) {
		int LoopCmb;
		for (LoopCmb = 0; LoopCmb < 5; LoopCmb++) {
			SendDlgItemMessage(hDlg, LoopCmb + IDC_F_COMBO1, CB_SETCURSEL, 0, 0);
			SetDlgItemText(hDlg, LoopCmb + IDC_F_EDIT1, _T(""));
			SendDlgItemMessage(hDlg, LoopCmb + IDC_F_EDIT1, EM_SETREADONLY, TRUE, 0);
		}
		return TRUE;
	}

	// OK button was clicked.
	if (message == WM_COMMAND && WmId == IDOK) {
		if (UpdtFlag == FALSE) {
			int Loop;
			for (Loop = 0; Loop < 5; Loop++) {
				TCHAR Buf[256];
				GetDlgItemText(hDlg, IDC_F_EDIT1 + Loop, Buf, 256);
				lstrcpyn(SelectedComboItemWStr[Loop], Buf, 256);
				SelectedComboItemIdx[Loop] = (int)SendDlgItemMessage(hDlg, IDC_F_COMBO1 + Loop, CB_GETCURSEL, 0, 0);
				SendDlgItemMessage(hDlg, IDC_F_COMBO1 + Loop, CB_GETLBTEXT, SelectedComboItemIdx[Loop], (LPARAM)SelectedComboItemClm[Loop]);
			}
		} else {
			int Loop;
			int ColCnt = 0;
			ColumnData* ColDat[5];

			for (Loop = 0; Loop < 5; Loop++) {
				TCHAR BufClm[16];
				TCHAR BufDat[256];
				char BufData[256];
				int IntData;
				float FloatData;

				int ColIdx = (int)SendDlgItemMessage(hDlg, IDC_F_COMBO1 + Loop, CB_GETCURSEL, 0, 0);
				if (ColIdx == 0) {
					continue;
				}
				GetDlgItemText(hDlg, IDC_F_COMBO1 + Loop, BufClm, 16);
				GetDlgItemText(hDlg, IDC_F_EDIT1 + Loop, BufDat, 256);

				int Type = GetColumnType(SelectedTableName, BufClm);
				if (Type == COLUMN_TYPE_INT) {
					IntData = StrToInt(BufDat);
					ColDat[ColCnt] = new ColumnDataInt(BufClm, IntData);
				} else if (Type == COLUMN_TYPE_FLOAT) {
					swscanf_s(BufDat, _T("%g"), &FloatData);
					ColDat[ColCnt] = new ColumnDataFloat(BufClm, FloatData);
				} else if (Type == COLUMN_TYPE_STR) {
					sprintf_s(BufData, 256, "%S", BufDat);
					ColDat[ColCnt] = new ColumnDataStr(BufClm, BufData);
				} else if (Type == COLUMN_TYPE_WSTR) {
					ColDat[ColCnt] = new ColumnDataWStr(BufClm, BufDat);
				} else if (Type == COLUMN_TYPE_BIN) {
					continue;
				}
				ColCnt++;
			}
			if (UpdtRecDat != NULL) {
				ClearRecordData(UpdtRecDat);
			}
			UpdtRecDat = new RecordData(SelectedTableName, ColDat, ColCnt);
		}
		EndDialog(hDlg, 0);
		ViewData(FALSE);
		return TRUE;
	}

	// Cancel button was clicked.
	if (message == WM_COMMAND && WmId == IDCANCEL) {
		EndDialog(hDlg, 0);
		return TRUE;
	}
	return FALSE;
}

LRESULT CALLBACK FilterDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	return SpecifyColumn(hDlg, message, wParam, lParam, FALSE);
}

LRESULT CALLBACK UpdateDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	return SpecifyColumn(hDlg, message, wParam, lParam, TRUE);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static TCHAR FileName[32768] = _T("");
	static HWND hwndToolbar = NULL;
	int WmId, WmEvent;
	static LPTOOLTIPTEXT lptip;

	switch (message) {
	case WM_CREATE:
		InitCommonControls();

		{
			INITCOMMONCONTROLSEX InitCom;

			// Tool Bar Setting
			InitCom.dwSize = sizeof(INITCOMMONCONTROLSEX);
			InitCom.dwICC = ICC_BAR_CLASSES;
			InitCommonControlsEx(&InitCom);

			TBBUTTON tbButton[] = {
				{STD_FILENEW, ID_DATA_NEW, TBSTATE_ENABLED, BTNS_BUTTON | BTNS_AUTOSIZE, 0, 0, 0},
				{STD_FILEOPEN, ID_DATA_OPEN, TBSTATE_ENABLED, BTNS_BUTTON | BTNS_AUTOSIZE, 0, 0, 0},
				{STD_FILESAVE, ID_DATA_SAVE, TBSTATE_ENABLED, BTNS_BUTTON | BTNS_AUTOSIZE, 0, 0, 0},
				{0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0, 0, 0},
				{VIEW_NETCONNECT, ID_OPERATION_INSERT, TBSTATE_ENABLED, BTNS_BUTTON | BTNS_AUTOSIZE, 0, 0, 0},
				{VIEW_NETDISCONNECT, ID_OPERATION_DELETE, TBSTATE_ENABLED, BTNS_BUTTON | BTNS_AUTOSIZE, 0, 0, 0},
				{STD_PROPERTIES, ID_OPERATION_UPDATE, TBSTATE_ENABLED, BTNS_BUTTON | BTNS_AUTOSIZE, 0, 0, 0},
			};
			int nCount = sizeof(tbButton) / sizeof(tbButton[0]);

			TBADDBITMAP tbBitmap;
			hwndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS, 0, 0, 0, 0, hWnd, (HMENU)100, InstHndl, NULL);
			SendMessage(hwndToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);

			int BmOffset;

			tbBitmap.hInst = HINST_COMMCTRL;
			tbBitmap.nID = IDB_STD_SMALL_COLOR;
			BmOffset = SendMessage(hwndToolbar, TB_ADDBITMAP, 0, (LPARAM)&tbBitmap);
			tbButton[0].iBitmap += BmOffset;
			tbButton[1].iBitmap += BmOffset;
			tbButton[2].iBitmap += BmOffset;

			tbBitmap.hInst = HINST_COMMCTRL;
			tbBitmap.nID = IDB_VIEW_SMALL_COLOR;
			BmOffset = SendMessage(hwndToolbar, TB_ADDBITMAP, 0, (LPARAM)&tbBitmap);
			tbButton[4].iBitmap += BmOffset;
			tbButton[5].iBitmap += BmOffset;

			tbBitmap.hInst = HINST_COMMCTRL;
			tbBitmap.nID = IDB_STD_SMALL_COLOR;
			BmOffset = SendMessage(hwndToolbar, TB_ADDBITMAP, 0, (LPARAM)&tbBitmap);
			tbButton[6].iBitmap += BmOffset;

			SendMessage(hwndToolbar, TB_ADDBUTTONS, nCount, (LPARAM)&tbButton);

			RECT ToolBarRect;
			GetWindowRect(hwndToolbar, &ToolBarRect);

			// List View Setting
			InitCom.dwSize = sizeof(INITCOMMONCONTROLSEX);
			InitCom.dwICC  = ICC_LISTVIEW_CLASSES;
			InitCommonControlsEx(&InitCom);

			RECT ClientRect;
			GetClientRect(hWnd, &ClientRect);
			int ClientWidth  = ClientRect.right - ClientRect.left + 1;
			int ClientHeight = ClientRect.bottom - ClientRect.top - (ToolBarRect.bottom - ToolBarRect.top);

			ListWndHndl = CreateWindowEx(0, WC_LISTVIEW, _T(""), WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL, 0, ToolBarRect.bottom - ToolBarRect.top, ClientWidth, ClientHeight, hWnd, (HMENU)ID_LISTVIEW, InstHndl, NULL);
			ListView_SetExtendedListViewStyle(ListWndHndl, LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
			RefreshTableMenu(hWnd, NULL);
			ChangeWindowTitle(hWnd);
			ViewData(TRUE);
		}

		break;
	case WM_SIZE:
		RECT ToolBarRect;
		GetWindowRect(hwndToolbar, &ToolBarRect);
		MoveWindow(ListWndHndl, 0, ToolBarRect.bottom - ToolBarRect.top, LOWORD(lParam), HIWORD(lParam) - (ToolBarRect.bottom - ToolBarRect.top), TRUE);
		MoveWindow(hwndToolbar, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_COMMAND:
		WmId    = LOWORD(wParam); 
		WmEvent = HIWORD(wParam);

		// Table menu
		if (WmId >= 50000 && WmId < 60000) {
			// Select the table
			HMENU MenuHndl = GetMenu(hWnd);
			HMENU SubMenuHndl = GetSubMenu(MenuHndl, 1);
			CheckMenuItem(SubMenuHndl, SelectedTable, MF_BYPOSITION | MF_UNCHECKED);
			SelectedTable = WmId - 50000;
			CheckMenuItem(SubMenuHndl, SelectedTable, MF_BYPOSITION | MF_CHECKED);
			GetMenuString(SubMenuHndl, SelectedTable, SelectedTableName, 16, MF_BYPOSITION);

			// Clear the filtering conditions
			int LoopCmb;
			for (LoopCmb = 0; LoopCmb < 5; LoopCmb++) {
				SelectedComboItemIdx[LoopCmb] = 0;
				lstrcpyn(SelectedComboItemClm[LoopCmb], _T(""), 16);
				lstrcpyn(SelectedComboItemWStr[LoopCmb], _T(""), 256);
			}
			ViewData(FALSE);
			ChangeWindowTitle(hWnd);
			break;
		}

		// Refresh operation
		if (WmId == ID_OPERATION_REFRESH) {
			RefreshTableMenu(hWnd, SelectedTableName);
			ViewData(FALSE);
			ChangeWindowTitle(hWnd);
			break;
		}

		// File new
		if (WmId == ID_DATA_NEW) {
			lstrcpy(FileName, _T(""));
			TCHAR TableNames[MAX_TABLE_NUMBER][TABLE_NAME_SIZE];
			int TableNameCnt = GetTableName(TableNames);
			for (int Loop = 0; Loop < TableNameCnt; Loop++) {
				DeleteTable(TableNames[Loop]);
			}
			lstrcpy(SelectedTableName, _T(""));
			RefreshTableMenu(hWnd, NULL);
			ChangeWindowTitle(hWnd);
			ViewData(FALSE);
			break;
		}
		// File open
		if (WmId == ID_DATA_OPEN) {
			TCHAR FileNameA[32768] = _T("");
			if (GetOpenFileName(hWnd, FileNameA) == 0) {
				break;
			}
			lstrcpy(FileName, FileNameA);
			if (LockAllTable(LOCK_EXCLUSIVE) != 0) {
				UnlockAllTable();
			}
			LoadData(FileName);
			UnlockAllTable();
			RefreshTableMenu(hWnd, NULL);
			ChangeWindowTitle(hWnd);
			ViewData(FALSE);
			break;
		}
		// File save
		if (WmId == ID_DATA_SAVE) {
			if (lstrcmp(FileName, _T("")) == 0) {
				TCHAR FileNameA[32768] = _T("");
				if (GetSaveFileName(hWnd, FileNameA) == 0) {
					break;
				}
				lstrcpy(FileName, FileNameA);
			}
			if (LockAllTable(LOCK_SHARE) != 0) {
				UnlockAllTable();
			}
			SaveData(FileName);
			UnlockAllTable();
			RefreshTableMenu(hWnd, SelectedTableName);
			ViewData(FALSE);
			break;
		}
		// File save as
		if (WmId == ID_DATA_SAVEAS) {
			TCHAR FileNameA[32768] = _T("");
			if (GetSaveFileName(hWnd, FileNameA) == 0) {
				break;
			}
			lstrcpy(FileName, FileNameA);
			if (LockAllTable(LOCK_SHARE) != 0) {
				UnlockAllTable();
			}
			SaveData(FileName);
			UnlockAllTable();
			RefreshTableMenu(hWnd, SelectedTableName);
			ViewData(FALSE);
			break;
		}

		// Exit operation
		if (WmId == ID_EXIT) {
			DestroyWindow(hWnd);
			break;
		}

		// Show filter dialog box
		if (WmId == ID_OPERATION_FILTERING) {
			DialogBox(InstHndl, MAKEINTRESOURCE(IDD_COLUMN), hWnd, (DLGPROC)FilterDlg);
			break;
		}

		// Show insert dialog box
		if (WmId == ID_OPERATION_INSERT) {
			DialogBox(InstHndl, MAKEINTRESOURCE(IDD_INSERT), hWnd, (DLGPROC)InsertDlg);
			break;
		}

		// Update records
		if (WmId == ID_OPERATION_UPDATE) {
			RecordData* RecDat = GetCheckedRecord(hWnd);
			if (RecDat == NULL) {
				MessageBox(hWnd, _T("No record has been selected."), _T("Notice!"), MB_OK);
				break;
			}
			DialogBox(InstHndl, MAKEINTRESOURCE(IDD_COLUMN), hWnd, (DLGPROC)UpdateDlg);
			LockTable(SelectedTableName, LOCK_EXCLUSIVE);
			UpdateRecord(RecDat, UpdtRecDat);
			UnlockTable(SelectedTableName);
			ClearRecordData(RecDat);
			ViewData(FALSE);
			break;
		}

		// Delte records
		if (WmId == ID_OPERATION_DELETE) {
			DeleteRecord(hWnd);
			break;
		}

		// Add table
		if (WmId == ID_OPERATION_CREATETABLE) {
			DialogBox(InstHndl, MAKEINTRESOURCE(IDD_CREATETABLE), hWnd, (DLGPROC)CreateTableDlg);
			RefreshTableMenu(hWnd, SelectedTableName);
			ViewData(FALSE);
			break;
		}

		// Delete table
		if (WmId == ID_OPERATION_DELETETABLE) {
			if (MessageBox(hWnd, _T("Are you sure you want to delete the current table?"), _T("Notice!"), MB_OKCANCEL) == IDCANCEL) {
				break;
			}
			DeleteTable(SelectedTableName);
			if (GetTableCount() == 0) {
				lstrcpy(SelectedTableName, _T(""));
			}
			RefreshTableMenu(hWnd, NULL);
			ChangeWindowTitle(hWnd);
			ViewData(FALSE);
			break;
		}

		// Show table information
		if (WmId == ID_OPERATION_SHOWTABLEINFO) {
			if (lstrcmp(SelectedTableName, _T("")) == 0) {
				break;
			}
			TCHAR Buf[4096];
			TCHAR Buf2[256];
			lstrcpy(Buf, _T("Table name = "));
			lstrcat(Buf, SelectedTableName);
			wsprintf(Buf2, _T("\r\nMax number of records = %d\r\nNumber of records = %d"), 
				GetMaxNumOfRecords(SelectedTableName), GetNumOfRecords(SelectedTableName));
			lstrcat(Buf, Buf2);
			lstrcat(Buf, _T("\r\n\r\n"));
			TCHAR ColName[32][16];
			int ColCnt = GetColumnName(SelectedTableName, ColName);
			for (int Loop = 0; Loop < ColCnt; Loop++) {
				int CType = GetColumnType(SelectedTableName, ColName[Loop]);
				if (CType == COLUMN_TYPE_INT) {
					wsprintf(Buf2, _T("%02d: %s [Integer]\r\n"), Loop, ColName[Loop]);
				} else if (CType == COLUMN_TYPE_FLOAT) {
					wsprintf(Buf2, _T("%02d: %s [Float]\r\n"), Loop, ColName[Loop]);
				} else if (CType == COLUMN_TYPE_STR) {
					wsprintf(Buf2, _T("%02d: %s [String(%d)]\r\n"), Loop, ColName[Loop], GetColumnSize(SelectedTableName, ColName[Loop]));
				} else if (CType == COLUMN_TYPE_WSTR) {
					wsprintf(Buf2, _T("%02d: %s [WString(%d)]\r\n"), Loop, ColName[Loop], GetColumnSize(SelectedTableName, ColName[Loop]) / sizeof(TCHAR));
				} else if (CType == COLUMN_TYPE_BIN) {
					wsprintf(Buf2, _T("%02d: %s [Binary(%d)]\r\n"), Loop, ColName[Loop], GetColumnSize(SelectedTableName, ColName[Loop]));
				}
				lstrcat(Buf, Buf2);
			}
			MessageBox(hWnd, Buf, SelectedTableName, MB_OK);
			break;
		}

		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	case WM_NOTIFY:
		if (((LPNMHDR)lParam)->code == TTN_NEEDTEXT) {
			lptip = (LPTOOLTIPTEXT)lParam;
			lptip->hinst = InstHndl;
			switch (lptip->hdr.idFrom) {
			case ID_DATA_NEW:
				lptip->lpszText = TooltipDataNew;
				break;
			case ID_DATA_OPEN:
				lptip->lpszText = TooltipDataOpen;
				break;
			case ID_DATA_SAVE:
				lptip->lpszText = TooltipDataSave;
				break;
			case ID_OPERATION_INSERT:
				lptip->lpszText = TooltipOperationInsert;
				break;
			case ID_OPERATION_DELETE:
				lptip->lpszText = TooltipOperationDelete;
				break;
			case ID_OPERATION_UPDATE:
				lptip->lpszText = TooltipOperationUpdate;
				break;
			}
		}
		if ((int)wParam == ID_LISTVIEW) {
			NMLISTVIEW *Lview = (NMLISTVIEW *)lParam;
			if (Lview->hdr.code == LVN_COLUMNCLICK) {
				int Index = Lview->iSubItem;
				LVCOLUMN LvCol;
				TCHAR ClmName[16];
				LvCol.mask = LVCF_TEXT;
				LvCol.pszText = ClmName;
				LvCol.cchTextMax = 16;
				ListView_GetColumn(ListWndHndl, Index, &LvCol);

				SwitchSortRecord(SelectedTableName, LvCol.pszText);
				ViewData(FALSE);
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// This function refreshes the window menu.
// [in] HWND : Window handle
// [in] TCHAR* : Selected table name by window menu
void RefreshTableMenu(HWND WinHndl, TCHAR* SelTableName)
{
	MENUITEMINFO MItemInfo;
	HMENU SubMenuHndl = CreateMenu();

	HMENU MenuHndl = GetMenu(WinHndl);

	// Delete sub menu at first of all
	HMENU DelMenuHndl = GetSubMenu(MenuHndl, 1);
	if (DelMenuHndl != NULL) {
		DestroyMenu(DelMenuHndl);
	}

	BOOL IniFlag = TRUE;
	int Loop;
	int Pos = 0;
	SelectedTable = 0;
	TCHAR TableName[16][16];
	int TblCnt =GetTableName(TableName);
	for (Loop = 0; Loop < TblCnt; Loop++) {
		if (lstrcmp(TableName[Loop], _T("")) == 0) {
			continue;
		}

		memset(&MItemInfo, 0, sizeof(MENUITEMINFO));
		MItemInfo.cbSize = sizeof(MENUITEMINFO);
		MItemInfo.fMask = MIIM_ID | MIIM_TYPE | MIIM_STATE;
		if (SelTableName == NULL && IniFlag == TRUE) {
			lstrcpy(SelectedTableName, TableName[Loop]);
			IniFlag = FALSE;
		} else if (SelTableName != NULL && IniFlag == TRUE) {
			lstrcpy(SelectedTableName, SelTableName);
			IniFlag = FALSE;
		}
		if (SelTableName != NULL) {
			if (lstrcmp(SelTableName, TableName[Loop]) == 0) {
				SelectedTable = Pos;
				MItemInfo.fState = MFS_ENABLED | MFS_CHECKED;
			}
		} else if (Pos == 0) {
			MItemInfo.fState = MFS_ENABLED | MFS_CHECKED;
		} else {
		    MItemInfo.fState = MFS_ENABLED;
		}
		MItemInfo.fType = MFT_STRING;
	    MItemInfo.wID = 50000 + Pos;
	    MItemInfo.dwTypeData = TableName[Loop];
		InsertMenuItem(SubMenuHndl, Pos, TRUE, &MItemInfo);
		Pos++;
	}
	memset(&MItemInfo, 0, sizeof(MENUITEMINFO));
	MItemInfo.cbSize = sizeof(MENUITEMINFO);
	MItemInfo.fMask = MIIM_SUBMENU;
	MItemInfo.hSubMenu = SubMenuHndl;
	SetMenuItemInfo(MenuHndl, 1, TRUE, &MItemInfo);

	if (FileAccessFlag == FALSE) {
		HMENU FileMenuHndl = GetSubMenu(MenuHndl, 0);
		EnableMenuItem(FileMenuHndl, 0, MF_BYPOSITION | MF_GRAYED);
		EnableMenuItem(FileMenuHndl, 1, MF_BYPOSITION | MF_GRAYED);
	} else {
		HMENU FileMenuHndl = GetSubMenu(MenuHndl, 0);
		EnableMenuItem(FileMenuHndl, 0, MF_BYPOSITION | MF_ENABLED);
		EnableMenuItem(FileMenuHndl, 1, MF_BYPOSITION | MF_ENABLED);
	}

	UpdateWindow(WinHndl);
}

// This function refreshes List-View on the client area of the window.
// [in] BOOL : Initialization flag
void ViewData(BOOL Init)
{
	static int NumOfCol;
	if (Init == TRUE) {
		NumOfCol = 0;
	}

	LV_COLUMN LvColm;
	LV_ITEM LvItem;
	int Loop;

	// Delete items and columns
	ListView_DeleteAllItems(ListWndHndl);
	if (NumOfCol != 0) {
		for (Loop = 0; Loop < NumOfCol; Loop++) {
			ListView_DeleteColumn(ListWndHndl, 1);
		}
	}

	// Insert columns (1st column)
	LvColm.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	LvColm.fmt = LVCFMT_LEFT;
	if (NumOfCol == 0) {
		LvColm.cx = 30;
		LvColm.pszText = _T("");
		LvColm.iSubItem = 0;
		ListView_InsertColumn(ListWndHndl, 0, &LvColm);
	}

	// Insert columns (2nd and later column)
	TCHAR ColName[32][16];
	int ColCnt = GetColumnName(SelectedTableName, ColName);	
	for (Loop = 0; Loop < ColCnt; Loop++) {
		LvColm.cx = 100;
		LvColm.pszText = ColName[Loop];
		LvColm.iSubItem = Loop + 1;
		ListView_InsertColumn(ListWndHndl, Loop + 1, &LvColm);
	}
	NumOfCol = ColCnt;

	// Call GetRecord function.
	// In this function filtering conditions are checked.
	LockTable(SelectedTableName, LOCK_SHARE);
	RecordData* RtRecDat;
	int LoopFltr;
	int FoundFltrCond = 0;
	ColumnData* RqColDat[5];
	for (LoopFltr = 0; LoopFltr < 5; LoopFltr++) {
		if (SelectedComboItemIdx[LoopFltr] != 0) {
			int ColType = GetColumnType(SelectedTableName, SelectedComboItemClm[LoopFltr]);
			if (ColType == COLUMN_TYPE_INT) {
				int Val;
				Val = StrToInt(SelectedComboItemWStr[LoopFltr]);
				RqColDat[FoundFltrCond] = new ColumnDataInt(SelectedComboItemClm[LoopFltr], Val);
			} else if (ColType == COLUMN_TYPE_FLOAT) {
				float Val;
				swscanf_s(SelectedComboItemWStr[LoopFltr], _T("%g"), &Val);
				RqColDat[FoundFltrCond] = new ColumnDataFloat(SelectedComboItemClm[LoopFltr], Val);
			} else if (ColType == COLUMN_TYPE_STR) {
				char Buf[256];
				sprintf_s(Buf, 256, "%S", SelectedComboItemWStr[LoopFltr]);
				RqColDat[FoundFltrCond] = new ColumnDataStr(SelectedComboItemClm[LoopFltr], Buf);
			} else if (ColType == COLUMN_TYPE_WSTR) {
				RqColDat[FoundFltrCond] = new ColumnDataWStr(SelectedComboItemClm[LoopFltr], SelectedComboItemWStr[LoopFltr]);
			} else if (ColType == COLUMN_TYPE_BIN) {
				continue;
			}
			FoundFltrCond++;
		}
	}
	if (FoundFltrCond != 0) {
		RecordData* RqRecDat = new RecordData(SelectedTableName, RqColDat, FoundFltrCond);
		RtRecDat = GetRecord(RqRecDat);
		ClearRecordData(RqRecDat);
	} else {
		RtRecDat = GetRecord(SelectedTableName);
	}
	UnlockTable(SelectedTableName);

	// Insert records
	Loop = 0;
	RecordData *CurRecDat = RtRecDat;
	while(CurRecDat != NULL) {
		LvItem.mask = LVIF_TEXT;
		LvItem.pszText = _T("");
		LvItem.iItem = Loop;
		LvItem.iSubItem = 0;
		ListView_InsertItem(ListWndHndl, &LvItem);

		int RtColDatCnt = RtRecDat->GetColumnCount();
		int LoopRtColDat;
		for (LoopRtColDat = 0; LoopRtColDat < RtColDatCnt; LoopRtColDat++) {
			ColumnData* RtColDat = CurRecDat->GetColumn(LoopRtColDat);
			TCHAR Val[256];
			if (RtColDat->GetColumnType() == COLUMN_TYPE_INT) {
				ColumnDataInt* RtColDatInt = (ColumnDataInt*)RtColDat;
				int RtVal = RtColDatInt->GetValue();
				wsprintf(Val, _T("%d"), RtVal);
			} else if (RtColDat->GetColumnType() == COLUMN_TYPE_FLOAT) {
				ColumnDataFloat* RtColDatFloat = (ColumnDataFloat*)RtColDat;
				float RtVal = RtColDatFloat->GetValue();
				_stprintf_s(Val, _T("%.8e"), RtVal);
			} else if (RtColDat->GetColumnType() == COLUMN_TYPE_STR) {
				ColumnDataStr* RtColDatStr = (ColumnDataStr*)RtColDat;
				char* RtVal = RtColDatStr->GetValue();
				wsprintf(Val, _T("%S"), RtVal);
			} else if (RtColDat->GetColumnType() == COLUMN_TYPE_WSTR) {
				ColumnDataWStr* RtColDatWStr = (ColumnDataWStr*)RtColDat;
				lstrcpy(Val, RtColDatWStr->GetValue());
			} else if (RtColDat->GetColumnType() == COLUMN_TYPE_BIN) {
				lstrcpy(Val, _T("<<binary data>>"));
			}

			LvItem.pszText = Val;
			LvItem.iItem = Loop;
			LvItem.iSubItem = LoopRtColDat + 1;
			ListView_SetItem(ListWndHndl, &LvItem);
		}

		CurRecDat = CurRecDat->GetNextRecord();
		Loop++;
	};
	ClearRecordData(RtRecDat);
}

void ChangeWindowTitle(HWND WndHndl)
{
	TCHAR WinTitle[64];
	lstrcpy(WinTitle, WindowTitle);
	if (SelectedTableName != NULL && lstrcmp(SelectedTableName, _T("")) != 0) {
		lstrcat(WinTitle, _T("  ["));
		lstrcat(WinTitle, SelectedTableName);
		lstrcat(WinTitle, _T("]"));
	}
	SetWindowText(WndHndl, WinTitle);
}

int SwitchSortRecord(TCHAR* TableName, TCHAR* ColumnName)
{
	static TCHAR PrevTableName[16];
	static TCHAR PrevColumnName[16];
	static int PrevCompare = 0; // -1:Previous compare is [A to Z]. 1:Previous compare is [Z to A].
	if (PrevCompare == 0) {
		lstrcpy(PrevTableName, _T(""));
		lstrcpy(PrevColumnName, _T(""));
		PrevCompare = -1;
	}

	LockTable(TableName, LOCK_EXCLUSIVE);
	if (lstrcmp(TableName, PrevTableName) == 0 && lstrcmp(ColumnName, PrevColumnName) == 0) {
		if (PrevCompare == -1) {
			AzSortRecord(TableName, ColumnName);
			PrevCompare = 1;
		} else {
			ZaSortRecord(TableName, ColumnName);
			PrevCompare = -1; 
		}
	} else {
		AzSortRecord(TableName, ColumnName);
		PrevCompare = 1;
	}
	lstrcpy(PrevTableName, TableName);
	lstrcpy(PrevColumnName, ColumnName);
	UnlockTable(TableName);

	return PrevCompare;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	InstHndl = hInstance;
	EnableFileAccessFlag();
	lstrcpy(WindowTitle, _T("StkData GUI"));
	ShowStkDataWindow(WindowTitle, NULL);
	return 0;
}
