#pragma once
#include <windows.h>
#include <commctrl.h>

class StkThreadGuiManager
{
private:
	static const int IDC_START = 3201;
	static const int IDC_STOP = 3202;
	static const int IDC_ALLCHECK = 3210;
	static const int IDC_THREADLIST = 3211;
	static const int IDC_TRACELOG = 3212;

	static const int MAX_NUM_OF_THREADS = 1000;

	static VOID CALLBACK ReplaceStatus(HWND, UINT, UINT_PTR, DWORD);
	static LRESULT CALLBACK StkThreadProc(HWND, UINT, WPARAM, LPARAM);
	static BOOL CALLBACK SetStkThreadFontProc(HWND, LPARAM);

private:
	// This instance
	static StkThreadGuiManager* ThisInstance;

	// Handle
	HINSTANCE DlgInstHndl;
	HWND DlgWndHndl;
	HWND ListWndHndl;
	HWND Tracelog;

	WORD IconId;

	StkThreadGuiManager();
	~StkThreadGuiManager();

	int StartThreads(bool);
	int StopThreads(bool);

	void SetStatusOfMasterCheckBox();
	void ReplaceStatus(int);
	void ReplaceStatus(int, int);
	void RefreshLog(bool);
	void AddRecordToListView(int, wchar_t*, wchar_t*);
	void DeleteRecordFromListView(int);
	int InitListView();

public:
	// Get this instance
	static StkThreadGuiManager* GetInstance();
	int ShowStkThreadController(HWND, int, wchar_t*);
	void StartAllThreads();
	void StopAllThreads();
	void AddStkThreadForGui(int, wchar_t*, wchar_t*, void*, void*, void*, void*, void*);
	void DeleteStkThreadForGui(int);
};
