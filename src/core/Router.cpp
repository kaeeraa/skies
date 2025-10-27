#include "router.hpp"
#include <boost/beast/http.hpp>
#include <functional>
#include <string>
#include <unordered_map>

Response Router::route(const Request& request)
{
  const auto& target = std::string(request.target());
  if (request.method() == http::verb::get) {
    if (auto it = getRoutes.find(target); it != getRoutes.end()) {
      return it->second(request);
    }
  } else if (request.method() == http::verb::post) {
    if (auto it = postRoutes.find(target); it != postRoutes.end()) {
      return it->second(request);
    }
  }

  Response response { http::status::not_found, request.version() };
  response.set(http::field::content_type, "application/json");
  response.body() = R"({"error": "Route not found"})";
  response.prepare_payload();
  return response;
}
