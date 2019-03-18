commonfunc.a : StkObject.o StkProperties.o StkStringParser.o msgproc.o
	ar r commonfunc.a StkObject.o StkProperties.o StkStringParser.o msgproc.o

StkObject.o : StkObject.cpp
	g++ -c StkObject.cpp -std=c++1z

StkProperties.o : StkProperties.cpp
	g++ -c StkProperties.cpp -std=c++1z

StkStringParser.o : StkStringParser.cpp
	g++ -c StkStringParser.cpp -std=c++1z

msgproc.o : msgproc.cpp
	g++ -c msgproc.cpp -std=c++1z

clean :
	rm -f *.o *.a

all : clean commonfunc.a

