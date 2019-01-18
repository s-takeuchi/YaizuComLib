#pragma once
#include <mutex>

class StkThreadElement
{
public:
	static const int STKTHREAD_STATUS_READY = 100;
	static const int STKTHREAD_STATUS_STARTING = 101;
	static const int STKTHREAD_STATUS_RUNNING = 102;
	static const int STKTHREAD_STATUS_STOPPING = 103;
	static const int MAX_LENGTH_OF_STKTHREAD_NAME = 32;
	static const int MAX_LENGTH_OF_STKTHREAD_DESCRIPTION = 256;

private:
	bool StartStopFlag;
	int Status;
	int Id;
	wchar_t Name[MAX_LENGTH_OF_STKTHREAD_NAME];
	wchar_t Desc[MAX_LENGTH_OF_STKTHREAD_DESCRIPTION];
	int Interval;

	int StkThreadInit();
	int StkThreadMain();
	int StkThreadFinal();
	int StkThreadStart();
	int StkThreadStop();

	int (*ElemStkThreadInit) (int);
	int (*ElemStkThreadFinal) (int);
	int (*ElemStkThreadMain) (int);
	int (*ElemStkThreadStart) (int);
	int (*ElemStkThreadStop) (int);

	// Upper layer class cannot change the status directly.
	void SetStatus(int);
	// Upper layer class cannot change the ID.
	void SetId(int);

	// Default constructor cannot be called from upper layer class.
	StkThreadElement();

public:
	static int NumOfRunThread;

	StkThreadElement(int, const wchar_t*, const wchar_t*, int(*)(int), int(*)(int), int(*)(int), int(*)(int), int(*)(int));
	virtual ~StkThreadElement();

	bool GetStartStopFlag();
	void SetStartStopFlag(bool);

	int GetStatus();

	int GetId();

	wchar_t* GetName();
	void SetName(wchar_t*);

	wchar_t* GetDescription();
	void SetDescription(wchar_t*);

	int StkThreadLoop();

	int GetInterval();
	void SetInterval(int);

};
