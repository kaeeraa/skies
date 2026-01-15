#include "Server.hpp"
#include "Logger.hpp"
#include "Router.hpp"

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
  beast::error_code ec;
  beast::flat_buffer buffer;
  http::request<http::string_body> req;
  http::response<http::string_body> res;

  while (true) {
    buffer.consume(buffer.size());
    co_await http::async_read(socket_, buffer, req, asio::redirect_error(asio::use_awaitable, ec));
    if (ec == http::error::end_of_stream) {
      break;
    }
    if (ec) {
      Logger::instance().error("HTTP read error: " + ec.message());
      break;
    }

    try {
      res = co_await router_.route(req);
    } catch (const std::exception& e) {
      Logger::instance().error(e.what());
      res = { http::status::bad_request, req.version() };
    }

    res.prepare_payload();
    co_await http::async_write(socket_, res, asio::redirect_error(asio::use_awaitable, ec));
    if (ec) {
      Logger::instance().error("HTTP write error: " + ec.message());
      break;
    }
  }

  socket_.shutdown(tcp::socket::shutdown_send, ec);
}