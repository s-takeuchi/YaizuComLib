#pragma once
#include <windows.h>

#define MAX_NUM_OF_STKTHREADS 1000
#define MAX_LENGTH_OF_STKTHREAD_NAME 32
#define MAX_LENGTH_OF_STKTHREAD_DESCRIPTION 256
#define STKTHREAD_STATUS_READY 100
#define STKTHREAD_STATUS_STARTING 101
#define STKTHREAD_STATUS_RUNNING 102
#define STKTHREAD_STATUS_STOPPING 103

int GetNumOfRunStkThread();
int GetNumOfStkThread();
int GetNumOfStartingStkThread();
int GetNumOfStoppingStkThread();

int GetStkThreadInterval(int);
void SetStkThreadInterval(int, int);

void AddStkThread(int, TCHAR[MAX_LENGTH_OF_STKTHREAD_NAME], TCHAR[MAX_LENGTH_OF_STKTHREAD_DESCRIPTION], void*, void*, void*, void*, void*);
void DeleteStkThread(int);

int GetStkThreadIdByIndex(int);
int GetStkThreadIds(int[MAX_NUM_OF_STKTHREADS]);
int GetStkThreadStatus(int);
int GetStkThreadStatusByIndex(int);
int GetStkThreadName(int, TCHAR[MAX_LENGTH_OF_STKTHREAD_NAME]);
int GetStkThreadNameByIndex(int, TCHAR[MAX_LENGTH_OF_STKTHREAD_NAME]);
int GetStkThreadDescription(int, TCHAR[MAX_LENGTH_OF_STKTHREAD_DESCRIPTION]);
int GetStkThreadDescriptionByIndex(int, TCHAR[MAX_LENGTH_OF_STKTHREAD_DESCRIPTION]);

void SetProcBeforeFirstStkThreadStarts(void*);
void SetProcAfterLastStkThreadStops(void*);
int StartSpecifiedStkThreads(int[MAX_NUM_OF_STKTHREADS], int);
int StopSpecifiedStkThreads(int[MAX_NUM_OF_STKTHREADS], int);
int StartAllStkThreads();
int StopAllStkThreads();
