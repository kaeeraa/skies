
#pragma once
#include <boost/beast/http.hpp>
#include <functional>
#include <string>
#include <unordered_map>

namespace http = boost::beast::http;

using Request  = http::request<http::string_body>;
using Response = http::response<http::string_body>;
using Handler  = std::function<Response(const Request&)>;

class Router {
  // fields
  std::unordered_map<std::string, Handler> getRoutes;
  std::unordered_map<std::string, Handler> postRoutes;

  // methods (public)
  public:
  inline void get(const std::string& path, Handler handler)
  {
    getRoutes[path] = std::move(handler);
  }
  inline void post(const std::string& path, Handler handler)
  {
    postRoutes[path] = std::move(handler);
  }

  Response route(const Request& request);
};