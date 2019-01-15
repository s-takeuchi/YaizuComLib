#include "stkthreadgui.h"
#include "StkThreadGuiManager.h"
#include "StkThreadGuiLogging.h"

int ShowStkThreadController(HWND hWnd, int ParentIconId, wchar_t* Title)
{
	return StkThreadGuiManager::GetInstance()->ShowStkThreadController(hWnd, ParentIconId, Title);
}

void AddStkThreadForGui(int Id, wchar_t* Name, wchar_t* Description, int(*Init)(int), int(*Final)(int), int(*Main)(int), int(*Start)(int), int(*Stop)(int))
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

void AddStkThreadLog(wchar_t* Msg)
{
	StkThreadGuiLogging::GetInstance()->AddLog(Msg);
}

void AddStkThreadLogWithThreadInfo(wchar_t* Name, wchar_t* Msg)
{
	StkThreadGuiLogging::GetInstance()->AddLogWithThreadInfo(Name, Msg);
}

void GetStkThreadLog(wchar_t* Out, int Length)
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
