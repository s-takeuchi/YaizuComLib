#include <windows.h>

class StkThreadElement
{
public:
	static const int STKTHREAD_STATUS_READY = 100;
	static const int STKTHREAD_STATUS_STARTING = 101;
	static const int STKTHREAD_STATUS_RUNNING = 102;
	static const int STKTHREAD_STATUS_STOPPING = 104;
	static const int MAX_LENGTH_OF_STKTHREAD_NAME = 32;
	static const int MAX_LENGTH_OF_STKTHREAD_DESCRIPTION = 256;

private:
	BOOL StartStopFlag;
	int Status;
	int Id;
	TCHAR Name[MAX_LENGTH_OF_STKTHREAD_NAME];
	TCHAR Desc[MAX_LENGTH_OF_STKTHREAD_DESCRIPTION];
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

	static CRITICAL_SECTION CriticalSection;

public:
	static int NumOfRunThread;

	DWORD ThId;

	StkThreadElement(int, TCHAR*, TCHAR*, void*, void*, void*, void*, void*);
	virtual ~StkThreadElement();

	BOOL GetStartStopFlag();
	void SetStartStopFlag(BOOL);

	int GetStatus();

	int GetId();

	TCHAR* GetName();
	void SetName(TCHAR*);

	TCHAR* GetDescription();
	void SetDescription(TCHAR*);

	int StkThreadLoop();

	int GetInterval();
	void SetInterval(int);

};
