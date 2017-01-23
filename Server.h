#ifndef TIN_SERVER_H
#define TIN_SERVER_H

#include <string>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/types.h>
#include "RSA.h"
#include "UsersDatabase.h"
#include "TicketManager.h"

using std::string;

#define LINE_LENGTH_LIMIT 4096
#define SERVER_PORT 2007
#define SERVER_PORT_ECHO 2007
#define SERVER_PORT_TIME 2013
#define SERVER_PORT_TICKET 2000
#define LISTEN_QUEUE 1024
#define ECHO_PORT 2007
#define TIME_PORT 2013


class Server {
    RSA *rsa;
    UsersDatabase *database;
public:
    Server();
    ~Server();

    static int tcpUdpEchoServer();

    static int tcpEchoServer();

    static int udpEchoServer();

    static int Select(int activeSockDescCount, fd_set *readSockDescSet, fd_set *writeSockDescSet, fd_set *exceptSockDescSet, struct timeval *timeout);

    static ssize_t Read(int sockDesc, void *recvBuffer, size_t bytesCount);

    static void logError(string error);

    static string processMessage(string message, string address, int port);
};


#endif //TIN_SERVER_H
