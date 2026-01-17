#pragma once
#include "../core/Logger.hpp"
#include <boost/asio.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <string>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;
using Request = http::request<http::string_body>;
using Response = http::response<http::string_body>;

class DockerMiddleware {
  public:
  explicit DockerMiddleware(const asio::any_io_executor& ex)
    : ex_(ex)
  {
    const char* env = std::getenv("DOCKER_HOST");
    if (!env || !env[0]) {
      Logger::instance().fatal("DOCKER_HOST variable is empty");
      std::exit(1);
    }
    path_ = env;
    if (path_.rfind("unix://", 0) == 0) {
      path_ = path_.substr(7);
    }
  }

  asio::awaitable<json::value> request(
    http::verb method,
    std::string target,
    std::unique_ptr<json::object> body = nullptr);

  private:
  std::string path_;
  asio::any_io_executor ex_;
};
