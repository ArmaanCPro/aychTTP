#include "response_handler.h"
#include "tcp_server.h"

#include <unordered_map>
#include <string_view>
#include <chrono>
#include <format>
#include <fstream>

namespace aych
{
    namespace
    {
        struct HttpResponse
        {
            std::string version;
            std::string status_line;
            std::string body;
            std::string content_type = "text/plain; charset=utf-8";

            auto Write(tcp::socket& socket) const -> boost::asio::awaitable<void>
            {
                const std::string headers =
                    version + ' ' + status_line + "\r\n"
                    "Access-Control-Allow-Origin: *\r\n"
                    "Content-Type: " + content_type + "\r\n"
                    "Content-Length: " + std::to_string(body.size()) + "\r\n"
                    "Connection: close\r\n"
                    "\r\n";

                const std::string message = headers + body;

                co_await boost::asio::async_write(socket, boost::asio::buffer(message), boost::asio::use_awaitable);
            }
        };

        using path_handler_fn = boost::asio::awaitable<void> (*)(tcp::socket&, const HttpRequest&);
        const std::unordered_map<std::string_view, path_handler_fn> get_path_handlers = {
            {
                "/", [](tcp::socket& socket, const HttpRequest& request) -> boost::asio::awaitable<void> {
                    std::ifstream indexFile("test_client/client.html", std::ios::binary | std::ios::ate);
                    if (!indexFile.is_open() || !indexFile.good())
                    {
                        const HttpResponse response{request.version,
                            "500 Internal Server Error", "Failed to open client.html"};
                        co_await response.Write(socket);
                        co_return;
                    }

                    const auto size = indexFile.tellg();
                    std::string buffer(size, '\0');
                    indexFile.seekg(0);
                    indexFile.read(buffer.data(), size);

                    const HttpResponse response{
                        request.version, "200 OK",
                        buffer, "text/html; charset=utf-8"};

                    co_await response.Write(socket);
                }
            },
            {
                "/data", [](tcp::socket& socket, const HttpRequest& request) -> boost::asio::awaitable<void> {
                    const auto currentTime = std::chrono::system_clock::now();
                    const auto timeString = std::format("{:%Y-%m-%d %H:%M:%S}", currentTime);
                    const HttpResponse response{
                        request.version, "200 OK",
                        timeString};

                    co_await response.Write(socket);
                }
            }
        };

        using method_handler_fn = boost::asio::awaitable<void> (*)(tcp::socket&, const HttpRequest&);
        const std::unordered_map<std::string, method_handler_fn> method_handlers = {
            {
                "GET", [](tcp::socket& socket, const HttpRequest& request) -> boost::asio::awaitable<void> {
                    co_await get_path_handlers.at(request.path)(socket, request);
                }
            },
        };
    } // namespace

    namespace response_handler
    {
        auto handle(tcp::socket& socket, const HttpRequest& request) -> boost::asio::awaitable<void>
        {
            if (!method_handlers.contains(request.method) || !get_path_handlers.contains(request.path))
            {
                const HttpResponse response{request.version, "404 Not Found", "Not Found"};
                co_await response.Write(socket);
                co_return;
            }
            co_await method_handlers.at(request.method)(socket, request);
        }
    } // namespace response_handler
} // namespace aych
