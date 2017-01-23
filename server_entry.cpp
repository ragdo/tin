#include "Server.h"

#include <iostream>


int main() {
    std::cout << "Starting server..." << std::endl;
    Server::tcpUdpEchoServer();
    return 0;
}
