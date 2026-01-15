#include "DockerClient.hpp"
#include "../middleware/DockerMiddleware.hpp"
#include "../utility/ProtoBuffer.hpp"
#include "../utility/QueryBuilder.hpp"
#include <absl/status/status.h>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/json.hpp>
#include <string>

namespace containers = api::v1::containers;
namespace asio = boost::asio;
namespace json = boost::json;

asio::awaitable<containers::response::List> Docker::Containers::listUnwrapped(const containers::request::List request)
{
  containers::response::List response;
  QueryBuilder qb;
  qb.add("all", request.all())
    .add("limit", request.limit())
    .add("size", request.size())
    .add("filters", request.filters());

  const std::string target = qb.build("/containers/json");

  json::object object;
  try {
    object = {
      { "data", co_await middleware.request(http::verb::get, target) }
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

asio::awaitable<containers::response::Create> Docker::Containers::createUnwrapped(const containers::request::Create request)
{
  containers::response::Create response;
  const std::string target = "/containers/create";

  json::value rawRequest;
  if (const absl::Status status = MessageToJson(request, &rawRequest); !status.ok()) {
    Logger::instance().error("Failed to serialize {Containers::Create} request: " + status.ToString());
    response.clear_id();
    response.mutable_base()->set_error("Failed to serialize {Containers::Create} request");
    co_return response;
  }

  json::value raw;
  try {
    raw = co_await middleware.request(http::verb::post, target, rawRequest.as_object());
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
asio::awaitable<containers::response::List> Docker::Containers::list(const containers::request::List request)
{
  co_return co_await asio::co_spawn(pool_, [this, request]() -> asio::awaitable<containers::response::List> { co_return co_await listUnwrapped(request); }, asio::use_awaitable);
}

asio::awaitable<containers::response::Create> Docker::Containers::create(const containers::request::Create request)
{
  co_return co_await asio::co_spawn(pool_, [this, request]() -> asio::awaitable<containers::response::Create> { co_return co_await createUnwrapped(request); }, asio::use_awaitable);
}