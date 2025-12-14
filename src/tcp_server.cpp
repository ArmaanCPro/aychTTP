#include "tcp_server.h"

#include <iostream>
#include <thread>

#include "response_handler.h"
#include "http_request.h"

#include <coroutine>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>

using namespace std::literals;

namespace
{
    constexpr auto CORS_RESPONSE = "HTTP/1.1 200 OK\r\n"
    "Access-Control-Allow-Origin: *\r\n"
    "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
    "Access-Control-Allow-Headers: HX-Request, HX-Current-Url, HX-Target, HX-Trigger, HX-Get\r\n"
    "Access-Control-Max-Age: 86400\r\n"
    "Content-Length: 0\r\n"
    "Connection: close\r\n"
    "\r\n"sv;

    using boost::asio::ip::tcp;

    boost::asio::awaitable<void> handler(tcp::socket socket)
    {
        aych::HttpRequest request;

        boost::asio::streambuf request_buffer;
        co_await boost::asio::async_read_until(socket, request_buffer, "\r\n\r\n", boost::asio::use_awaitable);
        std::istream request_stream(&request_buffer);

        // parse request line
        request_stream >> request.method >> request.path >> request.version;

        // CORS preflight response
        if (request.method == "OPTIONS")
        {
            co_await boost::asio::async_write(socket, boost::asio::buffer(CORS_RESPONSE), boost::asio::use_awaitable);
            co_return;
        }

        std::cout << request.method << " " << request.path << " " << request.version << std::endl;

        co_await aych::response_handler::handle(socket, request);

        boost::system::error_code ignored_error;
        socket.shutdown(tcp::socket::shutdown_both, ignored_error);
        socket.close(ignored_error);
    }
}

namespace aych
{
    tcp_server::tcp_server(uint32_t port)
        :
        m_Acceptor(m_IoContext, tcp::endpoint(tcp::v4(), static_cast<boost::asio::ip::port_type>(port)))
    {
    }

    void tcp_server::run()
    {
        boost::asio::co_spawn(m_IoContext,
            [this]() -> boost::asio::awaitable<void>
            {
                for (;;)
                {
                    tcp::socket socket(m_IoContext);
                    co_await m_Acceptor.async_accept(socket, boost::asio::use_awaitable);

                    boost::asio::co_spawn(m_IoContext, handler(std::move(socket)), boost::asio::detached);
                }
            }, boost::asio::detached);

        m_IoContext.run();
    }
} // namespace aych
