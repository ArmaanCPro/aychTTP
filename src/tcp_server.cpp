#include "tcp_server.h"

#include <iostream>
#include <thread>
#include "response_handler.h"

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

    void handler(tcp::socket& socket)
    {
        aych::HttpRequest request;

        boost::asio::streambuf request_buffer;
        boost::asio::read_until(socket, request_buffer, "\r\n\r\n");
        std::istream request_stream(&request_buffer);

        // parse request line
        request_stream >> request.method >> request.path >> request.version;

        // CORS preflight response
        if (request.method == "OPTIONS")
        {
            boost::asio::write(socket, boost::asio::buffer(CORS_RESPONSE));
            return;
        }

        std::cout << request.method << " " << request.path << " " << request.version << std::endl;

        aych::response_handler::handle(socket, request);

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
        tcp::socket socket(m_IoContext);

        m_Acceptor.accept(socket);
        handler(socket);
    }
} // namespace aych
