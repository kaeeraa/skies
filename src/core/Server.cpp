#include "Server.hpp"
#include "Logger.hpp"
#include "Router.hpp"
#include <memory>

void Session::start()
{
  auto self = shared_from_this();

  aliases::net::co_spawn(
    socket_.get_executor(),
    [self]() -> aliases::net::awaitable<void> {
      co_await self->run();
    },
    aliases::net::detached);
}

aliases::net::awaitable<void> Session::run()
{
  aliases::beast::flat_buffer buffer;
  aliases::beast::error_code ec;

  for (;;) {
    buffer.consume(buffer.size());

    auto request = std::make_shared<aliases::Request>();
    aliases::Response response;

    co_await aliases::http::async_read(socket_, buffer, *request,
                                       aliases::net::redirect_error(aliases::net::use_awaitable, ec));

    if (ec) {
      if (ec == aliases::http::error::end_of_stream || ec == aliases::net::error::eof || ec == aliases::net::error::connection_reset) {
        break;
      }

      Logger::instance().error("HTTP read error: " + ec.message());
      break;
    }

    try {
      response = co_await router_.route(request);
    } catch (const std::exception& e) {
      Logger::instance().error(e.what());
      response = aliases::Response { aliases::http::status::internal_server_error, request->version() };
    }

    response.version(request->version());
    response.keep_alive(request->keep_alive());
    response.prepare_payload();

    co_await aliases::http::async_write(
      socket_, response,
      aliases::net::redirect_error(aliases::net::use_awaitable, ec));

    if (ec) {
      if (ec == aliases::net::error::not_connected || ec == aliases::net::error::broken_pipe) {
        break;
      }

      Logger::instance().error("HTTP write error: " + ec.message());
      break;
    }

    if (!response.keep_alive()) {
      break;
    }
  }

  socket_.close(ec);
}
