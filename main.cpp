#include <iostream>
#include <string>

#include "UsersDatabase.h"

int main() {

    UsersDatabase db;
    std::vector<std::string> v;
    v.push_back("echo");
    v.push_back("time");
    db.addUser("daniel", "haslo", v);
    db.writeFile("./users");

    return 0;
}