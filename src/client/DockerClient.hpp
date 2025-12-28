#pragma once
#include "../middleware/DockerMiddleware.hpp"
#include "api/v1/containers/Request.pb.h"
#include "api/v1/containers/Response.pb.h"
#include <boost/json.hpp>
#include <string_view>

namespace containers = api::v1::containers;
namespace json = boost::json;
using QueryMap = std::unordered_map<std::string, std::string>;

namespace Docker {
class Containers {
  private:
  DockerMiddleware middleware;

  public:
  containers::response::List list(const containers::request::List& list);
  containers::response::Create create(const containers::request::Create& body);
  json::value inspect(std::string_view id);
  json::value processes(std::string_view id);
  json::value export_(std::string_view id);
  json::value start(std::string_view id);
  json::value stop(std::string_view id);
  json::value restart(std::string_view id);
  json::value kill(std::string_view id);
  json::value update(std::string_view id, const json::object& body);
  json::value rename(std::string_view id, std::string_view name);
  json::value pause(std::string_view id);
  json::value unpause(std::string_view id);
  json::value attach(std::string_view id);
};

class Images {
  private:
  DockerMiddleware middleware;

  public:
  json::value list();
  json::value build();
  json::value prune();
  json::value create(std::string_view name, const json::object& body);
  json::value inspect();
};
}