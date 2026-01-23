#pragma once
#include "../utility/Shorthands.hpp"
#include "Logger.hpp"
#include <boost/asio/awaitable.hpp>
#include <boost/beast/http.hpp>
#include <unordered_map>

class Router {
  // fields
  aliases::RouteMap getRoutes;
  aliases::RouteMap postRoutes;

  public:
  inline void get(const std::string_view path, aliases::AsyncHandler handler)
  {
    Logger::instance().trace(std::format("Created route with path {} (M: GET)", path));
    getRoutes[path] = std::move(handler);
  }

  inline void post(const std::string_view path, aliases::AsyncHandler handler)
  {
    Logger::instance().trace(std::format("Created route with path {} (M: POST)", path));
    postRoutes[path] = std::move(handler);
  }

  aliases::net::awaitable<aliases::Response> route(std::shared_ptr<const aliases::Request> request) const;
};