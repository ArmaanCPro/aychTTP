#include "tcp_server.h"

#include <iostream>
#include <thread>
#include <unordered_map>

using namespace std::literals;

namespace
{
    constexpr auto CORS_RESPONSE = R"(
    HTTP/1.1 200 OK
    Access-Control-Allow-Origin: *
    Access-Control-Allow-Methods: GET, POST, OPTIONS
    Access-Control-Allow-Headers: hx-request, hx-current-url, hx-target, hx-trigger
    Content-Length: 0
    Connection: keep-alive
    )"sv;

    using boost::asio::ip::tcp;
    struct HttpRequest
    {
        std::string method;
        std::string path;
        std::string version;
        std::unordered_map<std::string, std::string> headers;
        std::string body;
    };

    void handler(tcp::socket& socket)
    {
        HttpRequest request;

        boost::asio::streambuf request_buffer;
        boost::asio::read_until(socket, request_buffer, " ");
        std::istream request_stream(&request_buffer);
        request_stream >> request.method;

        if (request.method == "OPTIONS")
        {
            boost::asio::write(socket, boost::asio::buffer(CORS_RESPONSE));

            request_stream >> request.method >> request.path >> request.version;
        }

        // temporary
        std::cout << request_stream.rdbuf() << '\n';
        //request_buffer.consume(request_buffer.size());
        std::cout << request.method << " " << request.path << " " << request.version << std::endl;
    }
}

namespace aych
{
    tcp_server::tcp_server(uint32_t port)
        :
        m_Acceptor(m_IoContext, tcp::endpoint(tcp::v4(), static_cast<boost::asio::ip::port_type>(port)))
    {
    }

    void tcp_server::Run()
    {
        tcp::socket socket(m_IoContext);
        m_Acceptor.accept(socket);

        std::jthread t(handler, std::ref(socket));
    }
} // namespace aych
