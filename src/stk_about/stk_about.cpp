#include <windows.h>
#include <tchar.h>
#include <commctrl.h>
#include <shlwapi.h>
#include "stk_about.h"

#define IDC_INFO                        3301
#define IDC_HISTORY                     3302

#define STKABOUT_ERRDLG_TITLE "Version information"
#define STKABOUT_MAX_ITEM 128 // Max number of history item
#define STKABOUT_TITLE_LENGTH 256 // Title and version length
#define STKABOUT_CHGID_LENGTH 5 // Change ID length
#define STKABOUT_VER_LENGTH 10 // Version length
#define STKABOUT_DESC_LENGTH 256 // Description length

HINSTANCE StkAboutInstHndl; // Handler of dialog instance
TCHAR Title[STKABOUT_TITLE_LENGTH]; // Program title and version
int NumOfItem; // Number of history items
WORD Type[STKABOUT_MAX_ITEM]; // type of history item (0:Bug fix, 1:Enhancement)
TCHAR ChgId[STKABOUT_MAX_ITEM][STKABOUT_CHGID_LENGTH]; // Change ID of history item
TCHAR Ver[STKABOUT_MAX_ITEM][STKABOUT_VER_LENGTH]; // Version of history item
TCHAR Desc[STKABOUT_MAX_ITEM][STKABOUT_DESC_LENGTH]; // Description of history item

int StkAbout_IconAbout;
int StkAbout_IconBug;
int StkAbout_IconEnhance;

int ErrorHistoryItemLimitation(HWND DlgHndl);
int ErrorInternalIconSet(HWND DlgHndl);
int ErrorInternalDlgTitle(HWND DlgHndl);


void SetStkAboutIconAbout(int IconId)
{
	StkAbout_IconAbout = IconId;
}

void SetStkAboutIconBug(int IconId)
{
	StkAbout_IconBug = IconId;
}

void SetStkAboutIconEnhance(int IconId)
{
	StkAbout_IconEnhance = IconId;
}

/********** This function sets program title and version **********/
void SetAboutTitle(TCHAR* TitleL)
{
	StrCpyN(Title, TitleL, STKABOUT_TITLE_LENGTH);
}

/********** This function adds a history item **********/
void AddHistoryItem(WORD TypeL, TCHAR *ChgIdL, TCHAR *VerL, TCHAR *DescL)
{
	if (NumOfItem >= STKABOUT_MAX_ITEM) {
		ErrorHistoryItemLimitation(NULL);
		return;
	}
	Type[NumOfItem] = TypeL;
	StrCpyN(ChgId[NumOfItem], ChgIdL, STKABOUT_CHGID_LENGTH);
	StrCpyN(Ver[NumOfItem], VerL, STKABOUT_VER_LENGTH);
	StrCpyN(Desc[NumOfItem], DescL, STKABOUT_DESC_LENGTH);
	NumOfItem++;
}

/********** This function clears all history items **********/
void ClearHistoryItem()
{
	NumOfItem = 0;
}

BOOL CALLBACK SetStkAboutFontProc(HWND hwnd, LPARAM lParam)
{
	static HFONT FontHndl = CreateFont(15, 0, 0, 0, FW_REGULAR, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T("Arial"));
	SendMessage(hwnd, WM_SETFONT, (WPARAM)FontHndl, (LPARAM)TRUE);
	return TRUE;
}

LRESULT CALLBACK AboutDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	HWND BkGndWndHndl;
	HWND ButtonOK;
	HWND ListWndHndl;
	HWND AboutTitle;

	RECT Wrect;

	// For icon information
	HICON IconHndlrS;
	HICON IconHndlrB;

	// For column name
	LV_COLUMN LvColumn[] = {
		{LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, LVCFMT_LEFT, 40, _T("Type"), 0, 0},
		{LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, LVCFMT_LEFT, 40, _T("ID"), 0, 1},
		{LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, LVCFMT_LEFT, 60, _T("Version"), 0, 2},
		{LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, LVCFMT_LEFT, 380, _T("Description"), 0, 3}
	};

	// For icon image
	HIMAGELIST ImLstHndl;

	// For row registration
	LVITEM LvItm;
	int ColumnLoop;
	int RowLoop;
	TCHAR TypeB[2] = _T("B");
	TCHAR TypeE[2] = _T("E");

	switch (message) {
	case WM_CREATE:
		InitCommonControls();
		GetClientRect(hWnd, &Wrect);

		BkGndWndHndl  = CreateWindow(_T("STATIC"), _T(""), WS_CHILD | WS_VISIBLE, 0, 0, Wrect.right, Wrect.bottom, hWnd, NULL, StkAboutInstHndl, NULL);
		ButtonOK      = CreateWindow(_T("BUTTON"), _T("OK"), WS_CHILD | WS_VISIBLE, Wrect.right - 60, Wrect.bottom - 35, 50, 25, hWnd, (HMENU)IDOK, StkAboutInstHndl, NULL);
		AboutTitle    = CreateWindowEx(WS_EX_CLIENTEDGE, _T("EDIT"), _T(""), WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_MULTILINE, 10, 10, Wrect.right - 20, 60, BkGndWndHndl, (HMENU)IDC_INFO, StkAboutInstHndl, NULL);
		ListWndHndl   = CreateWindowEx(0, WC_LISTVIEW, _T(""), WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS, 10, 70, Wrect.right - 20, Wrect.bottom - 110, BkGndWndHndl, (HMENU)IDC_HISTORY, StkAboutInstHndl, NULL);
		ListView_SetExtendedListViewStyle(ListWndHndl, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
		SendMessage(AboutTitle, EM_SETREADONLY, (WPARAM)TRUE, (LPARAM)0);

		// Sets icon information
		IconHndlrS = (HICON)LoadImage(StkAboutInstHndl, (LPCTSTR)StkAbout_IconAbout, IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
		IconHndlrB = (HICON)LoadImage(StkAboutInstHndl, (LPCTSTR)StkAbout_IconAbout, IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
		if (IconHndlrS == NULL || IconHndlrB == NULL) {
			ErrorInternalIconSet(hWnd);
			break;
		}
		SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)IconHndlrS);
		SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)IconHndlrB);

		// Sets icon images
		if ((ImLstHndl = ImageList_Create(16, 16, ILC_COLOR8 | ILC_MASK, 2, 0)) == NULL) {
			ErrorInternalIconSet(hWnd);
			EndDialog(hWnd, (INT_PTR)0);
		}
		ImageList_AddIcon(ImLstHndl, LoadIcon(StkAboutInstHndl, MAKEINTRESOURCE(StkAbout_IconBug)));
		ImageList_AddIcon(ImLstHndl, LoadIcon(StkAboutInstHndl, MAKEINTRESOURCE(StkAbout_IconEnhance)));
		SendDlgItemMessage(BkGndWndHndl, IDC_HISTORY, LVM_SETIMAGELIST, LVSIL_SMALL, (LPARAM)ImLstHndl);

		// Adds column names
		for (ColumnLoop = 0; ColumnLoop < 4; ColumnLoop++) {
			SendDlgItemMessage(BkGndWndHndl, IDC_HISTORY, LVM_INSERTCOLUMN, ColumnLoop, (LPARAM)&LvColumn[ColumnLoop]);
		}

		// Adds rows
		for (RowLoop = NumOfItem - 1; RowLoop >= 0; RowLoop--) {
			for (ColumnLoop = 0; ColumnLoop < 4; ColumnLoop++) {
				ZeroMemory(&LvItm, sizeof(LvItm));
				LvItm.iItem = NumOfItem - RowLoop - 1;
				LvItm.iSubItem = ColumnLoop;
				switch (ColumnLoop % 4) {
				case 0:
					if (StkAbout_IconBug != 0 && StkAbout_IconEnhance != 0) {
						LvItm.mask = LVIF_IMAGE | LVIF_TEXT;
						LvItm.iImage = Type[RowLoop];
					} else {
						LvItm.mask = LVIF_TEXT;
					}
					if (Type[RowLoop] == 0) {
						LvItm.pszText = TypeB;
					} else {
						LvItm.pszText = TypeE;
					}
					SendDlgItemMessage(BkGndWndHndl, IDC_HISTORY, LVM_INSERTITEM, 0, (LPARAM)&LvItm);
					break;
				case 1:
					LvItm.mask = LVIF_TEXT;
					LvItm.pszText = ChgId[RowLoop];
					SendDlgItemMessage(BkGndWndHndl, IDC_HISTORY, LVM_SETITEM, 0, (LPARAM)&LvItm);
					break;
				case 2:
					LvItm.mask = LVIF_TEXT;
					LvItm.pszText = Ver[RowLoop];
					SendDlgItemMessage(BkGndWndHndl, IDC_HISTORY, LVM_SETITEM, 0, (LPARAM)&LvItm);
					break;
				case 3:
					LvItm.mask = LVIF_TEXT;
					LvItm.pszText = Desc[RowLoop];
					SendDlgItemMessage(BkGndWndHndl, IDC_HISTORY, LVM_SETITEM, 0, (LPARAM)&LvItm);
					break;
				}
			}
		}

		// Sets title
		if (SetDlgItemText(BkGndWndHndl, IDC_INFO, Title) == 0) {
			ErrorInternalDlgTitle(hWnd);
			break;
		}

		EnumChildWindows(hWnd, SetStkAboutFontProc, NULL);
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			DestroyWindow(hWnd);
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

void StkAboutRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	wcex.cbSize        = sizeof(WNDCLASSEX); 
	wcex.style         = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc   = (WNDPROC)AboutDlgProc;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = 0;
	wcex.hInstance     = hInstance;
	wcex.hIcon         = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hIconSm       = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName  = NULL;
	wcex.lpszClassName = _T("Version Information");
	RegisterClassEx(&wcex);
}

/********** This function shows an ABOUT dialog box **********/
void ShowAboutDialog(HWND hWnd)
{
	static HWND WndHndl = NULL;

	if (WndHndl != NULL) {
		ShowWindow(WndHndl, SW_NORMAL);
		SetWindowPos(WndHndl, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		return;
	}
	StkAboutInstHndl = GetModuleHandle(NULL);
	MSG msg;
	StkAboutRegisterClass(StkAboutInstHndl);
	WndHndl = CreateWindow(_T("Version Information"), _T("Version Information"), WS_CAPTION | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 580, 250, hWnd, NULL, StkAboutInstHndl, NULL);
	ShowWindow(WndHndl, SW_SHOW);
	UpdateWindow(WndHndl);
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	WndHndl = NULL;
}

/********** A limitation error of history item occurs **********/
int ErrorHistoryItemLimitation(HWND DlgHndl)
{
	return MessageBox(DlgHndl, 
		_T("The number of history items that can be registered has been exceeded.\r\nThe history item presented has been dropped."), 
		_T(STKABOUT_ERRDLG_TITLE), 
		MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
}

/********** An internal error occurs (icon could not be loaded) **********/
int ErrorInternalIconSet(HWND DlgHndl)
{
	return MessageBox(DlgHndl, 
		_T("An internal error has occurred.\r\nThe icon information could not be set."), 
		_T(STKABOUT_ERRDLG_TITLE), 
		MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
}

/********** An internal error occurs (Dialog title could not be set) **********/
int ErrorInternalDlgTitle(HWND DlgHndl)
{
	return MessageBox(DlgHndl, 
		_T("An internal error has occurred.\r\nThe dialog title could not be set."), 
		_T(STKABOUT_ERRDLG_TITLE), 
		MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
}
