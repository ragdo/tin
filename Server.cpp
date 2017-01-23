#include "RSA.h"
#include "Server.h"
#include "Socket.h"

#include <cstring>

#include <iostream>
#include <vector>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using std::vector;

Server::Server()
{
    rsa = new RSA(293,233);
    database = new UsersDatabase("users");
}

Server::~Server()
{
    delete rsa;
    delete database;
}

int Server::tcpUdpEchoServer()
{
    int maxActiveSockDesc = STDERR_FILENO;
    fd_set readSet;
    fd_set allSet;
    vector<int> clientSockDescs;

    //create server tcp socket
    struct sockaddr_in tcpServerAddress = {};
    int tcpListenSockDesc = Socket::CreateSocket(AF_INET, SOCK_STREAM, 0);
    tcpServerAddress.sin_family = AF_INET;
    tcpServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    tcpServerAddress.sin_port = htons(SERVER_PORT_TCP_ECHO);
    Socket::Bind(tcpListenSockDesc, (struct sockaddr *) &tcpServerAddress, sizeof(tcpServerAddress));
    Socket::Listen(tcpListenSockDesc, LISTEN_QUEUE);
    maxActiveSockDesc = tcpListenSockDesc;

    //create server udp socket
    struct sockaddr_in udpServerAddress = {};
    int udpListenSockDesc = Socket::CreateSocket(AF_INET, SOCK_DGRAM, 0);
    udpServerAddress.sin_family = AF_INET;
    udpServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    udpServerAddress.sin_port = htons(SERVER_PORT_UDP_ECHO);
    Socket::Bind(udpListenSockDesc, (struct sockaddr *) &udpServerAddress, sizeof(udpServerAddress));
    maxActiveSockDesc = max(tcpListenSockDesc, udpListenSockDesc);

    //prepare FD_SET
    FD_ZERO(&allSet);
    FD_SET(tcpListenSockDesc, &allSet);
    FD_SET(udpListenSockDesc, &allSet);

    //server loop
    while(true)
    {
        readSet = allSet;

        int readySockCount = Select(maxActiveSockDesc + 1,
                                    &readSet,
                                    nullptr,
                                    nullptr,
                                    nullptr);

        if(FD_ISSET(tcpListenSockDesc, &readSet))
        {
            struct sockaddr_in clientAddress = {};
            socklen_t clientAddressSize = sizeof(clientAddress);

            int connectionSockDesc = Socket::Accept(tcpListenSockDesc,
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

        if(FD_ISSET(udpListenSockDesc, &readSet))
        {
            struct sockaddr_in clientAddress = {};
            socklen_t clientAddressSize = sizeof(clientAddress);
            char buffer[LINE_LENGTH_LIMIT];
            ssize_t recvBytesCount = 0;

            recvBytesCount = Socket::Recvfrom(udpListenSockDesc, buffer, LINE_LENGTH_LIMIT,
                                              0, (struct sockaddr *) &clientAddress, &clientAddressSize);
            Socket::Sendto(udpListenSockDesc, buffer, recvBytesCount,
                           0, (struct sockaddr *) &clientAddress, clientAddressSize);
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

            ++i;
        }
    }

    return 0;
}

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

            ++i;
        }
    }

    return 0;
}

int Server::udpEchoServer()
{
    int maxActiveSockDesc = STDERR_FILENO;

    fd_set readSet, writeSet, allSet;
    vector<int> clientSockDescs;

    struct sockaddr_in serverAddress = {};

    int listenSockDesc = Socket::CreateSocket(AF_INET, SOCK_DGRAM, 0);

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(SERVER_PORT);

    Socket::Bind(listenSockDesc, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

    maxActiveSockDesc = listenSockDesc;

    FD_ZERO(&allSet);
    FD_SET(listenSockDesc, &allSet);

    /**************************/
    while(true)
    {
        int buflen = 1024;
        char buf[buflen];
        readSet = allSet;
        int nbytes;
        int j_ntohs;
        bzero(buf,buflen);
        FD_ZERO(&readSet);
        FD_SET(listenSockDesc,&readSet);
        int readySockCount = Select(maxActiveSockDesc + 1,
                                    &readSet,
                                    &writeSet,
                                    nullptr,
                                    nullptr);

        struct sockaddr_in clientAddress = {};
        socklen_t clientAddressSize = sizeof(clientAddress);


        if(FD_ISSET(listenSockDesc, &readSet))
        {
            printf("Server is ready to read.\n");

            nbytes = Socket::Recvfrom(listenSockDesc,buf,buflen,0,(struct sockaddr*)&clientAddress,&clientAddressSize);

            //j_ntohs = ntohs(clientAddress.sin_port);
            printf("Server received: %s\n", buf);
            string response = processMessage(buf);
            nbytes = response.length();
            strcpy(buf,response.c_str());
            FD_CLR(listenSockDesc, &readSet);
        }

        FD_ZERO(&writeSet);
        FD_SET(listenSockDesc,&writeSet);
        if(FD_ISSET(listenSockDesc, &writeSet))
        {
            printf("Server is ready to write.\n");

            nbytes = Socket::Sendto(listenSockDesc,buf,buflen,0,(struct sockaddr*)&clientAddress,clientAddressSize);

            FD_CLR(listenSockDesc, &writeSet);
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

string Server::processMessage(string message)
{
    string serviceSizeStr = message.substr(0,3);
    int servSize = Converter::toInt(serviceSizeStr);
    string service = message.substr(3,servSize);

    string response;

    RSA *rsa = new RSA(293,233);
    UsersDatabase *database = new UsersDatabase("users");

    if(service == "TCKT")
    {
        string usernameSizeStr = message.substr(3+servSize,3);
        int unSize = Converter::toInt(usernameSizeStr);
        string username = message.substr(6+servSize,unSize);
        string passwordSizeStr = message.substr(6+servSize+unSize,3);
        int pwSize = Converter::toInt(passwordSizeStr);
        string password = message.substr(9+servSize+unSize,pwSize);
        string portSizeStr = message.substr(9+servSize+unSize+pwSize,3);
        int poSize = Converter::toInt(portSizeStr);
        string portStr = message.substr(12+servSize+unSize+pwSize,poSize);

        cout << username << endl;
        cout << password << endl;
        cout << portStr << endl;
        int reqPort = Converter::toInt(portStr);
        password = rsa->decode(password);
        cout << password << endl;
        bool pass = database->checkPassword(username,password);
        if(!pass)
        {
            response = "Wrong username or password";
            logError(response);
            return response;
        }
        if(reqPort == 7)
        {
            bool pass = database->checkService(username,"echo");
            if(!pass)
            {
                response = "Service not allowed";
                logError(response);
                return response;
            }
            return TicketManager::createTicket(4,"127.0.0.1",7,1000);
        }
        else if(reqPort == 13)
        {
        }
        else
        {
            response = "Unknown service";
            logError(response);
            return response;
        }
    }
    else if(service == "SRVC")
    {
    }
    else
    {
        logError("No service: "+service);
    }
}




