#include "stkthreadgui.h"
#include "StkThreadGuiManager.h"
#include "StkThreadGuiLogging.h"

int ShowStkThreadController(HWND hWnd, int ParentIconId, TCHAR* Title)
{
	return StkThreadGuiManager::GetInstance()->ShowStkThreadController(hWnd, ParentIconId, Title);
}

void AddStkThreadForGui(int Id, TCHAR* Name, TCHAR* Description, void* Init, void* Final, void* Main, void* Start, void* Stop)
{
	StkThreadGuiManager::GetInstance()->AddStkThreadForGui(Id, Name, Description, Init, Final, Main, Start, Stop);
}

void DeleteStkThreadForGui(int Id)
{
	StkThreadGuiManager::GetInstance()->DeleteStkThreadForGui(Id);
}

void StartAllStkThreads()
{
	return StkThreadGuiManager::GetInstance()->StartAllThreads();
}

void StopAllStkThreads()
{
	return StkThreadGuiManager::GetInstance()->StopAllThreads();
}

void ClearStkThreadLog()
{
	StkThreadGuiLogging::GetInstance()->ClearLog();
}

void AddStkThreadLog(TCHAR* Msg)
{
	StkThreadGuiLogging::GetInstance()->AddLog(Msg);
}

void AddStkThreadLogWithThreadInfo(TCHAR* Name, TCHAR* Msg)
{
	StkThreadGuiLogging::GetInstance()->AddLogWithThreadInfo(Name, Msg);
}

void GetStkThreadLog(TCHAR* Out, int Length)
{
	StkThreadGuiLogging::GetInstance()->GetLog(Out, Length);
}

int GetStkThreadLogUpdateVersion()
{
	return StkThreadGuiLogging::GetInstance()->GetLogUpdateVersion();
}

int GetStkThreadLogMaxSize()
{
	return StkThreadGuiLogging::GetInstance()->GetLogMaxSize();
}

int GetStkThreadLogSize()
{
	return StkThreadGuiLogging::GetInstance()->GetLogSize();
}

void ChangeStkThreadLogSize(int Size)
{
	StkThreadGuiLogging::GetInstance()->ChangeLogSize(Size);
}
