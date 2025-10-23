CCOPTS    = -DUNICODE -std=c++1z

stkdb.a :  DbAccessor.o DbMariaDbAccessor.o DbMySqlAccessor.oDbPostgreSqlAccessor.o FilteringCondition.o
	ar r stkdb.a DbAccessor.o DbMariaDbAccessor.o DbMySqlAccessor.o DbPostgreSqlAccessor.o FilteringCondition.o

DbAccessor.o : DbAccessor.cpp
	g++ -c DbAccessor.cpp $(CCOPTS)

DbMariaDbAccessor.o : DbMariaDbAccessor.cpp
	g++ -c DbMariaDbAccessor.cpp $(CCOPTS)

DbMySqlAccessor.o : DbMySqlAccessor.cpp
	g++ -c DbMySqlAccessor.cpp $(CCOPTS)

DbPostgreSqlAccessor.o : DbPostgreSqlAccessor.cpp
	g++ -c DbPostgreSqlAccessor.cpp $(CCOPTS)

FilteringCondition.o : FilteringCondition.cpp
	g++ -c FilteringCondition.cpp $(CCOPTS)

clean :
	rm -f *.o *.a

all : clean stkdb.a
