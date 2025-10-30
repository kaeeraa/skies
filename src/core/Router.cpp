#include "Router.hpp"
#include "Logger.hpp"
#include <boost/beast/http.hpp>
#include <boost/beast/http/fields.hpp>
#include <functional>
#include <string>
#include <unordered_map>

Response Router::route(const Request& request)
{
  const auto& method = std::string(http::to_string(request.method()));
  const auto& target = std::string(request.target());
  if (request.method() == http::verb::get) {
    if (auto it = getRoutes.find(target); it != getRoutes.end()) {
      Logger::instance()
        .trace(std::format("New request {} (success, M: {})", target, method));
      return it->second(request);
    }
  } else if (request.method() == http::verb::post) {
    if (auto it = postRoutes.find(target); it != postRoutes.end()) {
      Logger::instance()
        .trace(std::format("New request {} (success, M: {}) created!", target, method));
      return it->second(request);
    }
  }

  Logger::instance()
    .trace(std::format("New request {} (failed, M: {})", target, method));
  Response response { http::status::not_found, request.version() };
  response.set(http::field::content_type, "application/json");
  response.body() = R"({"error": "Route not found"})";
  response.prepare_payload();
  return response;
}
