#include "DockerClient.hpp"
#include "../middleware/DockerMiddleware.hpp"
#include "../utility/ProtoBuffer.hpp"
#include "../utility/QueryBuilder.hpp"
#include <absl/status/status.h>
#include <boost/json.hpp>
#include <boost/json/object.hpp>
#include <boost/json/parse.hpp>
#include <boost/json/serialize.hpp>
#include <boost/json/value_to.hpp>
#include <format>
#include <google/protobuf/util/json_util.h>
#include <string>

namespace containers = api::v1::containers;
using QueryMap = std::unordered_map<std::string, std::string>;

containers::response::List Docker::Containers::list(const containers::request::List& request)
{
  containers::response::List response;
  QueryBuilder qb;
  qb.add("all", request.all())
    .add("limit", request.limit())
    .add("size", request.size())
    .add("filters", request.filters());

  const std::string target = qb.build("/containers/json");
  const json::value raw = json::object {
    { "data", middleware.request(http::verb::get, target) }
  };

  if (raw.as_object().contains("error")) {
    response.mutable_data()->Clear();
    response.mutable_base()->set_error(raw.at("error").as_string().c_str());
  }
  if (const absl::Status status = JsonToMessage(raw, &response); !status.ok()) {
    Logger::instance().error("Failed to parse {Containers::List} response: " + status.ToString());
    response.mutable_data()->Clear();
    response.mutable_base()->set_error("Failed to parse {Containers::List} response");
  }

  return response;
}

containers::response::Create Docker::Containers::create(const containers::request::Create& request)
{
  containers::response::Create response;
  const std::string target = "/containers/create";
  json::value rawRequest;
  if (const absl::Status status = MessageToJson(request, &rawRequest); !status.ok()) {
    Logger::instance().error("Failed to serialize {Containers::Create} request: " + status.ToString());
    response.clear_id();
    response.mutable_base()->set_error("Failed to serialize {Containers::Create} request");
    return response;
  }
  json::object raw = middleware.request(http::verb::post, target, rawRequest.as_object()).as_object();
  if (raw.contains("message")) {
    response.clear_id();
    response.mutable_base()->set_error(raw["message"].as_string().c_str());
    return response;
  }

  response.set_id(raw["Id"].as_string().c_str());
  return response;
}

json::value Docker::Containers::inspect(std::string_view id)
{
  const std::string target = std::format("/containers/{}/json?size=1", id);
  return middleware.request(http::verb::get, target);
}

json::value Docker::Containers::processes(std::string_view id)
{
  const std::string target = std::format("/containers/{}/top", id);
  return middleware.request(http::verb::get, target);
}

json::value Docker::Containers::export_(std::string_view id)
{
  const std::string target = std::format("/containers/{}/export", id);
  // TODO: implement export using tarball
  return {};
}

json::value Docker::Containers::start(std::string_view id)
{
  const std::string target = std::format("/containers/{}/start", id);
  return middleware.request(http::verb::post, target);
}

json::value Docker::Containers::stop(std::string_view id)
{
  const std::string target = std::format("/containers/{}/stop", id);
  return middleware.request(http::verb::post, target);
}

json::value Docker::Containers::restart(std::string_view id)
{
  const std::string target = std::format("/containers/{}/restart", id);
  return middleware.request(http::verb::post, target);
}

json::value Docker::Containers::kill(std::string_view id)
{
  const std::string target = std::format("/containers/{}/kill", id);
  return middleware.request(http::verb::post, target);
}

json::value Docker::Containers::update(std::string_view id, const json::object& body)
{
  const std::string target = std::format("/containers/{}/update", id);
  return middleware.request(http::verb::post, target, body);
}

json::value Docker::Containers::rename(std::string_view id, std::string_view name)
{
  const std::string target = std::format("/containers/{}/rename?name={}", id, name);
  return middleware.request(http::verb::post, target);
}

json::value Docker::Containers::pause(std::string_view id)
{
  const std::string target = std::format("/containers/{}/pause", id);
  return middleware.request(http::verb::post, target);
}

json::value Docker::Containers::unpause(std::string_view id)
{
  const std::string target = std::format("/containers/{}/unpause", id);
  return middleware.request(http::verb::post, target);
}

json::value Docker::Containers::attach(std::string_view id)
{
  const std::string target = std::format("/containers/{}/attach/ws", id);
  // TODO: implement attaching via websocket
  return {};
}