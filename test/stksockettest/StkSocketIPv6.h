#pragma once
#include <windows.h>

class StkSocketIPv6
{
private:
	static DWORD WINAPI TestThreadForSend(LPVOID);
	static DWORD WINAPI TestThreadForRecv(LPVOID);
	static DWORD WINAPI TestThreadForSendUdp(LPVOID);
	static DWORD WINAPI TestThreadForRecvUdp(LPVOID);

	static int ThreadStartCount;
	static int ThreadEndCount;
public:
	void TestIPv6();
	void TestIPv6Udp();
};
