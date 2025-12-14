#pragma once

#include <boost/asio.hpp>

namespace aych
{
    using boost::asio::ip::tcp;

    class tcp_server
    {
    public:
        tcp_server(uint32_t port);

        void run();
    private:
        boost::asio::io_context m_IoContext;
        tcp::acceptor m_Acceptor;
    };
}