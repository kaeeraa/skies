#pragma once
#include "../middleware/DockerMiddleware.hpp"
#include "../schema/Request.schema.hpp"
#include <boost/json.hpp>
#include <string_view>

namespace Docker {
class Containers {
  private:
  DockerMiddleware middleware;

  public:
  boost::json::value list();
  boost::json::value create(const Requests::Containers::Create& body);
  boost::json::value inspect(std::string_view id);
  boost::json::value processes(std::string_view id);
  boost::json::value export_(std::string_view id);
  boost::json::value start(std::string_view id);
  boost::json::value stop(std::string_view id);
  boost::json::value restart(std::string_view id);
  boost::json::value kill(std::string_view id);
  boost::json::value update(std::string_view id, const boost::json::object& body);
  boost::json::value rename(std::string_view id, std::string_view name);
  boost::json::value pause(std::string_view id);
  boost::json::value unpause(std::string_view id);
  boost::json::value attach(std::string_view id);
};

class Images {
  private:
  DockerMiddleware middleware;

  public:
  boost::json::value list();
  boost::json::value build();
  boost::json::value prune();
  boost::json::value create(std::string_view name, const boost::json::object& body);
  boost::json::value inspect();
};
}