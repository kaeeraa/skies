#pragma once
#include "../core/Logger.hpp"
#include "../utility/Shorthands.hpp"
#include <boost/asio.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <string>

class DockerMiddleware {
  public:
  explicit DockerMiddleware(const aliases::net::any_io_executor& ex)
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

  aliases::net::awaitable<aliases::json::value> request(
    aliases::http::verb method,
    std::string target,
    std::unique_ptr<aliases::json::object> body = nullptr,
    boost::system::error_code* ec = nullptr) noexcept;

  private:
  std::string path_;
  aliases::net::any_io_executor ex_;
};
