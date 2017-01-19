#ifndef TIN_SOCKET_H
#define TIN_SOCKET_H

#include <string>
#include <cstdlib>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

using std::string;

#define LINE_LENGTH_LIMIT 4096
#define SERVER_PORT 34000
#define LISTEN_QUEUE 1024


class Socket {
public:
    static int read_cnt;
    static char* read_ptr;
    static char read_buf[LINE_LENGTH_LIMIT];

    static int CreateSocket(int family, int type, int protocol);
    static void Bind(int sockDesc, const struct sockaddr *sockAddr, socklen_t sockAddrLen);
    static void Listen(int sockDesc, int backlog);
    static int Accept(int sockDesc, struct sockaddr *sockAddr, socklen_t *sockAddrLenPtr);
    static void Connect(int sockDesc, const struct sockaddr *sockAddr, socklen_t sockAddrLen);
    static void Close(int sockDesc);

    static void InetPToN(int family, const char *strPtr, void *addrPtr);

    static void WriteBytes(int sockDesc, void *sendBuffer, size_t bytesCount);

    static ssize_t Readline(int fd, void *ptr, size_t maxlen);
    static ssize_t readline(int fd, void *vptr, size_t maxlen);
    static ssize_t my_read(int fd, char *ptr);

    static void logError(string error);

    static ssize_t Sendto(int sockfd,char *buf, size_t len, int flags, const struct sockaddr *dest, socklen_t addrlen);
    static ssize_t Recvfrom(int sfd, char *buf, size_t nbytes, int flags, struct sockaddr *from, socklen_t *addrlen);

private:
    static ssize_t writeBytes(int sockDesc, const void *sendBuffer, size_t bytesCount);
};


#endif //TIN_SOCKET_H
