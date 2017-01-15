#include "Server.h"
#include "Socket.h"

#include <cstring>

#include <iostream>
#include <vector>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using std::vector;


int Server::tcpEchoServer()
{
    int maxActiveSockDesc = STDERR_FILENO;
    fd_set readSet;
    fd_set allSet;
    vector<int> clientSockDescs;

    struct sockaddr_in serverAddress = {};
    int listenSockDesc = Socket::CreateSocket(AF_INET, SOCK_STREAM, 0);

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(SERVER_PORT);

    Socket::Bind(listenSockDesc, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

    Socket::Listen(listenSockDesc, LISTEN_QUEUE);
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

            int connectionSockDesc = Socket::Accept(listenSockDesc,
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
                    Socket::Close(clientSockDesc);
                    FD_CLR(clientSockDesc, &allSet);
                    clientSockDescs.erase(i);
                }
                else
                {
                    Socket::WriteBytes(clientSockDesc, buf, recvBytesCount);
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

ssize_t Server::Read(int sockDesc, void *recvBuffer, size_t bytesCount)
{
    ssize_t recvBytes;

    if((recvBytes = read(sockDesc, recvBuffer, bytesCount)) == -1)
    {
        logError("read error");
    }

    return recvBytes;
}

void Server::logError(string error)
{
    std::cerr << "Error: " << error << std::endl;
}
