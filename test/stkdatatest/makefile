stkdatatest : AutoSave.o BinaryColumn.o FloatColumn.o Insert16383RecordsAtOneTime.o ManyRecords.o SmallTable.o TableVersion.o insert16383records.o sample.cpp
	cd ../../src/stkdata; make
	cp ../../src/stkdata/stkdata.a .
	cd ../../src/stkpl; make
	cp ../../src/stkpl/stkpl.a .
	g++ -o stkdatatest AutoSave.o BinaryColumn.o FloatColumn.o Insert16383RecordsAtOneTime.o ManyRecords.o SmallTable.o TableVersion.o insert16383records.o sample.cpp stkpl.a stkdata.a -std=c++1z -lstdc++fs -pthread -lstdc++

StkPl.o : ../../src/StkPl.cpp
	g++ -c ../../src/StkPl.cpp -std=c++1z

AutoSave.o : AutoSave.cpp
	g++ -c AutoSave.cpp -std=c++1z

BinaryColumn.o : BinaryColumn.cpp
	g++ -c BinaryColumn.cpp -std=c++1z

FloatColumn.o : FloatColumn.cpp
	g++ -c FloatColumn.cpp -std=c++1z

Insert16383RecordsAtOneTime.o : Insert16383RecordsAtOneTime.cpp
	g++ -c Insert16383RecordsAtOneTime.cpp -std=c++1z

ManyRecords.o : ManyRecords.cpp
	g++ -c ManyRecords.cpp -std=c++1z

SmallTable.o : SmallTable.cpp
	g++ -c SmallTable.cpp -std=c++1z

TableVersion.o : TableVersion.cpp
	g++ -c TableVersion.cpp -std=c++1z

insert16383records.o : insert16383records.cpp
	g++ -c insert16383records.cpp -std=c++1z

sample.o : sample.cpp
	g++ -c sample.cpp -std=c++1z

clean :
	rm -f *.o *.a stkdatatest
	cd ../../src/stkdata; make clean

all : clean stkdatatest
