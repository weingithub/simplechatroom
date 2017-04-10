#include "epooldemo.h"

Epoll::Epoll()
{
    init();
}

int Epoll::init()
{
    //invoke epoll_create to create an epoll instance
   epfd= epoll_create(1000);
}
        
int Epoll::AddEpollEvent(int sockfd, struct epoll_event *event)
{
    //via epoll_ctl to control event
    //op is EPOLL_CTL_ADD
    return epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, event);
}

int Epoll::WaitEvent(struct epoll_event * event)
{
    return epoll_wait(epfd, event, 5, 1000*10);   //理论上来说，应该1ms就超时了,-1不超时，永远阻塞
}


