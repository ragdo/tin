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

int Client::udpEchoClient(string serverIP)
{
    int sockDesc;
    int buflen = 1024;
    char buffer[buflen];
    struct sockaddr_in serverAddress = {};

    sockDesc = Socket::CreateSocket(AF_INET, SOCK_DGRAM, 0);

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    Socket::InetPToN(AF_INET, serverIP.c_str(), &serverAddress.sin_addr);

    memset(serverAddress.sin_zero, '\0', sizeof serverAddress.sin_zero);

    socklen_t addr_size = sizeof serverAddress;

    while(1)
    {
        fflush(stdin);
        /*
        printf("Type message:");
        Fgets(buffer,buflen,stdin);
        int nBytes = strlen(buffer) + 1;
        printf("You typed: %s",buffer);
        */
        string login = logIn();
        cout << login << endl;
        int nBytes = login.length() + 1;
        //char *buf = new char[];
        strcpy(buffer, login.c_str());
        Socket::Sendto(sockDesc,buffer,nBytes,0,(struct sockaddr *)&serverAddress,addr_size);
        bzero(buffer,strlen(buffer));
        nBytes = Socket::Recvfrom(sockDesc,buffer,buflen,0,NULL,NULL);

        printf("Client received from server: %s\n",buffer);
    }
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

string Client::ticketRequest(string username, string password, int port)
{
    string portStr = Converter::toString(port);
    string tckt = "TCKT";

    string data = "";

    data += Converter::fill(Converter::toString(tckt.length()), '0', 3) + tckt;
    data += Converter::fill(Converter::toString(username.length()), '0', 3) + username;
    data += Converter::fill(Converter::toString(password.length()), '0', 3) + password;
    data += Converter::fill(Converter::toString(portStr.length()), '0', 3) + portStr;

    return data;
}

string Client::logIn()
{
    string username = "";
    string password = "";

    cout << "Type username:" << endl;
    getline(cin,username);
    cout << "Type password:" << endl;
    getline(cin,password);
    RSA* rsa = new RSA(293,233);
    string code = rsa->encode(password);
    cout << code <<endl;
    cout << rsa->getE() << endl;
    cout << rsa->getN() << endl;
    //string decode = rsa->decode(code);
    //cout << decode <<endl;
    string ticket = ticketRequest(username,code,7);
    delete rsa;
    return ticket;
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
