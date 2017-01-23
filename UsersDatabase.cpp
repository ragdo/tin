#include "UsersDatabase.h"

using namespace std;

UsersDatabase::UsersDatabase()
{
    readFile(fileStr);
}

UsersDatabase::UsersDatabase(string file_str)
{
    fileStr = file_str;
    readFile(fileStr);
}

UsersDatabase::~UsersDatabase()
{
    writeFile(fileStr);
}

bool UsersDatabase::addUser(string username, string password)
{
    user newUser;
    int vlen = users.size();
    for(int i=0; i<vlen; ++i)
    {
        if(users[i].username == username)
            return false;
    }
    newUser.username = username;
    newUser.password = password;
    newUser.services.clear();
    users.push_back(newUser);
    return true;
}

bool UsersDatabase::addUser(string username, string password, vector<string> services)
{
    user newUser;
    int vlen = users.size();
    for(int i=0; i<vlen; ++i)
    {
        if(users[i].username == username)
            return false;
    }
    newUser.username = username;
    newUser.password = password;
    newUser.services = services;
    users.push_back(newUser);
    return true;
}

void UsersDatabase::setService(string username, string setval)
{
    int vlen = users.size();
    for(int i=0; i<vlen; ++i)
    {
        if(users[i].username == username)
        {
            if(find(users[i].services.begin(),users[i].services.end(),setval) == users[i].services.end())
                users[i].services.push_back(setval);
            return;
        }
    }
}

bool UsersDatabase::readFile(string fileID)
{
    ifstream infile(fileID.c_str());
    if(!infile)
    {
        return false;
    }
    string line;
    while(getline(infile,line))
    {
        istringstream iss(line.c_str());
        string un, pw;
        string serv;
        vector<string> services;
        if(!(iss>>un>>pw))
        {
            return false;
        }
        while(iss>>serv)
        {
            cout<<serv<<endl;
            services.push_back(serv);
        }
        if(!(addUser(un,pw,services)))
        {
            return false;
        }
    }
    return true;
}

bool UsersDatabase::writeFile(string fileID)
{
    ofstream outfile(fileID.c_str());
    if(!outfile)
    {
        return false;
    }
    int len = users.size();
    for(int i=0; i<len; ++i)
    {
        outfile<<users[i].username<<" "<<users[i].password;
        int lenS = users[i].services.size();
        for(int j=0; j<lenS; ++j)
        {
            outfile << " " << users[i].services[j];
        }
        outfile<<endl;
    }
    return true;
}

bool UsersDatabase::checkPassword(string username, string password)
{
    int len = users.size();
    for(int i=0; i<len; ++i)
    {
        if(users[i].username == username)
        {
            if(users[i].password == password)
                return true;
            else
                return false;
        }
    }
    return false;
}

bool UsersDatabase::checkService(string username, string service)
{
    int len = users.size();
    for(int i=0; i<len; ++i)
    {
        if(users[i].username == username)
        {
            if(find(users[i].services.begin(),users[i].services.end(),service) == users[i].services.end())
                return false;
            else
                return true;
        }
    }
    return false;
}

