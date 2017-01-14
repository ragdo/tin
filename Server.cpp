#include "Server.h"

#include <sys/socket.h>
#include <netinet/in.h>

#include <strings.h>

int Server::tcpEchoServer() {


        int					i, maxi, maxfd, listenfd, connfd, sockfd;
        int					nready, client[FD_SETSIZE];
        ssize_t				n;
        fd_set				rset, allset;
        char				buf[MAXLINE];
        socklen_t			clilen;
        struct sockaddr_in	cliaddr, servaddr;

        listenfd = Socket(AF_INET, SOCK_STREAM, 0);

        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family      = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port        = htons(SERV_PORT);

        Bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

        Listen(listenfd, LISTENQ);

        maxfd = listenfd;			/* initialize */
        maxi = -1;					/* index into client[] array */
        for (i = 0; i < FD_SETSIZE; i++)
            client[i] = -1;			/* -1 indicates available entry */
        FD_ZERO(&allset);
        FD_SET(listenfd, &allset);
/* end fig01 */

/* include fig02 */
        for ( ; ; ) {
            rset = allset;		/* structure assignment */
            nready = Select(maxfd+1, &rset, NULL, NULL, NULL);

            if (FD_ISSET(listenfd, &rset)) {	/* new client connection */
                clilen = sizeof(cliaddr);
                connfd = Accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
#ifdef	NOTDEF
                printf("new client: %s, port %d\n",
					Inet_ntop(AF_INET, &cliaddr.sin_addr, 4, NULL),
					ntohs(cliaddr.sin_port));
#endif

                for (i = 0; i < FD_SETSIZE; i++)
                    if (client[i] < 0) {
                        client[i] = connfd;	/* save descriptor */
                        break;
                    }
                if (i == FD_SETSIZE) {
                    logError("too many clients");
                    exit(-1);
                }

                FD_SET(connfd, &allset);	/* add new descriptor to set */
                if (connfd > maxfd)
                    maxfd = connfd;			/* for select */
                if (i > maxi)
                    maxi = i;				/* max index in client[] array */

                if (--nready <= 0)
                    continue;				/* no more readable descriptors */
            }

            for (i = 0; i <= maxi; i++) {	/* check all clients for data */
                if ( (sockfd = client[i]) < 0)
                    continue;
                if (FD_ISSET(sockfd, &rset)) {
                    if ( (n = Read(sockfd, buf, MAXLINE)) == 0) {
                        /*4connection closed by client */
                        Close(sockfd);
                        FD_CLR(sockfd, &allset);
                        client[i] = -1;
                    } else
                        Writen(sockfd, buf, n);

                    if (--nready <= 0)
                        break;				/* no more readable descriptors */
                }
            }
        }

/* end fig02 */





    return 0;
}

int Server::Socket(int family, int type, int protocol) {
    int n;

    if((n = socket(family, type, protocol)) < 0)
    {
        logError("socket error");
    }

    return n;
}

void Server::logError(string error) {

}

void Server::Bind(int fd, const struct sockaddr *sa, __socklen_t salen) {
    if(bind(fd, sa, salen) < 0)
    {
        logError("bind error");
    }
}

void Server::Listen(int fd, int backlog) {
    if(listen(fd, backlog) < 0)
    {
        logError("listen error");
    }
}

int Server::Accept(int fd, struct sockaddr *sa, socklen_t *salenptr) {
    int n;

    if((n = accept(fd, sa, salenptr)) < 0)
    {
        logError("accept error");
    }

    return n;

}

int Server::Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) {
    int n;

    if((n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0)
    {
        logError("select error");
    }

    return n;
}

ssize_t Server::writen(int fd, const void *vptr, size_t n) {
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr = (const char *) vptr;
    nleft = n;

    while(nleft > 0)
    {
        if((nwritten = write(fd, ptr, nleft)) <= 0)
        {
            if(nwritten < 0 && errno == EINTR)
            {
                nwritten = 0;
            }
            else
            {
                return -1;
            }
        }

        nleft -= nwritten;
        ptr += nwritten;
    }

    return n;
}

void Server::Writen(int fd, void *ptr, size_t nbytes) {
    if(writen(fd, ptr, nbytes) != nbytes)
    {
        logError("writen error");
    }
}

void Server::Close(int fd) {
    if(close(fd) == -1)
    {
        logError("close error");
    }
}

ssize_t Server::Read(int fd, void *ptr, size_t nbytes) {
    ssize_t n;

    if((n = read(fd, ptr, nbytes)) == -1)
    {
        logError("read error");
    }

    return n;
}
