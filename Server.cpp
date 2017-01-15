#include "Server.h"

#include <cstring>

#include <vector>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>

using std::vector;


int Server::tcpEchoServer()
{
    int maxActiveSockDesc = STDERR_FILENO;
    fd_set readSet;
    fd_set allSet;
    vector<int> clientSockDescs;

    struct sockaddr_in serverAddress = {};
    int listenSockDesc = Socket(AF_INET, SOCK_STREAM, 0);

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(SERVER_PORT);

    Bind(listenSockDesc, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

    Listen(listenSockDesc, LISTEN_QUEUE);
    maxActiveSockDesc = listenSockDesc;

    FD_ZERO(&allSet);
    FD_SET(listenSockDesc, &allSet);

    while(true)
    {
        readSet = allSet;

        int readySockCount = Select(maxActiveSockDesc + 1,
                                    &readSet,
                                    nullptr,
                                    nullptr,
                                    nullptr);

        if(FD_ISSET(listenSockDesc, &readSet))
        {
            struct sockaddr_in clientAddress = {};
            socklen_t clientAddressSize = sizeof(clientAddress);

            int connectionSockDesc = Accept(listenSockDesc,
                                            (struct sockaddr *) &clientAddress,
                                            &clientAddressSize);

            if (clientSockDescs.size() + 1 <= FD_SETSIZE)
            {
                clientSockDescs.push_back(connectionSockDesc);
            }
            else
            {
                logError("Too many clients.");
                exit(-1);
            }

            FD_SET(connectionSockDesc, &allSet);

            if (connectionSockDesc > maxActiveSockDesc) {
                maxActiveSockDesc = connectionSockDesc;
            }

            if (--readySockCount <= 0) {
                continue;
            }
        }

        auto i = std::begin(clientSockDescs);

        while(i != std::end(clientSockDescs))
        {
            int clientSockDesc = *i;

            if(FD_ISSET(clientSockDesc, &readSet))
            {
                char buf[LINE_LENGTH_LIMIT];
                ssize_t recvBytesCount = 0;

                if((recvBytesCount = Read(clientSockDesc, buf, LINE_LENGTH_LIMIT)) == 0)
                {
                    Close(clientSockDesc);
                    FD_CLR(clientSockDesc, &allSet);
                    clientSockDescs.erase(i);
                }
                else
                {
                    Writen(clientSockDesc, buf, recvBytesCount);
                }

                if(--readySockCount <= 0)
                {
                    break;
                }
            }
        }
    }

    return 0;
}

int Server::Socket(int family, int type, int protocol)
{
    int sockDesc;

    if((sockDesc = socket(family, type, protocol)) < 0)
    {
        logError("socket error");
    }

    return sockDesc;
}

void Server::Bind(int sockDesc, const struct sockaddr *sockAddr, socklen_t sockAddrLen)
{
    if(bind(sockDesc, sockAddr, sockAddrLen) < 0)
    {
        logError("bind error");
    }
}

void Server::Listen(int sockDesc, int backlog) {
    if(listen(sockDesc, backlog) < 0)
    {
        logError("listen error");
    }
}

int Server::Accept(int sockDesc, struct sockaddr *sockAddr, socklen_t *sockAddrLenPtr)
{
    int newConnSockDesc;

    if((newConnSockDesc = accept(sockDesc, sockAddr, sockAddrLenPtr)) < 0)
    {
        logError("accept error");
    }

    return newConnSockDesc;

}

int Server::Select(int activeSockDescCount, fd_set *readSockDescSet,
                                            fd_set *writeSockDescSet,
                                            fd_set *exceptSockDescSet,
                                            struct timeval *timeout)
{
    int readySockCount;

    if((readySockCount = select(activeSockDescCount, readSockDescSet,
                                                     writeSockDescSet,
                                                     exceptSockDescSet,
                                                     timeout))
                                                     < 0)
    {
        logError("select error");
    }

    return readySockCount;
}

void Server::Close(int sockDesc)
{
    if(close(sockDesc) == -1)
    {
        logError("close error");
    }
}

ssize_t Server::Read(int sockDesc, void *recvBuffer, size_t bytesCount)
{
    ssize_t recvBytes;

    if((recvBytes = read(sockDesc, recvBuffer, bytesCount)) == -1)
    {
        logError("read error");
    }

    return recvBytes;
}

ssize_t Server::writen(int sockDesc, const void *sendBuffer, size_t bytesCount)
{
    const char *bufferPointer = (const char *) sendBuffer;
    size_t bytesLeft = bytesCount;
    ssize_t bytesWritten = 0;

    while(bytesLeft > 0)
    {
        if((bytesWritten = write(sockDesc, bufferPointer, bytesLeft)) <= 0)
        {
            if(bytesWritten < 0 && errno == EINTR)
            {
                bytesWritten = 0;
            }
            else
            {
                return -1;
            }
        }

        bytesLeft -= bytesWritten;
        bufferPointer += bytesWritten;
    }

    return bytesCount;
}

void Server::Writen(int sockDesc, void *sendBuffer, size_t bytesCount)
{
    if(writen(sockDesc, sendBuffer, bytesCount) != bytesCount)
    {
        logError("writen error");
    }
}

void Server::logError(string error)
{
    std::cerr << "Error: " << error << std::endl;
}
