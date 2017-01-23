#ifndef TICKETBASE_H
#define TICKETBASE_H

#include "iostream"
#include "vector"
#include "string"
#include "TicketManager.h"

class TicketBase
{
    std::vector<std::string> tickets;
    public:
        TicketBase();
        virtual ~TicketBase();

        void addTicket(std::string ticket);
        void printTickets();
        std::string getTicket(int id);
        std::string chooseTicket();

    protected:
    private:
};

#endif // TICKETBASE_H
