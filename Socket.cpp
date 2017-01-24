#include "Socket.h"

#include <iostream>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <string.h>

string Socket::Address()
{
    struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;
    string ret = "";

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
    {

        if (ifa->ifa_addr->sa_family == AF_INET)
        { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            string add = addressBuffer;
            if(add.compare("127.0.0.1") != 0)
                ret = add;
        }

    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);

    return ret;
}


int Socket::read_cnt;
char* Socket::read_ptr;
char Socket::read_buf[LINE_LENGTH_LIMIT];

int Socket::CreateSocket(int family, int type, int protocol)
{
    int sockDesc;

    if((sockDesc = socket(family, type, protocol)) < 0)
    {
        logError("socket error");
    }

    return sockDesc;
}

void Socket::Bind(int sockDesc, const struct sockaddr *sockAddr, socklen_t sockAddrLen)
{
    if(bind(sockDesc, sockAddr, sockAddrLen) < 0)
    {
        logError("bind error");
    }
}

void Socket::Listen(int sockDesc, int backlog) {
    if(listen(sockDesc, backlog) < 0)
    {
        logError("listen error");
    }
}

int Socket::Accept(int sockDesc, struct sockaddr *sockAddr, socklen_t *sockAddrLenPtr)
{
    int newConnSockDesc;

    if((newConnSockDesc = accept(sockDesc, sockAddr, sockAddrLenPtr)) < 0)
    {
        logError("accept error");
    }

    return newConnSockDesc;

}

void Socket::Connect(int sockDesc, const struct sockaddr *sockAddr, socklen_t sockAddrLen)
{
    if(connect(sockDesc, sockAddr, sockAddrLen) < 0)
    {
        logError("connect error");
    }
}

void Socket::Close(int sockDesc)
{
    if(close(sockDesc) == -1)
    {
        logError("close error");
    }
}

void Socket::InetPToN(int family, const char *strPtr, void *addrPtr) {
    int status;

    if((status = inet_pton(family, strPtr, addrPtr)) < 0)
    {
        logError("inet_pton error");
    }
    else if(status == 0)
    {
        logError("inet_pton error");
        exit(-1);
    }
}

ssize_t Socket::writeBytes(int sockDesc, const void *sendBuffer, size_t bytesCount)
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

void Socket::WriteBytes(int sockDesc, void *sendBuffer, size_t bytesCount)
{
    if(writeBytes(sockDesc, sendBuffer, bytesCount) != bytesCount)
    {
        logError("writeBytes error");
    }
}

ssize_t Socket::Readline(int fd, void *ptr, size_t maxlen) {
    ssize_t n;

    if((n = readline(fd, ptr, maxlen)) < 0)
    {
        logError("readline error");
    }

    return n;
}

ssize_t Socket::readline(int fd, void *vptr, size_t maxlen) {
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

ssize_t Socket::my_read(int fd, char *ptr) {
    if (Socket::read_cnt <= 0) {
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

ssize_t Socket::Sendto(int sockfd,char *buf, size_t len, int flags, const struct sockaddr *dest, socklen_t addrlen)
{
    ssize_t n;
    if((n = sendto(sockfd,buf,len,flags,dest,addrlen)) < 0)
    {
        logError("sendto error");
    }
    return n;
}

ssize_t Socket::Recvfrom(int sfd, char *buf, size_t nbytes, int flags, struct sockaddr *from, socklen_t *addrlen)
{
    ssize_t n;
    if((n = recvfrom(sfd,buf,nbytes,flags,from,addrlen)) < 0)
    {
        logError("recvfrom error");
    }
    return n;
}

void Socket::logError(string error)
{
    std::cerr << "Error: " << error << std::endl;
}
