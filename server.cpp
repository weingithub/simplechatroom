#include "base.h"
#include "epooldemo.h"
#include "threadpool.h"
#include "common.h"
#include "server.h"
#include <cstdio>
#include <fcntl.h>

using namespace std;

void* server_process(void * args)
{
    cout<<"server_process"<<endl;
    Server::Param * pPram = (Server::Param *) args;

    char buff[MAX_BUFF] = {0};

    sprintf(buff, "欢迎用户:%d进入测试聊天室", pPram->fd);

    //聊天室服务器负责什么？转发用户的消息，显示用户的输入内容，这里的fd是指向对应客户端的
    //接收fd的信息，然后广播全部用户,那么，如何广播呢
    //将信息发送给所有在线用户
    
    Server * pserv = (Server *) pPram->ptr;

    pserv->Broadcast(buff, strlen(buff));

    int fd =  pPram->fd;

    //监控连接套接字的读
    Epoll poll;

    epoll_event event;
    event.events = EPOLLIN|EPOLLET;
    event.data.fd = pPram->fd; 

    int ret = poll.AddEpollEvent(fd, &event);

    if (ret)
    {   
        cerr<<"add event fail. reason"<<strerror(errno)<<endl;
        return (void*)1;
    }   

    epoll_event* pevent = new epoll_event[5];

    while(1)
    {
        //监听epoll事件
        int ret = poll.WaitEvent(pevent);

        if (-1 == ret)
        {
            cerr<<"超时，继续等待"<<endl;
            continue;
        }
        else if (ret)
        {
            for(int i= 0; i < ret; ++i)
            {
                if (fd == pevent[i].data.fd)
                {
                    string msg;
                    int res = Socket::Recv(fd, msg);

                    int len = msg.length() + 30;
                    char * buff = new char[len];

                    memset(buff, 0, len);
                    sprintf(buff, "用户%d说:%s", fd, msg.c_str());

                    if (res)
                    {
                        //客户端中断连接，服务端关闭该进程
                        pserv->DelClient(fd);
                        
                        memset(buff, 0, len);
                        sprintf(buff, "用户%d下线", fd);

                        pserv->Broadcast(buff, len);

                        delete[] buff;
                        return (void *)1;
                    }
                    else
                    {
                        //cout<<"收到客户端信息:"<<msg<<endl;
                        //转发收到的信息给所有客户端
                        pserv->Broadcast(buff, len);

                        delete[] buff;
                    }
                }
            }
        }
    }

    return (void*)0;
}

Server::Server()
{
    pthread_mutex_init(&client_mutex, NULL);
}

int Server::Run()
{
    Socket * sock = new SocketServer;

    int ret = sock->CreateSock("*", 18888);

    if (ret == -1)
    {
        cerr<<"create server socket error,reason:"<<strerror(errno)<<endl;
        return 1;
    }

    int fd = sock->GetSockfd();

    //how to use epoll
    //first, set no block option
    epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = fd;

    ret = poll.AddEpollEvent(fd, &event);

    if (ret)
    {
        cerr<<"add event fail. reason"<<strerror(errno)<<endl;
        return 1;
    }

    epoll_event* pevent = new epoll_event[5];

    //开启线程池处理

    while(1)
    {
        //wait for thing
        int ret = poll.WaitEvent(pevent);

        if (-1 == ret)
        {
            cerr<<"超时，继续等待"<<endl;
            continue;
        }
        else if (ret)
        {
            //have more than 1 decriptor is already
            for(int i = 0; i < ret; ++i)
            {
                //get the event info
                if (fd == pevent[i].data.fd)
                {
                    cout<<"Listen is ready to accept"<<endl;
                    //准备接受
                    int connfd = sock->Accept();

                    if (connfd == -1)
                    {
                        cerr<<"accept error. reason:"<<strerror(errno)<<endl;
                        continue;
                    }
                    else
                    {
                        //连接完毕，准备和客户端进行交互

                        //这是一个会阻塞的方法，这么说，线程池并不符合我的需求
                        //设置异步
                        fcntl(connfd, F_SETFL, O_NONBLOCK);

                        pthread_mutex_lock(&client_mutex);
                        clients[connfd] = 1;
                        pthread_mutex_unlock(&client_mutex);

                        Param param;
                        param.ptr = this;
                        param.fd = connfd;
                        pool.AddTask(server_process, &param);
                        

                        cout<<"AddTask 完毕"<<endl;
                    }
                }
            }
        }
        else
        {
            //cout<<"等待事件"<<endl;
        }
    }

    cout<<"Run 结束了"<<endl;
    return 0;
}

int Server::Broadcast(const char *msg, int len)
{

    cout<<"服务器开始广播"<<endl;
    pthread_mutex_lock(&client_mutex);

    int ret = 0;

    //遍历客户端，给客户端发消息
    for(map<int, int>::iterator it = clients.begin(); it != clients.end(); )
    {
        //发送消息
        int fd = it->first;

        ret = Socket::Send(fd, msg, len);

        if (ret)
        {
            //发送失败，默认客户端掉线，删除该客户端
            clients.erase(it++);
        }
        else
        {
            ++it;
        }
    }

    pthread_mutex_unlock(&client_mutex);
}

int Server::DelClient(int fd)
{
    cout<<"客户端:"<<fd<<"已关闭，删除该客户端"<<endl;
    clients.erase(fd);

    return 0;
}

int main()
{
    Server ser;

    ser.Run();

    return 0;
}
