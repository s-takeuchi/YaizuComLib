LDFLAGS = -L/usr/local/lib64 -L/usr/local/lib
LIBS    = -lssl -lcrypto

stksockettest : StkSocketIPv6.o StkSocketMemoryLeak.o StkSocketTest.o StkSocketTestGetSockInfo.o StkSocketTestHttp.o StkSocketTestMa.o
	cd ../../src/stksocket; make
	cp ../../src/stksocket/stksocket.a .
	cd ../../src/stkpl; make
	cp ../../src/stkpl/stkpl.a .
	g++ -o stksockettest StkSocketIPv6.o StkSocketMemoryLeak.o StkSocketTest.o StkSocketTestGetSockInfo.o StkSocketTestHttp.o StkSocketTestMa.o stkpl.a stksocket.a -std=c++1z -lstdc++fs -pthread -static-libgcc -static-libstdc++ $(LDFLAGS) $(LIBS)

StkSocketIPv6.o : StkSocketIPv6.cpp
	g++ -c StkSocketIPv6.cpp -std=c++1z

StkSocketMemoryLeak.o : StkSocketMemoryLeak.cpp
	g++ -c StkSocketMemoryLeak.cpp -std=c++1z

StkSocketTest.o : StkSocketTest.cpp
	g++ -c StkSocketTest.cpp -std=c++1z -lstdc++fs

StkSocketTestGetSockInfo.o : StkSocketTestGetSockInfo.cpp
	g++ -c StkSocketTestGetSockInfo.cpp -std=c++1z

StkSocketTestHttp.o : StkSocketTestHttp.cpp
	g++ -c StkSocketTestHttp.cpp -std=c++1z

StkSocketTestMa.o : StkSocketTestMa.cpp
	g++ -c StkSocketTestMa.cpp -std=c++1z

clean :
	rm -f *.o *.a stksockettest
	cd ../../src/stksocket; make clean

all : clean stksockettest

