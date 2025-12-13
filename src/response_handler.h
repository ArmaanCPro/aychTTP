#pragma once

#include <boost/asio.hpp>

namespace aych
{
    struct HttpRequest;
    using boost::asio::ip::tcp;

    namespace response_handler
    {
        auto handle(tcp::socket& socket, const HttpRequest& request) -> void;
    };
}