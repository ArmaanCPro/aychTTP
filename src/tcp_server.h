#pragma once

#include <boost/asio.hpp>
#include <cstdint>

namespace aych
{
    using boost::asio::ip::tcp;

    class tcp_server
    {
    public:
        tcp_server(uint32_t port);

        void Run();
    private:
        boost::asio::io_context m_IoContext;
        tcp::acceptor m_Acceptor;
    };
}