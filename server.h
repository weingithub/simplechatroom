#ifndef _SERVER_H
#define _SERVER_H
#include <map>
#include <pthread.h>

class Server
{
public:
    Server();

    struct Param
    {
        void * ptr;
        int fd;
    };

    int Run();

    int Broadcast(const char *msg, int len);

    int DelClient(int fd);

    Epoll & GetEpoll() {return poll;}
private:
   std::map<int, int> clients;
   pthread_mutex_t client_mutex;
   ThreadPool pool;
   Epoll poll;
};

#endif
