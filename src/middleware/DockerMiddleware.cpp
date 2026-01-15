#include "DockerMiddleware.hpp"

#include "../core/Logger.hpp"
#include <boost/asio.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <iostream>
#include <string>

asio::awaitable<json::value> DockerMiddleware::request(
  http::verb method,
  std::string target,
  const json::object body)
{
  boost::asio::local::stream_protocol::socket socket(ex_);
  boost::system::error_code ec;
  co_await socket.async_connect({ path_ }, asio::use_awaitable);

  http::request<http::string_body> req;
  req.method(method);
  req.target(target);
  req.version(11);
  req.set(http::field::host, "localhost");

  if (!body.empty()) {
    req.body() = json::serialize(body);
    req.set(http::field::content_type, "application/json");
  }
  req.prepare_payload();

  co_await http::async_write(socket, req, asio::use_awaitable);

  beast::flat_buffer buffer;
  http::response<http::string_body> resp;
  co_await http::async_read(socket, buffer, resp, asio::use_awaitable);

  co_return json::parse(resp.body());
}
