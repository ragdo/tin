#include "Client.h"
#include "Socket.h"


#include <iostream>


int main() {
    std::cout << "Starting client..." << Socket::Address() << std::endl;
    Client client;
    client.runClient("127.0.0.1");
    //client.tcpEchoClient("127.0.0.1");
    return 0;
}

