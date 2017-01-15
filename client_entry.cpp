#include "Client.h"

#include <iostream>


int main() {
    std::cout << "Starting client..." << std::endl;
    Client::tcpEchoClient("127.0.0.1");
    return 0;
}

