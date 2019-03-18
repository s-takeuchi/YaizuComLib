stkwebapp.a : StkWebApp.o
	ar r stkwebapp.a StkWebApp.o

StkWebApp.o : StkWebApp.cpp
	g++ -c StkWebApp.cpp -std=c++1z

clean :
	rm -f *.o *.a

all : clean stkwebapp.a

