#include "base.h"
#include "epooldemo.h"
#include <errno.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

void calltest()
{
    cout<<"just for test"<<endl;
}

ofstream errorlog("error.log", ios::out);

int client(char * ip, int port)
{
    Socket* sock = new SocketClient;
    int ret = sock->CreateSock(ip, port);

    if (ret == -1)
    {
        cerr<<"create socket error."<<endl;
        return -1;
    }

    int fd = sock->GetSockfd();

    //how to use epoll
    //first, set no block option
    Epoll poll;
    epoll_event event;
    event.events = EPOLLIN|EPOLLET;
    event.data.fd = fd;

    ret = poll.AddEpollEvent(fd, &event);

    if (ret)
    {
        cerr<<"add event fail. reason:"<<strerror(errno)<<endl;
        return 1;
    }

    
    epoll_event othevent;
    othevent.events = EPOLLIN | EPOLLET;
    event.data.fd = STDIN_FILENO;  //标准输入

    ret = poll.AddEpollEvent(STDIN_FILENO, &othevent);
    
    if (ret)
    {
        cerr<<"add event fail. reason:"<<strerror(errno)<<endl;
        return 1;
    }


    epoll_event* pevent = new epoll_event[5];

    while(1)
    {
        //wait for thing
        int ret = poll.WaitEvent(pevent);

        if (0 == ret)
        {
            //cerr<<"超时，继续等待"<<endl;
            continue;
        }
        else if (ret)
        {
            //have more than 1 decriptor is already
            for(int i = 0; i < ret; ++i)
            {
                //get the event info
                if(pevent[i].events == EPOLLIN)
                {
                    
                    //cout<<"fd:"<<pevent[i].data.fd<<endl;
                    if (fd == pevent[i].data.fd)
                    {
                        //cout<<"ready to read"<<endl;
                        //读取数据

                        string msg;
                        int res = sock->Recv(fd, msg);

                        if (res)
                        {
                            cerr<<"服务器异常关闭,原因:"<<strerror(errno)<<endl;
                            return 0;
                        }
                        else if(!msg.empty())
                        {
                            //如何做到清除上一行
                            cout<<"\33[2K\r";
                            cout<<msg<<endl;
                        }
                    }
                    else  //if (STDIN_FILENO == pevent[i].data.fd)
                    {
                        //ready to accept thing
                        string msg;
                        //cout<<"\n>";
                        getline(cin, msg);

                        //判断输入的特殊指令
                        if ("quit()" == msg)
                        {
                            //结束进程
                            //先关闭描述符
                            close(fd);
                            return 1;
                        }

                        //通过fd发送
                        int res = sock->Send(fd, msg.c_str(), msg.length());

                        if (res)
                        {
                            errorlog<<"send msg error. msg:"<<msg<<endl;                      
                        }
                        else
                        {
                            ;
                        }
                    }
                }
            }
        }
    }

    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        cout<<"outrage:[ ./client ip port]"<<endl;
        return 1;
    }

    int port = atoi(argv[2]);
    client(argv[1], port);

    errorlog.close();

    return 0;
}
