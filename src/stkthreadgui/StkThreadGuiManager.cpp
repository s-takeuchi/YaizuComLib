#include "StkThreadGuiManager.h"
#include "..\stkthread\stkthread.h"
#include "StkThreadGuiLogging.h"


StkThreadGuiManager* StkThreadGuiManager::ThisInstance;

VOID CALLBACK StkThreadGuiManager::ReplaceStatus(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	StkThreadGuiManager* StkThGuiMgr = StkThreadGuiManager::GetInstance();

	//Replace status
	for (int RowLoop = 0; RowLoop < GetNumOfStkThread(); RowLoop++) {
		StkThGuiMgr->ReplaceStatus(RowLoop);
	}

	// Change Start and Stop button status
	BOOL Fnd = FALSE;
	BOOL AllReady = TRUE;
	for (int RowLoop = 0; RowLoop < GetNumOfStkThread(); RowLoop++) {
		int St = GetStkThreadStatusByIndex(RowLoop);
		if (St == STKTHREAD_STATUS_STARTING || St == STKTHREAD_STATUS_STOPPING) {
			Fnd = TRUE;
		}
		if (St != STKTHREAD_STATUS_READY) {
			AllReady = FALSE;
		}
	}
	if (Fnd == TRUE) {
		//Disable Start and Stop button.
		EnableWindow(GetDlgItem(StkThGuiMgr->DlgWndHndl, IDC_START), FALSE);
		EnableWindow(GetDlgItem(StkThGuiMgr->DlgWndHndl, IDC_STOP), FALSE);
	} else {
		//Enable Start and Stop button.
		EnableWindow(GetDlgItem(StkThGuiMgr->DlgWndHndl, IDC_START), TRUE);
		EnableWindow(GetDlgItem(StkThGuiMgr->DlgWndHndl, IDC_STOP), TRUE);
	}
	if (AllReady == FALSE) {
		StkThGuiMgr->RefreshLog(FALSE);
	}
}

BOOL CALLBACK StkThreadGuiManager::SetStkThreadFontProc(HWND hwnd, LPARAM lParam)
{
	static HFONT FontHndl = CreateFont(16, 0, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T("Arial"));
	SendMessage(hwnd, WM_SETFONT, (WPARAM)FontHndl, (LPARAM)TRUE);
	return TRUE;
}

LRESULT CALLBACK StkThreadGuiManager::StkThreadProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	StkThreadGuiManager* StkThGuiMgr = StkThreadGuiManager::GetInstance();

	// For icon information
	HICON IconHndlrS = NULL;
	HICON IconHndlrB = NULL;

	HWND BkGndWndHndl = NULL;
	HWND ButtonStart = NULL;
	HWND ButtonStop = NULL;
	HWND CheckAll = NULL;

	RECT Wrect;

	// Flag which checks whether the check box on list-view was clicked.
	static BOOL MasterClkFlag = FALSE;

	switch (message) {
	case WM_CREATE:
		InitCommonControls();
		GetClientRect(hWnd, &Wrect);

		BkGndWndHndl = CreateWindow(_T("STATIC"), _T(""), WS_CHILD | WS_VISIBLE | WS_DLGFRAME, 0, Wrect.bottom - 50, Wrect.right, 50, hWnd, NULL, StkThGuiMgr->DlgInstHndl, NULL);
		StkThGuiMgr->ListWndHndl = CreateWindowEx(0, WC_LISTVIEW, _T(""), WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | WS_TABSTOP, 0, 190, Wrect.right, Wrect.bottom - 240, hWnd, (HMENU)IDC_THREADLIST, StkThGuiMgr->DlgInstHndl, NULL);
		CheckAll = CreateWindow(_T("BUTTON"), _T("Check/Uncheck All (&A)"), WS_CHILD | WS_VISIBLE | BS_3STATE | WS_TABSTOP, Wrect.left + 10, Wrect.bottom - 35, 180, 25, hWnd, (HMENU)IDC_ALLCHECK, StkThGuiMgr->DlgInstHndl, NULL);
		ButtonStart = CreateWindow(_T("BUTTON"), _T("Start (&R)"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, Wrect.right - 170, Wrect.bottom - 35, 70, 25, hWnd, (HMENU)IDC_START, StkThGuiMgr->DlgInstHndl, NULL);
		ButtonStop = CreateWindow(_T("BUTTON"), _T("Stop (&S)"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, Wrect.right - 90, Wrect.bottom - 35, 70, 25, hWnd, (HMENU)IDC_STOP, StkThGuiMgr->DlgInstHndl, NULL);
		StkThGuiMgr->Tracelog = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), _T(""), WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE | WS_TABSTOP, 0, 0, Wrect.right, 190, hWnd/*BkGndWndHndl*/, (HMENU)IDC_TRACELOG, StkThGuiMgr->DlgInstHndl, NULL);
		ListView_SetExtendedListViewStyle(StkThGuiMgr->ListWndHndl, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
		SendMessage(StkThGuiMgr->Tracelog, EM_SETREADONLY, (WPARAM)TRUE, (LPARAM)0);
		SendMessage(StkThGuiMgr->Tracelog, EM_SETLIMITTEXT, (WPARAM)0, (LPARAM)0);

		StkThGuiMgr->RefreshLog(TRUE);

		EnumChildWindows(hWnd, SetStkThreadFontProc, NULL);

		// Sets icon information for the dialog box
		IconHndlrS = (HICON)LoadImage(StkThGuiMgr->DlgInstHndl, (LPCTSTR)MAKEINTRESOURCEW(StkThGuiMgr->IconId), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
		IconHndlrB = (HICON)LoadImage(StkThGuiMgr->DlgInstHndl, (LPCTSTR)MAKEINTRESOURCEW(StkThGuiMgr->IconId), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
		if (IconHndlrS != NULL && IconHndlrB != NULL) {
			SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)IconHndlrS);
			SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)IconHndlrB);
		}

		// Init list-view
		if (StkThGuiMgr->InitListView() == -1) {
			return 0;
		}

		// Init Check box
		CheckDlgButton(hWnd, IDC_ALLCHECK, BST_CHECKED);

		SetTimer(StkThGuiMgr->DlgWndHndl, 333, 2000, (TIMERPROC)ReplaceStatus);

		break;

	case WM_COMMAND:
		// Check/Uncheck box was clicked
		if (HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == IDC_ALLCHECK) {
			UINT Sts = IsDlgButtonChecked(hWnd, IDC_ALLCHECK);
			BOOL ChkUnchk;
			static BOOL PrevChkUnchk = TRUE;
			if (Sts == BST_CHECKED) {
				ChkUnchk = FALSE;
			} else if (Sts == BST_UNCHECKED) {
				ChkUnchk = TRUE;
			} else if (Sts == BST_INDETERMINATE) {
				ChkUnchk = (PrevChkUnchk == TRUE)? FALSE : TRUE;
			}
			MasterClkFlag = TRUE;
			CheckDlgButton(hWnd, IDC_ALLCHECK, (ChkUnchk == TRUE)? BST_CHECKED : BST_UNCHECKED);
			PrevChkUnchk = ChkUnchk;
			int RowLoop;
			for (RowLoop = 0; RowLoop < GetNumOfStkThread(); RowLoop++) {
				ListView_SetCheckState(StkThGuiMgr->ListWndHndl, RowLoop, ChkUnchk);
			}
			break;
		}

		// Close button was clicked
		if (LOWORD(wParam) == IDCANCEL) {
			DestroyWindow(hWnd);
		}

		// Start button
		if (LOWORD(wParam) == IDC_START) {
			StkThGuiMgr->StartThreads(FALSE);
		}

		// Stop button
		if (LOWORD(wParam) == IDC_STOP) {
			StkThGuiMgr->StopThreads(FALSE);
		}
		break;

	case WM_NOTIFY:
		if (wParam == IDC_THREADLIST) {

			// Preserve LPARAM
			NMLISTVIEW* NmListView = (NMLISTVIEW*)lParam;
			NMITEMACTIVATE* NmItemActivate = (NMITEMACTIVATE*)lParam;
			NMLVKEYDOWN* NmLvKeyDown = (NMLVKEYDOWN*)lParam;

			// Something on list-view area was clicked.
			if ((NmItemActivate->hdr).code == NM_CLICK) {
				MasterClkFlag = FALSE;
				break;
			}

			// Check box on list-view was changed.
			if ((NmListView->hdr).code == LVN_ITEMCHANGED &&
				(NmListView->uOldState & LVIS_STATEIMAGEMASK) != (NmListView->uNewState & LVIS_STATEIMAGEMASK) &&
				MasterClkFlag == FALSE) {
				StkThGuiMgr->SetStatusOfMasterCheckBox();
				break;
			}
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefDlgProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


//////////////////////////////////////////////////////////////////////////////
// PRIVATE operations
//////////////////////////////////////////////////////////////////////////////

StkThreadGuiManager::StkThreadGuiManager()
{
	DlgWndHndl = NULL;
	ListWndHndl = NULL;
	DlgInstHndl = NULL;
}

// Destructor
StkThreadGuiManager::~StkThreadGuiManager()
{
}

// Set status of master check box
// [in] Nothing
// [out] Nothing
void StkThreadGuiManager::SetStatusOfMasterCheckBox()
{
	int CheckedCnt = 0;
	int RowLoop;
	for (RowLoop = 0; RowLoop < GetNumOfStkThread(); RowLoop++) {
		if (ListView_GetCheckState(ListWndHndl, RowLoop) == TRUE) {
			CheckedCnt++;
		}
	}
	if (CheckedCnt == 0) {
		CheckDlgButton(DlgWndHndl, IDC_ALLCHECK, BST_UNCHECKED);
	} else if (CheckedCnt == GetNumOfStkThread()) {
		CheckDlgButton(DlgWndHndl, IDC_ALLCHECK, BST_CHECKED);
	} else {
		CheckDlgButton(DlgWndHndl, IDC_ALLCHECK, BST_INDETERMINATE);
	}
}

// ログ出力領域を更新する
// [in] Force : TRUEのときは強制的にログ出力領域を更新。FALSEのときは前回から変更あれば更新。
// [out] なし
void StkThreadGuiManager::RefreshLog(BOOL Force)
{
	StkThreadGuiLogging* StkLog = StkThreadGuiLogging::GetInstance();
	
	static int PrevUpdVer = 0;
	int CurrUpdVer = StkLog->GetLogUpdateVersion();

	if (Force == TRUE || PrevUpdVer != CurrUpdVer) {
		PrevUpdVer = CurrUpdVer;
		TCHAR *Out = new TCHAR[StkLog->GetLogMaxSize()];
		StkLog->GetLog(Out, StkLog->GetLogMaxSize());
		SendMessage(Tracelog, WM_SETTEXT, (WPARAM)0, (LPARAM)Out);
		delete Out;
	}
}


// Start the specified threads
// [in] AllThreads : If you start all threads, TRUE is to be passed.  Otherwize FALSE is passed.
// [out] Result Code
int StkThreadGuiManager::StartThreads(BOOL AllThreads)
{
	if (DlgWndHndl != NULL) {
		SetFocus(DlgWndHndl); // TABフォーカスが失われるのを防止する施策

		//Disable Start and Stop button.
		EnableWindow(GetDlgItem(DlgWndHndl, IDC_START), FALSE);
		EnableWindow(GetDlgItem(DlgWndHndl, IDC_STOP), FALSE);
	}

	// Change the status on GUI.
	for (int RowLoop = 0; RowLoop < GetNumOfStkThread(); RowLoop++) {
		if (AllThreads == TRUE || ListView_GetCheckState(ListWndHndl, RowLoop) == TRUE) {
			if (GetStkThreadStatusByIndex(RowLoop) != STKTHREAD_STATUS_RUNNING) {
				ReplaceStatus(RowLoop, STKTHREAD_STATUS_STARTING);
			}
		}
	}

	// Does selected thread IDs are stored into Ids[] variable.
	int Ids[MAX_NUM_OF_THREADS];
	int IdsIndex = 0;
	for (int RowLoop = 0; RowLoop < GetNumOfStkThread(); RowLoop++) {
		if ((AllThreads == TRUE || ListView_GetCheckState(ListWndHndl, RowLoop) == TRUE) &&
			GetStkThreadStatusByIndex(RowLoop) == STKTHREAD_STATUS_READY) {
			Ids[IdsIndex] = GetStkThreadIdByIndex(RowLoop);
			IdsIndex++;
		}
	}

	// Start selected threads.
	StartSpecifiedStkThreads(Ids, IdsIndex);

	// When AllThreads is TRUE, check whether all threads are running or not.
	// If some threads are not running, wait for the status change.
	while (AllThreads) {
		BOOL AllRunning = TRUE;
		for (int RowLoop = 0; RowLoop < GetNumOfStkThread(); RowLoop++) {
			if (GetStkThreadStatusByIndex(RowLoop) != STKTHREAD_STATUS_RUNNING) {
				AllRunning = FALSE;
				break;
			}
		}
		if (AllRunning == TRUE) {
			break;
		} else {
			Sleep(100);
		}
	}

	RefreshLog(FALSE);

	return 0;
}

// Stop the specified threads
// [in] AllThreads : If you stop all threads, TRUE is to be passed.  Otherwize FALSE is passed.
// [out] Result Code
int StkThreadGuiManager::StopThreads(BOOL AllThreads)
{
	if (DlgWndHndl != NULL) {
		SetFocus(DlgWndHndl); // TABフォーカスが失われるのを防止する施策

		//Disable Start and Stop button.
		EnableWindow(GetDlgItem(DlgWndHndl, IDC_START), FALSE);
		EnableWindow(GetDlgItem(DlgWndHndl, IDC_STOP), FALSE);
	}

	// Change the status on GUI.
	for (int RowLoop = 0; RowLoop < GetNumOfStkThread(); RowLoop++) {
		if (AllThreads == TRUE || ListView_GetCheckState(ListWndHndl, RowLoop) == TRUE) {
			if (GetStkThreadStatusByIndex(RowLoop) != STKTHREAD_STATUS_READY) {
				ReplaceStatus(RowLoop, STKTHREAD_STATUS_STOPPING);
			}
		}
	}

	// Does selected thread IDs are stored into Ids[] variable.
	int Ids[MAX_NUM_OF_THREADS];
	int IdsIndex = 0;
	for (int RowLoop = 0; RowLoop < GetNumOfStkThread(); RowLoop++) {
		if ((AllThreads == TRUE || ListView_GetCheckState(ListWndHndl, RowLoop) == TRUE) &&
			GetStkThreadStatusByIndex(RowLoop) == STKTHREAD_STATUS_RUNNING) {
			Ids[IdsIndex] = GetStkThreadIdByIndex(RowLoop);
			IdsIndex++;
		}
	}

	// Stop selected threads.
	StopSpecifiedStkThreads(Ids, IdsIndex);

	RefreshLog(FALSE);

	return 0;
}

// Replace status of specified item
// [in] int : Index
// [out] Nothing
void StkThreadGuiManager::ReplaceStatus(int Index)
{
	int St = GetStkThreadStatusByIndex(Index);
	ReplaceStatus(Index, St);
}

// Replace status of specified item
// Index [in] : Index
// Status [in] : Status
// [out] Nothing
void StkThreadGuiManager::ReplaceStatus(int Index, int St)
{
	LVITEM LvItm;
	ZeroMemory(&LvItm, sizeof(LvItm));
	LvItm.iItem = Index;
	LvItm.iSubItem = 3;
	LvItm.mask = LVIF_TEXT;
	if (St == STKTHREAD_STATUS_READY) {
		LvItm.pszText = _T("Ready");
	} else if (St == STKTHREAD_STATUS_STARTING) {
		LvItm.pszText = _T("Starting");
	} else if (St == STKTHREAD_STATUS_RUNNING) {
		LvItm.pszText = _T("Running");
	} else if (St == STKTHREAD_STATUS_STOPPING) {
		LvItm.pszText = _T("Stopping");
	}
	ListView_SetItem(ListWndHndl, &LvItm);
}

// Add recourd into List-View
// [in] int : Index
// [in] TCHAR* : Name of record
// [in] TCHAR* : Description
// [in] int : Status
// [out] Nothing
void StkThreadGuiManager::AddRecordToListView(int Index, TCHAR* Na, TCHAR* De)
{
	LVITEM LvItm;
	for (int ColumnLoop = 0; ColumnLoop < 4; ColumnLoop++) {
		ZeroMemory(&LvItm, sizeof(LvItm));
		LvItm.iItem = Index;
		LvItm.iSubItem = ColumnLoop;
		switch (ColumnLoop % 4) {
		case 0:
			LvItm.mask = LVIF_TEXT;
			LvItm.iImage = 0;
			LvItm.pszText = _T("");
			ListView_InsertItem(ListWndHndl, &LvItm);
			ListView_SetCheckState(ListWndHndl, Index, TRUE);
			break;
		case 1:
			LvItm.mask = LVIF_TEXT;
			LvItm.pszText = Na;
			ListView_SetItem(ListWndHndl, &LvItm);
			break;
		case 2:
			LvItm.mask = LVIF_TEXT;
			LvItm.pszText = De;
			ListView_SetItem(ListWndHndl, &LvItm);
			break;
		}
		ReplaceStatus(Index);
	}
}

// Delete recourd from List-View
// [in] int : Index
// [out] Nothing
void StkThreadGuiManager::DeleteRecordFromListView(int index)
{
	ListView_DeleteItem(ListWndHndl, index);
}

// Initialize list-view area
// [in] Nothing
// [out] int : Result code (0:Success, -1:Error)
int StkThreadGuiManager::InitListView()
{
	// Loop counter
	int ColumnLoop;
	int RowLoop;

	// Image-List handle

	// For column name
	LV_COLUMN LvColumn[] = {
		{LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, LVCFMT_LEFT, 30, _T(""), 0, 0},
		{LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, LVCFMT_LEFT, 160, _T("Name"), 0, 1},
		{LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, LVCFMT_LEFT, 250, _T("Description"), 0, 2},
		{LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, LVCFMT_LEFT, 80, _T("Status"), 0, 3}
	};

	// Sets List-View style
	ListView_SetExtendedListViewStyle(ListWndHndl, LVS_EX_CHECKBOXES | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	// Adds column names
	for (ColumnLoop = 0; ColumnLoop < 4; ColumnLoop++) {
		SendMessage(ListWndHndl, LVM_INSERTCOLUMN, ColumnLoop, (LPARAM)&LvColumn[ColumnLoop]);
	}

	// Delete all rows
	ListView_DeleteAllItems(ListWndHndl);

	// Adds rows
	for (RowLoop = 0; RowLoop < GetNumOfStkThread(); RowLoop++) {
		TCHAR WorkName[MAX_LENGTH_OF_STKTHREAD_NAME];
		TCHAR WorkDesc[MAX_LENGTH_OF_STKTHREAD_DESCRIPTION];
		GetStkThreadNameByIndex(RowLoop, WorkName);
		GetStkThreadDescriptionByIndex(RowLoop, WorkDesc);

		AddRecordToListView(RowLoop, WorkName, WorkDesc);
	}
	return 0;
}


//////////////////////////////////////////////////////////////////////////////
// PUBLIC operations
//////////////////////////////////////////////////////////////////////////////

// Get this instance
StkThreadGuiManager* StkThreadGuiManager::GetInstance()
{
	static int Init = 1;
	if (Init == 1) {
		Init = 0;
		ThisInstance = new StkThreadGuiManager();
	}
	return ThisInstance;
}

int StkThreadGuiManager::ShowStkThreadController(HWND ParentWnd, int ParentIconId, TCHAR* Title)
{
	if (DlgWndHndl != NULL) {
		ShowWindow(DlgWndHndl, SW_NORMAL);
		SetWindowPos(DlgWndHndl, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		SetForegroundWindow(DlgWndHndl);
		return 0;
	}

	IconId = ParentIconId;
	DlgInstHndl = GetModuleHandle(NULL);

	WNDCLASSEX wcex;
	wcex.cbSize        = sizeof(WNDCLASSEX); 
	wcex.style         = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc   = (WNDPROC)StkThreadGuiManager::StkThreadProc;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = DLGWINDOWEXTRA;
	wcex.hInstance     = DlgInstHndl;
	wcex.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName  = NULL;
	wcex.lpszClassName = Title;
	RegisterClassEx(&wcex);

	DlgWndHndl = CreateWindow(Title, Title, WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 580, 400, ParentWnd, NULL, DlgInstHndl, NULL);
	ShowWindow(DlgWndHndl, SW_SHOW);
	UpdateWindow(DlgWndHndl);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		if (!IsDialogMessage(DlgWndHndl, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//TranslateMessage(&msg);
		//DispatchMessage(&msg);
	}
	DlgWndHndl = NULL;

	return 0;
}

void StkThreadGuiManager::StartAllThreads()
{
	StartThreads(TRUE);
}

void StkThreadGuiManager::StopAllThreads()
{
	StopThreads(TRUE);
}

void StkThreadGuiManager::AddStkThreadForGui(int Id, TCHAR* Name, TCHAR* Description, void* Init, void* Final, void* Main, void* Start, void* Stop)
{
	int NumOfStkThread = GetNumOfStkThread();
	AddStkThread(Id, Name, Description, Init, Final, Main, Start, Stop);
	AddRecordToListView(NumOfStkThread, Name, Description);
	SetStatusOfMasterCheckBox();
}

void StkThreadGuiManager::DeleteStkThreadForGui(int TargetId)
{
	//Disable Start and Stop button.
	EnableWindow(GetDlgItem(DlgWndHndl, IDC_START), FALSE);
	EnableWindow(GetDlgItem(DlgWndHndl, IDC_STOP), FALSE);

	int TargetIndex = -1;
	for (int Loop = 0; Loop < GetNumOfStkThread(); Loop++) {
		if (GetStkThreadIdByIndex(Loop) == TargetId) {
			TargetIndex = Loop;
			break;
		}
	}
	DeleteStkThread(TargetId);

	DeleteRecordFromListView(TargetIndex);
	SetStatusOfMasterCheckBox();

	//Enable Start and Stop button.
	EnableWindow(GetDlgItem(DlgWndHndl, IDC_START), TRUE);
	EnableWindow(GetDlgItem(DlgWndHndl, IDC_STOP), TRUE);
}
