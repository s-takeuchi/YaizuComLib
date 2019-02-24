stkdata.a : StkPl.o ColumnDef.o ColumnDefBin.o ColumnDefFloat.o ColumnDefInt.o ColumnDefStr.o ColumnDefWStr.o ColumnData.o ColumnDataBin.o ColumnDataFloat.o ColumnDataInt.o ColumnDataStr.o ColumnDataWStr.o
	ar r stkdata.a StkPl.o ColumnDef.o ColumnDefBin.o ColumnDefFloat.o ColumnDefInt.o ColumnDefStr.o ColumnDefWStr.o ColumnData.o ColumnDataBin.o ColumnDataFloat.o ColumnDataInt.o ColumnDataStr.o ColumnDataWStr.o

StkPl.o : ../StkPl.cpp
	g++ -c ../StkPl.cpp -std=c++1z

ColumnDef.o : ColumnDef.cpp
	g++ -c ColumnDef.cpp -std=c++1z

ColumnDefBin.o : ColumnDefBin.cpp
	g++ -c ColumnDefBin.cpp -std=c++1z

ColumnDefFloat.o : ColumnDefFloat.cpp
	g++ -c ColumnDefFloat.cpp -std=c++1z

ColumnDefInt.o : ColumnDefInt.cpp
	g++ -c ColumnDefInt.cpp -std=c++1z

ColumnDefStr.o : ColumnDefStr.cpp
	g++ -c ColumnDefStr.cpp -std=c++1z

ColumnDefWStr.o : ColumnDefWStr.cpp
	g++ -c ColumnDefWStr.cpp -std=c++1z

ColumnData.o : ColumnData.cpp
	g++ -c ColumnData.cpp -std=c++1z

ColumnDataBin.o : ColumnDataBin.cpp
	g++ -c ColumnDataBin.cpp -std=c++1z

ColumnDataFloat.o : ColumnDataFloat.cpp
	g++ -c ColumnDataFloat.cpp -std=c++1z

ColumnDataInt.o : ColumnDataInt.cpp
	g++ -c ColumnDataInt.cpp -std=c++1z

ColumnDataStr.o : ColumnDataStr.cpp
	g++ -c ColumnDataStr.cpp -std=c++1z

ColumnDataWStr.o : ColumnDataWStr.cpp
	g++ -c ColumnDataWStr.cpp -std=c++1z

clean :
	rm -f *.o *.a

all : clean stkdata.a
