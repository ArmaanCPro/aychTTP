#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <functional>

void spawn_server_loop(
    boost::asio::io_context& io_context,
    boost::asio::ip::tcp::acceptor& acceptor,
    std::function<boost::asio::awaitable<void>(boost::asio::ip::tcp::socket)> handler_factory
);
