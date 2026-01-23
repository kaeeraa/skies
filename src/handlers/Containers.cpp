#include "Containers.hpp"
#include "../utility/ResponseBuilder.hpp"

namespace Docker::Handlers {
namespace Containers {
  aliases::net::awaitable<aliases::Response> List(std::shared_ptr<const aliases::Request> raw)
  {
    containers::response::List response;
    try {
      response = co_await containers->list(Query::get(raw->target()));
    } catch (const std::exception& e) {
      response.mutable_data()->Clear();
      response.mutable_base()->set_error("Failed to list containers: " + std::string(e.what()));
    }

    co_return buildResponse(response, raw->version());
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
    try {
      response = co_await containers->create(std::move(request));
    } catch (const std::exception& e) {
      response.clear_id();
      response.mutable_base()->set_error("Failed to create container: " + std::string(e.what()));
    }

    co_return buildResponse(response, raw->version());
  }

  aliases::net::awaitable<aliases::Response> Inspect(std::shared_ptr<const aliases::Request> raw)
  {
    containers::response::Inspect response;
    const Path::PathParams params = Path::get(raw->target(), "/api/containers/{id}");
    if (params.empty()) {
      response.mutable_base()->set_error("Invalid path parameters");
      co_return buildResponse(response, raw->version());
    }

    const std::string id = params.at("id");
    if (id.empty()) {
      response.mutable_base()->set_error("Invalid path parameters");
      co_return buildResponse(response, raw->version());
    }

    try {
      response = co_await containers->inspect(id);
    } catch (const std::exception& e) {
      response.mutable_base()->set_error("Failed to inspect container: " + std::string(e.what()));
    }

    co_return buildResponse(response, raw->version());
  }
} // namespace Containers
} // namespace Docker::Handlers