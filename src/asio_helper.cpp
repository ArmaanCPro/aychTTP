#include "asio_helper.h"

#include <boost/asio.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/awaitable.hpp>

void spawn_server_loop(boost::asio::io_context& io_context, boost::asio::ip::tcp::acceptor& acceptor, std::function<boost::asio::awaitable<void>(boost::asio::ip::tcp::socket)> handler_factory)
{
    boost::asio::co_spawn(io_context,
        [&io_context, &acceptor, handler = std::move(handler_factory)]() -> boost::asio::awaitable<void>
        {
            for (;;)
            {
                boost::asio::ip::tcp::socket socket(io_context);
                co_await acceptor.async_accept(socket, boost::asio::use_awaitable);

                boost::asio::co_spawn(io_context, handler(std::move(socket)), boost::asio::detached);
            }
        }, boost::asio::detached);
}
