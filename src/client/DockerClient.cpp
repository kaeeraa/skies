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
  containers::response::List response;
  boost::system::error_code ec;

  const std::string target = Query::append("/containers/json", std::move(queries));
  const aliases::json::value& raw = co_await middleware.request(aliases::http::verb::get, target, nullptr, &ec);
  if (ec) {
    setError(response, ec.message(), "Failed to list container (BP1)");
    co_return response;
  }

  auto object = raw.try_as_array();
  auto message = raw.try_as_object();
  if (!object.has_value() && message.has_value()) {
    co_return setError(
      response,
      message.value().try_at("message").value().as_string(),
      "Failed to list containers (BP2)");
  }

  auto data = aliases::json::object {
    { "data", object.value() }
  };
  if (const absl::Status status = JsonToMessage(data, &response); !status.ok()) {
    co_return setError(
      response, status.ToString(), "Failed to parse {Containers::List} response (BP3)");
  }

  co_return response;
}

aliases::net::awaitable<containers::response::Create> Docker::Containers::createUnwrapped(std::unique_ptr<containers::request::Create> request)
{
  containers::response::Create response;
  boost::system::error_code ec;

  auto rawRequest = std::make_unique<aliases::json::value>();
  if (const absl::Status status = MessageToJson(*request, rawRequest.get()); !status.ok()) {
    co_return setError(
      response, status.ToString(), "Failed to serialize {Containers::Create} request");
  }

  auto requestObject = std::make_unique<aliases::json::object>(rawRequest->as_object());
  const std::string target = "/containers/create";
  const aliases::json::value& raw = co_await middleware.request(
    aliases::http::verb::post, target, std::move(requestObject), &ec);
  if (ec) {
    setError(response, ec.message(), "Failed to create container (BP1)");
    co_return response;
  }

  aliases::json::object object = raw.as_object();
  if (object.empty()) {
    setError(response, "Empty response", "Failed to create container (BP2)");
    co_return response;
  }

  if (const auto& message = object.try_at("message"); message.has_value()) {
    co_return setError(
      response, message.value().as_string(), "Failed to create container (BP3)");
  }

  response.set_id(object["Id"].as_string().c_str());
  co_return response;
}

aliases::net::awaitable<containers::response::Inspect> Docker::Containers::inspectUnwrapped(std::unique_ptr<std::string> id)
{
  containers::response::Inspect response;
  boost::system::error_code ec;

  const std::string target = "/containers/" + *id + "/json";
  const aliases::json::value& raw = co_await middleware.request(aliases::http::verb::get, target, nullptr, &ec);
  if (ec) {
    setError(response, ec.message(), "Failed to inspect container (BP1)");
    co_return response;
  }

  aliases::json::object object = raw.as_object();
  if (object.empty()) {
    setError(response, "Empty response", "Failed to inspect container (BP2)");
    co_return response;
  }

  if (const auto& message = raw.as_object().try_at("message"); message.has_value()) {
    co_return setError(
      response, message.value().as_string(), "Failed to inspect containers (BP3)");
  }

  object = aliases::json::object {
    { "data", object }
  };
  if (const absl::Status status = JsonToMessage(object, &response); !status.ok()) {
    co_return setError(
      response, status.ToString(), "Failed to parse {Containers::Inspect} response");
  }

  co_return response;
}

aliases::net::awaitable<containers::response::Top> Docker::Containers::topUnwrapped(std::unique_ptr<std::string> id, std::unique_ptr<Query::QueryVec>&& queries)
{
  containers::response::Top response;
  boost::system::error_code ec;

  if (queries == nullptr) {
    queries = std::make_unique<Query::QueryVec>();
  }
  std::string target = "/containers/" + *id + "/top";
  Query::append(target, std::move(queries));

  const aliases::json::value& raw = co_await middleware.request(aliases::http::verb::get, target, nullptr, &ec);
  if (ec) {
    setError(response, ec.message(), "Failed to get top information (BP1)");
    co_return response;
  }

  aliases::json::object object = raw.as_object();
  if (object.empty()) {
    setError(response, "Empty response", "Failed to get top information (BP2)");
    co_return response;
  }

  if (const auto& message = raw.as_object().try_at("message"); message.has_value()) {
    co_return setError(
      response,
      std::format("{}: {}", "Failed to get top information (BP3)", message.value().as_string().c_str()));
  }

  aliases::json::object data = {
    { "data", object }
  };
  if (const absl::Status status = JsonToMessage(data, &response); !status.ok()) {
    setError(
      response, status.ToString(), "Failed to parse {Containers::Top} response (BP4)");
    co_return response;
  }
  co_return response;
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
