#include "DockerMiddleware.hpp"
#include "../core/Logger.hpp"
#include <boost/asio.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <boost/scope_exit.hpp>
#include <iostream>
#include <memory>
#include <string>

aliases::net::awaitable<aliases::json::value> DockerMiddleware::request(
  aliases::http::verb method,
  std::string target,
  std::unique_ptr<aliases::json::object> body)
{
  aliases::net::local::stream_protocol::socket socket(ex_);
  co_await socket.async_connect({ path_ }, aliases::net::use_awaitable);

  aliases::Request request;
  request.method(method);
  request.target(target);
  request.version(11);
  request.keep_alive(false);
  request.set(aliases::http::field::connection, "close");
  request.set(aliases::http::field::host, "localhost");

  if (body != nullptr && !body->empty()) {
    request.body() = aliases::json::serialize(*body);
    request.set(aliases::http::field::content_type, "application/json");
  }
  request.prepare_payload();

  co_await aliases::http::async_write(socket, request, aliases::net::use_awaitable);

  aliases::beast::flat_buffer buffer;
  aliases::Response response;
  co_await aliases::http::async_read(socket, buffer, response, aliases::net::use_awaitable);
  co_return aliases::json::parse(response.body());

  boost::system::error_code ec;
  socket.shutdown(aliases::net::local::stream_protocol::socket::shutdown_both, ec);
  socket.close(ec);

  if (ec) {
    Logger::instance().error("Failed to shutdown socket: " + ec.message());
  }
}
