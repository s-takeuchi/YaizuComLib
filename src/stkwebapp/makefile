stkwebapp.a : StkWebApp.o StkWebAppSend.o
	ar r stkwebapp.a StkWebApp.o StkWebAppSend.o

stkwebappstop : stkwebappstop.o
	cd ../../../YaizuComLib/src/stkpl; make
	cp ../../../YaizuComLib/src/stkpl/stkpl.a .
	cd ../../../YaizuComLib/src/commonfunc; make
	cp ../../../YaizuComLib/src/commonfunc/commonfunc.a .
	cd ../../../YaizuComLib/src/stksocket; make
	cp ../../../YaizuComLib/src/stksocket/stksocket.a .
	g++ -o stkwebappstop stkwebappstop.cpp stkpl.a stkwebapp.a commonfunc.a stksocket.a -std=c++1z -lstdc++fs -static-libgcc -static-libstdc++

StkWebApp.o : StkWebApp.cpp
	g++ -c StkWebApp.cpp -std=c++1z

StkWebAppSend.o : StkWebAppSend.cpp
	g++ -c StkWebAppSend.cpp -std=c++1z

clean :
	rm -f *.o *.a

all : clean stkwebapp.a stkwebappstop
