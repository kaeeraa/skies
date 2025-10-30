#pragma once
#include "../middleware/DockerMiddleware.hpp"
#include <string_view>

class DockerClient {
  private:
  static DockerMiddleware middleware;

  public:
  // constructors
  DockerClient() { };

  struct Containers {
    json::value list()
    {
      return middleware.request(http::verb::get, "/containers/json");
    }
    json::value create(const std::string_view name, const json::object& body)
    {
      const std::string target = std::format("/containers/create?name={}", name);
      return middleware.request(http::verb::post, target, body);
    }
    json::value inspect(const std::string_view id)
    {
      const std::string target = std::format("/containers/{}/json?size=1", id);
      return middleware.request(http::verb::get, target);
    }
    json::value processes(const std::string_view id)
    {
      const std::string target = std::format("/containers/{}/top", id);
      return middleware.request(http::verb::get, target);
    }
    json::value export_(const std::string_view id)
    {
      // TODO: implement export method
      return {};
    }
    json::value start(const std::string_view id)
    {
      const std::string target = std::format("/containers/{}/start", id);
      return middleware.request(http::verb::post, target);
    }
    json::value stop(const std::string_view id)
    {
      const std::string target = std::format("/containers/{}/stop", id);
      return middleware.request(http::verb::post, target);
    }
    json::value restart(const std::string_view id)
    {
      const std::string target = std::format("/containers/{}/restart", id);
      return middleware.request(http::verb::post, target);
    }
    json::value kill(const std::string_view id)
    {
      const std::string target = std::format("/containers/{}/kill", id);
      return middleware.request(http::verb::post, target);
    }
    json::value update(const std::string_view id, const json::object& body)
    {
      const std::string target = std::format("/containers/{}/update", id);
      return middleware.request(http::verb::post, target, body);
    }
    json::value rename(const std::string_view id, const std::string_view name)
    {
      const std::string target = std::format("/containers/{}/rename?name={}", id, name);
      return middleware.request(http::verb::post, target);
    }
    json::value pause(const std::string_view id)
    {
      const std::string target = std::format("/containers/{}/pause", id);
      return middleware.request(http::verb::post, target);
    }
    json::value unpause(const std::string_view id)
    {
      const std::string target = std::format("/containers/{}/unpause", id);
      return middleware.request(http::verb::post, target);
    }
    json::value attach(const std::string_view id)
    {
      const std::string target = std::format("/containers/{}/attach/ws", id);
      return middleware.request(http::verb::get, target);
    }
  };
};