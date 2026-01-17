#include "Server.hpp"
#include "Logger.hpp"
#include "Router.hpp"
#include <memory>

void Session::start()
{
  auto self = shared_from_this();

  asio::co_spawn(
    socket_.get_executor(),
    [self]() -> asio::awaitable<void> {
      co_await self->run();
    },
    asio::detached);
}

asio::awaitable<void> Session::run()
{
  beast::flat_buffer buffer;
  while (true) {
    auto request = std::make_shared<Request>();
    auto response = std::make_shared<Response>();
    response->version(11);

    beast::error_code ec;
    buffer.consume(buffer.size());
    co_await http::async_read(socket_, buffer, *request, asio::redirect_error(asio::use_awaitable, ec));
    if (ec) {
      break;
    }

    try {
      response = std::make_shared<Response>(co_await router_.route(request));
    } catch (const std::exception& e) {
      Logger::instance().error(e.what());
      response->result(http::status::internal_server_error);
    }

    response->prepare_payload();
    co_await http::async_write(socket_, *response, asio::redirect_error(asio::use_awaitable, ec));
    if (ec) {
      Logger::instance().error("HTTP write error: " + ec.message());
      break;
    }
  }

  beast::error_code ec;
  socket_.shutdown(tcp::socket::shutdown_send, ec);
  if (ec) {
    Logger::instance().error("shutdown error: " + ec.message());
  }
}
