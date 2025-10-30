#include "DockerMiddleware.hpp"

#include "../core/Logger.hpp"
#include <boost/asio.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <iostream>
#include <string>

json::value DockerMiddleware::request(const http::verb& method, const std::string_view target, const json::object& body)
{
  const auto& serialized   = json::serialize(body);
  const auto& methodString = std::string(to_string(method));

  Logger::instance()
    .trace(std::format("New request to docker (M:{} ; T:{} ; B: {})", methodString, target, serialized));

  http::request<http::string_body> request;
  request.method(method);
  request.target(target);
  request.version(11);
  request.set(http::field::host, "localhost");

  if (!body.empty()) {
    request.body() = serialized;
    request.set(http::field::content_type, "application/json");
  }

  request.prepare_payload();
  http::write(socket, request);

  beast::flat_buffer                buffer;
  http::response<http::string_body> response;
  http::read(socket, buffer, response);

  return json::parse(response.body());
}