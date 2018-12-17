#include <windows.h>
#include <shlwapi.h>
#include "StkThreadElement.h"

int StkThreadElement::NumOfRunThread;
CRITICAL_SECTION StkThreadElement::CriticalSection;


StkThreadElement::StkThreadElement()
{
}

StkThreadElement::StkThreadElement(int IDen, wchar_t* Na, wchar_t* De, void* Init, void* Final, void* Main, void* Start, void* Stop)
{
	static bool OneTimeInit = true;
	if (OneTimeInit == true) {
		OneTimeInit = false;
		NumOfRunThread = 0;
		InitializeCriticalSection(&CriticalSection);
	}

	StartStopFlag = false;
	Status = STKTHREAD_STATUS_READY;
	Id = IDen;
	StrCpyN(Name, Na, MAX_LENGTH_OF_STKTHREAD_NAME);
	StrCpyN(Desc, De, MAX_LENGTH_OF_STKTHREAD_DESCRIPTION);
	ElemStkThreadInit = (int (*)(int))Init;
	ElemStkThreadFinal = (int (*)(int))Final;
	ElemStkThreadMain = (int (*)(int))Main;
	ElemStkThreadStart = (int (*)(int))Start;
	ElemStkThreadStop = (int (*)(int))Stop;
	Interval = 30;
}

StkThreadElement::~StkThreadElement()
{
}

bool StkThreadElement::GetStartStopFlag()
{
	return StartStopFlag;
}

void StkThreadElement::SetStartStopFlag(bool Flag)
{
	StartStopFlag = Flag;
	if (StartStopFlag == true) {
		SetStatus(StkThreadElement::STKTHREAD_STATUS_STARTING);
		StkThreadStart();
	} else {
		SetStatus(StkThreadElement::STKTHREAD_STATUS_STOPPING);
		StkThreadStop();
	}
}

int StkThreadElement::GetStatus()
{
	return Status;
}

void StkThreadElement::SetStatus(int St)
{
	Status = St;
}

int StkThreadElement::GetId()
{
	return Id;
}

void StkThreadElement::SetId(int Iden)
{
	Id = Iden;
}

wchar_t* StkThreadElement::GetName()
{
	return Name;
}

void StkThreadElement::SetName(wchar_t* Na)
{
	StrCpyN(Name, Na, MAX_LENGTH_OF_STKTHREAD_NAME);
}

wchar_t* StkThreadElement::GetDescription()
{
	return Desc;
}

void StkThreadElement::SetDescription(wchar_t* De)
{
	StrCpyN(Desc, De, MAX_LENGTH_OF_STKTHREAD_DESCRIPTION);
}

int StkThreadElement::StkThreadLoop()
{
	EnterCriticalSection(&CriticalSection);
	NumOfRunThread++;
	StkThreadInit();
	SetStatus(STKTHREAD_STATUS_RUNNING);
	LeaveCriticalSection(&CriticalSection);
	Sleep(1000); // 暫定修正StkFwのスレッド同時実行時にInit処理が終わる前にMainが実行されてしまうことへの対策

	while (true) {
		StkThreadMain();
		if (StartStopFlag == false) {
			break;
		}
		Sleep(Interval);
	}

	EnterCriticalSection(&CriticalSection);
	StkThreadFinal();
	SetStatus(STKTHREAD_STATUS_READY);
	NumOfRunThread--;
	LeaveCriticalSection(&CriticalSection);

	return 0;
}

int StkThreadElement::StkThreadInit()
{
	if (ElemStkThreadInit != NULL) {
		return ElemStkThreadInit(Id);
	}
	return 0;
}

int StkThreadElement::StkThreadFinal()
{
	if (ElemStkThreadFinal != NULL) {
		return ElemStkThreadFinal(Id);
	}
	return 0;
}

int StkThreadElement::StkThreadMain()
{
	if (ElemStkThreadMain != NULL) {
		return ElemStkThreadMain(Id);
	}
	return 0;
}

int StkThreadElement::StkThreadStart()
{
	if (ElemStkThreadStart != NULL) {
		return ElemStkThreadStart(Id);
	}
	return 0;
}

int StkThreadElement::StkThreadStop()
{
	if (ElemStkThreadStop != NULL) {
		return ElemStkThreadStop(Id);
	}
	return 0;
}

int StkThreadElement::GetInterval()
{
	return Interval;
}

void StkThreadElement::SetInterval(int Millisec)
{
	if (Millisec < 0) {
		Interval = 0;
	} else if (Millisec > 60000) {
		Interval = 60000;
	} else {
		Interval = Millisec;
	}
}
