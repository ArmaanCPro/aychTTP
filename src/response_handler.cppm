module;

#include "asio_helper.h"

export module aych:response_handler;

import :http_request;

namespace aych
{
    using boost::asio::ip::tcp;
    export namespace response_handler
    {
        auto handle(tcp::socket& socket, const HttpRequest& request) -> boost::asio::awaitable<void>;
    } // namespace response_handler
} // namespace aych
