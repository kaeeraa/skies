#pragma once

#include "Router.hpp"
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <memory>

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
  public:
  Session(tcp::socket socket, Router& router)
    : socket_(std::move(socket))
    , router_(router)
  {
  }
  void start();

  private:
  asio::awaitable<void> run();

  tcp::socket socket_;
  Router& router_;
};

class Server {
  public:
  Server(asio::io_context& ioContext, tcp::endpoint endpoint, Router& router)
    : ioContext_(ioContext)
    , acceptor_(ioContext)
    , router_(router)
  {
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(asio::socket_base::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen(asio::socket_base::max_listen_connections);

    accept();
  }

  private:
  void accept()
  {
    acceptor_.async_accept([this](beast::error_code ec, tcp::socket socket) {
      if (!ec) {
        std::make_shared<Session>(std::move(socket), router_)->start();
      }
      accept();
    });
  };

  asio::io_context& ioContext_;
  tcp::acceptor acceptor_;
  Router& router_;
};