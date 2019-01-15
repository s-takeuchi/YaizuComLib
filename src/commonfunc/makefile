commonfunc.a : StkPl.o StkObject.o StkProperties.o StkStringParser.o msgproc.o
	ar r commonfunc.a StkPl.o StkObject.o StkProperties.o StkStringParser.o msgproc.o

StkPl.o : ../StkPl.cpp
	g++ -c ../StkPl.cpp -std=c++1z -lstdc++fs

StkObject.o : StkObject.cpp
	g++ -c StkObject.cpp -std=c++1z -lstdc++fs

StkProperties.o : StkProperties.cpp
	g++ -c StkProperties.cpp -std=c++1z -lstdc++fs

StkStringParser.o : StkStringParser.cpp
	g++ -c StkStringParser.cpp -std=c++1z -lstdc++fs

msgproc.o : msgproc.cpp
	g++ -c msgproc.cpp -std=c++1z -lstdc++fs


