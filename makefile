CPP = g++
Flag = -g -fPIC
TARGET =libtest.so

.PHONY: all clean test server client
OBJECT = base.o epooldemo.o threadpool.o
SOURCE = base.cpp epooldemo.cpp threadpool.cpp 

SERVER = server
CLIENT = client
PLINKOBJ =  -lpthread
LINKOBJ = -ltest -lpthread

test:
	$(CPP) $(Flag) -shared -o $(TARGET) $(SOURCE) $(PLINKOBJ)

server: 
	$(CPP) -g -o $(SERVER) server.cpp $(LINKOBJ)

client:
	$(CPP) -g -o $(CLIENT) client.cpp $(LINKOBJ)

clean:
	rm -f $(OBJECT) $(TARGET)

