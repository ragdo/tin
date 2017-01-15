#ifndef TIN_SERVER_H
#define TIN_SERVER_H

#include <string>

#include <unistd.h>

using std::string;

#define LINE_LENGTH_LIMIT 4096
#define BUFFER_SIZE 8192
#define SERVER_PORT 34000
#define LISTEN_QUEUE 1024


class Server {
public:
    static int tcpEchoServer();

    static int Socket(int family, int type, int protocol);
    static void Bind(int sockDesc, const struct sockaddr *sockAddr, socklen_t sockAddrLen);
    static void Listen(int sockDesc, int backlog);
    static int Accept(int sockDesc, struct sockaddr *sockAddr, socklen_t *sockAddrLenPtr);
    static void Close(int sockDesc);

    static int Select(int activeSockDescCount, fd_set *readSockDescSet, fd_set *writeSockDescSet, fd_set *exceptSockDescSet, struct timeval *timeout);

    static ssize_t writen(int sockDesc, const void *sendBuffer, size_t bytesCount);
    static void Writen(int sockDesc, void *sendBuffer, size_t bytesCount);

    static ssize_t Read(int sockDesc, void *recvBuffer, size_t bytesCount);

    static void logError(string error);
};


#endif //TIN_SERVER_H