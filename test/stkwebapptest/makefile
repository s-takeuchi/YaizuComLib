stkwebapptest : stkwebapptest.o StkWebAppTest1.o StkWebAppTest2.o StkWebAppTest3.o StkWebAppTest4.o StkWebAppTest5.o
	cd ../../src/stkwebapp; make
	cp ../../src/stkwebapp/stkwebapp.a .
	cd ../../src/commonfunc; make
	cp ../../src/commonfunc/commonfunc.a .
	cd ../../src/stksocket; make
	cp ../../src/stksocket/stksocket.a .
	cd ../../src/stkthread; make
	cp ../../src/stkthread/stkthread.a .
	g++ -o stkwebapptest stkwebapptest.o StkWebAppTest1.o StkWebAppTest2.o StkWebAppTest3.o StkWebAppTest4.o StkWebAppTest5.o stkwebapp.a commonfunc.a stksocket.a stkthread.a -std=c++1z -lstdc++fs -pthread

stkwebapptest.o : stkwebapptest.cpp
	g++ -c stkwebapptest.cpp -std=c++1z

StkWebAppTest1.o : StkWebAppTest1.cpp
	g++ -c StkWebAppTest1.cpp -std=c++1z

StkWebAppTest2.o : StkWebAppTest2.cpp
	g++ -c StkWebAppTest2.cpp -std=c++1z

StkWebAppTest3.o : StkWebAppTest3.cpp
	g++ -c StkWebAppTest3.cpp -std=c++1z

StkWebAppTest4.o : StkWebAppTest4.cpp
	g++ -c StkWebAppTest4.cpp -std=c++1z

StkWebAppTest5.o : StkWebAppTest5.cpp
	g++ -c StkWebAppTest5.cpp -std=c++1z

clean :
	rm -f *.o *.a stkwebapptest
	cd ../../src/stkwebapp; make clean
	cd ../../src/commonfunc; make clean
	cd ../../src/stksocket; make clean
	cd ../../src/stkthread; make clean

all : clean stkwebapptest

