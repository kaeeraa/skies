#include "Router.hpp"
#include "../utility/PopQuery.hpp"

asio::awaitable<Response> Router::route(const Request& request) const
{
  std::string_view target = request.target();
  auto query = popQuery(target);

  std::string_view path = target;
  if (auto qpos = target.find('?'); qpos != std::string_view::npos) {
    path.remove_suffix(target.size() - qpos);
  }

  const auto& routes = (request.method() == http::verb::get) ? getRoutes : postRoutes;
  if (auto it = routes.find(path); it != routes.end()) {
    co_return co_await it->second(request);
  }

  co_return Response { http::status::not_found, request.version() };
}
