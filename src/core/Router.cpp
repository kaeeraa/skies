#include "Router.hpp"
#include "../utility/Query.hpp"
#include <memory>

asio::awaitable<Response> Router::route(std::shared_ptr<const Request> request) const
{
  std::string target = request->target();
  auto queryPos = target.find('?');
  if (queryPos != std::string::npos) {
    target = target.substr(0, queryPos);
  }

  const RouteMap& routes = (request->method() == http::verb::get) ? getRoutes : postRoutes;
  if (auto it = routes.find(target); it != routes.end()) {
    co_return co_await it->second(std::move(request));
  }

  co_return Response { http::status::not_found, request->version() };
}
