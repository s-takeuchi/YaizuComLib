commonfunctest : StkStringParserTest.o StkPropertiesTest.o StkObjectTest.o MsgProcTest.o commonfunctest.o
	cd ../../src/commonfunc; make
	cp ../../src/commonfunc/commonfunc.a .
	cd ../../src/stkpl; make
	cp ../../src/stkpl/stkpl.a .
	g++ -o commonfunctest StkStringParserTest.o StkPropertiesTest.o StkObjectTest.o MsgProcTest.o commonfunctest.o stkpl.a commonfunc.a -std=c++1z -lstdc++fs -pthread -static-libgcc -lstdc++

StkStringParserTest.o : StkStringParserTest.cpp
	g++ -c StkStringParserTest.cpp -std=c++1z

StkPropertiesTest.o : StkPropertiesTest.cpp
	g++ -c StkPropertiesTest.cpp -std=c++1z

StkObjectTest.o : StkObjectTest.cpp
	g++ -c StkObjectTest.cpp -std=c++1z

MsgProcTest.o : MsgProcTest.cpp
	g++ -c MsgProcTest.cpp -std=c++1z

commonfunctest.o : commonfunctest.cpp
	g++ -c commonfunctest.cpp -std=c++1z

clean :
	rm -f *.o *.a commonfunctest
	cd ../../src/commonfunc; make clean

all : clean commonfunctest

