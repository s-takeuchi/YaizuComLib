#include "../../src/StkPl.h"
#include "../../src/stkthread/stkthread.h"

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

int MemoryLeakChecking1()
{
	StkPlPrintf("Checks memory leak between AddStkThread and DeleteStkThread...");
	long MaxMem[30];
	for (int CreationLoop = 0; CreationLoop < 30; CreationLoop++) {
		wchar_t BufName[32];
		for (int Loop = 0; Loop < MAX_NUM_OF_STKTHREADS; Loop++) {
			StkPlSwPrintf(BufName, 32, L"person#%04d", Loop);
			AddStkThread(Loop, BufName, L"Dummy thread", ElemStkThreadInit, ElemStkThreadFinal, ElemStkThreadMain, ElemStkThreadStart, ElemStkThreadStop);
		}
		while (GetNumOfStkThread() >= 1) {
			int TargetId = GetStkThreadIdByIndex(0);
			DeleteStkThread(TargetId);
		}
		MaxMem[CreationLoop] = StkPlGetUsedMemorySizeOfCurrentProcess();
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
		StkPlPrintf("[NG] : %d %d %d %d %d %d %d %d %d\r\n", MaxMem[0], MaxMem[3], MaxMem[6], MaxMem[9], MaxMem[12], MaxMem[15], MaxMem[18], MaxMem[21], MaxMem[24]);
		return -1;
	}
	StkPlPrintf("[OK] : %d %d %d %d %d %d %d %d %d\r\n", MaxMem[0], MaxMem[3], MaxMem[6], MaxMem[9], MaxMem[12], MaxMem[15], MaxMem[18], MaxMem[21], MaxMem[24]);
	return 0;
}

int MemoryLeakChecking2()
{
	StkPlPrintf("Checks memory leak between StartStkThread and StopStkThread...");
	long MaxMem[20];
	wchar_t BufName[32];
	for (int Loop = 0; Loop < MAX_NUM_OF_STKTHREADS; Loop++) {
		StkPlSwPrintf(BufName, 32, L"person#%04d", Loop);
		AddStkThread(Loop, BufName, L"Dummy thread", ElemStkThreadInit, ElemStkThreadFinal, ElemStkThreadMain, ElemStkThreadStart, ElemStkThreadStop);
	}

	for (int CreationLoop = 0; CreationLoop < 20; CreationLoop++) {
		int Ths[MAX_NUM_OF_STKTHREADS];
		for (int Loop = 0; Loop < MAX_NUM_OF_STKTHREADS; Loop++) {
			Ths[Loop] = Loop;
		}
		StartSpecifiedStkThreads(Ths, 1000);
		StkPlSleepMs(1000);
		StopSpecifiedStkThreads(Ths, 1000);
		MaxMem[CreationLoop] = StkPlGetUsedMemorySizeOfCurrentProcess();
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
		StkPlPrintf("[NG] : %d %d %d %d %d \r\n", MaxMem[0], MaxMem[4], MaxMem[8], MaxMem[12], MaxMem[16]);
		return -1;
	}
	StkPlPrintf("[OK] : %d %d %d %d %d \r\n", MaxMem[0], MaxMem[4], MaxMem[8], MaxMem[12], MaxMem[16]);
	return 0;
}

int Create1000Threads()
{
	//////////////////////////////
	StkPlPrintf("Checks whether number of current threads is 0 ...");
	if (GetNumOfStkThread() != 0) {
		StkPlPrintf("[NG] %d \r\n", GetNumOfStkThread());
		return -1;
	}
	StkPlPrintf("[OK]\r\n");
	//////////////////////////////
	StkPlPrintf("Checks whether number of running threads is 0 ...");
	if (GetNumOfRunStkThread() != 0) {
		StkPlPrintf("[NG] %d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	StkPlPrintf("[OK]\r\n");

	//////////////////////////////
	StkPlPrintf("Creates 1000 threads...");
	wchar_t BufName[32];
	for (int Loop = 0; Loop < MAX_NUM_OF_STKTHREADS; Loop++) {
		StkPlSwPrintf(BufName, 32, L"person#%04d", Loop);
		AddStkThread(Loop, BufName, L"Dummy thread", ElemStkThreadInit, ElemStkThreadFinal, ElemStkThreadMain, ElemStkThreadStart, ElemStkThreadStop);
	}
	if (GetNumOfStkThread() != 1000) {
		StkPlPrintf("[NG] NumOfStkThread=%d \r\n", GetNumOfStkThread());
		return -1;
	}
	if (GetNumOfRunStkThread() != 0) {
		StkPlPrintf("[NG] NumOfRunStkThread=%d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	StkPlPrintf("[OK] (NumOfStkThread=%d)\r\n", GetNumOfStkThread());

	//////////////////////////////
	StkPlPrintf("Starts 1000 threads...");
	int Ths[MAX_NUM_OF_STKTHREADS];
	for (int Loop = 0; Loop < MAX_NUM_OF_STKTHREADS; Loop++) {
		Ths[Loop] = Loop;
	}
	StartSpecifiedStkThreads(Ths, 1000);
	StkPlSleepMs(2000);
	if (GetNumOfRunStkThread() != 1000) {
		StkPlPrintf("[NG] NumOfRunStkThread=%d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	StkPlPrintf("[OK] (NumOfRunStkThread=%d)\r\n", GetNumOfRunStkThread());

	//////////////////////////////
	StkPlPrintf("Stops 1000 threads...");
	StopSpecifiedStkThreads(Ths, 1000);
	StkPlSleepMs(2000);
	if (GetNumOfRunStkThread() != 0) {
		StkPlPrintf("[NG] NumOfRunStkThread=%d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	StkPlPrintf("[OK] (NumOfRunStkThread=%d)\r\n", GetNumOfRunStkThread());

	//////////////////////////////
	StkPlPrintf("Check Number of threads...");
	int TmpIds[MAX_NUM_OF_STKTHREADS];
	int NumOfIds = GetStkThreadIds(TmpIds);
	if (NumOfIds != 1000) {
		StkPlPrintf("[NG] NumOfIds=%d \r\n", NumOfIds);
		return -1;
	}
	for (int Loop = 0; Loop < NumOfIds; Loop++) {
		if (TmpIds[Loop] < 0 || TmpIds[Loop] >= 1000) {
			StkPlPrintf("[NG] Invalid ID[%d] = %d \r\n", Loop, TmpIds[Loop]);
			return -1;
		}
	}
	StkPlPrintf("[OK] (NumOfIds=%d)\r\n", NumOfIds);

	//////////////////////////////
	StkPlPrintf("Starts all threads...");
	StartAllOfStkThreads();
	StkPlSleepMs(2000);
	if (GetNumOfRunStkThread() != 1000) {
		StkPlPrintf("[NG] NumOfRunStkThread=%d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	StkPlPrintf("[OK] (NumOfRunStkThread=%d)\r\n", GetNumOfRunStkThread());

	//////////////////////////////
	StkPlPrintf("Stops all threads...");
	StopAllOfStkThreads();
	StkPlSleepMs(2000);
	if (GetNumOfRunStkThread() != 0) {
		StkPlPrintf("[NG] NumOfRunStkThread=%d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	StkPlPrintf("[OK] (NumOfRunStkThread=%d)\r\n", GetNumOfRunStkThread());

	//////////////////////////////
	StkPlPrintf("Deletes 1000 threads...");
	while (GetNumOfStkThread() >= 1) {
		int TargetId = GetStkThreadIdByIndex(0);
		DeleteStkThread(TargetId);
	}
	if (GetNumOfStkThread() != 0) {
		StkPlPrintf("[NG] %d \r\n", GetNumOfStkThread());
		return -1;
	}
	StkPlPrintf("[OK] (NumOfStkThread=%d)\r\n", GetNumOfStkThread());

	return 0;
}

int TestProcBeforeAfterThreadOperation()
{
	wchar_t BufName[32];
	for (int Loop = 0; Loop < MAX_NUM_OF_STKTHREADS; Loop++) {
		StkPlSwPrintf(BufName, 32, L"person#%04d", Loop);
		AddStkThread(Loop, BufName, L"Dummy thread", ElemStkThreadInit, ElemStkThreadFinal, ElemStkThreadMain, ElemStkThreadStart, ElemStkThreadStop);
	}

	ProcCount = 0;
	SetProcAfterLastStkThreadStops(ProcAfter);
	SetProcBeforeFirstStkThreadStarts(ProcBefore);
	int Ths[10];
	for (int Loop = 0; Loop < 10; Loop++) {
		Ths[Loop] = Loop;
	}

	//////////
	StkPlPrintf("Start only one thread. Check ProcBeforeFirstStkThreadStarts is called...");
	StartSpecifiedStkThreads(&Ths[0], 1);
	StkPlSleepMs(1000);
	if (ProcCount != 1) {
		StkPlPrintf("[NG] ProcCount=%d \r\n", ProcCount);
	}
	if (GetNumOfRunStkThread() != 1) {
		StkPlPrintf("[NG] NumOfRunStkThread=%d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	StkPlPrintf("[OK]\r\n");

	//////////
	StkPlPrintf("Stop only one thread. Check ProcAfterLastStkThreadStops is called...");
	StopSpecifiedStkThreads(&Ths[0], 1);
	StkPlSleepMs(1000);
	if (ProcCount != 0) {
		StkPlPrintf("[NG] ProcCount=%d \r\n", ProcCount);
	}
	if (GetNumOfRunStkThread() != 0) {
		StkPlPrintf("[NG] NumOfRunStkThread=%d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	StkPlPrintf("[OK]\r\n");

	//////////
	StkPlPrintf("Stop the same thread one more time. But ProcAfterLastStkThreadStops is not called more...");
	StopSpecifiedStkThreads(&Ths[0], 1);
	StkPlSleepMs(1000);
	if (ProcCount != 0) {
		StkPlPrintf("[NG] ProcCount=%d \r\n", ProcCount);
	}
	if (GetNumOfRunStkThread() != 0) {
		StkPlPrintf("[NG] NumOfRunStkThread=%d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	StkPlPrintf("[OK]\r\n");

	//////////
	StkPlPrintf("Start three threads. Check ProcBeforeFirstStkThreadStarts is called...");
	StartSpecifiedStkThreads(&Ths[0], 3);
	StkPlSleepMs(1000);
	if (ProcCount != 1) {
		StkPlPrintf("[NG] ProcCount=%d \r\n", ProcCount);
	}
	if (GetNumOfRunStkThread() != 3) {
		StkPlPrintf("[NG] NumOfRunStkThread=%d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	StkPlPrintf("[OK]\r\n");

	//////////
	StkPlPrintf("Start four more threads. But ProcBeforeFirstStkThreadStarts is not called more...");
	StartSpecifiedStkThreads(&Ths[3], 4);
	StkPlSleepMs(1000);
	if (ProcCount != 1) {
		StkPlPrintf("[NG] ProcCount=%d \r\n", ProcCount);
	}
	if (GetNumOfRunStkThread() != 7) {
		StkPlPrintf("[NG] NumOfRunStkThread=%d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	StkPlPrintf("[OK]\r\n");

	//////////
	StkPlPrintf("Stop two threads. But ProcAfterLastStkThreadStops is not called...");
	StopSpecifiedStkThreads(&Ths[5], 2);
	StkPlSleepMs(1000);
	if (ProcCount != 1) {
		StkPlPrintf("[NG] ProcCount=%d \r\n", ProcCount);
	}
	if (GetNumOfRunStkThread() != 5) {
		StkPlPrintf("[NG] NumOfRunStkThread=%d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	StkPlPrintf("[OK]\r\n");

	//////////
	StkPlPrintf("Stop three more threads. But ProcAfterLastStkThreadStops is not called...");
	StopSpecifiedStkThreads(&Ths[2], 3);
	StkPlSleepMs(1000);
	if (ProcCount != 1) {
		StkPlPrintf("[NG] ProcCount=%d \r\n", ProcCount);
	}
	if (GetNumOfRunStkThread() != 2) {
		StkPlPrintf("[NG] NumOfRunStkThread=%d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	StkPlPrintf("[OK]\r\n");

	//////////
	StkPlPrintf("Stop two more threads. Check ProcAfterLastStkThreadStops is called...");
	StopSpecifiedStkThreads(&Ths[0], 2);
	StkPlSleepMs(1000);
	if (ProcCount != 0) {
		StkPlPrintf("[NG] ProcCount=%d \r\n", ProcCount);
	}
	if (GetNumOfRunStkThread() != 0) {
		StkPlPrintf("[NG] NumOfRunStkThread=%d \r\n", GetNumOfRunStkThread());
		return -1;
	}
	StkPlPrintf("[OK]\r\n");

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

	wchar_t BufName[32];
	for (int Loop = 0; Loop < 10; Loop++) {
		StkPlSwPrintf(BufName, 32, L"person#%04d", Loop);
		AddStkThread(Loop, BufName, L"Dummy thread", ElemStkThreadInit, ElemStkThreadFinal, ElemStkThreadMain, ElemStkThreadStart, ElemStkThreadStop);
	}

	int Ths[10];
	for (int Loop = 0; Loop < 10; Loop++) {
		Ths[Loop] = Loop;
	}

	//////////
	StkPlPrintf("Delete 10 running threads...");
	StartSpecifiedStkThreads(&Ths[0], 10);
	StkPlSleepMs(1000);
	if (GetNumOfStkThread() != 10 || GetNumOfRunStkThread() != 10) {
		StkPlPrintf("[NG]\r\n");
		return -1;
	}
	while (GetNumOfStkThread() >= 1) {
		int TargetId = GetStkThreadIdByIndex(0);
		DeleteStkThread(TargetId);
	}
	if (GetNumOfStkThread() != 0 || GetNumOfRunStkThread() != 0) {
		StkPlPrintf("[NG]\r\n");
		return -1;
	}
	StkPlPrintf("[OK]\r\n");

	return 0;
}

int AbnormalCase()
{
	SetProcAfterLastStkThreadStops(NULL);
	SetProcBeforeFirstStkThreadStarts(NULL);

	wchar_t Name[MAX_LENGTH_OF_STKTHREAD_NAME];
	wchar_t Desc[MAX_LENGTH_OF_STKTHREAD_DESCRIPTION];

	/////
	StkPlPrintf("Try to add more than 1000 threads. Check the number of added threads is 1000...");
	wchar_t BufName[32];
	for (int Loop = 0; Loop < 1010; Loop++) {
		StkPlSwPrintf(BufName, 32, L"person#%04d", Loop);
		AddStkThread(Loop, BufName, L"Dummy thread", ElemStkThreadInit, ElemStkThreadFinal, ElemStkThreadMain, ElemStkThreadStart, ElemStkThreadStop);
	}
	if (GetNumOfStkThread() != 1000) {
		StkPlPrintf("[NG] NumOfStkThread=%d \r\n", GetNumOfStkThread());
		return -1;
	}
	StkPlPrintf("[OK]\r\n");

	/////
	StkPlPrintf("Delete non-existing threads...");
	DeleteStkThread(2000);
	DeleteStkThread(3000);
	if (GetNumOfStkThread() != 1000 || GetNumOfRunStkThread() != 0) {
		StkPlPrintf("[NG]\r\n");
		return -1;
	}
	StkPlPrintf("[OK]\r\n");

	/////
	StkPlPrintf("Delete 1000 running threads...");
	int Ths[1000];
	for (int Loop = 0; Loop < 1000; Loop++) {
		Ths[Loop] = Loop;
	}
	StartSpecifiedStkThreads(&Ths[0], 1000);
	StkPlSleepMs(1000);
	if (GetNumOfStkThread() != 1000 || GetNumOfRunStkThread() != 1000) {
		StkPlPrintf("[NG]\r\n");
		return -1;
	}
	while (GetNumOfStkThread() >= 1) {
		int TargetId = GetStkThreadIdByIndex(0);
		DeleteStkThread(TargetId);
	}
	if (GetNumOfStkThread() != 0 || GetNumOfRunStkThread() != 0) {
		StkPlPrintf("[NG]\r\n");
		return -1;
	}
	StkPlPrintf("[OK]\r\n");

	/////
	StkPlPrintf("Start non-existing threads...");
	if (StartSpecifiedStkThreads(Ths, 1000) != 0) {
		StkPlPrintf("[NG]\r\n");
		return -1;
	}
	if (GetNumOfStkThread() != 0 || GetNumOfRunStkThread() != 0) {
		StkPlPrintf("[NG]\r\n");
		return -1;
	}
	StkPlPrintf("[OK]\r\n");

	/////
	StkPlPrintf("Stop non-existing threads...");
	if (StopSpecifiedStkThreads(Ths, 1000) != 0) {
		StkPlPrintf("[NG]\r\n");
		return -1;
	}
	if (GetNumOfStkThread() != 0 || GetNumOfRunStkThread() != 0) {
		StkPlPrintf("[NG]\r\n");
		return -1;
	}
	StkPlPrintf("[OK]\r\n");

	/////
	StkPlPrintf("Call GetStkThreadStatus for non-existing threads. -1 is returned...");
	if (GetStkThreadStatus(0) != -1) {
		StkPlPrintf("[NG]\r\n");
		return -1;
	}
	StkPlPrintf("[OK]\r\n");

	/////
	StkPlPrintf("Call GetStkThreadStatusByIndex for non-existing threads. -1 is returned...");
	if (GetStkThreadStatusByIndex(0) != -1) {
		StkPlPrintf("[NG]\r\n");
		return -1;
	}
	StkPlPrintf("[OK]\r\n");

	/////
	StkPlPrintf("Call GetInterval. -1 is returned...");
	if (GetStkThreadInterval(0) != -1) {
		StkPlPrintf("[NG]\r\n");
		return -1;
	}
	StkPlPrintf("[OK]\r\n");

	/////
	StkPlPrintf("Call GetStkThreadIdByIndex. -1 is returned...");
	if (GetStkThreadIdByIndex(0) != -1) {
		StkPlPrintf("[NG]\r\n");
		return -1;
	}
	StkPlPrintf("[OK]\r\n");

	/////
	StkPlPrintf("Call GetStkThreadName. -1 is returned...");
	if (GetStkThreadName(0, Name) != -1) {
		StkPlPrintf("[NG]\r\n");
		return -1;
	}
	StkPlPrintf("[OK]\r\n");

	/////
	StkPlPrintf("Call GetStkThreadNameByIndex. -1 is returned...");
	if (GetStkThreadNameByIndex(0, Name) != -1) {
		StkPlPrintf("[NG]\r\n");
		return -1;
	}
	StkPlPrintf("[OK]\r\n");

	/////
	StkPlPrintf("Call GetStkThreadDescription. -1 is returned...");
	if (GetStkThreadDescription(0, Desc) != -1) {
		StkPlPrintf("[NG]\r\n");
		return -1;
	}
	StkPlPrintf("[OK]\r\n");

	/////
	StkPlPrintf("Call GetStkThreadDescriptionByIndex. -1 is returned...");
	if (GetStkThreadDescriptionByIndex(0, Desc) != -1) {
		StkPlPrintf("[NG]\r\n");
		return -1;
	}
	StkPlPrintf("[OK]\r\n");

	return 0;
}

int NormalCase()
{
	SetProcAfterLastStkThreadStops(NULL);
	SetProcBeforeFirstStkThreadStarts(NULL);

	/////
	wchar_t BufName[32];
	for (int Loop = 0; Loop < 10; Loop++) {
		StkPlSwPrintf(BufName, 32, L"person#%04d", Loop);
		AddStkThread(Loop, BufName, L"Dummy thread", ElemStkThreadInit, ElemStkThreadFinal, ElemStkThreadMain, ElemStkThreadStart, ElemStkThreadStop);
	}

	/////
	StkPlPrintf("Check the name of StkThread is correct...");
	wchar_t Name[MAX_LENGTH_OF_STKTHREAD_NAME];
	if (GetStkThreadName(2, Name) != 0) {
		StkPlPrintf("[NG]\r\n");
		return -1;
	}
	if (StkPlWcsCmp(Name, L"person#0002") != 0) {
		StkPlPrintf("[NG] %S\r\n", Name);
		return -1;
	}
	StkPlPrintf("[OK]\r\n");

	/////
	StkPlPrintf("Check the description of StkThread is correct...");
	wchar_t Desc[MAX_LENGTH_OF_STKTHREAD_DESCRIPTION];
	if (GetStkThreadDescription(4, Desc) != 0) {
		StkPlPrintf("[NG]\r\n");
		return -1;
	}
	if (StkPlWcsCmp(Desc, L"Dummy thread") != 0) {
		StkPlPrintf("[NG]\r\n");
		return -1;
	}
	StkPlPrintf("[OK]\r\n");

	/////
	StkPlPrintf("Check the interval can be configured...");
	SetStkThreadInterval(0, 100);
	SetStkThreadInterval(1, 200);
	SetStkThreadInterval(2, 300);
	if (GetStkThreadInterval(0) != 100) {
		StkPlPrintf("[NG]\r\n");
		return -1;
	}
	if (GetStkThreadInterval(1) != 200) {
		StkPlPrintf("[NG]\r\n");
		return -1;
	}
	if (GetStkThreadInterval(2) != 300) {
		StkPlPrintf("[NG]\r\n");
		return -1;
	}
	StkPlPrintf("[OK]\r\n");

	return 0;
}

int main(int Argc, char* Argv[])
{
	StkPlPrintf("Test started.\r\n");

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

	StkPlPrintf("Test completed.\r\n");

	return 0;
}
