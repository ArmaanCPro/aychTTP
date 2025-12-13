#pragma once

#include <boost/asio.hpp>

namespace aych
{
    struct HttpRequest;
    using boost::asio::ip::tcp;

    class response_handler
    {
    public:
        response_handler(tcp::socket& socket, const HttpRequest& request);
    };
}