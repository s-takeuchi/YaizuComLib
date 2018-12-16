#pragma once
#include <windows.h>

class StkSocketMemoryLeak
{
private:
	static DWORD WINAPI TestThreadForSend(LPVOID);
	static DWORD WINAPI TestThreadForRecv(LPVOID);
	static DWORD WINAPI TestThreadForSendUdp(LPVOID);
	static DWORD WINAPI TestThreadForRecvUdp(LPVOID);

public:
	void CheckLeakTcp();
	void CheckLeakUdp();
};
