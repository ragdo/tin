cmake_minimum_required(VERSION 3.6)
project(TIN)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Server.cpp Server.h Client.cpp Client.h)
add_executable(TIN ${SOURCE_FILES})
add_executable(server_entry server_entry.cpp Server.cpp Server.h)
add_executable(client_entry client_entry.cpp Client.cpp Client.h Server.cpp Server.h)