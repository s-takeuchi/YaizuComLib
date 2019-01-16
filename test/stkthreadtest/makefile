stkthreadtest : stkthreadtest.o
	cd ../../src/stkthread; make
	cp ../../src/stkthread/stkthread.a .
	g++ -o stkthreadtest stkthreadtest.o stkthread.a -std=c++1z -lstdc++fs -pthread

stkthreadtest.o : stkthreadtest.cpp
	g++ -c stkthreadtest.cpp -std=c++1z

