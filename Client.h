#ifndef TIN_CLIENT_H
#define TIN_CLIENT_H

#include <string>

using std::string;

#define LINE_LENGTH_LIMIT 4096 // unify with server's one
#define SERVER_PORT 34000 // unify with server's one


class Client {
public:
    static int tcpEchoClient(string serverIP);

    static void str_cli(FILE *fp, int sockfd);

    static void Fputs(const char *ptr, FILE *stream);
    static char * Fgets(char *ptr, int n, FILE *stream);

    static void logError(string error);
};


#endif //TIN_CLIENT_H
