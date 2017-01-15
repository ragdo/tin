#include "Client.h"
#include "Socket.h"

#include <cstring>

#include <arpa/inet.h>
#include <iostream>


int Client::tcpEchoClient(string serverIP)
{
    int sockDesc;
    struct sockaddr_in serverAddress = {};

    sockDesc = Socket::CreateSocket(AF_INET, SOCK_STREAM, 0);

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    Socket::InetPToN(AF_INET, serverIP.c_str(), &serverAddress.sin_addr);

    Socket::Connect(sockDesc, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

    str_cli(stdin, sockDesc);

    return 0;
}

void Client::str_cli(FILE *fp, int sockfd) {
    char sendline[LINE_LENGTH_LIMIT];
    char recvline[LINE_LENGTH_LIMIT];

    while(Fgets(sendline, LINE_LENGTH_LIMIT, fp) != NULL)
    {
        Socket::WriteBytes(sockfd, sendline, strlen(sendline));

        if(Socket::Readline(sockfd, recvline, LINE_LENGTH_LIMIT) == 0)
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

void Client::logError(string error) {
    std::cerr << "Error: " << error << std::endl;
}