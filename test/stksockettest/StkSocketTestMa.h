#pragma once
#include <windows.h>

class StkSocketTestMa
{
private:
	static DWORD WINAPI TestThreadForAccept2Send1(LPVOID);
	static DWORD WINAPI TestThreadForAccept2Send2(LPVOID);
	static DWORD WINAPI TestThreadForAccept2Send3(LPVOID);
	static DWORD WINAPI TestThreadForAccept2Recv1(LPVOID);
	static DWORD WINAPI TestThreadForAccept2Recv2(LPVOID);
	static DWORD WINAPI TestThreadForAccept2Recv3(LPVOID);

	static int ThreadStartCount;
	static int ThreadEndCount;

public:
	void TestMultiAccept2();
};
