stkwebapp_um.a : ApiBase.o ApiGetUser.o ApiPostUser.o ApiDeleteUser.o ApiGetLanguage.o ApiLogging.o DataAccessUm.o UserManagement.o stkwebapp_um.o
	ar r stkwebapp_um.a ApiBase.o ApiGetUser.o ApiPostUser.o ApiDeleteUser.o ApiGetLanguage.o ApiLogging.o DataAccessUm.o UserManagement.o stkwebapp_um.o

ApiBase.o : ApiBase.cpp
	g++ -c ApiBase.cpp -std=c++1z

ApiGetUser.o : ApiGetUser.cpp
	g++ -c ApiGetUser.cpp -std=c++1z

ApiPostUser.o : ApiPostUser.cpp
	g++ -c ApiPostUser.cpp -std=c++1z

ApiDeleteUser.o : ApiDeleteUser.cpp
	g++ -c ApiDeleteUser.cpp -std=c++1z

ApiGetLanguage.o : ApiGetLanguage.cpp
	g++ -c ApiGetLanguage.cpp -std=c++1z

ApiLogging.o : ApiLogging.cpp
	g++ -c ApiLogging.cpp -std=c++1z

DataAccessUm.o : DataAccessUm.cpp
	g++ -c DataAccessUm.cpp -std=c++1z

UserManagement.o : UserManagement.cpp
	g++ -c UserManagement.cpp -std=c++1z

stkwebapp_um.o : stkwebapp_um.cpp
	g++ -c stkwebapp_um.cpp -std=c++1z

clean :
	rm -f *.o *.a

all : clean stkwebapp_um.a
