#include "base.h"
#include "common.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h> 
//#include <algorithm>

using namespace std;


Socket::Socket():
    sockfd(-1)
{
    
}

int Socket::Recv(int fd, string & msg, unsigned len)
{
    char buff[MAX_BUFF] = {0};
    
    int count = 0;
    
    int readlen;
    unsigned maxlen = MAX_BUFF;
    //读取指定长度的数据
    if (len > 0)
    {
        readlen = min(len, maxlen);
    }
    else
    {
        readlen = MAX_BUFF;
    }

    int sumlen = 0;

    while(1)
    {
        memset(buff, 0, sizeof(buff));

        int ret= read(fd, buff, readlen);
       
        if (0 == ret)
        {
            //没有数据可读，
            return 1;
        }
        else if (ret > 0)
        {
            msg += string(buff, ret);

            sumlen += ret;

            //判断是否读完指定字节的数据
            if (len > 0 && sumlen >= len)
            {
                return 0;
            }
        }
        else
        {
            if (errno == EAGAIN)
            {
                return 0;
            }

            return 1;
        }
    }
}

int Socket::Send(int fd, const char * pbuff, int len)
{
    int pos = 0, ret = 0;
    
    while(len > 0)
    {
        ret = write(fd, pbuff + pos, len);

        if (-1 == ret)
        {
            return -1;
        }

        if (ret > 0)
        {
            len -= ret;
            pos += ret;
        }
        else if (0 == ret)
        {
            break;
        }
    }


    return 0;
}

int Socket::SmallToBigEndian(unsigned char *pData, unsigned int uDataLen)
{
    unsigned char *pStart = pData;
    unsigned char *pEnd   = pData + uDataLen - 1;
    unsigned char cTmp;
     
    while(pEnd > pStart)
    {
        cTmp    = *pStart;
        *pStart = *pEnd;
        *pEnd   = cTmp;
 
        ++pStart;
        --pEnd;
    }
     
    return 0;
}

int Socket::BigToSmallEndian(unsigned char *pData, unsigned int uDataLen)
{
    return SmallToBigEndian(pData, uDataLen);
}

int SocketServer::CreateSock(const char * ip, int port, int nlisten)
{
    //first-create socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (-1 == fd)
    {
        return -1;
    }

    //second- bind

    //bind need exactly server address,so we create it.
       //at first-change char ip to numeric
    struct sockaddr_in servaddr;
    if (strcmp("*", ip) == 0)
    {
        //在任意地址上进行监听
        servaddr.sin_addr.s_addr = INADDR_ANY;
    }
    else
    {
        inet_aton(ip, &servaddr.sin_addr);
    }

    servaddr.sin_port = htons(port);  //host to network short

    //支持端口复用
    int  optval=1;
    setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(char*)&optval,sizeof(optval));

    int ret = bind(fd, (sockaddr *)&servaddr, sizeof(servaddr));

    if (-1 == ret)
    {
        return -1;
    }
    
    ret = listen(fd, nlisten);

    
    if (-1 == ret)
    {
        return -1;
    }

    sockfd = fd;

    //I think we should separate it from listen. because we can use epoll to produce this thing
    return 0;
}


int SocketServer::Accept()
{
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);

    return accept(sockfd, (sockaddr *)&cliaddr, &len);
}

int SocketClient::CreateSock(const char *ip, int port, int nlisten)
{
    //first-create socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (-1 == fd)
    {   
        cerr<<"create sockfd error"<<endl;
        return -1;
    }
    
    //second- bind
    
    //bind need exactly server address,so we create it.
       //at first-change char ip to numeric
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    inet_aton(ip, &servaddr.sin_addr);
    servaddr.sin_port = htons(port);  //host to network short

    int ret = connect(fd, (sockaddr *)&servaddr, sizeof(servaddr));

    if (ret)
    {
        cerr<<"connect fail. reason:"<<strerror(errno)<<endl;
        return -1;
    }

    sockfd = fd;

    //设置socket异步
    fcntl(sockfd, F_SETFL, O_NONBLOCK);

    return fd;
}

