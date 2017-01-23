#include "Client.h"
#include "Socket.h"

#include <cstring>

#include <arpa/inet.h>
#include <iostream>

Client::Client()
{
    ticketBase = new TicketBase();
}
Client::~Client()
{
    delete ticketBase;
}

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
    int nBytes;
    string c;
    char trash;
    sockDesc = Socket::CreateSocket(AF_INET, SOCK_DGRAM, 0);

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);
    Socket::InetPToN(AF_INET, serverIP.c_str(), &serverAddress.sin_addr);

    memset(serverAddress.sin_zero, '\0', sizeof serverAddress.sin_zero);

    socklen_t addr_size = sizeof serverAddress;
    while(1)
    {
        fflush(stdin);
        cout << "Do you want to request (s)ervice, get (t)icket or (q)uit? " << endl;
        getline(cin,c);
        if(c == "t") // ticket
        {
            string login = logIn();
            cout << login << endl;
            nBytes = login.length(); //??????????????????????????????
            strcpy(buffer, login.c_str());
            Socket::Sendto(sockDesc,buffer,nBytes,0,(struct sockaddr *)&serverAddress,addr_size);
            bzero(buffer,strlen(buffer));
            nBytes = Socket::Recvfrom(sockDesc,buffer,buflen,0,NULL,NULL);

            printf("Client received from server: %s\n",buffer);
            string bufferString = buffer;
            if(bufferString.substr(0,2) == "10")
            {
                cout << "Received ticket." << endl;
                bufferString = bufferString.substr(2,bufferString.length()-2);
                ticketBase->addTicket(bufferString);
                ticketBase->printTickets();
            }
            else if (bufferString.substr(0,2) == "21")
            {
                cout << "Wrong username or password" << endl;
            }
            else if (bufferString.substr(0,2) == "22")
            {
                cout << "Service not allowed" << endl;
            }
        }
        else if(c == "s") // service
        {
            string ticket = ticketBase->chooseTicket();
            if(ticket == "")
            {
                logError("Couldn't find a ticket!");
                continue;
            }
            string response = "004SRVC";
            int ticketSize = ticket.length();
            string ticketSizeStr = Converter::toString(ticketSize);
            response += Converter::fill(ticketSizeStr,'0',3);
            response += ticket;

            cout << "Write down your message in a single line." << endl;
            string message;
            getline(cin,message);

            int messageSize = message.length();
            string messageSizeStr = Converter::toString(messageSize);
            response += Converter::fill(messageSizeStr,'0',3);
            response += message;
            cout << "Response: " + response << endl;
            int nBytes = response.length(); //????????????????????????????
            strcpy(buffer, response.c_str());
            Socket::Sendto(sockDesc,buffer,nBytes,0,(struct sockaddr *)&serverAddress,addr_size);
            bzero(buffer,strlen(buffer));
            nBytes = Socket::Recvfrom(sockDesc,buffer,buflen,0,NULL,NULL);
            cout << buffer << endl;
        }
        else if(c == "q")
        {
            break;
        }
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

    RSA* rsa = new RSA(293,233);
    string code = rsa->encode(password,3,68269);

    data += Converter::fill(Converter::toString(tckt.length()), '0', 3) + tckt;
    data += Converter::fill(Converter::toString(username.length()), '0', 3) + username;
    data += Converter::fill(Converter::toString(code.length()), '0', 3) + code;
    data += Converter::fill(Converter::toString(portStr.length()), '0', 3) + portStr;

    return data;
}

string Client::logIn()
{
    string username = "";
    string password = "";
    string portStr = "";

    cout << "Type username:" << endl;
    getline(cin,username);
    cout << "Type password:" << endl;
    getline(cin,password);
    cout << "Type port number (2007 - echo, 2013 - time)" << endl;
    getline(cin,portStr);
    int port = Converter::toInt(portStr);

    string ticket = ticketRequest(username,password,port);
    //delete rsa;
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
