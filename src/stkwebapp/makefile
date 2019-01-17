stkwebapp.a : StkPl.o StkWebApp.o
	ar r stkwebapp.a StkWebApp.o

StkPl.o : ../StkPl.cpp
	g++ -c ../StkPl.cpp -std=c++1z

StkWebApp.o : StkWebApp.cpp
	g++ -c StkWebApp.cpp -std=c++1z

clean :
	rm -f *.o *.a

all : clean stkwebapp.a

