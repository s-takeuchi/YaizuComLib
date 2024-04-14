stkpltest : stkpltest.o
	cd ../../src/stkpl; make
	cp ../../src/stkpl/stkpl.a .
	g++ -o stkpltest stkpltest.o stkpl.a -std=c++1z -lstdc++fs -lstdc++

stkpltest.o : stkpltest.cpp
	g++ -c stkpltest.cpp -std=c++1z

clean :
	rm -f *.o *.a stkpltest
	cd ../../src/stkpl; make clean

all : clean stkpltest

