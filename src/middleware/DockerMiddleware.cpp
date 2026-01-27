#include "DockerMiddleware.hpp"
#include "../core/Logger.hpp"
#include <boost/asio.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http/status.hpp>
#include <boost/json.hpp>
#include <boost/scope_exit.hpp>
#include <boost/system/detail/error_code.hpp>
#include <memory>
#include <string>

aliases::net::awaitable<aliases::json::value>
  DockerMiddleware::request(
    aliases::http::verb method,
    std::string target,
    std::unique_ptr<aliases::json::object> body,
    boost::system::error_code* ec) noexcept
{
  if (ec) {
    (*ec).clear();
  }

  aliases::net::local::stream_protocol::socket socket(ex_);
  {
    co_await socket.async_connect(
      { path_ },
      aliases::net::redirect_error(aliases::net::use_awaitable, *ec));
    if (*ec) {
      co_return aliases::json::value();
    }
  }

  aliases::Request request;
  request.method(method);
  request.target(std::move(target));
  request.version(11);
  request.keep_alive(false);
  request.set(aliases::http::field::connection, "close");
  request.set(aliases::http::field::host, "localhost");

  if (body && !body->empty()) {
    request.body() = aliases::json::serialize(*body);
    request.set(aliases::http::field::content_type, "application/json");
  }
  request.prepare_payload();

  co_await aliases::http::async_write(
    socket,
    request,
    aliases::net::redirect_error(aliases::net::use_awaitable, *ec));
  if (*ec) {
    Logger::instance().error("HTTP write error: " + (*ec).message());
    co_return aliases::json::value();
  }

  aliases::Response response;
  {
    aliases::beast::flat_buffer buffer;

    co_await aliases::http::async_read(
      socket,
      buffer,
      response,
      aliases::net::redirect_error(aliases::net::use_awaitable, *ec));
    if (*ec) {
      Logger::instance().error("HTTP read error: " + (*ec).message());
      co_return aliases::json::value();
    }
  }

  aliases::json::value result = aliases::json::parse(response.body(), *ec);
  if (*ec) {
    Logger::instance().error("JSON parse error: " + (*ec).message());
    *ec = make_error_code(boost::system::errc::invalid_argument);
    co_return response.body();
  }

  {
    [[maybe_unused]] auto _ = socket.shutdown(
      aliases::net::local::stream_protocol::socket::shutdown_both, *ec);
    if (*ec) {
      Logger::instance().error("Shutdown error: " + (*ec).message());
      co_return result;
    }
  }
  {
    [[maybe_unused]] auto _ = socket.close(*ec);
    if (*ec) {
      Logger::instance().error("Close error: " + (*ec).message());
      co_return result;
    }
  }

  co_return result;
}
