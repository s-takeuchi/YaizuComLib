stkdb.a :  DbAccessor.o DbMariaDbAccessor.o DbMySqlAccessor.oDbPostgreSqlAccessor.o FilteringCondition.o
	ar r stkdb.a DbAccessor.o DbMariaDbAccessor.o DbMySqlAccessor.oDbPostgreSqlAccessor.o FilteringCondition.o

DbAccessor.o : DbAccessor.cpp
	g++ -c DbAccessor.cpp -std=c++1z

DbMariaDbAccessor.o : DbMariaDbAccessor.cpp
	g++ -c DbMariaDbAccessor.cpp -std=c++1z

DbMySqlAccessor.o : DbMySqlAccessor.cpp
	g++ -c DbMySqlAccessor.cpp -std=c++1z

DbPostgreSqlAccessor.o : DbPostgreSqlAccessor.cpp
	g++ -c DbPostgreSqlAccessor.cpp -std=c++1z

FilteringCondition.o : FilteringCondition.cpp
	g++ -c FilteringCondition.cpp -std=c++1z

clean :
	rm -f *.o *.a

all : clean stkdb.a
