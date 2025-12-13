#include <iostream>
#include "tcp_server.h"

static constexpr auto port = 3000u;

int main()
{
    try
    {
        aych::tcp_server server(port);
        for (;;)
        {
            server.run();
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
    }
    return 0;
}

