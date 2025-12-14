#include <iostream>
#include <boost/system/error_code.hpp>
#include <boost/asio.hpp>

import aych;

static constexpr auto port = 3000u;

int main()
{
    try
    {
        aych::tcp_server server(port);

        server.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
    }
    return 0;
}

