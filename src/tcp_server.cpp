module;

#include "asio_helper.h"
#include <iostream>
#include <coroutine>

module aych;

//import :response_handler;

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
        spawn_server_loop(m_IoContext, m_Acceptor, handler);
        m_IoContext.run();
    }
} // namespace aych
