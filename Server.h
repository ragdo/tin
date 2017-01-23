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
#define SERVER_PORT_TCP_ECHO 34001
#define SERVER_PORT_UDP_ECHO 34002
#define SERVER_PORT_UDP_TIME 34003
#define SERVER_PORT_UDP_TICKET 34004
#define LISTEN_QUEUE 1024


class Server {
    RSA *rsa;
    UsersDatabase *database;
public:
    Server();
    ~Server();

    static int tcpEchoServer();

    static int udpEchoServer();

    static int tcpUdpEchoServer();

    static int Select(int activeSockDescCount, fd_set *readSockDescSet, fd_set *writeSockDescSet, fd_set *exceptSockDescSet, struct timeval *timeout);

    static ssize_t Read(int sockDesc, void *recvBuffer, size_t bytesCount);

    static void logError(string error);

    static string processMessage(string message);
};


#endif //TIN_SERVER_H
