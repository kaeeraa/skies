#include "Router.hpp"
#include "../utility/PopQuery.hpp"
#include "Logger.hpp"
#include <boost/beast/http.hpp>
#include <boost/beast/http/fields.hpp>
#include <functional>
#include <string>
#include <unordered_map>

Response Router::route(const Request& request) const
{
  std::string target = request.target();
  popQuery(target);
  const auto& method = request.method();

  const auto& routes = (method == http::verb::get) ? getRoutes : postRoutes;

  if (auto it = routes.find(target); it != routes.end()) {
    Logger::instance().trace(std::format(
      "New request {} (success, M: {})",
      std::string(target),
      std::string(http::to_string(method))));
    return it->second(request);
  }

  Logger::instance().trace(std::format(
    "New request {} (failed, M: {})",
    std::string(target),
    std::string(http::to_string(method))));

  return Response { http::status::not_found, request.version() };
}
