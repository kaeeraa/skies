#pragma once

#include "../utility/Shorthands.hpp"
#include "Router.hpp"
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <memory>

class Session : public std::enable_shared_from_this<Session> {
  public:
  Session(aliases::tcp::socket socket, Router& router)
    : socket_(std::move(socket))
    , router_(router)
  {
  }
  void start();

  private:
  aliases::net::awaitable<void> run();

  aliases::tcp::socket socket_;
  Router& router_;
};

class Server {
  public:
  Server(aliases::net::io_context& ioContext, aliases::tcp::endpoint endpoint, Router& router)
    : ioContext_(ioContext)
    , acceptor_(ioContext)
    , router_(router)
  {
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(aliases::net::socket_base::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen(aliases::net::socket_base::max_listen_connections);

    accept();
  }

  private:
  void accept()
  {
    acceptor_.async_accept([this](aliases::beast::error_code ec, aliases::tcp::socket socket) {
      if (!ec) {
        std::make_shared<Session>(std::move(socket), router_)->start();
      }
      accept();
    });
  };

  aliases::net::io_context& ioContext_;
  aliases::tcp::acceptor acceptor_;
  Router& router_;
};