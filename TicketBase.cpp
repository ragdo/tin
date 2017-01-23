#include "TicketBase.h"

using namespace std;

TicketBase::TicketBase()
{
    //ctor
}

TicketBase::~TicketBase()
{
    //dtor
}

void TicketBase::addTicket(string ticket)
{
    tickets.push_back(ticket);
}
void TicketBase::printTickets()
{
    for(int i=0; i<tickets.size(); ++i)
    {
        cout << TicketManager::printTicket(tickets[i]);
    }
}
string TicketBase::getTicket(int id)
{
    if(id < 0 || id >= tickets.size())
        return "";
    return tickets[id];
}

string TicketBase::chooseTicket()
{
    cout << "Choose ticket by id" << endl;
    string response;
    printTickets();
    getline(cin,response);
    int id = Converter::toInt(response);
    return getTicket(id);
}
