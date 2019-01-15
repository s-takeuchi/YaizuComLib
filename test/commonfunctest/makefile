commonfunctest : StkStringParserTest.o StkPropertiesTest.o StkObjectTest.o MsgProcTest.o commonfunctest.o
	cd ../../src/commonfunc; make
	cp ../../src/commonfunc/commonfunc.a .
	g++ -o commonfunctest StkStringParserTest.o StkPropertiesTest.o StkObjectTest.o MsgProcTest.o commonfunctest.o commonfunc.a -std=c++1z -lstdc++fs

StkStringParserTest.o : StkStringParserTest.cpp
	g++ -c StkStringParserTest.cpp -std=c++1z -lstdc++fs

StkPropertiesTest.o : StkPropertiesTest.cpp
	g++ -c StkPropertiesTest.cpp -std=c++1z -lstdc++fs

StkObjectTest.o : StkObjectTest.cpp
	g++ -c StkObjectTest.cpp -std=c++1z -lstdc++fs

MsgProcTest.o : MsgProcTest.cpp
	g++ -c MsgProcTest.cpp -std=c++1z -lstdc++fs

commonfunctest.o : commonfunctest.cpp
	g++ -c commonfunctest.cpp -std=c++1z -lstdc++fs


