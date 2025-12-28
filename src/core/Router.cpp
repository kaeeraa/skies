#include "Router.hpp"
#include "Logger.hpp"
#include <boost/beast/http.hpp>
#include <boost/beast/http/fields.hpp>
#include <functional>
#include <string>
#include <unordered_map>

Response Router::route(const Request& request) const
{
  const auto& method = std::string(http::to_string(request.method()));
  const auto& target = std::string(request.target());

  auto it = request.method() == http::verb::get
              ? getRoutes.find(target)
              : postRoutes.find(target);

  if (it != getRoutes.end() || it != postRoutes.end()) {
    Logger::instance().trace(std::format("New request {} (success, M: {})", target, method));
    return it->second(request);
  }

  Logger::instance().trace(std::format("New request {} (failed, M: {})", target, method));
  return Response { http::status::not_found, request.version() };
}
