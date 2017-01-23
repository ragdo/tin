#ifndef TIN_CLIENT_H
#define TIN_CLIENT_H

#include <string>
#include "Converter.h"
#include "RSA.h"
#include "TicketBase.h"
using std::string;

#define LINE_LENGTH_LIMIT 4096 // unify with server's one
#define SERVER_PORT 34000 // unify with server's one
#define SERVER_PORT_TCP_ECHO 34001
#define SERVER_PORT_UDP_ECHO 34002
#define SERVER_PORT_UDP_TIME 34003
#define SERVER_PORT_UDP_TICKET 34004
#define ECHO_PORT 2007
#define TIME_PORT 2013


class Client {
    int e, n;
    TicketBase *ticketBase;
public:
    Client();
    ~Client();

    static int tcpEchoClient(string serverIP);
    int udpEchoClient(string serverIP);

    static void str_cli(FILE *fp, int sockfd);

    static string ticketRequest(string username, string password, int port);
    static string logIn();

    static void Fputs(const char *ptr, FILE *stream);
    static char * Fgets(char *ptr, int n, FILE *stream);

    static void logError(string error);

};


#endif //TIN_CLIENT_H
