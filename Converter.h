#ifndef CONVERTER_H_INCLUDED
#define CONVERTER_H_INCLUDED

#include <cstdio>
#include <ctime>
#include <sstream>

using namespace std;

class Converter
{
public:
    static string toString(long int num)
    {
        ostringstream oss;
        oss << num;
        string str = oss.str();
        return str;
    }

    static long int toInt(string str)
    {
        for(int i = 0; i < (int)str.length(); i++)
        {
            if(!isdigit(str.c_str()[i]))
                return -1;
        }
        long int num;
        istringstream iss(str);
        iss >> num;
        return num;
    }

    static string fill(string str, char c, int length)
    {
        int size = str.length();

        for(int i = size; i < length; i++)
            str = c + str;

        return str;
    }

    static unsigned int hash( const char* s, unsigned int seed = 0)
    {
        unsigned hash = seed;
        while (*s)
        {
            hash = hash * 101  +  *s++;
        }
        return hash;
    }

    static string toTimeStr(time_t seconds)
    {
        //time(&seconds);
        tm* timeInfo;
        char buffer[80];

        //time(&seconds);
        timeInfo = localtime(&seconds);

        strftime(buffer,80,"%d-%m-%Y : H:%H min:%M sec:%S",timeInfo);

        return buffer;
    }
};

#endif // CONVERTER_H_INCLUDED
