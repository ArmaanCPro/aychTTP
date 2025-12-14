module;

#include "asio_helper.h"

export module aych:tcp_server;

import :http_request;

namespace aych
{
    using boost::asio::ip::tcp;

    export class tcp_server
    {
    public:
        tcp_server(uint32_t port);

        void run();
    private:
        boost::asio::io_context m_IoContext;
        tcp::acceptor m_Acceptor;
    };
}