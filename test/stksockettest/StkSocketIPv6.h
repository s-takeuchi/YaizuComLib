#pragma once

class StkSocketIPv6
{
private:
	static void TestThreadForSend();
	static void TestThreadForRecv();
	static void TestThreadForSendUdp();
	static void TestThreadForRecvUdp();

	static int ThreadStartCount;
	static int ThreadEndCount;

	static int ErrFlag;

public:
	void TestIPv6();
	void TestIPv6Udp();
};
