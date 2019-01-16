stksocket.a : StkPl.o StkThreadElement.o StkThreadManager.o stkthread.o
	ar r stkthread.a StkPl.o StkThreadElement.o StkThreadManager.o stkthread.o

StkPl.o : ../StkPl.cpp
	g++ -c ../StkPl.cpp -std=c++1z

StkThreadElement.o : StkThreadElement.cpp
	g++ -c StkThreadElement.cpp -std=c++1z

StkThreadManager.o : StkThreadManager.cpp
	g++ -c StkThreadManager.cpp -std=c++1z

stkthread.o : stkthread.cpp
	g++ -c stkthread.cpp -std=c++1z

