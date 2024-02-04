#include <thread>

#include "../stkpl/StkPl.h"
#include "StkThreadManager.h"

StkThreadManager* StkThreadManager::ThisInstance;


void StkThreadManager::ThreadProc(int Param)
{
	StkThreadManager* StkThMgr = GetInstance();
	for (int RowLoop = 0; RowLoop < StkThMgr->StkThreadElementCount; RowLoop++) {
		if (StkThMgr->StkThreadArray[RowLoop]->GetId() == Param) {
			if (StkThMgr->StkThreadArray[RowLoop]->GetStartStopFlag() == true &&
				StkThMgr->StkThreadArray[RowLoop]->GetStatus() == STKTHREAD_STATUS_STARTING) {
				StkThMgr->StkThreadArray[RowLoop]->StkThreadLoop();
				break;
			}
		}
	}
}

// This method starts the specified threads.
// Ids [in] : IDs which are specified for starting threads.
// NumOfIds [in] : Number of IDs
// Return : Number of running threads when this method is called.
int StkThreadManager::StartSpecifiedStkThreads(int Ids[MAX_NUM_OF_STKTHREADS], int NumOfIds)
{
	static int Index[MAX_NUM_OF_STKTHREADS];
	int NumOfRunThreads = StkThreadElement::NumOfRunThread;
	if (NumOfRunThreads < StkThreadElementCount) {
		CheckAndExecProcBeforeFirstThreadStarts(Ids, NumOfIds);
	}
	if (NumOfRunThreads < StkThreadElementCount) {
		// Change the status STARTING for target thread
		int TargetId[MAX_NUM_OF_STKTHREADS];
		int TargetTh[MAX_NUM_OF_STKTHREADS];
		int TargetCount = 0;
		for (int IdLoop = 0; IdLoop < NumOfIds; IdLoop++) {
			for (int ThLoop = 0; ThLoop < StkThreadElementCount; ThLoop++) {
				if (StkThreadArray[ThLoop]->GetId() == Ids[IdLoop] &&
					StkThreadArray[ThLoop]->GetStatus() == STKTHREAD_STATUS_READY) {
					StkThreadArray[ThLoop]->SetStartStopFlag(true);
					TargetId[TargetCount] = Ids[IdLoop];
					TargetTh[TargetCount] = ThLoop;
					TargetCount++;
				}
			}
		}

		// Start threads which are specified.
		for (int Loop = 0; Loop < TargetCount; Loop++) {
			Index[Loop] = TargetId[Loop];
			std::thread MyThread(ThreadProc, Index[Loop]);
			MyThread.detach();
		}
	}
	return NumOfRunThreads;
}

// This method stops the specified thread.
// Ids [in] : IDs which are specified for stopping thread.
// NumOfIds [in] : Number of IDs
// Return : Number of running threads when this method is called.
int StkThreadManager::StopSpecifiedStkThreads(int Ids[MAX_NUM_OF_STKTHREADS], int NumOfIds)
{
	int NumOfRunThreads = StkThreadElement::NumOfRunThread;
	if (NumOfRunThreads != 0) {
		// Stop threads which are specified.
		for (int IdLoop = 0; IdLoop < NumOfIds; IdLoop++) {
			for (int RowLoop = 0; RowLoop < StkThreadElementCount; RowLoop++) {
				if (StkThreadArray[RowLoop]->GetId() == Ids[IdLoop] &&
					StkThreadArray[RowLoop]->GetStatus() == STKTHREAD_STATUS_RUNNING) {
					StkThreadArray[RowLoop]->SetStartStopFlag(false);
				}
			}
		}
		CheckAndExecProcAfterLastThreadStops(Ids, NumOfIds);
	}
	return NumOfRunThreads;
}

// This method starts all of StkThreads
// Return : Number of running threads before this thread is called
int StkThreadManager::StartAllStkThreads()
{
	int TmpIds[MAX_NUM_OF_STKTHREADS];
	int TmpNumOfIds = GetStkThreadIds(TmpIds);
	return StartSpecifiedStkThreads(TmpIds, TmpNumOfIds);
}

// This method stops all of StkThreads
// Return : Number of running threads before this thread is called
int StkThreadManager::StopAllStkThreads()
{
	int TmpIds[MAX_NUM_OF_STKTHREADS];
	int TmpNumOfIds = GetStkThreadIds(TmpIds);
	return StopSpecifiedStkThreads(TmpIds, TmpNumOfIds);
}

// This method checks the conditions and executes ProcBeforeFirstThreadStarts().
// Ids [in] : IDs which are specified for stopping thread.
// NumOfIds [in] : Number of IDs
void StkThreadManager::CheckAndExecProcBeforeFirstThreadStarts(int Ids[MAX_NUM_OF_STKTHREADS], int NumOfIds)
{
	int TargetIndex = -1;

	// If all of the specified IDs are invalid(Not exist), return from this function.
	bool AllInvalidIds = true;
	for (int IdLoop = 0; IdLoop < NumOfIds; IdLoop++) {
		for (int RowLoop = 0; RowLoop < StkThreadElementCount; RowLoop++) {
			if (StkThreadArray[RowLoop]->GetId() == Ids[IdLoop]) {
				AllInvalidIds = false;
				break;
			}
		}
		if (AllInvalidIds == false) {
			break;
		}
	}
	if (AllInvalidIds) {
		return;
	}

	// "ProcBeforeFirstThreadStarts" is called.
	// Condition 1: Every threads are not running now.
	// Condition 2: All of threads are in STATUS_READY status.
	// Condition 3: "ProcBeforeFirstThreadStarts" has already been presented.
	if (ProcBeforeFirstThreadStarts != NULL && StkThreadElement::NumOfRunThread == 0) { // Checking that no threads are running now.
		bool IsAllReady = true;
		for (int RowLoop = 0; RowLoop < StkThreadElementCount; RowLoop++) {
			// The checking below is necessary because there is a case which the status is starting or stopping but NumOfThread is 0.
			if (StkThreadArray[RowLoop]->GetStatus() != STKTHREAD_STATUS_READY) {
				IsAllReady = false;
				break;
			}
		}
		if (IsAllReady == true) {
			ProcBeforeFirstThreadStarts();
		}
	}
}

// This method checks the conditions and executes ProcAfterLastThreadStops().
// Ids [in] : IDs which are specified for stopping thread.
// NumOfIds [in] : Number of IDs
void StkThreadManager::CheckAndExecProcAfterLastThreadStops(int Ids[MAX_NUM_OF_STKTHREADS], int NumOfIds)
{
	// "ProcAfterLastThreadStops" is called.
	// Condition 1: At least one thread is running now.
	// Condition 2: All threads status is changed into [STATUS_READY or STATUS_STOPPING] to [STATUS_READY]
	// Condition 3: "ProcAfterLastThreadStops" has already been presented.

	// Check whether some threads are running or starting.
	bool FndFlag = true;
	for (int RowLoop = 0; RowLoop < StkThreadElementCount; RowLoop++) {
		if (StkThreadArray[RowLoop]->GetStatus() != STKTHREAD_STATUS_READY &&
			StkThreadArray[RowLoop]->GetStatus() != STKTHREAD_STATUS_STOPPING) {
			FndFlag = false;
			break;
		}
	}

	// If some threads are in the status of running or starting, following code is processed.
	if (FndFlag == true) {
		while (true) {
			bool AllReady = true;
			for (int RowLoop = 0; RowLoop < StkThreadElementCount; RowLoop++) {
				if (StkThreadArray[RowLoop]->GetStatus() != STKTHREAD_STATUS_READY) {
					AllReady = false;
				}
			}
			if (AllReady == true) {
				if (ProcAfterLastThreadStops != NULL) {
					ProcAfterLastThreadStops();
				}
				return;
			} else {
				StkPlSleepMs(100);
			}
		}
	}
}

StkThreadManager::StkThreadManager()
{
	StkThreadElementCount = 0;
	ProcBeforeFirstThreadStarts = (void (*)(void))NULL;
	ProcAfterLastThreadStops = (void (*)(void))NULL;
	for (int Loop = 0; Loop < MAX_NUM_OF_STKTHREADS; Loop++) {
		StkThreadArray[Loop] = NULL;
	}
}

// Destructor
StkThreadManager::~StkThreadManager()
{
}

// Get this instance
StkThreadManager* StkThreadManager::GetInstance()
{
	static int Init = 1;
	if (Init == 1) {
		Init = 0;
		ThisInstance = new StkThreadManager();
	}
	return ThisInstance;
}

// Add StkThreadElement instance to queue and list-view
// [out] Nothing
void StkThreadManager::AddStkThread(int Id, const wchar_t Name[MAX_LENGTH_OF_STKTHREAD_NAME], const wchar_t Description[MAX_LENGTH_OF_STKTHREAD_DESCRIPTION], int(*Init)(int), int(*Final)(int), int(*Main)(int), int(*Start)(int), int(*Stop)(int))
{
	if (StkThreadElementCount >= MAX_NUM_OF_STKTHREADS) {
		return;
	}
	if (StkPlWcsLen(Name) >= MAX_LENGTH_OF_STKTHREAD_NAME || StkPlWcsLen(Description) >= MAX_LENGTH_OF_STKTHREAD_DESCRIPTION) {
		return;
	}
	for (int Loop = 0; Loop < StkThreadElementCount; Loop++) {
		if (Id == StkThreadArray[Loop]->GetId()) {
			return;
		}
	}
	StkThreadElement* st = new StkThreadElement(Id, Name, Description, Init, Final, Main, Start, Stop);
	StkThreadArray[StkThreadElementCount] = st;
	StkThreadElementCount++;
}

// Delete StkThreadElement instance from queue and list-view
// [out] Nothing
void StkThreadManager::DeleteStkThread(int CmpId)
{
	int Loop;
	int LoopDel;
	for (Loop = 0; Loop < StkThreadElementCount; Loop++) {
		StkThreadElement* CurStkThread = StkThreadArray[Loop];
		wchar_t* Name = CurStkThread->GetName();
		if (CmpId == CurStkThread->GetId()) {
			// Stop a thread which is regarding the specified item.
			int Ids[1];
			Ids[0] = CmpId;
			StopSpecifiedStkThreads(Ids, 1);
			while (CurStkThread->GetStatus() != STKTHREAD_STATUS_READY) {
				StkPlSleepMs(1);
			}
			// Delete the thread
			delete CurStkThread;
			for (LoopDel = Loop; LoopDel < StkThreadElementCount - 1; LoopDel++) {
				StkThreadElement* Tmp = StkThreadArray[LoopDel + 1];
				StkThreadArray[LoopDel] = Tmp;
			}
			StkThreadElementCount--;
			return;
		}
	}
}

void StkThreadManager::SetProcBeforeFirstThreadStarts(void(*Proc)())
{
	ProcBeforeFirstThreadStarts = (void (*)(void))Proc;
}

void StkThreadManager::SetProcAfterLastThreadStops(void(*Proc)())
{
	ProcAfterLastThreadStops = (void (*)(void))Proc;
}

// Get number of running threads
// [in] Nothing
// [out] int : number of running threads
int StkThreadManager::GetNumOfRunStkThread()
{
	return StkThreadElement::NumOfRunThread;
}

// Get number of threads
// [in] Nothing
// [out] int : number of threads
int StkThreadManager::GetNumOfStkThread()
{
	return StkThreadElementCount;
}

// Get number of starting threads
// [out] number of starting threads
int StkThreadManager::GetNumOfStartingStkThread()
{
	int Count = 0;
	for (int Loop = 0; Loop < StkThreadElementCount; Loop++) {
		if (StkThreadArray[Loop]->GetStatus() == STKTHREAD_STATUS_STARTING) {
			Count++;
		}
	}
	return Count;
}

// Get number of stopping threads
// [out] number of stopping threads
int StkThreadManager::GetNumOfStoppingStkThread()
{
	int Count = 0;
	for (int Loop = 0; Loop < StkThreadElementCount; Loop++) {
		if (StkThreadArray[Loop]->GetStatus() == STKTHREAD_STATUS_STOPPING) {
			Count++;
		}
	}
	return Count;
}

// Get status of the specified StkThread
// TargetId [in] : ID of the target StkThread
// Return : Status of the specified StkThread. If the target ID is invalid, returns -1.
int StkThreadManager::GetStkThreadStatus(int TargetId)
{
	for (int IdLoop = 0; IdLoop < StkThreadElementCount; IdLoop++) {
		if (StkThreadArray[IdLoop]->GetId() == TargetId) {
			return StkThreadArray[IdLoop]->GetStatus();
		}
	}
	return -1;
}

// Get status of the specified index of StkThread.
// TargetIndex [in] : Index of the target StkThread
// Return : Status of the specified index of StkThread. If the target index is invalid, returns -1.
int StkThreadManager::GetStkThreadStatusByIndex(int TargetIndex)
{
	if (TargetIndex < 0 || TargetIndex >= StkThreadElementCount) {
		return -1;
	}
	return StkThreadArray[TargetIndex]->GetStatus();
}

// Get ID of the specified index of StkThread.
// Index [in] : Index of the target StkThread
// Return : ID of the specified index of StkThread. If the target index is invalid, returns -1.
int StkThreadManager::GetStkThreadIdByIndex(int Index)
{
	if (Index < 0 || Index >= StkThreadElementCount) {
		return -1;
	}
	return StkThreadArray[Index]->GetId();
}

// Get all registered IDs.
// Ids [out] : Acquired IDs
// Return : Number of acquired IDs
int StkThreadManager::GetStkThreadIds(int Ids[MAX_NUM_OF_STKTHREADS])
{
	for (int Loop = 0; Loop < StkThreadElementCount; Loop++) {
		Ids[Loop] = StkThreadArray[Loop]->GetId();
	}
	return StkThreadElementCount;
}

// Get name of the specified StkThread.
// TargetId [in] : ID of the target StkThread
// Name [out] : Acquired name of the target StkThread. Before you call this method you need to prepare Name[MAX_LENGTH_OF_NAME] on your caller method.
// Return : If name can be acquired, 0 is returned. Otherwise -1 is returned.
int StkThreadManager::GetStkThreadName(int TargetId, wchar_t Name[MAX_LENGTH_OF_STKTHREAD_NAME])
{
	for (int IdLoop = 0; IdLoop < StkThreadElementCount; IdLoop++) {
		if (StkThreadArray[IdLoop]->GetId() == TargetId) {
			StkPlWcsCpy(Name, MAX_LENGTH_OF_STKTHREAD_NAME, StkThreadArray[IdLoop]->GetName());
			return 0;
		}
	}
	return -1;
}

// Get name of the specified index of StkThread.
// TargetIndex [in] : Index of the target StkThread. Before you call this method you need to prepare Name[MAX_LENGTH_OF_NAME] on your caller method.
// Name [out] : Acquired name of the target StkThread
// Return : If name can be acquired, 0 is returned. Otherwise -1 is returned.
int StkThreadManager::GetStkThreadNameByIndex(int TargetIndex, wchar_t Name[MAX_LENGTH_OF_STKTHREAD_NAME])
{
	if (TargetIndex < 0 || TargetIndex >= StkThreadElementCount) {
		return -1;
	}
	StkPlWcsCpy(Name, MAX_LENGTH_OF_STKTHREAD_NAME, StkThreadArray[TargetIndex]->GetName());
	return 0;
}

// Get description of the specified StkThread.
// TargetId [in] : ID of the target StkThread
// Desc [out] : Acquired description of the target StkThread. Before you call this method you need to prepare Desc[MAX_LENGTH_OF_DESCRIPTION] on your caller method.
// Return : If description can be acquired, 0 is returned. Otherwise -1 is returned.
int StkThreadManager::GetStkThreadDescription(int TargetId, wchar_t Desc[MAX_LENGTH_OF_STKTHREAD_DESCRIPTION])
{
	for (int IdLoop = 0; IdLoop < StkThreadElementCount; IdLoop++) {
		if (StkThreadArray[IdLoop]->GetId() == TargetId) {
			StkPlWcsCpy(Desc, MAX_LENGTH_OF_STKTHREAD_DESCRIPTION, StkThreadArray[IdLoop]->GetDescription());
			return 0;
		}
	}
	return -1;
}

// Get description of the specified index of StkThread.
// TargetIndex [in] : Index of the target StkThread. Before you call this method you need to prepare Desc[MAX_LENGTH_OF_DESCRIPTION] on your caller method.
// Desc [out] : Acquired description of the target StkThread
// Return : If description can be acquired, 0 is returned. Otherwise -1 is returned.
int StkThreadManager::GetStkThreadDescriptionByIndex(int TargetIndex, wchar_t Desc[MAX_LENGTH_OF_STKTHREAD_DESCRIPTION])
{
	if (TargetIndex < 0 || TargetIndex >= StkThreadElementCount) {
		return -1;
	}
	StkPlWcsCpy(Desc, MAX_LENGTH_OF_STKTHREAD_DESCRIPTION, StkThreadArray[TargetIndex]->GetDescription());
	return 0;
}

// Get StartStopFlag of the specified StkThread.
// TargetId [in] : ID of the target StkThread
// Return : Flag (If the specified ID is invalid, false is returned.)
bool StkThreadManager::GetStkThreadStartStopFlag(int TargetId)
{
	for (int IdLoop = 0; IdLoop < StkThreadElementCount; IdLoop++) {
		if (StkThreadArray[IdLoop]->GetId() == TargetId) {
			return StkThreadArray[IdLoop]->GetStartStopFlag();
		}
	}
	return false;
}

// Get StartStopFlag of the specified index of StkThread.
// TargetIndex [in] : Index of the target StkThread.
// Return : Flag (If the specified index is invalid, false is returned.)
bool StkThreadManager::GetStkThreadStartStopFlagByIndex(int TargetIndex)
{
	if (TargetIndex < 0 || TargetIndex >= StkThreadElementCount) {
		return false;
	}
	return StkThreadArray[TargetIndex]->GetStartStopFlag();
}

// Get wait time (in millisecond) for executing ElemStkThreadMain method.
// TargetId [in] : ID of the target StkThread
// Return : Wait time in millisecond. If the specified ID is invalid, -1 is returned.
int StkThreadManager::GetStkThreadInterval(int TargetId)
{
	for (int IdLoop = 0; IdLoop < StkThreadElementCount; IdLoop++) {
		if (StkThreadArray[IdLoop]->GetId() == TargetId) {
			return StkThreadArray[IdLoop]->GetInterval();
		}
	}
	return -1;
}

// Set wait time (in millisecond) for executing ElemStkThreadMain method.
// TargetId [in] : ID of the target StkThread
// Millisec : Wait time in millisecond.
void StkThreadManager::SetStkThreadInterval(int TargetId, int Millisec)
{
	for (int IdLoop = 0; IdLoop < StkThreadElementCount; IdLoop++) {
		if (StkThreadArray[IdLoop]->GetId() == TargetId) {
			return StkThreadArray[IdLoop]->SetInterval(Millisec);
		}
	}
}
