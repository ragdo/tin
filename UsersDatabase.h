#ifndef USERSDATABASE_H
#define USERSDATABASE_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

struct user
{
    std::string username;
    std::string password;
    std::vector<std::string> services;
};

class UsersDatabase
{
    public:
        UsersDatabase();
        UsersDatabase(std::string file_str);
        virtual ~UsersDatabase();
        bool addUser(std::string username, std::string password);
        bool addUser(std::string username, std::string password, std::vector<std::string> services);
        void setService(std::string username, std::string setval);
        bool readFile(std::string fileID);
        bool writeFile(std::string fileID);
        bool checkPassword(std::string username, std::string password);
        bool checkService(std::string username, std::string service);
    protected:
    private:
        std::vector<user> users;
        std::string fileStr;
};

#endif // USERSDATABASE_H
