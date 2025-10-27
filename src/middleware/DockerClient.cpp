#include "DockerClient.hpp"

#include <boost/asio.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <iostream>
#include <string>

json::value DockerClient::request(const http::verb& method, const std::string& target, const json::object& body)
{
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
  http::write(socket, req);

  beast::flat_buffer                buffer;
  http::response<http::string_body> res;
  http::read(socket, buffer, res);

  return json::parse(res.body());
}