stkthreadtest : stkthreadtest.o
	cd ../../src/stkthread; make
	cp ../../src/stkthread/stkthread.a .
	cd ../../src/stkpl; make
	cp ../../src/stkpl/stkpl.a .
	g++ -o stkthreadtest stkthreadtest.o stkpl.a stkthread.a -std=c++1z -lstdc++fs -pthread -lstdc++

stkthreadtest.o : stkthreadtest.cpp
	g++ -c stkthreadtest.cpp -std=c++1z

clean :
	rm -f *.o *.a stkthreadtest
	cd ../../src/stkthread; make clean

all : clean stkthreadtest

