#ifndef TICKETMANAGER_H_INCLUDED
#define TICKETMANAGER_H_INCLUDED

#include <iostream>
#include <time.h>
#include "RSA.h"
#include "Converter.h"

class TicketManager
{
public:
    static string createTicket(int IPver, string IPaddr, int port, int period);

    static int checkTicket(string ticket, int IPver, string IPaddr, int port);

    static string printTicket(string ticket);

    static string sub(string ticket, int &index);
};


#endif // TICKETMANAGER_H_INCLUDED
