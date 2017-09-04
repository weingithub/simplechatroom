CPP = g++
Flag = -g -fPIC
TARGET =libtest.so

.PHONY: all clean test server client install mysqltest
OBJECT = base.o epooldemo.o threadpool.o mysqlutil.o
SOURCE = base.cpp epooldemo.cpp threadpool.cpp  mysqlutil.cpp

INCLUDE = -I /usr/include/mysql/

SERVER = server
CLIENT = client
MYSQLTEST = mysqltest

PLINKOBJ =  -lpthread -lmysqlclient
LINK_INC = -L /usr/lib/mysql -L //usr/local/lib/
LINKOBJ = -ltest -lpthread -lmysqlclient


test:
	$(CPP) $(Flag) -shared -o $(TARGET) $(SOURCE) $(LINK_INC) $(INCLUDE) $(PLINKOBJ) 

mysqltest:
	$(CPP) -g -o $(MYSQLTEST) mysqltest.cpp $(LINK_INC) $(LINKOBJ) $(INCLUDE)

server: 
	$(CPP) -g -o $(SERVER) server.cpp $(LINKOBJ)

client:
	$(CPP) -g -o $(CLIENT) client.cpp $(LINKOBJ)

install:
	cp -f $(TARGET) /usr/local/lib

clean:
	rm -f $(OBJECT) $(TARGET) $(SERVER) $(CLIENT) $(MYSQLTEST)

