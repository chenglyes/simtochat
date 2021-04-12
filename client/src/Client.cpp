#include "Socket.h"

#include <iostream>

int main()
{
    Socket client;
    char buf[1024] = {0};
    try
    {
        client.Connect("127.0.0.1", 8010);
        client.Read(buf, 1024);
        std::cout << buf << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    
    return 0;
}
