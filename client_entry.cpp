#include "Client.h"
#include "Socket.h"


#include <iostream>


int main() {
    std::cout << "Starting client..." << Socket::Address() << std::endl;
    string add;
    cin >> add;
    Client client;
    client.runClient(add);
    //client.tcpEchoClient("127.0.0.1");
    return 0;
}

