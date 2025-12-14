#include "response_handler.h"
#include "tcp_server.h"

#include <unordered_map>
#include <map>
#include <string_view>
#include <chrono>
#include <format>

#include <boost/asio/stream_file.hpp>

#include <nlohmann/json.hpp>

// inja has a warning with msvc
#pragma warning(push)
#pragma warning(disable: 4702)
#include <inja/inja.hpp>
#pragma warning(pop)

using namespace std::literals;

namespace aych
{
    namespace
    {
        struct HttpResponse
        {
        private:
            std::string version{};
            int status{0};
            std::string status_line{};
            std::string body{};
            mutable std::map<std::string, std::string> headers{};
        public:
            HttpResponse() = default;

            auto set_version(std::string new_version) -> HttpResponse& { this->version = std::move(new_version); return *this; }
            auto set_status(int new_status, std::string new_status_line) -> HttpResponse& { this->status = new_status; this->status_line = std::move(new_status_line); return *this; }
            auto set_header(std::string header, std::string value) -> HttpResponse& { headers[std::move(header)] = std::move(value); return *this; }
            auto set_body(std::string new_body) -> HttpResponse& { this->body = std::move(new_body); return *this; }

            auto write(tcp::socket& socket) const -> boost::asio::awaitable<void>
            {
                std::string message = version + ' ' + std::to_string(status) + ' ' + status_line + "\r\n";

                if (!headers.contains("Content-Type"))
                    headers["Content-Type"] = "text/plain; charset=utf-8";
                if (!headers.contains("Content-Length"))
                    headers["Content-Length"] = std::to_string(body.size());
                if (!headers.contains("Access-Control-Allow-Origin"))
                    headers["Access-Control-Allow-Origin"] = "*";

                for (const auto& [key, value] : headers)
                {
                    message += key + ": "s.append(value).append("\r\n");
                }
                message += "\r\n" + body;

                co_await boost::asio::async_write(socket, boost::asio::buffer(message), boost::asio::use_awaitable);
            }
        };

        using path_handler_fn = boost::asio::awaitable<HttpResponse> (*)(tcp::socket&, const HttpRequest&);
        const std::unordered_map<std::string_view, path_handler_fn> path_handlers = {
            {
                "/", [](tcp::socket& socket, const HttpRequest& request) -> boost::asio::awaitable<HttpResponse> {

                    if (request.method != "GET")
                    {
                        const auto response = HttpResponse{}.set_version(request.version)
                            .set_status(405, "Method Not Allowed")
                            .set_body("Method Not Allowed");
                        co_return response;
                    }

                    boost::asio::stream_file file(socket.get_executor());

                    boost::system::error_code ec;
                    file.open("test_client/client.html", boost::asio::stream_file::read_only, ec);

                    if (ec)
                    {
                        const auto response = HttpResponse{}.set_version(request.version)
                            .set_status(500, "Internal Server Error")
                            .set_body("Failed to open client.html\n" + ec.message());
                        co_return response;
                    }

                    const auto size = file.seek(0, boost::asio::stream_file::seek_end);
                    file.seek(0, boost::asio::stream_file::seek_set);

                    std::string buffer(size, '\0');
                    buffer.resize(size);

                    co_await boost::asio::async_read(file, boost::asio::buffer(buffer),
                        boost::asio::use_awaitable);

                    const auto response = HttpResponse{}
                        .set_version(request.version)
                        .set_status(200, "OK")
                        .set_body(buffer)
                        .set_header("Content-Type", "text/html; charset=utf-8");

                    co_return response;
                }
            },
            {
                "/time", [](tcp::socket& socket, const HttpRequest& request) -> boost::asio::awaitable<HttpResponse> {
                    if (request.method != "GET")
                    {
                        const auto response = HttpResponse{}
                            .set_version(request.version)
                            .set_status(405, "Method Not Allowed")
                            .set_body("Method Not Allowed");
                        co_return response;
                    }

                    nlohmann::json vm;
                    vm["time"] = std::format("{:%Y-%m-%d %H:%M:%S}", std::chrono::system_clock::now());

                    inja::Environment env;
                    const auto html = env.render_file("templates/time.html", vm);

                    const auto response = HttpResponse{}
                        .set_version(request.version)
                        .set_status(200, "OK")
                        .set_body(html);

                    co_return response;
                }
            }
        };
    } // namespace

    namespace response_handler
    {
        auto handle(tcp::socket& socket, const HttpRequest& request) -> boost::asio::awaitable<void>
        {
            if (!path_handlers.contains(request.path))
            {
                const auto response = HttpResponse{}
                    .set_version(request.version)
                    .set_status(404, "Not Found")
                    .set_body("Not Found");
                co_await response.write(socket);
                co_return;
            }

            auto response = co_await path_handlers.at(request.path)(socket, request);
            co_await response.write(socket);
        }
    } // namespace response_handler
} // namespace aych
