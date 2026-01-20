#include "DockerClient.hpp"
#include "../middleware/DockerMiddleware.hpp"
#include "../utility/ProtoBuffer.hpp"
#include "../utility/Query.hpp"
#include <absl/status/status.h>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/json.hpp>
#include <memory>
#include <string>

aliases::net::awaitable<containers::response::List> Docker::Containers::listUnwrapped(std::unique_ptr<Query::QueryVec> queries)
{
  containers::response::List response;
  const std::string target = Query::append("/containers/json", std::move(queries));

  aliases::json::object object;
  try {
    object = {
      { "data", co_await middleware.request(aliases::http::verb::get, target) }
    };
  } catch (const std::exception& e) {
    Logger::instance().error("Docker request failed: " + std::string(e.what()));
    response.mutable_data()->Clear();
    response.mutable_base()->set_error(e.what());
    co_return response;
  }

  if (object.contains("error")) {
    response.mutable_data()->Clear();
    response.mutable_base()->set_error(std::string(object.at("error").as_string()));
    co_return response;
  }

  if (const absl::Status status = JsonToMessage(object, &response); !status.ok()) {
    Logger::instance().error("Failed to parse {Containers::List} response: " + status.ToString());
    response.mutable_data()->Clear();
    response.mutable_base()->set_error("Failed to parse {Containers::List} response");
    co_return response;
  }

  co_return response;
}

aliases::net::awaitable<containers::response::Create> Docker::Containers::createUnwrapped(std::unique_ptr<containers::request::Create> request)
{
  containers::response::Create response;
  const std::string target = "/containers/create";

  aliases::json::value rawRequest;
  if (const absl::Status status = MessageToJson(*request, &rawRequest); !status.ok()) {
    Logger::instance().error("Failed to serialize {Containers::Create} request: " + status.ToString());
    response.clear_id();
    response.mutable_base()->set_error("Failed to serialize {Containers::Create} request");
    co_return response;
  }

  aliases::json::value raw;
  try {
    auto objectPtr = std::make_unique<aliases::json::object>(rawRequest.as_object());
    raw = co_await middleware.request(aliases::http::verb::post, target, std::move(objectPtr));
  } catch (const std::exception& e) {
    Logger::instance().error("Docker request failed: " + std::string(e.what()));
    response.clear_id();
    response.mutable_base()->set_error(e.what());
    co_return response;
  }

  if (raw.as_object().contains("message")) {
    response.clear_id();
    response.mutable_base()->set_error(raw.as_object()["message"].as_string().c_str());
    co_return response;
  }

  response.set_id(raw.as_object()["Id"].as_string().c_str());
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
