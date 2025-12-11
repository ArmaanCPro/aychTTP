#include <boost/asio.hpp>
#include <string_view>
#include <iostream>
#include "tcp_server.h"

using namespace boost::asio;

static constexpr auto port = 3000u;

int main()
{
    try
    {
        aych::tcp_server server(port);
        for (;;)
        {
            server.Run();
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
    }
    return 0;
}

