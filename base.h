#ifndef _BASE_H
#define _BASE_H
#include <iostream>

class Socket
{
public:
    Socket();

    virtual int CreateSock(const char * ip, int port, int nlisten = 5) {}
    
    virtual int Accept() {}

    static int Recv(int fd, std::string & msg, unsigned len = 0);
    static int Send(int fd, const char * pbuff, int len);

    static int SmallToBigEndian(unsigned char *pData, unsigned int uDataLen);

    static int BigToSmallEndian(unsigned char *pData, unsigned int uDataLen);

    int GetSockfd() {return sockfd;}
protected:
    int sockfd;
};

class SocketServer: public Socket
{
public:
    SocketServer() {};

    virtual int CreateSock(const char * ip, int port, int nlisten = 5);

    virtual int Accept();
};

class SocketClient : public Socket
{
public:    
    SocketClient(){};

    virtual int CreateSock(const char * ip, int port, int nlisten = 5);
};

#endif
