#include <windows.h>
#define STKTHREADDLL_MAX_PROC_COUNT 20

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

__declspec(dllexport) void __stdcall StkThreadDll_AddProcessInfo(TCHAR Name[32])
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
}

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
	StkThreadDll_ProcessInfo_Lock = FALSE;
}

__declspec(dllexport) int __stdcall StkThreadDll_GetProcessInfoCount()
{
	return StkThreadDll_ProcessInfo_Count;
}

__declspec(dllexport) void __stdcall StkThreadDll_GetProcessInfo(int Index, int* Id, TCHAR Name[32])
{
	*Id = StkThreadDll_ProcessInfo_Id[Index];
	lstrcpy(Name, StkThreadDll_ProcessInfo_Name[Index]);
}

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
	}
	for (int Loop2 = 0; Loop2 < StkThreadDll_ProcessInfo_Count; Loop2++) {
		if (StkThreadDll_ProcessInfo_Id[Loop2] == ProcId) {
			StkThreadDll_ProcessInfo_LastUpdated[Loop2] = GetTickCount();
		}
	}
	StkThreadDll_ThreadInfo_Lock = FALSE;
}

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
