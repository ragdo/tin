#include "Client.h"
#include "Socket.h"
#include "Server.h"

#include <cstring>

#include <arpa/inet.h>
#include <iostream>

Client::Client()
{
    string add = Socket::Address();
    ticketBase = new TicketBase();
    ticketBase->addTicket(TicketManager::createTicket(7, add, 2013, 300));
    ticketBase->addTicket(TicketManager::createTicket(4, "1.2.3.4", 2013, 300));
    ticketBase->addTicket(TicketManager::createTicket(4, add, 2005, 300));
    string ticket = TicketManager::createTicket(6, add, 2013, 300);
    ticket[3] = '4';
    ticketBase->addTicket(ticket);
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
    serverAddress.sin_port = htons(SERVER_PORT_ECHO);
    Socket::InetPToN(AF_INET, serverIP.c_str(), &serverAddress.sin_addr);

    Socket::Connect(sockDesc, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

    str_cli(stdin, sockDesc);

    return 0;
}

int Client::runClient(string serverIP)
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
            serverAddress.sin_port = htons(SERVER_PORT_TICKET);
            Socket::Sendto(sockDesc,buffer,nBytes,0,(struct sockaddr *)&serverAddress,addr_size);
            bzero(buffer, buflen);
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
            string request = "004SRVC";
            int ticketSize = ticket.length();
            string ticketSizeStr = Converter::toString(ticketSize);
            request += Converter::fill(ticketSizeStr,'0',3);
            request += ticket;

            int index = 0;
            TicketManager::sub(ticket, index);
            TicketManager::sub(ticket, index);
            string portString = TicketManager::sub(ticket, index);
            int portInt = Converter::toInt(portString);


            string message;
            int messageSize = 0;
            string messageSizeStr;

            switch(portInt)
            {
                case SERVER_PORT_ECHO:
                    do
                    {
                        cout << "Use (t)cp or (u)dp?" <<  endl;
                        getline(cin,c);
                        fflush(stdin);
                    }while(c != "t" && c != "u");
                    if(c == "t")
                    {
                        tcpEchoClient("127.0.0.1");
                    }
                    else
                    {
                        cout << "Write down your message in a single line." << endl;
                        getline(cin,message);
                        messageSize = message.length();
                        messageSizeStr = Converter::toString(messageSize);
                        request += Converter::fill(messageSizeStr,'0',3);
                        request += message;
                        serverAddress.sin_port = htons(SERVER_PORT_ECHO);
                    }
                    break;
                case SERVER_PORT_TIME:
                    serverAddress.sin_port = htons(SERVER_PORT_TIME);
                    break;

                default:
                    logError("Error: Unexpected service port in ticket.");
                    break;
            }



            cout << "Client sent: " + request << endl;
            int nBytes = request.length(); //????????????????????????????
            strcpy(buffer, request.c_str());
            Socket::Sendto(sockDesc,buffer,nBytes,0,(struct sockaddr *)&serverAddress,addr_size);
            bzero(buffer,strlen(buffer));
            fd_set readSet;
            fd_set allSet;
            FD_ZERO(&allSet);
            FD_SET(sockDesc, &allSet);
            timeval tv;
            tv.tv_sec = 1;
            tv.tv_usec = 0;
            int tries = 1;
            while(true)
            {
                readSet = allSet;

                int readySockCount = Server::Select(sockDesc + 1,
                                        &readSet,
                                        nullptr,
                                        nullptr,
                                        &tv);
                if(readySockCount == 0)
                {
                    cout << "Timeout: "  << tries << " try" << endl;
                    if(tries > 3)
                        break;
                    Socket::Sendto(sockDesc,buffer,nBytes,0,(struct sockaddr *)&serverAddress,addr_size);
                    tries++;
                    tv.tv_sec++;
                }
                else
                    break;
            }

            if(tries > 3)
            {
                cout << "Failed waiting for response" << endl;
                continue;
            }

            nBytes = Socket::Recvfrom(sockDesc,buffer,buflen,0,NULL,NULL);
            string res = buffer;
            cout << "Client received: " << res << endl;
            int resCode = Converter::toInt(res.substr(0, 2));
            string resData;
            index = 2;

            switch(resCode)
            {
                case 10:
                    resData = TicketManager::sub(res, index);
                    cout << "Data: " << resData << endl;
                    break;
                case 25:
                    cout << "Bad ticket" <<  endl;
                    break;
                case 24:
                    cout << "IP address does not match" << endl;
                    break;
                case 23:
                    cout << "Ticket expired" << endl;
                    break;
                case 22:
                    cout << "Service not allowed" << endl;
                    break;
                default:
                    cout << "Bad request" << endl;
            }
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
