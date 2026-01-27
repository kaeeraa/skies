#include "DockerClient.hpp"
#include "../handlers/Error.hpp"
#include "../middleware/DockerMiddleware.hpp"
#include "../utility/ProtoBuffer.hpp"
#include "api/v1/containers/Response.pb.h"
#include <absl/status/status.h>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/json.hpp>
#include <boost/json/object.hpp>
#include <memory>
#include <string>

aliases::net::awaitable<containers::response::List> Docker::Containers::listUnwrapped(std::unique_ptr<Query::QueryVec> queries)
{
  const std::string target = Query::append("/containers/json", std::move(queries));
  co_return co_await callAndParse<containers::response::List>(
    middleware,
    target,
    aliases::http::verb::get,
    nullptr,
    "Failed to list container");
}

aliases::net::awaitable<containers::response::Create> Docker::Containers::createUnwrapped(std::unique_ptr<containers::request::Create> request)
{
  containers::response::Create response;

  auto rawRequest = std::make_unique<aliases::json::value>();
  if (const absl::Status status = MessageToJson(*request, rawRequest.get()); !status.ok()) {
    co_return setError(response, status.ToString(), "Failed to serialize {Containers::Create} request");
  }

  auto requestObject = std::make_unique<aliases::json::object>(rawRequest->as_object());
  const std::string target = "/containers/create";

  co_return co_await callAndParse<containers::response::Create>(
    middleware,
    target,
    aliases::http::verb::post,
    std::move(requestObject),
    "Failed to create container",
    [](const aliases::json::object& object, containers::response::Create& resp) -> absl::Status {
      if (const auto& idVal = object.try_at("Id"); idVal.has_value()) {
        resp.set_id(idVal.value().as_string().c_str());
        return absl::OkStatus();
      }
      return absl::InvalidArgumentError("Missing Id in create response");
    });
}

aliases::net::awaitable<containers::response::Inspect> Docker::Containers::inspectUnwrapped(std::unique_ptr<std::string> id)
{
  const std::string target = "/containers/" + *id + "/json";
  co_return co_await callAndParse<containers::response::Inspect>(
    middleware,
    target,
    aliases::http::verb::get,
    nullptr,
    "Failed to inspect container");
}

aliases::net::awaitable<containers::response::Top> Docker::Containers::topUnwrapped(std::unique_ptr<std::string> id, std::unique_ptr<Query::QueryVec>&& queries)
{
  if (queries == nullptr) {
    queries = std::make_unique<Query::QueryVec>();
  }

  std::string target = "/containers/" + *id + "/top";
  Query::append(target, std::move(queries));

  co_return co_await callAndParse<containers::response::Top>(
    middleware,
    std::move(target),
    aliases::http::verb::get,
    nullptr,
    "Failed to get top information");
}

// --- Wrappers ---
aliases::net::awaitable<containers::response::List> Docker::Containers::list(std::unique_ptr<Query::QueryVec> queries)
{
  co_return co_await aliases::net::co_spawn(
    pool_,
    [this, queries = std::move(queries)]() mutable -> aliases::net::awaitable<containers::response::List> {
      co_return co_await listUnwrapped(std::move(queries));
    },
    aliases::net::use_awaitable);
}

aliases::net::awaitable<containers::response::Create> Docker::Containers::create(std::unique_ptr<containers::request::Create> request)
{
  co_return co_await aliases::net::co_spawn(
    pool_,
    [this, request = std::move(request)]() mutable -> aliases::net::awaitable<containers::response::Create> {
      co_return co_await createUnwrapped(std::move(request));
    },
    aliases::net::use_awaitable);
}

aliases::net::awaitable<containers::response::Inspect> Docker::Containers::inspect(std::unique_ptr<std::string> id)
{
  co_return co_await aliases::net::co_spawn(
    pool_,
    [this, id = std::move(id)]() mutable -> aliases::net::awaitable<containers::response::Inspect> {
      co_return co_await inspectUnwrapped(std::move(id));
    },
    aliases::net::use_awaitable);
}

aliases::net::awaitable<containers::response::Top> Docker::Containers::top(std::unique_ptr<std::string> id, std::unique_ptr<Query::QueryVec>&& queries)
{
  co_return co_await aliases::net::co_spawn(
    pool_,
    [this, id = std::move(id), queries = std::move(queries)]() mutable -> aliases::net::awaitable<containers::response::Top> {
      co_return co_await topUnwrapped(std::move(id), std::move(queries));
    },
    aliases::net::use_awaitable);
}
