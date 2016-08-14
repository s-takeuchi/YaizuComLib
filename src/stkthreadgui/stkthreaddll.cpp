#include <windows.h>
#include <tchar.h>

#define STKTHREADDLL_MAX_PROC_COUNT 20

__declspec(dllexport) DWORD __stdcall StkThreadDll_AddProcessInfo(TCHAR[32]);
__declspec(dllexport) void __stdcall StkThreadDll_DeleteProcessInfo(DWORD);
__declspec(dllexport) int __stdcall StkThreadDll_GetProcessInfoCount();
__declspec(dllexport) BOOL __stdcall StkThreadDll_GetProcessInfoByIndex(int, int*, TCHAR[32]);
__declspec(dllexport) BOOL __stdcall StkThreadDll_GetProcessInfoByProcId(int, TCHAR[32]);
__declspec(dllexport) void __stdcall StkThreadDll_AddThreadInfo(int, TCHAR[32], TCHAR[256]);
__declspec(dllexport) void __stdcall StkThreadDll_DeleteThreadInfo(int);
__declspec(dllexport) int __stdcall StkThreadDll_GetThreadInfoCount();


#pragma data_seg(".mydata")

	BOOL StkThreadDll_ProcessInfo_Lock = FALSE;
	int StkThreadDll_ProcessInfo_Count = 0;

	TCHAR StkThreadDll_ProcessInfo_Name[STKTHREADDLL_MAX_PROC_COUNT][32];
	DWORD StkThreadDll_ProcessInfo_Id[STKTHREADDLL_MAX_PROC_COUNT];
	DWORD StkThreadDll_ProcessInfo_LastUpdated[STKTHREADDLL_MAX_PROC_COUNT];

	BOOL StkThreadDll_ThreadInfo_Lock = FALSE;
	int StkThreadDll_ThreadInfo_Count = 0;
	DWORD StkThreadDll_ThreadInfo_ProcId[STKTHREADDLL_MAX_PROC_COUNT];
	int StkThreadDll_ThreadInfo_Id[STKTHREADDLL_MAX_PROC_COUNT];
	int StkThreadDll_ThreadInfo_Status[STKTHREADDLL_MAX_PROC_COUNT];
	TCHAR StkThreadDll_ThreadInfo_Name[STKTHREADDLL_MAX_PROC_COUNT][32];
	TCHAR StkThreadDll_ThreadInfo_Desc[STKTHREADDLL_MAX_PROC_COUNT][256];

#pragma data_seg()

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}

// Add process information
// Name [in] : Name of the process
// Return : Process ID
__declspec(dllexport) DWORD __stdcall StkThreadDll_AddProcessInfo(TCHAR Name[32])
{
	while (StkThreadDll_ProcessInfo_Lock) {
		Sleep(1);
	}
	StkThreadDll_ProcessInfo_Lock = TRUE;
	DWORD ProcId = GetCurrentProcessId();
	int Loop = 0;
	for (; Loop < StkThreadDll_ProcessInfo_Count; Loop++) {
		if (StkThreadDll_ProcessInfo_Id[Loop] == ProcId) {
			break;
		}
	}
	if (Loop == StkThreadDll_ProcessInfo_Count) {
		lstrcpy(StkThreadDll_ProcessInfo_Name[Loop], Name);
		StkThreadDll_ProcessInfo_Id[Loop] = ProcId;
		StkThreadDll_ProcessInfo_LastUpdated[Loop] = GetTickCount();
		StkThreadDll_ProcessInfo_Count++;
	}
	StkThreadDll_ProcessInfo_Lock = FALSE;
	return ProcId;
}

// Delete process information
// ProcId [in] : Target process ID
__declspec(dllexport) void __stdcall StkThreadDll_DeleteProcessInfo(DWORD ProcId)
{
	while (StkThreadDll_ProcessInfo_Lock) {
		Sleep(1);
	}
	StkThreadDll_ProcessInfo_Lock = TRUE;
	int Loop = 0;
	for (; Loop < StkThreadDll_ProcessInfo_Count; Loop++) {
		if (StkThreadDll_ProcessInfo_Id[Loop] == ProcId) {
			break;
		}
	}
	if (Loop != StkThreadDll_ProcessInfo_Count) {
		for (; StkThreadDll_ProcessInfo_Count - 1; Loop++) {
			lstrcpy(StkThreadDll_ProcessInfo_Name[Loop], StkThreadDll_ProcessInfo_Name[Loop + 1]);
			StkThreadDll_ProcessInfo_LastUpdated[Loop] = StkThreadDll_ProcessInfo_LastUpdated[Loop + 1];
		}
		StkThreadDll_ProcessInfo_Count--;
	}
	for (int Loop2 = 0; Loop2 < StkThreadDll_ThreadInfo_Count - 1; Loop2++) {
		if (StkThreadDll_ThreadInfo_ProcId[Loop] == ProcId) {
			StkThreadDll_DeleteThreadInfo(StkThreadDll_ThreadInfo_Id[Loop]);
		}
	}
	StkThreadDll_ProcessInfo_Lock = FALSE;
}

// Get number of process information
// Return : Number of process information
__declspec(dllexport) int __stdcall StkThreadDll_GetProcessInfoCount()
{
	return StkThreadDll_ProcessInfo_Count;
}

// Get process information from the specified index
// Index [in] : Index where you want to acquire the information
// Id [out] : Acquired process ID
// Name [out] : Acquired process name
// Return : If the information is acquired, TRUE is returned. Otherwize FALSE is returned.
__declspec(dllexport) BOOL __stdcall StkThreadDll_GetProcessInfoByIndex(int Index, int* Id, TCHAR Name[32])
{
	if (Index >= StkThreadDll_ProcessInfo_Count) {
		*Id = -1;
		lstrcpy(Name, _T(""));
		return FALSE;
	}
	*Id = StkThreadDll_ProcessInfo_Id[Index];
	lstrcpy(Name, StkThreadDll_ProcessInfo_Name[Index]);
	return TRUE;
}

// Get process information from the specified process ID
// ProcId [in] : Process ID where you want to acquire the information
// Name [out] : Acquired process name
// Return : If the information is acquired, TRUE is returned. Otherwize FALSE is returned.
__declspec(dllexport) BOOL __stdcall StkThreadDll_GetProcessInfoByProcId(int ProcId, TCHAR Name[32])
{
	for (int Loop = 0; Loop < StkThreadDll_ProcessInfo_Count; Loop++) {
		if (StkThreadDll_ProcessInfo_Id[Loop] == ProcId) {
			lstrcpy(Name, StkThreadDll_ProcessInfo_Name[Loop]);
			return TRUE;
		}
	}
	lstrcpy(Name, _T(""));
	return FALSE;
}

// Add the thread information associates the current process which calls this API.
// Id [in] : Thread ID (This ID can be decided by caller side of this API)
// Name [in] : Name of the thread
// Desc [in] : Description of the thread
__declspec(dllexport) void __stdcall StkThreadDll_AddThreadInfo(int Id, TCHAR Name[32], TCHAR Desc[256])
{
	while (StkThreadDll_ThreadInfo_Lock) {
		Sleep(1);
	}
	StkThreadDll_ThreadInfo_Lock = TRUE;
	DWORD ProcId = GetCurrentProcessId();
	int Loop = 0;
	for (; Loop < StkThreadDll_ThreadInfo_Count; Loop++) {
		if (StkThreadDll_ThreadInfo_ProcId[Loop] == ProcId &&
			StkThreadDll_ThreadInfo_Id[Loop] == Id) {
			break;
		}
	}
	if (Loop != StkThreadDll_ThreadInfo_Count) {
		StkThreadDll_ThreadInfo_ProcId[Loop] = ProcId;
		StkThreadDll_ThreadInfo_Id[Loop] = Id;
		StkThreadDll_ThreadInfo_Status[Loop] = 0;
		lstrcpy(StkThreadDll_ThreadInfo_Name[Loop], Name);
		lstrcpy(StkThreadDll_ThreadInfo_Desc[Loop], Desc);
		StkThreadDll_ThreadInfo_Count++;
	}
	for (int Loop2 = 0; Loop2 < StkThreadDll_ProcessInfo_Count; Loop2++) {
		if (StkThreadDll_ProcessInfo_Id[Loop2] == ProcId) {
			StkThreadDll_ProcessInfo_LastUpdated[Loop2] = GetTickCount();
		}
	}
	StkThreadDll_ThreadInfo_Lock = FALSE;
}

// Delete the thread information associates the current process which calls this API.
// Id [in] : Thread ID which you want to delete.
__declspec(dllexport) void __stdcall StkThreadDll_DeleteThreadInfo(int Id)
{
	while (StkThreadDll_ThreadInfo_Lock) {
		Sleep(1);
	}
	StkThreadDll_ThreadInfo_Lock = TRUE;
	DWORD ProcId = GetCurrentProcessId();
	int Loop = 0;
	for (; Loop < StkThreadDll_ThreadInfo_Count; Loop++) {
		if (StkThreadDll_ThreadInfo_ProcId[Loop] == ProcId &&
			StkThreadDll_ThreadInfo_Id[Loop] == Id) {
			break;
		}
	}
	if (Loop != StkThreadDll_ThreadInfo_Count) {
		for (; StkThreadDll_ThreadInfo_Count - 1; Loop++) {
			StkThreadDll_ThreadInfo_ProcId[Loop] = StkThreadDll_ThreadInfo_ProcId[Loop + 1];
			StkThreadDll_ThreadInfo_Id[Loop] = StkThreadDll_ThreadInfo_Id[Loop + 1];
			StkThreadDll_ThreadInfo_Status[Loop] = StkThreadDll_ThreadInfo_Status[Loop + 1];
			lstrcpy(StkThreadDll_ThreadInfo_Name[Loop], StkThreadDll_ThreadInfo_Name[Loop + 1]);
			lstrcpy(StkThreadDll_ThreadInfo_Desc[Loop], StkThreadDll_ThreadInfo_Desc[Loop + 1]);
		}
		StkThreadDll_ThreadInfo_Count--;
	}
	StkThreadDll_ThreadInfo_Lock = FALSE;
}

// Get number of thread information
// Return : Number of thread information
__declspec(dllexport) int __stdcall StkThreadDll_GetThreadInfoCount()
{
	return StkThreadDll_ThreadInfo_Count;
}
