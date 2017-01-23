#include "TicketManager.h"

using namespace std;
/*
Rozmiar...
Wersja  IP
Rozmiar
Adres IP
Rozmiar
Port
Rozmiar
Czas ważności
Rozmiar
Skrót
*/
string TicketManager::createTicket(int IPver, string IPaddr, int port, int period)
{
    time_t now;
    now = time( NULL );
    long validTime = now + period;

    string IPverStr = Converter::toString(IPver);
    string portStr = Converter::toString(port);
    string validTimeStr = Converter::toString(validTime);

    string data = "";

    data += Converter::fill(Converter::toString(IPverStr.length()), '0', 3) + IPverStr;
    data += Converter::fill(Converter::toString(IPaddr.length()), '0', 3) + IPaddr;
    data += Converter::fill(Converter::toString(portStr.length()), '0', 3) + portStr;
    data += Converter::fill(Converter::toString(validTimeStr.length()), '0', 3) + validTimeStr;

    long int hash= Converter::hash(data.c_str());
    string hashStr = Converter::toString(hash);

    RSA rsa(293, 233);
    string encodedHash = rsa.encode(hashStr);

    data += Converter::fill(Converter::toString(encodedHash.length()), '0', 3) + encodedHash;

    return data;
}

string TicketManager::sub(string ticket, int &index)
{
    if(ticket.length() < index + 3)
        return "";

    int size = Converter::toInt(ticket.substr(index, 3));

    if(size < 1)
        return "";

    if(ticket.length() < index + 3 + size)
        return "";

    string ret = ticket.substr(index + 3, size);

    index += 3 + size;

    return ret;
}

/*
10 - sukces
20 - nieprawidłowe żądanie
21 - niepoprawne dane użytkownika
22 - brak uprawnień
23 - nieważny bilet
24 - zły adres IP
25 - niepoprawny bilet
*/
int TicketManager::checkTicket(string ticket, int IPver, string IPaddr, int port)
{
    int size;
    int index = 0;

    RSA rsa(293, 233);

    string IPverStrT = sub(ticket, index);
    if(IPverStrT.length() == 0 || IPverStrT.compare(Converter::toString(IPver)) != 0)
        return 25;

    string IPaddrT = sub(ticket, index);
    if(IPaddrT.length() == 0 || IPaddrT.compare(IPaddr) != 0)
        return 24;

    string portStrT = sub(ticket, index);
    if(portStrT.length() == 0 || portStrT.compare(Converter::toString(port)) != 0)
        return 22;

    time_t now;
    now = time( NULL );

    string timeT = sub(ticket, index);
    if(timeT.length() == 0 || Converter::toInt(timeT) < now)
        return 23;

    string data = ticket.substr(0, index);
    string hashStrTEncoded = sub(ticket, index);
    if(hashStrTEncoded.length() == 0)
        return 25;

    string hashStrT = rsa.decode(hashStrTEncoded);
    string hashStr = Converter::toString(Converter::hash(data.c_str()));

    if(hashStrT.compare(hashStr) != 0)
        return 25;

    return 10;
}

string TicketManager::printTicket(string ticket)
{
    int size;
    int index = 0;
    string ret = "";
    string IPverStrT = sub(ticket, index);
    string IPaddrT = sub(ticket, index);
    string portStrT = sub(ticket, index);
    string timeT = sub(ticket, index);
    long int tim = Converter::toInt(timeT);
    time_t now = time( NULL );
    bool valid = tim > now;


    ret += "Wersja IP: " + IPverStrT;
    ret += " adres IP: " + IPaddrT;
    ret += " usluga na porcie: " + portStrT;
    ret += " data waznosci: " + Converter::toTimeStr(tim);
    ret += " wazny: ";
    if(valid)
        ret += "tak\n";
    else
        ret += "nie\n";

    return ret;
}
