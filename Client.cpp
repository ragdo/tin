#include <netinet/in.h>
#include <arpa/inet.h>

#include <strings.h>
#include <cstring>
#include "Client.h"

int Client::read_cnt;
char* Client::read_ptr;
char Client::read_buf[MAXLINE];

int Client::tcpEchoClient(char *ip) {
    int					sockfd;
    struct sockaddr_in	servaddr;


    sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);

    Inet_pton(AF_INET, ip, &servaddr.sin_addr);

    Connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    str_cli(stdin, sockfd);		/* do it all */

    exit(0);
}

void Client::logError(string error) {

}

int Client::Socket(int family, int type, int protocol) {
    int n;

    if((n = socket(family, type, protocol)) < 0)
    {
        logError("socket error");
    }

    return n;
}

void Client::Connect(int fd, const struct sockaddr *sa, socklen_t salen) {
    if(connect(fd, sa, salen) < 0)
    {
        logError("connect error");
    }
}

void Client::Inet_pton(int family, const char *strptr, void *addrptr) {
    int n;

    if((n = inet_pton(family, strptr, addrptr)) < 0)
    {
        logError("inet_pton error");
    }
    else if(n == 0)
    {
        logError("inet_pton error");
        exit(-1);
    }
}

void Client::str_cli(FILE *fp, int sockfd) {
    char sendline[MAXLINE];
    char recvline[MAXLINE];

    while(Fgets(sendline, MAXLINE, fp) != NULL)
    {
        Server::Writen(sockfd, sendline, strlen(sendline));

        if(Readline(sockfd, recvline, MAXLINE) == 0)
        {
            logError("str_cli: server terminated prematurely");
        }

        Fputs(recvline, stdout);
    }

}

void Client::Fputs(const char *ptr, FILE *stream) {
    if(fputs(ptr, stream) == EOF)
    {
        logError("fputs error");
    }
}

char *Client::Fgets(char *ptr, int n, FILE *stream) {
    char *rptr;

    if((rptr = fgets(ptr, n, stream)) == NULL && ferror(stream))
    {
        logError("fgets error");
    }

    return rptr;
}

ssize_t Client::Readline(int fd, void *ptr, size_t maxlen) {
    ssize_t n;

    if((n = readline(fd, ptr, maxlen)) < 0)
    {
        logError("readline error");
    }

    return n;
}

ssize_t Client::readline(int fd, void *vptr, size_t maxlen) {
    ssize_t n;
    ssize_t rc;
    char c;
    char *ptr;

    ptr = (char*) vptr;

    for (n = 1; n < maxlen; n++) {
        if ( (rc = my_read(fd, &c)) == 1) {
            *ptr++ = c;
            if (c == '\n')
                break;	/* newline is stored, like fgets() */
        } else if (rc == 0) {
            *ptr = 0;
            return(n - 1);	/* EOF, n - 1 bytes were read */
        } else
            return(-1);		/* error, errno set by read() */
    }

    *ptr = 0;	/* null terminate like fgets() */
    return(n);


}

ssize_t Client::my_read(int fd, char *ptr) {
    if (Client::read_cnt <= 0) {
        again:
        if ( (read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
            if (errno == EINTR)
                goto again;
            return(-1);
        } else if (read_cnt == 0)
            return(0);
        read_ptr = read_buf;
    }

    read_cnt--;
    *ptr = *read_ptr++;
    return(1);
}
