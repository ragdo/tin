#include <iostream>

#include "Server.h"

int main() {
    std::cout << "Starting server..." << std::endl;
    Server::tcpEchoServer();
    return 0;
}
