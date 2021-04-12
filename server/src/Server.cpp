#include "Socket.h"
#include "Config.h"

#include <iostream>

bool acceptor(Socket& client)
{
    std::cout << client.getIpStr() << " connect." << std::endl;
    return true;
}

bool processor(Socket& client)
{
    bool active = client.Write("Hello client.", 14);
    if (!active)
    {
        std::cout << client.getIpStr() << " has left." << std::endl;
    }
    return active;
}

int main()
{
    EpollServer server;
    server.setAcceptor(acceptor);
    server.setProcessor(processor);
    
    try
    {
        server.Run(SERVER_PORT);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
