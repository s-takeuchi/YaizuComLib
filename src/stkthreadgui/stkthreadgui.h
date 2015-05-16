#pragma once
#include <windows.h>

int ShowStkThreadController(HWND, int, TCHAR*);
void AddStkThreadForGui(int, TCHAR*, TCHAR*, void*, void*, void*, void*, void*);
void DeleteStkThreadForGui(int);
void StartAllStkThreads();
void StopAllStkThreads();

void ClearStkThreadLog();
void AddStkThreadLog(TCHAR*);
void AddStkThreadLogWithThreadInfo(TCHAR*, TCHAR*);
void GetStkThreadLog(TCHAR*, int);
int GetStkThreadLogUpdateVersion();
int GetStkThreadLogMaxSize();
int GetStkThreadLogSize();
void ChangeStkThreadLogSize(int);
