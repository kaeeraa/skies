#include "Router.hpp"
#include "../utility/Parameter.hpp"
#include <memory>

aliases::net::awaitable<aliases::Response> Router::route(std::shared_ptr<const aliases::Request> request) const
{
  std::string target = request->target();
  Query::remove(target);

  const aliases::RouteMap& routes = (request->method() == aliases::http::verb::get) ? getRoutes : postRoutes;
  for (const auto& [pattern, handler] : routes | std::views::all) {
    if (Path::verify(target, pattern)) {
      Logger::instance().debug(std::format("Match, Target: '{}', Pattern: '{}'", target, pattern));
      co_return co_await handler(request);
    }
    Logger::instance().debug(std::format("No match, Target: '{}', Pattern: '{}'", target, pattern));
  }

  co_return aliases::Response { aliases::http::status::not_found, request->version() };
}
