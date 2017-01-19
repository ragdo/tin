#ifndef TIN_SERVER_H
#define TIN_SERVER_H

#include <string>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/types.h>

using std::string;

#define LINE_LENGTH_LIMIT 4096
#define SERVER_PORT 34000
#define LISTEN_QUEUE 1024


class Server {
public:
    static int tcpEchoServer();

    static int udpEchoServer();

    static int Select(int activeSockDescCount, fd_set *readSockDescSet, fd_set *writeSockDescSet, fd_set *exceptSockDescSet, struct timeval *timeout);

    static ssize_t Read(int sockDesc, void *recvBuffer, size_t bytesCount);

    static void logError(string error);
};


#endif //TIN_SERVER_H
