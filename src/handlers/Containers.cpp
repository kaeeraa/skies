#include "Containers.hpp"
#include "../utility/ResponseBuilder.hpp"
#include <memory>

template <typename T>
concept containerResponse = requires(T& response) {
  response.mutable_base();
  response.mutable_base()->Clear();
};

template <containerResponse T>
std::unique_ptr<std::string> tryId(std::shared_ptr<const aliases::Request> raw, T& base, std::string_view pattern)
{
  const auto params = Path::get(raw->target(), pattern);
  if (params.empty()) {
    base.mutable_base()->set_error("Invalid path parameters (BR1)");
    return nullptr;
  }

  auto id = std::make_unique<std::string>(params.at("id"));
  if (id->empty()) {
    base.mutable_base()->set_error("Invalid path parameters (BR2)");
    return nullptr;
  }

  return id;
}

namespace Docker::Handlers {
namespace Containers {
  aliases::net::awaitable<aliases::Response> List(std::shared_ptr<const aliases::Request> raw)
  {
    containers::response::List response;

    co_return co_await handleRequest(
      raw,
      response,
      "List containers",
      "Failed to list containers",
      [&]() {
        return containers->list(Query::get(raw->target()));
      });
  }
  aliases::net::awaitable<aliases::Response> Create(std::shared_ptr<const aliases::Request> raw)
  {
    auto request = std::make_unique<containers::request::Create>();
    containers::response::Create response;

    if (raw->body().empty()) {
      response.mutable_base()->set_error("Request body is empty");
      co_return buildResponse(aliases::http::status::bad_request, response, raw->version());
    }

    if (!request->ParseFromString(raw->body()) || request == nullptr) {
      response.mutable_base()->set_error("Failed to parse {Containers::Create} request");
      co_return buildResponse(response, raw->version());
    }

    co_return co_await handleRequest(
      raw,
      response,
      "Create container: " + request->name(),
      "Failed to create container",
      [&]() {
        return containers->create(std::move(request));
      });
  }

  aliases::net::awaitable<aliases::Response> Inspect(std::shared_ptr<const aliases::Request> raw)
  {
    containers::response::Inspect response;

    const std::unique_ptr<Query::QueryVec> queries = Query::get(raw->target());
    const Path::PathParams params = Path::get(raw->target(), "/api/containers/{id}");
    if (params.empty()) {
      response.mutable_base()->set_error("Invalid path parameters");
      co_return buildResponse(response, raw->version());
    }

    auto id = tryId(raw, response, "/api/containers/{id}/top");
    co_return co_await handleRequest(
      raw,
      response,
      "Inspect container: " + *id,
      "Failed to inspect container",
      [&]() {
        return containers->inspect(std::move(id));
      });
  }

  aliases::net::awaitable<aliases::Response> Top(std::shared_ptr<const aliases::Request> raw)
  {
    containers::response::Top response;

    std::unique_ptr<Query::QueryVec> queries = Query::get(raw->target());
    const Path::PathParams params = Path::get(raw->target(), "/api/containers/{id}/top");
    if (params.empty()) {
      response.mutable_base()->set_error("Invalid path parameters");
      co_return buildResponse(response, raw->version());
    }

    auto id = tryId(raw, response, "/api/containers/{id}/top");
    co_return co_await handleRequest(
      raw,
      response,
      "Top container: " + *id,
      "Failed to get top information",
      [&]() {
        return containers->top(std::move(id), std::move(queries));
      });
  }
} // namespace Containers
} // namespace Docker::Handlers