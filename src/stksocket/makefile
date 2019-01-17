stksocket.a : StkPl.o StkSocketMgr.o stksocket.o
	ar r stksocket.a StkPl.o StkSocketMgr.o stksocket.o

StkPl.o : ../StkPl.cpp
	g++ -c ../StkPl.cpp -std=c++1z

StkSocketMgr.o : StkSocketMgr.cpp
	g++ -c StkSocketMgr.cpp -std=c++1z

stksocket.o : stksocket.cpp
	g++ -c stksocket.cpp -std=c++1z

clean :
	rm -f *.o *.a

all : clean stksocket.a

