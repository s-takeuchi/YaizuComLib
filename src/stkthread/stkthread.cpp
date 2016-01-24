#include <windows.h>
#include "StkThreadManager.h"

int GetNumOfRunStkThread()
{
	return StkThreadManager::GetInstance()->GetNumOfRunStkThread();
}

int GetNumOfStkThread()
{
	return StkThreadManager::GetInstance()->GetNumOfStkThread();
}

int GetNumOfStartingStkThread()
{
	return StkThreadManager::GetInstance()->GetNumOfStartingStkThread();
}

int GetNumOfStoppingStkThread()
{
	return StkThreadManager::GetInstance()->GetNumOfStoppingStkThread();
}

int GetStkThreadInterval(int TargetId)
{
	return StkThreadManager::GetInstance()->GetStkThreadInterval(TargetId);
}

void SetStkThreadInterval(int TargetId, int Millisec)
{
	StkThreadManager::GetInstance()->SetStkThreadInterval(TargetId, Millisec);
}

void AddStkThread(int Id, TCHAR Name[StkThreadManager::MAX_LENGTH_OF_STKTHREAD_NAME], TCHAR Description[StkThreadManager::MAX_LENGTH_OF_STKTHREAD_DESCRIPTION], void* Init, void* Final, void* Main, void* Start, void* Stop)
{
	StkThreadManager::GetInstance()->AddStkThread(Id, Name, Description, Init, Final, Main, Start, Stop);
}

void DeleteStkThread(int Id)
{
	StkThreadManager::GetInstance()->DeleteStkThread(Id);
}

int GetStkThreadIdByIndex(int Index)
{
	return StkThreadManager::GetInstance()->GetStkThreadIdByIndex(Index);
}

int GetStkThreadIds(int Ids[StkThreadManager::MAX_NUM_OF_STKTHREADS])
{
	return StkThreadManager::GetInstance()->GetStkThreadIds(Ids);
}

int GetStkThreadStatus(int Id)
{
	return StkThreadManager::GetInstance()->GetStkThreadStatus(Id);
}

int GetStkThreadStatusByIndex(int Index)
{
	return StkThreadManager::GetInstance()->GetStkThreadStatusByIndex(Index);
}

int GetStkThreadName(int Id, TCHAR Name[StkThreadManager::MAX_LENGTH_OF_STKTHREAD_NAME])
{
	return StkThreadManager::GetInstance()->GetStkThreadName(Id, Name);
}

int GetStkThreadNameByIndex(int Index, TCHAR Name[StkThreadManager::MAX_LENGTH_OF_STKTHREAD_NAME])
{
	return StkThreadManager::GetInstance()->GetStkThreadNameByIndex(Index, Name);
}

int GetStkThreadDescription(int Id, TCHAR Desc[StkThreadManager::MAX_LENGTH_OF_STKTHREAD_DESCRIPTION])
{
	return StkThreadManager::GetInstance()->GetStkThreadDescription(Id, Desc);
}

int GetStkThreadDescriptionByIndex(int Index, TCHAR Desc[StkThreadManager::MAX_LENGTH_OF_STKTHREAD_DESCRIPTION])
{
	return StkThreadManager::GetInstance()->GetStkThreadDescriptionByIndex(Index, Desc);
}

void SetProcBeforeFirstStkThreadStarts(void* Proc)
{
	StkThreadManager::GetInstance()->SetProcBeforeFirstThreadStarts(Proc);
}

void SetProcAfterLastStkThreadStops(void* Proc)
{
	StkThreadManager::GetInstance()->SetProcAfterLastThreadStops(Proc);
}

int StartSpecifiedStkThreads(int Ids[StkThreadManager::MAX_NUM_OF_STKTHREADS], int NumOfIds)
{
	return StkThreadManager::GetInstance()->StartSpecifiedStkThreads(Ids, NumOfIds);
}

int StopSpecifiedStkThreads(int Ids[StkThreadManager::MAX_NUM_OF_STKTHREADS], int NumOfIds)
{
	return StkThreadManager::GetInstance()->StopSpecifiedStkThreads(Ids, NumOfIds);
}

int StartAllStkThreads()
{
	return StkThreadManager::GetInstance()->StartAllStkThreads();
}

int StopAllStkThreads()
{
	return StkThreadManager::GetInstance()->StopAllStkThreads();
}
