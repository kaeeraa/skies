#include "DockerMiddleware.hpp"

#include "../core/Logger.hpp"
#include <boost/asio.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <iostream>
#include <memory>
#include <string>

asio::awaitable<json::value> DockerMiddleware::request(
  http::verb method,
  std::string target,
  std::unique_ptr<json::object> body)
{
  asio::local::stream_protocol::socket socket(ex_);
  co_await socket.async_connect({ path_ }, asio::use_awaitable);

  Request request;
  request.method(method);
  request.target(target);
  request.version(11);
  request.set(http::field::host, "localhost");

  if (body != nullptr && !body->empty()) {
    request.body() = json::serialize(*body);
    request.set(http::field::content_type, "application/json");
  }
  request.prepare_payload();

  co_await http::async_write(socket, request, asio::use_awaitable);

  beast::flat_buffer buffer;
  Response response;
  co_await http::async_read(socket, buffer, response, asio::use_awaitable);

  co_return json::parse(response.body());
}
