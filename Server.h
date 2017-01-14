#ifndef TIN_SERVER_H
#define TIN_SERVER_H

#include <string>
#include <unistd.h>

using std::string;

#define MAXLINE 4096
#define BUFFSIZE 8192
#define SERV_PORT 34000
#define LISTENQ 1024


class Server {
public:
    static int tcpEchoServer();

    static int Socket(int family, int type, int protocol);
    static void Bind(int fd, const struct sockaddr *sa, socklen_t salen);
    static void Listen(int fd, int backlog);
    static int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);

    static int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

    static ssize_t writen(int fd, const void *vptr, size_t n);
    static void Writen(int fd, void *ptr, size_t nbytes);

    static void Close(int fd);
    static ssize_t Read(int fd, void *ptr, size_t nbytes);

    static void logError(string error);
};


#endif //TIN_SERVER_H