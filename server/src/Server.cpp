#include "Socket.h"
#include "Config.h"

#include <iostream>

int main()
{
    SingleServer server;
    
    try
    {
        server.Listen(SERVER_PORT);
        while (true)
        {
            Socket client = server.Accept();
            client.Write("Hello client!", 14);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
