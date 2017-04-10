#ifndef EPOLL_DEMO
#define EPOLL_DEMO
#include <sys/epoll.h>

class Epoll
{
public:
    Epoll();

    int AddEpollEvent(int sockfd, struct epoll_event *event);
    
    int WaitEvent(struct epoll_event *event);
private:
    int init();
        
private:
    int epfd;
};

#endif

