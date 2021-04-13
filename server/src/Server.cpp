#include "Socket.h"
#include "Config.h"

#include <iostream>

bool acceptor(Socket& client)
{
    bool active = client.Write("Hello client.", 14);
    return true;
}

bool processor(Socket& client)
{
    char buf[1024] = {0};
    bool active = client.Read(buf, 1024);
    if (active)
    {
        std::cout << client.getIpStr() << ": " << buf << std::endl;
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
