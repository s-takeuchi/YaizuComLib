#pragma once
#include <windows.h>
#include <tchar.h>
#include <commctrl.h>
#include "StkThreadElement.h"

class StkThreadManager
{
public:
	static const int MAX_NUM_OF_STKTHREADS = 1000;
	static const int MAX_LENGTH_OF_STKTHREAD_NAME = StkThreadElement::MAX_LENGTH_OF_STKTHREAD_NAME;
	static const int MAX_LENGTH_OF_STKTHREAD_DESCRIPTION = StkThreadElement::MAX_LENGTH_OF_STKTHREAD_DESCRIPTION;
	static const int STKTHREAD_STATUS_READY = StkThreadElement::STKTHREAD_STATUS_READY;
	static const int STKTHREAD_STATUS_STARTING = StkThreadElement::STKTHREAD_STATUS_STARTING;
	static const int STKTHREAD_STATUS_RUNNING = StkThreadElement::STKTHREAD_STATUS_RUNNING;
	static const int STKTHREAD_STATUS_STOPPING = StkThreadElement::STKTHREAD_STATUS_STOPPING;

private:
	// This instance
	static StkThreadManager* ThisInstance;
	// Number of StkThreadElements in the array
	int StkThreadElementCount;
	// Array of StkThreadElement
	StkThreadElement* StkThreadArray[MAX_NUM_OF_STKTHREADS];
	// Call back function for CreateThread.
	static DWORD WINAPI ThreadProc(LPVOID);

	StkThreadManager();
	~StkThreadManager();

	void (*ProcBeforeFirstThreadStarts)(void);
	void (*ProcAfterLastThreadStops)(void);

	void CheckAndExecProcBeforeFirstThreadStarts(int[MAX_NUM_OF_STKTHREADS], int);
	void CheckAndExecProcAfterLastThreadStops(int[MAX_NUM_OF_STKTHREADS], int);

	BOOL GetStkThreadStartStopFlag(int);
	BOOL GetStkThreadStartStopFlagByIndex(int);

public:
	// Get this instance
	static StkThreadManager* GetInstance();

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

	void SetProcBeforeFirstThreadStarts(void*);
	void SetProcAfterLastThreadStops(void*);
	int StartSpecifiedStkThreads(int[MAX_NUM_OF_STKTHREADS], int);
	int StopSpecifiedStkThreads(int[MAX_NUM_OF_STKTHREADS], int);
	int StartAllStkThreads();
	int StopAllStkThreads();
};
