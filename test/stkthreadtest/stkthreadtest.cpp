#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <Psapi.h>
#include "..\..\src\stkthread\stkthread.h"

int ProcCount = 0;

int ElemStkThreadInit(int Id)
{
	return 0;
}

int ElemStkThreadFinal(int Id)
{
	return 0;
}

int ElemStkThreadMain(int Id)
{
	int Total = 0;
	for (int Loop = 0; Loop < 100000; Loop++) {
		Total = Total * Loop;
	}
	return 0;
}

int ElemStkThreadStart(int Id)
{
	return 0;
}

int ElemStkThreadStop(int Id)
{
	return 0;
}

void ProcBefore()
{
	ProcCount++;
}

void ProcAfter()
{
	ProcCount--;
}

int GetUsedMemorySizeOfCurrentProcess()
{
	DWORD dwProcessID = GetCurrentProcessId();
	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc = { 0 };

	long Size;
	if ((hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessID)) != NULL) {
		if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
			Size = pmc.WorkingSetSize;
		}
	}
	CloseHandle(hProcess);
	return Size;
}

int GetMaxUsedMemorySizeOfCurrentProcess()
{
	DWORD dwProcessID = GetCurrentProcessId();
	HANDLE hProcess;
	PROCESS_MEMORY_COUNTERS pmc = { 0 };

	long Size;
	if ((hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessID)) != NULL) {
		if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
			Size = pmc.PeakWorkingSetSize;
		}
	}
	CloseHandle(hProcess);
	return Size;
}

int MemoryLeakChecking1()
{
	printf("Checks memory leak between AddStkThread and DeleteStkThread...");
	long MaxMem[30];
	for (int CreationLoop = 0; CreationLoop < 30; CreationLoop++) {
		TCHAR BufName[32];
		for (int Loop = 0; Loop < MAX_NUM_OF_STKTHREADS; Loop++) {
			wsprintf(BufName, _T("person#%04d"), Loop);
			AddStkThread(Loop, BufName, _T("Dummy thread"), ElemStkThreadInit, ElemStkThreadFinal, ElemStkThreadMain, ElemStkThreadStart, ElemStkThreadStop);
		}
		while (GetNumOfStkThread() >= 1) {
			int TargetId = GetStkThreadIdByIndex(0);
			DeleteStkThread(TargetId);
		}
		MaxMem[CreationLoop] = GetUsedMemorySizeOfCurrentProcess();
	}
	if (MaxMem[0] < MaxMem[3] &&
		MaxMem[3] < MaxMem[6] &&
		MaxMem[6] < MaxMem[9] &&
		MaxMem[9] < MaxMem[12] &&
		MaxMem[12] < MaxMem[15] &&
		MaxMem[15] < MaxMem[18] &&
		MaxMem[18] < MaxMem[21] &&
		MaxMem[21] < MaxMem[24] &&
		MaxMem[24] < MaxMem[27]) {
		printf("[NG] : %d %d %d %d %d %d %d %d %d\r\n", MaxMem[0], MaxMem[3], MaxMem[6], MaxMem[9], MaxMem[12], MaxMem[15], MaxMem[18], MaxMem[21], MaxMem[24]);
		return -1;
	}
	printf("[OK] : %d %d %d %d %d %d %d %d %d\r\n", MaxMem[0], MaxMem[3], MaxMem[6], MaxMem[9], MaxMem[12], MaxMem[15], MaxMem[18], MaxMem[21], MaxMem[24]);
	return 0;
}

int MemoryLeakChecking2()
{
	printf("Checks memory leak between StartStkThread and StopStkThread...");
	long MaxMem[20];
	TCHAR BufName[32];
	for (int Loop = 0; Loop < MAX_NUM_OF_STKTHREADS; Loop++) {
		wsprintf(BufName, _T("person#%04d"), Loop);
		AddStkThread(Loop, BufName, _T("Dummy thread"), ElemStkThreadInit, ElemStkThreadFinal, ElemStkThreadMain, ElemStkThreadStart, ElemStkThreadStop);
	}

	for (int CreationLoop = 0; CreationLoop < 20; CreationLoop++) {
		int Ths[MAX_NUM_OF_STKTHREADS];
		for (int Loop = 0; Loop < MAX_NUM_OF_STKTHREADS; Loop++) {
			Ths[Loop] = Loop;
		}
		StartSpecifiedStkThreads(Ths, 1000);
		Sleep(1000);
		StopSpecifiedStkThreads(Ths, 1000);
		MaxMem[CreationLoop] = GetUsedMemorySizeOfCurrentProcess();
	}

	while (GetNumOfStkThread() >= 1) {
		int TargetId = GetStkThreadIdByIndex(0);
		DeleteStkThread(TargetId);
	}
	if (MaxMem[0] < MaxMem[4] &&
		MaxMem[4] < MaxMem[8] &&
		MaxMem[8] < MaxMem[12] &&
		MaxMem[12] < MaxMem[16] &&
		MaxMem[16] < MaxMem[20]) {
		printf("[NG] : %d %d %d %d %d \r\n", MaxMem[0], MaxMem[4], MaxMem[8], MaxMem[12], MaxMem[16]);
		return -1;
	}
	printf("[OK] : %d %d %d %d %d \r\n", MaxMem[0], MaxMem[4], MaxMem[8], MaxMem[12], MaxMem[16]);
	return 0;
}

int Create1000Threads()
{
	//////////////////////////////
	printf("Checks whether number of current threads is 0 ...");
	if (GetNumOfStkThread() != 0) {
		printf("[NG] %d \r\n", GetNumOfStkThread());
		return -1;
	}
	printf("[OK]\r\n");
	//////////////////////////////
	printf("Checks whether number of running threads is 0 ...");
	if (GetNumOfRunStkThread() != 0) {
		printf("[NG] %d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	printf("[OK]\r\n");

	//////////////////////////////
	printf("Creates 1000 threads...");
	TCHAR BufName[32];
	for (int Loop = 0; Loop < MAX_NUM_OF_STKTHREADS; Loop++) {
		wsprintf(BufName, _T("person#%04d"), Loop);
		AddStkThread(Loop, BufName, _T("Dummy thread"), ElemStkThreadInit, ElemStkThreadFinal, ElemStkThreadMain, ElemStkThreadStart, ElemStkThreadStop);
	}
	if (GetNumOfStkThread() != 1000) {
		printf("[NG] NumOfStkThread=%d \r\n", GetNumOfStkThread());
		return -1;
	}
	if (GetNumOfRunStkThread() != 0) {
		printf("[NG] NumOfRunStkThread=%d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	printf("[OK] (NumOfStkThread=%d)\r\n", GetNumOfStkThread());

	//////////////////////////////
	printf("Starts 1000 threads...");
	int Ths[MAX_NUM_OF_STKTHREADS];
	for (int Loop = 0; Loop < MAX_NUM_OF_STKTHREADS; Loop++) {
		Ths[Loop] = Loop;
	}
	StartSpecifiedStkThreads(Ths, 1000);
	Sleep(2000);
	if (GetNumOfRunStkThread() != 1000) {
		printf("[NG] NumOfRunStkThread=%d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	printf("[OK] (NumOfRunStkThread=%d)\r\n", GetNumOfRunStkThread());

	//////////////////////////////
	printf("Stops 1000 threads...");
	StopSpecifiedStkThreads(Ths, 1000);
	Sleep(2000);
	if (GetNumOfRunStkThread() != 0) {
		printf("[NG] NumOfRunStkThread=%d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	printf("[OK] (NumOfRunStkThread=%d)\r\n", GetNumOfRunStkThread());

	//////////////////////////////
	printf("Check Number of threads...");
	int TmpIds[MAX_NUM_OF_STKTHREADS];
	int NumOfIds = GetStkThreadIds(TmpIds);
	if (NumOfIds != 1000) {
		printf("[NG] NumOfIds=%d \r\n", NumOfIds);
		return -1;
	}
	for (int Loop = 0; Loop < NumOfIds; Loop++) {
		if (TmpIds[Loop] < 0 || TmpIds[Loop] >= 1000) {
			printf("[NG] Invalid ID[%d] = %d \r\n", Loop, TmpIds[Loop]);
			return -1;
		}
	}
	printf("[OK] (NumOfIds=%d)\r\n", NumOfIds);

	//////////////////////////////
	printf("Starts all threads...");
	StartAllOfStkThreads();
	Sleep(2000);
	if (GetNumOfRunStkThread() != 1000) {
		printf("[NG] NumOfRunStkThread=%d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	printf("[OK] (NumOfRunStkThread=%d)\r\n", GetNumOfRunStkThread());

	//////////////////////////////
	printf("Stops all threads...");
	StopAllOfStkThreads();
	Sleep(2000);
	if (GetNumOfRunStkThread() != 0) {
		printf("[NG] NumOfRunStkThread=%d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	printf("[OK] (NumOfRunStkThread=%d)\r\n", GetNumOfRunStkThread());

	//////////////////////////////
	printf("Deletes 1000 threads...");
	while (GetNumOfStkThread() >= 1) {
		int TargetId = GetStkThreadIdByIndex(0);
		DeleteStkThread(TargetId);
	}
	if (GetNumOfStkThread() != 0) {
		printf("[NG] %d \r\n", GetNumOfStkThread());
		return -1;
	}
	printf("[OK] (NumOfStkThread=%d)\r\n", GetNumOfStkThread());

	return 0;
}

int TestProcBeforeAfterThreadOperation()
{
	TCHAR BufName[32];
	for (int Loop = 0; Loop < MAX_NUM_OF_STKTHREADS; Loop++) {
		wsprintf(BufName, _T("person#%04d"), Loop);
		AddStkThread(Loop, BufName, _T("Dummy thread"), ElemStkThreadInit, ElemStkThreadFinal, ElemStkThreadMain, ElemStkThreadStart, ElemStkThreadStop);
	}

	ProcCount = 0;
	SetProcAfterLastStkThreadStops(ProcAfter);
	SetProcBeforeFirstStkThreadStarts(ProcBefore);
	int Ths[10];
	for (int Loop = 0; Loop < 10; Loop++) {
		Ths[Loop] = Loop;
	}

	//////////
	printf("Start only one thread. Check ProcBeforeFirstStkThreadStarts is called...");
	StartSpecifiedStkThreads(&Ths[0], 1);
	Sleep(1000);
	if (ProcCount != 1) {
		printf("[NG] ProcCount=%d \r\n", ProcCount);
	}
	if (GetNumOfRunStkThread() != 1) {
		printf("[NG] NumOfRunStkThread=%d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	printf("[OK]\r\n");

	//////////
	printf("Stop only one thread. Check ProcAfterLastStkThreadStops is called...");
	StopSpecifiedStkThreads(&Ths[0], 1);
	Sleep(1000);
	if (ProcCount != 0) {
		printf("[NG] ProcCount=%d \r\n", ProcCount);
	}
	if (GetNumOfRunStkThread() != 0) {
		printf("[NG] NumOfRunStkThread=%d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	printf("[OK]\r\n");

	//////////
	printf("Stop the same thread one more time. But ProcAfterLastStkThreadStops is not called more...");
	StopSpecifiedStkThreads(&Ths[0], 1);
	Sleep(1000);
	if (ProcCount != 0) {
		printf("[NG] ProcCount=%d \r\n", ProcCount);
	}
	if (GetNumOfRunStkThread() != 0) {
		printf("[NG] NumOfRunStkThread=%d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	printf("[OK]\r\n");

	//////////
	printf("Start three threads. Check ProcBeforeFirstStkThreadStarts is called...");
	StartSpecifiedStkThreads(&Ths[0], 3);
	Sleep(1000);
	if (ProcCount != 1) {
		printf("[NG] ProcCount=%d \r\n", ProcCount);
	}
	if (GetNumOfRunStkThread() != 3) {
		printf("[NG] NumOfRunStkThread=%d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	printf("[OK]\r\n");

	//////////
	printf("Start four more threads. But ProcBeforeFirstStkThreadStarts is not called more...");
	StartSpecifiedStkThreads(&Ths[3], 4);
	Sleep(1000);
	if (ProcCount != 1) {
		printf("[NG] ProcCount=%d \r\n", ProcCount);
	}
	if (GetNumOfRunStkThread() != 7) {
		printf("[NG] NumOfRunStkThread=%d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	printf("[OK]\r\n");

	//////////
	printf("Stop two threads. But ProcAfterLastStkThreadStops is not called...");
	StopSpecifiedStkThreads(&Ths[5], 2);
	Sleep(1000);
	if (ProcCount != 1) {
		printf("[NG] ProcCount=%d \r\n", ProcCount);
	}
	if (GetNumOfRunStkThread() != 5) {
		printf("[NG] NumOfRunStkThread=%d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	printf("[OK]\r\n");

	//////////
	printf("Stop three more threads. But ProcAfterLastStkThreadStops is not called...");
	StopSpecifiedStkThreads(&Ths[2], 3);
	Sleep(1000);
	if (ProcCount != 1) {
		printf("[NG] ProcCount=%d \r\n", ProcCount);
	}
	if (GetNumOfRunStkThread() != 2) {
		printf("[NG] NumOfRunStkThread=%d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	printf("[OK]\r\n");

	//////////
	printf("Stop two more threads. Check ProcAfterLastStkThreadStops is called...");
	StopSpecifiedStkThreads(&Ths[0], 2);
	Sleep(1000);
	if (ProcCount != 0) {
		printf("[NG] ProcCount=%d \r\n", ProcCount);
	}
	if (GetNumOfRunStkThread() != 0) {
		printf("[NG] NumOfRunStkThread=%d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	printf("[OK]\r\n");

	while (GetNumOfStkThread() >= 1) {
		int TargetId = GetStkThreadIdByIndex(0);
		DeleteStkThread(TargetId);
	}

	return 0;
}

int DeleteWithRunningThreads()
{
	SetProcAfterLastStkThreadStops(NULL);
	SetProcBeforeFirstStkThreadStarts(NULL);

	TCHAR BufName[32];
	for (int Loop = 0; Loop < 10; Loop++) {
		wsprintf(BufName, _T("person#%04d"), Loop);
		AddStkThread(Loop, BufName, _T("Dummy thread"), ElemStkThreadInit, ElemStkThreadFinal, ElemStkThreadMain, ElemStkThreadStart, ElemStkThreadStop);
	}

	int Ths[10];
	for (int Loop = 0; Loop < 10; Loop++) {
		Ths[Loop] = Loop;
	}

	//////////
	printf("Delete 10 running threads...");
	StartSpecifiedStkThreads(&Ths[0], 10);
	Sleep(1000);
	if (GetNumOfStkThread() != 10 || GetNumOfRunStkThread() != 10) {
		printf("[NG]\r\n");
		return -1;
	}
	while (GetNumOfStkThread() >= 1) {
		int TargetId = GetStkThreadIdByIndex(0);
		DeleteStkThread(TargetId);
	}
	if (GetNumOfStkThread() != 0 || GetNumOfRunStkThread() != 0) {
		printf("[NG]\r\n");
		return -1;
	}
	printf("[OK]\r\n");

	return 0;
}

int AbnormalCase()
{
	SetProcAfterLastStkThreadStops(NULL);
	SetProcBeforeFirstStkThreadStarts(NULL);

	TCHAR Name[MAX_LENGTH_OF_STKTHREAD_NAME];
	TCHAR Desc[MAX_LENGTH_OF_STKTHREAD_DESCRIPTION];

	/////
	printf("Try to add more than 1000 threads. Check the number of added threads is 1000...");
	TCHAR BufName[32];
	for (int Loop = 0; Loop < 1010; Loop++) {
		wsprintf(BufName, _T("person#%04d"), Loop);
		AddStkThread(Loop, BufName, _T("Dummy thread"), ElemStkThreadInit, ElemStkThreadFinal, ElemStkThreadMain, ElemStkThreadStart, ElemStkThreadStop);
	}
	if (GetNumOfStkThread() != 1000) {
		printf("[NG] NumOfStkThread=%d \r\n", GetNumOfStkThread());
		return -1;
	}
	printf("[OK]\r\n");

	/////
	printf("Delete non-existing threads...");
	DeleteStkThread(2000);
	DeleteStkThread(3000);
	if (GetNumOfStkThread() != 1000 || GetNumOfRunStkThread() != 0) {
		printf("[NG]\r\n");
		return -1;
	}
	printf("[OK]\r\n");

	/////
	printf("Delete 1000 running threads...");
	int Ths[1000];
	for (int Loop = 0; Loop < 1000; Loop++) {
		Ths[Loop] = Loop;
	}
	StartSpecifiedStkThreads(&Ths[0], 1000);
	Sleep(1000);
	if (GetNumOfStkThread() != 1000 || GetNumOfRunStkThread() != 1000) {
		printf("[NG]\r\n");
		return -1;
	}
	while (GetNumOfStkThread() >= 1) {
		int TargetId = GetStkThreadIdByIndex(0);
		DeleteStkThread(TargetId);
	}
	if (GetNumOfStkThread() != 0 || GetNumOfRunStkThread() != 0) {
		printf("[NG]\r\n");
		return -1;
	}
	printf("[OK]\r\n");

	/////
	printf("Start non-existing threads...");
	if (StartSpecifiedStkThreads(Ths, 1000) != 0) {
		printf("[NG]\r\n");
		return -1;
	}
	if (GetNumOfStkThread() != 0 || GetNumOfRunStkThread() != 0) {
		printf("[NG]\r\n");
		return -1;
	}
	printf("[OK]\r\n");

	/////
	printf("Stop non-existing threads...");
	if (StopSpecifiedStkThreads(Ths, 1000) != 0) {
		printf("[NG]\r\n");
		return -1;
	}
	if (GetNumOfStkThread() != 0 || GetNumOfRunStkThread() != 0) {
		printf("[NG]\r\n");
		return -1;
	}
	printf("[OK]\r\n");

	/////
	printf("Call GetStkThreadStatus for non-existing threads. -1 is returned...");
	if (GetStkThreadStatus(0) != -1) {
		printf("[NG]\r\n");
		return -1;
	}
	printf("[OK]\r\n");

	/////
	printf("Call GetStkThreadStatusByIndex for non-existing threads. -1 is returned...");
	if (GetStkThreadStatusByIndex(0) != -1) {
		printf("[NG]\r\n");
		return -1;
	}
	printf("[OK]\r\n");

	/////
	printf("Call GetInterval. -1 is returned...");
	if (GetStkThreadInterval(0) != -1) {
		printf("[NG]\r\n");
		return -1;
	}
	printf("[OK]\r\n");

	/////
	printf("Call GetStkThreadIdByIndex. -1 is returned...");
	if (GetStkThreadIdByIndex(0) != -1) {
		printf("[NG]\r\n");
		return -1;
	}
	printf("[OK]\r\n");

	/////
	printf("Call GetStkThreadName. -1 is returned...");
	if (GetStkThreadName(0, Name) != -1) {
		printf("[NG]\r\n");
		return -1;
	}
	printf("[OK]\r\n");

	/////
	printf("Call GetStkThreadNameByIndex. -1 is returned...");
	if (GetStkThreadNameByIndex(0, Name) != -1) {
		printf("[NG]\r\n");
		return -1;
	}
	printf("[OK]\r\n");

	/////
	printf("Call GetStkThreadDescription. -1 is returned...");
	if (GetStkThreadDescription(0, Desc) != -1) {
		printf("[NG]\r\n");
		return -1;
	}
	printf("[OK]\r\n");

	/////
	printf("Call GetStkThreadDescriptionByIndex. -1 is returned...");
	if (GetStkThreadDescriptionByIndex(0, Desc) != -1) {
		printf("[NG]\r\n");
		return -1;
	}
	printf("[OK]\r\n");

	return 0;
}

int NormalCase()
{
	SetProcAfterLastStkThreadStops(NULL);
	SetProcBeforeFirstStkThreadStarts(NULL);

	/////
	TCHAR BufName[32];
	for (int Loop = 0; Loop < 10; Loop++) {
		wsprintf(BufName, _T("person#%04d"), Loop);
		AddStkThread(Loop, BufName, _T("Dummy thread"), ElemStkThreadInit, ElemStkThreadFinal, ElemStkThreadMain, ElemStkThreadStart, ElemStkThreadStop);
	}

	/////
	printf("Check the name of StkThread is correct...");
	TCHAR Name[MAX_LENGTH_OF_STKTHREAD_NAME];
	if (GetStkThreadName(2, Name) != 0) {
		printf("[NG]\r\n");
		return -1;
	}
	if (lstrcmp(Name, _T("person#0002")) != 0) {
		printf("[NG] %S\r\n", Name);
		return -1;
	}
	printf("[OK]\r\n");

	/////
	printf("Check the description of StkThread is correct...");
	TCHAR Desc[MAX_LENGTH_OF_STKTHREAD_DESCRIPTION];
	if (GetStkThreadDescription(4, Desc) != 0) {
		printf("[NG]\r\n");
		return -1;
	}
	if (lstrcmp(Desc, _T("Dummy thread")) != 0) {
		printf("[NG]\r\n");
		return -1;
	}
	printf("[OK]\r\n");

	/////
	printf("Check the interval can be configured...");
	SetStkThreadInterval(0, 100);
	SetStkThreadInterval(1, 200);
	SetStkThreadInterval(2, 300);
	if (GetStkThreadInterval(0) != 100) {
		printf("[NG]\r\n");
		return -1;
	}
	if (GetStkThreadInterval(1) != 200) {
		printf("[NG]\r\n");
		return -1;
	}
	if (GetStkThreadInterval(2) != 300) {
		printf("[NG]\r\n");
		return -1;
	}
	printf("[OK]\r\n");

	return 0;
}

int main(int Argc, char* Argv[])
{
	printf("Test started.\r\n");

	if (Create1000Threads() != 0) {
		return -1;
	}
	if (TestProcBeforeAfterThreadOperation() != 0) {
		return -1;
	}
	if (DeleteWithRunningThreads() != 0) {
		return -1;
	}
	if (AbnormalCase() != 0) {
		return -1;
	}
	if (NormalCase() != 0) {
		return -1;
	}
	if (MemoryLeakChecking1() != 0) {
		return -1;
	}
	if (MemoryLeakChecking2() != 0) {
		return -1;
	}

	printf("Test completed.\r\n");

	return 0;
}
