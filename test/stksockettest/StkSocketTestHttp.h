#pragma once
#include <windows.h>

class StkSocketTestHttp
{
public:
	static DWORD WINAPI TestRecvHttpTermination1(LPVOID);
	static DWORD WINAPI TestSendHttpTermination1(LPVOID);
	static DWORD WINAPI TestRecvHttpTermination2(LPVOID);
	static DWORD WINAPI TestSendHttpTermination2(LPVOID);
	static bool FinishSendTest;
	static bool FinishRecvTest;

	void TestHttpTermination();
};
