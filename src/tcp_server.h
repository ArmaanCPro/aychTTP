#pragma once

#include <boost/asio.hpp>
#include <cstdint>

namespace aych
{
    using boost::asio::ip::tcp;

    struct HttpRequest
    {
        std::string method;
        std::string path;
        std::string version;
    };

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