#pragma once
#include "Logger.hpp"
#include <boost/asio/awaitable.hpp>
#include <boost/beast/http.hpp>
#include <functional>
#include <string>
#include <unordered_map>

namespace asio = boost::asio;
namespace http = boost::beast::http;

using Request = http::request<http::string_body>;
using Response = http::response<http::string_body>;
using AsyncHandler = std::function<asio::awaitable<Response>(const Request&)>;
using RouteMap = std::unordered_map<std::string_view, AsyncHandler, std::hash<std::string_view>>;

class Router {
  // fields
  RouteMap getRoutes;
  RouteMap postRoutes;

  public:
  inline void get(const std::string_view path, AsyncHandler handler)
  {
    Logger::instance().trace(std::format("Created route with path {} (M: GET)", path));
    getRoutes[path] = std::move(handler);
  }

  inline void post(const std::string_view path, AsyncHandler handler)
  {
    Logger::instance().trace(std::format("Created route with path {} (M: POST)", path));
    postRoutes[path] = std::move(handler);
  }

  asio::awaitable<Response> route(const Request& request) const;
};