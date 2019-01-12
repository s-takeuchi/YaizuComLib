#pragma once

class StkSocketMemoryLeak
{
private:
	static void TestThreadForSend();
	static void TestThreadForRecv();
	static void TestThreadForSendUdp();
	static void TestThreadForRecvUdp();

public:
	void CheckLeakTcp();
	void CheckLeakUdp();
};
