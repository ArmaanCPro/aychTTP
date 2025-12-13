#pragma once

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>

namespace aych
{
    struct HttpRequest;
    using boost::asio::ip::tcp;

    namespace response_handler
    {
        auto handle(tcp::socket& socket, const HttpRequest& request) -> boost::asio::awaitable<void>;
    };
}