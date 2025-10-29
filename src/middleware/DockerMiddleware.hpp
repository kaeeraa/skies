#pragma once
#include <boost/asio.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <iostream>
#include <string>

namespace asio  = boost::asio;
namespace beast = boost::beast;
namespace http  = beast::http;
namespace json  = boost::json;

class DockerMiddleware {
  public:
  // constructors
  DockerMiddleware()
    : io()
    , socket(io)
  {
    socket.connect(asio::local::stream_protocol::endpoint("/var/run/docker.sock"));
  }

  // methods (public)
  json::value request(const http::verb& method, const std::string& target, const json::object& body = {});

  private:
  // fields (private)
  asio::io_context                     io;
  asio::local::stream_protocol::socket socket;
};
