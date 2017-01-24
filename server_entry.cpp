#include "Server.h"
#include "Socket.h"

#include <iostream>


int main() {
    std::cout << "Starting server..." << Socket::Address() << std::endl;
    Server::tcpUdpEchoServer();
    return 0;
}
