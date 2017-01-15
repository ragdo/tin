#ifndef TIN_CLIENT_H
#define TIN_CLIENT_H

#include <string>
#include "Server.h"

using std::string;

class Client {
public:

    static int read_cnt;
    static char* read_ptr;
    static char read_buf[LINE_LENGTH_LIMIT];



    static int tcpEchoClient(char *ip);

    static int Socket(int family, int type, int protocol);
    static void Connect(int fd, const struct sockaddr *sa, socklen_t salen);

    static void Inet_pton(int family, const char *strptr, void *addrptr);

    static void str_cli(FILE *fp, int sockfd);

    static void Fputs(const char *ptr, FILE *stream);
    static char * Fgets(char *ptr, int n, FILE *stream);

    static ssize_t Readline(int fd, void *ptr, size_t maxlen);
    static ssize_t readline(int fd, void *vptr, size_t maxlen);
    static ssize_t my_read(int fd, char *ptr);


    static void logError(string error);
};

#endif //TIN_CLIENT_H
