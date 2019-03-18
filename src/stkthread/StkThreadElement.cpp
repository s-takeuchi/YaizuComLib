#include <mutex>

#include "../stkpl/StkPl.h"
#include "StkThreadElement.h"

int StkThreadElement::NumOfRunThread;

StkThreadElement::StkThreadElement()
{
}

StkThreadElement::StkThreadElement(int IDen, const wchar_t* Na, const wchar_t* De, int(*Init)(int), int(*Final)(int), int(*Main)(int), int(*Start)(int), int(*Stop)(int))
{
	static bool OneTimeInit = true;
	if (OneTimeInit == true) {
		OneTimeInit = false;
		NumOfRunThread = 0;
	}

	StartStopFlag = false;
	Status = STKTHREAD_STATUS_READY;
	Id = IDen;
	StkPlWcsCpy(Name, MAX_LENGTH_OF_STKTHREAD_NAME, Na);
	StkPlWcsCpy(Desc, MAX_LENGTH_OF_STKTHREAD_DESCRIPTION, De);
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
	StkPlWcsCpy(Name, MAX_LENGTH_OF_STKTHREAD_NAME, Na);
}

wchar_t* StkThreadElement::GetDescription()
{
	return Desc;
}

void StkThreadElement::SetDescription(wchar_t* De)
{
	StkPlWcsCpy(Desc, MAX_LENGTH_OF_STKTHREAD_DESCRIPTION, De);
}

int StkThreadElement::StkThreadLoop()
{
	static std::mutex CritSec;

	CritSec.lock();
	NumOfRunThread++;
	StkThreadInit();
	SetStatus(STKTHREAD_STATUS_RUNNING);
	CritSec.unlock();
	StkPlSleepMs(1000); // 暫定修正StkFwのスレッド同時実行時にInit処理が終わる前にMainが実行されてしまうことへの対策

	while (true) {
		StkThreadMain();
		if (StartStopFlag == false) {
			break;
		}
		StkPlSleepMs(Interval);
	}

	CritSec.lock();
	StkThreadFinal();
	SetStatus(STKTHREAD_STATUS_READY);
	NumOfRunThread--;
	CritSec.unlock();

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
