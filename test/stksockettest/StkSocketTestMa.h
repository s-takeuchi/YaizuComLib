#pragma once

class StkSocketTestMa
{
private:
	static void TestThreadForAccept2Send1();
	static void TestThreadForAccept2Send2();
	static void TestThreadForAccept2Send3();
	static void TestThreadForAccept2Recv1();
	static void TestThreadForAccept2Recv2();
	static void TestThreadForAccept2Recv3();

	static int ThreadStartCount;
	static int ThreadEndCount;

public:
	void TestMultiAccept2();
	void TestMultiAccept3();
};
