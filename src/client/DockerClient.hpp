#pragma once
#include "../middleware/DockerMiddleware.hpp"
#include "../utility/Parameter.hpp"
#include "../utility/Shorthands.hpp"
#include "api/v1/containers/Request.pb.h"
#include "api/v1/containers/Response.pb.h"
#include <boost/asio/io_context.hpp>
#include <boost/json.hpp>
#include <memory>
#include <string_view>

namespace containers = api::v1::containers;

namespace Docker {
class Containers {
  private:
  DockerMiddleware middleware;
  aliases::net::thread_pool pool_ { std::thread::hardware_concurrency() };

  aliases::net::awaitable<containers::response::List> listUnwrapped(std::unique_ptr<Query::QueryVec> queries);
  aliases::net::awaitable<containers::response::Create> createUnwrapped(std::unique_ptr<containers::request::Create> request);
  aliases::net::awaitable<containers::response::Inspect> inspectUnwrapped(std::unique_ptr<std::string> id);
  aliases::net::awaitable<containers::response::Top> topUnwrapped(
    std::unique_ptr<std::string> id, std::unique_ptr<Query::QueryVec>&& queries);

  public:
  explicit Containers(const aliases::net::any_io_executor& ex)
    : middleware(ex)
  {
  }

  aliases::net::awaitable<containers::response::List> list(std::unique_ptr<Query::QueryVec> queries);
  aliases::net::awaitable<containers::response::Create> create(std::unique_ptr<containers::request::Create> request);
  aliases::net::awaitable<containers::response::Inspect> inspect(std::unique_ptr<std::string> id);
  aliases::net::awaitable<containers::response::Top> top(
    std::unique_ptr<std::string> id, std::unique_ptr<Query::QueryVec>&& queries);
  aliases::json::value export_(std::string_view id);
  aliases::json::value start(std::string_view id);
  aliases::json::value stop(std::string_view id);
  aliases::json::value restart(std::string_view id);
  aliases::json::value kill(std::string_view id);
  aliases::json::value update(std::string_view id, const aliases::json::object& body);
  aliases::json::value rename(std::string_view id, std::string_view name);
  aliases::json::value pause(std::string_view id);
  aliases::json::value unpause(std::string_view id);
  aliases::json::value attach(std::string_view id);
};

class Images {
  private:
  DockerMiddleware middleware;

  public:
  aliases::json::value list();
  aliases::json::value build();
  aliases::json::value prune();
  aliases::json::value create(std::string_view name, const aliases::json::object& body);
  aliases::json::value inspect();
};
}