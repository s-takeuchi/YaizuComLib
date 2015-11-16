#pragma once
#include <windows.h>

class StkSocketIPv6
{
private:
	static DWORD WINAPI TestThreadForSend(LPVOID);
	static DWORD WINAPI TestThreadForRecv(LPVOID);

	static int ThreadStartCount;
	static int ThreadEndCount;
public:
	void TestIPv6();
};
