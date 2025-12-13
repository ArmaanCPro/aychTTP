#include "response_handler.h"
#include "tcp_server.h"

#include <unordered_map>
#include <string_view>
#include <chrono>
#include <format>
#include <fstream>
#include <iostream>

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

            void Write(tcp::socket& socket) const
            {
                boost::system::error_code ignored_error{};
                const std::string headers =
                    version + ' ' + status_line + "\r\n"
                    "Access-Control-Allow-Origin: *\r\n"
                    "Content-Type: " + content_type + "\r\n"
                    "Content-Length: " + std::to_string(body.size()) + "\r\n"
                    "Connection: close\r\n"
                    "\r\n";

                const std::string message = headers + body;
                boost::asio::write(socket, boost::asio::buffer(message), ignored_error);
            }
        };

        using path_handler_fn = void (*)(tcp::socket&, const HttpRequest&);
        const std::unordered_map<std::string_view, path_handler_fn> get_path_handlers = {
            {
                "/", [](tcp::socket& socket, const HttpRequest& request) {
                    std::ifstream indexFile("test_client/client.html", std::ios::binary | std::ios::ate);
                    if (!indexFile.is_open() || !indexFile.good())
                    {
                        const HttpResponse response{request.version,
                            "500 Internal Server Error", "Failed to open client.html"};
                        response.Write(socket);
                        return;
                    }

                    const auto size = indexFile.tellg();
                    std::string buffer(size, '\0');
                    indexFile.seekg(0);
                    indexFile.read(buffer.data(), size);

                    const HttpResponse response{
                        request.version, "200 OK",
                        buffer, "text/html; charset=utf-8"};
                    response.Write(socket);
                }
            },
            {
                "/data", [](tcp::socket& socket, const HttpRequest& request) {
                    const auto currentTime = std::chrono::system_clock::now();
                    const auto timeString = std::format("{:%Y-%m-%d %H:%M:%S}", currentTime);
                    const HttpResponse response{
                        request.version, "200 OK",
                        timeString};
                    response.Write(socket);
                }
            }
        };

        using method_handler_fn = void (*)(tcp::socket&, const HttpRequest&);
        const std::unordered_map<std::string, method_handler_fn> method_handlers = {
            {
                "GET", [](tcp::socket& socket, const HttpRequest& request) {
                    get_path_handlers.at(request.path)(socket, request);
                }
            },
        };
    } // namespace

    response_handler::response_handler(tcp::socket& socket, const HttpRequest& request)
    {
        if (!method_handlers.contains(request.method) || !get_path_handlers.contains(request.path))
        {
            const HttpResponse response{request.version, "404 Not Found", "Not Found"};
            response.Write(socket);
            return;
        }
        method_handlers.at(request.method)(socket, request);
    }
} // namespace aych
