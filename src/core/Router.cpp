#include "Router.hpp"
#include <memory>

aliases::net::awaitable<aliases::Response> Router::route(std::shared_ptr<const aliases::Request> request) const
{
  std::string target = request->target();
  auto queryPos = target.find('?');
  if (queryPos != std::string::npos) {
    target = target.substr(0, queryPos);
  }

  const aliases::RouteMap& routes = (request->method() == aliases::http::verb::get) ? getRoutes : postRoutes;
  if (auto it = routes.find(target); it != routes.end()) {
    co_return co_await it->second(std::move(request));
  }

  co_return aliases::Response { aliases::http::status::not_found, request->version() };
}
