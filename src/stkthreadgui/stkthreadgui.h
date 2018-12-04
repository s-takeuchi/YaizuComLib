#pragma once
#include <windows.h>

int ShowStkThreadController(HWND, int, wchar_t*);
void AddStkThreadForGui(int, wchar_t*, wchar_t*, void*, void*, void*, void*, void*);
void DeleteStkThreadForGui(int);
void StartAllStkThreads();
void StopAllStkThreads();

void ClearStkThreadLog();
void AddStkThreadLog(wchar_t*);
void AddStkThreadLogWithThreadInfo(wchar_t*, wchar_t*);
void GetStkThreadLog(wchar_t*, int);
int GetStkThreadLogUpdateVersion();
int GetStkThreadLogMaxSize();
int GetStkThreadLogSize();
void ChangeStkThreadLogSize(int);
