#pragma once
#include "../core/Logger.hpp"
#include <boost/asio.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <boost/system/detail/errc.hpp>
#include <filesystem>
#include <iostream>
#include <string>

namespace asio  = boost::asio;
namespace beast = boost::beast;
namespace http  = beast::http;
namespace json  = boost::json;
namespace errc  = boost::system::errc;

class DockerMiddleware {
  public:
  // constructors
  DockerMiddleware(const DockerMiddleware&)            = delete;
  DockerMiddleware& operator=(const DockerMiddleware&) = delete;
  DockerMiddleware()
    : socket(io)
  {
    socket.connect({ "/var/run/docker.sock" }, error);

    if (!error) {
      return;
    }

    if (error == errc::permission_denied) {
      Logger::instance()
        .fatal("Not enough permissions to connect to Docker socket");
      exit(1);
    } else if (error == errc::no_such_file_or_directory) {
      Logger::instance()
        .fatal("Docker socket not found");
      exit(1);
    } else {
      Logger::instance()
        .fatal("Unknown error while connecting to socket: " + error.message() + "\n");
      exit(1);
    }
  }

  // methods (public)
  json::value request(const http::verb& method, const std::string_view target, const json::object& body = {});

  private:
  // fields (private)
  asio::io_context                     io;
  asio::local::stream_protocol::socket socket;
  boost::system::error_code            error;
};
