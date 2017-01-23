cmake_minimum_required(VERSION 3.6)
project(TIN)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Server.cpp Server.h Client.cpp Client.h Socket.cpp Socket.h Socket.cpp Socket.h udp_client_entry.cpp TicketManager.cpp TicketManager.h UsersDatabase.cpp UsersDatabase.h RSA.cpp RSA.h Converter.h)
add_executable(TIN ${SOURCE_FILES})
add_executable(server_entry server_entry.cpp Server.cpp Server.h Socket.cpp Socket.h TicketManager.cpp UsersDatabase.cpp RSA.cpp)
add_executable(tcp_client_entry tcp_client_entry.cpp Client.cpp Client.h Server.cpp Server.h Socket.cpp Socket.h TicketManager.cpp UsersDatabase.cpp RSA.cpp)
add_executable(udp_client_entry udp_client_entry.cpp Client.cpp Client.h Server.cpp Server.h Socket.cpp Socket.h TicketManager.cpp UsersDatabase.cpp RSA.cpp)