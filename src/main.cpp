#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <string_view>
#include <iostream>

using namespace boost::asio;

static constexpr auto port = 3000u;

auto my_endpoint()
{

}

int main()
{
    try
    {
        boost::asio::io_context io_context{};
        ip::tcp::acceptor acceptor(io_context, { ip::tcp::v4(), port });

        for (;;)
        {
            ip::tcp::socket socket{io_context};
            acceptor.accept(socket);

            std::string message = "hi from server";
            boost::system::error_code ignore_error{};
            boost::asio::write(socket, boost::asio::buffer(message), ignore_error);
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
    }
    return 0;
}

