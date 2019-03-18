stkpl.a : StkPl.o
	ar r stkpl.a StkPl.o

StkPl.o : StkPl.cpp
	g++ -c StkPl.cpp -std=c++1z

clean :
	rm -f *.o *.a

all : clean stkpl.a

