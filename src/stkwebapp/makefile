stkwebapp.a : StkWebApp.o StkWebAppSend.o
	ar r stkwebapp.a StkWebApp.o StkWebAppSend.o

StkWebApp.o : StkWebApp.cpp
	g++ -c StkWebApp.cpp -std=c++1z

StkWebAppSend.o : StkWebAppSend.cpp
	g++ -c StkWebAppSend.cpp -std=c++1z

clean :
	rm -f *.o *.a

all : clean stkwebapp.a

